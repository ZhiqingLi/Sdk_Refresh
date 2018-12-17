/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌前台应用业务逻辑主循环实现。
 * 作者：cailizhen
 ********************************************************************************/

#include  "ap_btplay.h"

extern app_result_e key_fast_cancel_deal(void) __FAR__;
extern void handl_fast_auto_play(void)__FAR__;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
extern void check_tws_play_tts(void)__FAR__;
extern void tws_start_vol_sync(void) __FAR__;
#endif
static void update_sound_mode(void);
static void update_waves_bin_para(void);

static int8 btplay_sound_mode_timer_id = -1;

/******************************************************************************/
/*!
 * \par  Description:
 * \  获取蓝牙后台引擎的状态
 * \param[in]    none
 * \param[out]   bg_info 引擎状态指针
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 */
/*******************************************************************************/
bool get_engine_status(void)
{
    if (sys_share_query_read(APP_ID_BTPLAYEG, &g_btplay_cur_info) == -1)
    {
        //PRINT_ERR("btplay share query not exist!");
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \  根据后台蓝牙的状态进行处理
 * \param[in]    none
 * \param[out]   none
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 * \li
 */
/*******************************************************************************/
app_result_e bt_status_deal(void)
{
    app_result_e result = RESULT_NULL;

    get_engine_status();

    return result;
}

void btplay_tick_250ms(void)
{
    g_btplay_tick_250ms = 1;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    check_tws_play_tts();
    tws_start_vol_sync();
    if(g_tts_play_info.status==TTS_STATUS_IDLE)
   {
        //libc_print("volume_2",sys_comval->volume_current,2);  
        tws_volume=sys_comval->volume_current;
   }
#endif
}

#ifdef ENABLE_TRUE_WIRELESS_STEREO
void __section__(".text.autoplay") tws_vol_sync(void)
{

    uint8 i;
    uint16 vol;
    msg_apps_t msg;
    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (g_bt_stack_cur_info.rmt_dev[i].dev_type == TWS_DEV)
        {
            break;
        }
    }
    if(g_btmanager_gl_var.enable_avrcp_volume_sync!=0)
    {
        vol = (uint16) ((tws_volume * 0x7f) / VOLUME_VALUE_MAX);
        msg.type = MSG_BTSTACK_AVRCP_VOLUME_SYNC;
        msg.content.data[0] = vol;
        send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[i].addr.bytes, &msg, NULL, 0);
    }
    else
    {
        libc_print("@vol:",tws_volume,2);
        com_btmanager_tws_send(VOLUME_VALUE_FLAG, tws_volume,NULL,NULL,MSG_BTSTACK_TWS_SEND_MSG_SYNC); 
    }
    tws_sync_timer_id = -1;
}


void __section__(".text.autoplay") tws_start_vol_sync(void)
{
    if ((g_bt_stack_cur_info.dev_role == TWS_MASTER)
            &&(tws_vol_sync_flag == 0)
            &&(tws_sync_timer_id == -1))
    {
        PRINT_INFO("tws start vol sync");
        tws_sync_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,
                1000, tws_vol_sync);
        tws_vol_sync_flag = 1;
    }
    else if ((g_bt_stack_cur_info.dev_role != TWS_MASTER)&&(tws_vol_sync_flag == 1))
    {
        tws_timer_kill(&tws_sync_timer_id);
        tws_vol_sync_flag = 0;
    }
    else
    {
        ;// do nothing
    }
}
#endif

