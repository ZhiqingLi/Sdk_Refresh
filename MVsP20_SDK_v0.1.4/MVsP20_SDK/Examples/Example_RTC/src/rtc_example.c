////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2016, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:rtc_example.c
//		Author：Sean
//
//		Description	:
//					create example to show how to use rtc
//
//		Changelog	:	
//					2016-11-24	create rtc example
///////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include "irqn.h"
#include "cache_tcm.h"
//#include "gpio.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
//#include "timeout.h"
#include "clk.h"
#include "spi_flash.h"
#include "gpio.h"
#include "rtc.h"
#include "watchdog.h"
uint32_t SecLast = 0;
uint16_t FracLast = 0;
RTC_REG_TIME_UNIT time = {0,0};
RTC_REG_TIME_UNIT alarm ={0,0};



bool alarmFlag = FALSE;
int main(void)
{
	uint32_t rtcCase = 1;

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

	GPIO_UartTxIoConfig(0, 1);
	//GPIO_UartRxIoConfig(0, 1);
	UARTS_Init(0, 115200, 8, 0, 1);

	DBG("**********************RTC Example***************************\n");
	WDG_Disable();

	rtcCase = 0;
	switch(rtcCase)
	{
		case 0:
			DBG(">>>>>>>>>>>>>>>>>>Polling<<<<<<<<<<<<<<<<<<<<<<<\n\n");
			//RTC_ClockSrcSel(OSC_32K,32768);

			RTC_ClockSrcSel(OSC_12M, 31250);
			RTC_IntDisable();
			RTC_IntFlagClr();

			time.Sec = 2006;
			RTC_TimeSet(time);
			alarm.Sec = 2016;
			RTC_AlarmSet(alarm);

			RTC_IntFlagClr();

			RTC_IntEnable();
			NVIC_DisableIRQ(RTC_IRQn);
			while(1)
			{
				time = RTC_TimeGet();
				if(SecLast != time.Sec)
				{
					SecLast = time.Sec;
					DBG("rtc:%ds\n",time.Sec);
					if(RTC_IntFlagGet() == TRUE)
					{
						DBG("there is an alarm\n");
						RTC_IntFlagClr();
						alarm.Sec = time.Sec+10;
						RTC_AlarmSet(alarm);
					}
				}
			}
		  break;

		case 1:
			DBG(">>>>>>>>>>>>>>>>>>Int<<<<<<<<<<<<<<<<<<<<<<<\n\n");
			//RTC_ClockSrcSel(OSC_32K,32768);

			RTC_ClockSrcSel(OSC_12M, 31250);
			RTC_IntDisable();
			RTC_IntFlagClr();

			time.Sec = 2006;
			RTC_TimeSet(time);
			alarm.Sec = 2016;
			RTC_AlarmSet(alarm);

			RTC_IntFlagClr();

			RTC_IntEnable();

			NVIC_EnableIRQ(RTC_IRQn);
			GIE_ENABLE();

			while(1)
			{
				time = RTC_TimeGet();
				if(SecLast != time.Sec)
				{
					SecLast = time.Sec;
					DBG("rtc:%ds\n",time.Sec);

					if(TRUE == alarmFlag)
					{
						alarmFlag = FALSE;
						DBG("there is an alarm\n");
						RTC_IntFlagClr();
						alarm.Sec = time.Sec+10;
						RTC_AlarmSet(alarm);
					}
				}
			}
		  break;

		default:
			break;
	}

	return 0;
}

__attribute__((section(".driver.isr")))void RtcInterrupt(void)//RTC唤醒 并不会进入RTC中断
{
	if(RTC_IntFlagGet() == TRUE)
	{
		alarmFlag = TRUE;
		RTC_IntFlagClr();
	}
	DBG("-------------");
}

