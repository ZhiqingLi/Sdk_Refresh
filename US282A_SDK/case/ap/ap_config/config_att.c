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
#include "stub_interface.h"
#include "stub_command.h"
#include "att_interface.h"

static uint8 att_temp_buffer[512] _BANK_DATA_ATTR_;

static const uint8 g_ap_name[] =
{
    0xff, 0xfe,
    'a',  0x00,
    'c',  0x00,
    't',  0x00,
    'i',  0x00,
    'o',  0x00,
    'n',  0x00,
    's',  0x00,
    't',  0x00,
    'e',  0x00,
    's',  0x00,
    't',  0x00,
    '.',  0x00,
    'a',  0x00,
    't',  0x00,
    'f',  0x00,
    0x00, 0x00
};

app_result_e load_att_code(int32 file_sys_id, uint8 stg_base)
{
    int i;
    atf_head_t *atf_head;
    ap_head_t *head_buf;
    uint32 test_file_offset;
    int (*start)(int arg0, task_attr_t *task_attr);
    test_ap_info_t test_ap_info;
    uint32 ap_fp;

    //等待PA OK后再进行测试
    config_wait_pa_ok();

    head_buf = (ap_head_t *) att_temp_buffer;

    libc_memset(&test_ap_info, 0, sizeof(test_ap_info_t));

    if (stg_base == DISK_H)
    {
        ap_fp = vfs_file_open(file_sys_id, g_ap_name, R_NORMAL_SEEK);

        if (ap_fp == 0)
        {
            libc_print("atf file open err", 0, 0);
            _config_fs_deinit(stg_base, file_sys_id);
            return RESULT_NULL;
        }
        
        //进入调试模式不开启ESD功能，清除之前记忆的AP类型
        //del  act_writel(act_readl(RTC_BAK0) & (~(0xff << MY_RTC_FUNC_INDEX)), RTC_BAK0);
    
        //rtc register
        //del  act_writel(0xA596, RTC_REGUPDATA);
        //del  while (act_readl(RTC_REGUPDATA) != 0x5A69)
        {
            ;//wait for register update
        }     
    
        //del  act_writel(act_readl(RTC_BAK0) | (APP_FUNC_INVALID << MY_RTC_FUNC_INDEX), RTC_BAK0);
    
        //rtc register
        //del  act_writel(0xA596, RTC_REGUPDATA);
        //del  while (act_readl(RTC_REGUPDATA) != 0x5A69)
        {
            ;//wait for register update
        }    
        
        g_config_var.esd_flag = FALSE;
        
        sys_vm_write(&g_config_var, VM_AP_CONFIG, sizeof(g_config_var));   

        sys_disable_use_temp_pool();
        
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
        change_card_state(CARD_STATE_CARD_IN);
#endif

        vfs_file_read(file_sys_id, att_temp_buffer, 512, ap_fp);

        atf_head = (atf_head_t *) att_temp_buffer;

        for (i = 0; i < 15; i++)
        {
            if (libc_memcmp(atf_head->atf_dir[i].filename, "acttest.ap", 10) == 0)
            {
                test_ap_info.ap_file_offset = atf_head->atf_dir[i].offset;
            }

            if (libc_memcmp(atf_head->atf_dir[i].filename, "config.txt", 10) == 0)
            {
                test_ap_info.cfg_file_offset = atf_head->atf_dir[i].offset;
                test_ap_info.cfg_file_len = atf_head->atf_dir[i].length;
            }
        }

        test_file_offset = test_ap_info.ap_file_offset;

        vfs_file_seek(file_sys_id, test_file_offset, SEEK_SET, ap_fp);
        vfs_file_read(file_sys_id, head_buf, sizeof(ap_head_t), ap_fp);

        if ((head_buf->text_length) != 0x00)
        {
            vfs_file_seek(file_sys_id, head_buf->text_offset + test_file_offset, SEEK_SET, ap_fp);
            vfs_file_read(file_sys_id, head_buf->text_addr, head_buf->text_length, ap_fp);
        }

    }
    else
    {
        //host
        _config_fs_deinit(stg_base, file_sys_id);
        return RESULT_NULL;
    }

    test_ap_info.info_size   = sizeof(test_ap_info_t);
    test_ap_info.test_stage  = 0;
    test_ap_info.base_type   = stg_base;
    test_ap_info.stub_phy_type = g_app_info_state.stub_phy_type;

    start = (int(*)(int arg0, void *arg1)) head_buf->entry;

    //该函数注意卡驱动和文件系统没有卸载
    //在测试ap中会进行卸载，务必确保测试ap卸载相关驱动
    start(&test_ap_info, &(head_buf->task_attr));

    g_app_info_state.autotest_mode = TRUE;

    sys_os_sched_unlock();

    do  // config 应用挂起等待直到测试阶段 1;
    {
        sys_os_time_dly(10);
    }
    while (test_ap_info.test_stage == 0);
    
    g_app_info_state.stub_tools_type = STUB_PC_TOOL_ATT_MODE;

    return RESULT_ENTER_ASQT_HF_TEST;
}

