/*
  ******************************************************************************
  * @file    main.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"
#include "asm32s003_gpio.h"
#include "asm32s003_uart.h"
#include "asm32s003_counta.h"
#include "asm32s003_i2c.h"
#include "asm32s003_led.h"
#include "asm32s003_gtc.h"
#include "asm32s003_pwm.h"
#include "asm32s003_ifc.h"
#include "asm32s003_adc.h"
#include "asm32s003_syscon.h"
#include "app_config.h"
#include "string.h"
#include "stdlib.h"

/* externs--------- ----------------------------------------------------------*/
extern void ASM32S003_init(void);
extern void CORET_Systick_Check(void);

extern volatile uint8_t I2CWrBuffer[BUFSIZE];
extern volatile uint8_t I2CRdBuffer[BUFSIZE];

#define MAX_PWM_DUTY_CNT		16
#define LIGHT_PWM_CHANGE_CNT	200						//Ã¿´Îµ÷ÕûÁÁ¶ÈµÄÊ±¼ä¼ä¸ô£¬µ¥Î»mS


//È«¾Ö±äÁ¿½á¹¹Ìå¶¨Òå
typedef enum _LIGHT_WORK_STATUS_
{
	LIGHT_WORK_STATUS_SUNLIGHT = 0,
	LIGHT_WORK_STATUS_COLORLIGHT = 1,
}LIGHT_WORK_STATUS;

typedef enum _LIGHT_BLINK_STATUS_
{
	LIGHT_BLINK_STATUS_IDLE = 0,
	LIGHT_BLINK_STATUS_REDADD = 1,
	LIGHT_BLINK_STATUS_REDSUB = 2,
	LIGHT_BLINK_STATUS_GREENADD = 3,
	LIGHT_BLINK_STATUS_GREENSUB = 4,
	LIGHT_BLINK_STATUS_BLUEADD = 5,
	LIGHT_BLINK_STATUS_BLUESUB = 6,
}LIGHT_BLINK_STATUS;

typedef struct _GSYS_PARM_
{
	U32_T SystemWorkBuf;
	U32_T PwmFreqVal;
	U8_T Pwm0DutyVal;
	U8_T Pwm1DutyVal;
	U8_T Pwm2DutyVal;
	U8_T RedPwmDutyVal;
	U8_T GreenPwmDutyVal;
	U8_T BluePwmDutyVal;
	B_T  IsPowerOnOff;
	B_T IsPwmSetDone;
	LIGHT_WORK_STATUS  IsLightWorkState;
	LIGHT_BLINK_STATUS LightBlinkState;
}GSYS_PARM;

const static GSYS_PARM InitgSys =
{
	0x00000808,
	50000,
	0,
	0,
	0,
	0,
	0,
	0,
	FALSE,
	TRUE,
	LIGHT_WORK_STATUS_SUNLIGHT,
	LIGHT_BLINK_STATUS_IDLE,
};


const uint8_t  LightPwmArr[MAX_PWM_DUTY_CNT+1] =
{
	0,
	10,	11,	12,	13,	15,	17,	20,	24,	
	30,	36,	43,	51,	60,	69,	84,	100,
};

GSYS_PARM gSys;
SW_TIMER PwmScanTimer;
TIMER    RgbDisplayScanTimer;

/*************************************************************/
//Light ON/OFF
/*************************************************************/
void LightModeContorlChange(B_T IsChange)
{
	if(IsChange)
	{
		gSys.IsLightWorkState = !gSys.IsLightWorkState;
	}
	
	if(gSys.IsLightWorkState == LIGHT_WORK_STATUS_SUNLIGHT)
	{	
		GTC0_IO_Init(GTC_IO_TXOUT, GTC0_PWMPIN_GPIOA11);
		GTC1_IO_Init(GTC_IO_TXOUT, GTC1_PWMPIN_GPIOA9);	
		gSys.Pwm0DutyVal = MAX_PWM_DUTY_CNT/2;
		gSys.Pwm1DutyVal = MAX_PWM_DUTY_CNT/2;
		gSys.Pwm2DutyVal = 0;
		GPIOC0->CONLR=(GPIOC0->CONLR & 0XFFFF000F);
		GPIO_PullLow_Init(GPIOC0,1);
		GPIO_PullLow_Init(GPIOC0,2);
		GPIO_PullLow_Init(GPIOC0,3);
	}
	else
	{
		GTC0_IO_Init(GTC_IO_TXOUT, GTC0_PWMPIN_GPIOC3);
		GTC1_IO_Init(GTC_IO_TXOUT, GTC1_PWMPIN_GPIOC2);
		GTC2_IO_Init(GTC_IO_TXOUT, GTC2_PWMPIN_GPIOC1);
		gSys.Pwm0DutyVal = MAX_PWM_DUTY_CNT;
		gSys.Pwm1DutyVal = 0;
		gSys.Pwm2DutyVal = 0;
		gSys.LightBlinkState = LIGHT_BLINK_STATUS_GREENADD;
		GPIOA0->CONHR=(GPIOC0->CONLR & 0XFFFF0F0F);
		GPIO_PullLow_Init(GPIOA0,9);
		GPIO_PullLow_Init(GPIOA0,11);
	}
	
}

