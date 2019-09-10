/**
 **************************************************************************************
 * @file    bt_play_mode.c
 * @brief   
 *
 * @author  KK
 * @version V1.0.0
 *
 * $Created: 2017-12-28 18:00:00$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <string.h>
#include "clk.h"
#include "type.h"
#include "app_config.h"
#include "app_message.h"
#include "chip_info.h"
#include "gpio.h"
#include "dma.h"
#include "dac.h"
#include "audio_adc.h"
#include "main_task.h"
#include "audio_vol.h"
#include "rtos_api.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "bt_avrcp_api.h"
#include "bt_manager.h"
#include "bt_play_api.h"
#include "bt_play_mode.h"
#include "audio_core_api.h"
#include "decoder_service.h"
#include "audio_core_service.h"
#include "mode_switch_api.h"
#include "remind_sound_service.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "backup_interface.h"
#include "breakpoint.h"
#include "debug.h"
#include "otg_device_standard_request.h"
#include "irqn.h"
#include "otg_device_hcd.h"
#include "bt_stack_service.h"
#include "recorder_service.h"
#include "encoder_service.h"


#include "bt_ddb_flash.h"
#include "ctrlvars.h"

#ifdef CFG_APP_BT_MODE_EN
#define BT_PLAY_TASK_STACK_SIZE			512
#define BT_PLAY_TASK_PRIO				3
#define BT_NUM_MESSAGE_QUEUE			10

#define BT_PLAY_DECODER_SOURCE_NUM		1 


typedef struct _btPlayContext
{
	xTaskHandle 		taskHandle;
	MessageHandle		msgHandle;
	MessageHandle		parentMsgHandle;

	TaskState			state;

	QueueHandle_t 		audioMutex;
	QueueHandle_t		pcmBufMutex;

	uint16_t 			*Source1Decoder;
	AudioCoreContext 	*AudioCoreBtPlay;

#ifdef CFG_FUNC_RECORDER_EN

	TaskState			RecorderSync;
	TaskState			EncoderSync;

#endif

	//play
	uint32_t 			SampleRate;
	uint8_t				ChannelNums;
	uint8_t				BtSyncVolume;
	
	//used Service
	TaskState			DecoderSync;

	TIMER				SbcTimer;	//切歌时，确保上一首残留数据播放完毕
	bool				MuteForWaterLevel;//for 等待水位达到门限才开source。

	BT_PLAYER_STATE			curPlayState;
}BtPlayContext;

/**根据appconfig缺省配置:DMA 8个通道配置**/
/*1、SD卡录音需PERIPHERAL_ID_SDIO1 RX/TX*/
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

static BtPlayContext	*BtPlayCt;

static void BtPlayModeStarting(uint16_t msgId);
static void BtPlayModeStopping(uint16_t msgId);
static void BtPlayRunning(uint16_t msgId);

void BtPlayResRelease(void)
{
	if(BtPlayCt->Source1Decoder != NULL)
	{
		DBG("BtPlayCt->Source1Decoder\n");
		osPortFree(BtPlayCt->Source1Decoder);
		BtPlayCt->Source1Decoder = NULL;
	}
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustDisable();
#endif
}

bool BtPlayResMalloc(uint16_t SampleLen)
{
	BtPlayCt->Source1Decoder = (uint16_t*)osPortMallocFromEnd(SampleLen * 2 * 2);
	if(BtPlayCt->Source1Decoder == NULL)
	{
		return FALSE;
	}
	memset(BtPlayCt->Source1Decoder, 0, SampleLen * 2 * 2);
	return TRUE;
}

void BtPlayResInit(void)
{
	if(BtPlayCt->Source1Decoder != NULL)
	{
		BtPlayCt->AudioCoreBtPlay->AudioSource[BT_PLAY_DECODER_SOURCE_NUM].PcmInBuf = (int16_t *)BtPlayCt->Source1Decoder;
	}
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(BtPlayCt->Source1Decoder != NULL)
	{
		BtPlayCt->AudioCoreBtPlay->AudioSource[REMIND_SOURCE_NUM].PcmInBuf = (int16_t *)BtPlayCt->Source1Decoder;
	}
#endif

#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustEnable(BT_PLAY_DECODER_SOURCE_NUM, BT_SBC_LEVEL_LOW, BT_SBC_LEVEL_HIGH);
#endif
}

