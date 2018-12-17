/**
 **************************************************************************************
 * @file    led_main.c
 * @brief   ledc,fdma,dpa operation example
 *
 * @author  Castle
 * @version V1.0.0 	initial release
 *
 * $Id$
 * $Created: 2016-11-28 17:31:10$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */


/*
* 此代码样例演示了LEDC,FDMA,LACC模块配合使用，来达到显示动态LED图案的功能。
* 1. show_rtc_time 演示了显示当前RTC时间，并且动态更新。它是一个双buffer刷屏的例子。
* 
* 2. show_roll_text_dw 演示了如何使用LACC进行字幕滚动。它是一个单buffer刷屏的例子。
*    为了提高LACC的速度，将文字素材及转换方法都按DWORD进行。
* 
* 3. show_parallel_output_text 演示了使用并行模式输出静态字幕，每个管脚b[0]~b[31]各输出不同的数据。
*    此例程中，所有数据共用同一个时钟信号，stb，eon，abcd等，即数据吞吐率为一个clk输出4byte
*/


#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "type.h"
#include "clk.h"
#include "led.h"
#include "gpio.h"
#include "watchdog.h"
#include "debug.h"
#include "delay.h"
#include "timer.h"
#include "dma.h"
#include "dpa.h"
#include "rtc.h"
#include "irqn.h"
#include "cache_tcm.h"
#include "uarts_interface.h"
#include "spi_flash.h"
#include "led_func.h"

#define EXTEND_BRD_NUM 64 //max=128
#define TST_ONE_FRAME_LEN 64
#define TST_ONE_FRAME_LEN_DY 64*EXTEND_BRD_NUM

#define ETY_RIGHT 		(0)
#define ETY_LEFT		(1)
#define ETY_DOWN		(0)
#define ETY_UP			(1)
#define SCAN_FR_UP		(0)
#define SCAN_FR_DOWN	(1)

uint8_t display_bufferA[1024 * 32] = {0};
uint8_t display_bufferB[1024 * 32] = {0};
uint8_t frame_buf[32 * 1024] = {0}; //////////////////

volatile uint8_t *temp_buf_ptr = NULL;
volatile bool is_time_out = FALSE;
typedef void (*intFunc)(void);
static intFunc gINTFuncPtr = NULL;
static intFunc gTIME3FuncPtr = NULL;
static intFunc gTIME2FuncPtr = NULL;

volatile uint8_t buf_state = 3;
volatile bool is_first_buf = TRUE;

static uint8_t LedcAccDmaChannelMap[31] =
{
	255,//PERIPHERAL_ID_SPIS_RX = 0,    //0
	255,//PERIPHERAL_ID_SPIS_TX,        //1
	255,//PERIPHERAL_ID_SD_RX,          //2
	255,//PERIPHERAL_ID_SD_TX,          //3
	255,//PERIPHERAL_ID_SPIM0_RX,       //4
	255,//PERIPHERAL_ID_SPIM0_TX,       //5
	255,//PERIPHERAL_ID_SPIM1_RX,       //6
	255,//PERIPHERAL_ID_SPIM1_TX,       //7
	255,//PERIPHERAL_ID_UART0_RX,       //8
	255,//PERIPHERAL_ID_UART1_RX,       //9
	255,//PERIPHERAL_ID_UART2_RX,       //10
	255,//PERIPHERAL_ID_TIMER3,         //11
	255,//PERIPHERAL_ID_TIMER4,         //12
	255,//PERIPHERAL_ID_I2S_TX,         //13
	255,//PERIPHERAL_ID_I2S_RX,         //14
	255,//PERIPHERAL_ID_SARADC,         //15
	255,//PERIPHERAL_ID_UART0_TX,       //16
	255,//PERIPHERAL_ID_UART1_TX,       //17
	255,//PERIPHERAL_ID_UART2_TX,       //18
	255,//PERIPHERAL_ID_TIMER5,         //19
	255,//PERIPHERAL_ID_TIMER6,         //20
	255,//PERIPHERAL_ID_TIMER7,         //21
	255,//PERIPHERAL_ID_TIMER8,         //22
	0,  //PERIPHERAL_ID_LACC_SRC,       //23
	1,  //PERIPHERAL_ID_LACC_DST,       //24
	2,  //PERIPHERAL_ID_LACC_DSTRD,     //25
	255,//PERIPHERAL_ID_SOFTWARE,       //26
};


