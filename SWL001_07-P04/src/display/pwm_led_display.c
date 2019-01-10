#ifndef __PWM_LED_DISPLAY__
#define __PWM_LED_DISPLAY__


#include "gpio.h"
#include "pwm.h"
#include "app_config.h"
#include "sys_vol.h"
#include "sys_app.h"
#include "fat_file.h"
#include "pwm_led_display.h"
#include "wifi_control.h"
#include "player_control.h"
#include "bt_control_api.h"
#include "wifi_uart_com.h"

#if (defined(FUNC_LIGHT_PWM_LED_EN) || defined(FUNC_ALEXA_PWM_LED_EN))
bool  IsPwmLedDisplayFlag = FALSE;
void PwmLedDisplayControlChange(PWM_LED_COLOUR Colour);

#ifdef FUNC_ALEXA_PWM_LED_EN
TIMER PwmLedSpecTrumTimer;
bool  IsPwmLedDisplayCartoonDone = FALSE;
PWM_LED_STATE PwmLedState[PWM_LED_SCAN_MAX_NUM];

//LED 引脚对应 GPIO 初始化
void PwmAlexaLedDisplayInit(void)
{
	TimeOutSet(&PwmLedSpecTrumTimer, 0);
	IsPwmLedDisplayCartoonDone = TRUE;
	memset(&PwmLedState, 0, sizeof(PwmLedState));
    //关闭PWM输出，防止开机时灯闪烁
    UXLED_ALL_MODE_OFF();
	PwmDisableChannel(UXLED_RED_PWM_PORT_MASK);
	PwmDisableChannel(UXLED_GREEN_PWM_PORT_MASK);
	PwmDisableChannel(UXLED_BLUE_PWM_PORT_MASK);

#ifdef PWM_LED_SCAN_DISP_EN
	GpioSetRegOneBit(LED_PIN1_PORT_PU, LED_PIN1_PORT_BIT);
	GpioSetRegOneBit(LED_PIN2_PORT_PU, LED_PIN2_PORT_BIT);
	GpioSetRegOneBit(LED_PIN3_PORT_PU, LED_PIN3_PORT_BIT);
	GpioSetRegOneBit(LED_PIN4_PORT_PU, LED_PIN4_PORT_BIT);
	GpioSetRegOneBit(LED_PIN5_PORT_PU, LED_PIN5_PORT_BIT);
	GpioSetRegOneBit(LED_PIN6_PORT_PU, LED_PIN6_PORT_BIT);

	GpioSetRegOneBit(LED_PIN1_PORT_PD, LED_PIN1_PORT_BIT);
	GpioSetRegOneBit(LED_PIN2_PORT_PD, LED_PIN2_PORT_BIT);
	GpioSetRegOneBit(LED_PIN3_PORT_PD, LED_PIN3_PORT_BIT);
	GpioSetRegOneBit(LED_PIN4_PORT_PD, LED_PIN4_PORT_BIT);
	GpioSetRegOneBit(LED_PIN5_PORT_PD, LED_PIN5_PORT_BIT);
	GpioSetRegOneBit(LED_PIN6_PORT_PD, LED_PIN6_PORT_BIT);

	GpioClrRegOneBit(LED_PIN1_PORT_IE, LED_PIN1_PORT_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_IE, LED_PIN2_PORT_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_IE, LED_PIN3_PORT_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_IE, LED_PIN4_PORT_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_IE, LED_PIN5_PORT_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_IE, LED_PIN6_PORT_BIT);

	GpioSetRegOneBit(LED_PIN1_PORT_OE, LED_PIN1_PORT_BIT);
	GpioSetRegOneBit(LED_PIN2_PORT_OE, LED_PIN2_PORT_BIT);
	GpioSetRegOneBit(LED_PIN3_PORT_OE, LED_PIN3_PORT_BIT);
	GpioSetRegOneBit(LED_PIN4_PORT_OE, LED_PIN4_PORT_BIT);
	GpioSetRegOneBit(LED_PIN5_PORT_OE, LED_PIN5_PORT_BIT);
	GpioSetRegOneBit(LED_PIN6_PORT_OE, LED_PIN6_PORT_BIT);

	GpioSetRegOneBit(LED_PIN1_PORT_OUTDS,LED_PIN1_PORT_BIT);
	GpioSetRegOneBit(LED_PIN2_PORT_OUTDS,LED_PIN2_PORT_BIT);
	GpioSetRegOneBit(LED_PIN3_PORT_OUTDS,LED_PIN3_PORT_BIT);
	GpioSetRegOneBit(LED_PIN4_PORT_OUTDS,LED_PIN4_PORT_BIT);
	GpioSetRegOneBit(LED_PIN5_PORT_OUTDS,LED_PIN5_PORT_BIT);
	GpioSetRegOneBit(LED_PIN6_PORT_OUTDS,LED_PIN6_PORT_BIT);

	GpioClrRegOneBit(LED_PIN1_PORT_OUT, LED_PIN1_PORT_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_OUT, LED_PIN2_PORT_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_OUT, LED_PIN3_PORT_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_OUT, LED_PIN4_PORT_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_OUT, LED_PIN5_PORT_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_OUT, LED_PIN6_PORT_BIT);

#endif
}

//LED 各个引脚使用前要先关闭恒流源
void LedAllPinGpioOutoff(void)
{
#ifdef PWM_LED_SCAN_DISP_EN
	GpioClrRegOneBit(LED_PIN1_PORT_OUT, LED_PIN1_PORT_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_OUT, LED_PIN2_PORT_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_OUT, LED_PIN3_PORT_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_OUT, LED_PIN4_PORT_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_OUT, LED_PIN5_PORT_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_OUT, LED_PIN6_PORT_BIT);
#endif
}

