/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : func_led.c
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月18日
  最近修改   :
  功能描述   : LED显示实现
  函数列表   :
              Led_DispInit
              Led_DispScan
  修改历史   :
  1.日    期   : 2019年4月18日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "app_config.h"
/*----------------------------------------------*
 * 变量说明                                 *
 *----------------------------------------------*/
typedef struct _LED_DISP_ARR_ {
	DISP_COLOR_TYPEDEF DispColor;
	uint8_t DispDuty;
}LED_DISP_ARR;

typedef struct _LED_SCAN_PIN_ {
	GPIO_TypeDef * Gpio_Port;
	uint32_t Gpio_Clk;
	uint16_t Gpio_Pin;
}LED_SCAN_PIN;

typedef struct _LED_PWM_CHANNEL_ {
	uint16_t Pwm_OutChSel;
	uint8_t Pwm_OutIoSel;
	uint8_t Pwm_OutLevel;
}LED_PWM_CHANNEL;
 
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
static LED_DISP_ARR Led_ScanParam[USE_LED_SCAN_PIN];
static SW_TIMER Led_DispChangeTime;
static DISP_STATE_TYPEDEF CurDispState;
static DISP_COLOR_TYPEDEF CurDispColor;
static uint32_t CurDispTimer;

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const LED_SCAN_PIN Gpio_Define[MAX_LED_SCAN_PIN] = {
	{LED_SCAN_GPIO_PORT1, LED_SCAN_GPIO_CLK1, LED_SCAN_GPIO_PIN1},
	{LED_SCAN_GPIO_PORT2, LED_SCAN_GPIO_CLK2, LED_SCAN_GPIO_PIN2},
	{LED_SCAN_GPIO_PORT3, LED_SCAN_GPIO_CLK3, LED_SCAN_GPIO_PIN3},
	{LED_SCAN_GPIO_PORT4, LED_SCAN_GPIO_CLK4, LED_SCAN_GPIO_PIN4},
	{LED_SCAN_GPIO_PORT5, LED_SCAN_GPIO_CLK5, LED_SCAN_GPIO_PIN5},
	{LED_SCAN_GPIO_PORT6, LED_SCAN_GPIO_CLK6, LED_SCAN_GPIO_PIN6},
	{LED_SCAN_GPIO_PORT7, LED_SCAN_GPIO_CLK7, LED_SCAN_GPIO_PIN7},
	{LED_SCAN_GPIO_PORT8, LED_SCAN_GPIO_CLK8, LED_SCAN_GPIO_PIN8},
	{LED_SCAN_GPIO_PORT9, LED_SCAN_GPIO_CLK9, LED_SCAN_GPIO_PIN9},
	{LED_SCAN_GPIO_PORT10, LED_SCAN_GPIO_CLK10, LED_SCAN_GPIO_PIN10},
	{LED_SCAN_GPIO_PORT11, LED_SCAN_GPIO_CLK11, LED_SCAN_GPIO_PIN11},
	{LED_SCAN_GPIO_PORT12, LED_SCAN_GPIO_CLK12, LED_SCAN_GPIO_PIN12},
};

const LED_PWM_CHANNEL Pwm_Define[MAX_PWM_SCAN_CHANNEL] = {
	{PWM_SCAN_CHANNEL_SEL1, PWM_SCAN_OUTIO_SEL1, PWM_SCAN_LEVEL_SEL1},
	{PWM_SCAN_CHANNEL_SEL2, PWM_SCAN_OUTIO_SEL2, PWM_SCAN_LEVEL_SEL2},
	{PWM_SCAN_CHANNEL_SEL3, PWM_SCAN_OUTIO_SEL3, PWM_SCAN_LEVEL_SEL3},
};

const uint16_t PwmColorTable[DISP_COLOR_MAX+1][3] =
{
	{0,			0,			0},								//空闲
	{1000,		0,			0},								//红
	{1000,		150,		0}, 							//橙
	{1000,		500,		0}, 							//黄
	{0,			1000,		0}, 							//绿
	{0,			1000,		1000},							//青
	{0,			0,			1000}, 							//蓝
	{1000,		0,			1000}, 							//紫
	{1000,		1000,		1000}, 							//白
};


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

