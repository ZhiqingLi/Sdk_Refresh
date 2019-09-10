/**
 **************************************************************************************
 * @file    playback_service.h
 * @brief   
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-03-31 13:06:47$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __PLAYBACK_SERVICE_H__
#define __PLAYBACK_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
#include "decoder_service.h"
//#include "audio_utility.h"

uint32_t PlaybackServiceFillStreamCallback(void *buffer, uint32_t length);

MessageHandle GetPlaybackServiceMessageHandle(void);

int32_t PlaybackServiceCreate(MessageHandle parentMsgHandle);

void PlaybackServiceStart(void);


void PlaybackServiceStop(void);

void PlaybackServiceFree(void);


void PlaybackServiceKill(void);

void PlaybackServiceItemRequest(uint16_t SoundId);

void PlaybackServicePlay(void);

void PlaybackStop(void);
void PlaybackServiceFree(void);


uint16_t PlaybackDataGet(void* Buf, uint16_t Len);

uint16_t PlaybackDataLenGet(void);




#ifdef __cplusplus
}
#endif//__cplusplus

#endif /* __PLAYBACK_SERVICE_H__ */

