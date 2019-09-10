///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: power_management.c
//  maintainer: lujiangang
///////////////////////////////////////////////////////////////////////////////

#include "type.h"
#include "app_config.h"
#include "uart.h"
#include "adc.h"
#include "clk.h"
#include "gpio.h"
#include "wakeup.h"
#include "sys_app.h"
#include "watchdog.h"
#include "bt_control_api.h"
#include "breakpoint.h"
#include "dev_detect_driver.h" 
#include "sys_vol.h"
#include "power_management.h"
#ifdef FUNC_WIFI_EN
#include "wifi_control.h"
#include "wifi_uart_com.h"
#include "wifi_mcu_command.h"
#endif
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#include "singled_display.h"
#endif

#ifdef BT_APPLE_BATTERY_STATUS 
#define BT_BATTERY_LEVEL_CNT    9
static uint8_t BtBatteryLevel = BT_BATTERY_LEVEL_CNT;
TIMER BtBatteryDetTim;
#endif

#ifdef FUNC_POWER_MONITOR_EN

//#define POWER_MONITOR_DISP_EN

#define LDOIN_SAMPLE_COUNT			20		//获取LDOIN幅度时用来平均的采样次数
#define LDOIN_SAMPLE_PERIOD			50		//获取LDOIN幅度时获取采样值的间隔(ms)
#define LOW_POWER_DET_COUNT 		3		//低电检测报警次数
#define LOW_POWER_SOUND_CNT 		300		//低电报警提示音间隔时间(单位s)
#define LOW_POWEROFF_VOLTAGE		3		//低电关机电池电量百分比
#define LOW_POWER_RESET_VOLTAGE		30		//低电检测恢复电池电量百分比

//低电检测延时时间参数，用于消抖和提示音播放。
#define LOW_POWEROFF_DELAY			(500)	//单位：10ms
#define POWER_OFF_JITTER_TIMER		500

//以下定义不同的电压检测事件的触发电压(单位mV)，用户根据自身系统电池的特点来配置
#ifdef ADC_POWER_MONITOR_EN 
   #define LDOIN_VOLTAGE_FULL			3100
   #define LDOIN_VOLTAGE_HIGHT          3000
   #define LDOIN_VOLTAGE_HIGH			2800
   #define LDOIN_VOLTAGE_MID			2600
   #define LDOIN_VOLTAGE_LOW			2500
   #define LDOIN_VOLTAGE_OFF			2400	//注意：这里是ADC值，Vol=log/4095*3.41,单位V
#else
   #define LDOIN_VOLTAGE_FULL			4150
   #define LDOIN_VOLTAGE_HIGH			3900
   #define LDOIN_VOLTAGE_MID			3650
   #define LDOIN_VOLTAGE_LOW			3500
   #define LDOIN_VOLTAGE_OFF			3300	//低于此电压值进入关机powerdown状态
#endif

//电压检测时不同的显示处理
typedef enum _PWR_MNT_DISP
{
	PWR_MNT_DISP_NONE = 0,
	PWR_MNT_DISP_CHARGE,		 
	PWR_MNT_DISP_HIGH_V, 
	PWR_MNT_DISP_MID_V, 
	PWR_MNT_DISP_LOW_V, 
	PWR_MNT_DISP_EMPTY_V, 
	PWR_MNT_DISP_SYS_OFF
	 
} PWR_MNT_DISP;

#ifdef POWER_MONITOR_DISP_EN
//主要用于电压状态显示的变量
static TIMER BlinkTimer;
static PWR_MNT_DISP PwrMntDisp;
#endif

//用于电压检测的变量
TIMER PowerMonitorTimer;
uint32_t LdoinSampleSum = 0; 
uint16_t  LdoinSampleCnt = LDOIN_SAMPLE_COUNT;
uint32_t LdoinLevelAverage = 0;		//当前LDOIN电压平均值
//用于关机检查变量
TIMER PowerOffDetectTimer;

#ifdef FUNC_SOUND_REMIND
#include "sound_remind.h"
static bool 	IsSystemPowerLow = FALSE;
static uint8_t	LowPowerSoundCnt = LOW_POWER_DET_COUNT;
static uint32_t LowPowerDetCnt = 0;
#endif