/*************************************************************/
//Light ON/OFF
/*************************************************************/
void PowerOnOffContorl(void)
{	
	gSys.IsPowerOnOff = !gSys.IsPowerOnOff;
	
	if(gSys.IsPowerOnOff == TRUE)
	{
		gSys.Pwm0DutyVal = gSys.SystemWorkBuf&(U32_T)0xFF;
		gSys.Pwm1DutyVal = (gSys.SystemWorkBuf>>8)&(U32_T)0xFF;
		gSys.Pwm2DutyVal = (gSys.SystemWorkBuf>>16)&(U32_T)0xFF;
		LightModeContorlChange(FALSE);	
		GPIO_PullHigh_Init(GPIOB0,3);
		GPIO_Init(GPIOB0,3,Output);       //PA0.0 as output
		GPIO_Set_Value(GPIOB0,3,1);
	}
	else
	{	
		gSys.SystemWorkBuf = gSys.Pwm0DutyVal&(U32_T)0xFF;
		gSys.SystemWorkBuf |= gSys.Pwm1DutyVal<<8;
		gSys.SystemWorkBuf |= gSys.Pwm2DutyVal<<16;;
		gSys.Pwm0DutyVal = 0;
		gSys.Pwm1DutyVal = 0;
		gSys.Pwm2DutyVal = 0;
		GPIO_PullHigh_Init(GPIOB0,3);
		GPIO_Init(GPIOB0,3,Output);       //PA0.0 as output
		GPIO_Set_Value(GPIOB0,3,0);
	}
	gSys.IsPwmSetDone = TRUE;
}

