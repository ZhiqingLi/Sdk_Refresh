/**
 **************************************************************************************
 * @file    timer_example.c
 *
 * @author  Cecilia Wang
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */


#include <string.h>
#include <nds32_intrinsic.h>
#include "type.h"
#include "irqn.h"
#include "debug.h" 
#include "timer.h"
#include "watchdog.h"
#include "gpio.h"
#include "uarts.h"
#include "timeout.h"
#include "pwm.h"
#include "delay.h"
#include "uarts_interface.h"
#include "cache_tcm.h"
#include "clk.h"
#include "pwc.h"
#include "spi_flash.h"


#define UART_TX_IO	0
#define UART_RX_IO  0


char	tempString[20];

char GetChar(void)
{
	uint8_t		ch = 0;
	uint8_t		ret=0;
	while(ret == 0)
	{
		if(UARTS_Recv(UART_RX_IO, &ch, 1) > 0)
		{
			ret=1;
		}
	}
	return ch;
}

char * GetString(void)
{
	char			ch;
	char			*tempP;
	memset(tempString, 0, sizeof(tempString));
	tempP = tempString;

	ch = GetChar();
	while(ch != 0x0D)
	{
		DBG("%c", ch);
		*(tempP++) = ch;
		ch = GetChar();
	}

	return tempString;
}

uint32_t	StrToI(char * str)
{
	uint32_t			result;

	result = 0;
	while(*str)
	{
		result = result*10 + *(str++) - '0';
	}
	return result;
}


static void PWM_GpioConfigDeInit(TIMER_INDEX TimerIdx)
{
	if(TimerIdx == TIMER3)
	{
		PWM_GpioConfig(TIMER3_PWM1_A2_B22_A30_C7, 0, PWM_IO_MODE_NONE);
	}

	if(TimerIdx == TIMER4)
	{
		PWM_GpioConfig(TIMER4_PWM1_A3_B23_A31_C8, 0, PWM_IO_MODE_NONE);
	}
}

static void PWM_GpioConfigInit(TIMER_INDEX TimerIdx)
{
	if(TimerIdx == TIMER3)
	{
		PWM_GpioConfig(TIMER3_PWM1_A2_B22_A30_C7, 0, PWM_IO_MODE_OUT);
		DBG("PWM Init OUTPUT: A2\n");
	}

	if(TimerIdx == TIMER4)
	{
		PWM_GpioConfig(TIMER4_PWM1_A3_B23_A31_C8, 0, PWM_IO_MODE_OUT);
		DBG("PWM Init OUTPUT: A3\n");
	}
}

void TimerReset(void)
{
	//RES
}
__attribute__((section(".driver.isr"))) void Timer2Interrupt(void)
{
	GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22); // toggle A22

	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
}

__attribute__((section(".driver.isr"))) void Timer3Interrupt(void)
{

	GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22); // toggle A22
	Timer_InterruptFlagClear(TIMER3, UPDATE_INTERRUPT_SRC);
}

__attribute__((section(".driver.isr"))) void Timer4Interrupt(void)
{
	GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22); // toggle A22
	Timer_InterruptFlagClear(TIMER3, UPDATE_INTERRUPT_SRC);
}

void Timer2InterruptModeExample(uint32_t freq)
{
	uint8_t	recvBuf;

	DBG("Timer2InterruptModeExample\n");
	DBG("Input 'x' to exit this example\n");

	NVIC_EnableIRQ(TMR2_IRQn);

	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);

	GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22);

	Timer_Config(TIMER2, freq, 0);
	Timer_Start(TIMER2);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);
		if(recvBuf == 'x')
		{
			break;
		}
		if(recvBuf == 'p')
		{
			Timer_Pause(TIMER2, 1);
			DBG("Timer2 Pause\n");
		}
		if(recvBuf == 'c')
		{
			Timer_Pause(TIMER2, 0);
			DBG("Timer2 Continues\n");
		}
	}
}

void Timer2CheckModeExample(uint32_t freq)
{
	uint8_t	recvBuf;

	DBG("Timer2CheckModeExample\n");
	DBG("Input 'x' to exit this example\n");


	NVIC_DisableIRQ(TMR2_IRQn);

	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
	GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22);

	Timer_Config(TIMER2, freq, 0);
	Timer_Start(TIMER2);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);
		if(recvBuf == 'x')
		{
			break;
		}

		if(Timer_InterruptFlagGet(TIMER2, UPDATE_INTERRUPT_SRC))
		{
			GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22);
			Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
		}
	}
}

