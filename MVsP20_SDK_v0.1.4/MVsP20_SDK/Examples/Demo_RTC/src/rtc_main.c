#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "rtc.h"
#include "rtc_interface.h"

#include "debug.h"
#include "irqn.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "clk.h"
#include "gpio.h"
#include "watchdog.h"
#include "spi_flash.h"
#include "cache_tcm.h"


#define FUNC_RTC_EN
#ifdef FUNC_RTC_EN
	#define FUNC_RTC_LUNAR
	#define FUNC_RTC_ALARM
	#ifdef FUNC_RTC_ALARM
		#define   MAX_ALARM_NUM			5
	#endif
#endif



#ifdef FUNC_RTC_EN

#ifdef FUNC_RTC_ALARM
ALARM_TIME AlarmTimeBuf[MAX_ALARM_NUM];
ALARM_INFO sNvmRtcInfo;
#endif

RTC_CONTROL sRtcTemp;
RTC_CONTROL* sRtcControl = &sRtcTemp;

#ifdef FUNC_RTC_LUNAR
static void DisplayLunarDate(void)
{
	//农历年份名称
	const uint8_t LunarYearName[12][3] = {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"};
	
	//农历月份名称
	const uint8_t LunarMonthName[12][3] = {"正", "二", "三", "四", "五", "六", "七", "八", "九", "十", "冬", "腊"};
	
	//农历日期名称
	const uint8_t LunarDateName[30][5] = {"初一", "初二", "初三", "初四", "初五", "初六", "初七", "初八", "初九", "初十",
									  "十一", "十二", "十三", "十四", "十五", "十六", "十七", "十八", "十九", "二十",
									  "廿一", "廿二", "廿三", "廿四", "廿五", "廿六", "廿七", "廿八", "廿九", "三十"};
	
	//农历天干名称
	const uint8_t HeavenlyStemName[10][3] = {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"};
	
	//农历地支名称
	const uint8_t EarthlyBranchName[12][3] = {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"};
	
	RTC_LUNAR_DATE LunarDate;

	RTC_LunarSolarToLunar(&sRtcControl->DataTime, &LunarDate);
	DBG("农历 %04d年 ",LunarDate.Year);
	DBG("%s", HeavenlyStemName[RTC_LunarGetHeavenlyStem(LunarDate.Year)]);
	DBG("%s年 ", EarthlyBranchName[RTC_LunarGetEarthlyBranch(LunarDate.Year)]);

	DBG("%s年 ", LunarYearName[RTC_LunarGetEarthlyBranch(LunarDate.Year)]);
	
	if(LunarDate.IsLeapMonth)
	{
		DBG("闰");
	}
	DBG("%s月", LunarMonthName[LunarDate.Month - 1]);

	if(LunarDate.MonthDays == 29)
	{
		DBG("(小)");
	}
	else
	{
		DBG("(大)");
	}

	DBG("%s ", LunarDateName[LunarDate.Date - 1]);

	if((LunarDate.Month == 1) && (LunarDate.Date == 1))			//春节
	{
		DBG("春节");
	}
	else if((LunarDate.Month == 1) && (LunarDate.Date == 15))	//元宵节
	{
		DBG("元宵节");
	}
	else if((LunarDate.Month == 5) && (LunarDate.Date == 5))	//端午节
	{
		DBG("端午节");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 7))	//七夕情人节
	{
		DBG("七夕情人节");
	}
	else if((LunarDate.Month == 7) && (LunarDate.Date == 15))	//中元节
	{
		DBG("中元节");
	}
	else if((LunarDate.Month == 8) && (LunarDate.Date == 15))	//中秋节
	{
		DBG("中秋节");
	}
	else if((LunarDate.Month == 9) && (LunarDate.Date == 9))	//重阳节
	{
   		DBG("重阳节");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 8))	//腊八节
	{
	 	DBG("腊八节");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == 23))	//小年
	{
		DBG("小年");
	}
	else if((LunarDate.Month == 12) && (LunarDate.Date == LunarDate.MonthDays))	//除夕
	{
		DBG("除夕");
	}
}
#endif

