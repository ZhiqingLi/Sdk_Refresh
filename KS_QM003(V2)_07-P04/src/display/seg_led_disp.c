//#include "app/app_globals.h"
#include "app_config.h"
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN)|| defined(FUNC_AIP1629A_LED_EN))
#include "folder.h"
#include "Fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "radio_control.h"
#include "line_in_control.h"
#include "gpio.h"
//#include "app/audio_process.h"
#ifdef IDLE_MODULE
#include "idle_control.h"
#endif
#include "sys_app.h"

//#include "display/lcd_seg.h"
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"

extern bool IsUSBLink(void);
extern bool IsCardLink(void);

#ifdef FUNC_POWER_ON_AUDIO_EN
extern bool gUSBChargePowerOnFlag;
#endif


#define LED57_COM_NUM	5
#define LED57_SEG_NUM	7

#ifdef FUNC_SEG_LED_EN

#if defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN)
uint8_t const Led57IconSet[ICON_NUM] = {ICON57_PLAY, ICON57_PAUSE, ICON57_COLON, ICON57_USB, ICON57_SD, ICON57_FM_MHZ, ICON57_MP3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t const Led48IconSet[ICON_NUM] = {0xFF, 0xFF, ICON48_COLON, 0xFF, 0xFF, ICON48_FM_MHZ, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif

#ifdef FUNC_9PIN_SEG_LED_EN
uint8_t const LedCustomFace[8][MAX_LED_PIN_NUM] = {
	{0x0f,0x18,0x0c,0xc0,0x03,0x30,0x0c,0xc3,0x00},
	{0x00,0x00,0x00,0xfe,0x7f,0x38,0x1c,0x00,0x00},
	{0x00,0x00,0x00,0xfe,0x7f,0xe0,0x70,0x00,0x00},
	{0x00,0x00,0x00,0xfe,0x7f,0x0e,0x07,0x00,0x00},
	{0xf0,0x7f,0x7f,0xff,0xff,0xff,0xff,0x00,0x00},
	{0x00,0x1e,0x3c,0x42,0x42,0x81,0x81,0x00,0x00},
	{0x00,0x00,0x00,0x81,0x81,0x42,0x42,0xff,0x00},
	{0x00,0x80,0x80,0xfe,0x7f,0x18,0x18,0x66,0xff},
};


#endif
	
//LED 引脚对应 GPIO 初始化
void LedPinGpioInit(void)
{
#if (defined(FUNC_9PIN_SEG_LED_EN) || defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN))
	GpioSetRegOneBit(LED_PIN1_PORT_PU, LED_PIN1_BIT);
	GpioSetRegOneBit(LED_PIN2_PORT_PU, LED_PIN2_BIT);
	GpioSetRegOneBit(LED_PIN3_PORT_PU, LED_PIN3_BIT);
	GpioSetRegOneBit(LED_PIN4_PORT_PU, LED_PIN4_BIT);
	GpioSetRegOneBit(LED_PIN5_PORT_PU, LED_PIN5_BIT);
	GpioSetRegOneBit(LED_PIN6_PORT_PU, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioSetRegOneBit(LED_PIN7_PORT_PU, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioSetRegOneBit(LED_PIN7_PORT_PU, LED_PIN7_BIT);
	GpioSetRegOneBit(LED_PIN8_PORT_PU, LED_PIN8_BIT);
	GpioSetRegOneBit(LED_PIN9_PORT_PU, LED_PIN9_BIT);
#endif
	GpioClrRegOneBit(LED_PIN1_PORT_PD, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_PD, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_PD, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_PD, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_PD, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_PD, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_PD, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_PD, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_PD, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_PD, LED_PIN9_BIT);
#endif
	GpioClrRegOneBit(LED_PIN1_PORT_OE, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_OE, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_OE, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_OE, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_OE, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_OE, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_OE, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_OE, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_OE, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_OE, LED_PIN9_BIT);
#endif
	GpioClrRegOneBit(LED_PIN1_PORT_ICS_1MA7, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_ICS_1MA7, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_ICS_1MA7, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_ICS_1MA7, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_ICS_1MA7, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_ICS_1MA7, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_ICS_1MA7, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_ICS_1MA7, LED_PIN9_BIT);
#endif

	GpioClrRegOneBit(LED_PIN1_PORT_ICS_2MA4, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_ICS_2MA4, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_ICS_2MA4, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_ICS_2MA4, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_ICS_2MA4, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_ICS_2MA4, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_ICS_2MA4, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_ICS_2MA4, LED_PIN9_BIT);
#endif
#endif
}
//LED 引脚对应 GPIO 反初始化
void LedPinGpioDeInit(void)
{
#if (defined(FUNC_9PIN_SEG_LED_EN) || defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN))
	GpioSetRegOneBit(LED_PIN1_PORT_PU, LED_PIN1_BIT);
	GpioSetRegOneBit(LED_PIN2_PORT_PU, LED_PIN2_BIT);
	GpioSetRegOneBit(LED_PIN3_PORT_PU, LED_PIN3_BIT);
	GpioSetRegOneBit(LED_PIN4_PORT_PU, LED_PIN4_BIT);
	GpioSetRegOneBit(LED_PIN5_PORT_PU, LED_PIN5_BIT);
	GpioSetRegOneBit(LED_PIN6_PORT_PU, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioSetRegOneBit(LED_PIN7_PORT_PU, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioSetRegOneBit(LED_PIN7_PORT_PU, LED_PIN7_BIT);
	GpioSetRegOneBit(LED_PIN8_PORT_PU, LED_PIN8_BIT);
	GpioSetRegOneBit(LED_PIN9_PORT_PU, LED_PIN9_BIT);
#endif
	GpioClrRegOneBit(LED_PIN1_PORT_PD, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_PD, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_PD, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_PD, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_PD, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_PD, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_PD, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_PD, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_PD, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_PD, LED_PIN9_BIT);
#endif
	GpioClrRegOneBit(LED_PIN1_PORT_OE, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_OE, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_OE, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_OE, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_OE, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_OE, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_OE, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_OE, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_OE, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_OE, LED_PIN9_BIT);
#endif

	GpioClrRegOneBit(LED_PIN1_PORT_ICS_1MA7, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_ICS_1MA7, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_ICS_1MA7, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_ICS_1MA7, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_ICS_1MA7, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_ICS_1MA7, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_ICS_1MA7, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_ICS_1MA7, LED_PIN9_BIT);
#endif

	GpioClrRegOneBit(LED_PIN1_PORT_ICS_2MA4, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_ICS_2MA4, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_ICS_2MA4, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_ICS_2MA4, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_ICS_2MA4, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_ICS_2MA4, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_ICS_2MA4, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_ICS_2MA4, LED_PIN9_BIT);
#endif
#endif
}

//LED 各个引脚使用前要先关闭恒流源
void LedAllPinGpioInput(void)
{
#if (defined(FUNC_9PIN_SEG_LED_EN) || defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN))
	GpioClrRegOneBit(LED_PIN1_PORT_OE, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_OE, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_OE, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_OE, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_OE, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_OE, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_OE, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_OE, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_OE, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_OE, LED_PIN9_BIT);
#endif

	GpioClrRegOneBit(LED_PIN1_PORT_ICS_1MA7, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_ICS_1MA7, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_ICS_1MA7, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_ICS_1MA7, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_ICS_1MA7, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_ICS_1MA7, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_ICS_1MA7, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_ICS_1MA7, LED_PIN9_BIT);
#endif

	GpioClrRegOneBit(LED_PIN1_PORT_ICS_2MA4, LED_PIN1_BIT);
	GpioClrRegOneBit(LED_PIN2_PORT_ICS_2MA4, LED_PIN2_BIT);
	GpioClrRegOneBit(LED_PIN3_PORT_ICS_2MA4, LED_PIN3_BIT);
	GpioClrRegOneBit(LED_PIN4_PORT_ICS_2MA4, LED_PIN4_BIT);
	GpioClrRegOneBit(LED_PIN5_PORT_ICS_2MA4, LED_PIN5_BIT);
	GpioClrRegOneBit(LED_PIN6_PORT_ICS_2MA4, LED_PIN6_BIT);
#if defined(FUNC_7PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4, LED_PIN7_BIT);
#elif defined(FUNC_9PIN_SEG_LED_EN)
	GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4, LED_PIN7_BIT);
	GpioClrRegOneBit(LED_PIN8_PORT_ICS_2MA4, LED_PIN8_BIT);
	GpioClrRegOneBit(LED_PIN9_PORT_ICS_2MA4, LED_PIN9_BIT);
#endif
#endif
}

// 显示刷新.
// Call by display refresh ISR.
void LedFlushDisp(void)
{
#if (defined(FUNC_9PIN_SEG_LED_EN) || defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN))
	static uint8_t ScanPinNum = 0;

	if(gLedDispRefreshFlag == FALSE)
	{
		//	return;
	}
	ScanPinNum++;

	if(ScanPinNum >= (MAX_LED_PIN_NUM + 1))
	{
		ScanPinNum = 1;
	}

	LedAllPinGpioInput();
// LED 各个Pin 脚依次扫描
//请根据显示屏的脚位定义，修改下面的代码
	switch(ScanPinNum)
	{
		case 1:
			if(gDispBuff[0] & SEG_A)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[0] & SEG_B)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[0] & SEG_C)
			{
				LED_PIN4_IN_ON;
			}

			if(gDispBuff[0] & SEG_D)
			{
				LED_PIN5_IN_ON;
			}	//PLAY

			if(gDispBuff[0] & SEG_E)
			{
				LED_PIN6_IN_ON;
			}	//SD

#if defined(FUNC_7PIN_SEG_LED_EN)
			if(gDispBuff[0] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}
#elif defined(FUNC_9PIN_SEG_LED_EN)
			if(gDispBuff[0] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}

			if(gDispBuff[0] & SEG_G)
			{
				LED_PIN8_IN_ON;
			}	//PLAY

			if(gDispBuff[0] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
#endif
			LED_PIN1_OUT_HIGH;
			break;

		case 2:
			if(gDispBuff[1] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[1] & SEG_B)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[1] & SEG_C)
			{
				LED_PIN4_IN_ON;
			}

			if(gDispBuff[1] & SEG_D)
			{
				LED_PIN5_IN_ON;
			}	//PLAY

			if(gDispBuff[1] & SEG_E)
			{
				LED_PIN6_IN_ON;
			}	//SD

#if defined(FUNC_7PIN_SEG_LED_EN)
			if(gDispBuff[1] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}
#elif defined(FUNC_9PIN_SEG_LED_EN)
			if(gDispBuff[1] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}

			if(gDispBuff[1] & SEG_G)
			{
				LED_PIN8_IN_ON;
			}	//PLAY

			if(gDispBuff[1] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
#endif

			LED_PIN2_OUT_HIGH;
			break;

		case 3:
			if(gDispBuff[2] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[2] & SEG_B)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[2] & SEG_C)
			{
				LED_PIN4_IN_ON;
			}

			if(gDispBuff[2] & SEG_D)
			{
				LED_PIN5_IN_ON;
			}	//PLAY

			if(gDispBuff[2] & SEG_E)
			{
				LED_PIN6_IN_ON;
			}	//SD

#if defined(FUNC_7PIN_SEG_LED_EN)
			if(gDispBuff[2] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}
#elif defined(FUNC_9PIN_SEG_LED_EN)
			if(gDispBuff[2] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}

			if(gDispBuff[2] & SEG_G)
			{
				LED_PIN8_IN_ON;
			}	//PLAY

			if(gDispBuff[2] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
#endif
			LED_PIN3_OUT_HIGH;
			break;

		case 4:
			if(gDispBuff[3] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[3] & SEG_B)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[3] & SEG_C)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[3] & SEG_D)
			{
				LED_PIN5_IN_ON;
			}	//PLAY

			if(gDispBuff[3] & SEG_E)
			{
				LED_PIN6_IN_ON;
			}	//SD

#if defined(FUNC_7PIN_SEG_LED_EN)
			if(gDispBuff[3] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}
#elif defined(FUNC_9PIN_SEG_LED_EN)
			if(gDispBuff[3] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}

			if(gDispBuff[3] & SEG_G)
			{
				LED_PIN8_IN_ON;
			}	//PLAY

			if(gDispBuff[3] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
#endif
			LED_PIN4_OUT_HIGH;
			break;

		case 5:
			if(gDispBuff[4] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[4] & SEG_B)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[4] & SEG_C)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[4] & SEG_D)
			{
				LED_PIN4_IN_ON;
			}	//PLAY

			if(gDispBuff[4] & SEG_E)
			{
				LED_PIN6_IN_ON;
			}	//SD

#if defined(FUNC_7PIN_SEG_LED_EN)
			if(gDispBuff[4] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}
#elif defined(FUNC_9PIN_SEG_LED_EN)
			if(gDispBuff[4] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}

			if(gDispBuff[4] & SEG_G)
			{
				LED_PIN8_IN_ON;
			}	//PLAY

			if(gDispBuff[4] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
#endif

			LED_PIN5_OUT_HIGH;
			break;

		case 6:
			if(gDispBuff[5] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[5] & SEG_B)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[5] & SEG_C)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[5] & SEG_D)
			{
				LED_PIN4_IN_ON;
			}	//PLAY

			if(gDispBuff[5] & SEG_E)
			{
				LED_PIN5_IN_ON;
			}	//SD

#if defined(FUNC_7PIN_SEG_LED_EN)
			if(gDispBuff[5] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}
#elif defined(FUNC_9PIN_SEG_LED_EN)
			if(gDispBuff[5] & SEG_F)
			{
				LED_PIN7_IN_ON;
			}

			if(gDispBuff[5] & SEG_G)
			{
				LED_PIN8_IN_ON;
			}	//PLAY

			if(gDispBuff[5] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
#endif
			LED_PIN6_OUT_HIGH;
			break;

#if (defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_9PIN_SEG_LED_EN))
		case 7:
			if(gDispBuff[6] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[6] & SEG_B)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[6] & SEG_C)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[6] & SEG_D)
			{
				LED_PIN4_IN_ON;
			}	//PLAY

			if(gDispBuff[6] & SEG_E)
			{
				LED_PIN5_IN_ON;
			}	//SD

#ifdef FUNC_9PIN_SEG_LED_EN
			if(gDispBuff[6] & SEG_F)
			{
				LED_PIN6_IN_ON;
			}

			if(gDispBuff[6] & SEG_G)
			{
				LED_PIN8_IN_ON;
			}	//PLAY

			if(gDispBuff[6] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
#endif
			LED_PIN7_OUT_HIGH;
			break;
#endif

#ifdef FUNC_9PIN_SEG_LED_EN
		case 8:
			if(gDispBuff[7] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[7] & SEG_B)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[7] & SEG_C)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[7] & SEG_D)
			{
				LED_PIN4_IN_ON;
			}	//PLAY

			if(gDispBuff[7] & SEG_E)
			{
				LED_PIN5_IN_ON;
			}	//SD

			if(gDispBuff[7] & SEG_F)
			{
				LED_PIN6_IN_ON;
			}

			if(gDispBuff[7] & SEG_G)
			{
				LED_PIN7_IN_ON;
			}	//PLAY

			if(gDispBuff[7] & SEG_H)
			{
				LED_PIN9_IN_ON;
			}
			LED_PIN8_OUT_HIGH;
			break;

		case 9:
			if(gDispBuff[8] & SEG_A)
			{
				LED_PIN1_IN_ON;
			}

			if(gDispBuff[8] & SEG_B)
			{
				LED_PIN2_IN_ON;
			}

			if(gDispBuff[8] & SEG_C)
			{
				LED_PIN3_IN_ON;
			}

			if(gDispBuff[8] & SEG_D)
			{
				LED_PIN4_IN_ON;
			}	//PLAY

			if(gDispBuff[8] & SEG_E)
			{
				LED_PIN5_IN_ON;
			}	//SD

			if(gDispBuff[8] & SEG_F)
			{
				LED_PIN6_IN_ON;
			}

			if(gDispBuff[8] & SEG_G)
			{
				LED_PIN7_IN_ON;
			}	//PLAY

			if(gDispBuff[8] & SEG_H)
			{
				LED_PIN8_IN_ON;
			}
			LED_PIN9_OUT_HIGH;
			break;

