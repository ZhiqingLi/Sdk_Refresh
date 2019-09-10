/**
 **************************************************************************************
 * @file    Spdif_mode.c
 * @brief
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-12-26 13:06:47$
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
#include "spdif.h"
#include "dac_interface.h"
#include "clk.h"
#include "audio_core_api.h"
#include "audio_core_service.h"
#include "audio_effect.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "backup_interface.h"
#include "resampler.h"
#include "mcu_circular_buf.h"
#include "breakpoint.h"
#include "otg_device_hcd.h"
#include "otg_device_standard_request.h"
#include "main_task.h"
#include "decoder_service.h"
#include "remind_sound_service.h"
#include "recorder_service.h"
#include "encoder_service.h"
#include "spdif_mode.h"
#include "audio_vol.h"
#include "ctrlvars.h"
#include "reset.h"
#ifdef CFG_FUNC_SPDIF_EN

#define SPDIF_PLAY_TASK_STACK_SIZE		512//1024
#define SPDIF_PLAY_TASK_PRIO			4//3
#define SPDIF_NUM_MESSAGE_QUEUE			10

#define SPDIF_SOURCE_NUM				APP_SOURCE_NUM

//spdif单个采样点8字节
//recv, dma buf len,MAX_FRAME_SAMPLES * 2 * 2 * 2 * 2是基础，OS切换间隔实测需要加倍。
#define	SPDIF_FIFO_LEN					(MAX_FRAME_SAMPLES * 2 * 2 * 2 * 2 * 2)
//由于采样值从32bit转换为16bit，可以使用同一个buf，否则要独立申请
#define SPDIF_CARRY_LEN					(MAX_FRAME_SAMPLES * 2 * 2 * 2)//支持192000输入 buf len for get data form dma fifo, deal
//转采样输出buf,如果spdif转采样提升大于四倍需要加大此SPDIF_CARRY_LEN，比如输入8000以下转48000,需要缩小单次carry帧大小或调大SPDIF_RESAMPLER_OUT_LEN、SPDIF_PCM_FIFO_LEN
#define SPDIF_RESAMPLER_OUT_LEN			(MAX_FRAME_SAMPLES * 2 * 2 * 4)
//audiocore混音前缓冲fifo 大小
//#define SPDIF_PCM_FIFO_LEN				(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2 * 2 * 2)
//#define SPDIF_SOURCE_LEN				(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2) //one frame for audiocore source1 carry

/**根据appconfig缺省配置:DMA 8个通道配置**/
/*1、SD卡播放和录音需PERIPHERAL_ID_SDIO1 RX/TX*/
/*2、在线串口调音需PERIPHERAL_ID_UART1 RX/TX*/ 
/*3、SPDIF需PERIPHERAL_ID_SDPIF_RX*/
/*4、IR按键需PERIPHERAL_ID_TIMER4，mode之间通道必须一致*/
/*5、Mic开启需PERIPHERAL_ID_AUDIO_ADC1_RX，mode之间通道必须一致*/
/*6、Dac0开启需PERIPHERAL_ID_AUDIO_DAC0_TX mode之间通道必须一致*/
/*7、DacX需开启PERIPHERAL_ID_AUDIO_DAC1_TX mode之间通道必须一致*/

