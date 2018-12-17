/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：RCP 命令分发处理策略接口，客户可二次开发。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/* 应用切换类命令，映射为应用切换消息 */
const msg_apps_type_e rcp_cmd_apswitch_event[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    EVENT_ENTER_BTPLAY, /* 10 蓝牙推歌应用 */
    EVENT_ENTER_MUSIC, /* 11 music应用 */
    EVENT_ENTER_LINEIN, /* 12 linein应用 */
    EVENT_ENTER_RADIO, /* 13 fm应用 */
    EVENT_ENTER_ALARM_RCP, /* 14 闹钟应用 */
    EVENT_ENTER_MUSIC_CARD, /* 15 插卡播歌 */
    EVENT_ENTER_MUSIC_UDISK, /* 16 U盘播歌 */
    EVENT_RECORD_CPLAY_RCP, /* 17 卡录音回放*/
    EVENT_RECORD_UPLAY_RCP, /* 18 U盘录音回放 */
    EVENT_ENTER_CRECORD_RCP, /* 19 卡录音 */
    EVENT_ENTER_URECORD_RCP, /* 20 U盘录音 */
    EVENT_ENTER_USOUND, /* 21 USB音箱 */
};

/* COMMON set/get/control类命令回调函数注册表 */
const rcp_cmd_cb_t com_rcp_cmd_cb_tbl[] =
{
    { RCP_CMD_SETTING, SETTING_ID_SYSTEM_SETTING, (void *) com_set_system_setting }, //系统设置
    { RCP_CMD_SETTING, SETTING_ID_SYSTEM_STATUS, (void *) com_set_system_status }, //APK状态
    { RCP_CMD_SETTING, SETTING_ID_SYSTEM_DIALOG, (void *) com_set_system_dialog }, //对话框提示
    { RCP_CMD_SETTING, SETTING_ID_DEVICE_NAME, (void *) com_set_device_name }, //设置设备名称
    { RCP_CMD_SETTING, SETTING_ID_ALARM_ADD, (void *) time_alarm_set_alarm_add }, //添加或更新闹钟
    { RCP_CMD_SETTING, SETTING_ID_ALARM_DELETE, (void *) time_alarm_set_alarm_delete }, //删除闹钟
    { RCP_CMD_SETTING, SETTING_ID_ALARM_PARAM, (void *) time_alarm_set_alarm_param }, //设置闹钟参数
    { RCP_CMD_SETTING, SETTING_ID_OTA_CONNECT_SETTING, (void *) ota_receive_connect_data }, //设置握手参数
    { RCP_CMD_SETTING, SETTING_ID_OTA_RECIEVE_DATA, (void *) ota_receive_data }, //就收FW数据
    { RCP_CMD_SETTING, SETTING_ID_OTA_RECIEVE_VRAM_DATA, (void *) ota_receive_vram_data }, //就收FW数据
    { RCP_CMD_SETTING, SETTING_ID_SEND_COMPLETE, (void *) ota_send_complete }, //完成发送命令
    { RCP_CMD_SETTING, SETTING_ID_OTA_RESTAT_UD, (void *) ota_restart_to_upgrade }, //开机重启升级        
    { RCP_CMD_SETTING, SETTING_ID_OTA_RESTAT_CD, (void *) ota_restart_to_clean_data }, //开机重启清空数据

    { RCP_CMD_QUERY, QUERY_ID_GLOBAL_RCP_INFO, (void *) com_get_global_rcp_info }, //获取全局状态
    { RCP_CMD_QUERY, QUERY_ID_SUPPORT_FEATURE, (void *) com_get_support_feature }, //获取支持特性列表
    { RCP_CMD_QUERY, QUERY_ID_BT_DEVICE_INFO, (void *) com_get_bt_device_info }, //获取设备信息，包括名称等
    { RCP_CMD_QUERY, QUERY_ID_ALARM_LIST, (void *) time_alarm_get_alarm_list }, //获取闹钟列表
    { RCP_CMD_QUERY, QUERY_ID_RING_LIST, (void *) time_alarm_get_ring_list }, //获取内置闹铃列表
    { RCP_CMD_QUERY, QUERY_ID_RING_FOLDER, (void *) time_alarm_get_ring_folder }, //获取本地磁盘闹铃目录列表
    { RCP_CMD_QUERY, QUERY_ID_ALARM_PARAM, (void *) time_alarm_get_alarm_param }, //获取闹钟参数
    { RCP_CMD_QUERY, QUERY_ID_OTA_VER, (void *) ota_get_ver_num }, //获取小机版本号
    { RCP_CMD_QUERY, QUERY_ID_OTA_RECEIVE_STATE, (void *) ota_enquiry_message }, //获取小机接收数据的状态
    { RCP_CMD_QUERY, QUERY_ID_OTA_CONNECT, (void *) ota_enquiry_connect }, //获取握手状态

    { RCP_CMD_CONTROL, CONTROL_KEY_MUTE, (void *) com_ctrl_switch_mute }, //静音开关
    { RCP_CMD_CONTROL, CONTROL_ALARM_SNOOZE, (void *) alarm_alarm_snooze }, //闹钟SNOOZE
    { RCP_CMD_CONTROL, CONTROL_ALARM_SHUTOFF, (void *) alarm_alarm_close }, //闹钟关闭

    /* 结束项 */
    { 0, 0, NULL },
};

/* 检索get/set/control类命令回调函数 */
void * com_rcp_cmd_find_cb(uint8 cmd_type, uint8 cmd_id)
{
    const rcp_cmd_cb_t *this_rcp_cmd_cb;

    if (g_app_rcp_cmd_cb_tbl != NULL)
    {
        this_rcp_cmd_cb = g_app_rcp_cmd_cb_tbl;
        for (;; this_rcp_cmd_cb++)
        {
            if (this_rcp_cmd_cb->type == 0)
            {
                break;
            }

            if ((this_rcp_cmd_cb->type == cmd_type) && (this_rcp_cmd_cb->id == cmd_id))
            {
                return this_rcp_cmd_cb->cb;
            }
        }
    }

    this_rcp_cmd_cb = com_rcp_cmd_cb_tbl;
    for (;; this_rcp_cmd_cb++)
    {
        if (this_rcp_cmd_cb->type == 0)
        {
            break;
        }

        if ((this_rcp_cmd_cb->type == cmd_type) && (this_rcp_cmd_cb->id == cmd_id))
        {
            return this_rcp_cmd_cb->cb;
        }
    }

    return NULL;
}

/* 切换应用命令回调函数，在响应该命令之前调用 */
void com_apswitch_hook(void)
{
    if (g_ap_switch_var.apk_alarm_flag == 1)
    {
        set_cur_func_id(g_ap_switch_var.apk_alarm_last_func);
        com_rcp_set_callback((const rcp_cmd_cb_t *) g_ap_switch_var.apk_alarm_backup_cb, 
                (rcp_get_func) g_ap_switch_var.apk_alarm_backup_global_cb);
        g_ap_switch_var.apk_alarm_flag = 0;
    }
}

/* 命令分发之前勾子函数，进入RCP命令分发函数时调用 */
void __section__(".rcode") com_rcp_cmd_dispatch_before_hook(void)
{
    //收到APK命令，表示APK available
    g_app_info_state.apk_status = APK_FRONTGROUND;
    g_sys_counter.apk_counter = 0;
}

/* 命令分发之后勾子函数，在退出RCP命令分发函数时调用 */
#if 0
void __section__(".rcode") com_rcp_cmd_dispatch_after_hook(void)
{

}
#endif
