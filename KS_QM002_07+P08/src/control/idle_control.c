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
#include "sound_remind.h"

extern void SysPowerOnControl(bool Flag);

void IdleControl(void)
{
	uint16_t Msg = 0;

	APP_DBG("Enter Idle\n");
    
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif

#ifdef FUNC_SINGLE_LED_EN
	BlueLedOn();
#endif

	if(gSys.PowerOnInitFlag)
	{
		SoundRemind(SOUND_PWR_OFF);
		gSys.PowerOnInitFlag = FALSE;
	}
	
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
		if(!IsSoundRemindPlaying())
		{
			WaitMs(100);
			AmpMuteControl(1);
#ifdef FUNC_GPIO_POWER_ON_EN
			SysPowerOnControlDisable();
#endif
#ifdef FUNC_AMP_POWER_EN
            AmpPowerDisable();
#endif
			gSys.NextModuleID = MODULE_ID_POWEROFF;
			MsgSend(MSG_COMMON_CLOSE);
		}
		
		Msg = MsgRecv(20);// 消息接收，无消息阻塞20ms，有消息立即返回
		switch(Msg)
		{
			case MSG_MODE:        //working mode changing
			case MSG_COMMON_CLOSE:
				Msg = MSG_COMMON_CLOSE;
				break;
				
			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
				break;
			
			default:
				CommonMsgProccess(Msg);
				break;
		}
	}
	APP_DBG("Exit Idle\n");
}