/*************************************************************/
//Light DUTY change
/*************************************************************/
void LightDutyContorlChange(uint16_t Msg)
{
	static uint8_t MaxPwmDutyBuf = MAX_PWM_DUTY_CNT,PwmBeliel = 100;
	
	switch(Msg)
	{
		case MSG_VOL_DW:
			if((gSys.Pwm0DutyVal > 1) || (gSys.Pwm1DutyVal > 1))
			{
				if(gSys.Pwm0DutyVal == gSys.Pwm1DutyVal)
				{
					gSys.Pwm0DutyVal--;
					gSys.Pwm1DutyVal--;
				}
				else if(gSys.Pwm0DutyVal > gSys.Pwm1DutyVal)
				{
					gSys.Pwm0DutyVal--;
					gSys.Pwm1DutyVal = (gSys.Pwm0DutyVal*PwmBeliel/100);
				}
				else
				{
					gSys.Pwm1DutyVal--;
					gSys.Pwm0DutyVal = (gSys.Pwm1DutyVal*PwmBeliel/100);
				}
			}
		
			MaxPwmDutyBuf = (gSys.Pwm0DutyVal >= gSys.Pwm1DutyVal)? gSys.Pwm0DutyVal : gSys.Pwm1DutyVal; 
			break;
			
		case MSG_VOL_UP:
			if((gSys.Pwm0DutyVal < MAX_PWM_DUTY_CNT)
			&& (gSys.Pwm1DutyVal < MAX_PWM_DUTY_CNT))
			{
				if(gSys.Pwm0DutyVal == gSys.Pwm1DutyVal)
				{
					gSys.Pwm0DutyVal++;
					gSys.Pwm1DutyVal++;
				}
				else if(gSys.Pwm0DutyVal > gSys.Pwm1DutyVal)
				{
					gSys.Pwm0DutyVal++;
					gSys.Pwm1DutyVal = (gSys.Pwm0DutyVal*PwmBeliel/100);
				}
				else
				{
					gSys.Pwm1DutyVal++;
					gSys.Pwm0DutyVal = (gSys.Pwm1DutyVal*PwmBeliel/100);
				}
			}
			MaxPwmDutyBuf = (gSys.Pwm0DutyVal >= gSys.Pwm1DutyVal)? gSys.Pwm0DutyVal : gSys.Pwm1DutyVal; 
			break;
			
		case MSG_DUTY_DW:
			if(gSys.Pwm0DutyVal > 0)
			{
				gSys.Pwm0DutyVal--;
			}

			if(gSys.Pwm1DutyVal < MaxPwmDutyBuf)
			{
				gSys.Pwm1DutyVal++;
			}
			PwmBeliel = (gSys.Pwm0DutyVal >= gSys.Pwm1DutyVal)?(gSys.Pwm1DutyVal*100/gSys.Pwm0DutyVal):(gSys.Pwm0DutyVal*100/gSys.Pwm1DutyVal);
			break;
			
		case MSG_DUTY_UP:
			if(gSys.Pwm1DutyVal > 0)
			{
				gSys.Pwm1DutyVal--;
			}

			if(gSys.Pwm0DutyVal < MaxPwmDutyBuf)
			{
				gSys.Pwm0DutyVal++;
			}
			PwmBeliel = (gSys.Pwm0DutyVal >= gSys.Pwm1DutyVal)?(gSys.Pwm1DutyVal*100/gSys.Pwm0DutyVal):(gSys.Pwm0DutyVal*100/gSys.Pwm1DutyVal);
			break;

		default:
			gSys.Pwm0DutyVal = MAX_PWM_DUTY_CNT/2;
			gSys.Pwm1DutyVal = MAX_PWM_DUTY_CNT/2;
			MaxPwmDutyBuf = MAX_PWM_DUTY_CNT;
			PwmBeliel = 100;
			
			break;
	}

	gSys.IsPwmSetDone = TRUE;
	
}
//Æß²ÊµÆÉÁË¸¿ØÖÆ
void RGBDisplayScan(void)
{
	switch(gSys.LightBlinkState)
	{
		
		case LIGHT_BLINK_STATUS_REDADD:
			gSys.Pwm0DutyVal++;
			if(gSys.Pwm0DutyVal >= MAX_PWM_DUTY_CNT)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_BLUESUB;
			}
			break;
			
		case LIGHT_BLINK_STATUS_REDSUB:
			gSys.Pwm0DutyVal--;
			if(gSys.Pwm0DutyVal == 0)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_BLUEADD;
			}
			break;
			
		case LIGHT_BLINK_STATUS_GREENADD:
			gSys.Pwm1DutyVal++;
			if(gSys.Pwm1DutyVal >= MAX_PWM_DUTY_CNT)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_REDSUB;
			}
			break;
			
		case LIGHT_BLINK_STATUS_GREENSUB:
			gSys.Pwm1DutyVal--;
			if(gSys.Pwm1DutyVal == 0)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_REDADD;
			}
			break;
			
		case LIGHT_BLINK_STATUS_BLUEADD:
			gSys.Pwm2DutyVal++;
			if(gSys.Pwm2DutyVal >= MAX_PWM_DUTY_CNT)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_GREENSUB;
			}
			break;
			
		case LIGHT_BLINK_STATUS_BLUESUB:
			gSys.Pwm2DutyVal--;
			if(gSys.Pwm2DutyVal == 0)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_GREENADD;
			}
			break;

		default:
			gSys.Pwm0DutyVal = MAX_PWM_DUTY_CNT;
			gSys.Pwm1DutyVal = 0;
			gSys.Pwm2DutyVal = 0;
			gSys.LightBlinkState = LIGHT_BLINK_STATUS_GREENADD;
			break;
	}

	gSys.IsPwmSetDone = TRUE;
}