/**
 * @func        BtPlayInit
 * @brief       BtPlay模式参数配置，资源初始化
 * @param       MessageHandle parentMsgHandle  
 * @Output      None
 * @return      bool
 * @Others      任务块、Dac、AudioCore配置，数据源自DecoderService
 * @Others      数据流从Decoder到audiocore配有函数指针，audioCore到Dac同理，由audiocoreService任务按需驱动
 * Record
 */
static bool BtPlayInit(MessageHandle parentMsgHandle)
{
	//DMA channel
	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);

	BtPlayCt = (BtPlayContext*)osPortMalloc(sizeof(BtPlayContext));
	if(BtPlayCt == NULL)
	{
		return FALSE;
	}
	
	memset(BtPlayCt, 0, sizeof(BtPlayContext));
	BtPlayCt->msgHandle = MessageRegister(BT_NUM_MESSAGE_QUEUE);
	if(BtPlayCt->msgHandle == NULL)
	{
		return FALSE;
	}
	BtPlayCt->parentMsgHandle = parentMsgHandle;
	BtPlayCt->state = TaskStateCreating;
	/* Create media audio services */

	BtPlayCt->SampleRate = CFG_PARA_SAMPLE_RATE;//默认硬件初始化采样率

	BtPlayCt->AudioCoreBtPlay = (AudioCoreContext*)&AudioCore;
	
	if(!BtPlayResMalloc(mainAppCt.SamplesPreFrame))
	{
		DBG("BtPlayResMalloc Res Error!\n");
		return FALSE;
	}

	//Audio init
	//Soure1.
	DecoderSourceNumSet(BT_PLAY_DECODER_SOURCE_NUM);
	BtPlayCt->AudioCoreBtPlay->AudioSource[BT_PLAY_DECODER_SOURCE_NUM].Enable = 0;
	BtPlayCt->AudioCoreBtPlay->AudioSource[BT_PLAY_DECODER_SOURCE_NUM].FuncDataGet = DecodedPcmDataGet;
	BtPlayCt->AudioCoreBtPlay->AudioSource[BT_PLAY_DECODER_SOURCE_NUM].FuncDataGetLen = DecodedPcmDataLenGet;//NULL;//
	BtPlayCt->AudioCoreBtPlay->AudioSource[BT_PLAY_DECODER_SOURCE_NUM].IsSreamData = FALSE;//Decoder
	BtPlayCt->AudioCoreBtPlay->AudioSource[BT_PLAY_DECODER_SOURCE_NUM].PcmFormat = 2; //stereo
	BtPlayCt->AudioCoreBtPlay->AudioSource[BT_PLAY_DECODER_SOURCE_NUM].PcmInBuf = (int16_t*)BtPlayCt->Source1Decoder;

#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	BtPlayCt->AudioCoreBtPlay->AudioSource[REMIND_SOURCE_NUM].Enable = 0;
	BtPlayCt->AudioCoreBtPlay->AudioSource[REMIND_SOURCE_NUM].FuncDataGet = DecodedPcmDataGet;
	BtPlayCt->AudioCoreBtPlay->AudioSource[REMIND_SOURCE_NUM].FuncDataGetLen = NULL;
	BtPlayCt->AudioCoreBtPlay->AudioSource[REMIND_SOURCE_NUM].IsSreamData = FALSE;//Decoder
	BtPlayCt->AudioCoreBtPlay->AudioSource[REMIND_SOURCE_NUM].PcmFormat = 2; //stereo
	BtPlayCt->AudioCoreBtPlay->AudioSource[REMIND_SOURCE_NUM].PcmInBuf = (int16_t *)BtPlayCt->Source1Decoder;
#endif
	
#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
	//模式音量同步手机音量值
	//在无音量同步功能的手机连接成功后,不需要同步手机的音量
	if(GetBtManager()->avrcpSyncEnable)
	{
		AudioMusicVolSet(GetBtManager()->avrcpSyncVol);
	}