void RTC_SET_TEST(unsigned int year,unsigned char month,unsigned char day,unsigned char hour,unsigned char min,unsigned char sec)
{
	DBG("\n\nset rtc time:>>\n");
	sRtcControl->DataTime.Year = year;
	sRtcControl->DataTime.Mon = month;
	sRtcControl->DataTime.Date = day;
	sRtcControl->DataTime.Hour = hour;
	sRtcControl->DataTime.Min = min;
	sRtcControl->DataTime.Sec = sec;

	RTC_CurrTimeSet(&sRtcControl->DataTime);
}

#ifdef FUNC_RTC_ALARM
void RTC_ALARM_INFO_GET(void)
{
	uint8_t i = 0;
	uint8_t AlarmMode;
	uint8_t AlarmData;
	RTC_DATE_TIME AlarmTime;

	for(i = 0;i < MAX_ALARM_NUM;i++)
	{
		DBG("\nalarm_index:%d\n",i);

		RTC_AlarmTimeGet(i,&AlarmMode,&AlarmData,&AlarmTime);

		switch (AlarmMode)
		{
		case ALARM_MODE_ONCE_ONLY:
			DBG("once only\n");
			DBG("status:%d\n",RTC_AlarmEnDisGet(i));
			DBG("year:%d\n",AlarmTime.Year);
			DBG("mon:%d\n",AlarmTime.Mon);
			DBG("day:%d\n",AlarmTime.Date);
			DBG("hour:%d\n",AlarmTime.Hour);
			DBG("min:%d\n",AlarmTime.Min);
			DBG("sec:%d\n",AlarmTime.Sec);
			break;

		case ALARM_MODE_PER_DAY:
			DBG("every day\n");
			DBG("status:%d\n",RTC_AlarmEnDisGet(i));
			DBG("hour:%d\n",AlarmTime.Hour);
			DBG("min:%d\n",AlarmTime.Min);
			DBG("sec:%d\n",AlarmTime.Sec);
			break;

		case ALARM_MODE_PER_WEEK:
			DBG("per week\n");
			DBG("status:%d\n",RTC_AlarmEnDisGet(i));
			DBG("Wday:%d\n",AlarmTime.WDay);
			DBG("hour:%d\n",AlarmTime.Hour);
			DBG("min:%d\n",AlarmTime.Min);
			DBG("sec:%d\n",AlarmTime.Sec);
			break;

		case ALARM_MODE_WORKDAY:
			DBG("week day\n");
			DBG("status:%d\n",RTC_AlarmEnDisGet(i));
			//DBG("Wday:%d\n",AlarmTime.WDay);
			DBG("hour:%d\n",AlarmTime.Hour);
			DBG("min:%d\n",AlarmTime.Min);
			DBG("sec:%d\n",AlarmTime.Sec);
			break;

		case ALARM_MODE_USER_DEFINED:
			DBG("user defined\n");
			DBG("status:%d\n",RTC_AlarmEnDisGet(i));
			DBG("AlarmData:%d\n",AlarmData);
			//DBG("Wday:%d\n",AlarmTime.WDay);
			DBG("hour:%d\n",AlarmTime.Hour);
			DBG("min:%d\n",AlarmTime.Min);
			DBG("sec:%d\n",AlarmTime.Sec);
			break;

		default:
			break;
		}
	}
}
#endif


static bool alarmIntFlag = FALSE;
static RTC_DATE_TIME AlarmTime;

extern void rtc_test_case1(void);
extern void rtc_test_case2(void);
extern void rtc_test_case3(void);

