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
//#include "mp3enc_api.h"
//#include "app/MP3_encoder.h"
#include "idle_control.h"

#include "eq.h"
#include "eq_params.h"
#include "sys_vol.h"
#ifdef FUNC_RADIO_QN8027_EN
#include "QN8027.h"
#endif
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
//==============================================================================
// Global variants.
#ifdef FUNC_TM1628_LED_EN
DISP_TYPE	gDispBuff[MAX_COM_NUM * 2] = {0};
DISP_TYPE	gDispBuffBak[MAX_COM_NUM * 2] = {0};
#else
DISP_TYPE	gDispBuff[MAX_COM_NUM] = {0};
#endif
DISP_DEV	gDisplayMode = DISP_DEV_NONE;
uint8_t 	gDispPrevRep;
uint8_t 	gDispPrevRepAB;
uint8_t 	gDispPrevEQ;
uint8_t 	gDispCurVol;
uint8_t 	gDispPrevVol;
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
//TIMER		DispSetTmr;
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
//----------------------------------------------------------

// Clear digit-value(清数值显示位).
void ClearDigit(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) ||defined(FUNC_TM1628_LED_EN))
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
	DispIcon(ICON_MP3, LIGHT_OFF);
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

#else
		baGPIOCtrl[LED_SEG_PORT_1P7MA] &= (~MASK_LED_SEG);
		baGPIOCtrl[LED_SEG_PORT_2P4MA] &= (~MASK_LED_SEG);
#endif
	}
#endif
#ifdef FUNC_TM1628_LED_EN
	TM1628_ClearDisplay();
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
	gDispPrevVol = gSys.Volume;
	gBlink.BlinkFlag = 0;
	//gDispState = DISP_STATE_DEV;
	memset(&gDispBuff, 0, sizeof(gDispBuff));

#ifdef FUNC_SINGLE_LED_EN
	SLedInit();
#endif

#ifdef FUNC_SEG_LED_EN
	LedPinGpioInit();
#endif
#ifdef FUNC_TM1628_LED_EN
	TM1628_Init();
#endif

#ifdef FUNC_SEG_LCD_EN
	//SetGpioRegBit(GPIO_TYPE_REG, MSK_ADC_A1_SWITCH);		//ADC Vol  preq
//	LcdInit(MASK_LCD_SEG, MASK_LCD_COM, LCD_BIAS_2);
//	LcdInit(MASK_LCD_SEG, MASK_LCD_COM, LCD_BIAS_3);
//	LcdInit(MASK_LCD_SEG, MASK_LCD_COM, LCD_BIAS_4);
	LCD_Drv_Init();
//	DBG(("LcdInit(LCD初始化%x,%x,)\n", MASK_LCD_SEG, MASK_LCD_COM));
#endif

#if defined(FUNC_SEG_LED_EN)
	gLedDispRefreshFlag = TRUE;
