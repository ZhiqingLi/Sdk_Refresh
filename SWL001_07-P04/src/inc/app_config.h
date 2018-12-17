///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: app_config.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
#include "app_msg.h"
#include "timer_api.h"
#include "drc.h"
#include "gpio.h"
#include "three_d.h"	
#include "virtual_bass.h"
#include "rtc.h"
#include "bt_device_type.h"
#include "timeout.h"
#include "delay.h"
#include "chip_info.h"
#include "os.h"
#include "eq.h"
#include "eq_params.h"
#include "wifi_function_config.h"
#include "wifi_init_setting.h"
#include "wifi_mcu_command.h"
#include "wifi_wifi_command.h"

//****************************************************************************************
//					用户项目定义
//****************************************************************************************

#define FUNC_ALEXA_WIFI_EN
#define FUNC_POWERON_USB_UPDATA_EN						//开机检查U盘升级

//****************************************************************************************
//                 			WiFi 模组控制硬件配置        
//****************************************************************************************
#ifdef FUNC_WIFI_EN
//****************************************************************************************
//                 			WiFi 模组控制端口类型定义        
//****************************************************************************************
//此部分宏定义请保持打开
#define FUART_RX_GPIO_A1		0
#define FUART_RX_GPIO_B6		1
#define FUART_RX_GPIO_C4		2
#define FUART_RX_GPIO_NO		0xFF

#define FUART_TX_GPIO_A0		0
#define FUART_TX_GPIO_B7		1
#define FUART_TX_GPIO_C3		2
#define FUART_TX_GPIO_NO		0xFF

#define BUART_RX_GPIO_A13		0
#define BUART_RX_GPIO_A24		1
#define BUART_RX_GPIO_B8		2
#define BUART_RX_GPIO_B29		3
#define BUART_RX_GPIO_NO		0xFF

#define BUART_TX_GPIO_A16		0
#define BUART_TX_GPIO_A25		1
#define BUART_TX_GPIO_B9		2
#define BUART_TX_GPIO_B28		3
#define BUART_TX_GPIO_NO		0xFF

#define I2S_GPIO_GROUP_0		0 //lrck: B3, bclk: B4, din: B6, do: B5;
#define I2S_GPIO_GROUP_1		1 //lrck: B24, bclk: B25, din: B27, do: B26
#define I2S_GPIO_GROUP_2		2 //lrck:C9, bclk: C10, din: C12, do: C11  
#define I2S_GPIO_GROUP_NO		0xFF //不使用I2S

#define I2S_MCLK_PORT_0			0 //B2
#define I2S_MCLK_PORT_1			1 //C8
#define I2S_MCLK_PORT_NO		0xFF // //不使用I2S


//****************************************************************************************
//                 			WiFi串口类型选择配置
//****************************************************************************************
//如果有蓝牙，并且WiFi需要所有模式有功能，必须选择FUART串口  
#ifdef FUNC_ALEXA_WIFI_EN
#define WIFI_SELECT_FUART_COM	//打开此宏定义，选择FUART串口
#else
#define WIFI_SELECT_BUART_COM	 //打开此宏定义，选择BUART串口
#endif

#ifdef WIFI_SELECT_FUART_COM
#define WIFI_UART_RX_PORT		FUART_RX_GPIO_C4	// rx port
#define WIFI_UART_TX_PORT		FUART_TX_GPIO_C3	// tx port
#else
#define WIFI_UART_RX_PORT		BUART_RX_GPIO_A24	// rx port
#define WIFI_UART_TX_PORT		BUART_TX_GPIO_A25	// tx port
#endif

//****************************************************************************************
//                 			I2S端口组选择配置
//****************************************************************************************
//I2S Master and Slave 选择
#ifdef FUNC_ALEXA_WIFI_EN
#define WIFI_I2S_SLAVE_SELECT //打开此宏定义，WiFi端选择I2S Slave
#else
#define WIFI_I2S_MASTER_SELECT	//打开此宏定义，WiFi端选择I2S Master
#endif

//I2S LRCK/BLCK/DIN/DOUT同时只有一组有效，不能多组同时使用
#ifdef FUNC_ALEXA_WIFI_EN
#define WIFI_I2S_PORT_GROUP		I2S_GPIO_GROUP_0
#else
#define WIFI_I2S_PORT_GROUP		I2S_GPIO_GROUP_NO
#endif

//I2S MCLK根据实际情况选择有无
#ifdef FUNC_ALEXA_WIFI_EN
#define WIFI_I2S_MCLK_PORT		I2S_MCLK_PORT_NO	
#else
#define WIFI_I2S_MCLK_PORT		I2S_MCLK_PORT_NO
#endif

//****************************************************************************************
//                 			WiFi 串口GPIO 配置       
//****************************************************************************************
//Uart Tx
#ifdef WIFI_SELECT_FUART_COM
	#if (WIFI_UART_TX_PORT == FUART_TX_GPIO_C3)
	#define WIFI_UART_TXD_PORT_OUT				GPIO_C_OUT
	#define WIFI_UART_TXD_PORT_BIT				GPIOC3	
	#elif (WIFI_UART_TX_PORT == FUART_TX_GPIO_B7)
	#define WIFI_UART_TXD_PORT_OUT				GPIO_B_OUT
	#define WIFI_UART_TXD_PORT_BIT				GPIOB7	
	#elif (WIFI_UART_TX_PORT == FUART_TX_GPIO_A0)
	#define WIFI_UART_TXD_PORT_OUT				GPIO_A_OUT
	#define WIFI_UART_TXD_PORT_BIT				GPIOA0	
	#endif
#else
	#if (WIFI_UART_TX_PORT == BUART_TX_GPIO_A16)
	#define WIFI_UART_TXD_PORT_OUT				GPIO_A_OUT
	#define WIFI_UART_TXD_PORT_BIT				GPIOA16	
	#elif (WIFI_UART_TX_PORT == BUART_TX_GPIO_A25)
	#define WIFI_UART_TXD_PORT_OUT				GPIO_A_OUT
	#define WIFI_UART_TXD_PORT_BIT				GPIOA25	
	#elif (WIFI_UART_TX_PORT == BUART_TX_GPIO_B9)
	#define WIFI_UART_TXD_PORT_OUT				GPIO_B_OUT
	#define WIFI_UART_TXD_PORT_BIT				GPIOB9	
	#elif (WIFI_UART_TX_PORT == BUART_TX_GPIO_B28)
	#define WIFI_UART_TXD_PORT_OUT				GPIO_B_OUT
	#define WIFI_UART_TXD_PORT_BIT				GPIOB28	
	#endif
#endif

#ifdef WIFI_UART_TXD_PORT_BIT
#define WIFI_UART_TXD_PORT_IE				(WIFI_UART_TXD_PORT_OUT + 1)
#define WIFI_UART_TXD_PORT_OE				(WIFI_UART_TXD_PORT_OUT + 2)
#define WIFI_UART_TXD_PORT_PU				(WIFI_UART_TXD_PORT_OUT + 5)
#define WIFI_UART_TXD_PORT_PD				(WIFI_UART_TXD_PORT_OUT + 6)

#define WiFiUartTxdOn()				do{\
										GpioSetRegOneBit(WIFI_UART_TXD_PORT_PU, WIFI_UART_TXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_TXD_PORT_PD, WIFI_UART_TXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_TXD_PORT_IE, WIFI_UART_TXD_PORT_BIT);\
										GpioSetRegOneBit(WIFI_UART_TXD_PORT_OE, WIFI_UART_TXD_PORT_BIT);\
									}while(0)
									
#define WiFiUartTxdOff()			do{\
										GpioSetRegOneBit(WIFI_UART_TXD_PORT_PU, WIFI_UART_TXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_TXD_PORT_PD, WIFI_UART_TXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_TXD_PORT_OE, WIFI_UART_TXD_PORT_BIT);\
										GpioSetRegOneBit(WIFI_UART_TXD_PORT_IE, WIFI_UART_TXD_PORT_BIT);\
									}while(0)
#else
#define WiFiUartTxdOn()	
#define WiFiUartTxdOff()
#endif

//Uart Rx
#ifdef WIFI_SELECT_FUART_COM
	#if (WIFI_UART_RX_PORT == FUART_RX_GPIO_C4)
	#define WIFI_UART_RXD_PORT_OUT				GPIO_C_OUT
	#define WIFI_UART_RXD_PORT_BIT				GPIOC4	
	#elif (WIFI_UART_RX_PORT == FUART_RX_GPIO_B6)
	#define WIFI_UART_RXD_PORT_OUT				GPIO_B_OUT
	#define WIFI_UART_RXD_PORT_BIT				GPIOB6	
	#elif (WIFI_UART_RX_PORT == FUART_RX_GPIO_A1)
	#define WIFI_UART_RXD_PORT_OUT				GPIO_A_OUT
	#define WIFI_UART_RXD_PORT_BIT				GPIOA1	
	#endif
