#include "type.h"
#include "app_config.h"
#ifdef FUNC_SEG_LCD_EN
#include "folder.h"
#include "audio_decoder.h"
#include "fat_file.h"
#include "player_control.h"
#include "radio_control.h"
#include "line_in_control.h"
#include "led_display.h"
#include "seg_panel.h"
#include "seg_lcd_disp.h"
//#include "seg_led_disp.h"
//#include "sled_disp.h"
#include "lcd_seg.h" 
#include "sys_app.h"

TIMER  gAmplitudeTmr;
extern DISP_DEV gDisplayMode;

extern bool IsUDiskLink(void);
extern bool IsCardLink(void);
extern void ClearDigit(void);

#if defined(AU6210K_BOOMBOX_DEMO)
DISP_TYPE const LcdIconSet[ICON_NUM] = {0xFF, 0xFF, ICON58_COLON, ICON58_USB, ICON58_SD, ICON58_FM_MHZ, 0xFF, ICON58_REP, ICON58_RAN, ICON58_INTO, ICON58_MUTE, ICON58_L1, ICON58_L2, ICON58_L3, ICON58_L4, 0xFF, 0xFF, 0xFF, ICON58_STERO, ICON58_MEMORY, ICON58_ALARM1, ICON58_ALARM2};
#elif defined(FUNC_MS_RA1_PROJ_EN)
DISP_TYPE const LcdIconSet[ICON_NUM] = {ICON58_PLAY, ICON58_PAUSE, ICON58_COLON, ICON58_USB, ICON58_SD, ICON58_FM, ICON58_AM, ICON58_KHZ, ICON58_MHZ, ICON58_DOT, ICON58_ST, ICON58_REP, ICON58_1, ICON58_ALL, ICON58_FOLDER,ICON58_RAN};
#else
DISP_TYPE const LcdIconSet[ICON_NUM] = {0xFF, 0xFF, ICON58_COLON, ICON58_USB, ICON58_SD, ICON58_FM_MHZ, 0xFF, ICON58_REP, ICON58_RAN, ICON58_INTO, ICON58_MUTE, ICON58_L1, ICON58_L2, ICON58_L3, ICON58_L4};
#endif


#define HalfSecStatus()  (auxtmr_count_get()/1000%2)
// ÏÔÊ¾Ë¢ÐÂ.
// Lcd display refresh ISR.
void LcdFlushDisp(void)
{
	//static BYTE Count = 0;

	//Count ++;
	//if(Count == 2)	// 2   * 2ms = 2000us  2ms	 50
	#ifdef CHF_DEBUG_DISP_ERR
	if(!gSys.EraseFlagFlag)
	#endif
	{
		//Count = 0;
		LcdsegSetRefMem();
	}
}

extern DISP_TYPE gDispBuff[];
extern DISP_TYPE	gDispClearBuff[];

void LcdFlushDispData(void)
{	
	LcdsegSetData((uint8_t *)gDispBuff);
}

#ifdef FUNC_MS_RA1_PROJ_EN
void LcdClearFlushDisp()
{
   LcdsegSetData((uint8_t *)gDispClearBuff);
   LcdsegSetRefMem();
}
#endif

