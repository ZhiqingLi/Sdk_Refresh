

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_op_cpx.h"

#ifndef __MED_FFT_H__
#define __MED_FFT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_FFT_OBJ_NUM                  (2)                                    /*FFT实体总数 */
#define MED_IFFT_OBJ_NUM                 (2)                                    /*IFFT实体总数 */

#define MED_FFT_320
#define MED_FFT_640

#define MED_FFT_Q13                      (13)                                   /* Q13 */
#define MED_FFT_Q14                      (14)                                   /* Q14 */
#define MED_FFT_Q15                      (15)                                   /* Q15 */

#ifdef MED_FFT_1024
#define MED_FFT_1024_CMPX_LEN            (512)                                  /* 1024点FFT复序列长度 */
#define MED_FFT_1024_REAL_LEN            (MED_FFT_1024_CMPX_LEN*2)              /* 1024点FFT实序列长度 */
#endif

#ifdef MED_FFT_640
#define MED_FFT_640_CMPX_LEN             (320)                                  /* 640点FFT复序列长度 */
#define MED_FFT_640_REAL_LEN             (MED_FFT_640_CMPX_LEN*2)               /* 640点FFT实序列长度 */
#endif

#ifdef MED_FFT_512
#define MED_FFT_512_CMPX_LEN             (256)                                  /* 512点FFT复序列长度 */
#define MED_FFT_512_REAL_LEN             (MED_FFT_512_CMPX_LEN*2)               /* 512点FFT实序列长度 */
#endif

#ifdef MED_FFT_320
#define MED_FFT_320_CMPX_LEN             (160)                                  /* 320点FFT复序列长度 */
#define MED_FFT_320_REAL_LEN             (MED_FFT_320_CMPX_LEN*2)               /* 320点FFT实序列长度 */
#endif

#ifdef MED_FFT_256
#define MED_FFT_256_CMPX_LEN             (128)                                  /* 256点FFT复序列长度 */
#define MED_FFT_256_REAL_LEN             (MED_FFT_256_CMPX_LEN*2)               /* 256点FFT实序列长度 */
#endif

#ifdef MED_FFT_128
#define MED_FFT_128_CMPX_LEN             (64)                                   /* 128点FFT复序列长度 */
#define MED_FFT_128_REAL_LEN             (MED_FFT_128_CMPX_LEN*2)               /* 128点FFT实序列长度 */
#endif

#if defined(MED_FFT_1024)
#define MED_FFT_BUF_LEN                  (MED_FFT_1024_REAL_LEN)                /* FFT计算内部缓冲区长度*/
#elif defined(MED_FFT_640)
#define MED_FFT_BUF_LEN                  (MED_FFT_640_REAL_LEN)                /* FFT计算内部缓冲区长度*/
#elif defined(MED_FFT_512)
#define MED_FFT_BUF_LEN                  (MED_FFT_512_REAL_LEN)                /* FFT计算内部缓冲区长度*/
#elif defined(MED_FFT_320)
#define MED_FFT_BUF_LEN                  (MED_FFT_320_REAL_LEN)                /* FFT计算内部缓冲区长度*/
#elif defined(MED_FFT_256)
#define MED_FFT_BUF_LEN                  (MED_FFT_256_REAL_LEN)                /* FFT计算内部缓冲区长度*/
#elif defined(MED_FFT_128)
#define MED_FFT_BUF_LEN                  (MED_FFT_128_REAL_LEN)                /* FFT计算内部缓冲区长度*/
#else
error("FFT length not supported");
#endif


#define MED_FFT_MAX_FACTORS              (5)                                    /* 最大基个数, 4 */

#ifdef MED_FFT_1024
#define MED_FFT_512_FACTOR_NUM           (5)                                    /* 512点基个数,4*4*4*4*2=512 */
#endif

#ifdef MED_FFT_640
#define MED_FFT_320_FACTOR_NUM           (4)                                    /* 320点基个数,5*4*4*4=320 */
#endif

#ifdef MED_FFT_512
#define MED_FFT_256_FACTOR_NUM           (4)                                    /* 256点基个数,4*4*4*4=256 */
#endif

#ifdef MED_FFT_320
#define MED_FFT_160_FACTOR_NUM           (4)                                    /* 160点基个数,5*4*4*2=160 */
#endif

#ifdef MED_FFT_256
#define MED_FFT_128_FACTOR_NUM           (4)                                    /* 128点基个数,4*4*4*2=128 */
#endif

#ifdef MED_FFT_128
#define MED_FFT_64_FACTOR_NUM            (3)                                    /* 64点基个数,4*4*4=64 */
#endif

