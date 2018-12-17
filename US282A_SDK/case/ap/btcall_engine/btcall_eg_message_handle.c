/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   全局变量
 *      Cailizhen          2015-1          creat
 *       Wekan        2015-3-30       review
 *******************************************************************************/

#include  "btcall_engine.h"

/******************************************************************************
 * \par  Description:   音频源从样机切回手机
 * \param[in]     null0
 * \param[out]   null
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

void force_hfp_to_phone(void)
{
    msg_apps_t msg;
    int i;

    if (g_p_bt_stack_cur_info->rmt_dev[g_p_bt_stack_cur_info->hfp_active_id].hfp_attr.sco_link == 1)
    {
        //断开SCO链路
        msg.type = MSG_BTSTACK_HFP_RELEASESYN_SYNC;
        //发送同步消息
        send_sync_msg_btmanager(NULL, &msg, NULL, 0);

        //等待从音箱端切换回手机端
        for (i = 0; i < 10; i++)
        {
            sys_os_time_dly(1);

            if (g_p_bt_stack_cur_info->rmt_dev[g_p_bt_stack_cur_info->hfp_active_id].hfp_attr.sco_link == 0)
            {
                break;
            }
        }
    }
}

/******************************************************************************
 * \par  Description:   退出解码线程
 * \param[in]     null0
 * \param[out]   null
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

void force_quit_thread(void)
{
    btcall_gl_var.need_exit_thread = TRUE;

    force_hfp_to_phone();

    if (btcall_gl_var.g_sco_deal_flag != 0)
    {
        btcalleg_free_hfp_mmm();
    }
    //等待来电铃声线程退出
    while (g_callring_data.callring_thread_exist != 0)
    {
        sys_os_time_dly(1);
    }
}

/******************************************************************************
 * \par  Description:   播报电话本
 * \param[in]     null0
 * \param[out]   null
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

bool btcall_enter_pb_tts_play(void)
{
    PRINT_DBG("tts pb play\n");
    //防止本地来电铃声尚未置起
    if (btcall_gl_var.callin_ring_mode != CALLIN_RING_MODE_ONLY_PB)
    {
        if (g_callring_data.callring_timer_id != -1)
        {
            kill_app_timer(g_callring_data.callring_timer_id);
            g_callring_data.callring_timer_id = -1;
        }

        if (g_callring_data.callring_thread_exist == 1)
        {
            btcall_gl_var.need_exit_thread = TRUE;
            while (g_callring_data.callring_thread_exist != 0)
            {
                sys_os_time_dly(1);
            }
        }
    }

    if (btcall_gl_var.g_sco_deal_flag == 1)
    { //如果正在通话，先切成CAG模式
      //如果sco链路已存在，先杀掉sco线程
        btcalleg_free_hfp_mmm();
    }

    btcall_gl_var.tts_play_flag = 1;
    btcall_gl_var.tts_pb_play_flag = 1;

    return TRUE;
}

/******************************************************************************
 * \par  Description:   退出电话本播报
 * \param[in]     null0
 * \param[out]   null
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

bool btcall_exit_pb_tts_play(void)
{
    PRINT_DBG("tts pb finish\n");
    btcall_gl_var.tts_play_flag = 0;
    btcall_gl_var.tts_pb_play_flag = 0;
    btcall_gl_var.callin_pb_back = 1;
    if (g_p_bt_stack_cur_info->rmt_dev[g_p_bt_stack_cur_info->hfp_active_id].hfp_status == HFP_STATUS_CALLING)
    {
        bt_engine_hfp_sco_connect_ind_handle();
    }

    return TRUE;
}

/******************************************************************************
 * \par  Description:   回复同步消息
 * \param[in]    void  msg_ptr 消息结构体
 * \param[in]    ret_vals 上一事件的处理结果 TRUE 成功 FALSE 失败
 * \param[out]   none
 * \return           null
 * \note  回复消息时，根据ret_vals的结果，设置应答成功或失败
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

void btcall_reply_msg(void* msg_ptr, bool ret_vals)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    if (ret_vals == TRUE)
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }

    //回复同步消息(发送信号量)
    libc_sem_post(data_ptr->sem);
}

/******************************************************************************
 * \par  Description:   消息初始化
 * \param[in]    void  msg_ptr 消息结构体
 * \param[out]   none
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

app_result_e app_msg_deal_app_quit(void)
{
    force_quit_thread();

    return RESULT_APP_QUIT;
}

/******************************************************************************
 * \par  Description:  处理消息事件
 * \param[in]    pri_msg  私有消息指针
 * \param[out]   none
 * \return           需要顶层处理的事件
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

app_result_e app_message_deal(private_msg_t *pri_msg)
{
    app_result_e app_ret = RESULT_NULL;
    bool ret_val;
    switch (pri_msg->msg.type)
    {
        case MSG_SWITCH_AOUT_START_SYNC: //准备切换音频输出
        {
            ret_val = btcall_enter_pb_tts_play();
            btcall_gl_var.tts_play_flag = 1;
            btcall_reply_msg(pri_msg, ret_val);
        }
        break;

        case MSG_SWITCH_AOUT_STOP_SYNC: //完成切换音频输出
        {
            ret_val = btcall_exit_pb_tts_play();
            btcall_gl_var.tts_play_flag = 0;
            btcall_reply_msg(pri_msg, ret_val);
        }
        break;

        case MSG_TTS_START_SYNC: //准备进入TTS播报
        {
            bool pb_play_flag = pri_msg->msg.content.data[0];

            if (pb_play_flag == TRUE)
            {
                ret_val = btcall_enter_pb_tts_play();
            }
            else
            {
                PRINT_DBG("tts play\n");

                if ((g_p_btcall_info->status != BTCALL_IDLE)
                        && (g_p_btcall_info->status != BTCALL_STOP))
                {
                    //不允许TTS播报
                    ret_val = FALSE;
                }
                else
                {
                    btcall_gl_var.tts_play_flag = 1;
                    ret_val = TRUE;
                }
            }

            btcall_reply_msg(pri_msg, ret_val);
        }
        break;

        case MSG_TTS_STOP_SYNC: //TTS播报完成
        {
            bool pb_play_flag = pri_msg->msg.content.data[0];

            PRINT_DBG("tts finish\n");

            if (pb_play_flag == TRUE)
            {
                ret_val = btcall_exit_pb_tts_play();
            }
            else
            {
                btcall_gl_var.tts_play_flag = 0;
                ret_val = TRUE;
            }

            btcall_reply_msg(pri_msg, ret_val);
        }
        break;

        case MSG_ENGINE_STANDBY_SYNC:
        {
            os_event_t *standby_sem = (os_event_t *)(pri_msg->msg.content.addr);

            //备份现场，准备进入睡眠模式
            PRINT_INFO("engine standby enter");

            //回复前台，然后接着跑就会睡眠
            btcall_reply_msg(pri_msg, TRUE);

            libc_sem_wait(standby_sem, 0);

            //从睡眠模式回来，恢复现场
            PRINT_INFO("engine standby exit");
            standby_restart_all_app_timer(g_app_timer_vector, g_app_timer_count);
        }
        break;

        case MSG_APP_QUIT: //退出引擎
        PRINT_DBG("quit btcall engine\n");
        if ((pri_msg->msg.content.data[0] & 0x01) != 0)
        {
            PRINT_INFO("eg enter s3bt");
        }
        app_ret = app_msg_deal_app_quit();
        break;

        //通话远端静音
        case MSG_BTCALLEG_MIC_MUTE_SYNC:
        btcall_gl_var.g_mic_mute_flag = TRUE;
        g_dsp_init_parm.pcm_mode = 2;//上传静音数据
        libc_print("btcall-set-mic-mute.",0,0);
        btcall_reply_msg(pri_msg, TRUE);
        break;

        //通话远端恢复正常
        case MSG_BTCALLEG_MIC_RELEASEMUTE_SYNC:
        btcall_gl_var.g_mic_mute_flag = FALSE;
        g_dsp_init_parm.pcm_mode = 0;
        libc_print("btcall-set-mic-ok.",0,0);
        btcall_gl_var.g_actaec_reset_flag = TRUE;
        btcall_reply_msg(pri_msg, TRUE);
        break;


        case  MSG_BTCALLEG_GET_EG_VOLUM_SYNC:
        {
           uint32 *pu32 = (uint32*)pri_msg->reply->content;
           //*pu32 = g_asqt_parm.AsqtCfg.pa_volume;
           *pu32 = g_asqt_parm.asqt_volume;
           btcall_reply_msg(pri_msg, TRUE);
        }
        break;


        case MSG_BTCALLEG_GET_EG_PA_VOLUM_SYNC:
        {
           uint32 *pu32 = (uint32*)pri_msg->reply->content;
           *pu32 = g_asqt_parm.AsqtCfg.pa_volume;
           //*pu32 = g_asqt_parm.asqt_volume;
           btcall_reply_msg(pri_msg, TRUE);
        }
        break;

        default:
        btcall_reply_msg(pri_msg, TRUE);
        break;
    }

    return app_ret;
}
