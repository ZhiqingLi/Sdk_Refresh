#include "v_typdef.h"

#ifndef __MED_EMPH_FILTER_H__
#define __MED_EMPH_FILTER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    VOS_FLOAT   shwPreemph;
    VOS_FLOAT   shwPreemphMem;
} MED_PREEMPH_STRU;

typedef struct
{
    VOS_FLOAT   shwDeemph;
    VOS_FLOAT   shwDeemphMem;
} MED_DEEMPH_STRU;

extern VOS_VOID MED_PREEMPH_Init(
    MED_PREEMPH_STRU   *pstEmph,
    VOS_FLOAT shwEmph,
    VOS_INT32 shwFrameLength);

extern VOS_VOID MED_PREEMPH_MAIN(
    MED_PREEMPH_STRU   *pstEmph,
    VOS_INT16 *shwBuf,
    VOS_INT32 shwFrameLength);

extern VOS_VOID MED_DEEMPH_Init(
    MED_DEEMPH_STRU   *pstEmph,
    VOS_FLOAT shwEmph,
    VOS_INT32 shwFrameLength);

extern VOS_VOID MED_DEEMPH_MAIN(
    MED_DEEMPH_STRU   *pstEmph,
    VOS_INT16 *shwBuf,
    VOS_INT32 shwFrameLength);

#ifdef __cplusplus
}
#endif

#endif /* end of med_emph_filter.h */
