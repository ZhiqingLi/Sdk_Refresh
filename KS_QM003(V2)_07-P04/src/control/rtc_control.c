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
#ifdef FUNC_RTC_ALARM_SAVE2FLASH
ALARM_BP_INFO* sRtcMcuAlarm = NULL;
#endif
NVM_RTC_INFO sNvmRtcInfo; 
static TIMER RtcAutoOutTimer;

//相关参数宏定义
#define RTC_AUTO_OUT_TIME	         15000                 //设置项自动退出时间：单位mS
#define RTC_ALARM_RING_INTERVAL      1                     //闹钟闹响时铃声间隔：单位S
#define RTC_ALARM_RING_TIME          60                    //闹钟闹铃持续时间： 单位S
#define RTC_SECOND_ALARM_TIME        3                     //闹钟贪睡时间：单位min
#define RTC_ALARM_MAX_COUNT          3                     //闹钟循环最大次数

////////////////////////////////////
const RTC_DATE_TIME gSysCurDate = {2019, 06, 01, 01, 23, 59, 59};
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

#if (defined(FUNC_RTC_EN) && defined(FUNC_RTC_ALARM_SAVE2FLASH))
/*****************************************************************************
 函 数 名  : IsAlarmTimeNumInInfo
 功能描述  : 查找闹钟是否已经在索引区
 输入参数  : uint8_t BufTail   
             uint8_t NumIndex  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月6日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
bool IsAlarmTimeNumInInfo(uint8_t BufTail, uint8_t NumIndex)
{
    BP_RTC_INFO *BpRtcInfo;
    uint8_t Temp;

    BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);

    for (Temp = 0; Temp < BufTail; Temp++)
    {
        if(BpRtcInfo->AlarmIndex[Temp] == NumIndex)
        {
            return TRUE;
        }
    }

    return FALSE;
}
/*****************************************************************************
 函 数 名  : UpdataAlarmTimeIndex
 功能描述  : 刷新闹钟时间顺序索引，从近到远
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月6日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void UpdataAlarmTimeIndex(void)
{
    uint8_t TempBuf,i,j;
    BP_RTC_INFO *BpRtcInfo;
    RTC_DATE_TIME BpAlarmTime;

    BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
    memset((uint8_t *)&BpRtcInfo->AlarmIndex, 0xff, MAX_BP_INFO_ALARM_NUM+MAX_ALARM_NUM);
    
    for (i = 0; i < (BpRtcInfo->ValidAlarmNum+MAX_ALARM_NUM); i++)
    {
        /* 查找第一个还没有排序的闹钟编号 */
        for(TempBuf = 0; TempBuf < (MAX_BP_INFO_ALARM_NUM+MAX_ALARM_NUM); TempBuf++)
        {
            if(TempBuf == BpRtcInfo->ValidAlarmNum)
            {
                TempBuf = MAX_BP_INFO_ALARM_NUM;
            }
            
            if((ALARM_STATUS_OPENED == BpRtcInfo->AlarmBpInfo[TempBuf].AlarmState)
            && !IsAlarmTimeNumInInfo(i, TempBuf))
            {
                break;
            }
        }
        /* 没有有效闹钟需要处理，直接退出 */
        if ((MAX_BP_INFO_ALARM_NUM+MAX_ALARM_NUM) <= TempBuf)
        {
            break;
        }
        /* 从第一个没有排序的闹钟开始，顺序查找一个最小且没有排序的闹钟 */
        for (j = TempBuf; j < (MAX_BP_INFO_ALARM_NUM+MAX_ALARM_NUM); j++)
        {
            if(j == BpRtcInfo->ValidAlarmNum)
            {
                j = MAX_BP_INFO_ALARM_NUM;
            }
            
            if((ALARM_STATUS_OPENED != BpRtcInfo->AlarmBpInfo[j].AlarmState)
            || (TRUE == IsAlarmTimeNumInInfo(i, j)))
            {
                continue;
            }
            
            BpAlarmTime = BpRtcInfo->AlarmBpInfo[j].AlarmTime;
            BpAlarmTime.WDay = BpRtcInfo->AlarmBpInfo[TempBuf].AlarmTime.WDay;
            
            if(memcmp((void *)&BpRtcInfo->AlarmBpInfo[TempBuf].AlarmTime, (void *)&BpAlarmTime, sizeof(RTC_DATE_TIME)) > 0)
            {
                TempBuf = j;
            }        
        }

        BpRtcInfo->AlarmIndex[i] = TempBuf;
    }
}

