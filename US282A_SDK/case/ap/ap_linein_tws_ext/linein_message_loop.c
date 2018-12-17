/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌前台应用业务逻辑主循环实现。
 * 作者：
 ********************************************************************************/

#include  "ap_linein.h"

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
    if (sys_share_query_read(APP_ID_LINEIN_EG, &g_btplay_cur_info) == -1)
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
    //check_tws_play_tts();
    tws_start_vol_sync();
   if(g_tts_play_info.status==TTS_STATUS_IDLE)
   {
        //libc_print("volume_2",sys_comval->volume_current,2);  
        tws_volume=sys_comval->volume_current;
   }
   linein_check_disconn_deal();
#endif
}

#ifdef ENABLE_TRUE_WIRELESS_STEREO
void __section__(".text.autoplay") tws_vol_sync(void)
{

    uint8 i;
    uint16 vol;
    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (g_bt_stack_cur_info.rmt_dev[i].dev_type == TWS_DEV)
        {
            break;
        }
    }

    vol = (uint16) ((tws_volume * 0x7f) / VOLUME_VALUE_MAX);
    msg_apps_t msg;

    if(g_btmanager_gl_var.enable_avrcp_volume_sync!=0)
    {
        msg.type = MSG_BTSTACK_AVRCP_VOLUME_SYNC;
        msg.content.data[0] = vol;
    }
    else
    {
        msg.type = MSG_BTSTACK_TWS_SEND_MSG_SYNC;
        msg.content.data[0] = vol;

    }
    //    libc_print("vol:",vol,2);
    send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[i].addr.bytes, &msg, NULL, 0);
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

void __section__(".bank_2") message_loop_init(void)
{
    //等待BTPLAY ENGINE状态可用
    while (get_engine_status() == FALSE)
    {
        sys_os_time_dly(1);
    }

    g_last_btplay_status = g_btplay_cur_info.status;
    
    g_last_dev_role = g_bt_stack_cur_info.dev_role; 

    //如果当前已经不是IDLE状态，强制刷新一次
    if (g_btplay_cur_info.status != BTPLAY_IDLE)
    {
        g_need_draw = TRUE;
    }

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

        //if(g_bt_stack_cur_info.dev_role != 0)
        if(g_bt_stack_cur_info.dev_role == 1)
        {

            if ((result != RESULT_BLUETEETH_CALL_BACKGROUND)
            	  && (result != RESULT_ENTER_ALARM))
            {
                uint8 val_tmp = 1;
                if(g_bt_stack_cur_info.dev_role == 1)
                {
                    val_tmp = 0xf1;
                }
                tws_sync_deal(val_tmp);
                PRINT_INFO("a2");
            }
            //由于现在主箱切出应用,从箱不一起切出应用
            //所以主箱切出应用时,通知从箱卸载中间件
            //else
            {
                //line tws模式下,切进电话
                //通知从卸载中间件
                msg_apps_t msg;
                msg.content.data[0]=1;
                msg.type = MSG_BTSTACK_DECODE_QUIT_SYNC;
                send_sync_msg_btmanager(NULL, &msg, NULL, 0);
            }

        }

    }
#endif

    kill_app_timer(tick_timer_id);
    tick_timer_id = -1;
}