//根据相关工作状态，设置led         UI
void PwmLedDisplayControlSetting(void)
{
	static uint8_t PrevAvsState;
	static uint16_t LedBlinkCnt,CartoonCnt;
	uint8_t TempCnt;

	if(FALSE == IsPwmLedDisplayFlag)
	{
		memset(&PwmLedState, 0, sizeof(PwmLedState));
		return;
	}
	
	if(!IsTimeOut(&PwmLedSpecTrumTimer))
	{
		return;
	}
	
	LedBlinkCnt++;
	if(PrevAvsState != gWiFi.MicState)
	{
		PrevAvsState = gWiFi.MicState;
		IsPwmLedDisplayCartoonDone = TRUE;
		LedBlinkCnt = 0;
		CartoonCnt = 0;
	}
	
	if(gWiFi.TestModeState || (WiFiFirmwareUpgradeStateGet() == TRUE) || (WiFiFactoryStateGet() == TRUE))
	{
		TimeOutSet(&PwmLedSpecTrumTimer, 300);
		if(LedBlinkCnt == 1)
		{
			for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_RED;
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}
		}
		else if(LedBlinkCnt == 2)
		{
			for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_GREEN;
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}
		}
		else
		{
			for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}
			LedBlinkCnt = 0;
		}
		return;
	}

	//显示开机动画
	if(gWiFi.InitState == FALSE)
	{
		TimeOutSet(&PwmLedSpecTrumTimer, 100);

		CartoonCnt++;
		if(CartoonCnt >= PWM_LED_SCAN_MAX_NUM)
		{
			CartoonCnt = FALSE;
		}

		for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
		{
			PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_WHITE;
			PwmLedState[TempCnt].PwmLedDuty = 100;
		}

		PwmLedState[CartoonCnt].PwmLedColor = PWM_LED_COLOUR_ORANGE;
		if(CartoonCnt > 0)
		{
			PwmLedState[CartoonCnt-1].PwmLedColor = PWM_LED_COLOUR_ORANGE;
		}
		else
		{
			PwmLedState[PWM_LED_SCAN_MAX_NUM-1].PwmLedColor = PWM_LED_COLOUR_ORANGE;
		}
		return;
	}

	if((gWiFi.VisInfState == TRUE) || (gWiFi.MicState != WIFI_AVS_STATUS_IDLE))
	{
		if(gWiFi.MicState == WIFI_AVS_STATUS_LSN)
		{	
			TimeOutSet(&PwmLedSpecTrumTimer, 100);
			for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}
						
			if(LedBlinkCnt == 0)
			{
				PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_CYAN;
			}
			else if(LedBlinkCnt == 1)
			{
				PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_BLUE;
			}
			else if(LedBlinkCnt == 2)
			{
				PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_BLUE;
			}
			else if(IsPwmLedDisplayCartoonDone)
			{
				IsPwmLedDisplayCartoonDone = FALSE;
				CartoonCnt = (GetRandNum(5)-3);
				for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
				{
					if(TempCnt == CartoonCnt)
					{
						PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_CYAN;
					}
					else
					{
						PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_BLUE;
					}
				}
			}
		}
		else if(gWiFi.MicState == WIFI_AVS_STATUS_THK)
		{
			TimeOutSet(&PwmLedSpecTrumTimer, 150);

			for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}
						
			if(LedBlinkCnt%2 == TRUE)
			{
				PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_BLUE;
			}
			else
			{
				PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_BLUE;
				PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_CYAN;
			}				
		}
		else if(gWiFi.MicState == WIFI_AVS_STATUS_TTS)
		{
			TimeOutSet(&PwmLedSpecTrumTimer, 200);
			
			for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_IDLE;
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}

			LedBlinkCnt = LedBlinkCnt%4;
			if(LedBlinkCnt == 1)
			{
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_CYAN;
			}
			else if(LedBlinkCnt == 2)
			{
				PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_CYAN;
			}	
			else if(LedBlinkCnt == 3)
			{
				PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_CYAN;
				PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_CYAN;
			}
		}
		else if(gWiFi.MicState == WIFI_AVS_STATUS_ERR)
		{
			TimeOutSet(&PwmLedSpecTrumTimer, 100);
			if(LedBlinkCnt <= 50)
			{
				if(IsPwmLedDisplayCartoonDone)
				{
					CartoonCnt++;
					if(CartoonCnt >= 10)
					{
						IsPwmLedDisplayCartoonDone = FALSE;
					}
				}
				else
				{
					CartoonCnt--;
					if(CartoonCnt <= 1)
					{
						IsPwmLedDisplayCartoonDone = TRUE;
					}
					else if(CartoonCnt > 10)
					{
						CartoonCnt = 10;
					}
				}
				
				for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
				{
					PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_ORANGE;
					PwmLedState[TempCnt].PwmLedDuty = CartoonCnt*10;
				}
			}
			else
			{
				WiFiSetMicState(WIFI_AVS_STATUS_IDLE);
				LedBlinkCnt = 0;
			}
		}
		else if((gWiFi.VisInfState == TRUE) || (gWiFi.MicState == WIFI_AVS_STATUS_DIS))
		{		
			if(gWiFi.VisInfState == TRUE)
			{
				TimeOutSet(&PwmLedSpecTrumTimer, 100);
				if(LedBlinkCnt < 30)
				{
					if(IsPwmLedDisplayCartoonDone)
					{
						CartoonCnt++;
						if(CartoonCnt >= 10)
						{
							IsPwmLedDisplayCartoonDone = FALSE;
						}
					}
					else
					{
						CartoonCnt--;
						if(CartoonCnt <= 1)
						{
							IsPwmLedDisplayCartoonDone = TRUE;
						}
						else if(CartoonCnt > 10)
						{
							CartoonCnt = 10;
						}
					}

					for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
					{
						PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_YELLOW;
						PwmLedState[TempCnt].PwmLedDuty = CartoonCnt*10;
					}
				}
				
				if((gWiFi.MicState == WIFI_AVS_STATUS_DIS) && (LedBlinkCnt == 30))
				{
					TimeOutSet(&PwmLedSpecTrumTimer, 3000);
					for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
					{
						PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_RED;
						PwmLedState[TempCnt].PwmLedDuty = 100;
					}
				}

				if(LedBlinkCnt >= 31)
				{
					LedBlinkCnt = 0;
				}
			}
			else
			{
				for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
				{
					PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_RED;
					PwmLedState[TempCnt].PwmLedDuty = 100;
				}
			}
		}
		return;
	}

#ifdef FUNC_SLEEP_LEDOFF_EN
	if(gSys.SleepLedOffFlag)
	{
		memset(&PwmLedState, 0, sizeof(PwmLedState));
		return;
	}