/*****************************************************************************
 函 数 名  : GetWiFiAlarmTimeFromBpInfo
 功能描述  : 从记忆区获取一个WiFi闹钟数据
 输入参数  : ALARM_BP_INFO *AlarmInfo  
             uint8_t AlarmNum          
 输出参数  : 无
 返 回 值: TRUE:获取成功；FALSE：记忆区已经没有闹钟数据 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月4日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
bool GetNearAlarmTimeFromBpInfo(ALARM_BP_INFO *AlarmInfo, uint8_t AlarmNum)
{
    uint8_t TempIndex,DetIndex;
    BP_RTC_INFO *BpRtcInfo;
    RTC_DATE_TIME CurSysTime,BpAlarmTime;

    BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
    RtcGetCurrTime(&CurSysTime);
    
    /* 查询记忆中的时间，是否有超时的闹钟，如有则将其删除 */
    for (TempIndex = 0; TempIndex < BpRtcInfo->ValidAlarmNum; )
    {
        BpAlarmTime = BpRtcInfo->AlarmBpInfo[TempIndex].AlarmTime;
        /* 跳过星期判断 */
        BpAlarmTime.WDay = CurSysTime.WDay;
        if(memcmp((void *)&BpAlarmTime, (void *)&CurSysTime, sizeof(RTC_DATE_TIME)) <= 0)
        {
            DetIndex = TempIndex;
            BpRtcInfo->ValidAlarmNum--;
            while (DetIndex < BpRtcInfo->ValidAlarmNum)
            {
                BpRtcInfo->AlarmBpInfo[DetIndex] = BpRtcInfo->AlarmBpInfo[DetIndex+1];
                DetIndex++;
            } 
            /* 清除记忆区已经超时闹钟 */
            memset((uint8_t *)&BpRtcInfo->AlarmBpInfo[BpRtcInfo->ValidAlarmNum], 0, sizeof(ALARM_BP_INFO));
            BpRtcInfo->AlarmBpInfo[BpRtcInfo->ValidAlarmNum].AlarmState = ALARM_STATUS_CLOSED;
            UpdataAlarmTimeIndex();
            continue;
        }
        ++TempIndex;
    }

    /* 查询记忆中的时间，是否有超时的闹钟，如有则将其删除 */
    for (TempIndex = 0; TempIndex < MAX_ALARM_NUM; ++TempIndex)
    {
        /* 本地闹钟存放在WiFi闹钟的后面 */
        BpAlarmTime = BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime;
        /* 跳过星期判断 */
        BpAlarmTime.WDay = CurSysTime.WDay;
        if(memcmp((void *)&BpAlarmTime, (void *)&CurSysTime, sizeof(RTC_DATE_TIME)) <= 0)
        {
            if(ALARM_MODE_ONCE_ONLY == BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmMode)
            {
                memset((void *)&BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex], 0, sizeof(ALARM_BP_INFO));
                BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmMode = ALARM_STATUS_CLOSED;
            }
            else
            {
                BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Date++;
        		if(BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Date > RtcGetMonthDays(CurSysTime.Year, CurSysTime.Mon))
        		{
        			BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Date = 1;
        			BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Mon++;
        			if(BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Mon > 12)
        			{
        				BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Mon -= 12;
        				BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Year++;
        			}
        		}                
            }
            UpdataAlarmTimeIndex();
        }
    }
    
    if((AlarmNum < (BpRtcInfo->ValidAlarmNum+MAX_ALARM_NUM))
    && (BpRtcInfo->AlarmIndex[AlarmNum] < (MAX_BP_INFO_ALARM_NUM+MAX_ALARM_NUM))
    && (BpRtcInfo->AlarmBpInfo[BpRtcInfo->AlarmIndex[AlarmNum]].AlarmState == ALARM_STATUS_OPENED))
    {
        *AlarmInfo = BpRtcInfo->AlarmBpInfo[BpRtcInfo->AlarmIndex[AlarmNum]];
        return TRUE;
    }

    return FALSE;
}
/*****************************************************************************
 函 数 名  : SetWiFiAlarmTimeToBpInfo
 功能描述  : 将闹钟数据存入WiFi闹钟记忆区
 输入参数  : ALARM_BP_INFO *AlarmInfo  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月4日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void SetWiFiAlarmTimeToBpInfo(ALARM_BP_INFO *AlarmInfo)
{
    uint8_t TempIndex,DetIndex;
    BP_RTC_INFO *BpRtcInfo;
    RTC_DATE_TIME CurSysTime,BpAlarmTime;

    BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
    RtcGetCurrTime(&CurSysTime);
    
    /* 查询记忆中的时间，是否有超时的闹钟，如有则将其删除 */
    for (TempIndex = 0; TempIndex < BpRtcInfo->ValidAlarmNum; )
    {
        BpAlarmTime = BpRtcInfo->AlarmBpInfo[TempIndex].AlarmTime;
        /* 跳过星期判断 */
        BpAlarmTime.WDay = CurSysTime.WDay;
        if(memcmp((void *)&BpAlarmTime, (void *)&CurSysTime, sizeof(RTC_DATE_TIME)) <= 0)
        {
            DetIndex = TempIndex;
            BpRtcInfo->ValidAlarmNum--;
            while (DetIndex < BpRtcInfo->ValidAlarmNum)
            {
                BpRtcInfo->AlarmBpInfo[DetIndex] = BpRtcInfo->AlarmBpInfo[DetIndex+1];
                DetIndex++;
            } 
            /* 清除记忆区已经超时闹钟 */
            memset((uint8_t *)&BpRtcInfo->AlarmBpInfo[BpRtcInfo->ValidAlarmNum], 0, sizeof(ALARM_BP_INFO));
            BpRtcInfo->AlarmBpInfo[BpRtcInfo->ValidAlarmNum].AlarmState = ALARM_STATUS_CLOSED;
            continue;
        }
        ++TempIndex;
    }
    /* 定位当前闹钟存放的位置 */
    for ( TempIndex = 0 ; TempIndex < BpRtcInfo->ValidAlarmNum ; TempIndex++ )
    {
        BpAlarmTime = BpRtcInfo->AlarmBpInfo[TempIndex].AlarmTime;
        /* 跳过星期判断 */
        BpAlarmTime.WDay = AlarmInfo->AlarmTime.WDay;
        if(memcmp((void *)&BpAlarmTime, (void *)&AlarmInfo->AlarmTime, sizeof(RTC_DATE_TIME)) == 0)
        {
            BpRtcInfo->AlarmBpInfo[TempIndex] = *AlarmInfo;
            break;
        }
        if(memcmp((void *)&BpAlarmTime, (void *)&AlarmInfo->AlarmTime, sizeof(RTC_DATE_TIME)) > 0)
        {
            DetIndex = BpRtcInfo->ValidAlarmNum;
            if(MAX_BP_INFO_ALARM_NUM > BpRtcInfo->ValidAlarmNum)
            {
                BpRtcInfo->AlarmBpInfo[DetIndex] = BpRtcInfo->AlarmBpInfo[DetIndex-1];
                BpRtcInfo->ValidAlarmNum++;
            }
            DetIndex--;
            while (DetIndex > TempIndex)
            {
                BpRtcInfo->AlarmBpInfo[DetIndex] = BpRtcInfo->AlarmBpInfo[DetIndex-1];
                DetIndex--;
            }
            BpRtcInfo->AlarmBpInfo[TempIndex] = *AlarmInfo;
            break;
        }
    }

    if((TempIndex == BpRtcInfo->ValidAlarmNum) && (MAX_BP_INFO_ALARM_NUM > BpRtcInfo->ValidAlarmNum))
    {
        BpRtcInfo->AlarmBpInfo[TempIndex] = *AlarmInfo;
        BpRtcInfo->ValidAlarmNum++;
    }
    else if(MAX_BP_INFO_ALARM_NUM <= BpRtcInfo->ValidAlarmNum)
    {
        APP_DBG("Rtc BpInfo buffer is overflow!!!\n");
    }

    UpdataAlarmTimeIndex();
}
/*****************************************************************************
 函 数 名  : SyncMcuAlarmTimeFromBpInfo
 功能描述  : 同步记忆区的MCU本地闹钟
 输入参数  : bool IsLoad                 TRUE:从记忆区读；FALSE: 存入记忆区        
           ALARM_BP_INFO *AlarmInfo  闹钟数据指针
           uint8_t AlarmNum          闹钟号；        
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月4日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void SyncMcuAlarmTimeToBpInfo(ALARM_BP_INFO *AlarmInfo,uint8_t AlarmNum)
{
    uint8_t TempIndex;
    BP_RTC_INFO *BpRtcInfo;
    RTC_DATE_TIME CurSysTime,BpAlarmTime;

    BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
    RtcGetCurrTime(&CurSysTime);
    
    /* 查询记忆中的时间，是否有超时的闹钟，如有则将其删除 */
    for (TempIndex = 0; TempIndex < MAX_ALARM_NUM; ++TempIndex)
    {
        /* 本地闹钟存放在WiFi闹钟的后面 */
        BpAlarmTime = BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime;
        /* 跳过星期判断 */
        BpAlarmTime.WDay = CurSysTime.WDay;
        if(memcmp((void *)&BpAlarmTime, (void *)&CurSysTime, sizeof(RTC_DATE_TIME)) <= 0)
        {
            if(ALARM_MODE_ONCE_ONLY == BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmMode)
            {
                memset((void *)&BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex], 0, sizeof(ALARM_BP_INFO));
                BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmMode = ALARM_STATUS_CLOSED;
            }
            else
            {
                BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Date++;
        		if(BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Date > RtcGetMonthDays(CurSysTime.Year, CurSysTime.Mon))
        		{
        			BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Date = 1;
        			BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Mon++;
        			if(BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Mon > 12)
        			{
        				BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Mon -= 12;
        				BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+TempIndex].AlarmTime.Year++;
        			}
        		}                
            }
        }
    }
    
    BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+AlarmNum] = *AlarmInfo; 

    UpdataAlarmTimeIndex();
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RtcTimerCB(uint32_t unused)
{
	RtcGetCurrTime(&sRtcControl->DataTime);

	if((RTC_STATE_IDLE == sRtcControl->State) || (sRtcControl->SubState == RTC_SET_IDLE))
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

		APP_DBG("铃声音量:%d;", sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum]);
		APP_DBG("闹钟状态:%s;",     sRtcControl->AlarmState == ALARM_STATUS_OPENED?"ON":"OFF");
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
	InitTimer((SW_TIMER*)&sRtcControl->TimerHandle, 30000, (TIMER_CALLBACK)RtcTimerCB);
	StartTimer(&sRtcControl->TimerHandle);