#else
	#if (WIFI_UART_RX_PORT == BUART_RX_GPIO_A13)
	#define WIFI_UART_RXD_PORT_OUT				GPIO_A_OUT
	#define WIFI_UART_RXD_PORT_BIT				GPIOA13	
	#elif (WIFI_UART_RX_PORT == BUART_RX_GPIO_A24)
	#define WIFI_UART_RXD_PORT_OUT				GPIO_A_OUT
	#define WIFI_UART_RXD_PORT_BIT				GPIOA24	
	#elif (WIFI_UART_RX_PORT == BUART_RX_GPIO_B8)
	#define WIFI_UART_RXD_PORT_OUT				GPIO_B_OUT
	#define WIFI_UART_RXD_PORT_BIT				GPIOB8	
	#elif (WIFI_UART_RX_PORT == BUART_RX_GPIO_B29)
	#define WIFI_UART_RXD_PORT_OUT				GPIO_B_OUT
	#define WIFI_UART_RXD_PORT_BIT				GPIOB29	
	#endif								
#endif

#ifdef WIFI_UART_RXD_PORT_BIT
#define WIFI_UART_RXD_PORT_IE				(WIFI_UART_RXD_PORT_OUT + 1)
#define WIFI_UART_RXD_PORT_OE				(WIFI_UART_RXD_PORT_OUT + 2)
#define WIFI_UART_RXD_PORT_PU				(WIFI_UART_RXD_PORT_OUT + 5)
#define WIFI_UART_RXD_PORT_PD				(WIFI_UART_RXD_PORT_OUT + 6)

#define WiFiUartRxdOn()				do{\
										GpioSetRegOneBit(WIFI_UART_RXD_PORT_PU, WIFI_UART_RXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_RXD_PORT_PD, WIFI_UART_RXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_RXD_PORT_IE, WIFI_UART_RXD_PORT_BIT);\
										GpioSetRegOneBit(WIFI_UART_RXD_PORT_OE, WIFI_UART_RXD_PORT_BIT);\
									}while(0)
									
#define WiFiUartRxdOff()			do{\
										GpioSetRegOneBit(WIFI_UART_RXD_PORT_PU, WIFI_UART_RXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_RXD_PORT_PD, WIFI_UART_RXD_PORT_BIT);\
										GpioClrRegOneBit(WIFI_UART_RXD_PORT_OE, WIFI_UART_RXD_PORT_BIT);\
										GpioSetRegOneBit(WIFI_UART_RXD_PORT_IE, WIFI_UART_RXD_PORT_BIT);\
									}while(0)
#else
#define WiFiUartRxdOn()
#define WiFiUartRxdOff()
#endif

//****************************************************************************************
//                 			WiFi 电源控制GPIO 配置       
//****************************************************************************************
#ifdef FUNC_ALEXA_WIFI_EN
#define WIFI_POWER_PORT_OUT				GPIO_A_OUT
#define WIFI_POWER_PORT_BIT				GPIOA23
#endif
#ifdef WIFI_POWER_PORT_BIT
#define WIFI_POWER_PORT_IE				(WIFI_POWER_PORT_OUT + 1)
#define WIFI_POWER_PORT_OE				(WIFI_POWER_PORT_OUT + 2)
#define WIFI_POWER_PORT_PU				(WIFI_POWER_PORT_OUT + 5)
#define WIFI_POWER_PORT_PD				(WIFI_POWER_PORT_OUT + 6)

#define WiFiPowerOn()				do{\
										GpioSetRegOneBit(WIFI_POWER_PORT_PU, WIFI_POWER_PORT_BIT);\
										GpioClrRegOneBit(WIFI_POWER_PORT_PD, WIFI_POWER_PORT_BIT);\
										GpioClrRegOneBit(WIFI_POWER_PORT_IE, WIFI_POWER_PORT_BIT);\
										GpioSetRegOneBit(WIFI_POWER_PORT_OE, WIFI_POWER_PORT_BIT);\
										GpioSetRegOneBit(WIFI_POWER_PORT_OUT, WIFI_POWER_PORT_BIT);\
									}while(0)
									
#define WiFiPowerOff()			do{\
										GpioSetRegOneBit(WIFI_POWER_PORT_PU, WIFI_POWER_PORT_BIT);\
										GpioClrRegOneBit(WIFI_POWER_PORT_PD, WIFI_POWER_PORT_BIT);\
										GpioClrRegOneBit(WIFI_POWER_PORT_IE, WIFI_POWER_PORT_BIT);\
										GpioSetRegOneBit(WIFI_POWER_PORT_OE, WIFI_POWER_PORT_BIT);\
										GpioClrRegOneBit(WIFI_POWER_PORT_OUT, WIFI_POWER_PORT_BIT);\
									}while(0)
#else
#define WiFiPowerOn()
#define WiFiPowerOff()
#endif

//****************************************************************************************
//                 			WiFi MIC通道控制GPIO 配置       
//****************************************************************************************
#ifdef FUNC_ALEXA_WIFI_EN
#define WIFI_MIC_PORT_OUT				GPIO_A_OUT
#define WIFI_MIC_PORT_BIT				GPIOA19
#endif
#ifdef WIFI_MIC_PORT_BIT
#define WIFI_MIC_PORT_IE				(WIFI_MIC_PORT_OUT + 1)
#define WIFI_MIC_PORT_OE				(WIFI_MIC_PORT_OUT + 2)
#define WIFI_MIC_PORT_PU				(WIFI_MIC_PORT_OUT + 5)
#define WIFI_MIC_PORT_PD				(WIFI_MIC_PORT_OUT + 6)

#define WiFiMicOn()				do{\
										GpioSetRegOneBit(WIFI_MIC_PORT_PU, WIFI_MIC_PORT_BIT);\
										GpioClrRegOneBit(WIFI_MIC_PORT_PD, WIFI_MIC_PORT_BIT);\
										GpioClrRegOneBit(WIFI_MIC_PORT_IE, WIFI_MIC_PORT_BIT);\
										GpioSetRegOneBit(WIFI_MIC_PORT_OE, WIFI_MIC_PORT_BIT);\
										GpioSetRegOneBit(WIFI_MIC_PORT_OUT, WIFI_MIC_PORT_BIT);\
									}while(0)
									
#define WiFiMicOff()			do{\
										GpioSetRegOneBit(WIFI_MIC_PORT_PU, WIFI_MIC_PORT_BIT);\
										GpioClrRegOneBit(WIFI_MIC_PORT_PD, WIFI_MIC_PORT_BIT);\
										GpioClrRegOneBit(WIFI_MIC_PORT_IE, WIFI_MIC_PORT_BIT);\
										GpioSetRegOneBit(WIFI_MIC_PORT_OE, WIFI_MIC_PORT_BIT);\
										GpioClrRegOneBit(WIFI_MIC_PORT_OUT, WIFI_MIC_PORT_BIT);\
									}while(0)
#else
#define WiFiMicOn()
#define WiFiMicOff()
#endif
									
//****************************************************************************************
//                 			WiFi 复位控制GPIO 配置       
//****************************************************************************************
#ifdef FUNC_ALEXA_WIFI_EN
#define WIFI_RESET_PORT_OUT				GPIO_C_OUT
//#define WIFI_RESET_PORT_BIT				GPIOC12
#endif
#ifdef WIFI_RESET_PORT_BIT
#define WIFI_RESET_PORT_IE				(WIFI_RESET_PORT_OUT + 1)
#define WIFI_RESET_PORT_OE				(WIFI_RESET_PORT_OUT + 2)
#define WIFI_RESET_PORT_PU				(WIFI_RESET_PORT_OUT + 5)
#define WIFI_RESET_PORT_PD				(WIFI_RESET_PORT_OUT + 6)

#define WiFiResetOn()				do{\
										GpioSetRegOneBit(WIFI_RESET_PORT_PU, WIFI_RESET_PORT_BIT);\
										GpioClrRegOneBit(WIFI_RESET_PORT_PD, WIFI_RESET_PORT_BIT);\
										GpioClrRegOneBit(WIFI_RESET_PORT_IE, WIFI_RESET_PORT_BIT);\
										GpioSetRegOneBit(WIFI_RESET_PORT_OE, WIFI_RESET_PORT_BIT);\
										GpioSetRegOneBit(WIFI_RESET_PORT_OUT, WIFI_RESET_PORT_BIT);\
									}while(0)
									
#define WiFiResetOff()			do{\
										GpioSetRegOneBit(WIFI_RESET_PORT_PU, WIFI_RESET_PORT_BIT);\
										GpioClrRegOneBit(WIFI_RESET_PORT_PD, WIFI_RESET_PORT_BIT);\
										GpioClrRegOneBit(WIFI_RESET_PORT_IE, WIFI_RESET_PORT_BIT);\
										GpioSetRegOneBit(WIFI_RESET_PORT_OE, WIFI_RESET_PORT_BIT);\
										GpioClrRegOneBit(WIFI_RESET_PORT_OUT, WIFI_RESET_PORT_BIT);\
									}while(0)
#else
#define WiFiResetOn()
#define WiFiResetOff()
#endif

