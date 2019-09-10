/**
 **************************************************************************************
 * @file    bt_hf_mode.c
 * @brief   蓝牙通话模式
 *
 * @author  KK
 * @version V1.0.1
 *
 * $Created: 2019-3-28 18:00:00$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <string.h>
#include "type.h"
#include "app_config.h"
#include "app_message.h"
#include "mvintrinsics.h"
//driver
#include "chip_info.h"
#include "dac.h"
#include "gpio.h"
#include "dma.h"
#include "dac.h"
#include "audio_adc.h"
#include "debug.h"
//middleware
#include "main_task.h"
#include "audio_vol.h"
#include "rtos_api.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "bt_manager.h"
#include "resampler.h"
#include "mcu_circular_buf.h"
#include "audio_core_api.h"
#include "audio_decoder_api.h"
#include "sbcenc_api.h"
#include "bt_config.h"
#include "cvsd_plc.h"
#include "ctrlvars.h"
//application
#include "bt_hf_mode.h"
#include "decoder_service.h"
#include "audio_core_service.h"
#include "mode_switch_api.h"
#include "audio_core_api.h"
#include "bt_hf_api.h"
#include "bt_app_interface.h"
#include "recorder_service.h"

#ifdef CFG_FUNC_REMIND_SOUND_EN
#include "remind_sound_service.h"
#endif
//#include "powercontroller.h"

extern void ResumeAudioCoreMicSource(void);

#ifdef CFG_APP_BT_MODE_EN

#if (BT_HFP_SUPPORT == ENABLE)

static void BtHfModeStarted(void);
static void BtHfModeCreated(void);
static void BtHfModeStopping(uint16_t msgId);
static void BtHfModeStopped(void);

#define BT_HF_TASK_STACK_SIZE		384//512//256
#define BT_HF_TASK_PRIO				3
#define BT_HF_NUM_MESSAGE_QUEUE		10

//msbc encoder
#define MSBC_CHANNE_MODE	1 		// mono
#define MSBC_SAMPLE_REATE	16000	// 16kHz
#define MSBC_BLOCK_LENGTH	15

static void BtHfMsbcKill(void);
static void BtHfCvsdKill(void);

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

BtHfContext	*BtHfCt = NULL;

uint32_t hfModeRestart = 0;
uint32_t hfModeSuspend = 0;


static void BtHfModeCreate(void);

/*******************************************************************************
 * TIMER2定时器进行通话时长的计时
 * 1MS计算1次
 ******************************************************************************/
void BtHf_Timer1msCheck(void)
{
#ifdef BT_HFP_CALL_DURATION_DISP
	if(BtHfCt == NULL)
		return;
	
	if(GetHfpState() == BT_HFP_STATE_ACTIVE)
	{
		BtHfCt->BtHfTimerMsCount++;
		if(BtHfCt->BtHfTimerMsCount>=1000) //1s
		{
			BtHfCt->BtHfTimerMsCount = 0;
			BtHfCt->BtHfTimeUpdate = 1;
			BtHfCt->BtHfActiveTime++;
		}
	}
#endif
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfResRelease(void)
{
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC0_RX);
	
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_ERROR_INT);

	if(BtHfCt->ADCFIFO != NULL)
	{
		DBG("ADCFIFO\n");
		osPortFree(BtHfCt->ADCFIFO);
		BtHfCt->ADCFIFO = NULL;
	}
	if(BtHfCt->Source1Buf_ScoData != NULL)
	{
		DBG("Source1Buf_ScoData\n");
		osPortFree(BtHfCt->Source1Buf_ScoData);
		BtHfCt->Source1Buf_ScoData = NULL;
	}
/*#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	if(BtHfCt->SourceDecoder != NULL)
	{
		DBG("SourceDecoder\n");
		osPortFree(BtHfCt->SourceDecoder);
		BtHfCt->SourceDecoder = NULL;
	}
#endif*/
}

bool BtHfResMalloc(uint16_t SampleLen)
{
	//LineIn5  digital (DMA)
	//BtHfCt->ADCFIFO = (uint32_t*)osPortMalloc(ADC_FIFO_LEN);
	BtHfCt->ADCFIFO = (uint32_t*)osPortMallocFromEnd(SampleLen * 2 * 2 * 2);
	if(BtHfCt->ADCFIFO == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->ADCFIFO, 0, SampleLen * 2 * 2 * 2);

	//InCore1 buf
	//BtHfCt->Source1Buf_ScoData = (uint16_t*)osPortMalloc(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//stereo one Frame
	BtHfCt->Source1Buf_ScoData = (uint16_t*)osPortMallocFromEnd(SampleLen * 2 * 2);
	if(BtHfCt->Source1Buf_ScoData == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->Source1Buf_ScoData, 0, SampleLen * 2 * 2);
	
	//InCore buf
/*#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	//BtHfCt->SourceDecoder = (uint16_t*)osPortMalloc(CFG_PARA_SAMPLES_PER_FRAME * 2 * 2);//One Frame
	BtHfCt->SourceDecoder = (uint16_t*)osPortMallocFromEnd(SampleLen * 2 * 2);//One Frame
	if(BtHfCt->SourceDecoder == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->SourceDecoder, 0, SampleLen * 2 * 2);//2K
#endif*/
	return TRUE;
}

