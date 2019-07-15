//#include "app/app_globals.h"
#include "app_config.h"
#ifdef FUNC_DISP_EN
#include "folder.h"
#include "Fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "radio_control.h"
#include "line_in_control.h"
#include "sys_app.h"
#include "recorder_control.h"
#ifdef FUNC_RTC_EN
#include "rtc.h"
#include "rtc_control.h"
#endif
#include "idle_control.h"

#include "eq.h"
#include "eq_params.h"
#include "sys_vol.h"
#ifdef FUNC_MIC_ECHO_EN
extern uint8_t		EchoDelayStep;
extern uint8_t		EchoDepthStep;
#endif

#ifdef FUNC_POWER_MONITOR_EN
#include "power_management.h"
#endif
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#include "aip1629a.h"
#include "singled_display.h"
#ifdef FUNC_SEG_LCD_EN
#include "seg_lcd_disp.h"
#include "lcd_seg.h" 
extern TIMER	gAmplitudeTmr;
#endif

void ClearSceen(void);
#ifdef FUNC_SPEC_TRUM_EN
void DispSpecTrum(void);
#endif
void DispNo(void);
#ifdef FUNC_SOUND_REMIND
bool IsSoundRemindPlaying(void);
#endif
#ifdef FUNC_RTC_EN
void DispRtcIcon(void);
#endif
//==============================================================================
// Global variants.
#if defined(FUNC_AIP1629A_LED_EN)
DISP_TYPE	gDispBuff[MAX_COM_NUM * 2] = {0};
uint8_t     gDispFaceIndex;
#elif defined(FUNC_9PIN_SEG_LED_EN)
DISP_TYPE	gDispBuff[MAX_LED_PIN_NUM] = {0};
uint8_t     gDispFaceIndex;
#else
DISP_TYPE	gDispBuff[MAX_COM_NUM] = {0};
#endif
DISP_DEV	gDisplayMode = DISP_DEV_NONE;
uint8_t 	gDispPrevRep;
uint8_t 	gDispPrevRepAB;
uint8_t 	gDispPrevEQ;
uint8_t 	gDispCurVol;
uint8_t 	gDispCurMicVol;
#ifdef FUNC_GUITAR_EN
uint8_t 	gDispCurGuitarVol;
#endif
uint8_t 	gDispPrevMicVol;
#ifdef FUNC_GUITAR_EN
uint8_t 	gDispPrevGuitarVol;
#endif
uint8_t 	gDispPrevEchoDelStep;
uint8_t 	gDispPrevEchoDepthStep;
#ifdef FUNC_TREB_BASS_EN
uint8_t 	gDispCurBass;
uint8_t 	gDispPrevBass;
uint8_t 	gDispCurTreb;
uint8_t 	gDispPrevTreb;
#endif
uint8_t 	gDispPrevPlayState;
bool        gLedDispRefreshFlag;

TIMER		DispTmr;
TIMER		DispMsgTmr;
#ifdef FUNC_SPEC_TRUM_EN
TIMER		DispSpecTrumTmr;
TIMER		DispSpecTrumDnTmr;
#endif
#ifdef FUNC_PROGRAM_EN
TIMER		DispProgTmr;
bool        DispProgFlashFlag;
#endif

UN_BLINK gBlink;

//uint16_t DispHoldTime;
uint16_t Temp;
uint8_t     gDispMsgId;

#ifdef FUNC_POWER_ON_AUDIO_EN
extern bool gUSBChargePowerOnFlag;
#endif

#ifdef FUNC_NUMBER_KEY_EN
TIMER NumKeyErrDispTimer;
extern TIMER RecvNumTimer;
extern bool NumKeyErrorDispFlag;
extern bool NumKeyRadioFreqErrorFlag;
#endif


//DISP_STATE gDispState;//= DISP_STATE_DEV;

//DISP_INFO * gDisp= NULL;

void DispSendMessage(uint16_t DispMsgTime, uint8_t DispMsgId)
{
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if(DISP_MSG_MUTE == DispMsgId)
		{
			TimeOutSet(&DispMsgTmr, 0);
			DispMute();
		}
		else
		{
			TimeOutSet(&DispMsgTmr, DispMsgTime);
		}
		gDispMsgId = DispMsgId;
	}
}
//----------------------------------------------------------

// Clear digit-value(清数值显示位).
void ClearDigit(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) ||defined(FUNC_AIP1629A_LED_EN))
	DispString("    ");	  //灭
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
#endif
}

void ClearAllIcon(void)
{
	DispIcon(ICON_PLAY, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
	DispIcon(ICON_USB, LIGHT_OFF);
	DispIcon(ICON_SD, LIGHT_OFF);
	//DispIcon(ICON_FM, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
}

// Clear display(显示全灭).
void ClearScreen(void)
{
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		memset(gDispBuff, 0, sizeof(gDispBuff));
		LcdFlushDispData();
	}
#endif

#ifdef FUNC_SEG_LED_EN
	if((gDisplayMode < DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		memset(gDispBuff, 0, sizeof(gDispBuff));

#if (defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN))
		LedPinGpioInit();

#endif
	}
#endif
#ifdef FUNC_AIP1629A_LED_EN
	AIP1629ADisplayClr();
#endif
}

#ifdef FUNC_SEG_LCD_EN

uint8_t lcd_buf[(MAX_COM_NUM+MAX_SEG_NUM)*MAX_COM_NUM];

#define  LCDSEG_DEF       MASK_LCD_SEG  
#define  LCDCOM_DEF       MASK_LCD_COM
#ifdef FUNC_MS_RA1_PROJ_EN
#define  LCDBIAS_DEF      0    
#else
#define  LCDBIAS_DEF      1    
#endif

ST_LCD_SEG_CTRL *lcdinitdata = NULL;

void LCD_Drv_Init(void)
{
    uint8_t Voltage = 7;//LCD top voltage selection,thi pad is VDD33
    
	APP_DBG(" LCD_Drv_Init\r\n");
	lcdinitdata = APP_MMM_MALLOC(sizeof(ST_LCD_SEG_CTRL), MMM_ALLOC_NORMAL);
	lcdinitdata->LcdComMask = LCDCOM_DEF;
	lcdinitdata->LcdSegMask = LCDSEG_DEF;
	lcdinitdata->LcdBias = LCDBIAS_DEF;
	lcdinitdata->LcdDispData = lcd_buf;
	LcdsegInit(lcdinitdata,Voltage);    //
}
#endif 

// Display initilization
// Indicate whether the system in "STAND-BY" state or not.
void DispInit(bool IsStandBy)
{
	//DBG((">>DispInit(%bd)\n", IsStandBy));
#ifdef FUNC_PROGRAM_EN
	TimeOutSet(&DispProgTmr, 0);
#endif
	TimeOutSet(&DispTmr, 0);
#ifdef FUNC_SPEC_TRUM_EN
	TimeOutSet(&DispSpecTrumTmr, 0);
	TimeOutSet(&DispSpecTrumDnTmr, 0);
#endif
#ifdef FUNC_NUMBER_KEY_EN
	TimeOutSet(&NumKeyErrDispTimer, 0);
#endif
	gDispPrevRep = gPlayContrl->CurPlayMode;
	gDispPrevEQ = gSys.Eq;
	gDispPrevPlayState = gPlayContrl->State;
	gDispCurVol = gSys.Volume;
	memset(&gDispBuff, 0, sizeof(gDispBuff));

#ifdef FUNC_SEG_LED_EN
	LedPinGpioInit();
	gLedDispRefreshFlag = TRUE;
#endif
#ifdef FUNC_AIP1629A_LED_EN
	AIP1629ADisplayInit();
#endif

#ifdef FUNC_SEG_LCD_EN
	//SetGpioRegBit(GPIO_TYPE_REG, MSK_ADC_A1_SWITCH);		//ADC Vol  preq
//	LcdInit(MASK_LCD_SEG, MASK_LCD_COM, LCD_BIAS_2);
//	LcdInit(MASK_LCD_SEG, MASK_LCD_COM, LCD_BIAS_3);
//	LcdInit(MASK_LCD_SEG, MASK_LCD_COM, LCD_BIAS_4);
	LCD_Drv_Init();
//	DBG(("LcdInit(LCD初始化%x,%x,)\n", MASK_LCD_SEG, MASK_LCD_COM));
#endif

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))
	if(IsStandBy == TRUE)
	{
		ClearScreen();
	}
	else
	{
		if(gDisplayMode == DISP_DEV_CUSTOM)
		{
			//memcpy((void*)&gDispBuff, (void*)&LedCustomFace[0], MAX_COM_NUM * 2);
		}
		else
		{	
			ClearScreen();
			DispSendMessage(POWERON_INTERVL, DISP_MSG_PWR_ON);
			Display();
		}
	}
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		TimeOutSet(&gAmplitudeTmr, 0);
		LcdFlushDispData();
		LcdFlushDisp();
	}	