void Timer3InterruptModeExample(uint32_t freq)
{
	uint8_t	recvBuf;

	DBG("Timer3InterruptModeExample\n");
	DBG("Input 'x' to exit this example\n");

	NVIC_EnableIRQ(TMR3_IRQn);

	Timer_InterruptFlagClear(TIMER3, UPDATE_INTERRUPT_SRC);

	GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22);
	Timer_Config(TIMER3, freq, 0);
	Timer_Start(TIMER3);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);
		if(recvBuf == 'x')
		{
			break;
		}
	}
}

void Timer3CheckModeExample(uint32_t freq)
{
	uint8_t	recvBuf;

	DBG("Timer3CheckModeExample\n");
	DBG("Input 'x' to exit this example\n");

	NVIC_DisableIRQ(TMR3_IRQn);

	Timer_InterruptFlagClear(TIMER3, UPDATE_INTERRUPT_SRC);

	GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22);

	Timer_Config(TIMER3, freq, 0);
	Timer_Start(TIMER3);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);
		if(recvBuf == 'x')
		{
			break;
		}

		if(Timer_InterruptFlagGet(TIMER3, UPDATE_INTERRUPT_SRC))
		{
			GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22);
			Timer_InterruptFlagClear(TIMER3, UPDATE_INTERRUPT_SRC);
		}
	}
}

#define FREQUENCY_1US				120

void TimerPWM1Example(void)
{
	uint8_t						recvBuf;

	TIMER_INDEX					timer = TIMER3;
//	TIMER_INDEX					inputTimer;

	PWM_StructInit				pwmParam;
	PWM_COUNTER_MODE			countMode = PWM_COUNTER_MODE_DOWN;
	PWM_OUTPUT_TYPE				outputType = PWM_OUTPUT_SINGLE_1;
	uint32_t					frequency = 100;
	uint8_t						duty = 50;

	DBG("TimerPWM1Example:\n\n");

GOHERE:
	DBG("Timer%d, Count Mode = %d, Duty = %d, Output Type = %d, Frequency = %d us\n", timer+1, countMode, duty, outputType, frequency);

	PWM_GpioConfigDeInit(timer);
	PWM_GpioConfigInit(timer);

	pwmParam.CounterMode		= countMode;
	pwmParam.OutputType			= outputType;
	pwmParam.DMAReqEnable		= PWM_REQ_INTERRUPT_MODE;
	pwmParam.FreqDiv			= frequency * FREQUENCY_1US;
	pwmParam.Duty[0]			= duty;
	pwmParam.OutEnable[0]		= 3;
	PWM_Config(timer, PWM1, &pwmParam);
	PWM_Enable(timer);

	DBG("\nInput 'x' to exit this example\n\n");


	while(1)
	{
		recvBuf = GetChar();

		if(recvBuf == 'x')
		{
			break;
		}
		else if(recvBuf == 's')
		{
			char	ch = 0;
			// enter setting menu
			DBG("Enter setting menu:\n");
			DBG("1. Timer\n");
			DBG("2. CountMode\n");
			DBG("3. Duty\n");
			DBG("4. OutputType\n");
			DBG("5. Frequency\n");
			DBG("p. Print Settings\n");
			DBG("g. go!\n");

			while(ch != 'g')
			{
				char			*str;

				ch = GetChar();
				DBG("%c\n", ch);
REINPUT:
				switch(ch)
				{
				case '1':
					{
						uint8_t		tempVal;
						DBG("1.Timer:");
						str = GetString();
						DBG("\n");
						tempVal = StrToI(str);
						if(tempVal < 3 || tempVal > 4)
						{
							DBG("Error input timer (3 ~ 4)\n");
							timer = timer;
							goto REINPUT;
						}
						else
						{
							timer = TIMER3 + tempVal - 3;
							goto GOHERE;
						}
					}
					break;
				case '2':
					{
						uint32_t		tempVal;
						DBG("2.CountMode: (0 ~ 5)");
						str = GetString();
						DBG("\n");
						tempVal = StrToI(str);
						if(tempVal > 5)
						{
							DBG("Error input CountMode\n");
							countMode = countMode;
							goto REINPUT;
						}
						else
						{
							countMode = tempVal;
							goto GOHERE;
						}
					}
					break;
				case '3':
					{
						uint32_t		tempVal;
						DBG("3.Duty:");
						str = GetString();
						DBG("\n");
						tempVal = StrToI(str);
						if(tempVal > 100)
						{
							DBG("Error input Duty (0 ~ 100)\n");
							duty = duty;
							goto REINPUT;
						}
						else
						{
							duty = tempVal;
							goto GOHERE;
						}
					}
					break;
				case '4':
					{
						uint32_t		tempVal;
						DBG("4.OutputType:");
						str = GetString();
						DBG("\n");
						tempVal = StrToI(str);
						if(tempVal < 4 || tempVal > 14)
						{
							DBG("Error input OutputType (4 ~ 7)\n");
							outputType = outputType;
						}
						else
						{
							outputType = tempVal;
							goto GOHERE;
						}
					}
					break;
				case '5':
					{
						uint32_t		tempVal;
						DBG("5.Frequency:");
						str = GetString();
						DBG("us\n");
						tempVal = StrToI(str);
						frequency = tempVal;
						goto GOHERE;
					}
					break;
				case 'p':
					{
						DBG("1. Timer%d\t2. Count Mode = %d\t3. Duty = %d\t4. Output Type = %d\t5. Frequency = %d us\n", timer+1, countMode, duty, outputType, frequency);
					}
					break;
				case 'g':
					{
						goto GOHERE;
					}
					break;
				default:
					{
						DBG("Error input\n");
					}
					break;
				}
			}
		}
	}
}

