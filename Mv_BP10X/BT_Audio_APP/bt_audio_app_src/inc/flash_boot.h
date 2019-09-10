#ifndef __FLASH_BOOT_H__
#define __FLASH_BOOT_H__


#include "flash_config.h"
/*
bulid time:Sun Jun 23 14:23:31 2019
*/
#define FLASH_BOOT_EN      1

//TX PIN
#define BOOT_UART_TX_OFF	0//关闭串口
#define BOOT_UART_TX_A0		1
#define BOOT_UART_TX_A1		2
#define BOOT_UART_TX_A6		3
#define BOOT_UART_TX_A10	4
#define BOOT_UART_TX_A19	5
#define BOOT_UART_TX_A25	6
#define BOOT_UART_TX_PIN	BOOT_UART_TX_A6

//波特率配置
#define BOOT_UART_BAUD_RATE_11520	0
#define BOOT_UART_BAUD_RATE_256000	1
#define BOOT_UART_BAUD_RATE_512000	2
#define BOOT_UART_BAUD_RATE_1000000	3
#define BOOT_UART_BAUD_RATE_1500000	4
#define BOOT_UART_BAUD_RATE_2000000	5
#define BOOT_UART_BAUD_RATE		BOOT_UART_BAUD_RATE_2000000

#define BOOT_UART_CONFIG	((BOOT_UART_BAUD_RATE<<4)+BOOT_UART_TX_PIN)

//#define CONST_DATA_ADDR    0xD0000
//#define USER_DATA_ADDR     0x1D0000
//#define AUDIO_EFFECT_ADDR  0x1e0000

#if FLASH_BOOT_EN
extern const unsigned char flash_data[];
#endif


#endif

