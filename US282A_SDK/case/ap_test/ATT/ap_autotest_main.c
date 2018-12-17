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

int32 g_file_sys_id = 0;
uint8 test_stg_type = DISK_H;
os_event_t *thread_mutex;
uint32 tick_cnt;
uint8 g_test_mode;
uint8 g_read_code_mode;
test_share_info_t g_test_share_info;
test_ap_info_t *g_test_ap_info;
test_ap_old_info_t *g_old_test_info;
uint32 g_test_file_handle;
uint8 g_cur_line_num;
uint32 g_write_file_len;
uint32 g_epc_addr[2];
uint8 g_skip_product_test;
uint8 g_att_version;
uint32 g_app_func_id;
uint8 g_support_norflash_wp;
autotest_test_info_t g_test_info;
test_ap_info_t g_r_test_ap_info;
test_ap_old_info_t g_r_old_test_info;

test_ap_info_t *g_p_test_ap_info_bak;

uint32 g_test_base_time;

//uint8 att_cmd_temp_buffer[64] __attribute__((aligned(4)));

const uint8 g_ap_name[] =
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

uint32 att_write_data(uint16 cmd, uint32 payload_len, uint32 data_addr)
{
    int32 ret_val;
    stub_ext_param_t ext_param;

    if((payload_len + 8) == 64)
    {
        payload_len += 4;
    }

    ext_param.opcode = cmd;
    ext_param.payload_len = payload_len;
    ext_param.rw_buffer = (uint8 *) data_addr;

    ret_val = stub_ext_write(&ext_param);

    return ret_val;
}

uint32 att_read_data(uint16 cmd, uint32 payload_len, uint32 data_addr)
{
    int32 ret_val;
    stub_ext_param_t ext_param;

    ext_param.opcode = cmd;
    ext_param.payload_len = payload_len;
    ext_param.rw_buffer = (uint8 *) data_addr;

    ret_val = stub_ext_read(&ext_param);

    return ret_val;

}

uint32 att_test_fread(uint8 *buffer, uint32 len, uint32 offset)
{
    int ret_val;
    int read_len;
    uint32 loop_exit;
    uint8 cmd_data[16];
    att_fread_arg_t *att_fread_arg;

    uint32 true_offset = offset;

    stub_set_debug_mode(0);

    //stub_ioctrl_set(SET_READ_MODE, 0, 0);

    while (len > 0)
    {
        if (len > STUB_ONCE_BANK_READ_SIZE)
        {
            read_len = STUB_ONCE_BANK_READ_SIZE;
        }
        else
        {
            //保证读取的数据是4字节对齐
            read_len = ((len + 3) / 4) * 4;
        }

        att_fread_arg = (att_fread_arg_t *) (cmd_data);
        att_fread_arg->lengthl = read_len & 0xffff;
        att_fread_arg->lengthh = (read_len >> 16);
        att_fread_arg->offsetl = true_offset & 0xffff;
        att_fread_arg->offseth = (true_offset >> 16);

        ret_val = att_write_data(STUB_CMD_ATT_FREAD, 8, cmd_data);

        if (ret_val == 0)
        {
            ret_val = att_read_data(STUB_CMD_ATT_FREAD, read_len, STUB_ATT_READ_BANK_BUFFER);

            if (ret_val == 0)
            {
                if (read_len == STUB_ONCE_BANK_READ_SIZE)
                {
                    libc_memcpy(buffer, (uint8 *) (STUB_ATT_READ_BANK_BUFFER + sizeof(stub_ext_cmd_t)), read_len);
                }
                else
                {
                    //拷贝实际长度
                    libc_memcpy(buffer, (uint8 *) (STUB_ATT_READ_BANK_BUFFER + sizeof(stub_ext_cmd_t)), len);

                    read_len = len;
                }

                //回复ACK
                ret_val = att_write_data(STUB_CMD_ATT_ACK, 0, cmd_data);
            }
        }

        if (ret_val == 0)
        {
            buffer += read_len;
            len -= read_len;
            true_offset += read_len;
        }
    }

    //stub_ioctrl_set(SET_READ_MODE, 1, 0);

    stub_set_debug_mode(1);

    return ret_val;
}

