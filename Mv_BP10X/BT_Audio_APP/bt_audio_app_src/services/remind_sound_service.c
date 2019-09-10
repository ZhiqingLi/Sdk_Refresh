/**
 **************************************************************************************
 * @file    remind_sound_service.c
 * @brief   
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-2-27 13:06:47$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <string.h>
#include "app_config.h"
#include "rtos_api.h"
#include "app_message.h"
#include "type.h"
#include "spi_flash.h"
#include "debug.h"
#include "audio_utility.h"
#include "remind_sound_service.h"
#include "audio_core_api.h"
#include "decoder_service.h"
#include "device_service.h"
#include "audio_core_service.h"

#include "main_task.h"
#include "dac_interface.h"
#include "timeout.h"

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
extern osMutexId AudioEffectChangeDoneMutex;
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN

#pragma pack(1)
typedef struct _SongClipsHdr
{
	char sync[4];
	uint32_t crc;
	uint8_t cnt;
} SongClipsHdr;
#pragma pack()

#pragma pack(1)
typedef struct _SongClipsEntry
{
	uint8_t id[8];
	uint32_t offset;
	uint32_t size;
} SongClipsEntry;
#pragma pack()

typedef enum _REMIND_SOUND_STATE
{
	REMIND_STANDBY,
	REMIND_WAIT_DECODER,
	REMIND_PLAY,
	REMIND_WAIT_RENEW,
	REMIND_STOPPING,
	REMIND_GIVE_DECODER,
} REMINDSOUNDSTATE;

#define		REMIND_SOUND_ID_LEN			sizeof(((SongClipsEntry *)0)->id)


/***************************************************************************************
 *
 * External defines
 *
 */

#define REMIND_SOUND_SERVICE_TASK_STACK_SIZE		512//1024
#define REMIND_SOUND_SERVICE_TASK_PRIO				3



/***************************************************************************************
 *
 * Internal defines
 *
 */

#define REMIND_SOUND_SERVICE_AUDIO_DECODER_IN_BUF_SIZE	1024 * 19

typedef struct _RemindSoundServiceContext
{
	xTaskHandle 		taskHandle;
	MessageHandle		msgHandle;
	MessageHandle		parentMsgHandle;
	TaskState			RemindSoundServiceState;

	MemHandle			RemindMemHandle;
	
	uint32_t 			ConstDataAddr;
	uint32_t			ConstDataSize;
	uint32_t 			ConstDataOffset;
	
	uint8_t				RequestRemind[REMIND_SOUND_ID_LEN];
	REMINDSOUNDSTATE	RemindState;
}RemindSoundServiceContext;

#define REMIND_FLASH_MAX_NUM		255 //flash提示音区配置决定
#define REMIND_FLASH_HDR_SIZE		0x1000 //提示音条目信息区大小

#define REMIND_FLASH_READ_TIMEOUT 	100
#define REMIND_FLASH_ADDR(n) 		(REMIND_FLASH_STORE_BASE + sizeof(SongClipsEntry) * n + sizeof(SongClipsHdr))//flash提示音区配置决定

#define REMIND_FLASH_FLAG_STR		("MVUB")

static const unsigned short CrcCCITTTable[256] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};



/***************************************************************************************
 *
 * Internal varibles
 *
 */

#define REMIND_SOUND_SERVICE_NUM_MESSAGE_QUEUE		15

static RemindSoundServiceContext		RemindSoundServiceCt;

/** remind sound task name*/
const char RemindSoundServiceName[] = "RemindSoundService";

/***************************************************************************************
 *
 * Internal functions
 *
 */

/////////////////////////////////////////////////////////////////////////
uint32_t RemindSoundServiceFillStreamCallback(void *buffer, uint32_t length);