#endif

		default:
			break;
	}
#endif
}
#endif

void DispLedIcon(ICON_IDX Icon, ICON_STATU Light)
{
#if (defined(FUNC_9PIN_SEG_LED_EN) || defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN))

	uint8_t IconVal;
	uint8_t IconPos;

	if(Icon >= ICON_NUM)
	{
		return;
	}
	if(gDisplayMode == DISP_DEV_LED57)
	{
		//IconVal = Led57IconSet[Icon];
	}
	else
	{
		IconVal = Led48IconSet[Icon];
	}

	if(IconVal == 0xFF)
	{
		return;
	}
	IconPos = IconVal / 8;
	IconVal = (1 << (IconVal % 8));

	if(Light == LIGHT_ON)
	{
		gDispBuff[IconPos] |= (DISP_TYPE)IconVal;
	}
	else if(Light == LIGHT_OFF)
	{
		gDispBuff[IconPos] &= (DISP_TYPE)(~IconVal);
	}
#endif
}


void LedDispDevSymbol(void)
{
	if((gDisplayMode == DISP_DEV_LED8888) || (gDisplayMode == DISP_DEV_LED1888))
	{
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
		DispIcon(ICON_COLON, LIGHT_OFF);
		switch(gSys.CurModuleID)
		{
#ifdef IDLE_MODULE
			case MODULE_ID_IDLE:
#ifdef FUNC_RTC_EN
			case MODULE_ID_RTC:
				DispRtc(FALSE);
#else
				DispString(" NOD");
#endif
				break;
#endif
			
#ifdef FUNC_WIFI_EN
			case MODULE_ID_WIFI:
				DispString("HIFI");
				break;
#endif
#ifdef FUNC_PLAYER_EN
			case MODULE_ID_PLAYER_USB:
				DispString(" USB");
				break;
#endif

#ifdef FUNC_PLAYER_EN
			case MODULE_ID_PLAYER_SD:
				DispString(" SD ");
				break;
#endif

#ifdef FUNC_LINEIN_EN
			case MODULE_ID_LINEIN:
				DispString("AUX ");
			   // DispChar((InputChannel % 10), POS_COM_UNITS);
				break;
#endif
#ifdef FUNC_BT_EN
			case MODULE_ID_BLUETOOTH:
				DispString("BLUE");
				break;
#endif

#ifdef FUNC_RADIO_EN
			case MODULE_ID_RADIO:
				DispString(" RAD");
				break;
#endif

#ifdef FUNC_USB_AUDIO_EN
			case MODULE_ID_USB_AUDIO:
				DispString(" PC ");
				break;
#endif

#ifdef FUNC_USB_READER_EN
			case MODULE_ID_USB_READER:
				DispString(" REA");
				break;
#endif

#ifdef FUNC_USB_AUDIO_READER_EN
			case MODULE_ID_USB_AUDIO_READER:
				DispString(" PC ");
				break;
#endif

			default:
				break;
		}
	}
	else if(gDisplayMode == DISP_DEV_LED57)	//ICON_MP3
	{
		switch(gSys.CurModuleID)
		{
#ifdef IDLE_MODULE
			case MODULE_ID_IDLE:
#ifndef FUNC_RTC_EN
				DispString(" NOD");
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
#else
				DispRtc(FALSE);
#endif
				break;
#endif
#ifdef FUNC_PLAYER_EN
			case MODULE_ID_PLAYER_USB:
				DispIcon(ICON_USB, LIGHT_ON);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#ifdef FUNC_PLAYER_EN
			case MODULE_ID_PLAYER_SD:
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_ON);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#ifdef FUNC_LINEIN_EN
			case MODULE_ID_LINEIN:
				DispString(" AUX");
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif
#ifdef FUNC_BT_EN
			case MODULE_ID_BLUETOOTH:
				DispString(" BT ");
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#ifdef FUNC_RADIO_EN
			case MODULE_ID_RADIO:
				DispString(" rad");
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_ON);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#if (defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_AUDIO_READER_EN))
			case MODULE_ID_USB_AUDIO:
			case MODULE_ID_USB_AUDIO_READER:
				DispString(" PC ");
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
#endif

#ifdef FUNC_STANDBY_EN
			case SYS_MODE_STANDBY:
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				DispString(" CLO");
				break;
#endif

			default:
				DispIcon(ICON_USB, LIGHT_OFF);
				DispIcon(ICON_SD, LIGHT_OFF);
				DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
				DispIcon(ICON_PLAY, LIGHT_OFF);
				DispIcon(ICON_PAUSE, LIGHT_OFF);
				DispIcon(ICON_COLON, LIGHT_OFF);
				break;
		}
	}
}


