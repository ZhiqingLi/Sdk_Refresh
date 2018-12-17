/**
 **************************************************************************************
 * @file    i2s.h
 * @brief   I2S Module API
 *
 * @author  Cecilia Wang
 * @version V1.0.0
 *
 * $Created: 2014-10-22 15:32:38$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
 
/**
 * @addtogroup I2S
 * @{
 * @defgroup i2s i2s.h
 * @{
 */
 
#ifndef __I2S_H__
#define __I2S_H__
 
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
    
/**
 * @brief I2S支持的数据对齐格式
 */    
typedef enum _I2S_DATA_FORMAT
{
    I2S_FORMAT_RIGHT = 0,
    I2S_FORMAT_LEFT,
    I2S_FORMAT_I2S,
    I2S_FORMAT_DSPA,
    I2S_FORMAT_DSPB
} I2S_DATA_FORMAT;


/**
 * @brief I2S支持的数据位宽
 */ 
typedef enum _I2S_DATA_WORDLTH
{
    I2S_LENGTH_16BITS = 0,
    I2S_LENGTH_20BITS,
    I2S_LENGTH_24BITS,
    I2S_LENGTH_32BITS
} I2S_DATA_LENGTH;

/**
 * @brief  在DSP（A）数据对齐格式下，设置I2S进入单声道或双声道模式
 * @param  IsMono  : 1: 单声道数据传输
 *                    0: 双声道数据传输
 * @return 无
 */
void I2S_MonoModeSet(bool IsMono);

/****************************************************************************/
/*                Lrclk和Bclk时钟的分频和相位相关函数                    */
/****************************************************************************/
/**
 * @brief  设置I2S模块的Bclk时钟是否反相
 * @param  IsBclkInvert: 1:反相 0:不反相
 * @return 无
 */
void I2S_BclkInvertSet(bool IsBclkInvert);

/**
 * @brief  设置I2S模块的Bclk输出时钟频率
 * @param  BclkDiv  : Bclk分频值, 范围(0~20),只有I2S模块处于Master模式下有效
 * @param          |  0: i2s_clk     |  1: i2s_clk/1.5  |   2: i2s_clk/2  |
 * @param          |  3: i2s_clk/3   |  4: i2s_clk/4    |   5: i2s_clk/5  |
 * @param          |  6: i2s_clk/5.5 |  7: i2s_clk/6    |   8: i2s_clk/8  |
 * @param          |  9: i2s_clk/10  | 10: i2s_clk/11   |  11: i2s_clk/12 |
 * @param          | 12: i2s_clk/16  | 13: i2s_clk/20   |  14: i2s_clk/22 |
 * @param          | 15: i2s_clk/24  | 16: i2s_clk/25   |  17: i2s_clk/30 |
 * @param          | 18: i2s_clk/32  | 19: i2s_clk/44   |  20: i2s_clk/48 |
 * @return 无
 */
void I2S_BclkFreqSet(uint32_t BclkDiv);

/**
 * @brief  设置I2S模块的Lrclk时钟是否反相
 * @param  IsLrclkInvert: 1:反相 0: 不反相
 * @return 无
 */
void I2S_LrclkInvertSet(bool IsLrclkInvert);

/**
 * @brief  设置I2S模块Lrclk时钟的输出频率
 * @param  FreqDiv  : lrclk freq=bclk freq/FreqDiv,范围(8~2047),主要I2S模块处于
 *                    Master模式下时有效
 * @return NONE
 */
void I2S_LrclkFreqSet(uint32_t FreqDiv);

/****************************************************************************/
/*                Master模式和Slave模块相关函数                              */
/****************************************************************************/
/**
 * @brief  获取I2S模块当前采样率
 * @return 当前采样率: 8K/11.025K/12K/16K/22.05K/24K/32K/44.1K/48K/64K/88.2K/96K/176.4K/192K
 *         如果返回0，则错误
 */
uint32_t I2S_SampleRateGet(void);

/**
 * @brief  设置I2S模块采样率
 * @param  SampleRate: 需要设置的采样率，8K/11.025K/12K/16K/22.05K/24K/32K/44.1K/48K/64K/88.2K/96K/176.4K/192K
 * @return 无
 */ 
void I2S_SampleRateSet(uint32_t SampleRate);

/**
 * @brief  设置I2S模块进入Master模式
 * @param  I2SFormat : 数据对齐格式：right/left/i2s/dsp
 * @param  I2SWordlth: 数据位宽参数：16bits/20bits/24bits/32bits
 * @return 无
 */ 
