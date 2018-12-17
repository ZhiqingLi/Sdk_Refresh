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
#include <ap_manager_test.h>
#include "ap_autotest_mptest.h"


typedef void (*p_func_entry)(void);

//卸载物理介质和文件系统
static bool music_test_fs_exit(uint8 disk_type, int32 file_sys_id)
{
    int32 ret_val = 0;

    uint8 drv_type;

    if(file_sys_id != -1) {
        ret_val = sys_unmount_fs(file_sys_id);
        file_sys_id = -1;
        if(ret_val == 0) {
            if(disk_type == DISK_H) {
                drv_type = DRV_GROUP_STG_CARD;
            } else if(disk_type == DISK_U) {
                drv_type = DRV_GROUP_STG_UHOST;
            } else {
                return FALSE;
            }

            ret_val = sys_drv_uninstall(drv_type);
        }
    }
    if(0 > ret_val) {
        return FALSE;
    }

    return TRUE;
}

void card_adfu_launcher(void)
{
    uint32 i;

    p_func_entry p_card_adfu_launcher;

    print_log("enter card launcher...");

    p_card_adfu_launcher = *(uint32 *)0xbfc002b0;

    sys_local_irq_save();

    sys_unlock_adjust_freq();

    sys_adjust_clk(26, 0);

    act_writel(0, INTC_CFG0);
    act_writel(0, INTC_CFG1);
    act_writel(0, INTC_CFG2); //assgin usb intrrupt to IP2

    act_writel(0x58, WD_CTL);

    for(i = 0; i < 0x100; i++) {
        ;//nothing for QAC
    }

    p_card_adfu_launcher = *(uint32 *)0xbfc002b4;

    //完成数据的初始化
    p_card_adfu_launcher();

    p_card_adfu_launcher = *(uint32 *)0xbfc002b0;

    //跳转真正的函数入口
    p_card_adfu_launcher();
}