#endif

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))
	if(IsStandBy == TRUE)
	{
		ClearScreen();
	}
	else
	{
		if((gDisplayMode == DISP_DEV_NONE) && (gDisplayMode == DISP_DEV_SLED))
		{
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
#ifdef FUNC_TM1628_LED_EN
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

#ifdef FUNC_SINGLE_LED_EN
	if(gDisplayMode == DISP_DEV_SLED)
	{
		SLedDeinit();
	}
#endif

#if defined(FUNC_SEG_LED_EN) ||defined(FUNC_TM1628_LED_EN)
	gLedDispRefreshFlag = FALSE;	//解决进入Power down 过程中还会有段点亮问题

	if((gDisplayMode == DISP_DEV_LED1888) || (gDisplayMode == DISP_DEV_LED8888) || (gDisplayMode == DISP_DEV_LED57))
	{
#ifdef FUNC_TM1628_LED_EN
		TM1628_DisplayOff();
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
	gDispPrevVol = gSys.Volume;
}


// 设置Repeat模式时调用.
void DispRepeat(bool IsForced)
{
	if(gPlayContrl == NULL)
	{
		return;
	}
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
	if(((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD) 
		#ifdef FUNC_REC_PLAYBACK_EN
		|| (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		#endif
		)
	        && ((IsForced == TRUE) || (gDispPrevRep != gPlayContrl->CurPlayMode)))
	{
#if defined(FUNC_SEG_LED_EN) || defined(FUNC_TM1628_LED_EN)
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
	if(((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD 
		#ifdef FUNC_REC_PLAYBACK_EN
		|| (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		#endif
		))
	        && ((IsForced == TRUE) || (gDispPrevRepAB != gPlayContrl->RepeatAB.RepeatFlag)))
	{
#if defined(FUNC_SEG_LED_EN) || defined(FUNC_TM1628_LED_EN)
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		DispString("AU");
//		DispChar((InputChannel / 10), POS_COM_TENS);
//		DispChar((InputChannel % 10), POS_COM_UNITS);
		DispIcon(ICON_USB, LIGHT_OFF);
		DispIcon(ICON_SD, LIGHT_OFF);
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_MP3, LIGHT_OFF);
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

#if defined(FUNC_SEG_LED_EN)|| defined(FUNC_TM1628_LED_EN)
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


// 文件夹功能打开/关闭时调用.
void DispFoldState(void)
{
#if 0//(defined(FUNC_SEG_LCD_EN) || defined(FUNC_SEG_LED_EN)|| defined(FUNC_TM1628_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if(gPlayCtrl.FolderEnable == TRUE)
		{
			DispString(" FDI");
		}
		else
		{
			DispString(" FEN");
		}
		DispIcon(ICON_COLON, LIGHT_OFF);
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		TimeOutSet(&DispTmr, NORMAL_INTERVAL);
#ifdef FUNC_SEG_LCD_EN
		if(gDisplayMode == DISP_DEV_LCD58)
		{
			LcdFlushDispData();
		}
#endif
	}
#endif
}

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
// 音量设置时调用.
void DispVolume(bool IsForced)
{
	//DBG(("DispVolume,0\n"));
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if((IsForced == TRUE) || (gDispPrevVol !=  gDispCurVol))
		{
			//DBG(("DispVolume,1\n"));
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

	gDispPrevVol =  gDispCurVol;
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
#endif

#ifdef FUNC_GUITAR_EN
// GUITAR音量设置时调用.
void DispGuitarVolume(bool IsForced)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))

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
	#ifdef FUNC_TM1628_LED_EN
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))

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
#ifdef FUNC_TM1628_LED_EN
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

#if (defined(FUNC_SEG_LCD_EN) || defined(FUNC_SEG_LED_EN)|| defined(FUNC_TM1628_LED_EN))
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
		DispIcon(ICON_MP3, LIGHT_ON);
	}
	else
	{
		DispIcon(ICON_MP3, LIGHT_OFF);
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))

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
#ifdef FUNC_TM1628_LED_EN
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))

       if(sRadioControl == NULL)
	{
		return;
	}
	
	Num2Char((sRadioControl->ChlCount), FALSE);		
	TimeOutSet(&DispTmr, DEV_SYMBOL_INTERVAL);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
#ifdef FUNC_TM1628_LED_EN
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

