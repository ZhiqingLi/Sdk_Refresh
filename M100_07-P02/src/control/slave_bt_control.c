///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: slave_bt_control.c
//  maintainer: qing
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "dac.h"
#include "gpio.h"
#include "beep.h"
#include "echo.h"
#include "audio_adc.h"
#include "audio_path.h"
#include "clk.h"
#include "pmem.h"
#include "pcm_transfer.h"
#include "pcm_fifo.h"
#include "line_in_control.h"
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
#include "wifi_control.h" 

#ifdef	OPTION_CHARGER_DETECT
extern bool IsInCharge(void);
extern bool IsIChargeFullDet(void);
#endif

bool BluetoothSoundFlag = FALSE;
TIMER BtStatusDetTimer;
uint8_t SlaveBtStatus = 0;
uint8_t SlaveBtStatusJitterCnt = 0;
uint8_t PrevSlaveBtStatus = 0;

SW_TIMER BlueInTimer;
#define BLUE_LED_CB_TIME   200       //LED显示时间

#ifdef FUNC_SLAVE_BLUETOOTH_EN

static void SlaveBtGpioInit(void)
{
#ifdef SLAVE_BTDETE_EN
	//设为输入，带上拉
	GpioClrRegBits(SLAVE_BTDETE_PORT_OE, SLAVE_BTDETE_PORT_BIT);
	GpioClrRegBits(SLAVE_BTDETE_PORT_PU, SLAVE_BTDETE_PORT_BIT);
	GpioClrRegBits(SLAVE_BTDETE_PORT_PD, SLAVE_BTDETE_PORT_BIT);
	
	TimeOutSet(&BtStatusDetTimer, 0);
	GpioIntEn(SLAVE_BTDETE_PORT_INT, SLAVE_BTDETE_PORT_BIT, GPIO_NEG_EDGE_TRIGGER);
	//enable gpio irqc
	NVIC_EnableIRQ(GPIO_IRQn);
#endif
#ifdef SLAVE_BTMUTE_EN
	//设为输入，带上拉
	GpioClrRegBits(SLAVE_BTMUTE_PORT_OE, SLAVE_BTMUTE_PORT_BIT);
	GpioClrRegBits(SLAVE_BTMUTE_PORT_PU, SLAVE_BTMUTE_PORT_BIT);
	GpioClrRegBits(SLAVE_BTMUTE_PORT_PD, SLAVE_BTMUTE_PORT_BIT);
	GpioSetRegBits(SLAVE_BTMUTE_PORT_IE, SLAVE_BTMUTE_PORT_BIT);
#endif
#ifdef SLAVE_BTPLAY_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PU, SLAVE_BTPLAY_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PD, SLAVE_BTPLAY_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTPLAY_PORT_IE, SLAVE_BTPLAY_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPLAY_PORT_OE, SLAVE_BTPLAY_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTPLAY_PORT_OUT, SLAVE_BTPLAY_PORT_BIT);
#endif
#ifdef SLAVE_BTNEXT_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PU, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PD, SLAVE_BTNEXT_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTNEXT_PORT_IE, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_OE, SLAVE_BTNEXT_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTNEXT_PORT_OUT, SLAVE_BTNEXT_PORT_BIT);
#endif
#ifdef SLAVE_BTPREV_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_PU, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_PD, SLAVE_BTPREV_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTPREV_PORT_IE, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_OE, SLAVE_BTPREV_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTPREV_PORT_OUT, SLAVE_BTPREV_PORT_BIT);
#endif
#ifdef SLAVE_VOL_UP_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_VOL_UP_PORT_PU, SLAVE_VOL_UP_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_UP_PORT_PD, SLAVE_VOL_UP_PORT_BIT);
	GpioClrRegOneBit(SLAVE_VOL_UP_PORT_IE, SLAVE_VOL_UP_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_UP_PORT_OE, SLAVE_VOL_UP_PORT_BIT);
	GpioClrRegOneBit(SLAVE_VOL_UP_PORT_OUT, SLAVE_VOL_UP_PORT_BIT);
#endif	
#ifdef SLAVE_VOL_DW_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_VOL_DW_PORT_PU, SLAVE_VOL_DW_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_DW_PORT_PD, SLAVE_VOL_DW_PORT_BIT);
	GpioClrRegOneBit(SLAVE_VOL_DW_PORT_IE, SLAVE_VOL_DW_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_DW_PORT_OE, SLAVE_VOL_DW_PORT_BIT);
	GpioClrRegOneBit(SLAVE_VOL_DW_PORT_OUT, SLAVE_VOL_DW_PORT_BIT);
#endif
}

