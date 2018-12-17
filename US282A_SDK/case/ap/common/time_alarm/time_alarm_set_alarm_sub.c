/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟设置功能，选项切换，数值加，数值减接口实现。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_time_alarm.h"

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

app_result_e set_alarm_switch_deal(set_alarm_var_t *set_alarm_var)
{
    app_result_e result = RESULT_NULL;
    alarm_alarm_t *p_alarm_alarm = &(set_alarm_var->cur_alarm_record.alarm);

    switch (set_alarm_var->alarm_state)
    {
        /*! 设置开/关 */
        case alarm_onoff:
        if (p_alarm_alarm->alarm_onoff == FALSE)
        { //关闭闹钟，直接返回Clock显示
            set_alarm_var->alarm_state = alarm_finish;
            result = RESULT_BACK_SCENE_DISPATCH;
        }
        else
        {
            set_alarm_var->alarm_state = alarm_hour;
            set_alarm_var->setting_num8 = &(set_alarm_var->time.hour);
            set_alarm_var->setting_digits = 2;
        }
        break;

        /*! 设置小时 */
        case alarm_hour:
        if (is_hour_valid(set_alarm_var->time.hour) == FALSE)
        {
            result = RESULT_BACK_SCENE_DISPATCH;
        }
        else
        {
            set_alarm_var->alarm_state = alarm_minute;
            set_alarm_var->setting_num8 = &(set_alarm_var->time.minute);
            set_alarm_var->setting_digits = 2;
        }
        break;

        /*! 设置分钟 */
        case alarm_minute:
        if (is_minute_valid(set_alarm_var->time.minute) == FALSE)
        {
            result = RESULT_BACK_SCENE_DISPATCH;
            break;
        }
        else
        {
            set_alarm_var->alarm_state = alarm_finish;
        }
        /*已经finish，退出*/
        case alarm_finish:
        result = RESULT_BACK_SCENE_DISPATCH;
        break;

        default:
        break;
    }

    return result;
}

app_result_e set_alarm_sub_deal(set_alarm_var_t *set_alarm_var)
{
    app_result_e result = RESULT_NULL;
    alarm_alarm_t *p_alarm_alarm = &(set_alarm_var->cur_alarm_record.alarm);

    switch (set_alarm_var->alarm_state)
    {
        /*! 设置开/关 */
        case alarm_onoff:
        if (p_alarm_alarm->alarm_onoff == TRUE)
        {
            p_alarm_alarm->alarm_onoff = FALSE;
        }
        else
        {
            p_alarm_alarm->alarm_onoff = TRUE;
        }
        break;

        /*! 设置小时 */
        case alarm_hour:
        if (set_alarm_var->time.hour > 0)
        {
            set_alarm_var->time.hour--;
        }
        else
        {
            set_alarm_var->time.hour = 23;
        }
        break;

        /*! 设置分钟 */
        case alarm_minute:
        if (set_alarm_var->time.minute > 0)
        {
            set_alarm_var->time.minute--;
        }
        else
        {
            set_alarm_var->time.minute = 59;
        }
        break;

        default:
        break;
    }

    return result;
}

app_result_e set_alarm_add_deal(set_alarm_var_t *set_alarm_var)
{
    app_result_e result = RESULT_NULL;
    alarm_alarm_t *p_alarm_alarm = &(set_alarm_var->cur_alarm_record.alarm);

    switch (set_alarm_var->alarm_state)
    {
        /*! 设置开/关 */
        case alarm_onoff:
        if (p_alarm_alarm->alarm_onoff == TRUE)
        {
            p_alarm_alarm->alarm_onoff = FALSE;
        }
        else
        {
            p_alarm_alarm->alarm_onoff = TRUE;
        }
        break;

        /*! 设置小时 */
        case alarm_hour:
        if (set_alarm_var->time.hour < 23)
        {
            set_alarm_var->time.hour++;
        }
        else
        {
            set_alarm_var->time.hour = 0;
        }
        break;

        /*! 设置分钟 */
        case alarm_minute:
        if (set_alarm_var->time.minute < 59)
        {
            set_alarm_var->time.minute++;
        }
        else
        {
            set_alarm_var->time.minute = 0;
        }
        break;

        default:
        break;
    }

    return result;
}

