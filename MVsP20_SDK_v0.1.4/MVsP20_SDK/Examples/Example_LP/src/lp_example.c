/**
 *******************************************************************************
 * @file    lp_example.c
 * @brief	lp example

 * @author  Peter Zhai
 * @version V1.0.0

 * $Created: 2016-12-26 15:25:07$
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *******************************************************************************
 */

#include "type.h"
#include "lp.h"
#include "debug.h"
#include "uarts.h"
#include "gpio.h"
#include "cache_tcm.h"
#include "clk.h"
#include "uarts_interface.h"
#include "watchdog.h"
#include "spi_flash.h"

//#define ENABLE_GLITCH_FILTER

int main(void)
{
	WDG_Disable();

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
	GPIO_UartRxIoConfig(0, 1);

	UARTS_Init(0,  115200, 8,  0,  1);

	DBG("-------------Low Power Example-------------\n");

	DBG("0->RESE\n1->WORK_SW_PD\n3->BOOT_VIN_SWITCH_FAIL\n4->PU_TIMEOUT_FAIL\n5->WORK_VDD33_FAIL\n6->WORK_VIN_SWITCH_FAIL\n7->WORK_VIN_LOW_FAIL\n8->PD_VDD33_FAIL\n9->PD_VIN_SWITCH_FAIL\n10->PD_VIN_LOW_FAIL\n");
	DBG("PowerState:%d\n",LP_PowerDownStateGet());
	DBG("1->lvd\n2->por\n4->vin_sel\n8->uvlo\n");
	DBG("LPGlitchResetFlag:%d\n",LP_GlitchFlagGet());
	LP_GlitchFlagClear();
	
	LP_LVDThresholdSet(4);
	LP_NormalPowerDownEnable();
	LP_LVDEventFastISODisable();

#ifdef ENABLE_FILTER
	LP_PorGlitchWidthSet(10);
	LP_LVDGlitchWidthSet(10);
	LP_PorGlitchFilterEnable();
	LP_LVDGlitchFilterEnable();
#endif

	LP_LVDPowerDownEnable();

	while(1);
}
