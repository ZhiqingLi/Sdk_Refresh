////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:debug.h
//
//		Description	:
//					Define debug ordinary print & debug routine
//
//		Changelog	:
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEBUG_H__
#define __DEBUG_H__

/**
 * @addtogroup mv_utils
 * @{
 * @defgroup debug debug.h
 * @{
 */
 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <stdio.h>
#include "type.h"
#include "gpio.h"
#define	DBG(format, ...)			printf(format, ##__VA_ARGS__)

#define	APP_DBG(format, ...)		printf(format, ##__VA_ARGS__)

#define	OTG_DBG(format, ...)		//printf(format, ##__VA_ARGS__)

#define	BT_DBG(format, ...)			printf(format, ##__VA_ARGS__)//do{printf("[BT] "); printf(format, ##__VA_ARGS__);}while(0)

int DbgUartInit(int Which, unsigned int BaudRate, unsigned char DatumBits, unsigned char Parity, unsigned char StopBits);



#define PORT_BANK_A				(GPIO_A_START)
#define PORT_BANK_B				(GPIO_B_START)
#define PORT_IN_REG(X)		(GPIO_A_IN - GPIO_A_START + X)
#define PORT_OUT_REG(X)		(GPIO_A_OUT - GPIO_A_START + X)
#define PORT_SET_REG(X)		(GPIO_A_SET - GPIO_A_START + X)
#define PORT_CLR_REG(X)		(GPIO_A_CLR - GPIO_A_START + X)
#define PORT_TGL_REG(X)		(GPIO_A_TGL - GPIO_A_START + X)
#define PORT_IE_REG(X)		(GPIO_A_IE - GPIO_A_START + X)
#define PORT_OE_REG(X)		(GPIO_A_OE - GPIO_A_START + X)
#define PORT_DS_REG(X)		(GPIO_A_DS - GPIO_A_START + X)
#define PORT_PU_REG(X)		(GPIO_A_PU - GPIO_A_START + X)
#define PORT_PD_REG(X)		(GPIO_A_PD - GPIO_A_START + X)
#define PORT_ANA_REG(X)		(GPIO_A_ANA_EN - GPIO_A_START + X)
#define PORT_PULLDOWN_REG(X)	(GPIO_A_PULLDOWN - GPIO_A_START + X)
#define PORT_CORE_REG(X)	(GPIO_A_CORE_OUT_MASK - GPIO_A_START + X)
#define PORT_DMA_REG(X)		(GPIO_A_DMA_OUT_MASK - GPIO_A_START + X)


//测试IO,常用于程序执行时序可视化，标准SDK不使用
//输出高低电平，上升沿，下降沿，翻转，led等，1~4端口需配置，可屏蔽，led需配置有效电平。
//调用LedPortInit初始化端口配置。
#define LED_ON_LEVEL	0//灯亮高低电平 设定。
/**根据需要启动对应端口和序号**/
//#define LED1_PORT			PORT_BANK_A		//注释此行，可关闭LED1所有端口操作
#define LED1_INDEX		GPIO_INDEX7
#define LED2_PORT			PORT_BANK_A		//注释此行，可关闭LED2
#define LED2_INDEX		GPIO_INDEX8
#define LED3_PORT			PORT_BANK_A		//注释此行，可关闭LED3
#define LED3_INDEX		GPIO_INDEX9
#define LED4_PORT			PORT_BANK_A		//注释此行，可关闭LED4
#define LED4_INDEX		GPIO_INDEX10

void LedPortInit(void);
void LedOn(uint8_t Index);//1~4 是否生效取决于LEDx_PORT是否定义
void LedOff(uint8_t Index);
void LedToggle(uint8_t Index);
void LedPortRise(uint8_t Index);
void LedPortDown(uint8_t Index);
bool LedPortGet(uint8_t Index);//TRUE:高电平，FALSE:低电平，index无效时 默认FALSE

#ifdef __cplusplus
}
#endif//__cplusplus

/**
 * @}
 * @}
 */
 
#endif //__DBG_H__ 