void I2S_SetMasterMode(uint8_t I2SFormat, uint8_t I2SWordlth);

/**
 * @brief  设置I2S模块进入Slave模块
 * @param  I2SFormat : 数据对齐格式，right/left/i2s/dsp
 * @param  I2SWordlth: 数据位宽，16bits/20bits/24bits/32bits
 * @return NONE
 */ 
void I2S_SetSlaveMode(uint8_t I2SFormat, uint8_t I2SWordlth);

/**
 * @brief  设置I2S模块在PCM-Sync模式下的采样率为8KHZ
 * @return 无
 */
void I2S_PCMSyncSampleRateSet(void);

/**
 * @brief  设置I2S进入PCM-Sync模式并使能为Master模式
 * @return 无
 */
void I2S_PCMSyncSetMasterMode(void);

/**
 * @brief  设置I2S进入PCM-Sync模式并使能为Slave模式
 * @return 无
 */
void I2S_PCMSyncSetSlaveMode(void);

/****************************************************************************/
/*                使能、暂停和复位相关的函数                                 */
/****************************************************************************/

/**
 * @brief  使能I2S模块暂停或取消暂停功能
 * @param  IsPause  : 是否暂停参数，0:继续发送, 1:暂停发送
 * @return 无
 */
void I2S_ModuleTxPause(bool IsPause);

/**
 * @brief  使能I2S的发送模块
 * @param  IsEnable  : 1： 使能  0： 禁能
 * @return 无
 */
void I2S_ModuleTxEnable(bool IsEnable);

/**
 * @brief  使能I2S Dout高组模式，当I2S发送模块禁能时，Dout为高组状态
 *         当I2S发送模块使能时，如果使能高组模式，则当无效bit时处于高组状态
 *                            如果禁能高组模式，则当无效bit时处于低电平
 * @param  IsEnable  : 1： 使能  0： 禁能
 * @return 无
 */
void I2S_ModuleTxHizEnable(bool IsEnable);

/**
 * @brief  使能I2S的接收模块
 * @param  IsEnable  : 1： 使能  0： 禁能
 * @return 无
 */
void I2S_ModuleRxEnable(bool IsEnable);

/**
 * @brief  开启或关闭整个I2S模块
 * @param  IsEnable : 是否开启I2S模块，0:关闭, 1:开启
 * @return 无
 */
void I2S_ModuleEnable(uint8_t IsEnable);


/****************************************************************************/
/*                在Slave模块下采样率实时监测功能相关函数                    */
/****************************************************************************/
/**
 * @brief  使能或禁能I2S模块在Slave模式下采样率实时检测功能
 * @param  IsEnable : 是否使能，1:使能 0:禁能
 * @return 无
 */
void I2S_SampleRateCheckEnable(bool IsEnable);

/**
 * @brief  获取I2S采样率实时检测中断标志
 * @return 中断标志，1:检测完成 0:无效
 */
bool I2S_GetSampleRateCheckInt(void);

/**
 * @brief  清除I2S采样率实时检测中断标志
 * @return 无
 */
void I2S_ClrSampleRateCheckInt(void);

/****************************************************************************/
/*                       静音，淡入/淡出效果设置相关函数                     */
/****************************************************************************/
/**
 * @brief  设置I2S进入静音或非静音状态
 * @param  IsMute   : 是否静音标志，0:非静音 1:静音
 * @return 无
 */
void I2S_MuteSet(bool IsEnable);

/**
 * @brief  获取I2S模块当前静音状态
 * @return 1:静音状态 0:非静音状态
 */
bool I2S_GetMuteStatus(void);

/**
 * @brief  设置I2S模块淡入淡出的时间。
 * @param  FadeTime : 淡入淡出时间设置,单位:Ms,公式为：FadeTime = (2^12)/(Fs*(1 ~ 255)),
 *                    其中Fs为采样率，单位：KHz.
 *                    例如:1)采样率为8KHz时，FadeTime范围（2ms ~ 512ms）
 *                         2)采样率为192KHz时，FadeTime范围（1ms ~ 21ms）
 * @return 无
 */
void I2S_FadeTimeSet(uint32_t FadeTime);

/**
 * @brief  开启或关闭I2S模块淡入淡出功能
 * @param  IsEnable : 是否开启淡入淡出功能，0: 关闭 1: 开启
 * @return None
 */
void I2S_FadeEnable(bool IsEnable);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__I2S_H__

/**
 * @}
 * @}
 */
