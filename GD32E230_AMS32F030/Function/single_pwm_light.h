/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : func_led.h
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月18日
  最近修改   :
  功能描述   : func_led.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2019年4月18日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/
#ifndef __FUNC_LIGHT_H__
#define __FUNC_LIGHT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "stm32f0xx_conf.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
//模块宏定义
	
#define MAX_LIGHT_PWM_CHANNEL				3 

//实际使用功能宏定义
#define PWM_LIGHT_SCAN_FREQ					500
#define USE_LIGHT_SCAN_RATIO				20
#define USE_LIGHT_SCAN_CHANNEL				3 
#define PWM_LIGHT_MAX_DUTY					60
#define PWM_LIGHT_MIN_DUTY					(PWM_LIGHT_MAX_DUTY/USE_LIGHT_SCAN_RATIO)

//PWM控制接口定义，根据硬件修改
#define PWM_LIGHT_CHANNEL_RED				PWM1_CH3_A10
#define PWM_LIGHT_OUTIO_RED					PWM_IO_SEL0
#define PWM_LIGHT_LEVEL_RED					PWM_OUTPUT_HIGH

#define PWM_LIGHT_CHANNEL_GREEN				PWM1_CH2_A9
#define PWM_LIGHT_OUTIO_GREEN				PWM_IO_SEL0
#define PWM_LIGHT_LEVEL_GREEN				PWM_OUTPUT_HIGH

#define PWM_LIGHT_CHANNEL_BLUE				PWM1_CH1_A8
#define PWM_LIGHT_OUTIO_BLUE				PWM_IO_SEL0
#define PWM_LIGHT_LEVEL_BLUE				PWM_OUTPUT_HIGH

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

extern void Light_Display(void);
extern void Light_DispInit(void);
extern void Light_DispScan(void);
extern void Light_DispStateSet(DISP_STATE_TYPEDEF State, DISP_COLOR_TYPEDEF Color, uint32_t Timer);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __FUNC_LED_H__ */