#endif

	switch(gSys.CurModuleID)
	{
#ifdef FUNC_WIFI_EN
		case MODULE_ID_WIFI:
		case MODULE_ID_PLAYER_WIFI_USB:
		case MODULE_ID_PLAYER_WIFI_SD:
			if((WiFiWwwStateGet() != WIFI_STATUS_WWW_ENABLE) 
			&& (WiFiRa0StateGet() == WIFI_STATUS_AP_NO_CONNECTED))
			{
				TimeOutSet(&PwmLedSpecTrumTimer, 100);

				CartoonCnt++;
				if(CartoonCnt >= PWM_LED_SCAN_MAX_NUM)
				{
					CartoonCnt = FALSE;
				}
		
				memset(&PwmLedState, 0, sizeof(PwmLedState));

				PwmLedState[CartoonCnt].PwmLedColor = PWM_LED_COLOUR_WHITE;
				PwmLedState[CartoonCnt].PwmLedDuty = 100;
				if(CartoonCnt > 0)
				{
					PwmLedState[CartoonCnt-1].PwmLedColor = PWM_LED_COLOUR_WHITE;
					PwmLedState[CartoonCnt-1].PwmLedDuty = 100;
				}
				else
				{
					PwmLedState[PWM_LED_SCAN_MAX_NUM-1].PwmLedColor = PWM_LED_COLOUR_WHITE;
					PwmLedState[PWM_LED_SCAN_MAX_NUM-1].PwmLedDuty = 100;
				}
			}
			else
			{
				for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
				{
					PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_WHITE;
					PwmLedState[TempCnt].PwmLedDuty = 100;
				}
			}
			break;
#endif


#ifdef FUNC_LINEIN_EN
		case MODULE_ID_LINEIN:
			for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_PURPLE;
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}					
			break;
#endif

#ifdef FUNC_BT_EN
		case MODULE_ID_BLUETOOTH:
		case MODULE_ID_BT_HF:
			if(GetCurBtConnectedFlag() == FALSE)
			{
				TimeOutSet(&PwmLedSpecTrumTimer, 150);

				for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
				{
					PwmLedState[TempCnt].PwmLedDuty = 100;
				}
							
				if(LedBlinkCnt%2 == TRUE)
				{
					PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_RED;
					PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_BLUE;
					PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_RED;
					PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_BLUE;
					PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_RED;
					PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_BLUE;
				}
				else
				{
					PwmLedState[0].PwmLedColor = PWM_LED_COLOUR_BLUE;
					PwmLedState[1].PwmLedColor = PWM_LED_COLOUR_RED;
					PwmLedState[2].PwmLedColor = PWM_LED_COLOUR_BLUE;
					PwmLedState[3].PwmLedColor = PWM_LED_COLOUR_RED;
					PwmLedState[4].PwmLedColor = PWM_LED_COLOUR_BLUE;
					PwmLedState[5].PwmLedColor = PWM_LED_COLOUR_RED;
				}				
			}
			else
			{
				for(TempCnt = 0; TempCnt < PWM_LED_SCAN_MAX_NUM; TempCnt++)
				{
					PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_BLUE;
					PwmLedState[TempCnt].PwmLedDuty = 100;
				}
			}
			break;
#endif
        
		default:
			memset(&PwmLedState, 0, sizeof(PwmLedState));
			break;
	}
}

#endif

#ifdef FUNC_LIGHT_PWM_LED_EN
#define		ADC_KEY_COUNT				11 			//key count per adc channel
#define		ADC_KEY_FULL_VAL			4096		//
#define		MAX_BRIGHTNESS			    100 		//
#define		MIN_BRIGHTNESS			    15 			//

TIMER PwmLightSpecTrumTimer;
TIMER PwmLightControlTimer;
TIMER LightAutoControlTimer;
PWM_LED_COLOUR	PwmLightDisplayMode = PWM_LED_COLOUR_IDLE;
PWM_LED_DUTY 	PwmLightState[MAX_LIGHT_NUM];
uint8_t DutyBuf[3];
static uint8_t LightBrightness = MIN_BRIGHTNESS;

#ifdef FUNC_WIFI_ALEXA_PROJECT_EN
static uint8_t PwmSetDutyTemp[MAX_LIGHT_NUM];
#endif

static const uint8_t PwmColorTable[PWM_LED_COLOUR_COLORFUL][4] =
{
	{0,		0,		0,		0},								//空闲
	{0, 	0,		0,	  100}, 							//白
	{100,	0,		0,		0},								//红
	{100,	15,		0,		0}, 							//橙
	{100,	50,		0,		0}, 							//黄
	{0,		100,	0,		0}, 							//绿
	{0,		100,	100,	0},								//青
	{0,		0,		100,	0}, 							//蓝
	{100,	0,		100,	0}, 							//紫
};

#ifdef FUNC_WIFI_DUMI_PROJECT_EN
static const uint8_t SetSwitchCmdTable[14][12] =
{
	{0xE7,0xBA,0xA2,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},		//红色
    {0xE6,0xA9,0x99,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},      //橙色
    {0xE9,0xBB,0x84,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},      //黄色
    {0xE7,0xBB,0xBF,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},      //绿色
    {0xE9,0x9D,0x92,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},      //青色
    {0xE8,0x93,0x9D,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},      //蓝色
    {0xE7,0xB4,0xAB,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},      //紫色   
    {0xE9,0x9F,0xB3,0xE4,0xB9,0x90,0xE5,0xBE,0x8B,0xE5,0x8A,0xA8},      //音乐律动
    {0xE5,0x8A,0xA8,0xE8,0xB5,0xB7,0xE6,0x9D,0xA5,0x00,0x00,0x00},      //动起来   
    {0xE9,0x9A,0x8F,0xE9,0x9F,0xB3,0xE4,0xB9,0x90,0xE5,0x8F,0x98},      //随音乐变化
    {0xE8,0x8A,0x82,0xE8,0x83,0xBD,0xE6,0xA8,0xA1,0xE5,0xBC,0x8F},      //节能灯模式
    {0xE9,0x98,0x85,0xE8,0xAF,0xBB,0xE6,0xA8,0xA1,0xE5,0xBC,0x8F},      //阅读灯模式
    {0xE5,0xB0,0x8F,0xE5,0xA4,0x9C,0xE7,0x81,0xAF,0xE6,0xA8,0xA1},      //小夜灯模式
	{0xE7,0x99,0xBD,0xE8,0x89,0xB2,0x00,0x00,0x00,0x00,0x00,0x00},		//白色
};
#endif