#endif

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	#ifdef CFG_FUNC_MIC_KARAOKE_EN
	BtPlayCt->AudioCoreBtPlay->AudioEffectProcess = (AudioCoreProcessFunc)AudioEffectProcess;
	#else
	BtPlayCt->AudioCoreBtPlay->AudioEffectProcess = (AudioCoreProcessFunc)AudioMusicProcess;
	#endif
#else
	BtPlayCt->AudioCoreBtPlay->AudioEffectProcess = (AudioCoreProcessFunc)AudioBypassProcess;
#endif

#ifdef CFG_FUNC_RECORDER_EN
	BtPlayCt->RecorderSync = TaskStateNone;
	BtPlayCt->EncoderSync = TaskStateNone;

#endif

	//sbc decoder init
	if(SbcDecoderInit() != 0)
	{
		return FALSE;
	}
	BtPlayCt->SbcTimer.IsTimeOut = TRUE;//初始化。
	BtPlayCt->MuteForWaterLevel = FALSE;
	return TRUE;
}

static void BtPlayerDeinitialize(void)
{
	MessageContext		msgSend;

	BtPlayCt->state = TaskStateNone;
	// Send message to main app
	msgSend.msgId		= MSG_BT_PLAY_MODE_STOPPED;
	MessageSend(BtPlayCt->parentMsgHandle, &msgSend);
}

static void BtPlayModeCreate(void)
{
#if defined(CFG_FUNC_REMIND_SOUND_EN) && !defined(CFG_FUNC_REMIND_SBC)
	DecoderServiceCreate(BtPlayCt->msgHandle, DECODER_BUF_SIZE_MP3);
#else
	DecoderServiceCreate(BtPlayCt->msgHandle, DECODER_BUF_SIZE_SBC);
#endif
	BtPlayCt->DecoderSync = TaskStateCreating;
}

//All of services is created
//Send CREATED message to parent
static void BtPlayModeCreating(uint16_t msgId)
{
	MessageContext		msgSend;
	
	if(msgId == MSG_DECODER_SERVICE_CREATED)
	{
		BT_DBG("Decoder service created\n");
		BtPlayCt->DecoderSync = TaskStateReady;
	}

	if(BtPlayCt->DecoderSync == TaskStateReady)
	{
		//在蓝牙开始数据传输时,才开始DECODER/AUDIO_CORE的初始化动作
		BtPlayCt->state = TaskStateReady;
		msgSend.msgId		= MSG_BT_PLAY_MODE_CREATED;
		MessageSend(BtPlayCt->parentMsgHandle, &msgSend);
	}
}

static void BtPlayModeStart(void)
{
	DecoderServiceStart();
	BtPlayCt->DecoderSync = TaskStateStarting;
	BtPlayCt->state = TaskStateStarting;
}

static void BtPlayModeStarting(uint16_t msgId)
{
	MessageContext		msgSend;
	if(msgId == MSG_DECODER_SERVICE_STARTED)
	{
		BT_DBG("Decoder service Started\n");
		BtPlayCt->DecoderSync = TaskStateRunning;
	}

	//蓝牙在模式切换过来后说明模式创建成功,无需等待decoder_service创建成功
	if(BtPlayCt->DecoderSync == TaskStateRunning)
	{
		BT_DBG("Bt Play Mode Started\n");
		
		BtPlayCt->state = TaskStateRunning;
		msgSend.msgId		= MSG_BT_PLAY_MODE_STARTED;
		MessageSend(BtPlayCt->parentMsgHandle, &msgSend);
		
#ifdef CFG_FUNC_REMIND_SOUND_EN
		RemindSoundServiceItemRequest(SOUND_REMIND_BTMODE);
#endif
	}
}

static void BtPlayModeStop(void)
{
	bool NoService = TRUE;
	
	if(BtPlayCt->DecoderSync != TaskStateStopped && BtPlayCt->DecoderSync != TaskStateNone)
	{
		//先decoder stop
		DecoderServiceStop();
		NoService = FALSE;
		BtPlayCt->DecoderSync = TaskStateStopping;
	}
#ifdef CFG_FUNC_RECORDER_EN
	if(BtPlayCt->RecorderSync != TaskStateNone)
	{//此service 随用随Kill
		MediaRecorderServiceStop();
		BtPlayCt->RecorderSync = TaskStateStopping;
		NoService = FALSE;
	}
	if(BtPlayCt->EncoderSync != TaskStateNone)
	{//此service 随用随Kill
		EncoderServiceStop();
		BtPlayCt->EncoderSync = TaskStateStopping;
		NoService = FALSE;
	}
#endif
	BtPlayCt->state = TaskStateStopping;
	if(NoService)
	{
		BtPlayModeStopping(MSG_NONE);
	}
}

