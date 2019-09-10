/**
 *******************************************************************************
 * @file    sw_uart.c
 * @brief	software uart driver. When hardware uart pins is occupied as other
 *          functions, this software uart can output debug info. This software
 *          uart only has TX function.

 * @author  Sam
 * @version V1.0.0

 * $Created: 2018-03-13 16:14:05$
 * @Copyright (C) 2014, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *******************************************************************************
 */
#include "type.h"
#include "gpio.h"

#ifdef FUNC_OS_EN
#include "rtos_api.h"
osMutexId SwUARTMutex = NULL;
#endif

#ifdef CFG_APP_CONFIG
#include "app_config.h"
#endif
uint32_t OutRegAddr, OutRegBitMsk, DelayLoops;

/**
 * @brief  Init specified IO as software uart's TX.
 *         The BaudRate is valid in DPLL 96M,if you use RC48M its true value is an half of BaudRate.
 *         Any other divided frequency is the same with last example.
 * @param  PortIndex: select which gpio bank to use
 *     @arg  SWUART_GPIO_PORT_A
 *     @arg  SWUART_GPIO_PORT_B
 * @param  PinIndex:  0 ~ 31, select which gpio io to use.
 *         for example, if PortIndex = SWUART_GPIO_PORT_A, PinIndex = 10,
 *         GPIO_A10 is used as software uart's TX.
 * @param  BaudRate, can be 115200, 57600 or 38400
  *    @arg  115200
 *     @arg  57600
 *     @arg  38400
 * @return None.
 */
void SwUartTxInit(uint8_t PortIndex, uint8_t PinIndex, uint32_t BaudRate)
{
	OutRegAddr = 0x40010000 + (PortIndex + 1) * 4;//Output Register Address
	OutRegBitMsk  = 1 << PinIndex;//Output Register Bit Mask

	GPIO_RegOneBitSet(PortIndex + 1, OutRegBitMsk);//Must output high as default!
	GPIO_RegOneBitClear(PortIndex + 5, OutRegBitMsk);//Input disable
	GPIO_RegOneBitSet(PortIndex + 6, OutRegBitMsk);//Output enable

	if(BaudRate == 115200)
	{
		DelayLoops = 76;
	}
	else if(BaudRate == 256000)
	{
		DelayLoops = 29;
	}
	else if(BaudRate == 460800)
	{
		DelayLoops = 11;
	}
	else
		DelayLoops = 29;//default 256000
}

 /**
 * @brief  Deinit uart TX to default gpio.
 * @param  PortIndex: select which gpio bank to use
 *     @arg  SWUART_GPIO_PORT_A
 *     @arg  SWUART_GPIO_PORT_B
 *     @arg  SWUART_GPIO_PORT_C
 * @param  PinIndex:  0 ~ 31, select which gpio io to deinit.
 * @return None.
 */
void SwUartTxDeinit(uint8_t PortIndex, uint8_t PinIndex)
{
	OutRegAddr = 0x40010000 + (PortIndex + 1) * 4;//Output Register Address
	OutRegBitMsk  = 1 << PinIndex;//Output Register Bit Mask

	GPIO_RegOneBitClear(PortIndex + 1, OutRegBitMsk);//OUTPUT = 0;
	GPIO_RegOneBitSet(PortIndex + 5, OutRegBitMsk);//IE = 1
	GPIO_RegOneBitClear(PortIndex + 6, OutRegBitMsk);//OE = 0
}

/**
 * @Brief	make sw uart baudrate automatic  adaptation
 * @Param	PreFq System Frequency before changed
 * @Param	CurFq System Frequency after changed
 */
void SWUartBuadRateAutoAdap(char PreFq, char CurFq)
{

}


/**
 * @brief  Delay to keep tx's level for some time
 * @param  void
 * @return None.
 */
uint32_t Clock_SysClockFreqGet(void); //clk.h
//__attribute__((section(".tcm_section"), optimize("Og")))
void SwUartDelay(unsigned int us)//0.112209US
{
	unsigned int i;
	if(us == 0)
	{
		return;
	}

	//i = 40*6/2*us-7;

	i = Clock_SysClockFreqGet()*2/10000000;
	i =i/2*us;

	if(i>91)
		i -= 90;

	 __asm __volatile__(
			 ".align  2		   \n"
			 "1:			   \n"
			 "addi %0, %0, #-1 \n"
			 "bgtz %0, 1b      \n"
			 ::"r"(i):);
}
inline void GIE_DISABLE(void);
inline bool GIE_STATE_GET();
inline void GIE_ENABLE(void);
/**
* @brief  Send 1 byte
* @param  c: byte to be send
* @return None
*/
//__attribute__((section(".tcm_section"), optimize("Og")))
void SwUartSendByte(uint8_t c)
{
	uint8_t i;
	uint8_t Cnt = 0;
#ifndef	CFG_FUNC_LED_REFRESH
	bool ret;

	ret = GIE_STATE_GET();
	GIE_DISABLE();
#else
	InterruptLevelSet(1);
#endif
	(*(volatile unsigned long *) OutRegAddr) &= ~(OutRegBitMsk);
	SwUartDelay(DelayLoops);

	for(i=0; i<8; i++)
	{
		if(c & 0x01)
		{
			//GpioSetRegBits(PortIndex + 1, OutRegBitMsk);//OUTPUT = 0;
			(*(volatile unsigned long *) OutRegAddr) |= OutRegBitMsk;
			Cnt++;
		}
		else
		{
			//GpioClrRegBits(PortIndex + 1, OutRegBitMsk);//OUTPUT = 0;
			(*(volatile unsigned long *) OutRegAddr) &= ~(OutRegBitMsk);
		}
		SwUartDelay(DelayLoops);
		c >>= 1;
	}

	if(Cnt % 2)//Å¼Êý
	{
		(*(volatile unsigned long *) OutRegAddr) |= OutRegBitMsk;
	}
	else
	{
		(*(volatile unsigned long *) OutRegAddr) &= ~(OutRegBitMsk);
	}
	SwUartDelay(DelayLoops);

	(*(volatile unsigned long *) OutRegAddr) |= OutRegBitMsk;
	SwUartDelay(DelayLoops);
#ifndef	CFG_FUNC_LED_REFRESH
	if(ret)
	{
		GIE_ENABLE();
	}
#else
	InterruptLevelRestore();
#endif
}

/**
 * @brief  Send data from buffer
 * @param  Buf: Buffer address
 * @param  BufLen: Length of bytes to be send
 * @return None
 */
void SwUartSend(uint8_t* Buf, uint32_t BufLen)
{
	while(BufLen--)
	{
		SwUartSendByte(*Buf++);
	}
}