void DispLcdIcon(ICON_IDX Icon, ICON_STATU Light)
{
	DISP_TYPE IconVal;

    if(Icon >= ICON_NUM)
	{
		return;
	}
	IconVal = LcdIconSet[Icon];
	
	if(IconVal == 0xFF)
	{
		return;
	}
#if (MAX_SEG_NUM > 8)
	IconVal &= 0x1FFF;
#else
	IconVal = (1 << (IconVal % 8));
#endif
	//DBG(("LcdIcon,%bd,IconVal:%bx,%bd\n", LcdIconSet[Icon]/8, IconVal, Light));

	if(Light == LIGHT_ON)
	{
#if (MAX_SEG_NUM > 8)
        
		gDispBuff[LcdIconSet[Icon] >> 13] |= (DISP_TYPE)IconVal;
#else
		gDispBuff[LcdIconSet[Icon] / 8] |= (DISP_TYPE)IconVal;
#endif
	}
	else if(Light == LIGHT_OFF)
	{
#if (MAX_SEG_NUM > 8)
		gDispBuff[LcdIconSet[Icon] >> 13] &= (DISP_TYPE)(~IconVal);
#else
		gDispBuff[LcdIconSet[Icon] / 8] &= (DISP_TYPE)(~IconVal);
#endif
	}
#if 0	
	else if(Light == LIGHT_BLINK)
	{
//		if((gDisplayMode != DISP_DEV_NONE) && (gDisplayMode != DISP_DEV_LCD58))
//		{
		if(HalfSecStatus())
		{
#if (MAX_SEG_NUM > 8)
			gDispBuff[LcdIconSet[Icon] >> 13] |= (DISP_TYPE)IconVal;
#else
			gDispBuff[LcdIconSet[Icon] / 8] |= (DISP_TYPE)IconVal;
#endif
		}
		else
		{
#if (MAX_SEG_NUM > 8)
			gDispBuff[LcdIconSet[Icon] >> 13] |= (DISP_TYPE)IconVal;
#else
			gDispBuff[LcdIconSet[Icon] / 8] &= (DISP_TYPE)(~IconVal);
#endif
		}
//		}
//		else if(gDisplayMode == DISP_DEV_LCD58)
//		{
////			LcdBlinkCtrl(5, LcdIconSet[Icon] % 8, LcdIconSet[Icon] / 8);
//			//// 0 -> B0, 1-> B1... 7->B7, 8 -> D2, 9 -> D3 ... 13 -> D7
//			//BYTE CODE LcdSegChanNo[MAX_SEG_NUM]={6, 5, 3, 2, 1, 0, 4, 7};
//			//LcdBlinkCtrl(5, LcdSegChanNo[LcdIconSet[Icon]%MAX_SEG_NUM], LcdIconSet[Icon]/MAX_COM_NUM);
//		}
	}
	#endif
}

//ÆµÆ×ÏÔÊ¾
void LcdAmplitudeCurve(AMPL_CURVE Ampl)
{
	switch(Ampl)
	{
		case Ampl_Zero:
			DispIcon(ICON_L1, LIGHT_OFF);
			DispIcon(ICON_L2, LIGHT_OFF);
			DispIcon(ICON_L3, LIGHT_OFF);
			DispIcon(ICON_L4, LIGHT_OFF);
			break;
	
		case Ampl_One:
			DispIcon(ICON_L1, LIGHT_ON);
			DispIcon(ICON_L2, LIGHT_OFF);
			DispIcon(ICON_L3, LIGHT_OFF);
			DispIcon(ICON_L4, LIGHT_OFF);
			break;
	
		case Ampl_Two:
			DispIcon(ICON_L1, LIGHT_ON);
			DispIcon(ICON_L2, LIGHT_ON);
			DispIcon(ICON_L3, LIGHT_OFF);
			DispIcon(ICON_L4, LIGHT_OFF);
			break;
		case Ampl_Three:
			DispIcon(ICON_L1, LIGHT_ON);
			DispIcon(ICON_L2, LIGHT_ON);
			DispIcon(ICON_L3, LIGHT_ON);
			DispIcon(ICON_L4, LIGHT_OFF);
			break;
	
		case Ampl_Four:
			DispIcon(ICON_L1, LIGHT_ON);
			DispIcon(ICON_L2, LIGHT_ON);
			DispIcon(ICON_L3, LIGHT_ON);
			DispIcon(ICON_L4, LIGHT_ON);
			break;
	
		default:
			break;
	}		
}

void LcdDispAmplCurve(void)
{
#if 0
	if(gSys.MuteFg == TRUE)
	{
		LcdAmplitudeCurve(Ampl_Zero);
	}
	else if((((gSys.SystemMode == SYS_MODE_USB) || (gSys.SystemMode == SYS_MODE_SD)) && (gPlayCtrl.State == PLAY_STATE_PLAY))
#ifdef FUNC_AUDIO_EN
		|| ((gSys.SystemMode == SYS_MODE_AUDIO) && (gAudioState == AUDIO_PLAY))
#endif
#ifdef FUNC_AUDIO_READER_EN
		|| ((gSys.SystemMode == SYS_MODE_AUDIO_READER) && (gAudioState == AUDIO_PLAY))
#endif
	)
	{
		if(IsTimeOut(&gAmplitudeTmr))
		{
			//DBG(("LcdAmplitudeCurve,11\n"));
			LcdAmplitudeCurve(GetPCMAmplitude());
			TimeOutSet(&gAmplitudeTmr, AMPLITUDE_INTERVAL);	
		}		
	}
//	else if(((gSys.SystemMode == SYS_MODE_RADIO) && (gRadioCtrl.State == RADIO_IDLE))
//		|| (gSys.SystemMode == SYS_MODE_LINEIN)
//	)
//	{
//		if(IsTimeOut(&gAmplitudeTmr))
//		{
//			//DBG(("LcdAmplitudeCurve,22\n"));
//			///////////////LcdAmplitudeCurve(GetPCMAmplitude());
//			TimeOutSet(&gAmplitudeTmr, AMPLITUDE_INTERVAL);	
//		}
//	}
#endif
}