void BtHfResInit(void)
{
	if(BtHfCt->Source1Buf_ScoData != NULL)
	{
		BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].PcmInBuf = (int16_t *)BtHfCt->Source1Buf_ScoData;
	}
	
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC0_RX);
	DMA_CircularConfig(PERIPHERAL_ID_AUDIO_ADC0_RX, mainAppCt.SamplesPreFrame * 2 * 2, (void*)BtHfCt->ADCFIFO, mainAppCt.SamplesPreFrame * 2 * 2 * 2);
    DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_ADC0_RX);
    AudioADC_LREnable(ADC0_MODULE, 1, 1);
    AudioADC_Enable(ADC0_MODULE);
}

/***********************************************************************************
 **********************************************************************************/
bool BtHf_CvsdInit(void)
{
//CVSD 转采样配置 8K->16K       16K->8K
	//CVSD接收数据转采样 8K->16K
	BtHfCt->ScoInResamplerCt = (ResamplerContext*)osPortMalloc(sizeof(ResamplerContext));
	if(BtHfCt->ScoInResamplerCt == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->ScoInResamplerCt, 0, sizeof(ResamplerContext));
	BtHfCt->ScoInSrcOutBuf = (int16_t*)osPortMalloc(BTHF_RESAMPLE_FIFO_LEN);
	if(BtHfCt->ScoInSrcOutBuf == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->ScoInSrcOutBuf, 0, sizeof(BTHF_RESAMPLE_FIFO_LEN));
	resampler_init(BtHfCt->ScoInResamplerCt, 1, BTHF_CVSD_SAMPLE_RATE, CFG_BTHF_PARA_SAMPLE_RATE, 1, 0);
	
	//CVSD发送数据转采样 16K->8K
	BtHfCt->ScoOutResamplerCt = (ResamplerContext*)osPortMalloc(sizeof(ResamplerContext));
	if(BtHfCt->ScoOutResamplerCt == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->ScoOutResamplerCt, 0, sizeof(ResamplerContext));
	BtHfCt->ScoOutSrcOutBuf = (int16_t*)osPortMalloc(BTHF_RESAMPLE_FIFO_LEN);
	if(BtHfCt->ScoOutSrcOutBuf == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->ScoOutSrcOutBuf, 0, sizeof(BTHF_RESAMPLE_FIFO_LEN));
	resampler_init(BtHfCt->ScoOutResamplerCt, 1, CFG_BTHF_PARA_SAMPLE_RATE, BTHF_CVSD_SAMPLE_RATE, 1, 0);

	//cvsd plc config
	BtHfCt->cvsdPlcState = osPortMalloc(sizeof(CVSD_PLC_State));
	if(BtHfCt->cvsdPlcState == NULL)
	{
		DBG("CVSD PLC error\n");
		return FALSE;
	}
	memset(BtHfCt->cvsdPlcState, 0, sizeof(CVSD_PLC_State));
	cvsd_plc_init(BtHfCt->cvsdPlcState);

	BtHfCt->CvsdInitFlag = 1;
	return TRUE;
}

/***********************************************************************************
 **********************************************************************************/
bool BtHf_MsbcInit(void)
{
	//msbc decoder init
	if(BtHf_MsbcDecoderInit() != 0)
	{
		DBG("msbc decoder error\n");
		return FALSE;
	}

	//encoder init
	BtHf_MsbcEncoderInit();

	BtHfCt->MsbcInitFlag = 1;
	return TRUE;
}

/***********************************************************************************
 **********************************************************************************/
void BtHfModeRunning(void)
{
	if(BtHfCt->ModeKillFlag)
		return;
	
	if(GetHfpState() >= BT_HFP_STATE_CONNECTED)
	{
		if(GetScoConnectFlag() && hfModeRestart && BtHfCt->hfModeAudioResInitOk)
		{
			//AudioADC_DMARestart(ADC1_MODULE, (void*)mainAppCt.ADCFIFO, ADC_FIFO_LEN);
			
			if((!BtHfCt->CvsdInitFlag)&&(!BtHfCt->MsbcInitFlag))
			{
				DBG("calling start...\n");

				hfModeRestart = 0;
				
				//call resume 
				if(GetHfpScoAudioCodecType())
					BtHfCt->codecType = HFP_AUDIO_DATA_mSBC;
				else
					BtHfCt->codecType = HFP_AUDIO_DATA_PCM;
				
				if(BtHfCt->codecType == HFP_AUDIO_DATA_mSBC)
				{
					BtHf_MsbcInit();
					BtHfModeCreate();
				}
				else
				{
					BtHf_CvsdInit();
				}

				//sink1
				if(BtHfCt->codecType == HFP_AUDIO_DATA_mSBC)
				{
					//Audio init
					//Soure1.
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].Enable = 0;
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].FuncDataGet = DecodedPcmDataGet;
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].FuncDataGetLen = DecodedPcmDataLenGet;//NULL;
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].IsSreamData = FALSE;//Decoder
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].PcmFormat = 1; //mono
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].PcmInBuf = (int16_t*)BtHfCt->Source1Buf_ScoData;
					
				}
				else
				{
					//Audio init
					//Soure1.
					BtHfCt->DecoderSourceNum = BT_HF_SOURCE_NUM;
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].Enable = 0;
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].FuncDataGet = BtHf_ScoDataGet;
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].FuncDataGetLen = BtHf_ScoDataLenGet;
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].IsSreamData = FALSE;//Decoder
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].PcmFormat = 1; //mono
					BtHfCt->AudioCoreBtHf->AudioSource[BT_HF_SOURCE_NUM].PcmInBuf = (int16_t*)BtHfCt->Source1Buf_ScoData;
				}
				
				//adc
				AudioADC1ParamsSet(BtHfCt->SampleRate, BT_HFP_MIC_PGA_GAIN, 4);

				//sink2 channel
				memset(BtHfCt->Sink2ScoFifo, 0, BT_SCO_FIFO_LEN);
				MCUCircular_Config(&BtHfCt->Sink2ScoFifoCircular, BtHfCt->Sink2ScoFifo, BT_SCO_FIFO_LEN);

				BtHfCt->callingState = BT_HF_CALLING_ACTIVE;

				AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
				
				if(BtHfCt->codecType != HFP_AUDIO_DATA_mSBC)
				{
					bool flag = 0;
					BtAppiFunc_SaveScoData(BtHf_SaveScoData);
					AudioCoreSourceEnable(MIC_SOURCE_NUM);
					AudioCoreSourceEnable(BT_HF_SOURCE_NUM);
					AudioCoreSinkEnable(AUDIO_DAC0_SINK_NUM);
					AudioCoreSinkEnable(AUDIO_HF_SCO_SINK_NUM);

					BtHfCt->btHfScoSendReady = 1;
					BtHfCt->btHfScoSendStart = 0;

					GetBtHfpVoiceRecognition(&flag);
					if(flag)
					{
						SetHfpState(BT_HFP_STATE_ACTIVE);
					}
				}
			}
			else
			{
				BtHfTaskResume();
			}

			hfModeRestart = 0;
		}
	}
}