int main(void)
{
	uint8_t i = 0,testCase = 1;
	uint32_t t1 = 0;

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

	GPIO_UartTxIoConfig(0, 1);//C2
	GPIO_UartRxIoConfig(0, 1);//C3
	UARTS_Init(0, 115200, 8, 0, 1);

	WDG_Disable();

	DBG("\n");
    DBG("/-----------------------------------------------------\\\n");
    DBG("|                       RTC Demo                      |\n");
    DBG("|      Mountain View Silicon Technology Co.,Ltd.      |\n");
    DBG("\\-----------------------------------------------------/\n");
    DBG("\n");

	RTC_ClockSrcSel(OSC_12M, 31250);
	RTC_IntDisable();
	RTC_IntFlagClr();
	RTC_IntEnable();

	//NVIC_DisableIRQ(RTC_IRQn);//polling mode

	NVIC_EnableIRQ(RTC_IRQn);//int mode
	GIE_ENABLE();

	memset(&sRtcTemp, 0, sizeof(RTC_CONTROL));
	sRtcControl = &sRtcTemp;
	sRtcControl->State = RTC_STATE_IDLE;
	sRtcControl->SubState = RTC_SET_IDLE;
	sRtcControl->DataTime.Year = 1980;
	sRtcControl->DataTime.Mon = 1;
	sRtcControl->DataTime.Date = 1;
	sRtcControl->DataTime.Hour = 0;
	sRtcControl->DataTime.Min = 0;
	sRtcControl->DataTime.Sec = 1;

#ifdef FUNC_RTC_ALARM
	memset(&AlarmTimeBuf, 0, sizeof(ALARM_TIME) * MAX_ALARM_NUM);
	for(i = 0;i < MAX_ALARM_NUM; i++)
	{
		AlarmTimeBuf[i].AlarmData = 0;
		AlarmTimeBuf[i].AlarmMode = ALARM_MODE_ONCE_ONLY;
		AlarmTimeBuf[i].AlarmTimeAsSeconds = 0;
		AlarmTimeBuf[i].AlarmStatus = ALARM_STATUS_OPENED;//ALARM_STATUS_CLOSED;
	}
	sNvmRtcInfo.AlarmTimeList = AlarmTimeBuf;
	RTC_Init(sRtcControl,&sNvmRtcInfo,MAX_ALARM_NUM);
#else
	RTC_Init(sRtcControl,NULL,0);
#endif

#ifdef FUNC_RTC_ALARM

	AlarmTime.Year = 1980;
	AlarmTime.Mon = 1;
	AlarmTime.Date = 1;
	AlarmTime.Hour = 0;
	AlarmTime.Min = 0;
	AlarmTime.Sec = 3;
	RTC_AlarmTimeSet(0, ALARM_MODE_ONCE_ONLY, 0, &AlarmTime);

	AlarmTime.Hour = 0;
	AlarmTime.Min = 0;
	AlarmTime.Sec = 6;
	RTC_AlarmTimeSet(1, ALARM_MODE_PER_DAY, 0, &AlarmTime);

	AlarmTime.WDay = 2;
	AlarmTime.Hour = 0;
	AlarmTime.Min = 0;
	AlarmTime.Sec = 9;
	RTC_AlarmTimeSet(2, ALARM_MODE_PER_WEEK, 0, &AlarmTime);

	AlarmTime.Hour = 0;
	AlarmTime.Min = 0;
	AlarmTime.Sec = 12;
	RTC_AlarmTimeSet(3, ALARM_MODE_WORKDAY, 0, &AlarmTime);

	AlarmTime.Hour = 0;
	AlarmTime.Min = 0;
	AlarmTime.Sec = 15;
	RTC_AlarmTimeSet(4, ALARM_MODE_USER_DEFINED, 0x03, &AlarmTime);
	RTC_ALARM_INFO_GET();
#endif

	while(1)
	{
		RTC_CurrTimeGet(&sRtcControl->DataTime);
		if(t1 != sRtcControl->DataTime.Sec)
		{

			t1 = sRtcControl->DataTime.Sec;
			DBG("RtcTime(%04d-%02d-%02d %02d:%02d:%02d) Week:%d ",
					sRtcControl->DataTime.Year, sRtcControl->DataTime.Mon, sRtcControl->DataTime.Date,
					sRtcControl->DataTime.Hour, sRtcControl->DataTime.Min, sRtcControl->DataTime.Sec,
					sRtcControl->DataTime.WDay);
//			DBG("%08ld %08ld ",RTC_TimeGet().Sec,RTC_AlarmGet().Sec);
#ifdef FUNC_RTC_LUNAR
			DisplayLunarDate();
#endif
			DBG("\n");

#ifdef FUNC_RTC_ALARM
//			if(RTC_IntFlagGet() == TRUE)//polling mode
//			{
//				DBG("alarm int:%d\n",RTC_AlarmFlagGet());
//				RTC_IntFlagClr();
//				RTC_AlarmArrivedProcess();
//			}

			if(alarmIntFlag == TRUE)//int mode
			{
				alarmIntFlag = FALSE;
				DBG("alarm_int_%d\n",RTC_AlarmFlagGet());
				RTC_AlarmArrivedProcess();
			}
#endif

#ifdef FUNC_RTC_ALARM
			if(sRtcControl->DataTime.Sec >= 18)
			{
				if(testCase < 8)
				{
					testCase++;
//					RTC_SET_TEST(1980,1,testCase,0,0,0);
				}
				else
				{
					testCase = 1;
//					RTC_SET_TEST(1980,1,testCase,0,0,0);
//
//					rtc_test_case1();
//					rtc_test_case2();
//					rtc_test_case3();
				}
			}
#endif
		}
	}

//	RtcDeinitialize();//模式切换，退出RTC
	return TRUE;
}

