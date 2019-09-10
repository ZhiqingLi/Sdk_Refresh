/**
 **************************************************************************************
 * @file    audio_vol.c
 * @brief   audio syetem vol set here
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-1-7 15:42:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include "type.h"
#include "app_config.h"
#include "app_message.h"
#include "dac.h"
#include "audio_adc.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "debug.h"
#include "audio_vol.h"
#include "audio_core_api.h"
#include "main_task.h"
#include "timeout.h"
#include "bt_play_mode.h"

#if (BT_HFP_SUPPORT == ENABLE)
#include "bt_hf_mode.h"
#include "bt_hf_api.h"
#endif

#include "ctrlvars.h"
#ifdef CFG_FUNC_DISPLAY_EN
#include "display_service.h"
#endif

bool gIsVolSetEnable = FALSE;
int32_t SetChannel = VOL_SET_CHANNEL_NONE;
static int32_t Cnt = -1;

uint8_t ChannelValid[VOL_SET_CHANNEL_SUM] = {0};
static volatile bool WhetherRecMusic = 1;


TIMER MenuTimer;//菜单键 时间控制，如果超过一定时间无相关按键触发，关闭菜单功能。

//音量表中数据表示的是音频通路数字部分的Gain值
//4095表示0dB,为0时表示Mute。音量可调整增益表中只做负增益
//需要正增益设置每个source源的预增益
//两级音量之间的计算公式为 "20*log(Vol1/Vol2)"，单位dB
const uint16_t gSysVolArr[CFG_PARA_MAX_VOLUME_NUM + 1] =
{
	0,
	26,		31,		36,		43,		50,		59,		70,		82,
	97,		114,	134,	158,	186,	219,	258,	304,
	357,	420,	495,	582,	685,	806,	948,	1115,
	1312,	1544,	1816,	2137,	2514,	2958,	3480,	4095
};

//静音设置与取消
//当前case是静音所有输出通道
//应用层自行保存相关状态
//客户可以根据需要自行灵活调整
void AudioPlayerMute(void)
{
	uint16_t i;
	mainAppCt.gSysVol.MuteFlag = !mainAppCt.gSysVol.MuteFlag;

	TimeOutSet(&MenuTimer, 5000);
	// 调用驱动层静音设置接口
	//if(mainAppCt.gSysVol.MuteFlag)
	if(IsAudioPlayerMute() == TRUE)
	{
		for(i = 0; i < AUDIO_CORE_SINK_MAX_NUM ;i++)
		{
			AudioCoreSinkMute(i, TRUE, TRUE);
		}
		DBG("\nMute\n");
	}
	else
	{
		for(i = 0; i < AUDIO_CORE_SINK_MAX_NUM ;i++)
		{
			AudioCoreSinkUnmute(i, TRUE, TRUE);
		}
		SystemVolSet();
		DBG("\nUnmute\n");
	}
}


bool IsAudioPlayerMute(void)
{
	return mainAppCt.gSysVol.MuteFlag;
}

void AudioPlayerMenu(void)
{
	uint32_t ValidNum = 0;;
	uint32_t i;
	
	TimeOutSet(&MenuTimer, 5000);
	gIsVolSetEnable = TRUE;

	for(i=0; i<AUDIO_CORE_SOURCE_MAX_MUN; i++)
	{
		if(mainAppCt.AudioCore->AudioSource[i].Enable)
		{
			ChannelValid[ValidNum] = VOL_SET_CHANNEL_SOURCE0 + i;
			ValidNum++;
		}
	}
	for(i=0; i<AUDIO_CORE_SINK_MAX_NUM; i++)
	{
		if(mainAppCt.AudioCore->AudioSink[i].Enable)
		{
			ChannelValid[ValidNum] = VOL_SET_CHANNEL_SINK0 + i;
			ValidNum++;
		}
	}

	DBG("Cnt=%d\n", (int)Cnt);
	SetChannel = ChannelValid[Cnt > ValidNum ? ValidNum : Cnt];

	Cnt++;
	if(Cnt > ValidNum)
	{
		Cnt = 0;
	}
	
	//DBG
	switch(SetChannel)
	{
	case VOL_SET_CHANNEL_SOURCE0:
		DBG("Source0 vol set\n");
		break;
	case VOL_SET_CHANNEL_SOURCE1:
		DBG("Source1 vol set\n");
		break;
	case VOL_SET_CHANNEL_SOURCE2:
		DBG("Source2 vol set\n");
		break;
	case VOL_SET_CHANNEL_SOURCE3:
		DBG("Source3 vol set\n");
		break;

	case VOL_SET_CHANNEL_SINK0:
		DBG("Sink0 vol set\n");
		break;
	case VOL_SET_CHANNEL_SINK1:
		DBG("Sink1 vol set\n");
		break;
#if (defined(CFG_RES_AUDIO_DACX_EN ) || defined(CFG_RES_AUDIO_I2SOUT_EN))
	case VOL_SET_CHANNEL_SINK2:
		DBG("Sink2 vol set(DACX)\n");
		break;
#endif
//	dafault:
//		break;
	}
}

//检查Menu 定时器超时
void AudioPlayerMenuCheck(void)
{
	if(gIsVolSetEnable && IsTimeOut(&MenuTimer))
	{
		gIsVolSetEnable = FALSE;
		SetChannel = VOL_SET_CHANNEL_NONE;
		if(Cnt > 0)
		{
			Cnt--;
		}
		else
		{
			Cnt = VOL_SET_CHANNEL_SUM;
		}
		DBG("Menu Timer OUT\n");
	}
}

void AudioMusicVolUp(void)
{
	//if(mainAppCt.gSysVol.MuteFlag)
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}

	if(mainAppCt.MusicVolume < CFG_PARA_MAX_VOLUME_NUM)
	{
		mainAppCt.MusicVolume++;
	}
    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.MusicVolume;
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = mainAppCt.MusicVolume;
	#endif
	DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	DBG("source2 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
	AudioCoreSourceVolSet(APP_SOURCE_NUM, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]]);
	AudioCoreSourceVolSet(REMIND_SOURCE_NUM, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]]);
#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
	//add volume sync(bluetooth play mode)
	if(GetSystemMode() == AppModeBtAudioPlay)
	{
		MessageContext		msgSend;

		SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

		msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
		MessageSend(GetBtPlayMessageHandle(), &msgSend);
	}
#endif

#if (BT_HFP_SUPPORT == ENABLE)
	if(GetSystemMode() == AppModeBtHfPlay)
	{
		SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[1]);
	}
#endif
#endif
}

void AudioMusicVolDown(void)
{
	//if(mainAppCt.gSysVol.MuteFlag)
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}

	if(mainAppCt.MusicVolume > 0)
	{
		mainAppCt.MusicVolume--;
	}
    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.MusicVolume;
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = mainAppCt.MusicVolume;
	#endif
	DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	DBG("source2 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
	AudioCoreSourceVolSet(APP_SOURCE_NUM, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]]);
	AudioCoreSourceVolSet(REMIND_SOURCE_NUM, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]]);
#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
	//add volume sync(bluetooth play mode)
	if(GetSystemMode() == AppModeBtAudioPlay)
	{
		MessageContext		msgSend;

		SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

		msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
		MessageSend(GetBtPlayMessageHandle(), &msgSend);
	}
#endif

#if (BT_HFP_SUPPORT == ENABLE)
	if(GetSystemMode() == AppModeBtHfPlay)
	{
		SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[1]);
	}
#endif
#endif
}

void AudioMusicVolSet(uint8_t musicVol)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}

	if(musicVol > CFG_PARA_MAX_VOLUME_NUM)
		mainAppCt.MusicVolume = CFG_PARA_MAX_VOLUME_NUM;
	else
		mainAppCt.MusicVolume = musicVol;
	
    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.MusicVolume;
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = mainAppCt.MusicVolume;
	#endif
	DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	AudioCoreSourceVolSet(APP_SOURCE_NUM, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]]);
}

#if CFG_RES_MIC_SELECT
void AudioMicVolUp(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}

	if(mainAppCt.MicVolume < CFG_PARA_MAX_VOLUME_NUM)
	{
		mainAppCt.MicVolume++;
	}
    mainAppCt.gSysVol.AudioSourceVol[0] = mainAppCt.MicVolume;
	DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
	AudioCoreSourceVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]]);
}

void AudioMicVolDown(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}

	if(mainAppCt.MicVolume > 0)
	{
		mainAppCt.MicVolume--;
	}
    mainAppCt.gSysVol.AudioSourceVol[0] = mainAppCt.MicVolume;
	DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
	AudioCoreSourceVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]]);
}
#endif

void SystemVolUp(void)
{
	//if(mainAppCt.gSysVol.MuteFlag)
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}

	gIsVolSetEnable = TRUE;
	SetChannel = VOL_SET_CHANNEL_SINK0;

	if(gIsVolSetEnable == TRUE)
	{
		DBG("vol up \n");
		TimeOutSet(&MenuTimer, 5000);
		switch(SetChannel)
		{
			case VOL_SET_CHANNEL_SINK0://sink0
				if(mainAppCt.gSysVol.AudioSinkVol[0] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSinkVol[0]++;
				}

				DBG("sink0 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[0]);
				AudioCoreSinkVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[0]]);
				break;
			case VOL_SET_CHANNEL_SINK1://sink1
				if(mainAppCt.gSysVol.AudioSinkVol[1] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSinkVol[1]++;
				}

				DBG("sink1 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[1]);
				AudioCoreSinkVolSet(1, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[1]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[1]]);
				break;

#if (defined(CFG_RES_AUDIO_DACX_EN ) || defined(CFG_RES_AUDIO_I2SOUT_EN))
			case VOL_SET_CHANNEL_SINK2:
				if(mainAppCt.gSysVol.AudioSinkVol[2] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSinkVol[2]++;
				}

				DBG("sink2 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[2]);
				AudioCoreSinkVolSet(2, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[2]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[2]]);
				break;
#endif
			case VOL_SET_CHANNEL_SOURCE0://source0
				if(mainAppCt.gSysVol.AudioSourceVol[0] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSourceVol[0]++;
				}

				DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
				AudioCoreSourceVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]]);
				break;
			case VOL_SET_CHANNEL_SOURCE1://source1
				if(mainAppCt.gSysVol.AudioSourceVol[1] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSourceVol[1]++;
				}

				DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[1]);
				AudioCoreSourceVolSet(1, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[1]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[1]]);
#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
				//add volume sync(bluetooth play mode)
				if(GetSystemMode() == AppModeBtAudioPlay)
				{
					MessageContext		msgSend;

					SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[1]);

					msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
					MessageSend(GetBtPlayMessageHandle(), &msgSend);
				}
#endif

#if (BT_HFP_SUPPORT == ENABLE)
				if(GetSystemMode() == AppModeBtHfPlay)
				{
					SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[1]);
				}
#endif
#endif
				break;
			case VOL_SET_CHANNEL_SOURCE2://source2
				if(mainAppCt.gSysVol.AudioSourceVol[2] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSourceVol[2]++;
				}

				DBG("source2 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[2]);
				AudioCoreSourceVolSet(2, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[2]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[2]]);
				break;
			case VOL_SET_CHANNEL_SOURCE3://source3
				if(mainAppCt.gSysVol.AudioSourceVol[3] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSourceVol[3]++;
				}
				
				DBG("source3 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[3]);
				AudioCoreSourceVolSet(3, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[3]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[3]]);
				break;
/*
			case VOL_SET_CHANNEL_SOURCE4://source3
				if(mainAppCt.gSysVol.AudioSourceVol[4] < CFG_PARA_MAX_VOLUME_NUM)
				{
					mainAppCt.gSysVol.AudioSourceVol[4]++;
				}

				DBG("source4 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[4]);
				AudioCoreSourceVolSet(4, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[4]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[4]]);
				break;
*/
			default:
			break;
		}
	}
	else
	{
		DBG("vol set disable\n");
	}
}