/***********************************************************************************
 **********************************************************************************/
/**
 * @func        BtHfInit
 * @brief       BtHf模式参数配置，资源初始化
 * @param       MessageHandle parentMsgHandle  
 * @Output      None
 * @return      int32_t
 * @Others      任务块、Dac、AudioCore配置，数据源自DecoderService
 * @Others      数据流从Decoder到audiocore配有函数指针，audioCore到Dac同理，由audiocoreService任务按需驱动
 * @Others      初始化通话模式所必须的模块,待模式启动后,根据SCO链路建立交互的数据格式对应的创建其他的任务；
 * Record
 */
static bool BtHfInit(MessageHandle parentMsgHandle)
{
	//DMA channel
	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);
	
	//Task & App Config
	BtHfCt = (BtHfContext*)osPortMalloc(sizeof(BtHfContext));
	if(BtHfCt == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt, 0, sizeof(BtHfContext));
	
	BtHfCt->msgHandle = MessageRegister(BT_HF_NUM_MESSAGE_QUEUE);
	if(BtHfCt->msgHandle == NULL)
	{
		return FALSE;
	}
	BtHfCt->state = TaskStateCreating;
	BtHfCt->parentMsgHandle = parentMsgHandle;
	
	/* Create media audio services */
	BtHfCt->SampleRate = CFG_BTHF_PARA_SAMPLE_RATE;//蓝牙模式下采样率为16K
	
	/*if(!BtHfResMalloc(mainAppCt.SamplesPreFrame))
	{
		DBG("BtHf Res Error!\n");
		return FALSE;
	}*/

	BtHfCt->AudioCoreBtHf = (AudioCoreContext*)&AudioCore;

	//ADC1 参数配置
	AudioADC1ParamsSet(BtHfCt->SampleRate, BT_HFP_MIC_PGA_GAIN, 4);

	//Soure0.
	mainAppCt.AudioCore->AudioSource[MIC_SOURCE_NUM].Enable = 0;
	mainAppCt.AudioCore->AudioSource[MIC_SOURCE_NUM].FuncDataGet = AudioADC1DataGet;
	mainAppCt.AudioCore->AudioSource[MIC_SOURCE_NUM].FuncDataGetLen = AudioADC1DataLenGet;
	mainAppCt.AudioCore->AudioSource[MIC_SOURCE_NUM].IsSreamData = TRUE;
	mainAppCt.AudioCore->AudioSource[MIC_SOURCE_NUM].PcmFormat = 2;//test mic audio effect
	mainAppCt.AudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf = (int16_t *)mainAppCt.Source0Buf_ADC1;
	//AudioCoreSourceEnable(MIC_SOURCE_NUM);

	//sco input/output 参数配置
	BtHfCt->ScoInputFifo = osPortMalloc(SCO_INPUT_FIFO_SIZE);
	if(BtHfCt->ScoInputFifo == NULL)
	{
		return FALSE;
	}
	BtHfCt->ScoOutputFifo = osPortMalloc(SCO_OUTPUT_FIFO_SIZE);
	if(BtHfCt->ScoOutputFifo == NULL)
	{
		return FALSE;
	}
	
	//call resume 
	if(GetHfpScoAudioCodecType())
		BtHfCt->codecType = HFP_AUDIO_DATA_mSBC;
	else
		BtHfCt->codecType = HFP_AUDIO_DATA_PCM;
	
	if(BtHfCt->codecType == HFP_AUDIO_DATA_PCM)
	{
		//sco 接收缓存 fifo
		BtHfCt->ScoBuffer = (int8_t*)osPortMalloc(BT_SCO_FIFO_LEN);
		if(BtHfCt->ScoBuffer == NULL)
		{
			return FALSE;
		}
		MCUCircular_Config(&BtHfCt->BtScoCircularBuf, BtHfCt->ScoBuffer, BT_SCO_FIFO_LEN);
		memset(BtHfCt->ScoBuffer, 0, BT_SCO_FIFO_LEN);
	}

