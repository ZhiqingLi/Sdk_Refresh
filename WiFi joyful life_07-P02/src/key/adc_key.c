////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:adc_key_driver.c
//
//            maintainer: Halley
//
//		Description	:
//					adc keyboard driver
//
//		Changelog	:
//					2013-05-31	Created Source File
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "adc.h"
#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "sys_app.h"
#include "app_msg.h"


#ifdef FUNC_ADC_KEY_EN

#define 	ADC_KEY_SCAN_TIME			10
#define 	ADC_KEY_JITTER_TIME			50
#define 	ADC_KEY_CP_TIME				2000
#define 	ADC_KEY_CPH_TIME			200
#define		ADC_KEY_COUNT				10 			//key count per adc channel
#define		ADC_KEY_FULL_VAL			4096		//

static const uint16_t AdcKeyEvent[][5] = 
{
//	PDS(按键开始)		SPR(短按松开)		  CPS(长按开始)		   CPH(长按保持)	  CPR(长按松开)
	{MSG_NONE,			MSG_WIFI_TALK,      MSG_WIFI_MIC, 	   MSG_NONE,        MSG_NONE		},	//SW1
	{MSG_NONE,			MSG_WIFI_MIC, 	    MSG_WIFI_WPS, 		 MSG_WIFI_WPS,    MSG_NONE		},	//SW2
	{MSG_NONE,			MSG_MODE,           MSG_NONE,       	 MSG_NONE, 	      MSG_NONE	  },	//SW3
	{MSG_NONE,			MSG_VOL_DW,         MSG_PRE, 	         MSG_NONE, 		    MSG_NONE  	},	//SW4
	{MSG_NONE,			MSG_VOL_UP, 			  MSG_NEXT, 	       MSG_NONE, 		    MSG_NONE  	},	//SW5
																						
	{MSG_NONE,			MSG_PLAY_PAUSE, 		MSG_MODE, 	       MSG_NONE,        MSG_NONE  	},	//SW4
	{MSG_NONE,			MSG_WIFI_WPS, 		  MSG_NONE,       	 MSG_NONE,        MSG_NONE		},	//SW6
	{MSG_NONE,			MSG_MUTE, 			    MSG_VB, 			     MSG_NONE, 			  MSG_NONE		},	//SW7
	{MSG_NONE,			MSG_REC, 			      MSG_REC_PLAYBACK,  MSG_NONE, 			  MSG_NONE		},	//SW8
	{MSG_NONE,			MSG_MODE, 			    MSG_POWER, 			   MSG_NONE, 			  MSG_NONE		},	//SW9
	{MSG_NONE,			MSG_NONE, 			    MSG_NONE, 			   MSG_NONE, 			  MSG_NONE		},  //无实际按键
	
	{MSG_NONE,			MSG_MENU, 			    MSG_BROWSE,			   MSG_NONE, 			  MSG_NONE		},	//SW10
	{MSG_NONE,			MSG_FOLDER_PRE, 	  MSG_NONE, 			   MSG_NONE, 			  MSG_NONE		},	//SW11
	{MSG_NONE,			MSG_FOLDER_NEXT,	  MSG_NONE,			     MSG_NONE, 			  MSG_NONE		},	//SW12
	{MSG_NONE,			MSG_REPEAT,			    MSG_REPEAT_AB, 		 MSG_NONE, 			  MSG_NONE		},	//SW13
	{MSG_NONE,			MSG_REMIND,			    MSG_LANG, 			   MSG_NONE, 			  MSG_NONE		},	//SW14

	{MSG_NONE,			MSG_DRC,			      MSG_NONE, 			   MSG_NONE,			  MSG_NONE		},	//SW15
	{MSG_NONE,			MSG_NONE,			      MSG_NONE, 		   	 MSG_NONE,			  MSG_NONE		},	//SW16
	{MSG_NONE,			MSG_RTC_SET_TIME,   MSG_NONE, 			   MSG_NONE, 			  MSG_NONE		},	//SW17
	{MSG_NONE,			MSG_RTC_SET_ALARM,  MSG_NONE, 			   MSG_NONE, 			  MSG_NONE		},	//SW18
	{MSG_NONE,			MSG_NONE, 			    MSG_NONE, 			   MSG_NONE, 			  MSG_NONE		},	//无实际按键
};	