// Slave Bt Status detect interrupt.
__attribute__((section(".driver.isr"))) void GpioInterrupt(void)
{
	if(GpioIntFlagGet(SLAVE_BTDETE_PORT_INT) == SLAVE_BTDETE_PORT_BIT)
	{
		GpioIntClr(SLAVE_BTDETE_PORT_INT, SLAVE_BTDETE_PORT_BIT);
		
		if(SlaveBtStatus != (PastTimeGet(&BtStatusDetTimer)+25)/100)
		{
			if(PrevSlaveBtStatus != 0xFF)
			{
			  BluetoothSoundFlag = TRUE;
			}
			SlaveBtStatus = (PastTimeGet(&BtStatusDetTimer)+25)/100;
			SlaveBtStatusJitterCnt = 0;
			APP_DBG("SlaveBtStatus = %d\n", SlaveBtStatus);
		}
		else 
		{
			PrevSlaveBtStatus = SlaveBtStatus;
			if(SlaveBtStatus == 5)
			{
			  SlaveBtStatusJitterCnt++;
			  if(SlaveBtStatusJitterCnt >= 5)
			  {
			    BluetoothSoundFlag = TRUE;
				  PrevSlaveBtStatus = 0xFF;
				  SlaveBtStatusJitterCnt = 0;
			  }
		  }
		}
			
		TimeOutSet(&BtStatusDetTimer, 2000);
	}
}

static bool SlaveBtMuteDetect(void)
{
#ifdef SLAVE_BTMUTE_EN
	//设为输入，带上拉
	GpioClrRegBits(SLAVE_BTMUTE_PORT_OE, SLAVE_BTMUTE_PORT_BIT);
	GpioClrRegBits(SLAVE_BTMUTE_PORT_PU, SLAVE_BTMUTE_PORT_BIT);
	GpioClrRegBits(SLAVE_BTMUTE_PORT_PD, SLAVE_BTMUTE_PORT_BIT);

	GpioSetRegBits(SLAVE_BTMUTE_PORT_IE, SLAVE_BTMUTE_PORT_BIT);

	if(GpioGetReg(SLAVE_BTMUTE_PORT_IN) & SLAVE_BTMUTE_PORT_BIT)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#endif
}	

static void SlaveBtPlaypause(void)
{
#ifdef SLAVE_BTPLAY_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PU, SLAVE_BTPLAY_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PD, SLAVE_BTPLAY_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTPLAY_PORT_IE, SLAVE_BTPLAY_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPLAY_PORT_OE, SLAVE_BTPLAY_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPLAY_PORT_OUT, SLAVE_BTPLAY_PORT_BIT);
	WaitMs(120);
	GpioClrRegOneBit(SLAVE_BTPLAY_PORT_OUT, SLAVE_BTPLAY_PORT_BIT);
#endif
}

static void SlaveBtPrevSong(void)
{
#ifdef SLAVE_BTPREV_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_PU, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_PD, SLAVE_BTPREV_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTPREV_PORT_IE, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_OE, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_OUT, SLAVE_BTPREV_PORT_BIT);
	WaitMs(120);
	GpioClrRegOneBit(SLAVE_BTPREV_PORT_OUT, SLAVE_BTPREV_PORT_BIT);
#endif
}

static void SlaveBtNextSong(void)
{
#ifdef SLAVE_BTNEXT_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PU, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PD, SLAVE_BTNEXT_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTNEXT_PORT_IE, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_OE, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_OUT, SLAVE_BTNEXT_PORT_BIT);
	WaitMs(120);
	GpioClrRegOneBit(SLAVE_BTNEXT_PORT_OUT, SLAVE_BTNEXT_PORT_BIT);
#endif
}

void SlaveBtVolUp(void)
{
#ifdef SLAVE_VOL_UP_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_VOL_UP_PORT_PU, SLAVE_VOL_UP_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_UP_PORT_PD, SLAVE_VOL_UP_PORT_BIT);
	GpioClrRegOneBit(SLAVE_VOL_UP_PORT_IE, SLAVE_VOL_UP_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_UP_PORT_OE, SLAVE_VOL_UP_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_UP_PORT_OUT, SLAVE_VOL_UP_PORT_BIT);
	WaitMs(120);
	GpioClrRegOneBit(SLAVE_VOL_UP_PORT_OUT, SLAVE_VOL_UP_PORT_BIT);
#endif	
}

void SlaveBtVolDw(void)
{
#ifdef SLAVE_VOL_DW_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_VOL_DW_PORT_PU, SLAVE_VOL_DW_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_DW_PORT_PD, SLAVE_VOL_DW_PORT_BIT);
	GpioClrRegOneBit(SLAVE_VOL_DW_PORT_IE, SLAVE_VOL_DW_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_DW_PORT_OE, SLAVE_VOL_DW_PORT_BIT);
	GpioSetRegOneBit(SLAVE_VOL_DW_PORT_OUT, SLAVE_VOL_DW_PORT_BIT);
	WaitMs(120);
	GpioClrRegOneBit(SLAVE_VOL_DW_PORT_OUT, SLAVE_VOL_DW_PORT_BIT);
#endif	
}

