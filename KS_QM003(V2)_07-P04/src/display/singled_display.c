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

void SingleLedDisplayModeSet(LED_MODE_TYPE DisplayMode, bool IsOnOff)
{	
	if(gDisplayMode == DISP_DEV_SLED)
	{
		if((Led.CurDisplayMode&(1<<DisplayMode)) == IsOnOff)
		{
			return;
		}

		if(IsOnOff)
		{
			Led.CurDisplayMode |= (1<<DisplayMode);
		}
		else
		{
			Led.CurDisplayMode &= ~(1<<DisplayMode);
		}

		if (Led.CurDisplayMode&(1<<LED_DISPLAY_MODE_LOWBATTER)) {
			Led.RledFlag = 0xAA;
			Led.StepUpDuty = SINGLE_LED_MAX_DUTY/SINGLE_LED_DISP_SCAN_CNT;
			Led.DispBitCnt = 0;
			Led.MinDispDuty = Led.StepUpDuty;
			Led.DispChangeTime = 1500/SINGLE_LED_DISP_SCAN_CNT;
			Led.DisplayTime = 1500;
		}
		else {
			Led.RledFlag = 0x00;
		}

		if (Led.CurDisplayMode&(1<<LED_DISPLAY_MODE_WPSCONNECT)) {
			Led.BledFlag = 0xAA;
			Led.StepUpDuty = SINGLE_LED_MAX_DUTY/SINGLE_LED_DISP_SCAN_CNT;
			Led.DispBitCnt = 0;
			Led.MinDispDuty = Led.StepUpDuty;
			Led.DispChangeTime = 1500/SINGLE_LED_DISP_SCAN_CNT;
			Led.DisplayTime = 1500;
		}
		else if (Led.CurDisplayMode&(1<<LED_DISPLAY_MODE_NIGHTLAMP)) {
			Led.BledFlag = 0xFF;
		}
		else {
			Led.BledFlag = 0x00;
		}
	}
}

void LedFlushDisp(void)
{
#ifdef FUNC_SLEEP_LEDOFF_EN
	if (gSys.SleepLedOffFlag) {
		SINGLE_RLED_SETTING(0);
		SINGLE_BLED_SETTING(0);
		return;
	}
#endif
	
	if(IsTimeOut(&SingleLedChangeTime))
	{
		SingleLedChangeDutyFunc();
		TimeOutSet(&SingleLedChangeTime, Led.DispChangeTime);
	}
	
	if(!IsTimeOut(&SingleLedScanTime))
	{
		return;
	}
	TimeOutSet(&SingleLedScanTime, Led.DisplayTime);
	
	Led.DispBitCnt++;
	if(Led.DispBitCnt >= 8)
	{
		Led.DispBitCnt = 0;
	}
}

void SingleLedFlushDispInit(void)
{
	memset(&Led, 0, sizeof(SINGLE_LED_DISP_STRU));

	TimeOutSet(&SingleLedChangeTime, 0);
	TimeOutSet(&SingleLedScanTime, 0);
	SINGLE_BLED_INIT();
	SINGLE_RLED_INIT();
}

bool GetSingleLedDispMode(LED_MODE_TYPE Mode)
{
	return (Led.CurDisplayMode&(1<<Mode));
}
#endif