uint32 load_ap_bank(uint32 epc)
{
    uint32 sd_file_handle;
    ap_bank_head_t ap_bank_head;
    uint32 read_file_offset;
    uint32 bank_no;
    uint32 epc_mask;
    uint32 ap_offset;

    bank_no = ((epc & 0xfc0000) >> 18);

    epc_mask = (epc & (~SRAM_SIZE_MASK));

    read_file_offset = 0x60;

    read_file_offset += sizeof(ap_bank_head_t) * bank_no;

    //DEBUG_ATT_PRINT("  load_ap_bank:", epc, 2);
    // DEBUG_ATT_PRINT("  bank_no:", bank_no, 2);

    if (g_att_version == 0)
    {
        ap_offset = g_old_test_info->ap_file_offset;
    }
    else
    {
        ap_offset = g_test_ap_info->ap_file_offset;
    }
    if (bank_no < 50)
    {
        if (g_epc_addr[0] == epc_mask)
        {
            act_writel(epc_mask + 0x40000000, PageAddr3);

            return TRUE;
        }
    }
    else
    {
        if (g_epc_addr[1] == epc_mask)
        {
            act_writel(epc_mask + 0x40000000, PageAddr3);

            return TRUE;
        }
    }

    if (g_read_code_mode == TRUE)
    {
        //if (g_file_sys_id == -1)
        //{
        //    g_file_sys_id = _config_fs_init(test_stg_type);
        //}

        if (g_file_sys_id < 0)
        {
            return FALSE;
        }

        //读bank头部
        vfs_file_seek(g_file_sys_id, read_file_offset + ap_offset, SEEK_SET, g_test_file_handle);

        vfs_file_read(g_file_sys_id, &ap_bank_head, sizeof(ap_bank_head_t), g_test_file_handle);

        //读到last segment
        if (ap_bank_head.bank_addr == 0)
        {
            return FALSE;
        }

        //读常驻代码和数据
        vfs_file_seek(g_file_sys_id, ap_bank_head.bank_offset + ap_offset, SEEK_SET, g_test_file_handle);

        vfs_file_read(g_file_sys_id, ap_bank_head.bank_addr, ap_bank_head.bank_length, g_test_file_handle);
    }
    else
    {
        att_test_fread(&ap_bank_head, sizeof(ap_bank_head_t), read_file_offset + ap_offset);

        att_test_fread(ap_bank_head.bank_addr, ap_bank_head.bank_length, ap_bank_head.bank_offset + ap_offset);
    }

    if (bank_no < 50)
    {
        g_epc_addr[0] = epc_mask;
    }
    else
    {
        g_epc_addr[1] = epc_mask;
    }

    act_writel(epc_mask + 0x40000000, PageAddr3);

    return TRUE;
}

void wriet_log_data(uint8 *write_buffer, uint32 test_value, uint32 mode)
{
    uint32 write_len;
    uint8 *file_buffer = (uint8 *) ATT_LOG_FILE_BUFFER;

    write_len = 0;

    while (*write_buffer != '\0')
    {
        file_buffer[write_len + g_write_file_len] = *write_buffer;

        write_buffer++;

        write_len++;
    }

    g_write_file_len += write_len;

    if (mode == TRUE)
    {
        libc_itoa(test_value, &file_buffer[g_write_file_len], 8);
        g_write_file_len += 8;
    }

    file_buffer[g_write_file_len++] = 0x0D;
    file_buffer[g_write_file_len++] = 0x0A;

    return;
}

void att_write_test_info(uint8 *test_info, uint32 test_value, uint32 write_data)
{
    if (write_data == TRUE)
    {
        DEBUG_ATT_PRINT(test_info, test_value, 2);
    }
    else
    {
        DEBUG_ATT_PRINT(test_info, 0, 0);
    }

    if (g_test_mode == TEST_MODE_CARD)
    {
        wriet_log_data(test_info, test_value, write_data);
    }

    return;
}