#ifdef	OPTION_CHARGER_DETECT
//硬件检测PC 或充电器连接状态
//使能内部下拉。无PC或充电器连接时，检测口为低电平，有时检测口为高电平
bool IsInCharge(void)
{
	GpioSetRegBits(CHARGE_DETECT_PORT_PU, CHARGE_DETECT_BIT);
	GpioClrRegBits(CHARGE_DETECT_PORT_PD, CHARGE_DETECT_BIT);
	GpioClrRegBits(CHARGE_DETECT_PORT_OE, CHARGE_DETECT_BIT);	   
	GpioSetRegBits(CHARGE_DETECT_PORT_IE, CHARGE_DETECT_BIT);
	WaitUs(2);

	if(GpioGetReg(CHARGE_DETECT_PORT_IN)&CHARGE_DETECT_BIT) {
		return TRUE;
	} 

	return FALSE;
}
#endif
#ifdef	OPTION_CHAR_FUL_DETECT
/*****************************************************************************
 函 数 名  : IsChargeFul
 功能描述  : 电池充满电检测
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月29日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
bool IsChargeFull(void)
{
//设为输入，无上下拉

	GpioSetRegBits(CHAR_FUL_DETECT_PORT_PU, CHAR_FUL_DETECT_BIT);
	GpioClrRegBits(CHAR_FUL_DETECT_PORT_PD, CHAR_FUL_DETECT_BIT);
	GpioClrRegBits(CHAR_FUL_DETECT_PORT_OE, CHAR_FUL_DETECT_BIT);	   
	GpioSetRegBits(CHAR_FUL_DETECT_PORT_IE, CHAR_FUL_DETECT_BIT);
	WaitUs(2);
	if(!(GpioGetReg(CHAR_FUL_DETECT_PORT_IN) & CHAR_FUL_DETECT_BIT))
	{
		return TRUE;
	}   	

	return FALSE;
}
#endif

void PowerMonitorDisp(void)
{
#if (defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))
	static uint8_t  ShowStep = 0;
	static bool IsToShow = FALSE;

	switch(PwrMntDisp)
	{			
		case PWR_MNT_DISP_CHARGE:
			//闪烁充电ICON,表示正在充电
			if(IsTimeOut(&BlinkTimer))
			{
				TimeOutSet(&BlinkTimer, 500);
				switch(ShowStep)
				{

					case 0:
						DispIcon(ICON_BAT1, FALSE);
						DispIcon(ICON_BAT2, FALSE);
						DispIcon(ICON_BAT3, FALSE);
						break;
					case 1:
						DispIcon(ICON_BAT1, TRUE);
						DispIcon(ICON_BAT2, FALSE);
						DispIcon(ICON_BAT3, FALSE);
						break;
					case 2:
						DispIcon(ICON_BAT1, TRUE);
						DispIcon(ICON_BAT2, TRUE);
						DispIcon(ICON_BAT3, FALSE);
						break;
					case 3:
						DispIcon(ICON_BAT1, TRUE);
						DispIcon(ICON_BAT2, TRUE);
						DispIcon(ICON_BAT3, TRUE);
						break;
				}		
				if(ShowStep < 3)
				{
					ShowStep++;
				}
				else
				{
					ShowStep = 0;
				}
			}
			
			break;

		case PWR_MNT_DISP_HIGH_V:
			//DBG("BAT FULL\n");			
			DispIcon(ICON_BATFUL, TRUE);
			DispIcon(ICON_BATHAF, FALSE);
			//可以显示满格电量，请添加显示代码
			break;
			
		case PWR_MNT_DISP_MID_V:
			//DBG("BAT HALF\n");		
			DispIcon(ICON_BATFUL, FALSE);
			DispIcon(ICON_BATHAF, TRUE);
			//可以显示2格电量，请添加显示代码
			break;

		case PWR_MNT_DISP_LOW_V:
			DispIcon(ICON_BATFUL, FALSE);
			DispIcon(ICON_BATHAF, TRUE);
			//可以显示1格电量，请添加显示代码
			break;
			
		case PWR_MNT_DISP_EMPTY_V:
			//DBG("BAT EMPTY\n");				
			DispIcon(ICON_BATFUL, FALSE);

			if(IsTimeOut(&BlinkTimer))
			{
				TimeOutSet(&BlinkTimer, 300);
				if(IsToShow)
				{
					DispIcon(ICON_BATHAF, TRUE);
				}
				else
				{
					DispIcon(ICON_BATHAF, FALSE);
				}
				IsToShow = !IsToShow;
			}
			//可以显示0格电量，请添加显示代码
			break;
		
		case PWR_MNT_DISP_SYS_OFF:
			//DBG("BAT OFF\n");
			//ClearScreen();			//清除显示				
			//DispString(" LO ");
			break;
			
		default:
			break;
	}
#endif
}
//监测LDOIN的电压值，执行对应检测点的处理
//PowerOnInitFlag: TRUE--第一次上电执行电源监控检测
static void PowerLdoinLevelMonitor(bool PowerOnInitFlag)
{		
	if(LdoinSampleCnt > 0)
	{
	#ifdef ADC_POWER_MONITOR_EN
		LdoinSampleSum += (SarAdcChannelGetValue(ADC_POWER_MONITOR_PORT)*3400/4095);
	#else
		LdoinSampleSum += SarAdcGetLdoinVoltage();		
	#endif		
		LdoinSampleCnt--;
	}

	//采样够LDOIN_SAMPLE_COUNT次数，求LDOIN平均值
	if(LdoinSampleCnt == 0)
	{
		LdoinLevelAverage = (LdoinSampleSum / LDOIN_SAMPLE_COUNT);

		//DBG("Battery Level Average: %lu\n", (uint32_t)LdoinLevelAverage);

		//为继续检测LDOIN，初始化变量
		LdoinSampleCnt = LDOIN_SAMPLE_COUNT;
		LdoinSampleSum = 0;

#ifdef BT_APPLE_BATTERY_STATUS 
		if(IsTimeOut(&BtBatteryDetTim) && GetCurBtConnectedFlag())
		{
			TimeOutSet(&BtBatteryDetTim, 5000);
			BtBatteryLevel = (LdoinLevelAverage - LDOIN_VOLTAGE_OFF)/100;
			if(BtBatteryLevel > BT_BATTERY_LEVEL_CNT)
			{
				BtBatteryLevel = BT_BATTERY_LEVEL_CNT;
			}
            BTSetBatteryState(BtBatteryLevel, TRUE);
            DBG("Bt Battery Volt: %lu\n", (uint32_t)LdoinLevelAverage);
		}
//#else
		//BtDisableBatteryState();
#endif	

#ifdef	OPTION_CHARGER_DETECT
		if(IsInCharge())		//充电器已经接入的处理
		{	
			//DBG("IsInCharge\n");
			IsSystemPowerLow = FALSE;
			LowPowerSoundCnt = LOW_POWER_DET_COUNT;
			LowPowerDetCnt = (LOW_POWER_SOUND_CNT*100-LOW_POWEROFF_DELAY);
#ifdef FUNC_SINGLE_LED_EN
			SingleLedDisplayModeSet(LED_DISPLAY_MODE_LOWBATTER, FALSE);
#endif
			return;
		}
#endif

#ifdef POWER_MONITOR_DISP_EN		
		if(LdoinLevelAverage > LDOIN_VOLTAGE_HIGH)	  
		{
			//可以显示满格电量，请在PowerMonitorDisp中添加显示代码
			PwrMntDisp = PWR_MNT_DISP_HIGH_V;
			//DBG("bat full\n");
		}
		else if(LdoinLevelAverage > LDOIN_VOLTAGE_MID)
		{
			//可以显示2格电量，请在PowerMonitorDisp中添加显示代码
			PwrMntDisp = PWR_MNT_DISP_MID_V;
		}
		else if(LdoinLevelAverage > LDOIN_VOLTAGE_LOW)
		{
			//可以显示1格电量，请在PowerMonitorDisp中添加显示代码
			PwrMntDisp = PWR_MNT_DISP_LOW_V;
			//DBG("bat LOW\n");
		}
		else if(LdoinLevelAverage > LDOIN_VOLTAGE_OFF)
		{
			//可以显示0格电量，请在PowerMonitorDisp中添加显示代码
			PwrMntDisp = PWR_MNT_DISP_EMPTY_V;
		}
		else
		{
		    PwrMntDisp = PWR_MNT_DISP_SYS_OFF;
		}
#endif

		if (LOW_POWEROFF_VOLTAGE >= GetCurBatterLevelAverage())
		{
			//低于关机电压，进入关机流程
			//停止正常工作流程，包括关显示、关DAC、关功放电源等动作
			DBG("PowerMonitor, Low Voltage!PD. %d:%d\n", LowPowerDetCnt, LowPowerSoundCnt);
			
			//low level voltage detect in power on sequence, power down system directly
			if(PowerOnInitFlag == TRUE)
			{
				APP_DBG("Power on Low level voltage detected->Power Off\n");
				LowPowerSoundCnt = 1;
				LowPowerDetCnt = (LOW_POWER_SOUND_CNT*100-LOW_POWEROFF_DELAY);
				IsSystemPowerLow = TRUE; 
			}
			    
			if(LowPowerDetCnt >= (LOW_POWER_SOUND_CNT*100))
			{
				if (LowPowerSoundCnt) {
					APP_DBG("PowerMonitor, Low Voltage sound remind!!!!");
					MsgSend(MSG_BAT_LOW_PWR);
					LowPowerSoundCnt--;
			#ifdef FUNC_SINGLE_LED_EN
					SingleLedDisplayModeSet(LED_DISPLAY_MODE_LOWBATTER, TRUE);
			#endif
				}

				IsSystemPowerLow = TRUE;
				LowPowerDetCnt = 0;
			}
			
			if(!LowPowerSoundCnt && (LowPowerDetCnt >= LOW_POWEROFF_DELAY*2)
#ifdef FUNC_WIFI_EN
			&& (!WiFiFirmwareUpgradeStateGet())
#endif
			) {
				if((MODULE_ID_POWEROFF != gSys.CurModuleID) && IsTimeOut(&PowerOffDetectTimer))	/*MSG_COMMON_CLOSE only need send once*/
				{
					/*if use push button, send message, for task's saving info.*/
					APP_DBG("Low level voltage detected->send message common close\n");
					TimeOutSet(&PowerOffDetectTimer, POWER_OFF_JITTER_TIMER);
				#if 0 //def FUNC_WIFI_POWER_KEEP_ON
					WiFiRequestMcuPowerOff();
				#else
					gSys.NextModuleID = MODULE_ID_POWEROFF;
					MsgSend(MSG_COMMON_CLOSE);
				#endif
				}
			}
		}
		else if (!IsSystemPowerLow || (LOW_POWER_RESET_VOLTAGE <= GetCurBatterLevelAverage())){
			IsSystemPowerLow = FALSE;
			LowPowerSoundCnt = LOW_POWER_DET_COUNT;
			LowPowerDetCnt = (LOW_POWER_SOUND_CNT*100-LOW_POWEROFF_DELAY);
		#ifdef FUNC_SINGLE_LED_EN
			SingleLedDisplayModeSet(LED_DISPLAY_MODE_LOWBATTER, FALSE);
		#endif
		}
	}
	PowerMonitorDisp();
}