static void Led_DispAutoChange(void) {
	static uint8_t Temp = 0, Duty = 0, CurParam = 0;
	static DISP_COLOR_TYPEDEF CurColor = DISP_COLOR_OFF;
	static FlagStatus Flip = RESET;
	
	switch (CurDispState) {
		case DISP_STATE_POWER_ON:
			for (Temp = 0; Temp < USE_LED_SCAN_PIN; Temp++) {
				Led_ScanParam[Temp].DispColor = CurDispColor;
				Led_ScanParam[Temp].DispDuty = MAX_DISP_DUTY;
			}
			Led_ScanParam[CurParam].DispColor = DISP_COLOR_ORANGE;
			Led_ScanParam[(CurParam+1)%MAX_LED_SCAN_PIN].DispColor = DISP_COLOR_ORANGE;
			
			CurParam++;
			CurParam %= MAX_LED_SCAN_PIN;
			ChangeTimerPeriod(&Led_DispChangeTime, CurDispTimer/USE_LED_SCAN_PIN);
			break;

		case DISP_STATE_SINGLE_COLOR:
			for (Temp = 0; Temp < USE_LED_SCAN_PIN; Temp++) {
				Led_ScanParam[Temp].DispColor = CurDispColor;
				Led_ScanParam[Temp].DispDuty = MAX_DISP_DUTY;
			}
			ChangeTimerPeriod(&Led_DispChangeTime, CurDispTimer);
			break;

		case DISP_STATE_CUR_BLINK:
			Duty = 0;
			Flip = (FlagStatus)!Flip;
			if (SET == Flip) {
				Duty = MAX_DISP_DUTY;
			}
			
			for (Temp = 0; Temp < USE_LED_SCAN_PIN; Temp++) {
				Led_ScanParam[Temp].DispColor = CurDispColor;
				Led_ScanParam[Temp].DispDuty = Duty;
			}
			ChangeTimerPeriod(&Led_DispChangeTime, CurDispTimer);
			break;

		case DISP_STATE_FULL_COLOR:
		case DISP_STATE_LOOP_CHANGE:
			if (Duty >= MAX_DISP_DUTY) {
				Flip = RESET;
				Duty = MAX_DISP_DUTY;
			}
			else if (Duty <= MIN_DISP_DUTY) {
				Flip = SET;
				Duty = MIN_DISP_DUTY;
				CurParam = 0xFF;
			}
		
			if (SET == Flip) {
				Duty += (MAX_DISP_DUTY/USE_SCAN_RATIO);
			}
			else {
				Duty -= (MAX_DISP_DUTY/USE_SCAN_RATIO);
			}

			if (DISP_STATE_FULL_COLOR == CurDispState) {
				if ((0xFF == CurParam) && (SET == Flip)) {
					CurParam = 0;
					CurColor++;
				}
				CurColor %= DISP_COLOR_MAX;
				if (DISP_COLOR_OFF == CurColor) {
					CurColor = DISP_COLOR_RED;
				}
			}
			else {
				CurColor = CurDispColor;
			}
			
			for (Temp = 0; Temp < USE_LED_SCAN_PIN; Temp++) {
				Led_ScanParam[Temp].DispColor = CurColor;
				Led_ScanParam[Temp].DispDuty = Duty;
			}
			//printf("DispColor=%d,DispDuty=%d,Flip=%d;\n",CurColor,Duty,Flip);
			ChangeTimerPeriod(&Led_DispChangeTime, CurDispTimer/USE_SCAN_RATIO);
			break;

		case DISP_STATE_LOOP_SPIN:
			CurParam++;
			CurParam %= USE_LED_SCAN_PIN;

			for (Temp = 0; Temp < USE_LED_SCAN_PIN; Temp++) {
				Led_ScanParam[Temp].DispColor = DISP_COLOR_OFF;
				Led_ScanParam[Temp].DispDuty = MAX_DISP_DUTY;
			}

			for (Temp = 0; Temp < SCAN_LOOP_SPIN_NUM; Temp++) {
				Led_ScanParam[(Temp+CurParam)%USE_LED_SCAN_PIN].DispColor = CurDispColor;
				Duty = (MAX_DISP_DUTY/SCAN_LOOP_SPIN_NUM*(SCAN_LOOP_SPIN_NUM-Temp));
				Led_ScanParam[(Temp+CurParam)%USE_LED_SCAN_PIN].DispDuty = Duty;
			}
			ChangeTimerPeriod(&Led_DispChangeTime, CurDispTimer/USE_LED_SCAN_PIN);
			break;

		default:
			memset(&Led_ScanParam, 0, sizeof(Led_ScanParam));
			ChangeTimerPeriod(&Led_DispChangeTime, CurDispTimer);
			break;	
	}
}