#ifdef FUNC_RTC_ALARM	
#ifdef FUNC_RTC_ALARM_SAVE2FLASH
	{
		ALARM_BP_INFO BpAlarmInfo;

		for(sRtcControl->AlarmNum = 1; sRtcControl->AlarmNum <= MAX_ALARM_NUM; sRtcControl->AlarmNum++)
		{
            if(GetNearAlarmTimeFromBpInfo(&BpAlarmInfo, sRtcControl->AlarmNum-1))      
            {
			    sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = BpAlarmInfo.RingType;
    			sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = BpAlarmInfo.AlarmVolume;
    			sRtcControl->AlarmMode = BpAlarmInfo.AlarmMode; 
    			sRtcControl->AlarmTime = BpAlarmInfo.AlarmTime;
    			sRtcControl->AlarmState = BpAlarmInfo.AlarmState;
                RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
                RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
			}
            else
            {
                RtcAlarmSetStatus(sRtcControl->AlarmNum, ALARM_STATUS_CLOSED);
            }
		}
        //保存到记忆区
        AudioSysInfoSetBreakPoint();
	}
#endif
	NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
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
				if((sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] > MAX_VOLUME)
				|| (sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] == FALSE))
				{
					sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = DEFAULT_VOLUME;
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
#if (defined(FUNC_RTC_EN) && defined(FUNC_RTC_ALARM_SAVE2FLASH))
            BP_RTC_INFO *BpRtcInfo;
            ALARM_BP_INFO *AlarmInfo;

            BpRtcInfo = (BP_RTC_INFO *)BP_GetInfo(BP_RTC_INFO_TYPE);
            AlarmInfo = &BpRtcInfo->AlarmBpInfo[MAX_BP_INFO_ALARM_NUM+sRtcControl->AlarmNum-1];
            /* 设置闹钟常量 */
    		sRtcControl->AlarmMode = AlarmInfo->AlarmMode;
    		sRtcControl->AlarmState = AlarmInfo->AlarmState;
    		sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = AlarmInfo->AlarmVolume;
    		sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = AlarmInfo->RingType;
    		sRtcControl->AlarmTime = AlarmInfo->AlarmTime;
            /* 判断时间数据是否合法 */
            if(sRtcControl->AlarmTime.Hour > 24)
            {
                sRtcControl->AlarmTime.Hour = sRtcControl->DataTime.Hour;
            }
            if(sRtcControl->AlarmTime.Min > 59)
            {
                sRtcControl->AlarmTime.Min = sRtcControl->DataTime.Min;
            }
            if(sRtcControl->AlarmTime.Sec > 59)
            {
                sRtcControl->AlarmTime.Sec = sRtcControl->DataTime.Sec;
            }
            sRtcControl->AlarmTime.Year = sRtcControl->DataTime.Year;
            sRtcControl->AlarmTime.Mon = sRtcControl->DataTime.Mon;
            sRtcControl->AlarmTime.Date = sRtcControl->DataTime.Date;

            if((sRtcControl->AlarmMode < ALARM_MODE_ONCE_ONLY) || (sRtcControl->AlarmMode >= ALARM_MODE_USER_DEFINED))
			{
				sRtcControl->AlarmMode = ALARM_MODE_ONCE_ONLY;
			}
            /* 判断闹钟音量是否合法 */
			if((sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] > MAX_VOLUME)
			|| (sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] == FALSE))
			{
				sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = DEFAULT_VOLUME;
			}
            /* 判断闹钟铃声是否合法 */
			if((sNvmRtcInfo.RingType[sRtcControl->AlarmNum] > RTC_RING_TYPE_MAX)
			|| (sNvmRtcInfo.RingType[sRtcControl->AlarmNum] < RTC_RING_SOUND1_TYPE))
			{
				sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = RTC_RING_SOUND1_TYPE;
			}
