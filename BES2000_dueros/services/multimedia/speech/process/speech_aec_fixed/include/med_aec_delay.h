#include "med_aec_comm.h"

#ifndef _MED_AEC_DELAY_H
#define _MED_AEC_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    VOS_INT16 frameLength;
    VOS_INT16 delayLength;
    VOS_INT16 *delayBuf;
} MED_AEC_DELAY_STRU;

extern VOS_VOID MED_AEC_DelayInit(VOS_INT16 frameLength, VOS_INT16 delayLength, MED_AEC_DELAY_STRU *pstDelay);

extern VOS_VOID MED_AEC_DelayDestroy(MED_AEC_DELAY_STRU *pstDelay);

extern VOS_VOID MED_AEC_DelayMain(MED_AEC_DELAY_STRU *pstHpf, VOS_INT16 *pstBuf);


#ifdef __cplusplus
}
#endif

#endif /* end of med_delay.h */