static void BtPlayModeStopping(uint16_t msgId)
{
	MessageContext		msgSend;
	
	if(msgId == MSG_DECODER_SERVICE_STOPPED)
	{
		BT_DBG("Btplay:Decoder service Stopped\n");
		BtPlayCt->DecoderSync = TaskStateStopped;
	}
#ifdef CFG_FUNC_RECORDER_EN
	if(msgId == MSG_MEDIA_RECORDER_SERVICE_STOPPED)
	{
		DBG("Btplay:RecorderKill");
		MediaRecorderServiceKill();
		BtPlayCt->RecorderSync = TaskStateNone;
	}
	else if(msgId == MSG_ENCODER_SERVICE_STOPPED)
	{
		DBG("Btplay:EncoderKill");
		EncoderServiceKill();
		BtPlayCt->EncoderSync = TaskStateNone;
	}
#endif	
	if((BtPlayCt->state == TaskStateStopping)
#ifdef CFG_FUNC_RECORDER_EN
		&& (BtPlayCt->RecorderSync == TaskStateNone)
		&& (BtPlayCt->EncoderSync == TaskStateNone)
#endif
		&& (BtPlayCt->DecoderSync == TaskStateNone || BtPlayCt->DecoderSync == TaskStateStopped)
		)
	{
		//Set para
		
		//clear msg
		MessageClear(BtPlayCt->msgHandle);

		//Set state
		BtPlayCt->state = TaskStateStopped;

		//reply
		msgSend.msgId		= MSG_BT_PLAY_MODE_STOPPED;
		MessageSend(BtPlayCt->parentMsgHandle, &msgSend);
	}
}

static void BtPlayEntrance(void * param)
{
	MessageContext		msgRecv;
#ifdef CFG_FUNC_DISPLAY_EN
	uint8_t Bt_link_bak = 0xff;
#endif
	BtPlayModeCreate();
	BT_DBG("Bt Play mode\n");

#ifdef CFG_FUNC_AUDIO_EFFECT_EN	
	AudioEffectModeSel(mainAppCt.EffectMode, 2);//0=init hw,1=effect,2=hw+effect
	AudioEffectParamSync();
#ifdef CFG_COMMUNICATION_BY_UART
	UART1_Communication_Init((void *)(&UartRxBuf[0]), 1024, (void *)(&UartTxBuf[0]), 1024);
#endif
#endif

	//SbcDecoderInit();
	//BtPlayBPUpdata();
#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_SYS_INFO);
#endif
	//如果进入模式AudioCore为静音状态，则unmute
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}

	if(GetA2dpState() == BT_A2DP_STATE_STREAMING)
		SetBtPlayState(BT_PLAYER_STATE_PLAYING);
	else
		SetBtPlayState(BT_PLAYER_STATE_STOP);