//****************************************************************************************
//                 			WiFi 模组I2S 控制GPIO 配置       
//****************************************************************************************
//WiFi I2S LRCK
#if (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_0)
#define WIFI_I2S_LRCK_PORT_OUT				GPIO_B_OUT
#define WIFI_I2S_LRCK_PORT_BIT				GPIOB3	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_1)
#define WIFI_I2S_LRCK_PORT_OUT				GPIO_B_OUT
#define WIFI_I2S_LRCK_PORT_BIT				GPIOB24	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_2)
#define WIFI_I2S_LRCK_PORT_OUT				GPIO_C_OUT
#define WIFI_I2S_LRCK_PORT_BIT				GPIOC9	
#endif
#ifdef WIFI_I2S_LRCK_PORT_BIT
#define WIFI_I2S_LRCK_PORT_IE				(WIFI_I2S_LRCK_PORT_OUT + 1)
#define WIFI_I2S_LRCK_PORT_OE				(WIFI_I2S_LRCK_PORT_OUT + 2)
#define WIFI_I2S_LRCK_PORT_PU				(WIFI_I2S_LRCK_PORT_OUT + 5)
#define WIFI_I2S_LRCK_PORT_PD				(WIFI_I2S_LRCK_PORT_OUT + 6)

#define WiFiI2sLrckOn()				do{\
										GpioSetRegOneBit(WIFI_I2S_LRCK_PORT_PU, WIFI_I2S_LRCK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_LRCK_PORT_PD, WIFI_I2S_LRCK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_LRCK_PORT_IE, WIFI_I2S_LRCK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_LRCK_PORT_OE, WIFI_I2S_LRCK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_LRCK_PORT_OUT, WIFI_I2S_LRCK_PORT_BIT);\
									}while(0)
									
#define WiFiI2sLrckOff()			do{\
										GpioSetRegOneBit(WIFI_I2S_LRCK_PORT_PU, WIFI_I2S_LRCK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_LRCK_PORT_PD, WIFI_I2S_LRCK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_LRCK_PORT_IE, WIFI_I2S_LRCK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_LRCK_PORT_OE, WIFI_I2S_LRCK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_LRCK_PORT_OUT, WIFI_I2S_LRCK_PORT_BIT);\
									}while(0)
#else
#define WiFiI2sLrckOn()	
#define WiFiI2sLrckOff()
#endif

//WiFi I2S BCLK
#if (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_0)
#define WIFI_I2S_BCLK_PORT_OUT			GPIO_B_OUT
#define WIFI_I2S_BCLK_PORT_BIT			GPIOB4	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_1)
#define WIFI_I2S_BCLK_PORT_OUT			GPIO_B_OUT
#define WIFI_I2S_BCLK_PORT_BIT			GPIOB25	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_2)
#define WIFI_I2S_BCLK_PORT_OUT			GPIO_C_OUT
#define WIFI_I2S_BCLK_PORT_BIT			GPIOC10	
#endif
#ifdef WIFI_I2S_BCLK_PORT_BIT
#define WIFI_I2S_BCLK_PORT_IE				(WIFI_I2S_BCLK_PORT_OUT + 1)
#define WIFI_I2S_BCLK_PORT_OE				(WIFI_I2S_BCLK_PORT_OUT + 2)
#define WIFI_I2S_BCLK_PORT_PU				(WIFI_I2S_BCLK_PORT_OUT + 5)
#define WIFI_I2S_BCLK_PORT_PD				(WIFI_I2S_BCLK_PORT_OUT + 6)

#define WiFiI2sBclkOn()				do{\
										GpioSetRegOneBit(WIFI_I2S_BCLK_PORT_PU, WIFI_I2S_BCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_BCLK_PORT_PD, WIFI_I2S_BCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_BCLK_PORT_IE, WIFI_I2S_BCLK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_BCLK_PORT_OE, WIFI_I2S_BCLK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_BCLK_PORT_OUT, WIFI_I2S_BCLK_PORT_BIT);\
									}while(0)
									
#define WiFiI2sBclkOff()			do{\
										GpioSetRegOneBit(WIFI_I2S_BCLK_PORT_PU, WIFI_I2S_BCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_BCLK_PORT_PD, WIFI_I2S_BCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_BCLK_PORT_IE, WIFI_I2S_BCLK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_BCLK_PORT_OE, WIFI_I2S_BCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_BCLK_PORT_OUT, WIFI_I2S_BCLK_PORT_BIT);\
									}while(0)
#else
#define WiFiI2sBclkOn()	
#define WiFiI2sBclkOff()
#endif

//WiFi I2S DIN
#if (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_0)
#define WIFI_I2S_DIN_PORT_OUT			GPIO_B_OUT
#define WIFI_I2S_DIN_PORT_BIT			GPIOB6	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_1)
#define WIFI_I2S_DIN_PORT_OUT			GPIO_B_OUT
#define WIFI_I2S_DIN_PORT_BIT			GPIOB27	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_2)
#define WIFI_I2S_DIN_PORT_OUT			GPIO_C_OUT
#define WIFI_I2S_DIN_PORT_BIT			GPIOC12	
#endif
#ifdef WIFI_I2S_DIN_PORT_BIT
#define WIFI_I2S_DIN_PORT_IE				(WIFI_I2S_DIN_PORT_OUT + 1)
#define WIFI_I2S_DIN_PORT_OE				(WIFI_I2S_DIN_PORT_OUT + 2)
#define WIFI_I2S_DIN_PORT_PU				(WIFI_I2S_DIN_PORT_OUT + 5)
#define WIFI_I2S_DIN_PORT_PD				(WIFI_I2S_DIN_PORT_OUT + 6)

#define WiFiI2sDinOn()				do{\
										GpioSetRegOneBit(WIFI_I2S_DIN_PORT_PU, WIFI_I2S_DIN_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DIN_PORT_PD, WIFI_I2S_DIN_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DIN_PORT_IE, WIFI_I2S_DIN_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_DIN_PORT_OE, WIFI_I2S_DIN_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_DIN_PORT_OUT, WIFI_I2S_DIN_PORT_BIT);\
									}while(0)
									
#define WiFiI2sDinOff()			do{\
										GpioSetRegOneBit(WIFI_I2S_DIN_PORT_PU, WIFI_I2S_DIN_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DIN_PORT_PD, WIFI_I2S_DIN_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DIN_PORT_IE, WIFI_I2S_DIN_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_DIN_PORT_OE, WIFI_I2S_DIN_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DIN_PORT_OUT, WIFI_I2S_DIN_PORT_BIT);\
									}while(0)
#else
#define WiFiI2sDinOn()	
#define WiFiI2sDinOff()	
#endif

//WiFi I2S DO
#if (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_0)
#define WIFI_I2S_DO_PORT_OUT		GPIO_B_OUT
#define WIFI_I2S_DO_PORT_BIT		GPIOB5	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_1)
#define WIFI_I2S_DO_PORT_OUT		GPIO_B_OUT
#define WIFI_I2S_DO_PORT_BIT		GPIOB26	
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_2)
#define WIFI_I2S_DO_PORT_OUT		GPIO_C_OUT
#define WIFI_I2S_DO_PORT_BIT		GPIOC11	
#endif
#ifdef WIFI_I2S_DO_PORT_BIT
#define WIFI_I2S_DO_PORT_IE				(WIFI_I2S_DO_PORT_OUT + 1)
#define WIFI_I2S_DO_PORT_OE				(WIFI_I2S_DO_PORT_OUT + 2)
#define WIFI_I2S_DO_PORT_PU				(WIFI_I2S_DO_PORT_OUT + 5)
#define WIFI_I2S_DO_PORT_PD				(WIFI_I2S_DO_PORT_OUT + 6)

#define WiFiI2sDoutOn()				do{\
										GpioSetRegOneBit(WIFI_I2S_DO_PORT_PU, WIFI_I2S_DO_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DO_PORT_PD, WIFI_I2S_DO_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DO_PORT_IE, WIFI_I2S_DO_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_DO_PORT_OE, WIFI_I2S_DO_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_DO_PORT_OUT, WIFI_I2S_DO_PORT_BIT);\
									}while(0)
									
#define WiFiI2sDoutOff()			do{\
										GpioSetRegOneBit(WIFI_I2S_DO_PORT_PU, WIFI_I2S_DO_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DO_PORT_PD, WIFI_I2S_DO_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DO_PORT_IE, WIFI_I2S_DO_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_DO_PORT_OE, WIFI_I2S_DO_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_DO_PORT_OUT, WIFI_I2S_DO_PORT_BIT);\
									}while(0)
#else
#define WiFiI2sDoutOn()	
#define WiFiI2sDoutOff()	
#endif