#ifdef FUNC_RADIO_QN8027_EN
static void DispQN8027Freq(void)
{
	DispIcon(ICON_USB, LIGHT_OFF);
	DispIcon(ICON_SD, LIGHT_OFF);
#ifdef FUNC_TM1628_LED_EN
	DispIcon(ICON_FM, LIGHT_ON);
	DispIcon(ICON_DP, LIGHT_ON);
	DispIcon(ICON_MHZ, LIGHT_ON);
#endif
	DispIcon(ICON_DP_FM_MHZ, LIGHT_ON);
	DispIcon(ICON_MP3, LIGHT_OFF);
	DispIcon(ICON_PLAY, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
	DispIcon(ICON_PROG, LIGHT_OFF);
#endif	
  Num2Char(gSys.QN8027_Freq, FALSE);	
}
#endif
// Exit from Special Display-Mode.
void DispResume(void)
{
#if (defined(FUNC_SEG_LCD_EN) || defined(FUNC_SEG_LED_EN)|| defined(FUNC_TM1628_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		switch(gSys.CurModuleID)
		{
#ifdef IDLE_MODULE
			case MODULE_ID_IDLE:
#ifndef FUNC_RTC_EN
				//DBG(("NOD1"));
				DispString(" NOD");
#else
				DispRtc();
#endif
				break;
#endif

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
#ifdef FUNC_TM1628_LED_EN
				DispIcon(ICON_FM, LIGHT_ON);
				DispIcon(ICON_DP, LIGHT_ON);
				DispIcon(ICON_MHZ, LIGHT_ON);
#endif
				DispIcon(ICON_DP_FM_MHZ, LIGHT_ON);
				DispIcon(ICON_MP3, LIGHT_OFF);
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

#ifdef FUNC_TM1628_LED_EN
				if(gSys.CurModuleID == MODULE_ID_LINEIN)
				{
					DispIcon(ICON_AUX, LIGHT_ON);
				}
#endif

				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
			    DispIcon(ICON_PROG, LIGHT_OFF);
#endif								
#ifdef FUNC_TM1628_LED_EN
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_DP, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
#endif
				break;
#endif
#ifdef FUNC_BT_EN
			case MODULE_ID_BLUETOOTH:
#ifdef FUNC_RADIO_QN8027_EN
			  DispQN8027Freq();
#else
				DispString(" BT ");
#ifdef FUNC_TM1628_LED_EN
				DispIcon(ICON_BT, LIGHT_ON);
#endif

				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
#ifdef FUNC_PROGRAM_EN
			    DispIcon(ICON_PROG, LIGHT_OFF);
#endif	
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
#ifdef FUNC_TM1628_LED_EN
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
#endif
				break;
#endif
//#ifdef FUNC_RTC_EN
//	   		case SYS_MODE_RTC:
//				Temp = gRtcTime.Hour*100;
//				Temp += gRtcTime.Min;
//				Num2Char(Temp, TRUE);
//				DispIcon(ICON_COLON, LIGHT_BLINK);
//				break;
//#endif

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
#ifndef FUNC_RTC_EN
				DispString(" CLO");
#else
				DispRtc();
#endif
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

#if defined(FUNC_SEG_LED_EN)|| defined(FUNC_TM1628_LED_EN)
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
				//DBG(("MUTE Exit\n"));
				gBlink.Blink.MuteBlink = 0;
				gBlink.Blink.MuteBlankScreen = 0;
				TimeOutSet(&DispTmr, 0);
//				// Todo here...
			}
			else if(!gBlink.Blink.MuteBlink)
			{
				//DBG(("MUTE Enter\n"));
				gBlink.Blink.MuteBlink = 1;
				gBlink.Blink.MuteBlankScreen = 1;
				ClearDigit();
				TimeOutSet(&DispTmr, MUTE_BLINK_INTERVAL);
			}
		}
		else
		{
			gBlink.Blink.MuteBlink = 0;
			gBlink.Blink.MuteBlankScreen = 0;
			TimeOutSet(&DispTmr, 0);
			// Todo here...
		}
	}
#endif
}


//// 换台、存台时调用.
//void DispStationNum(void)
//{
//
//}


