

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
//#include "codec_com_codec.h"
#include "med_fft.h"

//#include "codec_op_etsi.h"
//#include "codec_op_lib.h"
//#include "codec_op_netsi.h"
//#include "codec_op_float.h"
//#include "codec_com_codec.h"
#include "med_pp_comm.h"
#include "med_aec_comm.h"

#ifndef __MED_AEC_HF_NLP_H__
#define __MED_AEC_HF_NLP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 访问全局变量 */
#define MED_AEC_HF_GetNlpHannWin()               (&g_ashwMedAecHfNlpHannWinNb[0])/* 窄带下NLP时域汉宁窗 */
#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_HF_NLP_FRM_LEN_NB                (128)                          /* 窄带帧长 */
#define MED_AEC_HF_NLP_FFT_LEN_NB                (256)                          /* 窄带FFT长度 */
#define MED_AEC_HF_NLP_FRM_LEN_WB                (256)                          /* 宽带帧长 */
#else
#define MED_AEC_HF_NLP_FRM_LEN_NB                (120)                          /* 窄带帧长 */
#define MED_AEC_HF_NLP_FFT_LEN_NB                (240)                          /* 窄带FFT长度 */
#define MED_AEC_HF_NLP_FRM_LEN_WB                (240)                          /* 宽带帧长 */
#endif
#define MED_AEC_HF_NLP_CN_ALPHA                  (8192)                         /* 近端功率谱的滤波系数, Q15, 0.25 */
#define MED_AEC_HF_NLP_CN_ONE_SUB_ALPHA          (29491)                        /* 近端功率谱的滤波系数, Q15, 0.9 */
#define MED_AEC_HF_NLP_CN_STEP                   (9830)                         /* 噪声功率估计的调整步长, Q15, 0.3 */
#define MED_AEC_HF_NLP_CN_RAMP                   (33)                           /* 噪声功率估计的爬坡系数, Q15, 0.001 */
#define MED_AEC_HF_NLP_CN_NEARPSD_INIT           (1)                            /* 近端功率谱初始值 */
#define MED_AEC_HF_NLP_CN_PSD_INIT_NB            (98)                           /* 窄带功率谱初始值 */
#define MED_AEC_HF_NLP_CN_PSD_INIT_WB            (24)                           /* 宽带功率谱初始值 */

#define MED_AEC_HF_CANCEL_COUNT_THD              (25)                           /* 帧计数器门限 */

/* SmoothPara */
#define MED_AEC_HF_NLP_SMOOTH_PARA_POW_THD       (1465)                         /* 平滑能量阈值 150000000/FFT_Len^2*/
#define MED_AEC_HF_NLP_SMOOTH_PARA_FAST          (13107)                        /* 快速滤波系数  0.4 Q15*/
#define MED_AEC_HF_NLP_SMOOTH_PARA_SLOW          (29491)                        /* 慢速滤波系数 0.9 Q15*/
#define MED_AEC_HF_NLP_SMOOTH_PARA_SLOW_SUB      (3276)                         /* 慢速滤波系数和1的差 0.1 Q15*/
#define MED_AEC_HF_NLP_SMOOTH_PARA_GAMMA_CNT_MIN (0)                            /* Gamma计数最小值 */
#define MED_AEC_HF_NLP_SMOOTH_PARA_GAMMA_CNT_MAX (5)                            /* Gamma计数最大值 */

#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_HF_ECHO_BIN_START                (10)                           /* 窄带参与的回声块起始位置 */
#define MED_AEC_HF_ECHO_BIN_RANGE_NB             (50)                           /* 窄带参与排序的回声块长度 */
#define MED_AEC_HF_ECHO_BIN_RANGE_WB             (98)                          /* 宽带参与排序的回声块长度 */
#else
#define MED_AEC_HF_ECHO_BIN_START                (9)                           /* 窄带参与的回声块起始位置 */
#define MED_AEC_HF_ECHO_BIN_RANGE_NB             (47)                           /* 窄带参与排序的回声块长度 */
#define MED_AEC_HF_ECHO_BIN_RANGE_WB             (91)                          /* 宽带参与排序的回声块长度 */
#endif
#define MED_AEC_HF_SORT_MIN                      (24575)                        /* SortQ的最小门限 Q15*/
#define MED_AEC_HF_SORT_HIGH                     (29490)                        /* SortQ的高门限 Q15*/
#define MED_AEC_HF_SORT_LOW                      (26214)                        /* SortQ的中门限 Q15*/