#define MED_FFT_RADIX_TWO                (2)                                    /* 基2 */
#define MED_FFT_RADIX_FOUR               (4)                                    /* 基4 */
#define MED_FFT_RADIX_FIVE               (5)                                    /* 基5 */
#define MED_FFT_BFLY5_DIV                (5)                                    /* 基5蝶形运算时用到的除数 */
#define MED_FFT_FIX_DIV5_PARAM           (6553)                                 /* 对基5蝶形运算FixDiv函数的运算参数 */

#define MED_FFT_COS_L1                   (32767)
#define MED_FFT_COS_L2                   (-7651)
#define MED_FFT_COS_L3                   (8277)
#define MED_FFT_COS_L4                   (-626)

#define MED_FFT_COSNORM_MASK1            ((VOS_INT32)0x0001ffff)
#define MED_FFT_COSNORM_MASK2            ((VOS_INT32)0x00007fff)
#define MED_FFT_COSNORM_MASK3            ((VOS_INT32)0x0000ffff)
#define MED_FFT_COSNORM_THD1             ((VOS_INT32)0x10000)
#define MED_FFT_COSNORM_THD2             ((VOS_INT32)0x8000)
#define MED_FFT_COSNORM_CONST1           ((VOS_INT32)0x20000)
#define MED_FFT_COSNORM_CONST2           ((VOS_INT32)65536)

#define ROUND_ASYM_ENABLE                                                       /* 非对称饱和操作，即无论正数还是负数，均向正无穷四舍五入。 可提高精度 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum MED_FFT_NUM_ENUM
{
#ifdef MED_FFT_128
     MED_FFT_NUM_128 = 0,
#endif
#ifdef MED_FFT_256
     MED_FFT_NUM_256,
#endif
#ifdef MED_FFT_320
     MED_FFT_NUM_320,
#endif
#ifdef MED_FFT_512
     MED_FFT_NUM_512,
#endif
#ifdef MED_FFT_640
     MED_FFT_NUM_640,
#endif
#ifdef MED_FFT_1024
     MED_FFT_NUM_1024,
#endif
#ifdef MED_FFT_128
     MED_IFFT_NUM_128,
#endif
#ifdef MED_FFT_256
     MED_IFFT_NUM_256,
#endif
#ifdef MED_FFT_320
     MED_IFFT_NUM_320,
#endif
#ifdef MED_FFT_512
     MED_IFFT_NUM_512,
#endif
#ifdef MED_FFT_640
     MED_IFFT_NUM_640,
#endif
#ifdef MED_FFT_1024
     MED_IFFT_NUM_1024,
#endif
     MED_FFT_NUM_BUTT
};
typedef VOS_INT16 MED_FFT_NUM_ENUM_INT16;

/*****************************************************************************
 实体名称  : MED_FFT_CORE_STRU
 功能描述  : 指定点数FFT对象
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwIsInverted;                          /* 逆变换标志 */
    VOS_INT16                           shwN;                                   /* 复数FFT点数 */
    CODEC_OP_CMPX_STRU                   *pstTwiddles;                            /* 复数计算旋转因子 */
    CODEC_OP_CMPX_STRU                   *pstSuperTwiddles;                       /* 实数计算旋转因子 */
    VOS_INT16                          *pshwFactors;                            /* 基数和步长指针 */
    VOS_INT16                          *pshwSortTable;                          /* 位序重排指针 */
    VOS_INT16                           shwNumOfRadix;                          /* 基数个数 */
    VOS_INT16                           shwResvered;
}MED_FFT_CORE_STRU;

