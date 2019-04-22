/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : AsmPlatformconfig.c
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月15日
  最近修改   :
  功能描述   : ASM32芯片平台配置文件
  函数列表   :
              fputc
              Usart_Debug_Init
  修改历史   :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "stm32f0xx_conf.h"
#include "app_config.h"
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
extern volatile uint32_t CORET_Systick_Count;			//CORET Systick count
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

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*****************************************************************************
 函 数 名  : Usart_Debug_Init
 功能描述  : 串口打印初始化
 输入参数  : uint32_t Baudrate  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void Usart_Debug_Init(uint32_t Baudrate){
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_DeInit(USART1);
	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_1);

	/* Connect PXx to USARTx_Rx */
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_2);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */
	//GPIO_InitStructure.GPIO_Pin = GPIO_pin_15;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USARTx configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- Stop Bit = 1 Stop Bit
	- Parity = No Parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = Baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
	printf("Usart debug init OK!\n");
}
/*****************************************************************************
 函 数 名  : fputc
 功能描述  : 打印输出重定向到USART TX
 输入参数  : int ch   
             FILE *f  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint16_t)ch);
    while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));

    return ch;
}
/*****************************************************************************
 函 数 名  : System_Systick_Init
 功能描述  : 系统滴答定时器初始化函数
 输入参数  : uint32_t ticks  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void System_Systick_Init(uint32_t ticks)
{
	if (SysTick_Config(SystemCoreClock / ticks)){
	/* capture error */
		printf("SysTick_Config ERRO!!!\n");
		return;
	}
	/* configure the systick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00U);
	printf("SysTick_Config %d uS OK.\n", ticks);
}
/*****************************************************************************
 函 数 名  : System_WaterPump_Init
 功能描述  : 水泵控制和检测相关接口初始化
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月20日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void System_WaterPump_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);

	/* Configure ADC Channel11 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//ADC Change config
	ADC_InitTypeDef 	ADC_InitStructure;
	
	/* ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* ADCs DeInit */  
	ADC_DeInit(ADC1);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure); 

	/* Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time */ 
	ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_239_5Cycles);

	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* Enable the ADC peripheral */
	ADC_Cmd(ADC1, ENABLE);     

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
}
/*****************************************************************************
 函 数 名  : Pwm_SaveConfig
 功能描述  : 喇叭输出信号配置
 输入参数  : uint32_t Freq  
             uint16_t Duty  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月20日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void Pwm_SaveConfig(uint32_t Freq, uint16_t Duty) {
	PwmConfigChannel(PWM14_CH1_A4_B1, Freq, Duty);
	PwmEnableChannel(PWM14_CH1_A4_B1, PWM_IO_SEL1, PWM_OUTPUT_HIGH);
}
/*****************************************************************************
 函 数 名  : System_WaterPump_Control
 功能描述  : 水泵工作状态控制
 输入参数  : FlagStatus NewState  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月20日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void System_WaterPump_Control(FunctionalState NewState) {
	if (ENABLE == NewState) {
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	}
	else {
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	}
}

/*****************************************************************************
 函 数 名  : WaterPump_WorkStateDetect
 功能描述  : 水泵工作状态检测
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月22日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
#define WATER_PUMP_DET_JITTER			20
#define WATER_PUMP_DETECT_NULL			(100*4095/3300)
#define WATER_PUMP_DETECT_SORT			(500*4095/3300)

uint16_t WaterPump_WorkStateDetect(void) {
	uint16_t WaterPumpDetectVal = 0, DetectMsg = MSG_VOL_NORMAL;
	static uint8_t	 CurIndex,PrevIndex = 0xFF;
	static TIMER WaterPumpDetTimer;
	const uint16_t WaterPumpParam[3] = {WATER_PUMP_DETECT_NULL, WATER_PUMP_DETECT_SORT, 4095};
	const uint16_t WaterPumpDetMsg[3] = {MSG_VOL_LOW, MSG_VOL_NORMAL, MSG_VOL_HIGHT};
	
	if (SET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) {
		WaterPumpDetectVal = ADC_GetConversionValue(ADC1);

		for (CurIndex = 0; CurIndex < 3; CurIndex++) {
			if (WaterPumpDetectVal < WaterPumpParam[CurIndex]) {
				break;
			}
		}

		if (PrevIndex != CurIndex) {
			TimeOutSet(&WaterPumpDetTimer, WATER_PUMP_DET_JITTER);
			PrevIndex = CurIndex;
		}
		else if (IsTimeOut(&WaterPumpDetTimer)) {
			TimeOutSet(&WaterPumpDetTimer, 500);
			DetectMsg = WaterPumpDetMsg[CurIndex];
		}
	}

	return DetectMsg;
}
/*****************************************************************************
 函 数 名  : Display
 功能描述  : 显示参数刷新，由main函数调用
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月20日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void Display(void) {
	Led_Display();
	Light_Display();
}
/*****************************************************************************
 函 数 名  : DisplayScan
 功能描述  : 显示扫描函数，由定时器中断调用
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月20日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void DisplayScan(void) {
	Led_DispScan();
	Light_DispScan();
}


