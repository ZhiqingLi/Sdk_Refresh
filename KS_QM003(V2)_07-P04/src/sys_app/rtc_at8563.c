#include <string.h>
#include "type.h"
#include "app_config.h"
#include "i2c.h"
#include "clk.h"
#include "gpio.h"
#include "rtc_control.h"
#include "sys_app.h"
#include "power_management.h"
#include "nvm.h"

#ifdef FUNC_RTC_AT8563T_EN

#define AT8563T_CHIP_ADDR 	0xA2
#define AT8563T_BASE_ADDR 	0x00
#define AT8563T_REG_LEN		16

#define AT8563T_TIME_ADDR 	0x02
#define AT8563T_TIME_LEN 	0x07

#define AT8563T_ALARM_ADDR 	0x09
#define AT8563T_ALARM_LEN 	0x04

#define AT8563T_SYSTEM_CONTROL_ADDR		0x00
#define AT8563T_STATE_CONTROL_ADDR		0x01

#define AT8563T_TIMER_SEC_ADDR			0x02
#define AT8563T_TIMER_MIN_ADDR			0x03
#define AT8563T_TIMER_HOUR_ADDR			0x04
#define AT8563T_TIMER_WDAY_ADDR			0x06
#define AT8563T_TIMER_DATE_ADDR			0x05
#define AT8563T_TIMER_MON_ADDR			0x07
#define AT8563T_TIMER_YEAR_ADDR			0x08

#define AT8563T_ALARM_MIN_ADDR			0x09
#define AT8563T_ALARM_HOUR_ADDR			0x0A
#define AT8563T_ALARM_DATE_ADDR			0x0B
#define AT8563T_ALARM_WDAY_ADDR			0x0C

#define AT8563T_CLKOUT_CONTROL_ADDR		0x0D
#define AT8563T_TIMER_CONTROL_ADDR		0x0E
#define AT8563T_TIMER_COUNT_ADDR		0x0F