//电能监视初始化
//实现系统启动过程中的低电压检测处理，以及初始化充电设备接入检测IO等
void PowerMonitorInit(void)
{
	TimeOutSet(&PowerMonitorTimer, 0);
	TimeOutSet(&PowerOffDetectTimer, 0);
	
#ifdef ADC_POWER_MONITOR_EN
	SarAdcGpioSel(ADC_POWER_MONITOR_PORT);
#endif	
	
#ifdef OPTION_CHARGER_DETECT
	//如果系统启动时，充电设备已经接入，就不执行下面这段低电压检测和处理过程
	if(!IsInCharge())
#endif
	{
#ifdef POWERON_DETECT_VOLTAGE	// 开机检测电压
		//系统启动过程中的低电压检测
		//开机时电压检测，如果小于开机电压，不进入设备检测和播放流程，直接关机
		//检测过程为时50ms，并进行对应的处理
		while(LdoinSampleCnt)
		{
			LdoinSampleCnt--;
#ifdef ADC_POWER_MONITOR_EN
			LdoinSampleSum += (SarAdcChannelGetValue(ADC_POWER_MONITOR_PORT)*3400/4095);
#else
			LdoinSampleSum += SarAdcGetLdoinVoltage();		
	  #endif	
			WaitMs(5);
		}		
		//为后边的LDOIN检测初始化变量
		PowerLdoinLevelMonitor(TRUE);
#endif
	}
#ifdef BT_APPLE_BATTERY_STATUS
	TimeOutSet(&BtBatteryDetTim, 0);	
#endif
}