void LedDispRepeat(void)
{
	if(gPlayContrl->CurPlayMode == PLAY_MODE_REPEAT_ALL)
	{
		DispString(" ALL");
	}
	else if(gPlayContrl->CurPlayMode == PLAY_MODE_REPEAT_ONE)
	{
		DispString(" ONE");
	}
	else if(gPlayContrl->CurPlayMode == PLAY_MODE_RANDOM)
	{
		DispString(" RAN");
	}
	else if(gPlayContrl->CurPlayMode == PLAY_MODE_PREVIEW_PLAY)
	{
		DispString("INTO");
	}
	//else if(gPlayContrl->CurPlayMode == REPEAT_FOLDER)
	//{
	//	DispString("FLOD");
	//}
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
}

void LedDispRepeatAB(void)
{
	if(gPlayContrl == NULL)
	{
		return;
	}
	if(gPlayContrl->RepeatAB.RepeatFlag == REPEAT_A_SETED)
	{
		DispString(" A- ");
	}
	else if(gPlayContrl->RepeatAB.RepeatFlag == REPEAT_OPENED)
	{
		DispString(" A-B");
	}
	//else if(gPlayContrl->CurPlayMode == REPEAT_FOLDER)
	//{
	//	DispString("FLOD");
	//}
	DispIcon(ICON_COLON, LIGHT_OFF);
	DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
}