void SystemVolDown(void)
{
	//if(mainAppCt.gSysVol.MuteFlag)
	if(IsAudioPlayerMute() == TRUE)
	{
		AudioPlayerMute();
	}
	gIsVolSetEnable = TRUE;
	SetChannel = VOL_SET_CHANNEL_SINK0;
	if(gIsVolSetEnable == TRUE)
	{
		DBG("vol down \n");
		TimeOutSet(&MenuTimer, 5000);
		switch(SetChannel)
		{
			case VOL_SET_CHANNEL_SINK0://sink0
				if(mainAppCt.gSysVol.AudioSinkVol[0] > 0)
				{
					mainAppCt.gSysVol.AudioSinkVol[0]--;
				}

				DBG("sink0 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[0]);
				AudioCoreSinkVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[0]]);
				break;
			case VOL_SET_CHANNEL_SINK1://sink1
				if(mainAppCt.gSysVol.AudioSinkVol[1] > 0)
				{
					mainAppCt.gSysVol.AudioSinkVol[1]--;
				}

				DBG("sink1 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[1]);
				AudioCoreSinkVolSet(1, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[1]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[1]]);
				break;

#if (defined(CFG_RES_AUDIO_DACX_EN ) || defined(CFG_RES_AUDIO_I2SOUT_EN))
			case VOL_SET_CHANNEL_SINK2:
				if(mainAppCt.gSysVol.AudioSinkVol[2] > 0)
				{
					mainAppCt.gSysVol.AudioSinkVol[2]--;
				}
				
				DBG("sink2 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[2]);
				AudioCoreSinkVolSet(2, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[2]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[2]]);
				break;
#endif

			case VOL_SET_CHANNEL_SOURCE0://source0
				if(mainAppCt.gSysVol.AudioSourceVol[0] > 0)
				{
					mainAppCt.gSysVol.AudioSourceVol[0]--;
				}

				DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
				AudioCoreSourceVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]]);
				break;
			case VOL_SET_CHANNEL_SOURCE1://source1
				if(mainAppCt.gSysVol.AudioSourceVol[1] > 0)
				{
					mainAppCt.gSysVol.AudioSourceVol[1]--;
				}

				DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[1]);
				AudioCoreSourceVolSet(1, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[1]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[1]]);
