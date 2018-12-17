#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "irqn.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "clk.h"
#include "gpio.h"
#include "sd_card.h"
#include "timeout.h"
#include "watchdog.h"
#include "debug.h"
#include "spi_flash.h"
#include "timeout.h"
#include "cache_tcm.h"
#include "dma.h"


 uint8_t SDIO_DmaChannelMap[27] =
{
		255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
		255,//PERIPHERAL_ID_SPIS_TX,		//1
		0,//PERIPHERAL_ID_SD_RX,			//2
		1,//PERIPHERAL_ID_SD_TX,			//3
		255,//PERIPHERAL_ID_SPIM0_RX,		//4
		255,//PERIPHERAL_ID_SPIM0_TX,		//5
		255,//PERIPHERAL_ID_SPIM1_RX,		//6
		255,//PERIPHERAL_ID_SPIM1_TX,		//7
		255,//PERIPHERAL_ID_UART0_RX,		//8
		255,//PERIPHERAL_ID_UART1_RX,		//9
		255,//PERIPHERAL_ID_UART2_RX,		//10
		255,//PERIPHERAL_ID_TIMER3,			//11
		255,//PERIPHERAL_ID_TIMER4,			//12
		255,//PERIPHERAL_ID_I2S_TX,			//13
		255,//PERIPHERAL_ID_I2S_RX,			//14
		255,//PERIPHERAL_ID_SARADC,			//15
		255,//PERIPHERAL_ID_UART0_TX,		//16
		255,//PERIPHERAL_ID_UART1_TX,		//17
		255,//PERIPHERAL_ID_UART2_TX,		//18
		255,//PERIPHERAL_ID_TIMER5,			//19
		255,//PERIPHERAL_ID_TIMER6,			//20
		255,//PERIPHERAL_ID_TIMER7,			//21
		255,//PERIPHERAL_ID_TIMER8,			//22
		255,//PERIPHERAL_ID_LACC_SRC,		//23
		255,//PERIPHERAL_ID_LACC_DST,		//24
		255,//PERIPHERAL_ID_LACC_DSTRD,     //25
		255,//PERIPHERAL_ID_SOFTWARE,		//26
};

uint8_t Sec[2048];
int main(void)
{	
//	 unsigned int i = 0;

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

	SpiFlashInit(80000000, MODE_4BIT, 0, 1);

	//注意：晶体如果没有选择，则此时UART波特率不正确
	//GPIO_UartTxIoConfig(0, 0);
	//GPIO_UartRxIoConfig(0, 0);
	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);
	UARTS_Init(0, 115200, 8, 0, 1);

	SysTickInit();

	GIE_ENABLE();

	DBG("------------------------------------------------------\n");
	DBG("            sdio Operation Example                      \n");
	DBG("       Mountain View Silicon Technology Co.,Ltd.               \n");
	DBG("------------------------------------------------------\r\n\r\n");


	GPIO_SDIOConfig(3);

	Clock_ModuleEnable(ALL_MODULE_CLK_SWITCH);

	DBG("SDIO Memory Card Test\n");
	DMA_ChannelAllocTableSet(SDIO_DmaChannelMap);
	if(SDCard_Init() == 0)
	{
		DBG("SDCard Init Success!\n");
	}
	else
	{
		DBG("SdInit Failed!\n");
	}

	memset(Sec,0,2048);
	if(SDCard_ReadBlock(0, Sec, 2) != 0)
	{
		DBG("Sd write error!\n");
	}
	if(SDCard_WriteBlock(0, Sec, 2) != 0)
	{
		DBG("Sd write error!\n");
	}

	if(SDCard_ReadBlock(0, Sec+1024, 2) != 0)
	{
		DBG("Sd write error!\n");
	}

	if(memcmp(Sec+1024,Sec,1024) != 0)
	{
		DBG("Sd write read error!\n");
	}
	else
	{
		DBG("Sd write read ok!\n");
	}
	while(1);
	return 0;
}