/*****************************************************************************
 函 数 名  : GetCurBatterLevelAverage
 功能描述  : 获取当前电池电量的百分比
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年7月9日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t GetCurBatterLevelAverage(void)
{
	uint8_t AdcBatVol = 0;

	if(LdoinLevelAverage > LDOIN_VOLTAGE_FULL)
	{
		AdcBatVol = 100;
	}
	else if(LdoinLevelAverage < LDOIN_VOLTAGE_LOW)
	{
		AdcBatVol = 1;
	}
	else
	{
		AdcBatVol = ((LdoinLevelAverage - LDOIN_VOLTAGE_LOW)*100/(LDOIN_VOLTAGE_FULL - LDOIN_VOLTAGE_LOW));
	}
	
	//DBG("Battery Level Average: %lu\n", (uint32_t)LdoinLevelAverage);
	return AdcBatVol;
}

//系统电源状态监控和处理
//系统启动后如果LDOIN大于开机电压，放在系统正常运行中监测LDOIN
void PowerMonitor(void)
{	
	if(IsTimeOut(&PowerMonitorTimer))
	{
		static bool IsBatterFull = FALSE;
		static bool IsFristCharge = FALSE;
		
		TimeOutSet(&PowerMonitorTimer, LDOIN_SAMPLE_PERIOD);
#ifdef OPTION_CHARGER_DETECT
		if(IsInCharge()) {		//充电器已经接入的处理	
			if (IS_CUR_WORK_MODULE()
			//&& !IS_RTC_WAKEUP()
			&& !WiFiFirmwareUpgradeStateGet()
			&& (MODULE_ID_UNKNOWN == gSys.NextModuleID)) {
				gSys.NextModuleID = MODULE_ID_IDLE;
				MsgSend(MSG_COMMON_CLOSE);
			}

			if (IsFristCharge) {
				IsFristCharge = FALSE;
				if (IS_RTC_WAKEUP()) {
					RTC_WAKEUP_FLAG_CLR();
					APP_DBG ("Charge clr RTC wakeup flag;\n");
				}
			}
#ifdef OPTION_CHAR_FUL_DETECT
			if (IsChargeFull() && !IsBatterFull) {
				//提示充满电
				IsBatterFull = TRUE;
				MsgSend(MSG_BAT_FUL_PWR);
			}
#endif
		}
		else {
			IsBatterFull = FALSE;
			IsFristCharge = TRUE;
			if (!IS_CUR_WORK_MODULE() && (MODULE_ID_UNKNOWN == gSys.NextModuleID)) {
				gSys.NextModuleID = MODULE_ID_WIFI;
				MsgSend(MSG_COMMON_CLOSE);
			}
		}
#endif
		IsBatterFull = IsBatterFull;	//去除编译警告。
		//没有采样够LDOIN_SAMPLE_COUNT次数，继续采样
		PowerLdoinLevelMonitor(FALSE);
	}
#ifdef FUNC_SOUND_REMIND
	LowPowerDetCnt++;			//低电提示音定时计算；每10MS一次；
#endif
}
#ifdef LOW_POWER_SAVE_PLAYTIME_TO_FLASH
extern uint32_t AudioPlaySavePlayTimeToFlash(void);

void LowPowerDetProc(void)
{
	uint16_t RetVal;

	RetVal = SarAdcGetLdoinVoltage();

	if((RetVal < 4200) && (RetVal > 3400))	
   	{
	   	AudioPlaySavePlayTimeToFlash();
   	}
}
#endif
#endif	//end of FUNC_POWER_MONITOR_EN

/*****************************************************************************
 函 数 名  : powerkey软件检测函数，配合实现硬开关
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月20日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/

#ifdef FUNC_POWERKEY_SOFT_POWERON_EN
#define SOFT_SWITCH_ONTIME				500

bool SoftPowerKeyDetect(void)
{
	static uint16_t PowerKeyLinkState = 0, IsStateChange = 0xffff;
	static bool PrevPowerKeyState = FALSE;
	bool CurPowerKeyState = FALSE, ret = FALSE;

	//设为输入，带下拉
	GpioClrRegBits(POWERKEY_DETECT_PORT_OE, POWERKEY_DETECT_PORT_BIT);
	GpioSetRegBits(POWERKEY_DETECT_PORT_IE, POWERKEY_DETECT_PORT_BIT);
	GpioClrRegBits(POWERKEY_DETECT_PORT_PU, POWERKEY_DETECT_PORT_BIT);
	GpioClrRegBits(POWERKEY_DETECT_PORT_PD, POWERKEY_DETECT_PORT_BIT);

	if (GpioGetReg(POWERKEY_DETECT_PORT_IN)&POWERKEY_DETECT_PORT_BIT) {
		CurPowerKeyState = TRUE;
	}

	if (CurPowerKeyState != PrevPowerKeyState) {
		PowerKeyLinkState = 0;
		PrevPowerKeyState = CurPowerKeyState;
		if (0xffff != IsStateChange) {
			IsStateChange = 0x00FF;
		}
	}

	if (PowerKeyLinkState < SOFT_SWITCH_ONTIME)
	{
		PowerKeyLinkState++;
	}
	else {	
		if (IS_RTC_WAKEUP()) {
			if (0x00FF == IsStateChange) {
				RTC_WAKEUP_FLAG_CLR();
				APP_DBG ("Switch change clr RTC wakeup flag;\n");
			}
		}
		else {
			ret = CurPowerKeyState;
		}
		IsStateChange = 0;
	}

	return ret;
}
#endif

// 进入deep sleep，为了降低功耗，默认设置为输入上拉。
// 客户可以根据需要进行一些定制化配置
void SysGotoDeepSleepGpioCfg(void)
{
	GpioSetRegBits(GPIO_A_IE, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_A_OE, 0xFFFFFFFF);
	GpioSetRegBits(GPIO_A_PU, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_A_PD, 0xFFFFFFFF);

	GpioSetRegBits(GPIO_B_IE, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_B_OE, 0xFFFFFFFF);
	GpioSetRegBits(GPIO_B_PU, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_B_PD, 0xFFFFFFFF);

	GpioSetRegBits(GPIO_C_IE, 0x7FFF);
	GpioClrRegBits(GPIO_C_OE, 0x7FFF);
	GpioSetRegBits(GPIO_C_PU, 0x7FFF);
	GpioClrRegBits(GPIO_C_PD, 0x7FFF);

	// BT close, not arbitrarily modify
	BTDevicePowerOff();
}

// 进入PowerDown，为了芯片能进入，默认设置为输出下拉。
void SysGotoPowerDownGpioCfg(void)
{
	GpioSetRegBits(GPIO_A_OE, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_A_OUT, 0xFFFFFFFF);

	GpioSetRegBits(GPIO_B_OE, 0xFFFFFFFF);
	GpioClrRegBits(GPIO_B_OUT, 0xFFFFFFFF);

	GpioSetRegBits(GPIO_C_OE, 0x7FFF);
	GpioClrRegBits(GPIO_C_OUT, 0x7FFF);
	// BT close, not arbitrarily modify
	BTDevicePowerOff();
}

/**
 * @brief 		system power off detect function
 *			this function should be called in timer INT function or systick function for periodically detecting
 *			when USE_POWERKEY_SLIDE_SWITCH, the system will be powered off directly
 *			when USE_POWERKEY_SOFT_PUSH_BUTTON, it wil send message:MSG_COMMON_CLOSE, for task's saving information, 
* 			and then power off system
 * @param	None
 * @return	None
 */