#ifdef FUNC_RTC_ALARM
__attribute__((section(".driver.isr")))void RtcInterrupt(void)//int mode
{
	if(RTC_IntFlagGet() == TRUE)
	{
		alarmIntFlag = TRUE;
		RTC_IntFlagClr();
	}
}
#endif

#if 0
static bool alarmEnableFlag = TRUE;
//测试代码
void rtc_test_case1(void)
{
	if(alarmEnableFlag == FALSE)
	{
		DBG("alarm en_____________________\n");
		alarmEnableFlag = TRUE;
		RTC_AlarmEnable(0);
		RTC_AlarmEnable(1);
		RTC_AlarmEnable(2);
		RTC_AlarmEnable(3);
		RTC_AlarmEnable(4);
		RTC_ALARM_INFO_GET();
	}
	else
	{
		DBG("alarm dis_____________________\n");
		alarmEnableFlag = FALSE;
		RTC_AlarmDisable(0);
		RTC_AlarmDisable(1);
		RTC_AlarmDisable(2);
		RTC_AlarmDisable(3);
		RTC_AlarmDisable(4);
		RTC_ALARM_INFO_GET();
	}
}

void rtc_test_case2(void)
{
	if(alarmEnableFlag == FALSE)
	{
		DBG("\n\n\nAlarm Change1:\n\n\n");
		alarmEnableFlag = TRUE;
		AlarmTime.Year = 1980;
		AlarmTime.Mon = 1;
		AlarmTime.Date = 1;
		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 3;
		RTC_AlarmTimeSet(0, ALARM_MODE_ONCE_ONLY, 0, &AlarmTime);

		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 6;
		RTC_AlarmTimeSet(1, ALARM_MODE_PER_DAY, 0, &AlarmTime);

		AlarmTime.WDay = 2;
		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 9;
		RTC_AlarmTimeSet(2, ALARM_MODE_PER_WEEK, 0, &AlarmTime);

		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 12;
		RTC_AlarmTimeSet(3, ALARM_MODE_WORKDAY, 0, &AlarmTime);

		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 15;
		RTC_AlarmTimeSet(4, ALARM_MODE_USER_DEFINED, 0x03, &AlarmTime);

		RTC_ALARM_INFO_GET();
	}
	else
	{
		DBG("\n\n\nAlarm Change2:\n\n\n");
		alarmEnableFlag = FALSE;

		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 2;
		RTC_AlarmTimeSet(0, ALARM_MODE_USER_DEFINED, 0x07, &AlarmTime);

		AlarmTime.Year = 1980;
		AlarmTime.Mon = 1;
		AlarmTime.Date = 1;
		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 14;
		RTC_AlarmTimeSet(1, ALARM_MODE_ONCE_ONLY, 0, &AlarmTime);

		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 7;
		RTC_AlarmTimeSet(2, ALARM_MODE_PER_DAY, 0, &AlarmTime);

		AlarmTime.WDay = 3;
		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 10;
		RTC_AlarmTimeSet(3, ALARM_MODE_PER_WEEK, 0, &AlarmTime);

		AlarmTime.Hour = 0;
		AlarmTime.Min = 0;
		AlarmTime.Sec = 5;
		RTC_AlarmTimeSet(4, ALARM_MODE_WORKDAY, 0, &AlarmTime);
		RTC_ALARM_INFO_GET();
	}
}

