/**
 **************************************************************************************
 * @file    power_example.c
 * @brief   power management example
 *
 * @author  Peter
 * @version V1.0.0
 *
 * $Created: 2017-06-01 11:30:00$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdio.h>
#include <nds32_intrinsic.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "watchdog.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "irqn.h"
#include "spi_flash.h"
#include "timeout.h"
#include "clk.h"
#include "delay.h"
#include "lp.h"

void InitZiArea(void)
{
	/* Use compiler builtin and memset */
	#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

	extern char _end;
	extern char __bss_start;
	int size;

	/* Clear bss section */
	size = &_end - &__bss_start;
	MEMSET(&__bss_start, 0, size);
	return;
}

void Clock_GetDPll(uint32_t* K1, uint32_t* OS, uint32_t* NDAC);

void ClosePeripheralClk(void)
{
	Clock_ModuleDisable(USB_CLK_480M_EN );
	Clock_ModuleDisable(EFUSE_CLK_EN    );
	Clock_ModuleDisable(I2S_CLK_EN      );
	Clock_ModuleDisable(SARADC_CLK_EN   );
	Clock_ModuleDisable(I2C_CLK_EN      );
	Clock_ModuleDisable(SD_CLK_EN       );
	Clock_ModuleDisable(SPIM0_CLK_EN    );
	Clock_ModuleDisable(SPIM1_CLK_EN    );
	Clock_ModuleDisable(UART0_CLK_EN    );
	Clock_ModuleDisable(UART1_CLK_EN    );
	Clock_ModuleDisable(UART2_CLK_EN    );
	Clock_ModuleDisable(USB_CLK_EN      );
	Clock_ModuleDisable(DMA_CLK_EN      );
	Clock_ModuleDisable(FDMA_CLK_EN     );
	Clock_ModuleDisable(DPA_CLK_EN      );
	Clock_ModuleDisable(LEDC_CLK_EN     );
	Clock_ModuleDisable(TIMER1_CLK_EN   );
	Clock_ModuleDisable(TIMER2_CLK_EN   );
	Clock_ModuleDisable(TIMER3_CLK_EN   );
	Clock_ModuleDisable(TIMER4_CLK_EN   );
	Clock_ModuleDisable(TIMER5_CLK_EN   );
	Clock_ModuleDisable(TIMER6_CLK_EN   );
	Clock_ModuleDisable(TIMER7_CLK_EN   );
	Clock_ModuleDisable(TIMER8_CLK_EN   );
}

void LP_EfuseReadDisable(void);

//#define POWER_TEST0 //RC时钟，sleep功耗
//#define POWER_TEST1 //pll开环，cpu用RC时钟，sleep功耗
//#define POWER_TEST2 //pll闭环，cpu用RC时钟，sleep功耗
//#define POWER_TEST3 //pll闭环，cpu用pll时钟，cpu时钟10分频，sleep功耗
//#define POWER_TEST4 //pll闭环，cpu用pll时钟，sleep功耗
#define POWER_TEST5 //pll闭环，cpu用pll时钟，跑while(1)功耗
//#define POWER_TEST6 //pll闭环，cpu用pll时钟，UART循环打印字符功耗
//#define POWER_TEST7 //pll开环，cpu用pll时钟，跑while(1)功耗
//#define POWER_TEST8 //cpu用RC时钟，跑while(1)功耗
//#define POWER_TEST9 //Deepsleep功耗

//注意：测试时需要要将B0和B1短接起来
int main()
{
	uint8_t	num = 0;
	uint32_t K1, OS, NDAC;

    WDG_Disable();

    CacheTcmDeInit();
    CacheTcmInitFast(PHYMEM_16KBPM0/*cache*/, PHYMEM_NONE/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);
    InitZiArea();
    __nds32__mtsr(1, NDS32_SR_INT_PEND2);
    LP_EfuseReadDisable();
    Clock_DynamicClockGateEnable();

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);

	GPIO_RegOneBitClear(GPIO_B_OE, GPIOB0);
	GPIO_RegOneBitSet(GPIO_B_IE, GPIOB0);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB0);
	GPIO_RegOneBitClear(GPIO_B_PD, GPIOB0);

	GPIO_RegOneBitClear(GPIO_B_IE, GPIOB1);
	GPIO_RegOneBitSet(GPIO_B_OE, GPIOB1);
	GPIO_RegOneBitClear(GPIO_B_OUT, GPIOB1);

	while(GPIO_RegOneBitGet(GPIO_B_IN,GPIOB0)==1);

	GPIO_RegOneBitClear(GPIO_B_OE, GPIOB0);
	GPIO_RegOneBitClear(GPIO_B_IE, GPIOB0);
	GPIO_RegOneBitSet(GPIO_B_PU, GPIOB0);
	GPIO_RegOneBitClear(GPIO_B_PD, GPIOB0);

	GPIO_RegOneBitClear(GPIO_B_IE, GPIOB1);
	GPIO_RegOneBitClear(GPIO_B_OE, GPIOB1);
	GPIO_RegOneBitSet(GPIO_B_PU, GPIOB0);
	GPIO_RegOneBitClear(GPIO_B_PD, GPIOB0);