void SystemPowerOffDetect(void)
{

#ifdef USE_POWERKEY_SLIDE_SWITCH
#ifdef FUNC_POWERKEY_SOFT_POWERON_EN
	#define SLIDE_SWITCH_ONTIME 2
	static uint16_t  slide_switch_pd_cnt = SLIDE_SWITCH_ONTIME;//消抖时间0.5s，见PowerKeyDetect()描述
	if(SoftPowerKeyDetect() && IsTimeOut(&PowerOffDetectTimer))
#else
	#define SLIDE_SWITCH_ONTIME 500
	static uint16_t  slide_switch_pd_cnt = SLIDE_SWITCH_ONTIME;//消抖时间0.5s，见PowerKeyDetect()描述
	if(PowerKeyDetect())
#endif
	{
		if((slide_switch_pd_cnt-- == 0)
#ifdef FUNC_WIFI_EN                             //WiFi升级中禁止关机
		&& (!WiFiFirmwareUpgradeStateGet())
#endif
		)
		{
			/*if slide switch, power down system directly*/
			APP_DBG("PowerKeyDetect->go to PowerDown\n");
#ifdef FUNC_GPIO_POWER_ON_EN
			TimeOutSet(&PowerOffDetectTimer, POWER_OFF_JITTER_TIMER);
			SysPowerOnControl(FALSE);
			SysGotoPowerDownGpioCfg();
#else
			SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_POWERKEY);
			SysGotoPowerDown(); 
			while(1);
#endif
		}
	}
	else
	{
		slide_switch_pd_cnt = SLIDE_SWITCH_ONTIME;
	}