static uint8_t DmaChannelMap[29] = {
	255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
	255,//PERIPHERAL_ID_SPIS_TX,		//1
	255,//PERIPHERAL_ID_TIMER3,			//2
	5,//PERIPHERAL_ID_SDIO_RX,			//3
	6,//PERIPHERAL_ID_SDIO0_TX,			//4
	255,//PERIPHERAL_ID_UART0_RX,		//5
	255,//PERIPHERAL_ID_TIMER1,			//6
	255,//PERIPHERAL_ID_TIMER2,			//7
	4,//PERIPHERAL_ID_SDPIF_RX,		//8 SPDIF_RX /TX same chanell
	4,//PERIPHERAL_ID_SDPIF_TX,		//8 SPDIF_RX /TX same chanell
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


typedef struct _SpdifPlayContext
{
	xTaskHandle 		taskHandle;
	MessageHandle		msgHandle;
	MessageHandle		parentMsgHandle;

	TaskState			state;
	uint32_t			*SpdifPwcFIFO;		//
	uint16_t 			*Source1Buf_Spdif;//
	uint32_t            *SpdifCarry;
	uint32_t			*SpdifPcmFifo;
	MCU_CIRCULAR_CONTEXT SpdifPcmCircularBuf;

	AudioCoreContext 	*AudioCoreSpdif;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	uint16_t*			Source2Decoder;
	TaskState			DecoderSync;
	bool				IsSoundRemindDone;
#endif

#ifdef CFG_FUNC_RECORDER_EN
	TaskState			RecorderSync;
	TaskState			EncoderSync;
#endif
	//play
	uint32_t 			SampleRate;
	
#ifdef CFG_FUNC_MIXER_SRC_EN
	ResamplerContext*	ResamplerCt;
	uint32_t*			resampleOutBuf;
#endif
	uint32_t			SpdifSampleRate;

	uint32_t 			SpdifDmaWritePtr;
	uint32_t 			SpdifDmaReadPtr;
	uint32_t 			SpdifPreSample;
	uint8_t  			SpdifSampleRateCheckFlg;
	uint32_t 			SpdifSampleRateFromSW;
}SpdifPlayContext;
static  SpdifPlayContext*		SpdifPlayCt;

static void SpdifPlayModeCreating(uint16_t msgId);
static void SpdifPlayModeStarting(uint16_t msgId);
static void SpdifPlayModeStopping(uint16_t msgId);
uint16_t Spdif_Rx_DataLenGet(void);
uint16_t Spdif_Rx_DataGet(void *pcm_out, uint16_t MaxSize);
void SpdifDataCarry(void);
static void SpdifPlayRunning(uint16_t msgId);

void SpdifPlayResRelease(void)
{
	DMA_ChannelDisable(PERIPHERAL_ID_SPDIF_RX);
	Reset_FunctionReset(DMAC_FUNC_SEPA);
	
	DMA_InterruptFlagClear(PERIPHERAL_ID_SPDIF_RX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_SPDIF_RX, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_SPDIF_RX, DMA_ERROR_INT);

	if(SpdifPlayCt->Source1Buf_Spdif != NULL)
	{
		DBG("Source1Buf_Spdif\n");
		osPortFree(SpdifPlayCt->Source1Buf_Spdif);
		SpdifPlayCt->Source1Buf_Spdif = NULL;
	}

	if(SpdifPlayCt->SpdifPcmFifo != NULL)
	{
		DBG("SpdifPcmFifo\n");
		osPortFree(SpdifPlayCt->SpdifPcmFifo);
		SpdifPlayCt->SpdifPcmFifo = NULL;
	}
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(SpdifPlayCt->Source2Decoder != NULL)
	{
		DBG("Source2Decoder\n");
		osPortFree(SpdifPlayCt->Source2Decoder);
		SpdifPlayCt->Source2Decoder = NULL;
	}
#endif
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustDisable();
#endif
}

bool SpdifPlayResMalloc(uint16_t SampleLen)
{
	//Spdif数据Rx是32bit存储，取出过程帧空间需放大到帧* 8。
	SpdifPlayCt->Source1Buf_Spdif = (uint16_t*)osPortMallocFromEnd(SampleLen * 2 * 2);//stereo
	if(SpdifPlayCt->Source1Buf_Spdif == NULL)
	{
		return FALSE;
	}
	memset(SpdifPlayCt->Source1Buf_Spdif, 0, SampleLen * 2 * 2);

	SpdifPlayCt->SpdifPcmFifo = (uint32_t *)osPortMallocFromEnd(SampleLen * 2 * 2 * 2 * 2);
	if(SpdifPlayCt->SpdifPcmFifo == NULL)
	{
		return FALSE;
	}
	MCUCircular_Config(&SpdifPlayCt->SpdifPcmCircularBuf, SpdifPlayCt->SpdifPcmFifo, SampleLen * 2 * 2 * 2 * 2);
	memset(SpdifPlayCt->SpdifPcmFifo, 0, SampleLen * 2 * 2 * 2 * 2);
	
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	SpdifPlayCt->Source2Decoder = (uint16_t*)osPortMallocFromEnd(SampleLen * 2 * 2);//One Frame
	if(SpdifPlayCt->Source2Decoder == NULL)
	{
		return FALSE;
	}
	memset(SpdifPlayCt->Source2Decoder, 0, SampleLen * 2 * 2);//2K 
#endif

	return TRUE;
}

void SpdifPlayResInit(void)
{
	if(SpdifPlayCt->Source1Buf_Spdif != NULL)
	{
		SpdifPlayCt->AudioCoreSpdif->AudioSource[SPDIF_SOURCE_NUM].PcmInBuf = (int16_t *)SpdifPlayCt->Source1Buf_Spdif;
	}
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(SpdifPlayCt->Source2Decoder != NULL)
	{
		SpdifPlayCt->AudioCoreSpdif->AudioSource[REMIND_SOURCE_NUM].PcmInBuf = (int16_t *)SpdifPlayCt->Source2Decoder;
	}
#endif	
	DMA_ChannelEnable(PERIPHERAL_ID_SPDIF_RX);
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustEnable(SPDIF_SOURCE_NUM, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) / 4, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) * 3 / 4);
#endif
}

