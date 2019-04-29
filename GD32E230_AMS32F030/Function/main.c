/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example
    
    \version 2018-06-19, V1.0.0, firmware for GD32E230
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "app_config.h"

#define	WAVE_MIN_FREQ					490			//喇叭输出最小频率
#define WAVE_MAX_FREQ					510			//喇叭输出最大频率
#define WAVE_STILL_FREQ					495			//水珠静止状态频率		
#define WAVE_PWM_DUTY					500			//喇叭输出信号占空比
#define WAVE_MAX_OPERATION				42			//喇叭输出参数最大种组合
#define PWM_OUTPUT_OFF					0
#define	LIGHT_MAX_DISP_NUM				4			//LED照明灯的工作状态


typedef struct _LED_SET_PARAM_ {
	DISP_STATE_TYPEDEF CurDispStateNum;
	DISP_COLOR_TYPEDEF CurDispColorNum;
	uint32_t ScanTimerNum;
}LED_SET;

typedef struct _WAVE_SET_PARAM_ {
	uint32_t Timer;
	uint16_t Freq;
}WAVE_SET;

const LED_SET Light_WorkState[LIGHT_MAX_DISP_NUM] = {
	{DISP_STATE_IDLE, DISP_COLOR_OFF, 0},
	{DISP_STATE_SINGLE_COLOR, DISP_COLOR_WHITE, 1500},
	{DISP_STATE_SINGLE_COLOR, DISP_COLOR_BLUE, 1500},
	{DISP_STATE_FULL_COLOR, DISP_COLOR_RED, 1500},
};


const WAVE_SET WAVE_OPERATION_DATA[WAVE_MAX_OPERATION] = {
	{5000*5, WAVE_MIN_FREQ},
	{5000*5, WAVE_MIN_FREQ},
	{5000*5, WAVE_MIN_FREQ},
	
	{1000, WAVE_MIN_FREQ+1},
	{1000, WAVE_MIN_FREQ+2},        
	{1000, WAVE_MIN_FREQ+3},
	{1000, WAVE_MIN_FREQ+4},
	{1000, WAVE_MIN_FREQ+5},
	{1000, WAVE_MIN_FREQ+6},
	{1000, WAVE_MIN_FREQ+7},        
	{1000, WAVE_MIN_FREQ+8},
	{1000, WAVE_MIN_FREQ+9},
	
	{3000*5, WAVE_STILL_FREQ},        
	{1000, WAVE_MAX_FREQ-9},
	{1000, WAVE_MAX_FREQ-8},
	{1000, WAVE_MAX_FREQ-7},
	{1000, WAVE_MAX_FREQ-6},
	{1000, WAVE_MAX_FREQ-5},        
	{1000, WAVE_MAX_FREQ-4},
	{1000, WAVE_MAX_FREQ-3},
	{1000, WAVE_MAX_FREQ-2},
	{1000, WAVE_MAX_FREQ-1},  

	{4000*5, WAVE_MAX_FREQ},        
	{1000, WAVE_MAX_FREQ-1},
	{1000, WAVE_MAX_FREQ-2},
	{1000, WAVE_MAX_FREQ-3},
	{1000, WAVE_MAX_FREQ-4},
	{1000, WAVE_MAX_FREQ-5},
	{1000, WAVE_MAX_FREQ-6},
	{1000, WAVE_MAX_FREQ-7},
	{1000, WAVE_MAX_FREQ-8},
	{1000, WAVE_MAX_FREQ-9},

	{3000*5, WAVE_STILL_FREQ},
	{1000, WAVE_MIN_FREQ+9},
	{1000, WAVE_MIN_FREQ+8},
	{1000, WAVE_MIN_FREQ+7},
	{1000, WAVE_MIN_FREQ+6},
	{1000, WAVE_MIN_FREQ+5},
	{1000, WAVE_MIN_FREQ+4},
	{1000, WAVE_MIN_FREQ+3},   
	{1000, WAVE_MIN_FREQ+2},        
	{1000, WAVE_MIN_FREQ+1},
};