#endif 
	
#ifdef USE_POWERKEY_SOFT_PUSH_BUTTON
#ifdef FUNC_POWERKEY_SOFT_POWERON_EN
	if(SoftPowerKeyDetect()
#else
	if(PowerKeyDetect()
#endif
#ifdef FUNC_WIFI_EN
	//WiFi升级中禁止关机
	&& (!WiFiFirmwareUpgradeStateGet())
#endif
	)
	{
		if((MODULE_ID_POWEROFF != gSys.CurModuleID) && IsTimeOut(&PowerOffDetectTimer))	/*MSG_COMMON_CLOSE only need send once*/
		{
			/*if use push button, send message, for task's saving info.*/
			APP_DBG("PowerKeyDetect->send message common close;\n");
			TimeOutSet(&PowerOffDetectTimer, POWER_OFF_JITTER_TIMER);
		#if 0//def FUNC_WIFI_POWER_KEEP_ON
			if (!IsPowerOff) {
				IsPowerOff = TRUE;
				WiFiRequestMcuPowerOff();
			}
		#else
			gSys.NextModuleID = MODULE_ID_POWEROFF;
			MsgSend(MSG_COMMON_CLOSE);
		#endif
		}
	}
#endif	
	
}

/**
 * @brief 		system power off flow
 *			user can add other functions before SysGotoPowerDown()
 *			
 * @param	None
 * @return	None
 */
