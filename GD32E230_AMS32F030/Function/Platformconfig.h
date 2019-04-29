/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : AsmPlatformconfig.h
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月20日
  最近修改   :
  功能描述   : AsmPlatformconfig.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2019年4月20日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/
#include "stm32f0xx.h"
/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#ifndef __ASMPLATFORMCONFIG_H__
#define __ASMPLATFORMCONFIG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void Display(void);
extern void DisplayScan(void);
extern int fputc(int ch, FILE *f);
extern void Pwm_WaveConfig(uint32_t Freq, uint16_t Duty);
extern void System_Systick_Init(uint32_t ticks);
extern void System_WaterPump_Control(FunctionalState NewState);
extern void WaterPump_WorkDetectInit(void);
extern uint16_t WaterPump_WorkStateDetect(void);
extern void System_WaterPump_Init(void);
extern void Usart_Debug_Init(uint32_t Baudrate);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ASMPLATFORMCONFIG_H__ */