//I2S MCLK
#if (WIFI_I2S_MCLK_PORT == I2S_MCLK_PORT_0)
#define WIFI_I2S_MCLK_PORT_OUT			GPIO_B_OUT
#define WIFI_I2S_MCLK_PORT_BIT			GPIOB2	
#elif (WIFI_I2S_MCLK_PORT == I2S_MCLK_PORT_1)
#define WIFI_I2S_MCLK_PORT_OUT			GPIO_C_OUT
#define WIFI_I2S_MCLK_PORT_BIT			GPIOC8	
#endif
#ifdef WIFI_I2S_MCLK_PORT_BIT
#define WIFI_I2S_MCLK_PORT_IE				(WIFI_I2S_MCLK_PORT_OUT + 1)
#define WIFI_I2S_MCLK_PORT_OE				(WIFI_I2S_MCLK_PORT_OUT + 2)
#define WIFI_I2S_MCLK_PORT_PU				(WIFI_I2S_MCLK_PORT_OUT + 5)
#define WIFI_I2S_MCLK_PORT_PD				(WIFI_I2S_MCLK_PORT_OUT + 6)

#define WiFiI2sMclkOn()				do{\
										GpioSetRegOneBit(WIFI_I2S_MCLK_PORT_PU, WIFI_I2S_MCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_MCLK_PORT_PD, WIFI_I2S_MCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_MCLK_PORT_IE, WIFI_I2S_MCLK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_MCLK_PORT_OE, WIFI_I2S_MCLK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_MCLK_PORT_OUT, WIFI_I2S_MCLK_PORT_BIT);\
									}while(0)
									
#define WiFiI2sMclkOff()			do{\
										GpioSetRegOneBit(WIFI_I2S_MCLK_PORT_PU, WIFI_I2S_MCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_MCLK_PORT_PD, WIFI_I2S_MCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_MCLK_PORT_IE, WIFI_I2S_MCLK_PORT_BIT);\
										GpioSetRegOneBit(WIFI_I2S_MCLK_PORT_OE, WIFI_I2S_MCLK_PORT_BIT);\
										GpioClrRegOneBit(WIFI_I2S_MCLK_PORT_OUT, WIFI_I2S_MCLK_PORT_BIT);\
									}while(0)
#else
#define WiFiI2sMclkOn()	
#define WiFiI2sMclkOff()
#endif
#endif


//****************************************************************************************
//获取一个随机数，原是在player_control.c中定义的，为了方便使用，改为定义在app_config.h中
//****************************************************************************************

  #define GetRandNum(MaxValue) ((OSSysTickGet()%MaxValue) + 3) 
	
//****************************************************************************************
//                  系统默认音量配置         
//****************************************************************************************
	#define MAX_VOLUME  32
	#define DEFAULT_VOLUME  20
//	#define POWERON_DEFAULT_VOLUME_EN     //每次开机使用默认音量，不使用记忆音量
	#define DEC_FADEIN_TIME					1000	//fadein时间	

	#define ADC_DIGITAL_VOL				VOLUME_ADC_DIGITAL
	#define DAC_DIGITAL_VOL				VOLUME_DAC_DIGITAL
	
//	#define SOUND_REMIND_VOL	    24	//设置提示音音量
	#define WIFI_SOUND_REMIND_VOL   24  //WiFi提示音音量
	
//****************************************************************************************
//                  输出通道选择       
//****************************************************************************************
#ifndef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE
    //同一时刻只能使能其中的一种
//    #define OUTPUT_CHANNEL_DAC                      //DAC输出
//    #define OUTPUT_CHANNEL_I2S                    //标准I2S输出
    #define OUTPUT_CHANNEL_DAC_I2S                //标准I2S以及DAC同时输出
//    #define OUTPUT_CHANNEL_CLASSD                 //以外接ST系列CLASSD的I2S输出
//    #define OUTPUT_CHANNEL_DAC_CLASSD             //以外接ST系列CLASSD的I2S以及DAC输出:该输出不能播放采样率低于32KHZ,如果需要全部支持，可以开启转采样功能
#endif
    
//****************************************************************************************
//					升级功能
//****************************************************************************************
	#define FUNC_SPI_UPDATE_EN
	
//	#define FUNC_UPDATE_CONTROL   //升级交互过程控制

//****************************************************************************************
//                  频谱采集配置        
//****************************************************************************************
	#define FUNC_SPEC_TRUM_EN                      	//假频谱功能开关
	
//****************************************************************************************
//                 语音提示音功能配置        
//****************************************************************************************
	#define FUNC_SOUND_REMIND		//语音提示

//****************************************************************************************
//                  系统采样率配置            
//****************************************************************************************
//使能该宏表示MIXER模块自动将各种采样率统一转换为44.1KHZ
//当做卡拉OK类的应用时，强烈建议使能该宏
//使能该宏后，MIXER模块对非44.1KHZ采样率的数据做采样率转换，高频信号会略有衰减
//	#define FUNC_MIXER_SRC_EN   					
													
//****************************************************************************************
//                  软件微调配置       
//****************************************************************************************
//使能该宏，则会在Normal模式输出时，进行软件微调
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE
//	#define FUNC_SOFT_ADJUST_EN
#endif

//****************************************************************************************
//                  文件系统配置
//****************************************************************************************
	#define FS_EXFAT_EN

//****************************************************************************************
//                  Decoder 相应超时设置
//****************************************************************************************
	#define DECODER_TASK_WAIT_TIMEOUT		10*1000	// 10S

//****************************************************************************************
//                  各音频源使用的MIXER模块的输入通道号配置             
//****************************************************************************************
	#define MIXER_SOURCE_DEC				0		//U盘/SD卡播放
	#define MIXER_SOURCE_BT					0		//蓝牙播放
	#define MIXER_SOURCE_USB				0		//USB声卡播放
	#define MIXER_SOURCE_MIC				1		//MIC
	#define MIXER_SOURCE_ANA_MONO			2		//单声道模拟输入，单声道GUITAR、单声道GUITAR+LINEIN
	#define MIXER_SOURCE_ANA_STERO			3		//双声道模拟输入，双声道LINEIN、双声道FM，此时MIC不可用


//****************************************************************************************
//                  BASS/TREB功能配置                        
//****************************************************************************************
//	#define FUNC_TREB_BASS_EN					//高低音调节功能
	#ifdef FUNC_TREB_BASS_EN
		#define MAX_TREB_VAL 			10		//高音调节的级数：0--10
		#define MAX_BASS_VAL 			10		//低音调节的级数：0--10
		#define DEFAULT_TREB_VAL		5		//高音调节的默认级数
		#define DEFAULT_BASS_VAL		5 		//高音调节的默认级数
	#endif
//****************************************************************************************
//                 EQ功能配置        
//****************************************************************************************
	#define EQ_STYLE_MODE				EQ_MODE
	
//****************************************************************************************
//                 DC Blocker功能配置        
//****************************************************************************************
	#define FUNC_DC_BLOCKER

//****************************************************************************************
//                 Silence detector 功能配置        
//****************************************************************************************
	#ifdef FUNC_DC_BLOCKER
//		#define FUNC_SILENCE_DETECTOR

		#define SILENCE_THRSHOLD				100
		#define SILENCE_COUNT					200
	#endif

//****************************************************************************************
//                  MIC功能配置  
//****************************************************************************************
#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
	#define FUNC_MIC_EN								//MIC功能
#endif
	#ifdef FUNC_MIC_EN
		#define FUNC_MIC_ALC_EN							//MIC ALC功能开关
		#define MAX_MICIN_VOLUME				16		//MIC音量可调的级数：0--16
		#define DEFAULT_MICIN_VOLUME			12		//MIC音量默认值
		#define MICIN_ANA_GAIN_VAL				VOLUME_ADC_ANA_MIC		//MIC输入通路模拟增益(22.9dB),GIAN 对应关系见 sys_vol.h
		//#define FUNC_MIC_DET_EN  						//MIC插拔检测定义
		#ifdef FUNC_MIC_DET_EN
			#define	MICIN_DETECT_PORT_IN		GPIO_A_IN
			#define	MICIN_DETECT_PORT_IE		GPIO_A_IE
			#define	MICIN_DETECT_PORT_OE		GPIO_A_OE
			#define	MICIN_DETECT_PORT_PU		GPIO_A_PU
			#define	MICIN_DETECT_PORT_PD		GPIO_A_PD
			#define MICIN_DETECT_BIT			(1 << 0)
		#endif

//		#define FUNC_MIC_ECHO_EN						//MIC ECHO功能
		#ifdef FUNC_MIC_ECHO_EN
			#define MAX_ECHO_DELAY_STEP			30		//MIC回声延时可调的级数：0--30
			#define DEFAULT_DELAY_STEP			22		//MIC回声延时默认值
			#define DELAY_ONE_STEP				400		//MIC回声延时每调一级对应的采样点数差异
			#define MAX_ECHO_DEPTH_STEP			16		//MIC回声强度可调的级数：0--16
			#define DEFAULT_DEPTH_STEP			12		//MIC回声强度默认值
			#define DEPTH_ONE_STEP				1024	//MIC回声强度每调一级对应的强度差异	
			
			#define MIC_ECHO_MIX_BYPASS_EN				//MIC回声是否叠加原始数据
			#define MIC_BYPASS_VOL				4095	//MIC回声音量增益
			#define MIC_ECHO_EFFECT_VOL			4095	//MIC原始音量增益
		#endif
		
		#define CFG_ECHO_DELAY                         (17640)  //400 ms
		#define CFG_ECHO_ATTENUATION                   (11626)  //-9 dB
		#define CFG_ECHO_DIRECT_SND_EN                 (    1)
		#define CFG_ECHO_CUTOFF_FREQ                   (    0)  //Disabled
	#endif