#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC == ENABLE)
				//add volume sync(bluetooth play mode)
				if(GetSystemMode() == AppModeBtAudioPlay)
				{
					MessageContext		msgSend;

					SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[1]);

					msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
					MessageSend(GetBtPlayMessageHandle(), &msgSend);
				}
#endif

#if (BT_HFP_SUPPORT == ENABLE)
				if(GetSystemMode() == AppModeBtHfPlay)
				{
					SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[1]);
				}
#endif
#endif
				break;
			case VOL_SET_CHANNEL_SOURCE2://source2
				if(mainAppCt.gSysVol.AudioSourceVol[2] > 0)
				{
					mainAppCt.gSysVol.AudioSourceVol[2]--;
				}

				DBG("source2 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[2]);
				AudioCoreSourceVolSet(2, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[2]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[2]]);
				break;
			case VOL_SET_CHANNEL_SOURCE3://source3
				if(mainAppCt.gSysVol.AudioSourceVol[3] > 0)
				{
					mainAppCt.gSysVol.AudioSourceVol[3]--;
				}

				DBG("source3 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[3]);
				AudioCoreSourceVolSet(3, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[3]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[3]]);
				break;

	
			default:
			break;
		}
	}
	else
	{
		DBG("vol set disable\n");
	}
}

