///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: rtc_control.c
// maintainer: Sam
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "os.h"
#include "rtc.h"
#include "sys_app.h"
#include "recorder_control.h"
#include "dev_detect_driver.h"
#include "sound_remind.h"
#include "nvm.h"
#include "sys_vol.h"
#include "breakpoint.h"
#include "rtc_control.h"

#ifdef FUNC_WIFI_EN
#include "wifi_control.h"
#endif
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#endif

#ifdef FUNC_RTC_EN
RTC_CONTROL* sRtcControl = NULL;
NVM_RTC_INFO sNvmRtcInfo; 
static TIMER RtcAutoOutTimer;

//相关参数宏定义
#define RTC_AUTO_OUT_TIME	         15000                 //设置项自动退出时间：单位mS
#define RTC_ALARM_RING_INTERVAL      1                     //闹钟闹响时铃声间隔：单位S
#define RTC_ALARM_RING_TIME          60                    //闹钟闹铃持续时间： 单位S
#define RTC_SECOND_ALARM_TIME        9                     //闹钟贪睡时间：单位min
#define RTC_ALARM_MAX_COUNT          3                     //闹钟循环最大次数

////////////////////////////////////
const RTC_DATE_TIME gSysCurDate = {2018, 01, 01, 01, 00, 00, 00};
const uint8_t Alarm_Ring[7] = {0xff, SOUND_ALARM_RING1, SOUND_ALARM_RING2, SOUND_ALARM_RING3, SOUND_ALARM_RING4, SOUND_ALARM_RING5, 0xff};


