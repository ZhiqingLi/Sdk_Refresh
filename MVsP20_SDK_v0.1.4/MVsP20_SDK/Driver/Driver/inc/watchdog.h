/**
 *******************************************************************************
 * @file    watchdog.c
 * @author  Sean
 * @version V1.0.0
 * @date    2016/11/14
 * @brief		watchdog driver source file
 ******************************************************************************* 
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2014 MVSilicon </center></h2>
 */

/**
 * @addtogroup WATCHDOG
 * @{
 * @defgroup watchdog watchdog.h
 * @{
 */
 
#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__
 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 *  定义看门狗的复位周期
 */
typedef enum _WDG_STEP_SEL
{
    WDG_STEP_1S = 0,/**<看门狗的复位周期:1S*/
    WDG_STEP_3S = 1,/**<看门狗的复位周期:3S*/
    WDG_STEP_4S = 2 /**<看门狗的复位周期:4S*/
} WDG_STEP_SEL;


/**
 * @brief  看门狗使能
 * @param  Mode 设置看门狗的复位周期
 *     @arg WDG_STEP_1S
 *     @arg WDG_STEP_3S
 *     @arg WDG_STEP_4S
 * @return 无
 */
void WDG_Enable(WDG_STEP_SEL Mode);


/**
 * @brief  看门狗禁止
 * @param  无
 * @return 无
 */
void WDG_Disable(void);


/**
 * @brief  喂狗
 * @param  无
 * @return 无
 */
void WDG_Feed(void);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
