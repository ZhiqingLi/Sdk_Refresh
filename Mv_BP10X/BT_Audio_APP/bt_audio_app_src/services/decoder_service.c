/**
 **************************************************************************************
 * @file    decoder_service.c
 * @brief   
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2018-1-10 20:21:00$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <stdio.h>
#include <string.h>

#include "type.h"
#include "app_message.h"
#include "rtos_api.h"
#include "mvstdio.h"
#include "audio_decoder_api.h"
#include "audio_core_service.h"
#include "resampler.h"
#include "decoder_service.h"
#include "mcu_circular_buf.h"
#include "app_config.h"
#include "core_d1088.h"
#include "debug.h"
#include "main_task.h"

#ifdef CFG_APP_BT_MODE_EN
#include "bt_config.h"
#include "bt_play_api.h"
#if (BT_HFP_SUPPORT == ENABLE)
#include "bt_hf_mode.h"
#include "bt_hf_api.h"
#endif
#endif

static void DecoderProcess(void);
static int16_t SaveDecodedPcmData(int16_t * PcmData, uint16_t PcmDataLen);
void DecoderStopProcess(void);


#define DECODER_SERVICE_SIZE				640//1024
#define DECODER_SERVICE_PRIO				4
#define DECODER_SERVICE_TIMEOUT				1	/*unit ms */
#define DECODER_SERVICE_NUM_MESSAGE_QUEUE	10

#define SBC_DECODER_FIFO_MIN				(119*17)//238//2052//
#define MSBC_DECODER_FIFO_MIN				57*2//238


#define DECODER_FIFO_SIZE   				(1024 * 8)	//输出缓存，Unit:  BYTE
#define	SRC_OUT_BUF_LEN						(MAX_FRAME_SAMPLES * 2 * 6)

typedef struct _DecoderServiceContext
{
	xTaskHandle 		taskHandle;
	MessageHandle		msgHandle;
	MessageHandle		parentMsgHandle;
	TaskState			serviceState;

	/*for audio decode init*/
	int32_t				ioType;
	int32_t				decoderType;
	uint32_t			DecoderSize;

	/* for decoded buffer */
	MCU_CIRCULAR_CONTEXT DecoderCircularBuf;
	int16_t				*DecoderFifo;
	osMutexId			DecoderPcmFifoMutex;
	int8_t				*decoder_buf;
	uint8_t				Channel;//Decoderfifo内数据声道参数

	/* for save decoded PCM data, unit: Sample*/
	uint32_t			pcmDataSize;
	uint32_t			savedSize;
	uint16_t			*toSavePos;

	DecoderState		decoderState;

	uint32_t			StepTime;				//快进快退

	/*for play timer*/
	uint32_t			DecoderSamples;			//当前尚未计入(指示)时间的采样点
	uint32_t			DecoderCurPlayTime;		//当前播放的时间
	uint32_t			DecoderLastPlayTime;

#ifdef CFG_FUNC_MIXER_SRC_EN
	ResamplerContext*	ResamplerCt;			
	int16_t*			SRCOutBuf;
#endif
}DecoderServiceContext;

static 	DecoderServiceContext 		DecoderServiceCt;
static uint8_t	DecoderSourecNum;

/**
 * @func        DecoderService_Init
 * @brief       DecoderService初始化
 * @param       MessageHandle parentMsgHandle  
 * @param		BufSize	Decoder buffer Size:4K @wav? 19K @mp3,40K @flac
 * @Output      None
 * @return      bool
 * @Others      解码输出流buf，开启锁机制。
 * Record
 */
