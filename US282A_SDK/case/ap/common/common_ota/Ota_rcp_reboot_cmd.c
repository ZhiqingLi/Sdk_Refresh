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


/*APK向小机发送重启升级固件命令*/
app_result_e ota_restart_to_upgrade(uint32 data1, uint32 data2,  void *data, uint16 data_len)
{
    //libc_print("receive restat CMD!",0,0);    
    sys_vm_read(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));   //读VM为了把版本读回来  
    g_ota_restore_data.reboot_flg = 1 ;
    sys_vm_write(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));   //写VM   
    //for ota flag clear 
#ifdef OTA_VERSION    
    {
    otaval_t g_otaval;
    sys_vm_read(&g_otaval, VM_OTA_UPGRADE, sizeof(otaval_t));
    g_otaval.ota_upgrade_flag = 0;    // 清除
    sys_vm_write(&g_otaval, VM_OTA_UPGRADE, sizeof(otaval_t));
  }
#endif 
    reboot_system();
    return RESULT_NULL;
}


/*APK向小机发送重启清空备份区数据*/
app_result_e ota_restart_to_clean_data(uint32 data1, uint32 data2, void *data, uint16 data_len)
{

    uint32 ret ;
    libc_print("receive restat clean_data CMD!",0,0);    
    sys_vm_read(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));
    g_ota_restore_data.updata_statue = OTA_UPDATE_STATE_NULL;
    g_ota_restore_data.dowmload_statue = OTA_DOWNLOAD_STATE_CLEAR;
    g_ota_restore_data.ud_att = 0;
    ret = base_ota_clear_part_table();  
    if(ret == 0)
    {
        libc_print("clear OTA data fail!!",0,0);     
    }  
    sys_vm_write(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));
    
    return RESULT_NULL;

}

/*发送完成命令*/
app_result_e ota_send_complete(uint32 data1, uint32 data2,  void *data, uint16 data_len)
{
    libc_print("send complete",0,0);    
    sys_vm_read(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));   //读VM为了把版本读回来  
    g_ota_restore_data.dowmload_statue = OTA_DOWNLOAD_STATE_END ;
    g_OTA_var->g_cur_rec_state.rec_data_state = RECEIVE_DARA_COMLETE;
    sys_vm_write(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));   //写VM   

    return RESULT_NULL;
}



