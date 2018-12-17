/**
 **************************************************************************************
 * @file    i2s_example.c
 * @brief   I2S Example
 *
 * @author  Aissen Li
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdio.h>
#include "type.h"
#include "cache_tcm.h"
#include "clk.h"
#include "i2s.h"
#include "gpio.h"
#include "watchdog.h"
#include "debug.h"
#include "delay.h"
#include "uarts_interface.h"
#include "spi_flash.h"

#define UART_GPIO_GROUP_SEL						(0)
#define UART_GPIO_MODEL_SEL						(1)

#define I2S_MODE								(0)   //0: SLAVE, 1:MASTER
#define I2S_MODE_SEL							(0)
#define I2S_GPIO_SEL							(1)   //0: B7-B11, 1: B12-B15,B20
#define I2S_SAMPLE_RATE							(44100)
#define I2S_FRAME_SAMPLES						(192)
#define I2S_FORMAT								(I2S_FORMAT_I2S)
#define I2S_DATA_WIDTH							(I2S_LENGTH_16BITS)
#define I2sTxDmaBufferLen						(sizeof(I2sTxDmaBuffer))
#define I2sRxDmaBufferLen						(sizeof(I2sRxDmaBuffer))
uint8_t I2sTxDmaBuffer[I2S_FRAME_SAMPLES * 8];
uint8_t I2sRxDmaBuffer[I2S_FRAME_SAMPLES * 8];

static uint8_t DMAChannelAllocMap[31] =
{
	255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
	255,//PERIPHERAL_ID_SPIS_TX,		//1
	255,//PERIPHERAL_ID_SD_RX,			//2
	255,//PERIPHERAL_ID_SD_TX,			//3
	255,//PERIPHERAL_ID_SPIM0_RX,		//4
	255,//PERIPHERAL_ID_SPIM0_TX,		//5
	255,//PERIPHERAL_ID_SPIM1_RX,		//6
	255,//PERIPHERAL_ID_SPIM1_TX,		//7
	255,//PERIPHERAL_ID_UART0_RX,		//8
	0,	//PERIPHERAL_ID_UART1_RX,		//9
	255,//PERIPHERAL_ID_UART2_RX,		//10
	3,	//PERIPHERAL_ID_TIMER3,			//11
	7,	//PERIPHERAL_ID_TIMER4,			//12
	6,	//PERIPHERAL_ID_I2S_TX,			//13
	5,	//PERIPHERAL_ID_I2S_RX,			//14
	4,	//PERIPHERAL_ID_SARADC,			//15
	255,//PERIPHERAL_ID_UART0_TX,		//16
	1,	//PERIPHERAL_ID_UART1_TX,		//17
	255,//PERIPHERAL_ID_UART2_TX,		//18
	255,//PERIPHERAL_ID_TIMER5,			//19
	255,//PERIPHERAL_ID_TIMER6,			//20
	255,//PERIPHERAL_ID_TIMER7,			//21
	255,//PERIPHERAL_ID_TIMER8,			//22
	255,//PERIPHERAL_ID_LACC_SRC,		//23
	255,//PERIPHERAL_ID_LACC_DST,		//24
	255,//PERIPHERAL_ID_LACC_DSTRD,     //25
	2,	//PERIPHERAL_ID_SOFTWARE,		//26
};

int main(void)
{
	int8_t I2sMode = I2S_MODE;
	int8_t I2sFrameBuffer[I2S_FRAME_SAMPLES * 4];

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

	GPIO_UartTxIoConfig(UART_GPIO_GROUP_SEL, UART_GPIO_MODEL_SEL);
	GPIO_UartRxIoConfig(UART_GPIO_GROUP_SEL, UART_GPIO_MODEL_SEL);

	UARTS_Init(UART_GPIO_GROUP_SEL, 115200, 8, 0, 1);

	DBG("\n");
    DBG("/-----------------------------------------------------\\\n");
    DBG("|                     I2S Example                     |\n");
    DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
    DBG("\\-----------------------------------------------------/\n");
    DBG("\n");

    Clock_ModuleEnable(DMA_CLK_EN | I2S_CLK_EN);
    DMA_ChannelAllocTableSet(DMAChannelAllocMap);

    GPIO_I2sBclkLrclkConfig(I2S_GPIO_SEL);
    GPIO_I2sDoutConfig(I2S_GPIO_SEL);
    GPIO_I2sDinConfig(I2S_GPIO_SEL);

    DMA_ChannelEnable(PERIPHERAL_ID_I2S_TX, 0);
    DMA_ChannelEnable(PERIPHERAL_ID_I2S_RX, 0);
    DMA_CircularConfig(PERIPHERAL_ID_I2S_TX, I2sTxDmaBufferLen / 2, I2sTxDmaBuffer, I2sTxDmaBufferLen);
    DMA_CircularConfig(PERIPHERAL_ID_I2S_RX, I2sRxDmaBufferLen / 2, I2sRxDmaBuffer, I2sRxDmaBufferLen);
    DMA_ChannelEnable(PERIPHERAL_ID_I2S_TX, 1);
    DMA_ChannelEnable(PERIPHERAL_ID_I2S_RX, 1);

    Clock_MclkConfig(I2S_MODE_SEL, 0x2710);//0x2710该参数只在I2S_MODE_SEL为3的时候才起效

    if(I2sMode == 0)
    {
    	GPIO_MclkIoConfig(I2S_GPIO_SEL);
    	I2S_SampleRateCheckEnable(1);
    	I2S_SetSlaveMode(I2S_FORMAT, I2S_DATA_WIDTH);
    }
    else
    {
    	GPIO_MclkIoConfig(2 + I2S_GPIO_SEL);
    	I2S_SampleRateSet(I2S_SAMPLE_RATE);
    	I2S_SetMasterMode(I2S_FORMAT, I2S_DATA_WIDTH);
    }

    I2S_ModuleEnable(1);

    while(1)
    {
    	uint8_t Key;

    	if(DMA_CircularDataLenGet(PERIPHERAL_ID_I2S_RX) >= sizeof(I2sFrameBuffer))
    	{
    		DMA_CircularDataGet(PERIPHERAL_ID_I2S_RX, I2sFrameBuffer, sizeof(I2sFrameBuffer));
    		DMA_CircularDataPut(PERIPHERAL_ID_I2S_TX, I2sFrameBuffer, sizeof(I2sFrameBuffer));
    		//DBG(".");
    	}

    	if(I2sMode == 0)
    	{
    		if(I2S_GetSampleRateCheckInt())
    		{
    			I2S_ClrSampleRateCheckInt();
    			DBG("[I2S]: Sample Rate ---> %d\n", I2S_SampleRateGet());
    		}
    	}

    	if(UARTS_Recv(UART_GPIO_GROUP_SEL, &Key, 1) > 0)
    	{
    		switch(Key)
    		{
    		case 'm':
    		case 'M':
    			I2sMode = 1;
    			GPIO_MclkIoConfig(2 + I2S_GPIO_SEL);
    			I2S_SampleRateSet(I2S_SAMPLE_RATE);
    			I2S_SetMasterMode(I2S_FORMAT, I2S_DATA_WIDTH);
    			DBG("[I2S]: I2S Mode ---> Master\n");
    			break;
    		case 's':
    		case 'S':
    			I2sMode = 0;
    			GPIO_MclkIoConfig(I2S_GPIO_SEL);
    			I2S_SampleRateCheckEnable(1);
    			I2S_SetSlaveMode(I2S_FORMAT, I2S_DATA_WIDTH);
    			DBG("[I2S]: I2S Mode ---> Slaver\n");
    			break;
    		default:
    			break;
    		}
    	}
    }

    return 0;
}
