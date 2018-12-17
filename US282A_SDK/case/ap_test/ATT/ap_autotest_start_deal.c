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
#include <sys_info.h>

void read_bt_addr(uint8 *byte_buffer, uint32 read_mode)
{
    int ret_val;
    nvram_param_rw_t param_rw;
    bt_addr_vram_t bt_addr_vram;
    uint32 i;
    
    if(g_support_norflash_wp == FALSE)
    {
        param_rw.logical_index = PARAM_BT_ADDR;
        param_rw.rw_len = 6;
        param_rw.rw_buffer = bt_addr_vram.bt_addr;

        ret_val = base_param_read(&param_rw);

        if (ret_val != 0)
        {
            sys_vm_read(&bt_addr_vram, VM_BTSTACK, sizeof(bt_addr_vram));
        
            if (bt_addr_vram.magic != VRAM_MAGIC(VM_BTSTACK))
            { 
                //没读到蓝牙地址，则设置log文件名为默认文件名
                bt_addr_vram.bt_addr[0] = 0;
                bt_addr_vram.bt_addr[1] = 0;
                bt_addr_vram.bt_addr[2] = 0;
                bt_addr_vram.bt_addr[3] = 0;
                bt_addr_vram.bt_addr[4] = 0;
                bt_addr_vram.bt_addr[5] = 0;                            
            }       
        }        
    }
    else
    {
        sys_vm_read(&bt_addr_vram, VM_BTSTACK, sizeof(bt_addr_vram));
    
        if (bt_addr_vram.magic != VRAM_MAGIC(VM_BTSTACK))
        { 
            param_rw.logical_index = PARAM_BT_ADDR;
            param_rw.rw_len = 6;
            param_rw.rw_buffer = bt_addr_vram.bt_addr;
    
            ret_val = base_param_read(&param_rw);
    
            if (ret_val != 0)
            {
                //没读到蓝牙地址，则设置log文件名为默认文件名
                bt_addr_vram.bt_addr[0] = 0;
                bt_addr_vram.bt_addr[1] = 0;
                bt_addr_vram.bt_addr[2] = 0;
                bt_addr_vram.bt_addr[3] = 0;
                bt_addr_vram.bt_addr[4] = 0;
                bt_addr_vram.bt_addr[5] = 0;                      
            }                
        }            
    }

    if (read_mode == 0)
    {
        for(i = 0; i < 6; i++)
        {
            byte_buffer[i] = bt_addr_vram.bt_addr[5 - i];
        }
    }
    else
    {
        for(i = 0; i < 6; i++)
        {
            byte_buffer[i] = bt_addr_vram.bt_addr[i];
        }        
    }
}

uint32 get_sdk_version(uint8 *sdk_version_buffer)
{
    uint32 sdk_version;

    sdk_version = ((sdk_version_buffer[0] - '0') << 8);
    sdk_version |= ((sdk_version_buffer[2] - '0') << 4);
    sdk_version |= ((sdk_version_buffer[3] - '0'));

    return sdk_version;
}

int32 act_test_start_deal(void)
{
    int32 ret_val;
    uint8 sdk_version[4];
    att_start_arg_t *att_start_arg;
    att_pc_test_info_t *att_pc_test_info;
       
    sys_get_fw_info((void*) sdk_version, FW_INFO_SDK_VERSION_ADDR, FW_INFO_SDK_VERSION_LEN);
    
    if(get_sdk_version(sdk_version) == 0x360)
    {
        g_support_norflash_wp = TRUE;    
    }    
    else
    {
        g_support_norflash_wp = FALSE;    
    }
    
    g_test_base_time = sys_get_ab_timer();
    
    if (g_test_mode == TEST_MODE_CARD) // 卡模式?
        return TRUE;

    att_start_arg = (att_start_arg_t *) STUB_ATT_RW_TEMP_BUFFER;

    att_pc_test_info = (att_pc_test_info_t *) STUB_ATT_RW_TEMP_BUFFER;

    ret_val = att_write_data(STUB_CMD_ATT_READ_TESTINFO, 0, STUB_ATT_RW_TEMP_BUFFER);

    if (ret_val == 0)
    {
        ret_val = att_read_data(STUB_CMD_ATT_ACK, sizeof(pc_test_info_t), STUB_ATT_RW_TEMP_BUFFER);
    }

    //如果上一项是量产测试，则说明是由量产重启，直接进行下一项测试
    //测试成功不发送START命令
    if (att_pc_test_info->pc_test_info.last_test_id == TESTID_PRODUCT_TEST)
    {
        /* 上报量产成功结果
         */
        return_result_t *return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_PRODUCT_TEST;
        return_data->test_result = 1;

        act_test_report_result(return_data, 4);

        return TRUE;
    }

    if (g_att_version == 1)
    {
        if (g_test_ap_info->stub_phy_type == STUB_PHY_USB)
        {
            //测试通过USB线连接，PC与DUT直接连接
            att_start_arg->dut_connect_mode = DUT_CONNECT_MODE_DIRECT;
        }
        else
        {
            //测试通过转接板连接，PC与DUT不直接连接
            att_start_arg->dut_connect_mode = DUT_CONNECT_MODE_UDA;
        }
    }
    else
    {
        att_start_arg->dut_connect_mode = DUT_CONNECT_MODE_UDA;
    }
    
    read_bt_addr(att_start_arg->bdaddr, 0);

    att_start_arg->dut_work_mode = DUT_WORK_MODE_NORMAL;
    //att_start_arg->dut_work_mode = DUT_WORK_MODE_SPECIAL;

    att_start_arg->timeout = 5; // 获取测试项超时
    att_start_arg->reserved = 0;

    ret_val = att_write_data(STUB_CMD_ATT_START, 32, STUB_ATT_RW_TEMP_BUFFER);

    if (ret_val == 0)
    {
        ret_val = att_read_data(STUB_CMD_ATT_ACK, 0, STUB_ATT_RW_TEMP_BUFFER);

        return TRUE;
    }

    return FALSE;
}
