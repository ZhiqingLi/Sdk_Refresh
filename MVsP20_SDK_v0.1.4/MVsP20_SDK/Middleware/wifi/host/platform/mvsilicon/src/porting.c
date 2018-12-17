/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/

#include <ssv_lib.h>
#include <log.h>
#include <rtos.h>
#include "gpio.h"
#include "porting.h"
#include "uarts_interface.h"
#include "debug.h"

extern uint8_t DebugPrintPort;
extern bool sdio_if_load_fw(u8* fw_bin, u32 fw_bin_len);
extern void *pvPortMalloc( size_t size );
extern void vPortFree( void *pv );

extern uint8_t DebugOsFlage;
extern OsMutex uart_mutex;

int hal_putchar(int c)
{
	if(DebugOsFlage)
	{
		OS_MutexLock(uart_mutex);
		if (c == '\n')
		{
			UARTS_SendByte(DebugPrintPort, '\r');
			UARTS_SendByte(DebugPrintPort, '\n');
		}
		else
		{
			UARTS_SendByte(DebugPrintPort, c);
		}
		OS_MutexUnLock(uart_mutex);
	}
	else
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
	}
	return c;
}

//extern uint32_t  gSysTick;
uint8_t hal_getchar(void)
{
	uint8_t val=0;
	if(UARTS_RecvByte(DebugPrintPort,&val))
	{
		return val;
	}
	else
	{
		return 0;
	}
#if 0
	if(!IsRxQueueEmpty())
	{
		///DBG("%c\n",GetRxQueueData());
		return GetRxQueueData();
	}
	else
	{
		return 0;
	}
#endif
}



#ifndef __MTRACE_CHECK__

OS_APIs void *OS_MemAlloc( u32 size )
{
    /**
        *  Platform dependent code. Please rewrite 
        *  this piece of code for different system.
        */
		void *ptr = NULL;
		
		ptr = pvPortMalloc(size);
		if (ptr)
		{
		    OS_MemSET(ptr, 0, size);
        }
		return ptr;
}


/**
 *  We do not recommend using OS_MemFree() API 
 *  because we do not want to support memory 
 *  management mechanism in embedded system.
 */
OS_APIs void __OS_MemFree( void *m )
{
    /**
        *  Platform depedent code. Please rewrite
        *  this piece of code for different system.
        */
    vPortFree(m);
}
#endif//#ifdef __MTRACE_CHECK__

void OS_MemCPY(void *pdest, const void *psrc, u32 size)
{
    memcpy(pdest,psrc,size);
}

void OS_MemSET(void *pdest, u8 byte, u32 size)
{
    memset(pdest,byte,size);
}

void platform_ldo_en_pin_init(void)
{
    GPIO_RegOneBitClear(LDO_EN_PORT(IE),LDO_EN_PIN);
    GPIO_RegOneBitSet(LDO_EN_PORT(OE),LDO_EN_PIN);

    GPIO_RegOneBitClear(LDO_EN_PORT(PU),LDO_EN_PIN);
    GPIO_RegOneBitClear(LDO_EN_PORT(PD),LDO_EN_PIN);
}
void platform_ldo_en(bool en)
{
    if ( 1 == en)
    {
        GPIO_RegOneBitSet(LDO_EN_PORT(OUT), LDO_EN_PIN);
    }
    else
    {
        GPIO_RegOneBitClear(LDO_EN_PORT(OUT), LDO_EN_PIN);
    }
}
//=====================find fw to download=======================
#if 1
#define APP_INCLUDE_WIFI_FW	1
#include "ssv6200_uart_bin.h"
#include "debug.h"
void platform_download_firmware(void)
{
    DBG("bin size =%d\r\n",sizeof(ssv6200_uart_bin));
    ssv6xxx_download_fw((u8 *)ssv6200_uart_bin,sizeof(ssv6200_uart_bin));
#else
bool platform_download_firmware(void)
{
    unsigned int fw_size, i;
 
    extern const unsigned char RES_WIFI_FW_START[];
    extern const unsigned char RES_WIFI_END[];
    unsigned char* fw = (unsigned char*)&RES_WIFI_FW_START;
    fw_size = ((unsigned int)&RES_WIFI_END) - ((unsigned int)&RES_WIFI_FW_START);
    printf("fw_size=%d,%x, fw=%x,%x\r\n",fw_size,*fw ,(unsigned int)fw,(unsigned int)&RES_WIFI_FW_START);
    return ssv6xxx_download_fw((u8 *)&RES_WIFI_FW_START-1,fw_size);//??? u8* bin

#endif
}
void platform_read_firmware(void *d,void *s,u32 len)
{
    OS_MemCPY(d,(void *)s,len);
}