/* 常量定义 */
static uint8_t CurWorkMode = 0;
static TIMER WaveAutoSwitchTimer;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
	//USART初始化，115200波特率
	Usart_Debug_Init(115200);
	//系统滴答定时器初始化，单位uS
	System_Systick_Init(1000);
	/* 喇叭驱动信号输出初始化 */
	Pwm_WaveConfig(WAVE_MIN_FREQ, PWM_OUTPUT_OFF);
	TimeOutSet(&WaveAutoSwitchTimer, 0);
	/* GUI LED显示初始化 */
	Led_DispInit();
	Led_DispStateSet(DISP_STATE_POWER_ON, DISP_COLOR_WHITE, 1500);
	
	/* 按键扫描初始化 */
	GpioKeyInit();
	/* 水泵控制及检测初始化 */
	System_WaterPump_Init();
	/* 照明灯初始化 */
	Light_DispInit();
	Light_DispStateSet(DISP_STATE_IDLE, DISP_COLOR_OFF, 0);

	while (1) {
		static uint8_t WaveAutoIndex = 0;
		
		Display();
		
		if (MSG_NEXT == GpioKeyScan()) {
			CurWorkMode++;
			CurWorkMode %= LIGHT_MAX_DISP_NUM;
			WaveAutoIndex = 0;

			if (DISP_STATE_IDLE != Light_WorkState[CurWorkMode].CurDispStateNum) {
				System_WaterPump_Control(ENABLE);
				TimeOutSet(&WaveAutoSwitchTimer, 0);
				Led_DispStateSet(DISP_STATE_LOOP_CHANGE, DISP_COLOR_CYAN, 2000);
			}
			else {
				System_WaterPump_Control(DISABLE);
				Pwm_WaveConfig(WAVE_MIN_FREQ, PWM_OUTPUT_OFF);
				Led_DispStateSet(DISP_STATE_POWER_ON, DISP_COLOR_WHITE, 1500);
			}
			Light_DispStateSet(Light_WorkState[CurWorkMode].CurDispStateNum,
			Light_WorkState[CurWorkMode].CurDispColorNum, Light_WorkState[CurWorkMode].ScanTimerNum);
		}

		if (0 == CurWorkMode) {
			continue;
		}

		uint16_t Msg = WaterPump_WorkStateDetect();
		if (MSG_NONE != Msg) {
			printf("WaterPump_WorkStateDetect Msg = %x;\n", Msg);
		}

		if (MSG_VOL_NORMAL == Msg) {
			if (IsTimeOut(&WaveAutoSwitchTimer)) {
				WaveAutoIndex++;
				WaveAutoIndex %= WAVE_MAX_OPERATION;

				TimeOutSet(&WaveAutoSwitchTimer, WAVE_OPERATION_DATA[WaveAutoIndex].Timer);
				Pwm_WaveConfig(WAVE_OPERATION_DATA[WaveAutoIndex].Freq, WAVE_PWM_DUTY);
				Led_DispStateSet(DISP_STATE_LOOP_SPIN, DISP_COLOR_CYAN, 1500);
			}
		}
		else if (MSG_VOL_LOW == Msg){
			Led_DispStateSet(DISP_STATE_LOOP_CHANGE, DISP_COLOR_RED, 2000);
			CurWorkMode = 0;
			Light_DispStateSet(DISP_STATE_IDLE, DISP_COLOR_OFF, 0);
			Pwm_WaveConfig(WAVE_MIN_FREQ, PWM_OUTPUT_OFF);
			System_WaterPump_Control(DISABLE);
		}
		else if (MSG_VOL_HIGHT == Msg) {	
			Led_DispStateSet(DISP_STATE_LOOP_CHANGE, DISP_COLOR_RED, 500);
			CurWorkMode = 0;
			Light_DispStateSet(DISP_STATE_IDLE, DISP_COLOR_OFF, 0);
			Pwm_WaveConfig(WAVE_MIN_FREQ, PWM_OUTPUT_OFF);
			System_WaterPump_Control(DISABLE);
		}
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
	/* Infinite loop */
	while (1)
	{
	}
}
#endif