static int32_t RemindSoundServiceInit(MessageHandle parentMsgHandle)
{
	memset(&RemindSoundServiceCt, 0, sizeof(RemindSoundServiceCt));
//下列const data安全检查至少要开启项。
	if(!sound_clips_all_crc())
	{
		SoftFlagRegister(SoftFlagNoRemind);//task还是要启动，否则maintask无法继续。
	}

//或只检测头部标记
//	SongClipsHdr	ConstDataHead;
//	if(FLASH_NONE_ERR != SpiFlashRead(REMIND_FLASH_STORE_BASE, (uint8_t*)&ConstDataHead,sizeof(SongClipsHdr), REMIND_FLASH_READ_TIMEOUT))
//	{
//		return -1;
//	}
//	if(memcmp(ConstDataHead.sync, REMIND_FLASH_FLAG_STR, sizeof(ConstDataHead.sync)) || !ConstDataHead.cnt)
//	{
//		SoftFlagRegister(SoftFlagNoRemind); //提示音数据没烧录。
//	}

//	DBG("%d remind items @ %x in flash\n", ConstDataHead.cnt, REMIND_FLASH_STORE_BASE);

	/* message handle */
	RemindSoundServiceCt.msgHandle = MessageRegister(REMIND_SOUND_SERVICE_NUM_MESSAGE_QUEUE);

	/* Parent message handle */
	RemindSoundServiceCt.parentMsgHandle = parentMsgHandle;
	RemindSoundServiceCt.RemindSoundServiceState = TaskStateCreating;
	RemindSoundServiceCt.RemindMemHandle.addr = NULL;//启用Callback后 实际此结构体未被解码器使用。保留api参数。
	RemindSoundServiceCt.RemindMemHandle.mem_capacity = 0;
	RemindSoundServiceCt.RemindMemHandle.mem_len = 0;
	RemindSoundServiceCt.RemindMemHandle.p = 0;
	RemindSoundServiceCt.RemindState = REMIND_STANDBY;
	return 0;
}



//根据flash驱动设计，最大支持255条提示音。
static bool	RemindSoundServiceReadItemInfo(uint8_t *RemindItem)
{
	uint16_t j;
	SongClipsEntry SongClips;

	//查找对应的ConstDataId
	for(j = 0; j < SOUND_REMIND_TOTAL; j++)
	{
		if(FLASH_NONE_ERR != SpiFlashRead(REMIND_FLASH_ADDR(j), (uint8_t *)&SongClips, sizeof(SongClipsEntry), REMIND_FLASH_READ_TIMEOUT))
		{
			return FALSE;
		}
		if(memcmp(&SongClips.id,RemindItem, sizeof(SongClips.id)) == 0)//找到
		{
			RemindSoundServiceCt.ConstDataOffset = 0;
			RemindSoundServiceCt.ConstDataAddr = SongClips.offset + REMIND_FLASH_STORE_BASE; //工具制作提示音bin 使用相对地址
			RemindSoundServiceCt.ConstDataSize = SongClips.size;
			return TRUE;
		}
	}

	return FALSE;
}

static void RemindSoundServicePlayEnd(void)
{
	mv_mread_callback_unset();
	AudioCoreSourceDisable(REMIND_SOURCE_NUM);
	RemindSoundServiceCt.RequestRemind[0] = 0;//响应结束，Item名清零作为标记。
	if(!SoftFlagGet(SoftFlagDecoderApp))//app使用解码器时，复用flag都有app控制
	{
		SoftFlagDeregister(SoftFlagDecoderRemind);
	}
	//区别于播放stopped消息，使用reset解码器消息，作为app与提示音之间的临界交接解码器消息。
	//严禁重复调用，状态保护。确认解码器已获取后可用
	if(RemindSoundServiceCt.RemindState != REMIND_GIVE_DECODER)
	{
		RemindSoundServiceCt.RemindState = REMIND_GIVE_DECODER;
		DecoderReset();
	}
}


static bool RemindSoundServicePlayItem(void)
{
	RemindSoundServiceCt.RequestRemind[0] = 0;//Item名清零作为标记。
	if(!SoftFlagGet(SoftFlagDecoderRun))
	{
		DBG("Decoder Not Run\n");
		return FALSE;
	}
	RemindSoundServiceCt.RemindMemHandle.mem_len = 0;
	RemindSoundServiceCt.RemindMemHandle.p = 0;
	mv_mread_callback_set(RemindSoundServiceFillStreamCallback);

	if(DecoderInit(&RemindSoundServiceCt.RemindMemHandle, (uint32_t)IO_TYPE_MEMORY, (uint32_t)MP3_DECODER) != RT_SUCCESS)
	{
		DBG("Remind:Decoder Init Error\n");
		return FALSE;
	}
	RemindSoundServiceCt.RemindState = REMIND_PLAY;
	{
		MessageContext		msgSend;
		msgSend.msgId = MSG_REMIND_SOUND_PLAY_START;
		MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
	}
	DecoderSourceNumSet(REMIND_SOURCE_NUM);
	AudioCoreSourceEnable(REMIND_SOURCE_NUM);
	DecoderPlay();
	
#ifndef CFG_FUNC_MIXER_SRC_EN
	SongInfo* PlayingSongInfo = audio_decoder_get_song_info();
#ifdef CFG_RES_AUDIO_DACX_EN
	AudioDAC_SampleRateChange(ALL, PlayingSongInfo->sampling_rate);
#else
	AudioDAC_SampleRateChange(DAC0, PlayingSongInfo->sampling_rate);
#endif
	DBG("Remind:DAC Sample rate = %d\n", (int)AudioDAC_SampleRateGet(0));
#endif
	return TRUE;
}