/******************************************************************************/
//日光灯和七彩灯设置，IsPwmLedDisplayLightMode == TRUE 为日光灯
/******************************************************************************/
void PwmDisplayLightOnOff(void)
{
	if(PWM_LED_COLOUR_IDLE != PwmLightDisplayMode)
	{
		LightBrightness = MIN_BRIGHTNESS;
		PwmLightDisplayMode = PWM_LED_COLOUR_IDLE;
		Mcu_SendCmdToWiFi(MCU_IOTN_OFF, NULL);
		PwmLedDisplayControlChange(PwmLightDisplayMode);
	}
	else
	{
	}
}

/*****************************************************************************
 函 数 名  : PwmLightBrightnessSetting
 功能描述  : 按键调整LED亮度
 输入参数  : uint16_t Msg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年10月18日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void PwmLightBrightnessSetting(uint16_t Msg)
{
	if (MSG_LIGHT_SET_UP == Msg)
	{
	    LightBrightness += 10;
	    if(LightBrightness >= MAX_BRIGHTNESS)
	    {
	    	LightBrightness = MAX_BRIGHTNESS;
	    }
	}
	else if(MSG_LIGHT_SET_DW == Msg)
	{
		LightBrightness -= 10;
	    if((LightBrightness <= MIN_BRIGHTNESS) || (LightBrightness >= MAX_BRIGHTNESS))
	    {
	    	LightBrightness = MIN_BRIGHTNESS;
	    }
	}
	
	PwmLedDisplayControlChange(PwmLightDisplayMode);
	Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
}
/******************************************************************************/
//PWM显示LED颜色变化
/******************************************************************************/
void PwmLedDisplayControlChange(PWM_LED_COLOUR Colour)
{
	uint8_t Temp;
	
	if(PWM_LED_COLOUR_COLORFUL > Colour)
	{
		for (Temp = 0; Temp < MAX_LIGHT_NUM; ++Temp)
		{
			PwmLightState[Temp].PwmSetDuty = PwmColorTable[Colour][Temp]*LightBrightness/100;
#ifdef FUNC_WIFI_ALEXA_PROJECT_EN
			PwmSetDutyTemp[Temp] = PwmColorTable[Colour][Temp];
#endif
		}
	}
	else
	{
		for (Temp = 0; Temp < MAX_LIGHT_NUM; ++Temp)
		{
			PwmLightState[Temp].PwmSetDuty = 0;
#ifdef FUNC_WIFI_ALEXA_PROJECT_EN
			PwmSetDutyTemp[Temp] = 0;
#endif
		}
	}

	APP_DBG("Set LED colour = %d;\n", Colour);
}

/******************************************************************************/
//PWM显示LED显示颜色调整，由通用按键处理函数调用
/******************************************************************************/
void PwmLightDisplayControlSetting(void)
{
	if(PWM_LED_COLOUR_IDLE == PwmLightDisplayMode)
	{
		LightBrightness = MAX_BRIGHTNESS;
		PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
		Mcu_SendCmdToWiFi(MCU_IOTN__ON, NULL);
		WaitMs(10);
    	Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
		PwmLedDisplayControlChange(PwmLightDisplayMode);
	}
	else
	{    
		PwmLightDisplayMode++;
		LightBrightness = MAX_BRIGHTNESS;
		if(PwmLightDisplayMode >= PWM_LED_COLOUR_MAX)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;	
		}
		
		PwmLedDisplayControlChange(PwmLightDisplayMode);
		DutyBuf[0] = PwmLightState[0].PwmSetDuty*255/100;
		DutyBuf[1] = PwmLightState[1].PwmSetDuty*255/100;
		DutyBuf[2] = PwmLightState[2].PwmSetDuty*255/100;
		Mcu_SendCmdToWiFi(MCU_IOTN_COLOR, &DutyBuf[0]);
		WaitMs(10);
		Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
	}
}

#ifdef LIGHT_DETECT_AUTO_CONTROL_EN

/******************************************************************************/
//暖光LED检测当前环境亮度。
/******************************************************************************/
bool IsLightAdcDetect(void)
{
	uint16_t LightAdcValue;
	
	LightAdcValue = SarAdcChannelGetValue(LIGHT_DETECT_ADC_CHANNEL);
	
	if(LightAdcValue >= (ADC_KEY_FULL_VAL-(ADC_KEY_FULL_VAL/ADC_KEY_COUNT/4)))
	{
		APP_DBG("Get Light Adc Detect Value = %d\n", LightAdcValue);
		return TRUE;	//no pressed key.
	}

	return FALSE;
}

/******************************************************************************/
//暖光LED人体检测感应
/******************************************************************************/
bool LedGpioDetectScan(void)							
{
	static LIGHT_DETECT_STATE LedGpioDetectState = LIGHT_DETECT_STATE_IDLE;
	static TIMER LedGpioWaitTimer;
	uint8_t LedGpioInput = 0;

	LedGpioInput = (0
				#ifdef LIGHT_DETECT_GPIO1_MASK_BIT	
				   || !(GpioGetReg(LIGHT_DETECT_GPIO1_PORT_IN) & LIGHT_DETECT_GPIO1_MASK_BIT)
				#endif
				#ifdef LIGHT_DETECT_GPIO2_MASK_BIT
				   || !(GpioGetReg(LIGHT_DETECT_GPIO2_PORT_IN) & LIGHT_DETECT_GPIO2_MASK_BIT)
				#endif
				#ifdef LIGHT_DETECT_GPIO3_MASK_BIT
				   || !(GpioGetReg(LIGHT_DETECT_GPIO3_PORT_IN) & LIGHT_DETECT_GPIO3_MASK_BIT)
				#endif
				   );
	
	switch(LedGpioDetectState)
	{
		case LIGHT_DETECT_STATE_IDLE:
			if(LedGpioInput == TRUE)
			{
				TimeOutSet(&LedGpioWaitTimer, 50);
				LedGpioDetectState = LIGHT_DETECT_STATE_JITTER;
			}
			return FALSE;
			
		case LIGHT_DETECT_STATE_JITTER:
			if(LedGpioInput == FALSE)
			{
				LedGpioDetectState = LIGHT_DETECT_STATE_IDLE;
			}
			else if(IsTimeOut(&LedGpioWaitTimer))
			{
				LedGpioDetectState = LIGHT_DETECT_STATE_PRESS_DOWN;
				TimeOutSet(&LedGpioWaitTimer, 100);
				return TRUE;			//return key sp value
			}
			return FALSE;

		case LIGHT_DETECT_STATE_PRESS_DOWN:
			if(LedGpioInput == FALSE)
			{
				LedGpioDetectState = LIGHT_DETECT_STATE_PRESS_UP;
				TimeOutSet(&LedGpioWaitTimer, 500);
				APP_DBG("Light Gpio Detect Enter Off;\n");
			}
			else if(IsTimeOut(&LedGpioWaitTimer))
			{
				LedGpioDetectState = LIGHT_DETECT_STATE_PRESS_DOWN;
				TimeOutSet(&LedGpioWaitTimer, 100);
			}
			return TRUE;

		case LIGHT_DETECT_STATE_PRESS_UP:
			if((LedGpioInput == FALSE) && IsTimeOut(&LedGpioWaitTimer))
			{
				LedGpioDetectState = LIGHT_DETECT_STATE_IDLE;
				return FALSE;
			}
			else if(LedGpioInput == TRUE)
			{
				LedGpioDetectState = LIGHT_DETECT_STATE_PRESS_DOWN;
				TimeOutSet(&LedGpioWaitTimer, 100);
			}
			return TRUE;
			
		default:
			LedGpioDetectState = LIGHT_DETECT_STATE_IDLE;
			break;
	}

	return FALSE;
}

