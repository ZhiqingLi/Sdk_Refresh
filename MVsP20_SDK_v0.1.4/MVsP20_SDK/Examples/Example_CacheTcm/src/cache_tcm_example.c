/**
 **************************************************************************************
 * @file    cache_tcm_example.c
 * @brief   Cache & TCM Example
 *
 * @author  Aissen Li
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdio.h>
#include "type.h"
#include "cache_tcm.h"
#include "clk.h"
#include "gpio.h"
#include "debug.h"
#include "delay.h"
#include "timeout.h"
#include "watchdog.h"
#include "uarts_interface.h"
#include "timer.h"

#define UART_GPIO_GROUP_SEL     (0)
#define UART_GPIO_MODEL_SEL     (1)

int main(void)
{
	int8_t  CacheTcmFlag = 0;
	int32_t TimerCount   = 0;

	TIMER t;

	//Disable watchDog
	WDG_Disable();

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);

	GPIO_UartTxIoConfig(UART_GPIO_GROUP_SEL, UART_GPIO_MODEL_SEL);
	GPIO_UartRxIoConfig(UART_GPIO_GROUP_SEL, UART_GPIO_MODEL_SEL);

	UARTS_Init(UART_GPIO_GROUP_SEL, 115200, 8, 0, 1);

	DBG("\n");
    DBG("/-----------------------------------------------------\\\n");
    DBG("|                 Cache & TCM Example                 |\n");
    DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
    DBG("\\-----------------------------------------------------/\n");
    DBG("\n");

    SysTickInit();
    TimeOutSet(&t, 1000);

    while(1)
    {
    	uint8_t Key;

    	WaitMs(10);

    	if(IsTimeOut(&t))
    	{
    		TimerCount++;
    		DBG("%d\n", TimerCount);
    		TimeOutSet(&t, 1000);
    	}

    	if(UARTS_Recv(UART_GPIO_GROUP_SEL, &Key, 1) > 0)
    	{
    		switch(Key)
    		{
    		case 'c':
    		case 'C':
                CacheTcmFlag = !CacheTcmFlag;
    			if(CacheTcmFlag)
                {
                    CacheTcmInitFast(PHYMEM_16KBPM0 + PHYMEM_16KBPM1/*cache*/, PHYMEM_64KBPM2/*tcm r0*/, 0/*tcm r0 offset*/, 64/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);
                }
                else
                {
                    CacheTcmDeInit();
                }
    			DBG("Cache & TCM %s\n", CacheTcmFlag ? "enabled" : "disabled");
    			break;
    		default:
    			break;
    		}
    	}
    }

    return 0;
}