static void RemindSoundServiceEntrance(void * param)
{
	MessageContext		msgRecv;
	MessageContext		msgSend;

	msgSend.msgId		= MSG_REMIND_SOUND_SERVICE_CREATED;
	MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);

	while(1)
	{
		MessageRecv(RemindSoundServiceCt.msgHandle, &msgRecv, MAX_RECV_MSG_TIMEOUT);
		switch(msgRecv.msgId)
		{
			case MSG_TASK_START:
				msgSend.msgId		= MSG_REMIND_SOUND_SERVICE_STARTED;
				MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
				RemindSoundServiceCt.RemindSoundServiceState = TaskStateRunning;
				break;
				
			case MSG_TASK_PAUSE:
				RemindSoundServiceCt.RemindSoundServiceState = TaskStatePaused;
				msgSend.msgId		= MSG_REMIND_SOUND_SERVICE_PAUSED;
				MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
				break;
				
			case MSG_TASK_RESUME:
				RemindSoundServiceCt.RemindSoundServiceState = TaskStateRunning;
				break;
				
			case MSG_TASK_STOP:
				msgSend.msgId		= MSG_REMIND_SOUND_SERVICE_STOPPED;
				MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
				break;

			case MSG_REMIND_SOUND_PLAY_REQUEST://响应 有效的提示音播放请求，
				if(!SoftFlagGet(SoftFlagDecoderRun))
				{
					msgSend.msgId = MSG_REMIND_SOUND_PLAY_REQUEST_FAIL;
					MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
					RemindSoundServiceCt.RemindState = REMIND_STANDBY;
				}
				else
				{
					if(RemindSoundServiceReadItemInfo(RemindSoundServiceCt.RequestRemind))//查询有效
					{
						switch(RemindSoundServiceCt.RemindState)
						{
							case REMIND_STANDBY: //空闲
							case REMIND_GIVE_DECODER: //解码器已交出，app待收或已收到
								if(!SoftFlagGet(SoftFlagDecoderApp))//app 不使用解码器
								{
									DBG("Remind:Play Direct\n");
									SoftFlagRegister(SoftFlagDecoderRemind);//标记提示音占有解码器，可不注销。
									RemindSoundServicePlayItem();//如果播放失败，不影响app
								}
								else //等待app 释放解码器
								{
									DBG("Remind:Wait Decoder\n");
									RemindSoundServiceCt.RemindState = REMIND_WAIT_DECODER;
									msgSend.msgId = MSG_REMIND_SOUND_NEED_DECODER;
									MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);//
								}
								break;
								
							case REMIND_WAIT_DECODER:
								break;//继续等待。
								
							case REMIND_PLAY://插播
								RemindSoundServiceCt.RemindState = REMIND_WAIT_RENEW;
								DecoderStop();
								break;
								
							case REMIND_WAIT_RENEW:
								break;
								
							case REMIND_STOPPING:
								RemindSoundServiceCt.RemindState = REMIND_WAIT_RENEW;
								break;
						}
					}
					else //此提示音无效
					{
						DBG("Remind:No item,break!\n");
						switch(RemindSoundServiceCt.RemindState)//功能：上一个提示音需要停止。
						{
							case REMIND_STANDBY: //继续闲置
							case REMIND_GIVE_DECODER: //解码器已交出，app待收或已收到
							case REMIND_STOPPING:	//继续停止进程
								break;
								
							case REMIND_WAIT_DECODER://收到解码器play通知后，需要立即返回。
							case REMIND_WAIT_RENEW:
								RemindSoundServiceCt.RemindState = REMIND_STOPPING;
								break;
								
							case REMIND_PLAY:
								RemindSoundServiceCt.RemindState = REMIND_STOPPING;
								DecoderStop();
								break;
						}
						msgSend.msgId = MSG_REMIND_SOUND_PLAY_REQUEST_FAIL;
						MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
					}
				}
				break;

			case MSG_REMIND_SOUND_PLAY: //提示音播放;此消息要求发送者确保解码器已停止。
				if(SoftFlagGet(SoftFlagDecoderRemind) && RemindSoundServiceCt.RemindState == REMIND_WAIT_DECODER)
				{
					DBG("Remind:Play\n");
					if(!RemindSoundServicePlayItem()) //播放失败需要处理。
					{
						RemindSoundServicePlayEnd();
						{
							msgSend.msgId = MSG_REMIND_SOUND_PLAY_DONE;
							MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
						}
					}
				}
				else if(SoftFlagGet(SoftFlagDecoderRemind) && RemindSoundServiceCt.RemindState == REMIND_STOPPING)
				{//要了解消息时差，在等待解码器过程中，提示音被停止了，需返回。
					DBG("Remind:return\n");
					RemindSoundServiceCt.RemindState = REMIND_GIVE_DECODER;
					DecoderReset();
				}
				else
				{
					DBG("Remind:System error\n");//不应该出现
					RemindSoundServiceCt.RemindState = REMIND_STANDBY;
				}
				break;

				//提示音service参数和功能复位，无消息返回，解码器状态由消息发起方保障。
			case MSG_REMIND_SOUND_PLAY_RESET:
				mv_mread_callback_unset();
				if(SoftFlagGet(SoftFlagDecoderRemind))
				{
					DecoderStop();
					SoftFlagDeregister(SoftFlagDecoderRemind);//要求app对解码器复位，否则缺mv_mread_callback_unset
				}	
				AudioCoreSourceDisable(REMIND_SOURCE_NUM);

				RemindSoundServiceCt.RemindState = REMIND_STANDBY;
				break;

			case MSG_DECODER_STOPPED:
				if(SoftFlagGet(SoftFlagDecoderRemind) && (RemindSoundServiceCt.RemindState == REMIND_STOPPING))
				{//处理提示音结束
					DBG("Remind Stopped\n");
					RemindSoundServicePlayEnd();
					{
						msgSend.msgId = MSG_REMIND_SOUND_PLAY_DONE;
						MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
					}
				}
				else if(SoftFlagGet(SoftFlagDecoderRemind) && (RemindSoundServiceCt.RemindState == REMIND_WAIT_RENEW))
				{	//再次做检查。
					if(RemindSoundServiceReadItemInfo(RemindSoundServiceCt.RequestRemind) && SoftFlagGet(SoftFlagDecoderRun))
					{
						if(RemindSoundServicePlayItem())//失败要处理
						{
							DBG("Remind:Play Item\n");//播放新提示音有效。
							break;
						}
					}
					else
					{
						msgSend.msgId = MSG_REMIND_SOUND_PLAY_REQUEST_FAIL;
						MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
					}
				}
				break;
		}
	}
}


