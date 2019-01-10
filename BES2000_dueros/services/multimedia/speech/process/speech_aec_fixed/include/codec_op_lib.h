
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
//#include "CodecInterface.h"
#include "ucom_comm.h"
#include "codec_typedefine.h"
#include "codec_op_etsi.h"
#include "codec_op_netsi.h"
#include "codec_op_vec.h"


#ifndef __CODEC_COM_LIB_H__
#define __CODEC_COM_LIB_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  2.1 基本宏定义
*****************************************************************************/
#define CODEC_OP_INT16_MAX            ((VOS_INT16)32767)                          /* 16bit 有符号数最大值 */
#define CODEC_OP_INT16_MIN            ((VOS_INT16)-32768)                         /* 16bit 有符号数最小值 */
#define CODEC_OP_INT32_MAX            ((VOS_INT32)0x7fffffff)                     /* 32bit 有符号数最大值 */
#define CODEC_OP_INT32_MIN            ((VOS_INT32)0x80000000)                     /* 32bit 有符号数最小值 */
/*****************************************************************************
  2.2 内存操作转定义
*****************************************************************************/

/*****************************************************************************
  2.3 ETSI/ITU/自定义运算函数转定义
*****************************************************************************/
#define CODEC_OpAdd(shwA, shwB)           add(shwA, shwB)
#define CODEC_OpSub(shwA, shwB)           sub(shwA, shwB)
#define CODEC_OpAbs_s(shwA)               abs_s(shwA)
#define CODEC_OpShl(shwA, shwN)           shl(shwA, shwN)
#define CODEC_OpShr(shwA, shwN)           shr(shwA, shwN)
#define CODEC_OpMult(shwA, shwB)          mult(shwA, shwB)
#define CODEC_OpNegate(shwA)              negate(shwA)
#define CODEC_OpExtract_h(swA)            extract_h(swA)
#define CODEC_OpExtract_l(swA)            extract_l(swA)
#define CODEC_OpRound(swA)                round_etsi(swA)
#define CODEC_OpL_Comp(shwA, shwB)        L_Comp(shwA, shwB)
#define CODEC_OpL_mac(swC, shwA, shwB)    L_mac(swC, shwA, shwB)
#define CODEC_OpL_msu(swC, shwA, shwB)    L_msu(swC, shwA, shwB)
#define CODEC_OpL_macNs(swC, shwA, shwB)  L_macNs(swC, shwA, shwB)
#define CODEC_OpL_msuNs(swC, shwA, shwB)  L_msuNs(swC, shwA, shwB)
#define CODEC_OpL_add(swA, swB)           L_add(swA, swB)
#define CODEC_OpL_sub(swA, swB)           L_sub(swA, swB)
#define CODEC_OpL_add_c(swA, swB)         L_add_c(swA, swB)
#define CODEC_OpL_sub_c(swA, swB)         L_sub_c(swA, swB)
#define CODEC_OpL_abs(swA)                L_abs (swA)
#define CODEC_OpShr_r(shwA, shwN)         shr_r(shwA, shwN)
#define CODEC_OpL_shl(swA, shwN)          L_shl(swA, shwN)
#define CODEC_OpL_shr(swA, shwN)          L_shr(swA, shwN)
#define CODEC_OpL_shr_r(swA, shwN)        L_shr_r(swA, shwN)
#define CODEC_OpMult_r(shwA, shwB)        mult_r(shwA, shwB)
#define CODEC_OpL_mult(shwA, shwB)        L_mult(shwA, shwB)
#define CODEC_OpL_mult0(shwA, shwB)       L_mult0(shwA, shwB)
#define CODEC_OpMac_r(swC, shwA, shwB)    mac_r(swC, shwA, shwB)
#define CODEC_OpMsu_r(swC, shwA, shwB)    msu_r(swC, shwA, shwB)
#define CODEC_OpL_negate(swA)             L_negate(swA)
#define CODEC_OpL_deposit_l(shwA)         L_deposit_l(shwA)
#define CODEC_OpL_deposit_h(shwA)         L_deposit_h(shwA)
#define CODEC_OpSaturate(swA)             saturate(swA)
#define CODEC_OpNorm_s(shwA)              norm_s(shwA)
#define CODEC_OpNorm_l(swA)               norm_l(swA)
#define CODEC_OpDiv_s(shwNum, shwDenom)                       div_s(shwNum, shwDenom)
#define CODEC_OpDiv_32(swNum, shwDenomH, shwDenomL)           Div_32(swNum, shwDenomH, shwDenomL)
#define CODEC_OpMac_32(swC, shwAHi, shwALo, shwBHi, shwBLo)   Mac_32(swC, shwAHi, shwALo, shwBHi, shwBLo)
#define CODEC_OpMac_32_16(swC, shwAHi, shwALo, shwN)          Mac_32_16(swC, shwAHi, shwALo, shwN)
#define CODEC_OpMpy_32(shwAHi, shwALo, shwBHi, shwBLo)        Mpy_32(shwAHi, shwALo, shwBHi, shwBLo)
#define CODEC_OpMpy_32_16(shwAHi, shwALo, shwN)               Mpy_32_16(shwAHi, shwALo, shwN)
#define CODEC_OpL_mpy_32_16(swA, shwB)                        L_mpy_ls(swA, shwB)
#define CODEC_OpMax(shwA, shwB)      \
    (((VOS_INT16)(shwA)>(VOS_INT16)(shwB))?(VOS_INT16)(shwA):(VOS_INT16)(shwB))
#define CODEC_OpL_max(swA, swB)        \
    (((VOS_INT32)(swA)>(VOS_INT32)(swB))?(VOS_INT32)(swA):(VOS_INT32)(swB))
#define CODEC_OpMin(shwA, shwB)      \
    (((VOS_INT16)(shwA)<(VOS_INT16)(shwB))?(VOS_INT16)(shwA):(VOS_INT16)(shwB))
#define CODEC_OpL_min(swA, swB)        \
    (((VOS_INT32)(swA)<(VOS_INT32)(swB))?(VOS_INT32)(swA):(VOS_INT32)(swB))
#define CODEC_OpMod32(swA, swB)        (((VOS_INT32)(swA)) % ((VOS_INT32)(swB)))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_INT32 CODEC_OpL_Exp(
                       VOS_INT16               shwExponent,
                       VOS_INT16               shwQn1,
                       VOS_INT16               shwQnOut );
extern VOS_INT32 CODEC_OpL_Exp2(
                       VOS_INT16               shwExponent,
                       VOS_INT16               shwFraction,
                       VOS_INT16               shwQn);
extern VOS_INT32 CODEC_OpL_Integral( VOS_INT16 shwX );
extern VOS_INT32 CODEC_OpL_Power(
                       VOS_INT16               shwBottom,
                       VOS_INT16               shwQn1,
                       VOS_INT16               shwPower,
                       VOS_INT16               shwQn2,
                       VOS_INT16               shwQnOut);
extern VOS_INT16 MED_PRECISE_OpLog2(
                       VOS_INT16               shwAmIn,
                       VOS_INT16               shwQn);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_com_lib.h */
