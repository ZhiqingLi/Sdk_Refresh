/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：日历设置功能，选项切换，数值加，数值减接口实现。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_time_alarm.h"

const uint8 day_table[12] =
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static bool is_year_valid(uint16 year)
{
    if ((year >= 2000) && (year <= 2099))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static bool is_month_valid(uint8 month)
{
    if ((month >= 1) && (month <= 12))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static uint8 get_days_max(date_t *this_date)
{
    uint8 days_max;
    bool is_leap;

    if (is_month_valid(this_date->month) == FALSE) //for QAM
    {
        return 0;
    }

    is_leap = (uint8) ((((this_date->year % 4) == 0) && ((this_date->year % 100) != 0))
            || ((this_date->year % 400) == 0));
    days_max = day_table[this_date->month - 1];
    if ((is_leap == TRUE) && (this_date->month == 2))
    {
        days_max = 29;
    }

    return days_max;
}

static bool is_day_valid(date_t *this_date)
{
    if ((this_date->day >= 1) && (this_date->day <= get_days_max(this_date)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static bool is_hour_valid(uint8 hour)
{
    if (hour <= 23)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static bool is_minute_valid(uint8 minute)
{
    if (minute <= 59)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//写入rtc寄存器
void sys_write_rtc(set_calendar_var_t *set_calendar_var)
{
    if (set_calendar_var->write_date_falg == TRUE)
    {
        sys_set_date(&(set_calendar_var->date));
    }
    if (set_calendar_var->write_time_falg == TRUE)
    {
        set_calendar_var->time.second = 0;
        sys_set_time(&(set_calendar_var->time));
    }
}

app_result_e set_calendar_switch_deal(set_calendar_var_t *set_calendar_var)
{
    app_result_e result = RESULT_NULL;

    switch (set_calendar_var->calendar_state)
    {
        /*! 设置年份 */
        case calendar_year:
        if (is_year_valid(set_calendar_var->date.year) == FALSE)
        {
            result = RESULT_BACK_SCENE_DISPATCH;
        }
        else
        {
            set_calendar_var->calendar_state = calendar_month;
            set_calendar_var->setting_num8 = &(set_calendar_var->date.month);
            set_calendar_var->setting_digits = 2;
        }
        break;

        /*! 设置月份 */
        case calendar_month:
        if (is_month_valid(set_calendar_var->date.month) == FALSE)
        {
            result = RESULT_BACK_SCENE_DISPATCH;
        }
        else
        {
            set_calendar_var->calendar_state = calendar_day;
            set_calendar_var->setting_num8 = &(set_calendar_var->date.day);
            set_calendar_var->setting_digits = 2;
        }
        break;

        /*! 设置日期 */
        case calendar_day:
        if (is_day_valid(&(set_calendar_var->date)) == FALSE)
        {
            result = RESULT_BACK_SCENE_DISPATCH;
        }
        else
        {
            set_calendar_var->calendar_state = calendar_hour;
            set_calendar_var->setting_num8 = &(set_calendar_var->time.hour);
            set_calendar_var->setting_digits = 2;
        }
        break;

        /*! 设置小时 */
        case calendar_hour:
        if (is_hour_valid(set_calendar_var->time.hour) == FALSE)
        {
            result = RESULT_BACK_SCENE_DISPATCH;
        }
        else
        {
            set_calendar_var->calendar_state = calendar_minute;
            set_calendar_var->setting_num8 = &(set_calendar_var->time.minute);
            set_calendar_var->setting_digits = 2;
        }
        break;

        /*! 设置分钟 */
        case calendar_minute:
        if (is_minute_valid(set_calendar_var->time.minute) == FALSE)
        {
            result = RESULT_BACK_SCENE_DISPATCH;
            break;
        }
        else
        {
            set_calendar_var->calendar_state = calendar_finish;
        }
        /*已经finish，退出*/
        case calendar_finish:
        result = RESULT_BACK_SCENE_DISPATCH;
        break;

        default:
        break;
    }

    return result;
}

app_result_e set_calendar_sub_deal(set_calendar_var_t *set_calendar_var)
{
    app_result_e result = RESULT_NULL;

    switch (set_calendar_var->calendar_state)
    {
        /*! 设置年份 */
        case calendar_year:
        if (set_calendar_var->date.year > 2000)
        {
            set_calendar_var->date.year--;
        }
        else
        {
            set_calendar_var->date.year = 2099;
        }
        set_calendar_var->write_date_falg = TRUE;
        break;

        /*! 设置月份 */
        case calendar_month:
        if (set_calendar_var->date.month > 1)
        {
            set_calendar_var->date.month--;
        }
        else
        {
            set_calendar_var->date.month = 12;
        }
        set_calendar_var->write_date_falg = TRUE;
        break;

        /*! 设置日期 */
        case calendar_day:
        if (set_calendar_var->date.day > 1)
        {
            set_calendar_var->date.day--;
        }
        else
        {
            set_calendar_var->date.day = get_days_max(&(set_calendar_var->date));
        }
        set_calendar_var->write_date_falg = TRUE;
        break;

        /*! 设置小时 */
        case calendar_hour:
        if (set_calendar_var->time.hour > 0)
        {
            set_calendar_var->time.hour--;
        }
        else
        {
            set_calendar_var->time.hour = 23;
        }
        set_calendar_var->write_time_falg = TRUE;
        break;

        /*! 设置分钟 */
        case calendar_minute:
        if (set_calendar_var->time.minute > 0)
        {
            set_calendar_var->time.minute--;
        }
        else
        {
            set_calendar_var->time.minute = 59;
        }
        set_calendar_var->write_time_falg = TRUE;
        break;

        default:
        break;
    }

    return result;
}

app_result_e set_calendar_add_deal(set_calendar_var_t *set_calendar_var)
{
    app_result_e result = RESULT_NULL;

    switch (set_calendar_var->calendar_state)
    {
        /*! 设置年份 */
        case calendar_year:
        if (set_calendar_var->date.year < 2099)
        {
            set_calendar_var->date.year++;
        }
        else
        {
            set_calendar_var->date.year = 2000;
        }
        set_calendar_var->write_date_falg = TRUE;
        break;

        /*! 设置月份 */
        case calendar_month:
        if (set_calendar_var->date.month < 12)
        {
            set_calendar_var->date.month++;
        }
        else
        {
            set_calendar_var->date.month = 1;
        }
        set_calendar_var->write_date_falg = TRUE;
        break;

        /*! 设置日期 */
        case calendar_day:
        if (set_calendar_var->date.day < get_days_max(&(set_calendar_var->date)))
        {
            set_calendar_var->date.day++;
        }
        else
        {
            set_calendar_var->date.day = 1;
        }
        set_calendar_var->write_date_falg = TRUE;
        break;

        /*! 设置小时 */
        case calendar_hour:
        if (set_calendar_var->time.hour < 23)
        {
            set_calendar_var->time.hour++;
        }
        else
        {
            set_calendar_var->time.hour = 0;
        }
        set_calendar_var->write_time_falg = TRUE;
        break;

        /*! 设置分钟 */
        case calendar_minute:
        if (set_calendar_var->time.minute < 59)
        {
            set_calendar_var->time.minute++;
        }
        else
        {
            set_calendar_var->time.minute = 0;
        }
        set_calendar_var->write_time_falg = TRUE;

        break;

        default:
        break;
    }

    return result;
}