//AudioCore通路配置
	//source in buf
	BtHfCt->Source1Buf_ScoData = (uint16_t*)osPortMalloc(CFG_BTHF_PARA_SAMPLES_PER_FRAME * 2);
	if(BtHfCt->Source1Buf_ScoData == NULL)
	{
		return FALSE;
	}
	memset(BtHfCt->Source1Buf_ScoData, 0, CFG_BTHF_PARA_SAMPLES_PER_FRAME * 2);
	
	#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	#ifdef CFG_FUNC_MIC_KARAOKE_EN
		BtHfCt->AudioCoreBtHf->AudioEffectProcess = (AudioCoreProcessFunc)AudioEffectProcess;
	#else
		BtHfCt->AudioCoreBtHf->AudioEffectProcess = (AudioCoreProcessFunc)AudioMusicProcess;
	#endif
	#else
		BtHfCt->AudioCoreBtHf->AudioEffectProcess = (AudioCoreProcessFunc)AudioBypassProcess;
	#endif

//DAC0/DACX
#ifdef CFG_RES_AUDIO_DACX_EN
	AudioDAC_SampleRateChange(ALL, BTHF_DAC_SAMPLE_RATE);//16K
#else
	AudioDAC_SampleRateChange(DAC0, BTHF_DAC_SAMPLE_RATE);//16K
#endif

	//sink2
	BtHfCt->Sink2PcmFifo = (int16_t*)osPortMalloc(CFG_BTHF_PARA_SAMPLES_PER_FRAME * 2);
	if(BtHfCt->Sink2PcmFifo == NULL)
	{
		return FALSE;
	}

	BtHfCt->Sink2ScoFifo = (int16_t*)osPortMalloc(BT_SCO_FIFO_LEN);
	if(BtHfCt->Sink2ScoFifo == NULL) 
	{
		return FALSE;
	}
	MCUCircular_Config(&BtHfCt->Sink2ScoFifoCircular, BtHfCt->Sink2ScoFifo, BT_SCO_FIFO_LEN);

//AudioCore sink2 config -- 用于HFP SCO发送数据
	BtHfCt->AudioCoreSinkHfSco = &AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM];
	BtHfCt->AudioCoreSinkHfSco->Enable = 0;
	BtHfCt->AudioCoreSinkHfSco->PcmFormat = 1;//nomo
	BtHfCt->AudioCoreSinkHfSco->FuncDataSet = BtHf_SinkScoDataSet;
	BtHfCt->AudioCoreSinkHfSco->FuncDataSpaceLenSet = BtHf_SinkScoDataSpaceLenGet;
	BtHfCt->AudioCoreSinkHfSco->PcmOutBuf = (int16_t *)BtHfCt->Sink2PcmFifo;

	AudioCoreSinkEnable(AUDIO_HF_SCO_SINK_NUM);

	if((BtHfCt->Sink2ScoFifoMutex = xSemaphoreCreateMutex()) == NULL)
	{
		return FALSE;
	}

	//需要发送数据缓存
	BtHfCt->BtScoSendFifo = (uint8_t*)osPortMalloc(BT_SCO_FIFO_LEN);
	if(BtHfCt->BtScoSendFifo == NULL)
	{
		return FALSE;
	}
	MCUCircular_Config(&BtHfCt->BtScoSendCircularBuf, BtHfCt->BtScoSendFifo, BT_SCO_FIFO_LEN);
	memset(BtHfCt->BtScoSendFifo, 0, BT_SCO_FIFO_LEN);

	//AEC
	BtHf_AECInit();
	return TRUE;
}


/*****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfMsgToParent(uint16_t id)
{
	MessageContext		msgSend;

	// Send message to main app
	msgSend.msgId		= id;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

uint8_t BtHfDecoderSourceNum(void)
{
	return BtHfCt->DecoderSourceNum;
}

static void BtHfDeinitialize(void)
{
	MessageContext		msgSend;

	BtHfCt->state = TaskStateNone;
	// Send message to main app
	msgSend.msgId		= MSG_BT_HF_MODE_STOPPED;
	MessageSend(BtHfCt->parentMsgHandle, &msgSend);
}

/*****************************************************************************************
 * 任务创建
 ****************************************************************************************/
//MSBC:先创建模式任务，然后等待Decoder服务创建，才判断为HF模式创建完成
static void BtHfModeCreate(void)
{
	DecoderServiceCreate(BtHfCt->msgHandle, DECODER_BUF_SIZE_SBC);
	BtHfCt->DecoderSync = TaskStateCreating;
}

//CVSD:直接开启HF模式，不需要Decoder
static void BtHfModeCreated(void)
{
	MessageContext		msgSend;
	
	DBG("Bt Handfree Mode Created\n");
	
	BtHfCt->state = TaskStateReady;
	msgSend.msgId		= MSG_BT_HF_MODE_CREATED;
	MessageSend(BtHfCt->parentMsgHandle, &msgSend);
}

/*****************************************************************************************
 * 任务启动
 ****************************************************************************************/
static void BtHfModeStarted(void)
{
	MessageContext		msgSend;
	
	DBG("Bt Handfree Mode Started\n");
	
	BtHfCt->state = TaskStateRunning;
	msgSend.msgId		= MSG_BT_HF_MODE_STARTED;
	MessageSend(BtHfCt->parentMsgHandle, &msgSend);
}

