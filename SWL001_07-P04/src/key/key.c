///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: key.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "beep.h"
#include "sys_app.h"
#include "dac.h"
#include "sys_vol.h"
#include "dev_detect_driver.h"
#include "key.h"
#ifdef FUNC_WIFI_EN
#include "wifi_control.h"
#include "wifi_uart_com.h"
#include "wifi_mcu_command.h"
#endif
#ifdef FUNC_RTC_EN
#include "rtc.h"
#include "rtc_control.h"
#endif


#if (defined(FUNC_ADC_KEY_EN) || defined(FUNC_IR_KEY_EN) || defined(FUNC_CODING_KEY_EN) || defined(FUNC_POWER_KEY_EN)) || defined(FUNC_IIC_KEY_EN)

extern void I2cKeyInit(void);
extern void AdcKeyInit(void);
extern void IrKeyInit(void);
extern void CodingKeyInit(void);
extern void GpioKeyScanInit(void);
extern void PowerKeyScanInit(void);

extern uint16_t I2cKeyScan(void);
extern uint16_t AdcKeyScan(void);
extern uint16_t IrKeyScan(void);
extern uint16_t CodingKeyScan(void);
extern uint16_t GpioKeyScan(void);
extern uint16_t PowerKeyScan(void);

#ifdef FUNC_AUDIO_MENU_EN
static TIMER MenuTimer;
#endif
#ifdef FUNC_KEY_BEEP_SOUND_EN
static TIMER BeepDelayMuteTimer;
#endif
#ifdef FUNC_KEY_SCAN_DELAY_EN
static TIMER KeyScanInitDelayTimer;
#endif

void KeyInit(void)
{
#ifdef FUNC_ADC_KEY_EN
	AdcKeyInit();
#endif

#ifdef FUNC_IR_KEY_EN
	IrKeyInit();
#endif

#ifdef FUNC_CODING_KEY_EN
	CodingKeyInit();
#endif

#if (defined(FUNC_POWER_KEY_EN) && defined(USE_POWERKEY_SOFT_PUSH_BUTTON))
	PowerKeyScanInit();
#endif
	
#ifdef FUNC_IIC_KEY_EN 
	I2cKeyInit();
#endif
	
#ifdef FUNC_GPIO_KEY_EN 
	GpioKeyScanInit();
#endif	
	
#ifdef FUNC_AUDIO_MENU_EN
	TimeOutSet(&MenuTimer, 0);
	gSys.AudioSetMode = AUDIO_SET_MODE_MAIN_VOL;	
#endif
	
#ifdef FUNC_KEY_BEEP_SOUND_EN
  TimeOutSet(&BeepDelayMuteTimer, 0);
#endif

#ifdef FUNC_KEY_SCAN_DELAY_EN
  TimeOutSet(&KeyScanInitDelayTimer, 8000);
#endif
}

#ifdef FUNC_KEY_BEEP_SOUND_EN
bool IsBeepSoundEnd(void)
{
	if(gSys.CurModuleID == MODULE_ID_IDLE)
	{
		return TRUE;
	}
	return IsTimeOut(&BeepDelayMuteTimer);
}
#endif

#ifdef FUNC_POWERON_USB_UPDATA_EN
bool IsHardwareUpdataLink(void)
{
	
#ifdef FUNC_ADC_KEY_EN
	if(GetAdcKeyIndex() != 0xFF)
	{
		return TRUE;
	}
#endif

#ifdef FUNC_IR_KEY_EN
	if(GetIrKeyIndex() != 0xFF)
	{
		return TRUE;
	}
#endif
	
#if 0 //def FUNC_IIC_KEY_EN
	if(GetI2cKeyIndex() != 0xFF)
	{
		return TRUE;
	}
#endif

#ifdef FUNC_GPIO_KEY_EN 
	if(GetGpioKeyIndex() != 0xFF)
	{
		return TRUE;
	}
#endif

	return FALSE;
}
#endif

#ifdef FUNC_KEY_CPH_EN
uint8_t GetKeyState(void)
{
	uint8_t State = 0;
	
#ifdef FUNC_ADC_KEY_EN
	if(State == FALSE)
	{
		State = GetAdcKeyState();
	}
#endif

#ifdef FUNC_IR_KEY_EN
	if(State == FALSE)
	{
		State = GetIrKeyState();
	}
#endif
	
#ifdef FUNC_IIC_KEY_EN 
	if(State == FALSE)
	{
		State = GetI2cKeyState();
	}
#endif
	return State;
}
#endif

