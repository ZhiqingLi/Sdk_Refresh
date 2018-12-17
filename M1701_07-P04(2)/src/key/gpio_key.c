////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2014, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:power_key_driver.c
//
//		Description	:
//					power keyboard driver(soft push botton)
//
//             maintainer: lujiangang
//
//		Changelog	:
//					2014-09-25	Created Source File
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "adc.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"
#include "wakeup.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "sys_app.h"

#define 	GPIO_KEY_JITTER_TIME		20			//消抖时间，该时间和软开关开关机硬件时间有关
#define 	GPIO_KEY_CP_TIME			  5000		
#define 	GPIO_KEY_PWRON_TIME			10000	


#ifdef FUNC_GPIO_KEY_EN

#ifdef GPIO_KEY_PORT_BIT
  #define GPIO_KEY_PORT_IN				  (GPIO_KEY_PORT_OUT - 1)
  #define GPIO_KEY_PORT_IE				  (GPIO_KEY_PORT_OUT + 1)
  #define GPIO_KEY_PORT_OE				  (GPIO_KEY_PORT_OUT + 2)
  #define GPIO_KEY_PORT_PU				  (GPIO_KEY_PORT_OUT + 5)
  #define GPIO_KEY_PORT_PD				  (GPIO_KEY_PORT_OUT + 6)
#endif

typedef enum _GPIO_KEY_STATE
{
	GPIO_KEY_STATE_IDLE,
	GPIO_KEY_STATE_JITTER,
	GPIO_KEY_STATE_PRESS_DOWN,
	GPIO_KEY_STATE_CP,

} GPIO_KEY_STATE;

TIMER			GpioKeyWaitTimer;
GPIO_KEY_STATE	GpioKeyState;
										
static const uint16_t GpioKeyEvent[5] = {MSG_NONE/*按键开始*/, MSG_NONE/*短按松开*/,MSG_WIFI_WPS,MSG_WIFI_WPS,MSG_NONE};	


// Initialize POWER_KEY scan operation.
void GpioKeyScanInit(void)
{
	GpioKeyState = GPIO_KEY_STATE_IDLE;
	GpioClrRegOneBit(GPIO_KEY_PORT_OE, GPIO_KEY_PORT_BIT);
	GpioClrRegOneBit(GPIO_KEY_PORT_PU, GPIO_KEY_PORT_BIT);
	GpioClrRegOneBit(GPIO_KEY_PORT_PD, GPIO_KEY_PORT_BIT);
	GpioSetRegOneBit(GPIO_KEY_PORT_IE, GPIO_KEY_PORT_BIT);
	APP_DBG("Gpio Key  Init\n");
}


uint16_t GpioKeyScan(void)							
{
	//APP_DBG("Get Gpio Key Status = %d\n", GpioGetReg(GPIO_KEY_PORT_IN) & GPIO_KEY_PORT_BIT);
	switch(GpioKeyState)
	{
		case GPIO_KEY_STATE_IDLE:
			if(GpioGetReg(GPIO_KEY_PORT_IN) & GPIO_KEY_PORT_BIT)
			{
				return MSG_NONE;
			}
			else
			{	
				TimeOutSet(&GpioKeyWaitTimer, GPIO_KEY_JITTER_TIME);
				GpioKeyState = GPIO_KEY_STATE_JITTER;
			}
			break;
		case GPIO_KEY_STATE_JITTER:
			if(GpioGetReg(GPIO_KEY_PORT_IN) & GPIO_KEY_PORT_BIT)
			{
				GpioKeyState = GPIO_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&GpioKeyWaitTimer))
			{
				GpioKeyState = GPIO_KEY_STATE_PRESS_DOWN;
				TimeOutSet(&GpioKeyWaitTimer, GPIO_KEY_CP_TIME);
				return GpioKeyEvent[0];			//return key sp value
			}
			break;
			
		case GPIO_KEY_STATE_PRESS_DOWN:
			if((GpioGetReg(GPIO_KEY_PORT_IN) & GPIO_KEY_PORT_BIT) && !IsTimeOut(&GpioKeyWaitTimer))
			{
				GpioKeyState = GPIO_KEY_STATE_IDLE;
				return GpioKeyEvent[1];
			}
			else if(IsTimeOut(&GpioKeyWaitTimer))
			{
				GpioKeyState = GPIO_KEY_STATE_CP;
				return GpioKeyEvent[2];
			}
			break;
			
		case GPIO_KEY_STATE_CP:
			//此处仅保证一次按键不会响应多次短按
			if((GpioGetReg(GPIO_KEY_PORT_IN) & GPIO_KEY_PORT_BIT))
			{
				GpioKeyState = GPIO_KEY_STATE_IDLE;
				return GpioKeyEvent[0];//return MSG_NONE;				
			}
			else
			{
				//do no thing
			}
			break;
			
		default:
			GpioKeyState = GPIO_KEY_STATE_IDLE;
			break;
	}
	return MSG_NONE;
}

#endif
//end #if (defined(FUNC_POWER_KEY_EN) && defined(USE_POWERKEY_SOFT_PUSH_BUTTON))
