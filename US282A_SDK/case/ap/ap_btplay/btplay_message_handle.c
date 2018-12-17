/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌前台应用用户交互实现，包括按键事件和系统事件处理。
 * 作者：cailizhen
 ********************************************************************************/

#include  "ap_btplay.h"

app_result_e a2dp_play_deal(void);
app_result_e a2dp_pause_deal(void);
app_result_e a2dp_next_deal(void);
app_result_e a2dp_prev_deal(void);

app_result_e key_ff_deal(void);
app_result_e key_fb_deal(void);
app_result_e key_fast_cancel_deal(void);

app_result_e key_play_deal(void);
app_result_e key_next_deal(void);
app_result_e key_prev_deal(void);

//app_result_e com_hid_send_key(void);
//app_result_e com_hid_disconnect(void);
app_result_e deal_btstack_hd_err(void);
app_result_e deal_dsp_reset(void);
#ifdef __BQB_TEST_PROFILE_
app_result_e key_deal_delay_report(void);
#endif

const sys_event_map_t __section__(".rodata.se_maplist") btplay_se_maplist[] =
{
    {
        {   MSG_BTSTACK_ERR_HARDWARE_EXCEPTION, SYSMSG_STOP_TTS_YES}, deal_btstack_hd_err},

    {
        {   MSG_DSP_RESET, SYSMSG_STOP_TTS_YES}, deal_dsp_reset},
        
    /*! 结束标志 */
    {
        {   MSG_NULL, 0}, NULL},
};

const key_event_map_t __section__(".rodata.ke_maplist") btplay_ke_maplist[] =
{
    {
        {   KEY_NEXT, 0, KEY_TYPE_SHORT_UP, 0}, key_next_deal},
    {
        {   KEY_PREV, 0, KEY_TYPE_SHORT_UP, 0}, key_prev_deal},
    {
        {   KEY_NEXT_VOLADD, 0, KEY_TYPE_SHORT_UP, 0}, key_next_deal},
    {
        {   KEY_PREV_VOLSUB, 0, KEY_TYPE_SHORT_UP, 0}, key_prev_deal},
    {
        {   KEY_PLAY, 0, KEY_TYPE_SHORT_UP, 0}, key_play_deal},
    {
        {   KEY_PHONEPLAY, 0, KEY_TYPE_SHORT_UP, 0}, key_play_deal},

    {
        {   KEY_NEXT, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, 0}, key_ff_deal},
    {
        {   KEY_NEXT, 0, KEY_TYPE_LONG_UP, 0}, key_fast_cancel_deal},
    {
        {   KEY_NEXT_VOLADD, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, 0}, key_ff_deal},
    {
        {   KEY_NEXT_VOLADD, 0, KEY_TYPE_LONG_UP, 0}, key_fast_cancel_deal},

    {
        {   KEY_PREV, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, 0}, key_fb_deal},
    {
        {   KEY_PREV, 0, KEY_TYPE_LONG_UP, 0}, key_fast_cancel_deal},
    {
        {   KEY_PREV_VOLSUB, 0, KEY_TYPE_LONG | KEY_TYPE_HOLD, 0}, key_fb_deal},
    {
        {   KEY_PREV_VOLSUB, 0, KEY_TYPE_LONG_UP, 0}, key_fast_cancel_deal},

#ifdef __BQB_TEST_PROFILE_
    {
        {   KEY_PREV, 0, KEY_TYPE_DBL_CLICK, 0}, key_deal_delay_report},
#endif

    /*! 结束标志 */
    {
        {   KEY_NULL, 0, KEY_TYPE_NULL, 0}, NULL},
};

void avrcp_tts_play(uint8 tts_id)
{
    if (g_avrcp_tts_enable == 1)
    {
        com_tts_state_play(TTS_MODE_ONLYONE, (void *) (uint32) tts_id);
    }
}

#ifdef ENABLE_TRUE_WIRELESS_STEREO

void btplay_tws_sync_deal(uint8 sync_val)
{
	 msg_apps_t msg;
   msg.type = MSG_BTSTACK_TWS_DEAL_MSG_SYNC;
   msg.content.data[0] = sync_val;
   send_sync_msg_btmanager(NULL, &msg, NULL, 0);	
}

#if 0
void send_cmd_to_other(uint8 op)
{
     msg_apps_t msg;
     msg.type = MSG_BTSTACK_TWS_SUB_MODE_FLAG;
     msg.content.data[0] = op;
     send_sync_msg_btmanager(NULL, &msg, NULL, 0);
}
#endif

