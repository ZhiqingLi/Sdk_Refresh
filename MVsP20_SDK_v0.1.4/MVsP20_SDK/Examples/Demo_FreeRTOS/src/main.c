#include <stdio.h>
#include <nds32_intrinsic.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "watchdog.h"
#include "get_bootup_info.h"
#include "uarts_interface.h"
#include "debug.h"
#include "irqn.h"
#include "spi_flash.h"
#include "timeout.h"
#include "clk.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "type.h"
#include "delay.h"

#define SET_LP_ROM_REG(x) \
do{		\
	REG_LP_ACCESS_PROTECT = 0xFF;	\
	REG_LP_USER_DEFINED_REG0 = x;	\
	REG_LP_ACCESS_PROTECT = 0x0;	\
}while(0);


xQueueHandle xQueue;

uint32_t SendCount = 0;
uint32_t RecvCount = 0;
uint32_t result[100];

void SDIOAppTask(void);
void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions );

void prvInitialiseHeap(void)
{
	extern char _end;
	HeapRegion_t xHeapRegions[2];

	xHeapRegions[0].pucStartAddress = (uint8_t*)&_end;
	xHeapRegions[0].xSizeInBytes = 0x20020000-(uint32_t)&_end;

	xHeapRegions[1].pucStartAddress = NULL;
	xHeapRegions[1].xSizeInBytes = 0;

	vPortDefineHeapRegions( (HeapRegion_t *)xHeapRegions );
}

void SendTask(void)
{
	while(1){
	DelayMs(500);
	SendCount++;
	DBG("SendMsg:0x%x\n",SendCount);
	xQueueSend( xQueue, &SendCount, portMAX_DELAY );
	}
}

void RecvTask(void)
{
	uint32_t temp;
	while(1){
	 xQueueReceive( xQueue, &temp, portMAX_DELAY );
	 DBG("RecvMsg:0x%x\n\n",temp);
	 result[RecvCount++] = temp;
	 if(RecvCount == 100)
		 RecvCount = 0;
	}
}

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

int main()
{
	WDG_Disable();

	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);
	InitZiArea();

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
	DbgUartInit(0,  115200, 8,  0,  1);

	prvInitialiseHeap();

	GIE_ENABLE();

	NVIC_EnableIRQ(SWI_IRQn);

	DBG("****************************************************************\n");
	DBG("               MVsP2_Demo 2015 MVSilicon                         \n");
	DBG("SDK  Version     :%d.%d.%d\n", (GetSdkVer() >> 8) & 0xFF, (GetSdkVer() >> 16) & 0xFF, GetSdkVer() >> 24);
	DBG("Code Size        :%d(%d KB)\n", GetCodeSize(), GetCodeSize() / 1024);
	DBG("****************************************************************\n");

	xQueue = xQueueCreate( 4, sizeof(uint32_t) );

	xTaskCreate( (TaskFunction_t)SendTask, "SendMsgTask", 512, NULL, 1, NULL );

	xTaskCreate( (TaskFunction_t)RecvTask, "RecvMsgTask", 512, NULL, 2, NULL );

	vTaskStartScheduler();

	while(1);

}

