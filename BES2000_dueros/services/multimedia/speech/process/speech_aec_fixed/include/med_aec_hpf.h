
#include "med_aec_comm.h"
#include "iir_process.h"

#ifndef _MED_AEC_HPF_H
#define _MED_AEC_HPF_H

#ifdef __cplusplus
extern "C" {
#endif

/* HPF ״̬*/
typedef struct
{
    VOS_INT32 shwFrmLen;
    IIR_MONO_CFG_T cfg;
} MED_AEC_HPF_STRU;

extern VOS_VOID MED_AEC_HPFInit(VOS_INT32 frameLength, MED_AEC_HPF_STRU *pstHpf);

extern VOS_VOID MED_AEC_HPFMain(MED_AEC_HPF_STRU *pstHpf, VOS_INT16 *pstBuf);

#ifdef __cplusplus
}
#endif

#endif /* end of med_aec_hpf.h */
