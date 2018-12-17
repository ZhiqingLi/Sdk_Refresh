///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: line_in_control.c
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "dac.h"
#include "beep.h"
#include "echo.h"
#include "audio_adc.h"
#include "audio_path.h"
#include "clk.h"
#include "pmem.h"
#include "pcm_transfer.h"
#include "pcm_fifo.h"
#include "line_in_control.h"
#include "wifi_mcu_command.h"
#include "wifi_control.h" 
#include "wifi_uart_com.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "dev_detect_driver.h"
#include "recorder_control.h"
#include "breakpoint.h"
#include "sys_app.h"
#include "nvm.h"
#include "eq_params.h"
#include "bt_app_func.h"
#include "mixer.h"
#include "gpio.h"
#ifdef FUNC_SLAVE_UART_EN
#include "slave_mcu_command.h"
#include "slave_uart_com.h"
#endif
#ifdef	OPTION_CHARGER_DETECT
extern bool IsInCharge(void);
#endif
TIMER LineInLedTmr;

#ifdef FUNC_LINEIN_EN

void LineInModuleLedCb(void)
{
	static bool LedFlag = FALSE;
	
	LedFlag = !LedFlag;
			
	if(gWiFi.VisInfState)
	{
		LED_WHITE_MODE_OFF();
		TimeOutSet(&LineInLedTmr, 500);
		if(LedFlag)
		{
			LED_YELLOW_MODE_ON();
		}					
		return;
	}
	
#ifdef FUNC_SLEEP_LEDOFF_EN
	if(!gSys.SleepLedOffFlag)
#endif
	{
		LED_CYAN_MODE_ON();
	}
	else
	{
		LED_WHITE_MODE_OFF();
	}
}

bool LineInControl(void)
{
	uint16_t Msg = 0;

	APP_DBG("Enter LineInControl...\n");

	if(!IsLineInLink())
	{
		APP_DBG("Exit LineInControl...\n");
		return TRUE;
	}

#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif	
	
	LED_WIFI_MODE_ON();
	LED_BT_MODE_ON();
	SoundRemind(SOUND_LINEIN); 
	TimeOutSet(&LineInLedTmr, 0);
    
#ifdef FUNC_TREB_BASS_EN
	if(gSys.EqStatus == 0)
	{
		TrebBassSet(gSys.TrebVal, gSys.BassVal);
	}
#endif
	
	if(gSys.MicEnable)
	{
		MixerMute(MIXER_SOURCE_ANA_MONO);	
		MixerMute(MIXER_SOURCE_MIC);
	}
	else
	{
		MixerMute(MIXER_SOURCE_ANA_STERO);
	}
	WaitMs(20);
	
	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_LINEIN : AUDIO_CH_LINEIN);
	
	if(gSys.MicEnable)
	{
		MixerUnmute(MIXER_SOURCE_ANA_MONO);	
		MixerUnmute(MIXER_SOURCE_MIC);
	}
	else
	{
		MixerUnmute(MIXER_SOURCE_ANA_STERO);
	}
	
	// 消息注册
    if(gSys.MuteFlag)
	{
    	gSys.MuteFlag = 0;
		AudioPlayerMute(gSys.MuteFlag);
	} 

	AudioSysInfoSetBreakPoint();

	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	
	while(Msg != MSG_COMMON_CLOSE)
	{	
		Msg = MsgRecv(5);

		if(IsTimeOut(&LineInLedTmr))
		{ 
      LineInModuleLedCb();
	  }
		
		switch(Msg)
		{			
			case MSG_MODE:        //working mode changing
				Msg = MSG_COMMON_CLOSE;
				break;
			
			case MSG_POWER:
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
				break;
			
			case MSG_PLAY_PAUSE:
				gSys.MuteFlag = !gSys.MuteFlag;
				break;
			
#ifdef FUNC_RECORD_EN
			case MSG_REC:  //录音
				gSys.ModuleIDBackUp = gSys.CurModuleID;
				gSys.NextModuleID = MODULE_ID_RECORDER;
				gSys.RecordState = RECORD_STATE_REC;
				gSys.RecordSource = RECORD_SOURCE_LINEIN;
				Msg = MSG_COMMON_CLOSE;
				break;
#endif
           
#ifdef FUNC_REC_PLAYBACK_EN
			case MSG_REC_PLAYBACK:
				EnterRecBackPlay(gSys.CurModuleID, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0, 0);
				break;
#endif
			
			case MSG_WIFI_WPS:
				if(WiFiPowerOnInitStateGet() == TRUE)
				{
					WiFiWorkStateSet(WIFI_STATUS_STATION_DISCONNECTED);
					Mcu_SendCmdToWiFi(MCU_WIF_WPS, NULL);
				}
				break;
			
			case MSG_WIFI_FACTORY: //WiFi模组恢复出厂设置 
				if(WiFiPowerOnInitStateGet() == TRUE)
				{
					if(gWiFi.TestModeState)
					{
						Mcu_SendCmdToWiFi(MCU_FCRYPOW, NULL);
					}
					else
					{
						Mcu_SendCmdToWiFi(MCU_FACTORY, NULL);
					}
				}
				break;
				
			default:
				CommonMsgProccess(Msg);
				break;

		}
	}
#if (defined(FUNC_BREAKPOINT_EN) && (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)))
	{
		BP_SYS_INFO *pBpSysInfo;
		pBpSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
		BP_SET_ELEMENT(pBpSysInfo->Volume, gSys.Volume);
    BP_SET_ELEMENT(pBpSysInfo->Eq, gSys.Eq);
		BP_SaveInfo(BP_SAVE2NVM);
	}
#endif
	
  LED_WIFI_MODE_OFF();
	LED_BT_MODE_OFF();
	
	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);	
	APP_DBG("leave LineInControl...\n");
	return TRUE;//OK
}
#endif

