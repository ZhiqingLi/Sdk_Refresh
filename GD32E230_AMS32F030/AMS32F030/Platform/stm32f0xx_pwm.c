/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : stm32f0xx_pwm.c
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月16日
  最近修改   :
  功能描述   : PWM驱动接口
  函数列表   :
              PwmEnableChannel
  修改历史   :
  1.日    期   : 2019年4月16日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "stm32f0xx_tim.h"
#include "stm32f0xx_pwm.h"
#include <stdio.h>

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const uint32_t PWMOUT_GPIO_CLK_ARRY[PWM_OUTPUT_NUM][PWM_IO_NUM] = {
	{RCC_AHBPeriph_GPIOA, NULL},
	{RCC_AHBPeriph_GPIOA, NULL},
	{RCC_AHBPeriph_GPIOA, NULL},
	{RCC_AHBPeriph_GPIOA, NULL},
	{RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB},
	{RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB},
	{RCC_AHBPeriph_GPIOB, NULL},
	{RCC_AHBPeriph_GPIOB, NULL},
	{RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB},
	{RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB},
	{RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB},
	{RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB},
	{RCC_AHBPeriph_GPIOA, RCC_AHBPeriph_GPIOB},
};

const uint16_t PWMOUT_GPIO_NUM_ARRY[PWM_OUTPUT_NUM][PWM_IO_NUM] = {
	{GPIO_Pin_8, NULL},
	{GPIO_Pin_9, NULL},
	{GPIO_Pin_10, NULL},
	{GPIO_Pin_11, NULL},
	{GPIO_Pin_6, GPIO_Pin_4},
	{GPIO_Pin_7, GPIO_Pin_5},
	{GPIO_Pin_0, NULL},
	{GPIO_Pin_1, NULL},
	{GPIO_Pin_4, GPIO_Pin_1},
	{GPIO_Pin_2, GPIO_Pin_14},
	{GPIO_Pin_3, GPIO_Pin_15},
	{GPIO_Pin_6, GPIO_Pin_8},
	{GPIO_Pin_7, GPIO_Pin_9},
};

GPIO_TypeDef * PWMOUT_GPIO_PORT_ARRY[PWM_OUTPUT_NUM][PWM_IO_NUM] = {
	{GPIOA, NULL},
	{GPIOA, NULL},
	{GPIOA, NULL},
	{GPIOA, NULL},
	{GPIOA, GPIOB},
	{GPIOA, GPIOB},
	{GPIOB, NULL},
	{GPIOB, NULL},
	{GPIOA, GPIOB},
	{GPIOA, GPIOB},
	{GPIOA, GPIOB},
	{GPIOA, GPIOB},
	{GPIOA, GPIOB},
};

const uint8_t PWMOUT_PIN_SOUR_ARRY[PWM_OUTPUT_NUM][PWM_IO_NUM] = {
	{GPIO_PinSource8, NULL},
	{GPIO_PinSource9, NULL},
	{GPIO_PinSource10, NULL},
	{GPIO_PinSource11, NULL},
	{GPIO_PinSource6, GPIO_PinSource4},
	{GPIO_PinSource7, GPIO_PinSource5},
	{GPIO_PinSource0, NULL},
	{GPIO_PinSource1, NULL},
	{GPIO_PinSource4, GPIO_PinSource1},
	{GPIO_PinSource2, GPIO_PinSource14},
	{GPIO_PinSource3, GPIO_PinSource15},
	{GPIO_PinSource6, GPIO_PinSource8},
	{GPIO_PinSource7, GPIO_PinSource9},
};

const uint8_t PWMOUT_PIN_AFX_ARRY[PWM_OUTPUT_NUM][PWM_IO_NUM] = {
	{GPIO_AF_2, NULL},
	{GPIO_AF_2, NULL},
	{GPIO_AF_2, NULL},
	{GPIO_AF_2, NULL},
	{GPIO_AF_1, GPIO_AF_1},
	{GPIO_AF_1, GPIO_AF_1},
	{GPIO_AF_1, NULL},
	{GPIO_AF_1, NULL},
	{GPIO_AF_4, GPIO_AF_0},
	{GPIO_AF_0, GPIO_AF_1},
	{GPIO_AF_0, GPIO_AF_1},
	{GPIO_AF_5, GPIO_AF_2},
	{GPIO_AF_5, GPIO_AF_2},
};


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define TIMER_MAX_COUNTER        (uint16_t)(0xFFFF)

