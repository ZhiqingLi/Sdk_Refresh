/*******************************************************************************
 *                              US212A
 *                            Module: Picture
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-19 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     user1_main.c
 * \brief    picture主模块，负责进程初始化，退出处理，场景调度
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include <ap_manager_test.h>

#define BT_PLAY_TEST_RETRY_CNT  8

uint8 g_connect_cnt;
test_btstack_status_t g_bt_status;

void test_get_btstack_status(test_btstack_status_t *bt_status)
{
    msg_apps_t msg;

    msg.type = MSG_AUTOTEST_GET_BTSTACK_STATUS_SYNC;
    msg.content.addr = bt_status;

    send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 0);

    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \  清除后台蓝牙的错误状态
 * \param[in]    none
 * \param[out]   none
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 * \li
 */
/*******************************************************************************/
void bt_clear_error(void)
{
    msg_apps_t msg;
    msg.type = MSG_AUTOTEST_CLEAR_BTSTACK_ERR_SYNC;
    send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 0);
    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \  向后台发送消息要求建立A2DP信道
 * \param[in]    none
 * \param[out]   none
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 * \li    由于发射机确定不会主动连接小机，因此连接都是小机通过回连方式进行的
 */
/*******************************************************************************/
void connect_source(void)
{
    msg_apps_t msg;
    msg_reply_t temp_reply;

    //if (g_bt_status.num_connected == 0)
    {
        //if ((g_bt_status.support_profile & (uint8) HFP_SUPPORTED) != 0)
        {
            //消息类型(即消息名称)
            //msg.type = MSG_AUTOTEST_HFP_CONNECT_SYNC;
            //发送同步消息
            //send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, &temp_reply, 0);
        }
        //else
        {
            msg.type = MSG_AUTOTEST_A2DP_CONNECT_SYNC;
            send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, &temp_reply, 0);
        }

        g_connect_cnt++;
    }

    return;
}


bool test_bstack_status_deal(btplay_test_arg_t *btplay_test_arg, bt_paired_dev_info2_t *bt_paired_dev2, uint32 *ret_val)
{
    bool loop_exit = FALSE;
    uint32 status_bak;

    status_bak = g_bt_status.conn_status;

    test_get_btstack_status(&g_bt_status);

    //DEBUG_ATT_PRINT("bt status", g_bt_status.conn_status, 2);

    //DEBUG_ATT_PRINT("err status", g_bt_status.err_status, 2);

    //DEBUG_ATT_PRINT("hfp status", g_bt_status.hfp_status, 2);

    //DEBUG_ATT_PRINT("support profile", g_bt_status.support_profile, 2);

    if (g_bt_status.conn_status == TEST_CONN_STATUS_ERROR)
    {
        //硬件出错，立即退出，并设置测试失败
        if (g_bt_status.err_status == TEST_BT_ERR_HW)
        {
            loop_exit = TRUE;
        }
        else if (g_bt_status.err_status == TEST_BT_ERR_PAGE_TIMEOUT)
        {
            //链路超时，重试一段时间
            if (g_connect_cnt < BT_PLAY_TEST_RETRY_CNT)
            {
                bt_clear_error();

                connect_source();
                tick_cnt = 0;
            }
            else
            {
                loop_exit = TRUE;
            }

        }
        else
        {
            ;//nothing for QAC
        }
    }
    else if (g_bt_status.conn_status == TEST_CONN_STATUS_WAIT_PAIR)
    {
        if (g_connect_cnt == 0)
        {
            connect_source();
        }
    }
    else if (g_bt_status.conn_status == TEST_CONN_STATUS_LINKED)
    {
        *ret_val = TRUE;
        if ((status_bak != g_bt_status.conn_status) && (btplay_test_arg->bt_fast_mode == TRUE))
        {
            test_btstack_get_paired_dev_info(bt_paired_dev2);               
        }
        
        //连接性测试，发现连接成功立即返回
        if (btplay_test_arg->bt_test_mode == 0)
        {    
            loop_exit = TRUE;
        }
    }
    else
    {
        ;//nothing for QAC
    }

    return loop_exit;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \  蓝牙状态机处理
 * \param[in]    none
 * \param[out]   none
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 * \li
 */
/*******************************************************************************/
bool bt_connect_proc(void)
{
    bool loop_exit = FALSE;
    //if ((g_disp_status == STATUS_A2DP_PAUSE) || (g_disp_status == STATUS_A2DP_PLAY))
    //{
    //    //如果当前已经为A2DP播放或暂停状态，时间到结束蓝牙模组测试
    //    quit_app(TRUE);
    //    return TRUE;
    //}
    //else
    if ((g_bt_status.conn_status == TEST_CONN_STATUS_WAIT_PAIR) || (g_bt_status.conn_status == TEST_CONN_STATUS_NONE))
    {
        //回连次数控制
        if (g_connect_cnt < BT_PLAY_TEST_RETRY_CNT)
        {
            connect_source();
        }
        else
        {
            loop_exit = TRUE;
        }
    }
    //else if (g_disp_status == STATUS_LINKED)
    //{
    //    if(p_test_arg->play_time < 10)
    //    {
    //        //一般情况下不会出现该状态，因为配对后立即会进入A2DP，如果出现，则由于逻辑链路建立
    //        //成功，也认为测试成功
    //        quit_app(TRUE);
    //        return TRUE;
    //    }
    //}
    else
    {
        ;//nothing
    }
    return loop_exit;
}

static void test_bt_force_unlink(void)
{
    msg_apps_t msg;

    //消息类型(即消息名称)
    msg.type = MSG_BTSTACK_FORCE_UNLINK_SYNC;
    msg.content.data[0] = 0x00;

    send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 0);
}

