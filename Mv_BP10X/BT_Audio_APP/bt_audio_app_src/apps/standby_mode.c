/**
 **************************************************************************************
 * @file    standby_mode.c
 * @brief   
 *
 * @author  Pi
 * @version V1.0.0
 *
 * $Created: 2018-09-26 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "type.h"
#include "irqn.h"
#include "gpio.h"
#include "dma.h"
#include "rtos_api.h"
#include "app_message.h"
#include "app_config.h"
#include "debug.h"
#include "audio_core_api.h"
#include "main_task.h"
#include "recorder_service.h"
#include "audio_core_service.h"
#include "decoder_service.h"
#include "remind_sound_service.h"
#include "audio_effect.h"
#include "breakpoint.h"
#include "otg_device_hcd.h"
#include "otg_device_standard_request.h"
#include "backup_interface.h"
#include "timeout.h"
#include "ctrlvars.h"

#define STANDBY_PLAY_TASK_STACK_SIZE		512//1024
#define STANDBY_PLAY_TASK_PRIO				3
#define STANDBY_NUM_MESSAGE_QUEUE			5


typedef struct _StandbyPlayContext
{
	xTaskHandle 		taskHandle;
	MessageHandle		msgHandle;
	MessageHandle		parentMsgHandle;

	TaskState			state;
	AudioCoreContext 	*AudioCoreStandby;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	uint16_t*			Source1Decoder;
	TaskState			DecoderSync;
	bool				FirstRemindEnd;//开机提示音结束
#endif

	//play
	uint32_t 			SampleRate; //带提示音时，如果不重采样，要避免采样率配置冲突

	uint32_t			StandbyTick;//standby启动的时间计数器值。
}StandbyPlayContext;


/**根据appconfig缺省配置:DMA 8个通道配置**/
/*1、SD卡播放和录音需PERIPHERAL_ID_SDIO1 RX/TX*/
/*2、在线串口调音需PERIPHERAL_ID_UART1 RX/TX*/ 
/*3、IR按键需PERIPHERAL_ID_TIMER4，mode之间通道必须一致*/
/*4、Mic开启需PERIPHERAL_ID_AUDIO_ADC1_RX，mode之间通道必须一致*/
/*5、Dac0开启需PERIPHERAL_ID_AUDIO_DAC0_TX mode之间通道必须一致*/
/*6、DacX需开启PERIPHERAL_ID_AUDIO_DAC1_TX mode之间通道必须一致*/

static const uint8_t DmaChannelMap[29] = {
	255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
	255,//PERIPHERAL_ID_SPIS_TX,		//1
	255,//PERIPHERAL_ID_TIMER3,			//2
	4,//PERIPHERAL_ID_SDIO_RX,			//3
	5,//PERIPHERAL_ID_SDIO0_TX,			//4
	255,//PERIPHERAL_ID_UART0_RX,		//5
	255,//PERIPHERAL_ID_TIMER1,			//6
	255,//PERIPHERAL_ID_TIMER2,			//7
	255,//PERIPHERAL_ID_SDPIF_RX,		//8 SPDIF_RX /TX same chanell
	255,//PERIPHERAL_ID_SDPIF_TX,		//8 SPDIF_RX /TX same chanell
	255,//PERIPHERAL_ID_SPIM_RX,		//9
	255,//PERIPHERAL_ID_SPIM_TX,		//10
	255,//PERIPHERAL_ID_UART0_TX,		//11
	
#ifdef CFG_COMMUNICATION_BY_UART	
	7,//PERIPHERAL_ID_UART1_RX,			//12
	6,//PERIPHERAL_ID_UART1_TX,			//13
#else
	255,//PERIPHERAL_ID_UART1_RX,		//12
	255,//PERIPHERAL_ID_UART1_TX,		//13
#endif

	255,//PERIPHERAL_ID_TIMER4,			//14
	255,//PERIPHERAL_ID_TIMER5,			//15
	255,//PERIPHERAL_ID_TIMER6,			//16
	0,//PERIPHERAL_ID_AUDIO_ADC0_RX,	//17
	1,//PERIPHERAL_ID_AUDIO_ADC1_RX,	//18
	2,//PERIPHERAL_ID_AUDIO_DAC0_TX,	//19
	3,//PERIPHERAL_ID_AUDIO_DAC1_TX,	//20
	255,//PERIPHERAL_ID_I2S0_RX,		//21
#if	(defined(CFG_RES_AUDIO_I2SOUT_EN )&&(CFG_RES_I2S_PORT==0))
	7,//PERIPHERAL_ID_I2S0_TX,			//22
#else	
	255,//PERIPHERAL_ID_I2S0_TX,		//22
#endif	
	255,//PERIPHERAL_ID_I2S1_RX,		//23
#if	(defined(CFG_RES_AUDIO_I2SOUT_EN )&&(CFG_RES_I2S_PORT==1))
	7,	//PERIPHERAL_ID_I2S1_TX,		//24
#else
	255,//PERIPHERAL_ID_I2S1_TX,		//24
#endif
	255,//PERIPHERAL_ID_PPWM,			//25
	255,//PERIPHERAL_ID_ADC,     		//26
	255,//PERIPHERAL_ID_SOFTWARE,		//27
};

