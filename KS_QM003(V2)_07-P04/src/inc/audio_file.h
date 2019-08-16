/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : audio_file.h
  Version       : Initial Draft
  Author        : qing
  Created       : 2019/8/14
  Last Modified :
  Description   : audio_file.c header file
  Function List :
  History       :
  1.Date        : 2019/8/14
    Author      : qing
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

#ifndef __AUDIO_FILE_H__
#define __AUDIO_FILE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern bool IsRmsPcmDataReminding(void);
extern void MixerSoundRemind(uint16_t SoundId);
extern uint32_t RmsPcmDataRead(uint8_t* Buffer, uint32_t NumSamplesRequired);
extern void RmsPcmDataRemindingSet(bool State);
extern void IsRmsPcmDataRemindInit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __AUDIO_FILE_H__ */