uint8_t *numberSet[10] = {	number0, number1, number2, number3, number4,
                            number5, number6, number7, number8, number9
                         };


__attribute__((section(".driver.isr")))void FDMAInterrupt(void)
{
	if(gINTFuncPtr)
		(*gINTFuncPtr)();
}

__attribute__((section(".driver.isr"))) void Timer2Interrupt(void)
{
	if(gTIME2FuncPtr)
		(*gTIME2FuncPtr)();
	//DBG("A");
	Timer_InterruptFlagClear(TIMER2, UPDATE_INTERRUPT_SRC);
}

__attribute__((section(".driver.isr"))) void Timer3Interrupt(void)
{
	if(gTIME3FuncPtr)
		(*gTIME3FuncPtr)();
	//DBG("A");
	Timer_InterruptFlagClear(TIMER3, UPDATE_INTERRUPT_SRC);
}

void register_fdma_int_func(intFunc func)
{
	gINTFuncPtr = func;
}

void register_timer2_int_func(intFunc func)
{
	gTIME2FuncPtr = func;
}

void register_timer3_int_func(intFunc func)
{
	gTIME3FuncPtr = func;
}


void FDMAInterruptForRTC(void)
{
	if(FDMA_GetInterruptFlag(FDMA_DONE_INT) && FDMA_GetInterruptEn(FDMA_DONE_INT))
	{
		FDMA_StartStop(0);/////////////////2
		FDMA_InterruptFlagClr(FDMA_DONE_INT);///////////1

		//DBG("\nCastleSeeB: %X, %X, %d\n", SREG_FDMA_SWSET_PTR, SREG_FDMA_DONE_LEN, counttt);
		//switch to current display buffer
		FDMA_CircularConfig(0, 0, 64 * EXTEND_BRD_NUM, (uint8_t*)temp_buf_ptr, 64 * EXTEND_BRD_NUM);
		FDMA_CircularSetWritePtr(64 * EXTEND_BRD_NUM + 4);

		FDMA_StartStop(1);
		Timer_Config(TIMER2, 20000, 1);
		Timer_Start(TIMER2);
	}

	if(FDMA_GetInterruptFlag(FDMA_THRESHOLD_INT) && FDMA_GetInterruptEn(FDMA_THRESHOLD_INT))
	{

	}

	if(FDMA_GetInterruptFlag(FDMA_ERROR_INT) && FDMA_GetInterruptEn(FDMA_ERROR_INT))
	{

	}
}