static  StandbyPlayContext*		StandbyPlayCt;


static void StandbyPlayModeCreating(uint16_t msgId);
static void StandbyPlayModeStarting(uint16_t msgId);
static void StandbyPlayModeStopping(uint16_t msgId);
static void StandbyPlayModeStopped(void);
static void StandbyPlayRunning(uint16_t msgId);
uint8_t StandbyDecoderSourceNum(void);

static bool StandbyPlay_Init(MessageHandle parentMsgHandle)
{
//System config
	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);
//Task & App Config
	StandbyPlayCt = (StandbyPlayContext*)osPortMalloc(sizeof(StandbyPlayContext));
	if(StandbyPlayCt == NULL)
	{
		return FALSE;
	}
	memset(StandbyPlayCt, 0, sizeof(StandbyPlayContext));
	StandbyPlayCt->msgHandle = MessageRegister(STANDBY_NUM_MESSAGE_QUEUE);
	if(StandbyPlayCt->msgHandle == NULL)
	{
		return FALSE;
	}
	StandbyPlayCt->parentMsgHandle = parentMsgHandle;
	StandbyPlayCt->state = TaskStateCreating;
	StandbyPlayCt->SampleRate = CFG_PARA_SAMPLE_RATE;

	//Core Source1 para
	StandbyPlayCt->AudioCoreStandby = (AudioCoreContext*)&AudioCore;
	//Audio init
