/*
  ******************************************************************************
  * @file    asm32s003_gtc.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_gtc.h"
#include "app_config.h"

#ifdef FUNC_PWM_EN
/*************************************************************/
//GTC2 IO Init
//EntryParameter:GTC_IO_MODE_X,GTC_IO_G
//GTC_IO_MODE_X:GTC_IO_TXOUT,GTC_IO_TCLKX,GTC_IO_TCAPX
//GTC_IO_G;GTC_IO_TXOUT(0->PB0.00 ;1->PA0.13;2->PA0.07;3->PA0.04),GTC_IO_TXCLK(0->PA0.04;1->PB0.2;2->PA0.9),GTC_IO_TCAPX(0->PA0.08;1->PB0.2)
//ReturnValue:NONE
/*************************************************************/
void GTC_PwmControl_Init(void)
{
#ifdef FUNC_PWM0_OUT_EN
	GTC_RESET_VALUE(GTC0);															//GTC0 all register reset value
    GTC0_IO_Init(GTC_IO_TXOUT, FUNC_PWM0_GPIO_PORT);                                //set tcout
	GTC_PwmControl_Configure(GTC0, GTC_FIN_PCK, Counter_Size_16BIT, 50000, 0);				//TCCLK=sysclock/2^1/10茂录?0m-->1us
    GTC_ControlSet_Configure(GTC0,FUNC_PWM0_CONFIGURE,ENABLE);						//loop mode
    GTC_ConfigInterrupt_CMD(GTC0 , FUNC_PWM0_INT_CONFIG, ENABLE);                  	//counter start int Enable
	GTC_Start(GTC0);                                                               	//start counter
	if(FUNC_PWM0_INT_CONFIG)
    {
    	GTC0_Int_Enable();                                                          //enable GTC0 int
	}
	//GTC0_WakeUp_Enable();	
#endif

#ifdef FUNC_PWM1_OUT_EN
	GTC_RESET_VALUE(GTC1);															//GTC0 all register reset value
	GTC1_IO_Init(GTC_IO_TXOUT, FUNC_PWM1_GPIO_PORT);								//set tcout
	GTC_PwmControl_Configure(GTC1, GTC_FIN_PCK, Counter_Size_16BIT, 50000, 0); 			//TCCLK=sysclock/2^1/10茂录?0m-->1us
	GTC_ControlSet_Configure(GTC1,FUNC_PWM1_CONFIGURE,ENABLE);						//loop mode
	GTC_ConfigInterrupt_CMD(GTC1 , FUNC_PWM1_INT_CONFIG, ENABLE);					//counter start int Enable
	GTC_Start(GTC1);																//start counter
	if(FUNC_PWM1_INT_CONFIG)
	{
		GTC1_Int_Enable();															//enable GTC0 int
	}
	//GTC1_WakeUp_Enable(); 
#endif

#ifdef FUNC_PWM2_OUT_EN
	GTC_RESET_VALUE(GTC2);															//GTC0 all register reset value
	GTC2_IO_Init(GTC_IO_TXOUT, FUNC_PWM2_GPIO_PORT);								//set tcout
	GTC_PwmControl_Configure(GTC2, GTC_FIN_PCK, Counter_Size_16BIT, 50000, 0); 			//TCCLK=sysclock/2^1/10茂录?0m-->1us
	GTC_ControlSet_Configure(GTC2,FUNC_PWM2_CONFIGURE,ENABLE);						//loop mode
	GTC_ConfigInterrupt_CMD(GTC2 , FUNC_PWM2_INT_CONFIG, ENABLE);					//counter start int Enable
	GTC_Start(GTC2);																//start counter
	if(FUNC_PWM2_INT_CONFIG)
	{
		GTC2_Int_Enable();															//enable GTC0 int
	}
	//GTC2_WakeUp_Enable(); 
#endif

#ifdef FUNC_PWM3_OUT_EN
	GTC_RESET_VALUE(GTC3);															//GTC0 all register reset value
	GTC3_IO_Init(GTC_IO_TXOUT, FUNC_PWM3_GPIO_PORT);								//set tcout
	GTC_PwmControl_Configure(GTC3, GTC_FIN_PCK, Counter_Size_16BIT, 10000, 50); 			//TCCLK=sysclock/2^1/10茂录?0m-->1us
	GTC_ControlSet_Configure(GTC3,FUNC_PWM3_CONFIGURE,ENABLE);						//loop mode
	GTC_ConfigInterrupt_CMD(GTC3 , FUNC_PWM3_INT_CONFIG, ENABLE);					//counter start int Enable
	GTC_Start(GTC3);																//start counter
	if(FUNC_PWM3_INT_CONFIG)
	{
		GTC3_Int_Enable();															//enable GTC0 int
	}
	//GTC3_WakeUp_Enable(); 
#endif
	GPIO_PullLow_Init(GPIOA0,7);
	GPIO_PullLow_Init(GPIOA0,9);
	GPIO_PullLow_Init(GPIOA0,11);
}