void rtc_test_case3(void)
{
	if(alarmEnableFlag == FALSE)
		{
			DBG("\n\n\nAlarm Change1:\n\n\n");
			alarmEnableFlag = TRUE;
			AlarmTime.Year = 1980;
			AlarmTime.Mon = 1;
			AlarmTime.Date = 1;
			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 3;
			RTC_AlarmTimeSet(0, ALARM_MODE_ONCE_ONLY, 0, &AlarmTime);

			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 6;
			RTC_AlarmTimeSet(1, ALARM_MODE_PER_DAY, 0, &AlarmTime);

			AlarmTime.WDay = 2;
			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 9;
			RTC_AlarmTimeSet(2, ALARM_MODE_PER_WEEK, 0, &AlarmTime);

			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 12;
			RTC_AlarmTimeSet(3, ALARM_MODE_WORKDAY, 0, &AlarmTime);

			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 15;
			RTC_AlarmTimeSet(4, ALARM_MODE_USER_DEFINED, 0x03, &AlarmTime);

			RTC_AlarmEnable(0);
			RTC_AlarmEnable(1);
			RTC_AlarmDisable(2);
			RTC_AlarmDisable(3);
			RTC_AlarmDisable(4);

			RTC_ALARM_INFO_GET();
		}
		else
		{
			DBG("\n\n\nAlarm Change2:\n\n\n");
			alarmEnableFlag = FALSE;

			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 2;
			RTC_AlarmTimeSet(0, ALARM_MODE_USER_DEFINED, 0x07, &AlarmTime);

			AlarmTime.Year = 1980;
			AlarmTime.Mon = 1;
			AlarmTime.Date = 1;
			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 14;
			RTC_AlarmTimeSet(1, ALARM_MODE_ONCE_ONLY, 0, &AlarmTime);

			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 7;
			RTC_AlarmTimeSet(2, ALARM_MODE_PER_DAY, 0, &AlarmTime);

			AlarmTime.WDay = 3;
			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 10;
			RTC_AlarmTimeSet(3, ALARM_MODE_PER_WEEK, 0, &AlarmTime);

			AlarmTime.Hour = 0;
			AlarmTime.Min = 0;
			AlarmTime.Sec = 5;
			RTC_AlarmTimeSet(4, ALARM_MODE_WORKDAY, 0, &AlarmTime);

			RTC_AlarmEnable(0);
			RTC_AlarmEnable(1);
			RTC_AlarmDisable(2);
			RTC_AlarmDisable(3);
			RTC_AlarmDisable(4);
			RTC_ALARM_INFO_GET();
		}
}
#endif
#endif
