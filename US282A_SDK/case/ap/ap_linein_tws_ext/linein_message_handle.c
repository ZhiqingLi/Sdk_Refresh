/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌前台应用用户交互实现，包括按键事件和系统事件处理。
 * 作者：
 ********************************************************************************/

#include  "ap_linein.h"

app_result_e a2dp_play_deal(void);
app_result_e a2dp_pause_deal(void);

app_result_e key_play_deal(void);

app_result_e deal_btstack_hd_err(void);
app_result_e deal_dsp_reset(void);
app_result_e deal_linein_in_status(void);

app_result_e linein_sys_deal_aux_out(void *ev_param);

const sys_event_map_t __section__(".rodata.se_maplist") linein_se_maplist[] =
{
    { { MSG_BTSTACK_ERR_HARDWARE_EXCEPTION, SYSMSG_STOP_TTS_YES}, deal_btstack_hd_err},

    { { MSG_DSP_RESET, SYSMSG_STOP_TTS_YES}, deal_dsp_reset},

    { { MSG_LINEIN_OUT, SYSMSG_STOP_TTS_YES}, linein_sys_deal_aux_out},
    
    { { MSG_LINEIN_IN_STATUS_SYNC, SYSMSG_STOP_TTS_YES}, deal_linein_in_status},

    /*! 结束标志 */
    { { MSG_NULL, 0}, NULL},
};

const key_event_map_t __section__(".rodata.ke_maplist") linein_ke_maplist[] =
{

    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, 0}, key_play_deal},
    
    { { KEY_PHONEPLAY, 0, KEY_TYPE_SHORT_UP, 0}, key_play_deal},

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, 0}, NULL},
};



void tws_sync_deal(uint8 sync_val)
{
    msg_apps_t msg;
    msg.type = MSG_BTSTACK_TWS_DEAL_MSG_SYNC;
    msg.content.data[0] = sync_val;
    send_sync_msg_btmanager(NULL, &msg, NULL, 0);
}

void handl_fast_auto_play(void)
{

#ifdef ENABLE_TRUE_WIRELESS_STEREO
    tws_timer_kill(&tws_sync_timer_id);
    tws_vol_sync_flag = 0;

#endif

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

app_result_e a2dp_play_deal(void)
{
    linein_send_msg(NULL, NULL, MSG_LINEIN_EG_PLAY_SYNC);
    g_config_var.engine_state = BTPLAY_PLAY;
    sys_vm_write(&g_config_var,VM_AP_CONFIG,sizeof(g_config_var_t));

    return RESULT_NULL;
}

app_result_e a2dp_pause_deal(void)
{

    linein_send_msg(NULL, NULL, MSG_LINEIN_EG_STOP_SYNC);
    g_config_var.engine_state = BTPLAY_PAUSE;
    sys_vm_write(&g_config_var,VM_AP_CONFIG,sizeof(g_config_var_t));
    

    return RESULT_NULL;
}

uint32 key_mute_fun(void)
{
    uint32 cru_time, twice_change_time;
    cru_time = sys_get_ab_timer();
    twice_change_time = cru_time - g_last_change_state_time;

    if (twice_change_time >= 1000)
    {
        g_second_change_state_flag = 0;
    }
    return cru_time;
}
app_result_e key_play_deal(void)
{

    if (g_btplay_cur_info.status == BTPLAY_PLAY)
    {
        a2dp_pause_deal();
    }
    else
    {
        a2dp_play_deal();
    }
    //g_need_draw = TRUE;
    return RESULT_NULL;
}

app_result_e deal_btstack_hd_err(void)
{

    //卸载掉btplay_eg
    kill_console();
    //先卸载掉BT STACK，并断电
    com_btmanager_exit(FALSE, TRUE);

    //再装载BT STACK
    g_need_auto_connect_timeout = 1;
    g_neednot_tts_play_reset = 1;
    com_btmanager_init(FALSE);
    g_need_auto_connect_timeout = 0;
    linein_set_discover_ctl(1);
    //创建btplay_eg
    create_console();

    return RESULT_NULL;
}

app_result_e deal_dsp_reset(void)
{

    //卸载掉btplay_eg
    kill_console();

    //创建btplay_eg
    create_console();

    return RESULT_NULL;
}

app_result_e linein_sys_deal_aux_out(void *ev_param)
{

    app_result_e result = RESULT_NULL;

    if (g_bt_stack_cur_info.dev_role != 2)
    {
        result = RESULT_NEXT_FUNCTION;
    }

    return result;
}


app_result_e deal_linein_in_status(void)
{
	 if (g_bt_stack_cur_info.dev_role == TWS_SLAVE)
	 {
	 	  msg_apps_t msg;
          msg.type = MSG_BTSTACK_TWS_SYNC_CMD;
          msg.content.data[0] = TWS_SLAVE_ENTER_SYNC;
          send_sync_msg_btmanager(NULL, &msg, NULL, 0);	 	
	 }	
	 return RESULT_NULL;
}


