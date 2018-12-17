///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: idle_control.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "idle_control.h"
#include "sys_app.h"
#include "sys_vol.h"
#include "gpio.h"
#include "wifi_control.h"

TIMER ChargeLedTmr;

void SysPowerOnControl(bool Flag);

void IdleControl(void)
{
	uint16_t Msg = 0;

	APP_DBG("Enter Idle\n");

#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif

#ifdef FUNC_AMP_MUTE_EN
	AmpMuteControl(TRUE);
#endif	
	
#ifdef FUNC_GPIO_POWER_ON_EN 
	SysPowerOnControl(FALSE);
#endif	
	
	TimeOutSet(&ChargeLedTmr, 0);
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
#ifdef FUNC_WIFI_POWER_KEEP_ON
	  WiFiPowerOff();
#endif
		Msg = MsgRecv(20);// 消息接收，无消息阻塞20ms，有消息立即返回
		
		if(IsTimeOut(&ChargeLedTmr))
		{
			static bool LedFlag = FALSE;
			TimeOutSet(&ChargeLedTmr, 500);
			LedFlag = !LedFlag;
			LED_ALL_MODE_OFF();
		}
		
		switch(Msg)
		{
			case MSG_MODE:        //working mode changing
				//Msg = MSG_COMMON_CLOSE;
				break;
				
			case MSG_COMMON_CLOSE:
				break;
			
			default:
				//CommonMsgProccess(Msg);
				break;
		}
	}
	
	APP_DBG("Exit Idle\n");
	
#ifdef FUNC_WIFI_POWER_KEEP_ON
	WiFiControlGpioInit();	
	WaitMs(500);
	WiFiPowerOn();
#endif
	
}