static void PWC_GpioConfigInit(TIMER_INDEX TimerIdx)
{
	if(TimerIdx == TIMER3)
	{
		PWC_GpioConfig(TIMER3_PWC1_A2_B22_A30_C7, 0, 1);
		DBG("PWC Init INPUT: A2\n");
	}

	if(TimerIdx == TIMER4)
	{
		PWC_GpioConfig(TIMER4_PWC1_A3_B23_A31_C8, 0, 1);
		DBG("PWC Init INPUT: A3\n");
	}
}


void TimerPWC1Example(void)
{

	char						recvBuf;
	PWC_StructInit				pwcParam;
	PWM_StructInit				pwmParam;
	uint8_t						pwcTimer = TIMER3;
	uint8_t						pwmTimer = TIMER4;

	uint8_t						polarity = PWC_POLARITY_FALLING;
	uint8_t						isSingle = PWC_CAPTURE_CONTINUES;
	uint8_t						dmaReqEnable = PWC_REQ_INTERRUPT_MODE;
	uint16_t					timeScale = 120;
	uint8_t						filterTime = 6;

	uint8_t						counterMode = PWM_COUNTER_MODE_DOWN;
	uint8_t						duty = 50;
	uint8_t						outputType = PWM_OUTPUT_SINGLE_1;
	uint32_t					frequency = 1000;		// 500 ms

RE_PWC1_EXAMPLE:
	TimerReset();
	// PWC setting
	pwcParam.Polarity[0]		= polarity;
	pwcParam.SingleGet			= isSingle;
	pwcParam.DMAReqEnable		= dmaReqEnable;
	pwcParam.TimeScale			= timeScale;
	pwcParam.FilterTime			= filterTime;

	PWC_GpioConfigInit(pwcTimer);
	PWC_Config(pwcTimer, &pwcParam, PWC_INPUT_CHANNEL_PWC1);


	// PWM setting
	PWM_GpioConfigDeInit(pwmTimer);
	PWM_GpioConfigInit(pwmTimer);

	pwmParam.CounterMode		= counterMode;
	pwmParam.Duty[0]			= duty;
	pwmParam.OutputType			= outputType;
	pwmParam.FreqDiv			= frequency * FREQUENCY_1US;
	pwmParam.DMAReqEnable		= PWM_REQ_INTERRUPT_MODE;
	pwmParam.OutEnable[0]		= OUTP_AND_OUTN_ENABLE;
	PWM_Config(pwmTimer, PWM1, &pwmParam);


	DBG("-------------------------------------------------------\n");
	DBG("\t\tTimerPWC1Example\n");
	DBG("Examp Menu:\n");
	DBG("x: exit this example\n");
	DBG("e: enable PWM & PWC\n");
	DBG("d: disable PWM & PWC\n");
	DBG("c: capture value\n");
	DBG("-------------------------------------------------------\n");
	while(1)
	{
		recvBuf = 0;
		recvBuf = GetChar();

		if(recvBuf == 'x')
		{
			break;
		}
		else if(recvBuf == 'e')
		{
			// enable PWM & PWC
			DBG("Enable PWM & PWC\n");
			PWC_Enable(pwcTimer);
			PWM_Enable(pwmTimer);
		}
		else if(recvBuf == 'd')
		{
			// disable PWM & PWC
			DBG("Disable PWM & PWC\n");
			PWC_Disable(pwcTimer);
			PWM_Disable(pwmTimer);
		}
		else if(recvBuf == 'c')
		{
			// capture value
			uint32_t	   pwcValue;
			PWC_IOCTRL_ARG Arg;

			pwcValue = PWC_IOCTRL(pwcTimer, PWC1, PWC_DATA_GET, &Arg);
			DBG("Capture Value : %d\n", pwcValue);
		}
		else if(recvBuf == 's')
		{
			// PWM & PWC setting menu

			// Add setting menu code here...


			goto RE_PWC1_EXAMPLE;
		}
	}
}

