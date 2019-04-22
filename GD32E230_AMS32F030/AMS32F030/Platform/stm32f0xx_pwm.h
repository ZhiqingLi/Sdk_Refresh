/**
  *****************************************************************************
  * @file:			pwm.h
  * @author			YunWang
  * @version		V1.0.0
  * @data			4-May-2014
  * @Brief			PWM module driver header file.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

#ifndef __STM32F0XX_PWM_H__
#define	__STM32F0XX_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <stdint.h>
#include "stm32f0xx_tim.h"
/*
**********************************************************
*					TYPE DEFINE
**********************************************************
*/
/**
 * PWM Timer select definition
 */
#define PWM_IS_TIMER1		(uint16_t)(0x1000)
#define PWM_IS_TIMER2		(uint16_t)(0x2000)
#define PWM_IS_TIMER3		(uint16_t)(0x3000)
#define PWM_IS_TIMER14		(uint16_t)(0x4000)
#define PWM_IS_TIMER15		(uint16_t)(0x5000)
#define PWM_IS_TIMER16		(uint16_t)(0x6000)
#define PWM_IS_TIMER17		(uint16_t)(0x7000)

#define PWM_IS_CH1			(TIM_Channel_1<<8)
#define PWM_IS_CH2			(TIM_Channel_2<<8)
#define PWM_IS_CH3			(TIM_Channel_3<<8)
#define PWM_IS_CH4			(TIM_Channel_4<<8)

#define PWM_CHANNEL_MASK	(uint16_t)(0x0F00)
#define PWM_TIMER_MASK		(uint16_t)(0xF000)


/**
 * PWM Channel select definition
 */
#define PWM1_CH1_A8			(uint16_t)(PWM_IS_TIMER1|PWM_IS_CH1|(0x00))
#define PWM1_CH2_A9			(uint16_t)(PWM_IS_TIMER1|PWM_IS_CH2|(0x01))
#define PWM1_CH3_A10		(uint16_t)(PWM_IS_TIMER1|PWM_IS_CH3|(0x02))
#define PWM1_CH4_A11		(uint16_t)(PWM_IS_TIMER1|PWM_IS_CH4|(0x03))
#define PWM3_CH1_A6_B4		(uint16_t)(PWM_IS_TIMER3|PWM_IS_CH1|(0x04))
#define PWM3_CH2_A7_B5		(uint16_t)(PWM_IS_TIMER3|PWM_IS_CH2|(0x05))
#define PWM3_CH3_B0			(uint16_t)(PWM_IS_TIMER3|PWM_IS_CH3|(0x06))
#define PWM3_CH4_B1			(uint16_t)(PWM_IS_TIMER3|PWM_IS_CH4|(0x07))
#define PWM14_CH1_A4_B1		(uint16_t)(PWM_IS_TIMER14|PWM_IS_CH1|(0x08))
#define PWM15_CH1_A2_B14	(uint16_t)(PWM_IS_TIMER15|PWM_IS_CH1|(0x09))
#define PWM15_CH2_A3_B15	(uint16_t)(PWM_IS_TIMER15|PWM_IS_CH2|(0x0A))
#define PWM16_CH1_A6_B8		(uint16_t)(PWM_IS_TIMER16|PWM_IS_CH1|(0x0B))
#define PWM17_CH1_A7_B9		(uint16_t)(PWM_IS_TIMER17|PWM_IS_CH1|(0x0C))

/**
 * PWM I/O select definition
 */
#define PWM_IO_SEL0			0
#define PWM_IO_SEL1			1

/**
 * PWM param definition
 */
#define PWM_OUTPUT_NUM		13
#define PWM_IO_NUM			2

/**
 * PWM output mode definition
 */
#define PWM_OUTPUT_HIGH		0
#define PWM_OUTPUT_LOW		1


/*
**********************************************************
*					GLOBAL FUNCTION PROTOTYPE
**********************************************************
*/
/**
 * @Brief	PWM I/O reuse setting and pwm output/pulldown1/pulldown2 drive mode select
 * @Param	PwmChSel refer MACRO definition
 * @Param	PwmIoSel refer MACRO definition
 * @Param	PwmMode refer MACRO definition
 * @Return	None
 * @Note	Call API PwmConfig first and then PwmEnableChannel to generate PWM
 */
void PwmEnableChannel(uint16_t PwmChSel, uint8_t PwmIoSel, uint8_t PwmMode);

/**
 * @Brief	PWM io reuse disable
 * @Param	PwmChSel refer MACRO definition
 * @Return	None
 * @Note@ 	Disable the Channel of Group 0 and Group 1 together
 */
void PwmDisableChannel(uint16_t PwmChSel, uint8_t PwmIoSel);

/**
 * @Brief	PWM Frequency and duty configuration
 * @Param	PwmChSel refer MACRO definition
 * @Param	FreqDiv
 *			pwm_freq =  FreqDiv/10  step-by-step 0.1Hz
 * @Param	Duty
 *			pwm duty = Duty/1000 * 100% step-by-step 0.1%
 * @Return	None
 * @Note@	When Duty = 1000£¬output high level£¬When Duty = 0£¬output low level
 */
void PwmConfigChannel(uint16_t PwmChSel, uint32_t FreqDiv, uint16_t Duty);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
