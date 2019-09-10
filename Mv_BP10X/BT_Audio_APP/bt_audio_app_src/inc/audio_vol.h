/**
 **************************************************************************************
 * @file    audio_vol.h
 * @brief   audio syetem vol set here
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-1-7 15:42:47$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef __AUDIO_VOL_H__
#define __AUDIO_VOL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"

typedef enum
{
	VOL_SET_CHANNEL_NONE    = -1,
	VOL_SET_CHANNEL_SOURCE0 = 0,
	VOL_SET_CHANNEL_SOURCE1,
	VOL_SET_CHANNEL_SOURCE2,
	VOL_SET_CHANNEL_SOURCE3,
	//VOL_SET_CHANNEL_SOURCE4,//from owen x6
	VOL_SET_CHANNEL_SINK0,
	VOL_SET_CHANNEL_SINK1,
#if (defined(CFG_RES_AUDIO_DACX_EN ) || defined(CFG_RES_AUDIO_I2SOUT_EN))
	VOL_SET_CHANNEL_SINK2,
#endif
	VOL_SET_CHANNEL_SUM,
}VOL_CHANNEL_SEL;


bool IsAudioPlayerMute(void);
void AudioPlayerMute(void);
void AudioPlayerMenu(void);
void AudioPlayerMenuCheck(void);

void AudioMusicVolSet(uint8_t musicVol);

void SystemVolUp(void);
void SystemVolDown(void);
void SystemVolSet(void);
void SystemVolSetChannel(VOL_CHANNEL_SEL SetChannel, uint8_t volume);

void AudioEffectParamSync(void);

void CommonMsgProccess(uint16_t Msg);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