//A2DP和AVRCP连接成功后自动播放
void __section__(".text.autoplay") btplay_autoplay_deal(void)
{

#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if (g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].dev_type == TWS_DEV)
    {
        return;
    }
#endif

/*#ifdef __ESD_MODE_
    //libc_print("g_end_autoconnt",g_end_autoconnt,2);
    if(g_end_autoconnt == 1)
    {
        g_send_avrcp_play_tick = 0;
        g_fake_play_tick = 0;
        g_avrcp_autoplay_flag = 1;
        g_avrcp_autoplay_try = 0;
        g_avrcp_autoplay_cnt = 2;
        g_end_autoconnt = 0 ;
        libc_print("g_btplay_cur_info.status",g_btplay_cur_info.status,2);
    }
#endif */ 

    if (g_avrcp_autoplay_cnt == 0)
    {
        return;
    }

    //如果刚连接上后是处于linked状态，则需要发一个play消息启动播歌
    if ((g_last_btplay_status == BTPLAY_IDLE) && (g_btplay_cur_info.status != BTPLAY_IDLE))
    {
        g_send_avrcp_play_tick = 0;
        g_fake_play_tick = 0;
        g_avrcp_autoplay_flag = 1;
        g_avrcp_autoplay_try = 0;
        //libc_print("set autoplay_flag",g_avrcp_autoplay_flag,2);
    }

    if ((g_avrcp_autoplay_flag == 1) && (g_btplay_tick_250ms == 1))
    {
        g_btplay_tick_250ms = 0;

        //A2DP已连接但未播放
        if ((g_btplay_cur_info.status == BTPLAY_STOP) || (g_btplay_cur_info.status == BTPLAY_PAUSE))
        {
            //只有连上了a2dp和avrcp之后才发
            if ((g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].serv_conn & AVRCP_CONNECTED_MY) != 0)
            {
                g_send_avrcp_play_tick++;
                if (g_send_avrcp_play_tick >= (AVRCP_PLAY_START_TIME / 250)) //连上后等AVRCP_PLAY_START_TIME ms

                {
                      PRINT_INFO("BT PLAY AUTO PLAY");
                      //将回连上的标志清0
#ifdef ENABLE_TRUE_WIRELESS_STEREO
                      g_btmanager_gl_var.autoconnect_link_succ_flag =0;
                      g_auto_btplay_flag=1;
#endif
                    if (com_btmanager_avrcp_op(BTPLAY_AVRCP_PLAY) == 0)
                    {
                        sys_os_time_dly(10); //不要使用sys_mdelay，这种接口在低优先级任务不准确
                    }

                    g_avrcp_autoplay_try++;
                    g_send_avrcp_play_tick = 0;
                }

                if (g_avrcp_autoplay_try >= g_avrcp_autoplay_cnt) //发送次数限制

                {
                    g_send_avrcp_play_tick = 0;

                    g_avrcp_autoplay_flag = 0;
                }
            }
            g_fake_play_tick = 0;
        }
        else
        {
            g_send_avrcp_play_tick = 0;
            g_fake_play_tick++;
            //如果是播歌状态，延时3S才放弃后续发命令,过滤回连成功后的假start状态
            //如果此过程回到IDLE状态，那么也放弃自动播放
            if ((g_btplay_cur_info.status == BTPLAY_IDLE)
                    || (g_fake_play_tick >= (AVRCP_FAKE_START_TIME / 250)))
            {
                g_avrcp_autoplay_flag = 0;
            }
        }
    }
}

void __section__(".bank_2") bt_conn_states_init(void)
{
    uint8 cur_hfp_status;
    uint8 cur_a2dp_status;

#if (LINK_TTS_SEL == 0)
    cur_hfp_status = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status;
    cur_a2dp_status = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status;
    g_connected_flag = (cur_hfp_status != HFP_STATUS_NONE) || (cur_a2dp_status != A2DP_STATUS_NONE);
    g_last_connected_flag = g_connected_flag;
    //如果刚开始还未连接，强制播报一次等待连接
    if (g_connected_flag == FALSE)
    {
         if(g_ap_switch_var.call_background_status != CALL_BACK_STATUS_BACK)
         {
            g_need_tts_play = TRUE;
            PRINT_INFO_INT("g_last_connected_flag:",g_last_connected_flag);
         }
    }
#else
    //如果刚开始还未连接，强制播报一次等待连接
    if (g_btplay_cur_info.status == BTPLAY_IDLE)
    {
        g_need_tts_play = TRUE;
    }
#endif

}


void __section__(".bank_2") message_loop_init(void)
{
    //等待BTPLAY ENGINE状态可用
    while (get_engine_status() == FALSE)
    {
        sys_os_time_dly(1);
    }

    g_last_btplay_status = g_btplay_cur_info.status;

    //如果当前已经不是IDLE状态，强制刷新一次
    if (g_btplay_cur_info.status != BTPLAY_IDLE)
    {
        g_need_draw = TRUE;
    }

    g_last_conn_status = g_bt_stack_cur_info.conn_status;

    g_btplay_check_btplay_status_timer = sys_get_ab_timer();
    g_last_btplay_status_before_unlink = BTPLAY_IDLE;
    g_btplay_autoplay_flag = 0;
    
    tick_timer_id = set_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8), 250,
            (timer_proc) btplay_tick_250ms);    
}

