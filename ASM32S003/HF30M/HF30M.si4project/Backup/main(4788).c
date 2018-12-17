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

#define MAX_PWM_DUTY_CNT		100
#define LIGHT_PWM_CHANGE_CNT	200						//每次调整亮度的时间间隔，单位mS
#define POWER_OFF_WAIT_TIME 	5000					//关机等待时间，单位mS

//全局变量结构体定义
typedef enum _WIFI_POWER_STATUS_
{
	WIFI_POWER_STATUS_IDLE = 0,
	WIFI_POWER_STATUS_UNALLOWED = 0x80,
	WIFI_POWER_STATUS_WAIT = 0x86,
	WIFI_POWER_STATUS_AFFIRM = 0x88,
}WIFI_POWER_STATUS;

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
	B_T IsPwmSetDone;
    B_T IsRequestPowerOff;
	LIGHT_BLINK_STATUS LightBlinkState;
}GSYS_PARM;

const static GSYS_PARM InitgSys =
{
	0x00000808,
	50000,
	100,
	100,
	100,
	0,
	0,
	0,
	TRUE,
	FALSE,
	LIGHT_BLINK_STATUS_IDLE,
};

GSYS_PARM gSys;
SW_TIMER PwmScanTimer;
TIMER    RgbDisplayScanTimer;
TIMER    PowerOffTimer;

//七彩灯闪烁控制
void RGBDisplayScan(void)
{
    #define STEP_COUNT     5
    
	switch(gSys.LightBlinkState)
	{
		
		case LIGHT_BLINK_STATUS_REDADD:
			I2CRdBuffer[RED_BASE] += STEP_COUNT;
			if(I2CRdBuffer[RED_BASE] >= MAX_PWM_DUTY_CNT)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_BLUESUB;
			}
			break;
			
		case LIGHT_BLINK_STATUS_REDSUB:
			I2CRdBuffer[RED_BASE] -= STEP_COUNT;
			if(I2CRdBuffer[RED_BASE] == 0)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_BLUEADD;
			}
			break;
			
		case LIGHT_BLINK_STATUS_GREENADD:
			I2CRdBuffer[GREEN_BASE] += STEP_COUNT;
			if(I2CRdBuffer[GREEN_BASE] >= MAX_PWM_DUTY_CNT)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_REDSUB;
			}
			break;
			
		case LIGHT_BLINK_STATUS_GREENSUB:
			I2CRdBuffer[GREEN_BASE] -= STEP_COUNT;
			if(I2CRdBuffer[GREEN_BASE] == 0)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_REDADD;
			}
			break;
			
		case LIGHT_BLINK_STATUS_BLUEADD:
			I2CRdBuffer[BLUE_BASE] += STEP_COUNT;
			if(I2CRdBuffer[BLUE_BASE] >= MAX_PWM_DUTY_CNT)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_GREENSUB;
			}
			break;
			
		case LIGHT_BLINK_STATUS_BLUESUB:
			I2CRdBuffer[BLUE_BASE] -= STEP_COUNT;
			if(I2CRdBuffer[BLUE_BASE] == 0)
			{
				gSys.LightBlinkState = LIGHT_BLINK_STATUS_GREENADD;
			}
			break;

		default:
			I2CRdBuffer[RED_BASE] = MAX_PWM_DUTY_CNT;
			I2CRdBuffer[GREEN_BASE] = 0;
			I2CRdBuffer[BLUE_BASE] = 0;
			gSys.LightBlinkState = LIGHT_BLINK_STATUS_GREENADD;
			break;
	}

	gSys.IsPwmSetDone = TRUE;
}

