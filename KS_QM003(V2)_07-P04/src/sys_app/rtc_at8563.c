#include <string.h>
#include "type.h"
#include "app_config.h"
#include "i2c.h"
#include "clk.h"
#include "gpio.h"
#include "rtc_control.h"

#ifdef FUNC_RTC_AT8563T_EN

#define AT8563T_CHIP_ADDR 	0xA2
#define AT8563T_BASE_ADDR 	0x00

#define AT8563T_TIME_ADDR 	0x02
#define AT8563T_TIME_LEN 	0x07

#define AT8563T_ALARM_ADDR 	0x09
#define AT8563T_ALARM_LEN 	0x04

static void * RtcI2cHandler = NULL;
//RTC初始化时间：2019.8.9 星期5，0：0：0
static const uint8_t InitTime[] = {0x00, 0x00, 0x00, 0x09, 0x05, 0x08, 0x19};
static uint8_t CurRtcTime[7];
static TIMER	RtcScanTimer;
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
bool RtcAt8563tReadTime(uint8_t *Time)
{
	if (I2cReadNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_TIME_ADDR, Time, AT8563T_TIME_LEN)) {
		*Time &= 0x7f;			//秒
		*(Time+1) &= 0x7f;		//分钟
		*(Time+2) &= 0x3f;		//小时
		*(Time+3) &= 0x3f;		//日期
		*(Time+4) &= 0x07;		//星期
		*(Time+5) &= 0x9f;		//月份,保留世纪位
		*(Time+6) &= 0xff;		//年份
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
bool RtcAt8563tWriteTime(uint8_t *Time)
{
	if (I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_TIME_ADDR, Time, AT8563T_TIME_LEN)) {
		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************
 Prototype    : RtcAt8563tPowerOn
 Description  : 第一次上电信息初始化
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
bool RtcAt8563tPowerOn(void)
{
	uint8_t PowerInit[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x05, 0x08, 0x19, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00};
	if (I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_BASE_ADDR, PowerInit, 15)) {
		return TRUE;
	}
	return FALSE;
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
	uint8_t Time[7];
	
	Time[0] = ((CurTime->Sec/10 << 4) + CurTime->Sec%10);
	Time[1] = ((CurTime->Min/10 << 4) + CurTime->Min%10);
	Time[2] = ((CurTime->Hour/10 << 4) + CurTime->Hour%10);
	Time[3] = ((CurTime->Date/10 << 4) + CurTime->Date%10);
	Time[4] = (CurTime->WDay%10);
	if (CurTime->Year/100 >= 20) {
		Time[5] = ((CurTime->Mon/10 << 4) + CurTime->Mon%10);
	}
	else {	
		Time[5] = ((CurTime->Mon/10 << 4) + CurTime->Mon%10)|0x80;
	}
	Time[6] = ((CurTime->Year%100/10 << 4) + CurTime->Year%10);
	RtcAt8563tWriteTime(Time);
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
	uint8_t Time[4], AlarmCmd;

	memset(Time, 0x80, sizeof(Time));
	if (IsOnOff) {
		Time[0] = ((AlarmTime->Min/10 << 4) + AlarmTime->Min%10);
		Time[1] = ((AlarmTime->Hour/10 << 4) + AlarmTime->Hour%10);
		Time[2] = ((AlarmTime->Date/10 << 4) + AlarmTime->Date%10)|0x80;
		Time[3] = (AlarmTime->WDay%10)|0x80;
	}
	if (!I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, AT8563T_ALARM_ADDR, Time, AT8563T_ALARM_LEN)) {
		APP_DBG("AT8563T alarm set fail!!!\n");
	}

	if (IsOnOff) {
		//打开报警及报警中断，
		AlarmCmd = 0x02;
		I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, 0x01, &AlarmCmd, 1);
	}
	else {
		//关闭报警及报警中断，
		AlarmCmd = 0x00;
		I2cWriteNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, 0x01, &AlarmCmd, 1);
	}
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

	TimeOutSet(&RtcScanTimer, 0);
	if(!RtcI2cHandler)
	{
		RtcI2cHandler = I2cMasterCreate(PORT_C, 3, PORT_C, 4);
	}
	
	if(RtcI2cHandler)
	{
		if (RtcAt8563tReadTime(CurRtcTime)) {
			if (CurRtcTime[5]&0x80) {
				APP_DBG("AT8563T power on time : %x: %x: %x: %x: %x: %x: %x\n",
				CurRtcTime[6], CurRtcTime[5], CurRtcTime[3], CurRtcTime[4], CurRtcTime[2], CurRtcTime[1], CurRtcTime[0]);
				RtcAt8563tPowerOn();
				RtcAt8563tWriteTime((uint8_t *)InitTime);
				RtcAt8563tReadTime(CurRtcTime);
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
	uint8_t AlarmMask;

	I2cReadNByte(RtcI2cHandler, AT8563T_CHIP_ADDR, 0x01, &AlarmMask, 1);

	if (AlarmMask&(1<<3)) {
		return TRUE;
	}
	return FALSE;
}
/*****************************************************************************
 Prototype    : RtcAt8563tControlProcess
 Description  : RTC 事件轮询
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
void RtcAt8563tControlProcess(void) 
{
	if (IsTimeOut(&RtcScanTimer)) {
		TimeOutSet(&RtcScanTimer, 10000);
		if (RtcAt8563tReadTime(CurRtcTime)) {
			APP_DBG("AT8563T RtcTime(20%02x-%02x-%02x %02x:%02x:%02x) Week:%x;",
			CurRtcTime[6], CurRtcTime[5], CurRtcTime[3], CurRtcTime[2], CurRtcTime[1], CurRtcTime[0], CurRtcTime[4]);
		}
		else {
			APP_DBG("I2c read AT8563T erro!!!");
		}

		if (RtcAt8563tAlarmCome()) {
			APP_DBG("ALARM COME!!!");
		}

		APP_DBG("\n");
	}
}

#endif	//#ifdef FUNC_RTC_AT8563T_EN