void Led_DispScan(void) {
	static uint8_t LedScanIndex = 0;
	uint8_t ScanNum = 0;

	for (ScanNum = 0; ScanNum < USE_LED_SCAN_PIN; ScanNum++) {
		GPIO_ResetBits(Gpio_Define[ScanNum].Gpio_Port, Gpio_Define[ScanNum].Gpio_Pin);
	}
	
	for (ScanNum = 0; ScanNum < USE_PWM_SCAN_CHANNEL; ScanNum++) {
		DISP_COLOR_TYPEDEF Color = Led_ScanParam[LedScanIndex].DispColor;		//获取当前SEG的颜色
		uint16_t Duty = PwmColorTable[Color][ScanNum];							//获取当前颜色下正在设置PWM通道的占空比

		Duty = (Duty*Led_ScanParam[LedScanIndex].DispDuty/100);					//计算实际需要设置的占空比
		PwmConfigChannel(Pwm_Define[ScanNum].Pwm_OutChSel, PWM_LED_SCAN_FREQ, Duty);
	}

	GPIO_SetBits(Gpio_Define[LedScanIndex].Gpio_Port, Gpio_Define[LedScanIndex].Gpio_Pin);
	LedScanIndex++;
	LedScanIndex %= USE_LED_SCAN_PIN;

	//printf ("Led_DispScan LedScanIndex=%d,Gpio_Port=%x,Gpio_Pin=%x\n",LedScanIndex,Gpio_Define[LedScanIndex].Gpio_Port,Gpio_Define[LedScanIndex].Gpio_Pin);
}

static void Led_ScanGpioConfig(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t GpioInitIndex;

	for (GpioInitIndex = 0; GpioInitIndex < USE_LED_SCAN_PIN; GpioInitIndex++) {
		/* Enable GPIO clock */
		RCC_AHBPeriphClockCmd(Gpio_Define[GpioInitIndex].Gpio_Clk, ENABLE);

		/* Configure GPIO function */
		GPIO_InitStructure.GPIO_Pin = Gpio_Define[GpioInitIndex].Gpio_Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(Gpio_Define[GpioInitIndex].Gpio_Port, &GPIO_InitStructure);
		GPIO_ResetBits(Gpio_Define[GpioInitIndex].Gpio_Port, Gpio_Define[GpioInitIndex].Gpio_Pin);
	}
}

static void Led_ScanPwmConfig(void) {
	uint8_t PwmInitIndex = 0;
	for (PwmInitIndex = 0; PwmInitIndex < USE_PWM_SCAN_CHANNEL; PwmInitIndex++) {
		PwmConfigChannel(Pwm_Define[PwmInitIndex].Pwm_OutChSel, PWM_LED_SCAN_FREQ, 0);
		PwmEnableChannel(Pwm_Define[PwmInitIndex].Pwm_OutChSel, Pwm_Define[PwmInitIndex].Pwm_OutIoSel, Pwm_Define[PwmInitIndex].Pwm_OutLevel);
	}
}

void Led_DispInit(void) {
	Led_ScanGpioConfig();
	Led_ScanPwmConfig();
	
	InitTimer(&Led_DispChangeTime, 0, Led_DispAutoChange);
	StartTimer(&Led_DispChangeTime);
	
	memset(&Led_ScanParam, 0, sizeof(Led_ScanParam));

	Led_DispStateSet(DISP_STATE_IDLE, DISP_COLOR_OFF, 0);

	printf ("Led disp init OK!!!\n");
}

void Led_Display(void) {
	CheckTimer(&Led_DispChangeTime);
}

void Led_DispStateSet(DISP_STATE_TYPEDEF State, DISP_COLOR_TYPEDEF Color, uint32_t Timer) {
	if ((CurDispState != State) || (CurDispColor != Color)) {	
		CurDispState = State;
		CurDispColor = Color;
		CurDispTimer = Timer;
	}
}