#define MED_AEC_HF_OVRD_MINCTR_MAX               (65534)
#define MED_AEC_HF_COHED_MEAN_HIGH               (32112)                        /* cohedMean的高门限 Q15*/
#define MED_AEC_HF_COHED_MEAN_LOW                (31129)                        /* cohedMean的低门限 Q15*/

#define MED_AEC_HF_PREAVG_LOW                    (19660)                        /* PrefAvgLow的门限 Q15*/
//#define MED_AEC_HF_OVRD_MIN                      (6141)                         /* ovrd的最小值 Q11*/
#define MED_AEC_HF_OVRD_SUB                      (4096)                         /* ovrd的减2 Q11*/
//#define MED_AEC_HF_OVRD_LOW                      (29490)                        /* ovrd滤波低门限 Q15*/
//#define MED_AEC_HF_OVRD_HIGH                     (31130)//(32439)                        /* ovrd滤波高门限 Q15*/
#define MED_AEC_HF_OVRD_MULT                     (209709)                       /* Mult被除数高值 Q15*/
#define MED_AEC_HF_OVRD_HALF_MULT                (MED_AEC_HF_OVRD_MULT>>1)      /* Mult被除数低值 Q15*/
#define MED_AEC_HF_OVRD_LOG_CONST                (705)                          /* 对数门限 Q10*/
//#define MED_AEC_HF_OVRD_LOG_DIVED                (5120)                         /* 对数的被除数常量 Q10*/

#define MED_AEC_HF_NLP_ERRPOW_FAC                (17203)                        /* 1.05->17203(Q14),残差信号能量门限系数 */
#define MED_AEC_HF_NLP_NEARPOW_FAC               (20429)                        /* 19.95->20429(Q10),近场信号能量门限系数 */
#define MED_AEC_HF_NLP_DYNAMICSHIFT_TH           (536870912)                    /* 2^29=536870912,动态移位门限1073741824 */

#define MED_AEC_HF_NLP_SMOOTHGAIN_GAMMA          (13107)                        /* 0.4->13107(Q15)*/

#define MED_AEC_HF_NLP_INSERTCN_Q30_MAX          (1073676289)                   /* (1073741824) */
#define MED_AEC_HF_NLP_INSERTCN_Q13_MAX          (8192)
#define MED_AEC_HF_NLP_INSERTCN_DYNAMICSHIFT_TH  (536870912)                    /* 2^29=536870912,动态移位门限1073741824*/
#define MED_AEC_HF_NLP_INSERTCN_NORMALIZE_NB     (204)                          /* 1/161->204(Q15),能量归一化 */
#define MED_AEC_HF_NLP_INSERTCN_NORMALIZE_WB     (102)                          /* 1/321->102(Q15),能量归一化 */

#define MED_AEC_HF_SHW_MAX                       (32767)                        /* 16s的最大值 Q15*/
#define MED_AEC_HF_SW_MAX                        (2147483647)                   /* 32s的最大值 Q15*/