bool check_tws_tts_play(void)
{
    bool ret = FALSE;

    PRINT_INFO_INT("phcon:", g_bt_stack_cur_info.tws_master_phonecon);

    if ((g_bt_stack_cur_info.tws_master_phonecon == 1)
            || (g_bt_stack_cur_info.dev_role == 0))
    {
        ret = TRUE;
    }

    return ret;
}

void tws_sync_phoncon(void)
{
    msg_apps_t msg;
    uint8 i;

    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (g_bt_stack_cur_info.rmt_dev[i].dev_type == TWS_DEV)
        {
            break;
        }
    }
    msg.type = MSG_BTSTACK_TWS_SEND_PHONCON_SYNC;
    send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[i].addr.bytes, &msg, NULL, 0);
    
    //发消息通知stack清除丢数标志
    send_cmd_to_other(3);
}

void __section__(".text.autoplay") check_tws_play_tts(void)
{
    tws_con_flag= (( g_bt_stack_cur_info.tws_hfp_con_flag== 1) ||(g_bt_stack_cur_info.phonetomastercon == 1));
   // tws_con_flag= (( g_bt_stack_cur_info.tws_hfp_con_flag== 1)&&(g_bt_stack_cur_info.phonetomastercon == 1));
    tws_discon_flag=(( g_bt_stack_cur_info.tws_hfp_con_flag== 0) &&(g_bt_stack_cur_info.phonetomasterdis== 1));
    //当主箱与手机蓝牙连接的时候播报蓝牙连接成功
    //  if((flag1!=g_bt_stack_cur_info.phonetomastercon)&&(g_bt_stack_cur_info.phonetomastercon==1))
    if((flag1!=tws_con_flag)&&(tws_con_flag==1))
    {
    	  g_need_draw = TRUE;
        g_tws_need_tts_play1 = TRUE;
    }
     //   flag1=g_bt_stack_cur_info.phonetomastercon;
     flag1=tws_con_flag;
 
    //主箱断开手机
    //if((flag_ph_dis_master!=g_bt_stack_cur_info.phonetomasterdis)&&(g_bt_stack_cur_info.phonetomasterdis==1))
    if((flag_ph_dis_master!=tws_discon_flag)&&(tws_discon_flag==1))
    {
        //PRINT_INFO(" GGGGG");
        g_tws_need_tts_play = TRUE;
    }
    //flag_ph_dis_master=g_bt_stack_cur_info.phonetomasterdis;
    flag_ph_dis_master=tws_discon_flag;

    if(flag_pair!=g_bt_stack_cur_info.pair)
    {
        if(g_bt_stack_cur_info.pair==1)
        {
            g_tws_need_tts_play = TRUE;
            if(flag_no_tts==1)
            {
                flag_no_tts=0;
            }
            else
            {
                flag2=1;   
                btplay_tws_sync_deal(4);   
            }
           // flag2=1;
        }
        //调试对箱显示 ，主副箱连上就显示BU
        g_need_draw =TRUE;

    }
    flag_pair=g_bt_stack_cur_info.pair;
    
    if(g_last_role_save != g_bt_stack_cur_info.dev_role)
    {
        if(g_bt_stack_cur_info.dev_role==TWS_SLAVE)
        {
            //发消息通知stack清除丢数标志
            send_cmd_to_other(2);
        }
    }
    g_last_role_save = g_bt_stack_cur_info.dev_role;
    
    //主箱断开副箱
    if((flag_dis_tws_dev!=g_bt_stack_cur_info.tws_dev_discon)&&(g_bt_stack_cur_info.tws_dev_discon==1))
    {
        g_tws_need_tts_play = TRUE;
     //   PRINT_INFO(" G");
    }
    flag_dis_tws_dev=g_bt_stack_cur_info.tws_dev_discon;
    //当主箱连接上手机的时候，需要通知从箱
    if((g_bt_stack_cur_info.dev_role==TWS_MASTER)&&(g_bt_stack_cur_info.tws_master_phonecon==1))
    {
        if ((tws_phcon_sync_flag== 0)
                &&(tws_phcon_timer_id == -1))
        {
            //在此处将丢数标志清除
            //此时打开丢数标志是否可能出现对不齐的问题  ??
          /*  if(sys_comval->tts_enable==FALSE)
            {
                  send_cmd_to_other(3);
            }*/
            g_need_draw = TRUE;
            tws_phcon_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,
                    3000, tws_sync_phoncon);
            tws_phcon_sync_flag = 1;
        }
    }
  else if ((g_bt_stack_cur_info.tws_master_phonecon==0)&&(tws_phcon_sync_flag == 1))
    {
        tws_timer_kill(&tws_phcon_timer_id);
        tws_phcon_sync_flag = 0;
    }
    else
    {
        ;// do nothing
    }

}
#endif