/*****************************************************************************
 实体名称  : MED_FFT_OBJ_STRU
 功能描述  : FFT实数变换实体结构体
*****************************************************************************/
typedef struct
{
#ifdef MED_FFT_128
    MED_FFT_CORE_STRU                   stFft128;                               /* 128点实数FFT变换实体  */
#endif
#ifdef MED_FFT_256
    MED_FFT_CORE_STRU                   stFft256;                               /* 256点实数FFT变换实体  */
#endif
#ifdef MED_FFT_320
    MED_FFT_CORE_STRU                   stFft320;                               /* 320点实数FFT变换实体  */
#endif
#ifdef MED_FFT_512
    MED_FFT_CORE_STRU                   stFft512;                               /* 512点实数FFT变换实体  */
#endif
#ifdef MED_FFT_640
    MED_FFT_CORE_STRU                   stFft640;                               /* 640点实数FFT变换实体  */
#endif
#ifdef MED_FFT_1024
    MED_FFT_CORE_STRU                   stFft1024;                              /* 1024点实数FFT变换实体  */
#endif
#ifdef MED_FFT_128
    MED_FFT_CORE_STRU                   stIfft128;                              /* 128点实数IFFT变换实体 */
#endif
#ifdef MED_FFT_256
    MED_FFT_CORE_STRU                   stIfft256;                              /* 256点实数IFFT变换实体 */
#endif
#ifdef MED_FFT_320
    MED_FFT_CORE_STRU                   stIfft320;                              /* 320点实数IFFT变换实体 */
#endif
#ifdef MED_FFT_512
    MED_FFT_CORE_STRU                   stIfft512;                              /* 512点实数IFFT变换实体 */
#endif
#ifdef MED_FFT_640
    MED_FFT_CORE_STRU                   stIfft640;                              /* 640点实数IFFT变换实体 */
#endif
#ifdef MED_FFT_1024
    MED_FFT_CORE_STRU                   stIfft1024;                             /* 1024点实数IFFT变换实体 */
#endif
} MED_FFT_OBJ_STRU;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/



/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/

/* 由于优化引入差异 */
#ifndef _MED_C89_

extern const VOS_INT16                  g_shwDigperm[];                         /* FFT公共的查找表 */

extern const VOS_UINT16                 g_auhwMedFft1024Twiddles[];             /* 1024阶FFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft1024Twiddles[];            /* 1024阶IFFT复数计算旋转因子 */

extern const VOS_INT32                  g_aswMedFft1024Twiddles[];              /* 1024阶FFT复数计算旋转因子 */

extern const VOS_INT32                  g_aswMedFft640TwiddlesDit[];               /* 640阶FFT复数计算旋转因子 */

extern const VOS_INT32                  g_aswMedFft512Twiddles[];               /* 512阶FFT复数计算旋转因子 */
extern const VOS_INT32                  g_aswMedFft320TwiddlesDit[];               /* 320阶FFT复数计算旋转因子 */

extern const VOS_INT32                  g_aswMedFft256Twiddles[];               /* 256阶FFT复数计算旋转因子 */
#else

extern const VOS_UINT16                 g_auhwMedFft1024Twiddles[];             /* 1024阶FFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft1024Twiddles[];            /* 1024阶IFFT复数计算旋转因子 */

extern const VOS_UINT16                 g_auhwMedFft640Twiddles[];              /* 640阶FFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft640Twiddles[];             /* 640阶IFFT复数计算旋转因子 */

extern const VOS_UINT16                 g_auhwMedFft512Twiddles[];              /* 512阶FFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft512Twiddles[];             /* 512阶IFFT复数计算旋转因子 */

extern const VOS_UINT16                 g_auhwMedFft320Twiddles[];              /* 320阶FFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft320Twiddles[];             /* 320阶IFFT复数计算旋转因子 */

extern const VOS_UINT16                 g_auhwMedFft256Twiddles[];              /* 256阶FFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft256Twiddles[];             /* 256阶IFFT复数计算旋转因子 */

#endif

/* 1024阶FFT相关 */
extern const VOS_UINT16                 g_auhwMedFft1024SuperTwiddles[];        /* 1024阶FFT实数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft1024SuperTwiddles[];       /* 1024阶IFFT实数计算旋转因子 */
extern const VOS_INT16                  g_ashwMedFft512RadixAndStride[];        /* 512点FFT基数和步长 */
extern const VOS_INT16                  g_ashwMedFft512SortTable[];             /* 512阶位序转换表 */

/* 640阶FFT相关 */
extern const VOS_UINT16                 g_auhwMedFft640SuperTwiddles[];         /* 640阶FFT实数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft640SuperTwiddles[];        /* 640阶IFFT实数计算旋转因子 */
extern const VOS_INT16                  g_ashwMedFft320RadixAndStride[];        /* 320点FFT基数和步长 */
extern const VOS_INT16                  g_ashwMedFft320SortTable[];             /* 320阶位序转换表 */

/* 512阶FFT相关 */
extern const VOS_UINT16                 g_auhwMedFft512SuperTwiddles[];         /* 512阶FFT实数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft512SuperTwiddles[];        /* 512阶IFFT实数计算旋转因子 */
extern const VOS_INT16                  g_ashwMedFft256RadixAndStride[];        /* 256点FFT基数和步长 */
extern const VOS_INT16                  g_ashwMedFft256SortTable[];             /* 256阶位序转换表 */