void SlaveBtClearPaired(void)
{
#ifdef SLAVE_BTNEXT_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PU, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PD, SLAVE_BTNEXT_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTNEXT_PORT_IE, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_OE, SLAVE_BTNEXT_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTNEXT_PORT_OUT, SLAVE_BTNEXT_PORT_BIT);
#endif
#ifdef SLAVE_BTPREV_EN
	//设置为输出，带下拉；
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_PU, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_PD, SLAVE_BTPREV_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTPREV_PORT_IE, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_OE, SLAVE_BTPREV_PORT_BIT);
	GpioSetRegOneBit(SLAVE_BTPREV_PORT_OUT, SLAVE_BTPREV_PORT_BIT);
#endif	
	 
	WaitMs(120);
	GpioClrRegOneBit(SLAVE_BTPREV_PORT_OUT, SLAVE_BTPREV_PORT_BIT);
	GpioClrRegOneBit(SLAVE_BTNEXT_PORT_OUT, SLAVE_BTNEXT_PORT_BIT);
}

void SlaveBtRedialAndReject(void)
{
	if(SlaveBtStatus == 5)
	{
#ifdef SLAVE_BTPLAY_EN
	  //设置为输出，带下拉；
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PU, SLAVE_BTPLAY_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PD, SLAVE_BTPLAY_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_BTPLAY_PORT_IE, SLAVE_BTPLAY_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_OE, SLAVE_BTPLAY_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_OUT, SLAVE_BTPLAY_PORT_BIT);
#endif
#ifdef SLAVE_BTNEXT_EN
	  //设置为输出，带下拉；
	  GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PU, SLAVE_BTNEXT_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTNEXT_PORT_PD, SLAVE_BTNEXT_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_BTNEXT_PORT_IE, SLAVE_BTNEXT_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTNEXT_PORT_OE, SLAVE_BTNEXT_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTNEXT_PORT_OUT, SLAVE_BTNEXT_PORT_BIT);
#endif
#ifdef SLAVE_BTPREV_EN
	  //设置为输出，带下拉；
	  GpioSetRegOneBit(SLAVE_BTPREV_PORT_PU, SLAVE_BTPREV_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPREV_PORT_PD, SLAVE_BTPREV_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_BTPREV_PORT_IE, SLAVE_BTPREV_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPREV_PORT_OE, SLAVE_BTPREV_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPREV_PORT_OUT, SLAVE_BTPREV_PORT_BIT);
#endif	
	 
	  WaitMs(120);
		GpioClrRegOneBit(SLAVE_BTPLAY_PORT_OUT, SLAVE_BTPLAY_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_BTPREV_PORT_OUT, SLAVE_BTPREV_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_BTNEXT_PORT_OUT, SLAVE_BTNEXT_PORT_BIT);
	}
	else
	{
#ifdef SLAVE_BTPLAY_EN
	  //设置为输出，带下拉；
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PU, SLAVE_BTPLAY_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_PD, SLAVE_BTPLAY_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_BTPLAY_PORT_IE, SLAVE_BTPLAY_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_OE, SLAVE_BTPLAY_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_BTPLAY_PORT_OUT, SLAVE_BTPLAY_PORT_BIT);
#endif
#ifdef SLAVE_VOL_UP_EN
	  //设置为输出，带下拉；
	  GpioSetRegOneBit(SLAVE_VOL_UP_PORT_PU, SLAVE_VOL_UP_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_VOL_UP_PORT_PD, SLAVE_VOL_UP_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_VOL_UP_PORT_IE, SLAVE_VOL_UP_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_VOL_UP_PORT_OE, SLAVE_VOL_UP_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_VOL_UP_PORT_OUT, SLAVE_VOL_UP_PORT_BIT);
#endif	
#ifdef SLAVE_VOL_DW_EN
	  //设置为输出，带下拉；
	  GpioSetRegOneBit(SLAVE_VOL_DW_PORT_PU, SLAVE_VOL_DW_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_VOL_DW_PORT_PD, SLAVE_VOL_DW_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_VOL_DW_PORT_IE, SLAVE_VOL_DW_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_VOL_DW_PORT_OE, SLAVE_VOL_DW_PORT_BIT);
	  GpioSetRegOneBit(SLAVE_VOL_DW_PORT_OUT, SLAVE_VOL_DW_PORT_BIT);