//****************************************************************************************
//					String format convert
//****************************************************************************************
	#define FUNC_STRING_CONVERT_EN 			// 字符串编码转换

//****************************************************************************************
//                  播放模式配置                        
//****************************************************************************************
#ifdef FUNC_POWERON_USB_UPDATA_EN
	#define FUNC_USB_EN						// U盘播放功能
#endif
	#ifdef FUNC_USB_EN
		#define UDISK_PORT_NUM		        1		// USB端口定义
	#endif
//	#define FUNC_CARD_EN						// SD卡播放功能	
	#ifdef FUNC_CARD_EN
		#define	SD_PORT_NUM                 1		// SD卡端口定义
	#endif

	#if (SD_PORT_NUM == 1)
		//SD_CLK复用为硬件检测脚：A20
		#define CARD_DETECT_PORT_IN			GPIO_A_IN	
		#define CARD_DETECT_PORT_OE			GPIO_A_OE	
		#define CARD_DETECT_PORT_PU			GPIO_A_PU	
		#define CARD_DETECT_PORT_PD			GPIO_A_PD	
		#define CARD_DETECT_PORT_IE  		GPIO_A_IE
		#define CARD_DETECT_BIT_MASK		(1 << 20)
	#else
		#define CARD_DETECT_PORT_IN			GPIO_B_IN	
		#define CARD_DETECT_PORT_OE			GPIO_B_OE	
		#define CARD_DETECT_PORT_PU			GPIO_B_PU	
		#define CARD_DETECT_PORT_PD			GPIO_B_PD	
		#define CARD_DETECT_PORT_IE  		GPIO_B_IE
		#define CARD_DETECT_BIT_MASK		(GPIOB4)
	#endif

	#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))
		#define FUNC_LRC_EN							// LRC歌词文件解析
		//#define FUNC_FFFB_WITHOUT_SOUND     		//可选项，快进快退是否播放声音，默认出声音
		//#define FUNC_FFFB_END_WITHOUT_AUTO_SWITCH	//可选项，快进曲尾或者快退到曲首是否停止seek，默认保持快进快退
	#endif

//****************************************************************************************
//                 文件浏览模式配置        
//****************************************************************************************
	#define FUNC_BROWSER_EN  				//文件浏览功能

//****************************************************************************************
//                 FM收音机模式配置 
//****************************************************************************************
//	#define FUNC_RADIO_EN						//Fm Radio
	#ifdef FUNC_RADIO_EN
		#define FMIN_ANA_GAIN_VAL			VOLUME_ADC_ANA_LIN23	//FM输入通道的模拟增益(10.8dB)，如果作为LineIn使用需要设置为18(-3.65dB)，否则失真
												//GIAN 对应关系见 sys_vol.h
		#define FUNC_RADIO_DIV_CLK_EN			//FM降频工作

		#define FUNC_RADIO_RDA5807_EN
//		#define FUNC_RADIO_QN8035_EN

		#define MAX_RADIO_CHANNEL_NUM    50
		#define RADIO_SEEK_PREVIEW_SUPPORT		//支持搜台时，每搜到一个电台都自动播放几秒钟
		#define RADIO_DELAY_SWITCH_CHANNEL		//延迟处理电台切换，合并连续按键切换电台的特殊情况
		#define FM_CHL_SEARCH_TIME_UNIT 100		//搜台时频率扫描间隔ms
		#define FM_PERVIEW_TIMEOUT 1000			//FM搜台预览时间，该值不能小于MIN_TIMER_PERIOD，否则精确度会有问题(unit ms)
	#endif


//****************************************************************************************
//                 LINEIN模式配置 
//****************************************************************************************
//	#define FUNC_LINEIN_EN   					// Linein
	#ifdef FUNC_LINEIN_EN
		#define LINEIN_ANA_GAIN_VAL			VOLUME_ADC_ANA_LIN1	//LINEIN输入通道的模拟增益(-3.83),GIAN 对应关系见 sys_vol.h
		#define LINEIN_DETECT_PORT_IN		GPIO_B_IN
		#define LINEIN_DETECT_PORT_OE		GPIO_B_OE
		#define LINEIN_DETECT_PORT_IE		GPIO_B_IE
		#define LINEIN_DETECT_PORT_PU		GPIO_B_PU
		#define LINEIN_DETECT_PORT_PD		GPIO_B_PD
		#define LINEIN_DETECT_BIT_MASK		GPIOB7
	#endif

//****************************************************************************************
//                 功放配置        
//****************************************************************************************
#define FUNC_AMP_MUTE_EN         //功放Mute功能配置										
									
#ifdef FUNC_AMP_MUTE_EN
																
	#define AMP_SILENCE_MUTE_EN     //打开该宏，支持在DAC输出无音时MUTE功放
	#define AMP_GPIO_MUTE_EN        //打开该宏，支持GPIO控制功放MUTE
//	#define FUNC_AMP_POWER_EN
	
	#ifdef  AMP_SILENCE_MUTE_EN
		#define SILENCE_MUTE_TIMER        500     //无信号MUTE检测时间
		#define SILENCE_UNMUTE_TIMER      30      //解MUTE检测时间
		#define DEC_SILENCE_MUTE_VALUE    1       //DEC解码MUTE阀值
		#define ANA_SILENCE_MUTE_VALUE    16      //模拟输入MUTE阀值
		#define SILENCE_MUTE_DET_CNT      85      //百分比，在200ms内低于检测阀值的百分比达到该值，启动功放MUTE
	#endif
		
	#ifdef AMP_GPIO_MUTE_EN		
		#define AMP_MUTE_PORT_OUT				GPIO_A_OUT
		#define AMP_MUTE_PORT_BIT				GPIOA0

		#define AMP_MUTE_PORT_IE				(AMP_MUTE_PORT_OUT + 1)
		#define AMP_MUTE_PORT_OE				(AMP_MUTE_PORT_OUT + 2)
		#define AMP_MUTE_PORT_PU				(AMP_MUTE_PORT_OUT + 5)
		#define AMP_MUTE_PORT_PD				(AMP_MUTE_PORT_OUT + 6)

		#define GpioAmpMuteEnable()				do{\
												GpioSetRegOneBit(AMP_MUTE_PORT_PU, AMP_MUTE_PORT_BIT);\
												GpioClrRegOneBit(AMP_MUTE_PORT_PD, AMP_MUTE_PORT_BIT);\
												GpioClrRegOneBit(AMP_MUTE_PORT_IE, AMP_MUTE_PORT_BIT);\
												GpioSetRegOneBit(AMP_MUTE_PORT_OE, AMP_MUTE_PORT_BIT);\
												GpioClrRegOneBit(AMP_MUTE_PORT_OUT, AMP_MUTE_PORT_BIT);\
												}while(0)

		#define GpioAmpMuteDisable()			do{\
												GpioSetRegOneBit(AMP_MUTE_PORT_PU, AMP_MUTE_PORT_BIT);\
												GpioClrRegOneBit(AMP_MUTE_PORT_PD, AMP_MUTE_PORT_BIT);\
												GpioClrRegOneBit(AMP_MUTE_PORT_IE, AMP_MUTE_PORT_BIT);\
												GpioSetRegOneBit(AMP_MUTE_PORT_OE, AMP_MUTE_PORT_BIT);\
												GpioSetRegOneBit(AMP_MUTE_PORT_OUT, AMP_MUTE_PORT_BIT);\
												}while(0)
	#else
	
		#define GpioAmpMuteEnable()	
		#define GpioAmpMuteDisable()
	#endif

	#ifdef FUNC_AMP_POWER_EN
																						
		#define AMP_POWER_PORT_OUT				GPIO_A_OUT
		#define AMP_POWER_PORT_BIT				GPIOA22

		#define AMP_POWER_PORT_IE				(AMP_POWER_PORT_OUT + 1)
		#define AMP_POWER_PORT_OE				(AMP_POWER_PORT_OUT + 2)
		#define AMP_POWER_PORT_PU				(AMP_POWER_PORT_OUT + 5)
		#define AMP_POWER_PORT_PD				(AMP_POWER_PORT_OUT + 6)

		#define AmpPowerEnable()				do{\
												GpioSetRegOneBit(AMP_POWER_PORT_PU, AMP_POWER_PORT_BIT);\
												GpioClrRegOneBit(AMP_POWER_PORT_PD, AMP_POWER_PORT_BIT);\
												GpioClrRegOneBit(AMP_POWER_PORT_IE, AMP_POWER_PORT_BIT);\
												GpioSetRegOneBit(AMP_POWER_PORT_OE, AMP_POWER_PORT_BIT);\
												GpioSetRegOneBit(AMP_POWER_PORT_OUT, AMP_POWER_PORT_BIT);\
												}while(0)

		#define AmpPowerDisable()				do{\
												GpioSetRegOneBit(AMP_POWER_PORT_PU, AMP_POWER_PORT_BIT);\
												GpioClrRegOneBit(AMP_POWER_PORT_PD, AMP_POWER_PORT_BIT);\
												GpioClrRegOneBit(AMP_POWER_PORT_IE, AMP_POWER_PORT_BIT);\
												GpioSetRegOneBit(AMP_POWER_PORT_OE, AMP_POWER_PORT_BIT);\
												GpioClrRegOneBit(AMP_POWER_PORT_OUT, AMP_POWER_PORT_BIT);\
												}while(0)

	#endif	