#ifdef SPEECH_STREAM_UNIT_128
#define MED_AEC_HF_NLP_RESDPOW_LOWBAND_8k        (13)                           /* 窄带残差信号能量统计起始频点 */
#define MED_AEC_HF_NLP_RESDPOW_HIGHBAND_8k       (34)                           /* 窄带残差信号能量统计截止频点 */
#define MED_AEC_HF_NLP_RESDPOW_LOWBAND_16k       (24)                           /* 宽带残差信号能量统计起始频点 */
#define MED_AEC_HF_NLP_RESDPOW_HIGHBAND_16k      (66)                           /* 宽带残差信号能量统计截止频点 */
#define MED_AEC_HF_NLP_REFRPOW_THD               (16384)                        /* 远端参考信号能量门限 */
#else
#define MED_AEC_HF_NLP_RESDPOW_LOWBAND_8k        (12)                           /* 窄带残差信号能量统计起始频点 */
#define MED_AEC_HF_NLP_RESDPOW_HIGHBAND_8k       (31)                           /* 窄带残差信号能量统计截止频点 */
#define MED_AEC_HF_NLP_RESDPOW_LOWBAND_16k       (22)                           /* 宽带残差信号能量统计起始频点 */
#define MED_AEC_HF_NLP_RESDPOW_HIGHBAND_16k      (61)                           /* 宽带残差信号能量统计截止频点 */
#define MED_AEC_HF_NLP_REFRPOW_THD               (16384)                        /* 远端参考信号能量门限 */
#endif

#define MED_AEC_HF_NLP_COMP_LOW                  (6554)                         /* 0.2(Q15) */
#define MED_AEC_HF_NLP_VAD_FAR_HANGOUVER         (3)

#define MED_AEC_HF_SMOOTH_GAIN_TH                (1000)
#define MED_AEC_HF_SMOOTH_HIGH_FREQ_TH           (20)
#define MED_AEC_HF_SMOOTH_HI_GAIN_FREQ_TH        (25)
#define MED_AEC_HF_SMOOTH_HI_GAIN_FREQ_NUM       (40)
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
/* 窄带下NLP时域汉宁窗 */
extern VOS_INT16 g_ashwMedAecHfNlpHannWinNb[MED_AEC_HF_NLP_FRM_LEN_NB * 2];

/* 宽带下NLP时域汉宁窗 */
extern VOS_INT16 g_ashwMedAecHfNlpHannWinWb[MED_AEC_HF_NLP_FRM_LEN_WB * 2];

extern VOS_INT16 g_ashwWeightNB[];
extern VOS_INT16 g_ashwWeightWB[];
extern VOS_INT16 g_ashwFreqNB[];
extern VOS_INT16 g_ashwFreqWB[];
extern VOS_INT16 g_ashwCoeff[];

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_SMOOTH_GAIN_STRU
 功能描述  : 描述施加增益结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwRefCntThd;                           /* NLP: 远端信号计数门限 */
    VOS_INT16                           shwRefCnt;                              /* NLP: 远端信号计数 */
    VOS_INT16                           shwRefAmp1;                             /* NLP: 远端信号能量门限1，用于抑制第一声回声 */
    VOS_INT16                           shwExOverdrive;                         /* NLP: 加强的Overdrive */
    VOS_INT16                           shwOvrdod;                              /* 增益因子的幂指数  Q10*/
    VOS_INT16                           shwReserve;                            
    VOS_INT16                          *pashwWeight;
    VOS_INT16                          *pashwFreq;
}MED_AEC_HF_NLP_SMOOTH_GAIN_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_TRANS_STRU
 功能描述  : 描述时频变换和频时变换状态结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                          *pshwWin;                                /* 汉宁窗的指针 */
    VOS_INT16                           shwFrmLen;                              /* 帧长 */
    VOS_INT16                           shwFftLen;                              /* NLP FFT/IFFT的长度 */
    MED_FFT_NUM_ENUM_INT16              enFftNumIndex;                          /* FFT长度索引枚举 */
    MED_FFT_NUM_ENUM_INT16              enIfftNumIndex;                         /* IFFT长度索引枚举 */
    VOS_INT16                           ashwLastResdFrm[MED_AEC_HF_NLP_FRM_LEN_WB];/* 上一帧残差时域信号 */
    VOS_INT16                           ashwNearFreq[MED_AEC_MAX_FFT_LEN];      /* 近端频域信号 */
    VOS_INT16                           ashwResdFreq[MED_AEC_MAX_FFT_LEN];      /* 残差频域信号 */
    VOS_INT16                           ashwRefrFreq[MED_AEC_MAX_FFT_LEN];      /* 远端频域信号 */
    VOS_INT16                           ashwResdFrm[MED_AEC_MAX_FFT_LEN];       /* 上一帧和当前帧残差时域信号 */
} MED_AEC_HF_NLP_TRANS_STRU;


