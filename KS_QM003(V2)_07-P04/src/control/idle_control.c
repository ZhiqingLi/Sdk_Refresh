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
#include "sound_remind.h"
#include "wifi_control.h"
#include "wakeup.h"
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#include "singled_display.h"
#endif

TIMER ChargeLedTmr;

void IdleControl(void)
{
	uint16_t Msg = 0;

	APP_DBG("Enter Idle\n");

#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif

	if (IsInCharge()) {
		SoundRemind(SOUND_CHARGING);
		if (IS_RTC_WAKEUP()) {
			RTC_WAKEUP_FLAG_CLR();
			APP_DBG ("Charge clr RTC wakeup flag;\n");
		}
	}

#ifdef FUNC_AMP_MUTE_EN
	AmpMuteControl(TRUE);
#endif	
	
#ifdef FUNC_GPIO_POWER_ON_EN 
	SysPowerOnControl(FALSE);
#endif	

#ifdef FUNC_SINGLE_LED_EN
	SingleLedDisplayModeSet(LED_DISPLAY_MODE_NIGHTLAMP, FALSE);
	SingleLedDisplayModeSet(LED_DISPLAY_MODE_WPSCONNECT, FALSE);
	SingleLedDisplayModeSet(LED_DISPLAY_MODE_LOWBATTER, FALSE);
#endif

	AudioAnaSetChannel(AUDIO_CH_NONE);
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
		}
		
		switch(Msg)
		{
			case MSG_MODE:        //working mode changing
				Msg = MSG_COMMON_CLOSE;
				break;
				
			case MSG_COMMON_CLOSE:
				break;
			
			default:
				CommonMsgProccess(Msg);
				break;
		}
	}
	
	APP_DBG("Exit Idle\n");

	if (IS_NEXT_WORK_MODULE()) {
#ifdef FUNC_SINGLE_LED_EN
		SingleLedDisplayModeSet(LED_DISPLAY_MODE_WPSCONNECT, TRUE);
#endif
#ifdef FUNC_WIFI_POWER_KEEP_ON
		WiFiControlGpioInit();	
		WaitMs(500);
		WiFiPowerOn();
#endif
		if(!IS_RTC_WAKEUP())
		{
			gSys.IsRemindPowerOn = TRUE;
		}	
	}
}
