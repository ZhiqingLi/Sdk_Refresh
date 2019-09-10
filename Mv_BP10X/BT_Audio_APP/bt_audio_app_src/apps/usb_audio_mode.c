/**
 **************************************************************************************
 * @file    usb_audio_mode.c
 * @brief
 *
 * @author  Owen
 * @version V1.0.0
 *
 * $Created: 2018-04-27 13:06:47$
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
#include "delay.h"
#include "audio_adc.h"
#include "dac.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "otg_device_hcd.h"
#include "otg_device_audio.h"
#include "otg_device_standard_request.h"
#include "mcu_circular_buf.h"
#include "audio_core_api.h"
#include "audio_core_service.h"
#include "decoder_service.h"
#include "remind_sound_service.h"
#include "main_task.h"
#include "timer.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "resampler.h"
#include "backup_interface.h"
#include "breakpoint.h"
#include "ctrlvars.h"
#include "usb_audio_mode.h"
#include "recorder_service.h"
#include "encoder_service.h"
#include "audio_vol.h"
#include "ctrlvars.h"
#include "device_detect.h" 

#ifdef CFG_APP_USB_AUDIO_MODE_EN

#define USB_DEVICE_PLAY_TASK_STACK_SIZE		512//1024
#define USB_DEVICE_PLAY_TASK_PRIO			3
#define USB_DEVICE_NUM_MESSAGE_QUEUE		10

//#define	USB_FIFO_LEN						(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2 * 2 * 2)
#define USB_AUDIO_SRC_BUF_LEN				(48 * 2 * 2)

typedef struct _UsbDevicePlayContext
{
	xTaskHandle 		taskHandle;
	MessageHandle		msgHandle;
	MessageHandle		parentMsgHandle;

	TaskState			state;

	uint32_t			*ADCFIFO1;				//ADC0的DMA循环fifo
	uint16_t 			*Source1Buf_USBAduio;	//ADC0 取LineIn数据
	AudioCoreContext 	*AudioCoreUsb;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	uint16_t*			Source2Decoder;
	TaskState			DecoderSync;
#endif

#ifdef CFG_FUNC_RECORDER_EN
	TaskState			RecorderSync;
	TaskState			EncoderSync;
#endif

	//play
	uint32_t 			SampleRate;
	int8_t*				SpeakerBuffer; 	     //声卡缓存数据

	int16_t				LeftVolSave;		//保存进 usb audio mode前的音量
	int16_t				RightVolSave;

#ifdef CFG_FUNC_MIXER_SRC_EN
	ResamplerContext*	ResamplerCt;
	int16_t*			SRCOutBuf;
#endif

}UsbDevicePlayContext;

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


static  UsbDevicePlayContext*		UsbDevicePlayCt;
bool IsUsbAudioMode =  TRUE;
MCU_CIRCULAR_CONTEXT SpeakerCircularBuf;
const uint8_t SilentData[192] =
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

//#define USB_TEST
#ifdef USB_TEST
uint32_t PcmBuffer[192/4];
#endif

static void UsbDevicePlayModeCreating(uint16_t msgId);
static void UsbDevicePlayModeStarting(uint16_t msgId);
static void UsbDevicePlayModeStopping(uint16_t msgId);
uint16_t UsbAudioDataGet(void *Buffer,uint16_t Len);
uint16_t UsbAudioDataLenGet();
static void UsbDevicePlayRunning(uint16_t msgId);

void UsbDevicePlayResRelease(void)
{
	if(UsbDevicePlayCt->SpeakerBuffer != NULL)
	{
		DBG("SpeakerBuffer\n");
		osPortFree(UsbDevicePlayCt->SpeakerBuffer);
		UsbDevicePlayCt->SpeakerBuffer = NULL;
	}
	if(UsbDevicePlayCt->Source1Buf_USBAduio != NULL)
	{
		DBG("Source1Buf_USBAduio\n");
		osPortFree(UsbDevicePlayCt->Source1Buf_USBAduio);
		UsbDevicePlayCt->Source1Buf_USBAduio = NULL;
	}
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(UsbDevicePlayCt->Source2Decoder != NULL)
	{
		DBG("Source2Decoder\n");
		osPortFree(UsbDevicePlayCt->Source2Decoder);
		UsbDevicePlayCt->Source2Decoder = NULL;
	}
#endif
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustDisable();
#endif
}

bool UsbDevicePlayResMalloc(uint16_t SampleLen)
{
		//申请USB 缓存fifo，为块处理的 8倍大小
	UsbDevicePlayCt->SpeakerBuffer = (int8_t*)osPortMalloc(SampleLen * 2 * 2 * 2 * 2);
	if(UsbDevicePlayCt->SpeakerBuffer == NULL)
	{
		return FALSE;
	}
	MCUCircular_Config(&SpeakerCircularBuf, UsbDevicePlayCt->SpeakerBuffer, SampleLen * 2 * 2 * 2 * 2);
	memset(UsbDevicePlayCt->SpeakerBuffer, 0, SampleLen * 2 * 2 * 2 * 2);

	//InCore1 buf
	UsbDevicePlayCt->Source1Buf_USBAduio = (uint16_t*)osPortMalloc(SampleLen * 2 * 2);//stereo one Frame
	if(UsbDevicePlayCt->Source1Buf_USBAduio == NULL)
	{
		return FALSE;
	}
	memset(UsbDevicePlayCt->Source1Buf_USBAduio, 0, SampleLen * 2 * 2);
		//InCore buf
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	UsbDevicePlayCt->Source2Decoder = (uint16_t*)osPortMalloc(SampleLen * 2 * 2);//One Frame
	if(UsbDevicePlayCt->Source2Decoder == NULL)
	{
		return FALSE;
	}
	memset(UsbDevicePlayCt->Source2Decoder, 0, SampleLen * 2 * 2);//2K
#endif

	return TRUE;
}

void UsbDevicePlayResInit(void)
{
	if(UsbDevicePlayCt->Source1Buf_USBAduio != NULL)
	{
		UsbDevicePlayCt->AudioCoreUsb->AudioSource[USB_AUDIO_SOURCE_NUM].PcmInBuf = (int16_t *)UsbDevicePlayCt->Source1Buf_USBAduio;
	}
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(UsbDevicePlayCt->Source2Decoder != NULL)
	{
		UsbDevicePlayCt->AudioCoreUsb->AudioSource[REMIND_SOURCE_NUM].PcmInBuf = (int16_t *)UsbDevicePlayCt->Source2Decoder;
	}
#endif

#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustEnable(USB_AUDIO_SOURCE_NUM, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) / 4, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) * 3 / 4);
#endif
}

//USB声卡模式参数配置，资源初始化
static bool UsbDevicePlay_Init(MessageHandle parentMsgHandle)
{
	memset(&UsbAudioSpeaker,0,sizeof(UsbAudioSpeaker));
	memset(&UsbAudioMic,0,sizeof(UsbAudioMic));

//System config
	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);

//Task & App Config
	UsbDevicePlayCt = (UsbDevicePlayContext*)osPortMalloc(sizeof(UsbDevicePlayContext));
	memset(UsbDevicePlayCt, 0, sizeof(UsbDevicePlayContext));
	UsbDevicePlayCt->msgHandle = MessageRegister(USB_DEVICE_NUM_MESSAGE_QUEUE);
	if(UsbDevicePlayCt->msgHandle == NULL)
	{
		return FALSE;
	}
	UsbDevicePlayCt->parentMsgHandle = parentMsgHandle;
	UsbDevicePlayCt->state = TaskStateCreating;
	UsbDevicePlayCt->SampleRate = CFG_PARA_SAMPLE_RATE;

#ifdef CFG_FUNC_MIXER_SRC_EN
	extern ResamplerContext*	ResamplerCt1;
	extern int16_t* SRCOutBuf11;

	UsbDevicePlayCt->ResamplerCt = (ResamplerContext*)osPortMalloc(sizeof(ResamplerContext));
	if(UsbDevicePlayCt->ResamplerCt == NULL)
	{
		return FALSE;
	}
	ResamplerCt1 = UsbDevicePlayCt->ResamplerCt;
	UsbDevicePlayCt->SRCOutBuf = (int16_t*)osPortMalloc(USB_AUDIO_SRC_BUF_LEN);
	if(UsbDevicePlayCt->SRCOutBuf == NULL)
	{
		return FALSE;
	}
	SRCOutBuf11 = UsbDevicePlayCt->SRCOutBuf;
#endif

	if(!UsbDevicePlayResMalloc(mainAppCt.SamplesPreFrame))
	{
		DBG("UsbDevicePlayResMalloc Res Error!\n");
		return FALSE;
	}
//	//申请USB 缓存fifo，为块处理的 8倍大小
//	UsbDevicePlayCt->SpeakerBuffer = (int8_t*)osPortMalloc(USB_FIFO_LEN);
//	if(UsbDevicePlayCt->SpeakerBuffer == NULL)
//	{
//		return FALSE;
//	}
//	MCUCircular_Config(&SpeakerCircularBuf, UsbDevicePlayCt->SpeakerBuffer, USB_FIFO_LEN);
//	memset(UsbDevicePlayCt->SpeakerBuffer, 0, USB_FIFO_LEN);
//
//	//InCore1 buf
//	UsbDevicePlayCt->Source1Buf_USBAduio = (uint16_t*)osPortMalloc(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//stereo one Frame
//	if(UsbDevicePlayCt->Source1Buf_USBAduio == NULL)
//	{
//		return FALSE;
//	}
//	memset(UsbDevicePlayCt->Source1Buf_USBAduio, 0, CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);

	//Core Source1 para
	UsbDevicePlayCt->AudioCoreUsb = (AudioCoreContext*)&AudioCore;
	//Audio init
//	//note Soure0.和sink0已经在main app中配置，不要随意配置
	//Core Soure1.Para

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	//保存现有的音量
	UsbDevicePlayCt->LeftVolSave = UsbDevicePlayCt->AudioCoreUsb->AudioSink[0].LeftCurVol;
	UsbDevicePlayCt->RightVolSave = UsbDevicePlayCt->AudioCoreUsb->AudioSink[0].RightCurVol;
#endif

	UsbDevicePlayCt->AudioCoreUsb->AudioSource[USB_AUDIO_SOURCE_NUM].Enable = FALSE;//一开始为FALSE,枚举完成之后再置位,防止破音
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[USB_AUDIO_SOURCE_NUM].FuncDataGet = UsbAudioDataGet;//AudioADC0DataGet;
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[USB_AUDIO_SOURCE_NUM].FuncDataGetLen = UsbAudioDataLenGet;//AudioADC0DataLenGet;
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[USB_AUDIO_SOURCE_NUM].IsSreamData = TRUE;//TRUE;
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[USB_AUDIO_SOURCE_NUM].PcmFormat = 2;//stereo
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[USB_AUDIO_SOURCE_NUM].PcmInBuf = (int16_t *)UsbDevicePlayCt->Source1Buf_USBAduio;

	//InCore2 buf
//#if	defined(CFG_FUNC_REMIND_SOUND_EN)
//	UsbDevicePlayCt->Source2Decoder = (uint16_t*)osPortMalloc(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//One Frame
//	if(UsbDevicePlayCt->Source2Decoder == NULL)
//	{
//		return FALSE;
//	}
//	memset(UsbDevicePlayCt->Source2Decoder, 0, CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//2K
//#endif
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	//Core Soure2 Para
	DecoderSourceNumSet(REMIND_SOURCE_NUM);
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[REMIND_SOURCE_NUM].Enable = 0;
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[REMIND_SOURCE_NUM].FuncDataGet = DecodedPcmDataGet;
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[REMIND_SOURCE_NUM].FuncDataGetLen = NULL;
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[REMIND_SOURCE_NUM].IsSreamData = FALSE;//Decoder
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[REMIND_SOURCE_NUM].PcmFormat = 2; //stereo
	UsbDevicePlayCt->AudioCoreUsb->AudioSource[REMIND_SOURCE_NUM].PcmInBuf = (int16_t *)UsbDevicePlayCt->Source2Decoder;
#endif

	//Core Process
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
    #ifdef CFG_FUNC_MIC_KARAOKE_EN
	UsbDevicePlayCt->AudioCoreUsb->AudioEffectProcess = (AudioCoreProcessFunc)AudioEffectProcess;
	#else
	UsbDevicePlayCt->AudioCoreUsb->AudioEffectProcess = (AudioCoreProcessFunc)AudioMusicProcess;
	#endif
#else
	UsbDevicePlayCt->AudioCoreUsb->AudioEffectProcess = (AudioCoreProcessFunc)AudioBypassProcess;
#endif
#ifdef CFG_FUNC_RECORDER_EN
	UsbDevicePlayCt->RecorderSync = TaskStateNone;
	UsbDevicePlayCt->EncoderSync = TaskStateNone;

#endif

	return TRUE;
}

//创建从属services
static void UsbDevicePlayModeCreate(void)
{
	bool NoService = TRUE;
	// Create service task
#if defined(CFG_FUNC_REMIND_SBC)
	DecoderServiceCreate(UsbDevicePlayCt->msgHandle, DECODER_BUF_SIZE_SBC);//提示音格式决定解码器内存消耗
	NoService = FALSE;
#elif defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceCreate(UsbDevicePlayCt->msgHandle, DECODER_BUF_SIZE_MP3);
	NoService = FALSE;
#endif
	if(NoService)
	{
		UsbDevicePlayModeCreating(MSG_NONE);
	}
}

//All of services is created, Send CREATED message to parent
static void UsbDevicePlayModeCreating(uint16_t msgId)
{
	MessageContext		msgSend;
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(msgId == MSG_DECODER_SERVICE_CREATED)
	{
		UsbDevicePlayCt->DecoderSync = TaskStateReady;
	}
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(UsbDevicePlayCt->DecoderSync == TaskStateReady)
#endif
	{
		msgSend.msgId		= MSG_USB_DEVICE_MODE_CREATED;
		MessageSend(UsbDevicePlayCt->parentMsgHandle, &msgSend);
		UsbDevicePlayCt->state = TaskStateReady;
	}
}

static void UsbDevicePlayModeStart(void)
{
	bool NoService = TRUE;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceStart();
	UsbDevicePlayCt->DecoderSync = TaskStateStarting;
	NoService = FALSE;
#endif
	if(NoService)
	{
		UsbDevicePlayModeStarting(MSG_NONE);
	}
	else
	{
		UsbDevicePlayCt->state = TaskStateStarting;
	}
}

static void UsbDevicePlayModeStarting(uint16_t msgId)
{
	MessageContext		msgSend;
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(msgId == MSG_DECODER_SERVICE_STARTED)
	{
		UsbDevicePlayCt->DecoderSync = TaskStateRunning;
	}
#endif

	DBG("UsbDevicePlayModeStarting\n");
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(UsbDevicePlayCt->DecoderSync == TaskStateRunning)
#endif
	{
		msgSend.msgId		= MSG_USB_DEVICE_MODE_STARTED;
		MessageSend(UsbDevicePlayCt->parentMsgHandle, &msgSend);
		UsbDevicePlayCt->state = TaskStateRunning;

#ifdef CFG_FUNC_REMIND_SOUND_EN
		if(!RemindSoundServiceItemRequest(SOUND_REMIND_SHENGKAM)) //插播提示音:模式启动
		{
			AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
			DBG("usb device Unmute\n");
		}
#endif
	}
}

static void UsbDevicePlayModeStop(void)
{
	bool NoService = TRUE;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceStop();
	NoService = FALSE;
#endif
#ifdef CFG_FUNC_RECORDER_EN
	if(UsbDevicePlayCt->RecorderSync != TaskStateNone)
	{//此service 随用随Kill
		MediaRecorderServiceStop();
		UsbDevicePlayCt->RecorderSync = TaskStateStopping;
		NoService = FALSE;
	}
	if(UsbDevicePlayCt->EncoderSync != TaskStateNone)
	{//此service 随用随Kill
		EncoderServiceStop();
		UsbDevicePlayCt->EncoderSync = TaskStateStopping;
		NoService = FALSE;
	}
#endif

	UsbDevicePlayCt->state = TaskStateStopping;
	if(NoService)
	{
		UsbDevicePlayModeStopping(MSG_NONE);
	}

}

static void UsbDevicePlayModeStopping(uint16_t msgId)
{
	MessageContext		msgSend;

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	//退出usb模式后还原音量
	UsbDevicePlayCt->AudioCoreUsb->AudioSink[0].LeftVol = UsbDevicePlayCt->LeftVolSave;
	UsbDevicePlayCt->AudioCoreUsb->AudioSink[0].RightVol = UsbDevicePlayCt->RightVolSave;
#endif

	//Set para
#ifdef CFG_FUNC_RECORDER_EN
	if(msgId == MSG_MEDIA_RECORDER_SERVICE_STOPPED)
	{
		DBG("UsbDevice:RecorderKill");
		MediaRecorderServiceKill();
		UsbDevicePlayCt->RecorderSync = TaskStateNone;
	}
	if(msgId == MSG_ENCODER_SERVICE_STOPPED)
	{
		DBG("UsbDevice:EncoderKill");
		EncoderServiceKill();
		UsbDevicePlayCt->EncoderSync = TaskStateNone;
	}
#endif
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(msgId == MSG_DECODER_SERVICE_STOPPED)
	{
		UsbDevicePlayCt->DecoderSync = TaskStateNone;
	}
#endif

	if((UsbDevicePlayCt->state == TaskStateStopping)
#ifdef CFG_FUNC_RECORDER_EN
		&& (UsbDevicePlayCt->RecorderSync == TaskStateNone)
		&& (UsbDevicePlayCt->EncoderSync == TaskStateNone)
#endif
#ifdef CFG_FUNC_REMIND_SOUND_EN
		&& (UsbDevicePlayCt->DecoderSync == TaskStateNone)
#endif
		)
	{
		NVIC_DisableIRQ(Usb_IRQn);
		OTG_DeviceDisConnect();
		IsUsbAudioMode = FALSE;

		//clear msg
		MessageClear(UsbDevicePlayCt->msgHandle);

		//Set state
		UsbDevicePlayCt->state = TaskStateStopped;

		//reply
		msgSend.msgId = MSG_USB_DEVICE_MODE_STOPPED;
		MessageSend(UsbDevicePlayCt->parentMsgHandle, &msgSend);
		vTaskDelay(0xFFFFFFFF);//wait task killed
	}
}


uint16_t UsbAudioDataGet(void *Buffer,uint16_t Len)// bkd find get usb data to dac dma buffer
{
	uint16_t Length = 0;//Samples

	Len = Len*4;
	Length = MCUCircular_GetData(&SpeakerCircularBuf,Buffer,Len);

	return Length/4;
}

uint16_t UsbAudioDataLenGet()
{
	uint16_t Len;
	Len = MCUCircular_GetDataLen(&SpeakerCircularBuf);
	Len = Len/4;
	return Len;
}


static void UsbDevicePlayEntrance(void * param)
{
	MessageContext		msgRecv;

#if (defined(CFG_APP_USB_AUDIO_MODE_EN))&&(defined(CFG_COMMUNICATION_BY_USB))
	SetDeviceInitState(TRUE);
#endif

	DBG("UsbDevice:App\n");
	UsbDevicePlayModeCreate();
	
#ifdef CFG_FUNC_AUDIO_EFFECT_EN	
	AudioEffectModeSel(mainAppCt.EffectMode, 2);//0=init hw,1=effect,2=hw+effect
	AudioEffectParamSync();
#ifdef CFG_COMMUNICATION_BY_UART
	UART1_Communication_Init((void *)(&UartRxBuf[0]), 1024, (void *)(&UartTxBuf[0]), 1024);
#endif
#endif

#ifdef CFG_FUNC_MIXER_SRC_EN
	resampler_init(UsbDevicePlayCt->ResamplerCt, 2, 48000, CFG_PARA_SAMPLE_RATE, 0, 0);
#endif
	IsUsbAudioMode = TRUE;
	OTG_DeviceModeSel(AUDIO_HID,0x8888,0x1234);
	OTG_DeviceInit();
	NVIC_EnableIRQ(Usb_IRQn);

#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_SYS_INFO);
#endif

	//如果进入模式AudioCore为静音状态，则unmute
//	if(IsAudioPlayerMute() == TRUE)
//	{
//		AudioPlayerMute();
//	}
#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceUnmute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif

	while(1)
	{
		MessageRecv(UsbDevicePlayCt->msgHandle, &msgRecv, 10);
		
		switch(msgRecv.msgId)
		{
			case MSG_DECODER_SERVICE_CREATED:
				UsbDevicePlayModeCreating(msgRecv.msgId);
				break;

			case MSG_TASK_START:
				DBG("UsbDevice:TASK_START\n");
				UsbDevicePlayModeStart();
				break;

			case MSG_DECODER_SERVICE_STARTED:
				UsbDevicePlayModeStarting(msgRecv.msgId);
#ifdef CFG_FUNC_FREQ_ADJUST	//此处需要加声卡数据确认
				AudioCoreSourceFreqAdjustEnable(USB_AUDIO_SOURCE_NUM, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) / 4, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) * 3 / 4);
#endif
				break;

			case MSG_TASK_STOP:
#ifdef CFG_FUNC_REMIND_SOUND_EN
				RemindSoundServiceReset();
#endif
				UsbDevicePlayModeStop();
				break;

			case MSG_MEDIA_RECORDER_SERVICE_STOPPED:
			case MSG_DECODER_SERVICE_STOPPED:
			case MSG_ENCODER_SERVICE_STOPPED:
				UsbDevicePlayModeStopping(msgRecv.msgId);
				break;
			
			case MSG_APP_RES_RELEASE:
				UsbDevicePlayResRelease();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_RELEASE_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_MALLOC:
				UsbDevicePlayResMalloc(mainAppCt.SamplesPreFrame);
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_MALLOC_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_INIT:
				UsbDevicePlayResInit();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_INIT_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_REMIND_SOUND_PLAY_START:
				DBG("usb device: MSG_REMIND_SOUND_PLAY_START：\n");
				break;
					
			case MSG_REMIND_SOUND_PLAY_DONE://提示音播放结束
			case MSG_REMIND_SOUND_PLAY_REQUEST_FAIL:
				DBG("usb device: MSG_REMIND_SOUND_PLAY_DONE：\n");
				AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
				break;	
			default:
				if(UsbDevicePlayCt->state == TaskStateRunning)
				{
					UsbDevicePlayRunning(msgRecv.msgId);
				}
				break;
		}
		OTG_DeviceRequestProcess();
	}
}

void PCAudioPP(void);
void PCAudioNext(void);
void PCAudioPrev(void);
void PCAudioStop(void);
static void UsbDevicePlayRunning(uint16_t msgId)
{
	switch(msgId)
	{
#ifdef	CFG_FUNC_POWERKEY_EN
		case MSG_TASK_POWERDOWN:
			DBG("UsbDevice:PowerDown MSG, Please breakpoint\n");
			SystemPowerDown();
			break;
#endif
		case MSG_DECODER_STOPPED:
#if defined(CFG_FUNC_REMIND_SOUND_EN)
		{
			MessageContext		msgSend;
			msgSend.msgId = msgId;
			MessageSend(GetRemindSoundServiceMessageHandle(), &msgSend);//提示音期间转发解码器消息
		}
#endif
			break;

#ifdef CFG_FUNC_RECORDER_EN
		case MSG_DEVICE_SERVICE_RECORDER:
			if(ResourceValue(AppResourceCard) || ResourceValue(AppResourceUDisk))
			{
				if(UsbDevicePlayCt->RecorderSync == TaskStateNone)
				{
					MediaRecorderServiceCreate(UsbDevicePlayCt->msgHandle);
					EncoderServiceCreate(UsbDevicePlayCt->msgHandle);
					UsbDevicePlayCt->RecorderSync = TaskStateCreating;
					UsbDevicePlayCt->EncoderSync = TaskStateCreating;
				}
				else if(UsbDevicePlayCt->RecorderSync == TaskStateRunning)//再按录音键停止
				{
					MediaRecorderStop();
					MediaRecorderServiceStop();
					EncoderServiceStop();
					UsbDevicePlayCt->RecorderSync = TaskStateStopping;
					UsbDevicePlayCt->EncoderSync = TaskStateStopping;
				}
			}
			else
			{//flashfs录音 待处理
				DBG("Usbdevice:error, no disk!!!\n");
			}
			break;

		case MSG_MEDIA_RECORDER_SERVICE_CREATED:
			UsbDevicePlayCt->RecorderSync = TaskStateStarting;
			MediaRecorderServiceStart();
			break;

		case MSG_ENCODER_SERVICE_CREATED:
			UsbDevicePlayCt->EncoderSync = TaskStateStarting;
			EncoderServiceStart();
			break;

		case MSG_MEDIA_RECORDER_SERVICE_STARTED:
			MediaRecorderRun();
			UsbDevicePlayCt->RecorderSync = TaskStateRunning;
			break;

		case MSG_ENCODER_SERVICE_STARTED:
			UsbDevicePlayCt->EncoderSync = TaskStateRunning;
			break;

		case MSG_MEDIA_RECORDER_STOPPED:
			MediaRecorderServiceStop();
			UsbDevicePlayCt->RecorderSync = TaskStateStopping;
			break;

		case MSG_MEDIA_RECORDER_ERROR:
		case MSG_DEVICE_SERVICE_STOP:
			if(UsbDevicePlayCt->RecorderSync == TaskStateRunning)
			{
				MediaRecorderStop();
				MediaRecorderServiceStop();
				UsbDevicePlayCt->RecorderSync = TaskStateStopping;
				EncoderServiceStop();
				UsbDevicePlayCt->EncoderSync = TaskStateStopping;
			}
			else
			{
				PCAudioStop();
			}
			break;

		case MSG_DEVICE_SERVICE_PAUSE:
			DBG("Play Pause\n");
			PCAudioPP();
			break;

		case MSG_DEVICE_SERVICE_PRE:
			DBG("PRE Song\n");
			PCAudioPrev();
			break;

		case MSG_DEVICE_SERVICE_NEXT:
			DBG("next Song\n");
			PCAudioNext();
			break;
#endif //录音

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
bool UsbDevicePlayCreate(MessageHandle parentMsgHandle)
{
	bool	ret;

	ret = UsbDevicePlay_Init(parentMsgHandle);
	if(ret)
	{
		UsbDevicePlayCt->taskHandle = NULL;
		xTaskCreate(UsbDevicePlayEntrance,
					"UsbDevicePlay",
					USB_DEVICE_PLAY_TASK_STACK_SIZE,
					NULL, USB_DEVICE_PLAY_TASK_PRIO,
					&UsbDevicePlayCt->taskHandle);
		if(UsbDevicePlayCt->taskHandle == NULL)
		{
			ret = FALSE;
		}
	}
	if(!ret)
	{
		DBG("UsbDevice:create fail!\n");
	}
	return ret;
}


bool UsbDevicePlayStart(void)
{
	MessageContext		msgSend;
	if(UsbDevicePlayCt == NULL)
	{
		return FALSE;
	}
	DBG("UsbDevice:PlayStart\n");
	msgSend.msgId		= MSG_TASK_START;
	return MessageSend(UsbDevicePlayCt->msgHandle, &msgSend);
}

bool UsbDevicePlayPause(void)
{
	MessageContext		msgSend;
	if(UsbDevicePlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_PAUSE;
	return MessageSend(UsbDevicePlayCt->msgHandle, &msgSend);
}

bool UsbDevicePlayResume(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_TASK_RESUME;
	MessageSend(UsbDevicePlayCt->msgHandle, &msgSend);

	return 0;
}

bool UsbDevicePlayStop(void)
{
	MessageContext		msgSend;
	if(UsbDevicePlayCt == NULL)
	{
		return FALSE;
	}
	AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceMute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif
	vTaskDelay(30);

	msgSend.msgId		= MSG_TASK_STOP;
	return MessageSend(UsbDevicePlayCt->msgHandle, &msgSend);
}


bool UsbDevicePlayKill(void)
{
	if(UsbDevicePlayCt == NULL)
	{
		return FALSE;
	}
	//Kill used services
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	AudioCoreSourceDisable(REMIND_SOURCE_NUM);
	DecoderServiceKill();
#endif
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustDisable();
#endif

	//注意：AudioCore父任务调整到mainApp下，此处只关闭AudioCore通道，不关闭任务
	AudioCoreProcessConfig(NULL);
	AudioCoreSourceDisable(USB_AUDIO_SOURCE_NUM);
	vTaskDelay(2);

	//task
	if(UsbDevicePlayCt->taskHandle != NULL)
	{
		vTaskDelete(UsbDevicePlayCt->taskHandle);
		UsbDevicePlayCt->taskHandle = NULL;
	}

	//Msgbox
	if(UsbDevicePlayCt->msgHandle != NULL)
	{
		MessageDeregister(UsbDevicePlayCt->msgHandle);
		UsbDevicePlayCt->msgHandle = NULL;
	}

	//PortFree
	UsbDevicePlayCt->AudioCoreUsb = NULL;
	if(UsbDevicePlayCt->Source1Buf_USBAduio != NULL)
	{
		osPortFree(UsbDevicePlayCt->Source1Buf_USBAduio);
		UsbDevicePlayCt->Source1Buf_USBAduio = NULL;
	}

	if(UsbDevicePlayCt->SpeakerBuffer != NULL)
	{
		osPortFree(UsbDevicePlayCt->SpeakerBuffer);
		UsbDevicePlayCt->SpeakerBuffer = NULL;
	}

#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(UsbDevicePlayCt->Source2Decoder != NULL)
	{
		osPortFree(UsbDevicePlayCt->Source2Decoder);
		UsbDevicePlayCt->Source2Decoder = NULL;
	}
#endif

#ifdef CFG_FUNC_MIXER_SRC_EN
	if(UsbDevicePlayCt->SRCOutBuf != NULL)
	{
		osPortFree(UsbDevicePlayCt->SRCOutBuf);
		UsbDevicePlayCt->SRCOutBuf = NULL;
	}
	if(UsbDevicePlayCt->ResamplerCt != NULL)
	{
		osPortFree(UsbDevicePlayCt->ResamplerCt);
		UsbDevicePlayCt->ResamplerCt = NULL;
	}
#endif

	osPortFree(UsbDevicePlayCt);
	UsbDevicePlayCt = NULL;

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioEffectsDeInit();
#endif

#if (defined(CFG_APP_USB_AUDIO_MODE_EN))&&(defined(CFG_COMMUNICATION_BY_USB))
	SetUSBAudioExitFlag(TRUE);
#endif

	return TRUE;
}

MessageHandle GetUsbDeviceMessageHandle(void)
{
	if(UsbDevicePlayCt == NULL)
	{
		return NULL;
	}
	return UsbDevicePlayCt->msgHandle;
}

uint8_t UsbAudioSourceNum(void)
{
	return USB_AUDIO_SOURCE_NUM;
}

void UsbAudioTimer1msProcess(void)
{
	uint32_t FramLen = 192;
	static uint32_t FramCount = 0;
	static  bool IsUsbAduioInitFlag = FALSE;

	if(IsUsbAudioMode == FALSE)
	{
		return;
	}
	if(UsbAudioMic.InitOk == 0)
	{
		IsUsbAduioInitFlag = FALSE;
		return;
	}
	if(UsbAudioSpeaker.InitOk == 0)
	{
		IsUsbAduioInitFlag = FALSE;
		return;
	}

	FramCount++;
	if(AudioDAC_SampleRateGet(DAC0) == 44100)
	{
		FramLen = 176;
		if((FramCount%10) == 0)
		{
			FramCount = 0;
			FramLen = 180;
		}
	}
	//if(SpeakerRun == 0)////PC没有传送任何数据
	if(UsbAudioSpeaker.AltSet == 0)
	{
		//需要根据采样率填写数据
		MCUCircular_PutData(&SpeakerCircularBuf,(void*)SilentData,FramLen);
	}
#ifdef USB_TEST //audio core 送数据
	{
		uint32_t Len = 0;
		Len = MCUCircular_GetDataLen(&SpeakerCircularBuf);
		//定时送数据
		if(Len >= FramLen)
		{
			Len = FramLen;
		}
		MCUCircular_GetData(&SpeakerCircularBuf,PcmBuffer,Len);
		AudioDAC_DataSet(DAC0, PcmBuffer, Len/4);
	}
#endif

	//adc
	//if(MicRun == 0)////PC没有传送任何数据
	if(UsbAudioMic.AltSet == 0)
	{
		//需要根据采样率丢弃数据
	}
	if(!IsUsbAduioInitFlag)
	{
		UsbDevicePlayCt->AudioCoreUsb->AudioSource[1].Enable = TRUE;
		//DBG("%d\n",UsbDevicePlayCt.AudioCoreUsb->AudioSource[1].Enable);
		IsUsbAduioInitFlag = TRUE;
	}
}

#endif //CFG_APP_USB_AUDIO_MODE_EN

