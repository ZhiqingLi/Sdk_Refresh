/**
 **************************************************************************************
 * @file    SpiMasterExample.c
 * @brief   Spi master
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
#include "spim.h"
#include "irqn.h"

#define SPI0_MASTER_DATA_TRANSFER_TEST   1


#define SPIM0_INTERRUPT_TEST   1
#define MCU_RECV   0
#define MCU_SEND   1

#define SPI_SYNC_BYTE 0xEB

#define SPIM0_CS_INIT()		    GPIO_RegOneBitClear(GPIO_A_IE,GPIOA3);\
							    GPIO_RegOneBitSet(GPIO_A_OE,GPIOA3)

#define SPIM0_CS_H()			GPIO_RegOneBitSet(GPIO_A_OUT,GPIOA3)
#define SPIM0_CS_L()			GPIO_RegOneBitClear(GPIO_A_OUT,GPIOA3)
#define SPIM0_ENABLE_SPIS()	    SPIM0_CS_L()
#define SPIM0_DISABLE_SPIS()	SPIM0_CS_H()


#define SPIM1_CS_INIT()		    GPIO_RegOneBitClear(GPIO_A_IE,GPIOA3);\
							    GPIO_RegOneBitSet(GPIO_A_OE,GPIOA3)

#define SPIM1_CS_H()			GPIO_RegOneBitSet(GPIO_A_OUT,GPIOA3)
#define SPIM1_CS_L()			GPIO_RegOneBitClear(GPIO_A_OUT,GPIOA3)
#define SPIM1_ENABLE_SPIS()	    SPIM1_CS_L()
#define SPIM1_DISABLE_SPIS()	SPIM1_CS_H()

uint8_t SpimRecvBuf[8192];
uint32_t SPIMErrorTimes = 0;
uint32_t LoopTime = 1;
extern const unsigned char TestData[4096];

static void SpimLookForSync();
static void SpimDMALookForSync();

static void Spim0InterruptTest();

static void Spim0DataTransfer();


static uint8_t DmaChannelMap[27] =
{
	255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
	255,//PERIPHERAL_ID_SPIS_TX,		//1
	255,//PERIPHERAL_ID_SD_RX,			//2
	255,//PERIPHERAL_ID_SD_TX,			//3
	0,//PERIPHERAL_ID_SPIM0_RX,		//4
	0,//PERIPHERAL_ID_SPIM0_TX,		//5
	1,//PERIPHERAL_ID_SPIM1_RX,		//6
	1,//PERIPHERAL_ID_SPIM1_TX,		//7
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
void SPIM0_Interrupt(void)
{
	if( SPIM0_GetIntFlag() )
	{
		SPIM0_ClrIntFlag();
		DBG("CLR_INTF\n");
	}
	if( SPI0_Get_OverWr( ) )
	{
		SPI0_Set_OverWr();
		DBG("over\n");
	}
}


__attribute__((section(".driver.isr")))
void SPIM1_Interrupt(void)
{
	if(SPIM1_GetIntFlag() )
	{
		SPIM0_ClrIntFlag();
		DBG("CLR_INTF\n");
	}
	if( SPI1_Get_OverWr() )
	{
		SPI1_Set_OverWr();
		DBG("over\n");
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

	GIE_ENABLE();



	DBG("****************************************************************\n");
	DBG("               P2A CHIP SpiMaster Module TEST 2016 MVSilicon       \n");
	DBG("****************************************************************\n");

	Clock_ModuleEnable(ALL_MODULE_CLK_SWITCH);
#if SPI0_MASTER_DATA_TRANSFER_TEST
	Spim0DataTransfer();//This example show the DMA and MCU recv test
#else
	/*
	 * master send 0xEB,
	 * slave send the same byte to activate the interrupt
	 */
	while(1){
		Spim0InterruptTest();
	}
#endif



#if SPI1_MASTER_DATA_TRANSFER_TEST //test mode is the same as SPI0

#endif

	while(1){

	}

	return 0;//cheat the compiler
}




static void SpimLookForSync()
{
	uint8_t c = 0;
	while(1)
	{
		SPIM0_ENABLE_SPIS();
		SPIM0_RecvNBytes(&c, 1);
		SPIM0_DISABLE_SPIS();

		if(c == SPI_SYNC_BYTE)
		{
			break;
		}
	}


}

static void SpimDMALookForSync()
{
	uint8_t c;
	while(1){
		SPIM0_ENABLE_SPIS();
		SPIM0_DMA_RecvData(&c, 1);
		SPIM0_DISABLE_SPIS();

	    if(c == SPI_SYNC_BYTE)
		   break;
	}
}

#if 0
static void Spim1LookForSync()
{
	uint8_t c = 0;
	while(1)
	{
		SPIM1_ENABLE_SPIS();
		SPIM1_RecvNBytes(&c, 1);
		SPIM1_DISABLE_SPIS();

		if(c == SPI_SYNC_BYTE)
		{
			break;
		}
	}


}
#endif

//SPIM MCU Interrupt
static void Spim0InterruptTest()
{
	uint8_t i;

	DBG("INT TEST START\n");
	NVIC_EnableIRQ(SPIM0_IRQn);

	GPIO_Spim0IoConfig(0);

	SPIM0_CS_INIT();
	SPIM0_CS_H();
	if(SPIM0_Init(0,0))
	{
		DBG("spi0 master init success!\n");
	}

	SPIM0_SendByte(0xEB);
	SPIM0_IntEn();
	DBG("\n\nFifo Test When Recv\n");

	i = SPIM0_RecvByte();
	DBG("Int test recv data = %x\n",i);
	//while(1);
}