#else
            sRtcControl->AlarmTime = sRtcControl->DataTime;
#endif
			APP_DBG("Set Alarm Time Mode\n");
			sRtcControl->SubState = RTC_SET_HOUR;
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
#if (defined(FUNC_RTC_EN) && defined(FUNC_RTC_ALARM_SAVE2FLASH))
        ALARM_BP_INFO AlarmInfo;
        
        /* 设置闹钟常量 */
		AlarmInfo.AlarmMode = sRtcControl->AlarmMode;
		AlarmInfo.AlarmState = sRtcControl->AlarmState;
		AlarmInfo.AlarmVolume = sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum];
		AlarmInfo.RingType = sNvmRtcInfo.RingType[sRtcControl->AlarmNum];
		AlarmInfo.AlarmTime = sRtcControl->AlarmTime;

		/* 闹钟数据存放到MCU闹钟记忆区 */
		SyncMcuAlarmTimeToBpInfo(&AlarmInfo, sRtcControl->AlarmNum-1);

		/* 更新到本地闹钟 */
		for(sRtcControl->AlarmNum = 1; sRtcControl->AlarmNum <= MAX_ALARM_NUM; sRtcControl->AlarmNum++)
		{
		    if(GetNearAlarmTimeFromBpInfo(&AlarmInfo, sRtcControl->AlarmNum-1) == TRUE)
		    {
    			sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = AlarmInfo.RingType;
    			sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = AlarmInfo.AlarmVolume;
    			sRtcControl->AlarmMode = AlarmInfo.AlarmMode; 
    			sRtcControl->AlarmTime = AlarmInfo.AlarmTime;
    			sRtcControl->AlarmState = AlarmInfo.AlarmState;
    			RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
    			RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
			}
			else
			{
			    RtcAlarmSetStatus(sRtcControl->AlarmNum, ALARM_STATUS_CLOSED);
			}
		}
        //保存到记忆区
        AudioSysInfoSetBreakPoint();