/***************************************************************************************
 *
 * APIs
 *
 */
MessageHandle GetRemindSoundServiceMessageHandle(void)
{
	return RemindSoundServiceCt.msgHandle;
}


int32_t RemindSoundServiceCreate(MessageHandle parentMsgHandle)
{
	int32_t		ret = 0;
	ret = RemindSoundServiceInit(parentMsgHandle);
	if(!ret)
	{
		RemindSoundServiceCt.taskHandle = NULL;
		xTaskCreate(RemindSoundServiceEntrance, RemindSoundServiceName, REMIND_SOUND_SERVICE_TASK_STACK_SIZE, NULL, REMIND_SOUND_SERVICE_TASK_PRIO, &RemindSoundServiceCt.taskHandle);
		if(RemindSoundServiceCt.taskHandle == NULL)
		{
			ret = -1;
		}
	}
	if(ret)
	{
		SoftFlagRegister(SoftFlagNoRemind);
		DBG("Remind:%s create fail!\n", RemindSoundServiceName);
	}
	return ret;
}

void RemindSoundServiceStart(void)
{
	MessageContext		msgSend;
	msgSend.msgId		= MSG_TASK_START;
	MessageSend(RemindSoundServiceCt.msgHandle, &msgSend);
}

void RemindSoundServicePause(void)
{
	MessageContext		msgSend;
	msgSend.msgId		= MSG_TASK_PAUSE;
	MessageSend(RemindSoundServiceCt.msgHandle, &msgSend);
}