/*****************************************************************************************
 * 任务停止
 ****************************************************************************************/
//MSBC:
static void BtHfModeStop(void)
{
	bool NoService = TRUE;
	if(BtHfCt->DecoderSync != TaskStateStopped && BtHfCt->DecoderSync != TaskStateNone)
	{
		//先decoder stop
		DecoderServiceStop();
		NoService = FALSE;
		BtHfCt->DecoderSync = TaskStateStopping;
	}
#ifdef CFG_FUNC_RECORDER_EN
	if(BtHfCt->RecorderSync != TaskStateNone)
	{//此service 随用随Kill
		MediaRecorderServiceStop();
		BtHfCt->RecorderSync = TaskStateStopping;
		NoService = FALSE;
	}
#endif
	BtHfCt->state = TaskStateStopping;
	if(NoService)
	{
		BtHfModeStopping(MSG_NONE);
	}
}

static void BtHfModeStopping(uint16_t msgId)
{
	if(msgId == MSG_DECODER_SERVICE_STOPPED)
	{
		BT_DBG("BtHf:Decoder service Stopped\n");
		BtHfCt->DecoderSync = TaskStateStopped;
	}
#ifdef CFG_FUNC_RECORDER_EN
	else if(msgId == MSG_MEDIA_RECORDER_SERVICE_STOPPED)
	{
		DBG("BtHf:RecorderKill");
		MediaRecorderServiceKill();
		BtHfCt->RecorderSync = TaskStateNone;
	}
#endif	
	if((BtHfCt->state = TaskStateStopping)
#ifdef CFG_FUNC_RECORDER_EN
		&& (BtHfCt->RecorderSync == TaskStateNone)
#endif
		&& (BtHfCt->DecoderSync == TaskStateNone || BtHfCt->DecoderSync == TaskStateStopped)
		)
	{
		BtHfModeStopped();
	}
}

//CVSD
static void BtHfModeStopped(void)
{
	MessageContext		msgSend;
	
	DBG("Bt Handfree Mode Stopped\n");
	//Set para
	
	//clear msg
	MessageClear(BtHfCt->msgHandle);

	//Set state
	BtHfCt->state = TaskStateStopped;

	//reply
	msgSend.msgId		= MSG_BT_HF_MODE_STOPPED;
	MessageSend(BtHfCt->parentMsgHandle, &msgSend);
}