#endif
#endif
#ifdef FUNC_AIP1629A_LED_EN
	if(IsStandBy == TRUE)
	{
		//DBG(("CLS\n"));
		ClearScreen();
	}
	else
	{
		ClearScreen();
		DispSendMessage(POWERON_INTERVL, DISP_MSG_PWR_ON);
	}
#endif
#ifdef FUNC_SINGLE_LED_EN
	SingleLedFlushDispInit();
#endif
}


// Display de-initialization.
// Indicate whether the system in "STAND-BY" state or not.
void DispDeInit(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))
	if((gDisplayMode > 0) && (gDisplayMode != DISP_DEV_SLED))
	{
		ClearScreen();
	}
#endif

#if defined(FUNC_SEG_LED_EN) ||defined(FUNC_AIP1629A_LED_EN)
	gLedDispRefreshFlag = FALSE;	//解决进入Power down 过程中还会有段点亮问题

	if((gDisplayMode == DISP_DEV_LED1888) || (gDisplayMode == DISP_DEV_LED8888) || (gDisplayMode == DISP_DEV_LED57))
	{
#ifdef FUNC_AIP1629A_LED_EN
		AIP1629ADisplayClr();
#else
		LedPinGpioDeInit();
#endif
	}
#endif
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		//SetGpioRegBit(GPIO_TYPE_REG, MSK_ADC_A1_SWITCH);		//ADC Vol  preq
		LcdsegDeinit();
	}
#endif

	gDispPrevRep = gPlayContrl->CurPlayMode;
	gDispPrevEQ = gSys.Eq;
	gDispPrevPlayState = gPlayContrl->State;
}


// 设置Repeat模式时调用.
void DispRepeat(bool IsForced)
{
	if(gPlayContrl == NULL)
	{
		return;
	}
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if(((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD) 
		#ifdef FUNC_REC_PLAYBACK_EN
		|| (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		#endif
		)
	        && ((IsForced == TRUE) || (gDispPrevRep != gPlayContrl->CurPlayMode)))
	{
#if defined(FUNC_SEG_LED_EN) || defined(FUNC_AIP1629A_LED_EN)
		if((gDisplayMode < DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
		{
			LedDispRepeat();
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
		}
#endif

#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdDispRepeat();
			LcdFlushDispData();
		}
#endif

		gDispPrevRep = gPlayContrl->CurPlayMode;
//		gstDispPlayArg.bRep = gPlayCtrl.RepeatMode & 0x03;
	}
#endif

}
// 设置Repeat模式时调用.
void DispRepeatAB(bool IsForced)
{
	if(gPlayContrl == NULL)
	{
		return;
	}
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if(((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD 
		#ifdef FUNC_REC_PLAYBACK_EN
		|| (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		#endif
		))
	        && ((IsForced == TRUE) || (gDispPrevRepAB != gPlayContrl->RepeatAB.RepeatFlag)))
	{
#if defined(FUNC_SEG_LED_EN) || defined(FUNC_AIP1629A_LED_EN)
		if((gDisplayMode < DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
		{
			LedDispRepeatAB();
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
		}
#endif
		gDispPrevRepAB = gPlayContrl->RepeatAB.RepeatFlag;
	}
#endif
}
// 设置EQ时调用.
void DispEQ(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	//if((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD))
	{
		uint8_t  EqName[EQ_STYLE_SUM][6] =
		{
			" FLAT", " CLA", " POP", " ROC", " JAZZ"
		};

		if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
		{
			if((IsForced == TRUE) || (gDispPrevEQ != gSys.Eq))
			{
				DispString(&EqName[gSys.Eq][0]);
			}
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				LcdFlushDispData();
			}
#endif
		}
	}
	gDispPrevEQ = gSys.Eq;
#endif

}

// 设置EQ时调用.
void DispDecoderMix(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispString("AU");
//		DispChar((InputChannel / 10), POS_COM_TENS);
//		DispChar((InputChannel % 10), POS_COM_UNITS);
		DispIcon(ICON_USB, LIGHT_OFF);
		DispIcon(ICON_SD, LIGHT_OFF);
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_PLAY, LIGHT_OFF);
		DispIcon(ICON_PAUSE, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		TimeOutSet(&DispTmr, INTERIM_INTERVAL);
	}
#endif

}
// 播放状态变化时调用(Play/Pause/Stop/FF/FB).
void DispPlayState(void)
{
	if(gPlayContrl == NULL)
	{
		return;
	}
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdDispPlayState();
		LcdFlushDispData();
	}
#endif

#if defined(FUNC_SEG_LED_EN)|| defined(FUNC_AIP1629A_LED_EN)
	if((gDisplayMode <= DISP_DEV_LED57) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		LedDispPlayState();
		if(gDisplayMode != DISP_DEV_LED57)
		{
			TimeOutSet(&DispTmr, NORMAL_INTERVAL);
		}
	}
#endif

	gDispPrevPlayState = gPlayContrl->State;
}

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
// 音量设置时调用.
void DispVolume(bool IsForced)
{
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gSys.Volume !=  gDispCurVol))
		{
			gDispCurVol = gSys.Volume;
			DispChar(' ', POS_COM_THOUSAND);
			DispChar('U', POS_COM_HUNDREDS);
			DispChar((gDispCurVol / 10), POS_COM_TENS);
			DispChar((gDispCurVol % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}
}

// MIC音量设置时调用.
void DispMicVolume(bool IsForced)
{
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispPrevMicVol !=  gSys.MicVolume))
		{
			//DBG(("DispVolume,1\n"));
			DispChar(' ', POS_COM_THOUSAND);
			DispChar('C', POS_COM_HUNDREDS);
			DispChar((gSys.MicVolume / 10), POS_COM_TENS);
			DispChar((gSys.MicVolume % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}

	gDispPrevMicVol = gSys.MicVolume;
}

#ifdef FUNC_BT_HF_EN
// 蓝牙通话音量设置时调用.
void DispBTHfVolume(bool IsForced)
{
	static uint8_t gDispHfVolume;
	
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispHfVolume !=  gSys.HfVolume))
		{
			gDispHfVolume = gSys.HfVolume;
			DispChar(' ', POS_COM_THOUSAND);
			DispChar('U', POS_COM_HUNDREDS);
			DispChar((gDispHfVolume / 10), POS_COM_TENS);
			DispChar((gDispHfVolume % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}

	gDispPrevMicVol = gSys.MicVolume;
}
#endif
#endif

#ifdef FUNC_GUITAR_EN
// GUITAR音量设置时调用.
void DispGuitarVolume(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispPrevGuitarVol != gSys.GuitarVolume))
		{
			//DBG(("DispVolume,1\n"));
			DispChar(' ', POS_COM_THOUSAND);
			DispChar('G', POS_COM_HUNDREDS);
			DispChar((gSys.GuitarVolume / 10), POS_COM_TENS);
			DispChar((gSys.GuitarVolume % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
			//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}

	gDispPrevGuitarVol = gSys.GuitarVolume;
#endif

	IsForced = 0;
}
#endif
#ifdef FUNC_MIC_ECHO_EN
// echo delay 调节时调用.
void DispEchoDelayStep(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispPrevEchoDelStep != EchoDelayStep))
		{
			//DBG(("DispVolume,1\n"));
			DispChar('d', POS_COM_THOUSAND);
			DispChar('l', POS_COM_HUNDREDS);
			DispChar((EchoDelayStep / 10), POS_COM_TENS);
			DispChar((EchoDelayStep % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}
	gDispPrevEchoDelStep = EchoDelayStep;
#endif
	IsForced = 0;
}

// echo depth 调节时调用.
void DispEchoDepthStep(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispPrevEchoDepthStep !=  EchoDepthStep))
		{
			//DBG(("DispVolume,1\n"));
			DispChar('d', POS_COM_THOUSAND);
			DispChar('p', POS_COM_HUNDREDS);
			DispChar((EchoDepthStep / 10), POS_COM_TENS);
			DispChar((EchoDepthStep % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);

#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}

	gDispPrevEchoDepthStep = EchoDepthStep;
#endif
	IsForced = 0;
}
#endif

#ifdef FUNC_TREB_BASS_EN
// Treb设置时调用.
void DispTreb(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispPrevTreb != gDispCurTreb))
		{
			//DBG(("DispVolume,1\n"));
			gDispCurTreb = gSys.TrebVal;
			DispChar('T', POS_COM_THOUSAND);
			#ifdef DEBUG_TREB_BASS_GAIN
			if(gDispCurTreb >= 6)
			{
				if(gDispCurTreb == 6)
				{
					gDispCurTreb = 0;
				}
				else
				{
					gDispCurTreb = gDispCurTreb - 6;
				}
				DispChar(' ', POS_COM_HUNDREDS);
			}
			else
			{
				gDispCurTreb = 6 - gDispCurTreb;
				DispChar('-', POS_COM_HUNDREDS);

			}
			#else
			if(gDispCurTreb >= 5)
			{
				if(gDispCurTreb == 5)
				{
					gDispCurTreb = 0;
				}
				else
				{
					gDispCurTreb = gDispCurTreb - 5;
				}
				DispChar(' ', POS_COM_HUNDREDS);
			}
			else
			{
				gDispCurTreb = 5 - gDispCurTreb;
				DispChar('-', POS_COM_HUNDREDS);

			}
            #endif
			DispChar((gDispCurTreb / 10), POS_COM_TENS);
			DispChar((gDispCurTreb % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}

	gDispPrevTreb = gDispCurTreb;
#endif

	IsForced = 0;
}

// Bass设置时调用.
void DispBass(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispPrevBass != gDispCurBass))
		{
			//DBG(("DispVolume,1\n"));
			gDispCurBass = gSys.BassVal;
			DispChar('B', POS_COM_THOUSAND);
			#ifdef DEBUG_TREB_BASS_GAIN
			if(gDispCurBass >= 6)
			{
				if(gDispCurBass == 6)
				{
					gDispCurBass = 0;
				}
				else
				{
					gDispCurBass = gDispCurBass - 6;
				}
				DispChar(' ', POS_COM_HUNDREDS);
			}
			else
			{
				gDispCurBass = 6 - gDispCurBass;
				DispChar('-', POS_COM_HUNDREDS);

			}
			#else
			if(gDispCurBass >= 5)
			{
				if(gDispCurBass == 5)
				{
					gDispCurBass = 0;
				}
				else
				{
					gDispCurBass = gDispCurBass - 5;
				}
				DispChar(' ', POS_COM_HUNDREDS);
			}
			else
			{
				gDispCurBass = 5 - gDispCurBass;
				DispChar('-', POS_COM_HUNDREDS);

			}
            #endif
			DispChar((gDispCurBass / 10), POS_COM_TENS);
			DispChar((gDispCurBass % 10), POS_COM_UNITS);
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				DBG(("DispVolume,LcdFlushDispData\n"));
				LcdFlushDispData();
			}
#endif
		}
	}

	gDispPrevBass = gDispCurBass;