void handl_fast_auto_play(void)
{
    //退出时还处于快进快退中，需要退出快进快退
    if (fast_play_flag != BTPLAY_FAST_NORMAL)
    {
        key_fast_cancel_deal();
    }

#ifdef ENABLE_TRUE_WIRELESS_STEREO
  tws_timer_kill(&tws_sync_timer_id);
   tws_vol_sync_flag = 0;

#endif

}

app_result_e a2dp_play_deal(void)
{ 
    if (com_btmanager_avrcp_op(BTPLAY_AVRCP_PAUSE) == 0)
    {
        sys_os_time_dly(10); //不要使用sys_mdelay，这种接口在低优先级任务不准确
    }
#ifndef __BQB_TEST_PROFILE_
    if ((g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].serv_conn & AVRCP_CONNECTED_MY) != 0)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
#ifdef ENABLE_PLAY_PAUSE_TTS
        if(g_bt_stack_cur_info.dev_role!=0)
        {
            if ((g_avrcp_tts_enable ==TRUE)&&(sys_comval->tts_enable!=FALSE))           
            {
                //发送卸载中间件命令给从箱
                if(g_bt_stack_cur_info.dev_role==TWS_SLAVE)
                {
                //   g_app_info_state_all.g_send_exit_mmm_tts=1;
                }
            }
            else
            {
                if(g_bt_stack_cur_info.dev_role==TWS_MASTER)
                {
                    //发送卸载中间件命令给从箱
                     g_app_info_state_all.g_send_exit_mmm_tts=2;

                }
            }
        }
#endif
#endif
#ifdef ENABLE_TRUE_WIRELESS_STEREO
#ifdef ENABLE_PLAY_PAUSE_TTS
        avrcp_tts_play(TTS_PAUSE_SONG);
#endif
#else
        avrcp_tts_play(TTS_PAUSE_SONG);
#endif
    }
#endif

    return RESULT_NULL;
}

app_result_e a2dp_pause_deal(void)
{
#ifdef ENABLE_TRUE_WIRELESS_STEREO
#ifdef ENABLE_PLAY_PAUSE_TTS
    if ((g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].serv_conn & AVRCP_CONNECTED_MY) != 0)
    {

        avrcp_tts_play(TTS_PLAY_SONG);
    }
#endif
#else
    if ((g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].serv_conn & AVRCP_CONNECTED_MY) != 0)
    {

        avrcp_tts_play(TTS_PLAY_SONG);
    }
#endif
    if (com_btmanager_avrcp_op(BTPLAY_AVRCP_PLAY) == 0)
    {
        sys_os_time_dly(10); //不要使用sys_mdelay，这种接口在低优先级任务不准确
    }

    return RESULT_NULL;
}

app_result_e a2dp_next_deal(void)
{
    if (com_btmanager_avrcp_op(BTPLAY_AVRCP_NEXT) == 0)
    {
        sys_os_time_dly(10); //不要使用sys_mdelay，这种接口在低优先级任务不准确
    }

    if ((g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].serv_conn & AVRCP_CONNECTED_MY) != 0)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        if(g_bt_stack_cur_info.dev_role!=0)
        {
            g_app_info_state_all.g_send_exit_mmm_tts=1;
        }
#endif

        avrcp_tts_play(TTS_NEXT_SONG);
    }

    return RESULT_NULL;
}

app_result_e a2dp_prev_deal(void)
{
    if (com_btmanager_avrcp_op(BTPLAY_AVRCP_PREV) == 0)
    {
        sys_os_time_dly(10); //不要使用sys_mdelay，这种接口在低优先级任务不准确
    }

    if ((g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].serv_conn & AVRCP_CONNECTED_MY) != 0)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //语音播报上一曲
        if(g_bt_stack_cur_info.dev_role!=0)
        {
            g_app_info_state_all.g_send_exit_mmm_tts=1;
        }
#endif
        avrcp_tts_play(TTS_LAST_SONG);

    }

    return RESULT_NULL;
}