/*************************************************************/
//GTC_PwmControl_Configure
//EntryParameter:*GTC,GTC_IO_G
//GTC_IO_MODE_X:GTC_IO_TXOUT,GTC_IO_TCLKX,GTC_IO_TCAPX
//GTC_IO_G;GTC_IO_TXOUT(0->PB0.02 ;1->PA0.03;2->PB0.03)GTC_IO_TCAPX(0->PB0.00)
//ReturnValue:NONE
/*************************************************************/
void GTC_PwmControl_Configure(CSP_GTC_T *GTC, GTC_FIN_TypeDef  GTC_FIN_X, Counter_Size_TypeDef  Counter_Size_X, U32_T PwmFreq, U8_T PwmDuty)
{
	U32_T GtcFreqVal = 0;
	U16_T GtcTimerVal = 0;
	#define MaxSecondToUs     1000000000ul

	GTC->CSSR = GTC_FIN_X;													//selected GTC clk
	GTC->CEDR = GTC_CLKEN|GTC_DBGEN;										//ENABLE GTC CLK
	GTC->CSMR = Counter_Size_X; 											//selected GTC conter size

	APP_DBG("Freq = %d, Duty = %d, ", PwmFreq, PwmDuty);
	if(PwmFreq > (WorkFreq/10))
	{
		APP_DBG("Erro pwm freq set!!!!!\n");
		return;
	}
	else
	{
		GtcTimerVal = MaxSecondToUs/WorkFreq;								//计算每个计数周期的时间长度，单位：us
		GtcFreqVal = MaxSecondToUs/PwmFreq;									//计算完整PWM周期的时间长度，单位：us
		GtcFreqVal = GtcFreqVal/GtcTimerVal;								//计算PWM周期时间内的最大计数值
		APP_DBG(" PRDR = %d;\n", GtcFreqVal);
		
		GTC->CDR = FALSE;										
		GTC->PRDR = GtcFreqVal;													//Period of GTC date register
		GTC->PULR = (PwmDuty*GtcFreqVal/100);									//Pulse of GTC date register
	}
}

/*************************************************************/
//GTC_PwmControl_Configure
//EntryParameter:*GTC,GTC_IO_G
//GTC_IO_MODE_X:GTC_IO_TXOUT,GTC_IO_TCLKX,GTC_IO_TCAPX
//GTC_IO_G;GTC_IO_TXOUT(0->PB0.02 ;1->PA0.03;2->PB0.03)GTC_IO_TCAPX(0->PB0.00)
//ReturnValue:NONE
/*************************************************************/
void GTC_PwmControl_Change(CSP_GTC_T *GTC, U32_T PwmFreq, U8_T PwmDuty)
{
	U32_T GtcFreqVal = 0;
	U16_T GtcTimerVal = 0;
	#define MaxSecondToUs     1000000000ul

	//APP_DBG("Change Freq = %d, Duty = %d, ", PwmFreq, PwmDuty);
	if(PwmFreq > (WorkFreq/10))
	{
		APP_DBG("Erro pwm freq set!!!!!\n");
		return;
	}
	else
	{
		GtcTimerVal = MaxSecondToUs/WorkFreq;								//计算每个计数周期的时间长度，单位：us
		GtcFreqVal = MaxSecondToUs/PwmFreq;									//计算完整PWM周期的时间长度，单位：us
		GtcFreqVal = GtcFreqVal/GtcTimerVal;								//计算PWM周期时间内的最大计数值
		//APP_DBG(" PRDR = %d;\n", GtcFreqVal);
		
		GTC_ControlSet_Configure(GTC,GTC_ControlSet_UPDATE,ENABLE); 
		GTC->CDR = FALSE;										
		GTC->PRDR = GtcFreqVal;													//Period of GTC date register
		GTC->PULR = (PwmDuty*GtcFreqVal/100);									//Pulse of GTC date register
		GTC_ControlSet_Configure(GTC,GTC_ControlSet_UPDATE,DISABLE); 
	}
}

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/
#endif
