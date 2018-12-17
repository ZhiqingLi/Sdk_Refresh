/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief    打电话显示模块
 *      <author>       <time>
 *       Wekan   2015-3-27
 *******************************************************************************/

#include  "ap_btcall.h"

#if (SUPPORT_LED_DRIVER == 1)

/******************************************************************************
 * \par  Description:   显示蓝牙连接状态
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

void wait_link_display(void)
{
    if (g_btcall_cur_info.status != BTCALL_IDLE)
    {
        return;
    }

    if (g_display_flag == TRUE)
    {
        led_display(NUMBER2, NUM_b, 1);
        led_display(NUMBER3, NUM_L, 1);
#if 1
        led_display(LCD_AUX, 0xff, 1);
#endif
        g_display_flag = FALSE;
    }
    else
    {
        led_clear_screen();
        g_display_flag = TRUE;
    }
}

/******************************************************************************
 * \par  Description:   显示蓝牙当前状态,BL,  C in,  C out,  AG,  GH
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

void btcall_ui_display(void)
{
    led_clear_screen();
    switch (g_btcall_cur_info.status)
    {
        case BTCALL_CALLIN:
        led_display(NUMBER1, NUM_C, 1);
        led_display(NUMBER2, NUM_ERROR, g_set_mic_mute);// -
        led_display(NUMBER3, NUM_I, 1);
        led_display(NUMBER4, NUM_N, 1);
        break;

        case BTCALL_CALLOUT:
        led_display(NUMBER1, NUM_C, 1);
        led_display(NUMBER2, NUM_ERROR, g_set_mic_mute);// -
        led_display(NUMBER3, NUM_O, 1);
        led_display(NUMBER4, NUM_U, 1);
        break;

        case BTCALL_HFP:
        led_display(NUMBER1, NUM_C, 1);
        led_display(NUMBER2, NUM_ERROR, g_set_mic_mute);// -
        led_display(NUMBER3, NUM_H, 1);
        led_display(NUMBER4, NUM_F, 1);
        break;

        case BTCALL_PHONE:
        led_display(NUMBER1, NUM_C, 1);
        led_display(NUMBER3, NUM_A, 1);
        led_display(NUMBER4, NUM_G, 1);
        break;
#if (__SUPPORT_SIRI_ == 1)
        case BTCALL_SIRI:
        led_display(NUMBER1, NUM_S, 1);
        led_display(NUMBER2, NUM_I, 1);
        led_display(NUMBER3, NUM_R, 1);
        led_display(NUMBER4, NUM_I, 1);
        break;
#endif //#if (__SUPPORT_SIRI_ == 1)
        default:
        led_display(NUMBER2, NUM_b, 1);
        led_display(NUMBER3, NUM_L, 1);
#if 1
        led_display(LCD_AUX, 0xff, 1);
#endif
        break;
    }
}
#endif

/******************************************************************************
 * \par  Description:   显示蓝牙主视图
 * \param[in]     mode 显示模式，参照view_update_e
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

void btcall_main_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                //btcall_ui_display();
                g_display_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 500, (timer_proc) wait_link_display);
                g_display_flag = FALSE;
            }
#endif
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                if (g_display_timer_id != -1)
                {
                    kill_app_timer(g_display_timer_id);
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
                btcall_ui_display();
            }
#endif
        }
        break;

        default:
        break;
    }
}

/******************************************************************************
 * \par  Description:   创建显示蓝牙主视图
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

void btcall_create_main_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MAIN;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 0;
    create_view_param.ke_maplist = btcall_ke_maplist;
    create_view_param.se_maplist = btcall_se_maplist;
    create_view_param.view_cb = btcall_main_view;
    com_view_add(APP_VIEW_ID_MAIN, &create_view_param);
}
