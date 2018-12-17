/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：时钟显示功能主模块，实现用户交互等。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_time_alarm.h"

const key_event_map_t clock_ke_maplist[];
void tm_alarm_clock_view(view_update_e mode);
void tm_alarm_create_clock_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_SUB;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 8000;
    create_view_param.ke_maplist = clock_ke_maplist;
    create_view_param.se_maplist = NULL;
    create_view_param.view_cb = tm_alarm_clock_view;
    com_view_add(COM_VIEW_ID_DISP_CLOCK, &create_view_param);
}

app_result_e clock_key_deal_exit(void);
app_result_e clock_key_deal_switch_set_calendar(void);
app_result_e clock_key_deal_switch_set_alarm(void);

void clock_handle(void);

const key_event_map_t __section__(".rodata.ke_maplist") clock_ke_maplist[] =
{
#if 0 //不支持在小机下操作闹钟和日历
    /*! 切换到设置日历功能 */
    { { KEY_CLK, 0, KEY_TYPE_SHORT_UP, 0 }, clock_key_deal_switch_set_calendar },
    { { KEY_ABREC, 0, KEY_TYPE_SHORT_UP, 0 }, clock_key_deal_switch_set_calendar },

    /*! 切换到设置闹钟功能 */
    { { KEY_ABREC, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, clock_key_deal_switch_set_alarm },
    { { KEY_CLK, 0, KEY_TYPE_LONG, KEY_DEAL_FILTER }, clock_key_deal_switch_set_alarm },
#endif
    /*! 确定退出 */
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, 0 }, clock_key_deal_exit },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, 0 }, NULL },
};

app_result_e clock_key_deal_exit(void)
{
    com_view_remove_top_view();

    return RESULT_NULL;
}

app_result_e clock_key_deal_switch_set_calendar(void)
{
    tm_alarm_create_set_calendar_view();

    return RESULT_NULL;
}

app_result_e clock_key_deal_switch_set_alarm(void)
{
    tm_alarm_create_set_alarm_view();

    return RESULT_NULL;
}

static void __tm_alarm_clock_redraw(void)
{
    uint16 hour, minute;

    sys_get_time(&(g_p_clock_show->time));

    hour = g_p_clock_show->time.hour;
    minute = g_p_clock_show->time.minute;

    led_display(LCD_COL, 0xff, 0); //清除:号
    led_display(LCD_FM, 0xff, 0); //清楚.号及FM标记
    led_display(LCD_NO1, 0xff, 0); //清楚.号及FM标记

    led_display(LCD_COL, 0xff, (uint32 )g_p_clock_show->twinkling_flag);
    led_display(0, (hour / 10) + NUM_0, 1);
    led_display(1, (hour % 10) + NUM_0, 1);
    led_display(2, (minute / 10) + NUM_0, 1);
    led_display(3, (minute % 10) + NUM_0, 1);

    if (g_p_clock_show->twinkling_flag == TRUE)
    {
        g_p_clock_show->twinkling_flag = FALSE;
    }
    else
    {
        g_p_clock_show->twinkling_flag = TRUE;
    }
}

void tm_alarm_clock_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            g_p_clock_show = sys_malloc(sizeof(clock_show_var_t));
            if (g_p_clock_show == NULL)
            {
                PRINT_ERR("clock view malloc fail!!");
                while (1)
                {
                    ; //nothing for QAC
                }
            }

            sys_get_date(&(g_p_clock_show->date));
            if (g_p_clock_show->date.year > 2099)
            {
                g_p_clock_show->date.year = 2000;
                sys_set_date(&(g_p_clock_show->date));
            }

            g_p_clock_show->twinkling_flag = TRUE;
            g_p_clock_show->twinkling_timer_id = set_app_timer(APP_TIMER_ATTRB_COM_UI \
                | (COM_VIEW_ID_DISP_CLOCK << 8), 500, clock_handle);            
            __tm_alarm_clock_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
            kill_app_timer(g_p_clock_show->twinkling_timer_id);

            sys_free(g_p_clock_show);
            g_p_clock_show = NULL;
        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            __tm_alarm_clock_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            __tm_alarm_clock_redraw();
        }
        break;

        default:
        break;
    }
}

//时钟2Hz定时处理
void clock_handle(void)
{
    tm_alarm_clock_view(VIEW_UPDATE_REDRAW);
}

