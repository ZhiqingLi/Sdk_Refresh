/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_main_sub.c
 * \brief    开关机的处理
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "ap_config.h"

static const uint8 card_drv_name[] = "card.drv";
static const uint8 uhost_drv_name[] = "uhost.drv";
const uint8 fw_file_name[] =
{
    0xff, 0xfe,
    'U', 0x00,
    'P', 0x00,
    'G', 0x00,
    'R', 0x00,
    'A', 0x00,
    'D', 0x00,
    'E', 0x00,
    '.', 0x00,
    'H', 0x00,
    'E', 0x00,
    'X', 0x00,
    0x00, 0x00
};

const uint8 fwu_file_name[] =
{
    0xff, 0xfe,
    'U', 0x00,
    'P', 0x00,
    'G', 0x00,
    'R', 0x00,
    'A', 0x00,
    'D', 0x00,
    'E', 0x00,
    '.', 0x00,
    'F', 0x00,
    'W', 0x00,
    'U', 0x00,
    0x00, 0x00
};

const uint8 ota_file_name_1[] = 
{
    0xff, 0xfe,
    'U', 0x00,
    'S', 0x00,
    '2', 0x00,
    '8', 0x00,
    '2', 0x00,
    'A', 0x00,
    '_', 0x00,
    'B', 0x00,
    'T', 0x00,
    'B', 0x00,
    'O', 0x00,
    'X', 0x00,
    '1', 0x00,
    '_', 0x00,
    '0', 0x00,
    '0', 0x00,
    '.', 0x00,
    'O', 0x00,  
    'T', 0x00, 
    'A', 0x00,     
    0x00, 0x00

};
const uint8 ota_file_name_2[] = 
{
    0xff, 0xfe,
    'U', 0x00,
    'S', 0x00,
    '2', 0x00,
    '8', 0x00,
    '2', 0x00,
    'A', 0x00,
    '_', 0x00,
    'B', 0x00,
    'T', 0x00,
    'B', 0x00,
    'O', 0x00,
    'X', 0x00,
    '2', 0x00,
    '_', 0x00,
    '0', 0x00,
    '0', 0x00,
    '.', 0x00,
    'O', 0x00,  
    'T', 0x00, 
    'A', 0x00,     
    0x00, 0x00

};

const uint8 fw_ascfile_name[] =
{ "UPGRADE HEX" };
const uint8 fwu_ascfile_name[] =
{ "UPGRADE FWU" };


/******************************************************************************/
/*!
 * \par  Description:
 *  装载物理介质和文件系统
 * \param[in]    disk_type 磁盘类型
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
int32 _config_fs_init(uint8 disk_type)
{
    uint8 drv_type;
    uint8 *drv_name;
    int32 file_sys_id;

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8 *) card_drv_name;
    }
    else if (disk_type == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
        drv_name = (uint8 *) uhost_drv_name;
    }
    else
    {
        return -1;
    }

    //装载物理介质
    if (sys_drv_install(drv_type, 0, drv_name) != 0)
    {
        return -2;
    }

    //装载文件系统
    file_sys_id = sys_mount_fs(drv_type, disk_type, 0);

    if (file_sys_id == -1)
    {
        //卸载驱动程序
        sys_drv_uninstall(drv_type);
        return -3;
    }

    return file_sys_id;
}
/******************************************************************************/
/*!
 * \par  Description:
 *  卸载物理介质和文件系统
 * \param[in]    disk_type 磁盘类型
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
bool _config_fs_deinit(uint8 disk_type, int32 file_sys_id)
{
    int32 ret_val;
    uint8 drv_type;

    if (file_sys_id != -1)
    {
        ret_val = sys_unmount_fs(file_sys_id);
        file_sys_id = -1;
    }
    else
    {
        return TRUE;
    }

    if (0 > ret_val)
    {
        return FALSE;
    }

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
    }
    else if (disk_type == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
    }
    else
    {
        return FALSE;
    }

    ret_val = sys_drv_uninstall(drv_type);

    if (0 > ret_val)
    {
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    转换文件名，将长文件名转化为unicode字符
 * \param[in]    src_name 源文件名缓冲区
 * \param[in]   dest_name 目标文件名缓冲区
 * \return       none
 * \ingroup      common
 * \note
 *******************************************************************************/
void format_long_name(const uint8 * src_name, uint8 * dest_name)
{
    *dest_name = 0xff;
    *(dest_name + 1) = 0xfe;
    dest_name += 2;

    while (*src_name != '\0')
    {
        *dest_name = *src_name;
        *(dest_name + 1) = 0x00;
        dest_name += 2;
        src_name++;
    }

    *dest_name = 0x00;
    *(dest_name + 1) = 0x00;

    return;
}

