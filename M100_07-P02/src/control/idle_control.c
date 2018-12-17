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

#ifdef	OPTION_CHARGER_DETECT
extern bool IsInCharge(void);
extern bool IsIChargeFullDet(void);
#endif
TIMER ChargeLedTmr;

void SysPowerOnControl(bool Flag);

void IdleControl(void)
{
	uint16_t Msg = 0;

	APP_DBG("Enter Idle\n");

#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif

#ifdef FUNC_GPIO_POWER_ON_EN
	SysPowerOnControl(FALSE);
#endif	

#ifdef FUNC_WIFI_POWER_KEEP_ON
	WiFiPowerOff();
#endif 
	
#ifdef FUNC_AMP_MUTE_EN
	AmpMuteControl(TRUE);
#endif
	
	TimeOutSet(&ChargeLedTmr, 0);
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
		Msg = MsgRecv(20);// 消息接收，无消息阻塞20ms，有消息立即返回
		
		if(IsTimeOut(&ChargeLedTmr))
		{
			static bool LedFlag = FALSE;
			TimeOutSet(&ChargeLedTmr, 500);
			LedFlag = !LedFlag;
			LED_ALL_MODE_OFF();
#ifdef OPTION_CHARGER_DETECT
			if(LedFlag && !IsIChargeFullDet())
			{
			  LED_RED_MODE_ON();
			}
#endif
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
	
	LED_ALL_MODE_OFF();
	APP_DBG("Exit Idle\n");
	
}
