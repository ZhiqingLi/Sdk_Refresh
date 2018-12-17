#include "app_config.h"
#include "sys_app.h"
#include "fat_file.h"
#include "bt_app_func.h"
#include "player_control.h"
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#include "single_led_disp.h"
#endif
#ifdef FUNC_WIFI_EN
#include "wifi_control.h"
#include "wifi_uart_com.h"
#endif
#ifdef FUNC_SLAVE_UART_EN
#include "slave_mcu_command.h"
#include "slave_uart_com.h"
#endif

#ifdef	OPTION_CHARGER_DETECT
extern bool IsInCharge(void);
#endif

LED_DISP_STATE LedDispState = LED_DISP_STATE_IDLE;
bool RedLedDispFlag = FALSE;
bool GreenLedDispFlag = FALSE;
bool BlueLedDispFlag = FALSE;

#ifdef FUNC_SINGLE_LED_EN

void SLedInit(void)
{
#ifdef LED_RED_MODE_BIT_MASK
	GpioClrRegOneBit(LED_RED_MODE_PORT_IE, LED_RED_MODE_BIT_MASK);	
	GpioSetRegOneBit(LED_RED_MODE_PORT_OE, LED_RED_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_RED_MODE_PORT_PU, LED_RED_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_RED_MODE_PORT_PD, LED_RED_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_RED_MODE_PORT_1P7MA, LED_RED_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_RED_MODE_PORT_2P4MA, LED_RED_MODE_BIT_MASK);
	LED_RED_MODE_OFF();
#endif

#ifdef LED_GREEN_MODE_BIT_MASK
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_IE, LED_GREEN_MODE_BIT_MASK);	
	GpioSetRegOneBit(LED_GREEN_MODE_PORT_OE, LED_GREEN_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_PU, LED_GREEN_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_PD, LED_GREEN_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_1P7MA, LED_GREEN_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_2P4MA, LED_GREEN_MODE_BIT_MASK);
	LED_GREEN_MODE_OFF();
#endif
	
#ifdef LED_BLUE_MODE_BIT_MASK
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_IE, LED_BLUE_MODE_BIT_MASK);	
	GpioSetRegOneBit(LED_BLUE_MODE_PORT_OE, LED_BLUE_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_PU, LED_BLUE_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_PD, LED_BLUE_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_1P7MA, LED_BLUE_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_2P4MA, LED_BLUE_MODE_BIT_MASK);
	LED_BLUE_MODE_OFF();
#endif
}

//LED显示状态设置
void SingLedDispStateSet(LED_DISP_STATE DispState, bool State)
{
	 
   if((LedDispState == LED_DISP_STATE_WHITE) && (DispState == LED_DISP_STATE_CHREGERED))
	 {
		 return;
	 }
	 
	 LedDispState = DispState;
	 
	if(DispState == LED_DISP_STATE_WHITE || DispState == LED_DISP_STATE_ALL)
	{
		if(!gWiFi.MicMuteState)
		{
		  RedLedDispFlag = State;
		}
		GreenLedDispFlag = State;
		BlueLedDispFlag = State;
	}
	else if(DispState == LED_DISP_STATE_RED)
	{
		RedLedDispFlag = State;
	}
	else if(DispState == LED_DISP_STATE_GREEN || DispState == LED_DISP_STATE_CHREGERED)
	{
		GreenLedDispFlag = State;
	}
	else if(DispState == LED_DISP_STATE_BLUE)
	{
		BlueLedDispFlag = State;
	}
}

//单颗LED显示扫描
void SLedDisplay(void)
{
   if(RedLedDispFlag == TRUE)
	 {
		 LED_RED_MODE_ON();
	 }
	 else
	 {
		 LED_RED_MODE_OFF();
	 }
	 
	 if(GreenLedDispFlag == TRUE)
	 {
		 LED_GREEN_MODE_ON();
	 }
	 else
	 {
		 LED_GREEN_MODE_OFF();
	 }
	 
	  if(BlueLedDispFlag == TRUE)
	 {
		 LED_BLUE_MODE_ON();
	 }
	 else
	 {
		 LED_BLUE_MODE_OFF();
	 }
}

//LED  GPIO去初始化
void SLedDeinit(void)
{
#ifdef LED_RED_MODE_BIT_MASK
	GpioClrRegOneBit(LED_RED_MODE_PORT_IE, LED_RED_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_RED_MODE_PORT_OE, LED_RED_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_RED_MODE_PORT_PU, LED_RED_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_RED_MODE_PORT_PD, LED_RED_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_RED_MODE_PORT_1P7MA, LED_RED_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_RED_MODE_PORT_2P4MA, LED_RED_MODE_BIT_MASK);
	LED_RED_MODE_OFF();
#endif

#ifdef LED_GREEN_MODE_BIT_MASK
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_IE, LED_GREEN_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_OE, LED_GREEN_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_PU, LED_GREEN_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_PD, LED_GREEN_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_1P7MA, LED_GREEN_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_GREEN_MODE_PORT_2P4MA, LED_GREEN_MODE_BIT_MASK);
	LED_GREEN_MODE_OFF();
#endif
	
#ifdef LED_BLUE_MODE_BIT_MASK
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_IE, LED_BLUE_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_OE, LED_BLUE_MODE_BIT_MASK);	
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_PU, LED_BLUE_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_PD, LED_BLUE_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_1P7MA, LED_BLUE_MODE_BIT_MASK);
	GpioClrRegOneBit(LED_BLUE_MODE_PORT_2P4MA, LED_BLUE_MODE_BIT_MASK);
	LED_BLUE_MODE_OFF();
#endif

}
#endif  //FUNC_SINGLE_LED_EN