void SystemVolSet(void)
{
	uint32_t i;
	
	for(i=0; i<AUDIO_CORE_SOURCE_MAX_MUN; i++)
	{
		AudioCoreSourceVolSet(i, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[i]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[i]]);
	}
	for(i=0; i<AUDIO_CORE_SINK_MAX_NUM; i++)
	{
		AudioCoreSinkVolSet(i, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[i]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[i]]);
	}
}

void SystemVolSetChannel(VOL_CHANNEL_SEL SetChannel, uint8_t volume)
{
	switch(SetChannel)
	{
		case VOL_SET_CHANNEL_SINK0:
			//if(mainAppCt.gSysVol.MuteFlag)
			if(IsAudioPlayerMute() == TRUE)
			{
				AudioPlayerMute();
			}
			mainAppCt.gSysVol.AudioSinkVol[0] = volume;
			DBG("sink0 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[0]);
			AudioCoreSinkVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[0]]);
			break;
			
		case VOL_SET_CHANNEL_SINK1:
			//if(mainAppCt.gSysVol.MuteFlag)
			if(IsAudioPlayerMute() == TRUE)
			{
				AudioPlayerMute();
			}
			mainAppCt.gSysVol.AudioSinkVol[1] = volume;
			DBG("sink1 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[1]);
			AudioCoreSinkVolSet(1, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[1]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[1]]);
			break;
			
#if (defined(CFG_RES_AUDIO_DACX_EN ) || defined(CFG_RES_AUDIO_I2SOUT_EN))
		case VOL_SET_CHANNEL_SINK2:
			//if(mainAppCt.gSysVol.MuteFlag)
			if(IsAudioPlayerMute() == TRUE)
			{
				AudioPlayerMute();
			}
			mainAppCt.gSysVol.AudioSinkVol[2] = volume;
			DBG("sink2 vol = %d\n", mainAppCt.gSysVol.AudioSinkVol[2]);
			AudioCoreSinkVolSet(2, gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[2]], gSysVolArr[mainAppCt.gSysVol.AudioSinkVol[2]]);
			break;
