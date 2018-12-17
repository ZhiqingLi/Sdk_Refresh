/*
 * retarget.c
 *
 *  Created on: Mar 8, 2017
 *      Author: peter
 */

#include <stdio.h>
#include "uarts_interface.h"
#include "type.h"
#include <ctype.h> /* for toupper() */

uint8_t DebugPrintPort = UART_PORT0;

#undef putchar
int putchar(int c)
{
	if (c == '\n')
	{
		UARTS_SendByte(DebugPrintPort, '\r');
		UARTS_SendByte(DebugPrintPort, '\n');
	}
	else
	{
		UARTS_SendByte(DebugPrintPort, c);
	}
	return c;
}

__attribute__((used))
void nds_write(const unsigned char *buf, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		putchar(buf[i]);
	}
}

int DbgUartInit(int Which, unsigned int BaudRate, unsigned char DatumBits, unsigned char Parity, unsigned char StopBits)
{
	DebugPrintPort = Which;
	return UARTS_Init(Which, BaudRate, DatumBits,  Parity,  StopBits);
}