#endif
	IsForced = 0;
}
#endif

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
// 显示数值.
void DispNum(uint16_t Num)
{
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
#ifdef	FUNC_NUMBER_KEY_EN
		if(((NumKeyRadioFreqErrorFlag == TRUE) && (gSys.SystemMode == SYS_MODE_RADIO))
		        || ((Num > gFsInfo.FileSum) && ((gSys.SystemMode == SYS_MODE_USB) || (gSys.SystemMode == SYS_MODE_SD))))
		{
			if(NumKeyErrorDispFlag == FALSE)
			{
				Num2Char(Num, FALSE);
				NumKeyErrorDispFlag = TRUE;
				TimeOutSet(&RecvNumTimer, NORMAL_INTERVAL);
				TimeOutSet(&DispTmr, NORMAL_INTERVAL);
				TimeOutSet(&NumKeyErrDispTimer, 500);  //数字显示500ms
			}
			else
			{
				if(IsTimeOut(&NumKeyErrDispTimer))
				{
					DispString(" Err");
				}
			}
		}
		else
#endif
		{
			Num2Char(Num, FALSE);
		}

#ifdef	FUNC_NUMBER_KEY_EN
		if((NumKeyErrorDispFlag == FALSE) && (NumKeyRadioFreqErrorFlag == FALSE))
#endif
		{
			TimeOutSet(&DispTmr, SET_INTERVL);
		}
#ifdef	FUNC_NUMBER_KEY_EN
		else
		{
			if(IsTimeOut(&DispTmr))
			{
				NumKeyRadioFreqErrorFlag = FALSE;
				NumKeyErrorDispFlag = FALSE;
			}
		}
#endif
		DispIcon(ICON_COLON, LIGHT_OFF);
	}
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
}
#endif

// 文件切换时调用.
void DispFileNum(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))

#if 0//def FUNC_RTC_EN
	if(RtcAutOutTimeCount > 0)
	{
		return;
	}
#endif
    if(gPlayContrl == NULL || gPlayContrl->FileOpenSuccessFlag == NULL)
	{
	   	return;
	}
	//DBG((">>DispFileNum\n"));
	//if(gFsInfo.FileSum > 0)	//if(gFsInitOK == 1)
	{
		//DBG(("gPlayCtrl.FileNum:%u\n", gPlayCtrl.FileNum));
#if 1		
		Num2Char(gPlayContrl->PlayerFile.FileNumInDisk, FALSE);		
		if((gPlayContrl->PlayerFile.FileNumInDisk > 9999) && (gDisplayMode > DISP_DEV_LED1888))
#else
		Num2Char(gPlayContrl->CurFileIndex, FALSE);		
		if((gPlayContrl->CurFileIndex > 9999) && (gDisplayMode > DISP_DEV_LED1888))
#endif
		{
			DispChar('F', 3);
		}
	}
	//else
	//{
	//	DBG(("NOF\n"));
	//	DispString(" NOF");
	//}
	TimeOutSet(&DispTmr, DEV_SYMBOL_INTERVAL);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
#ifdef FUNC_ZHIFENG_SOUNDBAR_DEMO_V1_0_EN
	DispIcon(ICON_DP, LIGHT_OFF);
#endif
	#ifdef FUNC_AIP1629A_LED_EN
	DispIcon(ICON_FM, LIGHT_OFF);
	DispIcon(ICON_DP, LIGHT_OFF);
	DispIcon(ICON_MHZ, LIGHT_OFF);
	#endif
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
#endif

}

// 数字选曲时调用.
void DispSelNum(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))

	if(gPlayContrl == NULL)
	{
		return;
	}
	Num2Char(gPlayContrl->InputNumber, FALSE);
	TimeOutSet(&DispTmr, DEV_SYMBOL_INTERVAL);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
#ifdef FUNC_ZHIFENG_SOUNDBAR_DEMO_V1_0_EN
	DispIcon(ICON_DP, LIGHT_OFF);
#endif
#ifdef FUNC_AIP1629A_LED_EN
	DispIcon(ICON_FM, LIGHT_OFF);
	DispIcon(ICON_DP, LIGHT_OFF);
	DispIcon(ICON_MHZ, LIGHT_OFF);
