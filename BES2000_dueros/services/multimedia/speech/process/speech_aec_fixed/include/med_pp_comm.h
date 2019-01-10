
#ifndef _MED_PP_COMM_H_
#define _MED_PP_COMM_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"
//#include "ucom_mem_dyn.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
//#define MED_PP_GetFrameLength()                     (g_shwMedPpFrameLength)                         /* 获取当前语音帧长度 */
//#define MED_PP_SetFrameLength(shwPpFrameLen)        (g_shwMedPpFrameLength = (shwPpFrameLen))       /* 设置当前语音帧长度 */

//#define AUDIO_PP_GetFrameLength()                   (g_shwAudioPpFrameLength)                       /* 获取当前音频帧长度 */
//#define AUDIO_PP_SetFrameLength(shwAudioPpFrameLen) (g_shwAudioPpFrameLength = (shwAudioPpFrameLen))  /* 设置当前音频帧长度 */


/* NV项中各模块长度宏定义 */
#define CODEC_NV_VOL_LEN                  ((VOS_INT16)2)                                     /* NV项中音量长度 */
#define CODEC_NV_PREEMP_LEN               ((VOS_INT16)7)                                     /* NV项中预加重长度 */
#define CODEC_NV_DEEMP_LEN                ((VOS_INT16)7)                                     /* NV项中去加重长度 */
#define CODEC_NV_HPF_TX_LEN               ((VOS_INT16)17)                                    /* NV项中上行高通滤波长度 */
#define CODEC_NV_HPF_RX_LEN               ((VOS_INT16)17)                                    /* NV项中下行高通滤波长度 */
#define CODEC_NV_DEVGAIN_LEN              ((VOS_INT16)2)                                     /* NV项中设备增益长度 */
#define CODEC_NV_SIDEGAIN_LEN             ((VOS_INT16)1)                                     /* NV项中侧音增益长度 */
#define CODEC_NV_COMP_TX_LEN              ((VOS_INT16)29)                                    /* NV项中上行补偿滤波长度 */
#define CODEC_NV_COMP_RX_LEN              ((VOS_INT16)29)                                    /* NV项中下行补偿滤波长度 */
#define CODEC_NV_AEC_LEN                  ((VOS_INT16)41)                                    /* NV项中AEC长度 */
#define CODEC_NV_EANR_1MIC_LEN            ((VOS_INT16)11)                                    /* NV项中ANR_1MIC长度 */
#define CODEC_NV_ANR_2MIC_LEN             ((VOS_INT16)21)                                    /* NV项中ANR_2MIC长度 */
#define CODEC_NV_AGC_TX_LEN               ((VOS_INT16)7)                                     /* NV项中上行AGC长度 */
#define CODEC_NV_AGC_RX_LEN               ((VOS_INT16)7)                                     /* NV项中下行AGC长度 */
#define CODEC_NV_MBDRC_LEN                ((VOS_INT16)151)                                   /* NV项中MBDRC长度 */
#define CODEC_NV_AIG_TX_LEN               ((VOS_INT16)41)                                    /* NV项中AIG长度 */
#define CODEC_NV_AIG_RX_LEN               ((VOS_INT16)41)                                    /* NV项中AIG长度 */
#define CODEC_NV_AVC_LEN                  ((VOS_INT16)11)                                    /* NV项中AVC长度 */
#define CODEC_NV_VAD_TX_LEN               ((VOS_INT16)4)                                     /* NV项中上行VAD长度 */
#define CODEC_NV_VAD_RX_LEN               ((VOS_INT16)4)                                     /* NV项中下行VAD长度 */
#define CODEC_NV_PP_RESERVE_LEN           ((VOS_INT16)2)                                    /* NV项中预留数组长度 */

#define AUDIO_NV_COMP_TX_LEN              ((VOS_INT16)150)                                    /* NV项中上行补偿滤波长度 */
#define AUDIO_NV_COMP_RX_LEN              ((VOS_INT16)150)                                    /* NV项中下行补偿滤波长度 */
#define AUDIO_NV_MBDRC_LEN                ((VOS_INT16)251)                                   /* NV项中MBDRC长度 */

