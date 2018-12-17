/**
 *******************************************************************************
 * @file    bt_audio_play.h
 * @author  Halley
 * @version V1.0.1
 * @date    27-Apr-2016
 * @brief   Auido play
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __BT_AUDIO_PLAY_H__
#define __BT_AUDIO_PLAY_H__

#include "type.h"

typedef enum 
{
	BT_AUDIO_PLAY_STATE_STOP = 0,
	BT_AUDIO_PLAY_STATE_PLAYING
} BT_AUDIO_PLAY_STATE;

void SetBtAudioPlayState(BT_AUDIO_PLAY_STATE state);

BT_AUDIO_PLAY_STATE GetBtAudioPlayState(void);

bool IsBtAudioPlayPlaying(void);

#endif

