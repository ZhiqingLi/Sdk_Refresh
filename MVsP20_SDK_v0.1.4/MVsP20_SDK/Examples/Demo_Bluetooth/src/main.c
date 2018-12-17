#include <stdio.h>
#include <nds32_intrinsic.h>
#include "cache_tcm.h"
#include "debug.h"
#include "irqn.h"
#include "spi_flash.h"
#include "clk.h"
#include "FreeRTOS.h"
#include "type.h"
#include "gpio.h"

#define SET_LP_ROM_REG(x) \
do{		\
	REG_LP_ACCESS_PROTECT = 0xFF;	\
	REG_LP_USER_DEFINED_REG0 = x;	\
	REG_LP_ACCESS_PROTECT = 0x0;	\
}while(0);

SPI_FLASH_INFO flashinfo;

extern void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions );
extern void DebugUartInit();
extern void wifi_task_init(void);

void prvInitialiseHeap(void)
{
	extern char _end;
	HeapRegion_t xHeapRegions[2];

	xHeapRegions[0].pucStartAddress = (uint8_t*)&_end;
	xHeapRegions[0].xSizeInBytes = 0x20030000-(uint32_t)&_end;

	DBG("xHeapRegions[0].xSizeInBytes = %d\n",xHeapRegions[0].xSizeInBytes);

	xHeapRegions[1].pucStartAddress = NULL;
	xHeapRegions[1].xSizeInBytes = 0;

	vPortDefineHeapRegions( (HeapRegion_t *)xHeapRegions );
}

void InitZiArea(void)
{
	/* Use compiler builtin and memset */
	#define __MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

	extern char _end;
	extern char __bss_start;
	int size;

	/* Clear bss section */
	size = &_end - &__bss_start;
	__MEMSET(&__bss_start, 0, size);
	return;
}

void Clock_Enable(void)
{

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);
	Clock_ModuleEnable(ALL_MODULE_CLK_SWITCH);

}

void NVIC_Configuration(void)
{
	GIE_ENABLE();
	NVIC_EnableIRQ(SWI_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_EnableIRQ(GPIO_IRQn);

	NVIC_SetPriority(SWI_IRQn,4);
	NVIC_SetPriority(TMR1_IRQn,3);
	NVIC_SetPriority(GPIO_IRQn,2);
	NVIC_SetPriority(UART0_IRQn,2);
}
void Cache_Configuration(void)
{
	CacheTcmDeInit();
	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);

	InitZiArea();
	prvInitialiseHeap();

	SpiFlashInit(90000000, 2, 0, 1);
	memcpy(&flashinfo,SpiFlashInfoGet(),sizeof(SPI_FLASH_INFO));
}


int main()
{
	WDG_Disable();
	Clock_Enable();//时钟使能
	GPIO_UartTxIoConfig(0, 1);//C2
	GPIO_UartRxIoConfig(0, 1);//C3
	DbgUartInit(0, 115200, 8, 0, 1);//串口初始化
	DBG("****************************************************************\n");
	DBG("                 MVBluetooth 2017 MVSilicon                     \n");
	DBG("***************************************************************\n");
	Cache_Configuration();//Cache、SpiFlash配置
	NVIC_Configuration();//中断配置

//	FlashFSInit();       //FS文件系统初始化
	StartBtProtStackTask(2048, NULL, 3);
	vTaskStartScheduler();
	while(1);
}
