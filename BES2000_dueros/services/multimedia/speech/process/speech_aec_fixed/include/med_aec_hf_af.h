

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "med_aec_comm.h"

#ifndef _MED_AEC_HF_AF_H_
#define _MED_AEC_HF_AF_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* AF 宏定义*/
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_HF_AF_FILT_NB           (256)                                   /* 窄带MDF滤波块长度 */
#else
#define MED_AEC_HF_AF_FILT_NB           (240)                                   /* 窄带MDF滤波块长度 */
#endif
#define MED_AEC_HF_AF_W_QN_FILT         (13)                                    /* Filter函数中W 原始定标精度 */
#define MED_AEC_HF_AF_WEIGPOWER_THD     (8)                                     /* IFFT函数输入信号的最大限制2^9 */
#define MED_AEC_HF_AF_ERRABS_QN         (9)                                     /* 频域误差信号的绝对值放大倍数*/
#define MED_AEC_HF_AF_BIGNORMERR_QN     (13)                                    /* 滤波器系数计算过程中临时定标精度 */
#define MED_AEC_HF_AF_SMALLNORMERR_QN   (20)                                    /* 滤波器系数计算过程中临时定标精度 */
#define MED_AEC_HF_AF_Q5                (32)                                    /* 2^5 */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
/* AF公共结构体 */

/* AF系数更新结构体 */
typedef struct
{
    VOS_INT16                           shwStep;
    VOS_INT16                           shwPowTh;
    VOS_INT32                           swFrmCount;
    VOS_INT16                           shwSegmLen;
    VOS_INT16                           shwAlphFac;
    VOS_INT16                           shwMaxWIdx;
    VOS_INT16                           shwReserve;
    VOS_INT32                           aswWeight[MED_AEC_HF_AF_M][MED_AEC_MAX_FFT_LEN];
    VOS_INT32                           aswFarPow[(MED_AEC_MAX_FFT_LEN / 2) + 1];
	VOS_INT32                           aswGrad[MED_AEC_HF_AF_M][MED_AEC_MAX_FFT_LEN];
} MED_AEC_HF_COEFUPDT_STRU;

/* AF状态 */
typedef struct
{
    MED_AEC_AF_FFT_LEN_ENUM_INT16       shwFftLen;                              /* FFT长度 */
    MED_FFT_NUM_ENUM_INT16              enFftNumIndex;                          /* FFT长度索引枚举 */
    MED_FFT_NUM_ENUM_INT16              enIfftNumIndex;                         /* IFFT长度索引枚举 */
    VOS_INT16                           shwReserve;
    VOS_INT16                           ashwFar[MED_AEC_MAX_FFT_LEN];
    VOS_INT16                           ashwFarFreq[MED_AEC_HF_AF_M][MED_AEC_MAX_FFT_LEN];
    VOS_INT16                           ashwNear[MED_AEC_MAX_FFT_LEN];
    MED_AEC_HF_COEFUPDT_STRU            stCoefUpdt;
} MED_AEC_HF_AF_STRU;


/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_VOID MED_AEC_HF_AfInit(
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_HF_AF_STRU               *pstHfAf);

extern VOS_VOID MED_AEC_HF_AfMain (
                MED_AEC_HF_AF_STRU            *pstHfAf,
                VOS_INT16                     *pshwNear,
                VOS_INT16                     *pshwFar,
                VOS_INT16                     *pshwErr);

extern VOS_VOID MED_AEC_HF_AfFilt(
                VOS_INT16  ashwFarFreq[][MED_AEC_MAX_FFT_LEN],
                VOS_INT32  aswWeight[][MED_AEC_MAX_FFT_LEN],
                VOS_INT16  enFftLen,
                VOS_INT16  enIfftNumIndex,
                VOS_INT16 *pshwEcho);

extern VOS_VOID MED_AEC_HF_AfErrorCalc(
                VOS_INT16 *pshwEcho,
                VOS_INT16 *pshwNear,
                VOS_INT16  enFftLen,
                VOS_INT16  enFftNumIndex,
                VOS_INT16 *pshwErr,
                VOS_INT16 *pshwErrFreq);

extern VOS_VOID MED_AEC_HF_AfCoefUpdt(
                MED_AEC_HF_COEFUPDT_STRU *pstCoefUpdt,
                VOS_INT16 *pshwErrFreq,
                VOS_INT16  ashwFarFreq[][MED_AEC_MAX_FFT_LEN],
                VOS_INT16  enFftLen,
                VOS_INT16  enFftNumIndex,
                VOS_INT16  enIfftNumIndex);

extern VOS_VOID MED_AEC_HF_AfCoefTDMang(
                VOS_INT32  aswGrad[][MED_AEC_MAX_FFT_LEN],
                VOS_INT16  shwFftLen,
                VOS_INT16  enIfftNumIndex,
                VOS_INT16  enFftNumIndex,
                VOS_INT32  aswWeight[][MED_AEC_MAX_FFT_LEN]);

extern VOS_VOID MED_AEC_HF_AfEchoLocat(
                MED_AEC_HF_COEFUPDT_STRU   *pstCoefUpdt,
                VOS_INT16                   shwFftLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_af.h*/

