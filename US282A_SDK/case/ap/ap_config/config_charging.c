/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_charging
 * \brief    关机后充电的处理，包括充电状态的显示，满电的监测以及按键，拔线的处理
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "ap_config.h"

static uint8 config_charge_mode = 0; //充电模式：0表示开机充电模式，会开启后台蓝牙，拔线后返回第一个应用，
                                     //1表示关机充电模式，拔线后关机；同以下宏定义
//#define SWITCH_ON           0
//#define SWITCH_OFF          1
static int8 display_timer_id = -1;
static uint16 charger_out_cnt = 0;
static bool charge_full_flag = FALSE;

app_result_e config_charge_key_deal_quit(void);

const key_event_map_t __section__(".rodata.ke_maplist") config_charge_ke_maplist[] =
{
    {{ KEY_MODE, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, config_charge_key_deal_quit},
    {{ KEY_PLAY, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, config_charge_key_deal_quit},
    {{ KEY_PHONEPLAY, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, config_charge_key_deal_quit},

    /*! 其他按键都不响应 */
    { { KEY_MUTE, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, NULL },
    { { KEY_VADD, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, NULL },
    { { KEY_NEXT_VOLADD, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, NULL },
    { { KEY_VSUB, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, NULL },
    { { KEY_PREV_VOLSUB, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, NULL },
    { { KEY_NEXT, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, NULL },
    { { KEY_PREV, 0, KEY_TYPE_ALL, KEY_DEAL_FILTER}, NULL },

    /*! 结束标志 */
    {{ KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL}, NULL},
};

app_result_e config_charge_key_deal_quit(void)
{
    return RESULT_CONFIG_CHARGE_QUIT;
}

app_result_e config_charge_sys_deal_quit(void *ev_param);

const sys_event_map_t __section__(".rodata.se_maplist") config_charge_se_maplist[] =
{
    {{MSG_USB_UNSTICK, SYSMSG_STOP_TTS_YES },      NULL},
    {{MSG_SYS_ADAPTOR_OUT, SYSMSG_STOP_TTS_YES },  NULL},

    /*! 结束标志 */
    {{MSG_NULL, 0 }, NULL},
};

app_result_e config_charge_sys_deal_quit(void *ev_param)
{
    if (config_charge_mode == SWITCH_OFF)
    {
        return RESULT_POWER_OFF;
    }
    else
    {
        return RESULT_CONFIG_CHARGE_QUIT;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  判断是否充满电，显示充电状态
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
void _display_full(void)
{
    //显示充电已满
    if (config_charge_mode == SWITCH_ON)
    {
        led_display(NUMBER1, NUM_F, 1);
        led_display(NUMBER2, NUM_U, 1);
        led_display(NUMBER3, NUM_L, 1);
        led_display(NUMBER4, NUM_L, 1);
    }
}

void _display_charging(void)
{
    if (get_charge_state() == BAT_FULL)
    {
        if (charge_full_flag == FALSE)
        {
            _display_full();

            charge_full_flag = TRUE;
        }
    }
}

void _display_charge_redraw(void)
{
    led_clear_screen();
    if (config_charge_mode == SWITCH_ON)
    {
        if (get_charge_state() == BAT_FULL)
        {
            _display_full();
        }
        else
        {
            led_display(NUMBER1, NUM_C, 1);
            led_display(NUMBER2, NUM_H, 1);
            led_display(NUMBER3, NUM_A, 1);
            led_display(NUMBER4, NUM_R, 1);
        }
    }
}

void config_charge_main_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                _display_charge_redraw();
                display_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 1000, (timer_proc) _display_charging);
            }
#endif
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                if (display_timer_id != -1)
                {
                    kill_app_timer(display_timer_id);
                }
            }
#endif
        }
        break;

        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                _display_charge_redraw();
            }
#endif
        }
        break;

        default:
        break;
    }
}

void config_charge_create_main_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MAIN;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 0;
    create_view_param.ke_maplist = config_charge_ke_maplist;
    create_view_param.se_maplist = config_charge_se_maplist;
    create_view_param.view_cb = config_charge_main_view;
    com_view_add(APP_VIEW_ID_MAIN, &create_view_param);
}

/******************************************************************************/
/*!
 * \par  Description:
 *  充电的处理
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
app_result_e config_charging(int param)
{
    app_result_e result = RESULT_NULL;

    config_charge_mode = (uint8) param;

    //强制进入充电模式
    set_cur_func_id(APP_FUNC_CHARGING);

    //创建主视图
    config_charge_create_main_view();

    if (config_charge_mode == SWITCH_ON)
    {
        if (g_comval.support_bt_inback == 1)
        {
            //开机进入charge,需要创建蓝牙协议栈
            com_btmanager_init(FALSE);
        }
    }

    while (1)
    {
        result = com_view_loop();

        if (result > RESULT_COMMON_RESERVE)
        {
            break;
        }

        if (detect_power_on() == FALSE)//主动监测电源拔出，实际上可以不要，后面实验一下是否可去掉
        {
            charger_out_cnt ++;
            if (charger_out_cnt > 100)
            {
                charger_out_cnt = 0;
                result = config_charge_sys_deal_quit(NULL);
                break;
            }
        }
        else
        {
            charger_out_cnt = 0;
        }

        //挂起10ms，多任务调度
        sys_os_time_dly(1);
    }

    if ((config_charge_mode == SWITCH_OFF) && (result == RESULT_ENTER_ALARM))
    {
        g_ap_switch_var.rtcalarm_poweron = TRUE;
    }

    if (result == RESULT_CONFIG_CHARGE_QUIT)
    {
        result = RESULT_NULL;//进入第一个应用，由 config.txt 配置项指定
    }

    return result;
}

