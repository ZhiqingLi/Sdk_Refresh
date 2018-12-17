/**
 *****************************************************************************
 * @file     rtc_interface.h
 * @author   Sean
 * @version  V1.0.0
 * @date     29-03-2017
 * @brief    rtc module interface header file
 * @maintainer: Sean
 * change log:
 *			 Modify by Sean -20170329
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

/**
 * @addtogroup RTC_INTERFACE
 * @{
 * @defgroup rtc_interface rtc_interface.h
 * @{
 */

#ifndef __RTC_INTERFACE_H__
#define __RTC_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * 闹钟状态定义
 */
#define ALARM_STATUS_OPENED      0x01  /**< 打开状态   */
#define ALARM_STATUS_CLOSED      0x02  /**< 关闭状态   */
/**
 * 闹钟模式定义
 */
typedef enum _RTC_ALARM_MODE
{
	ALARM_MODE_ONCE_ONLY = 1,	/**< 单次闹钟 */
	ALARM_MODE_PER_DAY,			/**< 每天闹钟 */
	ALARM_MODE_PER_WEEK,		/**< 每周闹钟 */
	ALARM_MODE_WORKDAY,			/**< 工作日闹钟 */
	ALARM_MODE_USER_DEFINED,	/**< 自定义闹钟 */
	ALARM_MODE_MAX,	
} RTC_ALARM_MODE;


#pragma pack(1)	/*Ensure this structure is byte aligned, and not use padding bytes */
typedef struct _ALARM_TIME
{
	uint32_t AlarmTimeAsSeconds;
	uint8_t  AlarmStatus : 2;
	uint8_t  AlarmMode : 4;
	uint8_t  AlarmData; //闹钟周期，bit0：星期天，bit6：星期六
} ALARM_TIME;
#pragma pack()


/**
 * 阳历时间结构体定义
 */
typedef struct _RTC_DATE_TIME
{
	uint16_t	Year;  /**< 阳历年 */
	uint8_t	Mon;   /**< 阳历月 */
	uint8_t	Date;  /**< 阳历日 */
	uint8_t	WDay;  /**< 星期号，0代表周日，1~6代表周1到周6 */
	uint8_t	Hour;  /**< 小时数 */
	uint8_t	Min;   /**< 分钟数 */
	uint8_t	Sec;   /**< 秒数 */
} RTC_DATE_TIME;


/**
 * 农历时间结构体定义
 */
typedef struct _RTC_LUNAR_DATE
{
	uint16_t Year;         /**< 农历年 */
	uint8_t Month;        /**< 农历月 */
	uint8_t Date;         /**< 农历日 */
	uint8_t MonthDays;	   /**< 农历每月的天数，30 或者 29*/
	bool IsLeapMonth;  /**< 是否为闰月*/
} RTC_LUNAR_DATE;


typedef enum _RTC_STATE_
{
	RTC_STATE_IDLE = 0,
	RTC_STATE_SET_TIME,
	RTC_STATE_SET_ALARM,

} RTC_STATE;

typedef enum _RTC_SUB_STATE_
{
    RTC_SET_IDLE = 0,	// 空闲中
	RTC_SET_YEAR,    	// 设置年份
    RTC_SET_MON,     	// 设置月份
    RTC_SET_DAY,     	// 设置日期
	RTC_SET_WEEK,		// 设置周
    RTC_SET_HOUR,    	// 设置小时
    RTC_SET_MIN,     	// 设置分钟
    RTC_SET_SEC,     	// 设置秒钟
	RTC_SET_ALARM_NUM,
	RTC_SET_ALARM_MODE,

} RTC_SUB_STATE;


typedef enum _RTC_WEEK_
{
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,

} RTC_WEEK;

typedef struct _RTC_CONTROL_
{
	uint8_t  		State;          // RTC当前状态,IDLE, Set Time, Set Alarm
	uint8_t  		SubState;       // RTC当前子状态,set year/mon/...

	RTC_DATE_TIME 	DataTime;  		// 时间日期
	uint8_t	 		AlarmMode;		// 闹钟模式
	uint8_t  		AlarmData;		// 自定义闹钟模式下，闹钟时间选择，bit0：星期天，bit1：星期一，...,bit6：星期六,置位表示选择
	uint8_t  		CurAlarmNum;	// 当前响应的闹钟编号

	RTC_LUNAR_DATE	LuarDate; 		// 农历
} RTC_CONTROL;