/******************************************************************************/
//暖光LED根据环境亮度及人物活动情况自动检测开关。
/******************************************************************************/
void LightLedDisplayAutoControl(void)
{
	static bool SetPwmLightFlag = FALSE,PrevLedGpioInput;
	bool LedGpioDetectstate;

	LedGpioDetectstate = LedGpioDetectScan();

	if(PrevLedGpioInput != LedGpioDetectstate)
	{
		PrevLedGpioInput = LedGpioDetectstate;
		APP_DBG("Get Light Gpio Detect Status = %x;\n", PrevLedGpioInput);
	}
	
	if(LedGpioDetectstate == TRUE)
	{
		if((FALSE == PwmLightState[3].PwmSetDuty) 
        && IsLightAdcDetect() && (FALSE == SetPwmLightFlag))
		{
			LightBrightness = MAX_BRIGHTNESS;
			PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
			Mcu_SendCmdToWiFi(MCU_IOTN__ON, NULL);
			WaitMs(10);
			Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
			PwmLedDisplayControlChange(PwmLightDisplayMode);
			SetPwmLightFlag = TRUE;
		}
	}
	else
	{
		if(SetPwmLightFlag == TRUE)
		{
			LightBrightness = MIN_BRIGHTNESS;
			PwmLightDisplayMode = PWM_LED_COLOUR_IDLE;
			Mcu_SendCmdToWiFi(MCU_IOTN_OFF, NULL);
			PwmLedDisplayControlChange(PwmLightDisplayMode);
			SetPwmLightFlag = FALSE;
		}	
	}
}
#endif
/*****************************************************************************
 函 数 名  : WiFi_CtrlPwmLight
 功能描述  : WiFi设置LED亮度及颜色
 输入参数  : uint8_t* Cmdbuf  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年10月9日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
#ifdef FUNC_WIFI_ALEXA_PROJECT_EN
void WiFi_CtrlPwmLight(uint8_t* Cmdbuf, bool IsPasCmd)
{
    uint8_t Temp = 0,Count = 0;

    if(memcmp(Cmdbuf, "+ON", 3) == 0)
	{
		PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
		LightBrightness = MAX_BRIGHTNESS;
		PwmLedDisplayControlChange(PwmLightDisplayMode);
		if(TRUE == IsPasCmd)
		{
			Mcu_SendCmdToWiFi(MCU_IOTN__ON, NULL);
		}
		WaitMs(10);
		Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
	}
	else if(memcmp(Cmdbuf, "+OFF", 4) == 0)
	{
		PwmLightDisplayMode = PWM_LED_COLOUR_IDLE;
		LightBrightness = 0;
		PwmLedDisplayControlChange(PwmLightDisplayMode);
		if(TRUE == IsPasCmd)
		{
			Mcu_SendCmdToWiFi(MCU_IOTN_OFF, NULL);
		}
	}
	else if(memcmp(Cmdbuf, "Color+", 6) == 0)
	{
		Temp = 6;
		for(Count = 0; Count < 3; Count++)
		{
			if(Cmdbuf[Temp] < 0x3a)
			{
				PwmLightState[Count].PwmSetDuty = (Cmdbuf[Temp]-0x30)*16;
			}
			else if(Cmdbuf[Temp] < 0x47)
			{
				PwmLightState[Count].PwmSetDuty = (Cmdbuf[Temp]-0x41+10)*16;
			}
			else
			{
				PwmLightState[Count].PwmSetDuty = (Cmdbuf[Temp]-0x61+10)*16;
			}
			Temp++;
			
			if(Cmdbuf[Temp] < 0x3a)
			{
				PwmLightState[Count].PwmSetDuty += (Cmdbuf[Temp]-0x30);
			}
			else if(Cmdbuf[Temp] < 0x47)
			{
				PwmLightState[Count].PwmSetDuty += (Cmdbuf[Temp]-0x41+10);
			}
			else
			{
				PwmLightState[Count].PwmSetDuty += (Cmdbuf[Temp]-0x61+10);
			}
			Temp++;
			
			PwmLightState[Count].PwmSetDuty = (PwmLightState[Count].PwmSetDuty*100)/255;
			//APP_DBG("PwmLightState[%d].PwmSetDuty = %d;\n", Count, PwmLightState[Count].PwmSetDuty);
			if(PwmLightState[Count].PwmSetDuty >= 100)
	        {
	            PwmLightState[Count].PwmSetDuty = 100;
	        }
		}
		
		PwmLightState[3].PwmSetDuty = 0;

		for (Temp = 0; Temp < MAX_LIGHT_NUM; ++Temp)
		{
			PwmSetDutyTemp[Temp] = PwmLightState[Temp].PwmSetDuty;
		}

		if(TRUE == IsPasCmd)
		{
			DutyBuf[0] = PwmSetDutyTemp[0]*255/100;
			DutyBuf[1] = PwmSetDutyTemp[1]*255/100;
			DutyBuf[2] = PwmSetDutyTemp[2]*255/100;
			Mcu_SendCmdToWiFi(MCU_IOTN_COLOR, &DutyBuf[0]);
		}
		WaitMs(10);
		Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
	}
    else if(memcmp(Cmdbuf, "Percentage++", 12) == 0)
    {
        LightBrightness += 20;
        if(LightBrightness >= MAX_BRIGHTNESS)
        {
            LightBrightness = MAX_BRIGHTNESS;
        }

        if(TRUE == IsPasCmd)
		{
			Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
		}
    }
    else if(memcmp(Cmdbuf, "Percentage--", 12) == 0)
    {
        LightBrightness -= 20;
        if((LightBrightness > MAX_BRIGHTNESS) || (LightBrightness < MIN_BRIGHTNESS))
        {
            LightBrightness = MIN_BRIGHTNESS;
        }

        if(TRUE == IsPasCmd)
		{
			Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
		}
    }
    else if(memcmp(Cmdbuf, "Percentage+", 11) == 0)
    {
		Temp = 11;
		while(('&' != Cmdbuf[Temp]) && ('\0' != Cmdbuf[Temp]))
		{
			Temp++;
		}

		while ('+' != Cmdbuf[--Temp])
		{
		    Count++;
		    if(Count == 1)
		        LightBrightness = (Cmdbuf[Temp]-0x30); 
		    else if(Count == 2)
		        LightBrightness += (Cmdbuf[Temp]-0x30)*10;
		    else if(Count == 3)
                LightBrightness += (Cmdbuf[Temp]-0x30)*100;
		}
		
        if(LightBrightness > MAX_BRIGHTNESS)
        {
            LightBrightness = MAX_BRIGHTNESS;
        }

        if(TRUE == IsPasCmd)
		{
			Mcu_SendCmdToWiFi(MCU_IOTN_LIGHT, &LightBrightness);
		}
        APP_DBG("Alexa set Light duty = %d!\n", LightBrightness);
    }

    if((memcmp(Cmdbuf, "+Getlightstatus", 15) == 0)
    || (FALSE == IsPasCmd))
    {
    	if(PWM_LED_COLOUR_IDLE != PwmLightDisplayMode)
    	{
    		Mcu_SendCmdToWiFi(MCU_PAS_IOTN__ON, NULL);
    	}
    	else
    	{
    		Mcu_SendCmdToWiFi(MCU_PAS_IOTN_OFF, NULL);
    	}
    	WaitMs(10);
    	Mcu_SendCmdToWiFi(MCU_PAS_IOTN_LIGHT, &LightBrightness);
    	WaitMs(10);
		DutyBuf[0] = PwmLightState[0].PwmSetDuty*255/100;
		DutyBuf[1] = PwmLightState[1].PwmSetDuty*255/100;
		DutyBuf[2] = PwmLightState[2].PwmSetDuty*255/100;
    	Mcu_SendCmdToWiFi(MCU_PAS_IOTN_COLOR, &DutyBuf[0]);
    }

	for (Temp = 0; Temp < MAX_LIGHT_NUM; ++Temp)
	{
		PwmLightState[Temp].PwmSetDuty = PwmSetDutyTemp[Temp]*LightBrightness/100;
	}
}
#endif
/******************************************************************************/
//暖光LED根据环境亮度及人物活动情况自动检测开关。
/******************************************************************************/
#ifdef FUNC_WIFI_DUMI_PROJECT_EN
void LightLedDisplayAvsCmdProcess(uint8_t* AvsCmdBuf,uint16_t Len)
{
	uint8_t AvsCmdProcessBuf[64];
	uint16_t AvsCmdLen = 0,Temp = 0;

	memset(AvsCmdProcessBuf, 0, sizeof(AvsCmdProcessBuf));

	while(AvsCmdLen <= Len)
	{	
        if(memcmp(&AvsCmdBuf[AvsCmdLen], "name", 4) == 0)
        {
            AvsCmdLen += 5; 
            do{
                AvsCmdLen++;
    			AvsCmdProcessBuf[Temp++] = AvsCmdBuf[AvsCmdLen];
    		}
    		while (AvsCmdBuf[AvsCmdLen] != ',');
    		break;
        }

        AvsCmdLen++;
        if(AvsCmdLen >= Len)
        {
            return;
        }
	}

    while(AvsCmdLen <= Len)
	{	
        if(memcmp(&AvsCmdBuf[AvsCmdLen], "payload", 7) == 0)
        {
            AvsCmdLen += 8; 
            do{
                AvsCmdLen++;
    			AvsCmdProcessBuf[Temp++] = AvsCmdBuf[AvsCmdLen];
    		}
    		while (AvsCmdBuf[AvsCmdLen] != '}');
    		break;
        }

        AvsCmdLen++;
        if(AvsCmdLen >= Len)
        {
            return;
        }
    }

	APP_DBG("Pass through Light Cmd is:%s;\n", AvsCmdProcessBuf);

	if(memcmp(&AvsCmdProcessBuf[1], "TurnOn", 6) == 0)
	{
		PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
		LightBrightness = MAX_BRIGHTNESS;
	}
	else if(memcmp(&AvsCmdProcessBuf[1], "TurnOff", 7) == 0)
	{
		PwmLightDisplayMode = PWM_LED_COLOUR_IDLE;
		LightBrightness = 0;
	}
	else if(memcmp(&AvsCmdProcessBuf[1], "SetBrightness", 13) == 0)
	{
		AvsCmdLen = 13;
		while(AvsCmdLen <= Temp)
		{	
	        if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], "Num", 3) == 0)
	        {
	            AvsCmdLen += 6;                                  //指向第一个数字
	    		while((AvsCmdProcessBuf[AvsCmdLen] >= '0') && (AvsCmdProcessBuf[AvsCmdLen] <= '9'))
	    		{
	    			AvsCmdLen++;
	    		}
	    		break;
	        }

	        AvsCmdLen++;
	        if(AvsCmdLen >= Temp)
	        {
	            return;
	        }
		}

		Temp = 0;
		while (AvsCmdProcessBuf[--AvsCmdLen] != ':')
		{
			Temp++;
			if(Temp == 1)
			{
				LightBrightness = (AvsCmdProcessBuf[AvsCmdLen]-0x30);
			}
			else if(Temp == 2)
			{
				LightBrightness += (AvsCmdProcessBuf[AvsCmdLen]-0x30)*10;
			}
			else if(Temp == 3)
			{
				LightBrightness += (AvsCmdProcessBuf[AvsCmdLen]-0x30)*100;
			}
			else 
			{
				LightBrightness += (AvsCmdProcessBuf[AvsCmdLen]-0x30)*1000;
			}
		}

		if(LightBrightness > MAX_BRIGHTNESS)
		{
			LightBrightness = MAX_BRIGHTNESS;
		}	
	}
    else if(memcmp(&AvsCmdProcessBuf[1], "AddBrightness", 13) == 0)
    {
		LightBrightness += 20;
        if(LightBrightness >= MAX_BRIGHTNESS)
        {
            LightBrightness = MAX_BRIGHTNESS;
        }
    }
    else if(memcmp(&AvsCmdProcessBuf[1], "ReduceBrightness", 16) == 0)
    {
        LightBrightness -= 20;
        if((LightBrightness > MAX_BRIGHTNESS) || (LightBrightness <= MIN_BRIGHTNESS))
        {
            LightBrightness = MIN_BRIGHTNESS;
        }
    }
    else if(memcmp(&AvsCmdProcessBuf[1], "MaxBrightness", 13) == 0)
    {
        LightBrightness = MAX_BRIGHTNESS;
    }
    else if(memcmp(&AvsCmdProcessBuf[1], "MinBrightness", 13) == 0)
    {
        LightBrightness = MIN_BRIGHTNESS;
    }
	else if(memcmp(&AvsCmdProcessBuf[1], "SetColor", 8) == 0)
	{
		AvsCmdLen = 8;
		while(AvsCmdLen <= Temp)
		{	
	        if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], "color", 5) == 0)
	        {
	            AvsCmdLen += 5; 
	    		while (AvsCmdProcessBuf[++AvsCmdLen] != ':');
	    		AvsCmdLen += 2;
	    		break;
	        }

	        AvsCmdLen++;
	        if(AvsCmdLen >= Temp)
	        {
	            return;
	        }
		}
        
		if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[0], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_RED;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[1], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_ORANGE;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[2], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_YELLOW;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[3], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_GREEN;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[4], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_CYAN;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[5], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_BLUE;
		}
        else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[6], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_PURPLE;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[13], 3) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
		}
        else if((memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[7], sizeof(SetSwitchCmdTable[7])) == 0)
             || (memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[8], 9) == 0)
             || (memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[9], sizeof(SetSwitchCmdTable[9])) == 0))
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_COLORFUL;
		}

        LightBrightness = MAX_BRIGHTNESS;
	}
	else if(memcmp(&AvsCmdProcessBuf[1], "SetMode", 7) == 0)
	{
		AvsCmdLen = 7;
		while(AvsCmdLen <= Temp)
		{	
	        if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], "model", 5) == 0)
	        {
	            AvsCmdLen += 5; 
	    		while (AvsCmdProcessBuf[++AvsCmdLen] != ':');
	    		AvsCmdLen += 2;
	    		break;
	        }

	        AvsCmdLen++;
	        if(AvsCmdLen >= Temp)
	        {
	            return;
	        }
		}
		
		if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[10], sizeof(SetSwitchCmdTable[10])) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
			LightBrightness = 70;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[11], sizeof(SetSwitchCmdTable[11])) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
			LightBrightness = 45;
		}
		else if(memcmp(&AvsCmdProcessBuf[AvsCmdLen], &SetSwitchCmdTable[12], sizeof(SetSwitchCmdTable[12])) == 0)
		{
			PwmLightDisplayMode = PWM_LED_COLOUR_WHITE;
			LightBrightness = 15;
		}
	}	

	APP_DBG("Baidu avs set light brightness = %d!!!\n", LightBrightness);
	PwmLedDisplayControlChange(PwmLightDisplayMode);
}
#endif
/******************************************************************************/
//红绿蓝随机闪烁，时间随机
/******************************************************************************/
void PwmRandomColourTimerBlink(void)
{
	static PWM_LED_COLOUR LedBlinkColour;
	uint8_t Temp;
	
	if(FALSE == IsPwmLedDisplayFlag)
	{
		for (Temp = 0; Temp < MAX_LIGHT_NUM; ++Temp)
		{
			PwmLightState[Temp].PwmSetDuty = 0;
		}
		return;
	}

	//暖光LED自动检测控制。
#ifdef LIGHT_DETECT_AUTO_CONTROL_EN
	if((PWM_LED_COLOUR_WHITE == PwmLightDisplayMode) || (PWM_LED_COLOUR_IDLE == PwmLightDisplayMode))
	{
		if(IsTimeOut(&LightAutoControlTimer))
			LightLedDisplayAutoControl();
	}
#endif
	
	if(PWM_LED_COLOUR_COLORFUL != PwmLightDisplayMode || !IsTimeOut(&PwmLightSpecTrumTimer))
	{
		return;
	}

	TimeOutSet(&PwmLightSpecTrumTimer, 30);
	if(IsTimeOut(&PwmLightControlTimer))
	{
		Temp = (GetRandNum(8)%5 + 1);
		TimeOutSet(&PwmLightControlTimer, Temp*300);
		
		LedBlinkColour++;
		if((PWM_LED_COLOUR_COLORFUL <= LedBlinkColour) || (PWM_LED_COLOUR_RED > LedBlinkColour))
		{
			LedBlinkColour = PWM_LED_COLOUR_RED;	
		}
		PwmLedDisplayControlChange(LedBlinkColour);
	}

	if(((gSys.CurModuleID == MODULE_ID_BLUETOOTH) && (GetPlayState() != PLAYER_STATE_PLAYING))
	|| ((gSys.CurModuleID == MODULE_ID_WIFI) && (gWiFi.PlayState == FALSE)))
	{
		LightBrightness = MAX_BRIGHTNESS;
	}
	else if(AudioSpecTrumStep > (LightBrightness/10))
	{
		LightBrightness = AudioSpecTrumStep*10;
	}
	else if(LightBrightness > 6)
	{
		LightBrightness -= 5;
	}

	for (Temp = 0; Temp < MAX_LIGHT_NUM; ++Temp)
	{
		PwmLightState[Temp].PwmSetDuty = PwmColorTable[LedBlinkColour][Temp]*LightBrightness/100;
	}
	//APP_DBG("LedPwmDuty = %d;\n", LedPwmDuty);
}