static bool DecoderService_Init(MessageHandle parentMsgHandle, uint32_t BufSize)
{
	//DecoderServiceCt = (DecoderServiceContext)osPortMalloc(sizeof(DecoderServiceContext));
	memset(&DecoderServiceCt, 0, sizeof(DecoderServiceContext));

	/* register message handle */
	if((DecoderServiceCt.msgHandle = MessageRegister(DECODER_SERVICE_NUM_MESSAGE_QUEUE)) == NULL)
	{
		return FALSE;
	}
	DecoderServiceCt.parentMsgHandle = parentMsgHandle;
	DecoderServiceCt.serviceState = TaskStateCreating;

	DecoderServiceCt.DecoderSize = BufSize;

#ifdef CFG_FUNC_MIXER_SRC_EN
	if((DecoderServiceCt.ResamplerCt = (ResamplerContext*)osPortMalloc(sizeof(ResamplerContext))) == NULL)
	{
		return FALSE;
	}
	if((DecoderServiceCt.SRCOutBuf = (int16_t*)osPortMalloc(SRC_OUT_BUF_LEN)) == NULL)
	{
		return FALSE;
	}

#endif

	if(!DecoderServiceCt.DecoderSize)
	{
		return FALSE;
	}

	if((DecoderServiceCt.decoder_buf = (int8_t*)osPortMalloc(DecoderServiceCt.DecoderSize)) == NULL)//DECODER_BUF_SIZE);
	{
		return FALSE;
	}

	if((DecoderServiceCt.DecoderPcmFifoMutex = xSemaphoreCreateMutex()) == NULL)
	{
		return FALSE;
	}

	if((DecoderServiceCt.DecoderFifo = (int16_t*)osPortMalloc(DECODER_FIFO_SIZE)) == NULL)
	{
		return FALSE;
	}	
	MCUCircular_Config(&DecoderServiceCt.DecoderCircularBuf, DecoderServiceCt.DecoderFifo, DECODER_FIFO_SIZE);

	return TRUE;
}

static void DecoderServiceEntrance(void * param)
{
	MessageContext		msgRecv;
	MessageContext		msgSend;

	DecoderServiceCt.serviceState = TaskStateReady;

	/* Send message to parent*/
	msgSend.msgId		= MSG_DECODER_SERVICE_CREATED;
	MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);

	while(1)
	{

		MessageRecv(DecoderServiceCt.msgHandle, &msgRecv, DECODER_SERVICE_TIMEOUT);
		switch(msgRecv.msgId)
		{
			case MSG_TASK_RESUME:
				DBG("Decoder:Resume\n");
			case MSG_TASK_START:
				if(DecoderServiceCt.serviceState == TaskStateReady)
				{
					msgSend.msgId		= MSG_DECODER_SERVICE_STARTED;
					MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);				
					DecoderServiceCt.serviceState = TaskStateRunning;
					SoftFlagRegister(SoftFlagDecoderRun);
				}
				break;

			case MSG_TASK_PAUSE:
				DecoderDeinit();
				DecoderServiceCt.decoderState = DecoderStateNone;
				DecoderServiceCt.serviceState = TaskStateReady;
				msgSend.msgId		= MSG_DECODER_SERVICE_PAUSED;
				MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);	
				break;

			case MSG_TASK_STOP:
				DBG("MSG_TASK_STOP\n");
				SoftFlagDeregister(SoftFlagDecoderRun);
				{
					//Set para
					DecoderDeinit();
					DecoderServiceCt.decoderState = DecoderStateNone;

					//clear msg
					MessageClear(DecoderServiceCt.msgHandle);
					
					//Set state
					DecoderServiceCt.serviceState = TaskStateStopped;

					//reply
					msgSend.msgId		= MSG_DECODER_SERVICE_STOPPED;
					MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);
				}
				break;
			case MSG_DECODER_RESET:
				msgSend.msgId		= MSG_DECODER_RESET;
				MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);
				DecoderServiceCt.decoderState = DecoderStateNone;
				osMutexLock(DecoderServiceCt.DecoderPcmFifoMutex);
				MCUCircular_Config(&DecoderServiceCt.DecoderCircularBuf, DecoderServiceCt.DecoderFifo, DECODER_FIFO_SIZE);
				osMutexUnlock(DecoderServiceCt.DecoderPcmFifoMutex);
				break;
			
			case MSG_DECODER_PAUSE:
				DecoderServiceCt.decoderState = DecoderStatePause;
				break;
			case MSG_DECODER_PLAY:
				if(DecoderServiceCt.decoderState == DecoderStateInitialized)
				{
					//DBG("Decoder:Play @state:%d\n", DecoderServiceCt.serviceState);
					DecoderServiceCt.decoderState = DecoderStatePlay;
				}
				break;
			case MSG_DECODER_RESUME:
				if(DecoderServiceCt.decoderState != DecoderStateNone)
				{
					DecoderServiceCt.decoderState = DecoderStatePlay;
				}
				break;

			case MSG_DECODER_STOP:
				DecoderServiceCt.decoderState = DecoderStateStop;
				break;

			case MSG_DECODER_FF:
				{
					uint32_t Timer = NULL;
					Timer = DecoderServiceCt.DecoderCurPlayTime * 1000 + DecoderServiceCt.StepTime;
					DBG("PlayTime = %d\n", (int)Timer);
					if(Timer > audio_decoder->song_info->duration)
					{
						DecoderServiceCt.decoderState = DecoderStatePause;
						DBG("SONG FF END; Play next song.\n");
						msgSend.msgId		= MSG_MEDIA_PLAY_NEXT;
						MessageSend(GetAppMessageHandle(), &msgSend);

						break;
					}
					audio_decoder_seek(Timer);
					DecoderServiceCt.DecoderSamples = 0;
					DecoderServiceCt.DecoderCurPlayTime += DecoderServiceCt.StepTime / 1000;
				}
				break;

			case MSG_DECODER_FB:
				{
					uint32_t Timer = 0;

					if(DecoderServiceCt.DecoderCurPlayTime * 1000 == 0)
					{
						DecoderServiceCt.decoderState = DecoderStatePause;
						DBG("SONG FB END; Play pre song.\n");
						msgSend.msgId		= MSG_MEDIA_PLAY_PRE;
						MessageSend(GetAppMessageHandle(), &msgSend);
						break;
					}
					else if(DecoderServiceCt.DecoderCurPlayTime * 1000 < DecoderServiceCt.StepTime)
					{
						DecoderServiceCt.DecoderCurPlayTime = 0;
					}
					else
					{
						Timer = DecoderServiceCt.DecoderCurPlayTime * 1000 - DecoderServiceCt.StepTime;
					}
					audio_decoder_seek(Timer);
					DecoderServiceCt.DecoderSamples = 0;
					DecoderServiceCt.DecoderCurPlayTime -= DecoderServiceCt.StepTime / 1000;
				}
				break;

			default:
				break;
		}
		if(DecoderServiceCt.serviceState == TaskStateRunning)
		{
			DecoderProcess();

			{
				MessageContext		msgSend;
				msgSend.msgId		= MSG_NONE;
				MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);
			}
		}
	}
}


