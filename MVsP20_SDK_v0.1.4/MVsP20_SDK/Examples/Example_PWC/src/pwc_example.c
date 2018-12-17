/**
 **************************************************************************************
 * @file    PWC_example.c
 *
 * @author  Grayson Chen
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
#include "pwc.h"
#include "dma.h"
#include "watchdog.h"


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



#define PWC_UART_TX_IO	0
#define PWC_UART_RX_IO  0

void PwcTimerReset(void)
{

}


static void PWC_GpioConfigInit(TIMER_INDEX TimerIdx, PWC_INDEX PwcIdx)
{
    if(TimerIdx == TIMER3)
    {
        PWC_GpioConfig(TIMER3_PWC1_A2_B22_A30_C7, 0, PWC_IO_MODE_IN);
        DBG("PWC INPUT: A2_B22\n");
    }

    if(TimerIdx == TIMER4)
    {
        PWC_GpioConfig(TIMER4_PWC1_A3_B23_A31_C8, 0, PWC_IO_MODE_IN);
        DBG("PWC INPUT: A3_B23\n");
    }

    if(TimerIdx == TIMER5)
    {
        if(PwcIdx == PWC1)
        {
            PWC_GpioConfig(TIMER5_PWC1_A16, 0, PWC_IO_MODE_IN);
            DBG("PWC1 INPUT: A16\n");
        }

        if(PwcIdx == PWC2)
        {
            PWC_GpioConfig(TIMER5_PWC2_A17, 0, PWC_IO_MODE_IN);
            DBG("PWC2 INPUT: A17\n");
        }

        if(PwcIdx == PWC3)
        {
            PWC_GpioConfig(TIMER5_PWC3_A18, 0, PWC_IO_MODE_IN);
            DBG("PWC3 INPUT: A18\n");
        }

        if(PwcIdx == PWC4)
        {
            PWC_GpioConfig(TIMER5_PWC4_A19, 0, PWC_IO_MODE_IN);
            DBG("PWC4 INPUT: A19\n");
        }
    }

    if(TimerIdx == TIMER6)
    {
        if(PwcIdx == PWC1)
        {
            PWC_GpioConfig(TIMER6_PWC1_B6, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: B6\n");
        }

        if(PwcIdx == PWC2)
        {
            PWC_GpioConfig(TIMER6_PWC2_B4, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: B4\n");
        }

        if(PwcIdx == PWC3)
        {
            PWC_GpioConfig(TIMER6_PWC3_B2, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: B2\n");
        }

        if(PwcIdx == PWC4)
        {
            PWC_GpioConfig(TIMER6_PWC4_B5, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: B5\n");
        }
    }

    if(TimerIdx == TIMER7)
    {
        if(PwcIdx == PWC1)
        {
            PWC_GpioConfig(TIMER7_PWC1_C4, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: C4\n");
        }

        if(PwcIdx == PWC2)
        {
            PWC_GpioConfig(TIMER7_PWC2_C5, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: C5\n");
        }

        if(PwcIdx == PWC3)
        {
            PWC_GpioConfig(TIMER7_PWC3_C6, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: C6\n");
        }

        if(PwcIdx == PWC4)
        {
            PWC_GpioConfig(TIMER7_PWC4_C3, 0, PWC_IO_MODE_IN);
            DBG("PWC INPUT: C3\n");
        }
    }

    if(TimerIdx == TIMER8)
	{
		if(PwcIdx == PWC1)
		{
			PWC_GpioConfig(TIMER8_PWC1_B9, 0, PWC_IO_MODE_IN);
			DBG("PWC INPUT: B9\n");
		}

		if(PwcIdx == PWC2)
		{
			PWC_GpioConfig(TIMER8_PWC2_B11, 0, PWC_IO_MODE_IN);
			DBG("PWC INPUT: B11\n");
		}

		if(PwcIdx == PWC3)
		{
			PWC_GpioConfig(TIMER8_PWC3_B13, 0, PWC_IO_MODE_IN);
			DBG("PWC INPUT: B13\n");
		}

		if(PwcIdx == PWC4)
		{
			PWC_GpioConfig(TIMER8_PWC4_B8, 0, PWC_IO_MODE_IN);
			DBG("PWC INPUT: B8\n");
		}
	}
}

static void PWM_GpioConfigInit(TIMER_INDEX TimerIdx, PWM_INDEX PWMIdx)
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

		if(TimerIdx == TIMER5)
		{
			if(PWMIdx == PWM1)
			{
				PWM_GpioConfig(TIMER5_PWM1_N_A19, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER5_PWM1_P_A16, 0, PWM_IO_MODE_OUT);
				DBG("PWM1 Init OUTPUT: A19_N, A16_P\n");
			}

			if(PWMIdx == PWM2)
			{
				PWM_GpioConfig(TIMER5_PWM2_N_A20, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER5_PWM2_P_A17, 0, PWM_IO_MODE_OUT);
				DBG("PWM2 Init OUTPUT: A20_N, A17_P\n");
			}

			if(PWMIdx == PWM3)
			{
				PWM_GpioConfig(TIMER5_PWM3_N_A21, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER5_PWM3_P_A18, 0, PWM_IO_MODE_OUT);
				DBG("PWM3 Init OUTPUT: A21_N, A18_P\n");
			}

			if(PWMIdx == PWM4)
			{
				PWM_GpioConfig(TIMER5_PWM4_P_A19, 0,  PWM_IO_MODE_OUT);
				DBG("PWM4 Init OUTPUT: A19_P,\n");
			}
		}

		if(TimerIdx == TIMER6)
		{
			if(PWMIdx == PWM1)
			{
				PWM_GpioConfig(TIMER6_PWM1_N_B5, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER6_PWM1_P_B6, 0, PWM_IO_MODE_OUT);
				DBG("PWM1 Init OUTPUT: B5_N, B6_P\n");
			}

			if(PWMIdx == PWM2)
			{
				PWM_GpioConfig(TIMER6_PWM2_N_B3, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER6_PWM2_P_B4, 0, PWM_IO_MODE_OUT);
				DBG("PWM2 Init OUTPUT: B3_N, B4_P\n");
			}

			if(PWMIdx == PWM3)
			{
				PWM_GpioConfig(TIMER6_PWM3_N_B1, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER6_PWM3_P_B2, 0, PWM_IO_MODE_OUT);
				DBG("PWM3 Init OUTPUT: B1_N, B2_P\n");
			}

			if(PWMIdx == PWM4)
			{
				PWM_GpioConfig(TIMER6_PWM4_P_B5, 0, PWM_IO_MODE_OUT);
				DBG("PWM4 Init OUTPUT: B5_P\n");
			}
		}
		if(TimerIdx == TIMER7)
		{
			if(PWMIdx == PWM1)
			{
				PWM_GpioConfig(TIMER7_PWM1_N_C3, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER7_PWM1_P_C4, 0, PWM_IO_MODE_OUT);
				DBG("PWM1 Init OUTPUT: C3_N, C4_P\n");
			}

			if(PWMIdx == PWM2)
			{
				PWM_GpioConfig(TIMER7_PWM2_N_C2, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER7_PWM2_P_C5, 0, PWM_IO_MODE_OUT);
				DBG("PWM2 Init OUTPUT: C2_N, C5_P\n");
			}

			if(PWMIdx == PWM3)
			{
				PWM_GpioConfig(TIMER7_PWM3_N_C1, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER7_PWM3_P_C6, 0, PWM_IO_MODE_OUT);
				DBG("PWM3 Init OUTPUT: C1_N, C6_P\n");
			}

			if(PWMIdx == PWM4)
			{
				PWM_GpioConfig(TIMER7_PWM4_P_C3, 0, PWM_IO_MODE_OUT);
				DBG("PWM4 Init OUTPUT: C3_P\n");
			}
		}

		if(TimerIdx == TIMER8)
		{
			if(PWMIdx == PWM1)
			{
				PWM_GpioConfig(TIMER8_PWM1_N_B8, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER8_PWM1_P_B9, 0, PWM_IO_MODE_OUT);
				DBG("PWM1 Init OUTPUT: B8_N, B9_P\n");
			}

			if(PWMIdx == PWM2)
			{
				PWM_GpioConfig(TIMER8_PWM2_N_B10, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER8_PWM2_P_B11, 0, PWM_IO_MODE_OUT);
				DBG("PWM2 Init OUTPUT: B10_N, B11_P\n");
			}

			if(PWMIdx == PWM3)
			{
				PWM_GpioConfig(TIMER8_PWM3_N_B12, 0, PWM_IO_MODE_OUT);
				PWM_GpioConfig(TIMER8_PWM3_P_B13, 0, PWM_IO_MODE_OUT);
				DBG("PWM3 Init OUTPUT: B12_N, B13_P\n");
			}

			if(PWMIdx == PWM4)
			{
				PWM_GpioConfig(TIMER8_PWM4_P_B8, 0, PWM_IO_MODE_OUT);
				DBG("PWM4 Init OUTPUT: B8_P\n");
			}
		}
}

__attribute__((section(".driver.isr"))) void Timer5Interrupt(void)
{
    uint32_t PWCValue;
    uint32_t ret;

    ret = Timer_InterruptFlagGet(TIMER5,PWC1_CAP_DATA_INTERRUPT_SRC | PWC2_CAP_DATA_INTERRUPT_SRC | \
    									PWC3_CAP_DATA_INTERRUPT_SRC | PWC4_CAP_DATA_INTERRUPT_SRC |\
    									PWC_OVER_RUN_INTERRUPT_SRC  | UPDATE_INTERRUPT_SRC);

    if(ret & PWC1_CAP_DATA_INTERRUPT_SRC)
    {
    	PWCValue = PWC_IOCTRL(TIMER5, PWC1, PWC_DATA_GET, NULL);//读比较值，自动清CAP中断
    	DBG("\n> Get value1 : %d\n", PWCValue);
    }
    if(ret & PWC2_CAP_DATA_INTERRUPT_SRC)
    {
    	PWCValue = PWC_IOCTRL(TIMER5, PWC2, PWC_DATA_GET, NULL);//读比较值，自动清CAP中断
    	DBG("\n> Get value2 : %d\n", PWCValue);
    }

    if(ret & PWC3_CAP_DATA_INTERRUPT_SRC)
    {
    	PWCValue = PWC_IOCTRL(TIMER5, PWC3, PWC_DATA_GET, NULL);//读比较值，自动清CAP中断
    	DBG("\n> Get value3 : %d\n", PWCValue);
    }

    if(ret & PWC4_CAP_DATA_INTERRUPT_SRC)
    {
    	PWCValue = PWC_IOCTRL(TIMER5, PWC4, PWC_DATA_GET, NULL);//读比较值，自动清CAP中断
    	DBG("\n> Get value4 : %d\n", PWCValue);
    }

    if(ret & PWC_OVER_RUN_INTERRUPT_SRC)//捕获定时器溢出
	{
		Timer_InterruptFlagClear(TIMER5,PWC_OVER_RUN_INTERRUPT_SRC);	  //清中断标志
		DBG("\nTIMER5 PWC_OVER_RUN\n");
	}

    if(ret & UPDATE_INTERRUPT_SRC)//update interrupt
	{
		Timer_InterruptFlagClear(TIMER5,UPDATE_INTERRUPT_SRC);
	}
}



void PwcSingleOrContinueMode(bool IsSingle)
{
    TIMER_INDEX TimerIdx = TIMER5;
    uint8_t     PolarIdx = PWC_POLARITY_RAISING;
    uint8_t     TimeScaleIdx = 2;
    uint8_t     PWMFreqIdx   = 2;
    TIMER_INDEX PWMIdx;

    uint32_t    TimeScaleTbl[]  = {1, 10, 100, 1000};
    uint32_t    PWMFreqTbl[4][3] = {
                                    {20,       800,       4000},
                                    {400,      4000,      80000},
                                    {100000,   4000000,   800000},
                                    {200000,   8000000,   50000000},
                                   };


    PWC_StructInit  PWCParam;
    PWM_StructInit  PWMParam;

    uint8_t IrqTbl[8] = {TMR1_IRQn, TMR2_IRQn, TMR3_IRQn, TMR4_IRQn, TMR5_IRQn, TMR6_IRQn, TMR7_IRQn, TMR8_IRQn};

    uint32_t PWCValue;
    uint8_t  Buf[10];

    PwcTimerReset();

    //查询方式
	DBG("\n> Timer%d, PolarIdx: %d,Time Scale: %d,FreqDiv: %d\n", TimerIdx+1, PolarIdx,TimeScaleTbl[TimeScaleIdx],PWMFreqTbl[TimeScaleIdx][PWMFreqIdx]);
	PWMIdx = (TimerIdx == TIMER3? TIMER6:TIMER3);

	//PWC复用GPIO配置
	PWC_GpioConfigInit(TimerIdx, PWC1);

	//PWM复用GPIO配置
	PWM_GpioConfigInit(PWMIdx, PWM1);

	//PWC参数配置
	PWCParam.PWC1Source      = PWC1_INPUT_PWC1_ONLY;// PWC1直接输入
	PWCParam.Polarity[PWC1]  = PolarIdx;
	PWCParam.SingleGet       = IsSingle;//IsSingle;
	PWCParam.DMAReqEnable    = 0;
	PWCParam.TimeScale       = TimeScaleTbl[TimeScaleIdx];
	PWCParam.FilterTime      = 3;
	//PWM参数配置
	PWMParam.CounterMode        = PWM_COUNTER_MODE_UP;
	PWMParam.OutputType         = PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable       = 0;
	PWMParam.FreqDiv            = PWMFreqTbl[TimeScaleIdx][PWMFreqIdx];
	PWMParam.Duty[PWC1]         = 50;
	PWMParam.OutEnable[PWC1]    = OUTP_AND_OUTN_ENABLE;

	PWM_Config(PWMIdx, PWM1, &PWMParam);
	PWM_Enable(PWMIdx);

	PWC_Config(TimerIdx, &PWCParam,PWC_INPUT_CHANNEL_PWC1);
//	PWC_Enable(TimerIdx);

	DBG("-----------------PWC Mode:Check------------------------\n");
	DBG("Examp Menu:\n");
	DBG("x: exit \n");
	DBG("s: Start\n");
	DBG("p: Pause\n");
	DBG("r: Resume\n");
	DBG("-------------------------------------------------------\n");

	while(1)
	{
		if(PWC_IOCTRL(TimerIdx, PWC1,PWC_DONE_STATUS_GET,NULL))
		{
			PWCValue = PWC_IOCTRL(TimerIdx, PWC1, PWC_DATA_GET, NULL);
			DBG("\n> Get value : %d\n", PWCValue);
		}
		Buf[0] = 0x0;
		UARTS_Recv(PWC_UART_RX_IO, Buf, 1);
		if(Buf[0] == 'x')
		{
			DBG("x\n");
			PWC_Disable(TimerIdx);
			break;
		}
		if(Buf[0] == 's')
		{
			DBG("Start\n");
			PWC_Enable(TimerIdx);
		}
		if(Buf[0] == 'p')
		{
			DBG("Pause\n");
			Timer_Pause(TimerIdx, 1);
		}
		if(Buf[0] == 'r')
		{
			DBG("Resume\n");
			Timer_Pause(TimerIdx, 0);
		}
	}



	DBG("\n> Timer%d, PolarIdx: %d,Time Scale: %d,FreqDiv: %d\n", TimerIdx+1, PolarIdx,TimeScaleTbl[TimeScaleIdx],PWMFreqTbl[TimeScaleIdx][PWMFreqIdx]);
	DBG("-----------------PWC Mode:Interrupt--------------------\n");
	DBG("Examp Menu:\n");
	DBG("x: exit \n");
	DBG("s: Start\n");
	DBG("p: Pause\n");
	DBG("r: Resume\n");
	DBG("-------------------------------------------------------\n");

    //中断方式

	NVIC_DisableIRQ(TMR3_IRQn);
	NVIC_DisableIRQ(TMR4_IRQn);
	NVIC_DisableIRQ(TMR5_IRQn);
	NVIC_DisableIRQ(TMR6_IRQn);
	NVIC_DisableIRQ(TMR7_IRQn);
	NVIC_DisableIRQ(TMR8_IRQn);

	NVIC_EnableIRQ(IrqTbl[TimerIdx]);
	PWMIdx = (TimerIdx == TIMER3? TIMER6:TIMER3);

	PWC_GpioConfigInit(TimerIdx, PWC1);//PWC复用GPIO配置
	PWM_GpioConfigInit(PWMIdx, PWM1);//PWM复用GPIO配置


	//PWC参数配置
	PWCParam.PWC1Source      = PWC1_INPUT_PWC1_ONLY;// PWC1作为输入源
	PWCParam.Polarity[PWC1]  = PolarIdx;
	PWCParam.SingleGet       = IsSingle;//IsSingle;
	PWCParam.DMAReqEnable    = 0;
	PWCParam.TimeScale       = TimeScaleTbl[TimeScaleIdx];
	PWCParam.FilterTime      = 3;

	//PWM参数配置
	PWMParam.CounterMode       = PWM_COUNTER_MODE_UP;
	PWMParam.OutputType        = PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable      = 0;
	PWMParam.FreqDiv           = PWMFreqTbl[TimeScaleIdx][PWMFreqIdx];
	PWMParam.Duty[PWC1]         = 50;
	PWMParam.OutEnable[PWC1]    = 3;

	PWM_Config(PWMIdx, PWM1, &PWMParam);
	PWM_Enable(PWMIdx);

	PWC_Config(TimerIdx, &PWCParam,PWC_INPUT_CHANNEL_PWC1);
	Timer_InterrputSrcEnable(TimerIdx, PWC1_CAP_DATA_INTERRUPT_SRC);
//	PWC_Enable(TimerIdx);

	while(1)
	{
		Buf[0] = 0x0;
		UARTS_Recv(PWC_UART_RX_IO, Buf, 1);
		if(Buf[0] == 'x')
		{
			DBG("x\n");
			PWC_Disable(TimerIdx);
			break;
		}
		if(Buf[0] == 's')
		{
			DBG("Start\n");
			PWC_Enable(TimerIdx);
		}
		if(Buf[0] == 'p')
		{
			DBG("Pause\n");
			Timer_Pause(TimerIdx, 1);
		}
		if(Buf[0] == 'r')
		{
			DBG("Resume\n");
			Timer_Pause(TimerIdx, 0);
		}
	}

}



void PwcSimultaneWorkMode(void)
{
    TIMER_INDEX     TimerIdx = TIMER5;
    PWC_StructInit  PWCParam;
    PWM_StructInit  PWMParam;

//    uint32_t        PWCValue1;
//    uint32_t        PWCValue2;
    uint8_t         Buf[10];
    uint32_t        FreqDivVal = 4000000;
    uint32_t        DutyVal = 60;
    uint32_t        TimeScale = 100;
    //1. 两个同时工作
	//PWC复用GPIO配置
	PWC_GpioConfigInit(TimerIdx, PWC1);
	PWC_GpioConfigInit(TimerIdx, PWC2);

	//PWM复用GPIO配置
	PWM_GpioConfigInit(TIMER3, PWM1);

	memset(&PWCParam,0,sizeof(PWC_StructInit));
	memset(&PWMParam,0,sizeof(PWM_StructInit));

	//PWC参数配置
	PWCParam.PWC1Source      = PWC1_INPUT_PWC1_ONLY;// PWC1直接输入
	PWCParam.Polarity[PWC1]  = PWC_POLARITY_RAISING;
	PWCParam.Polarity[PWC2]  = PWC_POLARITY_FALLING;
	PWCParam.SingleGet       = PWC_CAPTURE_CONTINUES;
	PWCParam.DMAReqEnable    = 0;
	PWCParam.TimeScale       = TimeScale;
	PWCParam.FilterTime      = 3;


	//PWM参数配置
	PWMParam.CounterMode     = PWM_COUNTER_MODE_UP;
	PWMParam.OutputType      = PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable    = 0;
	PWMParam.FreqDiv         = FreqDivVal;
	PWMParam.Duty[PWM1]      = DutyVal;
	PWMParam.OutEnable[PWM1] = 1;

	PWM_Config(TIMER3, PWM1, &PWMParam);
	PWM_Enable(TIMER3);

	PWC_Config(TimerIdx, &PWCParam,PWC_INPUT_CHANNEL_PWC1_PWC2);
//	PWC_Enable(TimerIdx);

	NVIC_EnableIRQ(TMR5_IRQn);
	Timer_InterrputSrcEnable(TimerIdx, PWC1_CAP_DATA_INTERRUPT_SRC | PWC2_CAP_DATA_INTERRUPT_SRC);

	DBG("PWM TimeScale:%d,FreqDiv:%d,Duty:%d\n",TimeScale,FreqDivVal,DutyVal);

	DBG("-----------------PWC Mode:Simultaneous Work------------\n");
	DBG("Examp Menu:\n");
	DBG("x: exit \n");
	DBG("s: Start\n");
	DBG("p: Pause\n");
	DBG("r: Resume\n");
	DBG("-------------------------------------------------------\n");

	while(1)
	{
//		if(PWC_IOCTRL(TimerIdx, PWC1,PWC_DONE_STATUS_GET,NULL))//查询模式
//		{
//			if(PWC_IOCTRL(TimerIdx, PWC2,PWC_DONE_STATUS_GET,NULL))
//			{
//				PWCValue1 = PWC_IOCTRL(TimerIdx, PWC1, PWC_DATA_GET, NULL);
//				PWCValue2 = PWC_IOCTRL(TimerIdx, PWC2, PWC_DATA_GET, NULL);
//				DBG("\n> Get value : %d, %d\n", PWCValue1, PWCValue2);
//			}
//		}
		Buf[0] = 0x0;
		UARTS_Recv(PWC_UART_RX_IO, Buf, 1);
		if(Buf[0] == 'x')
		{
			DBG("x\n");
			PWC_Disable(TimerIdx);
			break;
		}
		if(Buf[0] == 's')
		{
			DBG("Start\n");
			PWC_Enable(TimerIdx);
		}
		if(Buf[0] == 'p')
		{
			DBG("Pause\n");
			Timer_Pause(TimerIdx, 1);
		}
		if(Buf[0] == 'r')
		{
			DBG("Resume\n");
			Timer_Pause(TimerIdx, 0);
		}
	}
}



void PwcXorMode(void)
{
    TIMER_INDEX     TimerIdx = TIMER5;
    uint8_t         PWMIdx1;
    PWC_StructInit  PWCParam;
    PWM_StructInit  PWMParam;
    uint8_t         Buf[10];
    uint32_t        FreqDivVal = 12000000;
    uint32_t        TimeScale = 120;
   //中断方式
    NVIC_EnableIRQ(TMR5_IRQn);

	if(TimerIdx == TIMER5)
	{
		PWMIdx1 = TIMER6;
	}

	//PWM复用GPIO配置
	PWM_GpioConfigInit(PWMIdx1, PWM1);
	PWM_GpioConfigInit(PWMIdx1, PWM2);
	PWM_GpioConfigInit(PWMIdx1, PWM3);


	memset(&PWCParam,0,sizeof(PWC_StructInit));
	memset(&PWMParam,0,sizeof(PWM_StructInit));

	//PWM2参数配置
	PWMParam.CounterMode     = PWM_COUNTER_MODE_CENTER_ALIGNED1;
	PWMParam.OutputType      = PWM_OUTPUT_SINGLE_1;
	PWMParam.DMAReqEnable    = 0;
	PWMParam.FreqDiv         = FreqDivVal;
//	PWMParam.Duty[PWM1]      = 16;
	PWMParam.OutEnable[PWM1] = OUTP_AND_OUTN_ENABLE;
	PWM_Config(PWMIdx1, PWM1, &PWMParam);
//	PWMParam.Duty[PWM2]      = 50;
	PWMParam.OutEnable[PWM2] = OUTP_AND_OUTN_ENABLE;
	PWM_Config(PWMIdx1, PWM2, &PWMParam);
//	PWMParam.Duty[PWM3]      = 84;
	PWMParam.OutEnable[PWM3] = OUTP_AND_OUTN_ENABLE;
	PWM_Config(PWMIdx1, PWM3, &PWMParam);

	//设置高精度占空比
	PWM_DutyConfigIQ16(PWMIdx1, PWM1,10923);//1/6
	PWM_DutyConfigIQ16(PWMIdx1, PWM2,32768);//3/6
	PWM_DutyConfigIQ16(PWMIdx1, PWM3,54613);//5/6
	PWM_Enable(PWMIdx1);


	//PWC复用GPIO配置
	PWC_GpioConfigInit(TimerIdx, PWC1);
	PWC_GpioConfigInit(TimerIdx, PWC2);
	PWC_GpioConfigInit(TimerIdx, PWC3);

	//PWC参数配置
	Timer_InterrputSrcEnable(TimerIdx, PWC1_CAP_DATA_INTERRUPT_SRC);

	PWCParam.PWC1Source      = PWC1_INPUT_PWC1_XOR_PWC2PWC3;// PWC1~4  XOR输入
	PWCParam.Polarity[PWC1]  = PWC_POLARITY_BOTH;
	PWCParam.SingleGet       = 0;
	PWCParam.DMAReqEnable    = 0;
	PWCParam.TimeScale       = TimeScale;
	PWCParam.FilterTime      = 3;

	Timer_InputTriggerSelect(TimerIdx, NONE_INTPUT_TRIGGER);
	PWC_Config(TimerIdx, &PWCParam,PWC_INPUT_CHANNEL_PWC1);
//	PWC_Enable(TimerIdx);

	DBG("PWM FreqDiv:%d,PWC TimeScale:%d,\n",TimeScale,FreqDivVal);

	DBG("-----------------PWC Mode:Xor--------------------------\n");
	DBG("Examp Menu:\n");
	DBG("x: exit \n");
	DBG("s: Start\n");
	DBG("p: Pause\n");
	DBG("r: Resume\n");
	DBG("-------------------------------------------------------\n");

	while(1)
	{
		Buf[0] = 0x0;
		UARTS_Recv(PWC_UART_RX_IO, Buf, 1);
		if(Buf[0] == 'x')
		{
			DBG("x\n");
			PWC_Disable(TimerIdx);
			break;
		}
		if(Buf[0] == 's')
		{
			DBG("Start\n");
			PWC_Enable(TimerIdx);
		}
		if(Buf[0] == 'p')
		{
			DBG("Pause\n");
			Timer_Pause(TimerIdx, 1);
		}
		if(Buf[0] == 'r')
		{
			DBG("Resume\n");
			Timer_Pause(TimerIdx, 0);
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
		DBG("\t\tPWC Example\n");
		DBG("Example Menu:\n");
		DBG("1: Single mode example\n");
		DBG("2: Continue mode example\n");
		DBG("3: simultaneous work mode example\n");
		DBG("4: XOR mode example\n");
		DBG("-------------------------------------------------------\n");
		recvBuf = 0;
		recvBuf = GetChar();

		if(recvBuf == '1')
		{
			PwcSingleOrContinueMode(1);//single mode
			goto RESTART;
		}
		else if(recvBuf == '2')
		{
			PwcSingleOrContinueMode(0);//Continue mode
		}
		else if(recvBuf == '3')
		{
			PwcSimultaneWorkMode();//simultaneous  work
		}
		else if(recvBuf == '4')
		{
			PwcXorMode();//xor mode
		}
	}
}
