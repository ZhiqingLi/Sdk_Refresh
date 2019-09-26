///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: task_main.c
//  maintainer Halley
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "bt_app_func.h"
#include "line_in_control.h"
#include "i2s_in_control.h"
#include "gpio.h"
#include "sd_card.h"
#include "recorder_control.h"
#include "radio_control.h"
#include "usb_audio_control.h"
#include "rtc_control.h"
#include "idle_control.h"
#include "lcd.h"
#include "wakeup.h"
#include "clk.h"
#include "os.h"
#include "sys_vol.h"
#include "audio_path.h"
#include "task_decoder.h"
#include "nvm.h"
#include "debug.h"
#include "power_management.h"
#include "breakpoint.h"
#include "sys_app.h"
#include "dac.h"
#include "timer.h"
#include "bt_control_api.h"
#include "player_control.h"
#include "recorder_control.h"
#include "dev_detect_driver.h"
#include "sound_remind.h"
#ifdef FUNC_WIFI_EN
#include "wifi_control.h"
#endif
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#include "singled_display.h"
#endif

void EnterIntoNextMode(void);
#ifdef FUNC_RADIO_DIV_CLK_EN
void SysClkSet(uint8_t clk_sel, uint8_t clk_div);
#endif

extern bool BtHfControl(void);

extern uint32_t GetNextModeId(uint32_t CurModeId);

extern int32_t BtTaskHandle;

//main entrance of main task
//主任务程序入口，可按需在对应入口处增加对应的模块调用
void GuiTaskEntrance(void)
{
	#ifdef FUNC_BT_EN
	uint16_t i;
	#endif

	APP_DBG("main task Init...\n");
	SetSysVol();

#ifdef FUNC_SPI_UPDATE_EN
	if(gSys.UpgradeFileSource == UPGRADE_SOURCE_USBSD)
	{
		SoundRemind(SOUND_UPDATE_SUCC);
		gSys.NextModuleID = MODULE_ID_POWEROFF;
		gSys.UpgradeFileSource = UPGRADE_SOURCE_IDLE;
#ifdef FUNC_BREAKPOINT_EN	
		{
			BP_SYS_INFO *pSysInfo;

			pSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
			BP_SET_ELEMENT(pSysInfo->UpgradeFileSource, gSys.UpgradeFileSource);
			BP_SaveInfo(BP_SAVE2NVM);
#ifdef BP_SAVE_TO_FLASH // 掉电记忆
			BP_SaveInfo(BP_SAVE2FLASH);
#endif
		}
#endif
	}
	else
#endif
	if((IS_CUR_WORK_MODULE()) && !IS_RTC_WAKEUP()
#ifdef OPTION_CHARGER_DETECT
	&& !IsInCharge()
#endif
	)
	{
		SoundRemind(SOUND_PWR_ON);
	}

	OSRescheduleTimeout(500);

	// 消息注册
	MsgAddSet(MSG_MAIN_CLASS);
	MsgAddSet(MSG_DEV_CLASS);
	MsgAddSet(MSG_BT_CLASS);
	MsgAddSet(MSG_USB_DEVICE_CLASS);
	
#ifdef FUNC_BT_EN
	APP_DBG("waiting bt dev init.\n");
	for(i = 0; i < 100; i++)
	{
		if((BtTaskHandle != 0) && IsBtLink())
		{
			break;
		}
		OSRescheduleTimeout(5);
	}
#endif

	if(gSys.NextModuleID != MODULE_ID_UNKNOWN) // 开机的过程中插入U盘、SD卡，需要做合理的模式切换
	{
		gSys.CurModuleID = gSys.NextModuleID;
	}
	
#ifdef FUNC_REC_PLAYBACK_EN
	if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY || gSys.CurModuleID == MODULE_ID_RECORDER)
	{
		EnterRecBackPlay(IsUDiskLink() ? MODULE_ID_PLAYER_USB : MODULE_ID_PLAYER_SD, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0 ,0);
	}