#ifdef FUNC_AUDIO_CHANNEL_SWITCH_EN
static const uint16_t AudioChannelNumberEvent[] = 
{
	MSG_NUM_0,	
	MSG_NUM_1,	
	MSG_NUM_2,	
	MSG_NUM_3,	
	MSG_NUM_4,	
	MSG_NUM_5,	
	MSG_NUM_6,	
	MSG_NUM_7,	
	MSG_NUM_8,	
	MSG_NUM_9,
};	
#endif

typedef enum _ADC_KEY_STATE
{
    ADC_KEY_STATE_IDLE,
    ADC_KEY_STATE_JITTER,
    ADC_KEY_STATE_PRESS_DOWN,
    ADC_KEY_STATE_CP

} ADC_KEY_STATE;


TIMER			AdcKeyWaitTimer;
TIMER			AdcKeyScanTimer;
TIMER			AudioChannelJitterTimer;
ADC_KEY_STATE	AdcKeyState;
uint8_t  AudioChannelIndex;

//获取ADK状态
uint16_t GetAdcKeyState(void)
{
	return AdcKeyState;
}

// Key process, image key value to key event.
static uint8_t AdcChannelKeyGet(uint8_t Channel)
{
	uint16_t	Val;
	uint8_t	KeyIndex;

	Val = SarAdcChannelGetValue(Channel);
	
//	if(Val >= (ADC_KEY_FULL_VAL - (ADC_KEY_FULL_VAL / ADC_KEY_COUNT) / 2))
	if(Val >= (ADC_KEY_FULL_VAL - (ADC_KEY_FULL_VAL / ADC_KEY_COUNT)))
	{
		return 0xFF;	//no pressed key.
	}
	//APP_DBG("ADC KEY = %d\n", Val);
	KeyIndex = (Val + (ADC_KEY_FULL_VAL / ADC_KEY_COUNT) / 2) * ADC_KEY_COUNT / ADC_KEY_FULL_VAL;
	return KeyIndex;
}

#ifdef FUNC_AUDIO_CHANNEL_SWITCH_EN
//输入信号切换
void AudioChannelDetect(void)
{
	static uint8_t TempChannelIndex,PrevChannelIndex;
	 
	if(gSys.CurModuleID == MODULE_ID_IDLE)
	{
		//APP_DBG("System no power on!!!!!!!!\n");
		return;
	}
	
	TempChannelIndex = AdcChannelKeyGet(ADC_CHANNEL_DETECT_PORT);
	if(AudioChannelIndex != TempChannelIndex)
	{
		if(PrevChannelIndex == TempChannelIndex)
		{
			if(IsTimeOut(&AudioChannelJitterTimer))
			{
				AudioChannelIndex = PrevChannelIndex;
			}
		}
		else
		{
			TimeOutSet(&AudioChannelJitterTimer, 500);
			PrevChannelIndex = TempChannelIndex;
		}
		return;
	}
	
	if(gSys.NextAudioChannel != AudioChannelNumberEvent[AudioChannelIndex])
	{
		gSys.NextAudioChannel = AudioChannelNumberEvent[AudioChannelIndex];
		if(gSys.NextAudioChannel == MSG_NUM_0)
		{
			gSys.NextModuleID = MODULE_ID_WIFI;
			MsgSend(MSG_COMMON_CLOSE);
		}
		else
		{
			gSys.NextModuleID = MODULE_ID_LINEIN;
			MsgSend(MSG_COMMON_CLOSE);
		}
		//通道切换到WiFi，播放模式切换提示音。
		gSys.CurAudioChannel = MSG_NUM_0;	
	}
}	
#endif