#endif

    WaitMs(120);
		GpioClrRegOneBit(SLAVE_BTPLAY_PORT_OUT, SLAVE_BTPLAY_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_VOL_UP_PORT_OUT, SLAVE_VOL_UP_PORT_BIT);
	  GpioClrRegOneBit(SLAVE_VOL_DW_PORT_OUT, SLAVE_VOL_DW_PORT_BIT);
	}
}

//蓝牙模式下提示音播放
void SlaveBluetoothSoundRemind(void)
{
	APP_DBG("PrevSlaveBtStatus = %d\n", PrevSlaveBtStatus);
	if((SlaveBtStatus == 2) && (PrevSlaveBtStatus != SlaveBtStatus))
	{
		SoundRemind(SOUND_WAIT_PAIR);
	}
	else if((SlaveBtStatus == 3) && (PrevSlaveBtStatus < 3))
	{
		SoundRemind(SOUND_BT_CONNECT);
	}
	else if(SlaveBtStatus == 5)
	{
		if(PrevSlaveBtStatus == 0XFF)
		{
		  SoundRemind(SOUND_CALL_INCOM);
		}
	}
}

//蓝牙模式下的LED显示处理(200ms调用一次，此时间间隔WIFI_LED_CB_TIME 可以用户自己定义)
void BlueLedCb(void* Param)
{
	static uint8_t BlueLedDispCnt;
	
	BlueLedDispCnt++;
	if(gSys.SleepStartPowerOff == TRUE)
	{
		gSys.NextModuleID = MODULE_ID_IDLE;
	  MsgSend(MSG_COMMON_CLOSE);
	}
	
	if(SlaveBtStatus < 3)
	{
		ChangeTimerPeriod(&BlueInTimer, 200);
		StartTimer(&BlueInTimer); 
		LED_ALL_MODE_OFF();
		if(BlueLedDispCnt >= 2 )
		{
			LED_BLUE_MODE_ON();
			BlueLedDispCnt = 0;
		}
	}
	else if(SlaveBtStatus == 7)
	{
		ChangeTimerPeriod(&BlueInTimer, 800);
		StartTimer(&BlueInTimer);
    LED_ALL_MODE_OFF();		
		if(BlueLedDispCnt >= 2 )
		{
			LED_BLUE_MODE_ON();
			BlueLedDispCnt = 0;
		}
	}
	else 
	{	
		LED_GREEN_MODE_OFF();
		LED_RED_MODE_OFF();
		LED_BLUE_MODE_ON();
	}
}

//蓝牙从机控制
bool SlaveBluetoothControl(void)
{
	uint16_t Msg = 0;

	APP_DBG("Enter SlaveBluetoothControl...\n");

	SlaveBtGpioInit();
	SlaveBtPowerOn();
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif	
	
	InitTimer(&BlueInTimer, BLUE_LED_CB_TIME, BlueLedCb);
	StartTimer(&BlueInTimer); 
	
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
	
	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC_FM : AUDIO_CH_FM);
	
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

	SoundRemind(SOUND_BT_MODE);
	AudioSysInfoSetBreakPoint();

	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	
	while(Msg != MSG_COMMON_CLOSE)
	{	
		CheckTimer(&BlueInTimer);
		
		if(BluetoothSoundFlag)
		{
			BluetoothSoundFlag = FALSE;
			SlaveBluetoothSoundRemind();
		}
		
		if(SlaveBtStatus == 3)
		{	
			gSys.MuteFlag = TRUE;
		}
		else
		{
			gSys.MuteFlag = SlaveBtMuteDetect();
		}
		
		Msg = MsgRecv(5);

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
				SlaveBtPlaypause();
			  if(SlaveBtStatus == 5)
	      {
					BluetoothSoundFlag = FALSE;
			    SlaveBtStatusJitterCnt = 0;
				}
				break;
			
			case MSG_WIFI_TALK:
				if(SlaveBtStatus == 5)
	      {	
					BluetoothSoundFlag = FALSE;
			    SlaveBtPlaypause();
			    SlaveBtStatusJitterCnt = 0;
		    }
				break;
			
      case MSG_NEXT:
				SlaveBtNextSong();
				break;
			
			case MSG_PRE:
				SlaveBtPrevSong();
				break;
			
			case MSG_STOP:
				SlaveBtRedialAndReject();
				break;
			
			case MSG_WIFI_WPS:
				SlaveBtClearPaired();
				break;
			
			default:
				if(Msg == MSG_VOL_UP)
				{
					SlaveBtVolUp();
				}
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
	
  LED_ALL_MODE_OFF();
	PrevSlaveBtStatus = 0;
	
	SlaveBtPowerOff();
	AudioAnaSetChannel(gSys.MicEnable ? AUDIO_CH_MIC : AUDIO_CH_NONE);	
	APP_DBG("leave SlaveBluetoothControl...\n");
	return TRUE;//OK
}
#endif

