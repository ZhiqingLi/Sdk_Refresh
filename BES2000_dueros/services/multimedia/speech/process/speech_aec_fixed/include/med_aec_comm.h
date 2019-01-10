
#ifndef _MED_AEC_COMM_H_
#define _MED_AEC_COMM_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "med_fft.h"
#include "codec_com_codec.h"
#include "med_aec_main.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* FFT IFFT回调函数 */
typedef VOS_VOID (*MED_AEC_FFT_CALLBACK)(VOS_INT16 *, VOS_INT16 *);

#ifdef AEC_ONLY_SUPPORT_8k
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_FFT_LEN_NB              (256)
#else
#define MED_AEC_FFT_LEN_NB              (240)
#endif
#define MED_AEC_MAX_FFT_LEN             (MED_AEC_FFT_LEN_NB)                    /* 宽带FFT长度 */
#else
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_FFT_LEN_NB              (256)
#define MED_AEC_FFT_LEN_WB              (512)
#else
#define MED_AEC_FFT_LEN_NB              (240)
#define MED_AEC_FFT_LEN_WB              (480)
#endif
#define MED_AEC_MAX_FFT_LEN             (MED_AEC_FFT_LEN_WB)                    /* 宽带FFT长度 */
#endif

#define MED_AEC_HF_AF_M                 (2)

/* 临时全局数组，供AEC各模块使用 */
//extern VOS_INT16                        g_ashwMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];
//extern VOS_INT16                        g_ashwMedAecTmp2Len640[MED_AEC_MAX_FFT_LEN];
//extern VOS_INT32                        g_aswMedAecTmp1Len320[CODEC_FRAME_LENGTH_WB];
//extern VOS_INT32                        g_aswMedAecTmp1Len640[MED_AEC_MAX_FFT_LEN];

/* AEC 公共宏定义*/
//#define MED_AEC_GetshwVecTmp640Ptr1()   (&g_ashwMedAecTmp1Len640[0])            /* 临时全局数组指针 长度640 INT16 */
//#define MED_AEC_GetshwVecTmp640Ptr2()   (&g_ashwMedAecTmp2Len640[0])            /* 临时全局数组指针 长度640 INT16 */
//#define MED_AEC_GetswVecTmp320Ptr1()    (&g_aswMedAecTmp1Len320[0])             /* 临时全局数组指针 长度320 INT32 */
//#define MED_AEC_GetswVecTmp640Ptr1()    (&g_aswMedAecTmp1Len640[0])             /* 临时全局数组指针 长度640 INT32 */

#define MED_AEC_OFFSET_THD              (60)                                    /* 延迟补偿阈值 */
#define MED_AEC_MAX_OFFSET              (960)                                   /* 最大补偿长度，单位采样点 Q0*/
#define MED_AEC_MAX_TAIL_LEN            (960)                                   /* 最大尾端长度，单位ms，最大支持60ms尾端延迟 Q0*/

#define MED_AEC_MAX_DTD_ST_MODI_THD     (5000)                                  /* DTD单讲修正级别最大值 Q0*/
#define MED_AEC_MAX_DTD_DT_MODI_THD     (30000)                                 /* DTD双讲修正级别最大值Q15*/
#define MED_AEC_MAX_DTD_POWER_THD       (30000)                                 /* DTD能量级别最大值 Q0*/
#define MED_AEC_MAX_NLP_DT2ST_THD       (20)                                    /* NLP双讲切换单讲级别最大值 Q0*/
#define MED_AEC_MAX_NLP_ST2DT_THD       (20)                                    /* NLP单讲切换双讲级别最大值 Q0*/
#define MED_AEC_MAX_NLP_CNG_THD         (2000)                                  /* NLP插入舒适噪声级别最大值 Q0*/
#define MED_AEC_MAX_NLP_NOISE_FLOOR_THD (256)                                   /* NLP本底噪声级别最大值 Q0*/
#define MED_AEC_MAX_NLP_MAX_SUPPS_LVL   (32767)                                 /* NLP高频抑制级别最大值 Q15*/
#define MED_AEC_MAX_NLP_NON_LINEAR_THD  (5000)                                  /* NLP高频非线性抑制门限最大值 Q0*/
#define MED_AEC_MAX_DTD_EST_POWER_THD   (30000)                                 /* 增强单讲使能时能量阈值最大值 Q0*/
#define MED_AEC_MAX_DTD_EST_AMPL_THD    (30000)                                 /* 增强单讲使能时幅度阈值最大值 Q0*/
#define MED_AEC_DTD_EST_POWER_BASE      (1000)                                  /* 增强单讲使能时能量阈值单位值 Q0*/
#define MED_AEC_DTD_EST_HANG_OVER_LEN   (4)                                     /* 增强单讲使能时hangover帧长度 Q0*/