//extern MemHandle SBC_MemHandle;
static void DecoderProcess(void)
{
	int32_t DecoderErrCode;
	static int32_t DecoderCnt = 0;
	uint16_t SampleForRetry = CFG_PARA_MAX_SAMPLES_PER_FRAME;//单次解码的目标解码采样点数量。

	do{
		switch(DecoderServiceCt.decoderState)
		{
			case DecoderStateInitialized:
				SampleForRetry = 0;
				break;

			case DecoderStatePlay:
				SampleForRetry = 0;
				{
					DecoderServiceCt.decoderState	= DecoderStateDecoding;
					MessageContext		msgSend;
					msgSend.msgId		= MSG_DECODER_PLAYING;
					MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);
					osMutexLock(DecoderServiceCt.DecoderPcmFifoMutex);
					MCUCircular_Config(&DecoderServiceCt.DecoderCircularBuf, DecoderServiceCt.DecoderFifo, DECODER_FIFO_SIZE);
					AudioCoreSourcePcmFormatConfig(DecoderSourecNum, (int)audio_decoder->song_info->num_channels);//配置audiocore
					DecoderServiceCt.Channel = audio_decoder->song_info->num_channels;//配置本地fifo
					AudioCoreSourceUnmute(DecoderSourecNum, TRUE, TRUE);//编码歌曲大多有淡入，sdk淡入时间短且早，效果不显。
					osMutexUnlock(DecoderServiceCt.DecoderPcmFifoMutex);
				}
				break;

			case DecoderStateDecoding:
				if(RT_SUCCESS != audio_decoder_can_continue())
				{
					DecoderServiceCt.decoderState = DecoderStatePause;//DecoderStateStop;//数据不够，
					if(DecoderServiceCt.serviceState == TaskStateRunning)
					{
						MessageContext		msgSend;
						//DecoderServiceCt.serviceState = TaskStateStopped;
						msgSend.msgId		= MSG_DECODER_SERVICE_SONG_PLAY_END;
						MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);
					}
					return;
				}

	#ifdef CFG_APP_BT_MODE_EN
				//蓝牙模式下SBC解码,需要判断decoder_mem水位,避免数据太少引起解码错误。
				if(mainAppCt.appCurrentMode == AppModeBtAudioPlay)
				{
					if(!SoftFlagGet(SoftFlagDecoderRemind))
					{
						if(GetValidSbcDataSize() <= SBC_DECODER_FIFO_MIN)
						{
							return;
						}
					}
				}
				
				if(mainAppCt.appCurrentMode == AppModeBtHfPlay)
				{
					if(!SoftFlagGet(SoftFlagDecoderRemind))
					{
						if(BtHf_MsbcDataLenGet() <= MSBC_DECODER_FIFO_MIN)
						{
							return;
						}
					}
				}
	#endif

				if(audio_decoder_decode() != RT_SUCCESS)
				{
					DecoderErrCode = audio_decoder_get_error_code();
					DBG("Deocder error %d\n", (int)DecoderErrCode);
					if(-127 == DecoderErrCode)
					{
						MessageContext		msgSend;

						DecoderCnt++;
						//异常,连续500次读不到数据
						if(DecoderCnt > 500)
						{
							DecoderCnt = 0;
							DecoderServiceCt.decoderState = DecoderStatePause;
							msgSend.msgId		= MSG_DECODER_SERVICE_SONG_PLAY_END;
							DBG("END SONG FOR DECODER TIMEOUT\n");
						}
						else
						{
							msgSend.msgId		= MSG_DECODER_SERVICE_FIFO_EMPTY;
						}
						MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);
					}
					else
					{
						if(mainAppCt.appCurrentMode == AppModeBtAudioPlay)
						{
							//出现错误时，重新初始化decoder
							BtSbcDecoderRefresh();
						}
					}
					return;
				}
				else if(DecoderCnt != 0)
				{
					DBG("FIFO_EMPTY Cnt = %d\n",(int)DecoderCnt);
				}
				DecoderCnt = 0;
				//计算播放时间
				{
					uint32_t Temp;
					DecoderServiceCt.DecoderSamples += audio_decoder->song_info->pcm_data_length;
					Temp = DecoderServiceCt.DecoderSamples / audio_decoder->song_info->sampling_rate;
					DecoderServiceCt.DecoderSamples -= Temp * audio_decoder->song_info->sampling_rate;
					DecoderServiceCt.DecoderCurPlayTime += Temp;
					if((DecoderServiceCt.DecoderCurPlayTime ) > (DecoderServiceCt.DecoderLastPlayTime))
					{
						MessageContext		msgSend;

						msgSend.msgId		= MSG_DECODER_SERVICE_UPDATA_PLAY_TIME;
						MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);

						//DecoderServiceCt.DecoderLastPlayTime = DecoderServiceCt.DecoderCurPlayTime;
					}
					DecoderServiceCt.DecoderLastPlayTime = DecoderServiceCt.DecoderCurPlayTime;
				}
				DecoderServiceCt.decoderState = DecoderStateToSavePcmData;
			case DecoderStateToSavePcmData:
				{
					DecoderServiceCt.pcmDataSize	= audio_decoder->song_info->pcm_data_length * audio_decoder->song_info->num_channels;
					DecoderServiceCt.savedSize		= 0;
					DecoderServiceCt.toSavePos		= (uint16_t *)audio_decoder->song_info->pcm_addr;
					DecoderServiceCt.decoderState = DecoderStateSavePcmData;
					if(DecoderServiceCt.pcmDataSize == 0)//add by sam, 20180814，增加一个容错处理，有客户歌曲，解码出来，length为0
					{
						DecoderServiceCt.decoderState = DecoderStateDecoding;
						SampleForRetry -= 1;//避免死机,反复解出0个数据时退出。
						break;
					}
				}

			case DecoderStateSavePcmData:
				{
					int32_t		savedSize;  //采样点值

					savedSize = SaveDecodedPcmData((int16_t*)DecoderServiceCt.toSavePos,
								(DecoderServiceCt.pcmDataSize - DecoderServiceCt.savedSize) / audio_decoder->song_info->num_channels);

					if(savedSize > 0)
					{
						DecoderServiceCt.savedSize += savedSize * audio_decoder->song_info->num_channels;
						DecoderServiceCt.toSavePos += savedSize * audio_decoder->song_info->num_channels;
						if(DecoderServiceCt.savedSize == DecoderServiceCt.pcmDataSize) //上次解码输出已全部 放入fifo
						{
							DecoderServiceCt.decoderState = DecoderStateDecoding;
							if(SampleForRetry > savedSize)
							{
								SampleForRetry -= savedSize;
								break;//避免单次解码数据太少，引起OS切换后断音，流和buf可用时，再次解码。
							}
						}
					}
					SampleForRetry = 0;
				}
				break;

			case DecoderStateDeinitializing:
				SampleForRetry = 0;
				DecoderServiceCt.decoderState = DecoderStateNone;
				break;

			case DecoderStateStop:
				DecoderCnt = 0;
				SampleForRetry = 0;
				DecoderStopProcess();
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_DECODER_STOPPED;
					MessageSend(DecoderServiceCt.parentMsgHandle, &msgSend);
				}
				break;

			case DecoderStatePause:
				SampleForRetry = 0;
				//DBG("Decoder play Pause\n");
				//DecoderServiceCt.decoderState = DecoderStateNone;
				break;

			default:
				SampleForRetry = 0;
				break;
		}
	}while(SampleForRetry);
}