#endif

	APP_DBG("Power on CurModuleId = %d\n",gSys.CurModuleID);
	//Task main loop
	while(1)
	{
#ifdef FUNC_WIFI_POWER_KEEP_ON
		//进入一个模式前，如果不是正常工作模式，关闭WiFi电源：20190725
		if (!IS_CUR_WORK_MODULE()) {
			WiFiControlGpioInit();	
			WaitMs(100);
		}
#endif

		gSys.NextModuleID = MODULE_ID_UNKNOWN;	//init the flag	
		MsgClearClass(MSG_MAIN_CLASS);			    //进入一个模式前，先清空主消息队列
		MsgClearClass(MSG_DEV_CLASS);			    //进入一个模式前，先清空主消息队列

#ifdef BT_RF_POWER_ON_OFF
		if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
		{
			BtHwPowerOn();
		}
		else if(gSys.CurModuleID == MODULE_ID_BT_HF)
		{
			
		}
		else
		{
			BtHwPowerOff();
		}
#endif

#ifdef FAST_POWER_ON_OFF_BT
		if((gSys.CurModuleID == MODULE_ID_BLUETOOTH) || (gSys.CurModuleID == MODULE_ID_BT_HF))
		{
			if(IsBTScanPageEnable() != BT_SCAN_PAGE_ENABLE)
			{
				FastEnableBT();
#ifdef BT_POWERON_RECONNECTION
				StartBtReConnection(BT_POR_TRY_COUNTS, BT_POR_INTERNAL_TIME);// reconnect last bt device, try 3 times
#endif
			}
		}
		else
		{
			if(IsBTScanPageEnable() != BT_SCAN_PAGE_DISABLE)
			{
				FastDisableBT();
			}
		}
#endif

#ifdef FUNC_WIFI_EN
#ifdef FUNC_OUTPUT_CHANNEL_AUTO_CHANGE
#ifdef FUNC_BT_HF_EN
		if(gSys.CurModuleID == MODULE_ID_BT_HF)
		{
			gSys.OutputChannelSelect = DAC_OUTPUT_CHANNEL;	
#ifndef FUNC_BT_HF_UART
			GpioPcmSyncIoConfig(1); 		
#endif
		}
		else
#endif
		{
			if(WiFiSlaveSounBoxCntGet() && (gSys.CurModuleID != MODULE_ID_WIFI) 
			&& (gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_WIFI_USB))
			{
				gSys.OutputChannelSelect = CLASSD_OUTPUT_CHANNEL;
			}
			else
			{
				DacVolumeSet(DAC_DIGITAL_VOL, DAC_DIGITAL_VOL);	
#ifdef FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN
				if((gSys.CurModuleID == MODULE_ID_WIFI) || (gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_SD)
				|| (gSys.CurModuleID == MODULE_ID_PLAYER_WIFI_USB))
				{
					gSys.OutputChannelSelect = DAC_CLASSD_OUTPUT_CHANNEL;
				}
				else
#endif
				{
					gSys.OutputChannelSelect = DAC_OUTPUT_CHANNEL;
				}
			}
		}
			
		AudioOutputInit();		
		McuSetPlayMode(gSys.CurModuleID);

#endif
#endif
		
#if defined(FUNC_AMP_MUTE_EN) && defined(AMP_SILENCE_MUTE_EN)
		SilenceMuteDetInit();                   //进入模式前，初始化无信号检测
#endif
		switch(gSys.CurModuleID)//init value can be M0
		{	
			case MODULE_ID_PLAYER_SD:  // enter into module player
			case MODULE_ID_PLAYER_USB: // enter into module player
			case MODULE_ID_BLUETOOTH:  // BT shares the same entry func.
#if defined(FUNC_USB_EN) || defined(FUNC_CARD_EN) || defined(FUNC_BT_EN)
				PlayerControl();
#endif
				break;
			
#ifdef FUNC_BT_HF_EN
			case MODULE_ID_BT_HF:
				BtHfControl();
				break;
#endif
			
//			case MODULE_ID_ALARM_REMIND: // Alarm remind shares the same entry func
//				break;

#ifdef FUNC_LINEIN_EN
			case MODULE_ID_LINEIN:
				LineInControl();
				break;
#endif

#ifdef FUNC_I2SIN_EN
      case MODULE_ID_I2SIN:
#ifndef FUNC_WIFI_EN
                I2sInControl();
#endif
        break;
#endif			
			
#ifdef FUNC_WIFI_EN
#ifdef FUNC_WIFI_PLAY_CARD_EN
			case MODULE_ID_PLAYER_WIFI_SD:
#endif
#ifdef FUNC_WIFI_PLAY_USB_EN
			case MODULE_ID_PLAYER_WIFI_USB:
#endif
			case MODULE_ID_WIFI:
				WiFiControl();
				break;			
#endif
            
#ifdef FUNC_RECORD_EN
			case MODULE_ID_RECORDER:
				RecorderControl();
				break;
#endif

#ifdef FUNC_RADIO_EN
			case MODULE_ID_RADIO:
#ifdef FUNC_SOUND_REMIND
#ifdef FUNC_RADIO_DIV_CLK_EN	
				{
//					ClkSysClkSel(1); // 降频至80M
//					SysClkDivison(gSys.FmSysClkDivFactor);	//ljg, call directly
					SysClkSet(1, gSys.FmSysClkDivFactor);
					gSys.IsSysClk96MHz = 0;  
				}
#endif
#endif
				RadioControl();
#ifdef FUNC_SOUND_REMIND
#ifdef FUNC_RADIO_DIV_CLK_EN
				if(0 == gSys.IsSysClk96MHz)
				{
//					ClkSysClkSel(0); // 再恢复到96M
//					SysClkDivison(1);	//ljg, call directly
					SysClkSet(0, 1);
					gSys.IsSysClk96MHz = 1;  
				}
#endif
#endif
				break;
#endif

#if (defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN))
			case MODULE_ID_USB_AUDIO:
			case MODULE_ID_USB_READER:
			case MODULE_ID_USB_AUDIO_READER:
				UsbAudioControl();
				break;
#endif

#ifdef FUNC_RTC_EN
			case MODULE_ID_RTC:
				//RtcControl();
				break;
#endif
			
#ifdef FUNC_REC_PLAYBACK_EN
            case MODULE_ID_REC_BACK_PLAY:
				RecBackPlayControl();
				break;
#endif

			case MODULE_ID_POWEROFF:
				SystemPowerOffControl();
				break;
		
			case MODULE_ID_STANDBY:
				SystemStandbyControl();
				break;
		
			case MODULE_ID_IDLE:
				IdleControl();
				break;
			
			default:
				APP_DBG("Enter Module error");
				IdleControl();
				break;
		}
		//from cache list or top-frame, update gCurBackModuleID for two UI task's mode
		//use module top diagram for one UI task
		//new device plugin, then switch to the related module:
		//set NextModuleID and gCurModuleID at the interrupt or detection part
		if(gSys.NextModuleID == MODULE_ID_UNKNOWN)
		{
			gSys.NextModuleID = GetNextModeId(gSys.CurModuleID);
			APP_DBG("Next ModuleId = %d\n", gSys.NextModuleID);
		}
		
		gSys.CurModuleID = gSys.NextModuleID;//set mode to the next mode
		if (gSys.IsRemindPowerOn) {
			gSys.IsRemindPowerOn = FALSE;
			SoundRemind(SOUND_PWR_ON);
		}

		// quick response
		SetQuickResponseFlag(FALSE);
#ifdef FUNC_BREAKPOINT_EN		
		if((IS_CUR_WORK_MODULE()) 
		&& (gSys.CurModuleID != MODULE_ID_BT_HF)) 
		{
			BP_SYS_INFO *pSysInfo;
			pSysInfo = (BP_SYS_INFO *)BP_GetInfo(BP_SYS_INFO_TYPE);
			if(gSys.CurModuleID == MODULE_ID_RECORDER)
			{
#ifdef FUNC_REC_PLAYBACK_EN
				BP_SET_ELEMENT(pSysInfo->CurModuleId, MODULE_ID_REC_BACK_PLAY);
#endif
			}
			else
			{
				BP_SET_ELEMENT(pSysInfo->CurModuleId, gSys.CurModuleID);
			}
			APP_DBG("pSysInfo->CurModuleId:%d\n", pSysInfo->CurModuleId);
			BP_SaveInfo(BP_SAVE2NVM);
#ifdef BP_SAVE_TO_FLASH // 掉电记忆
			BP_SaveInfo(BP_SAVE2FLASH);
#ifdef FUNC_WIFI_EN
			McuGetWiFiPlayStateParams();
#endif
#endif
		}
#endif
	}
}