#ifdef FUNC_RTC_LUNAR
static void DisplayLunarDate(void)
{
	//农历年份名称
	const uint8_t LunarYearName[12][2] = {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"};
	
	//农历月份名称
	const uint8_t LunarMonthName[12][2] = {"正", "二", "三", "四", "五", "六", "七", "八", "九", "十", "冬", "腊"};
	
	//农历日期名称
	const uint8_t LunarDateName[30][4] = {"初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十", 
									  "十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十",
									  "廿一", "廿二", "廿三", "廿四", "廿五", "廿六", "廿七", "廿八", "廿九", "三十"};
	
	//农历天干名称
	const uint8_t HeavenlyStemName[10][2] = {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};
	
	//农历地支名称
	const uint8_t EarthlyBranchName[12][2] = {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};
	
	RTC_LUNAR_DATE LunarDate;

	SolarToLunar(&sRtcControl->DataTime, &LunarDate);	
	APP_DBG("农历 %d年 ", (uint32_t)LunarDate.Year);
	APP_DBG("%-.2s%-.2s年 ", HeavenlyStemName[GetHeavenlyStem(LunarDate.Year)], 
						  EarthlyBranchName[GetEarthlyBranch(LunarDate.Year)]);
	APP_DBG("%-.2s年 ", LunarYearName[GetEarthlyBranch(LunarDate.Year)]);
	if(LunarDate.IsLeapMonth)
	{
		APP_DBG("闰");
	}
	APP_DBG("%-.2s月", LunarMonthName[LunarDate.Month - 1]);
	
	if(LunarDate.MonthDays == 29)
	{
		APP_DBG("(小)");
	}
	else
	{
		APP_DBG("(大)");
	}
	
	APP_DBG("%-.4s ", LunarDateName[LunarDate.Date - 1]);

	if((LunarDate.Month == 1) && (LunarDate.Date == 1))			//春节
	{
		APP_DBG("春节");
	}
	else if((LunarDate.Month == 1) && (LunarDate.Date == 15))	//元宵节
	{
		APP_DBG("元宵节");
	}
	else if((LunarDate.Month == 5) && (LunarDate.Date == 5))	//端午节
	{
		APP_DBG("端午节");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 7))	//七夕情人节
	{
		APP_DBG("七夕情人节");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 15))	//中元节
	{
		APP_DBG("中元节");
	}
	else if((LunarDate.Month == 8) && (LunarDate.Date == 15))	//中秋节
	{
		APP_DBG("中秋节");
	}
	else if((LunarDate.Month == 9) && (LunarDate.Date == 9))	//重阳节
	{
   		APP_DBG("重阳节");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 8))	//腊八节
	{
	 	APP_DBG("腊八节");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 23))	//小年
	{
		APP_DBG("小年");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == LunarDate.MonthDays))	//除夕
	{
		APP_DBG("除夕");
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RtcTimerCB(uint32_t unused)
{
	RtcGetCurrTime(&sRtcControl->DataTime);

	if(0)//(RTC_STATE_IDLE == sRtcControl->State) || (sRtcControl->SubState == RTC_SET_IDLE))
	{
		APP_DBG("RtcTime(%04d-%02d-%02d %02d:%02d:%02d) Week:%d ",
	        sRtcControl->DataTime.Year, sRtcControl->DataTime.Mon, sRtcControl->DataTime.Date,
	        sRtcControl->DataTime.Hour, sRtcControl->DataTime.Min, sRtcControl->DataTime.Sec,
	        sRtcControl->DataTime.WDay);
#ifdef FUNC_RTC_LUNAR
		DisplayLunarDate();
#endif
		APP_DBG("\n");
	}
	else if((RTC_STATE_SET_TIME == sRtcControl->State) && (sRtcControl->SubState != RTC_SET_IDLE))
	{
		APP_DBG("RTC set Time(%04d-%02d-%02d %02d:%02d:%02d) Week:%d ",
	        sRtcControl->DataTime.Year, sRtcControl->DataTime.Mon, sRtcControl->DataTime.Date,
	        sRtcControl->DataTime.Hour, sRtcControl->DataTime.Min, sRtcControl->DataTime.Sec,
	        sRtcControl->DataTime.WDay);
#ifdef FUNC_RTC_LUNAR
		DisplayLunarDate();
#endif
		APP_DBG("\n");
	}
#ifdef FUNC_RTC_ALARM	
	else if((RTC_STATE_SET_ALARM == sRtcControl->State) && (sRtcControl->SubState != RTC_SET_IDLE))
	{
		APP_DBG("闹钟号:%d ", sRtcControl->AlarmNum);
		switch(sRtcControl->AlarmMode)
		{
				case ALARM_MODE_ONCE_ONLY:
					APP_DBG("闹钟模式:单次");
					APP_DBG("%04d-%02d-%02d (星期%02d) ", 
					sRtcControl->AlarmTime.Year, 
					sRtcControl->AlarmTime.Mon, 
					sRtcControl->AlarmTime.Date, 
					sRtcControl->AlarmTime.WDay);
					break;
	
	
				case ALARM_MODE_PER_DAY:
					APP_DBG("闹钟模式:每天;");
					break;
	
				case ALARM_MODE_PER_WEEK:
					APP_DBG("闹钟模式:每周(%d);", sRtcControl->AlarmTime.WDay);
					break;
				case ALARM_MODE_WORKDAY:
					APP_DBG("闹钟模式:工作日;");
					break;
				case ALARM_MODE_USER_DEFINED:
					APP_DBG("闹钟模式:客户自定义;");
					break;
	
				default:
					APP_DBG(("模式错误(mode error)\n"));
					break;
		}

		switch(sNvmRtcInfo.RingType[sRtcControl->AlarmNum])
		{
				case RTC_RING_SOUND1_TYPE:
					APP_DBG("铃声类型:铃声一;");
					break;
	
				case RTC_RING_SOUND2_TYPE:
					APP_DBG("铃声类型:铃声二;");
					break;
	
				case RTC_RING_SOUND3_TYPE:
					APP_DBG("铃声类型:铃声三;");
					break;
					
				case RTC_RING_SOUND4_TYPE:
					APP_DBG("铃声类型:铃声四;");
					break;
					
				case RTC_RING_SOUND5_TYPE:
					APP_DBG("铃声类型:铃声五;");
					break;

				case RTC_RING_SDCARD_TYPE:
					APP_DBG("铃声类型:SD卡歌曲;");
					break;
	
				default:
					APP_DBG(("不支持的铃声类型;"));
					break;
		}

		APP_DBG("铃声音量:%d;", sNvmRtcInfo.AlarmVolume);
		APP_DBG("闹钟状态:%s;", RtcGetAlarmStatus(sRtcControl->AlarmNum) == ALARM_STATUS_OPENED?"ON":"OFF");
		APP_DBG("AlarmTime(%02d:%02d:%02d)\n", sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec);	
	}
#endif
}

// RTC初始化
bool RtcInitialize(void)
{
	APP_DBG("RtcInitialize\n");
#ifdef FUNC_RTC_ALARM
	NvmRead(RTC_NVM_START_ADDR, (uint8_t*)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
	RtcInit((void *)(sNvmRtcInfo.AlarmTimeList), sizeof(ALARM_TIME_INFO) * MAX_ALARM_NUM);//硬件初始化，不要删除
#else
	RtcInit(NULL, 0); // 该函数必须调用
#endif	
	if(sRtcControl != NULL)
	{
		APP_DBG("Init fail: RTC is running\n");
		return FALSE;
	}

	sRtcControl = APP_MMM_MALLOC(sizeof(RTC_CONTROL), MMM_ALLOC_NORMAL);
	ASSERT(sRtcControl != NULL);
	memset(sRtcControl, 0, sizeof(RTC_CONTROL));

	// 读取eprom或nvram中的：时间日期等信息
	RtcGetCurrTime(&sRtcControl->DataTime);
	//判断是否第一次上电，第一次上电则初始化系统时间
	if(sRtcControl->DataTime.Year < gSysCurDate.Year)
	{
		memcpy(&sRtcControl->DataTime, &gSysCurDate, sizeof(RTC_DATE_TIME));
		RtcSetCurrTime(&sRtcControl->DataTime);
	}
	
#ifdef FUNC_RTC_ALARM
	NVIC_EnableIRQ(RTC_IRQn);           // 打开闹钟提醒中断,必须在sRtcControl内存分配之后
#endif
	
	// 定时器
	TimeOutSet(&RtcAutoOutTimer, 0);
	InitTimer((SW_TIMER*)&sRtcControl->TimerHandle, 1000, (TIMER_CALLBACK)RtcTimerCB);
	StartTimer(&sRtcControl->TimerHandle);
#ifdef FUNC_RTC_ALARM	
	NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#ifdef FUNC_RTC_ALARM_SAVE2FLASH
	{
		BP_RTC_INFO *BpRtcInfo;
		
		BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
		sNvmRtcInfo.AlarmVolume = BpRtcInfo->AlarmVolume;
		for(sRtcControl->AlarmNum = 1; sRtcControl->AlarmNum <= MAX_ALARM_NUM; sRtcControl->AlarmNum++)
		{
			sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = BpRtcInfo->RingType[sRtcControl->AlarmNum-1];
			sRtcControl->AlarmMode = BpRtcInfo->AlarmMode[sRtcControl->AlarmNum-1]; 
			sRtcControl->AlarmTime = BpRtcInfo->AlarmTime[sRtcControl->AlarmNum-1];
			RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
			RtcAlarmSetStatus(sRtcControl->AlarmNum, BpRtcInfo->AlarmState[sRtcControl->AlarmNum-1]);
		}
		BP_SaveInfo(BP_SAVE2FLASH);
	}
#endif
#endif
	return TRUE;
}

// 去初始化
bool RtcDeinitialize(void)
{
	
	APP_DBG("RtcDeinitialize\n");
	if(sRtcControl == NULL)
	{
		return FALSE;
	}

	DeinitTimer(&sRtcControl->TimerHandle);

	// 取消时间日期设置
	if(sRtcControl->State != RTC_STATE_IDLE)
	{

	}
	// 调用驱动接口，停止相关工作

	// 释放资源
	APP_MMM_FREE(sRtcControl);
	sRtcControl = NULL;

	return TRUE;
}


void RtcSubStateDbg(void)
{
	switch(sRtcControl->SubState)
	{
		case RTC_SET_YEAR:
			APP_DBG("设置“年”\n");
			break;
		case RTC_SET_MON:
			APP_DBG("设置“月”\n");
			break;
		case RTC_SET_DAY:
			APP_DBG("设置“日”\n");
			break;
#ifdef FUNC_RTC_ALARM
		case RTC_SET_WEEK:
			APP_DBG("设置“周”\n");
			break;
#endif
		case RTC_SET_HOUR:
			APP_DBG("设置“时”\n");
			break;
		case RTC_SET_MIN:
			APP_DBG("设置“分”\n");
			break;
			
#ifdef FUNC_RTC_ALARM
		case RTC_SET_ALARM_MODE:
			APP_DBG("设置“闹钟模式”\n");
			break;

		case RTC_SET_ALARM_SOURCE:
			APP_DBG("设置“闹钟铃声类型”\n");
			break;

		case RTC_SET_ALARM_VOLUME:
			APP_DBG("设置“闹钟铃声音量”\n");
			break;

		case RTC_SET_ALARM_ONOFF:
			APP_DBG("设置“闹钟状态开关”\n");
			break;
#endif
		default:
			break;
	}
}

// 切换时间设置项
bool RtcSwitchNextSubState(void)
{
	switch(sRtcControl->State)
	{
		case RTC_STATE_IDLE:
			APP_DBG("Failed to switch focus\n");
			return FALSE;
		
		case RTC_STATE_SET_TIME:
			sRtcControl->SubState++;
			if(sRtcControl->SubState > RTC_SET_MIN)
			{
				sRtcControl->SubState = RTC_SET_YEAR;
			}
			else if(sRtcControl->SubState == RTC_SET_WEEK)//时间设置不支持周时间设置
			{
				sRtcControl->SubState++;
			}
			break;
			
#ifdef FUNC_RTC_ALARM	
		case RTC_STATE_SET_ALARM:
			//以下代码，控制闹钟设置顺序为：时、分、模式、年、月、日、周、铃声、音量、开关。
			if(sRtcControl->SubState == RTC_SET_MIN)
			{
				sRtcControl->SubState = RTC_SET_ALARM_MODE;
			}
			else if(sRtcControl->SubState == RTC_SET_ALARM_MODE)
			{
				sRtcControl->SubState = RTC_SET_YEAR;
			}
			else if(sRtcControl->SubState == RTC_SET_WEEK)
			{
				sRtcControl->SubState = RTC_SET_ALARM_SOURCE;
			}
			else
			{
				sRtcControl->SubState++;
			}
			
			//所有闹钟设置，从小时开始RTC_SET_YEAR;		
			if(sRtcControl->SubState >= RTC_SET_MAX)
			{
				sRtcControl->SubState = RTC_SET_HOUR;             
			}

			//根据闹钟模式，跳过一项或多项设置
			if(sRtcControl->AlarmMode == ALARM_MODE_PER_WEEK)      //周闹钟不支持年、月、日设置
			{
				if(sRtcControl->SubState < RTC_SET_WEEK)         
				{
					sRtcControl->SubState = RTC_SET_WEEK;
				}
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_ONCE_ONLY)//单次闹钟不支持周设置
			{
				if(sRtcControl->SubState == RTC_SET_WEEK)
				{
					sRtcControl->SubState = RTC_SET_ALARM_SOURCE;          
				}
			}
			else                                                   //其他闹钟不支持年月日周设置
			{
				if(sRtcControl->SubState <= RTC_SET_WEEK)
				{
					sRtcControl->SubState = RTC_SET_ALARM_SOURCE;  //跳到设置闹钟铃声
				}
			}

			//根据设置状态，设置相关参数
			if(sRtcControl->SubState == RTC_SET_ALARM_VOLUME)
			{
				if((sNvmRtcInfo.AlarmVolume > MAX_VOLUME) || (sNvmRtcInfo.AlarmVolume == 0))
				{
					sNvmRtcInfo.AlarmVolume = DEFAULT_VOLUME;
				}
			}
			else if(sRtcControl->SubState == RTC_SET_ALARM_SOURCE)
			{
				if((sNvmRtcInfo.RingType[sRtcControl->AlarmNum] >= RTC_RING_TYPE_MAX)
				|| (sNvmRtcInfo.RingType[sRtcControl->AlarmNum] < RTC_RING_SOUND1_TYPE))
				{
					sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = RTC_RING_SOUND1_TYPE;
				}
			}
			break;
#endif
			
		default:
			break;
	}

	APP_DBG("sRtcControl->SubState:%d\n", sRtcControl->SubState);
	return TRUE;
}

void RtcControlEnterSetMode(void)
{
	if(sRtcControl->SubState == RTC_STATE_IDLE)
	{
#ifdef FUNC_RTC_ALARM
		if(sRtcControl->State == RTC_STATE_SET_ALARM)
		{
			APP_DBG("Set Alarm Time Mode\n");
			sRtcControl->SubState = RTC_SET_HOUR;                      //所有闹钟设置，从小时开始RTC_SET_YEAR;
			sRtcControl->AlarmTime.Year = sRtcControl->DataTime.Year;
			sRtcControl->AlarmTime.Mon = sRtcControl->DataTime.Mon;
			sRtcControl->AlarmTime.Date = sRtcControl->DataTime.Date;  //将闹钟日期设置为当前时间;
		}
		else
#endif
		{
			APP_DBG("Set Rtc Time Mode\n");
			sRtcControl->State = RTC_STATE_SET_TIME;
			sRtcControl->SubState = RTC_SET_YEAR;
		}

		RtcCurAlarmSleepAndStop(RTC_ALARM_STATE_STOP);
	    TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
	    RtcSubStateDbg();
    }
    else                                                               //设置项确认，退出设置模式
    {
    	APP_DBG("Rtc Normer Mode\n");
		sRtcControl->State = RTC_STATE_IDLE;
		sRtcControl->SubState = RTC_SET_IDLE;
		sRtcControl->AlarmNum = 0;
    }
                                             		//清除当闹钟信息
}

void RtcControlSetParm(void)
{
	if(sRtcControl->SubState == RTC_SET_IDLE)
	{
		if((sRtcControl->State == RTC_STATE_SET_ALARM) && (sRtcControl->AlarmNum >= MAX_ALARM_NUM))
		{
			sRtcControl->State = RTC_STATE_SET_TIME;
			sRtcControl->AlarmNum = 0;
		}
		else
		{
			if(sRtcControl->State != RTC_STATE_SET_ALARM)
			{
				sRtcControl->AlarmNum = 0;
			}
			sRtcControl->AlarmNum++;
			RtcGetAlarmTime(sRtcControl->AlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
			sRtcControl->AlarmState = RtcGetAlarmStatus(sRtcControl->AlarmNum); 
			if((sRtcControl->AlarmMode < ALARM_MODE_ONCE_ONLY) || (sRtcControl->AlarmMode >= ALARM_MODE_USER_DEFINED))
			{
				sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
			}

			sRtcControl->State = RTC_STATE_SET_ALARM;
		}

		sRtcControl->SubState = RTC_SET_IDLE;
		APP_DBG("Rtc set status change: %s\n", sRtcControl->State == RTC_STATE_SET_ALARM? "Set Alarm Mode": "Set Time Mode");
	}
	else if(sRtcControl->SubState != RTC_SET_IDLE)
	{
		RtcSwitchNextSubState();
	}

	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
	RtcSubStateDbg();
}

void RtcTimeUp(RTC_DATE_TIME *Time)
{
	switch(sRtcControl->SubState)
	{
		case RTC_SET_YEAR:
			Time->Year++;
			if(Time->Year > 2099)
			{
				Time->Year = 1980;	
			}
			break;
		case RTC_SET_MON:
			Time->Mon++;
			if(Time->Mon > 12)
			{
				Time->Mon = 1;
			}
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_WEEK:
			Time->WDay++;
			if(Time->WDay > 6)
			{
				Time->WDay = 0;	
			}
			break;
		case RTC_SET_DAY:	
			Time->Date++;
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = 1;
			}
			break;
		case RTC_SET_HOUR:
			Time->Hour++;
			Time->Sec = 0;
			if(Time->Hour > 23)
			{
				Time->Hour = 0;
			}
			break;
		case RTC_SET_MIN:
			Time->Min++;
			Time->Sec = 0;
			if(Time->Min > 59)
			{
				Time->Min = 0;
			}
			break;
		default:
			break;
	}
}

void RtcTimeDown(RTC_DATE_TIME *Time)
{
	switch(sRtcControl->SubState)
	{	
		case RTC_SET_YEAR:
			Time->Year--;
			if(Time->Year < 1980)
			{
				Time->Year = 2099;	
			}
			break;
		case RTC_SET_MON:
			Time->Mon--;
			if(Time->Mon == 0)
			{
				Time->Mon = 12;
			}
			if(Time->Date > RtcGetMonthDays(Time->Year, Time->Mon))
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_WEEK:
			Time->WDay--;			
			if(Time->WDay > 6)
			{
				Time->WDay = 6;	
			}
			break;
		case RTC_SET_DAY:	
			Time->Date--;
			if(Time->Date == 0)
			{
				Time->Date = RtcGetMonthDays(Time->Year, Time->Mon);
			}
			break;
		case RTC_SET_HOUR:
			Time->Hour--;
			Time->Sec = 0;
			if(Time->Hour > 23)
			{
				Time->Hour = 23;
			}
			break;
		case RTC_SET_MIN:
			Time->Min--;
			Time->Sec = 0;
			if(Time->Min > 59)
			{
				Time->Min = 59;
			}
			break;
		default:
			break;
	}
}

// 系统时间当前设置项值的增
void RtcSetTimeUp(void)
{
	if(RTC_SET_IDLE == sRtcControl->SubState)
	{
		//恢复到显示时钟
		APP_DBG("Rtc Normer Mode\n");
		sRtcControl->State = RTC_STATE_IDLE;
		sRtcControl->SubState = RTC_SET_IDLE;
		sRtcControl->AlarmNum = 0;
		return;
	}

	APP_DBG("RtcSetValueUp(state = %d, substate = %d)\n", sRtcControl->State, sRtcControl->SubState);
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		RtcTimeUp(&sRtcControl->DataTime);
		RtcSetCurrTime(&sRtcControl->DataTime);
	}
#ifdef FUNC_RTC_ALARM
	else if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{
		if(RTC_SET_ALARM_MODE == sRtcControl->SubState)
		{
			sRtcControl->AlarmMode++;
			if(sRtcControl->AlarmMode >= ALARM_MODE_USER_DEFINED)
			{
				sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_USER_DEFINED)
			{
				sRtcControl->AlarmData = 0x7E; // 客户可以自己修改
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_WORKDAY)
			{
				sRtcControl->AlarmData = 0x3E;
			}
			else
			{
				sRtcControl->AlarmData = 0x0;
			}
			APP_DBG("+++sRtcControl->ModeData;%x\n", sRtcControl->AlarmData);
		}
		else if(RTC_SET_ALARM_SOURCE == sRtcControl->SubState)         //设置闹钟提示音音源
		{
			sNvmRtcInfo.RingType[sRtcControl->AlarmNum]++;
			if(sNvmRtcInfo.RingType[sRtcControl->AlarmNum] >= RTC_RING_TYPE_MAX)
			{
			  sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = RTC_RING_SOUND1_TYPE;
			}
		}
		else if(RTC_SET_ALARM_ONOFF == sRtcControl->SubState)          //设置闹钟开关
		{
			if(RtcGetAlarmStatus(sRtcControl->AlarmNum) == ALARM_STATUS_OPENED)
			{
				sRtcControl->AlarmState = ALARM_STATUS_CLOSED;
			}
			else
			{
				sRtcControl->AlarmState = ALARM_STATUS_OPENED;
			}
		}
		else if(RTC_SET_ALARM_VOLUME == sRtcControl->SubState)         //设置闹钟提示音音量
		{
			sNvmRtcInfo.AlarmVolume++;
			if(sNvmRtcInfo.AlarmVolume >= MAX_VOLUME)
			{
			  sNvmRtcInfo.AlarmVolume = 0;
			}
		}
		else
		{
			RtcTimeUp(&sRtcControl->AlarmTime);
		}
		
		//设置闹钟及闹钟状态，需要先设置闹钟，最后设置闹钟状态；否则设置时钟时会强制打开闹钟状态
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
		
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#ifdef FUNC_RTC_ALARM_SAVE2FLASH
		{
			BP_RTC_INFO *BpRtcInfo;
				
			BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
			BpRtcInfo->AlarmMode[sRtcControl->AlarmNum-1] = sRtcControl->AlarmMode;
			BpRtcInfo->RingType[sRtcControl->AlarmNum-1] = sNvmRtcInfo.RingType[sRtcControl->AlarmNum];
			BpRtcInfo->AlarmState[sRtcControl->AlarmNum-1] = sRtcControl->AlarmState;
			BpRtcInfo->AlarmTime[sRtcControl->AlarmNum-1] = sRtcControl->AlarmTime;
			BP_SaveInfo(BP_SAVE2FLASH);
		}
#endif
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
#ifdef FUNC_DISP_EN
	DispSendMessage(DEV_SYMBOL_INTERVAL, DISP_MSG_RTC_SET);
#endif
}

// 系统时间当前设置项值的减
void RtcSetTimeDown(void)
{
	if(RTC_SET_IDLE == sRtcControl->SubState)
	{
		//恢复到显示时钟
		APP_DBG("Rtc Normer Mode\n");
		sRtcControl->State = RTC_STATE_IDLE;
		sRtcControl->SubState = RTC_SET_IDLE;
		sRtcControl->AlarmNum = 0;
		return;
	}

	APP_DBG("RtcSetValueDown(state = %d, substate = %d)\n", sRtcControl->State, sRtcControl->SubState);
	if(RTC_STATE_SET_TIME == sRtcControl->State)
	{
		RtcTimeDown(&sRtcControl->DataTime);
		RtcSetCurrTime(&sRtcControl->DataTime);
	}
#ifdef FUNC_RTC_ALARM
	else if(RTC_STATE_SET_ALARM == sRtcControl->State)
	{	
		if(RTC_SET_ALARM_MODE == sRtcControl->SubState)
		{
			sRtcControl->AlarmMode--;
			if((sRtcControl->AlarmMode < ALARM_MODE_ONCE_ONLY) || (sRtcControl->AlarmMode >= ALARM_MODE_USER_DEFINED))
			{
				sRtcControl->AlarmMode = (ALARM_MODE_USER_DEFINED - 1);
			}
			
			if(sRtcControl->AlarmMode == ALARM_MODE_USER_DEFINED)
			{
				sRtcControl->AlarmData = 0x7E; // 客户可以自己修改
			}
			else if(sRtcControl->AlarmMode == ALARM_MODE_WORKDAY)
			{
				sRtcControl->AlarmData = 0x3E;
			}
			else
			{
				sRtcControl->AlarmData = 0x0;
			}
			APP_DBG("---sRtcControl->ModeData;%x\n", sRtcControl->AlarmData);
		}
		else if(RTC_SET_ALARM_SOURCE == sRtcControl->SubState)         //设置闹钟提示音音源
		{
			sNvmRtcInfo.RingType[sRtcControl->AlarmNum]--;
			if((sNvmRtcInfo.RingType[sRtcControl->AlarmNum] >= RTC_RING_TYPE_MAX)
			|| (sNvmRtcInfo.RingType[sRtcControl->AlarmNum] < RTC_RING_SOUND1_TYPE))
			{
			  sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = (RTC_RING_TYPE_MAX-1);
			}
		}
		else if(RTC_SET_ALARM_ONOFF == sRtcControl->SubState)          //设置闹钟开关
		{ 
			if(RtcGetAlarmStatus(sRtcControl->AlarmNum) == ALARM_STATUS_OPENED)
			{
				sRtcControl->AlarmState = ALARM_STATUS_CLOSED;
			}
			else
			{
				sRtcControl->AlarmState = ALARM_STATUS_OPENED;
			}
		}
		else if(RTC_SET_ALARM_VOLUME == sRtcControl->SubState)         //设置闹钟提示音音量
		{
			sNvmRtcInfo.AlarmVolume--;
			if(sNvmRtcInfo.AlarmVolume > MAX_VOLUME)
			{
			  sNvmRtcInfo.AlarmVolume = MAX_VOLUME;
			}
		}
		else
		{
			RtcTimeDown(&sRtcControl->AlarmTime);
		}

		//设置闹钟及闹钟状态，需要先设置闹钟，最后设置闹钟状态；否则设置时钟时会强制打开闹钟状态
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
		
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#ifdef FUNC_RTC_ALARM_SAVE2FLASH
		{
			BP_RTC_INFO *BpRtcInfo;
				
			BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
			BpRtcInfo->AlarmMode[sRtcControl->AlarmNum-1] = sRtcControl->AlarmMode;
			BpRtcInfo->RingType[sRtcControl->AlarmNum-1] = sNvmRtcInfo.RingType[sRtcControl->AlarmNum];
			BpRtcInfo->AlarmState[sRtcControl->AlarmNum-1] = sRtcControl->AlarmState;
			BpRtcInfo->AlarmTime[sRtcControl->AlarmNum-1] = sRtcControl->AlarmTime;
			BP_SaveInfo(BP_SAVE2FLASH);
		}
#endif
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
#ifdef FUNC_DISP_EN
	DispSendMessage(DEV_SYMBOL_INTERVAL, DISP_MSG_RTC_SET);
#endif
}

//检查是否有闹钟正在闹响
bool IsRtcAlarmRingPlaying(uint16_t CheckMsg)
{
	if(sRtcControl == NULL)
	{
		APP_DBG("fail: RTC is not running\n");
		return FALSE;
	}
	else if(CheckMsg == MSG_RTC_SET_PARM)
	{
		return sRtcControl->CurAlarmRun;
	}
	else
	{
		return sRtcControl->CurRingDuration;
	}
}

//当前闹钟进入贪睡模式
//Parma：TRUE 按键进入贪睡，FLSE：超时进入贪睡
bool RtcCurAlarmSleepAndStop(RTC_ALARM_STATE state)
{
	bool Ret = FALSE;
	RTC_DATE_TIME  CurDateTime;
	uint32_t AlarmTemp,DateTemp;
	static uint8_t AlarmRingCnt,CurVolumeBackUp;

	if(sRtcControl == NULL)
	{
		return FALSE;
	}
	
	//读回当前RTC时间,闹钟时间
	RtcGetCurrTime(&CurDateTime); 

	AlarmTemp = sRtcControl->AlarmTime.Hour*10000;
	AlarmTemp += sRtcControl->AlarmTime.Min*100;
	AlarmTemp += sRtcControl->AlarmTime.Sec;

	DateTemp = CurDateTime.Hour*10000;
	DateTemp += CurDateTime.Min*100;
	DateTemp += CurDateTime.Sec;

	if(state != RTC_ALARM_STATE_STOP)
	{
		if((AlarmTemp == DateTemp) && (state == RTC_ALARM_STATE_IDLE))
		{
			Ret = TRUE;
			//第一次开始闹钟，初始化闹钟环境。
			if(sRtcControl->CurRingDuration == FALSE)
			{
				CurVolumeBackUp = gSys.Volume;
				gSys.Volume = sNvmRtcInfo.AlarmVolume;
				SetSysVol();
			}
		}
		else if(((sRtcControl->CurRingDuration != FALSE) && (state == RTC_ALARM_STATE_SLEEP))
			|| (sRtcControl->CurRingDuration >= RTC_ALARM_RING_TIME))
		{
			sRtcControl->AlarmTime.Min += RTC_SECOND_ALARM_TIME;
			sRtcControl->AlarmTime.Sec = 0;
			sRtcControl->CurRingDuration = 0;
			state = RTC_ALARM_STATE_SLEEP;
			AlarmRingCnt++;

			//判断闹钟时、分是否需要进位。
			sRtcControl->AlarmTime.Hour += (sRtcControl->AlarmTime.Min/60);
			sRtcControl->AlarmTime.Hour = (sRtcControl->AlarmTime.Hour%24);
			sRtcControl->AlarmTime.Min = (sRtcControl->AlarmTime.Min%60);

			APP_DBG("进入闹钟贪睡模式：%02d-%02d-%02d; CurRingDuration：%d;AlarmRingCnt :%d;\n", 
			sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec,
			sRtcControl->CurRingDuration, AlarmRingCnt);
			
			Ret = FALSE;
		}
		else if(((AlarmTemp < DateTemp) && ((CurDateTime.Hour-sRtcControl->AlarmTime.Hour) < 2))
			|| ((AlarmTemp > DateTemp) && ((sRtcControl->AlarmTime.Hour - CurDateTime.Hour) >= 2)))
		{ 	
			//计算当前闹钟已经闹响的时间
			DateTemp = (sRtcControl->CurRingDuration%60);
			sRtcControl->CurRingDuration += ((CurDateTime.Sec + 60) - DateTemp)%60;
				
			sRtcControl->AlarmTime.Min += ((CurDateTime.Sec+RTC_ALARM_RING_INTERVAL)/60);
			sRtcControl->AlarmTime.Sec = ((CurDateTime.Sec+RTC_ALARM_RING_INTERVAL)%60);
			state = RTC_ALARM_STATE_IDLE;

			//判断闹钟时、分是否需要进位。
			sRtcControl->AlarmTime.Hour += (sRtcControl->AlarmTime.Min/60);
			sRtcControl->AlarmTime.Hour = (sRtcControl->AlarmTime.Hour%24);
			sRtcControl->AlarmTime.Min = (sRtcControl->AlarmTime.Min%60);

			APP_DBG("进入闹钟贪睡模式：%02d-%02d-%02d; CurRingDuration：%d; AlarmRingCnt: %d\n", 
			sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec,
			sRtcControl->CurRingDuration, AlarmRingCnt);
			
			Ret = FALSE;
		}
	}

	if(state != RTC_ALARM_STATE_IDLE)
	{
		//恢复现场
		if(CurVolumeBackUp != FALSE)
		{
			gSys.Volume = CurVolumeBackUp;
			CurVolumeBackUp = FALSE;
		}
		SetSysVol();
		
		if(state == RTC_ALARM_STATE_STOP || (AlarmRingCnt >= RTC_ALARM_MAX_COUNT))
		{
			sRtcControl->CurAlarmNum = 0;
			sRtcControl->CurAlarmRun = 0;
			sRtcControl->CurRingDuration = 0;
			AlarmRingCnt = 0;
			APP_DBG("Rtc alarm ring is stop!!!!\n");
		}
	}
	
	return Ret;
}

bool RtcControl(void)
{
	uint16_t Msg  = 0;

	APP_DBG("Enter RTC\n");
#ifndef FUNC_MIXER_SRC_EN
	AudioSampleRateSet(44100);
#endif

#ifndef FUNC_WIFI_SUPPORT_RTC_EN 
	//初始化
	if(!RtcInitialize())
	{
		return FALSE;
	}
#endif

	SetModeSwitchState(MODE_SWITCH_STATE_DONE);
	while(Msg != MSG_COMMON_CLOSE)
	{
	
		RtcControlProcess();
		Msg = MsgRecv(20);

		switch(Msg)
		{
			case MSG_POWER:
				APP_DBG("[RTC] MSG_POWER\n");
			#ifdef FUNC_WIFI_POWER_KEEP_ON
				WiFiRequestMcuPowerOff();
			#else
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
			#endif
				break;
			
			case MSG_MODE:
				APP_DBG("Exit RTC\n");
				Msg = MSG_COMMON_CLOSE;
				break;

			case MSG_RTC_SET_PARM:
				RtcControlSetParm();
				break;

            case MSG_WIFI_WPS:
            	RtcControlEnterSetMode();
            	break;
            	
			case MSG_RTC_SET_TIME:					
			case MSG_RTC_SET_ALARM:
				break;
				
				// 设置项的值增加
			case MSG_VOL_UP:
			case MSG_RTC_SET_UP:
				RtcSetTimeUp();
				break;
			
				// 设置项的值减少
			case MSG_VOL_DW:
			case MSG_RTC_SET_DW:
				RtcSetTimeDown();
				break;
			
#ifdef FUNC_RECORD_EN
			case MSG_REC:  //录音
				gSys.ModuleIDBackUp = gSys.CurModuleID;
				gSys.NextModuleID = MODULE_ID_RECORDER;
				gSys.RecordState = RECORD_STATE_REC;
				gSys.RecordSource = RECORD_SOURCE_LINEIN;
				Msg = MSG_COMMON_CLOSE;
				break;
#endif
           
#ifdef FUNC_REC_PLAYBACK_EN
			case MSG_REC_PLAYBACK:
				EnterRecBackPlay(gSys.CurModuleID, IsUDiskLink() ? DEV_ID_USB : DEV_ID_SD, TRUE, 0, 0);
				break;
#endif			

			default:
				CommonMsgProccess(Msg);
				break;
		}

		if(((RTC_STATE_IDLE != sRtcControl->State)||(RTC_SET_IDLE != sRtcControl->SubState)) && IsTimeOut(&RtcAutoOutTimer))
		{
			//恢复到显示时钟
			APP_DBG("Rtc Normer Mode\n");
			sRtcControl->State = RTC_STATE_IDLE;
			sRtcControl->SubState = RTC_SET_IDLE;
			sRtcControl->AlarmNum = 0;
		}
	}
	
#ifndef FUNC_WIFI_SUPPORT_RTC_EN 
	RtcDeinitialize();//模式切换，退出RTC
#endif
    //退出RTC时，清除所有设置状态。
	APP_DBG("Exit Rtc Mode\n");
	sRtcControl->State = RTC_STATE_IDLE;
	sRtcControl->SubState = RTC_SET_IDLE;

	return TRUE;
}

#ifdef FUNC_RTC_EN
bool RtcControlProcess(void)
{
	
	CheckTimer(&sRtcControl->TimerHandle);

#ifdef FUNC_RTC_ALARM	
	if(RTC_STATE_IDLE == sRtcControl->State)
	{		
		if(sRtcControl->CurAlarmNum)
		{
			if(!sRtcControl->CurAlarmRun)
			{
				RtcGetAlarmTime(sRtcControl->CurAlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
				if(sRtcControl->AlarmMode == ALARM_MODE_ONCE_ONLY)
				{
					//单次闹钟，闹响后将闹钟关闭
					RtcAlarmSetStatus(sRtcControl->CurAlarmNum, ALARM_STATUS_CLOSED);
				}
				sRtcControl->AlarmState = RtcGetAlarmStatus(sRtcControl->AlarmNum);
				//刷新当前实时时钟
				RtcGetCurrTime(&sRtcControl->DataTime);
				APP_DBG("RTC ALARM(%d) COME! 闹钟时间%02d-%02d-%02d\n",sRtcControl->CurAlarmNum, 
				sRtcControl->AlarmTime.Hour, sRtcControl->AlarmTime.Min, sRtcControl->AlarmTime.Sec);
				RtcAlarmArrivedProcess();
				//如果闹钟铃声为WiFi SD卡，则自动切换SD模式
				if(IsWiFiCardLink() && (sNvmRtcInfo.RingType[sRtcControl->CurAlarmNum] == RTC_RING_SDCARD_TYPE))
				{
					gSys.NextModuleID = MODULE_ID_PLAYER_WIFI_SD;	
					MsgSend(MSG_MODE);	
					sRtcControl->CurAlarmRun = FALSE;
					sRtcControl->CurAlarmNum = FALSE;
				}
				//如果闹钟为WiFI端闹钟，当前网络连接正常，本地不闹响当前闹钟
				else if((sNvmRtcInfo.RingType[sRtcControl->CurAlarmNum] == RTC_RING_WIFISD_TYPE)
				&& (WiFiWwwStateGet() == WIFI_STATUS_WWW_ENABLE)
				&& (WiFiLoginStateGet() == WIFI_STATUS_LOGIN_SUCCESSFUL))
				{
					sRtcControl->CurAlarmRun = FALSE;
					sRtcControl->CurAlarmNum = FALSE;
				}
				else
				{
					if(sNvmRtcInfo.RingType[sRtcControl->CurAlarmNum] >= RTC_RING_SDCARD_TYPE)
					{
						sNvmRtcInfo.RingType[sRtcControl->CurAlarmNum] = RTC_RING_SOUND1_TYPE;
					}
					sRtcControl->CurAlarmRun = TRUE;
				}
#ifdef FUNC_RTC_ALARM			
				NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#ifdef FUNC_RTC_ALARM_SAVE2FLASH
				{
					BP_RTC_INFO *BpRtcInfo;
						
					BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
					BpRtcInfo->AlarmMode[sRtcControl->AlarmNum-1] = sRtcControl->AlarmMode;
					BpRtcInfo->RingType[sRtcControl->AlarmNum-1] = sNvmRtcInfo.RingType[sRtcControl->AlarmNum];
					BpRtcInfo->AlarmState[sRtcControl->AlarmNum-1] = sRtcControl->AlarmState;
					BpRtcInfo->AlarmTime[sRtcControl->AlarmNum-1] = sRtcControl->AlarmTime;
					BP_SaveInfo(BP_SAVE2FLASH);
				}
#endif
#endif
			}
			else if(sRtcControl->CurAlarmRun && !IsSoundRemindPlaying())
			{
				if(RtcCurAlarmSleepAndStop(RTC_ALARM_STATE_IDLE))
				{
					//播放闹铃提示音。
					SoundRemind(Alarm_Ring[sNvmRtcInfo.RingType[sRtcControl->CurAlarmNum]]);
				}
			}
		}
	}
	
	if((RTC_STATE_SET_ALARM == sRtcControl->State) && !IsSoundRemindPlaying())
	{
		if((RTC_SET_ALARM_SOURCE == sRtcControl->SubState)
		&& (sNvmRtcInfo.RingType[sRtcControl->AlarmNum] < RTC_RING_SDCARD_TYPE))
		{
			SoundRemind(Alarm_Ring[sNvmRtcInfo.RingType[sRtcControl->AlarmNum]]);
		}
	}
#endif
	return TRUE;
}
#endif

#ifdef FUNC_RTC_ALARM
__attribute__((section(".driver.isr"), weak)) void RtcInterrupt(void)
{
	sRtcControl->CurAlarmRun = FALSE;
	sRtcControl->CurAlarmNum = RtcCheckAlarmFlag();
	RtcAlarmIntClear();// 清除闹钟提醒中断
}
#endif
#endif