static int16_t SaveDecodedPcmData(int16_t * PcmData, uint16_t PcmDataLen)
{
	int32_t		WriteDataLen;//SRC前有效数据

#ifdef CFG_FUNC_MIXER_SRC_EN
	int32_t SRCDoneLen; 			//SRC之后存放于outbuf的数据长度
	int32_t MaxLen = MAX_FRAME_SAMPLES;
#endif

	if(PcmData == NULL || PcmDataLen == 0)
	{
		return -1;
	}
	osMutexLock(DecoderServiceCt.DecoderPcmFifoMutex);
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(SoftFlagGet(SoftFlagDecoderRemind))
	{
		WriteDataLen = MCUCircular_GetSpaceLen(&DecoderServiceCt.DecoderCircularBuf)/(audio_decoder->song_info->num_channels * 2)
				- ((DECODER_FIFO_SIZE)/(audio_decoder->song_info->num_channels * 2) - 2 * mainAppCt.SamplesPreFrame);//提示音时压缩解码输出buf至两帧
	}
	else
#endif
	{
		WriteDataLen = MCUCircular_GetSpaceLen(&DecoderServiceCt.DecoderCircularBuf)/(audio_decoder->song_info->num_channels * 2) - 4;
	}
#ifdef CFG_FUNC_MIXER_SRC_EN
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT == ENABLE))
	if(GetSystemMode() == AppModeBtHfPlay)
	{
		//蓝牙通话模式下，ADC/DAC都为16K，并msbc采样率为16K，故不需要转采样
		if(WriteDataLen > PcmDataLen)
		{
			WriteDataLen = PcmDataLen;
		}
		MCUCircular_PutData(&DecoderServiceCt.DecoderCircularBuf, PcmData, WriteDataLen * audio_decoder->song_info->num_channels * 2);
	}
	else
	{
#endif
		//if(writeDataLen < MAX_FRAME_SAMPLES * MAX_FRAME_SAMPLES / audio_decoder->song_info->num_channels)
		if(WriteDataLen < MAX_FRAME_SAMPLES)
		{
			osMutexUnlock(DecoderServiceCt.DecoderPcmFifoMutex);
			return 0;
		}
		if(audio_decoder->song_info->sampling_rate != CFG_PARA_SAMPLE_RATE)//不为44100
		{
			MaxLen = audio_decoder->song_info->sampling_rate / 360;//小于128sample
		}
		if(WriteDataLen > PcmDataLen)
		{
			WriteDataLen = PcmDataLen;
		}
		if(WriteDataLen > MaxLen)
		{
			WriteDataLen = MaxLen;
		}
		if(WriteDataLen > MAX_FRAME_SAMPLES)
		{
			WriteDataLen = MAX_FRAME_SAMPLES;
		}

		if(audio_decoder->song_info->sampling_rate != CFG_PARA_SAMPLE_RATE) //不为44100
		{
			SRCDoneLen = resampler_apply(DecoderServiceCt.ResamplerCt, PcmData, DecoderServiceCt.SRCOutBuf, WriteDataLen);
			if(SRCDoneLen > 0)
			{
				MCUCircular_PutData(&DecoderServiceCt.DecoderCircularBuf, DecoderServiceCt.SRCOutBuf, SRCDoneLen * audio_decoder->song_info->num_channels * 2);
			}
		}
		else
		{
			MCUCircular_PutData(&DecoderServiceCt.DecoderCircularBuf, PcmData, WriteDataLen * audio_decoder->song_info->num_channels * 2);
		}
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT == ENABLE))
	}
