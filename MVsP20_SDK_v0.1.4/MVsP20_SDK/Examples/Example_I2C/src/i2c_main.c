/**
 **************************************************************************************
 * @file    i2c_main.c
 * @brief   i2c operation example
 * 
 * @author  KK
 * @version V1.0.0 	initial release
 * 
 * $Id$
 * $Created: 2016-11-28 17:31:10$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "clk.h"
#include "gpio.h"
#include "debug.h"
#include "delay.h"
#include "timer.h"
#include "irqn.h"
#include "cache_tcm.h"
#include "timeout.h"
#include "uarts_interface.h"
#include "i2c_interface.h"
#include "watchdog.h"
#include "spi_flash.h"


#define MASTER_SLAVE	1//0:SLAVE MODE  1:MASTER MODE
#define SLAVE_INT		1//0:NO INT      1:INT

#define P20_I2C_MASTER_ADDR		0X04
#define P20_I2C_SLAVE_ADDR		0X13

#define PACKET_LEN		16
#define DATA_LEN		16

uint8_t SendBuf[DATA_LEN];
uint8_t RecvBuf[DATA_LEN];

void send_buf(uint8_t *SendBuf, uint32_t BufLen, uint32_t timeout)
{
	uint32_t i;
	TIMER I2CTimer;
	TimeOutSet(&I2CTimer, timeout);
	I2C_IntClr();
	I2C_SendStart();
	for(i = 0;i < BufLen; i++)
	{
		I2C_SendByte(SendBuf[i]);
		while((!I2C_GetIntFlag())&&(!IsTimeOut(&I2CTimer)));
		I2C_IntClr();
		if(I2C_ReceiveAcknowledge())
		{
		//	printf("%d\n",i);
			break;
		}
	}
	I2C_SlaveReleaseI2C();
	I2C_ReceiveByte();
	I2C_IntEn(1);
}


__attribute__((section(".driver.isr")))
void I2C_Interrupt(void)
{
	static uint8_t count=0;
	if(I2C_IsAddressMatched())//判断地址匹配
	{
		 I2C_SlaveConfig();
		 if(I2C_SlaveReadWriteGet())//判断读写位
		 {
			I2C_IntEn(0);//发送数据时将中断触发关掉
			send_buf(SendBuf,DATA_LEN,20);
		 }
		 else
		 {
			 I2C_ReceiveByte();
		 }
	}
	else
	{
		RecvBuf[count++] = I2C_ReceiveByte();
		if(count == DATA_LEN)
		{
			count = 0;
		}
	}
	I2C_IntClr();
}

static int32_t WaitDatum1Ever(void)//从串口等待接收一个字节
{
	uint8_t Data;
	while(UART0_Recv(&Data, 1)<=0);
	return Data-0x30;
}


void MasterExample(void)
{
	uint32_t i;
	uint8_t temp=0;
    I2C_ErrorState ret;
    DBG("**** I2C_MASTER test...\n");
	DelayMs(1);
	I2C_Init(0x2f,I2C_PORT_A30_A31, P20_I2C_MASTER_ADDR);//时钟分频/GPIO设置
	DelayMs(1000);

	 Master_start:
	 DBG("Input data 1 master mode to send data\n");
	 DBG("Input data 2 master mode to receive data\n");
	 DBG("/***** ************************************/\n");
	 temp = WaitDatum1Ever();
	 if(temp == 1)
	 {
		for(i=0;i<PACKET_LEN;i++)
		{
			SendBuf[i] = i;
		}
		//测试主机发送
		temp = 0;
		ret = I2C_MasterSendBuffer(P20_I2C_SLAVE_ADDR,SendBuf, PACKET_LEN, 1000);
		while(ret!=ERROR_OK)
		{
			DBG("**** Write to slave Err = [%d]...\n", ret);
			DelayMs(2000);
			ret = I2C_MasterSendBuffer(P20_I2C_SLAVE_ADDR,SendBuf, PACKET_LEN, 1000);
			temp++;
			if(temp>5)
			{
				temp =0;
				goto  Master_start;
			}
		}
		DBG("**** Write to slave OK\n");
	 }
	 else if(temp == 2)
	 {
		memset(RecvBuf, 0x00, PACKET_LEN);
		ret = I2C_MasterReceiveBuffer(P20_I2C_SLAVE_ADDR, RecvBuf, PACKET_LEN, 5000);
		if(ret!=ERROR_OK)
		{
			DBG("**** Read from slave Err = [%d]...\n", ret);
		}
		else
		{
			for(i = 0; i < PACKET_LEN; i++)
			{
				if(RecvBuf[i] != i)
				{
					DBG("\n=============================================\n");
					DBG("%02X != %02X \r\n",RecvBuf[i], (uint8_t)i);
					DBG("\nErr\n");
					DBG("\n=============================================\n");
				}
				else
				{
					DBG("%02X is equal\n",RecvBuf[i]);
				}
			}
		}
	}
	goto  Master_start;
}


