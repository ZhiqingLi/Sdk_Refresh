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

#ifdef FUNC_ALEXA_PWM_LED_EN

TIMER PwmLedSpecTrumTimer;
TIMER PwmLedControlTimer;
TIMER PwmLedAutoControlTimer;
bool  IsPwmLedDisplayCartoonDone = FALSE;
PWM_LED_STATE PwmLedState[MAX_LED_PIN_NUM];

//LED 引脚对应 GPIO 初始化
void PwmLedDisplayInit(void)
{
	TimeOutSet(&PwmLedSpecTrumTimer, 0);
	TimeOutSet(&PwmLedControlTimer, 0);
	TimeOutSet(&PwmLedAutoControlTimer, 0);
	IsPwmLedDisplayCartoonDone = TRUE;
	memset(&PwmLedState, 0, sizeof(PwmLedState));

#ifdef PWM_LED_SCAN_DISP_EN
	GpioSetRegOneBit(LED_PIN1_PORT_PU, LED_PIN1_PORT_BIT);
	GpioSetRegOneBit(LED_PIN2_PORT_PU, LED_PIN2_PORT_BIT);
	GpioSetRegOneBit(LED_PIN3_PORT_PU, LED_PIN3_PORT_BIT);
	GpioSetRegOneBit(LED_PIN4_PORT_PU, LED_PIN4_PORT_BIT);
	GpioSetRegOneBit(LED_PIN5_PORT_PU, LED_PIN5_PORT_BIT);
	GpioSetRegOneBit(LED_PIN6_PORT_PU, LED_PIN6_PORT_BIT);

	GpioClrRegOneBit(LED_PIN1_PORT_PD, LED_PIN1_PORT_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_PD, LED_PIN2_PORT_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_PD, LED_PIN3_PORT_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_PD, LED_PIN4_PORT_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_PD, LED_PIN5_PORT_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_PD, LED_PIN6_PORT_BIT);

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
// 显示刷新.
// Call by display refresh ISR.
void PwmLedFlushDisp(void)
{
#ifdef PWM_LED_SCAN_DISP_EN
	static uint8_t ScanPinNum = 0;

	LedAllPinGpioOutoff();
	
	ScanPinNum++;
	if(ScanPinNum >= (MAX_LED_PIN_NUM))
	{
		ScanPinNum = 0;
	}
	
	switch(PwmLedState[ScanPinNum].PwmLedColor)
	{
		case PWM_LED_COLOUR_RED:
			LED_RED_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;

		case PWM_LED_COLOUR_ORANGE:
			LED_ORANGE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;

		case PWM_LED_COLOUR_YELLOW:
			LED_YELLOW_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;

		case PWM_LED_COLOUR_GREEN:
			LED_GREEN_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;

		case PWM_LED_COLOUR_CYAN:
			LED_CYAN_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;

		case PWM_LED_COLOUR_BLUE:
			LED_BLUE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;

		case PWM_LED_COLOUR_PURPLE:
			LED_PURPLE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;

		case PWM_LED_COLOUR_WHITE:
			LED_WHITE_MODE_ON(PwmLedState[ScanPinNum].PwmLedDuty);
			break;
				
		default:
			LED_ALL_MODE_OFF();
			break;
	}

	WaitUs(5);
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
#endif
}

//根据相关工作状态，设置led         UI
void PwmLedDisplayControlSetting(void)
{
	static uint8_t PrevAvsState;
	static uint16_t LedBlinkCnt,CartoonCnt;
	uint8_t TempCnt;
	
	if(!IsTimeOut(&PwmLedSpecTrumTimer))
	{
		return;
	}
	else
	{
		LedBlinkCnt++;
		if(PrevAvsState != gWiFi.MicState)
		{
			PrevAvsState = gWiFi.MicState;
			IsPwmLedDisplayCartoonDone = TRUE;
			LedBlinkCnt = 0;
			CartoonCnt = 0;
		}
	}
	
	if(gWiFi.TestModeState || (WiFiFirmwareUpgradeStateGet() == TRUE) || (WiFiFactoryStateGet() == TRUE))
	{
		TimeOutSet(&PwmLedSpecTrumTimer, 300);
		if(LedBlinkCnt == 1)
		{
			for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_RED;
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}
		}
		else if(LedBlinkCnt == 2)
		{
			for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
			{
				PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_GREEN;
				PwmLedState[TempCnt].PwmLedDuty = 100;
			}
		}
		else
		{
			for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
		if(IsPwmLedDisplayCartoonDone)
		{
			CartoonCnt++;
			if(CartoonCnt >= MAX_LED_PIN_NUM)
			{
				IsPwmLedDisplayCartoonDone = FALSE;
			}
		}
		else
		{
			CartoonCnt--;
			if(CartoonCnt <= 0)
			{
				IsPwmLedDisplayCartoonDone = TRUE;
			}
			else if(CartoonCnt > MAX_LED_PIN_NUM)
			{
				CartoonCnt = MAX_LED_PIN_NUM;
			}
		}

		for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
		{
			PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_WHITE;
			PwmLedState[TempCnt].PwmLedDuty = 100;
		}

		if(CartoonCnt >= MAX_LED_PIN_NUM)
		{
			PwmLedState[MAX_LED_PIN_NUM-1].PwmLedColor = PWM_LED_COLOUR_ORANGE;
		}
		else
		{
			PwmLedState[CartoonCnt].PwmLedColor = PWM_LED_COLOUR_ORANGE;
			if(CartoonCnt > 0)
			{
				PwmLedState[CartoonCnt-1].PwmLedColor = PWM_LED_COLOUR_ORANGE;
			}
		}
		return;
	}

	if((gWiFi.VisInfState == TRUE) || (gWiFi.MicState != WIFI_AVS_STATUS_IDLE))
	{
		if(gWiFi.MicState == WIFI_AVS_STATUS_LSN)
		{	
			TimeOutSet(&PwmLedSpecTrumTimer, 100);
			for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
				for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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

			for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
			
			for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
				
				for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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

					for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
					{
						PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_YELLOW;
						PwmLedState[TempCnt].PwmLedDuty = CartoonCnt*10;
					}
				}
				
				if((gWiFi.MicState == WIFI_AVS_STATUS_DIS) && (LedBlinkCnt == 30))
				{
					TimeOutSet(&PwmLedSpecTrumTimer, 3000);
					for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
				for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
				if(IsPwmLedDisplayCartoonDone)
				{
					CartoonCnt++;
					if(CartoonCnt >= MAX_LED_PIN_NUM)
					{
						IsPwmLedDisplayCartoonDone = FALSE;
					}
				}
				else
				{
					CartoonCnt--;
					if(CartoonCnt <= 0)
					{
						IsPwmLedDisplayCartoonDone = TRUE;
					}
					else if(CartoonCnt > MAX_LED_PIN_NUM)
					{
						CartoonCnt = MAX_LED_PIN_NUM;
					}
				}

				memset(&PwmLedState, 0, sizeof(PwmLedState));

				if(CartoonCnt >= MAX_LED_PIN_NUM)
				{
					PwmLedState[MAX_LED_PIN_NUM-1].PwmLedColor = PWM_LED_COLOUR_WHITE;
					PwmLedState[MAX_LED_PIN_NUM-1].PwmLedDuty = 100;
				}
				else
				{
					PwmLedState[CartoonCnt].PwmLedColor = PWM_LED_COLOUR_WHITE;
					PwmLedState[CartoonCnt].PwmLedDuty = 100;
					if(CartoonCnt > 0)
					{
						PwmLedState[CartoonCnt-1].PwmLedColor = PWM_LED_COLOUR_WHITE;
						PwmLedState[CartoonCnt-1].PwmLedDuty = 100;
					}
				}
			}
			else
			{
				for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
				{
					PwmLedState[TempCnt].PwmLedColor = PWM_LED_COLOUR_WHITE;
					PwmLedState[TempCnt].PwmLedDuty = 100;
				}
			}
			break;
#endif


#ifdef FUNC_LINEIN_EN
		case MODULE_ID_LINEIN:
			for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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

				for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
				for(TempCnt = 0; TempCnt < MAX_LED_PIN_NUM; TempCnt++)
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
#endif

