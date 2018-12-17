/**
 **************************************************************************************
 * @file    clock_example.c
 * @brief   clock example
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-11-30 11:48:00$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <stdlib.h>
#include <string.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "timeout.h"
#include "clk.h"
#include "dma.h"
#include "timer.h"
#include "pwm.h"
#include "watchdog.h"
#include "spi_flash.h"

//时钟演示工程，主要演示系统时钟选择参考源
//1: 32k晶体起振DPLL锁定480M，系统时钟PLL4分频为120M
//2: 12M晶体起振DPLL锁定480M，系统时钟PLL4分频为120M
//3：RC12M，系统工作于RC时钟下
//仅用于演示，外部晶体起振时需要接相关晶体
//用PWM 10分频后 GPIOA2上示波器观察
//系统默认使用32K晶体锁PLL480M
int main(void)
{
	uint8_t Key = 0;;
	PWM_StructInit PWMParam;
	PWM_IOCTRL_ARG PWMArg;
	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);

	SpiFlashInit(80000000, MODE_4BIT, 0, 1);


	GPIO_UartTxIoConfig(0, 1);//C2
	GPIO_UartRxIoConfig(0, 1);//C3
	UARTS_Init(0, 115200, 8, 0, 1);

	WDG_Disable();
	DBG("\n");
    DBG("/-----------------------------------------------------\\\n");
    DBG("|                    Clock Example                    |\n");
    DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
    DBG("\\-----------------------------------------------------/\n");
    DBG("\n");

    PWMArg.OutputType = PWM_OUTPUT_SINGLE_FORCE_NOACTIVE;
	PWM_IOCTRl(TIMER3, PWM1, OUTPUT_TYPE_UPDATE, &PWMArg);
	PWM_GpioConfig(TIMER3_PWM1_A2_B22_A30_C7, 0, PWM_IO_MODE_OUT);//PWM GPIOA2输出

	//PWM参数配置
	PWMParam.CounterMode   = PWM_COUNTER_MODE_DOWN;
	PWMParam.OutputType    = PWM_OUTPUT_SINGLE_1;

	PWMParam.FreqDiv       = 10;
	PWMParam.Duty[0]       = 50;

	PWMParam.DMAReqEnable  = 0;
	PWMParam.OutPolarity[0]   = 0;
	PWMParam.OutEnable[0]     = 3;

	PWM_Config(TIMER3, PWM1, &PWMParam);
	PWM_Enable(TIMER3);

	DBG("system work on PLL 480M, Soc 32k\n");
    while(1)
    {
    	if(UARTS_Recv(0, &Key, 1) > 0)
    	{
    		switch(Key)
			{
			/*case 'A':
			case 'a':
				Clock_Config(1, 32768);
				Clock_PllLock(480000);
				Clock_SysClkSelect(PLL_CLK_MODE);
				Clock_UARTClkSelect(1);
				GPIO_UartTxIoConfig(0, 1);//C2
				GPIO_UartRxIoConfig(0, 1);//C3
				UARTS_Init(0, 115200, 8, 0, 1);
				DBG("system work on PLL 480M, Soc 32k\n");
				break;
				*/
			case 'B':
			case 'b':
				GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
				GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
				GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
				GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
				Clock_Config(1, 12000000);
				Clock_PllLock(480000);
				Clock_SysClkSelect(PLL_CLK_MODE);
				Clock_UARTClkSelect(1);
				GPIO_UartTxIoConfig(0, 1);//C2
				GPIO_UartRxIoConfig(0, 1);//C3
				UARTS_Init(0, 115200, 8, 0, 1);
				DBG("system work on PLL 480M, Soc 12M\n");
				break;
			case 'C':
			case 'c':
				Clock_SysClkSelect(RC_CLK_MODE);
				Clock_UARTClkSelect(0);
				GPIO_UartTxIoConfig(0, 1);//C2
				GPIO_UartRxIoConfig(0, 1);//C3
				UARTS_Init(0, 115200, 8, 0, 1);
				DBG("system work on RC 12M\n");
				break;
			default:
				break;
			}
    	}
    }


    DBG("END!\n");
	while(1);
}