void __section__(".bank_2") message_loop_exit(app_result_e result)
{
    handl_fast_auto_play();

#ifdef ENABLE_TRUE_WIRELESS_STEREO
//当进入s3bt的问题
    if(result != RESULT_SYSTEM_ENTER_S3)
    {

      if(g_bt_stack_cur_info.dev_role != 0)
      {
            send_cmd_to_other(1);
            //libc_print("@ret:",result,2);
           if ((result != RESULT_BLUETEETH_CALL_BACKGROUND)
           	   && (result != RESULT_ENTER_ALARM))
           {
        	   //注释掉,客户需要时,由客户自行修改
        	   // uint8 val_tmp = 3;	
            	//btplay_tws_sync_deal(val_tmp);
           }
           if ((g_btplay_cur_info.status == BTPLAY_PLAY)
           	   &&(result != RESULT_BLUETEETH_CALL_BACKGROUND)
           	   &&(result != RESULT_ENTER_ALARM))

           {
             //会影响从箱进入s3bt的时候，从箱给主箱发送一个暂停消息，使主箱迟一个周期进入s3bt
             com_btmanager_avrcp_op(BTPLAY_AVRCP_PAUSE);   
           }
       }
/*       if(g_btplay_cur_info.status == BTPLAY_PLAY)
       {
            //libc_print("pause:",0,0);
            //从箱切换应用,通知主箱暂停音乐播放
           //a2dp_play_deal();
           //会影响从箱进入s3bt的时候，从箱给主箱发送一个暂停消息，使主箱迟一个周期进入s3bt
           com_btmanager_avrcp_op(BTPLAY_AVRCP_PAUSE);   
        }   
*/   
    }
#else
    if ((g_btplay_cur_info.status == BTPLAY_PLAY)
        &&(result != RESULT_BLUETEETH_CALL_BACKGROUND)
        &&(result != RESULT_ENTER_ALARM))

    {
        com_btmanager_avrcp_op(BTPLAY_AVRCP_PAUSE);   
     }

#endif

    kill_app_timer(tick_timer_id);
    tick_timer_id = -1;    
}
void __section__(".bank_2")det_auto_play_flag(void)
{
    if((g_btplay_flag == BTPLAY_PLAY))
    {        
        if((g_customer_state & CUSTOMER_ESD_WAKEUP)!=0)
        {
            g_avrcp_autoplay_cnt = 1;
            #ifdef __ESD_MODE_
            g_avrcp_autoplay_flag = 1;
            libc_print("autoplay_cnt",g_avrcp_autoplay_cnt,2);
            #endif
            
        }
    }
    g_customer_state&= ~CUSTOMER_ESD_WAKEUP;
}


/******************************************************************************/
/*!
 * \par  Description:
 *    MainMenu 应用的入口函数和场景管理器模块
 * \param[in]    PARAM_FROM_XXX
 * \param[out]   none
 * \return       the result
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
app_result_e get_message_loop(void)
{
    app_result_e result = RESULT_NULL;
    
    uint8 cur_hfp_status;
    uint8 cur_a2dp_status;
    uint32 cur_ab_timer;
    bool need_check_status = FALSE;
    bool need_deal_s3bt_back = g_standby_exit_flag;  
    
    message_loop_init();
    
    det_auto_play_flag();
#ifdef __ESD_MODE_
    //g_avrcp_autoplay_flag = 1;
    //g_avrcp_autoplay_cnt = 1;
#endif    
   
    while (1)
    {
        update_sound_mode();
        update_waves_bin_para();
        
        result = bt_status_deal();
        /*if (result > RESULT_COMMON_RESERVE)
         {
         break;
         }*/

        if ((need_deal_s3bt_back == TRUE) && (g_standby_exit_flag == FALSE))
        {
#if (LINK_TTS_SEL == 0)
            g_last_connected_flag = (g_btplay_enter_s3bt_hfp_status != HFP_STATUS_NONE)
                    || (g_btplay_enter_s3bt_a2dp_status != A2DP_STATUS_NONE);
            g_last_btplay_status = g_btplay_enter_s3bt_status;
#else
            g_last_btplay_status = g_btplay_enter_s3bt_status;
#endif

            need_deal_s3bt_back = FALSE;
        }

        cur_ab_timer = sys_get_ab_timer();
    