//PWM输出扫描
void PwmDisplayScan(void)
{
#ifdef FUNC_PWM_EN
	if(((gSys.RedPwmDutyVal != gSys.Pwm0DutyVal)
	|| (gSys.GreenPwmDutyVal != gSys.Pwm1DutyVal)
	|| (gSys.BluePwmDutyVal != gSys.Pwm2DutyVal))
	&& gSys.IsPwmSetDone)
	{
		uint8_t ChangeTimer = 0,Temp;

		gSys.IsPwmSetDone = FALSE;
		
		ChangeTimer = abs(gSys.RedPwmDutyVal - gSys.Pwm0DutyVal);
		Temp = abs(gSys.GreenPwmDutyVal - gSys.Pwm1DutyVal);
		ChangeTimer = (Temp > ChangeTimer)? Temp : ChangeTimer;
		Temp = abs(gSys.BluePwmDutyVal - gSys.Pwm2DutyVal);
		ChangeTimer = (Temp > ChangeTimer)? Temp : ChangeTimer;
		APP_DBG("get max val = %d :", ChangeTimer);
		
		ChangeTimer = LIGHT_PWM_CHANGE_CNT/ChangeTimer;
		ChangeTimerPeriod(&PwmScanTimer, ChangeTimer);
		APP_DBG("ChangeTimerPeriod = %d ;\n", ChangeTimer);
	}

	if((gSys.RedPwmDutyVal == gSys.Pwm0DutyVal)
	&& (gSys.GreenPwmDutyVal == gSys.Pwm1DutyVal)
	&& (gSys.BluePwmDutyVal == gSys.Pwm2DutyVal))
	{
		gSys.IsPwmSetDone = TRUE;
	}
	
	if(gSys.RedPwmDutyVal != gSys.Pwm0DutyVal)
	{
		if(gSys.RedPwmDutyVal > gSys.Pwm0DutyVal)
		{
			gSys.RedPwmDutyVal--;
		}
		else
		{
			gSys.RedPwmDutyVal++;
		}
		GTC_PwmControl_Change(GTC0, gSys.PwmFreqVal, gSys.RedPwmDutyVal);
	}

	if(gSys.GreenPwmDutyVal != gSys.Pwm1DutyVal)
	{
		if(gSys.GreenPwmDutyVal > gSys.Pwm1DutyVal)
		{
			gSys.GreenPwmDutyVal--;
		}
		else
		{
			gSys.GreenPwmDutyVal++;
		}
		GTC_PwmControl_Change(GTC1, gSys.PwmFreqVal, gSys.GreenPwmDutyVal);
	}

	if(gSys.BluePwmDutyVal != gSys.Pwm2DutyVal)
	{
		if(gSys.BluePwmDutyVal > gSys.Pwm2DutyVal)
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
#ifdef FUNC_GPIO_POWERON_EN
    SysPowerOnControl(TRUE);
#endif
	InitTimer(&PwmScanTimer, 20, PwmDisplayScan);
	StartTimer(&PwmScanTimer);
	TimeOutSet(&RgbDisplayScanTimer, LIGHT_PWM_CHANGE_CNT);

	memcpy((void*)&gSys, (void*)&InitgSys, sizeof(GSYS_PARM));
#ifdef FUNC_IIC_EN
    /* 同步数据到IIC缓冲区 */
	memcpy((void*)&I2CRdBuffer[RGB_CLOUR_BASE], (void*)&gSys.Pwm0DutyVal, 3);
	memcpy((void*)&I2CWrBuffer[RGB_CLOUR_BASE], (void*)&gSys.Pwm0DutyVal, 3);
#endif
	while(1)
	{
		uint16_t Msg;
		
		CORET_Systick_Check();
		CheckTimer(&PwmScanTimer);
#ifdef FUNC_KEY_EN
		Msg = KeyScan();
#endif
#ifdef FUNC_IIC_EN
        if(Msg != MSG_NONE)
        {
            /* 通知主机接收按键消息 */
            I2CWrBuffer[KEY_MSG_BASE] = (Msg&0x00FF);   
            APP_DBG("I2CWrBuffer[KEY_MSG_BASE] = %x!\n", I2CWrBuffer[KEY_MSG_BASE]);
#ifdef FUNC_IIC_INT_INFORM_EN
            GPIO_Write_High(GPIO_INT_INFORM_PORT_OUT,GPIO_INT_INFORM_PORT_BIT);
            WaitMs(5);
            GPIO_Write_Low(GPIO_INT_INFORM_PORT_OUT,GPIO_INT_INFORM_PORT_BIT);
#endif
            if(MSG_POWER == Msg)
            {
                TimeOutSet(&PowerOffTimer, POWER_OFF_WAIT_TIME);
                gSys.IsRequestPowerOff = TRUE;
            }
        }
        
        if(memcmp((void*)&gSys.Pwm0DutyVal, (void*)&I2CRdBuffer[RGB_CLOUR_BASE], 3) != 0)
        {
            memcpy((void*)&gSys.Pwm0DutyVal, (void*)&I2CRdBuffer[RGB_CLOUR_BASE], 3);
            memcpy((void*)&I2CWrBuffer[RGB_CLOUR_BASE], (void*)&I2CRdBuffer[RGB_CLOUR_BASE], 3);
        }

        if(FALSE != I2CRdBuffer[KEY_MSG_BASE])
        {
            switch (I2CRdBuffer[KEY_MSG_BASE])
            {
                case WIFI_POWER_STATUS_WAIT:
                    TimeOutSet(&PowerOffTimer, POWER_OFF_WAIT_TIME);
                    gSys.IsRequestPowerOff = TRUE;
                    break;
                    
                case WIFI_POWER_STATUS_UNALLOWED:
                    gSys.IsRequestPowerOff = FALSE;
                    break;
                    
                case WIFI_POWER_STATUS_AFFIRM:
                    SystemGotoDeepSleepPowerDown();
                    break;
                    
                default:
                    gSys.IsRequestPowerOff = FALSE;
                    break; 
            }
            
            I2CRdBuffer[KEY_MSG_BASE] = FALSE; 
        }
#endif
        if((TRUE == gSys.IsRequestPowerOff) && IsTimeOut(&PowerOffTimer))
        {
            gSys.IsRequestPowerOff = FALSE;
            SystemGotoDeepSleepPowerDown();
        }
        
		if(IsTimeOut(&RgbDisplayScanTimer))
		{
            TimeOutSet(&RgbDisplayScanTimer, LIGHT_PWM_CHANGE_CNT);
//			RGBDisplayScan();
		}
	}

}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/


