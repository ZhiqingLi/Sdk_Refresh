////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2016, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:dma_example.c
//		Author：Peter zhai
//
//		Description	:
//					create example to show how to use dma
//
//		Changelog	:	
//					2016-11-24	create dma example
///////////////////////////////////////////////////////////////////////////////
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
#include "dma.h"
#include "timer.h"
#include "irqn.h"
#include "spi_flash.h"

static uint8_t DmaChannelMap[31] =
{
	255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
	255,//PERIPHERAL_ID_SPIS_TX,		//1
	255,//PERIPHERAL_ID_SD_RX,			//2
	255,//PERIPHERAL_ID_SD_TX,			//3
	255,//PERIPHERAL_ID_SPIM0_RX,		//4
	255,//PERIPHERAL_ID_SPIM0_TX,		//5
	255,//PERIPHERAL_ID_SPIM1_RX,		//6
	255,//PERIPHERAL_ID_SPIM1_TX,		//7
	0,//PERIPHERAL_ID_UART0_RX,		//8
	255,//PERIPHERAL_ID_UART1_RX,		//9
	255,//PERIPHERAL_ID_UART2_RX,		//10
	3,//PERIPHERAL_ID_TIMER3,			//11
	255,//PERIPHERAL_ID_TIMER4,			//12
	255,//PERIPHERAL_ID_I2S_TX,			//13
	255,//PERIPHERAL_ID_I2S_RX,			//14
	255,//PERIPHERAL_ID_SARADC,			//15
	1,//PERIPHERAL_ID_UART0_TX,		//16
	255,	//PERIPHERAL_ID_UART1_TX,	//17
	255,//PERIPHERAL_ID_UART2_TX,		//18
	255,//PERIPHERAL_ID_TIMER5,			//19
	255,//PERIPHERAL_ID_TIMER6,			//20
	255,//PERIPHERAL_ID_TIMER7,			//21
	255,//PERIPHERAL_ID_TIMER8,			//22
	255,//PERIPHERAL_ID_LACC_SRC,		//23
	255,//PERIPHERAL_ID_LACC_DST,		//24
	255,//PERIPHERAL_ID_LACC_DSTRD,     //25
	2,//PERIPHERAL_ID_SOFTWARE,			//26
};

uint8_t DmaTestBuf[512];
uint32_t  DmaTestLen;

DMA_ERROR DMA_MemCopy(uint32_t SrcAddr, uint32_t DstAddr, uint16_t Length, uint8_t SrcDwidth, uint8_t DstDwidth, uint8_t SrcAincr, uint8_t DstAincr);


static void DmaTestBMM()              //0
{
	//BLOCK MEM2MEM
	uint32_t i, n, error_flag = 0;
	uint32_t DmaTestSrcAddr,DmaTestDstAddr;
	DBG("---------DMA BLOCK MEM2MEM TEST-----------\n");
	DBG("先赋值00~FF到源内存块，再使用DMA将源内存内容传输到目的内存块\n");
	DmaTestSrcAddr = 0x20008000;
	DmaTestDstAddr = 0x20018000;
	n = 65532;
	for(i = 0; i < n; i++)
	{
		((uint8_t *)DmaTestSrcAddr)[i] = (uint8_t)i;
	}

	for(i = 0; i < n; i++)
	{
		((uint8_t *)DmaTestDstAddr)[i] = (uint8_t)0;
	}

	DMA_MemCopy(DmaTestSrcAddr, DmaTestDstAddr, n, DMA_SRC_DWIDTH_BYTE, DMA_DST_DWIDTH_BYTE, DMA_SRC_AINCR_SRC_WIDTH, DMA_DST_AINCR_DST_WIDTH);
	while(!DMA_InterruptFlagGet(PERIPHERAL_ID_SOFTWARE, DMA_DONE_INT));
	DMA_InterruptFlagClear(PERIPHERAL_ID_SOFTWARE, DMA_DONE_INT);
	for(i = 0; i < n; i++)
	{
		if(((uint8_t *)DmaTestSrcAddr)[i] != ((uint8_t *)DmaTestDstAddr)[i])
		{
			DBG("Error Happend %d != %d\n", ((uint8_t *)DmaTestSrcAddr)[i], ((uint8_t *)DmaTestDstAddr)[i]);
			error_flag = 1;
		}
	}

	DMA_MemCopy(DmaTestSrcAddr, DmaTestDstAddr, n, DMA_SRC_DWIDTH_WORD, DMA_DST_DWIDTH_WORD, DMA_SRC_AINCR_SRC_WIDTH, DMA_DST_AINCR_DST_WIDTH);
	while(!DMA_InterruptFlagGet(PERIPHERAL_ID_SOFTWARE, DMA_DONE_INT));
	DMA_InterruptFlagClear(PERIPHERAL_ID_SOFTWARE, DMA_DONE_INT);
	for(i = 0; i < n; i++)
	{
		if(((uint8_t *)DmaTestSrcAddr)[i] != ((uint8_t *)DmaTestDstAddr)[i])
		{
			DBG("Error Happend %d != %d\n", ((uint8_t *)DmaTestSrcAddr)[i], ((uint8_t *)DmaTestDstAddr)[i]);
			error_flag = 1;
		}
	}

	if(!error_flag)
	{
		DBG("success! no error happen!\n");
	}
}