#endif
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
#endif

}

#if (defined(FUNC_SEG_LCD_EN) || defined(FUNC_SEG_LED_EN)|| defined(FUNC_AIP1629A_LED_EN))
// 显示播放时间.
void DispPlayTime(uint8_t Type)
{
	uint16_t TimeMMSS;
	static uint32_t PrevTime = 0;
	if(gPlayContrl == NULL)
	{
		return;
	}
	if(DIS_PLAYTIME_PALY == Type)
	{
		if(PrevTime != gPlayContrl->CurPlayTime)
		{
			PrevTime = gPlayContrl->CurPlayTime;
		}
		else
		{
			return;
		}
	}

	TimeMMSS = (gPlayContrl->CurPlayTime / 1000) / 60;
	TimeMMSS *= 100;
	TimeMMSS += (gPlayContrl->CurPlayTime / 1000) % 60;
	Num2Char(TimeMMSS, TRUE);
	//DBG(("gSongInfo.CurPlayTime:%u, TimeMMSS:%u\n", gSongInfo.CurPlayTime, TimeMMSS));
	if((TimeMMSS > 9999) && (gDisplayMode > DISP_DEV_LED1888))
	{
		DispChar('P', 3);
	}

	if(DIS_PLAYTIME_PAUSE == Type)
	{
		return;
	}

	DispIcon(ICON_COLON, LIGHT_ON);
	DispIcon(ICON_PLAY, LIGHT_ON);
	DispIcon(ICON_PAUSE, LIGHT_OFF);

	if((gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP2) || (gPlayContrl->CurSongInfo->stream_type == STREAM_TYPE_MP3))
	{
		DispIcon(ICON_SD, LIGHT_ON);
	}
	else
	{
		DispIcon(ICON_SD, LIGHT_OFF);
	}
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
	//DBG(("**%02BX, %02BX, %02BX, %02BX, %02BX\n", gDispBuff[0], gDispBuff[1], gDispBuff[2], gDispBuff[3], gDispBuff[3]));
}
#endif

#ifdef FUNC_RADIO_EN
// 显示FM预存台号
void DispRadioCHNum(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))

	if(sRadioControl == NULL)
	{
		return;
	}
	Num2Char((sRadioControl->CurStaIdx + 1), FALSE);
	TimeOutSet(&DispTmr, DEV_SYMBOL_INTERVAL);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
#ifdef FUNC_ZHIFENG_SOUNDBAR_DEMO_V1_0_EN
	DispIcon(ICON_DP, LIGHT_OFF);
#endif
#ifdef FUNC_AIP1629A_LED_EN
	DispIcon(ICON_FM, LIGHT_OFF);
	DispIcon(ICON_DP, LIGHT_OFF);
	DispIcon(ICON_MHZ, LIGHT_OFF);
#endif
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
#endif

}

void DispRadioPreviewNum(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))

       if(sRadioControl == NULL)
	{
		return;
	}
	
	Num2Char((sRadioControl->ChlCount), FALSE);		
	TimeOutSet(&DispTmr, DEV_SYMBOL_INTERVAL);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
#ifdef FUNC_AIP1629A_LED_EN
	DispIcon(ICON_FM, LIGHT_OFF);
	DispIcon(ICON_DP, LIGHT_OFF);
	DispIcon(ICON_MHZ, LIGHT_OFF);
#endif
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
#endif

}
#endif