#endif
			
		case VOL_SET_CHANNEL_SOURCE0:
			//if(mainAppCt.gSysVol.MuteFlag)
			if(IsAudioPlayerMute() == TRUE)
			{
				AudioPlayerMute();
			}
			mainAppCt.gSysVol.AudioSourceVol[0] = volume;
			DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
			AudioCoreSourceVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]]);
			break;
		
		case VOL_SET_CHANNEL_SOURCE1:
			//if(mainAppCt.gSysVol.MuteFlag)
			if(IsAudioPlayerMute() == TRUE)
			{
				AudioPlayerMute();
			}
			mainAppCt.gSysVol.AudioSourceVol[1] = volume;
			DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[1]);
			AudioCoreSourceVolSet(1, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[1]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[1]]);
			break;
			
		case VOL_SET_CHANNEL_SOURCE2:
			//if(mainAppCt.gSysVol.MuteFlag)
			if(IsAudioPlayerMute() == TRUE)
			{
				AudioPlayerMute();
			}
			mainAppCt.gSysVol.AudioSourceVol[2] = volume;
			DBG("source2 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[2]);
			AudioCoreSourceVolSet(2, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[2]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[2]]);
			break;
			
		case VOL_SET_CHANNEL_SOURCE3:
			//if(mainAppCt.gSysVol.MuteFlag)
			if(IsAudioPlayerMute() == TRUE)
			{
				AudioPlayerMute();
			}
			mainAppCt.gSysVol.AudioSourceVol[3] = volume;
			DBG("source3 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[3]);
			AudioCoreSourceVolSet(3, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[3]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[3]]);
			break;

	
		default:
			break;
	}
}