//Spdif模式参数配置，资源初始化
static bool SpdifPlay_Init(MessageHandle parentMsgHandle)
{
	//将SPDIF时钟切换到AUPLL
	Clock_SpdifClkSelect(APLL_CLK_MODE);

	DMA_ChannelAllocTableSet(DmaChannelMap);//optical

	SpdifPlayCt = (SpdifPlayContext*)osPortMalloc(sizeof(SpdifPlayContext));
	memset(SpdifPlayCt, 0, sizeof(SpdifPlayContext));
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	SpdifPlayCt->IsSoundRemindDone = FALSE;
	DBG("IsSoundRemindDone = %d\n", SpdifPlayCt->IsSoundRemindDone);
#endif
	SpdifPlayCt->msgHandle = MessageRegister(SPDIF_NUM_MESSAGE_QUEUE);
	SpdifPlayCt->parentMsgHandle = parentMsgHandle;
	SpdifPlayCt->state = TaskStateCreating;

	// Audio core config
	SpdifPlayCt->SampleRate = 0;//CFG_PARA_SAMPLE_RATE;
	if(!SpdifPlayResMalloc(mainAppCt.SamplesPreFrame))
	{
		DBG("LineInPlay Res Error!\n");
		return FALSE;
	}

	//LineIn1  digital (DMA)
	SpdifPlayCt->SpdifPwcFIFO = (uint32_t*)osPortMalloc(SPDIF_FIFO_LEN);
	if(SpdifPlayCt->SpdifPwcFIFO == NULL)
	{
		return FALSE;
	}
	memset(SpdifPlayCt->SpdifPwcFIFO, 0, SPDIF_FIFO_LEN);

#ifdef CFG_APP_OPTICAL_MODE_EN
	if(GetSystemMode() == AppModeOpticalAudioPlay)
	{
		//spdif config
#ifndef PORT_B_INPUT_DIGATAL	//bkd add	
		GPIO_PortAModeSet(SPDIF_OPTICAL_INDEX, SPDIF_OPTICAL_PORT_MODE);
#else
		GPIO_PortBModeSet(SPDIF_OPTICAL_INDEX, SPDIF_OPTICAL_PORT_MODE);
#endif
#ifdef CFG_APP_COAXIAL_MODE_EN
		GPIO_PortAModeSet(SPDIF_COAXIAL_INDEX, 0);
#endif
		//SPDIF_AnalogModuleDisable();//bkd mark
		SPDIF_AnalogModuleEnable(SPDIF_OPTICAL_PORT_ANA_INPUT, SPDIF_ANA_LEVEL_300mVpp);
	}
#endif
#ifdef CFG_APP_COAXIAL_MODE_EN
	if(GetSystemMode() == AppModeCoaxialAudioPlay)
	{	
		GPIO_PortAModeSet(SPDIF_COAXIAL_INDEX, SPDIF_COAXIAL_PORT_MODE);
#ifdef CFG_APP_OPTICAL_MODE_EN
		GPIO_PortAModeSet(SPDIF_OPTICAL_INDEX, 0);
#endif
		SPDIF_AnalogModuleEnable(SPDIF_COAXIAL_PORT_ANA_INPUT, SPDIF_ANA_LEVEL_300mVpp);
	}
#endif

	DMA_ChannelDisable(PERIPHERAL_ID_SPDIF_RX);
	Reset_FunctionReset(DMAC_FUNC_SEPA);
	SPDIF_ModuleDisable();
	SPDIF_RXInit(1, 0, 0);
	//使用
	DMA_CircularConfig(PERIPHERAL_ID_SPDIF_RX, SPDIF_FIFO_LEN / 2, (void*)SpdifPlayCt->SpdifPwcFIFO, SPDIF_FIFO_LEN);
	SPDIF_ModuleEnable();
	DMA_ChannelEnable(PERIPHERAL_ID_SPDIF_RX);

	//Spdif数据Rx是32bit存储，取出过程帧空间需放大到帧* 8。
//	SpdifPlayCt->Source1Buf_Spdif = (uint16_t*)osPortMalloc(SPDIF_SOURCE_LEN);//stereo
//	if(SpdifPlayCt->Source1Buf_Spdif == NULL)
//	{
//		return FALSE;
//	}
//	memset(SpdifPlayCt->Source1Buf_Spdif, 0, SPDIF_SOURCE_LEN);

	SpdifPlayCt->SpdifCarry = (uint32_t *)osPortMalloc(SPDIF_CARRY_LEN);
	if(SpdifPlayCt->SpdifCarry == NULL)
	{
		return FALSE;
	}
	memset(SpdifPlayCt->SpdifCarry, 0, SPDIF_CARRY_LEN);

#ifdef CFG_FUNC_MIXER_SRC_EN
	SpdifPlayCt->ResamplerCt = (ResamplerContext*)osPortMalloc(sizeof(ResamplerContext));
	if(SpdifPlayCt->ResamplerCt == NULL)
	{
		return FALSE;
	}

	SpdifPlayCt->resampleOutBuf = (uint32_t *)osPortMalloc(SPDIF_RESAMPLER_OUT_LEN);
	if(SpdifPlayCt->resampleOutBuf == NULL)
	{
		return FALSE;
	}
#endif
	SpdifPlayCt->SpdifDmaWritePtr 		 = 0;
	SpdifPlayCt->SpdifDmaReadPtr  		 = 0;
	SpdifPlayCt->SpdifSampleRateCheckFlg = 0;
	SpdifPlayCt->SpdifSampleRateFromSW 	 = 0;
	SpdifPlayCt->SpdifPreSample			 = 0;


//	SpdifPlayCt->SpdifPcmFifo = (uint32_t *)osPortMalloc(SPDIF_PCM_FIFO_LEN);
//	if(SpdifPlayCt->SpdifPcmFifo == NULL)
//	{
//		return FALSE;
//	}
//	MCUCircular_Config(&SpdifPlayCt->SpdifPcmCircularBuf, SpdifPlayCt->SpdifPcmFifo, SPDIF_PCM_FIFO_LEN);
//	memset(SpdifPlayCt->SpdifPcmFifo, 0, SPDIF_PCM_FIFO_LEN);


	//Audio init
	SpdifPlayCt->AudioCoreSpdif = (AudioCoreContext*)&AudioCore;
	//Soure1.
	SpdifPlayCt->AudioCoreSpdif->AudioSource[SPDIF_SOURCE_NUM].Enable = 0;
	SpdifPlayCt->AudioCoreSpdif->AudioSource[SPDIF_SOURCE_NUM].FuncDataGet = Spdif_Rx_DataGet;
	SpdifPlayCt->AudioCoreSpdif->AudioSource[SPDIF_SOURCE_NUM].FuncDataGetLen = Spdif_Rx_DataLenGet;
	SpdifPlayCt->AudioCoreSpdif->AudioSource[SPDIF_SOURCE_NUM].IsSreamData = TRUE;
	SpdifPlayCt->AudioCoreSpdif->AudioSource[SPDIF_SOURCE_NUM].PcmFormat = 2;//stereo
	SpdifPlayCt->AudioCoreSpdif->AudioSource[SPDIF_SOURCE_NUM].PcmInBuf = (int16_t *)SpdifPlayCt->Source1Buf_Spdif;

	//InCore2 buf
//#if	defined(CFG_FUNC_REMIND_SOUND_EN)
//	SpdifPlayCt->Source2Decoder = (uint16_t*)osPortMalloc(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//One Frame
//	if(SpdifPlayCt->Source2Decoder == NULL)
//	{
//		return FALSE;
//	}
//	memset(SpdifPlayCt->Source2Decoder, 0, CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//2K 
//#endif
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	//Core Soure2 Para
	DecoderSourceNumSet(REMIND_SOURCE_NUM);
	SpdifPlayCt->AudioCoreSpdif->AudioSource[REMIND_SOURCE_NUM].Enable = 0;
	SpdifPlayCt->AudioCoreSpdif->AudioSource[REMIND_SOURCE_NUM].FuncDataGet = DecodedPcmDataGet;
	SpdifPlayCt->AudioCoreSpdif->AudioSource[REMIND_SOURCE_NUM].FuncDataGetLen = NULL;
	SpdifPlayCt->AudioCoreSpdif->AudioSource[REMIND_SOURCE_NUM].IsSreamData = FALSE;//Decoder
	SpdifPlayCt->AudioCoreSpdif->AudioSource[REMIND_SOURCE_NUM].PcmFormat = 2; //stereo
	SpdifPlayCt->AudioCoreSpdif->AudioSource[REMIND_SOURCE_NUM].PcmInBuf = (int16_t *)SpdifPlayCt->Source2Decoder;
#endif
	
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
    #ifdef CFG_FUNC_MIC_KARAOKE_EN
	SpdifPlayCt->AudioCoreSpdif->AudioEffectProcess = (AudioCoreProcessFunc)AudioEffectProcess;
	#else
	SpdifPlayCt->AudioCoreSpdif->AudioEffectProcess = (AudioCoreProcessFunc)AudioMusicProcess;
	#endif
#else
	SpdifPlayCt->AudioCoreSpdif->AudioEffectProcess = (AudioCoreProcessFunc)AudioBypassProcess;
#endif
#ifdef CFG_FUNC_RECORDER_EN
	SpdifPlayCt->RecorderSync = TaskStateNone;
	SpdifPlayCt->EncoderSync = TaskStateNone;
#endif	
	return TRUE;
}

