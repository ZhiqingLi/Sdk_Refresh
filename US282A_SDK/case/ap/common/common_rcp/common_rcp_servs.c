/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙数传SPP/BLE相关应用层接口，相当于BT STACK的SPP/BLE的适配层。
 * 作者：fiona
 ********************************************************************************/

#include "applib.h"
#include "common_func.h"

//#if 1 // __SUPPORT_ANCS_

uint8 g_ancs_data_buf[16];

uint8 com_ancs_GetNotificationAttributes(uint8 * ntf_uuid)
{
    uint8 i;

    g_ancs_data_buf[0] = 0;
    for (i = 0; i < 4; i++)
    {
        g_ancs_data_buf[i + 1] = ntf_uuid[i];
    }
    g_ancs_data_buf[5] = 1; // attribute id

    g_ancs_data_buf[6] = 2; // len
    g_ancs_data_buf[7] = 3;

    g_ancs_data_buf[8] = 4; // attribute id

    g_ancs_data_buf[9] = 5; // len
    g_ancs_data_buf[10] = 6;

    com_rcp_ble_serv_send(g_ancs_data_buf, 11, RCP_ANCS_CTRL_POINT_FLAG);

    return TRUE;
}

app_result_e ancs_notification_cb(void *data, uint16 data_len, uint8 serv_flag)
{
    uint8 i = 0, *cmd_buf = NULL;
    uint32 ntf_uuid = 0;

    //PRINT_INFO("==com_ancs_callback==   Notification ");    
    if (data != NULL)
    {
        cmd_buf = (uint8 *) data;
        ntf_uuid = cmd_buf[4] | (cmd_buf[5] << 8) | (cmd_buf[6] << 16) | (cmd_buf[7] << 24);
        //PRINT_INFO_INT("Event ID	  :", cmd_buf[0]);
        //PRINT_INFO_INT("Event Flag	  :", cmd_buf[1]);
        //PRINT_INFO_INT("Category ID   :", cmd_buf[2]);
        //PRINT_INFO_INT("Category Count:", cmd_buf[3]);
        //PRINT_INFO_INT("Notification UUID:", ntf_uuid);
        //com_start_key_tone(KEYTONE_NOBLOCK);

        com_ancs_GetNotificationAttributes(&cmd_buf[4]);
    }
    libc_print("==com_ancs_cb==  Notify",ntf_uuid, 2);
    PRINT_DATA(cmd_buf, data_len);
    return RESULT_NULL;
}

app_result_e ancs_data_cb(void *data, uint16 data_len, uint8 serv_flag)
{
    uint8 i = 0, *cmd_buf;

    //PRINT_INFO("==com_ancs_callback==   Data ");
    libc_print("==com_ancs_cb==  Data", 0, 0);
    if (data != NULL)
    {
        cmd_buf = (uint8 *) data;
        //PRINT_DATA(data, data_len);
    }
    
    return RESULT_NULL;
}

void com_ancs_init(void)
{
    com_rcp_set_ext_cb(ancs_notification_cb, RCP_ANCS_SERVN_FLAG);
    com_rcp_set_ext_cb(ancs_data_cb, RCP_ANCS_SERVD_FLAG);
}

//#endif	// __SUPPORT_ANCS_

//for tip use

app_result_e tip_recv_cb(void *data, uint16 data_len, uint8 serv_flag)
{
    //PRINT_INFO("tip time info");
    libc_print("tip time info", 0, 0);
    return RESULT_NULL;
}

void com_tip_init(void)
{
    com_rcp_set_ext_cb(tip_recv_cb, RCP_TIP_SERV_FLAG);
}