//PWMÊä³öÉ¨Ãè
void PwmDisplayScan(void)
{
#ifdef FUNC_PWM_EN
	if(((gSys.RedPwmDutyVal != LightPwmArr[gSys.Pwm0DutyVal])
	|| (gSys.GreenPwmDutyVal != LightPwmArr[gSys.Pwm1DutyVal])
	|| (gSys.BluePwmDutyVal != LightPwmArr[gSys.Pwm2DutyVal]))
	&& gSys.IsPwmSetDone)
	{
		uint8_t ChangeTimer = 0,Temp;

		gSys.IsPwmSetDone = FALSE;
		
		ChangeTimer = abs(gSys.RedPwmDutyVal - LightPwmArr[gSys.Pwm0DutyVal]);
		Temp = abs(gSys.GreenPwmDutyVal - LightPwmArr[gSys.Pwm1DutyVal]);
		ChangeTimer = (Temp > ChangeTimer)? Temp : ChangeTimer;
		Temp = abs(gSys.BluePwmDutyVal - LightPwmArr[gSys.Pwm2DutyVal]);
		ChangeTimer = (Temp > ChangeTimer)? Temp : ChangeTimer;
		APP_DBG("get max val = %d :", ChangeTimer);
		
		ChangeTimer = LIGHT_PWM_CHANGE_CNT/ChangeTimer;
		ChangeTimerPeriod(&PwmScanTimer, ChangeTimer);
		APP_DBG("ChangeTimerPeriod = %d ;\n", ChangeTimer);
	}

	if((gSys.RedPwmDutyVal == LightPwmArr[gSys.Pwm0DutyVal])
	&& (gSys.GreenPwmDutyVal == LightPwmArr[gSys.Pwm1DutyVal])
	&& (gSys.BluePwmDutyVal == LightPwmArr[gSys.Pwm2DutyVal]))
	{
		gSys.IsPwmSetDone = TRUE;
	}
	
	if(gSys.RedPwmDutyVal != LightPwmArr[gSys.Pwm0DutyVal])
	{
		if(gSys.RedPwmDutyVal > LightPwmArr[gSys.Pwm0DutyVal])
		{
			gSys.RedPwmDutyVal--;
		}
		else
		{
			gSys.RedPwmDutyVal++;
		}
		GTC_PwmControl_Change(GTC0, gSys.PwmFreqVal, gSys.RedPwmDutyVal);
	}

	if(gSys.GreenPwmDutyVal != LightPwmArr[gSys.Pwm1DutyVal])
	{
		if(gSys.GreenPwmDutyVal > LightPwmArr[gSys.Pwm1DutyVal])
		{
			gSys.GreenPwmDutyVal--;
		}
		else
		{
			gSys.GreenPwmDutyVal++;
		}
		GTC_PwmControl_Change(GTC1, gSys.PwmFreqVal, gSys.GreenPwmDutyVal);
	}

	if(gSys.BluePwmDutyVal != LightPwmArr[gSys.Pwm2DutyVal])
	{
		if(gSys.BluePwmDutyVal > LightPwmArr[gSys.Pwm2DutyVal])
		{
			gSys.BluePwmDutyVal--;
		}
		else
		{
			gSys.BluePwmDutyVal++;
		}
		GTC_PwmControl_Change(GTC2, gSys.PwmFreqVal, gSys.BluePwmDutyVal);
	}
#endif

}
/*************************************************************/
//main
/*************************************************************/
int main(void)
{

	ASM32S003_init();
	InitTimer(&PwmScanTimer, 20, PwmDisplayScan);
	StartTimer(&PwmScanTimer);

	TimeOutSet(&RgbDisplayScanTimer, LIGHT_PWM_CHANGE_CNT);

	memcpy((void*)&gSys, (void*)&InitgSys, sizeof(GSYS_PARM));
	while(1)
	{
		uint16_t Msg;
		
		CORET_Systick_Check();
		CheckTimer(&PwmScanTimer);
	#ifdef FUNC_KEY_EN
		Msg = KeyScan();
	#endif
		if((gSys.IsPowerOnOff != TRUE) && (Msg != MSG_LIGHT_ON_OFF))
		{
			continue;
		}
		
		switch(Msg)
		{
			case MSG_VOL_DW:
			case MSG_VOL_UP:
			case MSG_DUTY_DW:
			case MSG_DUTY_UP:
				if(gSys.IsLightWorkState == LIGHT_WORK_STATUS_SUNLIGHT)
				{
					LightDutyContorlChange(Msg);
				}
				break;

			case MSG_MODE:
				LightModeContorlChange(TRUE);
				break;
				
			case MSG_LIGHT_ON_OFF:
				PowerOnOffContorl();
				break;
				
			default:
				break;
		}

		if((gSys.IsLightWorkState == LIGHT_WORK_STATUS_COLORLIGHT)
		&& IsTimeOut(&RgbDisplayScanTimer))
		{
			RGBDisplayScan();
		}
	}

}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/


