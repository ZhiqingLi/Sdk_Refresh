/**
 **************************************************************************************
 * @file    PWM_example.c
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
#include "gpio.h"
#include "uarts.h"
#include "timeout.h"
#include "pwm.h"
#include "delay.h"
#include "uarts_interface.h"
#include "cache_tcm.h"
#include "clk.h"
#include "watchdog.h"
#include "pwc.h"


#define UART_TX_IO	0
#define UART_RX_IO  0

char GetChar(void)
{
	uint8_t		ch = 0;
	while(ch == 0)
	{
		UARTS_Recv(UART_RX_IO, &ch, 1);
	}
	return ch;
}

void PwmForceOutputExample(void)
{
	TIMER_INDEX 	TimerIndex = TIMER5;
	PWM_INDEX   	PWMIndex   = PWM1;
	PWM_StructInit  PWMParam;
	PWM_OUTPUT_POLARITY_MODE PolarityIndex = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;

	uint8_t			recvBuf;

	DBG("PWM force output example\n");
	PWMParam.CounterMode   			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType    			= PWM_OUTPUT_SINGLE_FORCE_ACTIVE;
	PWMParam.DMAReqEnable  			= PWM_REQ_INTERRUPT_MODE;
	PWMParam.OutPolarity[PWMIndex]  = PolarityIndex;
	PWMParam.OutEnable[PWMIndex]    = OUTP_AND_OUTN_ENABLE;
	PWMParam.FreqDiv				= 12000; //120M / 12000 = 100us
	PWMParam.Duty[PWMIndex]			= 50;

	//GPIO Config
	PWM_GpioConfig(TIMER5_PWM1_P_A16, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER5_PWM1_N_A19, 0, PWM_IO_MODE_OUT);
	DBG("PWM GPIO P-A16, N-A19\n");

	//PWM Config
	PWM_Config(TimerIndex, PWMIndex, &PWMParam);

	//PWM Start
	PWM_Enable(TimerIndex);
	DBG("\nInput 'x' to exit this example\n\n");

	 while(1)
	 {
		 recvBuf = 0;
		 recvBuf = GetChar();

		 if(recvBuf == 'x')
		 {
			 break;
		 }
		 else if(recvBuf == 'p')
		 {
			 PolarityIndex++;
			 if(PolarityIndex > OUTP_ACTIVE_L_AND_OUTN_ACTIVE_L)
			 {
				 PolarityIndex = 0;
			 }
			 DBG("Polarity: %d\n", PolarityIndex);
			 PWMParam.OutPolarity[PWMIndex]  = PolarityIndex;
			 PWM_Config(TimerIndex, PWMIndex, &PWMParam);
			 PWM_Enable(TimerIndex);
		 }
	 }
}

void PwmCombined_OR_OutputExample(void)
{
	TIMER_INDEX 	TimerIndex = TIMER6;
	PWM_INDEX   	PWMIndex1  = PWM1;
	PWM_INDEX       PWMIndex2  = PWM2;
	PWM_StructInit  PWMParam;

	uint8_t			recvBuf;

	DBG("PWM combined(OR) output example\n");
	PWMParam.CounterMode   			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType    			= PWM_OUTPUT_COMBINED_PWM1_OR_PWM2;
	PWMParam.DMAReqEnable  			= PWM_REQ_INTERRUPT_MODE;
	PWMParam.OutPolarity[PWMIndex1] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutEnable[PWMIndex1]   = OUTP_AND_OUTN_ENABLE;
	PWMParam.OutPolarity[PWMIndex2] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutEnable[PWMIndex2]   = OUTP_AND_OUTN_ENABLE;
	PWMParam.FreqDiv				= 12000; //120M / 12000 = 100us
	PWMParam.Duty[PWMIndex1]		= 25;
	PWMParam.Duty[PWMIndex2]		= 50;

	//GPIO Config
	PWM_GpioConfig(TIMER6_PWM1_P_B6, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER6_PWM1_N_B5, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER6_PWM2_P_B4, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER6_PWM2_N_B3, 0, PWM_IO_MODE_OUT);
	DBG("PWM1 GPIO P-B6, N-B5, PWM2 GPIO P-B4, N-B3\n");

	//PWM Config
	PWM_Config(TimerIndex, PWMIndex1, &PWMParam);

	//PWM Start
	PWM_Enable(TimerIndex);
	DBG("\nInput 'x' to exit this example\n\n");


	 while(1)
	 {
		 recvBuf = 0;
		 recvBuf = GetChar();

		 if(recvBuf == 'x')
		 {
			 break;
		 }
		 else if(recvBuf == 'D')
		 {
			 PWM_IOCTRL_ARG Arg;

			 DBG("Change Duty1 = 10 and Duty2 = 40\n");
			 Arg.Duty = 10;
			 PWM_IOCTRl(TimerIndex, PWM1, OUTPUT_DUTY_UPDATE, &Arg);
			 Arg.Duty = 40;
			 PWM_IOCTRl(TimerIndex, PWM2, OUTPUT_DUTY_UPDATE, &Arg);
		 }
	 }
}

void PwmCombined_AND_OutputExample(void)
{
	TIMER_INDEX 	TimerIndex = TIMER6;
	PWM_INDEX   	PWMIndex1  = PWM3;
	PWM_INDEX       PWMIndex2  = PWM4;
	PWM_StructInit  PWMParam;

	uint8_t			recvBuf;

	DBG("PWM combined(AND) output example\n");
	PWMParam.CounterMode   			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType    			= PWM_OUTPUT_COMBINED_PWM3_AND_PWM4;
	PWMParam.DMAReqEnable  			= PWM_REQ_INTERRUPT_MODE;
	PWMParam.OutPolarity[PWMIndex1] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutEnable[PWMIndex1]   = OUTP_AND_OUTN_ENABLE;
	PWMParam.OutPolarity[PWMIndex2] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutEnable[PWMIndex2]   = OUTP_AND_OUTN_ENABLE;
	PWMParam.FreqDiv				= 12000; //120M / 12000 = 100us
	PWMParam.Duty[PWMIndex1]		= 25;
	PWMParam.Duty[PWMIndex2]		= 50;

	//GPIO Config
	PWM_GpioConfig(TIMER6_PWM3_P_B2, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER6_PWM3_N_B1, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER6_PWM4_P_B5, 0, PWM_IO_MODE_OUT);
	DBG("PWM3 GPIO P-B2, N-B1, PWM4 GPIO P-B5\n");

	//PWM Config
	PWM_Config(TimerIndex, PWMIndex1, &PWMParam);

	//PWM Start
	PWM_Enable(TimerIndex);

	DBG("\nInput 'x' to exit this example\n\n");
	 while(1)
	 {
		 recvBuf = 0;
		 recvBuf = GetChar();

		 if(recvBuf == 'x')
		 {
			 break;
		 }
		 else if(recvBuf == 'D')
		 {
			 PWM_IOCTRL_ARG Arg;

			 DBG("Change Duty1 = 10 and Duty2 = 40\n");
			 Arg.Duty = 10;
			 PWM_IOCTRl(TimerIndex, PWM3, OUTPUT_DUTY_UPDATE, &Arg);
			 Arg.Duty = 40;
			 PWM_IOCTRl(TimerIndex, PWM4, OUTPUT_DUTY_UPDATE, &Arg);
		 }
	 }
}

void PwmAsymmetricOutputExample(void)
{
	TIMER_INDEX 	TimerIndex1 = TIMER7;
	TIMER_INDEX 	TimerIndex2 = TIMER8;

	PWM_INDEX   	PWMIndex1  = PWM1;
	PWM_INDEX       PWMIndex2  = PWM2;
	PWM_INDEX   	PWMIndex3  = PWM3;
	PWM_INDEX       PWMIndex4  = PWM4;
	PWM_StructInit  PWMParam;

	uint8_t			recvBuf;

	DBG("PWM asymmetric output example\n");
	//GPIO Config
	PWM_GpioConfig(TIMER7_PWM1_P_C4, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER7_PWM3_P_C6, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER8_PWM1_P_B9, 0, PWM_IO_MODE_OUT);

	DBG("TIMER7 PWM1 GPIO P-C4, PWM3 GPIO P-C6, TIMER8 PWM1 GPIO P-B9\n");

	PWMParam.CounterMode   			= PWM_COUNTER_MODE_CENTER_ALIGNED1;
	PWMParam.OutputType    			= PWM_OUTPUT_ASYMMETRIC_1;
	PWMParam.DMAReqEnable  			= PWM_REQ_INTERRUPT_MODE;
	PWMParam.OutPolarity[PWMIndex1] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutPolarity[PWMIndex2] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutPolarity[PWMIndex3] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutPolarity[PWMIndex4] = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWMParam.OutEnable[PWMIndex1]   = OUTP_ENABLE_AND_OUTN_DISABLE;
	PWMParam.OutEnable[PWMIndex2]   = OUTP_ENABLE_AND_OUTN_DISABLE;
	PWMParam.OutEnable[PWMIndex3]   = OUTP_ENABLE_AND_OUTN_DISABLE;
	PWMParam.OutEnable[PWMIndex4]   = OUTP_ENABLE_AND_OUTN_DISABLE;
	PWMParam.FreqDiv				= 12000; //120M / 12000 = 100us
	PWMParam.Duty[PWMIndex1]		= 33;
	PWMParam.Duty[PWMIndex2]		= 67;
	PWMParam.Duty[PWMIndex3]		= 67;
	PWMParam.Duty[PWMIndex4]		= 33;

	//TimerIndex1 PWM Config
	PWM_Config(TimerIndex1, PWMIndex1, &PWMParam);

	PWMParam.Duty[PWMIndex1]		= 100;
	PWMParam.Duty[PWMIndex2]		= 0;

	//TimerIndex2 PWM Config
	PWM_Config(TimerIndex2, PWMIndex1, &PWMParam);

	//sync
	Timer_OutputTriggerSelect(TimerIndex1, OUTPUT_TRIGGER_ENABLE);
	Timer_InputTriggerSelect(TimerIndex2, INPUT_TRIGGER_0);

	//PWM Start
	PWM_Enable(TimerIndex2);
	PWM_Enable(TimerIndex1);
	DBG("\nInput 'x' to exit this example\n\n");
	 while(1)
	 {
		 recvBuf = 0;
		 recvBuf = GetChar();

		 if(recvBuf == 'x')
		 {
			 break;
		 }
	 }
}

void PwmOnePulseOutputExample(void)
{
	TIMER_INDEX 	TimerIndex = TIMER5;
	PWM_INDEX   	PWMIndex    = PWM1;
	PWM_StructInit  PWMParam;
	PWM_OUTPUT_POLARITY_MODE PolarityIndex = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;

	uint8_t			recvBuf;

	DBG("PWM one pulse output example\n");
	PWMParam.CounterMode   			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType    			= PWM_OUTPUT_ONE_PULSE;//;
	PWMParam.DMAReqEnable  			= PWM_REQ_INTERRUPT_MODE;
	PWMParam.OutPolarity[PWMIndex]  = PolarityIndex;
	PWMParam.OutEnable[PWMIndex]    = OUTP_AND_OUTN_ENABLE;
	PWMParam.FreqDiv				= 120000; //120M / 120000 = 1000us
	PWMParam.Duty[PWMIndex]			= 50;

	//GPIO Config
	PWM_GpioConfig(TIMER5_PWM1_P_A16, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER5_PWM1_N_A19, 0, PWM_IO_MODE_OUT);
	DBG("TIMER5 PWM GPIO P-A16, N-A19\n");

	//PWM Config
	PWM_Config(TimerIndex, PWMIndex, &PWMParam);

	//PWM Start
	PWM_Enable(TimerIndex);
	DBG("\nInput 'x' to exit this example\n\n");
	 while(1)
	 {
		 recvBuf = 0;
		 recvBuf = GetChar();

		 if(recvBuf == 'x')
		 {
			 break;
		 }
		 else if(recvBuf == 'p')
		 {
			 PolarityIndex++;
			 if(PolarityIndex > OUTP_ACTIVE_L_AND_OUTN_ACTIVE_L)
			 {
				 PolarityIndex = 0;
			 }
			 DBG("Polarity: %d\n", PolarityIndex);
			 PWMParam.OutPolarity[PWMIndex]  = PolarityIndex;
			 PWM_Config(TimerIndex, PWMIndex, &PWMParam);
			 PWM_Enable(TimerIndex);
		 }
		 else if(recvBuf == 's')
		 {
			DBG("Start\n");
			PWM_Enable(TimerIndex);
		 }
	 }
}

void PwmBreakInAndDeadTimeExample(void)
{
	TIMER_INDEX 	TimerIndex = TIMER8;
	PWM_INDEX   	PWMIndex   = PWM1;
	PWM_StructInit  PWMParam;
	BDTInfo         BreakDeadTimeParam;
	PWM_IOCTRL_ARG	Arg;
	BREAK_IN_STATE_MODE            BreakOffStateSel = 0;
	BREAK_IN_EXIT_TRIGGER_MODE     AutoExitIdleEnable = 0;
	uint8_t			recvBuf;

	PWM_OUTPUT_POLARITY_MODE PolarityIndex = OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H;
	PWM_IDLE_STATE_SEL IdleStateIndex = IDLE_P_LOW_AND_N_LOW;

	DBG("PWM break in and dead time example\n");

	//TIMER5 GPIO Config
//	PWM_GpioConfig(TIMER5_PWM1_P_A16, 0, PWM_IO_MODE_OUT);
//	PWM_GpioConfig(TIMER5_PWM1_N_A19, 0, PWM_IO_MODE_OUT);
//	DBG("TIMER5 PWM GPIO P-A16, N-A19\n");
//	PWM_BreakGpioConfig(TIMER5_BREAK_A15, PWM_IO_MODE_OUT);
//	DBG("TIMER5 Break GPIO A15\n");

	//TIMER8 GPIO Config
	PWM_GpioConfig(TIMER8_PWM1_P_B9, 0, PWM_IO_MODE_OUT);
	PWM_GpioConfig(TIMER8_PWM1_N_B8, 0, PWM_IO_MODE_OUT);
	DBG("TIMER8 PWM GPIO P-B9, N-B8\n");
	PWM_BreakGpioConfig(TIMER8_BREAK_B7, PWM_IO_MODE_OUT);
	DBG("TIMER8 Break GPIO B7\n");



	//PWM Config
	PWMParam.CounterMode   			= PWM_COUNTER_MODE_UP;
	PWMParam.OutputType    			= PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable  			= PWM_REQ_INTERRUPT_MODE;
	PWMParam.OutPolarity[PWMIndex]  = PolarityIndex;
	PWMParam.OutIdleState[PWMIndex] = IdleStateIndex;
	PWMParam.OutEnable[PWMIndex]    = OUTP_AND_OUTN_ENABLE;
	PWMParam.FreqDiv				= 12000; //120M / 12000 = 100us
	PWMParam.Duty[PWMIndex]			= 50;
	PWM_Config(TimerIndex, PWMIndex, &PWMParam);

	//break config
	BreakDeadTimeParam.DTFreqDiv    = SYS_CLK_DIV_4;
	BreakDeadTimeParam.DTCycle		= 200;

	BreakDeadTimeParam.FilterTime	= 3;
	BreakDeadTimeParam.BreakPolar   = BREAK_POLARITY_LOW;
	BreakDeadTimeParam.BreakOffStateSel = BreakOffStateSel;
	BreakDeadTimeParam.AutoExitIdleEnable = AutoExitIdleEnable;

	PWM_DeadTimeConfig(TimerIndex, &BreakDeadTimeParam);
	PWM_DeadTimeEnable(TimerIndex);
	PWM_BreakInConfig(TimerIndex, &BreakDeadTimeParam);
	PWM_BreakInEnable(TimerIndex);

	PWM_Enable(TimerIndex);
	DBG("\nInput 'x' to exit this example\n\n");
	while(1)
	 {
		 recvBuf = 0;
		 recvBuf = GetChar();

		 if(recvBuf == 'x')
		 {
			 break;
		 }
		 else if(recvBuf == 'p')
		 {
			 PolarityIndex++;
			 if(PolarityIndex > OUTP_ACTIVE_L_AND_OUTN_ACTIVE_L)
			 {
				 PolarityIndex = 0;
			 }
			 DBG("Polarity: %d\n", PolarityIndex);
			 Arg.OutPolarity = PolarityIndex;
			 PWM_IOCTRl(TimerIndex, PWMIndex, OUTPUT_POLARITY_UPDATE, &Arg);
		 }
		 else if(recvBuf == 'i')
		 {
			 IdleStateIndex ++;
			 if(IdleStateIndex > IDLE_P_HIGH_AND_N_HIGH)
			 {
				 IdleStateIndex = 0;
			 }
			 DBG("idle state: %d\n", IdleStateIndex);
			 PWMParam.OutIdleState[PWMIndex] = IdleStateIndex;
			 PWM_Config(TimerIndex, PWMIndex, &PWMParam);
			 PWM_IOCTRl(TimerIndex, PWMIndex, OUTPUT_SOFTWARE_UPDATE, &Arg);
			 PWM_Enable(TimerIndex);
		 }
		 else if(recvBuf == 'u')
		 {
			 DBG("UPDATE\n");
			 PWM_IOCTRl(TimerIndex, PWMIndex, OUTPUT_SOFTWARE_UPDATE, &Arg);
		 }
		 else if(recvBuf == 'b')
		 {
			 DBG("Break in\n");
			 GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA22);
			 GPIO_RegOneBitSet(GPIO_A_TGL, GPIOA23);
		 }
	 }
}

int main(void)
{
	uint8_t	recvBuf;

	WDG_Disable();

	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);

#if  1    //OSC_12M
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);	//PU=0,PD=1:模拟输入
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);	//PU=0,PD=1:模拟
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);

	Clock_Config(1, 12000000);//用12M时钟需要将B30、B31改为模拟输入
#else//32K
	Clock_Config(1, 32768);
#endif
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);


	GPIO_UartTxIoConfig(0, 1);//C2
	GPIO_UartRxIoConfig(0, 1);//C3
	UARTS_Init(0, 115200, 8, 0, 1);

	SysTickInit();

	GIE_ENABLE();

RESTART:
	//TimerReset();

	while(1)
	{
		DBG("-------------------------------------------------------\n");
		DBG("\t\tPWM Example\n");
		DBG("Example Menu:\n");
		DBG("1: Force output example\n");
		DBG("2: combined(OR) mode example\n");
		DBG("3: combined(AND) mode example\n");
		DBG("4: Asymmetric mode example\n");
		DBG("5: One pulse output example\n");
		DBG("6: Break in and dead time example\n");
		DBG("-------------------------------------------------------\n");
		recvBuf = 0;
		recvBuf = GetChar();

		if(recvBuf == '1')
		{
			PwmForceOutputExample();
			goto RESTART;
		}
		else if(recvBuf == '2')
		{
			PwmCombined_OR_OutputExample();
		}
		else if(recvBuf == '3')
		{
			PwmCombined_AND_OutputExample();
		}
		else if(recvBuf == '4')
		{
			PwmAsymmetricOutputExample();
		}
		else if(recvBuf == '5')
		{
			PwmOnePulseOutputExample();
		}
		else if(recvBuf == '6')
		{
			PwmBreakInAndDeadTimeExample();
		}
	}
}



