/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2013-01-18 16:40     1.0                build this file
 ********************************************************************************/
#ifndef __USOUND_ASRC_H__
#define __USOUND_ASRC_H__

#include <attr.h>
#include "typeext.h"
#include "audio_device.h"

#define K_RECORD_LEN              3

#define K_HE2HF                   (0)
#define K_HF2HE                   (1)
#define K_NO_RECORD               (2)

#define K_DELTA_INIT_DECIMATION0  (800)
#define K_DELTA_INIT_DECIMATION1  (800)

//#define K_SAMPLERATE              (48000)
#define K_LOCK_PROTECT_FREQ1      (K_SAMPLERATE/1000)

#define K_LOCK_PROTECT_FREQ2      (K_SAMPLERATE/4000)

#define K_NO_NEED_MOD_THRESHOLD   (0x4000)

extern asrc_adjust_record_t g_asrc_out1_record;

extern void init_channel_record(asrc_adjust_record_t *pCh) __FAR__;

extern void deal_timer_process(void) __FAR__;

#endif //__USOUND_ASRC_H__