void KeyScan(void)
{
	uint16_t Msg = MSG_NONE;
#ifdef FUNC_KEY_BEEP_SOUND_EN
	static bool KeyBeepSoundFlag = FALSE;
	static uint16_t BeepKeyMsg;
  uint32_t SampleRateTmp;
#endif

#ifdef FUNC_KEY_SCAN_DELAY_EN
	if(!IsTimeOut(&KeyScanInitDelayTimer))
	{
		return;
	}
#endif
	
#ifdef FUNC_AUDIO_MENU_EN
	if((gSys.AudioSetMode != AUDIO_SET_MODE_MAIN_VOL) && IsTimeOut(&MenuTimer))
	{
		gSys.AudioSetMode = AUDIO_SET_MODE_MAIN_VOL;
		DBG("menu time out!\n");
	}
#endif
	
#ifdef FUNC_ADC_KEY_EN
	if(Msg == MSG_NONE)
	{
		Msg = AdcKeyScan();
	}
#endif

#ifdef FUNC_IR_KEY_EN
	if(Msg == MSG_NONE)
	{
		Msg = IrKeyScan();
	}
#endif

#ifdef FUNC_CODING_KEY_EN
	if(Msg == MSG_NONE)
	{
		Msg = CodingKeyScan();
	}
#endif

#if (defined(FUNC_POWER_KEY_EN) && defined(USE_POWERKEY_SOFT_PUSH_BUTTON))
	if(Msg == MSG_NONE)
	{
		Msg = PowerKeyScan();
	}
#endif
	
#ifdef FUNC_GPIO_KEY_EN
	if(Msg == MSG_NONE)
	{
		Msg = GpioKeyScan();
	}
#endif
	
#ifdef FUNC_IIC_KEY_EN 
	if(Msg == MSG_NONE)
	{
		Msg = I2cKeyScan();
	}
#endif

#ifdef FUNC_KEY_BEEP_SOUND_EN
	//有些功放Unmute有延时，按键Beep beep声适当延时后在播放
	if(KeyBeepSoundFlag)
	{	
#if defined(FUNC_AMP_MUTE_EN) && defined(AMP_SILENCE_MUTE_EN)
		if(((BeepKeyMsg != MSG_MODE) && (PastTimeGet(&BeepDelayMuteTimer) >= 150))
		|| (BeepKeyMsg == MSG_MODE) && (PastTimeGet(&BeepDelayMuteTimer) >= 400))
#endif
		{
			KeyBeepSoundFlag = FALSE;	
			BeepKeyMsg = MSG_NONE;		
			SampleRateTmp = DacAdcSampleRateGet();
			if(0 == SampleRateTmp)
			{
				SampleRateTmp = 44100;
			}		
			APP_DBG("Key  BeepStart>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
			BeepStart(2800, 258, SampleRateTmp, 35, 10);
		}
	}	
#endif	
	
#ifdef FUNC_KEY_DBCK_EN                                  //按键双击功能
	if((Msg == MSG_NONE || Msg == MSG_RTC_SET_PARM)
	&& (gSys.CurModuleID != MODULE_ID_IDLE && gSys.CurModuleID != MODULE_ID_RTC))
	{
		static uint16_t PrevKeyMsg = MSG_NONE;
		static TIMER PowerKeyTimer;
		if(PrevKeyMsg == MSG_NONE && Msg != MSG_NONE)
		{
			TimeOutSet(&PowerKeyTimer, 600);
			PrevKeyMsg = Msg;
			Msg = MSG_NONE;
			DBG("Start!\n");
		}
		else
		{
			if(IsTimeOut(&PowerKeyTimer) && Msg == MSG_NONE)
			{
				Msg = PrevKeyMsg;
				PrevKeyMsg = MSG_NONE;				
			}
			else if(Msg != MSG_NONE && Msg == PrevKeyMsg)
			{
				Msg = MSG_REPEAT;
				PrevKeyMsg = MSG_NONE;
				DBG("End.......!\n");
			}	
		}
	}
#endif
	
#ifdef FUNC_KEY_CPH_EN
	{
		static uint16_t KeyCpCount;
		if(Msg == MSG_WIFI_WPS)
		{
			KeyCpCount++;
			if(KeyCpCount == 10)
			{
				Msg = MSG_WIFI_WPS;
			}
			else
			{
				Msg = MSG_NONE;
			}
		}

		if(GetKeyState() == FALSE)
		{
			KeyCpCount = 0;
		}
	}
#endif
		
	if(Msg != MSG_NONE)
	{
#ifdef FUNC_RTC_EN
#ifdef FUNC_RTC_ALARM
		//根据按键消息，判断是取消闹钟还是闹钟进入下一次贪睡
		if(IsRtcAlarmRingPlaying(Msg))
		{
			if(Msg == MSG_RTC_SET_PARM)
			{
				RtcCurAlarmSleepAndStop(RTC_ALARM_STATE_STOP);
			}
			else
			{
				RtcCurAlarmSleepAndStop(RTC_ALARM_STATE_SLEEP);
			}
			Msg = MSG_NONE;
		}
#endif
#endif
	
		// quick response
		if(Msg == MSG_MODE)
		{
			SetQuickResponseFlag(TRUE);
			SetModeSwitchState(MODE_SWITCH_STATE_START);
		}
#ifdef FUNC_KEY_BEEP_SOUND_EN	
		if((Msg != MSG_WIFI_MIC) && (Msg != MSG_WIFI_TALK))
		{
			KeyBeepSoundFlag = TRUE;
			BeepKeyMsg = Msg;			
#if defined(FUNC_AMP_MUTE_EN) && defined(AMP_SILENCE_MUTE_EN)
			TimeOutSet(&BeepDelayMuteTimer, 500);
			//打开功放，防止按键音被切断
			AmpMuteControl(0);
#endif
		}
#endif

#ifdef  FUNC_UPDATE_CONTROL	
		{
			extern uint8_t ConfirmUpgradeFlag;
			if(ConfirmUpgradeFlag == 1)
			{
				if(Msg == MSG_PLAY_PAUSE)
					ConfirmUpgradeFlag = 2;
			}
		}
#endif


#ifdef FUNC_AUDIO_MENU_EN
		//菜单键处理，设置音量加、音量减按键为不同的功能
		if(Msg == MSG_MENU)
		{
			gSys.AudioSetMode = (gSys.AudioSetMode + 1) % AUDIO_SET_MODE_SUM;	
			TimeOutSet(&MenuTimer, MENU_TIMEOUT);			
		}
		else if((Msg != MSG_VOL_UP) && (Msg != MSG_VOL_DW))
		{
			gSys.AudioSetMode = AUDIO_SET_MODE_MAIN_VOL;	
		}		
		
		if((Msg == MSG_VOL_UP) || (Msg == MSG_VOL_DW))
		{
			TimeOutSet(&MenuTimer, MENU_TIMEOUT);	
			//将MSG_VOL_UP、MSG_VOL_DW转换为相应的复用功能消息		
			switch(gSys.AudioSetMode)
			{
				case AUDIO_SET_MODE_MIC_VOL:
					Msg = (Msg == MSG_VOL_UP) ? MSG_MIC_VOL_UP : MSG_MIC_VOL_DW;
					break;				
				case AUDIO_SET_MODE_ECHO_DEPTH:
					Msg = (Msg == MSG_VOL_UP) ? MSG_ECHO_DEPTH_UP : MSG_ECHO_DEPTH_DW;
					break;				
				case AUDIO_SET_MODE_ECHO_DELAY:
					Msg = (Msg == MSG_VOL_UP) ? MSG_ECHO_DELAY_UP : MSG_ECHO_DELAY_DW;
					break;				
				case AUDIO_SET_MODE_TREB:
					Msg = (Msg == MSG_VOL_UP) ? MSG_TREB_UP : MSG_TREB_DW;
					break;				
				case AUDIO_SET_MODE_BASS:
					Msg = (Msg == MSG_VOL_UP) ? MSG_BASS_UP : MSG_BASS_DW;
					break;
			}
		}				
#endif

#ifdef FUNC_SLEEP_EN
		gSys.SleepTimeCnt = FALSE;
		gSys.SleepStartPowerOff = FALSE;
#endif

#ifdef FUNC_SLEEP_LEDOFF_EN
		gSys.SleepLedOffCnt = FALSE;
		gSys.SleepLedOffFlag = FALSE;
#endif
		
#ifdef FUNC_WIFI_EN                             //WiFi升级中禁止按键操作
		if(WiFiFirmwareUpgradeStateGet() != TRUE)
#endif
		{
			APP_DBG("Key Msg : %x\n", Msg);
			MsgSend(Msg);
		}
	}
}
#endif