// Show Device symbol.
void DispDevSymbol(void)
{
#if defined(FUNC_SEG_LED_EN)|| defined(FUNC_TM1628_LED_EN)
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
	ClearScreen();
	DispString(" PC ");
#endif
}
#ifdef FUNC_MULTI_UDISK_PORT_EN
//显示UDiskPort number信息
void DispUDiskPortNum(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
#if 0
// 设备切换时调用.
void DispDev(void)
{
//	DBG((">>DispDev()\n"));

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if(gDispState == DISP_STATE_IDLE)
		{
			gDispState = DISP_STATE_DEV;
		}
		//DBG(("gDispState:%bd\n", gDispState));

		if(IsTimeOut(&DispTmr))
		{
			switch(gDispState)
			{
				case DISP_STATE_DEV:
					//DBG(("DISP_STATE_DEV\n"));
					DispDevSymbol();
					if((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD))
					{
						//if(gFsInitOK == 1)
						if(gPlayContrl->CurFileIndex)
						{
							gDispState = DISP_STATE_FILE;
						}
						else// if(gFsInitOK != 0)
						{
							gDispState = DISP_STATE_IDLE;
						}
					}
					else
					{
						gDispState = DISP_STATE_IDLE;
					}
					break;

				case DISP_STATE_FILE:
					//DBG(("DISP_STATE_FILE\n"));
					if((gSys.CurModuleID == MODULE_ID_PLAYER_USB) || (gSys.CurModuleID == MODULE_ID_PLAYER_SD))
					{
						//if(gFsInitOK == 1)
						if(gPlayContrl->CurFileIndex)
						{
							DispFileNum();
						}
						else
						{
							DispString(" FER");
						}
					}
					gDispState = DISP_STATE_IDLE;
					break;

				default:
					//DBG(("DISP_STATE_IDLE\n"));
					gDispState = DISP_STATE_IDLE;
					break;
			}

#ifdef FUNC_SEG_LCD_EN
			//DBG(("gDisplayMode:%bd\n", gDisplayMode));
			if(gDisplayMode == DISP_DEV_LCD58)
			{
				LcdFlushDispData();
				LcdFlushDisp();
			}
#endif
			//DBG(("%02BX, %02BX, %02BX, %02BX, %02BX\n", gDispBuff[0], gDispBuff[1], gDispBuff[2], gDispBuff[3], gDispBuff[3]));
		}
	}
#endif

//	DBG(("<<DispDev()\n"));
}
#endif

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
			DispIcon(ICON_MP3, LIGHT_OFF);
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