static void DmaBlockMem2Peri(void)
{
	uint32_t n, i;
	DBG("---------DMA Block模式下 Memory到外设示例，使用查询模式-----------\n");
	DBG("Uart0 Tx配成了DMA Block模式\n");
	DBG("uart0在dma模式下循环向外发送字符\n");

	n = 10;
	for(i=0;i<n;i++)
	{
		DmaTestBuf[i] = i+'0';
	}

	DMA_BlockConfig(PERIPHERAL_ID_UART0_TX);
	UART0_IOCtl(UART_IOCTL_DMA_TX_EN, 1);

	DMA_BlockBufSet(PERIPHERAL_ID_UART0_TX, DmaTestBuf, n);
	DMA_ChannelEnable(PERIPHERAL_ID_UART0_TX, 1);

	while(1)
	{
		while(!DMA_InterruptFlagGet(PERIPHERAL_ID_UART0_TX, DMA_DONE_INT));
		DMA_InterruptFlagClear(PERIPHERAL_ID_UART0_TX, DMA_DONE_INT);

		DMA_BlockBufSet(PERIPHERAL_ID_UART0_TX, DmaTestBuf, n);
		DMA_ChannelEnable(PERIPHERAL_ID_UART0_TX, 1);
	}

}

void InterruptBlockPeri2Mem(void)
{
	uint32_t n=10;
	DBG("Enter UART0 DMA接收中断\n");
	DMA_InterruptFlagClear(PERIPHERAL_ID_UART0_RX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_UART0_RX, DMA_THRESHOLD_INT);

	DBG("UART0 DMA收到的数据为：\n");
	DBG(DmaTestBuf);

	memset(DmaTestBuf, 0, n+1);

	DMA_BlockBufSet(PERIPHERAL_ID_UART0_RX, DmaTestBuf, n);
	DMA_ChannelEnable(PERIPHERAL_ID_UART0_RX, 1);
}

static void DmaBlockPeri2Mem(void)
{
	uint32_t n;
	DBG("---------DMA BLOCK模式下外设到Memory，使用中断模式-----------\n");
	DBG("Uart0 Rx配制成了DMA Block模式\n");

	n = 10;
	DMA_BlockConfig(PERIPHERAL_ID_UART0_RX);

	GIE_ENABLE();
	DMA_InterruptFunSet(PERIPHERAL_ID_UART0_RX, DMA_DONE_INT, InterruptBlockPeri2Mem);
	DMA_InterruptEnable(PERIPHERAL_ID_UART0_RX, DMA_DONE_INT, 1);

	UART0_IOCtl(UART_IOCTL_DMA_RX_EN, 1);

	DMA_BlockBufSet(PERIPHERAL_ID_UART0_RX, DmaTestBuf, n);
	DMA_ChannelEnable(PERIPHERAL_ID_UART0_RX, 1);

	while(1);
}