typedef struct
{
    VOS_INT16                           ashwRefrWFreq[MED_AEC_HF_AF_M][MED_AEC_MAX_FFT_LEN];
} MED_AEC_HF_NLP_PART_DELAY_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_NLP_SMOOTH_PSD_STRU
 功能描述  : 描述功率谱和相干系数的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32                           aswSee[1 + (MED_AEC_MAX_FFT_LEN / 2)];   /* 残差自功率谱 */
    VOS_INT32                           aswSdd[1 + (MED_AEC_MAX_FFT_LEN / 2)];   /* 近端信号自功率谱 */
	VOS_INT32                           aswSddOrg[1 + (MED_AEC_MAX_FFT_LEN / 2)];/* 滤波前近端信号自功率谱 */
    VOS_INT32                           aswSxx[1 + (MED_AEC_MAX_FFT_LEN / 2)];   /* 远端信号自功率谱 */
    VOS_INT32                           aswSxd[MED_AEC_MAX_FFT_LEN];             /* 远端信号和近端信号互功率谱 */
    VOS_INT32                           aswSed[MED_AEC_MAX_FFT_LEN];             /* 残差和近端信号互功率谱 */
    VOS_INT16                           ashwcohed[1 + (MED_AEC_MAX_FFT_LEN / 2)];/* 残差和近端信号相干系数 */
    VOS_INT16                           ashwcohxd[1 + (MED_AEC_MAX_FFT_LEN / 2)];/* 远端和近端信号相干系数 */
    VOS_INT16                           ashwhnled[1 + (MED_AEC_MAX_FFT_LEN / 2)];/* 回声不相干系数 */
    VOS_INT16                           shwFFTLenth;                             /* FFT长度 */
    VOS_INT16                           shwGammaCount;
    VOS_INT16                           shwGammaT;
    VOS_INT16                           shwMaxFarSmooth;
    VOS_INT16                           shwReserve;
}MED_AEC_HF_NLP_SMOOTH_PSD_STRU;


typedef struct
{
	VOS_BOOL                           shwRefVad;
	VOS_INT16                          swVadThr;
	VOS_INT16                          shwSpeCnt;
	VOS_INT16                          shwSilCnt;
	VOS_INT16                          shwSpe2Sil;
	VOS_INT16                          shwSil2Spe;
}MED_AEC_HF_NLP_REF_VAD_STRU;

typedef struct
{
	VOS_BOOL                           shwDtd;
	VOS_INT16                          shwBinStart;
	VOS_INT16                          shwEchoRange;
	VOS_INT16                          shwHangoverInMax;
	VOS_INT16                          shwHangoverOutMax;
	VOS_INT16                          shwHangoverOutMax2;
	VOS_INT16                          shwDtdCountMax;
	VOS_INT16                          shwHangoverIn;
	VOS_INT16                          shwHangoverOut;
	VOS_INT16                          shwDtdCount;
	VOS_FLOAT                          shwGammaUp;
	VOS_FLOAT                          shwGammaDown;
	VOS_FLOAT                          shwThrRatioMax;
	VOS_FLOAT                          shwThrAbs;
	VOS_FLOAT                          shwThrMin;
}MED_AEC_HF_NLP_DTD_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_BAND_SORT_STRU
 功能描述  : 描述MED_AEC_HF模块BandSort的结构体