/*****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfTaskPause(void)
{
	MessageContext		msgSend;
	if(!BtHfCt)
		return;
	
	DBG("Bt Handfree Mode Pause\n");
	
	//Set state
	BtHfCt->state = TaskStatePausing;

	//reply
	msgSend.msgId		= MSG_TASK_PAUSE;
	MessageSend(BtHfCt->msgHandle, &msgSend);
}

void BtHfTaskResume(void)
{
	MessageContext		msgSend;
	if(!BtHfCt)
		return;
	
	DBG("Bt Handfree Mode Resume\n");
	
	//Set state
	BtHfCt->state = TaskStateResuming;

	//reply
	msgSend.msgId		= MSG_TASK_RESUME;
	MessageSend(BtHfCt->msgHandle, &msgSend);
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
static void BtHfEntrance(void * param)
{
	MessageContext		msgRecv;

	BtHfCt->hfModeAudioResInitOk = 0;

	DBG("Bt Handfree Call mode\n");
	BtHfModeCreated();

#ifdef BT_HFP_CALL_DURATION_DISP
	BtHfCt->BtHfActiveTime = 0;
	BtHfCt->BtHfTimerMsCount = 0;
	BtHfCt->BtHfTimeUpdate = 0;
#endif

	SetBtHfSyncVolume(mainAppCt.MusicVolume);

	//BtHf_AECEffectInit();
//#ifdef CFG_FUNC_AUDIO_EFFECT_EN
//	AudioEffectModeSel(mainAppCt.EffectMode, 2);//0=init hw,1=effect,2=hw+effect
//	AudioEffectParamSync();
//#ifdef CFG_COMMUNICATION_BY_UART
//	UART1_Communication_Init((void *)(&UartRxBuf[0]), 1024, (void *)(&UartTxBuf[0]), 1024);
//#endif
//#endif
	
#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceUnmute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif

	while(1)
	{
		MessageRecv(BtHfCt->msgHandle, &msgRecv, 1);

		switch(msgRecv.msgId)
		{
			//decoder service event
			case MSG_DECODER_SERVICE_CREATED:
				if(BtHfCt->state == TaskStateRunning)
				{
					BtHfCt->DecoderSync = TaskStateReady;
					DecoderServiceStart();
					BtHfCt->DecoderSync = TaskStateStarting;
				}
				break;
				
			//decoder service event
			case MSG_DECODER_SERVICE_STARTED:
				if(BtHfCt->state == TaskStateRunning)
				{
					bool flag = 0;
					BtHfCt->DecoderSync = TaskStateRunning;
					DBG("MSG_DECODER_SERVICE_STARTED\n");
					
					BtAppiFunc_SaveScoData(BtHf_SaveScoData);
					AudioCoreSourceEnable(MIC_SOURCE_NUM);
					AudioCoreSourceEnable(BT_HF_SOURCE_NUM);
					AudioCoreSinkEnable(AUDIO_HF_SCO_SINK_NUM);
					
					BtHfCt->callingState = BT_HF_CALLING_ACTIVE;
					BtHfCt->btHfScoSendReady = 1;

					
					GetBtHfpVoiceRecognition(&flag);
					if(flag)
					{
						SetHfpState(BT_HFP_STATE_ACTIVE);
					}
				}
				break;
			
			case MSG_TASK_START:
				//以下代码effect 流程整理之后需要删掉
				#ifdef CFG_FUNC_AUDIO_EFFECT_EN
				gCtrlVars.SamplesPerFrame = 256;
				if(gCtrlVars.SamplesPerFrame != mainAppCt.SamplesPreFrame)
				{
					extern bool IsEffectSampleLenChange;
					IsEffectSampleLenChange = 1;
					DBG("MSG_TASK_START IsEffectSampleLenChange == 1\n");
				}
				else
				{
					gCtrlVars.audio_effect_init_flag = 1;
					BtHf_AECEffectInit();
					gCtrlVars.audio_effect_init_flag = 0;
					BtHfCt->hfModeAudioResInitOk = 1;
				}
				#endif
				BtHfModeStarted();
				break;

			case MSG_TASK_PAUSE:
				BT_DBG("MSG_TASK_PAUSED\n");
				if((BtHfCt->callingState == BT_HF_CALLING_ACTIVE)&&(hfModeSuspend))
				{
					//通话挂起
					BtHfCt->state = TaskStatePaused;
					
					BtHfCt->callingState = BT_HF_CALLING_SUSPEND;

					//暂停source和sink服务
					BtAppiFunc_SaveScoData(NULL);

					//close source0
					//memset(mainAppCt.ADCFIFO, 0, ADC_FIFO_LEN);
					AudioCoreSourceDisable(MIC_SOURCE_NUM);

					//clear sco data
					//msbc
					BtHf_MsbcMemoryReset();
					//cvsd
					if(BtHfCt->ScoBuffer)
					{
						memset(BtHfCt->ScoBuffer, 0, BT_SCO_FIFO_LEN);
					}
					
					BtHfCt->btHfScoSendStart = 0;
				}
				break;

			case MSG_TASK_RESUME:
				//通话恢复
				if(GetHfpScoAudioCodecType() != (BtHfCt->codecType - 1))
				{
					BT_DBG("!!!SCO Audio Codec Type is changed %d -> %d", (BtHfCt->codecType-1), GetHfpScoAudioCodecType());
				}
				else
				{
					BT_DBG("sco audio resume...\n");
					BtAppiFunc_SaveScoData(BtHf_SaveScoData);
					BtHfCt->btHfScoSendReady = 1;
					BtHfCt->btHfScoSendStart = 0;
					
					AudioCoreSourceEnable(MIC_SOURCE_NUM);
					AudioCoreSourceEnable(BT_HF_SOURCE_NUM);
					AudioCoreSinkEnable(AUDIO_DAC0_SINK_NUM);
					AudioCoreSinkEnable(AUDIO_HF_SCO_SINK_NUM);
				}
				
				BtHfCt->state = TaskStateRunning;
				
				break;
			
			case MSG_TASK_STOP://此处响应Created时exit
//#ifdef CFG_FUNC_REMIND_SOUND_EN//如插播提示音，需复位。
//				RemindSoundServiceReset();
//#endif
#if 0//CFG_COMMUNICATION_BY_USB
				NVIC_DisableIRQ(Usb_IRQn);
				OTG_DeviceDisConnect();
#endif
				if(BtHfCt->codecType == HFP_AUDIO_DATA_mSBC)
					BtHfModeStop();
				else
					BtHfModeStopped();
				break;

			case MSG_MEDIA_RECORDER_SERVICE_STOPPED:
			case MSG_DECODER_SERVICE_STOPPED:
				if(BtHfCt->ModeKillFlag)
				{
					BtHfModeStopping(msgRecv.msgId);
				}
				else
				{	
					BtHfCt->DecoderSync = TaskStateStopped;
					BT_DBG("msg:BtHf:Decoder service Stopped\n");
					BtHfMsbcKill();
				}
				break;

			case MSG_APP_RES_RELEASE:
				//BtHfResRelease();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_RELEASE_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_MALLOC:
				//BtHfResMalloc(mainAppCt.SamplesPreFrame);
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_MALLOC_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}
				break;
			case MSG_APP_RES_INIT:
				//BtHfResInit();
				gCtrlVars.audio_effect_init_flag = 1;
				BtHf_AECEffectInit();
				gCtrlVars.audio_effect_init_flag = 0;
				
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_APP_RES_INIT_SUC;
					MessageSend(GetMainMessageHandle(), &msgSend);
				}

				BtHfCt->hfModeAudioResInitOk = 1;
				break;

			///////////////////////////////////////////////////////
			//HFP control
			case MSG_BT_HF_PLAY_PAUSE:
				switch(GetHfpState())
				{
					case BT_HFP_STATE_INCOMING:
						DBG("call answer\n");
						HfpAnswerCall();
						break;

					case BT_HFP_STATE_OUTGOING:
					case BT_HFP_STATE_ACTIVE:
						{
							bool flag = 0;
							GetBtHfpVoiceRecognition(&flag);
							if(flag)
							{
								DBG("close voice recognition\n");
								HfpVoiceRecognition(0);
							}
							else
							{
								DBG("call hangup\n");
								HfpHangup();
							}
						}
						break;

					default:
						break;
				}
				break;
			
			case MSG_BT_HF_CALL_REJECT:
				switch(GetHfpState())
				{
					case BT_HFP_STATE_INCOMING:
					case BT_HFP_STATE_OUTGOING:
					case BT_HFP_STATE_ACTIVE:
						DBG("call reject\n");
						HfpHangup();

						ExitBtHfMode();
						break;

					default:
						break;
				}
				break;
			
			case MSG_BT_HF_TRANS_CHANGED:
				DBG("Hfp Audio Transfer.\n");
				HfpAudioTransfer();
				break;
				
			case MSG_REMIND_SOUND_PLAY_START:
				break;
			
			case MSG_REMIND_SOUND_PLAY_DONE://提示音播放结束
				//AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
				break;
			
			default:
				CommonMsgProccess(msgRecv.msgId);
				break;

		}
		BtHf_MicProcess();

		BtHfModeRunning();
		
#ifdef BT_HFP_CALL_DURATION_DISP
		if(BtHfCt->BtHfTimeUpdate)
		{
			BtHfCt->BtHfTimeUpdate = 0;
			BT_DBG("通话中：%02d:%02d:%02d", BtHfCt->BtHfActiveTime / 3600, (BtHfCt->BtHfActiveTime % 3600) / 60, BtHfCt->BtHfActiveTime % 60);
		}
#endif
	}
}


/*****************************************************************************************
 * 创建HF任务
 ****************************************************************************************/
