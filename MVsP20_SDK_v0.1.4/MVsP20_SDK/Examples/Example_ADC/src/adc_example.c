/**
 **************************************************************************************
 * @file    adc_example.c
 * @brief   adc example
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-11-30 11:30:00$
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

//  DC 通道说明，GPIO通道需要将对应的GPIO端口配置为模拟输入；
//	0:  VDD33
//	1:  VDD_BK
//	2:  VDD12
//	3:  GPIOA3
//	4:  GPIOA4
//	5:  GPIOA5
//	6:  GPIOA6
//	7:  GPIOA9
//	8:  GPIOA10
//	9:  GPIOA11
//	10: GPIOA12
//	11: GPIOA13
//	12: GPIOA14
//	13: GPIOA16
//	14: GPIOA17
//	15: GPIOA18
//	16: GPIOB0
//	17: GPIOB1
//	18: GPIOB2
//	19: GPIOB3
//	20: GPIOB24
//	21: GPIOB25
//	22: GPIOB26
//	23: GPIOB27
//	24: GPIOB28
//	25: GPIOB29
//	26: GPIOC10
//	27: GPIOC11

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
    DBG("|                     ADC Example                     |\n");
    DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
    DBG("\\-----------------------------------------------------/\n");
    DBG("\n");

	DBG("SarADC DC example\n");
	//GPIOA3做为ADC采样输入引脚
	GPIO_RegOneBitClear(GPIO_A_PU, GPIOA3);//ADC3
	GPIO_RegOneBitSet(GPIO_A_PD, GPIOA3);

	Clock_ModuleEnable(SARADC_CLK_EN);
	Clock_SarADCClkDivSet(15);//15M = 480/2(15+1)
	Clock_SarADCClkSelect(1);

	ADC_ModeSet(0);   			//1:AC;    0:Dc
	ADC_VrefSet(1);				//1:VDDA; 0:2.4*VGA
	ADC_DCClkDivSet(0); 		//0:no div
	ADC_DCChannelSet(3);
	ADC_Enable();

	while(1)
	{
		DC_Data = ADC_DCChannelDataGet(3);
		DBG("DC_Data = %d\n", DC_Data) ;
		for(i=0; i<0x1000000;i++);//仅仅用于延时，串口打印显示看的清楚
	}
	while(1);
}


