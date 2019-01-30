#include "app_config.h"
#ifdef FUNC_SINGLE_LED_EN
#include "folder.h"
#include "Fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "radio_control.h"
#include "line_in_control.h"
#include "sys_app.h"
#include "recorder_control.h"
#include "idle_control.h"
#include "eq.h"
#include "eq_params.h"
#include "sys_vol.h"

#ifdef FUNC_POWER_MONITOR_EN
#include "power_management.h"
#endif
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#include "aip1629a.h"
#include "singled_display.h"


//==============================================================================
static SINGLE_LED_DISP_STRU Led;
static SINGLE_LED_CB_STRU* Led_Cb;
static SINGLE_LED_CB_STRU Led_Cb_Once;
static SINGLE_LED_CB_STRU Led_Cb_Loop;
static SINGLE_LED_CB_STRU Led_Cb_Keep;
static TIMER SingleLedScanTime;
static TIMER SingleLedChangeTime;

static void SingleLedChangeDutyFunc(void)
{
	if(Led.BledFlag&(1<<Led.DispBitCnt))
	{
		if((SINGLE_LED_MAX_DUTY-Led.BledDuty) >= Led.StepUpDuty)
		{
			Led.BledDuty += Led.StepUpDuty;
		}
		else if(SINGLE_LED_MAX_DUTY > Led.StepUpDuty)
		{
			Led.BledDuty = SINGLE_LED_MAX_DUTY;
		}
	}
	else if(!(Led.BledFlag&(1<<Led.DispBitCnt)))
	{
		if((Led.BledDuty-Led.MinDispDuty) >= Led.StepUpDuty)
		{
			Led.BledDuty -= Led.StepUpDuty;
		}
		else if(Led.BledDuty > Led.MinDispDuty)
		{
			Led.BledDuty = Led.MinDispDuty;
		}
	}
	SINGLE_BLED_SETTING(Led.BledDuty);

	if(Led.RledFlag&(1<<Led.DispBitCnt))
	{
		if((SINGLE_LED_MAX_DUTY-Led.RledDuty) >= Led.StepUpDuty)
		{
			Led.RledDuty += Led.StepUpDuty;
		}
		else if(SINGLE_LED_MAX_DUTY > Led.StepUpDuty)
		{
			Led.RledDuty = SINGLE_LED_MAX_DUTY;
		}
	}
	else if(!(Led.RledFlag&(1<<Led.DispBitCnt)))
	{
		if((Led.RledDuty-Led.MinDispDuty) >= Led.StepUpDuty)
		{
			Led.RledDuty -= Led.StepUpDuty;
		}
		else if(Led.RledDuty > Led.MinDispDuty)
		{
			Led.RledDuty = Led.MinDispDuty;
		}
	}
	SINGLE_RLED_SETTING(Led.RledDuty);
}

static void SingleLedCfg(uint8_t Bled, uint8_t Rled, uint8_t StepUp, uint32_t DispTime)
{
	memset(&Led, 0x00, sizeof(SINGLE_LED_DISP_STRU));

	Led.BledFlag = Bled;
	Led.RledFlag = Rled;
	Led.StepUpDuty = StepUp;
	Led.MinDispDuty = StepUp%SINGLE_LED_MAX_DUTY;
	Led.DispChangeTime = DispTime/SINGLE_LED_DISP_SCAN_CNT;
	Led_Cb->DisplayTimer = DispTime;
}