*****************************************************************************/
typedef struct
{

    VOS_INT16                           shwFFTLenth;
    VOS_INT16                           shwEchoLeng;                            /* 参与的回声块长度 */
    VOS_INT16                           shwEchoRange;                           /* 参与的回声块长度 */
    VOS_INT16                           shwCohedMean;                           /* 加窗残差和加窗近端的相干系数adhnled在指定频点范围内的均值Q15*/
    VOS_INT16                           shwHnlSortQ;                            /* 加窗远端和加窗近端的不相干系数(1-adcohxd)在指定频点范围内的均值  Q15*/
    VOS_INT16                           shwHnlPrefAvg;                          /* 回声不相干系数adhnled升序排序后结果的一般分段点对应数值  Q15*/
    VOS_INT16                           shwHnlPrefAvgLow;                       /* 回声不相干系数adhnled升序排序后结果的低分段点对应数值  Q15*/
    VOS_INT16                           shwHnlIdx;                              /* 回声不相干系数adhnled升序排序后结果的一般分段点Idx*/
    VOS_INT16                           shwHnlIdxLow;                           /* 回声不相干系数adhnled升序排序后结果的低分段点Idx*/
    VOS_INT16                           shwBinStart;                            /* 参与的回声块的初始Idx */
} MED_AEC_HF_NLP_BAND_SORT_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_OVER_DRIVE_STRU
 功能描述  : 描述MED_AEC_HF模块OverDrive的结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwCohxdLocalMin;                       /* dhnlSortQ的局部下限  Q15*/                           /*   */
    VOS_INT16                           shwSuppState;                           /* 回声抑制判断，0表示不抑制，1表示抑制  Q15*/
    VOS_INT16                           shwHnlLocalMin;                         /* dhnlPrefAvgLow下限  Q15*/
    VOS_INT16                           shwHnlNewMin;                           /* 回声不相干系数的条件处理控制，可能值：0/1  Q15*/
    VOS_INT16                           shwMult;                                /* 10的倍数  Q0*/
    VOS_INT16                           shwDivergeState;                        /* 回声发散指示，可能值：0/1，0表示不发散，1表示发散，即残差能量高于低端能量  Q0*/
    VOS_INT16                           shwOvrd;                                /* 标称过冲量  Q11*/
    VOS_INT16                           shwOvrdSm;                              /* 实际过冲量  Q11*/
    VOS_INT32                           swHnlMinCtr;                            /* 回声不相干系数的条件处理控制，可能值：0/1/2  Q*/
    VOS_INT32                           swHnlMin;                               /* 回声不相干系数的条件处理控制，可能值：0/1  Q*/
    VOS_INT16                           shwTargetSupp;                          /* 非线性回声抑制量 Q10 */
    VOS_INT16                           shwMinOvrd;                             /* 最小过冲量 Q11 */
    VOS_INT16                           shwOvrdHigh;
    VOS_INT16                           shwOvrdLow;
}MED_AEC_HF_NLP_OVER_DRIVE_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_CN_STRU
 功能描述  : 描述插入舒适噪声状态结构体
*****************************************************************************/
typedef struct
{
    VOS_BOOL                            enCnFlag;                               /* 插入舒适噪声使能指示 */
    VOS_INT16                           shwBinLen;                              /* 频点数 */
    VOS_INT32                           aswNearPsdBin[1 + (MED_AEC_MAX_FFT_LEN / 2)]; /* 功率谱密度 */
    VOS_INT32                           aswPsdBin[1 + (MED_AEC_MAX_FFT_LEN / 2)];/* 功率谱密度 */
    VOS_INT32                           swCancelCount;                          /* 帧计数器 */
    VOS_INT16                           shwNormalize;                           /* 噪声能量归一化系数 */
    VOS_INT16                           shwReserved;
} MED_AEC_HF_NLP_CN_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_STSUPPRE_STRU
 功能描述  : 描述单讲进一步抑制状态结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32                           swSumRefr;                              /* 远端参考信号能量 */
    VOS_INT32                           swSumResd;                              /* NLP残差信号能量 */
    VOS_INT16                           shwResdPowAlph;                         /* NLP处理后残差信号能量计算Alpha滤波系数 */
    VOS_INT16                           shwResdPowThd;                          /* NLP处理后残差信号能量门限，低于此门限进一步抑制*/
    VOS_INT16                           shwResdPowBandLow;                      /* 残差信号能量统计起始频点 */
    VOS_INT16                           shwResdPowBandHi;                       /* 残差信号能量统计截止频点 */
    VOS_BOOL                            swVadfr;                                /* 远端语音信号存在标志 */
    VOS_INT16                           swVadCount;                             /* 远端语音信号存在帧数 */
} MED_AEC_HF_NLP_STSUPPRE_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_CLIP_STRU
 功能描述  : 描述非线性修剪状态结构体