/*****************************************************************************
 函 数 名  : PwmEnableChannel
 功能描述  : pwm 通道使能
 输入参数  : uint16_t PwmChSel  
             uint8_t PwmIoSel   
             uint8_t PwmMode    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月18日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void PwmEnableChannel(uint16_t PwmChSel, uint8_t PwmIoSel, uint8_t PwmMode) {
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t PwmIndexMask;

	PwmIndexMask = ((~(PWM_TIMER_MASK|PWM_CHANNEL_MASK))&PwmChSel);
	
	/* GPIO Clocks enable */
	RCC_AHBPeriphClockCmd(PWMOUT_GPIO_CLK_ARRY[PwmIndexMask][PwmIoSel], ENABLE);

	/* GPIO Configuration: as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = PWMOUT_GPIO_NUM_ARRY[PwmIndexMask][PwmIoSel];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(PWMOUT_GPIO_PORT_ARRY[PwmIndexMask][PwmIoSel], &GPIO_InitStructure);
	GPIO_PinAFConfig(PWMOUT_GPIO_PORT_ARRY[PwmIndexMask][PwmIoSel], PWMOUT_PIN_SOUR_ARRY[PwmIndexMask][PwmIoSel], PWMOUT_PIN_AFX_ARRY[PwmIndexMask][PwmIoSel]);
	
	/* Channel Configuration in PWM mode */
	TIM_TypeDef* TIM_Num;
	uint16_t PwmTimerMask = (PWM_TIMER_MASK&PwmChSel);
	
	if (PWM_IS_TIMER1 == PwmTimerMask) {
		TIM_Num = TIM1;
	}
	else if(PWM_IS_TIMER2 == PwmTimerMask) {
		TIM_Num = TIM2;
	}
	else if(PWM_IS_TIMER3 == PwmTimerMask) {
		TIM_Num = TIM3;
	}
	else if(PWM_IS_TIMER14 == PwmTimerMask) {
		TIM_Num = TIM14;
	}
	else if(PWM_IS_TIMER15 == PwmTimerMask) {
		TIM_Num = TIM15;
	}
	else if(PWM_IS_TIMER16 == PwmTimerMask) {
		TIM_Num = TIM16;
	}
	else if(PWM_IS_TIMER17 == PwmTimerMask) {
		TIM_Num = TIM17;
	}
	
	uint16_t PwmChannelNum = ((PWM_CHANNEL_MASK&PwmChSel)>>8);
	uint16_t OcOutputState = (PWM_OUTPUT_LOW == PwmMode)? TIM_OCPolarity_Low: TIM_OCPolarity_High;
	
	TIM_SelectOCxM(TIM_Num, PwmChannelNum, TIM_OCMode_PWM1);
	if (TIM_Channel_1 == PwmChannelNum) {
		TIM_OC1PolarityConfig(TIM_Num, OcOutputState);
	}
	else if (TIM_Channel_2 == PwmChannelNum) {
		TIM_OC2PolarityConfig(TIM_Num, OcOutputState);
	}
	else if (TIM_Channel_3 == PwmChannelNum) {
		TIM_OC3PolarityConfig(TIM_Num, OcOutputState);
	}
	else if (TIM_Channel_4 == PwmChannelNum) {
		TIM_OC4PolarityConfig(TIM_Num, OcOutputState);
	}
	TIM_CCxCmd(TIM_Num, PwmChannelNum, TIM_CCx_Enable);
	
	//printf("TIM config TIM_Num=%x,PwmChannelMask=%x;\n",TIM_Num,PwmChannelNum);
}
/*****************************************************************************
 函 数 名  : PwmDisableChannel
 功能描述  : pwm通道关闭输出
 输入参数  : uint16_t PwmChSel  
             uint8_t PwmIoSel   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月18日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void PwmDisableChannel(uint16_t PwmChSel, uint8_t PwmIoSel) {
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t PwmIndexMask;

	PwmIndexMask = ((~(PWM_TIMER_MASK|PWM_CHANNEL_MASK))&PwmChSel);

	/* GPIO Configuration: as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = PWMOUT_GPIO_NUM_ARRY[PwmIndexMask][PwmIoSel];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PWMOUT_GPIO_PORT_ARRY[PwmIndexMask][PwmIoSel], &GPIO_InitStructure);
	
	/* Channel Configuration in PWM mode disable */

	TIM_TypeDef* TIM_Num;
	uint16_t PwmTimerMask = (PWM_TIMER_MASK&PwmChSel);
	if (PWM_IS_TIMER1 == PwmTimerMask) {
		TIM_Num = TIM1;
	}
	else if(PWM_IS_TIMER2 == PwmTimerMask) {
		TIM_Num = TIM2;
	}
	else if(PWM_IS_TIMER3 == PwmTimerMask) {
		TIM_Num = TIM3;
	}
	else if(PWM_IS_TIMER14 == PwmTimerMask) {
		TIM_Num = TIM14;
	}
	else if(PWM_IS_TIMER15 == PwmTimerMask) {
		TIM_Num = TIM15;
	}
	else if(PWM_IS_TIMER16 == PwmTimerMask) {
		TIM_Num = TIM16;
	}
	else if(PWM_IS_TIMER17 == PwmTimerMask) {
		TIM_Num = TIM17;
	}
	
	uint16_t PwmChannelMask = (PWM_CHANNEL_MASK&PwmChSel);
	if (PWM_IS_CH1 == PwmChannelMask) {
		TIM_CCxCmd(TIM_Num, TIM_Channel_1, TIM_CCx_Disable);
	}
	else if (PWM_IS_CH2 == PwmChannelMask) {
		TIM_CCxCmd(TIM_Num, TIM_Channel_2, TIM_CCx_Disable);
	}
	else if (PWM_IS_CH3 == PwmChannelMask) {
		TIM_CCxCmd(TIM_Num, TIM_Channel_3, TIM_CCx_Disable);
	}
	else if (PWM_IS_CH4 == PwmChannelMask) {
		TIM_CCxCmd(TIM_Num, TIM_Channel_4, TIM_CCx_Disable);
	}
	
	printf("PWM Disable config TIM_Num=%x,PwmChannelMask=%x;\n",TIM_Num,PwmChannelMask);
}
/*****************************************************************************
 函 数 名  : PwmConfigChannel
 功能描述  : pwm通道配置
 输入参数  : uint16_t PwmChSel  
             uint16_t FreqDiv   
             uint16_t Duty      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月18日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void PwmConfigChannel(uint16_t PwmChSel, uint32_t FreqDiv, uint16_t Duty) {
	TIM_TypeDef* TIM_Num;
	uint16_t PwmTimerMask,PwmChannelMask;

	/* TIMER clock enable */
	PwmTimerMask = (PWM_TIMER_MASK&PwmChSel);
	
	if (PWM_IS_TIMER1 == PwmTimerMask) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
		TIM_Num = TIM1;
	}
	else if(PWM_IS_TIMER2 == PwmTimerMask) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
		TIM_Num = TIM2;
	}
	else if(PWM_IS_TIMER3 == PwmTimerMask) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
		TIM_Num = TIM3;
	}
	else if(PWM_IS_TIMER14 == PwmTimerMask) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14 , ENABLE);
		TIM_Num = TIM14;
	}
	else if(PWM_IS_TIMER15 == PwmTimerMask) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15 , ENABLE);
		TIM_Num = TIM15;
	}
	else if(PWM_IS_TIMER16 == PwmTimerMask) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16 , ENABLE);
		TIM_Num = TIM16;
	}
	else if(PWM_IS_TIMER17 == PwmTimerMask) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17 , ENABLE);
		TIM_Num = TIM17;
	}

	if((TIM_Num == TIM1) || (TIM_Num == TIM2) || (TIM_Num == TIM3)) {
		TIM_CounterModeConfig(TIM_Num, TIM_CounterMode_Up);
	}
	if(TIM_Num != TIM6) {
		TIM_SetClockDivision(TIM_Num, TIM_CKD_DIV1);
	}

	//判断时钟分频系数
	uint16_t Prescaler,AutoReload;
	if (1000 >= FreqDiv) {				//小于100HZ,1uS
		Prescaler = ((SystemCoreClock/1000000)-1);
		AutoReload = ((10000000/FreqDiv)-1);
	}
	else if (1000000 >= FreqDiv) {		//小于100KHZ,0.0625uS
		Prescaler = ((SystemCoreClock/16000000)-1);
		AutoReload = ((160000000/FreqDiv)-1);
	}
	else {								//约0.02083uS
		Prescaler = 0;
		AutoReload = ((SystemCoreClock/FreqDiv)-1);
	}
	TIM_SetAutoreload(TIM_Num, AutoReload);
	TIM_PrescalerConfig(TIM_Num, Prescaler, TIM_PSCReloadMode_Update);

	PwmChannelMask = (PwmChSel&PWM_CHANNEL_MASK);
	if (PWM_IS_CH1 == PwmChannelMask) {
		TIM_SetCompare1(TIM_Num, Duty*AutoReload/1000);
	}
	else if(PWM_IS_CH2 == PwmChannelMask) {
		TIM_SetCompare2(TIM_Num, Duty*AutoReload/1000);
	}
	else if(PWM_IS_CH3 == PwmChannelMask) {
		TIM_SetCompare3(TIM_Num, Duty*AutoReload/1000);
	}
	else if(PWM_IS_CH4 == PwmChannelMask) {
		TIM_SetCompare4(TIM_Num, Duty*AutoReload/1000);
	}

	TIM_Cmd(TIM_Num, ENABLE);
	if ((TIM_Num == TIM1) || (TIM_Num == TIM15) || (TIM_Num == TIM16) || (TIM_Num == TIM17)) {
		TIM_CtrlPWMOutputs(TIM_Num, ENABLE);
	}

	//printf("pwm config Prescaler = %d, AutoReload = %d, Compare = %d;\n",Prescaler, AutoReload, Duty*AutoReload/1000);
}