/******************************************************************************/																																					
//PWM LED显示模块初始化
/*****************************************************************************/
void PwmLightLedDisplayInit(void)
{
	TimeOutSet(&PwmLightSpecTrumTimer, 0);
	TimeOutSet(&PwmLightControlTimer, 0);
	memset(&PwmLightState, 0, sizeof(PwmLightState));
	PwmLightDisplayMode = PWM_LED_COLOUR_IDLE;
	LED_ALL_MODE_OFF();
	
#ifdef LIGHT_DETECT_AUTO_CONTROL_EN
#ifdef LIGHT_DETECT_GPIO1_MASK_BIT
	GpioClrRegOneBit(LIGHT_DETECT_GPIO1_PORT_OE, LIGHT_DETECT_GPIO1_MASK_BIT);
	GpioClrRegOneBit(LIGHT_DETECT_GPIO1_PORT_PU, LIGHT_DETECT_GPIO1_MASK_BIT);
	GpioClrRegOneBit(LIGHT_DETECT_GPIO1_PORT_PD, LIGHT_DETECT_GPIO1_MASK_BIT);
	GpioSetRegOneBit(LIGHT_DETECT_GPIO1_PORT_IE, LIGHT_DETECT_GPIO1_MASK_BIT);
#endif
#ifdef LIGHT_DETECT_GPIO2_MASK_BIT
	GpioClrRegOneBit(LIGHT_DETECT_GPIO2_PORT_OE, LIGHT_DETECT_GPIO2_MASK_BIT);
	GpioClrRegOneBit(LIGHT_DETECT_GPIO2_PORT_PU, LIGHT_DETECT_GPIO2_MASK_BIT);
	GpioClrRegOneBit(LIGHT_DETECT_GPIO2_PORT_PD, LIGHT_DETECT_GPIO2_MASK_BIT);
	GpioSetRegOneBit(LIGHT_DETECT_GPIO2_PORT_IE, LIGHT_DETECT_GPIO2_MASK_BIT);
#endif
#ifdef LIGHT_DETECT_GPIO3_MASK_BIT
	GpioClrRegOneBit(LIGHT_DETECT_GPIO3_PORT_OE, LIGHT_DETECT_GPIO3_MASK_BIT);
	GpioClrRegOneBit(LIGHT_DETECT_GPIO3_PORT_PU, LIGHT_DETECT_GPIO3_MASK_BIT);
	GpioClrRegOneBit(LIGHT_DETECT_GPIO3_PORT_PD, LIGHT_DETECT_GPIO3_MASK_BIT);
	GpioSetRegOneBit(LIGHT_DETECT_GPIO3_PORT_IE, LIGHT_DETECT_GPIO3_MASK_BIT);
#endif
#ifdef LIGHT_DETECT_ADC_CHANNEL
	SarAdcGpioSel(LIGHT_DETECT_ADC_CHANNEL);
#endif
#endif
}

