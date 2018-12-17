/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：方案外置PA与喇叭控制模块相关宏定义，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _common_speaker_H
#define _common_speaker_H

#include "psp_includes.h"
#include "case_independent.h"

#define SPEAKER_A_PA_TYPE_D          0  //D类功放
#define SPEAKER_A_PA_TYPE_AB         1  //AB类功放

#define SPEAKER_A_PA_BST_EN_TIME     3  //外置PA 供电稳定时间，单位10ms
#define SPEAKER_A_PA_BYPASS_TIME     15 //外置PA BYPASS电容充电稳定时间，单位10ms

//DAC输出到模拟PA与喇叭，需要一个EN管脚控制
#define SPEAKER_A_PA_EN_GPIOOUT      GPIOAOUTEN
#define SPEAKER_A_PA_EN_GPIODATA     GPIOADAT
#if (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823)
#define SPEAKER_A_PA_EN_PIN          (1 << 21)
#else
#define SPEAKER_A_PA_EN_PIN          (1 << 5)
#endif
#define SPEAKER_A_PA_EN_LEVEL        ENABLE_LEVEL_HIGH
#define SPEAKER_A_PA_BST_EN_GPIOOUT  GPIOAOUTEN
#define SPEAKER_A_PA_BST_EN_GPIODATA GPIOADAT
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#define SPEAKER_A_PA_BST_EN_PIN      (1 << 6)
#else
#define SPEAKER_A_PA_BST_EN_PIN      (1 << 22)
#endif
#define SPEAKER_A_PA_BST_EN_LEVEL    ENABLE_LEVEL_HIGH
#define SPEAKER_A_PA_D_AB_GPIOOUT    GPIOAOUTEN
#define SPEAKER_A_PA_D_AB_GPIODATA   GPIOADAT
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#define SPEAKER_A_PA_D_AB_PIN        (1 << 21)
#else
#define SPEAKER_A_PA_D_AB_PIN        (1 << 6)
#endif
#define SPEAKER_A_PA_D_AB_LEVEL      ENABLE_LEVEL_HIGH //D类功放为高电平，AB类功放为低电平

//I2S输出到数字PA与喇叭，需要一个RESET管脚和一个EN管脚，以及I2S的MCLK，BCLK，LRCLK，DOUT 4根线
#define SPEAKER_D_PA_RESET_GPIOOUT   GPIOAOUTEN
#define SPEAKER_D_PA_RESET_GPIODATA  GPIOADAT
#define SPEAKER_D_PA_RESET_PIN       (1 << 0)
#define SPEAKER_D_PA_RESET_LEVEL     ENABLE_FIX_ACTIVE //直接拉高可以始终使能PA
#define SPEAKER_D_PA_EN_GPIOOUT      GPIOAOUTEN
#define SPEAKER_D_PA_EN_GPIODATA     GPIOADAT
#define SPEAKER_D_PA_EN_PIN          (1 << 5)
#define SPEAKER_D_PA_EN_LEVEL        ENABLE_LEVEL_HIGH


extern void SPEAKER_CONTROL_ENABLE(void) __FAR__;
extern void SPEAKER_CONTROL_DISABLE(void) __FAR__;
extern void SPEAKER_ON(void) __FAR__;
extern void SPEAKER_OFF(void) __FAR__;
extern bool IS_SPEAKER_ON(void) __FAR__;
extern void SPEAKER_D_AB_SWITCH(uint8 type) __FAR__;
extern void SPEAKER_OUT_SWITCH(uint8 hp_in) __FAR__;

#endif
