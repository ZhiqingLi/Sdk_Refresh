/*
  ******************************************************************************
  * @file    asm32s003_gtc.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_pwm_H
#define _asm32s003_pwm_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"
#include "asm32s003_gtc.h"

//--------------------------------------------------------------------------------
//-----------------------------GTC  value enum define----------------------------
//--------------------------------------------------------------------------------
/**
  * @brief  GTC clock
  */
typedef enum
{
	GTC0_PWMPIN_GPIOB2	=	0,
	GTC0_PWMPIN_GPIOB1	=	1,
	GTC0_PWMPIN_GPIOA0	=	2,
	GTC0_PWMPIN_GPIOA11	=	3,
	GTC0_PWMPIN_GPIOC3	=	4,
	GTC0_TCK0PIN_GPIOA6	=	0,
	GTC0_TCK2PIN_GPIOB3	=	0,
	GTC0_TCK2PIN_GPIOA9	=	1,
	GTC0_TCPPIN_GPIOA12	=	0,
	GTC0_TCPPIN_GPIOA3	=	1,
}GTC0_PIN_TypeDef;

  
/**
  * @brief  GTC INT MASK SET/CLR  Set
  */
typedef enum
{
	GTC1_PWMPIN_GPIOA9	=	0,
	GTC1_PWMPIN_GPIOA8	=	1,
	GTC1_PWMPIN_GPIOB3	=	2,
	GTC1_PWMPIN_GPIOA5	=	3,
	GTC1_PWMPIN_GPIOC2	=	4,
	GTC1_TCK0PIN_GPIOA6	=	0,
	GTC1_TCK2PIN_GPIOB3	=	0,
	GTC1_TCK2PIN_GPIOA9	=	1,
	GTC1_TCPPIN_GPIOA10	=	0,
	GTC1_TCPPIN_GPIOA6	=	1,
}GTC1_PIN_TypeDef;

/**
  * @brief  GTC INT MASK SET/CLR  Set
  */
typedef enum
{
	GTC2_PWMPIN_GPIOB0	=	0,
	GTC2_PWMPIN_GPIOA13	=	1,
	GTC2_PWMPIN_GPIOA7	=	2,
	GTC2_PWMPIN_GPIOA4	=	3,
	GTC2_PWMPIN_GPIOC1	=	4,
	GTC2_TCK0PIN_GPIOA6	=	0,
	GTC2_TCK2PIN_GPIOB3	=	0,
	GTC2_TCK2PIN_GPIOA9	=	1,
	GTC2_TCPPIN_GPIOA8	=	0,
	GTC2_TCPPIN_GPIOB2	=	1,
}GTC2_PIN_TypeDef;

typedef enum
{
	GTC3_PWMPIN_GPIOB2	=	0,
	GTC3_PWMPIN_GPIOA3	=	1,
	GTC3_PWMPIN_GPIOB3	=	2,
	GTC3_PWMPIN_GPIOC0	=	4,
	GTC3_TCK0PIN_GPIOA6	=	0,
	GTC3_TCK2PIN_GPIOB3	=	0,
	GTC3_TCK2PIN_GPIOA9	=	1,
	GTC3_TCPPIN_GPIOB0	=	0,
}GTC3_PIN_TypeDef;

/*********************** GTC External Functions Declaration **********************
******************************************************************************/

extern void GTC_PwmControl_Configure(CSP_GTC_T *GTC, GTC_FIN_TypeDef  GTC_FIN_X, Counter_Size_TypeDef  Counter_Size_X, U32_T PwmFreq, U8_T PwmDuty);
extern void GTC_PwmControl_Init(void);
extern void GTC_PwmControl_Change(CSP_GTC_T *GTC, U32_T PwmFreq, U8_T PwmDuty);

#endif   /* asm32s003_gtc_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/