#endif
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
			/* 判断闹钟音量是否合法 */
			if((sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] > MAX_VOLUME)
			|| (sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] == FALSE))
			{
				sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = DEFAULT_VOLUME;
			}
            /* 判断闹钟铃声是否合法 */
			if((sNvmRtcInfo.RingType[sRtcControl->AlarmNum] > RTC_RING_WIFISD_TYPE)
			|| (sNvmRtcInfo.RingType[sRtcControl->AlarmNum] < RTC_RING_SOUND1_TYPE))
			{
				sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = RTC_RING_SOUND1_TYPE;
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
			if(ALARM_STATUS_OPENED == sRtcControl->AlarmState)
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
			sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum]++;
			if(sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] >= MAX_VOLUME)
			{
			  sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = 0;
			}
		}
		else
		{
			RtcTimeUp(&sRtcControl->AlarmTime);
		}
#ifndef FUNC_RTC_ALARM_SAVE2FLASH
		//设置闹钟及闹钟状态，需要先设置闹钟，最后设置闹钟状态；否则设置时钟时会强制打开闹钟状态
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
#endif		
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
	DispSendMessage(DEV_SYMBOL_INTERVAL, DISP_MSG_RTC_SET);
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
			if(ALARM_STATUS_OPENED == sRtcControl->AlarmState)
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
			sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum]--;
			if(sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] > MAX_VOLUME)
			{
			  sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = MAX_VOLUME;
			}
		}
		else
		{
			RtcTimeDown(&sRtcControl->AlarmTime);
		}
