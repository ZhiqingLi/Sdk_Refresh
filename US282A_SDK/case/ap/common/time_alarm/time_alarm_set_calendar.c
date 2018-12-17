/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：日历设置功能主模块，实现用户交互等。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_time_alarm.h"

const key_event_map_t calendar_ke_maplist[];
void tm_alarm_set_calendar_view(view_update_e mode);
void tm_alarm_create_set_calendar_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_SUB;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 8000;
    create_view_param.ke_maplist = calendar_ke_maplist;
    create_view_param.se_maplist = NULL;
    create_view_param.view_cb = tm_alarm_set_calendar_view;
    com_view_add(COM_VIEW_ID_SET_CALENDAR, &create_view_param);
}

app_result_e calendar_key_deal_switch_clock(void);
app_result_e calendar_key_deal_switch_menu(void);
app_result_e calendar_key_deal_add(void);
app_result_e calendar_key_deal_sub(void);
app_result_e calendar_key_deal_number_input(void);

void tm_alarm_set_calendar_view(view_update_e mode);
void calendar_handle(void);

const key_event_map_t __section__(".rodata.ke_maplist") calendar_ke_maplist[] =
{
    /*! 返回时钟功能 */
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, 0 }, calendar_key_deal_switch_clock },

    /*! 切换设置菜单 */
    { { KEY_ABREC, 0, KEY_TYPE_SHORT_UP, 0 }, calendar_key_deal_switch_menu },
    { { KEY_CLK, 0, KEY_TYPE_SHORT_UP, 0 }, calendar_key_deal_switch_menu },

    /*! 调节加 */
    { { KEY_NEXT, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, 0 }, calendar_key_deal_add },
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, 0 }, calendar_key_deal_add },

    /*! 调节减 */
    { { KEY_PREV, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, 0 }, calendar_key_deal_sub },
    { { KEY_PREV_VOLSUB, 0, KEY_TYPE_DOWN | KEY_TYPE_LONG | KEY_TYPE_HOLD, 0 }, calendar_key_deal_sub },

    /*! 数字按键调节 */
    { { KEY_NUMBER, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, 0 }, calendar_key_deal_number_input },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, 0 }, NULL },
};

app_result_e calendar_key_deal_switch_clock(void)
{
    //tm_alarm_create_clock_view();
    com_view_remove_top_view();

    return RESULT_NULL;
}

app_result_e calendar_key_deal_switch_menu(void)
{
    app_result_e result;

    result = set_calendar_switch_deal(g_p_set_calendar);
    if (result == RESULT_BACK_SCENE_DISPATCH)
    {
        com_view_remove_top_view();
        result = RESULT_NULL;
    }
    else
    {
        tm_alarm_set_calendar_view(VIEW_UPDATE_REDRAW);
        restart_app_timer(g_p_set_calendar->twinkling_timer_id);
        g_p_set_calendar->twinkling_flag = TRUE;
        g_p_set_calendar->digit_flag = FALSE;
    }

    return result;
}

app_result_e calendar_key_deal_add(void)
{
    set_calendar_add_deal(g_p_set_calendar);

    tm_alarm_set_calendar_view(VIEW_UPDATE_REDRAW);
    restart_app_timer(g_p_set_calendar->twinkling_timer_id);
    g_p_set_calendar->twinkling_flag = TRUE;

    g_p_set_calendar->digit_flag = FALSE;
    return RESULT_NULL;
}

app_result_e calendar_key_deal_sub(void)
{
    set_calendar_sub_deal(g_p_set_calendar);

    tm_alarm_set_calendar_view(VIEW_UPDATE_REDRAW);
    restart_app_timer(g_p_set_calendar->twinkling_timer_id);
    g_p_set_calendar->twinkling_flag = TRUE;

    g_p_set_calendar->digit_flag = FALSE;
    return RESULT_NULL;
}

app_result_e calendar_key_deal_number_input(void)
{
    uint8 number = (uint8) (g_p_view_ke->val - KEY_NUM0);

    if (g_p_set_calendar->digit_flag == FALSE)
    {
        g_p_set_calendar->now_digits = 0;
        switch (g_p_set_calendar->setting_digits)
        {
            case 2:
            *(g_p_set_calendar->setting_num8) = 0;
            break;

            case 4:
            *(g_p_set_calendar->setting_num16) = 0;
            break;

            default:
            break;
        }

        g_p_set_calendar->digit_flag = TRUE;
    }

    switch (g_p_set_calendar->setting_digits)
    {
        case 2:
        if (g_p_set_calendar->now_digits >= 2)
        {
            *(g_p_set_calendar->setting_num8) = number;
            g_p_set_calendar->now_digits = 1;
        }
        else
        {
            *(g_p_set_calendar->setting_num8) = (*(g_p_set_calendar->setting_num8) * 10) + number;
            g_p_set_calendar->now_digits++;
        }
        break;

        case 4:
        if (g_p_set_calendar->now_digits >= 4)
        {
            *(g_p_set_calendar->setting_num16) = number;
            g_p_set_calendar->now_digits = 1;
        }
        else
        {
            *(g_p_set_calendar->setting_num16) = (*(g_p_set_calendar->setting_num16) * 10) + number;
            g_p_set_calendar->now_digits++;
        }
        break;

        default:
        break;
    }

    tm_alarm_set_calendar_view(VIEW_UPDATE_REDRAW);
    restart_app_timer(g_p_set_calendar->twinkling_timer_id);
    g_p_set_calendar->twinkling_flag = TRUE;

    return RESULT_NULL;
}