#ifdef POWER_TEST0 //RC时钟，sleep功耗
	Clock_SysClkSelect(RC_CLK_MODE);
    Clock_PllClose();
    Clock_OSC12MDisable();
    __nds32__standby_no_wake_grant();
    while(1);
#endif

#ifdef POWER_TEST1 //pll开环，cpu用RC时钟，sleep功耗
    Clock_GetDPll(&K1, &OS, &NDAC);
	ClosePeripheralClk();
	Clock_SysClkSelect(RC_CLK_MODE);
    Clock_PllClose();
    Clock_OSC12MDisable();
    Clock_PllFreeRun(480000, K1, OS, NDAC);
    __nds32__standby_no_wake_grant();
    while(1);
#endif

#ifdef POWER_TEST2 //pll闭环，cpu用RC时钟，sleep功耗
	ClosePeripheralClk();
	Clock_SysClkSelect(RC_CLK_MODE);
    __nds32__standby_no_wake_grant();
    while(1);
#endif

#ifdef POWER_TEST3 //pll闭环，cpu用pll时钟，cpu时钟10分频，sleep功耗
	ClosePeripheralClk();
	Clock_SysClkDivSet(10);
    __nds32__standby_no_wake_grant();
    while(1);
#endif

#ifdef POWER_TEST4
	ClosePeripheralClk();
    __nds32__standby_no_wake_grant();
    while(1);
#endif

#ifdef POWER_TEST5 //pll闭环，cpu用pll时钟，跑while(1)功耗
	ClosePeripheralClk();
    while(1);
#endif

#ifdef POWER_TEST6 //pll闭环，cpu用pll时钟，UART循环打印字符功耗
	Clock_UARTClkSelect(1);
	GPIO_UartTxIoConfig(0, 1);//C2
	UARTS_Init(0, 115200, 8, 0, 1);
	ClosePeripheralClk();
	Clock_ModuleEnable(USB_CLK_480M_EN );
	Clock_ModuleEnable(UART0_CLK_EN    );
    while(1)
    {
    	DBG(".");
    }
#endif

#ifdef POWER_TEST7 //pll开环，cpu用pll时钟，跑while(1)功耗
	Clock_GetDPll(&K1, &OS, &NDAC);
	ClosePeripheralClk();
	Clock_SysClkSelect(RC_CLK_MODE);
    Clock_PllClose();
    Clock_OSC12MDisable();
    Clock_PllFreeRun(480000, K1, OS, NDAC);
    Clock_SysClkSelect(PLL_CLK_MODE);
    while(1);
#endif

#ifdef POWER_TEST8 //cpu用RC时钟，跑while(1)功耗
	ClosePeripheralClk();
	Clock_SysClkSelect(RC_CLK_MODE);
    Clock_PllClose();
    Clock_OSC12MDisable();
    while(1);
#endif

#ifdef POWER_TEST9 //Deepsleep功耗
    Clock_SysClkSelect(RC_CLK_MODE);
    Clock_PllClose();
    Clock_OSC12MDisable();
    //LP_OSC32KDisable();
    ClosePeripheralClk();

    GPIO_RegSet(GPIO_A_IE, 0);
    GPIO_RegSet(GPIO_A_OE, 0);
    GPIO_RegSet(GPIO_A_PU, 0xffffffff);
    GPIO_RegSet(GPIO_A_PD, 0);

    GPIO_RegSet(GPIO_B_IE, 0);
    GPIO_RegSet(GPIO_B_OE, 0);
    GPIO_RegSet(GPIO_B_PU, 0xffffffff);
    GPIO_RegSet(GPIO_B_PD, 0);

    GPIO_RegSet(GPIO_C_IE, 0);
    GPIO_RegSet(GPIO_C_OE, 0);
    GPIO_RegSet(GPIO_C_PU, 0xfff);
    GPIO_RegSet(GPIO_C_PD, 0);

    CacheTcmDeInit();
    __nds32__standby_wake_grant();
    while(1);
#endif

    return 0;
}