// Exit from Special Display-Mode.
void DispResume(void)
{
#if 0//(defined(FUNC_SEG_LCD_EN) || (defined(FUNC_SEG_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
#if (defined(FUNC_RTC_EN) && defined(FUNC_AIP1629A_LED_EN))
		DispRtcIcon();
#endif
		switch(gSys.CurModuleID)
		{
			case MODULE_ID_IDLE:
#ifdef FUNC_RTC_EN
			case MODULE_ID_RTC:
				DispRtc(FALSE);
#else
				//DBG(("NOD1"));
				DispString(" NOD");
#endif
				break;

#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))
            case MODULE_ID_PLAYER_USB:
			case MODULE_ID_PLAYER_SD:
#ifdef FUNC_REC_PLAYBACK_EN
            case MODULE_ID_REC_BACK_PLAY:
#endif
				if(gPlayContrl == NULL)
				{
					return;
				}
				if(GetPlayState() == PLAYER_STATE_IDLE) 
				{
				    #ifdef FUNC_PROGRAM_EN
					if(gSys.ProgramEdit)
					{
						if(IsTimeOut(&DispProgTmr))
						{
							TimeOutSet(&DispProgTmr, 500);
							if(!IsTimeOut(&ProgEditTmr))
							{
								Num2Char(gSys.ProgramFile, FALSE);
							}
							else
							{
								if(DispProgFlashFlag)
								{
									DispProgFlashFlag = 0;
									Num2Char(gSys.ProgramFile, FALSE);
								}
								else
								{
									DispProgFlashFlag = 1;
									DispString("   ");
								}
							}
						}
						else
						{
							if(!IsTimeOut(&ProgEditTmr))
							{
								Num2Char(gSys.ProgramFile, FALSE);
							}
						}				
					}
					else if(gSys.ProgramMode)
					{
						Num2Char(gSys.ProgramFileLength, FALSE);
					}
					else
					#endif	
					{
						DispPlayState();
					}
				}
				else if(GetPlayState() == PLAYER_STATE_PAUSE)
				{
						DispPlayState();
				}	
				else if((GetPlayState() == PLAYER_STATE_PLAYING) || (GetPlayState() == PLAYER_STATE_FF) || (GetPlayState() == PLAYER_STATE_FB))
				{
					if(gDisplayMode == DISP_DEV_LED1888)
					{
						DispFileNum();
					}
					else
					{
						DispPlayTime(DIS_PLAYTIME_PALY);
					}
				}
#ifdef FUNC_REC_PLAYBACK_EN                
				if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
				{
					if(FsGetDriveType() == DEV_ID_SD)
					{
						DispIcon(ICON_SD, LIGHT_ON);
						DispIcon(ICON_USB, LIGHT_OFF);
					}
					else
					{
						DispIcon(ICON_USB, LIGHT_ON);
						DispIcon(ICON_SD, LIGHT_OFF);
					}
					break;
				}
#endif
				if(gSys.CurModuleID == MODULE_ID_PLAYER_SD)
				{
					DispIcon(ICON_SD, LIGHT_ON);
					DispIcon(ICON_USB, LIGHT_OFF);
				}
				else
				{
					DispIcon(ICON_USB, LIGHT_ON);
					DispIcon(ICON_SD, LIGHT_OFF);
				}
#ifdef FUNC_PROGRAM_EN
				if(gSys.ProgramEdit)
				{
				    if(DispProgFlashFlag)
			    	{
						DispIcon(ICON_PROG, LIGHT_ON);
			    	}
					else
					{
						DispIcon(ICON_PROG, LIGHT_ON);
					}
				}
				else if(gSys.ProgramMode)
				{
					DispIcon(ICON_PROG, LIGHT_ON);
				}
				else
				{
					DispIcon(ICON_PROG, LIGHT_OFF);
				}
#endif

				break;
#endif

#ifdef FUNC_RADIO_EN
			case MODULE_ID_RADIO:
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
#ifdef FUNC_AIP1629A_LED_EN
				DispIcon(ICON_FM, LIGHT_ON);
				DispIcon(ICON_DP, LIGHT_ON);
				DispIcon(ICON_MHZ, LIGHT_ON);
#endif
				DispIcon(ICON_DP_FM_MHZ, LIGHT_ON);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
			    DispIcon(ICON_PROG, LIGHT_OFF);
#endif				
				if(sRadioControl == NULL)
				{
					return;
				}
				if(sRadioControl->State == RADIO_STATUS_PREVIEW)
				{
					DispRadioPreviewNum();
				}
				else
				{
					if(sRadioControl->Freq == 0)
					{
						sRadioControl->Freq = RADIO_DEF_MIN_FREQ_VALUE;
					}
					Num2Char(sRadioControl->Freq, FALSE);
				}
				break;

#endif

#ifdef FUNC_LINEIN_EN
			case MODULE_ID_LINEIN:
				DispString(" AUX");
				DispIcon(ICON_AUX, LIGHT_ON);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispIcon(ICON_BT, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_DP, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
			    DispIcon(ICON_PROG, LIGHT_OFF);
#endif								
				break;
#endif

#ifdef FUNC_WIFI_EN
			case MODULE_ID_WIFI:
				DispString("HIFI");
				DispIcon(ICON_USB, LIGHT_ON);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispIcon(ICON_BT, LIGHT_OFF);
				//DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_AUX, LIGHT_OFF);
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_DP, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
				DispIcon(ICON_PROG, LIGHT_OFF);
#endif
				break;

			case MODULE_ID_PLAYER_WIFI_SD:
				DispString(" SD ");
				DispIcon(ICON_SD, LIGHT_ON);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispIcon(ICON_BT, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_AUX, LIGHT_OFF);
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_DP, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
				DispIcon(ICON_PROG, LIGHT_OFF);
#endif
				break;
#endif


#ifdef FUNC_BT_EN
			case MODULE_ID_BLUETOOTH:
				DispString("BLUE");
				DispIcon(ICON_BT, LIGHT_ON);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_AUX, LIGHT_OFF);
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_DP, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
				DispIcon(ICON_PROG, LIGHT_OFF);
#endif							
				break;
#endif
            
#ifdef FUNC_RECORD_EN
			case MODULE_ID_RECORDER:
				switch(RecordState)
				{
					case RECORD_STATE_INIT:
						DispString("Load");
						DispIcon(ICON_COLON, LIGHT_OFF);
						break;

					case RECORD_STATE_REC:
						{
							uint32_t Time    = GetRecordingTime();
							Num2Char((uint16_t)((Time / 1000) / 60) * 100 + (Time / 1000) % 60, TRUE);
						}
						DispIcon(ICON_COLON, LIGHT_ON);
						break;

					case RECORD_STATE_REC_PAUSE:

						DispString("Paus");
						DispIcon(ICON_COLON, LIGHT_OFF);
						break;
#ifndef FUNC_REC_PLAYBACK_EN
					case RECORD_STATE_PLAYINIT:
					case RECORD_STATE_PLAYBACK:
						{
							uint32_t Time    = GetRecBackPlayingTime();
							Num2Char((uint16_t)((Time / 1000) / 60) * 100 + (Time / 1000) % 60, TRUE);
						}
						DispIcon(ICON_COLON, LIGHT_ON);
						break;

					case RECORD_STATE_PLAYBACK_PAUSE:
						DispString("Paus");
						break;
#endif
					case RECORD_STATE_BROWSER:
						DispString("STOP");
						DispIcon(ICON_COLON, LIGHT_OFF);
						break;
				}

				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				if(gRecordContrl->RecordDeviceID == USB_DISK)
				{
					DispIcon(ICON_USB, LIGHT_ON);
					DispIcon(ICON_SD, LIGHT_OFF);
				}
				else
				{
					DispIcon(ICON_USB, LIGHT_OFF);
					DispIcon(ICON_SD, LIGHT_ON);
				}
#ifdef FUNC_PROGRAM_EN
			    DispIcon(ICON_PROG, LIGHT_OFF);
#endif								
#ifdef FUNC_AIP1629A_LED_EN
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
#endif
				break;
#endif

#ifdef FUNC_USB_AUDIO_EN
			case MODULE_ID_USB_AUDIO:
				DispString(" PC ");
				break;
#endif

#ifdef FUNC_USB_READER_EN
			case MODULE_ID_USB_READER:
				DispString(" REA");
				break;
#endif

#ifdef FUNC_USB_AUDIO_READER_EN
			case MODULE_ID_USB_AUDIO_READER:
				DispString(" PC ");
				break;
#endif

#ifdef FUNC_STANDBY_EN
			case SYS_MODE_STANDBY:
				DispString(" CLO");
				break;
#endif

			default:
				break;
		}

#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}


// Mute状态变化时调用.
void DispMute(void)
{
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		if(gSys.MuteFlag == TRUE)
		{
			DispIcon(ICON_MUTE, LIGHT_ON);
		}
		else
		{
			DispIcon(ICON_MUTE, LIGHT_OFF);
		}

		LcdFlushDispData();
	}
#endif

#if defined(FUNC_SEG_LED_EN)|| defined(FUNC_AIP1629A_LED_EN)
//这部分代码用户可以根据需要打开
	if((gDisplayMode < DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if(gSys.MuteFlag == TRUE)
		{
			//DBG(("LED MUTE\n"));
			if(((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD)
#ifdef FUNC_REC_PLAYBACK_EN                
                || (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
#endif
                )
			        && (GetPlayState() == PLAYER_STATE_IDLE))
			{

				TimeOutSet(&DispTmr, 0);
			}
		}
	}
#endif
}

// Show Device symbol.
void DispDevSymbol(void)
{
#if defined(FUNC_SEG_LED_EN)|| defined(FUNC_AIP1629A_LED_EN)
	if((gDisplayMode == DISP_DEV_LED8888) || (gDisplayMode == DISP_DEV_LED1888) || (gDisplayMode == DISP_DEV_LED57))
	{
		ClearAllIcon();
		LedDispDevSymbol();
	}
#endif

#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdDispDevSymbol();
		LcdFlushDispData();
	}
#endif

}


void DispLoad(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
#ifdef FUNC_POWER_MONITOR_EN
		PowerMonitorDisp();
#endif
		DispString(" LOD");
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDisp();
		}
#endif
	}
#endif
}


// 进入Audio显示时调用
void DispAudio(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	ClearScreen();
	DispString(" PC ");
#endif
}
#ifdef FUNC_MULTI_UDISK_PORT_EN
//显示UDiskPort number信息
void DispUDiskPortNum(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispIcon(ICON_COLON, LIGHT_OFF);
		if(gSys.UDiskPortNum == 1)
		{
			DispString("USB1");
		}
		else
		{
			DispString("USB2");
		}
	}
#endif
}
#endif
//显示Power on信息
void DispPowerOn(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispString(" HI ");
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
			LcdFlushDisp();
		}
#endif
	}
#endif
}
//显示Power off信息
void DispPowerOff(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispString(" Bye");
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
			LcdFlushDisp();
		}
#endif
	}
#endif
}