#ifndef ENABLE_TRUE_WIRELESS_STEREO    
        if ((g_btplay_cur_info.status != g_last_btplay_status) && (g_btplay_cur_info.status >= BTPLAY_STOP))
        {
            g_btplay_check_btplay_status_timer = cur_ab_timer;
            need_check_status = TRUE;
        }   
        else if (g_btplay_cur_info.status == BTPLAY_IDLE)
        {
            need_check_status = FALSE;
        }
        if (need_check_status == TRUE)
        {
             if ((cur_ab_timer - g_btplay_check_btplay_status_timer) > 20000)
             {
                 g_last_btplay_status_before_unlink = g_btplay_cur_info.status;
                  need_check_status = FALSE;
             }
        }
#else
       if( g_last_stack_play_status != g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status)
       {
           g_btplay_check_btplay_status_timer = cur_ab_timer;
           need_check_status = TRUE;
           g_last_stack_play_status=g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status;
           //PRINT_INFO("A1");
       }
       else if(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status<A2DP_STATUS_PLAY)
       {
            g_btplay_check_btplay_status_timer = 0;
            need_check_status = FALSE;
            //PRINT_INFO("A3");
       }
       if (need_check_status == TRUE)
        {
            if ((cur_ab_timer - g_btplay_check_btplay_status_timer) > 20000)
            {
                g_last_btplay_status_before_unlink = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status;
                need_check_status = FALSE;
                //PRINT_INFO("A4");
            }
        }
        if((g_btmanager_gl_var.autoconnect_link_succ_flag == 1) && (g_last_btplay_status_before_unlink == BTPLAY_PAUSE))
        {
            //超时断开回连后，需要保证清除变量
            g_btmanager_gl_var.autoconnect_link_succ_flag=0;
         //   PRINT_INFO("A7");
        }
#endif
        //断开连接后，音箱回连成功后，恢复播放
        if ((g_btmanager_gl_var.autoconnect_link_succ_flag == 1) && (g_last_btplay_status_before_unlink == BTPLAY_PLAY)
                && (g_btplay_cur_info.status != BTPLAY_PLAY) && (g_btplay_autoplay_flag == 0))
        {
            
            g_avrcp_autoplay_cnt = 3;
            g_btplay_autoplay_flag = 1;
            //PRINT_INFO("A5");
        }    
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        else if ((g_btplay_autoplay_flag == 1) && (g_btplay_cur_info.status ==BTPLAY_PLAY)&&(g_auto_btplay_flag==1))
        {
            //PRINT_INFO("A6");
            g_last_btplay_status_before_unlink = BTPLAY_IDLE;
            g_avrcp_autoplay_cnt = 0;
            g_btplay_autoplay_flag = 0;
            g_auto_btplay_flag=0;
        }
#else
        else if ((g_btplay_autoplay_flag == 1) && (g_btplay_cur_info.status ==BTPLAY_PLAY))
        {
            g_last_btplay_status_before_unlink = BTPLAY_IDLE;
            g_avrcp_autoplay_cnt = 0;
            g_btplay_autoplay_flag = 0;
        }
#endif

#if (LINK_TTS_SEL == 0)

#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //单箱下才进入下面
        if(g_bt_stack_cur_info.dev_role==0)
#endif
        {
            cur_hfp_status = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status;
            cur_a2dp_status = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status;
            g_connected_flag = (cur_hfp_status != HFP_STATUS_NONE) || (cur_a2dp_status != A2DP_STATUS_NONE);

            if (g_connected_flag != g_last_connected_flag)
            {
                g_need_tts_play = TRUE;
            }
        }