static void SingleLedDisplayCfg(void)
{
	static SINGLE_LED_CB_STRU Led_Cb_Back;
	
	if(Led_Cb_Once.CurDisplayMode)
	{
		Led_Cb = &Led_Cb_Once;
	}
	else if(Led_Cb_Keep.CurDisplayMode)
	{
		Led_Cb = &Led_Cb_Keep;
	
}
	else
	{
		Led_Cb = &Led_Cb_Loop;
	}

	if(memcmp(&Led_Cb_Back, Led_Cb, sizeof(SINGLE_LED_CB_STRU)) == 0)
	{
		return;
	}
	Led_Cb_Back = *Led_Cb;

	if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_TLAK_ON))
	{
		SingleLedCfg(0x00, 0x00, 100, 0);
	}
	else if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_CHARGING))
	{
		SingleLedCfg(0x00, 0x00, 100, 0);
	}
	else if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_LOWBATTER))
	{
		SingleLedCfg(0x00, 0xAA, 100, 350);
	}
	else if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_POWER_ON))
	{
		SingleLedCfg(0xDB, 0x00, 100, 800);
	}
	else if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_WPSCONNECT))
	{
		SingleLedCfg(0x00, 0xAA, 100/SINGLE_LED_DISP_SCAN_CNT, 1500);
	}
	/*else if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_ALARM_CLOCK))
	{
		SingleLedCfg(0x88, 0x22, 100/SINGLE_LED_DISP_SCAN_CNT, 1000);
		Led.MinDispDuty = SINGLE_LED_MIN_DUTY;
	}*/
	else if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_BREATHING))
	{
		SingleLedCfg(0xAA, 0x00, 100/SINGLE_LED_DISP_SCAN_CNT, 1500);
	}
	else if(Led_Cb->CurDisplayMode&(1<<LED_DISPLAY_MODE_NIGHTLAMP))
	{
		SingleLedCfg(0xFF, 0x00, 100, 0);
	}
	else
	{
		SingleLedCfg(0x00, 0x00, 100, 0);
	}
}

void SingleLedDisplayModeSet(LED_MODE_TYPE DisplayMode, bool IsOnOff, LED_DISPLAY_STATUS IsBackup)
{	
	if(gDisplayMode == DISP_DEV_SLED)
	{
		SINGLE_LED_CB_STRU* Led_P = NULL;

		if(LED_DISPLAY_KEEP == IsBackup)
		{
			Led_P = &Led_Cb_Keep;
		}
		else if(LED_DISPLAY_LOOP == IsBackup)
		{
			Led_P = &Led_Cb_Loop;
		}
		else
		{
			Led_P = &Led_Cb_Once;
		}

		if((Led_P->CurDisplayMode&(1<<DisplayMode)) == IsOnOff)
		{
			return;
		}

		if(IsOnOff)
		{
			Led_P->CurDisplayMode |= (1<<DisplayMode);
		}
		else
		{
			Led_P->CurDisplayMode &= ~(1<<DisplayMode);
		}
		Led_P->DispStatus = IsBackup;

		SingleLedDisplayCfg();       
	}
}

void LedFlushDisp(void)
{
	if(IsTimeOut(&SingleLedChangeTime))
	{
		SingleLedChangeDutyFunc();
		TimeOutSet(&SingleLedChangeTime, Led.DispChangeTime);
	}
	
	if(!IsTimeOut(&SingleLedScanTime))
	{
		return;
	}
	TimeOutSet(&SingleLedScanTime, Led_Cb->DisplayTimer);
	
	Led.DispBitCnt++;
	if(Led.DispBitCnt >= 8)
	{
		Led.DispBitCnt = 0;
		if(LED_DISPLAY_ONCE == Led_Cb->DispStatus)
		{
			memset(&Led_Cb_Once, 0, sizeof(SINGLE_LED_CB_STRU));
			SingleLedDisplayCfg();
		}
	}
}

void SingleLedFlushDispInit(void)
{
	memset(&Led_Cb, 0, sizeof(SINGLE_LED_CB_STRU));
	memset(&Led_Cb_Once, 0, sizeof(SINGLE_LED_CB_STRU));
	memset(&Led_Cb_Loop, 0, sizeof(SINGLE_LED_CB_STRU));
	memset(&Led_Cb_Keep, 0, sizeof(SINGLE_LED_CB_STRU));

	TimeOutSet(&SingleLedChangeTime, 0);
	TimeOutSet(&SingleLedScanTime, 0);
	SINGLE_BLED_INIT();
	SINGLE_RLED_INIT();
}

bool GetSingleLedDispMode(LED_MODE_TYPE Mode)
{
	return (Led_Cb->CurDisplayMode&(1<<Mode));
}
#endif

