/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2010, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			coding_key.c
* Description:		coding_key scan and process c file
*
*  maintainer: Fenston
*
* Change History:
*			chf	- 01/22/2014 - V0.1
*					- created
******************************************************************************
*/
#include "app_config.h"
#include "app_msg.h"

#ifdef FUNC_CODING_KEY_EN

//clockwise rotation
//    ----|		   |--------|		 |-------------------
//A       |        |        |        |
//        |--------|        |--------|

//     -------|        |--------|		 |---------------
//B           |        |        |		 |
//            |--------|        |--------|


//counterclockwise rotation
//    --------|		   |--------|		 |---------------
//A           |		   |        |		 |
//            |--------|        |--------|

//     ---|        |--------|		 |-------------------
//B       |        |        |		 |
//        |--------|        |--------|


volatile U8_T		ClockWiseCnt;
volatile U8_T		CounterClockWiseCnt;

// Initialize coding key scan (GPIO) operation.
// Config interrupt at negative edge of signal-A
void CodingKeyInit(void)
{
	GPIO_PullHigh_Init(CODING_KEY_A_PORT_OUT,CODING_KEY_A_PORT_BIT);
	if(CODING_KEY_A_PORT_OUT == GPIOA0)
	{
		GPIOGRP->IGRP = GPIOGRP->IGRP & (~(0x00000003 << (CODING_KEY_A_PORT_BIT*2)));
		GPIOA0_EXI_Init(CODING_KEY_A_PORT_BIT); 
	}
	else if(CODING_KEY_A_PORT_OUT == GPIOB0)
	{
		GPIOGRP->IGRP = GPIOGRP->IGRP | (0x00000001 << (CODING_KEY_A_PORT_BIT*2));
		GPIOB0_EXI_Init(CODING_KEY_A_PORT_BIT); 
	}
	else if(CODING_KEY_A_PORT_OUT == GPIOC0)
	{
		GPIOGRP->IGRP = GPIOGRP->IGRP | (0x00000002 << (CODING_KEY_A_PORT_BIT*2));
		GPIOC0_EXI_Init(CODING_KEY_A_PORT_BIT); 
	}
	EXTI_trigger_CMD(ENABLE,(CSP_REGISTER_T)(0X01ul<<CODING_KEY_A_PORT_BIT),_EXIFT); 
	EXTI_interrupt_CMD(ENABLE,(CSP_REGISTER_T)(0X01ul<<CODING_KEY_A_PORT_BIT));
	GPIO_EXTI_interrupt(CODING_KEY_A_PORT_OUT,(CSP_REGISTER_T)(0X01ul<<CODING_KEY_A_PORT_BIT));

	//设置第二个检测口为上拉输入。
	GPIO_PullHigh_Init(CODING_KEY_B_PORT_OUT,CODING_KEY_B_PORT_BIT);
	GPIO_Init(CODING_KEY_B_PORT_OUT,CODING_KEY_B_PORT_BIT,Intput);

	if(CODING_KEY_A_PORT_BIT == 0)
	{
    	EXI0_Int_Enable();                                          //EXI0 INT Vector
    }
    else if(CODING_KEY_A_PORT_BIT == 1)
    {
    	EXI1_Int_Enable();                                          //EXI1 INT Vector
    }
    else if((CODING_KEY_A_PORT_BIT >= 2) && (CODING_KEY_A_PORT_BIT <= 3))
    {
    	EXI2_Int_Enable();                                          //EXI2~EXI3 INT Vector
    }
    else if((CODING_KEY_A_PORT_BIT >= 4) && (CODING_KEY_A_PORT_BIT <= 8))
    {
    	EXI3_Int_Enable();                                          //EXI4~EXI8 INT Vector
    }
    else if((CODING_KEY_A_PORT_BIT >= 9) && (CODING_KEY_A_PORT_BIT <= 13))
    {
    	EXI4_Int_Enable();                                          //EXI9~EXI13 INT Vector
	}
}

// Key process, image key value to key event.
uint16_t CodingKeyScan(void)
{
	uint16_t Msg = MSG_NONE;

	if(ClockWiseCnt)
	{
		Msg = MSG_VOL_UP;
	}
	else if(CounterClockWiseCnt)
	{
		Msg = MSG_VOL_DW;
	}
	ClockWiseCnt = 0;
	CounterClockWiseCnt = 0;

	return Msg;
}
#endif // FUNC_CODING_KEY_EN