#ifdef CFG_FUNC_EQ_MODE_EN
/*
****************************************************************
* EQ Mode调节函数
*
*
****************************************************************
*/
void EqModeSet(uint8_t EqMode)
{
    switch(EqMode)
	{
		case EQ_MODE_FLAT:
			LoadEqMode(&Eq_Flat[0]);
			break;
		case EQ_MODE_CLASSIC:
			LoadEqMode(&Eq_Classic[0]);
			break;
		case EQ_MODE_POP:
			LoadEqMode(&Eq_Pop[0]);
			break;	
		case EQ_MODE_ROCK:
			LoadEqMode(&Eq_Rock[0]);
			break;
		case EQ_MODE_JAZZ:
			LoadEqMode(&Eq_Jazz[0]);
			break;
		case EQ_MODE_VOCAL_BOOST:
			LoadEqMode(&Eq_VocalBoost[0]);
			break;			
	}
}
#endif
/*
****************************************************************
* 用户相关音效调节参数同步函数
*说明:
*    调用AudioEffectModeSel()设置音效模式之后，需要再调用下此函数，保证用户设置音效参数同步
****************************************************************
*/
void AudioEffectParamSync(void)
{
#ifdef CFG_FUNC_TREB_BASS_EN
	BassTrebAjust(mainAppCt.BassStep, mainAppCt.TrebStep);	
#endif
#ifdef CFG_FUNC_EQ_MODE_EN	
	EqModeSet(mainAppCt.EqMode);	
#endif
#ifdef CFG_FUNC_MIC_KARAOKE_EN	
	ReverbStepSet(mainAppCt.ReverbStep);
#endif
}

#ifdef CFG_ADC_LEVEL_KEY_EN	
/*
****************************************************************
* 电位器消息接收处理函数
*说明:
*    AdcLevelCh和app_config.h中选择的GPIO有如下对应关系:

       #define  ADCLEVL_CHANNEL_MAP            (ADC_GPIOA20|ADC_GPIOA21|ADC_GPIOA22)
       
       ADC_GPIOA20对应AdcLevelCh为1；
       ADC_GPIOA21对应AdcLevelCh为2；
       ADC_GPIOA22对应AdcLevelCh为3；
****************************************************************
*/
void AdcLevelMsgProcess(uint16_t Msg)//Sliding resistance
{
	uint16_t AdcLevelCh, AdcValue;

    if( (Msg > MSG_ADC_LEVEL_MSG_START)&&(Msg < MSG_ADC_LEVEL_MSG_END) )
	{				
		AdcLevelCh      =   Msg&0xff00;
		AdcLevelCh      -=  MSG_ADC_LEVEL_MSG_START;
		AdcLevelCh      >>= 8;

		AdcValue      =   Msg &0x00ff;//(0~~16) = (MSG_ADC_LEVEL_MSG_0~~MSG_ADC_LEVEL_MSG_16)

        DBG("AdcLevelCh = %d\n", AdcLevelCh);
		DBG("AdcValue = %d\n",AdcValue);
		switch(AdcLevelCh)
		{

			case 1://ADC LEVEL Channel 1
			    #if CFG_RES_MIC_SELECT
			    mainAppCt.gSysVol.AudioSourceVol[0] = mainAppCt.MicVolume = AdcValue;
				DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
				AudioCoreSourceVolSet(0, gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]], gSysVolArr[mainAppCt.gSysVol.AudioSourceVol[0]]);
				#endif
				break;
				
			case 2://ADC LEVEL Channel 2
				#ifdef CFG_FUNC_MIC_KARAOKE_EN	
			    mainAppCt.ReverbStep = AdcValue;
				ReverbStepSet(mainAppCt.ReverbStep);
				DBG("ReverbStep = %d\n", mainAppCt.ReverbStep);
				#endif
				break;
				
			case 3://ADC LEVEL Channel 3
				mainAppCt.BassStep = 15 - AdcValue/2;
				mainAppCt.TrebStep = AdcValue/2;
				BassTrebAjust(mainAppCt.BassStep, mainAppCt.TrebStep);
				break;

			case 4://ADC LEVEL Channel 4
				
				break;
			case 5://ADC LEVEL Channel 5
				
				break;
			case 6://ADC LEVEL Channel 6
				
				break;
			case 7://ADC LEVEL Channel 7
				
				break;
			case 8://ADC LEVEL Channel 8
				
				break;
			case 9://ADC LEVEL Channel 9
				
				break;
			case 10://ADC LEVEL Channel 10
				
				break;
			case 11://ADC LEVEL Channel 11
				
				break;
		}
	}
}
#endif