void act_test_flashtest(void *arg_buffer)
{
    return_result_t *return_data;

    //该测试项必须卡中量产固件才可以测试成功
    if(sys_detect_disk(DRV_GROUP_STG_CARD) != -1) {
        card_adfu_launcher();
    }

    if(g_test_mode != TEST_MODE_CARD) {
        return_data = (return_result_t *)(STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_FLASHTEST;

        return_data->test_result = 0;

        act_test_report_result(return_data, 4);
    } else {
        act_test_report_test_log(0, TESTID_FLASHTEST);
    }
}

test_result_e act_test_enter_ft_mode(void *arg_buffer)
{
    return_result_t *return_data;

    if(g_test_mode != TEST_MODE_CARD) {
        return_data = (return_result_t *)(STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_FTMODE;

        return_data->test_result = TRUE;

        act_test_report_result(return_data, 4);

        act_test_read_testid(arg_buffer, 80);
    }

    sys_local_irq_save();

    //A21 digital function
    act_writel((act_readl(AD_Select) & 0xfffffffc), AD_Select);

    //A22 digital function
    act_writel((act_readl(AD_Select1) & 0xfffffff9), AD_Select);

    act_writel(act_readl(GPIOAOUTEN) | (1 << 21), GPIOAOUTEN);

    act_writel(act_readl(GPIOAOUTEN) | (1 << 22), GPIOAOUTEN);

    act_writel(act_readl(GPIOADAT) & (~(1 << 21)), GPIOADAT);

    act_writel(act_readl(GPIOADAT) & (~(1 << 22)), GPIOADAT);

    act_writel((act_readl(VD15_DCDC_CTL) & 0xffffe7ff), VD15_DCDC_CTL);

    DEBUG_ATT_PRINT("FT MODE", 0, 0);

    //FT mode
    sys_mdelay(20);
    sys_mdelay(10);
    act_writel(act_readl(0xc01b0000) | 0x00000014, 0xc01b0000); //ft test mode
    sys_mdelay(20);
    act_writel((act_readl(WD_CTL) & (~0x10)), WD_CTL);
    while(1) {
        ;
    }
}

static void switch_app(uint32 app_func_id)
{
    msg_apps_t msg;

    g_app_func_id = app_func_id;

    //消息类型(即消息名称)
    msg.type = MSG_AUTOTEST_SWITCH_APP;
    msg.content.addr = &g_app_func_id;

    send_async_msg(MSG_TARGET_ID_FRONTAPP, &msg);
}


test_result_e act_test_enter_BQB_mode(void *arg_buffer)
{
    int i;

    bool ret;

    msg_apps_t msg;

    return_result_t *return_data;

    if(g_att_version == 1) {
        /* config 应用继续运行并启动其它程序;
         */
        g_p_test_ap_info_bak->test_stage = 1;
    }

    sys_os_sched_unlock();

    while(1) {
        switch_app(APP_ATT_FUNC_ID_BTPLAY);

        if(g_test_share_info.front_ap_id == APP_ID_BTPLAY) {
            g_test_share_info.ap_switch_flag = FALSE;
            break;
        }

        sys_os_time_dly(5);
    }
#if 0
    while(1) {
        DEBUG_ATT_PRINT("send install msg", 0, 0);

        //等待BTSTACK加载
        msg.type = MSG_AUTOTEST_QUERY_BTSTACK_WORK_SYNC;

        ret = send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 100);

        if(ret == TRUE) {
            break;
        }

        //sys_os_time_dly(1);
    }
#endif
    DEBUG_ATT_PRINT("btstack install", 0, 0);

    if(g_test_mode != TEST_MODE_CARD) {
        return_data = (return_result_t *)(STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_BQBMODE;

        return_data->test_result = TRUE;

        act_test_report_result(return_data, 4);

        act_test_read_testid(arg_buffer, 80);
    }

    //消息类型(即消息名称)
    msg.type = MSG_BTSTACK_BQB_TEST_SYNC;

    //发送同步消息
    send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 0);

}

static int32 att_index_to_cap(uint32 index)
{
    uint8 bc_val;
    uint8 tc_val;
    uint16 cap_val;
    int32 ret_val;

    bc_val = (index >> 5);

    switch(bc_val) {
        case CAP_INDEX_0:
            bc_val = 0;
            break;

        case CAP_INDEX_1:
            bc_val = 1;
            break;

        case CAP_INDEX_2:
            bc_val = 2;
            break;

        case CAP_INDEX_3:
            bc_val = 3;
            break;

        case CAP_INDEX_4:
            bc_val = 6;
            break;

        case CAP_INDEX_5:
            bc_val = 7;
            break;

        default:
            bc_val = 1;
            break;
    }

    tc_val = (index & 0x1f);

    cap_val = ((bc_val << 5) | tc_val);

    cap_val = (cap_val | (cap_val << 8));

    return cap_val;
}

int read_trim_cap_efuse(uint32 mode)
{
    uint32 ret_val;
    uint32 trim_cap_value;

    ret_val = base_trim_cap_read(&trim_cap_value, mode);

    if(ret_val == TRIM_CAP_READ_NO_ERROR) {
        ;//读取到正确的频偏数值
    } else {
        trim_cap_value = -1;
    }

    return trim_cap_value;
}

static int write_trim_cap_efuse(int value, uint32 mode)
{
    uint32 ret_val;
    int value_bak;

    ret_val = base_trim_cap_write(&value, mode);

    if(ret_val == TRIM_CAP_WRITE_NO_ERROR) {
        //重新读取频偏值，看二者是否相等
        ret_val = base_trim_cap_read(&value_bak, mode);

        if(ret_val == TRIM_CAP_READ_NO_ERROR) {
            if(value_bak == value) {
                return TRUE;
            }
        }
        return FALSE;
    } else {
        if(ret_val == TRIM_CAP_WRITE_ERR_NO_RESOURSE) {
            //efuse已经写满
            print_log("efuse write over!", 0, 0);
        }

        return FALSE;
    }
}

uint32 att_write_trim_cap(uint16 index, uint32 mode)
{
    uint32 ret_val;

    uint32 read_cap_value;

    uint32 write_cap_value;

    write_cap_value = att_index_to_cap(index);

    read_cap_value = read_trim_cap_efuse(mode);

    if(read_cap_value != write_cap_value) {
        //g_efuse_value = cap_value;
        print_log("write efuse: %x index %d\n", write_cap_value, index);
        ret_val = write_trim_cap_efuse(write_cap_value, mode);

        if(ret_val == FALSE) {
            return ret_val;
        }

        //设置系统频偏
        sys_set_hosc_param(write_cap_value);
    }

    return TRUE;
}