#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceUnmute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif
	while(1)
	{
        #ifdef CFG_FUNC_DISPLAY_EN
		MessageRecv(BtPlayCt->msgHandle, &msgRecv, 100);
        #else
		MessageRecv(BtPlayCt->msgHandle, &msgRecv, MAX_RECV_MSG_TIMEOUT);
        #endif
	
		switch(msgRecv.msgId)
		{
			case MSG_DECODER_SERVICE_CREATED:
				BtPlayModeCreating(msgRecv.msgId);
				break;
			
			case MSG_TASK_START:
				BtPlayModeStart();
				break;
			
			case MSG_DECODER_SERVICE_STARTED:
				BtPlayModeStarting(msgRecv.msgId);
				break;
			
			case MSG_TASK_STOP:
#ifdef CFG_FUNC_REMIND_SOUND_EN //确认提示音流程完毕
				RemindSoundServiceReset();
#endif
#if 0//CFG_COMMUNICATION_BY_USB
				NVIC_DisableIRQ(Usb_IRQn);
				OTG_DeviceDisConnect();
#endif
				//模式切换时,暂停正在播放的歌曲
				if(GetA2dpState() == BT_A2DP_STATE_STREAMING)
				{
					//pause
					BTCtrlPause();
				}
				BtPlayModeStop();
				break;
				
			case MSG_MEDIA_RECORDER_SERVICE_STOPPED:
			case MSG_DECODER_SERVICE_STOPPED:
			case MSG_ENCODER_SERVICE_STOPPED:
				BtPlayModeStopping(msgRecv.msgId);
				break;

			case MSG_APP_RES_RELEASE:
				BtPlayResRelease();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_RELEASE_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_MALLOC:
				BtPlayResMalloc(mainAppCt.SamplesPreFrame);
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_MALLOC_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_INIT:
				BtPlayResInit();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_INIT_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_REMIND_SOUND_PLAY_START:
				break;
			
			case MSG_REMIND_SOUND_PLAY_DONE://提示音播放结束
				AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
				break;
				
			default:
				if(BtPlayCt->state == TaskStateRunning)
				{
					BtPlayRunning(msgRecv.msgId);
		            #ifdef CFG_FUNC_DISPLAY_EN
					if(Bt_link_bak != GetA2dpState())
					{
						if(GetA2dpState() >= BT_A2DP_STATE_CONNECTED)
							msgRecv.msgId = MSG_DISPLAY_SERVICE_BT_LINKED;
						else
							msgRecv.msgId = MSG_DISPLAY_SERVICE_BT_UNLINK;
						MessageSend(GetDisplayMessageHandle(), &msgRecv);
					}
					Bt_link_bak = GetA2dpState();
		            #endif
				}
				break;

		}
	}
}