#if 0
RTC_DATE_TIME rdt;
void update_date_to_memory(void *buff)
{
	uint32_t i, jj;
	static uint8_t **tNum = numberSet;
	memset(frame_buf, 0x00, sizeof(frame_buf));

	RTC_GetCurrTime(&rdt);

	merge_matrix_by_size(tNum[rdt.Sec % 10], 8, 1, frame_buf, 16, 4);
	for(jj = 0; jj < 6; jj++)
		move_line_l2r(frame_buf, 16, 4, 1);
	merge_matrix_by_size(tNum[rdt.Sec / 10], 8, 1, frame_buf, 16, 4);
	merge_matrix_by_size(colon, 8, 1, frame_buf, 16, 4);

	for(jj = 0; jj < 8; jj++)
		move_line_l2r(frame_buf, 16, 4, 1);
	merge_matrix_by_size(tNum[rdt.Min % 10], 8, 1, frame_buf, 16, 4);
	for(jj = 0; jj < 6; jj++)
		move_line_l2r(frame_buf, 16, 4, 1);
	merge_matrix_by_size(tNum[rdt.Min / 10], 8, 1, frame_buf, 16, 4);

	for(jj = 0; jj < 8; jj++)
		move_line_u2d(frame_buf, 16, 4, 1);

	merge_matrix_by_size(colon, 8, 1, frame_buf + 1024, 16, 4);
	for(jj = 0; jj < 8; jj++)
		move_line_l2r(frame_buf + 1024, 16, 4, 1);
	merge_matrix_by_size(tNum[rdt.Hour % 10], 8, 1, frame_buf + 1024, 16, 4);
	for(jj = 0; jj < 6; jj++)
		move_line_l2r(frame_buf + 1024, 16, 4, 1);
	merge_matrix_by_size(tNum[rdt.Hour / 10], 8, 1, frame_buf + 1024, 16, 4);

	merge_matrix_by_size(frame_buf + 1024, 16, 4, frame_buf, 16, 4);
	inversion_every_bit(frame_buf, 64 * EXTEND_BRD_NUM);
	fetch_one_serial_data(	frame_buf, 4, 4 * EXTEND_BRD_NUM, 4,
	                        ETY_LEFT, ETY_DOWN, SCAN_FR_DOWN,
	                        buff);
	//DBG("%04d-%02d-%02d %02d:%02d:%02d\n", rdt.Year, rdt.Mon, rdt.Date,rdt.Hour,rdt.Min,rdt.Sec);

}
#endif



#define MAX_BRIGHT 1000
#define MOVE_INTERVAL 30000
uint32_t current_bright = 300;

void show_roll_text_timer(void)
{
	buf_state = 0xff;
}

void show_roll_text_fdma(void)
{
	if(FDMA_GetInterruptFlag(FDMA_DONE_INT) && FDMA_GetInterruptEn(FDMA_DONE_INT))
	{
		FDMA_StartStop(0);
		FDMA_InterruptFlagClr(FDMA_DONE_INT);
		DBG("\nCastleSeeB: DONE = %d\n", buf_state);
	}

	if(FDMA_GetInterruptFlag(FDMA_THRESHOLD_INT) && FDMA_GetInterruptEn(FDMA_THRESHOLD_INT))
	{
		//close threshold interrupt, clear flag, but fdma will continue work.
		FDMA_InterruptEn(FDMA_THRESHOLD_INT, 0);
		FDMA_InterruptFlagClr(FDMA_THRESHOLD_INT);

		//DBG("INT: %d.\n", buf_state);
		if(buf_state == 1)
		{
			buf_state = 2;
			__builtin_memcpy((void*)display_bufferA, (void*)temp_buf_ptr, TST_ONE_FRAME_LEN_DY / 2);

			FDMA_CircularSetWritePtr(TST_ONE_FRAME_LEN_DY / 2);

			//after moving SREG_FDMA_SWSET_PTR, the TINT can be high,
			//so it always needed to clear after SREG_FDMA_SWSET_PTR changed
			FDMA_InterruptFlagClr(FDMA_THRESHOLD_INT);
			FDMA_InterruptEn(FDMA_THRESHOLD_INT, 1);
		}
		else if(buf_state == 2)
		{
			buf_state = 3;
			__builtin_memcpy((void*)display_bufferA + TST_ONE_FRAME_LEN_DY / 2, (void*)temp_buf_ptr + TST_ONE_FRAME_LEN_DY / 2, TST_ONE_FRAME_LEN_DY / 2);

			if(1/*some_flag1++<1*/)
			{
				FDMA_CircularSetWritePtr(TST_ONE_FRAME_LEN_DY + 4);
				//if timeout, but current round not finished,
				//will waiting until [0<SREG_FDMA_DONE_LEN<8]
				//DBG("OneRd\n");
				Timer_Config(TIMER3, MOVE_INTERVAL, 1);
				Timer_Start(TIMER3);
				//Timer_Start(TIMER2, MOVE_INTERVAL / 16, 1);
			}
			else
			{
				FDMA_CircularSetWritePtr(0);
			}
		}
	}

	if(FDMA_GetInterruptFlag(FDMA_ERROR_INT) && FDMA_GetInterruptEn(FDMA_ERROR_INT))
	{
		FDMA_InterruptFlagClr(FDMA_ERROR_INT);
		DBG("CastleSee Error...");
	}
}


