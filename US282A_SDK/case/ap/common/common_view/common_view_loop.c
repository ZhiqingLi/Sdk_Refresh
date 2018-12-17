/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用视图LOOP，需要挂载在前台应用场景主循环中运行；视图LOOP负责：按键
 *       事件分发处理，系统事件分发处理，BT事件分发处理及BT检测，软定时器HANDLE，
 *       代理DSP打印和中断打印，KETTONE播放，TTS播放等。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_func.h"
#include "app_view.h"

app_result_e __section__(".text.com_view_loop_key") com_view_loop_key(input_gui_msg_t *p_gui_msg)
{
    app_result_e result = RESULT_NULL;
    key_event_cb ke_cb;
    key_event_t *p_ke = &(p_gui_msg->data.kmsg);
    bool only_common = FALSE;

    key_event_deal_agin:
    if (com_view_key_event(p_ke, g_view_manager_infor.p_key_view_infor->ke_maplist, &ke_cb, only_common) == 0)
    {
        if (ke_cb != NULL)
        {
            g_p_view_ke = p_ke;
            result = ke_cb();
            if (result == RESULT_KEY_EVENT_REDEAL_BY_COMMON)
            {
                only_common = TRUE;
                goto key_event_deal_agin;
            }
        }
    }
    else
    {
        //遇到不认识的按键消息
        switch (g_view_manager_infor.p_key_view_infor->unknown_key_deal)
        {
            case UNKNOWN_KEY_QUIT_FILTER:
            com_view_remove_top_view();
            break;

            case UNKNOWN_KEY_QUIT_REDEAL:
            case UNKNOWN_KEY_QUIT_IFMAIN:
            com_view_remove_top_view();
            libc_memcpy(&g_buffer_gui_msg, p_gui_msg, sizeof(input_gui_msg_t));
            break;

            case UNKNOWN_KEY_IGNORE:
            default:
            break;
        }
    }

    return result;
}

app_result_e __section__(".text.com_view_loop_sys") com_view_loop_sys(private_msg_t *p_pri_msg, bool tts_flag)
{
    app_result_e result = RESULT_NULL;
    sys_event_t tmp_se;
    sys_event_t *p_se = &tmp_se;
    bool only_common = FALSE;
    const sys_event_map_t *p_se_maplist = NULL;
    sys_event_ret_t tmp_ret;

    if (g_view_manager_infor.top < MVC_VIEW_DEEPTH)
    {
        p_se_maplist = g_view_infor[0].se_maplist;
    }
    else
    {
        p_se_maplist = NULL;
    }

    p_se->msg_type = p_pri_msg->msg.type;
    p_se->msg_option = 0;

    sys_event_deal_agin:
    if (com_view_sys_event(p_se, p_se_maplist, &tmp_ret, only_common) == 0)
    {
        if (tmp_ret.ret_cb != NULL)
        {
            if ((tts_flag == TRUE) && ((tmp_ret.ret_option & SYSMSG_STOP_TTS_OPTION_MASK) == SYSMSG_STOP_TTS_YES))
            {
                result = RESULT_FORCE_TTS_STOP;
            }
            else
            {
                result = (tmp_ret.ret_cb)(p_pri_msg->msg.content.addr);
                if (result == RESULT_SYS_MSG_REDEAL_BY_COMMON)
                {
                    only_common = TRUE;
                    goto sys_event_deal_agin;
                }
            }
        }
    }

    return result;
}

app_result_e com_view_loop(void)
{
    app_result_e result = RESULT_NONE_EVENT;
    input_gui_msg_t gui_msg;
    private_msg_t pri_msg;

    //实时能量检测，动态更新DAE参数
#if (SUPPORT_DETECT_ENERGY == 1)
    if ((sys_comval->signal_energy_enable == 1)
            && (sys_comval->signal_variable_mode == 1)
            && (g_app_info_state_all.energy_available_flag == TRUE))
    {
        com_detect_energy_realtime();
    }
#endif

    if (get_gui_msg(&gui_msg) == TRUE)
    {
        result = com_view_loop_key(&gui_msg);
    }
    else if (get_app_msg(&pri_msg) == TRUE)
    {
        result = com_view_loop_sys(&pri_msg, FALSE);
    }
    else if (g_app_info_vector[APP_TYPE_BTSTACK].used != 0)
    {
        result = com_btmanager_loop(FALSE);
    }
    else
    {
        ; //nothing for QAC
    }

    if (result > RESULT_COMMON_RESERVE)
    {
        return result;
    }

    //软定时器扫描
    if (g_app_timer_count > 0)
    {
        handle_timers(g_app_timer_vector, g_app_timer_count);
    }
    handle_timers(g_com_app_timer_vct, COM_APP_TIMER_MAX);

    //DSP打印和中断打印代理打印
    //sys_dsp_print();

    if ((g_keytone_infor.count > 0) || (g_keytone_infor.thread_eixt == TRUE))
    {
        keytone_play_deal();
    }

    if ((g_tts_play_info.status != TTS_STATUS_IDLE) || (g_tts_play_info.count > 0))
    {
        com_tts_state_play_deal();
    }

    return result;
}