static uint8_t GetAdcKeyIndex(void)
{
	uint8_t KeyIndex = 0xFF;

#ifdef FUNC_AUDIO_CHANNEL_SWITCH_EN
	AudioChannelDetect();
#endif	
	
#ifdef ADC_KEY_PORT_CH1
	KeyIndex = AdcChannelKeyGet(ADC_KEY_PORT_CH1);
#endif

#ifdef ADC_KEY_PORT_CH2
	if(KeyIndex == 0xFF)
	{
		KeyIndex = AdcChannelKeyGet(ADC_KEY_PORT_CH2);
#ifdef ADC_KEY_PORT_CH1
		if(KeyIndex != 0xFF)
		{
			KeyIndex += ADC_KEY_COUNT;
		}
#endif
	}
#endif

	return KeyIndex;
}


// adc key scan
// return: 0---no key, else---key msg
uint16_t AdcKeyScan(void)
{
	static 	uint8_t 	PreKeyIndex = 0xFF;
	uint8_t				KeyIndex;

	if(!IsTimeOut(&AdcKeyScanTimer))
	{
		return MSG_NONE;
	}
	TimeOutSet(&AdcKeyScanTimer, ADC_KEY_SCAN_TIME);

	KeyIndex = GetAdcKeyIndex();
	
	switch(AdcKeyState)
	{
		case ADC_KEY_STATE_IDLE:
			if(KeyIndex == 0xFF)
			{
				return MSG_NONE;
			}

			PreKeyIndex = KeyIndex;
			TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_JITTER_TIME);
			//DBG("GOTO JITTER!\n");
			AdcKeyState = ADC_KEY_STATE_JITTER;

		case ADC_KEY_STATE_JITTER:
			if(PreKeyIndex != KeyIndex)
			{
				//DBG("GOTO IDLE Because jitter!\n");
				AdcKeyState = ADC_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//DBG("GOTO PRESS_DOWN!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CP_TIME);
				AdcKeyState = ADC_KEY_STATE_PRESS_DOWN;
				return AdcKeyEvent[PreKeyIndex][0];
			}
			break;

		case ADC_KEY_STATE_PRESS_DOWN:
			if(PreKeyIndex != KeyIndex)
			{
				//return key sp value
				//DBG("ADC KEY SP%bu\n", PreKeyIndex);
				AdcKeyState = ADC_KEY_STATE_IDLE;
				return AdcKeyEvent[PreKeyIndex][1];
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//return key cp value
				//DBG("ADC KEY CPS!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);
				AdcKeyState = ADC_KEY_STATE_CP;
				return AdcKeyEvent[PreKeyIndex][2];
			}
			break;

		case ADC_KEY_STATE_CP:
			if(PreKeyIndex != KeyIndex)
			{
				//return key cp value
				//DBG("ADC KEY CPR!\n");
				AdcKeyState = ADC_KEY_STATE_IDLE;
				return AdcKeyEvent[PreKeyIndex][4];
			}
			else if(IsTimeOut(&AdcKeyWaitTimer))
			{
				//return key cph value
				//DBG("ADC KEY CPH!\n");
				TimeOutSet(&AdcKeyWaitTimer, ADC_KEY_CPH_TIME);
				return AdcKeyEvent[PreKeyIndex][3];
			}
			break;

		default:
			AdcKeyState = ADC_KEY_STATE_IDLE;
			break;
	}

	return MSG_NONE;
}

int32_t AdcKeyInit(void)
{
	DBG("AdcKeyInit()\n");

	AdcKeyState = ADC_KEY_STATE_IDLE;
	TimeOutSet(&AdcKeyScanTimer, 0);

#ifdef ADC_KEY_PORT_CH1
	SarAdcGpioSel(ADC_KEY_PORT_CH1);
#endif

#ifdef ADC_KEY_PORT_CH2
	SarAdcGpioSel(ADC_KEY_PORT_CH2);
#endif

#ifdef FUNC_AUDIO_CHANNEL_SWITCH_EN
	SarAdcGpioSel(ADC_CHANNEL_DETECT_PORT);
	TimeOutSet(&AudioChannelJitterTimer, 0);
#endif

	return 0;
}

#endif