#endif
#else
	if(WriteDataLen > PcmDataLen)
	{
		WriteDataLen = PcmDataLen;
	}
	MCUCircular_PutData(&DecoderServiceCt.DecoderCircularBuf, PcmData, WriteDataLen * audio_decoder->song_info->num_channels * 2);
#endif
	osMutexUnlock(DecoderServiceCt.DecoderPcmFifoMutex);
	if(WriteDataLen <= 0)
		return -1;

	return WriteDataLen;
}

void DecoderStopProcess(void)
{
	DBG("Decoder:play stop\n");
	DecoderServiceCt.DecoderLastPlayTime = 0;
	DecoderServiceCt.DecoderCurPlayTime = 0;
	DecoderServiceCt.DecoderSamples = 0;
	DecoderServiceCt.decoderState = DecoderStateNone;
}


/***************************************************************************************
 *
 * APIs
 *
 */
bool DecoderServiceCreate(MessageHandle parentMsgHandle, uint32_t BufSize)
{
	bool		ret = TRUE;

	ret = DecoderService_Init(parentMsgHandle, BufSize);
	if(ret)
	{
		DecoderServiceCt.taskHandle = NULL;
		xTaskCreate(DecoderServiceEntrance,"DecoderService", DECODER_SERVICE_SIZE, NULL, DECODER_SERVICE_PRIO, &DecoderServiceCt.taskHandle);
		if(DecoderServiceCt.taskHandle == NULL)
		{
			ret = FALSE;
		}
	}
	if(!ret)
		DBG("DecoderService create fail!\n");
	return ret;
}