#ifdef FUN_SYSTEM_POWEROFF_WAIT_TIME
//该函数级别高主要处理设置
static uint8_t DispSysTimOff(void)
{
	if((gSysTimeOffState.ProState > SYS_OFF_TIMEING_IDLE) && (gSysTimeOffState.ProState < SYS_OFF_TIMEING_WAIT_POWEROFF))
	{
		if(gDisplayMode == DISP_DEV_LED57)
		{
			DispIcon(ICON_PLAY, LIGHT_OFF);
			DispIcon(ICON_PAUSE, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
		}
		else if(gDisplayMode == DISP_DEV_LED8888)
		{
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
		}

		if(gSysTimeOffState.EnState == TRUE)
		{
			DispIcon(ICON_SLEEP, LIGHT_ON);

			switch(gSysTimeOffState.SleepState)
			{
				case SLEEP_15:
					DispString("  15");
					break;

				case SLEEP_30:
					DispString("  30");
					break;

				case SLEEP_45:
					DispString("  45");
					break;

				case SLEEP_60:
					DispString("  60");
					break;

				case SLEEP_75:
					DispString("  75");
					break;

				case SLEEP_90:
					DispString("  90");
					break;

				case SLEEP_105:
					DispString(" 105");
					break;

				case SLEEP_120:
					DispString(" 120");
					break;

				default:
					break;
			}
		}
		else
		{
			DispString(" CLO");
			DispIcon(ICON_SLEEP, LIGHT_OFF);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#endif


#ifdef FUNC_RTC_EN
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))

const ICON_IDX Alarm_ICON[3][3] = {ICON_NULL, ICON_A1MARK, ICON_A2MARK, ICON_NULL, ICON_ALARM1, ICON_ALARM2, ICON_NULL, ICON_A1LOOP, ICON_A2LOOP};

//显示RTC标志
void DispRtcIcon(void)
{
	DispIcon(ICON_A1MARK, LIGHT_OFF);
	DispIcon(ICON_A2MARK, LIGHT_OFF);
	DispIcon(ICON_ALARM1, LIGHT_OFF);
	DispIcon(ICON_ALARM2, LIGHT_OFF);
	DispIcon(ICON_A1LOOP, LIGHT_OFF);
	DispIcon(ICON_A2LOOP, LIGHT_OFF);

	if(sRtcControl->State == RTC_STATE_IDLE)										//显示实时时钟
	{	
		if(((RtcGetAlarmStatus(1) == ALARM_STATUS_OPENED) && (sRtcControl->CurAlarmNum != 1))
		|| ((sRtcControl->CurAlarmNum == 1) && gBlink.FalshBlinkFlag && sRtcControl->CurAlarmRun))
		{
			DispIcon(ICON_A1MARK, LIGHT_ON);
			DispIcon(ICON_ALARM1, LIGHT_ON);
			if(sNvmRtcInfo.AlarmTimeList[0].AlarmMode != ALARM_MODE_ONCE_ONLY)
			{
				DispIcon(ICON_A1LOOP, LIGHT_ON);
			}
		}

		if(((RtcGetAlarmStatus(2) == ALARM_STATUS_OPENED) && (sRtcControl->CurAlarmNum != 2))
		|| ((sRtcControl->CurAlarmNum == 2) && gBlink.FalshBlinkFlag && sRtcControl->CurAlarmRun))
		{
			DispIcon(ICON_A2MARK, LIGHT_ON);
			DispIcon(ICON_ALARM2, LIGHT_ON);
			if(sNvmRtcInfo.AlarmTimeList[1].AlarmMode != ALARM_MODE_ONCE_ONLY)
			{
				DispIcon(ICON_A2LOOP, LIGHT_ON);
			}
		}
	}
	else 
	{
		if(sRtcControl->State == RTC_STATE_SET_ALARM)							   //显示当前闹钟
		{
			DispIcon(Alarm_ICON[0][sRtcControl->AlarmNum], LIGHT_ON);
			DispIcon(Alarm_ICON[1][sRtcControl->AlarmNum], LIGHT_ON);
		}
		DispIcon(ICON_COLON, LIGHT_ON);
	}
}

//param:TRUE 设置状态显示；FALSE    ：空闲状态显示
void DispRtc(bool DispState)
{
	RTC_DATE_TIME gDisRtcVal;

	//设置状态，取消设置位闪烁。
	if(DispState == TRUE)
	{
		gBlink.FalshBlinkFlag = TRUE;
	}
	
#ifdef FUNC_RTC_ALARM
	if(sRtcControl->State == RTC_STATE_SET_ALARM)
	{
		gDisRtcVal = sRtcControl->AlarmTime;
	}
	else
#endif
	{
		gDisRtcVal = sRtcControl->DataTime;
	}

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispIcon(ICON_PLAY, LIGHT_OFF);
		DispIcon(ICON_PAUSE, LIGHT_OFF);
		DispIcon(ICON_WIFI, LIGHT_OFF);
		DispIcon(ICON_BT, LIGHT_OFF);
		DispIcon(ICON_USB, LIGHT_OFF);
		DispIcon(ICON_SD, LIGHT_OFF);
		DispIcon(ICON_AUX, LIGHT_OFF);
		DispIcon(ICON_FM, LIGHT_OFF);
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispRtcIcon();
		//显示相关状态及信息
		switch(sRtcControl->SubState)
		{
			case RTC_SET_IDLE:
				Temp = gDisRtcVal.Hour * 100;
				Temp += gDisRtcVal.Min;
				Num2Char(Temp, TRUE);

				if(gBlink.FalshBlinkFlag)//秒闪烁
				{
					DispIcon(ICON_COLON, LIGHT_ON);
				}
				else
				{
					DispIcon(ICON_COLON, LIGHT_OFF);
				}
				break;

			case RTC_SET_YEAR:
				DispIcon(ICON_COLON, LIGHT_OFF);
				if(gBlink.FalshBlinkFlag)
				{
					Temp = gDisRtcVal.Year;
					Num2Char(Temp, TRUE);
				}
				else
				{
				    DispString("    ");
				}
				break;

			case RTC_SET_MON:
				DispIcon(ICON_COLON, LIGHT_OFF);
				if(gBlink.FalshBlinkFlag)
				{
					Temp = gDisRtcVal.Mon * 100;
					Temp += gDisRtcVal.Date;
					Num2Char(Temp, TRUE);
				}
				else
				{
					DispChar(' ', 1);
					DispChar(' ', 0);
				}
				break;

			case RTC_SET_DAY:
				DispIcon(ICON_COLON, LIGHT_OFF);
				if(gBlink.FalshBlinkFlag)
				{
					Temp = gDisRtcVal.Mon * 100;
					Temp += gDisRtcVal.Date;
					Num2Char(Temp, TRUE);
				}
				else
				{
					DispChar(' ', 3);
					DispChar(' ', 2);
				}
				break;

			case RTC_SET_HOUR:
				DispIcon(ICON_COLON, LIGHT_ON);
				if(gBlink.FalshBlinkFlag)
				{
					Temp = gDisRtcVal.Hour * 100;
					Temp += gDisRtcVal.Min;
					Num2Char(Temp, TRUE);
				}
				else
				{
					DispChar(' ', 1);
					DispChar(' ', 0);
				}
				break;

			case RTC_SET_MIN:
				DispIcon(ICON_COLON, LIGHT_ON);
				if(gBlink.FalshBlinkFlag)
				{
					Temp = gDisRtcVal.Hour * 100;
					Temp += gDisRtcVal.Min;
					Num2Char(Temp, TRUE);
				}
				else
				{
					DispChar(' ', 3);
					DispChar(' ', 2);
				}
				break;

#ifdef FUNC_RTC_ALARM
			case RTC_SET_ALARM_MODE:
				DispIcon(ICON_COLON, LIGHT_OFF);
				if(sNvmRtcInfo.AlarmTimeList[sRtcControl->AlarmNum-1].AlarmMode != ALARM_MODE_ONCE_ONLY)
				{
					DispIcon(Alarm_ICON[2][sRtcControl->AlarmNum], LIGHT_ON);
				}
				
				if(sRtcControl->AlarmMode == ALARM_MODE_ONCE_ONLY)
				{
					DispString("ONCE");
				}
				else if(sRtcControl->AlarmMode == ALARM_MODE_PER_DAY)
				{
					DispString(" ALL");
				}
				else if(sRtcControl->AlarmMode == ALARM_MODE_PER_WEEK)
				{
					DispString("WEEK");
				}
				else if(sRtcControl->AlarmMode == ALARM_MODE_WORKDAY)
				{
					DispString(" JOB");
				}
				else if(sRtcControl->AlarmMode == ALARM_MODE_USER_DEFINED)
				{
					DispString("USER");
				}
				else
				{
					DispString("ERRO");
				}
				break;

			case RTC_SET_WEEK:
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispChar('X', 0);
				DispChar('Q', 1);
				DispChar(' ', 2);
				if(gBlink.FalshBlinkFlag)
				{
					DispChar((gDisRtcVal.WDay + 0x30), 3);
				}
				else
				{
					DispChar(' ', 3);
				}
				break;

			case RTC_SET_ALARM_ONOFF:
				DispIcon(ICON_COLON, LIGHT_OFF);
				if(RtcGetAlarmStatus(sRtcControl->AlarmNum) == ALARM_STATUS_OPENED)
				{	
					DispString("  ON");
					DispIcon(Alarm_ICON[1][sRtcControl->AlarmNum], LIGHT_ON);
				}
				else
				{
					DispString(" OFF");
				}
				break;
				
			case RTC_SET_ALARM_SOURCE:
				if(sNvmRtcInfo.RingType[sRtcControl->AlarmNum] == RTC_RING_SDCARD_TYPE)
				{
					DispString("S-SD");
				}
				else
				{
					DispChar('S', 0);
					DispChar('-', 1);
					DispChar((sNvmRtcInfo.RingType[sRtcControl->AlarmNum]/10), 2);
					DispChar((sNvmRtcInfo.RingType[sRtcControl->AlarmNum]%10), 3);
				}
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;

			case RTC_SET_ALARM_VOLUME:
				DispChar(' ', POS_COM_THOUSAND);
				DispChar('U', POS_COM_HUNDREDS);
				DispChar((sNvmRtcInfo.AlarmVolume / 10), POS_COM_TENS);
				DispChar((sNvmRtcInfo.AlarmVolume % 10), POS_COM_UNITS);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

			default:
				break;
		}
	}
#endif
}
#endif
#endif

#ifdef FUNC_SPEC_TRUM_EN
void DispSpecTrum(void)
{
#if 0//ndef FUNC_AUTO_TREB_OFF_EN
	if(!IsTimeOut(&DispSpecTrumTmr))
	{
		return;
	}
	TimeOutSet(&DispSpecTrumTmr, 20);
	if(AudioSpecTrumStepBak <= AudioSpecTrumStep)
	{
		AudioSpecTrumStepBak = AudioSpecTrumStep;
	}
	else
	{
		if(!IsTimeOut(&DispSpecTrumDnTmr))
		{
			return;
		}
		TimeOutSet(&DispSpecTrumDnTmr, 50);
		AudioSpecTrumStepBak--;
	}
	switch(AudioSpecTrumStepBak)
	{
		case 1:
			//DispIcon(ICON_FFT_L1, LIGHT_ON);
			//DispIcon(ICON_FFT_L2, LIGHT_OFF);
			//DispIcon(ICON_FFT_L3, LIGHT_OFF);
			//DispIcon(ICON_FFT_L4, LIGHT_OFF);
			//DispIcon(ICON_FFT_L5, LIGHT_OFF);
			//DispIcon(ICON_FFT_R1, LIGHT_ON);
			//DispIcon(ICON_FFT_R2, LIGHT_OFF);
			//DispIcon(ICON_FFT_R3, LIGHT_OFF);
			//DispIcon(ICON_FFT_R4, LIGHT_OFF);
			//DispIcon(ICON_FFT_R5, LIGHT_OFF);
			break;
		case 2:
			//DispIcon(ICON_FFT_L1, LIGHT_ON);
			//DispIcon(ICON_FFT_L2, LIGHT_ON);
			//DispIcon(ICON_FFT_L3, LIGHT_OFF);
			//DispIcon(ICON_FFT_L4, LIGHT_OFF);
			//DispIcon(ICON_FFT_L5, LIGHT_OFF);
			//DispIcon(ICON_FFT_R1, LIGHT_ON);
			//DispIcon(ICON_FFT_R2, LIGHT_ON);
			//DispIcon(ICON_FFT_R3, LIGHT_OFF);
			//DispIcon(ICON_FFT_R4, LIGHT_OFF);
			//DispIcon(ICON_FFT_R5, LIGHT_OFF);
			break;
		case 3:
			//DispIcon(ICON_FFT_L1, LIGHT_ON);
			//DispIcon(ICON_FFT_L2, LIGHT_ON);
			//DispIcon(ICON_FFT_L3, LIGHT_ON);
			//DispIcon(ICON_FFT_L4, LIGHT_OFF);
			//DispIcon(ICON_FFT_L5, LIGHT_OFF);
			//DispIcon(ICON_FFT_R1, LIGHT_ON);
			//DispIcon(ICON_FFT_R2, LIGHT_ON);
			//DispIcon(ICON_FFT_R3, LIGHT_ON);
			//DispIcon(ICON_FFT_R4, LIGHT_OFF);
			//DispIcon(ICON_FFT_R5, LIGHT_OFF);
			break;
		case 4:
			//DispIcon(ICON_FFT_L1, LIGHT_ON);
			//DispIcon(ICON_FFT_L2, LIGHT_ON);
			//DispIcon(ICON_FFT_L3, LIGHT_ON);
			//DispIcon(ICON_FFT_L4, LIGHT_ON);
			//DispIcon(ICON_FFT_L5, LIGHT_OFF);
			//DispIcon(ICON_FFT_R1, LIGHT_ON);
			//DispIcon(ICON_FFT_R2, LIGHT_ON);
			//DispIcon(ICON_FFT_R3, LIGHT_ON);
			//DispIcon(ICON_FFT_R4, LIGHT_ON);
			//DispIcon(ICON_FFT_R5, LIGHT_OFF);
			break;
		case 5:
			//DispIcon(ICON_FFT_L1, LIGHT_ON);
			//DispIcon(ICON_FFT_L2, LIGHT_ON);
			//DispIcon(ICON_FFT_L3, LIGHT_ON);
			//DispIcon(ICON_FFT_L4, LIGHT_ON);
			//DispIcon(ICON_FFT_L5, LIGHT_ON);
			//DispIcon(ICON_FFT_R1, LIGHT_ON);
			//DispIcon(ICON_FFT_R2, LIGHT_ON);
			//DispIcon(ICON_FFT_R3, LIGHT_ON);
			//DispIcon(ICON_FFT_R4, LIGHT_ON);
			//DispIcon(ICON_FFT_R5, LIGHT_ON);
			break;
		default:
			//DispIcon(ICON_FFT_L1, LIGHT_OFF);
			//DispIcon(ICON_FFT_L2, LIGHT_OFF);
			//DispIcon(ICON_FFT_L3, LIGHT_OFF);
			//DispIcon(ICON_FFT_L4, LIGHT_OFF);
			//DispIcon(ICON_FFT_L5, LIGHT_OFF);
			//DispIcon(ICON_FFT_R1, LIGHT_OFF);
			//DispIcon(ICON_FFT_R2, LIGHT_OFF);
			//DispIcon(ICON_FFT_R3, LIGHT_OFF);
			//DispIcon(ICON_FFT_R4, LIGHT_OFF);
			//DispIcon(ICON_FFT_R5, LIGHT_OFF);
			break;
	}
#endif
}
#endif
#ifdef FUNC_SOUND_REMIND
void DispAlarmRemind1(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if(gSys.SoundRemindOn)
		{
			DispString(" ON ");
		}
		else
		{
			DispString(" OFF");
		}
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}
void DispAlarmRemind2(void)
{
#if 0//(defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if(!gSys.AlarmRemindCompulsoryOff) 
       	{
			DispString(" ON ");
       	}
		else
		{
			DispString(" OFF");
       	}
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}
#endif
#ifdef FUNC_MIC_FIRST_EN
void DispMicFirst(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
	       if(MicFirstFlag) 
       	{
			DispString(" ON ");
       	}
		else
		{
			DispString(" OFF");
       	}
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
void Disp3D(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
	       if(gSys.Audio3dEffectFlag) 
       	{
			DispString(" ON ");
       	}
		else
		{
			DispString(" OFF");
       	}
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}
#endif
void DispErr(void)
{
 #if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispString(" ERR ");
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif       

}
#ifdef FUNC_RECORD_FILE_DELETE_EN
void DispFileDelete(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispString(" Del");
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}
#endif
void DispNo(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispString(" No ");
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}

void DispVersion(void)
{
      DispString("1410");
    //  TimeOutSet(&DispMsgTmr, 2000);	  
}
#ifdef FUNC_SLEEP_EN
void DispSleepRemainTime(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))
	//if((gSys->CurModuleID == MODULE_ID_PLAYER_USB) || (gSys->CurModuleID == MODULE_ID_PLAYER_SD))
	{
	    uint8_t Temp;
			
		if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
		{
			if(gSys.SleepTime== 0)
			{
				DispChar(' ', 0);
				DispChar('O', 1);
				DispChar('F', 2);
				DispChar('F', 3);
				DispChar(' ', 4);
			}
			else 
			{
				Temp = gSys.SleepTime - gSys.SleepTimeCnt/30000;				
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(Temp/10, 3);
				DispChar(Temp%10, 4);				
			}			
			//ClearIcon();
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				LcdFlushDispData();
			}
#endif
		}
	}
	gDispPrevEQ = gSys.Eq;
