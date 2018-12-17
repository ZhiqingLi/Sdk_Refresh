/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙管理模块杂项功能，包括强制断开连接、进入BQB测试、获取配对列表等。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_btmanager.h"
#include "common_rcp.h"
#include "config_id.h"
#include "common_func.h"


//在蓝牙连接状态下，发送关机命令给对方
void com_btmanager_power_off(void)
{
    if((g_bt_stack_cur_info.conn_status == CONN_STATUS_LINKED))
    {
        msg_apps_t msg;
         //消息类型(即消息名称)
        msg.type = MSG_BTSTACK_TWS_POWER_OFF;
        // msg.content.data[0] = 0x01;
        //发送同步消息
        send_sync_msg_btmanager(NULL, &msg, NULL, 0);
    }
}



int com_btmanager_force_unlink(void)
{
    msg_apps_t msg;

    if ((g_app_info_vector[APP_TYPE_BTSTACK].used == 1)
            && ((g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR)
                    || (g_bt_stack_cur_info.conn_status == CONN_STATUS_LINKED)))
    {
        g_bt_auto_connect_ctrl.need_auto_connect = 0x0;

        //消息类型(即消息名称)
        msg.type = MSG_BTSTACK_FORCE_UNLINK_SYNC;
        msg.content.data[0] = 0x00;
        //发送同步消息
        send_sync_msg_btmanager(NULL, &msg, NULL, 0);
    }
    else
    {
        g_ap_switch_var.forbid_auto_connect = TRUE;
    }

    return 0;
}

int com_btmanager_clearpairlist(void)
{
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if ((g_app_info_vector[APP_TYPE_BTSTACK].used == 1)
            && (g_bt_stack_cur_info.conn_status >= CONN_STATUS_WAIT_PAIR))
    {
        msg_apps_t msg;
        //消息类型(即消息名称)
        msg.type = MSG_BTSTACK_CLEAR_TWSPAIR_LIST;
        msg.content.data[0] = 0;
        //发送同步消息
        send_sync_msg_btmanager(NULL, &msg, NULL, 0);
    }
#endif
    return 0;
}

int com_btmanager_BQB_test(void)
{
    msg_apps_t msg;

    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 0)
    {
        return -1;
    }

    if (g_this_app_info->app_id == APP_ID_BTPLAY)
    {
        //消息类型(即消息名称)
        msg.type = MSG_BTSTACK_BQB_TEST_SYNC;
        //发送同步消息
        send_sync_msg_btmanager(NULL, &msg, NULL, 0);
    }

    return 0;
}

int com_btmanager_rmt_dev_set(void)
{
#if 0
    bt_paired_info_t cur_list;
    msg_apps_t msg;

    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 0)
    {
        return -1;
    }

    libc_memset(&cur_list, 0x0, sizeof(bt_paired_info_t));
    msg.type = MSG_BTSTACK_GET_PAIRED_LIST_SYNC;
    msg.content.addr = &cur_list;
    send_sync_msg_btmanager(NULL, &msg, NULL, 0);

    if (cur_list.dev_num > 1)
    {
        //存在多于1个已配对的设备
        //在这里获取到所选择的设备地址，然后用该地址进行操作
    }
#endif
    return RESULT_NULL;
}