int main(void)
{
	uint8_t	recvBuf;
	// Timer基本功能展示
	//Disable watchDog
	WDG_Disable();

	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);

	SpiFlashInit(80000000, MODE_4BIT, 0, 1);


	GPIO_UartTxIoConfig(0, 1);//C2
	GPIO_UartRxIoConfig(0, 1);//C3
	UARTS_Init(0, 115200, 8, 0, 1);

	SysTickInit();

	GIE_ENABLE();

	RESTART:
	TimerReset();


	while(1)
	{
		DBG("-------------------------------------------------------\n");
		DBG("\t\tTimer Example\n");
		DBG("Examp Menu:\n");
		DBG("a: Timer2 Interrupt mode example\n");
		DBG("A: Timer2 Check mode example\n");
		DBG("b: Timer3 Interrupt mode example\n");
		DBG("B: Timer3 Check mode example\n");
		DBG("c: PWM example\n");
		DBG("d: PWC example\n");
		DBG("-------------------------------------------------------\n");

		recvBuf = 0;
		recvBuf = GetChar();

		if(recvBuf == 'a')
		{
			uint32_t		freq = 10000;
			//Timer2 中断模式Toggle GPIO A22
			//设置GPIOA22为输出
			GPIO_RegOneBitClear(GPIO_A_IE, GPIOA22);
			GPIO_RegOneBitSet(GPIO_A_OE, GPIOA22);

			DBG("Timer2 Interrupt mode, freq = %d ms\n", freq/1000);
			TimerReset();
			Timer2InterruptModeExample(freq); // 10ms
			goto RESTART;
		}
		else if(recvBuf == 'A')
		{
			uint32_t		freq = 20000;
			//Timer2 查询模式 Toggle GPIO A22
			//设置GPIOA22为输出
			GPIO_RegOneBitClear(GPIO_A_IE, GPIOA22);
			GPIO_RegOneBitSet(GPIO_A_OE, GPIOA22);

			DBG("Timer2 check mode, freq = %d ms\n", freq/1000);
			TimerReset();
			Timer2CheckModeExample(freq); // 20ms
			goto RESTART;
		}
		else if(recvBuf == 'b')
		{
			uint32_t		freq = 30000;
			// Timer3 中断模式 Toggle GPIO A22
			//设置GPIOA22为输出
			GPIO_RegOneBitClear(GPIO_A_IE, GPIOA22);
			GPIO_RegOneBitSet(GPIO_A_OE, GPIOA22);

			DBG("Timer3 Interrupt mode, freq = %d ms\n", freq/1000);
			TimerReset();
			Timer3InterruptModeExample(freq); // 30ms
			goto RESTART;
		}
		else if(recvBuf == 'B')
		{
			uint32_t		freq = 40000;
			// Timer3 查询模式Toggle GPIO A22
			//设置GPIOA22为输出
			GPIO_RegOneBitClear(GPIO_A_IE, GPIOA22);
			GPIO_RegOneBitSet(GPIO_A_OE, GPIOA22);

			DBG("Timer3 check mode, freq = %d ms\n", freq/1000);
			TimerReset();
			Timer3CheckModeExample(freq); //40 ms
			goto RESTART;
		}
		else if(recvBuf == 'c')
		{
			// PWM 输出例子
			TimerReset();
			TimerPWM1Example();
			goto RESTART;
		}
		else if(recvBuf == 'd')
		{
			// PWC 输入例子
			// 通过PWM 给 PWC 输入信号，由PWC抓取信号，得到数据
			TimerReset();
			TimerPWC1Example();
			goto RESTART;
		}
	}
}





