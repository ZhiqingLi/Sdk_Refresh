/**
 *****************************************************************************
 * @file     MOTOR.h
 * @author   Grayson
 * @version  V1.0.0
 * @date     25-May-2017
 * @brief    MOTOR module driver header file
 * @maintainer: Grayson
 * change log:
 *			 Add by Grayson -20170529
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
/**
 * @addtogroup 驱动
 * @{
 * @defgroup MOTOR MOTOR
 * @{
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "type.h"
#include "timer.h"
#include "pwm.h"

/**
 * @brief      HALL换向
 *
 * @param[in]  TimerIndex  定时器索引号，支持TIMER5~TIMER8
 * @param[in]  ucHall      霍尔值
 * @param[in]  Upload      执行换向 ，1=立即换向 0=定时器周期换向
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
TIMER_ERROR_CODE Motor_HallCommutator(TIMER_INDEX TimerIndex,uint8_t ucHall,bool Upload);


/**
 * @brief      触发延时设置,延时时间到达将输出触发信号到所设置的TIMER（对电机主要用于反电动势捕获后延时换向）
 *
 * @param[in]  TimerIndex  定时器索引号，支持TIMER5~TIMER8
 * @param[in]  Val         延时时间，定时器时钟数
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
TIMER_ERROR_CODE Motor_CommutDelaySet(TIMER_INDEX TimerIndex, uint16_t Val);

/**
 * @brief      获取PWC当前计数器值（对电机即上次HALL捕获到目前为止的计数值）
 *
 * @param[in]  TimerIndex  定时器索引号，支持TIMER5~TIMER8
 *
 * @return     PWC计数器值
 */
uint16_t Motor_PWCCounterGet(TIMER_INDEX TimerIndex);


/**
 * @brief      电机三相PWM输出设置
 *
 * @param[in]  TimerIndex  定时器索引号，支持TIMER5~TIMER8
 * @param[in]  Polar       PWM的输出极性选择
 * @param[in]  A_P         A相上桥臂设置(PWM1_P)，1=开启 0=关闭
 * @param[in]  A_N         A相下桥臂设置(PWM1_N)，1=开启 0=关闭
 * @param[in]  B_P         B相上桥臂设置(PWM2_P)，1=开启 0=关闭
 * @param[in]  B_N         B相下桥臂设置(PWM2_N)，1=开启 0=关闭
 * @param[in]  C_P         C相上桥臂设置(PWM3_P)，1=开启 0=关闭
 * @param[in]  C_N         C相下桥臂设置(PWM3_N)，1=开启 0=关闭
 *
 * @return     错误号：0 - 正确，其他为错误 ，详细参考 PWM_ERROR_CODE
 */
TIMER_ERROR_CODE  Motor_PWMOutPutSet(TIMER_INDEX TimerIndex,PWM_OUTPUT_POLARITY_MODE Polar,bool A_P,bool A_N,bool B_P,bool B_N,bool C_P,bool C_N);


/**
 * @brief      SVPWM发波
 *
 * @param[in]  TimerIndex  定时器索引号，支持TIMER5~TIMER8
 * @param[in]  Ualpha      alpha轴电压分量IQ24
 * @param[in]  Ubeta       beta 轴电压分量IQ24
 * @param[in]  Ts          载波计数周期值，非载波周期(中心对齐模式载波周期的一半)
 *
 * @return     无
 */
void Motor_SVPWM(TIMER_INDEX TimerIndex,int32_t Ualpha,int32_t Ubeta,int16_t Ts);

/**
 * @brief      SVPWM发波方式，角度初始化
 *
 * @param[in]  ucID            电机索引号,0~1
 * @param[in]  ucDir           电机运行方向,0=正，1=反
 * @param[in]  ucHall          Hall传感器值
 * @param[in]  usMaxLmtAng     角度限定偏移值（当HALL信号不变化时，最大角度偏移）,0~0x3FFF
 *
 * @return     电角度,0~0xFFFF(对应角度0~2*Pi)
 */
uint16_t Motor_SvpwmAngleInit(uint8_t ucID,uint8_t ucDir,uint8_t ucHall,uint16_t usMaxLmtAng);


/**
 * @brief      SVPWM发波方式，霍尔传感器角度校正
 *
 * @param[in]  ucID            电机索引号,0~1
 * @param[in]  sFreqFed        电机运行频率
 * @param[in]  ucHall          Hall传感器值
 *
 * @return     无
 */
void Motor_SvpwmAngleHall(uint8_t ucID,int16_t sFreqFed,uint8_t ucHall);


/**
 * @brief      SVPWM发波方式，角度计算
 *
 * @param[in]  ucID            电机索引号,0~1
 * @param[in]  usAng           当前电角度,0~0xFFFF
 * @param[in]  usDelta         电角度增量（一个PWM周期内的角度增量）
 * @param[in]  sFreqFed        电机运行频率
 * @param[in]  ucHall          Hall传感器值
 * @param[in]  usMaxLmtAng     角度限定偏移值（当HALL信号不变化时，最大角度偏移）,0~0x3FFF
 * @param[in]  ucFltTime       角度滤波次数
 *
 * @return     电角度计算值,0~0xFFFF(对应角度0~2*Pi)
 */
uint16_t Motor_SvpwmAngleProc(uint8_t ucID,uint16_t usAng,uint16_t usDelta,int16_t sFreqFed,uint8_t ucHall,uint16_t usMaxLmtAng,uint8_t ucFltTime);



#endif

/**
 * @}
 * @}
 */