static void * RtcI2cHandler = NULL;
//RTC初始化时间：2019.8.9 星期5，0：0：0					//系统：状态：秒 ：分钟：小时：日期：星期：月份：年份：警分：警时：警日：警星：时钟：计时：计数
static const uint8_t	InitTime[AT8563T_REG_LEN] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x02, 0x08, 0x19, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00};
static uint8_t			CurRtcTime[AT8563T_REG_LEN];
static bool				IsAt8563tInitOk = FALSE, IsSecondAlarm = FALSE;	
/*****************************************************************************
 Prototype    : RtcAt8563tReadTime
 Description  : 读RTC时间
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/8/9
    Author       : qing
    Modification : Created function

*****************************************************************************/
static bool RtcAt8563tReadParam(void)
{
	if (I2cReadNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_BASE_ADDR, CurRtcTime, AT8563T_REG_LEN)) {
		CurRtcTime[AT8563T_TIMER_SEC_ADDR] &= 0x7f;			//秒
		CurRtcTime[AT8563T_TIMER_MIN_ADDR] &= 0x7f;			//分钟
		CurRtcTime[AT8563T_TIMER_HOUR_ADDR] &= 0x3f;		//小时
		CurRtcTime[AT8563T_TIMER_DATE_ADDR] &= 0x3f;		//日期
		CurRtcTime[AT8563T_TIMER_WDAY_ADDR] &= 0x07;		//星期
		CurRtcTime[AT8563T_TIMER_MON_ADDR] &= 0x9f;			//月份,保留世纪位
		CurRtcTime[AT8563T_TIMER_YEAR_ADDR] &= 0xff;		//年份
		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************
 Prototype    : RtcAt8563tWriteTime
 Description  : 写时间到AT8563T
 Input        : uint8_t *Time  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/8/9
    Author       : qing
    Modification : Created function

*****************************************************************************/
static bool RtcAt8563tWriteParam(void)
{
	if (I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_BASE_ADDR, CurRtcTime, AT8563T_REG_LEN)) {
		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************
 Prototype    : ResetAt8563tSecondAlarm
 Description  : Reset second alarm
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/9/2
    Author       : qing
    Modification : Created function

*****************************************************************************/
void SetAt8563tSecondAlarm(bool SecondAlarm) 
{
	IsSecondAlarm = SecondAlarm;
}
/*****************************************************************************
 Prototype    : WiFiSetAt8563tSystemTime
 Description  : WiFi同步当前系统时间
 Input        : RTC_DATE_TIME* CurTime  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/8/9
    Author       : qing
    Modification : Created function

*****************************************************************************/
void WiFiSetAt8563tSystemTime(RTC_DATE_TIME* CurTime)
{	
	if (!IsAt8563tInitOk) {
		return;
	}
	
	CurRtcTime[AT8563T_TIMER_SEC_ADDR] = ((CurTime->Sec/10 << 4) + CurTime->Sec%10);
	CurRtcTime[AT8563T_TIMER_MIN_ADDR] = ((CurTime->Min/10 << 4) + CurTime->Min%10);
	CurRtcTime[AT8563T_TIMER_HOUR_ADDR] = ((CurTime->Hour/10 << 4) + CurTime->Hour%10);
	CurRtcTime[AT8563T_TIMER_DATE_ADDR] = ((CurTime->Date/10 << 4) + CurTime->Date%10);
	CurRtcTime[AT8563T_TIMER_WDAY_ADDR] = (CurTime->WDay%10);
	if (CurTime->Year/100 >= 20) {
		CurRtcTime[AT8563T_TIMER_MON_ADDR] = ((CurTime->Mon/10 << 4) + CurTime->Mon%10);
	}
	else {	
		CurRtcTime[AT8563T_TIMER_MON_ADDR] = ((CurTime->Mon/10 << 4) + CurTime->Mon%10)|0x80;
	}
	CurRtcTime[AT8563T_TIMER_YEAR_ADDR] = ((CurTime->Year%100/10 << 4) + CurTime->Year%10);
	if (!I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_TIME_ADDR, &CurRtcTime[AT8563T_TIME_ADDR], AT8563T_TIME_LEN)) {
		APP_DBG("AT8563T alarm set fail!!!\n");
	}
}

/*****************************************************************************
 Prototype    : WiFiGetAt8563tSystemTime
 Description  : WiFi获取RTC时间
 Input        : RTC_DATE_TIME* SystemTime  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/8/20
    Author       : qing
    Modification : Created function

*****************************************************************************/
void WiFiGetAt8563tSystemTime(RTC_DATE_TIME* SystemTime)
{
	if (!IsAt8563tInitOk) {
		return;
	}
	
	if (RtcAt8563tReadParam()) {
		SystemTime->Sec = CurRtcTime[AT8563T_TIMER_SEC_ADDR]/16*10 + CurRtcTime[AT8563T_TIMER_SEC_ADDR]%16;
		SystemTime->Min = CurRtcTime[AT8563T_TIMER_MIN_ADDR]/16*10 + CurRtcTime[AT8563T_TIMER_MIN_ADDR]%16;
		SystemTime->Hour = CurRtcTime[AT8563T_TIMER_HOUR_ADDR]/16*10 + CurRtcTime[AT8563T_TIMER_HOUR_ADDR]%16;
		SystemTime->WDay = CurRtcTime[AT8563T_TIMER_WDAY_ADDR]%16;
		SystemTime->Date = CurRtcTime[AT8563T_TIMER_DATE_ADDR]/16*10 + CurRtcTime[AT8563T_TIMER_DATE_ADDR]%16;
		SystemTime->Mon = (CurRtcTime[AT8563T_TIMER_MON_ADDR]&0x1F)/16*10 + CurRtcTime[AT8563T_TIMER_MON_ADDR]%16;
		if (CurRtcTime[AT8563T_TIMER_MON_ADDR]&0x80) {
			SystemTime->Year = CurRtcTime[AT8563T_TIMER_YEAR_ADDR]/16*10 + CurRtcTime[AT8563T_TIMER_YEAR_ADDR]%16 + 1900;
		}
		else {
			SystemTime->Year = CurRtcTime[AT8563T_TIMER_YEAR_ADDR]/16*10 + CurRtcTime[AT8563T_TIMER_YEAR_ADDR]%16 + 2000;
		}
		//闹钟时间到，
		if (RtcAt8563tAlarmCome()) {
			APP_DBG("AT8563T alarm in comeing!!!\n");
#ifdef FUNC_GPIO_POWER_ON_EN
			SysPowerOnControl(TRUE);
#endif
			if (!IS_CUR_WORK_MODULE()) {
				APP_DBG("AT8563T alarm wakeup WiFi!!!\n");
				gSys.NextModuleID = MODULE_ID_WIFI;
				MsgSend(MSG_COMMON_CLOSE);
				gSys.WakeUpSource = WAKEUP_FLAG_POR_RTC;
			}

			if (!IsSecondAlarm && (GetCurBatterLevelAverage() >= 10)
#ifdef OPTION_CHARGER_DETECT
			&& !IsInCharge()
#endif
			) {
				RTC_DATE_TIME AlarmTime;
				
				SetAt8563tSecondAlarm(TRUE);
				//第一次闹响，设置1分钟后的闹钟，防止闹钟唤醒后WiFi未启动而用户关机
				memcpy((void*)&AlarmTime, (void*)SystemTime, sizeof(RTC_DATE_TIME));
				//分钟加1；
				AlarmTime.Min++;
				if (AlarmTime.Min >= 60) {
					AlarmTime.Hour++;
					AlarmTime.Min %= 60;
					AlarmTime.Hour %= 24;
				}
				AlarmTime.Sec = 0;

				WiFiSetAt8563tAlarmTime(TRUE, &AlarmTime);
				//闹钟数据同时设置到AP80
				sRtcControl->AlarmNum = 1;
				sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
				RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &AlarmTime);
				RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
				APP_DBG("SetSecondAralmTime(AralmNum:%d; %02d:%02d:%02d)!!!\n", sRtcControl->AlarmNum, 
						sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec);
#ifdef FUNC_RTC_ALARM		
				NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
			}
			else {
				//第二次闹钟闹响，关闭闹钟
				WiFiSetAt8563tAlarmTime(FALSE, NULL);
				SetAt8563tSecondAlarm(FALSE);
			}
		}
	}
	else {
		APP_DBG("Read at8563t system time erro!!!\n");
	}
}