uint8_t cbt_n = 0;
static void InterruptCirclePeri2Mem(void)
{
	uint8_t buf[20];
	DBG("Enter DMA Int\n");
	memset(buf, 0, 20);
	DMA_CircularDataGet(PERIPHERAL_ID_UART0_RX, buf, DMA_CircularThresholdLenGet(PERIPHERAL_ID_UART0_RX));
	DBG("DMA收到的数据为：\n");
	DBG(buf);
	cbt_n++;
	if(cbt_n == 10)
	{
		cbt_n = 1;
	}
	DMA_CircularThresholdLenSet(PERIPHERAL_ID_UART0_RX, cbt_n);
	DBG("\n水位设置为:%d\n",cbt_n);
	DMA_InterruptFlagClear(PERIPHERAL_ID_UART0_RX, DMA_THRESHOLD_INT);
}


static void DmaCirclePeri2Mem(void)
{

	DBG("---------DMA CIRCULAR模式下外设到Memory-----------\n");
	DBG("将uart0 rx配置成dma circular模式，1/2水位，从uart0输入n/2个字节后会从uart0打印出来\n");
	DmaTestLen = 10;

	GIE_ENABLE();
	NVIC_EnableIRQ(UART0_IRQn);
	UART0_IOCtl(UART_IOCTL_DMA_RX_EN, 1);

	DMA_CircularConfig(PERIPHERAL_ID_UART0_RX, DmaTestLen/2, DmaTestBuf, DmaTestLen);
	DMA_InterruptFunSet(PERIPHERAL_ID_UART0_RX, DMA_THRESHOLD_INT, InterruptCirclePeri2Mem);
	DMA_InterruptEnable(PERIPHERAL_ID_UART0_RX, DMA_THRESHOLD_INT, 1);
	DMA_ChannelEnable(PERIPHERAL_ID_UART0_RX, 1);

	while(1);
}


static void DmaCircleMem2Peri(void)
{
	uint32_t i;
	uint8_t buf[20];
	DBG("---------DMA CIRCULAR MEM2PERI TEST-----------\n");
	DBG("将uart1 tx配制成DMA Circular模式，1/2水位，测试开始后将不断从uart1打印00~FF\n");
	DmaTestLen = 20;

	memset(buf, 0, 20);

	memcpy(buf, "1234567890", 10);

	UART0_IOCtl(UART_IOCTL_DMA_TX_EN, 1);
	DMA_CircularConfig(PERIPHERAL_ID_UART0_TX, DmaTestLen/2, DmaTestBuf, DmaTestLen);

	DMA_ChannelEnable(PERIPHERAL_ID_UART0_TX, 1);
	DMA_CircularDataPut(PERIPHERAL_ID_UART0_TX, DmaTestBuf, DmaTestLen/2);
	while(1)
	{
		while(!DMA_InterruptFlagGet(PERIPHERAL_ID_UART0_TX, DMA_THRESHOLD_INT));
		DMA_CircularDataPut(PERIPHERAL_ID_UART0_TX, buf, DmaTestLen / 2);
		DMA_InterruptFlagClear(PERIPHERAL_ID_UART0_TX, DMA_THRESHOLD_INT);
	}
}

#define ADDR_GPIO_B_REG_O  0x40010038
#define REG_TIMER3_CTRL    *(volatile uint32_t*)0x4002B000