#if 0//def FUNC_RTC_EN
void DispRtc(void)
{
	RTC_DATE_TIME gDisRtcVal;

#ifdef FUNC_ALARM_EN
	if(RtcState == RTC_STATE_SET_ALARM)
	{
		gDisRtcVal = gAlarmTime;
	}
	else
#endif
	{
		gDisRtcVal = gRtcTime;
	}

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
		if(gDisplayMode == DISP_DEV_LED57)
		{
			DispIcon(ICON_PLAY, LIGHT_OFF);
			DispIcon(ICON_PAUSE, LIGHT_OFF);
			DispIcon(ICON_USB, LIGHT_OFF);
			DispIcon(ICON_SD, LIGHT_OFF);
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			DispIcon(ICON_MP3, LIGHT_OFF);
		}
		else if(gDisplayMode == DISP_DEV_LED8888)
		{
			DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		}

#ifdef	AU6210K_CAR_MODEL_DEMO
		if(RtcState == RTC_STATE_SET_SOURCE)
		{
			switch(RtcSourceState)
			{
				case RTC_SOURCE_CLOCK:
					Temp = gRtcTime.Hour * 100;
					Temp += gRtcTime.Min;
					Num2Char(Temp, TRUE);
					DispIcon(ICON_SLEEP, LIGHT_OFF);
					if(gRtcTime.Sec % 2) //秒闪烁
					{
						DispIcon(ICON_COLON, LIGHT_ON);
					}
					else
					{
						DispIcon(ICON_COLON, LIGHT_OFF);
					}
					break;

				case RTC_SOURCE_YEAR:
					Temp = gRtcTime.Year;
					Num2Char(Temp, TRUE);
					DispIcon(ICON_COLON, LIGHT_OFF);
					break;

				case RTC_SOURCE_MON:
					Temp = gRtcTime.Mon * 100;
					Temp += gRtcTime.Date;
					Num2Char(Temp, TRUE);
					DispIcon(ICON_COLON, LIGHT_OFF);
					break;

				case RTC_SOURCE_ALARM:
					Temp = gAlarmTime.Hour * 100;
					Temp += gAlarmTime.Min;
					Num2Char(Temp, TRUE);
					DispIcon(ICON_COLON, LIGHT_ON);
					if(gRtcTime.Sec % 2) //秒闪烁
					{
						DispIcon(ICON_SLEEP, LIGHT_ON);
					}
					else
					{
						DispIcon(ICON_SLEEP, LIGHT_OFF);
					}
					break;

				case RTC_SOURCE_ONOFF:
					DispIcon(ICON_COLON, LIGHT_OFF);
					if(gRtcTime.Sec % 2) //秒闪烁
					{
						DispIcon(ICON_SLEEP, LIGHT_ON);
					}
					else
					{
						DispIcon(ICON_SLEEP, LIGHT_OFF);
					}
					if(gAlarmState == ALARM_STATUS_OPEN)
					{
						DispChar(' ', 0);
						DispChar('O', 1);
						DispChar('N', 2);
						DispChar(' ', 3);
					}
					else if(gAlarmState == ALARM_STATUS_CLOSE)
					{
						DispChar(' ', 0);
						DispChar('O', 1);
						DispChar('F', 2);
						DispChar('F', 3);
					}
					break;

				default:
					break;
			}
		}
		else
#endif
		{
			switch(RtcSubState)
			{
				case RTC_SET_NONE:
					Temp = gRtcTime.Hour * 100;
					Temp += gRtcTime.Min;
					Num2Char(Temp, TRUE);

#ifdef AU6210K_BOOMBOX_DEMO
					if(gAlarm1State == ALARM_STATUS_OPEN)
					{
						DispIcon(ICON_ALARM1, LIGHT_ON);
					}
					else
					{
						DispIcon(ICON_ALARM1, LIGHT_OFF);
					}

					if(gAlarm2State == ALARM_STATUS_OPEN)
					{
						DispIcon(ICON_ALARM2, LIGHT_ON);
					}
					else
					{
						DispIcon(ICON_ALARM2, LIGHT_OFF);
					}

					DispIcon(ICON_COLON, LIGHT_ON);

#else

					if(gRtcTime.Sec % 2)//秒闪烁
					{
						DispIcon(ICON_COLON, LIGHT_ON);
					}
					else
					{
						DispIcon(ICON_COLON, LIGHT_OFF);
					}
#endif
					break;

#ifdef FUNC_ALARM_EN
				case RTC_SET_ALARM_NUM:
					DispIcon(ICON_COLON, LIGHT_OFF);
					DispChar('A', 0);
					DispChar('L', 1);
					DispChar(' ', 2);

					if(RtcAutOutTimeCount % 2)
					{
						DispChar(RtcAlarmNum + 0x30, 3);
					}
					else
					{
						DispChar(' ', 3);
					}
					break;

				case RTC_SET_ALARM_MODE:
					DispIcon(ICON_COLON, LIGHT_OFF);
					DispChar('A', 0);
					DispChar('O', 1);
					DispChar(' ', 2);

					if(RtcAutOutTimeCount % 2)
					{
						DispChar(gAlarmMode + 0x30, 3);
					}
					else
					{
						DispChar(' ', 3);
					}
					break;

				case RTC_SET_WEEK:
					DispIcon(ICON_COLON, LIGHT_OFF);
					DispChar('A', 0);
					DispChar(' ', 1);
					DispChar(' ', 2);
					if(RtcAutOutTimeCount % 2)
					{
						DispChar((gDisRtcVal.WDay + 0x30), 3);
					}
					else
					{
						DispChar(' ', 3);
					}
					break;
#endif

				case RTC_SET_YEAR:
					DispIcon(ICON_COLON, LIGHT_OFF);
					if(gRtcRdWrFlag == SET_RTC_TIME_YEAR)
					{
						if(RtcAutOutTimeCount % 2)
						{
							Temp = gDisRtcVal.Year;
							Num2Char(Temp, TRUE);
						}
						else
						{
							DispChar(' ', 3);
							DispChar(' ', 2);
							DispChar(' ', 1);
							DispChar(' ', 0);
						}
					}
					else if(gRtcRdWrFlag == READ_RTC_TIME)//year->mon data->weekday->hour min
					{
						if(RtcAutOutTimeCount > 15)
						{
							Temp = gDisRtcVal.Year;
						}
						else if(RtcAutOutTimeCount > 10)
						{
							Temp = gDisRtcVal.Mon * 100;
							Temp += gDisRtcVal.Date;
						}
						else if(RtcAutOutTimeCount > 5)
						{
							Temp = gDisRtcVal.WDay;
						}
						else
						{
							Temp = gDisRtcVal.Hour * 100;
							Temp += gDisRtcVal.Min;
						}
						Num2Char(Temp, TRUE);
					}
					break;

				case RTC_SET_MON:
					DispIcon(ICON_COLON, LIGHT_OFF);
					if(RtcAutOutTimeCount % 2)
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

				case RTC_SET_DATE:
					DispIcon(ICON_COLON, LIGHT_OFF);
					if(RtcAutOutTimeCount % 2)
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

				case RTC_SET_HR:
					DispIcon(ICON_COLON, LIGHT_ON);
					if(RtcAutOutTimeCount % 2)
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
					if(RtcAutOutTimeCount % 2)
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

#ifdef	AU6210K_CAR_MODEL_DEMO
				case RTC_SET_ALARM_ONOFF:
					DispIcon(ICON_COLON, LIGHT_OFF);
					if(gAlarmState == ALARM_STATUS_OPEN)
					{
						if(RtcAutOutTimeCount % 2)
						{
							DispChar(' ', 0);
							DispChar('O', 1);
							DispChar('N', 2);
							DispChar(' ', 3);
						}
						else
						{
							DispChar(' ', 3);
							DispChar(' ', 2);
							DispChar(' ', 1);
							DispChar(' ', 0);
						}
					}
					else if(gAlarmState == ALARM_STATUS_CLOSE)
					{
						if(RtcAutOutTimeCount % 2)
						{
							DispChar(' ', 0);
							DispChar('O', 1);
							DispChar('F', 2);
							DispChar('F', 3);
						}
						else
						{
							DispChar(' ', 3);
							DispChar(' ', 2);
							DispChar(' ', 1);
							DispChar(' ', 0);
						}
					}
					break;
#endif

#ifdef AU6210K_BOOMBOX_DEMO
				case RTC_SET_ALARM_SOURCE:
					if(gAlarmSource == RTC_ALARM_SOURCE_RADIO)
					{
						DispChar(' ', 0);
						DispChar('A', 1);
						DispChar('F', 2);
						DispChar(' ', 3);
					}
					else if(gAlarmSource == RTC_ALARM_SOURCE_USB)
					{
						DispChar(' ', 0);
						DispChar('A', 1);
						DispChar('U', 2);
						DispChar(' ', 3);
					}
					else if(gAlarmSource == RTC_ALARM_SOURCE_CARD)
					{
						DispChar(' ', 0);
						DispChar('A', 1);
						DispChar('D', 2);
						DispChar(' ', 3);
					}
					else if(gAlarmSource == RTC_ALARM_SOURCE_AUX)
					{
						DispChar(' ', 0);
						DispChar('A', 1);
						DispChar('A', 2);
						DispChar(' ', 3);
					}
					else if(gAlarmSource == RTC_ALARM_ONOFF)
					{
						DispChar(' ', 0);
						DispChar('O', 1);
						DispChar('F', 2);
						DispChar('F', 3);
					}
					DispIcon(ICON_COLON, LIGHT_OFF);
					break;

				case RTC_SET_ALARM_VOLUME:
					DispChar(' ', POS_COM_THOUSAND);
					DispChar('U', POS_COM_HUNDREDS);
					DispChar((gAlarmVolume / 10), POS_COM_TENS);
					DispChar((gAlarmVolume % 10), POS_COM_UNITS);
					DispIcon(ICON_COLON, LIGHT_OFF);
					break;
#endif

				default:
					break;
			}
		}
	}