void SystemPowerOffControl(void)
{
#ifdef FUNC_AMP_MUTE_EN
	gSys.MuteFlag = TRUE;
	GpioAmpMuteEnable();
#endif
	WaitMs(50);
	APP_DBG("SystemPowerOffControl->system will power off soon!\n");
#ifdef FUNC_WATCHDOG_EN
	WdgDis();				// disable watch dog
#endif

	SysGotoPowerDownGpioCfg();
	SysVarDeinit();
	//APP_DBG("REG_LP_WAKEUP = %d, s_onoff = %d\n", (*(volatile unsigned long*)0x4002200C), PowerkeyGetSOnOff());
	//SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_POWERKEY);
	SysSetWakeUpSrcInPowerDown(WAKEUP_SRC_PD_RTC);
	SysGotoPowerDown(); 

	/*Never reach here,  expect power down fail*/
	APP_DBG("System Power Down Fail!");
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	//关机失败，软件重启芯片。20190722
	//NVIC_SystemReset();
#ifdef FUNC_WATCHDOG_EN
	WdgEn(WDG_STEP_3S);
#endif
	while(1);
}

/**
 * @brief 		system standby flow
 *			user can add other functions before SysGotoDeepSleep()
 *			
 * @param	None
 * @return	None
 */
void SystemStandbyControl(void)
{
	uint32_t Port = GPIOC7;
	APP_DBG("StandBy Mode\n");
	SysVarDeinit();
	
	SysGotoDeepSleepGpioCfg();

	//set GPIOB24 input enable, pull down
	GpioSetRegOneBit(GPIO_C_IE, Port);
	GpioSetRegOneBit(GPIO_C_PU, Port);
	GpioSetRegOneBit(GPIO_C_PD, Port);  

	//set wakeup source GPIOB24, Wakeup Polarith: High wakeup, pin wakeup valid minimal time is 1ms
	SysSetWakeUpSrcInDeepSleep(WAKEUP_SRC_SLEEP_C7, WAKEUP_POLAR_C7_HI, WAKEUP_TMODE_1MS);
	SysGotoDeepSleep();

	/*Never reach here,  expect go to deepsleep fail*/
	APP_DBG("System Go to Deep Sleep Fail!");
	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(1);
}