#endif			
									
//****************************************************************************************
//                 I2S模式配置        
//****************************************************************************************
#ifdef FUNC_WIFI_EN
  #define FUNC_I2SIN_EN
#endif

#ifdef WIFI_I2S_SLAVE_SELECT
	#define FUNC_I2S_MASTER_EN				//enbale:master mode, disable: slave mode
#endif
#if (WIFI_I2S_MCLK_PORT == I2S_MCLK_PORT_0)
        #define I2SIN_MCLK_IO_PORT      0   //mclk port: 0--B2
#elif (WIFI_I2S_MCLK_PORT == I2S_MCLK_PORT_1)
		#define I2SIN_MCLK_IO_PORT      1   //mclk port: 1--C8
#elif (WIFI_I2S_MCLK_PORT == I2S_MCLK_PORT_NO)
		#define I2SIN_MCLK_IO_PORT      0xFF   //mclk port:  0xFF--NO USE
#endif

#if (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_0)
        #define I2SIN_I2S_IO_PORT       0   //i2s port: 0-- lrck: B3, bclk: B4, din: B6, do: B5;
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_1)
		#define I2SIN_I2S_IO_PORT       1   //          1-- lrck: B24, bclk: B25, din: B27, do: B26
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_2)
		#define I2SIN_I2S_IO_PORT       2   //          2-- lrck: C9, bclk: C10, din: C12, do: C11  
#elif (WIFI_I2S_PORT_GROUP == I2S_GPIO_GROUP_NO)
		#define I2SIN_I2S_IO_PORT       0xFF    //   0xFF--NO USE
#endif
                                            //          0xff--NO USE

//****************************************************************************************
//                 USB一线通模式配置        
//****************************************************************************************
//	#define FUNC_USB_AUDIO_EN				//单独的USB声卡模式
//	#define FUNC_USB_READER_EN				//单独的USB读卡器模式
//	#define FUNC_USB_AUDIO_READER_EN		//一线通模式
		#define PC_PORT_NUM			        1

//****************************************************************************************
//                 录音模式配置        
//****************************************************************************************
	#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))
		//#define FUNC_PLAY_RECORD_FOLDER_EN		//播放模式播放record文件夹
		//#define FUNC_RECORD_EN   				//录音功能
		#ifdef FUNC_RECORD_EN
			#define FUNC_RECORD_FILE_DELETE_EN 		//支持录音删除功能
			#define FUNC_REC_FILE_REFRESH_EN 		//定时保存文件（避免掉电后录音数据不能保存的问题）
			#define FUNC_REC_PLAYBACK_EN 			//录音回放功能

			#define RECORD_FORMAT		MP3_FORMAT	// MP3_FORMAT , WAV_FORMAT
			#define RECORD_BITRATE		96			// 双声道最高支持320kbps，单声道最高支持192kbps
			#define RECORD_CHANNEL		1			// 必须是 1 或者 2，等于2时可能MEM不够，需要去除一些功能
		#endif
	#endif

//****************************************************************************************
//                 RTC功能配置        
//****************************************************************************************
#ifdef FUNC_WIFI_SUPPORT_RTC_EN
	#define FUNC_RTC_EN						//rtc1|?ü
#endif

	#ifdef FUNC_RTC_EN
		#define FUNC_RTC_ALARM 				//alarm功能
		#define FUNC_RTC_LUNAR
		#define FUNC_RTC_ALARM_SAVE2FLASH 	//alarm闹钟参数保存到FLASH，用于没有电池的时钟产品
	#endif


//****************************************************************************************
//                 蓝牙模式配置        
//****************************************************************************************
	#define FUNC_BT_EN											//蓝牙功能
		#define BT_DEVICE_TYPE		BTUartDeviceMTK662X			// bt chip type

#if (BT_DEVICE_TYPE == BTUartDeviceRTL8761)
		#define BLE_STACK										// 使能BLE
#endif
		#define BT_DEVICE_NAME		"SWL001"					// 蓝牙设备名

		#define BT_LDOEN_RST_GPIO_PORT				GPIO_PORT_C
		#define BT_LDOEN_RST_GPIO_PIN				(0)

		#define BT_UART_RTS_PORT					(1)			// 0 --- A12; 1 --- B31; 2 --- C1

//		#define AP80_DEV_BOARD									// AP80_DEV_BOARD : 为MV的AP80开发板，不定义则适用于MVs的大邮票版

		#ifdef AP80_DEV_BOARD
			#define MTK_BT_PWR_EN					GPIOB23
			#define MTK_BT_RESET					GPIOB31
		#else
			#define MTK_BT_PWR_EN					GPIOC0
			#define MTK_BT_RESET					GPIOB31
		#endif

	#ifdef FUNC_BT_EN
//		#define FUNC_BT_HF_EN									//bt hf call module
		#ifdef FUNC_BT_HF_EN

			//#define FUNC_BT_HF_UART								// 蓝牙电话语音通过UART传输(仅RTK8761支持此功能)

			#define MAX_BTHF_VOLUME					(15)		//蓝牙最大音量等级
			#ifdef FUNC_RECORD_EN
				#define BT_HF_RECORD_FUNC						// 蓝牙通话录音功能
			#endif

			#ifdef FUNC_SOUND_REMIND
//				#define BT_HF_NUMBER_REMIND						// 蓝牙语音报号功能
			#endif

			#define NON_LINEAR_PROCESS				(1)			// "非线性处理"(AEC之后的后处理模块，起到进一步压制回声，以及底噪抑制等)打开或者关闭；0:关闭，1:打开
			#define NOISE_SUPPRESSION_LEVEL			(0)			// 噪声抑制等级。取值范围(1 ~ 5)，0为关闭此功能
			#define BT_REDIAL_CALL_IPONE                		// 蓝牙回拨功能
		#endif

		#define BT_RECONNECTION_FUNC							// 蓝牙自动重连功能
		#ifdef BT_RECONNECTION_FUNC
			#define BT_POWERON_RECONNECTION						// 开机自动重连
			#ifdef BT_POWERON_RECONNECTION
				#define BT_POR_TRY_COUNTS			(7)			// 开机重连尝试次数
				#define BT_POR_INTERNAL_TIME		(3)			// 开机重连每两次间隔时间(in seconds)
			#endif

			#define BT_BB_LOST_RECONNECTION						// BB Lost之后自动重连
			#ifdef BT_BB_LOST_RECONNECTION
				#define BT_BLR_TRY_COUNTS			(90)		// BB Lost 尝试重连次数
				#define BT_BLR_INTERNAL_TIME		(5)			// BB Lost 重连每两次间隔时间(in seconds)
			#endif

		#endif

		#define BT_ADVANCED_AVRCP_SUPPORT						// 高级AVRCP支持
		#ifdef BT_ADVANCED_AVRCP_SUPPORT
			#define BT_VOLUME_SYNC								// 音量同步
			#define BT_SONG_TRACK_INFOR							// 歌曲ID3信息反馈
//			#define BT_SONG_PLAY_STATUS							// 歌曲播放状态
		#endif

		//#define FUNC_BT_DISABLE_SIMPLE_PARING					//取消simple pairing功能，自定义PIN CODE

		//#define FUNC_BT_PBAP_EN									//PBAP功能
		
//		#define BT_APPLE_BATTERY_STATUS							// iphone显示电池电量

//		#define MASS_PRODUCTION_TEST_FUNC				  		// 量产测试功能

//		#define FAST_POWER_ON_OFF_BT							// 快速打开/关闭蓝牙功能

//		#define BT_RF_POWER_ON_OFF								// BT RF 硬件开关
		
		#define TEST_CLEAR_BT_PAIRED_INFO         				//强制进入配对模式

//		#define AUTO_MV_BT_TEST_FUNC							// auto scan and test bt device, 
																// one must be initialized with 
																// [MvBtAutoTestFlag = 1] in this case,
																// please refer to the related files.
		#if defined(FAST_POWER_ON_OFF_BT) && defined(BT_RF_POWER_ON_OFF)
			#error	"Cannot define FAST_POWER_ON_OFF_BT and BT_RF_POWER_ON_OFF at the same time"
		#endif
	#endif

//****************************************************************************************
//                 UART DEBUG功能配置        
//****************************************************************************************
#define FUNC_DEBUG_EN