//	//note Soure0.和sink0已经在main app中配置，不要随意配置
	//InCore1 buf
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	StandbyPlayCt->Source1Decoder = (uint16_t*)osPortMalloc(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//One Frame
	if(StandbyPlayCt->Source1Decoder == NULL)
	{
		return FALSE;
	}
	memset(StandbyPlayCt->Source1Decoder, 0, CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//2K

	//Core Soure2 Para
	DecoderSourceNumSet(REMIND_SOURCE_NUM);
	StandbyPlayCt->AudioCoreStandby->AudioSource[REMIND_SOURCE_NUM].Enable = 0;
	StandbyPlayCt->AudioCoreStandby->AudioSource[REMIND_SOURCE_NUM].FuncDataGet = DecodedPcmDataGet;
	StandbyPlayCt->AudioCoreStandby->AudioSource[REMIND_SOURCE_NUM].FuncDataGetLen = NULL;
	StandbyPlayCt->AudioCoreStandby->AudioSource[REMIND_SOURCE_NUM].IsSreamData = FALSE;//Decoder
	StandbyPlayCt->AudioCoreStandby->AudioSource[REMIND_SOURCE_NUM].PcmFormat = 2; //stereo
	StandbyPlayCt->AudioCoreStandby->AudioSource[REMIND_SOURCE_NUM].PcmInBuf = (int16_t *)StandbyPlayCt->Source1Decoder;
#endif

	//Core Process

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
    #ifdef CFG_FUNC_MIC_KARAOKE_EN
	StandbyPlayCt->AudioCoreStandby->AudioEffectProcess = (AudioCoreProcessFunc)AudioEffectProcess;
	#else
	StandbyPlayCt->AudioCoreStandby->AudioEffectProcess = (AudioCoreProcessFunc)AudioMusicProcess;
	#endif
#else
	StandbyPlayCt->AudioCoreStandby->AudioEffectProcess = (AudioCoreProcessFunc)AudioBypassProcess;
#endif

	StandbyPlayCt->StandbyTick = GetSysTick1MsCnt();
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	StandbyPlayCt->FirstRemindEnd = TRUE;//缺省值。
#endif
	return TRUE;
}

//创建从属services
static void StandbyPlayModeCreate(void)
{
	bool NoService = TRUE;
	
	// Create service task
#if defined(CFG_FUNC_REMIND_SBC)
	DecoderServiceCreate(StandbyPlayCt->msgHandle, DECODER_BUF_SIZE_SBC);//提示音格式决定解码器内存消耗
	NoService = FALSE;
#elif defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceCreate(StandbyPlayCt->msgHandle, DECODER_BUF_SIZE_MP3);
	NoService = FALSE;
#endif
	if(NoService)
	{
		StandbyPlayModeCreating(MSG_NONE);
	}
}

//All of services is created
//Send CREATED message to parent
static void StandbyPlayModeCreating(uint16_t msgId)
{
	MessageContext		msgSend;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(msgId == MSG_DECODER_SERVICE_CREATED)
	{
		DBG("Standby:Decoder created\n");
		StandbyPlayCt->DecoderSync = TaskStateReady;
	}
#endif

#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(StandbyPlayCt->DecoderSync == TaskStateReady)
#endif
	{
		msgSend.msgId		= MSG_STANDBY_PLAY_MODE_CREATED;
		MessageSend(StandbyPlayCt->parentMsgHandle, &msgSend);
		StandbyPlayCt->state = TaskStateReady;
	}
}

static void StandbyPlayModeStart(void)
{
	bool NoService = TRUE;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceStart();
	StandbyPlayCt->DecoderSync = TaskStateStarting;
	NoService = FALSE;
#endif
	if(NoService)
	{
		StandbyPlayModeStarting(MSG_NONE);
	}
	else
	{
		StandbyPlayCt->state = TaskStateStarting;
	}
}

static void StandbyPlayModeStarting(uint16_t msgId)
{
	MessageContext		msgSend;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(msgId == MSG_DECODER_SERVICE_STARTED)
	{
		DBG("Standby:Decoder startted\n");
		StandbyPlayCt->DecoderSync = TaskStateRunning;
	}
#endif

#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(StandbyPlayCt->DecoderSync == TaskStateRunning)
#endif
	{
		msgSend.msgId		= MSG_STANDBY_PLAY_MODE_STARTED;
		MessageSend(StandbyPlayCt->parentMsgHandle, &msgSend);
		StandbyPlayCt->state = TaskStateRunning;
		
#ifdef CFG_FUNC_REMIND_SOUND_EN
		if(SoftFlagGet(SoftFlagFirstMode))
		{
			//开启提示功能，不播放开机提示音时，屏蔽下列两行代码
			StandbyPlayCt->FirstRemindEnd = FALSE;
			RemindSoundServiceItemRequest(SOUND_REMIND_KAIJI); //插播开机提示音 KAIJI
		}
		else
		{
			RemindSoundServiceItemRequest(SOUND_REMIND_STANDBYM);
		}
#endif
	}
}

static void StandbyPlayModeStop(void)
{
	bool NoService = TRUE;
#if	defined(CFG_FUNC_REMIND_SOUND_EN) 
	if(StandbyPlayCt->DecoderSync != TaskStateNone && StandbyPlayCt->DecoderSync != TaskStateStopped)
	{//解码器是 随app kill
		DecoderServiceStop();
		StandbyPlayCt->DecoderSync = TaskStateStopping;
		NoService = FALSE;
	}
#endif
	StandbyPlayCt->state = TaskStateStopping;
	if(NoService)
	{
		StandbyPlayModeStopped();
	}
}

static void StandbyPlayModeStopping(uint16_t msgId)//部分子service 随用随kill
{
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(msgId == MSG_DECODER_SERVICE_STOPPED)
	{
		StandbyPlayCt->DecoderSync = TaskStateNone;
	}
#endif
	if((StandbyPlayCt->state == TaskStateStopping)
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
		&& (StandbyPlayCt->DecoderSync == TaskStateNone)
#endif
	)
	{
		StandbyPlayModeStopped();
	}
}

static void StandbyPlayModeStopped(void)
{
	MessageContext		msgSend;
	//Set para
	
	//clear msg
	MessageClear(StandbyPlayCt->msgHandle);
	
	//Set state
	StandbyPlayCt->state = TaskStateStopped;

	//reply
	msgSend.msgId		= MSG_STANDBY_PLAY_MODE_STOPPED;
	MessageSend(StandbyPlayCt->parentMsgHandle, &msgSend);
}

bool StandbyPlayPermissionFirstMode(void)
{
	MessageContext		msgSend;
	if(StandbyPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_STANDY_PERMISSION_MODE;
	return MessageSend(StandbyPlayCt->parentMsgHandle, &msgSend);
}


static void StandbyPlayEntrance(void * param)
{
	MessageContext		msgRecv;
	
	DBG("Standby:App\n");
	// Create services
	StandbyPlayModeCreate();
#if (defined(CFG_FUNC_AUDIO_EFFECT_EN) && (defined(CFG_COMMUNICATION_BY_USB ))) || defined(CFG_APP_USB_AUDIO_MODE_EN)
	//standby 模式下关于USB中断，不进行调音
	NVIC_DisableIRQ(Usb_IRQn);
	OTG_DeviceDisConnect();
#endif

	while(1)
	{
		MessageRecv(StandbyPlayCt->msgHandle, &msgRecv, 50);//没消息时50ms进一次。
		if(StandbyPlayCt->state != TaskStateRunning)
		{
			switch(msgRecv.msgId)//警告：在此段代码，禁止新增提示音插播位置。
			{	
				case MSG_DECODER_SERVICE_CREATED:
					StandbyPlayModeCreating(msgRecv.msgId);
					break;
		
				case MSG_TASK_START:
					StandbyPlayModeStart();
					break;
				case MSG_DECODER_SERVICE_STARTED:
					//RemindSound request		
					StandbyPlayModeStarting(msgRecv.msgId);
					break;

				case MSG_TASK_STOP: //Created 时stop
#ifdef CFG_COMMUNICATION_BY_USB
					NVIC_DisableIRQ(Usb_IRQn);
					OTG_DeviceDisConnect();
#endif
					StandbyPlayModeStop();
					break;

				case MSG_DECODER_SERVICE_STOPPED:
				case MSG_MEDIA_RECORDER_SERVICE_STOPPED:
					StandbyPlayModeStopping(msgRecv.msgId);
					break;
			}
		}
		else //StandbyPlayCt->state != TaskStateRunning
		{
			StandbyPlayRunning(msgRecv.msgId);
		}
		if(SoftFlagGet(SoftFlagFirstMode) && GetSysTick1MsCnt() - StandbyPlayCt->StandbyTick > MODE_WAIT_DEVICE_TIME)
		{
#ifdef	CFG_FUNC_REMIND_SOUND_EN
			if(StandbyPlayCt->FirstRemindEnd)
#endif
			{
				StandbyPlayPermissionFirstMode();
			}
		}
	}
}

static void StandbyPlayRunning(uint16_t msgId)
{
	switch(msgId)
	{
		case MSG_TASK_STOP:
#ifdef CFG_FUNC_REMIND_SOUND_EN 
			RemindSoundServiceReset();
#endif
#ifdef CFG_COMMUNICATION_BY_USB
			NVIC_DisableIRQ(Usb_IRQn);
			OTG_DeviceDisConnect();
#endif
			StandbyPlayModeStop();
			break;

#ifdef	CFG_FUNC_POWERKEY_EN
		case MSG_TASK_POWERDOWN:
			DBG("Standby:MSG receive PowerDown, Please breakpoint\n");
			SystemPowerDown();
			break;
#endif
		case MSG_REMIND_SOUND_PLAY_REQUEST_FAIL:
#ifdef CFG_FUNC_REMIND_SOUND_EN
			//首次启动模式时 & 等待开机提示音结束 & device延时结束
			if(SoftFlagGet(SoftFlagFirstMode) && !StandbyPlayCt->FirstRemindEnd )
			{
				StandbyPlayCt->FirstRemindEnd = TRUE;
				if(GetSysTick1MsCnt() - StandbyPlayCt->StandbyTick > MODE_WAIT_DEVICE_TIME)
				{
					StandbyPlayPermissionFirstMode();
				}
			}
#endif
			break;

		case MSG_DECODER_STOPPED:
#if defined(CFG_FUNC_REMIND_SOUND_EN)
			{
				MessageContext		msgSend;
				msgSend.msgId = msgId;
				MessageSend(GetRemindSoundServiceMessageHandle(), &msgSend);//提示音期间转发解码器消息。
			}
#endif
			break;			

			case MSG_DECODER_RESET://解码器出让和回收，临界消息,提示音结束标记。
#ifdef CFG_FUNC_REMIND_SOUND_EN
			//首次启动模式时 & 等待开机提示音结束 & device延时结束
			if(SoftFlagGet(SoftFlagFirstMode) && !StandbyPlayCt->FirstRemindEnd )
			{
				StandbyPlayCt->FirstRemindEnd = TRUE;
				if(GetSysTick1MsCnt() - StandbyPlayCt->StandbyTick > MODE_WAIT_DEVICE_TIME)
				{
					StandbyPlayPermissionFirstMode();
				}
			}
#endif
			break;

		default:
			CommonMsgProccess(msgId);
			break;
	}
}

/***************************************************************************************
 *
 * APIs
 *
 */
bool StandbyPlayCreate(MessageHandle parentMsgHandle)
{
	bool		ret = TRUE;

	ret = StandbyPlay_Init(parentMsgHandle);
	if(ret)
	{
		StandbyPlayCt->taskHandle = NULL;
		xTaskCreate(StandbyPlayEntrance,
					"StandbyPlay",
					STANDBY_PLAY_TASK_STACK_SIZE,
					NULL, STANDBY_PLAY_TASK_PRIO,
					&StandbyPlayCt->taskHandle);
		if(StandbyPlayCt->taskHandle == NULL)
		{
			ret = FALSE;
		}
	}
	else
	{
		DBG("Standby:app create fail!\n");
	}
	return ret;
}

bool StandbyPlayStart(void)
{
	MessageContext		msgSend;

	if(StandbyPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_START;
	return MessageSend(StandbyPlayCt->msgHandle, &msgSend);
}

bool StandbyPlayPause(void)
{
	MessageContext		msgSend;
	if(StandbyPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_PAUSE;
	return MessageSend(StandbyPlayCt->msgHandle, &msgSend);
}

bool StandbyPlayResume(void)
{
	MessageContext		msgSend;
	if(StandbyPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_RESUME;
	return MessageSend(StandbyPlayCt->msgHandle, &msgSend);
}

bool StandbyPlayStop(void)
{
	MessageContext		msgSend;
	if(StandbyPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_STOP;
	return MessageSend(StandbyPlayCt->msgHandle, &msgSend);
}

bool StandbyPlayKill(void)
{
	if(StandbyPlayCt == NULL)
	{
		return FALSE;
	}

	//Kill used services
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	AudioCoreSourceDisable(REMIND_SOURCE_NUM);
	DecoderServiceKill();
#endif

	//注意：AudioCore父任务调整到mainApp下，此处只关闭AudioCore通道，不关闭任务
	AudioCoreProcessConfig(NULL);

	//task
	if(StandbyPlayCt->taskHandle != NULL)
	{
		vTaskDelete(StandbyPlayCt->taskHandle);
		StandbyPlayCt->taskHandle = NULL;
	}

	//Msgbox
	if(StandbyPlayCt->msgHandle != NULL)
	{
		MessageDeregister(StandbyPlayCt->msgHandle);
		StandbyPlayCt->msgHandle = NULL;
	}

	//PortFree
	StandbyPlayCt->AudioCoreStandby = NULL;

#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(StandbyPlayCt->Source1Decoder != NULL)
	{
		osPortFree(StandbyPlayCt->Source1Decoder);
		StandbyPlayCt->Source1Decoder = NULL;
	}
#endif

	osPortFree(StandbyPlayCt);
	StandbyPlayCt = NULL;
	DBG("Standby:Kill Ct\n");

	return TRUE;
}

MessageHandle GetStandbyPlayMessageHandle(void)
{
	if(StandbyPlayCt != NULL)
	{
		return StandbyPlayCt->msgHandle;
	}
	else
	{
		return NULL;
	}
}