#endif		
}

void DispSleepTime(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))
	//if((gSys->CurModuleID == MODULE_ID_PLAYER_USB) || (gSys->CurModuleID == MODULE_ID_PLAYER_SD))
	{
		if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
		{
			if(gSys.SleepTime == 0)
			{
				DispChar(' ', 0);
				DispChar('O', 1);
				DispChar('F', 2);
				DispChar('F', 3);
				DispChar(' ', 4);
			}
			else if(gSys.SleepTime == 10)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(1, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 20)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(2, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 30)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(3, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 40)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(4, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 50)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(5, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 60)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(6, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 70)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(7, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 80)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(8, 3);
				DispChar(0, 4);				
			}			
			else if(gSys.SleepTime == 90)
			{
				DispChar('S', 0);
				DispChar('L', 1);
				DispChar('P', 2);
				DispChar(9, 3);
				DispChar(0, 4);				
			}	
			//ClearIcon();
			TimeOutSet(&DispTmr, INTERIM_INTERVAL);
#ifdef FUNC_SEG_LCD_EN
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				LcdFlushDispData();
			}
#endif
		}
	}
	gDispPrevEQ = gSys.Eq;
#endif		
}
#endif

#ifdef FUNC_PROGRAM_EN
void DispProgramFull(void)
{
 #if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode > DISP_DEV_LED1888))
	{
		DispString("FULL");
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		//DispIcon(ICON_DP, LIGHT_OFF);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
	else if(gDisplayMode == DISP_DEV_LED1888)
	{
		DispString("FUL");
	}
#endif       

}
void DispProgramNum(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))    	
	DispChar('P', POS_COM_THOUSAND);
	DispChar((gSys.ProgramOffset / 10), POS_COM_HUNDREDS);
	DispChar((gSys.ProgramOffset % 10), POS_COM_TENS);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
	DispIcon(ICON_PLAY, LIGHT_OFF);
	DispIcon(ICON_FOLDER, LIGHT_OFF);
	DispIcon(ICON_PROG, LIGHT_ON);
