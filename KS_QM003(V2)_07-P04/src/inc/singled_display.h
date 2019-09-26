/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : singled_display.h
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年1月23日
  最近修改   :
  功能描述   : singled_display.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2019年1月23日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "pwm.h"

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

#ifndef __SINGLED_DISPLAY_H__
#define __SINGLED_DISPLAY_H__

#define SINGLE_LED_DISP_SCAN_CNT		50
#define SINGLE_LED_MAX_DUTY				100
#define SINGLE_LED_MIN_DUTY				0

//LED显示模式
typedef enum _LED_MODE_TYPE_
{
	LED_DISPLAY_MODE_CHARGING = 0,	    //显示充电中
	LED_DISPLAY_MODE_LOWBATTER,	    	//显示低电
	LED_DISPLAY_MODE_NIGHTLAMP,	    	//显示小夜灯
	LED_DISPLAY_MODE_WPSCONNECT,	    //显示联网中
	LED_DISPLAY_MODE_TLAK_ON,	    	//显示录音中
	LED_DISPLAY_MODE_POWER_ON,	    	//显示开机中
	LED_DISPLAY_MODE_ALARM_CLOCK,	    //显示闹钟闹响中
	LED_DISPLAY_MODE_BREATHING,			//显示呼吸灯
	LED_DISPLAY_MODE_OFF,				//关闭显示。
}LED_MODE_TYPE;

typedef enum _LED_DISPLAY_STATUS_
{
	LED_DISPLAY_ONCE = 0,
	LED_DISPLAY_LOOP,
	LED_DISPLAY_KEEP
}LED_DISPLAY_STATUS;

typedef struct SINGLE_LED_DISP
{
	uint8_t CurDisplayMode;				//当前显示模式
    uint8_t BledFlag;					//利用每个bit控制LED闪烁，呼吸等
    uint8_t RledFlag;					//利用每个bit控制LED闪烁，呼吸等
    uint8_t BledDuty;					//LED当前实时占空比
    uint8_t RledDuty;					//LED当前实时占空比
    uint8_t BledStepUpDuty;				//扫描占空比步进，利用这个实现灯光呼吸/闪烁等效果
    uint8_t RledStepUpDuty;					//扫描占空比步进，利用这个实现灯光呼吸/闪烁等效果
    uint8_t DispBitCnt;					//当前BIT位指针
    uint32_t DispChangeTime;			//每调整一个亮度等级的时间，单位：ms
}SINGLE_LED_DISP_STRU;

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void LedFlushDisp(void);
extern void SingleLedFlushDispInit(void);
extern void SingleLedDisplayModeSet(uint8_t DisplayMode, bool IsOnOff);
extern bool GetSingleLedDispMode(LED_MODE_TYPE Mode);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SINGLED_DISPLAY_H__ */
