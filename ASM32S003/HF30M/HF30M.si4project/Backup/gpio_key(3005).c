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
#include "app_config.h"


#define 	GPIO_KEY_JITTER_TIME		20			//消抖时间，该时间和软开关开关机硬件时间有关
#define 	GPIO_KEY_CP_TIME			500		
#define 	GPIO_KEY_PWRON_TIME			10000	


#ifdef FUNC_GPIO_KEY_EN

typedef enum _GPIO_KEY_STATE
{
	GPIO_KEY_STATE_IDLE,
	GPIO_KEY_STATE_JITTER,
	GPIO_KEY_STATE_PRESS_DOWN,
	GPIO_KEY_STATE_CP,

} GPIO_KEY_STATE;

TIMER			GpioKeyWaitTimer;
GPIO_KEY_STATE	GpioKeyState;
										
static const uint16_t GpioKeyEvent[2][5] = 
{
//	PDS(按键开始)				SPR(短按松开)			CPS(长按开始)			CPH(长按保持)			  	CPR(长按松开)
	{MSG_NONE, 				MSG_MODE,			MSG_MODE,			MSG_NONE,				MSG_NONE},
	{MSG_NONE, 				MSG_LIGHT_ON_OFF,	MSG_PWR_KEY_SP,		MSG_PWR_KEY_SP,				MSG_NONE},
};	

// Initialize POWER_KEY scan operation.
void GpioKeyScanInit(void)
{
	GpioKeyState = GPIO_KEY_STATE_IDLE;
#ifdef GPIO_KEY1_PORT_BIT
	GPIO_PullHigh_Init(GPIO_KEY1_PORT_OUT,GPIO_KEY1_PORT_BIT);
	GPIO_Init(GPIO_KEY1_PORT_OUT,GPIO_KEY1_PORT_BIT,Intput);       //PA0.0 as output
#endif

#ifdef GPIO_KEY2_PORT_BIT
	GPIO_PullHigh_Init(GPIO_KEY2_PORT_OUT,GPIO_KEY2_PORT_BIT);
	GPIO_Init(GPIO_KEY2_PORT_OUT,GPIO_KEY2_PORT_BIT,Intput);
#endif

	APP_DBG("Gpio Key  Init\n");
}


uint16_t GpioKeyScan(void)							
{
	static uint8_t PrevGpioKeyIndex = 0xff;
	uint8_t GpioKeyIndex = 0xff;

	if(!GPIO_Read_Status(GPIO_KEY1_PORT_OUT,GPIO_KEY1_PORT_BIT))
	{
		GpioKeyIndex = 0;
	}
#ifdef GPIO_KEY2_PORT_BIT
	else if(!GPIO_Read_Status(GPIO_KEY2_PORT_OUT,GPIO_KEY2_PORT_BIT))
	{
		GpioKeyIndex = 1;
	}
#endif
	else
	{
		GpioKeyIndex = 0xff;
	}
	
	switch(GpioKeyState)
	{
		case GPIO_KEY_STATE_IDLE:
			if(GpioKeyIndex == 0xff)
			{
				PrevGpioKeyIndex = 0xff;
				return MSG_NONE;
			}
			else
			{	
				TimeOutSet(&GpioKeyWaitTimer, GPIO_KEY_JITTER_TIME);
				PrevGpioKeyIndex = GpioKeyIndex;
				GpioKeyState = GPIO_KEY_STATE_JITTER;
			}
			break;
			
		case GPIO_KEY_STATE_JITTER:
			if(PrevGpioKeyIndex != GpioKeyIndex)
			{
				GpioKeyState = GPIO_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&GpioKeyWaitTimer))
			{
				GpioKeyState = GPIO_KEY_STATE_PRESS_DOWN;
				TimeOutSet(&GpioKeyWaitTimer, GPIO_KEY_CP_TIME);
				return GpioKeyEvent[PrevGpioKeyIndex][0];			//return key sp value
			}
			break;
			
		case GPIO_KEY_STATE_PRESS_DOWN:
			if(PrevGpioKeyIndex != GpioKeyIndex)
			{
				GpioKeyState = GPIO_KEY_STATE_IDLE;
				return GpioKeyEvent[PrevGpioKeyIndex][1];
			}
			else if(IsTimeOut(&GpioKeyWaitTimer))
			{
				GpioKeyState = GPIO_KEY_STATE_CP;
				return GpioKeyEvent[PrevGpioKeyIndex][2];
			}
			break;
			
		case GPIO_KEY_STATE_CP:
			//此处仅保证一次按键不会响应多次短按
			if(PrevGpioKeyIndex != GpioKeyIndex)
			{
				GpioKeyState = GPIO_KEY_STATE_IDLE;
				return GpioKeyEvent[PrevGpioKeyIndex][4];//return MSG_NONE;				
			}
			else 
			{
				return GpioKeyEvent[PrevGpioKeyIndex][3];
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