//创建从属services
static void SpdifPlayModeCreate(void)
{
	bool NoService = TRUE;
	// Create service task
#if defined(CFG_FUNC_REMIND_SBC)
	DecoderServiceCreate(SpdifPlayCt->msgHandle, DECODER_BUF_SIZE_SBC);//提示音格式决定解码器内存消耗
	NoService = FALSE;
#elif defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceCreate(SpdifPlayCt->msgHandle, DECODER_BUF_SIZE_MP3);
	NoService = FALSE;
#endif
	if(NoService)
	{
		SpdifPlayModeCreating(MSG_NONE);
	}
}

//All of services is created
//Send CREATED message to parent
static void SpdifPlayModeCreating(uint16_t msgId)
{
	MessageContext		msgSend;
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(msgId == MSG_DECODER_SERVICE_CREATED)
	{
		SpdifPlayCt->DecoderSync = TaskStateReady;
	}
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(SpdifPlayCt->DecoderSync == TaskStateReady)
#endif
	{
		msgSend.msgId		= MSG_SPDIF_AUDIO_MODE_CREATED;
		MessageSend(SpdifPlayCt->parentMsgHandle, &msgSend);
		SpdifPlayCt->state = TaskStateReady;
	}
}

static void SpdifPlayModeStart(void)
{
	bool NoService = TRUE;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceStart();
	SpdifPlayCt->DecoderSync = TaskStateStarting;
	NoService = FALSE;
#endif
	if(NoService)
	{
		SpdifPlayModeStarting(MSG_NONE);
	}
	else
	{
		SpdifPlayCt->state = TaskStateStarting;
	}
}

static void SpdifPlayModeStarting(uint16_t msgId)
{
	MessageContext		msgSend;
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(msgId == MSG_DECODER_SERVICE_STARTED)
	{
		SpdifPlayCt->DecoderSync = TaskStateRunning;
	}
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(SpdifPlayCt->DecoderSync == TaskStateRunning)
#endif
	{
		msgSend.msgId		= MSG_SPDIF_AUDIO_MODE_STARTED;
		MessageSend(SpdifPlayCt->parentMsgHandle, &msgSend);
		SpdifPlayCt->state = TaskStateRunning;
		
#ifdef CFG_FUNC_REMIND_SOUND_EN
		if(GetSystemMode() == AppModeOpticalAudioPlay)
		{
			if(!RemindSoundServiceItemRequest(SOUND_REMIND_GXIANMOD))//插播提示音
			{
				SpdifPlayCt->IsSoundRemindDone = TRUE;//无提示音，标志位位1
			}
		}
		else
		{
			if(!RemindSoundServiceItemRequest(SOUND_REMIND_TZHOUMOD))
			{
				SpdifPlayCt->IsSoundRemindDone = TRUE;
			}
		}
#endif
	}
}

