///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: key.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"

#if (defined(FUNC_ADC_KEY_EN) || defined(FUNC_IR_KEY_EN) || defined(FUNC_CODING_KEY_EN) || defined(FUNC_GPIO_KEY_EN)) || defined(FUNC_IIC_KEY_EN)
#ifdef FUNC_ADC_KEY_EN
extern void AdcKeyInit(void);
extern uint16_t AdcKeyScan(void);
#endif

#ifdef FUNC_IR_KEY_EN
extern void IrKeyInit(void);
extern uint16_t IrKeyScan(void);
#endif

#ifdef FUNC_CODING_KEY_EN
extern void CodingKeyInit(void);
extern uint16_t CodingKeyScan(void);
#endif
	
#ifdef FUNC_IIC_KEY_EN 
extern void I2cKeyInit(void);	
extern uint16_t I2cKeyScan(void);
#endif
	
#ifdef FUNC_GPIO_KEY_EN 
extern void GpioKeyScanInit(void);
extern uint16_t GpioKeyScan(void);
#endif

#ifdef FUNC_AUDIO_MENU_EN
static TIMER MenuTimer;
#endif
static TIMER KeyScanTimer;

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
	
#ifdef FUNC_IIC_KEY_EN 
	I2cKeyInit();
#endif
	
#ifdef FUNC_GPIO_KEY_EN 
	GpioKeyScanInit();
#endif	
	
#ifdef FUNC_AUDIO_MENU_EN
	TimeOutSet(&MenuTimer, 0);	
#endif

	TimeOutSet(&KeyScanTimer, 0);
}

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

uint16_t KeyScan(void)
{
	uint16_t Msg = MSG_NONE;

	if(!IsTimeOut(&KeyScanTimer))
	{
		return MSG_NONE;
	}
	TimeOutSet(&KeyScanTimer, 10);
	
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
		
	if((Msg != MSG_NONE) && (Msg != MSG_PWR_KEY_SP))
	{
	
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
		if((Msg == MSG_VOL_UP) || (Msg == MSG_VOL_DW))
		{
			if(GpioKeyScan() == MSG_PWR_KEY_SP)
			{
				Msg = (Msg == MSG_VOL_UP)? MSG_DUTY_UP : MSG_DUTY_DW; 
			}
		}				
#endif
		
		APP_DBG("Key Msg : %x\n", Msg);
		return Msg;
	}
	return MSG_NONE;
}
#endif
