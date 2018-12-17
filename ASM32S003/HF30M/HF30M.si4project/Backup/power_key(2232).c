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

#ifdef FUNC_GPIO_POWER_CONTROL_EN

#define 	POWER_KEY_JITTER_TIME		20			//消抖时间，该时间和软开关开关机硬件时间有关
#define 	POWER_KEY_CP_TIME			1000		
#define 	POWER_KEY_PWRON_TIME		3000	


typedef enum _POWER_KEY_STATE
{
	POWER_KEY_STATE_IDLE,
	POWER_KEY_STATE_JITTER,
	POWER_KEY_STATE_PRESS_DOWN,
	POWER_KEY_STATE_CP,

} POWER_KEY_STATE;

B_T PowerKeyInitFlag = TRUE;
TIMER			PowerKeyWaitTimer;
POWER_KEY_STATE	PowerKeyState;
										

#ifdef FUNC_GPIO_ON_OFF_EN
#ifdef FUNC_POWER_KEY_EN
    #warning    "MV: PUSH BUTTON POWER KEY ENABLE!"
#endif /* FUNC_POWER_KEY_EN */

static const uint16_t PowerKeyEvent[2] = {MSG_NONE/*按键开始*/, MSG_MODE/*短按松开*/};

// POWER_KEY与普通的按键不同，连接按钮开关（软开关）时的主要作用还是系统开关机，当然，也允许复用短按功能。
// 短按产生时，推送短按消息。超过短按区间，此处不做任何处理，由系统硬件检测关机。
uint16_t PowerKeyScan(void)							
{
	if(PowerKeyInitFlag)  
	//防止开机时长按POWER键产生短按消息，这里系统启动后，只有检测到按键松开后，才会开始检测短按消息
	{
		if(!GPIO_Read_Status(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT))
		{
			return MSG_NONE;
		}
		else if(GPIO_Read_Status(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT))
		{
			PowerKeyInitFlag = FALSE;
		}
	}
    
	switch(PowerKeyState)
	{
		case POWER_KEY_STATE_IDLE:
			if(GPIO_Read_Status(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT))
			{
				return MSG_NONE;
			}
			else
			{	
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_JITTER_TIME);
				PowerKeyState = POWER_KEY_STATE_JITTER;
			}
			break;
            
		case POWER_KEY_STATE_JITTER:
			if(GPIO_Read_Status(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT))
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_PRESS_DOWN;
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_CP_TIME);
				return PowerKeyEvent[0];			//return key sp value
			}
			break;
			
		case POWER_KEY_STATE_PRESS_DOWN:
			if(GPIO_Read_Status(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT))
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
#ifdef FUNC_POWER_KEY_EN
                /* 产生短按消息 */
				return PowerKeyEvent[1];
#else
				return MSG_NONE;
#endif
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_CP;
                TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_PWRON_TIME);
				return PowerKeyEvent[0];
			}
			break;
			
		case POWER_KEY_STATE_CP:
			//此处仅保证一次按键不会响应多次短按
			if(GPIO_Read_Status(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT))
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
				return PowerKeyEvent[0];//return MSG_NONE;				
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				/* 检测到关机按键,系统进入关机，不会返回*/
                APP_DBG("Detect power off key,System will enter power down!!!!\n");
                SystemGotoDeepSleepPowerDown();
			}
			break;
			
		default:
			PowerKeyState = POWER_KEY_STATE_IDLE;
			break;
	}
	return MSG_NONE;
}

/*****************************************************************************
 函 数 名  : SystemPowerOnDetect
 功能描述  : 系统开机条件检测，如果满足开机条件，系统开机，否系统进入睡眠模式，
            不会返回
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月12日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void SystemPowerOnDetect(void)
{
    
}
#endif
//end #ifdef FUNC_GPIO_ON_OFF_EN

#ifdef FUNC_GPIO_POWERON_EN 
void SysPowerOnControl(bool Flag)
{
	APP_DBG("GpioControlSysPower:%d*******\n", Flag);
	if(Flag)
	{
        GPIO_Write_High(GPIO_POWERON_PORT_OUT,GPIO_POWERON_PORT_BIT);
	}
	else
	{
        GPIO_Write_Low(GPIO_POWERON_PORT_OUT,GPIO_POWERON_PORT_BIT);
	}
}
#endif

// Initialize POWER_KEY scan operation.
void SystemPowerControlInit(void)
{
	APP_DBG("System power control Init*******\n");
    
#ifdef FUNC_GPIO_ON_OFF_EN
    PowerKeyState = POWER_KEY_STATE_IDLE;
	GPIO_PullHigh_Init(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT);
	GPIO_Init(GPIO_ON_OFF_PORT_OUT,GPIO_ON_OFF_PORT_BIT,Intput);       
#endif
#ifdef FUNC_GPIO_POWERON_EN
	GPIO_PullLow_Init(GPIO_POWERON_PORT_OUT,GPIO_POWERON_PORT_BIT);
	GPIO_Init(GPIO_POWERON_PORT_OUT,GPIO_POWERON_PORT_BIT,Output); 
    GPIO_Write_Low(GPIO_POWERON_PORT_OUT,GPIO_POWERON_PORT_BIT);
#endif /* FUNC_GPIO_POWERON_EN */
}
#endif //FUNC_GPIO_POWER_CONTROL_EN

