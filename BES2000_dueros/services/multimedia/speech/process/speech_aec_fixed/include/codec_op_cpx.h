

#ifndef _CODEC_BASIOP_CPX_H_
#define _CODEC_BASIOP_CPX_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_op_float.h"


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
  4 STRUCT定义
*****************************************************************************/

/* 复数 */
typedef struct
{
    VOS_INT16                           shwR;                                   /* 复数实部 */
    VOS_INT16                           shwI;                                   /* 复数虚部 */
} CODEC_OP_CMPX_STRU;

/* 32bit复数 */
typedef struct
{
    VOS_INT32                           swR;                                    /* 复数实部 */
    VOS_INT32                           swI;                                    /* 复数虚部 */
} CODEC_OP_CMPX32_STRU;

/*****************************************************************************
  5 全局变量声明
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

/*****************************************************************************
  9 内联函数实现
*****************************************************************************/
#ifdef _MSC_VER
#define CODEC_OpCpxAdd(ret, a, b) /*lint -e{717} */ \
    do { \
        (ret).shwR = CODEC_OpAdd( (a).shwR, (b).shwR ); \
        (ret).shwI = CODEC_OpAdd( (a).shwI, (b).shwI ); \
    }while(0)

#define CODEC_OpCpxSub(ret, a, b) /*lint -e{717} */ \
    do { \
        (ret).shwR = CODEC_OpSub( (a).shwR, (b).shwR ); \
        (ret).shwI = CODEC_OpSub( (a).shwI, (b).shwI ); \
    }while(0)
#else
#define CODEC_OpCpxAdd(ret, a, b) \
      do { \
        *((VOS_UINT32*)&(ret)) = __QADD16(*((VOS_UINT32*)&(a)), *((VOS_UINT32*)&(b))); \
      }while(0)
        
#define CODEC_OpCpxSub(ret, a, b) \
      do { \
        *((VOS_UINT32*)&(ret)) = __QSUB16(*((VOS_UINT32*)&(a)), *((VOS_UINT32*)&(b))); \
      }while(0)
#endif

/* 注意: ret不能与a或者b为相同变量 */
#ifdef _MSC_VER
#define CODEC_OpCpxMul(ret32, a, b) /*lint -e{717} */ \
    do{ \
        (ret32).swR = CODEC_OpL_sub( CODEC_OpL_mult0((a).shwR,(b).shwR), CODEC_OpL_mult0((a).shwI,(b).shwI) ); \
        (ret32).swI = CODEC_OpL_add( CODEC_OpL_mult0((a).shwR,(b).shwI), CODEC_OpL_mult0((a).shwI,(b).shwR) ); \
    }while(0)
#else
#define CODEC_OpCpxMul(ret32, a, b) /*lint -e{717} */ \
    do{ \
        (ret32).swR = __SMUSD(*((VOS_UINT32*)&(a)), *((VOS_UINT32*)&(b))); \
        (ret32).swI = __SMUADX(*((VOS_UINT32*)&(a)), *((VOS_UINT32*)&(b))); \
    }while(0)
#endif

#define CODEC_OpCpxShr_r16(ret, n) /*lint -e{717}*/\
    do{ \
        (ret).shwR = CODEC_OpShr_r((ret).shwR, n); \
        (ret).shwI = CODEC_OpShr_r((ret).shwI, n); \
    }while(0)

#define CODEC_OpCpxAdd32(ret, a, b) /*lint -e{717} */ \
    do { \
        (ret).swR = CODEC_OpL_add( (a).swR, (b).swR ); \
        (ret).swI = CODEC_OpL_add( (a).swI, (b).swI ); \
    }while(0)

#define CODEC_OpCpxSub32(ret, a, b) /*lint -e{717} */ \
    do { \
        (ret).swR = CODEC_OpL_sub( (a).swR, (b).swR ); \
        (ret).swI = CODEC_OpL_sub( (a).swI, (b).swI ); \
    }while(0)

#define CODEC_OpCpxShl32(ret, n) /*lint -e{717} */ \
    do{ \
        (ret).swR = CODEC_OpL_shl((ret).swR, n); \
        (ret).swI = CODEC_OpL_shl((ret).swI, n); \
    }while(0)

#define CODEC_OpCpxShr_r32(ret, n) /*lint -e{717} */ \
    do{ \
        (ret).swR = CODEC_OpL_shr_r((ret).swR, n); \
        (ret).swI = CODEC_OpL_shr_r((ret).swI, n); \
    }while(0)

#define CODEC_OpCpx16To32(ret32, a16) /*lint -e{717} */ \
    do{ \
        (ret32).swR = (VOS_INT32)((a16).shwR); \
        (ret32).swI = (VOS_INT32)((a16).shwI); \
    }while(0)

#define CODEC_OpCpx32To16(ret16, a32) /*lint -e{717} */ \
    do{ \
        (ret16).shwR = (VOS_INT16)CODEC_OpSaturate((a32).swR); \
        (ret16).shwI = (VOS_INT16)CODEC_OpSaturate((a32).swI); \
    }while(0)

#ifndef _MSC_VER
CODEC_INLINE CODEC_OP_CMPX_STRU CODEC_OpCpxMulQ15(
                CODEC_OP_CMPX_STRU  stA,
                CODEC_OP_CMPX_STRU  stB)
{
    CODEC_OP_CMPX_STRU ret;

    ret.shwR = CODEC_OpSaturate(CODEC_OpL_shr_r(
            __SMUSD(*((VOS_UINT32*)&(stA)), *((VOS_UINT32*)&(stB))),
            CODEC_OP_Q15));
    ret.shwI = CODEC_OpSaturate(CODEC_OpL_shr_r(
            __SMUADX(*((VOS_UINT32*)&(stA)), *((VOS_UINT32*)&(stB))),
            CODEC_OP_Q15));

    return ret;
}

CODEC_INLINE CODEC_OP_CMPX_STRU CODEC_OpCpxMulQ17(
                CODEC_OP_CMPX_STRU  stA,
                CODEC_OP_CMPX_STRU  stB)
{

    CODEC_OP_CMPX_STRU ret;

    ret.shwR = (VOS_INT16)CODEC_OpL_shr_r(
            __SMUSD(*((VOS_UINT32*)&(stA)), *((VOS_UINT32*)&(stB))),
            CODEC_OP_Q17);
    ret.shwI = (VOS_INT16)CODEC_OpL_shr_r(
            __SMUADX(*((VOS_UINT32*)&(stA)), *((VOS_UINT32*)&(stB))),
            CODEC_OP_Q17);

    return ret;
}
#else
extern CODEC_OP_CMPX_STRU CODEC_OpCpxMulQ15(
                CODEC_OP_CMPX_STRU  stA,
                CODEC_OP_CMPX_STRU  stB);

extern CODEC_OP_CMPX_STRU CODEC_OpCpxMulQ17(
                CODEC_OP_CMPX_STRU  stA,
                CODEC_OP_CMPX_STRU  stB);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of codec_op_cpx.h*/