static void __tm_alarm_set_calendar_redraw(void)
{
    uint16 year, month, day;
    uint16 hour, minute;
    uint16 half1st = 0, half2nd = 0;
    bool half1st_twinkling = FALSE, half2nd_twinkling = FALSE;

    year = g_p_set_calendar->date.year;
    month = g_p_set_calendar->date.month;
    day = g_p_set_calendar->date.day;
    hour = g_p_set_calendar->time.hour;
    minute = g_p_set_calendar->time.minute;

    switch (g_p_set_calendar->calendar_state)
    {
        /*! 设置年份 */
        case calendar_year:
        half1st = year / 100;
        half2nd = year % 100;
        half1st_twinkling = g_p_set_calendar->twinkling_flag;
        half2nd_twinkling = g_p_set_calendar->twinkling_flag;
        break;

        /*! 设置月份 */
        case calendar_month:
        half1st = month;
        half2nd = day;
        half1st_twinkling = g_p_set_calendar->twinkling_flag;
        half2nd_twinkling = TRUE;
        break;

        /*! 设置日期 */
        case calendar_day:
        half1st = month;
        half2nd = day;
        half1st_twinkling = TRUE;
        half2nd_twinkling = g_p_set_calendar->twinkling_flag;
        break;

        /*! 设置小时 */
        case calendar_hour:
        half1st = hour;
        half2nd = minute;
        half1st_twinkling = g_p_set_calendar->twinkling_flag;
        half2nd_twinkling = TRUE;
        break;

        /*! 设置分钟 */
        case calendar_minute:
        half1st = hour;
        half2nd = minute;
        half1st_twinkling = TRUE;
        half2nd_twinkling = g_p_set_calendar->twinkling_flag;
        break;

        default:
        break;
    }

    led_display(LCD_COL, 0xff, 0); //清除:号
    led_display(0, (half1st / 10) + NUM_0, half1st_twinkling);
    led_display(1, (half1st % 10) + NUM_0, half1st_twinkling);
    led_display(2, (half2nd / 10) + NUM_0, half2nd_twinkling);
    led_display(3, (half2nd % 10) + NUM_0, half2nd_twinkling);

    if (g_p_set_calendar->twinkling_flag == TRUE)
    {
        g_p_set_calendar->twinkling_flag = FALSE;
    }
    else
    {
        g_p_set_calendar->twinkling_flag = TRUE;
    }
}

void tm_alarm_set_calendar_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            g_p_set_calendar = sys_malloc(sizeof(set_calendar_var_t));
            if (g_p_set_calendar == NULL)
            {
                PRINT_ERR("set calendar view malloc fail!!");
                while (1)
                {
                    ; //nothing for QAC
                }
            }

            //读取日期和时间
            sys_get_date(&(g_p_set_calendar->date));
            if (g_p_set_calendar->date.year > 2099)
            {
                g_p_set_calendar->date.year = 2000;
                sys_set_date(&(g_p_set_calendar->date));
            }
            sys_get_time(&(g_p_set_calendar->time));

            g_p_set_calendar->calendar_state = calendar_year;
            g_p_set_calendar->setting_num16 = &(g_p_set_calendar->date.year);
            g_p_set_calendar->setting_digits = 4;
            g_p_set_calendar->now_digits = 0;
            g_p_set_calendar->digit_flag = FALSE;

            g_p_set_calendar->write_date_falg = FALSE;
            g_p_set_calendar->write_time_falg = FALSE;

            g_p_set_calendar->twinkling_flag = TRUE;
            g_p_set_calendar->twinkling_timer_id = set_app_timer(APP_TIMER_ATTRB_UI | (COM_VIEW_ID_SET_CALENDAR << 8),
                    500, calendar_handle);

            __tm_alarm_set_calendar_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
            sys_write_rtc(g_p_set_calendar);
            kill_app_timer(g_p_set_calendar->twinkling_timer_id);

            sys_free(g_p_set_calendar);
            g_p_set_calendar = NULL;
        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            __tm_alarm_set_calendar_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            __tm_alarm_set_calendar_redraw();
        }
        break;

        default:
        break;
    }
}

//日历设置2Hz定时处理
void calendar_handle(void)
{
    tm_alarm_set_calendar_view(VIEW_UPDATE_REDRAW);
}