/*
* OEN : 
* ____    ______     __________    ______    ____
* |  |    |    |     |        |    |    |    |  |
*    ------    -------        ------    ------  ---
*/
void eliminate_shadow_try(void)
{
	static uint8_t cnt = 0;
	cnt ++;
	if(cnt < 6)
	{
		current_bright = MAX_BRIGHT * (2 << cnt);
		//LED_OEN_SetValidTime(current_bright);
		Timer_Config(TIMER2,  MOVE_INTERVAL / 16, 1);
		Timer_Start(TIMER2);
	}
	else if(cnt > 9 && cnt < 15)
	{
		current_bright = MAX_BRIGHT * (2 << (9 - cnt));
		//LED_OEN_SetValidTime(current_bright);
		Timer_Config(TIMER2, MOVE_INTERVAL / 16, 1);
		Timer_Start(TIMER2);
	}
	else
	{
		current_bright = 0;
		//LED_OEN_SetValidTime(current_bright);
		cnt = 0;
		//Timer_Start(TIMER2, MOVE_INTERVAL / 16, 1);
		//DBG("F");
	}
}

uint32_t numberBuf3_dw[16] = {0};
void show_roll_text_dw(void)
{
	uint32_t i, j;
	LED_CONFIG cfg = {
		.isSerialOut = 		TRUE,
		.serialNum = 		0,
		
		.dataClkOutCycle = 	10,
		.dataClkHighTime = 	5,
		.dataClkPolarity = 	0,
		
		.clkOutmode = 		0,
		.clkOutPolarity = 	0,
		.clkOutNum = 		0,
		.clkOutHighTime= 	20,
		
		.OENPolarity = 		0,
		.OENToStbTime = 	30,
		.OENCnt = 			32 * 4 * EXTEND_BRD_NUM,
		.OENValidTime = 	MAX_BRIGHT,
		
		.STBPolarity = 		0,
		.STBValidTime = 	10,
		.STBStbTime = 		30,
		
		.lnSelMode = 		0,
		.lnSel = 			3,

		.fifoLvl = 			16,
		.inWidth = 			2,
		.outWidth = 		3,
	};

	DBG("show_roll_text_dw\n");

	NVIC_EnableIRQ(FDMA_IRQn);
	NVIC_EnableIRQ(TMR2_IRQn);
	NVIC_EnableIRQ(TMR3_IRQn);
	//GPIO config

	LED_DataCLKIOConfig(1);//dataclk a[18]
	LED_STBIOConfig(0);//stb 0:a[16] 1:a[28]
	LED_OENIOConfig(0);//oen 0:a[17] 1:a[29]
	LED_LineSelIOConfig(LED_LINE_SEL_A, 1);//A a[24]
	LED_LineSelIOConfig(LED_LINE_SEL_B, 1);//B a[25]
	LED_LineSelIOConfig(LED_LINE_SEL_C, 0xff);//C a[26]
	LED_LineSelIOConfig(LED_LINE_SEL_D, 0xff);//D a[27]

	LED_CLKOutIOConfig(LED_CLKOUT2_SEL);
	LED_DataOutIOConfig(0x01);//data: b[0]

	//LED module
	LED_Init();
	LED_Config(&cfg);
	
	merge_matrix_by_size_dw((uint32_t*)welcome_dw, 16, 4, (uint32_t*)frame_buf, 16, 1 * EXTEND_BRD_NUM);

	//add number:
	///*
#if 1
#define NUM_INTERVAL ((EXTEND_BRD_NUM/4)-4) //every 4 board do one number display...
	for(j = 0; j < NUM_INTERVAL; j++)
	{
		///*
		memset(numberBuf3_dw, 0x00, sizeof(numberBuf3_dw));
		for(i = 0; i < 16; i++)
		{
			numberBuf3_dw[i] |= numberSet[(j % 10) / 1][i];
			numberBuf3_dw[i] |= numberSet[(j % 100) / 10][i] << 8;
			numberBuf3_dw[i] |= numberSet[(j % 1000) / 100][i] << 16;
		}
		merge_matrix_by_size_offset_dw(numberBuf3_dw, 8, 1, (uint32_t*)frame_buf, 16, EXTEND_BRD_NUM, 4 * (j + 1));
	}
#endif
	revert_one_string_dw((uint32_t*)frame_buf, 16 * EXTEND_BRD_NUM); //to fit LACC bitshift

	inversion_every_bit_dw((uint32_t*)frame_buf, 16 * EXTEND_BRD_NUM);//数据取反
	fetch_one_serial_data_dw(	(uint32_t*)frame_buf, 4, 4 * EXTEND_BRD_NUM, 4,
	                            ETY_LEFT, ETY_DOWN, SCAN_FR_DOWN,
	                            display_bufferA);

	FDMA_CircularConfig(2, 2, TST_ONE_FRAME_LEN_DY / 2, display_bufferA, TST_ONE_FRAME_LEN_DY);

	FDMA_InterruptEn(FDMA_DONE_INT, TRUE);
	//FDMA_InterruptEn(FDMA_THRESHOLD_INT, TRUE);
	//FDMA_InterruptEn(FDMA_ERROR_INT, TRUE);

	//enable LED ctrl and never disable it, until done
	LED_Ctrl_Enable(1);

	LED_FIFO_DMA_Enable(1);
	FDMA_CircularSetWritePtr(TST_ONE_FRAME_LEN_DY + 4);

	//circular fdma enable, start working
	FDMA_StartStop(TRUE);

	//register_timer2_int_func(eliminate_shadow_try);
	//Timer_Start(TIMER2, MOVE_INTERVAL / 16, 1);
	//while(1);

	register_fdma_int_func(show_roll_text_fdma);
	register_timer3_int_func(show_roll_text_timer);
	Timer_Config(TIMER3, MOVE_INTERVAL, 1);
	Timer_Start(TIMER3);

	while(1)
	{
		static bool mDir = TRUE;
		//continue;
		{
			//for(i=0;i<10;i++);
			if(buf_state == 0xff)
			{
				buf_state = 1;

				temp_buf_ptr = display_bufferB;
				move_line_l2r_dpa_fullstep_dw_nobuf((uint32_t*)frame_buf, 16, 4 * EXTEND_BRD_NUM, mDir, 1);
				fetch_one_serial_data_dw(	(uint32_t*)frame_buf, 4, 4 * EXTEND_BRD_NUM, 4,
				                            ETY_LEFT, ETY_DOWN, SCAN_FR_DOWN,
				                            (uint8_t*)temp_buf_ptr);

				//small range to catch SREG_FDMA_DONE_LEN
				//SREG_FDMA_DONE_LEN=0 will trigle FDMA_THRESHOLD_INT if SREG_FDMA_SWSET_PTR goto 0
				while(FDMA_BlockGetDoneLen() > 80 || FDMA_BlockGetDoneLen() == 0);
				FDMA_CircularSetWritePtr(0);
				//if SREG_FDMA_SWSET_PTR is at the range out of FDMA buffer size, TINT flag always HIGH.
				//so it always needed to clear after SREG_FDMA_SWSET_PTR changed
				FDMA_InterruptFlagClr(FDMA_THRESHOLD_INT);
				FDMA_InterruptEn(FDMA_THRESHOLD_INT, TRUE);
				//waiting for 1/2 threshold trigger

				is_first_buf = !is_first_buf;
				//DBG("swb...\n");
			}
		}
	}
}