static void SpdifPlayModeStop(void)
{
	bool NoService = TRUE;
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	DecoderServiceStop();
	NoService = FALSE;
#endif
#ifdef CFG_FUNC_RECORDER_EN
	if(SpdifPlayCt->RecorderSync != TaskStateNone)
	{//此service 随用随Kill
		MediaRecorderServiceStop();
		SpdifPlayCt->RecorderSync = TaskStateStopping;
		NoService = FALSE;
	}
	if(SpdifPlayCt->EncoderSync != TaskStateNone)
	{//此service 随用随Kill
		EncoderServiceStop();
		SpdifPlayCt->EncoderSync = TaskStateStopping;
		NoService = FALSE;
	}
#endif

	SpdifPlayCt->state = TaskStateStopping;
	if(NoService)
	{
		SpdifPlayModeStopping(MSG_NONE);
	}
}

static void SpdifPlayModeStopping(uint16_t msgId)
{
	MessageContext		msgSend;
	//Set para
#ifdef CFG_FUNC_RECORDER_EN
	if(msgId == MSG_MEDIA_RECORDER_SERVICE_STOPPED)
	{
		DBG("Spdif:RecorderKill");
		MediaRecorderServiceKill();
		SpdifPlayCt->RecorderSync = TaskStateNone;
	}
	if(msgId == MSG_ENCODER_SERVICE_STOPPED)
	{
		DBG("Spdif:EncoderKill");
		EncoderServiceKill();
		SpdifPlayCt->EncoderSync = TaskStateNone;
	}
#endif	
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(msgId == MSG_DECODER_SERVICE_STOPPED)
	{
		SpdifPlayCt->DecoderSync = TaskStateNone;
	}
#endif

	if((SpdifPlayCt->state == TaskStateStopping)
#ifdef CFG_FUNC_RECORDER_EN
		&& (SpdifPlayCt->RecorderSync == TaskStateNone)
		&& (SpdifPlayCt->EncoderSync == TaskStateNone)
#endif
#ifdef CFG_FUNC_REMIND_SOUND_EN
		&& (SpdifPlayCt->DecoderSync == TaskStateNone)
#endif
		)
	{
		//Set para

		//clear msg
		MessageClear(SpdifPlayCt->msgHandle);

		//Set state
		SpdifPlayCt->state = TaskStateStopped;

		//reply
		msgSend.msgId		= MSG_SPDIF_AUDIO_MODE_STOPPED;
		MessageSend(SpdifPlayCt->parentMsgHandle, &msgSend);
	}
}

static void SpdifSampleRateChange(void)
{

#ifdef CFG_FUNC_MIXER_SRC_EN
	resampler_init(SpdifPlayCt->ResamplerCt, 2, SpdifPlayCt->SpdifSampleRate, CFG_PARA_SAMPLE_RATE, 0, 0);
#else
	if(SpdifPlayCt->SampleRate != SpdifPlayCt->SpdifSampleRate)
	{
		SpdifPlayCt->SampleRate = SpdifPlayCt->SpdifSampleRate;//注意此处调整会造成提示音和mic数据dac不正常。
		DBG("Dac Sample:%d\n",(int)SpdifPlayCt->SampleRate);
#ifdef CFG_RES_AUDIO_DACX_EN
		AudioDAC_SampleRateChange(ALL, SpdifPlayCt->SampleRate);
#else
		AudioDAC_SampleRateChange(DAC0, SpdifPlayCt->SampleRate);
#endif
	}
#endif

}