#endif

        if (g_btplay_cur_info.status != g_last_btplay_status)
        {
            g_need_draw = TRUE;
#if (LINK_TTS_SEL == 1)
            g_need_tts_play = TRUE;
#endif
        }

        if (g_need_draw == TRUE)
        {
            com_view_update(APP_VIEW_ID_MAIN);

            g_need_draw = FALSE;
        }

#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //check_tws_play_tts();
        //单箱下进入该处理
        if(g_bt_stack_cur_info.dev_role==0)
#endif

        {
            if ((g_last_conn_status == CONN_STATUS_NONE) && (g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR))
            {
             //   PRINT_INFO("FE");
                g_need_tts_play = TRUE;
            }
#ifdef ENABLE_TRUE_WIRELESS_STEREO
            //当双箱变成单箱的时候，会与主箱断开副箱的播报冲突，必须加下面的条件避免
            if(dev_role_flag!=g_bt_stack_cur_info.dev_role)
            {
                g_need_tts_play = FALSE;
                //主箱超时断开后，手机连接副箱再断开，副箱不会播报断开连接，但是之后手机再连接断开后会播报，必现
                g_neednot_tts_play_timeout=0;
            }
#endif
            if (g_need_tts_play == TRUE)
            {
                btplay_tts_play();
                g_need_tts_play = FALSE;
            }
        }
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //tws_start_vol_sync();
        dev_role_flag=g_bt_stack_cur_info.dev_role;
        if (g_tws_need_tts_play == TRUE)
        {
            btplay_tws_tts_play();
            g_tws_need_tts_play = FALSE;
        }
        if (g_tws_need_tts_play1 == TRUE)
        {
            btplay_tws_tts_play1();
            g_tws_need_tts_play1 = FALSE;
        }
#endif

#ifdef __ESD_MODE_
        if(g_bt_stack_cur_info.esd_reset_fla == 1 )
        {
            //libc_print("*",0,0);
            ENABLE_WATCH_DOG(1);
            while(1)
            ; 
        }
#endif        
        //自动播放处理
        btplay_autoplay_deal();
        g_last_btplay_status = g_btplay_cur_info.status;
        g_last_conn_status = g_bt_stack_cur_info.conn_status;
#if (LINK_TTS_SEL == 0)
        g_last_connected_flag = g_connected_flag;
#endif
        result = com_view_loop();
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //从箱，只响应退出\关机\低电等特殊消息
        if ( 
        	  (g_bt_stack_cur_info.dev_role==TWS_SLAVE)
        	  && (result != RESULT_APP_QUIT)  //退出
            && (result != RESULT_POWER_OFF) //关机
            && (result != RESULT_LOW_POWER) //低电
            && (result != RESULT_SYSTEM_ENTER_S3) //进入超低功耗模式S3            
           )
        {
            //主箱切换应用,不做处理
            if (g_tws_m_switchap_flag != 1)
            {	
              result = RESULT_NULL;
            }  
        }
#endif
        
        if (result > RESULT_COMMON_RESERVE)
        {
 /*           
            libc_print("result:",result,2);
            if(result==0x108)
            {
                //切换到btcall 打开丢数标志
                send_cmd_to_other(4);
            }
            */
            break;
        }
       
#ifdef SUPPORT_ASET_TEST
        if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
        {
            aset_test_loop_deal();
        }
#endif
        
        //挂起10ms，多任务调度
        sys_os_time_dly(1);
        g_standby_exit_flag = FALSE;
    }

    message_loop_exit(result);
    
    return result;
}

static void sound_mode_handle(void)
{

  com_btmanager_avrcp_op(BTPLAY_AVRCP_PLAY); 
  kill_app_timer(btplay_sound_mode_timer_id);
  
}



static void update_sound_mode(void)
{
    if (1 == sys_comval->dae_cfg.audiopp_type_update_flag)
    {      
        sys_comval->dae_cfg.audiopp_type_update_flag = 0;
        com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, TTS_MUTE);

       // com_btmanager_avrcp_op(BTPLAY_AVRCP_PAUSE);

       // btplay_sound_mode_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,20000, sound_mode_handle);
    }
}

static void update_waves_bin_para(void)
{
    if (1 == g_app_info_state_all.waves_dae_para_update_flag)
    {      
        g_app_info_state_all.waves_dae_para_update_flag = 0;
        com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, TTS_MUTE);
    }
}