void check_tws_play_tts(void)
{

    if (flag_pair != g_bt_stack_cur_info.pair)
    {
        if (g_bt_stack_cur_info.pair == 1)
        {
            g_tws_need_tts_play = TRUE;
            if (flag_no_tts == 1)
            {
                flag_no_tts = 0;
            }
            else
            {
                flag2 = 1;
                uint8 val_tmp = 2;
                if (g_bt_stack_cur_info.dev_role == 1)
                {
                    val_tmp = 0xf2;
                }
                tws_sync_deal(val_tmp);
                g_app_info_state_all.g_send_exit_mmm_tts = 1;
            }
            // flag2=1;
        }
        //调试对箱显示 ，主副箱连上就显示BU
        g_need_draw = TRUE;

    }
    flag_pair = g_bt_stack_cur_info.pair;
    //主箱断开副箱
    if ((flag_dis_tws_dev != g_bt_stack_cur_info.tws_dev_discon) && (g_bt_stack_cur_info.tws_dev_discon == 1))
    {
        g_app_info_state_all.g_send_exit_mmm_tts = 1;
        g_tws_need_tts_play = TRUE;
        //   PRINT_INFO(" G");
    }
    flag_dis_tws_dev = g_bt_stack_cur_info.tws_dev_discon;
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

    bool need_deal_s3bt_back = g_standby_exit_flag;

    message_loop_init();

    if ((g_btplay_flag == BTPLAY_PAUSE))
    {
        if ((g_customer_state & CUSTOMER_ESD_WAKEUP) != 0)
        {
            linein_send_msg(NULL, NULL, MSG_LINEIN_EG_STOP_SYNC);
            //            g_avrcp_autoplay_cnt = 1;
        }
    }

    g_customer_state &= ~CUSTOMER_ESD_WAKEUP;

    while (1)
    {
        update_sound_mode();
        update_waves_bin_para();
        
        result = bt_status_deal();
        /*if (result > RESULT_COMMON_RESERVE)
         {
         break;
         }*/

        if ((g_btplay_cur_info.status != g_last_btplay_status)
        	  || (g_last_dev_role != g_bt_stack_cur_info.dev_role))
        {
            g_need_draw = TRUE;

        }
        
        g_last_dev_role = g_bt_stack_cur_info.dev_role;

        if (g_need_draw == TRUE)
        {
            com_view_update(APP_VIEW_ID_MAIN);

            g_need_draw = FALSE;
        }

#ifdef ENABLE_TRUE_WIRELESS_STEREO
        check_tws_play_tts();
#endif

#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //tws_start_vol_sync();

        if (g_tws_need_tts_play == TRUE)
        {
            linein_tws_tts_play();
            g_tws_need_tts_play = FALSE;
        }

#endif

        g_last_btplay_status = g_btplay_cur_info.status;

        result = com_view_loop();
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //从箱，只响应退出\关机\低电等特殊消息
        if ( 
        	  (g_bt_stack_cur_info.dev_role==TWS_SLAVE)
        	  && (result != RESULT_APP_QUIT)  //退出
            && (result != RESULT_POWER_OFF) //关机
            && (result != RESULT_LOW_POWER) //低电
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

            libc_print("result:", result, 2);
            /*            if(result==0x108)
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


#ifdef __ESD_MODE_
        if(g_bt_stack_cur_info.esd_reset_fla == 1 )
        {
            libc_print("*",0,0);
            ENABLE_WATCH_DOG(1);
            while(1)
            ; 
        }
#endif 

        //挂起10ms，多任务调度
        sys_os_time_dly(1);
        g_standby_exit_flag = FALSE;
    }

    message_loop_exit(result);
    //exit recover
    linein_recover_conn_deal();

    return result;
}

static void sound_mode_handle(void)
{

    linein_send_msg(NULL, NULL, MSG_LINEIN_EG_PLAY_SYNC);
    kill_app_timer(btplay_sound_mode_timer_id);

}

static void update_sound_mode(void)
{
    if (1 == sys_comval->dae_cfg.audiopp_type_update_flag)
    {
        sys_comval->dae_cfg.audiopp_type_update_flag = 0;

        com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_BT_CON_OK);
        //linein_send_msg(NULL, NULL, MSG_LINEIN_EG_STOP_SYNC);

        //btplay_sound_mode_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL, 1000, sound_mode_handle);
    }
}

static void update_waves_bin_para(void)
{
    if (1 == g_app_info_state_all.waves_dae_para_update_flag)
    {
        g_app_info_state_all.waves_dae_para_update_flag = 0;

        com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_MUTE);
        //linein_send_msg(NULL, NULL, MSG_LINEIN_EG_STOP_SYNC);

        //btplay_sound_mode_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL, 1000, sound_mode_handle);
    }
}