bool BtHfCreate(MessageHandle parentMsgHandle)
{
	bool ret = 0;

	ret = BtHfInit(parentMsgHandle);
	if(ret)
	{
		BtHfCt->taskHandle = NULL;
		xTaskCreate(BtHfEntrance, "BtHfMode", BT_HF_TASK_STACK_SIZE, NULL, BT_HF_TASK_PRIO, &BtHfCt->taskHandle);
		if(BtHfCt->taskHandle == NULL)
		{
			ret = FALSE;
		}
	}
	if(!ret)
		DBG("BtAudioPlay app create fail!\n");
	return ret;
}


/*****************************************************************************************
 * 删除HF任务
 ****************************************************************************************/
static void BtHfMsbcKill(void)
{
	BtHfCt->MsbcInitFlag = 0;
	BtAppiFunc_SaveScoData(NULL);
	DecoderServiceKill();
	BtHf_MsbcDecoderDeinit();
	BtHf_MsbcEncoderDeinit();
}

static void BtHfCvsdKill(void)
{
	BtHfCt->CvsdInitFlag = 0;
	//deinit cvsd_plc
	if(BtHfCt->cvsdPlcState)
	{
		osPortFree(BtHfCt->cvsdPlcState);
		BtHfCt->cvsdPlcState = NULL;
	}

	if(BtHfCt->ScoOutSrcOutBuf)
	{
		osPortFree(BtHfCt->ScoOutSrcOutBuf);
		BtHfCt->ScoOutSrcOutBuf = NULL;
	}
	if(BtHfCt->ScoOutResamplerCt)
	{
		osPortFree(BtHfCt->ScoOutResamplerCt);
		BtHfCt->ScoOutResamplerCt = NULL;
	}

	if(BtHfCt->ScoInSrcOutBuf)
	{
		osPortFree(BtHfCt->ScoInSrcOutBuf);
		BtHfCt->ScoInSrcOutBuf = NULL;
	}
	if(BtHfCt->ScoInResamplerCt)
	{
		osPortFree(BtHfCt->ScoInResamplerCt);
		BtHfCt->ScoInResamplerCt = NULL;
	}
}

