/*
  ******************************************************************************
  * @file    main.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"
#include "asm32s003_gpio.h"
#include "asm32s003_uart.h"
#include "asm32s003_counta.h"
#include "asm32s003_i2c.h"
#include "asm32s003_led.h"
#include "asm32s003_gtc.h"
#include "asm32s003_ifc.h"
#include "asm32s003_adc.h"
#include "asm32s003_syscon.h"

/* externs--------- ----------------------------------------------------------*/
extern void ASM32S003_init(void);
extern volatile uint8_t I2CWrBuffer[BUFSIZE];
extern volatile uint8_t I2CRdBuffer[BUFSIZE];



/*************************************************************/
//main
/*************************************************************/
int main(void)
{
	ASM32S003_init();

	while(1);

}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/