#endif
}


//该 函数级别高主要处理RTC按键和判断闹钟是否到来
static uint8_t DispRtcSet(void)
{
	bool Ret = FALSE;

	if(RtcSubState == RTC_SET_NONE)
	{
#ifdef FUNC_ALARM_EN
		if(RtcAutOutTimeCount > 0)//闹钟到
		{
			Temp = gRtcTime.Hour * 100;//此时二者时间相等
			Temp += gRtcTime.Min;

#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
			if(gDisplayMode == DISP_DEV_LED57)
			{
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_MP3, LIGHT_OFF);
			}
			else if(gDisplayMode == DISP_DEV_LED8888)
			{
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
			}

			if(RtcAutOutTimeCount % 2)
			{
				Num2Char(Temp, TRUE);
			}
			else
			{
				ClearDigit();
			}

			Ret = TRUE;
#endif
		}
#endif
	}
	else
	{
		DispRtc();//设置时间或者闹钟
		Ret = TRUE;
	}

#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		LcdFlushDispData();
	}
#endif

	return Ret;
}
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
#if 0//(defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
 #if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN) || defined(FUNC_TM1628_LED_EN))
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
 #if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))    	
	DispChar('P', POS_COM_THOUSAND);
	DispChar((gSys.ProgramOffset / 10), POS_COM_HUNDREDS);
	DispChar((gSys.ProgramOffset % 10), POS_COM_TENS);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_PAUSE, LIGHT_OFF);
	DispIcon(ICON_PLAY, LIGHT_OFF);
	DispIcon(ICON_MP3, LIGHT_OFF);
	DispIcon(ICON_FOLDER, LIGHT_OFF);
	DispIcon(ICON_PROG, LIGHT_ON);