void SlaveExample(void)
{
	uint8_t i;
	DBG("**** I2C_SLAVE test(no int)...\n");
	DelayMs(1);
	I2C_Init(0x2f,I2C_PORT_A30_A31,P20_I2C_SLAVE_ADDR);//时钟分频/GPIO设置
	DelayMs(1000);
	memset(RecvBuf, 0x00, DATA_LEN);
	for(i=0;i<PACKET_LEN;i++)
	{
		SendBuf[i] = i;
	}
	while(1)
	{
		I2C_SlaveConfig();
		while(!I2C_GetIntFlag());
		I2C_IntClr();
		if(!I2C_IsAddressMatched())
		{
			DBG("Address Matched Error\n");
		}
		if(I2C_SlaveReadWriteGet())//检查发送或者接收状态
		{
			I2C_SendStart();
			for(i = 0;i < PACKET_LEN; i++)
			{
				I2C_SendByte(SendBuf[i]);
				while(!I2C_GetIntFlag());
				I2C_IntClr();
				if(I2C_ReceiveAcknowledge())
				{
					break;
				}
			}
			I2C_SlaveReleaseI2C();
			while(I2C_IsBusy());
		}
		else
		{
			RecvBuf[0] = I2C_ReceiveByte();//dummy read for trigger recv
			for(i = 0; i < DATA_LEN; i++)
			{
				while(!I2C_GetIntFlag());
				I2C_IntClr();
				RecvBuf[i] = I2C_ReceiveByte();
				if(!I2C_IsBusy())
				{
					break;
				}
			}
		}
	}
}



void SlaveIntExample(void)
{
	uint32_t i;
    DBG("**** I2C_SLAVE test(int)...\n");
	DelayMs(1);
	I2C_Init(0x2f,I2C_PORT_A30_A31, P20_I2C_SLAVE_ADDR);//时钟分频/GPIO设置
	NVIC_EnableIRQ(I2C_IRQn);
	I2C_IntEn(1);
	I2C_SlaveConfig();
	DelayMs(1000);
	memset(RecvBuf, 0x00, DATA_LEN);
	for(i=0;i<PACKET_LEN;i++)
	{
		SendBuf[i] = i;
	}
	while(1);
}







/**
 * @brief   主函数
 * @return
 */
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

	//串口初始化 UART0 C2/C3
	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);
	UARTS_Init(0,  115200, 8,  0,  1);
	SysTickInit();//模拟系统时钟初始化（timer2）
	GIE_ENABLE();
	DBG("\n");
	DBG("------------------------------------------------------\n");
	DBG("        I2C Operation Example              \n");
	DBG("       Mountain View Silicon Technology Co.,Ltd.      \n");
	DBG("------------------------------------------------------\n\n");

	//注意：需要将内部用到的I2C相关IO上拉（注意PU/PD的用法），也可在硬件设计时加上拉电阻
	GPIO_RegOneBitClear(GPIO_A_PU,GPIOA30);
	GPIO_RegOneBitClear(GPIO_A_PD,GPIOA30);
	GPIO_RegOneBitClear(GPIO_A_PU,GPIOA31);
	GPIO_RegOneBitClear(GPIO_A_PD,GPIOA31);
	
	if(MASTER_SLAVE==1)
	{
		MasterExample();
	}
	else
	{
		if(SLAVE_INT)
		{
			SlaveIntExample();
		}
		else
		{
			SlaveExample();
		}

	}
	DBG("The test is completed\n");
	while(1);
	return 0;
}