#ifndef FUNC_RTC_ALARM_SAVE2FLASH
		//设置闹钟及闹钟状态，需要先设置闹钟，最后设置闹钟状态；否则设置时钟时会强制打开闹钟状态
		RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
		RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
#endif		
#ifdef FUNC_RTC_ALARM		
		NvmWrite(RTC_NVM_START_ADDR, (uint8_t *)&sNvmRtcInfo, sizeof(NVM_RTC_INFO));
#endif
	}
#endif
	TimeOutSet(&RtcAutoOutTimer, RTC_AUTO_OUT_TIME);
	DispSendMessage(DEV_SYMBOL_INTERVAL, DISP_MSG_RTC_SET);
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
	static uint8_t AlarmRingCnt = 0,CurVolumeBackUp = 0;

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
				gSys.Volume = sNvmRtcInfo.AlarmVolume[sRtcControl->CurAlarmNum];
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
				gSys.NextModuleID = MODULE_ID_STANDBY;
				Msg = MSG_COMMON_CLOSE;
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
#ifdef FUNC_RTC_ALARM_SAVE2FLASH
    		    ALARM_BP_INFO BpAlarmInfo, AlarmTime;

				RtcGetAlarmTime(sRtcControl->CurAlarmNum, &AlarmTime.AlarmMode, &sRtcControl->AlarmData, (RTC_DATE_TIME *)&AlarmTime.AlarmTime);
                /* 等待系统时间超过闹钟时间，防止有时刷新闹钟时判断闹钟超时出现错误 */
                while(memcmp((void *)&sRtcControl->DataTime, (void *)&AlarmTime.AlarmTime, sizeof(RTC_DATE_TIME)) > 0)
                {
                    //刷新当前实时时钟
                    WaitMs(100);
				    RtcGetCurrTime(&sRtcControl->DataTime);
                    AlarmTime.AlarmTime.WDay = sRtcControl->DataTime.WDay;
                }
                
                for(sRtcControl->AlarmNum = 1; sRtcControl->AlarmNum <= MAX_ALARM_NUM; sRtcControl->AlarmNum++)
        		{
        		    if(GetNearAlarmTimeFromBpInfo(&BpAlarmInfo, sRtcControl->AlarmNum-1) == TRUE)
        		    {
            			sNvmRtcInfo.RingType[sRtcControl->AlarmNum] = BpAlarmInfo.RingType;
            			sNvmRtcInfo.AlarmVolume[sRtcControl->AlarmNum] = BpAlarmInfo.AlarmVolume;
                        sRtcControl->AlarmMode = BpAlarmInfo.AlarmMode;
                        sRtcControl->AlarmState = BpAlarmInfo.AlarmState;
                        sRtcControl->AlarmTime = BpAlarmInfo.AlarmTime;
            			RtcSetAlarmTime(sRtcControl->AlarmNum, sRtcControl->AlarmMode, sRtcControl->AlarmData, &sRtcControl->AlarmTime);
            			RtcAlarmSetStatus(sRtcControl->AlarmNum, sRtcControl->AlarmState);
        			}
        			else
        			{
        			    RtcAlarmSetStatus(sRtcControl->AlarmNum, ALARM_STATUS_CLOSED);
        			}
        		}
                sRtcControl->AlarmMode = AlarmTime.AlarmMode;
                sRtcControl->AlarmTime = AlarmTime.AlarmTime;
                //保存到记忆区
                AudioSysInfoSetBreakPoint();
