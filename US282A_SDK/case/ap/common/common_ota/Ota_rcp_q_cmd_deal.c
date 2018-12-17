/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：APK/APP OTA接口，包括获取版本号，下载固件，APK查询信息
             重启升级。
 * 作者：lizijian
 ********************************************************************************/
#include "Common_ota.h"
#include "vm_def.h"

/*APK获取小机固件版本号*/
app_result_e ota_get_ver_num(uint32 data1, uint32 data2)
{    

    uint8 tmp_com_rcp_info[sizeof(Fw_Ver_Module_t) + RCP_COMMAND_HEAD_SIZE];
    Fw_Ver_Module_t *p_com_rcp_info = (Fw_Ver_Module_t *) (tmp_com_rcp_info + RCP_COMMAND_HEAD_SIZE);
        
    libc_print("get_ver_num",0,0);
    
    get_ver_message(p_com_rcp_info);
        
    com_rcp_send_pkg(0, 0, tmp_com_rcp_info, sizeof(Fw_Ver_Module_t), sizeof(Fw_Ver_Module_t));

    return RESULT_NULL;
}

/*APK查询握手命令*/
app_result_e ota_enquiry_connect(uint32 data1, uint32 data2)
{
    uint8 tmp_com_rcp_info[sizeof(connect_reply_t) + RCP_COMMAND_HEAD_SIZE];
    connect_reply_t *p_com_rcp_info = (connect_reply_t *) (tmp_com_rcp_info + RCP_COMMAND_HEAD_SIZE);
    ota_bkinfo_t *p_com_rcp_bkinfo = (ota_bkinfo_t *) (tmp_com_rcp_info + RCP_COMMAND_HEAD_SIZE);

    libc_print("receive enquiry connect success or not CMD !!",0,0);

    if(g_OTA_var->g_reply_bkinfo.connect_state==5)
    {
        message_copy(p_com_rcp_bkinfo,2);
        g_OTA_var->g_reply_bkinfo.connect_state = 0;
        com_rcp_send_pkg(0, 0, p_com_rcp_bkinfo, sizeof(ota_bkinfo_t), sizeof(ota_bkinfo_t));

    }
    else
    {
        message_copy(p_com_rcp_info,1);
        com_rcp_send_pkg(0, 0, tmp_com_rcp_info, sizeof(connect_reply_t), sizeof(connect_reply_t));
    }
    

    return RESULT_NULL;
}



/*APK查询小机接受数据状态*/

app_result_e ota_enquiry_message(uint32 data1, uint32 data2)
{
    uint8 tmp_com_rcp_info[sizeof(enquiry_message_t) + RCP_COMMAND_HEAD_SIZE];
    
    enquiry_message_t *p_com_rcp_info = (enquiry_message_t *) (tmp_com_rcp_info + RCP_COMMAND_HEAD_SIZE);

    message_copy(p_com_rcp_info,3);
    if(g_OTA_var->g_send_erro_count>=1)
    {
        g_OTA_var->g_send_erro_count = 0;
        libc_print("g_send_erro_count chanhe to 0",0,0);
        return RESULT_NULL;
    }
    else
    {  
        //sys_os_time_dly(20);
        com_rcp_send_pkg(0, 0, tmp_com_rcp_info, sizeof(enquiry_message_t), sizeof(enquiry_message_t));
    }
    return RESULT_NULL;
}

uint32 message_copy(void *data,uint8 message_type)
{
    if(message_type==1)
    {
        libc_memcpy(data, &g_OTA_var->g_connect_repy, 16);

    }
    else if(message_type==2)
    {
        libc_memcpy(data, &g_OTA_var->g_reply_bkinfo, 16); 
    }
    else
    {
#ifdef        ENABLE_TRUE_WIRELESS_STEREO
        g_OTA_var->g_cur_rec_state.tws_connect_sta = g_bt_stack_cur_info.dev_role ;
        //libc_print("dev_role",g_OTA_var->g_cur_rec_state.tws_connect_sta,2);
#else
        g_OTA_var->g_cur_rec_state.tws_connect_sta = 0;
        //libc_print("dev_role",0,2);
#endif
        libc_memcpy(data, &g_OTA_var->g_cur_rec_state, 14);
    }
}
uint32 get_ver_message(Fw_Ver_Module_t *ver_buf)
{
    //从CONFIG读取case的版本号PID VID
    ver_buf->fw_ver.PID= com_get_config_default(SETTING_CASE_PID);
    ver_buf->fw_ver.VID = com_get_config_default(SETTING_CASE_VID);
    //com_get_config_struct(SETTING_CASE_VER, (void*) ver_buf->Version, 4);
    //sys_get_fw_case_version();
    sys_get_fw_info((void*) ver_buf->fw_ver.Version, FW_INFO_CASE_VERSION_ADDR, FW_INFO_CASE_VERSION_LEN);
    sys_get_fw_info((void*) ver_buf->module, FW_INFO_MODULE_NUM_ADDR, FW_INFO_MODULE_NUM_LEN);
    
    return 1;
}





void reboot_system(void)
{
    uint32 wd_ctl_tmp;

    //设置看门狗，176ms后复位重启
    wd_ctl_tmp = act_readl(WD_CTL);
    wd_ctl_tmp &= 0xffffffd1;//clksel = 176ms
    wd_ctl_tmp |= 0x11;//wden & clear wd timer
    act_writel(wd_ctl_tmp, WD_CTL);
    while (1)
    {
        ;//nothing
    }
}

