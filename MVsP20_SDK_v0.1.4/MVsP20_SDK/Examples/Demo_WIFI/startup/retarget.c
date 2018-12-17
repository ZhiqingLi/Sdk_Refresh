/*
 * retarget.c
 *
 *  Created on: Mar 8, 2017
 *      Author: peter
 */

#include <stdio.h>
#include <string.h>
#include "uarts_interface.h"
#include "type.h"
#include "rtos_def.h"
#include "irqn.h"
#include "gpio.h"
#include <rtos.h>

uint8_t DebugPrintPort = UART_PORT0;
uint8_t DebugOsFlage;
OsMutex uart_mutex;

int putchar(int c)
{
	uint8_t val = c;
	if (c == '\n')
	{
		val = '\r';
		UARTS_Send(DebugPrintPort, &val,1);
		val = '\n';
		UARTS_Send(DebugPrintPort, &val,1);
	}
	else
	{
		UARTS_Send(DebugPrintPort, &val,1);
	}
	return c;
}

__attribute__((used))
void nds_write(const unsigned char *buf, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if(DebugOsFlage)
		{
			OS_MutexLock(uart_mutex);
			putchar(buf[i]);
			OS_MutexUnLock(uart_mutex);
		}
		else
		{
			putchar(buf[i]);
		}
	}
}

void OsDebugInit(void)
{
	OS_MutexInit(&uart_mutex);
	DebugOsFlage = 1;
}
void DebugUartInit()
{
	DebugOsFlage = 0;
	DebugPrintPort = UART_PORT0;
	GPIO_UartTxIoConfig(DebugPrintPort, 1);
	GPIO_UartRxIoConfig(DebugPrintPort, 1);
	UARTS_Init(DebugPrintPort,  115200, 8,  0,  1);

//	shell_init();
}