typedef struct _ALARM_INFO_
{
	ALARM_TIME* AlarmTimeList;// 闹钟时间信息
}ALARM_INFO;


/**
 * @brief  RTC模块初始化
 * @param  时间结构体指针，用于返回当前时间
 * @return 无
 */
void RTC_Init(RTC_CONTROL* ptr1,ALARM_INFO* ptr2,uint8_t alarmSum);

/**
 * @brief  获取当前时间
 * @param  时间结构体指针，用于返回当前时间
 * @return 无
 */
void RTC_CurrTimeGet(RTC_DATE_TIME* CurrTime);

/**
 * @brief  设置当前时间
 * @param  时间结构体指针，为要设置的时间值
 * @return 无
 */
void RTC_CurrTimeSet(RTC_DATE_TIME* CurrTime);

/**
 * @brief  使能某个闹钟
 * @param  闹钟索引
 * @return 无
 */
void RTC_AlarmEnable(uint8_t AlarmID);

/**
 * @brief  禁止某个闹钟
 * @param  闹钟索引
 * @return 无
 */
void RTC_AlarmDisable(uint8_t AlarmID);

/**
 * @brief  获取某个闹钟状态
 * @param  闹钟索引
 * @return TRUE~使能 FALSE~禁止
 */
bool RTC_AlarmEnDisGet(uint8_t AlarmID);

/**
 * @brief  获取某个闹钟的闹钟模式和闹钟时间
 * @param  AlarmID		闹钟号
 * @param  AlarmMode	闹钟模式指针，用于保存获取的闹钟模式
 * @param  ModeData		周闹钟模式下闹钟有效的时间（周几，可以多个同时有效）
 * @param  AlarmTime	闹钟时间结构体指针，用于保存获取的闹钟时间
 * @return 如果执行成功返回TRUE，否则返回FALSE。
 */
void RTC_AlarmTimeGet(uint8_t AlarmID, uint8_t* AlarmMode, uint8_t* AlarmData, RTC_DATE_TIME* AlarmTime);

/**
 * @brief  设置某个闹钟的闹钟模式和闹钟时间
 * @param  AlarmID		闹钟号
 * @param  AlarmMode	闹钟模式
 * @param  ModeData		周闹钟模式下闹钟有效的时间（周几，可以多个同时有效）
 * @param  AlarmTime	闹钟时间结构体指针，用于保存闹钟时间
 * @return 如果执行成功返回TRUE，否则返回FALSE。
 */
void RTC_AlarmTimeSet(uint8_t AlarmID, uint8_t AlarmMode, uint8_t AlarmData, RTC_DATE_TIME* AlarmTime);

/**
 * @brief  检查是否有闹钟到了。
 * @param  无
 * @return 返回0表示没有闹钟到。返回大于0的值表示对应的闹钟到了
 */
uint8_t RTC_AlarmFlagGet(void);

/**
 * @brief  清除闹钟中断标志
 * @param  无
 * @return 无
 */
void RTC_AlarmIntClear(void);

/**
 * @brief  闹钟到时需要做的处理，上层接收到闹钟到的消息后，必须调用此函数。
 * @param  无
 * @return 无
 */
void RTC_AlarmArrivedProcess(void);


/**
 * @brief  将某年的农历信息提取到变量中
 * @param  Year:阳历年份
 * @return NONE
 */
void RTC_LunarLoadLunarInfo(uint16_t Year);

/**
 * @brief  阳历日期转农历日期
 * @param  DateTime:阳历时间结构体指针，作为输入参数
 * @param  LunarDate:农历时间结构体指针，作为输出参数
 * @return NONE
 */
void RTC_LunarSolarToLunar(RTC_DATE_TIME* DateTime, RTC_LUNAR_DATE* LunarDate);

/**
 * @brief  获取农历年份的天干
 * @param  Year:农历年份
 * @return 返回0-9, 分别对应天干: {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"}
 */
uint8_t RTC_LunarGetHeavenlyStem(uint16_t Year);

/**
 * @brief  获取农历年份的地支
 * @param  Year:农历年份
 * @return 返回0-11, 分别对应地支: {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"}
 *         分别对应生肖: {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"}
 */
uint8_t RTC_LunarGetEarthlyBranch(uint16_t Year);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
