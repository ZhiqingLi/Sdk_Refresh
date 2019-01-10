

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "v_typdef.h"

#ifndef _MED_AEC_MAIN_H_
#define _MED_AEC_MAIN_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
typedef VOS_VOID (*NS_HANDLER_T)(VOS_VOID *stNs, VOS_INT16 *shwBuf, VOS_INT32 swFrameLength);

/* AEC参数初始化接口 */
typedef struct
{
    VOS_BOOL                            enEAecEnable;                           /* AEC: 增强免提AEC使能开关 */
    VOS_BOOL                            enHpfEnable;                            /* HPF: 高通滤波开关 */
    VOS_BOOL                            enAfEnable;                             /* AF : 自适应滤波开关 */
    VOS_BOOL                            enNsEnable;                             /* NS : 噪声抑制开关 */
    VOS_BOOL                            enNlpEnable;                            /* NLP: 非线性抑制开关 */
    VOS_BOOL                            enCngEnable;                            /* CNG: 舒适噪声开关 */
    VOS_INT16                           shwDelayLength;                         /* DELAY: 固定延时 */
    VOS_INT16                           shwNlpRefCnt;                           /* NLP: 远端信号计数，用于抑制第一声回声 */
    VOS_INT16                           shwNlpRefAmp1;                          /* NLP: 远端信号能量门限1，用于抑制第一声回声 */
    VOS_INT16                           shwNlpExOverdrive;                      /* NLP: 加强的Overdrive，用于抑制第一声回声 */
    VOS_INT16                           shwReserve4;                            /* 保留4 */
    VOS_INT16                           shwNlpResdPowAlph;                      /* NLP: 残差信号能量滤波系数 */
    VOS_INT16                           shwNlpResdPowThd;                       /* NLP: 残差信号能量门限 */
    VOS_INT16                           shwNlpSmoothGainDod;                    /* NLP: 增益因子的幂指数"如果小于等于0，则计算实际的幂指数；否则，幂指数由其指定" */
    VOS_INT16                           shwNlpBandSortIdx;                      /* NLP: 窄带排序的高 位置MED_AEC_HF_ECHO_BIN_RANGE*(3/4)or(1/2)-1 */
    VOS_INT16                           shwNlpBandSortIdxLow;                   /* NLP: 窄带排序的低 位置MED_AEC_HF_ECHO_BIN_RANGE*(1/2)or(3/10)-1*/
    VOS_INT16                           shwNlpTargetSupp;                       /* NLP: 目标抑制量 */
    VOS_INT16                           shwNlpMinOvrd;                          /* NLP: 最小过冲量 */
    VOS_INT16                           shwNlpOvrdHigh;                         /* NLP: 过冲下降时的平滑因子，值越小，下降越快，一般 0.9 ~ 1*/
    VOS_INT16                           shwNlpOvrdLow;                          /* NLP: 过冲上升时的平滑因子，值越小，上升越快，一般 0.9 ~ 1，并且 shwNlpOvrdLow < shwNlpOvrdHigh */
} MED_AEC_NV_STRU;

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_UINT32  MED_AEC_Main(
	                   VOS_VOID               *pAecInstance,
                       VOS_INT16              *pshwMicIn,
					   VOS_INT16              *pshwSpkIn,
                       VOS_INT16              *pshwLineOut);
extern VOS_VOID* MED_AEC_Create(void);
extern VOS_UINT32  MED_AEC_Destroy(VOS_VOID  **ppAecObj);
extern VOS_UINT32 MED_AEC_SetPara (
                       VOS_VOID                        *pAecInstance,
					   MED_AEC_NV_STRU                 *pstNv,
                       VOS_INT32                       enSampleRate);
extern VOS_UINT32  MED_AEC_SetExtenalNsHandle(
                       VOS_VOID               *pAecInstance,
                       VOS_VOID               *pstNs,
                       NS_HANDLER_T           swNsHandler);
extern VOS_UINT32  MED_AEC_GetNlpGain(
                       VOS_VOID               *pAecInstance,
                       VOS_INT16              *pshwNlpGain);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

