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
#include "ap_manager_test.h"

static const uint8 card_drv_name[] = "card.drv";
static const uint8 uhost_drv_name[] = "uhost.drv";
static const uint8 g_ap_name[] =
{
    0xff, 0xfe,
    'a', 0x00,
    'c', 0x00,
    't', 0x00,
    'i', 0x00,
    'o', 0x00,
    'n', 0x00,
    's', 0x00,
    't', 0x00,
    'e', 0x00,
    's', 0x00,
    't', 0x00,
    '.', 0x00,
    'a', 0x00,
    't', 0x00,
    'f', 0x00,
    0x00, 0x00
};

//装载物理介质和文件系统
int32 music_test_fs_init(uint8 disk_type, uint8 drv_mode)
{
    uint8 drv_type;
    uint8 *drv_name;
    int32 vfs_mount = -1;

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8*) card_drv_name;
    }
    else if (disk_type == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
        drv_name = (uint8*) uhost_drv_name;
    }
    else
    {
        return -1;
    }

    //安装存储驱动
    if (sys_drv_install(drv_type, (uint32) drv_mode, drv_name) != 0)
    {
        return -1;
    }

    //挂载FS
    vfs_mount = sys_mount_fs(drv_type, disk_type, 0);

    if (vfs_mount == -1)
    {
        sys_drv_uninstall(drv_type);
    }

    return vfs_mount;
}

//卸载物理介质和文件系统
bool music_test_fs_exit(uint8 disk_type, int32 file_sys_id)
{
    int32 ret_val = 0;

    uint8 drv_type;

    if (file_sys_id != -1)
    {
        ret_val = sys_unmount_fs(file_sys_id);
        file_sys_id = -1;
        if (ret_val == 0)
        {
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
        }
    }
    if (0 > ret_val)
    {
        return FALSE;
    }

    return TRUE;
}

void act_test_report_music_test_result(uint16 test_id, uint32 ret_val)
{
    return_result_t *return_data;
    uint16 trans_bytes = 0;

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RW_TEMP_BUFFER);
        return_data->test_id = test_id;
        return_data->test_result = ret_val;

        act_test_report_result(return_data, 4);
    }
    else
    {
        act_test_report_test_log(ret_val, test_id);
    }
}

test_result_e act_test_uhost_play_test(void *arg_buffer)
{
    // uint32 ra = (uint32)test_dispatch;
    uint32  ret_val;
    int32 fs;

    DEBUG_ATT_PRINT("start uhost test", 0, 0);

    if (g_test_mode == TEST_MODE_CARD)
    {
        fs = music_test_fs_exit(DISK_H, 0);
    }
    
    fs = music_test_fs_init(DISK_U, 0);

    if (g_test_mode == TEST_MODE_CARD)
    {
        DEBUG_ATT_PRINT("uhost test FS:", fs, 2);
    }
    else
    {
        print_log("uhost test FS: %d", fs);
    }
    
    if (fs != -1)
    {
        ret_val = 1;
        att_write_test_info("uhost test ok", 0, 0);
    }
    else
    {
        ret_val = 0;
        att_write_test_info("uhost test failed", 0, 0);
    }
    
    music_test_fs_exit(DISK_U, fs);  
    
    if (g_test_mode == TEST_MODE_CARD)
    {
        music_test_fs_init(DISK_H, 0);
        g_test_file_handle = vfs_file_open(g_file_sys_id, g_ap_name, R_NORMAL_SEEK);
    }

    act_test_report_music_test_result(TESTID_UHOST_TEST, ret_val);    

    return TEST_PASS;
}

test_result_e act_test_sdcard_play_test(void *arg_buffer)
{
    uint32  ret_val;
    int32 fs;

    DEBUG_ATT_PRINT("start sdcard test", 0, 0);

    if (g_test_mode == TEST_MODE_CARD)
    {
        act_test_report_music_test_result(TESTID_SDCARD_TEST, 1); 

        return TEST_PASS;
    }
    
    fs = music_test_fs_init(DISK_H, 0);

    print_log("sdcard test FS: %d", fs);
    
    if (fs != -1)
    {
        ret_val = 1;
        print_log("sdcard test ok");
    }
    else
    {
        ret_val = 0;
        print_log("sdcard test failed");
    }
    
    music_test_fs_exit(DISK_H, fs);  
    
    act_test_report_music_test_result(TESTID_SDCARD_TEST, ret_val);
    
    DEBUG_ATT_PRINT("uhost test end ", 0, 0);

    return TEST_PASS;
}


test_result_e act_test_linein_play_test(void *arg_buffer)
{
    test_result_e result = TEST_PASS;

    return result;
}