#ifdef FUNC_DEBUG_EN
//	#define FUNC_FUART_DBG_EN							//FUART DEBUG
//	#define FUNC_BUART_DBG_EN							//BUART DEBUG
	#define FUNC_SWUART_DBG_EN   						//使用软件UART做打印口
	
	#if defined(FUNC_FUART_DBG_EN)
		#define DEBUG_RX_PORT			2  				//rx port  0--A1，1--B6，2--C4，0xFF--NO USE
		#define DEBUG_TX_PORT  			2				//tx port  0--A0，1--B7，2--C3，0xFF--NO USE

	#elif defined(FUNC_BUART_DBG_EN)
		#define DEBUG_RX_PORT			2  				//rx port  0--A13，1--A24，2--B8，3--B29, 0xFF--NO USE
		#define DEBUG_TX_PORT  			2				//tx port  0--A16，1--A25，2--B9，3--B28, 0xFF--NO USE		

	#elif defined(FUNC_SWUART_DBG_EN)
		#define SWUART_GPIO_PORT		SWUART_GPIO_PORT_A	
		#define SWUART_GPIO_INDX		25
		#define SWUART_BAUD_RATE		115200
	#endif
#endif

//****************************************************************************************
//                 WATCH DOG功能配置        
//****************************************************************************************
	#define FUNC_WATCHDOG_EN			//WATCHDOG功能


//****************************************************************************************		
//                 音效功能开关(3D, VB, DRC)
//****************************************************************************************
#define FUNC_AUDIO_EFFECT_EN

#ifdef FUNC_AUDIO_EFFECT_EN
	#include "audio_effect_codec.h"
	#define DIGITAL_PER_GAIN_EN                      //数字信号预增益设置，
	#define DIGITAL_PER_GAIN_VAL              1640   //数字信号预增益值，Vout = DIGITAL_PER_GAIN_VAL/4095*1000mV
	//#define FUNC_AUDIO_EFFECT_EQ_EN                  //EQ功能开关
#endif


//****************************************************************************************		
//				音效功能切换时淡入淡出操作，用于消除pop音(3D, VB, DRC，EQ)
//				目前标准只做了软件EQ切换时做了pop音消除，其他的需要做一定的改动
//****************************************************************************************
//	#define AUDIO_EFFECT_SWITCH_FADEOUT_FADEIN
	
//****************************************************************************************
//                 断点记忆功能配置        
//****************************************************************************************
	#define FUNC_BREAKPOINT_EN			// 系统断点信息管理
	#ifdef FUNC_BREAKPOINT_EN
		#define BP_SAVE_TO_NVM				//将断点信息保存到NVM 中
		#define BP_SAVE_TO_FLASH			// 掉电记忆功能（支持将断点信息保存到FLASH中，防止掉电丢失）
		//#define BP_SAVE_TO_EEPROM			// 掉电记忆功能（支持将断点信息保存到EEPROM中，防止掉电丢失）		
		#define FUNC_MATCH_PLAYER_BP		// 获取FS扫描后与播放模式断点信息相匹配的文件、文件夹ID号
	#endif


//****************************************************************************************
//                 VIN输入5V再次Trim LDO3V3功能配置        
//****************************************************************************************
//	#define FUNC_5VIN_TRIM_LDO3V3			//LDO3V3_TRIM 5VIN功能	


//****************************************************************************************
//                 POWER MONITOR功能配置        
//****************************************************************************************
// 定义电能监视(适用于带电池系统)的功能宏和选项宏
// 电能监视包括，电池电压检测及低电压后的系统行为以及充电指示等
// 电池电压检测，是指LDOIN输入端的电压检测功能(电池系统一般都是电池直接给LDOIN管脚供电)
// 该功能宏打开后，默认包含电池电压检测功能，有关电池电压检测的其它可定义参数，请详见power_monitor.c文件

//USE_POWERKEY_SLIDE_SWITCH 和USE_POWERKEY_SOFT_PUSH_BUTTON 两个宏不要同时定义
	#define USE_POWERKEY_SLIDE_SWITCH			//for slide switch case ONLY
//	#define USE_POWERKEY_SOFT_PUSH_BUTTON 		//for soft push button case ONLY

	#define POWERON_DETECT_VOLTAGE        		//开机检测电池电压，低于关机电压则直接进入关机流程
	#define FUNC_POWER_MONITOR_EN
	#define	OPTION_CHARGER_DETECT		      	//打开该宏定义，支持GPIO检测充电设备插入功能
	
	#ifdef  OPTION_CHARGER_DETECT
			//充电检测端口设置
			#define CHARGE_DETECT_PORT_PU			GPIO_A_PU
			#define CHARGE_DETECT_PORT_PD			GPIO_A_PD
			#define CHARGE_DETECT_PORT_IN			GPIO_A_IN
			#define CHARGE_DETECT_PORT_IE			GPIO_A_IE
			#define CHARGE_DETECT_PORT_OE			GPIO_A_OE
			#define CHARGE_DETECT_BIT				  GPIOA12
	#endif
	

//	#define FUNC_POWERKEY_SOFT_POWERON_EN		//软开关用普通GPIO检测，该功能需要用硬开机和普通GPIO配合实现
//	#define FUNC_GPIO_POWER_ON_EN				//GPIO控制系统上电

	#ifdef FUNC_GPIO_POWER_ON_EN				//GPIO控制系统上电
		#define POWERON_GPIO_PORT_PU			GPIO_A_PU
		#define POWERON_GPIO_PORT_PD			GPIO_A_PD
		#define POWERON_GPIO_PORT_OUT			GPIO_A_OUT
		#define POWERON_GPIO_PORT_IE			GPIO_A_IE
		#define POWERON_GPIO_PORT_OE			GPIO_A_OE
		#define POWERON_GPIO_PORT_BIT			GPIOA10
	#endif


//****************************************************************************************
//                 按键功能配置        
//****************************************************************************************
//	#define FUNC_KEY_BEEP_SOUND_EN    						//按键beep声功能

//ADC KEY定义
//	#define FUNC_ADC_KEY_EN								//ADC KEY宏开关  									
		#define ADC_KEY_PORT_CH1	ADC_CHANNEL_B7			//如果只使用1路ADC按键，则保留本行
//		#define	ADC_KEY_PORT_CH2	ADC_CHANNEL_B5			//如果只使用2路ADC按键，则保留本行

//	#define ADC_POWER_MONITOR_EN               //打开该宏定义，则电源检测在ADC检测电池电压
	#ifdef ADC_POWER_MONITOR_EN
		#define ADC_POWER_MONITOR_PORT    ADC_CHANNEL_B23 
	#endif 

//LIGHT 暖光灯自动检测接口定义
	#define LIGHT_DETECT_AUTO_CONTROL_EN
	#ifdef LIGHT_DETECT_AUTO_CONTROL_EN
		#define	  LIGHT_DETECT_ADC_CHANNEL		ADC_CHANNEL_B7
	
		#define   LIGHT_DETECT_GPIO1_PORT_OUT	GPIO_C_OUT	
		#define   LIGHT_DETECT_GPIO1_MASK_BIT	GPIOC2	
	
		//#define   LIGHT_DETECT_GPIO2_PORT_OUT	GPIO_C_OUT	
		//#define   LIGHT_DETECT_GPIO2_MASK_BIT	GPIOC11	
	
		//#define   LIGHT_DETECT_GPIO3_PORT_OUT	GPIO_C_OUT	
		//#define   LIGHT_DETECT_GPIO3_MASK_BIT	GPIOC12	
	#endif

//IR KEY定义
//	#define FUNC_IR_KEY_EN   							//IR KEY宏开关
		#define IR_KEY_PORT			IR_USE_GPIOB7			//IR使用的端口：IR_USE_GPIOA10, IR_USE_GPIOB7 or IR_USE_GPIOC2

//IIC从机按键定义
	#define FUNC_IIC_KEY_EN   							//IIC 从机按键
	#ifdef FUNC_IIC_KEY_EN
		#define I2C_KEY_SCL_PORT_OUT			GPIO_A_OUT		
		#define I2C_KEY_SCL_PORT_BIT			GPIOA22		
		#define I2C_KEY_SDA_PORT_OUT			GPIO_A_OUT	    
		#define I2C_KEY_SDA_PORT_BIT			GPIOA21	     	
	#endif

//GPIO按键定义
	#define FUNC_GPIO_KEY_EN   					//GPIO按键
	#ifdef FUNC_GPIO_KEY_EN
		#define GPIO_KEY1_PORT_OUT				GPIO_C_OUT		
		#define GPIO_KEY1_PORT_BIT				GPIOC1	

		#define GPIO_KEY2_PORT_OUT				GPIO_B_OUT		
		#define GPIO_KEY2_PORT_BIT				GPIOB2	
	#endif

