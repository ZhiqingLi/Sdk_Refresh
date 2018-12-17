/**
 **************************************************************************************
 * @file    main.c
 * @brief   main
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-11-22 11:30:00$
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
#include "timer.h"
#include "adc.h"
#include "watchdog.h"
#include "spi_flash.h"


int main(void)
{
	uint16_t DC_Data;
	uint32_t i;

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
    DBG("|                     ADC Key Demo                    |\n");
    DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
    DBG("\\-----------------------------------------------------/\n");
    DBG("\n");
    SysTickInit();

	Clock_ModuleEnable(SARADC_CLK_EN);
	Clock_SarADCClkDivSet(15);//15M = 480/2(15+1)
	Clock_SarADCClkSelect(1);

	ADC_ModeSet(0);   			//1:AC;    0:Dc
	ADC_VrefSet(1);				//1:VDDA; 0:2.4*VGA
	ADC_DCClkDivSet(0); 		//0:no div
	ADC_DCChannelSet(3);
	ADC_Enable();

	KeyInit();

	while(1)
	{
		KeyProcess();
		//do ...
	}
	while(1);
}