/*****************************************************************************
 Prototype    : WiFiSetAt8563tAlarmTime
 Description  : wifi设置闹钟时间
 Input        : RTC_DATE_TIME* AlarmTime  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/8/9
    Author       : qing
    Modification : Created function

*****************************************************************************/
void WiFiSetAt8563tAlarmTime(bool IsOnOff, RTC_DATE_TIME* AlarmTime)
{
	uint8_t AlarmCmd;

	if (!IsAt8563tInitOk) {
		return;
	}
	//因为闹钟只能到分钟，所以秒大于30，分钟增加一分钟
	if (AlarmTime->Sec > 30) {
		AlarmTime->Min++;
		if (AlarmTime->Min >= 60) {
			AlarmTime->Hour++;
			AlarmTime->Min %= 60;
			AlarmTime->Hour %= 24;
		}
	}
	
	if (IsOnOff) {
		CurRtcTime[AT8563T_ALARM_MIN_ADDR] = ((AlarmTime->Min/10 << 4) + AlarmTime->Min%10);
		CurRtcTime[AT8563T_ALARM_HOUR_ADDR] = ((AlarmTime->Hour/10 << 4) + AlarmTime->Hour%10);
		CurRtcTime[AT8563T_ALARM_DATE_ADDR] = ((AlarmTime->Date/10 << 4) + AlarmTime->Date%10)|0x80;
		CurRtcTime[AT8563T_ALARM_WDAY_ADDR] = (AlarmTime->WDay%10)|0x80;
		CurRtcTime[AT8563T_STATE_CONTROL_ADDR] = 0x02;					//清楚报警并打开报警中断，
	}
	else {
		CurRtcTime[AT8563T_ALARM_MIN_ADDR] = 0x80;
		CurRtcTime[AT8563T_ALARM_HOUR_ADDR] = 0x80;
		CurRtcTime[AT8563T_ALARM_DATE_ADDR] = 0x80;
		CurRtcTime[AT8563T_ALARM_WDAY_ADDR] = 0x80;
		CurRtcTime[AT8563T_STATE_CONTROL_ADDR] = 0x00;					//清楚报警并关闭报警中断
		IsSecondAlarm = FALSE;
	}

	if (!I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_ALARM_ADDR, &CurRtcTime[AT8563T_ALARM_ADDR], AT8563T_ALARM_LEN)) {
		APP_DBG("AT8563T alarm set fail!!!\n");
	}

	do {
		AlarmCmd = 0xff;
		WaitMs(10);
		I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_STATE_CONTROL_ADDR, &CurRtcTime[AT8563T_STATE_CONTROL_ADDR], 1);
		I2cReadNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_STATE_CONTROL_ADDR, &AlarmCmd, 1);
	} while (AlarmCmd&0x03 != CurRtcTime[AT8563T_STATE_CONTROL_ADDR]);
	
	APP_DBG("AT8563T alarm set: %d : %d!!!\n", IsOnOff, AlarmCmd&0x03);
}