static void DmaTimerCircleMode(void)
{
	//Timer3触发DMA Request， 将memory数据toggle到GPIOB
	//可以从示波器上GPIOB5上看到方波，周期为TIMER3定时的2倍
	uint32_t i;
	DMA_CONFIG DMAConfig;

	for(i = 0; i < 128 ; i++)
	{
		DmaTestBuf[i*4]   = 0x55;
		DmaTestBuf[i*4+1] = 0xAA;
		DmaTestBuf[i*4+2] = 0x55;
		DmaTestBuf[i*4+3] = 0xAA;
	}

	GPIO_RegOneBitSet(GPIO_B_OE, GPIOB5);
	GPIO_RegOneBitClear(GPIO_B_IE, GPIOB5);
	DmaTestLen = 512;

	DMAConfig.Dir = DMA_CHANNEL_DIR_MEM2PERI;
	DMAConfig.Mode = DMA_CIRCULAR_MODE;
	DMAConfig.SrcAddress = (uint32_t)DmaTestBuf;
	DMAConfig.SrcDataWidth = DMA_SRC_DWIDTH_BYTE;
	DMAConfig.SrcAddrIncremental = DMA_SRC_AINCR_SRC_WIDTH;
	DMAConfig.DstAddress = ADDR_GPIO_B_REG_O;
	DMAConfig.DstDataWidth = DMA_DST_DWIDTH_BYTE;
	DMAConfig.DstAddrIncremental = DMA_DST_AINCR_NO;
	DMAConfig.BufferLen = DmaTestLen;
	DMA_TimerConfig(PERIPHERAL_ID_TIMER3, &DMAConfig);

	DMA_CircularWritePtrSet(PERIPHERAL_ID_TIMER3, 2000);//将写指针指向buffer外
	DMA_ChannelEnable(PERIPHERAL_ID_TIMER3, 1);

	Timer_Config(TIMER3, 500, 0);
	Timer_Start(TIMER3);
	REG_TIMER3_CTRL |= (1<<8);
	while(1);

}

static void DmaTimerBlockMode(void)
{
	//Timer3触发DMA Request， 将memory数据toggle到GPIOB
	//可以从示波器上GPIOB5上看到方波，周期为TIMER3定时的2倍
	uint32_t i;
	DMA_CONFIG DMAConfig;

	for(i = 0; i < 128 ; i++)
	{
		DmaTestBuf[i*4]   = 0x55;
		DmaTestBuf[i*4+1] = 0xAA;
		DmaTestBuf[i*4+2] = 0x55;
		DmaTestBuf[i*4+3] = 0xAA;
	}

	GPIO_RegOneBitSet(GPIO_B_OE, GPIOB5);
	GPIO_RegOneBitClear(GPIO_B_IE, GPIOB5);
	DmaTestLen = 512;

	DMAConfig.Dir = DMA_CHANNEL_DIR_MEM2PERI;
	DMAConfig.Mode = DMA_BLOCK_MODE;
	DMAConfig.SrcAddress = (uint32_t)DmaTestBuf;
	DMAConfig.SrcDataWidth = DMA_SRC_DWIDTH_BYTE;
	DMAConfig.SrcAddrIncremental = DMA_SRC_AINCR_SRC_WIDTH;
	DMAConfig.DstAddress = ADDR_GPIO_B_REG_O;
	DMAConfig.DstDataWidth = DMA_DST_DWIDTH_BYTE;
	DMAConfig.DstAddrIncremental = DMA_DST_AINCR_NO;
	DMAConfig.BufferLen = DmaTestLen;
	DMA_TimerConfig(PERIPHERAL_ID_TIMER3, &DMAConfig);

	Timer_Config(TIMER3, 1000, 0);
	REG_TIMER3_CTRL |= (1<<8);

	DMA_BlockBufSet(PERIPHERAL_ID_TIMER3, DmaTestBuf, DmaTestLen);
	DMA_ChannelEnable(PERIPHERAL_ID_TIMER3, 1);
	Timer_Start(TIMER3);

	while(1)
	{
		while(!DMA_InterruptFlagGet(PERIPHERAL_ID_TIMER3, DMA_DONE_INT));
		DMA_InterruptFlagClear(PERIPHERAL_ID_TIMER3, DMA_DONE_INT);

		DMA_BlockBufSet(PERIPHERAL_ID_TIMER3, DmaTestBuf, DmaTestLen);
		DMA_ChannelEnable(PERIPHERAL_ID_TIMER3, 1);
	}

}

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
	UARTS_Init(0, 115200, 8, 0, 1);

	DMA_ChannelAllocTableSet(&DmaChannelMap[0]);

	//Dma block memory to memory
	DmaTestBMM();

	// Dma block memory to peripheral and peripheral to memory
	DmaBlockMem2Peri();

	DmaBlockPeri2Mem();

	// Dma circular peripheral to memory
	DmaCirclePeri2Mem();

	// Dma circular memory to peripheral
	DmaCircleMem2Peri();

	// Timer trigger DMA toggle GPIO
	DmaTimerCircleMode();

	// Timer trigger DMA toggle GPIO
	DmaTimerBlockMode();

	while(1);
}