uint32 key_mute_fun(void)
{
     uint32 cru_time,twice_change_time;
     cru_time = sys_get_ab_timer();
     twice_change_time = cru_time - g_last_change_state_time;
     
     if(twice_change_time >= 1000)
     {
         g_second_change_state_flag = 0;       
     }
     return cru_time;
}
app_result_e key_play_deal(void)
{
    uint32 cru_time;
    
#ifdef __BQB_TEST_PROFILE_
    libc_print("-BTPLAY-", g_btplay_cur_info.status, 2);
    a2dp_play_deal();
    return RESULT_NULL;
#endif

#ifdef ENABLE_TRUE_WIRELESS_STEREO  
    g_last_btplay_status_before_unlink=0;
    if (check_tws_tts_play() == FALSE)
    {
        PRINT_INFO(" g1");
        return RESULT_NULL;
    }
#endif

    if (g_btplay_init_flag == TRUE)
    {
      //  PRINT_INFO(" g2");
        return RESULT_NULL;
    }

    if (g_btplay_cur_info.status == BTPLAY_IDLE)
    {
   //     PRINT_INFO(" g3");
        return RESULT_NULL;
    }

#ifdef ENABLE_TRUE_WIRELESS_STEREO
#ifdef ENABLE_PLAY_PAUSE_TTS
    cru_time = key_mute_fun();
    if(g_second_change_state_flag != 1)
    {
         //   if(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status == A2DP_STATUS_PLAY) 
          if (g_btplay_cur_info.status == BTPLAY_PLAY)
          {
               a2dp_play_deal();
          }
           else
           {
                a2dp_pause_deal();
            }
            g_second_change_state_flag = 1;
            g_last_change_state_time = cru_time;
    }
    return RESULT_NULL;
#else
    return a2dp_pause_deal();
#endif

#else
    cru_time = key_mute_fun();
    if(g_second_change_state_flag != 1)
    {
         //   if(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status == A2DP_STATUS_PLAY) 
          if (g_btplay_cur_info.status == BTPLAY_PLAY)
          {
               a2dp_play_deal();
          }
           else
           {
                a2dp_pause_deal();
            }
            g_second_change_state_flag = 1;
            g_last_change_state_time = cru_time;
    }
    return RESULT_NULL;

#endif
/*
    if (g_btplay_cur_info.status == BTPLAY_PLAY)
 //if(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status == A2DP_STATUS_PLAY) 
    {
        //stop
        return a2dp_play_deal();
    }

    //play
    return a2dp_pause_deal();

*/    
}

app_result_e key_next_deal(void)
{
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if (check_tws_tts_play() == FALSE)
    {
        return RESULT_NULL;
    }
#endif

    if (g_btplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if (g_btplay_cur_info.status == BTPLAY_IDLE)
    {
        return RESULT_NULL;
    }

    return a2dp_next_deal();
}

app_result_e key_prev_deal(void)
{
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if (check_tws_tts_play() == FALSE)
    {
        return RESULT_NULL;
    }
#endif

    if (g_btplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if (g_btplay_cur_info.status == BTPLAY_IDLE)
    {
        return RESULT_NULL;
    }

    return a2dp_prev_deal();
}

app_result_e key_ff_deal(void)
{
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if (check_tws_tts_play() == FALSE)
    {
        return RESULT_NULL;
    }
#endif

    if (g_btplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if ((g_btplay_cur_info.status == BTPLAY_PLAY) && (fast_play_flag != BTPLAY_FAST_FORWARD))
    {
        if (com_btmanager_avrcp_op(BTPLAY_AVRCP_FFORWARD) == 0)
        {
            fast_play_flag = BTPLAY_FAST_FORWARD;
        }
    }

    return RESULT_NULL;
}

app_result_e key_fb_deal(void)
{
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if (check_tws_tts_play() == FALSE)
    {
        return RESULT_NULL;
    }
#endif

    if (g_btplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if ((g_btplay_cur_info.status == BTPLAY_PLAY) && (fast_play_flag != BTPLAY_FAST_BACKWARD))
    {
        if (com_btmanager_avrcp_op(BTPLAY_AVRCP_FBACKWARD) == 0)
        {
            fast_play_flag = BTPLAY_FAST_BACKWARD;
        }
    }

    return RESULT_NULL;
}

app_result_e key_fast_cancel_deal(void)
{
    if (g_btplay_init_flag == TRUE)
    {
        return RESULT_NULL;
    }

    if (fast_play_flag == BTPLAY_FAST_FORWARD)
    {
        if (com_btmanager_avrcp_op(BTPLAY_AVRCP_RELEASEFF) == 0)
        {
            fast_play_flag = BTPLAY_FAST_NORMAL;
        }
    }
    else if (fast_play_flag == BTPLAY_FAST_BACKWARD)
    {
        if (com_btmanager_avrcp_op(BTPLAY_AVRCP_RELEASEFB) == 0)
        {
            fast_play_flag = BTPLAY_FAST_NORMAL;
        }
    }
    else
    {
        ; //nothing
    }

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

#ifdef __BQB_TEST_PROFILE_
app_result_e key_deal_delay_report(void)
{
    libc_print("key_deal_delay_report", 0, 0);
    com_btmanager_delay_report();
    return RESULT_NULL;
}
#endif

