///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: line_in_control.c
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////


#ifndef _AUDIO_FILE_H_
#define _AUDIO_FILE_H_

extern void AdcRemindDetScan(void);
extern void GpioRemindDetScan(void);
extern bool IsRmsPcmDataReminding(void);
extern void MixerSoundRemindInit(void);
extern void MixerSoundRemind(uint16_t SoundId);
extern uint32_t RmsPcmDataRead(uint8_t* Buffer, uint32_t NumSamplesRequired);

#endif