void test_init(test_ap_info_t *test_ap_info)
{
    //初始化线程互斥信号量
    libc_sem_init(&thread_mutex, 1);

    GLOBAL_CARD_BANK_SWITCH_HANDLER = load_ap_bank;  

    if (g_att_version == 0)
    {
        libc_memcpy(&g_r_old_test_info, test_ap_info, sizeof(test_ap_old_info_t));
        g_old_test_info = (test_ap_old_info_t *) &g_r_old_test_info;
    }
    else
    {
        libc_memcpy(&g_r_test_ap_info, test_ap_info, sizeof(test_ap_info_t));
        
        g_p_test_ap_info_bak = test_ap_info;
        
        g_test_ap_info = &g_r_test_ap_info;
    }
    
    g_test_info.test_id = 0;

    //默认扫描512M,加快文件的创建速度
    g_fs_scan_space = 0x100000;

#if 0
    //进入测试模式,A2设置输入状态，检测ONOFF状态
    act_writel(act_readl(GPIOAINEN) | (1 << 2), GPIOAINEN);

    //进入测试模式,A21设置输入状态，检测RTCVDD状态
    act_writel(act_readl(GPIOAINEN) | (1 << 21), GPIOAINEN);

    //进入测试模式,A22设置输入状态，检测VCC状态
    act_writel(act_readl(GPIOAINEN) | (1 << 21), GPIOAINEN);

    //进入测试模式,B0设置输入状态，检测SVCC状态
    act_writel(act_readl(GPIOBINEN) | (1 << 0), GPIOBINEN);

    //A0默认输出高电平，IO口接下拉电阻
    act_writel(act_readl(GPIOAOUTEN) | (1 << 0), GPIOAOUTEN);

    act_writel(act_readl(GPIOADAT) | (1 << 0), GPIOADAT);
#endif

    g_cur_line_num = 1;

    g_write_file_len = 0;

    g_epc_addr[0] = 0;

    g_epc_addr[1] = 0;

    g_skip_product_test = FALSE;

    if (test_ap_info->base_type == DISK_H || g_old_test_info->base_type == DISK_H)
    {
        g_test_mode = TEST_MODE_CARD;

        g_read_code_mode = TRUE;

        //加载常驻代码段和数据段
        g_test_file_handle = vfs_file_open(g_file_sys_id, g_ap_name, R_NORMAL_SEEK);

        if (g_test_file_handle == 0)
        {
            //return FALSE;
            while (1)
                ;
        }

        DEBUG_ATT_PRINT("test counter: ", read_att_test_count(), 2);
    }
    else if (test_ap_info->base_type == DISK_S || g_old_test_info->base_type == DISK_S)
    {
        g_test_mode = TEST_MODE_USB;

        g_read_code_mode = FALSE;

#ifdef DEBUG_WRITE_CHANNEL_DATA
        g_file_sys_id = _config_fs_init(DISK_H);
#endif
    }
    else
    {
        ;//nothing for QAC
    }
    // test_ap_info->base_type = 0;

    //while(1);
}

void test_exit(void)
{
    libc_sem_destroy(&thread_mutex);

    GLOBAL_CARD_BANK_SWITCH_HANDLER = 0;

    DEBUG_ATT_PRINT("test ap end...", 0, 0);

    //控制测试结束是否重启
    //if((uint32)act_readl(DC5VADC_DATA) > 60)
    //    ENABLE_WATCH_DOG(6);

    //控制设备断电
    att_power_off();

    while (1)
    {
        sys_os_time_dly(1);
    }
}

void test_set_share_data(void)
{
    msg_apps_t msg;

    msg.type = MSG_AUTOTEST_SET_SHARE_DATA_SYNC;
    msg.content.addr = &g_test_share_info;

    send_sync_msg(MSG_TARGET_ID_MANAGER, &msg, NULL, 0);

    return;
}

void main(test_ap_info_t *test_ap_info)
{
    DEBUG_ATT_PRINT("  \r\ntest ap running...", 0, 0);
    
    sys_set_sys_info(1, SYS_PRINT_ONOFF);
    
    DISABLE_WATCH_DOG;

    /* 当前固件与测试程序不兼容时,
     * 需要重新从卡启动并烧录新固件.
     */
    if (test_ap_info->info_size != sizeof(test_ap_info_t))
    //reboot_to_card_product();
    {
        g_att_version = 0;
    }
    else
    {
        g_att_version = 1;
    }

    test_set_share_data();
    sys_os_sched_lock();
    test_init(test_ap_info);
    test_dispatch();
    test_exit();
    return;
}