void LcdDispDevSymbol(void)
{
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		switch(gSys.CurModuleID)
		{
#ifdef IDLE_MODULE		
			case MODULE_ID_IDLE:
#ifndef FUNC_RTC_EN	
				//DBG(("NOD2"));
				DispString(" NOD");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);				
#else
				DispRtc();
#endif
				break;
#endif
#ifdef FUNC_USB_EN
			case MODULE_ID_PLAYER_USB:
				APP_DBG("MODULE_ID_PLAYER_USB------\n");
				DispString(" LOD");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_ON);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				#ifdef FUNC_MS_RA1_PROJ_EN
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_AM, LIGHT_OFF);
				DispIcon(ICON_KHZ, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
				DispIcon(ICON_ST, LIGHT_OFF);
				DispIcon(ICON_1, LIGHT_OFF);
				DispIcon(ICON_ALL, LIGHT_OFF);
				DispIcon(ICON_FOLDER, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_DOT, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_ALL, LIGHT_OFF);
				DispIcon(ICON_1, LIGHT_OFF);
				DispIcon(ICON_FOLDER, LIGHT_OFF);
				DispIcon(ICON_RAN, LIGHT_OFF);							
				#endif
				break;
#endif

#ifdef FUNC_CARD_EN
			case MODULE_ID_PLAYER_SD:
				DispString(" LOD");
				APP_DBG("MODULE_ID_PLAYER_USB------\n");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_ON);
				DispIcon(ICON_COLON, LIGHT_OFF);
				#ifdef FUNC_MS_RA1_PROJ_EN
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_AM, LIGHT_OFF);
				DispIcon(ICON_KHZ, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
				DispIcon(ICON_ST, LIGHT_OFF);
				DispIcon(ICON_1, LIGHT_OFF);
				DispIcon(ICON_ALL, LIGHT_OFF);
				DispIcon(ICON_FOLDER, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_DOT, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_ALL, LIGHT_OFF);
				DispIcon(ICON_1, LIGHT_OFF);
				DispIcon(ICON_FOLDER, LIGHT_OFF);
				DispIcon(ICON_RAN, LIGHT_OFF);			
				#endif
				break;
#endif

	        case MODULE_ID_BLUETOOTH:
				DispString(" LOD");
				APP_DBG("MODULE_ID_BLUETOOTH------\n");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
//				DispRtc();
				break;
				
#ifdef FUNC_LINEIN_EN
			case MODULE_ID_LINEIN:
				DispString(" AUX");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				#ifdef FUNC_MS_RA1_PROJ_EN
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_AM, LIGHT_OFF);
				DispIcon(ICON_KHZ, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
				DispIcon(ICON_ST, LIGHT_OFF);
				DispIcon(ICON_1, LIGHT_OFF);
				DispIcon(ICON_ALL, LIGHT_OFF);
				DispIcon(ICON_FOLDER, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_DOT, LIGHT_OFF);
				#endif

				break;
#endif

#ifdef FUNC_RADIO_EN
			case MODULE_ID_RADIO:
				if(!sRadioControl)
				{
					break;
				}
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_ON);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				#ifdef FUNC_MS_RA1_PROJ_EN				
				DispIcon(ICON_ST, LIGHT_OFF);
				DispIcon(ICON_1, LIGHT_OFF);
				DispIcon(ICON_ALL, LIGHT_OFF);
				DispIcon(ICON_FOLDER, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);				
				#endif

				#ifdef FUNC_AM_EN
				if(sRadioControl->CurBand == RADIO_BAND_AM)
				{
					if(sRadioControl->AmFreq == 0)
					{
						sRadioControl->AmFreq = 522;//RADIO_LOWER_BOUND;
					}		
					DispChar(' ', 4);					
					Num2CharForFreq(sRadioControl->AmFreq, FALSE,1);
					#ifdef FUNC_MS_RA1_PROJ_EN			
					DispIcon(ICON_AM, LIGHT_ON);
					DispIcon(ICON_KHZ, LIGHT_ON);	
					DispIcon(ICON_DOT, LIGHT_OFF);
					#endif
				}
				else
				#endif
				{
					if(sRadioControl->Freq == 0)
					{
						sRadioControl->Freq = 8750;//RADIO_LOWER_BOUND;
					}					
					DispChar(' ', 4);					
					Num2CharForFreq(sRadioControl->Freq, FALSE,0);		
					#ifdef FUNC_MS_RA1_PROJ_EN			
					DispIcon(ICON_FM, LIGHT_ON);
					DispIcon(ICON_MHZ, LIGHT_ON);
					DispIcon(ICON_DOT, LIGHT_ON);
					#endif
				}
				break;
