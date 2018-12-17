/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：LineIn的处理函数
 * 作者：Gary Wang
 ********************************************************************************/
#include "app_linein.h"
static void update_sound_mode(void);
void  __section__(".BANK_CONTROL_1_2")update_waves_bin_para(void);
static int8 linein_sound_mode_timer_id = -1;

const key_event_map_t __section__(".rodata.ke_maplist") linein_ke_maplist[] =
{
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, key_play_deal },
    { { KEY_PHONEPLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, key_play_deal },
    //{{KEY_EQ, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, KEY_DEAL_NULL}, key_eq_deal},

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL }, NULL },
};

app_result_e key_play_deal(void)
{
    if (g_linein_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    switch (g_linein_eg_status.play_status)
    {
        case linein_play_status_stop:
        //play
        if (g_neednot_tts_play == FALSE)
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PLAY_SONG);
        }
        //此处加延时，是为了解决手机端从暂停切换到播放时会出现按键时不对的问题
        sys_os_time_dly(4);
        linein_play();
        break;
        case linein_play_status_play:
        default:
        //pause
        linein_stop();
        if (g_neednot_tts_play == FALSE)
        {
            com_tts_state_play(TTS_MODE_ONLYONE, (void *) TTS_PAUSE_SONG);
        }
        break;
    }

    return RESULT_NULL;
}

app_result_e key_eq_deal(void)
{
    if (g_linein_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    return RESULT_KEY_EVENT_REDEAL_BY_COMMON;
}

app_result_e deal_btstack_hd_err(void)
{
    //先卸载掉BT STACK，并断电
    com_btmanager_exit(FALSE, TRUE);

    //再装载BT STACK
    g_need_auto_connect_timeout = 1;
    g_neednot_tts_play_reset = 1;
    com_btmanager_init(FALSE);
    g_need_auto_connect_timeout = 0;

    return RESULT_NULL;
}


app_result_e linein_sys_deal_aux_out(void *ev_param);

const sys_event_map_t __section__(".rodata.se_maplist") linein_se_maplist[] =
{
    { { MSG_LINEIN_OUT, SYSMSG_STOP_TTS_YES }, linein_sys_deal_aux_out },

    { { MSG_BTSTACK_ERR_HARDWARE_EXCEPTION, SYSMSG_STOP_TTS_YES}, deal_btstack_hd_err},
    
    /*! 结束标志 */
    { { MSG_NULL, 0 }, NULL },
};

app_result_e linein_sys_deal_aux_out(void *ev_param)
{
    return RESULT_NEXT_FUNCTION;
}

app_result_e _linein_loop_deal(void)
{
    app_result_e ret_vals = RESULT_NULL;
    linein_play_status_e last_play_status = linein_play_status_stop;

    while (1)
    {
        update_sound_mode();
        if (1 == g_app_info_state_all.waves_dae_para_update_flag)
        {
            update_waves_bin_para(); 
        }
        
        //获取状态并处理
        linein_get_status(&g_linein_eg_status);
        if (g_linein_eg_status.play_status != last_play_status)
        {
            com_view_update(APP_VIEW_ID_MAIN);

            last_play_status = g_linein_eg_status.play_status;
        }

        ret_vals = com_view_loop();
        if (ret_vals > RESULT_COMMON_RESERVE)
        {
            break;
        }
        
#ifdef SUPPORT_ASET_TEST
        if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
        {
            aset_test_loop_deal();
        }
#endif 

        //挂起10ms 多任务交互
        sys_os_time_dly(1);
    } //end of while

    return ret_vals;
}

bool linein_send_msg(void *input_param, void *output_param, uint32 msg_type)
{
    bool bret = TRUE;
    msg_apps_t msg;
    //返回消息内容存放变量
    msg_reply_t reply;

    //存放输入参数的地址
    msg.content.addr = input_param;

    //存放获取参数的地址
    reply.content = output_param;

    //消息类型(即消息名称)
    msg.type = msg_type;

    //发送同步消息
    if (send_sync_msg(APP_ID_LINEIN_EG, &msg, &reply, 0) == ERR_NO_ERR)
    {
        if (reply.type == MSG_REPLY_FAILED)
        {
            bret = FALSE;
        }
    }
    else
    {
        bret = FALSE;
    }

    return bret;
}

bool __section__(".rcode") linein_get_status(linein_eg_status_t* play_status)
{
    if (sys_share_query_read(APP_ID_LINEIN_EG, play_status) == -1)
    {
        PRINT_ERR("linein share query not exist!\n");
        return FALSE;
    }

    return TRUE;
}

bool linein_play(void)
{
    return linein_send_msg(NULL, NULL, MSG_LINEIN_EG_PLAY_SYNC);
}

bool linein_stop(void)
{
    return linein_send_msg(NULL, NULL, MSG_LINEIN_EG_STOP_SYNC);
}

//for energy detect period count modify 
void linein_modify_timer_proc(void)
{
     g_comval.dae_cfg.period_count = g_period_cnt;
     kill_app_timer(g_energy_detect_cnt_timer);
     g_energy_detect_cnt_timer = -1;;
     libc_print("cnet",g_comval.dae_cfg.period_count,2);
     com_update_dae_config(&g_comval.dae_cfg);
     com_reset_sound_volume(0);
}

static void sound_mode_handle(void)
{
    linein_play();
    
    kill_app_timer(linein_sound_mode_timer_id);
}

static void update_sound_mode(void)
{
    if (1 == sys_comval->dae_cfg.audiopp_type_update_flag)
    {
        sys_comval->dae_cfg.audiopp_type_update_flag = 0;

        linein_stop();
        linein_sound_mode_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,1000, sound_mode_handle);
    }
}

void  __section__(".BANK_CONTROL_1_2")update_waves_bin_para(void)
{
    g_app_info_state_all.waves_dae_para_update_flag = 0;
    linein_stop();
    linein_sound_mode_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,1000, sound_mode_handle);
}