//1:rec music 	0:not rec music
static void SetRecMusic(void)
{	
	if(WhetherRecMusic)
	{		
		DBG("not rec music\n");
		WhetherRecMusic=0;		
	}
	else
	{
		DBG("rec music\n");
		WhetherRecMusic=1;
	}
}

//1:rec music 	0:not rec music
bool GetWhetherRecMusic(void)
{
	return WhetherRecMusic;
}

//各模式下的通用消息处理
void CommonMsgProccess(uint16_t Msg)
{
#ifdef CFG_FUNC_DISPLAY_EN
	MessageContext	msgSend;
#endif
	switch(Msg)
	{
		case MSG_DEVICE_SERVICE_MENU://菜单键
			DBG("menu key\n");
			AudioPlayerMenu();
			break;

		case MSG_DEVICE_SERVICE_MUTE:
			DBG("mute/unmute\n");
			AudioPlayerMute();
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_MUTE;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		case MSG_DEVICE_SERVICE_VOLUP:
			SystemVolUp();
			DBG("MSG_DEVICE_SERVICE_VOLUP\n");
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_VOL;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		case MSG_DEVICE_SERVICE_VOLDOWN:
			SystemVolDown();
			DBG("MSG_DEVICE_SERVICE_VOLDOWN\n");
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_VOL;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		case MSG_DEVICE_SERVICE_MUSIC_VOLUP:
			AudioMusicVolUp();
			DBG("MSG_DEVICE_SERVICE_MUSIC_VOLUP\n");
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_MUSIC_VOL;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
			
		case MSG_DEVICE_SERVICE_MUSIC_VOLDOWN:
			AudioMusicVolDown();
			DBG("MSG_DEVICE_SERVICE_MUSIC_VOLDOWN\n");
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_MUSIC_VOL;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		#if CFG_RES_MIC_SELECT	
		case MSG_DEVICE_SERVICE_MIC_VOLUP:
			AudioMicVolUp();
			DBG("MSG_DEVICE_SERVICE_MIC_VOLUP\n");
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_MIC_VOL;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
			
		case MSG_DEVICE_SERVICE_MIC_VOLDOWN:
			AudioMicVolDown();
			DBG("MSG_DEVICE_SERVICE_MIC_VOLDOWN\n");
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_MIC_VOL;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
		#endif
		
#ifdef CFG_FUNC_AUDIO_EFFECT_EN		
		#ifdef CFG_FUNC_MIC_KARAOKE_EN
		case MSG_DEVICE_SERVICE_MIC_EFFECT_UP:
			if(mainAppCt.ReverbStep < MAX_MIC_REVB_STEP)
			{
				mainAppCt.ReverbStep++;
			}
			else
			{
				mainAppCt.ReverbStep = 0;
			}
			ReverbStepSet(mainAppCt.ReverbStep);
			DBG("MSG_DEVICE_SERVICE_MIC_EFFECT_UP\n");
			DBG("ReverbStep = %d\n", mainAppCt.ReverbStep);
			break;
		#endif
		
		#ifdef CFG_FUNC_EQ_MODE_EN
		case MSG_DEVICE_SERVICE_EQ:
			DBG("MSG_DEVICE_SERVICE_EQ\n");
			if(mainAppCt.EqMode < EQ_MODE_VOCAL_BOOST)
			{
				mainAppCt.EqMode++;
			}
			else
			{
				mainAppCt.EqMode = EQ_MODE_FLAT;
			}
			EqModeSet(mainAppCt.EqMode);	
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_EQ;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
		#endif

		#ifdef CFG_FUNC_MIC_KARAOKE_EN
		case MSG_DEVICE_SERVICE_EFFECTMODE:
			DBG("MSG_DEVICE_SERVICE_EFFECTMODE\n");
			if(mainAppCt.EffectMode < EFFECT_MODE_WaWaYin)
			{
				mainAppCt.EffectMode++;
			}
			else
			{
				mainAppCt.EffectMode = EFFECT_MODE_HunXiang;
			}
			AudioEffectModeSel(mainAppCt.EffectMode, 1);//0=init hw,1=effect,2=hw+effect
			{
				void EffectChange(void);
				EffectChange();
			}
			break;
			
		case MSG_DEVICE_SERVICE_MIC_SHUNNING:
			#ifdef CFG_FUNC_SHUNNING_EN
			gCtrlVars.ShunningMode = !gCtrlVars.ShunningMode;
			#endif
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_SHUNNING;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
		#endif		
		
		#ifdef CFG_FUNC_TREB_BASS_EN
		case MSG_DEVICE_SERVICE_TREB_UP:
			DBG("TREB up\n");
			if(mainAppCt.TrebStep < MAX_BASS_TREB_GAIN)
			{
				mainAppCt.TrebStep++;
			}
			BassTrebAjust(mainAppCt.BassStep, mainAppCt.TrebStep);
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_TRE;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		case MSG_DEVICE_SERVICE_TREB_DW:
			DBG("TREB down\n");
			if(mainAppCt.TrebStep)
			{
				mainAppCt.TrebStep--;
			}
			BassTrebAjust(mainAppCt.BassStep, mainAppCt.TrebStep);
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_TRE;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
			
		case MSG_DEVICE_SERVICE_BASS_UP:
			DBG("BASS up\n");
			if(mainAppCt.BassStep < MAX_BASS_TREB_GAIN)
			{
				mainAppCt.BassStep++;
			}
			BassTrebAjust(mainAppCt.BassStep, mainAppCt.TrebStep);	
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_BAS;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		case MSG_DEVICE_SERVICE_BASS_DW:
			DBG("BASS down\n");
			if(mainAppCt.BassStep)
			{
				mainAppCt.BassStep--;
			}
			BassTrebAjust(mainAppCt.BassStep, mainAppCt.TrebStep);	
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_BAS;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
		#endif
		
		case MSG_DEVICE_SERVICE_3D:
			#if CFG_AUDIO_EFFECT_MUSIC_3D_EN
			gCtrlVars.music_threed_unit.three_d_en = !gCtrlVars.music_threed_unit.three_d_en;
			#endif
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_3D;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		case MSG_DEVICE_SERVICE_VOCAL_CUT:
			gCtrlVars.vocal_cut_unit.vocal_cut_en = !gCtrlVars.vocal_cut_unit.vocal_cut_en;
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_VOCAL_CUT;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;

		case MSG_DEVICE_SERVICE_VB:
			#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
			gCtrlVars.music_vb_unit.vb_en = !gCtrlVars.music_vb_unit.vb_en;
			#endif
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_VB;
            MessageSend(GetDisplayMessageHandle(), &msgSend);
			#endif
			break;
			
#endif

		case MSG_DEVICE_SERVICE_MUSIC_WHETHER_REC:
			SetRecMusic();
			break;

		default:
		
#ifdef CFG_ADC_LEVEL_KEY_EN			
			AdcLevelMsgProcess(Msg);
#endif
			break;
	}
}