#if 0
//SPIM MCU Interrupt,same as spim0
static void Spim1InterruptTest()
{
	uint8_t i;
	DBG("SPI1 INTERRUPT TEST START\n");

	NVIC_EnableIRQ(SPIM1_IRQn);

	//SREG_SPIM0_CTRL.ICNT = 3;
	GPIO_Spim1IoConfig(1);
	SPIM1_CS_INIT();
	if(SPIM1_Init(0,0))
	{
		DBG("spi1 master init success!\n");
	}


//TX INT

	SPIM1_SendByte(0x56);
	SPIM1_IntEn();

	DBG("\n\n Interrupt Test When Recv\n");
	//while(1)
	{
		i = SPIM1_RecvByte();
	}
}
#endif




static void Spim0DataTransfer()
{

/*
 *
 0: spim0 port#0
    spim0_miso b[0]
    spim0_clk  b[1]
    spim0_mosi b[2]
 1: spim0 port#1
    spim0_miso b[27]
    spim0_clk  b[28]
    spim0_mosi b[29]
 2: spim0 port#1
    spim0_miso b[20]
    spim0_clk  b[21]
    spim0_mosi b[22]
 * */


	uint32_t i;


	DBG("Spim0 Data Transfer start\n");

	SPIM0_CS_INIT();
	GPIO_Spim0IoConfig(0);
	if(SPIM0_Init(0,11))
	{
		DBG("spi0 master init success!\n");
	}
	DBG("read write by spim0\n");
	//read write by mcu;

#if ((!MCU_SEND)||(!MCU_RECV))
	DMA_ChannelAllocTableSet(DmaChannelMap);
#endif

	while(1)
	{

#if !MCU_SEND
		DBG("MCU write and recv data\n");
		SPIM0_ENABLE_SPIS();
		SPIM0_SendNBytes( ( uint8_t* )TestData,4096);
		SPIM0_DISABLE_SPIS();


		memset(SpimRecvBuf, 0x00 ,sizeof(SpimRecvBuf));

		SpimLookForSync();
		SPIM0_ENABLE_SPIS();
		SPIM0_RecvNBytes(SpimRecvBuf, 4096-1);
		SPIM0_DISABLE_SPIS();

		if(memcmp(SpimRecvBuf, &TestData[1], 4096-1) == 0){
			DBG("SPI 0 master MCU recv ok %d\n",i);
		}else{
				DBG("\n");
				for( i =0 ; i < 4095; i++)
					DBG(" %02x ", SpimRecvBuf[i]);

				DBG("SPI 0 master MCU recv error\n");
		}

#else
		DBG("DMA send and write data\n");
		SPIM0_ENABLE_SPIS();
		SPIM0_DMA_SendData((uint8_t*)TestData,4096);
		SPIM0_DISABLE_SPIS();

		SpimDMALookForSync();
		SPIM0_ENABLE_SPIS();
		SPIM0_DMA_RecvData(SpimRecvBuf, 4096 - 1);
		SPIM0_DISABLE_SPIS();
		WaitMs(10);


		if(memcmp(SpimRecvBuf, &TestData[1], 4096-1) == 0)
		{
			DBG("SPI 0 master DMA recv ok \n");
		}
		else
		{
			DBG("\n");
			for( i =0 ; i < 4095; i++)
				DBG(" %02x ", SpimRecvBuf[i]);
			DBG("SPI 0 master MCU recv error\n");

		}
		WaitMs(10);


#endif



	}
}

#if 0
static void Spim1DataTransfer()
{

	uint32_t k,kk,kkk,i;

//    spim1_miso c[0]
//    spim1_clk  c[1]
//    spim1_mosi c[2]

	kkk = 0;
	DBG("Spim1 Data Transfer start\n");
	SPIM1_CS_INIT();
	if(SPIM1_Init(3,0))
	{
		kprintf("spi init success!\n");
	}
	DBG("read write by mcu\n");
	//read write by mcu;
	DMA_ChannelAllocTableSet(DmaChannelMap);
	while(1)
	{
#if MCU_RECV
		Spim1LookForSync();
		SPIM1_ENABLE_SPIS();
		SPIM1_RecvNBytes(SpimRecvBuf, 4096-1);
		SPIM1_DISABLE_SPIS();



		if(memcmp(SpimRecvBuf,&TestData[1],4096-1) == 0)
		{
			printf("ok %d\n",i);
		}
		else
		{
			printf("error\n");
		}
#endif
#if DMA_RECV
		SpimLookForSync();
		memset( SpimRecvBuf,0x00, sizeof( SpimRecvBuf ) );
		SPIM1_ENABLE_SPIS();
		SPIM1_DMA_RecvData( SpimRecvBuf, 4096 - 1 );
		SPIM1_DISABLE_SPIS();

		if(memcmp(SpimRecvBuf, &TestData[1],  4096-1) == 0)
		{
			DBG("ok %d\n",i);
		}
		else
		{
			DBG("error\n");
		}
		WaitMs(10);
#endif
	}
}
#endif