#endif

/******************************************************************************/																																					
//PWM LED显示模块初始化
/*****************************************************************************/
void PwmLedDisplayInit(void)
{
#ifdef FUNC_ALEXA_PWM_LED_EN
	PwmAlexaLedDisplayInit();
#endif

#ifdef FUNC_LIGHT_PWM_LED_EN
	PwmLightLedDisplayInit();
#endif
}

// 显示刷新.
// Call by display refresh ISR.
void PwmLedFlushDisp(void)
{
#ifdef FUNC_LIGHT_PWM_LED_EN
	{
		uint8_t LightScanNum = 0;
		
		for(LightScanNum=0; LightScanNum<MAX_LIGHT_NUM; LightScanNum++)
		{
			if(PwmLightState[LightScanNum].PwmScanDuty < PwmLightState[LightScanNum].PwmSetDuty)
			{
				PwmLightState[LightScanNum].PwmScanDuty++;
			}
			else if(PwmLightState[LightScanNum].PwmScanDuty > PwmLightState[LightScanNum].PwmSetDuty)
			{
				PwmLightState[LightScanNum].PwmScanDuty--;
			}
			else if(PwmLightState[LightScanNum].PwmScanDuty == PwmLightState[LightScanNum].PwmSetDuty)
			{
				continue;
			}

			if(0 == LightScanNum)
			{
				LED_RED_MODE_ON(PwmLightState[LightScanNum].PwmScanDuty);
			}
			else if(1 == LightScanNum)
			{
				LED_GREEN_MODE_ON(PwmLightState[LightScanNum].PwmScanDuty);
			}
			else if(2 == LightScanNum)
			{
				LED_BLUE_MODE_ON(PwmLightState[LightScanNum].PwmScanDuty);
			}
			else if(3 == LightScanNum)
			{
				LED_LIGHT_MODE_ON(PwmLightState[LightScanNum].PwmScanDuty);
			}
		}
	}
#endif

#ifdef PWM_LED_SCAN_DISP_EN
	{
		static uint8_t ScanPinNum = 0;
		
	    //关闭所有输出
		LedAllPinGpioOutoff();
		PwmDisableChannel(UXLED_GREEN_PWM_PORT_MASK);
		PwmDisableChannel(UXLED_BLUE_PWM_PORT_MASK);
		PwmDisableChannel(UXLED_RED_PWM_PORT_MASK);
		
		ScanPinNum++;
		if(ScanPinNum >= PWM_LED_SCAN_MAX_NUM)
		{
			ScanPinNum = 0;
		}
		
		switch(PwmLedState[ScanPinNum].PwmLedColor)
		{
			case PWM_LED_COLOUR_RED:
				UXLED_RED_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;

			case PWM_LED_COLOUR_ORANGE:
				UXLED_ORANGE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;

			case PWM_LED_COLOUR_YELLOW:
				UXLED_YELLOW_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;

			case PWM_LED_COLOUR_GREEN:
				UXLED_GREEN_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;

			case PWM_LED_COLOUR_CYAN:
				UXLED_CYAN_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;

			case PWM_LED_COLOUR_BLUE:
				UXLED_BLUE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;

			case PWM_LED_COLOUR_PURPLE:
				UXLED_PURPLE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;

			case PWM_LED_COLOUR_WHITE:
				UXLED_WHITE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
				break;
					
			default:
				UXLED_ALL_MODE_OFF();
				break;
		}

		switch(ScanPinNum)
		{
			case 0:
				LED_PIN1_OUT_ON;
				break;

			case 1:
				LED_PIN2_OUT_ON;
				break;

			case 2:
				LED_PIN3_OUT_ON;
				break;

			case 3:
				LED_PIN4_OUT_ON;
				break;

			case 4:
				LED_PIN5_OUT_ON;
				break;

			case 5:
				LED_PIN6_OUT_ON;
				break;

			default:
				LED_ALL_POWER_OFF();
				break;
		}
	}
#endif
}

//开关LED扫描显示
void PwmLedDisplayScanSet(bool State)
{
	IsPwmLedDisplayFlag = State;
	if(State)
	{
		TimeOutSet(&LightAutoControlTimer, 40000);
	}
}

#endif
#endif