//CODING KEY定义
//	#define FUNC_CODING_KEY_EN							//CODING KEY宏开关			
	#ifdef FUNC_CODING_KEY_EN
		#define 	CODING_KEY_A_PORT_IN	GPIO_B_IN	
		#define		CODING_KEY_A_PORT_OE	GPIO_B_OE 			
		#define		CODING_KEY_A_PORT_PU	GPIO_B_PU 
		#define		CODING_KEY_A_PORT_PD	GPIO_B_PD 					
		#define		CODING_KEY_A_PORT_INT	GPIO_B_INT 				
		#define		CODING_KEY_A_BIT		(1 << 21)	//GPIO_B[21] for signal A 

		#define		CODING_KEY_B_PORT_IN	GPIO_B_IN 				
		#define		CODING_KEY_B_PORT_OE	GPIO_B_OE 			
		#define		CODING_KEY_B_PORT_PU	GPIO_B_PU 
		#define		CODING_KEY_B_PORT_PD	GPIO_B_PD 					
		#define		CODING_KEY_B_BIT		(1 << 20)	//GPIO_B[20] for signal B
	#endif
	
	#define FUNC_POWER_KEY_EN							//POWER KEY宏开关，软开关复用功能
														//需要同时开启宏USE_POWERKEY_SOFT_PUSH_BUTTON 该功能才真正有效
														
//  #define FUNC_KEY_CPH_EN							    //按键长按保持功能
//  #define FUNC_KEY_DBCK_EN							//按键双击
//  #define FUNC_KEY_SCAN_DELAY_EN				  		//按键扫描延时功能

//****************************************************************************************
//                 音频菜单配置        
//****************************************************************************************
//	#define FUNC_AUDIO_MENU_EN						//音量键功能复用选择：MIC音量、回声延时、回声强度、BASS调节、TREB调节
		#define		MENU_TIMEOUT		5000		//进入菜单调节后，如果5秒没有按MENU、VOL_UP、VOL_DW，则自动退出菜单模式

/*************************************************
 *               LED DISPLAY DEFINE                          *
 ************************************************/
 
// 显示模块定义，根据实际选择可支持LED,LCD,单颗LED灯
// FUNC_DISP_EN：是否开启显示功能
// FUNC_SEG_LCD_EN, FUNC_SEG_LED_EN,	FUNC_SINGLE_LED_EN根据需要选择一个或多个，其他更详细的信息见segPanel.h
// 另外还需要配置gDisplayMode变量为需要的模式,显示模式列在下面.
// DISP_DEV_SLED	-> 单颗LED灯
// DISP_DEV_LED1888	-> 三位半段码LED屏
// DISP_DEV_LED8888	-> 四位段码LED屏
// DISP_DEV_LED57	-> 5*7段码LED屏
// DISP_DEV_LCD58	-> 5*8段码LCD屏
//注意显示控制宏与gDisplayMode变量的对应关系，例如要选择LED57显示设备，必须要开启FUNC_SEG_LED_EN宏并将gDisplayMode赋值为DISP_DEV_LED57。

#define FUNC_DISP_EN					//显示功能

#ifdef FUNC_DISP_EN						              
//  #define FUNC_SEG_LED_EN				    //段码LED屏
//	#define FUNC_SEG_LCD_EN				    //段码LCD屏
//	#define FUNC_SINGLE_LED_EN			    //单颗LED灯
	#define FUNC_ALEXA_PWM_LED_EN			  //PWM LED灯
	#define FUNC_LIGHT_PWM_LED_EN			  //PWM LED灯
#endif

//特殊段码LED 屏选择
#ifdef	FUNC_SEG_LED_EN
  //#define FUNC_7PIN_SEG_LED_EN		//7只脚的段码LED 屏
  //#define FUNC_6PIN_SEG_LED_EN		  //6只脚的段码LED 屏
  #define FUNC_AIP1629A_LED_EN            //AIP1629LED段码驱动IC
#endif

#if (defined(FUNC_LIGHT_PWM_LED_EN) || defined(FUNC_ALEXA_PWM_LED_EN))
#include "pwm_led_display.h"												
#endif


//****************************************************************************************
//                 定时关机功能配置        
//****************************************************************************************
//#define FUNC_SLEEP_EN
#define FUNC_SLEEP_LEDOFF_EN
															
#ifdef FUNC_SLEEP_EN
  #define SLEEP_POWEROFF_TMR   30   //单位：分钟
#endif

#ifdef FUNC_SLEEP_LEDOFF_EN
  #define SLEEP_LED_OFF_TMR    5	//单位：秒
#endif

									
//****************************************************************************************
//                 系统模式值配置        
//****************************************************************************************
enum EnumModuleID
{
    MODULE_ID_UNKNOWN     = 0,	// idle

    MODULE_ID_PLAYER_SD,		// player
    MODULE_ID_PLAYER_USB,		// player
    MODULE_ID_BLUETOOTH,		// module bt
    MODULE_ID_RECORDER,			// recorder
    MODULE_ID_RADIO,			// FM/AM radio
    MODULE_ID_LINEIN,			// line in
    MODULE_ID_I2SIN,            // I2s in
    MODULE_ID_USB_AUDIO,		// usb audio
    MODULE_ID_USB_READER,		// usb audio
    MODULE_ID_USB_AUDIO_READER,	// usb audio

    //add more herr
    MODULE_ID_RTC,				// rtc
    MODULE_ID_BT_HF,
	MODULE_ID_WIFI,
	MODULE_ID_PLAYER_WIFI_SD,		// WiFi SD player
	MODULE_ID_PLAYER_WIFI_USB,		// WiFi USB player
	
	MODULE_ID_WIFI_DEFINE_END,		//WiFi端应用模式结束
	//////////////////////////////////////////////////////////////

    // 以上应用模式，可以通过mode键循环切换进入
    //////////////////////////////////////////////////////////////
    // DO NOT MODIFY FOLLOWINGS
    MODULE_ID_END,				// end of moduleID
    // 以下应用模式不能通过mode键切换进入，而是根据实际用途 通过快捷键或特殊条件满足时才会进入

    //MODULE_ID_ALARM_REMIND,		// alarm remind
    MODULE_ID_POWEROFF,			// faked module ID for Power OFF
    MODULE_ID_STANDBY,			// faked module ID for StandBy mode
	MODULE_ID_IDLE,
#ifdef FUNC_REC_PLAYBACK_EN
    MODULE_ID_REC_BACK_PLAY,	// 录音回放模式
#endif
    MODULE_ID_USER_PLUS,		// 用户扩展起始ID
};


//****************************************************************************************
//                 MEM分配        
//****************************************************************************************
//DEC
#define DEC_MEM_ADDR              		(VMEM_ADDR + 0 * 1024)		//decoder 28KB
#define DEC_MEM_MAX_SIZE          		(28 * 1024)            

//ENC
#define ENC_MEM_ADDR					(VMEM_ADDR + 1 * 1024)		//encoder 17KB mp3编码 定义方式 起始1k与pcm_sync.c中定义冲突  mp3只需16k  
#define ENC_MEM_SIZE					(17 * 1024)

//ENC BUF
#define MP3_OUT_SAMPLES_BUF             (VMEM_ADDR + 18 * 1024)		//record buf 19KB
#define MP3_OUT_SAMPLES_BUF_SIZE        (19 * 1024)

//ADPCM - ENCODE
#define ADPCM_ENC_MEM_ADDR				(VMEM_ADDR + 28 * 1024)		// adpcm for bt phone call recording
#define ADPCM_ENC_MEM_SIZE				(3*1024)

//ADPCM - ENCODE BUF
#define ADPCM_OUT_SAMPLES_BUF             (VMEM_ADDR + 31 * 1024)	//record buf 6KB
#define ADPCM_OUT_SAMPLES_BUF_SIZE        (6 * 1024)

//下面几个FIFO必须在PMEM中，即大于(VMEM_ADDR + 32 * 1024)
//PCM FIFO
#define PCM_FIFO_ADDR             		(VMEM_ADDR + 37 * 1024)		//pcm fifo 16KB
#define PCM_FIFO_LEN              		(16 * 1024)

//ADC FIFO
#define ADC_FIFO_ADDR         			(VMEM_ADDR + 53 * 1024)		//adc fifo 4KB
#define ADC_FIFO_LEN             		(4 * 1024)    

//I2S FIFO
#define I2SIN_FIFO_ADDR                 (VMEM_ADDR + 57 * 1024)
#define I2SIN_FIFO_LEN                  (3 * 1024)

//BUART
#define BUART_RX_TX_FIFO_ADDR  			(VMEM_ADDR + 60 * 1024)		//buart fifo 7KB，RX、TX连续相邻
#define BUART_RX_FIFO_SIZE             	(3 * 1024)         
#define BUART_TX_FIFO_SIZE             	(1 * 1024)         


////////////////////内存管理操作/////////////////////////
#define APP_MMM_MALLOC(MemSize, MemType)          OSMalloc(MemSize, MemType)
#define APP_MMM_REALLOC(MemPtr, MemSize, MemType) OSRealloc(MemPtr, MemSize, MemType)
#define APP_MMM_FREE(MemPtr)                      OSFree(MemPtr)

/////////////////////FLASH 操作/////////////////////////
#ifdef FUNC_WIFI_EN
  #define FLASH_ERASE_IN_TCM			//FLASH擦除代码放在TCM中,目前只适用于GD的FLASH
#endif

#include "debug.h"

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif/*_APP_CONFIG_H_*/