static void SpdifPlayEntrance(void * param)
{
	MessageContext		msgRecv;
	uint32_t samplerate = 0;
	bool SpdifLockFlag = FALSE;
	// Create services
	SpdifPlayModeCreate();
#ifdef CFG_FUNC_AUDIO_EFFECT_EN	
	AudioEffectModeSel(mainAppCt.EffectMode, 2);//0=init hw,1=effect,2=hw+effect
	AudioEffectParamSync();
#ifdef CFG_COMMUNICATION_BY_UART
	UART1_Communication_Init((void *)(&UartRxBuf[0]), 1024, (void *)(&UartTxBuf[0]), 1024);
#endif
#endif

	if(GetSystemMode() == AppModeCoaxialAudioPlay)
		DBG("Coaxial Mode\n");
	else
		DBG("Spdif:App\n");
	SpdifPlayModeCreating(MSG_NONE);//无service，直接返回created
#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_SYS_INFO);
#endif

//	//如果进入模式AudioCore为静音状态，则unmute
//	if(IsAudioPlayerMute() == TRUE)
//	{
//		AudioPlayerMute();
//	}
#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceUnmute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif
	while(1)
	{
		MessageRecv(SpdifPlayCt->msgHandle, &msgRecv, 1);
	
		switch(msgRecv.msgId)
		{
#ifdef	CFG_FUNC_POWERKEY_EN
			case MSG_TASK_POWERDOWN:
				DBG("MSG receive PowerDown, Please breakpoint\n");
				SystemPowerDown();
				break;
#endif
			case MSG_TASK_CREATE://API, not msg, only happy
				break;
				
			case MSG_DECODER_SERVICE_CREATED:
				SpdifPlayModeCreating(msgRecv.msgId);
				break;

			case MSG_TASK_START:
				SpdifPlayModeStart();
				SpdifPlayModeStarting(MSG_NONE);//无service，直接向上级返回
				//vTaskDelay(100);//延时用于确保提示音启动消息已经发出来并被app接收到
				break;

			case MSG_DECODER_SERVICE_STARTED:
				SpdifPlayModeStarting(msgRecv.msgId);//无service，直接向上级返回
				break;

			case MSG_TASK_STOP:
#ifdef CFG_FUNC_REMIND_SOUND_EN
			RemindSoundServiceReset();
#endif
#if 0//CFG_COMMUNICATION_BY_USB
				NVIC_DisableIRQ(Usb_IRQn);
				OTG_DeviceDisConnect();
#endif
				SpdifPlayModeStop();
				SpdifPlayModeStopping(MSG_NONE);//无service，直接向上级返回
				break;

			case MSG_MEDIA_RECORDER_SERVICE_STOPPED:
			case MSG_DECODER_SERVICE_STOPPED:
			case MSG_ENCODER_SERVICE_STOPPED:
				SpdifPlayModeStopping(msgRecv.msgId);
				break;

			case MSG_APP_RES_RELEASE:
				SpdifPlayResRelease();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_RELEASE_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_MALLOC:
				SpdifPlayResMalloc(mainAppCt.SamplesPreFrame);
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_MALLOC_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_INIT:
				SpdifPlayResInit();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_INIT_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
				
			case MSG_REMIND_SOUND_PLAY_START:
				DBG("spdifin: MSG_REMIND_SOUND_PLAY_START：\n");
				#if	defined(CFG_FUNC_REMIND_SOUND_EN)
				SpdifPlayCt->IsSoundRemindDone = FALSE;
				#endif
				break;
					
			case MSG_REMIND_SOUND_PLAY_DONE://提示音播放结束
			case MSG_REMIND_SOUND_PLAY_REQUEST_FAIL://提示音音源获取失败
				DBG("spdifin: MSG_REMIND_SOUND_PLAY_DONE：\n");
				#if	defined(CFG_FUNC_REMIND_SOUND_EN)
				SpdifPlayCt->IsSoundRemindDone = TRUE;
				#endif
				if(SpdifLockFlag == TRUE)
				{
					AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
				}
				break;
				
			default:
				if(SpdifPlayCt->state == TaskStateRunning)
				{
					SpdifPlayRunning(msgRecv.msgId);
					if(SpdifLockFlag && !SPDIF_FlagStatusGet(LOCK_FLAG_STATUS))
					{
						DBG("SPDIF RX UNLOCK!\n");
						SpdifLockFlag = FALSE;
						AudioCoreSourceDisable(SPDIF_SOURCE_NUM);
				#ifdef CFG_FUNC_FREQ_ADJUST
						AudioCoreSourceFreqAdjustDisable();
				#endif
					}
					if(!SpdifLockFlag && SPDIF_FlagStatusGet(LOCK_FLAG_STATUS)
					#if	defined(CFG_FUNC_REMIND_SOUND_EN)
						&&SpdifPlayCt->IsSoundRemindDone
					#endif	
					)
					{
						DBG("SPDIF RX LOCK!\n");
						SpdifLockFlag = TRUE;
						AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
						vTaskDelay(20);
						AudioCoreSourceEnable(SPDIF_SOURCE_NUM);
						AudioCoreSourceUnmute(SPDIF_SOURCE_NUM, TRUE, TRUE);
				#ifdef CFG_FUNC_FREQ_ADJUST
						AudioCoreSourceFreqAdjustEnable(SPDIF_SOURCE_NUM, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) / 4, (mainAppCt.SamplesPreFrame * 2 * 2 * 2 * 2 / 4 - mainAppCt.SamplesPreFrame) * 3 / 4);
				#endif
					}

					//监控SPDIF RX采样率是否改变
					if(SpdifLockFlag == TRUE)
					{
						if(samplerate != SPDIF_SampleRateGet())
						{
							samplerate = SPDIF_SampleRateGet();

							SpdifPlayCt->SpdifSampleRate = samplerate;
							DBG("Get SampleRate: %d\n", (int)SpdifPlayCt->SpdifSampleRate);
							SpdifSampleRateChange();
						}

						SpdifDataCarry();
					}
					//任务优先级设置为4,通过发送该命令，可以提高AudioCore service有效利用率
					{
						MessageContext		msgSend;
						msgSend.msgId		= MSG_NONE;
						MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);
					}
				}
				break;
		}
	}
}

