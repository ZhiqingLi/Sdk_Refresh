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

#define 	GPIO_KEY_NUM_CNT		    5			//GPIO 按键数量
#define 	GPIO_KEY_JITTER_TIME		20			//消抖时间，该时间和软开关开关机硬件时间有关
#define 	GPIO_KEY_CP_TIME			1000		
#define 	GPIO_KEY_CPH_TIME			250	


#ifdef FUNC_GPIO_KEY_EN

typedef enum _GPIO_KEY_STATE
{
	GPIO_KEY_STATE_IDLE,
	GPIO_KEY_STATE_JITTER,
	GPIO_KEY_STATE_PRESS_DOWN,
	GPIO_KEY_STATE_CP,

} GPIO_KEY_STATE;

TIMER			GpioKeyWaitTimer[GPIO_KEY_NUM_CNT];
GPIO_KEY_STATE	GpioKeyState[GPIO_KEY_NUM_CNT];
										
static const uint16_t GpioKeyEvent[][5] = 
{
//	PDS(按键开始)				SPR(短按松开)			CPS(长按开始)			CPH(长按保持)			  	CPR(长按松开)
	{MSG_NONE, 				MSG_VOL_DW,			MSG_VOL_DW, 		MSG_VOL_DW,				MSG_NONE},
	{MSG_NONE, 				MSG_VOL_UP,	        MSG_VOL_UP,     	MSG_VOL_UP, 			MSG_NONE},
    {MSG_NONE, 				MSG_PLAY_PAUSE,		MSG_NONE,			MSG_NONE,				MSG_NONE},
	{MSG_NONE, 				MSG_MUTE,   	    MSG_NONE,   		MSG_NONE,   			MSG_NONE},
    {MSG_NONE, 				MSG_WIFI_TALK,		MSG_NONE,			MSG_NONE,				MSG_NONE},
};	

// Initialize POWER_KEY scan operation.
void GpioKeyScanInit(void)
{
	//memset(&GpioKeyState, GPIO_KEY_STATE_IDLE, sizeof(GpioKeyState));
#ifdef GPIO_KEY1_PORT_BIT
	GPIO_PullHigh_Init(GPIO_KEY1_PORT_OUT,GPIO_KEY1_PORT_BIT);
	GPIO_Init(GPIO_KEY1_PORT_OUT,GPIO_KEY1_PORT_BIT,Intput);       //PA0.0 as output
#endif

#ifdef GPIO_KEY2_PORT_BIT
	GPIO_PullHigh_Init(GPIO_KEY2_PORT_OUT,GPIO_KEY2_PORT_BIT);
	GPIO_Init(GPIO_KEY2_PORT_OUT,GPIO_KEY2_PORT_BIT,Intput);
#endif

#ifdef GPIO_KEY1_PORT_BIT
    GPIO_PullHigh_Init(GPIO_KEY3_PORT_OUT,GPIO_KEY3_PORT_BIT);
    GPIO_Init(GPIO_KEY3_PORT_OUT,GPIO_KEY3_PORT_BIT,Intput);       //PA0.0 as output
#endif

#ifdef GPIO_KEY4_PORT_BIT
    GPIO_PullHigh_Init(GPIO_KEY4_PORT_OUT,GPIO_KEY4_PORT_BIT);
    GPIO_Init(GPIO_KEY4_PORT_OUT,GPIO_KEY4_PORT_BIT,Intput);
#endif

#ifdef GPIO_KEY5_PORT_BIT
    GPIO_PullHigh_Init(GPIO_KEY5_PORT_OUT,GPIO_KEY5_PORT_BIT);
    GPIO_Init(GPIO_KEY5_PORT_OUT,GPIO_KEY5_PORT_BIT,Intput);       //PA0.0 as output
#endif
    
	APP_DBG("Gpio Key  Init\n");
}