void RemindSoundServiceResume(void)
{
	MessageContext		msgSend;
	msgSend.msgId		= MSG_TASK_RESUME;
	MessageSend(RemindSoundServiceCt.msgHandle, &msgSend);
}

void RemindSoundServiceStop(void)
{
	MessageContext		msgSend;

	msgSend.msgId		= MSG_TASK_STOP;
	MessageSend(RemindSoundServiceCt.msgHandle, &msgSend);
}

void RemindSoundServiceKill(void)
{
	//task
	if(RemindSoundServiceCt.taskHandle != NULL)
	{
		vTaskDelete(RemindSoundServiceCt.taskHandle);
		RemindSoundServiceCt.taskHandle = NULL;
	}
	
	//Msgbox
	if(RemindSoundServiceCt.msgHandle != NULL)
	{
		MessageDeregister(RemindSoundServiceCt.msgHandle);
		RemindSoundServiceCt.msgHandle = NULL;
	}
	//PortFree...
}



//提示音请求，记录条目字符串。
bool RemindSoundServiceItemRequest(char *SoundItem)
{
	bool ret;
	MessageContext		msgSend;
	if(SoftFlagGet(SoftFlagNoRemind) || !SoftFlagGet(SoftFlagDecoderRun))
	{
		msgSend.msgId		= MSG_REMIND_SOUND_PLAY_REQUEST_FAIL;
		MessageSend(RemindSoundServiceCt.parentMsgHandle, &msgSend);
		ret = FALSE;
	}
	else
	{
		memcpy(RemindSoundServiceCt.RequestRemind, SoundItem, REMIND_SOUND_ID_LEN);
		#ifdef CFG_FUNC_AUDIO_EFFECT_EN
		//osMutexLock(AudioEffectChangeDoneMutex);
		#endif
		msgSend.msgId		= MSG_REMIND_SOUND_PLAY_REQUEST;
		MessageSend(RemindSoundServiceCt.msgHandle, &msgSend);
		ret = TRUE;
		#ifdef CFG_FUNC_AUDIO_EFFECT_EN
		//osMutexUnlock(AudioEffectChangeDoneMutex);
		#endif
	}
	return ret;
}

void RemindSoundServicePlay(void)
{
	MessageContext		msgSend;
	
	msgSend.msgId		= MSG_REMIND_SOUND_PLAY;
	MessageSend(RemindSoundServiceCt.msgHandle, &msgSend);
	return;
}

//提示音无条件复位。
void RemindSoundServiceReset(void)
{
	MessageContext		msgSend;
	
	msgSend.msgId		= MSG_REMIND_SOUND_PLAY_RESET;
	MessageSend(RemindSoundServiceCt.msgHandle, &msgSend);
	return ;
}

extern bool EffectProcessFlagGet(void);
uint32_t RemindSoundServiceFillStreamCallback(void *buffer, uint32_t length)
{
	int32_t RemainBytes;
	int32_t ReadBytes;
	TIMER Timer;

	if(length == 0)
	{
		return 0;
	}
	RemainBytes = RemindSoundServiceCt.ConstDataSize - RemindSoundServiceCt.ConstDataOffset;
	ReadBytes   = length > RemainBytes ? RemainBytes : length;
	if(ReadBytes == 0)
	{
		if(RemindSoundServiceCt.RemindState != REMIND_STOPPING )//避免消息时差，反复发起停止。
		{
			RemindSoundServiceCt.RemindState = REMIND_STOPPING;
			DecoderStop();//解码器不能自动停止。
		}
//		SoftFlagDeregister(SoftFlagDecoderRemind);//出让解码器，连续插播提示音时会有时隙。
		return 0;	//此次不加载数据
	}

	TimeOutSet(&Timer, 5000);
	while(EffectProcessFlagGet() == FALSE)
	{
		if(IsTimeOut(&Timer))
		{
			TimeOutSet(&Timer, 0);
			break;
		}
	}
	if(SpiFlashRead(RemindSoundServiceCt.ConstDataAddr + RemindSoundServiceCt.ConstDataOffset,
					buffer,
					ReadBytes,
					REMIND_FLASH_READ_TIMEOUT) != FLASH_NONE_ERR)
	{
		ReadBytes = 0;
		DBG("Remind:read const data error!\r\n");
	}
	DBG("Remind:%d@%d\n", (int)ReadBytes, (int)RemainBytes);
	RemindSoundServiceCt.ConstDataOffset += ReadBytes;
	
	return ReadBytes;
}