#define MED_AEC_MAX_DTD_NLINE_NEAR_FAR_RATIO_GAIN   (32765)                     /* 非线性DTD远端功率谱密度的增益最大值 Q11 */
#define MED_AEC_MAX_DTD_NLINE_SP_SER_THD            (32765)                     /* 非线性DTD语音存在概率为1的SER门限最大值 Q11 */
#define MED_AEC_MAX_DTD_NLINE_ECHO_SER_THD          (32765)                     /* 非线性DTD回声存在概率为0的SER门限最大值 Q11 */
#define MED_AEC_MAX_DTD_NLINE_BAND_PSD_MUTE_THD     (32765)                     /* 非线性DTD近端功率谱密度判为静音的门限最大值 Q0 */

#define MED_AEC_MAX_DTD_NLINE_SP_THD_INIT           (32000)                     /* 非线性DTD双讲判定语音存在概率阈值的最大值 Q15 */
#define MED_AEC_MAX_DTD_NLINE_SP_THD_MAX            (32000)                     /* 非线性DTD双讲判定语音存在概率阈值最大值的最大值 Q15 */
#define MED_AEC_MAX_DTD_NLINE_SP_THD_MIN            (32000)                     /* 非线性DTD双讲判定语音存在概率阈值最小值的最大值 Q15 */
#define MED_AEC_MAX_DTD_NLINE_SUM_PSD_THD           (32000)                     /* 非线性DTD双讲判定剩余能量阈值最大值的最大值 Q15 */

#define MED_AEC_MAX_NLP_OVERDRIVE_FAR_CNT           (30000)                     /* 远端信号计数最大值  Q0 */
#define MED_AEC_MAX_NLP_OVERDRIVE_FAR_THD           (30000)                     /* 远端信号计数幅度最大值  Q0 */
#define MED_AEC_MAX_NLP_OVERDRIVE_MAX               (32000)                     /* OVERDRIVE最大值  Q0 */
#define MED_AEC_MAX_NLP_STSUPPRESS_ALPH             (32000)                     /* OVERDRIVE最大值  Q0 */
#define MED_AEC_MAX_NLP_STSUPPRESS_POWTHD           (10000)                     /* OVERDRIVE最大值  Q0 */
#define MED_AEC_MAX_NLP_SMOOTH_GAIN_DOD             (32767)                     /* 增益因子的幂指数的最大值*/
#define MED_AEC_MAX_NLP_BANDSORT_IDX                (60)                        /*  NLP: 参与排序的最大数 */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* AF FFT长度 */
enum MED_AEC_AF_FFT_LEN_ENUM
{
#ifdef SPEECH_STREAM_UNIT_128
    MED_AEC_AF_FFT_LEN_NB = 256,                                                /* 窄带FFT长度 */
    MED_AEC_AF_FFT_LEN_WB = 512,                                                /* 宽带FFT长度 */
#else
    MED_AEC_AF_FFT_LEN_NB = 240,                                                /* 窄带FFT长度 */
    MED_AEC_AF_FFT_LEN_WB = 480,                                                /* 宽带FFT长度 */
#endif
    MED_AEC_AF_FFT_LEN_BUTT
};
typedef VOS_INT16  MED_AEC_AF_FFT_LEN_ENUM_INT16;

/* NLP FFT长度 */
enum MED_AEC_NLP_FFT_LEN_ENUM
{
    MED_1MIC_AEC_NLP_FFT_LEN_NB = 256,                                          /* 1MIC窄带FFT长度 */
    MED_1MIC_AEC_NLP_FFT_LEN_WB = 512,                                          /* 1MIC宽带FFT长度 */
    MED_2MIC_AEC_NLP_FFT_LEN_NB = 320,                                          /* 2MIC窄带FFT长度 */
    MED_2MIC_AEC_NLP_FFT_LEN_WB = 640,                                          /* 2MIC宽带FFT长度 */
    MED_AEC_NLP_FFT_LEN_BUTT
};
typedef VOS_INT16  MED_AEC_NLP_FFT_LEN_ENUM_INT16;

/* DTD检测标志 */
enum MED_AEC_DTD_FLAG_ENUM
{
    MED_AEC_DTD_FLAG_ST,                                                        /* 单讲 */
    MED_AEC_DTD_FLAG_DT,                                                        /* 双讲 */
    MED_AEC_DTD_FLAG_PASS,                                                      /* 全通 */
    MED_AEC_DTD_FLAG_BUTT
};
typedef VOS_INT16 MED_AEC_DTD_FLAG_ENUM_INT16;

/* 开关量枚举 */
enum CODEC_SWITCH_ENUM
{
	CODEC_SWITCH_OFF = 0,
	CODEC_SWITCH_ON,
	CODEC_SWITCH_BUTT
};
typedef VOS_UINT16 CODEC_SWITCH_ENUM_UINT16;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
#ifdef DEBUG_AEC
static VOS_VOID fprint_vec_int32(FILE *fd, VOS_INT32 *buf, VOS_INT32 len)
{
    for (VOS_INT32 i = 0; i < len - 1; i++) {
        fprintf(fd, "%d ", buf[i]);
    }
    fprintf(fd, "%d\n", buf[len - 1]);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