#ifdef FUNC_ZHIFENG_SOUNDBAR_DEMO_V1_0_EN
	DispIcon(ICON_DP, LIGHT_OFF);
#endif
	#ifdef FUNC_AIP1629A_LED_EN
	DispIcon(ICON_FM, LIGHT_OFF);
	DispIcon(ICON_DP, LIGHT_OFF);
	DispIcon(ICON_MHZ, LIGHT_OFF);
	#endif
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif
#endif

}
#endif

// 主函数调用.
void Display(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
  //增加单颗LED显扫描
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode > DISP_DEV_SLED))
	{
#ifdef FUNC_SPEC_TRUM_EN
		DispSpecTrum();
#endif
		if(!IsTimeOut(&DispMsgTmr))
		{
			switch(gDispMsgId)
			{
				case DISP_MSG_PWR_ON:
					DispPowerOn();
					break;
				case DISP_MSG_PWR_OFF:
					DispPowerOff();
					break;
				case DISP_MSG_READ:
					ClearAllIcon();
					DispLoad();
					break;
				case DISP_MSG_DEV:
					DispDevSymbol();
					break;
#ifdef FUNC_MULTI_UDISK_PORT_EN
				case DISP_MSG_UDISK_PORT:
					DispUDiskPortNum();
					break;
#endif
				case DISP_MSG_FILE:
					if((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD) 
						#ifdef FUNC_REC_PLAYBACK_EN
						|| (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
						#endif
						)
					{
						if(gPlayContrl == NULL)
						{
							return;
						}
						if(gPlayContrl->FileOpenSuccessFlag != NULL)
						{
							DispFileNum();
						}
					}
					break;
				case DISP_MSG_SEL_NUM:
					if((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD) 
						#ifdef FUNC_REC_PLAYBACK_EN
						|| (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
						#endif
						)
					{
						if(gPlayContrl == NULL)
						{
							return;
						}
						DispSelNum();
					}
					break;
				case DISP_MSG_PLAY_MODE:
					DispRepeat(TRUE);
					break;
				case DISP_MSG_REP_AB:
					DispRepeatAB(TRUE);
					break;
				case DISP_MSG_MVOL:
					DispVolume(TRUE);
					break;
				case DISP_MSG_EQ:
					DispEQ(TRUE);
					break;
				case DISP_MSG_MIC_VOL:
					DispMicVolume(TRUE);
					break;
#ifdef FUNC_GUITAR_EN
				case DISP_MSG_GUITAR_VOL:
					DispGuitarVolume(TRUE);
					break;
#endif
#ifdef FUNC_MIC_ECHO_EN
				case DISP_MSG_ECHO_DELAY:
					DispEchoDelayStep(TRUE);
					break;
				case DISP_MSG_ECHO_DEPTH:
					DispEchoDepthStep(TRUE);
					break;
#endif
#ifdef FUNC_TREB_BASS_EN
				case DISP_MSG_TREB:
					DispTreb(TRUE);
					break;
				case DISP_MSG_BASS:
					DispBass(TRUE);
					break;
#endif

#ifdef FUNC_RADIO_EN
				case DISP_MSG_RADIO_CH:
					DispRadioCHNum();
					break;
#endif
#ifdef FUNC_SOUND_REMIND
				case DISP_MSG_ALARM_REMIND1:
					DispAlarmRemind1();
					break;
				case DISP_MSG_ALARM_REMIND2:
					DispAlarmRemind2();
					break;
#endif
                case DISP_MSG_ERR:
					DispErr();			
					break;
#ifdef FUNC_RECORD_FILE_DELETE_EN
                case DISP_MSG_FILE_DEL:
					DispFileDelete();
					break;
#endif
#ifdef FUNC_MIC_FIRST_EN
                case DISP_MSG_MIC_FIRST:
					DispMicFirst();
					break;
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
                case DISP_MSG_3D:
					Disp3D();
					break;
#endif
                case DISP_MSG_NO:
					DispNo();
					break;
				case DISP_MSG_VER:
					DispVersion();
					break;
#ifdef FUNC_PROGRAM_EN
				case DISP_MSG_PROG_FULL:
					DispProgramFull();
					break;
				case DISP_MSG_PROG_NUM:
					DispProgramNum();
					break;
#endif	

#ifdef FUNC_RTC_EN
				case DISP_MSG_RTC_SET:
					DispRtc(TRUE);
					break;
#endif

#ifdef FUNC_BT_HF_EN
				case DISP_MSG_BTHF_VOL:
					DispBTHfVolume(TRUE);
					break;
#endif

				default:
					break;
			}
		}
		else
		{
			if(gDispMsgId == DISP_MSG_MVOL)
			{
				AudioSysInfoSetBreakPoint();
			}
			gDispMsgId = DISP_MSG_NONE;
			
			if(IsTimeOut(&DispTmr))
			{
				static uint8_t BlinkCnt;
				
				TimeOutSet(&DispTmr, FFFB_BLINK_INTERVAL);
				gBlink.FalshBlinkFlag = !gBlink.FalshBlinkFlag;
				BlinkCnt++;
				if(BlinkCnt >= 2)	
				{
					gBlink.SlowBlinkFlag = !gBlink.SlowBlinkFlag;
					BlinkCnt = 0;
				}
			}
			DispResume();
		}
#ifdef FUNC_AIP1629A_LED_EN					
		AIP1629A_FlushDispData();
#endif
	}
#endif

#ifdef FUNC_SINGLE_LED_EN
	LedFlushDisp();
#endif
}

#endif