static void BtPlayRunning(uint16_t msgId)
{
	switch(msgId)
	{
#ifdef	CFG_FUNC_POWERKEY_EN
		case MSG_TASK_POWERDOWN:
			DBG("MSG receive PowerDown, Please breakpoint\n");
			SystemPowerDown();
			break;
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
		case MSG_DECODER_RESET://解码器出让和回收，临界消息。
			if(SoftFlagGet(SoftFlagDecoderSwitch))
			{
				DBG("Bt:Switch out\n");
				AudioCoreSourceDisable(BT_PLAY_DECODER_SOURCE_NUM);
#ifdef CFG_FUNC_FREQ_ADJUST
				AudioCoreSourceFreqAdjustDisable();
#endif
				SoftFlagRegister(SoftFlagDecoderRemind);//出让解码器
				RemindSoundServicePlay();
			}
			else//非app使用解码器时 回收
			{
				DecoderSourceNumSet(BT_PLAY_DECODER_SOURCE_NUM);
				SoftFlagDeregister(SoftFlagDecoderRemind);//回收解码器
				SbcDecoderRefresh();
			}
			SoftFlagDeregister(SoftFlagDecoderSwitch);
			break;


		case MSG_DECODER_STOPPED:
			if(SoftFlagGet(SoftFlagDecoderRemind))
			{
				MessageContext		msgSend;
				msgSend.msgId = msgId;
				MessageSend(GetRemindSoundServiceMessageHandle(), &msgSend);
			}
			break;

		case MSG_REMIND_SOUND_NEED_DECODER:
			if(!SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))
			{
//				BtStackServiceHighPriority(FALSE);
				AudioCoreSourceMute(BT_PLAY_DECODER_SOURCE_NUM, TRUE, TRUE);
				SoftFlagRegister(SoftFlagDecoderSwitch);
				DecoderReset();//发起解码器复位，准备出让。
			}
			break;
			
		case MSG_REMIND_SOUND_PLAY_REQUEST_FAIL:
			break;
		
		case MSG_REMIND_SOUND_PLAY_RENEW:
			break;
			
#endif
#ifdef CFG_FUNC_RECORDER_EN	
		case MSG_DEVICE_SERVICE_RECORDER:
			if(ResourceValue(AppResourceCard) || ResourceValue(AppResourceUDisk))
			{
				if(BtPlayCt->RecorderSync == TaskStateNone)
				{
					MediaRecorderServiceCreate(BtPlayCt->msgHandle);
					EncoderServiceCreate(BtPlayCt->msgHandle);
					BtPlayCt->RecorderSync = TaskStateCreating;
					BtPlayCt->EncoderSync = TaskStateCreating;
				}
				else if(BtPlayCt->RecorderSync == TaskStateRunning)//再按录音键 停止
				{
					MediaRecorderStop();
					MediaRecorderServiceStop();
					EncoderServiceStop();
					BtPlayCt->RecorderSync = TaskStateStopping;
					BtPlayCt->EncoderSync = TaskStateStopping;
					
				}
			}
			else
			{//flashfs录音 不处理
				DBG("Btplay:error, no disk!!!\n");
			}
			break;
			
		case MSG_MEDIA_RECORDER_SERVICE_CREATED:
			BtPlayCt->RecorderSync = TaskStateStarting;
			MediaRecorderServiceStart();
			break;
			
		case MSG_ENCODER_SERVICE_CREATED:
			BtPlayCt->EncoderSync = TaskStateStarting;
			EncoderServiceStart();
			break;

		case MSG_MEDIA_RECORDER_SERVICE_STARTED:
			MediaRecorderRun();
			BtPlayCt->RecorderSync = TaskStateRunning;
			break;
			
		case MSG_ENCODER_SERVICE_STARTED:
			BtPlayCt->EncoderSync = TaskStateRunning;
			break;

		case MSG_MEDIA_RECORDER_STOPPED:
			MediaRecorderServiceStop();
			BtPlayCt->RecorderSync = TaskStateStopping;
			break;
			
		case MSG_MEDIA_RECORDER_ERROR:
		case MSG_DEVICE_SERVICE_STOP:
			if(BtPlayCt->RecorderSync == TaskStateRunning)
			{
				MediaRecorderStop();
				MediaRecorderServiceStop();
				BtPlayCt->RecorderSync = TaskStateStopping;
				EncoderServiceStop();
				BtPlayCt->EncoderSync = TaskStateStopping;
			}
			break;
#endif //录音

		case MSG_BT_PLAY_DECODER_START:
			{
				int32_t ret=0;
				DecoderSourceNumSet(BT_PLAY_DECODER_SOURCE_NUM);
				//AudioCoreSourceEnable(BT_PLAY_DECODER_SOURCE_NUM);
				AudioCoreSourceUnmute(BT_PLAY_DECODER_SOURCE_NUM, 1, 1);
#ifdef CFG_FUNC_FREQ_ADJUST
				AudioCoreSourceFreqAdjustEnable(BT_PLAY_DECODER_SOURCE_NUM, BT_SBC_LEVEL_LOW, BT_SBC_LEVEL_HIGH);
#endif
				ret = SbcDecoderStart();
				if(ret == 0)
				{
					BT_DBG("sbc decoder start success\n");
					BtPlayCt->DecoderSync = TaskStateStarting;
				}
			}
			break;
			
		case MSG_BT_PLAY_STATE_CHANGED:
			//BT_DBG("MSG_BT_PLAY_STATE_CHANGED\n");
			if(!GetSbcDecoderStarted())
				break;
				
			if(GetBtPlayState() == BT_PLAYER_STATE_PLAYING)
			{
				//DBG("BtPlayerPlay()\n");
				//DecoderResume();
				while(!IsTimeOut(&BtPlayCt->SbcTimer));
				if(GetValidSbcDataSize() < BT_SBC_LEVEL_LOW / 2)
				{
					AudioCoreSourceDisable(BT_PLAY_DECODER_SOURCE_NUM);
					BtPlayCt->MuteForWaterLevel = TRUE;
				}
				else
				{
					BtPlayerPlay();
				}

			}
			else if(GetBtPlayState() == BT_PLAYER_STATE_PAUSED || GetBtPlayState() == BT_PLAYER_STATE_STOP)
			{
				//DBG("BtPlayerPause()\n");
				AudioCoreSourceMute(BT_PLAY_DECODER_SOURCE_NUM, 1, 1);
				TimeOutSet(&BtPlayCt->SbcTimer, 500/*(BT_SBC_DECODER_INPUT_LEN * 1000) / CFG_PARA_SAMPLE_RATE*/ );//实际应该是蓝牙数据采样率
#ifdef CFG_FUNC_FREQ_ADJUST
				AudioCoreSourceFreqAdjustDisable();
#endif
				BtPlayerPause();
			}
			break;

		case MSG_BT_PLAY_SYNC_VOLUME_CHANGED:
			//BT_DBG("MSG_BT_PLAY_SYNC_VOLUME_CHANGED\n");

#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
			AudioMusicVolSet(GetBtSyncVolume());
#endif
			break;

		//AVRCP CONTROL
		case MSG_BT_PLAY_PLAY_PAUSE:
			BT_DBG("MSG_BT_PLAY_PLAY_PAUSE\n");
			
			if(GetBtPlayState() == BT_PLAYER_STATE_PLAYING)
			{
				//pause
				BTCtrlPause();
#ifdef CFG_FUNC_FREQ_ADJUST
				AudioCoreSourceFreqAdjustDisable();
#endif
			}
			else if((GetBtPlayState() == BT_PLAYER_STATE_PAUSED) 
				|| (GetBtPlayState() == BT_PLAYER_STATE_STOP))
			{
				//play
				BTCtrlPlay();
			}

			break;
		
		case MSG_BT_PLAY_NEXT:
			BT_DBG("MSG_BT_PLAY_NEXT\n");

			//if(GetA2dpState() == BT_A2DP_STATE_STREAMING)
			{
				BTCtrlNext();
			}
			break;
		
		case MSG_BT_PLAY_PRE:
			BT_DBG("MSG_BT_PLAY_PRE\n");
			
			//if(GetA2dpState() == BT_A2DP_STATE_STREAMING)
			{
				BTCtrlPrev();
			}
			break;

		case MSG_BT_PLAY_VOLUME_SET:
#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
			BT_DBG("MSG_BT_PLAY_VOLUME_SET\n");
			{
				uint16_t VolumePercent = GetBtSyncVolume();

				if(GetAvrcpState() != BT_AVRCP_STATE_CONNECTED)
					break;
				
				VolumePercent = VolumePercent*100/32;
				BTCtrlSetVol((uint8_t)VolumePercent);
			}
#endif
			break;

#if (BT_HFP_SUPPORT == ENABLE)
		case MSG_BT_HF_REDAIL_LAST_NUM:
			HfpRedialNumber();
			break;

		case MSG_BT_HF_VOICE_RECOGNITION:
			OpenBtHfpVoiceRecognitionFunc();
			break;
#endif

		case MSG_BT_CONNECT_CTRL:
			if((GetA2dpState() >= BT_A2DP_STATE_CONNECTED) 
				|| (GetHfpState() >= BT_HFP_STATE_CONNECTED) 
				|| (GetAvrcpState() >= BT_AVRCP_STATE_CONNECTED))
			{
				//手动断开
				BtDisconnectCtrl();
			}
			else
			{
				//手动连接
				BtConnectCtrl();
			}
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
bool BtPlayCreate(MessageHandle parentMsgHandle)
{
	bool		ret = TRUE;

#ifdef CFG_BT_BACKGROUND_RUN_EN
	//null
#else
	Clock_Module2Enable(ALL_MODULE2_CLK_SWITCH);
	vTaskDelay(50);
	BtStackServiceStart();
#endif
	
	ret = BtPlayInit(parentMsgHandle);
	if(ret)
	{
		BtPlayCt->taskHandle = NULL;
		xTaskCreate(BtPlayEntrance, "BtAudioPlay", BT_PLAY_TASK_STACK_SIZE, NULL, BT_PLAY_TASK_PRIO, &BtPlayCt->taskHandle);
		if(BtPlayCt->taskHandle == NULL)
		{
			ret = FALSE;
		}
	}
	else
	{
		BT_DBG("BtAudioPlay app create fail!\n");
	}
	
	return ret;
}

bool BtPlayKill(void)
{
	if(BtPlayCt == NULL)
	{
		return FALSE;
	}
//注意此处，如果在TaskStateCreating发起stop，它尚未init.
	BtPlayerDeinitialize();	
	AudioCoreProcessConfig(NULL);
	AudioCoreSourceDisable(BT_PLAY_DECODER_SOURCE_NUM);
	//AudioCoreSourceUnmute(BT_PLAY_DECODER_SOURCE_NUM, 1, 1);
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustDisable();
#endif

#ifndef CFG_FUNC_MIXER_SRC_EN
#ifdef CFG_RES_AUDIO_DACX_EN
	AudioDAC_SampleRateChange(ALL, CFG_PARA_SAMPLE_RATE);//恢复
#else
	AudioDAC_SampleRateChange(DAC0, CFG_PARA_SAMPLE_RATE);//恢复
#endif
#endif

	//Kill used services
	DecoderServiceKill();
	//AudioCoreServiceKill();

	//task 先删任务，再删邮箱，收资源
	if(BtPlayCt->taskHandle != NULL)
	{
		vTaskDelete(BtPlayCt->taskHandle);
		BtPlayCt->taskHandle = NULL;
	}
	
	//Msgbox
	if(BtPlayCt->msgHandle != NULL)
	{
		MessageDeregister(BtPlayCt->msgHandle);
		BtPlayCt->msgHandle = NULL;
	}


	//DMA通道关闭
	//BT需要跑后台，不关闭DMA通道

	SbcDecoderDeinit();
	
	//PortFree
	if(BtPlayCt->Source1Decoder != NULL)
	{
		osPortFree(BtPlayCt->Source1Decoder);
		BtPlayCt->Source1Decoder = NULL;
	}
	//osPortFree(BtPlayCt->AudioCoreBtPlay);
	BtPlayCt->AudioCoreBtPlay = NULL;
	
	osPortFree(BtPlayCt);
	BtPlayCt = NULL;
	DBG("!!BtPlayCt\n");
	
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioEffectsDeInit();
#endif

#ifdef CFG_BT_BACKGROUND_RUN_EN
	//null
#else
	if(mainAppCt.appTargetMode != AppModeBtHfPlay)
	{
		BtPowerOff();
	}
	else
	{
		BtStackServiceWaitResume();
	}
#endif
	return TRUE;
}

bool BtPlayStart(void)
{
	MessageContext		msgSend;
	if(BtPlayCt == NULL)
	{
		return FALSE;
	}

	msgSend.msgId		= MSG_TASK_START;
	MessageSend(BtPlayCt->msgHandle, &msgSend);

	return TRUE;
}

bool BtPlayStop(void)
{
	MessageContext		msgSend;
	if(BtPlayCt == NULL)
	{
		return FALSE;
	}
	
	AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceMute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif			
	vTaskDelay(30);

	msgSend.msgId		= MSG_TASK_STOP;
	MessageSend(BtPlayCt->msgHandle, &msgSend);

	return TRUE;
}

MessageHandle GetBtPlayMessageHandle(void)
{
	if(BtPlayCt == NULL)
	{
		return NULL;
	}
	return BtPlayCt->msgHandle;
}

void SetBtPlayState(uint8_t state)
{
	if(BtPlayCt->curPlayState != state)
	{
		BtPlayCt->curPlayState = state;
		//BT_DBG("BtPlayState[%d]", BtPlayCt->curPlayState);
	}
}

BT_PLAYER_STATE GetBtPlayState(void)
{
	return BtPlayCt->curPlayState;
}

#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
void SetBtSyncVolume(uint8_t volume)
{
	if(GetBtManager()->avrcpSyncEnable)
		GetBtManager()->avrcpSyncVol = volume;
}

uint8_t GetBtSyncVolume(void)
{
	return GetBtManager()->avrcpSyncVol;
}
#endif

void SbcDataNotify(void)
{
	if(BtPlayCt != NULL && BtPlayCt->DecoderSync == TaskStateRunning)
	{
		DecoderServiceMsg();
	}
}

bool GetBtMuteState(void)
{
	if(BtPlayCt == NULL || BtPlayCt->state != TaskStateRunning)
	{
		return FALSE;
	}
	return BtPlayCt->MuteForWaterLevel;
}

void SetBtMuteState(bool MuteState)
{
	if(BtPlayCt != NULL || BtPlayCt->state == TaskStateRunning)
	{
		BtPlayCt->MuteForWaterLevel = MuteState;
	}
}
#endif//#ifdef CFG_APP_BT_MODE_EN
