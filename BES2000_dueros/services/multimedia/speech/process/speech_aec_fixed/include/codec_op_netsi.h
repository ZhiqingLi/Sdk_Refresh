/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : Med_com_netsi_op.h
  版 本 号   : 初稿
  作    者   : 金剑
  生成日期   : 2011年9月1日
  最近修改   :
  功能描述   : Med_com_netsi_op.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年9月1日
    作    者   : 金剑
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "codec_op_etsi.h"
#include  "codec_typedefine.h"
#include  "ucom_comm.h"

#ifndef __CODEC_COM_NETSI_OP_H__
#define __CODEC_COM_NETSI_OP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define             CODEC_OP_LOG_OFFSET                       ((VOS_INT32)626255212) /* 9.3319 scaled as 5,26 */
#define             CODEC_OP_SQROOT_PLUS_HALF                 ((VOS_INT32)0x40000000)/* 0.5 */
#define             CODEC_OP_SQROOT_MINUS_ONE                 ((VOS_INT32)0x80000000)/* -1 */
#define             CODEC_OP_SQROOT_TERM5_MULTIPLER           ((VOS_INT16)0x5000)    /* 0.625 */
#define             CODEC_OP_SQROOT_TERM6_MULTIPLER           ((VOS_INT16)0x7000)    /* 0.875 */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 内联函数定义
*****************************************************************************/
#ifndef _MED_C89_

/*****************************************************************************
 函 数 名  : L_mult0
 功能描述  : return (long)shwVar1*shwVar2
 输入参数  : Word16 shwVar1  - 乘数1
             Word16 shwVar2  - 乘数2
 输出参数  : 无
 返 回 值  : Word32          - 乘积
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月2日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE Word32 L_mult0(Word16 shwVar1, Word16 shwVar2)
{
    Word32 swOut;

    swOut = (Word32)shwVar1 * (Word32)shwVar2;

    return (swOut);
}

/*****************************************************************************
 函 数 名  : L_mac0
 功能描述  : 16bit*16bit+32bit=32bit，16bit乘时不做移位，累加时做饱和
 输入参数  : Word32 swVar3       --累加初值
             Word16 shwVar1      --乘数
             Word16 shwVar2      --乘数
 输出参数  : 无
 返 回 值  : Word32              --返回累加值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月1日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE Word32 L_mac0(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    Word32      swVarOut;

    ae_p24x2s   p1, p2;
    ae_q56s     q1;

    /* shwVar(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* swVar3（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar3);

    /* q1(1.56)+p1(1.24)*p2(1.24) --> q1(1.56) */
    AE_MULAS56P24S_LL(q1, p1, p2);

    /* q1做48bit饱和 */
    q1 = AE_SATQ48S(q1);

    /* q1(17.48) --> swVarOut */
    swVarOut = AE_TRUNCA32Q48(q1);

    return swVarOut;

}

#endif

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
#ifdef _MED_C89_
extern Word32 L_mac0(Word32 swVar3, Word16 shwVar1, Word16 shwVar2);
UCOM_INLINE Word32 L_mult0(Word16 shwVar1, Word16 shwVar2)
{
    Word32 swOut;

    swOut = (Word32)shwVar1 * shwVar2;

    return (swOut);
}
#endif

#if 1
UCOM_INLINE Word32 L_mpy_ls(Word32 swVar1, Word16 shwVar2)
{
  Word32 swVarOut;

  swVarOut = ((Word64)swVar1 * shwVar2) >> 15;

  return (swVarOut);
}
#else
extern Word32 L_mpy_ls(Word32 swVar1, Word16 shwVar2);
#endif

extern VOS_INT16 CODEC_OpNormDiv_16 (Word16 shwNum, Word16 shwDenom, Word16 shwQn);
extern VOS_INT32 CODEC_OpExp10(VOS_INT32 swA);
extern VOS_INT32 CODEC_OpfnExp2(VOS_INT32 swInput);
extern VOS_INT32 CODEC_OpfnLog10(VOS_INT32 swInput);
extern VOS_INT32 CODEC_OpfnLog2(VOS_INT32 swInput);
extern VOS_INT16 CODEC_OpLog10(VOS_INT16 shwQ, VOS_INT32 swA);
extern VOS_INT32 CODEC_OpL_divide(VOS_INT32 swNum, VOS_INT32 swDenom);
extern VOS_INT32 CODEC_OpL_mpy_32(VOS_INT32 L_var1, VOS_INT32 L_var2);
extern VOS_INT32 CODEC_OpL_mpy_32_16_r(VOS_INT32 swA, VOS_INT16 shwB);
extern VOS_INT16 CODEC_OpMult_shr(VOS_INT16 shwA, VOS_INT16 shwB, VOS_INT16 shwShift);
extern VOS_INT32 CODEC_OpNormDiv_32(VOS_INT32 swNum, VOS_INT32 swDenom, VOS_INT16 shwQ);
extern VOS_INT16 CODEC_OpSqrt(VOS_INT32 swA);
extern VOS_INT16 CODEC_OpSqroot(VOS_INT32 swSqrtIn);

#if 0
extern VOS_INT16 CODEC_OpLog2(VOS_INT32 swX);
#else
UCOM_INLINE VOS_INT16 CODEC_OpLog2(VOS_INT32 swX)
{
    VOS_INT16  shwY = 30;
    if (swX <= 0)
    {
        if (0 == swX)
        {
            return -32768;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        shwY = shwY - norm_l(swX);
        return shwY;
    }
}
#endif







#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of Med_com_netsi_op.h */