#else
				RtcGetAlarmTime(sRtcControl->CurAlarmNum, &sRtcControl->AlarmMode, &sRtcControl->AlarmData, &sRtcControl->AlarmTime);
                if(sRtcControl->AlarmMode == ALARM_MODE_ONCE_ONLY)
				{
					//单次闹钟，闹响后将闹钟关闭
					RtcAlarmSetStatus(sRtcControl->CurAlarmNum, ALARM_STATUS_CLOSED);
				}
                //刷新当前实时时钟
				RtcGetCurrTime(&sRtcControl->DataTime);
#endif

				sRtcControl->AlarmState = RtcGetAlarmStatus(sRtcControl->AlarmNum);
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
				else if((RTC_RING_WIFISD_TYPE == sNvmRtcInfo.RingType[sRtcControl->CurAlarmNum])
				&& (WIFI_STATUS_WWW_ENABLE == WiFiWwwStateGet()))
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
    /* 新闹钟时间到，先停掉正在闹响的闹钟 */
    RtcCurAlarmSleepAndStop(RTC_ALARM_STATE_STOP);
	sRtcControl->CurAlarmNum = RtcCheckAlarmFlag();
	sRtcControl->CurAlarmNum = 0;
	RtcAlarmIntClear();// 清除闹钟提醒中断
}
#endif
#endif