void show_parallel_output_text(void)
{
	uint32_t i;
	LED_CONFIG cfg = {
		.isSerialOut = 		FALSE,
		.serialNum = 		0,
		
		.dataClkOutCycle = 	10,
		.dataClkHighTime = 	5,
		.dataClkPolarity = 	0,
		
		.clkOutmode = 		0,
		.clkOutPolarity = 	0,
		.clkOutNum = 		0,
		.clkOutHighTime= 	2,
		
		.OENPolarity = 		0,
		.OENToStbTime = 	10,
		.OENCnt = 			32 * 4 * 2,
		.OENValidTime = 	100,
		
		.STBPolarity = 		0,
		.STBValidTime = 	10,
		.STBStbTime = 		30,
		
		.lnSelMode = 		0,
		.lnSel = 			3,

		.fifoLvl = 			16,
		.inWidth = 			2,
		.outWidth = 		3,
	};

	NVIC_EnableIRQ(FDMA_IRQn);
	NVIC_EnableIRQ(TMR3_IRQn);
	//GPIO config

	LED_DataCLKIOConfig(1);//dataclk a[18]
	LED_STBIOConfig(0);//stb 0:a[16] 1:a[28]
	LED_OENIOConfig(0);//oen 0:a[17] 1:a[29]
	LED_LineSelIOConfig(LED_LINE_SEL_A, 1);//A a[24]
	LED_LineSelIOConfig(LED_LINE_SEL_B, 1);//B a[25]
	LED_LineSelIOConfig(LED_LINE_SEL_C, 0xff);//C a[26]
	LED_LineSelIOConfig(LED_LINE_SEL_D, 0xff);//D a[27]

	LED_CLKOutIOConfig(LED_CLKOUT2_SEL);
	LED_DataOutIOConfig(0x0f);//data: b[0]

	//LED module
	LED_Init();
	LED_Config(&cfg);
	
	merge_matrix_by_size(some_frame_4, 16, 4, display_bufferA, 16, 4 * 2);
	for(i = 0; i < 32; i++)
	{
		move_line_l2r(display_bufferA, 16, 4*2, 1);
		fetch_one_serial_data(	display_bufferA, 4, 8, 4,
		                        ETY_LEFT, ETY_DOWN, SCAN_FR_DOWN,
		                        frame_buf);
		extend_res_fr_s2p(frame_buf, 64*2, display_bufferB, 4, i);
	}

	inversion_every_bit(display_bufferB, 64 * 32 * 2);

	FDMA_CircularConfig(2, 2, 64 * 32 * 2, display_bufferB, 64 * 32 * 2);

	//FDMA_InterruptEn(FDMA_DONE_INT, TRUE);
	//FDMA_InterruptEn(FDMA_THRESHOLD_INT, TRUE);
	//FDMA_InterruptEn(FDMA_ERROR_INT, TRUE);

	//enable LED ctrl and never disable it, until done
	LED_Ctrl_Enable(1);

	//circular fdma enable
	LED_FIFO_DMA_Enable(1);
	FDMA_CircularSetWritePtr(64 * 32 * 2 + 4);
	//DBG("SWPTR:%X\n", SREG_FDMA_SWSET_PTR);

	//FDMA_CircularPutData(display_buffer, 64);
	FDMA_StartStop(TRUE);

	//Timer_Start(TIMER3, 100000, 0);
	
	while(1);
}
uint8_t zt=1;
int main(void)
{
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

	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);

	UARTS_Init(0,  115200, 8,  0,  1);

	//获取Flash信息
	//SpiFlashGetInfo(&FlashInfo);
	//FlashUnlock();

	DBG("\n");
	DBG("------------------------------------------------------\n");
	DBG("        LEDC,FDMA,LACC Operation Example              \n");
	DBG("       Mountain View Silicon Technology Co.,Ltd.      \n");
	DBG("------------------------------------------------------\n\n");
	GIE_ENABLE();
	DMA_ChannelAllocTableSet(LedcAccDmaChannelMap);
	//while(zt);
	//show_roll_text_dw();
	show_parallel_output_text();
	while(1);
}




//