#ifdef FUNC_ZHIFENG_SOUNDBAR_DEMO_V1_0_EN
	DispIcon(ICON_DP, LIGHT_OFF);
#endif
	#ifdef FUNC_TM1628_LED_EN
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
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_TM1628_LED_EN))
  //增加单颗LED显扫描
	if((gDisplayMode <= DISP_DEV_LCD58) && (gDisplayMode >= DISP_DEV_LED1888))
	{
#ifdef FUNC_SPEC_TRUM_EN
		DispSpecTrum();
#endif
#ifdef FUNC_SOUND_REMIND
		if(gSys.SoundReminding)
		{
			return;
		}
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
				default:
					break;
			}
#ifdef FUNC_TM1628_LED_EN
			TM1628_FlushDispData();
#endif
			return;
		}
		if(gDispMsgId == DISP_MSG_MVOL)
		{
			AudioSysInfoSetBreakPoint();
		}
		gDispMsgId = DISP_MSG_NONE;
		if(gSys.MuteFlag == TRUE)
		{
			if(IsTimeOut(&DispTmr))
			{
				if(gBlink.BlinkFlag)
				{
					TimeOutSet(&DispTmr, MUTE_BLINK_INTERVAL);
					if(gBlink.Blink.MuteBlink)		// MUTE闪烁
					{
						if(gBlink.Blink.MuteBlankScreen)
						{
							gBlink.Blink.MuteBlankScreen = 0;
							DispResume();
						}
						else
						{
							gBlink.Blink.MuteBlankScreen = 1;
							ClearDigit();	//清数值显示
						}
					}
				}
			}
		}
		else
		{
			DispResume();
		}
#ifdef FUNC_TM1628_LED_EN
		TM1628_FlushDispData();
#endif
	}
#ifdef FUNC_SEG_LCD_EN
	if(gDisplayMode == DISP_DEV_LCD58)
	{//为解决显示音量、EQ等状态时没有幅度显示的问题，增加以下代码。
		if(IsTimeOut(&gAmplitudeTmr))
		{
			LcdFlushDispData();//在LcdFlushDispData函数内部调用幅度显示函数。
		}
	}
#endif
#endif
	
#ifdef FUNC_SINGLE_LED_EN
	if(gDisplayMode == DISP_DEV_SLED)
	{
		SLedDisplay();
	}
#endif
}


#endif