#endif

#ifdef FUNC_AUDIO_EN
			case SYS_MODE_AUDIO:
				DispString(" PC ");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#ifdef FUNC_READER_EN
			case SYS_MODE_READER:
				DispString(" REA");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#ifdef FUNC_AUDIO_READER_EN
			case SYS_MODE_AUDIO_READER:
				DispString(" PC ");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#ifdef FUNC_RTC_EN
			case MODULE_ID_RTC:
				DispString("TIME");
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif
		
			default:
				ClearDigit();
				LcdAmplitudeCurve(Ampl_Zero);
				DispIcon(ICON_RAN, LIGHT_OFF);
				DispIcon(ICON_INTO, LIGHT_OFF);
				DispIcon(ICON_REP, LIGHT_OFF);
				DispIcon(ICON_MUTE, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				#ifdef FUNC_MS_RA1_PROJ_EN
				DispIcon(ICON_FM, LIGHT_OFF);
				DispIcon(ICON_AM, LIGHT_OFF);
				DispIcon(ICON_KHZ, LIGHT_OFF);
				DispIcon(ICON_MHZ, LIGHT_OFF);
				DispIcon(ICON_ST, LIGHT_OFF);
				DispIcon(ICON_1, LIGHT_OFF);
				DispIcon(ICON_ALL, LIGHT_OFF);
				DispIcon(ICON_FOLDER, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_DOT, LIGHT_OFF);
				#endif

				break;
		}
	}
}

void LcdDispRepeat(void)
{
}

extern uint8_t RtcAutOutTimeCount;

void LcdDispPlayState(void)
{
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))
	if(gDisplayMode == DISP_DEV_LCD58)
	{
		if((gPlayContrl->State != PLAYER_STATE_PLAYING) || (gSys.MuteFlag == TRUE))
		{
			LcdAmplitudeCurve(Ampl_Zero);
		}
		if(gPlayContrl->State == PLAYER_STATE_PAUSE)
		{
			DispIcon(ICON_PAUSE, LIGHT_ON);
			DispIcon(ICON_PLAY, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
			//if(RtcAutOutTimeCount%2)
			DispString("     ");
			//else
			//DispString("    ");		
			//DispPlayTime(DIS_PLAYTIME_PAUSE);
		}
		else if(gPlayContrl->State == PLAYER_STATE_IDLE)
		{
			DispIcon(ICON_PAUSE, LIGHT_OFF);
			DispIcon(ICON_PLAY, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
			DispIcon(ICON_REP, LIGHT_OFF);
			DispIcon(ICON_ALL, LIGHT_OFF);
			DispIcon(ICON_1, LIGHT_OFF);
			DispIcon(ICON_FOLDER, LIGHT_OFF);
			DispIcon(ICON_RAN, LIGHT_OFF);
			if(
#ifdef FUNC_USB_EN
				((gSys.CurModuleID== MODULE_ID_PLAYER_USB) && IsUDiskLink()) 
#endif
#ifdef FUNC_CARD_EN
				|| ((gSys.CurModuleID == MODULE_ID_PLAYER_SD) && IsCardLink())
#endif
			)
			{
			    if(gPlayContrl->TotalFileNumInDisk)
		    	{
					DispChar(' ', 0);		
					DispChar(' ', 1);		
					DispChar((gPlayContrl->TotalFileNumInDisk/100)%10, 2);
					DispChar((gPlayContrl->TotalFileNumInDisk/10)%10, 3);
					DispChar((gPlayContrl->TotalFileNumInDisk)%10, 4);
		    	}
			}
		}
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	}
#endif
}
#endif