/*****************************************************************************
 Prototype    : RtcAt8563tInit
 Description  : 芯片初始化
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/8/9
    Author       : qing
    Modification : Created function

*****************************************************************************/
bool RtcAt8563tInit(void)
{
	DBG("AT8563T Init\n");

	memset (CurRtcTime, 0x00, sizeof(CurRtcTime));
	if(!RtcI2cHandler)
	{
		RtcI2cHandler = I2cMasterCreate(PORT_B, 20, PORT_B, 21);
	}
	
	if(RtcI2cHandler)
	{
		if (RtcAt8563tReadParam()) {
			if (CurRtcTime[AT8563T_TIMER_MON_ADDR]&0x80) {
				APP_DBG("AT8563T power on time : %x: %x: %x: %x: %x: %x: %x\n",
				CurRtcTime[AT8563T_TIMER_YEAR_ADDR], CurRtcTime[AT8563T_TIMER_MON_ADDR], CurRtcTime[AT8563T_TIMER_DATE_ADDR], 
				CurRtcTime[AT8563T_TIMER_WDAY_ADDR], CurRtcTime[AT8563T_TIMER_HOUR_ADDR], CurRtcTime[AT8563T_TIMER_MIN_ADDR], CurRtcTime[AT8563T_TIMER_SEC_ADDR]);
				memcpy(CurRtcTime, InitTime, sizeof(CurRtcTime));
				RtcAt8563tWriteParam();
				RtcAt8563tReadParam();
			}
			
			IsAt8563tInitOk = TRUE;
			//闹钟唤醒开机。
			if (RtcAt8563tAlarmCome()) {	
				gSys.WakeUpSource = WAKEUP_FLAG_POR_RTC;
				APP_DBG("AT8563T alarm wakeup system;\n");
			}
		}
		else {
			APP_DBG("Rtc AT8563T read erro!!!\n");
			return FALSE;
		}
	}
	else
	{
		APP_DBG("RtcI2cMasterCreate Fail!\n");
		return FALSE;
	}

	return TRUE;
}

/*****************************************************************************
 Prototype    : RtcAt8563tAlarmCome
 Description  : 检测闹钟是否到了
 Input        : void  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2019/8/9
    Author       : qing
    Modification : Created function

*****************************************************************************/
bool RtcAt8563tAlarmCome(void) 
{	
	if (!IsAt8563tInitOk) {
		return FALSE;
	}
	return (CurRtcTime[AT8563T_STATE_CONTROL_ADDR]&(1<<3));
}

#endif	//#ifdef FUNC_RTC_AT8563T_EN