uint16_t GpioKeyScan(void)							
{
	static uint8_t PrevGpioKeyIndex = 0xff,GpioScanIndex = 0;
	uint8_t GpioKeyIndex = 0xff,GpioKeyMask;
    uint16_t Msg = MSG_NONE;
    
#ifdef GPIO_KEY1_PORT_BIT
	if(!GPIO_Read_Status(GPIO_KEY1_PORT_OUT,GPIO_KEY1_PORT_BIT))
	{
		GpioKeyIndex &= ~(1);
	}
#endif
#ifdef GPIO_KEY2_PORT_BIT
	if(!GPIO_Read_Status(GPIO_KEY2_PORT_OUT,GPIO_KEY2_PORT_BIT))
	{
		GpioKeyIndex &= ~(1<<1);
	}
#endif
#ifdef GPIO_KEY3_PORT_BIT
    if(!GPIO_Read_Status(GPIO_KEY3_PORT_OUT,GPIO_KEY3_PORT_BIT))
    {
        GpioKeyIndex &= ~(1<<2);
    }
#endif
#ifdef GPIO_KEY4_PORT_BIT
    if(!GPIO_Read_Status(GPIO_KEY4_PORT_OUT,GPIO_KEY4_PORT_BIT))
    {
        GpioKeyIndex &= ~(1<<3);
    }
#endif
#ifdef GPIO_KEY5_PORT_BIT
    if(!GPIO_Read_Status(GPIO_KEY5_PORT_OUT,GPIO_KEY5_PORT_BIT))
    {
        GpioKeyIndex &= ~(1<<4);
    }
#endif

    GpioScanIndex++;
    if(GpioScanIndex >= GPIO_KEY_NUM_CNT)
    {
        GpioScanIndex = 0;
    }
    GpioKeyMask = ~(1<<GpioScanIndex);
    
	switch(GpioKeyState[GpioScanIndex])
	{     
		case GPIO_KEY_STATE_IDLE:
			if((GpioKeyIndex|GpioKeyMask) == 0xff)
			{
				PrevGpioKeyIndex |= (~GpioKeyMask);
			}
			else
			{	
				TimeOutSet(&GpioKeyWaitTimer[GpioScanIndex], GPIO_KEY_JITTER_TIME);
				PrevGpioKeyIndex &= GpioKeyMask;
				GpioKeyState[GpioScanIndex] = GPIO_KEY_STATE_JITTER;
			}
            Msg = MSG_NONE;
			break;
			
		case GPIO_KEY_STATE_JITTER:
			if((PrevGpioKeyIndex|GpioKeyMask) != (GpioKeyIndex|GpioKeyMask))
			{
				GpioKeyState[GpioScanIndex] = GPIO_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&GpioKeyWaitTimer[GpioScanIndex]))
			{
				GpioKeyState[GpioScanIndex] = GPIO_KEY_STATE_PRESS_DOWN;
				TimeOutSet(&GpioKeyWaitTimer[GpioScanIndex], GPIO_KEY_CP_TIME);
				Msg = GpioKeyEvent[GpioScanIndex][0];			//return key sp value
			}
			break;
			
		case GPIO_KEY_STATE_PRESS_DOWN:
			if((PrevGpioKeyIndex|GpioKeyMask) != (GpioKeyIndex|GpioKeyMask))
			{
				GpioKeyState[GpioScanIndex] = GPIO_KEY_STATE_IDLE;
				Msg = GpioKeyEvent[GpioScanIndex][1];
			}
			else if(IsTimeOut(&GpioKeyWaitTimer[GpioScanIndex]))
			{
				GpioKeyState[GpioScanIndex] = GPIO_KEY_STATE_CP;
				TimeOutSet(&GpioKeyWaitTimer[GpioScanIndex], GPIO_KEY_CPH_TIME);
				Msg = GpioKeyEvent[GpioScanIndex][2];
			}
			break;
			
		case GPIO_KEY_STATE_CP:
			//此处仅保证一次按键不会响应多次短按
			if((PrevGpioKeyIndex|GpioKeyMask) != (GpioKeyIndex|GpioKeyMask))
			{
				GpioKeyState[GpioScanIndex] = GPIO_KEY_STATE_IDLE;
				Msg = GpioKeyEvent[GpioScanIndex][4];//return MSG_NONE;				
			}
			else if(IsTimeOut(&GpioKeyWaitTimer[GpioScanIndex])) 
			{                    
				TimeOutSet(&GpioKeyWaitTimer[GpioScanIndex], GPIO_KEY_CPH_TIME);
				Msg = GpioKeyEvent[GpioScanIndex][3];
			}
			break;
			
		default:
			GpioKeyState[GpioScanIndex] = GPIO_KEY_STATE_IDLE;
			break;
	}
    return Msg;
}

#endif
//end #if (defined(FUNC_POWER_KEY_EN) && defined(USE_POWERKEY_SOFT_PUSH_BUTTON))