void DecoderServiceStart(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_TASK_START;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

void DecoderServicePause(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_TASK_PAUSE;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

void DecoderServiceResume(void)
{
	MessageContext		msgSend;
	msgSend.msgId		= MSG_TASK_RESUME;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

void DecoderServiceStop(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_TASK_STOP;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

void DecoderServiceKill(void)
{
	//AudioCoreSourceUnmute(DecoderSourecNum, TRUE, TRUE);//解码器通道恢复常态。注意:kill时可能是提示音通道而非app音源通道。
	SoftFlagDeregister(SoftFlagDecoderRun);//存在强行杀死 状态恢复。
	//PortFree... 
	if(DecoderServiceCt.DecoderPcmFifoMutex != NULL)
	{
		osMutexLock(DecoderServiceCt.DecoderPcmFifoMutex);
	}
	
	if(DecoderServiceCt.DecoderFifo != NULL)
	{
		osPortFree(DecoderServiceCt.DecoderFifo);
		DecoderServiceCt.DecoderFifo = NULL;
	}

	if(DecoderServiceCt.decoder_buf != NULL)
	{
		osPortFree(DecoderServiceCt.decoder_buf);
		DecoderServiceCt.decoder_buf = NULL;
	}
	if(DecoderServiceCt.DecoderPcmFifoMutex != NULL)
	{
		osMutexUnlock(DecoderServiceCt.DecoderPcmFifoMutex);
	}

	//task 先删任务，再删邮箱，收资源
	if(DecoderServiceCt.taskHandle != NULL)
	{
		vTaskDelete(DecoderServiceCt.taskHandle);
		DecoderServiceCt.taskHandle = NULL;
	}

	//Msgbox
	if(DecoderServiceCt.msgHandle != NULL)
	{
		MessageDeregister(DecoderServiceCt.msgHandle);
		DecoderServiceCt.msgHandle = NULL;
	}

	if(DecoderServiceCt.DecoderPcmFifoMutex != NULL)
	{
		vSemaphoreDelete(DecoderServiceCt.DecoderPcmFifoMutex);
		DecoderServiceCt.DecoderPcmFifoMutex = NULL;
	}

#ifdef CFG_FUNC_MIXER_SRC_EN
	if(DecoderServiceCt.SRCOutBuf != NULL)
	{
		osPortFree(DecoderServiceCt.SRCOutBuf);
		DecoderServiceCt.SRCOutBuf = NULL;
	}

	if(DecoderServiceCt.ResamplerCt != NULL)
	{
		osPortFree(DecoderServiceCt.ResamplerCt);
		DecoderServiceCt.ResamplerCt = NULL;
	}
#endif	
}

//仅用于驱动解码器
void DecoderServiceMsg(void)
{
	MessageContext		msgSend;
	msgSend.msgId		= MSG_NONE;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

MessageHandle GetDecoderServiceMsgHandle(void)
{
	return DecoderServiceCt.msgHandle;
}

TaskState GetDecoderServiceState(void)
{
	return DecoderServiceCt.serviceState;
}

int32_t DecoderInit(void *io_handle, int32_t ioType, int32_t decoderType)
{
		int32_t 	ret;//= DECODER_NORMAL;
		if(DecoderServiceCt.decoderState != DecoderStateNone && DecoderServiceCt.decoderState != DecoderStateInitialized)
		{
			DBG("Decoder State Error :%d \n", DecoderServiceCt.decoderState);
			return RT_FAILURE;
		}
		
		ret = audio_decoder_initialize((uint8_t*)DecoderServiceCt.decoder_buf, io_handle, ioType, decoderType);
		if(audio_decoder_get_context_size() >= DecoderServiceCt.DecoderSize)
		{
			DBG("Decodersize set error!!!!!!!");
			return RT_FAILURE;//安全监测
		}
		if(ret == RT_SUCCESS)
		{
			DecoderServiceCt.decoderState = DecoderStateInitialized;
			//DBG("[SONG_INFO]: ChannelCnt : %6d\n",		  (int)audio_decoder->song_info->num_channels);
			DBG("[SONG_INFO]: SampleRate : %6d Hz\n",	  (int)audio_decoder->song_info->sampling_rate);
			//DBG("[SONG_INFO]: BitRate	 : %6d Kbps\n",   (int)audio_decoder->song_info->bitrate / 1000);
			//DBG("[SONG_INFO]: DecoderSize: %6d Bytes \n", (int)audio_decoder->decoder_size);
			if(audio_decoder->song_info->stream_type != STREAM_TYPE_FLAC && audio_decoder->song_info->sampling_rate > 48000)
			{
				return RT_FAILURE;//读取数据速度受限，屏蔽部分高码率歌曲。
			}
			
#ifdef CFG_FUNC_MIXER_SRC_EN
			//if(audio_decoder->song_info->sampling_rate != CFG_PARA_SAMPLE_RATE)
			{
				resampler_init(DecoderServiceCt.ResamplerCt, audio_decoder->song_info->num_channels, audio_decoder->song_info->sampling_rate, CFG_PARA_SAMPLE_RATE, 0, 0);
			}
#endif
		}
		else
		{
			int32_t 	errCode;
			errCode = audio_decoder_get_error_code();
			DBG("AudioDecoder init err code = %d\n", (int)errCode);
		}
		return ret;
}

void DecoderDeinit(void)
{
	DecoderServiceCt.decoderState = DecoderStateDeinitializing;
}


/**
 * @func        DecodedPcmDataGet
 * @brief       Decoder数据流输出的 拉流API
 * @param       void * pcmData
                uint16_t sampleLen  
 * @Output      None
 * @return      uint16_t 取出的采样点数
 * @Others      数据区需要锁定，指定采样点数取出数据，不足时，按实际长度取数据。
 * Record
 */
uint16_t DecodedPcmDataGet(void * pcmData, uint16_t sampleLen)
{
	int16_t		dataSize;
	uint32_t	getSize;
	uint32_t 	SpacLen = 0;
	uint32_t 	ValLen = 0;

	if(DecoderServiceCt.DecoderPcmFifoMutex == NULL)
	{
		return 0;
	}

	if(DecoderServiceCt.serviceState != TaskStateRunning)
	{
		return 0;
	}

	getSize = sampleLen * 2 * DecoderServiceCt.Channel;//采样点数 * 2byte深度* 通道数
	if(getSize == 0)
	{
		return 0;
	}

	osMutexLock(DecoderServiceCt.DecoderPcmFifoMutex);
	SpacLen = MCUCircular_GetSpaceLen(&DecoderServiceCt.DecoderCircularBuf);

	ValLen = DECODER_FIFO_SIZE - SpacLen;

	if(getSize > ValLen)
	{
		getSize = ValLen;
	}

	dataSize = MCUCircular_GetData(&DecoderServiceCt.DecoderCircularBuf, pcmData, getSize);
	osMutexUnlock(DecoderServiceCt.DecoderPcmFifoMutex);

	return dataSize / (2 * DecoderServiceCt.Channel);
}

/**
 * @func        DecodedPcmDataLenGet
 * @brief       Decoder数据输出FIFO内采样点数
 * @Output      None
 * @return      uint16_t FIFO中采样点数
 * Record
 */
uint16_t DecodedPcmDataLenGet(void)
{
	if(DecoderServiceCt.DecoderPcmFifoMutex == NULL || DecoderServiceCt.serviceState != TaskStateRunning)
	{
		return 0;
	}
	return MCUCircular_GetDataLen(&DecoderServiceCt.DecoderCircularBuf) / (2 * DecoderServiceCt.Channel);
}

SongInfo * GetSongInfo(void)
{
	return audio_decoder->song_info;
}

//解码器工作
void DecoderPlay(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_DECODER_PLAY;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

//解码器复位
void DecoderReset(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_DECODER_RESET;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

//解码器停止
void DecoderStop(void)
{
	MessageContext		msgSend;

	AudioCoreSourceMute(DecoderSourecNum, TRUE, TRUE);
	vTaskDelay(20);
	msgSend.msgId		= MSG_DECODER_STOP;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

void DecoderPause(void)
{
	MessageContext		msgSend;

	if(DecoderServiceCt.decoderState <= DecoderStatePause)
	{
		return;
	}

	msgSend.msgId		= MSG_DECODER_PAUSE;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

void DecoderResume(void)
{
	MessageContext		msgSend;

	DBG("Decoder Resume()\n");

	msgSend.msgId		= MSG_DECODER_RESUME;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

//注意，StepTime 单位为毫秒，实际传递值为整数秒
void DecoderFF(uint32_t StepTime)
{
	MessageContext		msgSend;

	DBG("Decoder FF()\n");
	DBG("Decoder state = %d\n", DecoderServiceCt.decoderState);
	if(DecoderServiceCt.decoderState <= DecoderStatePause)//Playing
	{
		return;
	}

	DecoderServiceCt.StepTime = StepTime;

	msgSend.msgId		= MSG_DECODER_FF;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

//注意，StepTime 单位为毫秒，实际传递值为整数秒
void DecoderFB(uint32_t StepTime)
{
	MessageContext		msgSend;

	DBG("Decoder FB()\n");

	if(DecoderServiceCt.decoderState <= DecoderStatePause)//Playing
	{
		return;
	}

	DecoderServiceCt.StepTime = StepTime;

	msgSend.msgId		= MSG_DECODER_FB;
	MessageSend(DecoderServiceCt.msgHandle, &msgSend);
}

//解码器seek 文件播放时间 秒，因不同格式情况，精度不定。
bool DecoderSeek(uint32_t Time)
{
	DBG("DecoderTaskSeek()\n");
	audio_decoder_seek(Time * 1000);
	DecoderServiceCt.DecoderSamples = 0;
	DecoderServiceCt.DecoderCurPlayTime = 0;
	if(audio_decoder_seek(Time * 1000) != RT_SUCCESS)
	{
		return FALSE;
	}
	DecoderServiceCt.DecoderCurPlayTime = Time;
	return TRUE;
}

uint32_t DecoderServicePlayTimeGet(void)
{
 	return DecoderServiceCt.DecoderCurPlayTime;
}

uint32_t GetDecoderState(void)
{
	return DecoderServiceCt.decoderState;
}

//此变量作为全局获取，不要为空。
uint8_t DecoderSourceNumGet(void)
{
	return DecoderSourecNum;
}

void DecoderSourceNumSet(uint8_t Num)
{
	DecoderSourecNum = Num;
}