*****************************************************************************/
typedef struct
{
	VOS_FLOAT                          swGamma;
	VOS_FLOAT                          swSuppThr;
	VOS_FLOAT                          shwResdPow;
	VOS_FLOAT                          shwRefrPow;
	VOS_FLOAT                          swGainM;
} MED_AEC_HF_NLP_CLIP_STRU;

/*****************************************************************************
 实体名称  : MED_AEC_HF_NLP_CN_STRU
 功能描述  : 描述NLP状态结构体
*****************************************************************************/
typedef struct
{
    MED_AEC_HF_NLP_TRANS_STRU           stTrans;                                /* NLP时频变换和频时变换状态结构体 */
#ifdef MED_AEC_DTD_ENABLE
    MED_AEC_HF_NLP_REF_VAD_STRU         stRefVad;
#endif
    MED_AEC_HF_NLP_PART_DELAY_STRU      stPartDelay;                            /* 参考信号延时结构体 */
    MED_AEC_HF_NLP_SMOOTH_PSD_STRU      stSmoothPSD;                            /* 描述功率谱和相干系数的结构体 */
#ifdef MED_AEC_DTD_ENABLE
    MED_AEC_HF_NLP_DTD_STRU             stDtd;
#endif
    MED_AEC_HF_NLP_BAND_SORT_STRU       stBandSort;                             /* 描述MED_AEC_HF模块BandSort的结构体 */
    MED_AEC_HF_NLP_OVER_DRIVE_STRU      stOverdrive;                            /* 描述MED_AEC_HF模块OverDrive的结构体 */
    MED_AEC_HF_NLP_CN_STRU              stCn;                                   /* NLP插入舒适噪声状态结构体 */
    MED_AEC_HF_NLP_SMOOTH_GAIN_STRU     stSmoothGain;                           /* 计算增益结构体 */
    MED_AEC_HF_NLP_STSUPPRE_STRU        stSTSuppress;                           /* 单讲加强抑制结构体 */
	MED_AEC_HF_NLP_CLIP_STRU            stClip;                                 /* 非线性修剪结构体 */
} MED_AEC_HF_NLP_STRU;

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_VOID MED_AEC_HF_NlpInit(
                MED_AEC_NV_STRU                  *pstAecParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_HF_NLP_STRU              *pstHfNlp);
extern VOS_VOID MED_AEC_HF_NlpMain(
                MED_AEC_HF_NLP_STRU              *pstNlp,
                VOS_INT16                         shwDelayIdx, 
                VOS_INT32                        *pswWeight,
                VOS_INT16                        *pshwNearFrm,
                VOS_INT16                        *pshwRefrFrm,
                VOS_INT16                        *pshwResdFrm,
                VOS_INT16                        *pshwOut);
extern VOS_VOID MED_AEC_HF_NlpTime2Freq(
                MED_AEC_HF_NLP_TRANS_STRU        *pstTransObj,
                VOS_INT16                        *pshwNearFrm,
                VOS_INT16                        *pshwRefrFrm,
                VOS_INT16                        *pshwResdFrm);
extern VOS_VOID MED_AEC_HF_PartitionDelay(
                MED_AEC_HF_NLP_PART_DELAY_STRU *pstPartDelay,
                MED_INT16                       shwDelayIdx,
                MED_INT16                      *pshwRefrFreq,
                MED_INT16                       shwFftLen);
extern VOS_VOID MED_AEC_HF_NlpGamma(
                MED_AEC_HF_NLP_SMOOTH_PSD_STRU   *pstSmoothPSD,
                MED_INT32                        *paswSx);

extern VOS_VOID MED_AEC_HF_NlpSmoothPSD(
                MED_AEC_HF_NLP_SMOOTH_PSD_STRU   *pstSmoothPSD,
                MED_AEC_HF_NLP_TRANS_STRU        *pstTime2Freq, 
				VOS_INT16                         shwMaxFar);