#define MED_PP_SHIFT_BY_1               (1)                                     /* 左移或者右移1位 */
#define MED_PP_SHIFT_BY_2               (2)                                     /* 左移或者右移2位 */
#define MED_PP_SHIFT_BY_3               (3)                                     /* 左移或者右移3位 */
#define MED_PP_SHIFT_BY_4               (4)                                     /* 左移或者右移4位 */
#define MED_PP_SHIFT_BY_5               (5)                                     /* 左移或者右移5位 */
#define MED_PP_SHIFT_BY_6               (6)                                     /* 左移或者右移6位 */
#define MED_PP_SHIFT_BY_7               (7)                                     /* 左移或者右移7位 */
#define MED_PP_SHIFT_BY_8               (8)                                     /* 左移或者右移8位 */
#define MED_PP_SHIFT_BY_9               (9)                                     /* 左移或者右移9位 */
#define MED_PP_SHIFT_BY_10              (10)                                    /* 左移或者右移10位 */
#define MED_PP_SHIFT_BY_11              (11)                                    /* 左移或者右移11位 */
#define MED_PP_SHIFT_BY_12              (12)                                    /* 左移或者右移12位 */
#define MED_PP_SHIFT_BY_13              (13)                                    /* 左移或者右移13位 */
#define MED_PP_SHIFT_BY_14              (14)                                    /* 左移或者右移14位 */
#define MED_PP_SHIFT_BY_15              (15)                                    /* 左移或者右移15位 */
#define MED_PP_SHIFT_BY_16              (16)                                    /* 左移或者右移16位 */
#define MED_PP_SHIFT_BY_17              (17)                                    /* 左移或者右移17位 */
#define MED_PP_SHIFT_BY_20              (20)                                    /* 左移或者右移20位 */
#define MED_PP_SHIFT_BY_21              (21)                                    /* 左移或者右移21位 */
#define MED_PP_SHIFT_BY_24              (24)                                    /* 左移或者右移24位 */
#define MED_PP_SHIFT_BY_25              (25)                                    /* 左移或者右移25位 */
#define MED_PP_SHIFT_BY_26              (26)                                    /* 左移或者右移26位 */
#define MED_PP_SHIFT_BY_27              (27)                                    /* 左移或者右移27位 */
#define MED_PP_SHIFT_BY_28              (28)                                    /* 左移或者右移28位 */
#define MED_PP_SHIFT_BY_29              (29)                                    /* 左移或者右移29位 */
#define MED_PP_SHIFT_BY_30              (30)                                    /* 左移或者右移30位 */
#define MED_PP_SHIFT_BY_31              (31)                                    /* 左移或者右移31位 */

#define MED_PP_MIN_PSD_VALUE            (1)                                     /* 功率谱最小值限定 */
#define NONZERO_FRM_NUM_INSERTED        (11)                                    /* 人为插入少量噪声的全零帧数 */

#define MED_SAMPLE_8K                   (8000)
#define MED_FRM_LEN_8K                  (160)
#define MED_FFT_LEN_8K                  (256)

#define MED_SAMPLE_16K                  (16000)
#define MED_FRM_LEN_16K                 (320)
#define MED_FFT_LEN_16K                 (512)

#define MED_SAMPLE_48K                  (48000)
#define MED_FRM_LEN_48K                 (960)
#define MED_FFT_LEN_48K                 (1024)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* 麦克风个数枚举 */
enum MED_PP_MIC_NUM_ENUM
{
    MED_PP_MIC_NUM_1                       = 1,
    MED_PP_MIC_NUM_2,
    MED_PP_MIC_NUM_BUTT
};
typedef VOS_UINT16 MED_PP_MIC_NUM_ENUM_UINT16;

/* 声道枚举 */
enum AUDIO_PP_CHANNEL_ENUM
{
    AUDIO_PP_CHANNEL_LEFT                  = 0,
    AUDIO_PP_CHANNEL_RIGHT,
    AUDIO_PP_CHANNEL_ALL,
    AUDIO_PP_CHANNEL_BUTT
};
typedef VOS_UINT16 AUDIO_PP_CHANNEL_ENUM_UINT16;

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
extern VOS_INT16 g_shwMedPpFrameLength;
extern VOS_INT16 g_shwAudioPpFrameLength;
extern const VOS_INT16 g_ashwMedPpBandTable1[40][2];
/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
#define MED_OBJ_HEADER                                          \
                CODEC_OBJ_USED_STATUS_ENUM_UINT16 enIsUsed;       \
                VOS_INT16                       shwReserved;

typedef struct MED_OBJ_HEADER_tag
{
    MED_OBJ_HEADER
}MED_OBJ_HEADER_STRU;

/*****************************************************************************
实体名称  : MED_OBJ_INFO
功能描述  : 结构体对象信息
*****************************************************************************/
typedef struct
{
    VOS_VOID               *pvObjArray;                                         /* 对象数组的头指针 */
    VOS_INT32               swObjNum;                                           /* 对象数组的长度   */
    VOS_INT32               swObjLen;                                           /* 对象的大小       */
}MED_OBJ_INFO;

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_UINT32 MED_PP_CheckPtrValid(
                       MED_OBJ_INFO           *pstInfo,
                       VOS_VOID               *pvTargetObj);
extern VOS_VOID* MED_PP_Create(
                       MED_OBJ_INFO           *pstInfo,
                       VOS_VOID               *pvObjArray,
                       VOS_INT32               swObjNum,
                       VOS_INT32               swObjLen);
extern VOS_VOID MED_PP_CalcPsdBin(
                       VOS_INT16              *pshwFreqBin,
                       VOS_INT16               shwFreqBinLen,
                       VOS_INT32              *pswPsdBin);
extern VOS_VOID MED_PP_CalcBandPsd(
                        VOS_INT32             *pswPsdBin,
                        VOS_INT16              shwBandLen,
                        VOS_INT32             *pswPsdBand);
extern VOS_VOID MED_PP_CalcSmoothPsd(
                        VOS_INT32             *pswPsdLast,
                        VOS_INT16              shwLen,
                        VOS_INT16              shwAlpha,
                        VOS_INT32             *pswPsd);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