bool BtHfKill(void)
{
	//clear voice recognition flag
	SetBtHfpVoiceRecognition(0);
	
	BtAppiFunc_SaveScoData(NULL);
	AudioCoreSourceDisable(BT_HF_SOURCE_NUM);
	
	if(BtHfCt == NULL)
	{
		return FALSE;
	}
	
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjustDisable();
#endif

//注意此处，如果在TaskStateCreating发起stop，它尚未init，暂时不会出错
	BtHfDeinitialize();	
	AudioCoreProcessConfig(NULL);

	//AEC
	BtHf_AECDeinit();

	if(BtHfCt->BtScoSendFifo)
	{
		osPortFree(BtHfCt->BtScoSendFifo);
		BtHfCt->BtScoSendFifo = NULL;
	}

	//msbc
	//if(BtHfCt->codecType == HFP_AUDIO_DATA_mSBC)
	{
		//Kill used services
		DecoderServiceKill();
	}

	if(BtHfCt->Sink2ScoFifoMutex != NULL)
	{
		vSemaphoreDelete(BtHfCt->Sink2ScoFifoMutex);
		BtHfCt->Sink2ScoFifoMutex = NULL;
	}

	if(BtHfCt->Sink2PcmFifo)
	{
		osPortFree(BtHfCt->Sink2PcmFifo);
		BtHfCt->Sink2PcmFifo = NULL;
	}

	if(BtHfCt->Sink2ScoFifo)
	{
		osPortFree(BtHfCt->Sink2ScoFifo);
		BtHfCt->Sink2ScoFifo = NULL;
	}

	//msbc
	//if(BtHfCt->codecType == HFP_AUDIO_DATA_mSBC)
	{
		//msbc decoder/encoder deinit
		BtHf_MsbcDecoderDeinit();
		BtHf_MsbcEncoderDeinit();
	}
	//else
	//cvsd
	{
		//deinit cvsd_plc
		if(BtHfCt->cvsdPlcState)
		{
			osPortFree(BtHfCt->cvsdPlcState);
			BtHfCt->cvsdPlcState = NULL;
		}
	}

	if(BtHfCt->Source1Buf_ScoData)
	{
		osPortFree(BtHfCt->Source1Buf_ScoData);
		BtHfCt->Source1Buf_ScoData = NULL;
	}

	if(BtHfCt->ScoBuffer)
	{
		osPortFree(BtHfCt->ScoBuffer);
		BtHfCt->ScoBuffer = NULL;
	}

	//cvsd
	if(BtHfCt->ScoOutSrcOutBuf)
	{
		osPortFree(BtHfCt->ScoOutSrcOutBuf);
		BtHfCt->ScoOutSrcOutBuf = NULL;
	}
	if(BtHfCt->ScoOutResamplerCt)
	{
		osPortFree(BtHfCt->ScoOutResamplerCt);
		BtHfCt->ScoOutResamplerCt = NULL;
	}

	if(BtHfCt->ScoInSrcOutBuf)
	{
		osPortFree(BtHfCt->ScoInSrcOutBuf);
		BtHfCt->ScoInSrcOutBuf = NULL;
	}
	if(BtHfCt->ScoInResamplerCt)
	{
		osPortFree(BtHfCt->ScoInResamplerCt);
		BtHfCt->ScoInResamplerCt = NULL;
	}

	//release used buffer: input/output sco fifo
	if(BtHfCt->ScoOutputFifo)
	{
		osPortFree(BtHfCt->ScoOutputFifo);
		BtHfCt->ScoOutputFifo = NULL;
	}
	if(BtHfCt->ScoInputFifo)
	{
		osPortFree(BtHfCt->ScoInputFifo);
		BtHfCt->ScoInputFifo = NULL;
	}

	//ADC/DAC resume
	ResumeAudioCoreMicSource();
	
	BtHfCt->AudioCoreBtHf = NULL;
	
	//task  先删任务，再删邮箱，收资源
	if(BtHfCt->taskHandle)
	{
		vTaskDelete(BtHfCt->taskHandle);
		BtHfCt->taskHandle = NULL;
	}

	//Msgbox
	if(BtHfCt->msgHandle)
	{
		MessageDeregister(BtHfCt->msgHandle);
		BtHfCt->msgHandle = NULL;
	}
	osPortFree(BtHfCt);
	BtHfCt = NULL;
	DBG("!!BtHfCt\n");

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioEffectsDeInit();
#endif

	return TRUE;
}

/*****************************************************************************************
 ****************************************************************************************/
bool BtHfStart(void)
{
	MessageContext		msgSend;

	if(BtHfCt == NULL)
		return FALSE;

	printf("BtHfStart\n");
	msgSend.msgId		= MSG_TASK_START;
	return MessageSend(BtHfCt->msgHandle, &msgSend);
}

bool BtHfStop(void)
{
	MessageContext		msgSend;

	if(BtHfCt == NULL)
		return FALSE;

	printf("BtHfStop\n");
	
	AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
#if (CFG_RES_MIC_SELECT) && defined(CFG_FUNC_AUDIO_EFFECT_EN)
	AudioCoreSourceMute(MIC_SOURCE_NUM, TRUE, TRUE);
#endif				
	vTaskDelay(30);

	msgSend.msgId		= MSG_TASK_STOP;
	return MessageSend(BtHfCt->msgHandle, &msgSend);
}

/*****************************************************************************************
 ****************************************************************************************/
MessageHandle GetBtHfMessageHandle(void)
{
	if(BtHfCt == NULL)
	{
		return NULL;
	}
	return BtHfCt->msgHandle;
}

/*****************************************************************************************
 * 进入/退出HF模式
 ****************************************************************************************/
void EnterBtHfMode(void)
{
	if(GetSystemMode() != AppModeBtHfPlay)
	{
		ResourceRegister(AppResourceBtHf);
		BtHfMsgToParent(MSG_DEVICE_SERVICE_BTHF_IN);
	}
}

void ExitBtHfMode(void)
{
	if(mainAppCt.appCurrentMode == AppModeBtHfPlay)
	{
		ResourceDeregister(AppResourceBtHf);
		BtHfMsgToParent(MSG_DEVICE_SERVICE_BTHF_OUT);

		//在退出通话模式前,先关闭sink2通路,避免AudioCore在轮转时,使用到sink相关的内存,和Hf模式kill流程相冲突,导致野指针问题
		AudioCoreSinkDisable(AUDIO_HF_SCO_SINK_NUM);

		BtHfCt->ModeKillFlag=1;
	}
}

#else
/*****************************************************************************************
 * 
 ****************************************************************************************/
MessageHandle GetBtHfMessageHandle(void)
{
	return NULL;
}

uint32_t BtHf_MsbcDataLenGet(void)
{
	return 0;
}

bool BtHfCreate(MessageHandle parentMsgHandle)
{
	return FALSE;
}

bool BtHfKill(void)
{
	return FALSE;
}

bool BtHfStart(void)
{
	return FALSE;
}

bool BtHfStop(void)
{
	return FALSE;
}

uint8_t BtHfDecoderSourceNum(void)
{
	return 0;
}

void ExitBtHfMode(void)
{
}

#endif
#endif

