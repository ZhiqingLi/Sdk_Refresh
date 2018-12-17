#include <stdio.h>
#include <nds32_intrinsic.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "spi_flash.h"
#include "timeout.h"
#include "clk.h"
#include "pwm.h"
#include "sleep.h"
#include "delay.h"
#include "rtc.h"
#include "irqn.h"
#include "watchdog.h"


#define GPIO_WAKEUP  1
#define RTC_WAKEUP   0
#define MIX_WAKEUP   0


//wakeup中断响应函数，在调用NVIC_EnableIRQ(Wakeup_IRQn)及Sleep_EnWakeUpInt()以及全局中断使能开启后此中断例程被响应
__attribute__((section(".driver.isr")))void WakeupInterrupt(void)
{
	uint32_t flag1, flag2,flag3;

	Sleep_WakeUpSrcFlagGet(&flag1, &flag2,&flag3);
	//SREG_WAKEUP_INT_CTRL.WKUP_INT_EN = 0;
	DBG("\n[Interrupt]WakeUpFlags:FLag0 %08X, Flag1 %08X, Flag2 %08X\n ", flag1, flag2, flag3);
	WakeUpFlagClear();
	Sleep_WakeUpSrcFlagGet(&flag1, &flag2,&flag3);
	WaitMs(1);
	DBG("[Interrupt]after clear WakeUpFlag:FLag0 %08X, Flag1 %08X, Flag2 %08X \n", flag1, flag2, flag3);

}

int main()
{

	 uint32_t  flag1,flag2,flag3;


	 //Disable watchDog
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

    __nds32__mtsr(1, NDS32_SR_INT_PEND2);
    LP_EfuseReadDisable();
    Clock_DynamicClockGateEnable();

	SpiFlashInit(80000000, MODE_4BIT, 0, 1);

	/********************GPIOC3---->RXD***************/
	/********************GPIOC2---->TXD***************/
	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);

	UARTS_Init(0,  115200, 8,  0,  1);

	GIE_ENABLE();



	DBG("****************************************************************\n");
	DBG("               P2A CHIP Wakeup Module TEST 2016 MVSilicon       \n");
	DBG("****************************************************************\n");



	Sleep_WakeUpSrcFlagGet(&flag1, &flag2,&flag3);
	DBG("[main]WakeUpFlag:FLag0 %08X, Flag1 %08X, Flag2 %08X\n", flag1, flag2,flag3);
	WakeUpFlagClear();

	WaitMs(1000);//暂停1000ms可以在未正确设置唤醒源进入Sleep状态通过上电复位的操作连接JLINK而重新烧录

	Sleep_WakeUpSrcFlagGet(&flag1, &flag2,&flag3);
	DBG("[main]after clear WakeUpFlag:FLag0 %08X, Flag1 %08X,Flag2 %08X\n", flag1, flag2,flag3);

	NVIC_EnableIRQ(Wakeup_IRQn);
	Sleep_WakeUpIntEnable();

#if GPIO_WAKEUP
	//设置GPIOC0/C1作为唤醒源，C0下降沿唤醒,C1上升沿唤醒
	Sleep_WakeUpSrcSet(0,     0,        WAKEUP_SRC2_SLEEP_C0|WAKEUP_SRC2_SLEEP_C1,\
				       0,     0,        (~WAKEUP_POLAR3_C0_HI)|(WAKEUP_POLAR3_C0_HI),\
				       0);//为0不设置电平宽度
	//Sleep_WakeUpTrgTimeSet(5);如果设置了电平宽度，此函数需要随后调用，传入的值不要超过15
#endif


#if RTC_WAKEUP
	//设置RTC为唤醒源，设置后需要设置RTC的闹钟时间
	Sleep_WakeUpSrcSet(  0,     0,        WAKEUP_SRC2_SLEEP_RTC,\
					   0,     0,        0,\
					   0);
	SetNSecondsAlarm(5);

#endif

#if MIX_WAKEUP
	Sleep_WakeUpSrcSet(  0,     0,        WAKEUP_SRC2_SLEEP_C0 | WAKEUP_SRC2_SLEEP_C1 | WAKEUP_SRC2_SLEEP_RTC,\
					   0,     0,        0,\
					   0);
	SetNSecondsAlarm(5);
#endif

	//Sleep_WkUpTrgTime(5);
	DBG("MCU go to sleep now\n");

	GIE_DISABLE();
    Clock_SysClkSelect(RC_CLK_MODE);
    Clock_PllClose();
    Clock_OSC12MDisable();

    Sleep_GoToSleep();

	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	GIE_ENABLE();

	Sleep_WakeUpSrcFlagGet(&flag1, &flag2,&flag3);

	DBG("[main]after clear WakeUpFlag:FLag0 %08X, Flag1 %08X,Flag2 %08X\n", flag1, flag2,flag3);
	WakeUpFlagClear();

	while(1){

	}

}