unsigned short CRC16(unsigned char *Buf, unsigned int BufLen, unsigned short CRC)
{
	unsigned int i;
	for(i = 0 ; i < BufLen; i++)
	{
		CRC = (CRC << 8) ^ CrcCCITTTable[((CRC >> 8) ^ *Buf++) & 0x00FF];
	}
	return CRC;
}

//提示音条目和数据区完整性校验，影响开机速度。
bool sound_clips_all_crc(void)
{
	SongClipsHdr *hdr;
	SongClipsEntry *ptr;
	uint16_t crc=0, i, j, CrcRead;
	uint32_t FlashAddr, all_len = 0;
	uint8_t *data_ptr = NULL;
	//uint8_t *data_ptr1 = NULL;// bkd add for test 2019.4.22

	bool ret = TRUE;
	FlashAddr = REMIND_FLASH_STORE_BASE;

	data_ptr = (uint8_t *)osPortMalloc(REMIND_FLASH_HDR_SIZE);
	//data_ptr1=data_ptr;
	if(data_ptr == NULL)
	{
		return FALSE;
	}
	if(SpiFlashRead(FlashAddr,
					data_ptr,
					REMIND_FLASH_HDR_SIZE,
					REMIND_FLASH_READ_TIMEOUT) != FLASH_NONE_ERR)
	{
		DBG("read const data error!\r\n");
		ret = FALSE;
	}
	else
	{

//		DBG("\r\n");

//	for(i=0;i<REMIND_FLASH_HDR_SIZE;i++)
	//	{
		
		//DBG("%x ",*data_ptr1);
		//data_ptr1++;
		
		//}
	//		DBG("\r\n");

		ptr = (SongClipsEntry*)(data_ptr + sizeof(SongClipsHdr));
		hdr = (SongClipsHdr *)(data_ptr);
		if(strncmp(hdr->sync, "MVUB", 4) || !hdr->cnt)
		{
			DBG("sync not found or no Item\n");
			ret = FALSE;
		}
		else
		{
			for(i = 0; i < hdr->cnt; i++)
			{
				all_len += ptr[i].size;
				for(j = 0; j < REMIND_SOUND_ID_LEN; j++)
				{
					DBG("%c", ((uint8_t *)&ptr[i].id)[j]);
				}
				DBG("/");
			}
			DBG("\nALL clips size = %d\n", (int)all_len);
			if(REMIND_FLASH_STORE_BASE + REMIND_FLASH_HDR_SIZE + all_len >= REMIND_FLASH_STORE_OVERFLOW)
			{
				DBG("Remind flash const data overflow.\n");
				ret = FALSE;
			}
			CrcRead = hdr->crc;
			crc = CRC16(data_ptr, 4, crc);
			crc = CRC16(data_ptr + 8, REMIND_FLASH_HDR_SIZE - 8, crc);
			FlashAddr += REMIND_FLASH_HDR_SIZE;
			while(all_len && ret)
			{
				if(all_len > REMIND_FLASH_HDR_SIZE)
				{
					i = REMIND_FLASH_HDR_SIZE;
				}
				else
				{
					i = all_len;
				}
				if(SpiFlashRead(FlashAddr,
								data_ptr,
								i,
								REMIND_FLASH_READ_TIMEOUT) != FLASH_NONE_ERR)
				{
					DBG("read const data error!\r\n");
					ret = FALSE;
				}
				else
				{
					crc = CRC16(data_ptr, i, crc);
					FlashAddr += i;
					all_len -= i;	
				}
			}
			DBG("crc = %04X|%04X\n", crc, CrcRead);
		}
	}
	osPortFree(data_ptr);
	return ret;
}

#endif

