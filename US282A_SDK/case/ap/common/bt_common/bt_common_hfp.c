/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙HFP相关应用层接口，相当于BT STACK的HFP PROFILE的适配层。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_func.h"
#include "common_btmanager.h"
#include "hfp_event_code.h"

static const enum HFP_APP_EventCodeEnum s_hfp_ev_codes[] =
{
    HFP_EV_ANSWER_CALL_REQ, /* 0 BTCALL_HFP_ANSWER_CALL*/
    HFP_EV_CANCEL_CALL_REQ, /* 1 BTCALL_HFP_REJECT_CALL*/
    HFP_EV_CANCEL_CALL_REQ, /* 2 BTCALL_HFP_HANGUP_CALL*/
};

int com_btmanager_hfp_call_req(btcall_hfp_req_e req)
{
    msg_apps_t msg;

    if (req >= BTCALL_HFP_MAX_REQ)
    {
        return -1;
    }

    //消息类型(即消息名称)
    msg.type = MSG_BTSTACK_HFP_CALL_REQ_SYNC;
    msg.content.data[0] = s_hfp_ev_codes[req];

    //发送同步消息
    send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].addr.bytes, &msg, NULL, 0);

    return 0;
}

int com_btmanager_hfp_call_last(void)
{
    msg_apps_t msg;
    uint32 cur_timestamp;

    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 0)
    {
        return 0;
    }

    cur_timestamp = sys_get_ab_timer();
    if ((cur_timestamp - g_btmanager_gl_var.last_call_last_timestamp) < 2000)
    {
        PRINT_INFO_INT("ignore call last:", cur_timestamp - g_btmanager_gl_var.last_call_last_timestamp);
        return 0;
    }

    if (g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status == HFP_STATUS_LINKED)
    {
        //消息类型(即消息名称)
        msg.type = MSG_BTSTACK_HFP_CALL_LAST_SYNC;
        //发送同步消息
        send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].addr.bytes,
            &msg, NULL, 0);
        
        g_btmanager_gl_var.last_call_last_timestamp = cur_timestamp;
    }

    return 0;
}

int com_btmanager_hfp_siri_handle(void)
{
    msg_apps_t msg;
    uint32 cur_timestamp;

    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 0)
    {
        return 0;
    }
#if (__SUPPORT_SIRI_ == 1)
    cur_timestamp = sys_get_ab_timer();
    if ((cur_timestamp - g_btmanager_gl_var.last_call_last_timestamp) < 2000)
    {
        //PRINT_INFO_INT("ignore call last:", cur_timestamp - g_btmanager_gl_var.last_call_last_timestamp);
        return 0;
    }

    //if (g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status == HFP_STATUS_LINKED)
    if(g_btmanager_gl_var.enable_siri!=0)
    {
        //消息类型(即消息名称)
        msg.type = MSG_BTSTACK_HFP_SET_SIRI_SYNC;
        //发送同步消息
        send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].addr.bytes,
                &msg, NULL, 0);

        g_btmanager_gl_var.last_call_last_timestamp = cur_timestamp;
    }
#endif //#if (__SUPPORT_SIRI_ == 1)
    return 0;
}

app_result_e com_btmanager_hfp_set_volume_to_soundbox(uint8 vol, bool tts_flag)
{
    app_result_e result = RESULT_NULL;
    uint8 tmp_volume;

    if (g_this_app_info->app_id == APP_ID_BTCALL)
    {
        tmp_volume = vol;
        tmp_volume = (tmp_volume * VOLUME_VALUE_MAX / 15);

        PRINT_INFO_INT("hfp set vol:", vol);
        if (tts_flag == FALSE)
        {
            com_volume_set(tmp_volume, SET_VOLUME_VIEW | SET_VOLUME_TTS);
        }
        else
        {
            com_volume_set(tmp_volume, SET_VOLUME_TTS);
        }
    }

    return result;
}

int com_btmanager_hfp_update_volume_to_phone(uint8 vol)
{
    msg_apps_t msg;
    uint16 tmp_volume;

    if (g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status == HFP_STATUS_NONE)
    {
        return 0;
    }

    tmp_volume = (uint16) (vol * 15) / VOLUME_VALUE_MAX;

    msg.type = MSG_BTSTACK_HFP_VOLUME_SYNC;
    msg.content.data[0] = (uint8) tmp_volume;
    send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].addr.bytes, &msg, NULL, 0);

    return 0;
}

void com_btmanager_hfp_battery_report_update_handle(void)
{
    com_btmanager_hfp_battery_report_update(g_app_info_state.bat_value);

    g_btmanager_gl_var.bat_rpt_timer_id = -1;
}

int com_btmanager_hfp_battery_report_enable(uint8 *bd)
{
    msg_apps_t msg;

    msg.type = MSG_BTSTACK_HFP_BATTERY_SYNC;
    msg.content.data[0] = BATTERY_REPORT_CMD_ID_ENABLE;
    send_sync_msg_btmanager(bd, &msg, NULL, 0);

    return 0;
}

//应用bat_value分为0-9级
int com_btmanager_hfp_battery_report_update(uint8 bat_value)
{
    msg_apps_t msg;
    uint8 tmp_bat;
    uint8 i;

    tmp_bat = bat_value * (BATTERY_REPORT_MAX - BATTERY_REPORT_MIN) / (BATTERY_GRADE_MAX - 0) + BATTERY_REPORT_MIN;

    msg.type = MSG_BTSTACK_HFP_BATTERY_SYNC;
    msg.content.data[0] = BATTERY_REPORT_CMD_ID_UPDATE;
    msg.content.data[1] = tmp_bat;

    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (g_bt_stack_cur_info.rmt_dev[i].hfp_attr.batt_supported == 1)
        {
            send_sync_msg_btmanager(g_bt_stack_cur_info.rmt_dev[i].addr.bytes, &msg, NULL, 0);
        }
    }
    return 0;
}
