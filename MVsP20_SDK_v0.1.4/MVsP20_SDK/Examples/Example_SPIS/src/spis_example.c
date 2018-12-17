/**
 **************************************************************************************
 * @file     SpiSlaveExample.c
 * @brief   Spi Slave
 * 
 * @author  shengqi_zhao
 * @version V1.0.0 	initial release
 * 
 * $Id$
 * $Created: 2016-11-29 17:31:10$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdio.h>
#include <nds32_intrinsic.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "watchdog.h"
#include "debug.h"
#include "spi_flash.h"
#include "timeout.h"
#include "clk.h"
#include "pwm.h"
#include "sleep.h"
#include "delay.h"
#include "rtc.h"
#include "spis.h"
#include "irqn.h"


#define SPIS_DMA_TEST         0
#define SPIS_INTERRUPT_TEST   1

uint8_t SpisSendBuf[513];
uint8_t SpisRecvBuf[4096];
extern const unsigned char spisTestData[4096];

unsigned char Temp;
unsigned char TempBuffer[10];

uint8_t DmaChannelMapSPIS[27] =
{
		0,//PERIPHERAL_ID_SPIS_RX = 0,	//0
		1,//PERIPHERAL_ID_SPIS_TX,		//1
		255,//PERIPHERAL_ID_SD_RX,			//2
		255,//PERIPHERAL_ID_SD_TX,			//3
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


__attribute__((section(".driver.isr")))
void SPIS_Interrupt(void)
{

	if(SPIS_GetRxIntFlag())
	{
		SPIS_ClrRxIntFlag();
		//DBG("int flag =  %d\n",SPIS_GetRxIntFlag( ) );

	}

	if(SPIS_GetRxOverrunFlag())
	{
		//DBG("RX_OVERRUN_CLR\n");
		SPIS_ClrRxOverrunFlag();
	}
}

int main()
{

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

	/********************GPIOC3---->RXD***************/
	/********************GPIOC2---->TXD***************/
	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);

	UARTS_Init(0,  115200, 8,  0,  1);

	SysTickInit();
	GIE_ENABLE();



	DBG("****************************************************************\n");
	DBG("               P2A CHIP SpiSlave Module TEST 2016 MVSilicon       \n");
	DBG("****************************************************************\n");


	Clock_ModuleEnable(ALL_MODULE_CLK_SWITCH);
#if SPIS_DMA_TEST
	SpisDMADataTest();
#else
	Spis_MCU_Datat_Test();
#endif

#if  SPIS_INTERRUPT_TEST
	SpisInterruptTest();
#endif

	while(1){
		//Spis_MCU_Datat_Test();
		//SpisDMADataTest();
		//SpisDMADataTest();

	}

	return 0;
}


void SpisLookForSync()
{
	uint8_t c = 0;
	while(1){
		SPIS_Recv(&c, 1, 0);
		if(c == 0xEB)
			break;
	}
}

void SpisDMALookForSync()
{
	uint8_t c = 0;
	while(1){
		SPIS_DMA_Recv(&c, 1,100);
		if( c == 0xEB)
			break;
	}

}

void SpisInterruptTest(void)
{
	uint8_t c;
	GPIO_SpisIoConfig(1);

	SPIS_Init(0);
	//RX
	SPIS_RX_DMA_MODE_SET(0);
	SPIS_RxIntEn(1);
	NVIC_EnableIRQ(SPIS_IRQn);

	SPIS_Recv(&c, 1, 0);
	if( c == 0xEB ){
		SPIS_Send(&c, 1, 1000);
	}
}

void Spis_MCU_Datat_Test()
{
	uint32_t i;
	GPIO_SpisIoConfig(1);
	SPIS_Init(0);
	SPIS_RX_DMA_MODE_SET(0);//diable DMA,enable MCU send /recv

	while(1)
		{
			memset( SpisRecvBuf, 0x00, sizeof( SpisRecvBuf ));
		    SpisLookForSync();
		    i = SPIS_Recv(SpisRecvBuf, 4096-1, 10000);

		    if(  i!= ( 4096-1 ) ){
		    	DBG("does not recieve enough data \n");

		    }

		    if(memcmp(SpisRecvBuf,&spisTestData[1], 4096-1) == 0)
			{
				printf("SPIS MCU recv ok\n\n");

			}
			else
			{
				printf("SPIS MCU recv error\n\n");
				DBG("\n");
				for( i =0 ; i < 4096-1; i++)
					DBG(" %02x ", SpisRecvBuf[i]);
				DBG("\n");
			}

			SPIS_Send((uint8_t*)spisTestData, 4096, 10000);
		}
}


uint8_t recv_buffer[8192];
uint8_t send_buffer[8192];
void SpisDMADataTest()
{
	uint32_t i;

	GPIO_SpisIoConfig(1);
	SPIS_Init(0);


	DMA_ChannelAllocTableSet(DmaChannelMapSPIS);
	if( SPIS_DMA_Init(recv_buffer,8192, send_buffer,8192) )
	{
		printf("spis dma int ok\n");
	}

	 SPIS_RX_DMA_MODE_SET(1);//enable DMA RX
	//DMA
	while(1)
	{
		SPIS_RX_DMA_MODE_SET(0);//disable DMA RX
		memset( SpisRecvBuf, 0x00, sizeof( SpisRecvBuf ));
		SpisDMALookForSync();

		SPIS_DMA_Recv(SpisRecvBuf, 4096 - 1, 10000);

		if( memcmp( SpisRecvBuf, &spisTestData[ 1 ], 4096 - 1 ) == 0)
		{
			printf("SPIS DMA recv ok\n\n");
		}
		else
		{
			DBG("\n");
			for( i =0 ; i < 4096-1; i++)
					DBG(" %02x ", SpisRecvBuf[i]);
			DBG("\n");
			printf("SPIS DMA recv error\n\n");
		}

		SPIS_DMA_Send( ( uint8_t* ) spisTestData, 4096, 10000);
	}

}