static void switch_app(uint32 app_func_id)
{
    msg_apps_t msg;
        
    g_app_func_id = app_func_id;
    
    //消息类型(即消息名称)
    msg.type = MSG_AUTOTEST_SWITCH_APP;
    msg.content.addr = &g_app_func_id;

    send_async_msg(MSG_TARGET_ID_FRONTAPP, &msg);    
}

uint32 test_bt_manager_loop_deal(btplay_test_arg_t *btplay_test_arg, bt_paired_dev_info2_t *bt_paired_dev2)
{
    uint8 loop_cnt = 0;
    uint32 timer_id;
    uint32 ret_val;
    uint32 total_tick = 100; //100*100ms = 10s
    uint8 loop_exit = FALSE;
    uint32 cur_time;

    g_connect_cnt = 0;

    timer_id = tick_ISR_install();

    ret_val = FALSE;

    cur_time = sys_get_ab_timer();

    while (1)
    {
        if (loop_cnt == 50)
        {
            loop_exit = test_bstack_status_deal(btplay_test_arg, bt_paired_dev2, &ret_val);
            loop_cnt = 0;
            
            if (loop_exit == TRUE)
            {
                switch_app(APP_ATT_FUNC_ID_BTPLAY);
            }            
        }

        if (loop_exit == TRUE && g_att_version == 0)
        {
            break;
        }  

        //测试80s自动退出
        if ((sys_get_ab_timer() - cur_time) > 30000)
        {
            if (ret_val == TRUE)
            {
                switch_app(APP_ATT_FUNC_ID_BTPLAY);
            }     
            else
            {
                break;
            }
        }

        if (g_test_share_info.ap_switch_flag == TRUE)
        {
            if(g_att_version == 0)
            {
                sys_os_sched_lock();
            }
            
            libc_print("ap quit, test over", 0, 0);
            g_test_share_info.ap_switch_flag = FALSE;
            break;
        }

        if (tick_cnt >= total_tick)
        {
            tick_cnt = 0;

            loop_exit = bt_connect_proc();

            if (loop_exit == TRUE)
            {
                break;
            }

        }
        loop_cnt++;
        sys_os_time_dly(1);
    }

    tick_ISR_uninstall(timer_id);
   
    if (ret_val == FALSE)
    {       
        att_write_test_info("btplay test failed", 0, 0);
    }
    else
    {        
        att_write_test_info("btplay test ok", 0, 0);
    }
    
    return ret_val;
}