static void SpdifPlayRunning(uint16_t msgId)
{
	switch(msgId)
	{
#ifdef	CFG_FUNC_POWERKEY_EN
		case MSG_TASK_POWERDOWN:
			DBG("MSG receive PowerDown, Please breakpoint\n");
			SystemPowerDown();
			break;
#endif	

#ifdef CFG_FUNC_RECORDER_EN	
		case MSG_DEVICE_SERVICE_RECORDER:
			if(ResourceValue(AppResourceCard) || ResourceValue(AppResourceUDisk))
			{
				if(SpdifPlayCt->RecorderSync == TaskStateNone)
				{
					MediaRecorderServiceCreate(SpdifPlayCt->msgHandle);
					EncoderServiceCreate(SpdifPlayCt->msgHandle);
					SpdifPlayCt->RecorderSync = TaskStateCreating;
					SpdifPlayCt->EncoderSync = TaskStateCreating;
				}
				else if(SpdifPlayCt->RecorderSync == TaskStateRunning)//再按录音键 停止
				{
					MediaRecorderStop();
					MediaRecorderServiceStop();
					EncoderServiceStop();
					SpdifPlayCt->RecorderSync = TaskStateStopping;
					SpdifPlayCt->EncoderSync = TaskStateStopping;
				}
			}
			else
			{//flashfs录音 待处理
				DBG("Spdif:error, no disk!!!\n");
			}
			break;
			
		case MSG_MEDIA_RECORDER_SERVICE_CREATED:
			SpdifPlayCt->RecorderSync = TaskStateStarting;
			MediaRecorderServiceStart();
			break;
			
		case MSG_ENCODER_SERVICE_CREATED:
			SpdifPlayCt->EncoderSync = TaskStateStarting;
			EncoderServiceStart();
			break;

		case MSG_MEDIA_RECORDER_SERVICE_STARTED:
			MediaRecorderRun();
			SpdifPlayCt->RecorderSync = TaskStateRunning;
			break;
			
		case MSG_ENCODER_SERVICE_STARTED:
			SpdifPlayCt->EncoderSync = TaskStateRunning;
			break;

		case MSG_MEDIA_RECORDER_STOPPED:
			MediaRecorderServiceStop();
			SpdifPlayCt->RecorderSync = TaskStateStopping;
			break;
			
		case MSG_MEDIA_RECORDER_ERROR:
		case MSG_DEVICE_SERVICE_STOP:
			if(SpdifPlayCt->RecorderSync == TaskStateRunning)
			{
				MediaRecorderStop();
				MediaRecorderServiceStop();
				SpdifPlayCt->RecorderSync = TaskStateStopping;
				EncoderServiceStop();
				SpdifPlayCt->EncoderSync = TaskStateStopping;
			}
			break;
#endif //录音

		case MSG_DECODER_STOPPED:
#if defined(CFG_FUNC_REMIND_SOUND_EN)
			{
				MessageContext		msgSend;
				msgSend.msgId = msgId;
				MessageSend(GetRemindSoundServiceMessageHandle(), &msgSend);//提示音期间转发解码器消息。
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
bool SpdifPlayCreate(MessageHandle parentMsgHandle)
{
	bool		ret;
	ret = SpdifPlay_Init(parentMsgHandle);
	if(ret)
	{
		xTaskCreate(SpdifPlayEntrance,
				"SpdifPlay",
				SPDIF_PLAY_TASK_STACK_SIZE,
				NULL, SPDIF_PLAY_TASK_PRIO,
				&SpdifPlayCt->taskHandle);
		if(SpdifPlayCt->taskHandle == NULL)
		{
			ret = FALSE;
		}
	}
	if(!ret)
	{
		DBG("spdif:app create fail!\n");
	}
	return TRUE;
}

bool SpdifPlayStart(void)
{
	MessageContext		msgSend;
	if(SpdifPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_START;
	return MessageSend(SpdifPlayCt->msgHandle, &msgSend);
}

bool SpdifPlayPause(void)
{
	MessageContext		msgSend;
	if(SpdifPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_PAUSE;
	return MessageSend(SpdifPlayCt->msgHandle, &msgSend);
}

bool SpdifPlayResume(void)
{
	MessageContext		msgSend;
	if(SpdifPlayCt == NULL)
	{
		return FALSE;
	}
	msgSend.msgId		= MSG_TASK_RESUME;
	return MessageSend(SpdifPlayCt->msgHandle, &msgSend);
}

bool SpdifPlayStop(void)
{
	MessageContext		msgSend;
	if(SpdifPlayCt == NULL)
	{
		return FALSE;
	}
	AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceMute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif
	vTaskDelay(30);

	msgSend.msgId		= MSG_TASK_STOP;
	return MessageSend(SpdifPlayCt->msgHandle, &msgSend);
}

bool SpdifPlayKill(void)
{
	if(SpdifPlayCt == NULL)
	{
		return FALSE;
	}
	//Kill used services
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	AudioCoreSourceDisable(REMIND_SOURCE_NUM);
	DecoderServiceKill();
#endif

	AudioCoreProcessConfig(NULL);
	AudioCoreSourceDisable(SPDIF_SOURCE_NUM);//SPDIF_SOURCE_NUM
#ifdef CFG_FUNC_FREQ_ADJUST	
	AudioCoreSourceFreqAdjustDisable();
#endif
	DMA_ChannelDisable(PERIPHERAL_ID_SPDIF_RX);
	DMA_ChannelClose(PERIPHERAL_ID_SPDIF_RX);
	SPDIF_ModuleDisable();
#ifndef CFG_FUNC_MIXER_SRC_EN
#ifdef CFG_RES_AUDIO_DACX_EN
	AudioDAC_SampleRateChange(ALL, CFG_PARA_SAMPLE_RATE);//恢复
#else
	AudioDAC_SampleRateChange(DAC0, CFG_PARA_SAMPLE_RATE);//恢复
#endif
#endif

	SPDIF_AnalogModuleDisable();
	//GPIO配置为普通IO
#ifdef CFG_APP_COAXIAL_MODE_EN
	GPIO_PortAModeSet(SPDIF_COAXIAL_INDEX, 0);
#endif
#ifdef CFG_APP_OPTICAL_MODE_EN
	GPIO_PortAModeSet(SPDIF_OPTICAL_INDEX, 0);
#endif


#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(SpdifPlayCt->Source2Decoder != NULL)
	{
		osPortFree(SpdifPlayCt->Source2Decoder);
		SpdifPlayCt->Source2Decoder = NULL;
	}
#endif

	//task
	if(SpdifPlayCt->taskHandle != NULL)
	{
		vTaskDelete(SpdifPlayCt->taskHandle);
		SpdifPlayCt->taskHandle = NULL;
	}

	//Msgbox
	if(SpdifPlayCt->msgHandle != NULL)
	{
		MessageDeregister(SpdifPlayCt->msgHandle);
		SpdifPlayCt->msgHandle = NULL;
	}

	//PortFree
	if(SpdifPlayCt->Source1Buf_Spdif != NULL)
	{
		osPortFree(SpdifPlayCt->Source1Buf_Spdif);
		SpdifPlayCt->Source1Buf_Spdif = NULL;
	}

	if(SpdifPlayCt->SpdifCarry != NULL)
	{
		osPortFree(SpdifPlayCt->SpdifCarry);
		SpdifPlayCt->SpdifCarry = NULL;
	}

	if(SpdifPlayCt->SpdifPcmFifo != NULL)
	{
		osPortFree(SpdifPlayCt->SpdifPcmFifo);
		SpdifPlayCt->SpdifPcmFifo = NULL;
	}

	if(SpdifPlayCt->SpdifPwcFIFO != NULL)
	{
		osPortFree(SpdifPlayCt->SpdifPwcFIFO);
		SpdifPlayCt->SpdifPwcFIFO = NULL;
	}
	SpdifPlayCt->AudioCoreSpdif = NULL;

#ifdef CFG_FUNC_MIXER_SRC_EN
	if(SpdifPlayCt->ResamplerCt != NULL)
	{
		osPortFree(SpdifPlayCt->ResamplerCt);
		SpdifPlayCt->ResamplerCt = NULL;
	}
	if(SpdifPlayCt->resampleOutBuf != NULL)
	{
		osPortFree(SpdifPlayCt->resampleOutBuf);
		SpdifPlayCt->resampleOutBuf = NULL;
	}
#endif

	osPortFree(SpdifPlayCt);
	SpdifPlayCt = NULL;
	
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioEffectsDeInit();
#endif

	return TRUE;
}

MessageHandle GetSpdifPlayMessageHandle(void)
{
	if(SpdifPlayCt == NULL)
	{
		return FALSE;
	}
	return SpdifPlayCt->msgHandle;
}


//sample为单位
uint16_t Spdif_Rx_DataLenGet(void)
{
	return MCUCircular_GetDataLen(&SpdifPlayCt->SpdifPcmCircularBuf)/4;
}

//sample为单位，buf大小：8 * MaxSize
uint16_t Spdif_Rx_DataGet(void *pcm_out, uint16_t MaxPoint)
{
	return MCUCircular_GetData(&SpdifPlayCt->SpdifPcmCircularBuf, pcm_out, MaxPoint * 4) / 4;
}

void SpdifDataCarry(void)
{
	uint16_t pcm_space;
	uint16_t spdif_len;
	int16_t pcm_len;
	int16_t *pcmBuf  = (int16_t *)SpdifPlayCt->SpdifCarry;
	uint16_t cnt;

	spdif_len = DMA_CircularDataLenGet(PERIPHERAL_ID_SPDIF_RX);
	pcm_space = MCUCircular_GetSpaceLen(&SpdifPlayCt->SpdifPcmCircularBuf) - 16;

#ifdef CFG_FUNC_MIXER_SRC_EN
	pcm_space = (pcm_space * SpdifPlayCt->SpdifSampleRate) / CFG_PARA_SAMPLE_RATE - 16;
#endif

	if((spdif_len >> 1) > pcm_space)
	{
		spdif_len = pcm_space * 2;
	}
	//spdif_len = spdif_len /(MAX_FRAME_SAMPLES * 2 * 4) * (MAX_FRAME_SAMPLES * 2 * 4);
	spdif_len = spdif_len & 0xFFF8;
	if(!spdif_len)
	{
		return ;
	}
	cnt = (spdif_len / 8) / MAX_FRAME_SAMPLES;

	while(cnt--)
	{
		DMA_CircularDataGet(PERIPHERAL_ID_SPDIF_RX, pcmBuf, MAX_FRAME_SAMPLES * 8);

		int32_t *temp = (int32_t *)pcmBuf;
		uint16_t Count = 0;

		if(temp[0] & (1 << 28)) //左声道数据
		{
			//DBG("%x\n",temp[0]);
		}
		else//第一个是右声道则判断为错误帧，丢弃，读一个右声道数据，保证下一次读出来第一个是左声道
		{
			while(DMA_CircularDataLenGet(PERIPHERAL_ID_SPDIF_RX) < 4)
			{
				Count++;
				if(Count > 1000)
					break;
			}
			DMA_CircularDataGet(PERIPHERAL_ID_SPDIF_RX, pcmBuf, 4);
			//DBG("drop error frame\n");
			return;
		}

		//由于从32bit转换为16bit，buf可以使用同一个，否则要独立申请。
		pcm_len = SPDIF_SPDIFDataToPCMData((int32_t *)pcmBuf, MAX_FRAME_SAMPLES * 8, (int32_t *)pcmBuf, SPDIF_WORDLTH_16BIT);

		if(pcm_len < 0)
		{
			return;
		}
		if((pcm_len / 4) >MAX_FRAME_SAMPLES)
		{
			memset(pcmBuf, 0, MAX_FRAME_SAMPLES*4);
			pcm_len = MAX_FRAME_SAMPLES * 4;
		}
#ifdef CFG_FUNC_MIXER_SRC_EN
		pcm_len = resampler_apply(SpdifPlayCt->ResamplerCt, (int16_t*)pcmBuf, (int16_t*)SpdifPlayCt->resampleOutBuf, pcm_len/4);
		if(pcm_len<0)
		{
			return ;
		}
		MCUCircular_PutData(&SpdifPlayCt->SpdifPcmCircularBuf, SpdifPlayCt->resampleOutBuf, pcm_len*4);
#else
		MCUCircular_PutData(&SpdifPlayCt->SpdifPcmCircularBuf, pcmBuf, pcm_len);
#endif
	}
}

#endif //CFG_FUNC_SPDIF_EN

