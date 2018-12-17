/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   响应通话中的操作，处理消息事件
 *      <author>       <time>
 *       Wekan   2015-3-27
 *******************************************************************************/

#include  "ap_btcall.h"

/******************************************************************************
 * \par  Description:   获取蓝牙后台引擎的状态
 * \param[in]    none
 * \param[out]   bg_info 引擎状态指针
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

bool get_engine_status(void)
{
    if (sys_share_query_read(APP_ID_BTCALLEG, &g_btcall_cur_info) == -1)
    {
        // rcode 空间不足， 关闭打印
        //PRINT_ERR("btcall share query not exist!\n");
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************
 * \par  Description: 根据后台蓝牙的状态进行处理
 * \param[in]    none
 * \param[out]   none
 * \return       需app响应的事件
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/
app_result_e bt_status_deal(void)
{
    app_result_e result = RESULT_NULL;

    get_engine_status();

    if (g_callin_ring_mode != CALLIN_RING_MODE_NORMAL)
    {
        if (g_btcall_cur_info.status == BTCALL_CALLIN)
        {
            if (g_call_phone_num == FALSE)
            {
                g_pb_result = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_attr.pb_result;
                if (g_pb_result != PB_RESULT_NULL)
                {
                    g_call_phone_num = TRUE;

                    //处理电话本播报内容
                    if (btcall_pb_tts_init() == FALSE)
                    {
                        g_pb_result = PB_RESULT_NULL;
                    }
                }
            }
        }
        else
        {
            g_call_phone_num = FALSE;
        }
    }

    if ((g_btcall_cur_info.status == BTCALL_HFP) || (g_btcall_cur_info.status == BTCALL_CALLIN)
            || (g_btcall_cur_info.status == BTCALL_CALLOUT) || (g_btcall_cur_info.status == BTCALL_PHONE)
            || (g_btcall_cur_info.status == BTCALL_SIRI))
    {
        g_state_call_phone = TRUE;
    }
    else
    {
        g_state_call_phone = FALSE;
    }

    return result;
}

/******************************************************************************
 * \par  Description: app线程处理消息事件
 * \param[in]    none
 * \param[out]   none
 * \return       需app响应的事件
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

app_result_e get_message_loop(void)
{
    app_result_e result = RESULT_NULL;
    int pb_play_cnt = (int) g_callin_ring_space_time * 100;//秒转换为10ms
    bool last_state_call_phone = FALSE;
    conn_status_e last_conn_status = CONN_STATUS_NONE;

    //等待BTCALL ENGINE状态可用
    while (get_engine_status() == FALSE)
    {
        sys_os_time_dly(1);
    }

    g_last_btcall_status = g_btcall_cur_info.status;

    //如果刚开始还未连接，强制播报一次等待连接
    if (g_btcall_cur_info.status == BTCALL_IDLE)
    {
        g_need_tts_play = TRUE;
    }

    //如果当前已经不是IDLE状态，强制刷新一次
    if (g_btcall_cur_info.status != BTCALL_IDLE)
    {
        g_need_draw = TRUE;
    }

    last_conn_status = g_bt_stack_cur_info.conn_status;

    while (1)
    {
        result = bt_status_deal();
        if (result > RESULT_COMMON_RESERVE)
        {
            break;
        }
        if ((g_btcall_cur_info.status != g_last_btcall_status) && (g_state_call_phone == TRUE))
        {
            // PRINT_INFO_INT("g_btcall_cur_info", g_btcall_cur_info.status);
            //  PRINT_INFO_INT("hfp_status:", g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status);
            g_need_draw = TRUE;
            g_need_tts_play = TRUE;
        }

        if (g_need_draw == TRUE)
        {
            com_view_update(APP_VIEW_ID_MAIN);

            g_need_draw = FALSE;
        }

        if ((last_conn_status == CONN_STATUS_NONE) && (g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR))
        {
            g_need_tts_play = TRUE;
        }

        if (g_need_tts_play == TRUE)
        {
            //后台方式，不播报断开和连接的tts
            // if (g_background_call_flag == FALSE)
            {
                btcall_tts_play();
            }

            g_need_tts_play = FALSE;
        }

        g_last_btcall_status = g_btcall_cur_info.status;
        last_conn_status = g_bt_stack_cur_info.conn_status;

        //循环播报电话本信息，每隔1s播报一次
        if (g_callin_ring_mode != CALLIN_RING_MODE_NORMAL)
        {
            if ((g_call_phone_num == TRUE) && (g_pb_result != PB_RESULT_NULL))
            {
                if (pb_play_cnt == ((int) g_callin_ring_space_time * 100)) //秒转换为10ms
                {
                    btcall_pb_tts_play();
                    pb_play_cnt = 0;
                }
                else
                {
                    pb_play_cnt++;
                }
            }
        }

        if (g_background_call_flag == TRUE)
        {
            if ((g_btcall_cur_info.status == BTCALL_IDLE) || (g_btcall_cur_info.status == BTCALL_STOP))
            {
                result = RESULT_LASTPLAY;
                break;
            }
        }
        else
        {
            if ((last_state_call_phone == FALSE) && (g_state_call_phone == TRUE))
            {
                volume_cfg(TRUE);
            }
            else if ((last_state_call_phone == TRUE) && (g_state_call_phone == FALSE))
            {
                volume_cfg(FALSE);
            }
            else
            {
                ;//nothing
            }
            get_eg_asqt_vol();
            get_eg_asqt_PA();
            last_state_call_phone = g_state_call_phone;
        }

        result = com_view_loop();

        //电话状态，只响应退出\关机\低电等特殊消息
        if (g_state_call_phone == TRUE)
        {
            if ((result != RESULT_APP_QUIT) //退出
                    && (result != RESULT_POWER_OFF) //关机
                    && (result != RESULT_LOW_POWER) //低电
            )
            {
                if (result > RESULT_COMMON_RESERVE)
                {
                    PRINT_INFO_INT("btcall ignore retult:", result);
                }
                result = RESULT_NULL;
            }
        }

        if (result > RESULT_COMMON_RESERVE)
        {
            break;
        }

        //挂起10ms，多任务调度
        sys_os_time_dly(1);
        g_standby_exit_flag = FALSE;
    }

    return result;
}