#ifdef FUNC_RADIO_DIV_CLK_EN
extern int calibrate_delay(void);
extern int systick_config(void);

void SysClkSet(uint8_t clk_sel, uint8_t clk_div)
{
	uint8_t cur_clk_sel, cur_clk_div;
	unsigned int cur_clk_freq, new_clk_freq;
	
	cur_clk_sel = ClkSysClkSelGet();
	cur_clk_div = ClkModuleDivGet();

	cur_clk_freq = (cur_clk_sel ? 80 : 96) / cur_clk_div;
	new_clk_freq = (clk_sel ? 80 : 96 ) / clk_div;

	ClkSysClkSel(clk_sel);

	OSSchedulerSuspend();
	SpiFlashClktoSysClk();

	ClkModuleDivSetNotify(cur_clk_div, clk_div);
	systick_config();
	
	Timer0Adjust(cur_clk_freq, new_clk_freq);
	Timer1Adjust(cur_clk_freq, new_clk_freq);

	calibrate_delay();
	
	SpiFlashClkRestore();
	OSSchedulerResume();
}
/*
void SysClkDivison(char NewClkDiv)
{
//	extern void SWUartBuadRateAutoAdap(char PreFq, char CurFq);	
//	extern void ClkModuleDivSetNotify(unsigned int PreFq, unsigned int CurFq);

	char CurClkDiv = ClkModuleDivGet();
	char FreqSel = ClkSysClkSelGet();

	OSSchedulerSuspend();
	SpiFlashClktoSysClk();
	
	ClkModuleDivSetNotify(CurClkDiv, NewClkDiv);
	systick_config();
	Timer0Adjust(CurClkDiv, NewClkDiv);
	Timer1Adjust(CurClkDiv, NewClkDiv);
	//SWUartBuadRateAutoAdap(CurClkDiv, NewClkDiv);
	calibrate_delay();
	
	SpiFlashClkRestore();
	OSSchedulerResume();
}
*/
#endif