#if 0
/******************************************************************************/
/*!
 * \par  Description:
 *    加载测试ap文件
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
app_result_e load_ap(int32 file_sys_id, uint8 stg_base)
{
    app_result_e ret_val;
    uint32 ap_fp;
    ap_head_t head_buf;
    int (*start)(int arg0, task_attr_t *task_attr);

    test_ap_info_t test_ap_info;

    //等待PA OK后再进行测试
    config_wait_pa_ok();     

    ap_fp = vfs_file_open(file_sys_id, ap_name, R_NORMAL_SEEK);

    if (ap_fp == 0)
    {
        _config_fs_deinit(stg_base, file_sys_id);
        return RESULT_NULL;
    }

    vfs_file_read(file_sys_id, &head_buf, sizeof(ap_head_t), ap_fp);

    if ((head_buf.text_length) != 0x00)
    {
        vfs_file_seek(file_sys_id, (int32) head_buf.text_offset, SEEK_SET, ap_fp);
        vfs_file_read(file_sys_id, (void *) head_buf.text_addr, head_buf.text_length, ap_fp);
    }

    vfs_file_close(file_sys_id, ap_fp);

    test_ap_info.base_type = stg_base;
    //test_ap_info.key_map = (uint32)g_key_infor.key_val_map;
    test_ap_info.file_sys_id = file_sys_id;
    
    start = (int(*)(int arg0, void *arg1))head_buf.entry;

    //该函数注意卡驱动和文件系统没有卸载
    //在测试ap中会进行卸载，务必确保测试ap卸载相关驱动
    ret_val = start(&test_ap_info, &(head_buf.task_attr));

    g_app_info_state.autotest_mode = TRUE;
    //禁止DAE
    com_set_dae_enable(FALSE);

    sys_os_sched_unlock();

    sys_os_time_dly(10);

    //进入蓝牙音箱应用
    ret_val = RESULT_ENTER_ASQT_HF_TEST;

    return ret_val;
}
#endif
/******************************************************************************/
/*!
 * \par  Description:
 *    检测升级固件是否存在
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/

uint32 check_upgrade_file(int32 file_sys_id)
{
    vfs_cd(file_sys_id, CD_ROOT, 0);
    if ((vfs_file_dir_exist(file_sys_id, fw_file_name, 1) > 0) || (vfs_file_dir_exist(file_sys_id, fwu_file_name, 1)
            > 0) || (vfs_file_dir_exist(file_sys_id, fw_ascfile_name, 1) > 0) || (vfs_file_dir_exist(file_sys_id,
            fwu_ascfile_name, 1) > 0))
    {
        return 1;
    }

    /*if(vfs_file_dir_exist(file_sys_id, ota_file_name_1, 1) > 0&&vfs_file_dir_exist(file_sys_id, ota_file_name_2,1)>0)
    {
        libc_print("ota find", 0, 0);

        return 2;
    }*/
    return 0;

}

extern void config_ota_test(uint32 fp, uint32 file_sys_id) __FAR__;
/******************************************************************************/
/*!
 * \par  Description:
 *  检测是否需要进入测试
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
app_result_e bluetooth_parse_cfg_sub(uint8 base_type)
{
    app_result_e ret_val = RESULT_NULL;
    int32 file_sys_id;
    uint32 fp;
    uint8 version[4];
    //装载主盘
    file_sys_id = _config_fs_init(base_type);
    
    if (file_sys_id < 0)
    {
        ret_val = RESULT_NULL;
    }
    else
    {
        //有upgrade固件
        ret_val = check_upgrade_file(file_sys_id);
        
        if (ret_val == 1)
        {
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
            change_card_state(CARD_STATE_CARD_IN);
#endif            
            _config_fs_deinit(base_type, file_sys_id);
            ret_val = RESULT_ENTER_UPGRADE;
        }
        /*else if(ret_val == 2)
        {
            sys_get_fw_info((void*)version, FW_INFO_CASE_VERSION_ADDR, FW_INFO_CASE_VERSION_LEN);
            if(libc_memcmp(version,"1.00",4) == 0)
            {
                 libc_print("ver_1.00",0,0);
                 fp = vfs_file_open(file_sys_id, ota_file_name_2, R_NORMAL_SEEK);
                 config_ota_test(fp, file_sys_id);
            }
            if(libc_memcmp(version,"2.00",4) == 0)
            {
                 libc_print("ver_2.00",0,0);
                 fp = vfs_file_open(file_sys_id, ota_file_name_1, R_NORMAL_SEEK);
                 config_ota_test(fp, file_sys_id);
            }
            
        }*/
        else
        {          
            ret_val = load_att_code(file_sys_id, base_type);
        }
    }
    return ret_val;
}

/******************************************************************************/
/*!
 * \par  Description:
 *  检测是否需要进入测试
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
app_result_e check_enter_test_mode(void)
{
    app_result_e result = RESULT_NULL;

#if ((CASE_BOARD_TYPE != CASE_BOARD_ATS2823) && (CASE_BOARD_TYPE != CASE_BOARD_DVB_ATS2823))
    if (g_comval.hard_support_card == 1)
    {       
        //检测到卡盘，确认是否进入测试模式或卡盘升级
        if (sys_detect_disk(DRV_GROUP_STG_CARD) == 0)
        {
            result = bluetooth_parse_cfg_sub(DISK_H);
        }
    }       
#endif    

    if (result == RESULT_NULL)
    {
        if (g_comval.hard_support_uhost == 1)
        {
            //检测到U盘，确认是否进入测试模式或U盘升级
            if (sys_detect_disk(DRV_GROUP_STG_UHOST) == 0)
            {
                result = bluetooth_parse_cfg_sub(DISK_U);
            }
        }
    }
    return result;
}