extern VOS_VOID MED_AEC_HF_NlpBandSort(
                VOS_INT16                        *pshwCohed,
                VOS_INT16                        *pshwCohxd,
                VOS_INT16                        *pshwHnled,
                MED_AEC_HF_NLP_BAND_SORT_STRU    *pstBandSort);

extern VOS_VOID MED_AEC_HF_NlpSwap(
                VOS_INT16                        *pshwTempa,
                VOS_INT16                        *pshwTempb);

extern VOS_VOID MED_AEC_HF_NlpShortSort(
                VOS_INT16                        *pshwLow,
                VOS_INT16                        *pshwHigh);
extern VOS_VOID  MED_AEC_HF_NlpOverdrive(
                VOS_INT16                        *pshwCohxd,
                VOS_INT16                        *pshwCohed,
                MED_AEC_HF_NLP_BAND_SORT_STRU    *pstBandSort,
                MED_AEC_HF_NLP_OVER_DRIVE_STRU   *pstOverDrive,
                VOS_INT16                        *pshwHnled);
extern VOS_VOID MED_AEC_HF_NlpDivergeProc(
                VOS_INT32                        *pswErrPSD,
                VOS_INT32                        *pswNearPSD,
                VOS_INT16                        *pshwNearFreq,
                VOS_INT16                        *pshwErrFreq,
                VOS_INT16                        *shwDivergeState,
                VOS_INT32                        *pswWeight,
                VOS_INT16                         shwFftLen );
extern  VOS_VOID MED_AEC_HF_NlpSmoothGain(
                MED_AEC_HF_NLP_SMOOTH_GAIN_STRU  *pstSmoothGain,
                VOS_INT16                         shwPrefAvg,
                VOS_INT16                        *pashwAdhnled,
                VOS_INT16                         shwDovrdSm,
                VOS_INT16                         shwFftLen,
                VOS_INT16                         shwGamma,
                VOS_INT16                        *pshwErrFreq,
                VOS_INT16                         shwMaxFar,
                VOS_INT16                         shwMinOvrd);
extern  VOS_VOID MED_AEC_HF_NlpInsertCN(
                MED_AEC_HF_NLP_CN_STRU           *pstCnObj,
                VOS_INT16                         shwFftLen,
                VOS_INT16                        *pshwErrFreq,
                VOS_INT16                        *pashwAdhnled );
extern VOS_VOID MED_AEC_HF_NlpFreq2Time(
                MED_AEC_HF_NLP_TRANS_STRU        *pstTransObj,
                VOS_INT16                        *pshwResdFreq,
                VOS_INT16                        *pshwOut);
extern VOS_VOID MED_AEC_HF_NlpNoisePower(
                    MED_AEC_HF_NLP_CN_STRU       *pstCnObj,
                    VOS_INT32                    *pswSddOrg);

extern VOS_VOID MED_AEC_HF_STSuppress(
                MED_AEC_HF_NLP_STSUPPRE_STRU     *pstSTSuppress,
                VOS_INT16                         shwFftLen,
                VOS_INT16                        *pshwRefrFrm,
                VOS_INT16                        *pshwResdFreq);

extern VOS_VOID MED_AEC_HF_NlpClip(
	           MED_AEC_HF_NLP_CLIP_STRU          *pstClip,
	           VOS_INT16                         *pshwRefrFreq,
	           VOS_INT16                         *pshwResdFreq);

extern VOS_VOID MED_AEC_HF_NlpRefVad(
	           MED_AEC_HF_NLP_REF_VAD_STRU       *pstRefVad,
	           VOS_INT16                          shwFftLen,
	           VOS_INT16                         *pshwRefrFrm);

extern VOS_VOID MED_AEC_HF_NlpDtd(
               MED_AEC_HF_NLP_DTD_STRU           *pstDtd,
               VOS_BOOL                           shwVad,
               VOS_INT16                         *pshwcohed,
               VOS_INT32                         *pswSdd,
               VOS_INT32                         *pswSee);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_hf_nlp.h*/
