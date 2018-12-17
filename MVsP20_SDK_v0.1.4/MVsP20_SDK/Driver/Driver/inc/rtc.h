/**
 *****************************************************************************
 * @file     rtc.h
 * @author   Sean
 * @version  V1.0.0
 * @date     29-Nov-2016
 * @brief    rtc module driver header file
 * @maintainer: Sean
 * change log:
 *			 Add by Sean -20161129
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
/**
 * @addtogroup RTC
 * @{
 * @defgroup rtc rtc.h
 * @{
 */

#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 *  RTC Clock Source Select
 */
typedef enum _RTC_CLOCK_SEL
{
    OSC_32K = 0,/**<RTC时钟选择晶体晶振32.768kHZ，时钟频率为32768HZ*/
    OSC_12M = 1,/**<RTC时钟选择晶体晶振12MHZ并且进行分频分频，时钟频率可配*/
    RC_32K = 2  /**<RTC时钟选择RC 32.768kHZ，时钟频率为32768HZ，RC误差较大*/
} RTC_CLOCK_SEL;

/**
 *  RTC计时和闹钟结构体，Sec单位为秒，Frac远小于秒单位为时钟周期。使用时一般使用Sec即可，Frac使用场景不多直接赋0即可。
 */
typedef struct _RTC_REG_TIME_UNIT
{
    uint32_t Sec;
    uint16_t Frac;
} RTC_REG_TIME_UNIT;

 /**
  * @brief  RTC中断使能
  * @param  无
  * @return 无
  */
void RTC_IntEnable(void);

 /**
  * @brief  RTC中断禁止
  * @param  无
  * @return 无
  */
void RTC_IntDisable(void);

 /**
  * @brief  RTC中断标志清除
  * @param  无
  * @return 无
  */
void RTC_IntFlagClr(void);

 /**
  * @brief  RTC中断标志获取
  * @param  无
  * @return 无
  */
bool RTC_IntFlagGet(void);

 /**
  * @brief  RTC唤醒功能开启
  * @param  无
  * @return 无
  */
void RTC_WakeupEnable(void);

 /**
  * @brief  RTC唤醒功能关闭
  * @param  无
  * @return 无
  */
void RTC_WakeupDisable(void);

/**
 * @brief  设置RTC
 * @param  RTC值
 * @return 无
 */
void RTC_TimeSet(RTC_REG_TIME_UNIT time);

/**
 * @brief  获取RTC
 * @param  无
 * @return 当前RTC
 */
RTC_REG_TIME_UNIT RTC_TimeGet(void);

/**
 * @brief  配置Alarm
 * @param  Alarm值
 * @return 无
 */
void RTC_AlarmSet(RTC_REG_TIME_UNIT alarm);
/**
 * @brief  获取Alarm
 * @param  无
 * @return 当前Alarm
 */
RTC_REG_TIME_UNIT RTC_AlarmGet(void);


/**
 * @brief  配置RTC模块的时钟源和频率
 * @param  ClkSel 配置RTC模块的时钟源
 *	@arg OSC_32K:选择32.768kHZ晶体晶振作为RTC时钟，时钟频率为32768HZ
 *	@arg OSC_12M:选择12MHZ晶体晶振并且对其进行分频作为RTC时钟，时钟频率由用户配置，单位为HZ
 *	@arg RC_32K: 选择32.768kHZ晶体晶振作为rtc时钟，时钟频率为32768HZ,RC时钟误差较大
 * @param Freq 配置RTC模块的频率，仅选择OSC_12M时可配，单位为HZ，其他时钟源默认32768HZ传递参数32768即可
 * @return 无
 */
void RTC_ClockSrcSel(RTC_CLOCK_SEL ClkSel,uint16_t Freq);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