void LedDispPlayState(void)
{
#if defined(FUNC_PLAYER_EN)
	if(gDisplayMode == DISP_DEV_LED57)
	{
		if(GetPlayState() == PLAYER_STATE_PAUSE)
		{
			//DBG(("LED57||\n"));
			DispPlayTime(DIS_PLAYTIME_PAUSE);
			DispIcon(ICON_PLAY, LIGHT_OFF);
			DispIcon(ICON_PAUSE, LIGHT_ON);
			DispIcon(ICON_COLON, LIGHT_ON);
		}
		else if((GetPlayState() == PLAYER_STATE_PLAYING) || (GetPlayState() == PLAYER_STATE_FF) || (GetPlayState() == PLAYER_STATE_FB))
		{
			DispIcon(ICON_PLAY, LIGHT_ON);
			DispIcon(ICON_PAUSE, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_ON);
		}
		else // if((gPlayCtrl.State == PLAY_STATE_STOP) || (gPlayCtrl.State == PLAY_STATE_IDLE) || (gPlayCtrl.State == PLAY_STATE_BROWSE))
		{
			DispIcon(ICON_PLAY, LIGHT_OFF);
			DispIcon(ICON_PAUSE, LIGHT_OFF);
			DispIcon(ICON_COLON, LIGHT_OFF);
			if(GetPlayState() == PLAYER_STATE_IDLE)
			{
				//DBG(("LED57 STOP"));
				if((gSys.CurModuleID == MODULE_ID_PLAYER_USB) && IsUDiskLink())
				{
					//DispString("STOP");
					//Num2Char(gPlayContrl->FileHandle->FsContext->gFsInfo.FileSum, FALSE);
					Num2Char(gPlayContrl->FolderHandle.RecFileCnt, FALSE);

				}
				if((gSys.CurModuleID == MODULE_ID_PLAYER_SD) && IsCardLink())
				{
					//DispString("STOP");
					//Num2Char(gPlayContrl->FileHandle->FsContext->gFsInfo.FileSum, FALSE);
					Num2Char(gPlayContrl->FolderHandle.RecFileCnt, FALSE);
				}
			}
		}
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	}
	else //if((gDisplayMode == DISP_DEV_LED8888) || (gDisplayMode == DISP_DEV_LED1888))
	{
		if(GetPlayState() == PLAYER_STATE_PAUSE)
		{
			DispString(" PAU");
		}
		else if(GetPlayState() == PLAYER_STATE_IDLE)//else if(gPlayCtrl.State == PLAY_STATE_STOP)
		{
			//DBG(("LED STOP\n"));
#if 1//def FUNC_USB_EN
			if((gSys.CurModuleID == MODULE_ID_PLAYER_USB) && IsUDiskLink())
			{
				DispString(" USB");
			}
#endif

#if 1//def FUNC_CARD_EN
			if((gSys.CurModuleID == MODULE_ID_PLAYER_SD) && IsCardLink())
			{
				DispString(" SD ");
			}
#endif
		}
		DispIcon(ICON_COLON, LIGHT_OFF);
		DispIcon(ICON_DP_FM_MHZ, LIGHT_OFF);
	}
#endif
}

#endif