/* 320阶FFT相关 */
extern const VOS_UINT16                 g_auhwMedFft320SuperTwiddles[];         /* 320阶FFT实数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft320SuperTwiddles[];        /* 320阶IFFT实数计算旋转因子 */
extern const VOS_INT16                  g_ashwMedFft160RadixAndStride[];        /* 160点FFT基数和步长 */
extern const VOS_INT16                  g_ashwMedFft160SortTable[];             /* 160阶位序转换表 */

/* 256阶FFT相关 */
extern const VOS_UINT16                 g_auhwMedFft256SuperTwiddles[];         /* 256阶FFT实数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft256SuperTwiddles[];        /* 256阶IFFT实数计算旋转因子 */
extern const VOS_INT16                  g_ashwMedFft128RadixAndStride[];        /* 128点FFT基数和步长 */
extern const VOS_INT16                  g_ashwMedFft128SortTable[];             /* 128阶位序转换表 */

/* 128阶FFT相关 */
extern const VOS_UINT16                 g_auhwMedFft128Twiddles[];              /* 128阶FFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedFft128SuperTwiddles[];         /* 128阶FFT实数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft128Twiddles[];             /* 128阶IFFT复数计算旋转因子 */
extern const VOS_UINT16                 g_auhwMedIfft128SuperTwiddles[];        /* 128阶IFFT实数计算旋转因子 */
extern const VOS_INT16                  g_ashwMedFft64RadixAndStride[];         /* 64点FFT基数和步长 */
extern const VOS_INT16                  g_ashwMedFft64SortTable[];              /* 64阶位序转换表 */

/*****************************************************************************
  10 函数声明
*****************************************************************************/
#ifndef _MED_C89_
extern VOS_VOID MED_FFT_cfft_dit_160_320(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y);

extern VOS_VOID MED_FFT_cifft_dit_160_320(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y);

extern VOS_VOID MED_FFT_cfft_dif_128_256(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y);

extern VOS_VOID MED_FFT_cifft_dif_128_256(
                MED_FFT_CORE_STRU       *pstObj,
                VOS_INT16               *x,
                VOS_INT16               *y);
#endif

extern VOS_VOID MED_FFT_Bfly2(
                       MED_FFT_CORE_STRU      *pstObj,
                       CODEC_OP_CMPX_STRU     *pstSn,
                       VOS_INT16               shwStride,
                       VOS_INT16               shwStageNum,
                       VOS_INT16               shwStageLen,
                       VOS_INT16               shwNextStageNum);
extern VOS_VOID MED_FFT_Bfly4(
                       MED_FFT_CORE_STRU      *pstObj,
                       CODEC_OP_CMPX_STRU     *pstSn,
                       VOS_INT16               shwStride,
                       VOS_INT16               shwStageNum,
                       VOS_INT16               shwStageLen,
                       VOS_INT16               shwNextStageNum);
extern VOS_VOID MED_FFT_Bfly5(
                       MED_FFT_CORE_STRU      *pstObj,
                       CODEC_OP_CMPX_STRU     *pstSn,
                       VOS_INT16               shwStride,
                       VOS_INT16               shwStageNum);
extern VOS_VOID MED_FFT_Cmpx(
                       MED_FFT_CORE_STRU        *pstObj,
                       CODEC_OP_CMPX_STRU       *pstIn,
                       CODEC_OP_CMPX_STRU       *pstOut);
extern VOS_VOID MED_FFT_Fft(VOS_INT16 shwFftLenIndex, VOS_INT16 *pshwIn, VOS_INT16 *pshwOut);
extern VOS_VOID MED_FFT_FftCore(MED_FFT_CORE_STRU *pstObj,VOS_INT16 *pshwIn, VOS_INT16 *pshwOut);
extern VOS_VOID MED_FFT_Ifft(VOS_INT16 shwIfftLenIndex, VOS_INT16 *pshwIn, VOS_INT16 *pshwOut);
extern VOS_VOID MED_FFT_IfftCore(MED_FFT_CORE_STRU *pstObj,  VOS_INT16 *pshwIn, VOS_INT16 *pshwOut);
extern VOS_VOID MED_FFT_InitAllObjs(void);
extern VOS_VOID MED_FFT_SplitFreq(
                       MED_FFT_CORE_STRU        *pstObj,
                       CODEC_OP_CMPX_STRU       *pstIn,
                       CODEC_OP_CMPX_STRU       *pstOut);
extern VOS_VOID MED_FFT_UniteFreq(
                       MED_FFT_CORE_STRU        *pstObj,
                       CODEC_OP_CMPX_STRU       *pstIn,
                       CODEC_OP_CMPX_STRU       *pstOut);




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_fft.h */