void act_test_btstack_install(btplay_test_arg_t *btplay_test_arg)
{
    int i;

    bool ret;

    msg_apps_t msg;

    bt_paired_dev_info_t bt_paired_dev;

    bt_paired_dev_info2_t bt_paired_dev2;

    while (1)
    {
        if (g_test_share_info.front_ap_id == APP_ID_BTCALL)
        {
            g_test_share_info.ap_switch_flag = FALSE;
            break;
        }

        sys_os_time_dly(1);
    }

    while (1)
    {
        DEBUG_ATT_PRINT("send install msg", 0, 0);

        //等待BTSTACK加载
        msg.type = MSG_AUTOTEST_QUERY_BTSTACK_WORK_SYNC;

        ret = send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 100);

        if (ret == TRUE)
        {
            break;
        }

        //sys_os_time_dly(1);
    }

    //DEBUG_ATT_PRINT("btstack install", 0, 0);

    //if ((g_att_version == 1) && (g_support_norflash_wp == TRUE))
   // {
   //     btplay_test_arg->bt_fast_mode = TRUE;    
   // }
   // else
    {
        btplay_test_arg->bt_fast_mode = FALSE;
    }

    if(btplay_test_arg->bt_fast_mode == FALSE)
    {
retry:
        for (i = 0; i < 6; i++)
        {
            bt_paired_dev.remote_addr[i] = btplay_test_arg->bt_transmitter_addr[i];
            //DEBUG_ATT_PRINT("remote addr: ", bt_paired_dev.remote_addr[i], 2);
        }

        bt_paired_dev.support_profile = (uint8)(A2DP_SUPPORTED | LINKKEY_VALID | PROFILE_VALID);
        
        msg.type = MSG_AUTOTEST_SET_PAIRED_DEV_SYNC;
        msg.content.addr = &bt_paired_dev;
    }
    else
    {
        for (i = 0; i < 6; i++)
        {
            bt_paired_dev2.remote_addr[i] = btplay_test_arg->bt_transmitter_addr[i];
            //DEBUG_ATT_PRINT("remote addr: ", bt_paired_dev2.remote_addr[i], 2);
        }

        ret = test_btstack_read_paired_dev_info(&bt_paired_dev2);

        //配对列表不存在，使用之前的连接方式
        if(ret == FALSE)
        {
            DEBUG_ATT_PRINT("no found pair list ", 0, 0);
            goto retry;    
        }
    
        msg.type = MSG_AUTOTEST_SET_PAIRED_DEV2_SYNC;
        msg.content.addr = &bt_paired_dev2;
    }
    send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 0);

    //DEBUG_ATT_PRINT("set pair list", 0, 0);

}

test_result_e act_test_bt_test(void *arg_buffer)
{
    int32 ret_val;
    return_result_t *return_data;
    uint16 trans_bytes = 0;

    bt_paired_dev_info2_t bt_paired_dev2;

    libc_memset(&bt_paired_dev2, 0, sizeof(bt_paired_dev_info2_t));

    btplay_test_arg_t *btplay_test_arg = (btplay_test_arg_t *) arg_buffer;
    
    if (g_att_version == 1)
    {
        /* config 应用继续运行并启动其它程序;
         */
        g_p_test_ap_info_bak->test_stage = 1;
    }

    sys_os_sched_unlock(); 
    
    act_test_btstack_install(btplay_test_arg);
    
    if (g_test_mode == TEST_MODE_CARD) 
    {
        vfs_file_close(g_file_sys_id, g_test_file_handle);
        
        _config_fs_deinit(DISK_H, &g_file_sys_id);    
    }     

    ret_val = test_bt_manager_loop_deal(btplay_test_arg, &bt_paired_dev2);
    
    if (g_test_mode == TEST_MODE_CARD) 
    {
        g_file_sys_id = _config_fs_init(DISK_H);  
        
        g_test_file_handle = vfs_file_open(g_file_sys_id, g_ap_name, R_NORMAL_SEEK);  
    } 

    if ((btplay_test_arg->bt_fast_mode == TRUE) && (ret_val == TRUE))
    {
        test_btstack_save_paired_dev_info(&bt_paired_dev2);    
    }

    if (g_test_mode != TEST_MODE_CARD)
    { 
        if (g_att_version == 0)
        {
           stub_ioctrl_set(SWITCH_URAM, STUB_USE_URAM1, 0);    
        }
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_BT_TEST;

        return_data->test_result = ret_val;

        bytes_to_unicode(btplay_test_arg->bt_transmitter_addr, 5, 6, return_data->return_arg, &trans_bytes);

        //添加参数分隔符','
        return_data->return_arg[trans_bytes++] = 0x002c;

        bytes_to_unicode(&(btplay_test_arg->bt_test_mode), 0, 1, &(return_data->return_arg[trans_bytes]), &trans_bytes);

        //添加参数分隔符','
        return_data->return_arg[trans_bytes++] = 0x002c;

        bytes_to_unicode(&(btplay_test_arg->bt_fast_mode), 0, 1, &(return_data->return_arg[trans_bytes]), &trans_bytes);

        //添加结束符
        return_data->return_arg[trans_bytes++] = 0x0000;

        //如果参数未四字节对齐，要四字节对齐处理
        if ((trans_bytes % 2) != 0)
        {
            return_data->return_arg[trans_bytes++] = 0x0000;
        }
       
        act_test_report_result(return_data, trans_bytes * 2 + 4);
    }
    else
    {
        act_test_report_test_log(ret_val, TESTID_BT_TEST);
    }

    sys_os_sched_lock();
    
    return ret_val;
}




