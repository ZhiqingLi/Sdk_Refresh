/**
 *******************************************************************************
 * @file    dpa.h
 * @author  Aissen Li
 * @version V1.0.0
 *
 * @brief   DPA 加速器驱动
 *
 *******************************************************************************
 */
 
 /**
 * @addtogroup DPA
 * @{
 * @defgroup dpa dpa.h
 * @{
 */
 
#ifndef __DPA__H__
#define __DPA__H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

enum
{
	DPA_ERROR_INVALID_FUNC_MODE = -256,
	DPA_ERROR_INVALID_REVERSE_MODE,
	DPA_ERROR_INVALID_SHIFT_BITS,
	DPA_ERROR_OK = 0,
};

enum
{
	DPA_FUNC_MODE_BIT_SHIFT,             //比特位移
	DPA_FUNC_MODE_BIT_SHIFT_AND,         //比特位移 & 与操作
	DPA_FUNC_MODE_BIT_SHIFT_OR,          //比特位移 & 或操作
	DPA_FUNC_MODE_BIT_SHIFT_XOR,         //比特位移 & 异或操作
	DPA_FUNC_MODE_BIT_REVERSE8,          //8位比特翻转
	DPA_FUNC_MODE_BIT_REVERSE16,         //16位比特翻转
	DPA_FUNC_MODE_BIT_REVERSE24,         //24位比特翻转
	DPA_FUNC_MODE_BIT_REVERSE32,         //32位比特翻转
	DPA_FUNC_MODE_BIT_TRANSPOSE8X8,      //8x8比特矩阵转置
	DPA_FUNC_MODE_BIT_TRANSPOSE16X16,    //16x16比特矩阵转置
};

enum
{
	DPA_REVERSE_MODE_CONTINOUS = 0,
	DPA_REVERSE_MODE_INTERLEAVE,
};

/**
 * @brief  DPA加速器配置
 * @param  FuncMode     加速器功能
 * @param  ReverseMode  翻转模式
 * @param  SrcOff       源数据偏移
 * @param  DstOff       目标数据偏移
 * @return 错误码
 */
int32_t DPA_Config(int32_t FuncMode, int32_t ReverseMode, int32_t SrcOff, int32_t DstOff);

/**
 * @brief  DPA加速器启动
 * @param  无
 * @return 无
 */
void DPA_Start();

/**
 * @brief  DPA加速器停止
 * @param  无
 * @return 无
 */
void DPA_Stop();

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__DPA__H__

/**
 * @}
 * @}
 */
