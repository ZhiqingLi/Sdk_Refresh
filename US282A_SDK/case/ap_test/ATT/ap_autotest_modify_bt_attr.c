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

static void act_test_modify_bt_vram(void)
{
    #define ATT_MAGIC  0xABCD

    bt_addr_vram_t*  p = (void*)STUB_ATT_RETURN_DATA_BUFFER;

    if(g_support_norflash_wp == TRUE)
    {
        /* 标记蓝牙参数已通过 ATT 修改 */
        p->magic = ATT_MAGIC;
        
        sys_vm_write(p, VM_BTSTACK, 4);
    }
}

#define BT_ADDR_LOG_FILE_LEN   (512)

#define BT_ADDR_LOG_MAGIC_L    (0x44434241)

#define BT_ADDR_LOG_MAGIC_H    (0x48474645)

static const uint8 att_record_filename[] =
{
    0xff, 0xfe,
    'a',  0x00,
    't',  0x00,
    't',  0x00,
    '_',  0x00,
    'r',  0x00,
    'e',  0x00,
    'c',  0x00,
    'o',  0x00,
    'r',  0x00,
    'd',  0x00,
    '.',  0x00,
    'b',  0x00,
    'i',  0x00,
    'n',  0x00,
    0x00, 0x00
};

static uint32 cal_att_file_checksum(btaddr_log_file_t *btaddr_log_file)
{
    uint32 checksum = 0;
    uint32 i;
    uint32 *pdata = (uint32 *) btaddr_log_file;

    for (i = 0; i < ((sizeof(btaddr_log_file_t) - 4) / 4); i++)
    {
        checksum += pdata[i];
    }

    return checksum;
}

static int32 write_param_data(nvram_param_rw_t *param_rw)
{
    int32 ret_val;

    ret_val = base_param_write(param_rw);

    if(ret_val != 0)
    {
        base_param_merge_deal();

        ret_val = base_param_write(param_rw);
    }

    return ret_val;
}
int act_test_report_modify_bt_addr_result(bt_addr_arg_t *bt_addr_arg)
{
    uint8 cmd_data[16];
    int ret_val;
    return_result_t *return_data;
    int trans_bytes;

    nvram_param_rw_t param_rw;

    param_rw.logical_index = PARAM_BT_ADDR;

    param_rw.rw_buffer = cmd_data;

    param_rw.rw_len = PARAM_BT_ADDR_LEN;

    base_param_read(&param_rw);

    if (libc_memcmp(bt_addr_arg->bt_addr, cmd_data, param_rw.rw_len) != 0)
    {
        ret_val = 0;
    }
    else
    {
        ret_val = 1;
    }

    if (ret_val == 1)
    {
        att_write_test_info("modify bt addr ok", 0, 0);
    }
    else
    {
        att_write_test_info("modify bt addr failed", 0, 0);
    }

    att_write_test_info("byte 5:", cmd_data[5], 1);
    att_write_test_info("byte 4:", cmd_data[4], 1);
    att_write_test_info("byte 3:", cmd_data[3], 1);
    att_write_test_info("byte 2:", cmd_data[2], 1);
    att_write_test_info("byte 1:", cmd_data[1], 1);
    att_write_test_info("byte 0:", cmd_data[0], 1);

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = g_test_info.test_id;

        return_data->test_result = ret_val;

        trans_bytes = 0;

        //蓝牙地址转unicode形式
        bytes_to_unicode(cmd_data, 5, 6, return_data->return_arg, &trans_bytes);

        //添加参数分隔符','
        return_data->return_arg[trans_bytes++] = 0x002c;

        bytes_to_unicode(&(bt_addr_arg->bt_addr_add_mode), 0, 1, &(return_data->return_arg[trans_bytes]), &trans_bytes);

        //添加参数分隔符','
        return_data->return_arg[trans_bytes++] = 0x002c;

        bytes_to_unicode(&(bt_addr_arg->bt_burn_mode), 0, 1, &(return_data->return_arg[trans_bytes]), &trans_bytes);

        //添加结束符
        return_data->return_arg[trans_bytes++] = 0x0000;

        //如果参数未四字节对齐，要四字节对齐处理
        if ((trans_bytes % 2) != 0)
        {
            return_data->return_arg[trans_bytes++] = 0x0000;
        }

        act_test_report_result(return_data, trans_bytes * 2 + 4);
    }
    else
    {
        //累加模式
        if (bt_addr_arg->bt_addr_add_mode == 0)
        {
            act_test_write_att_record_file(&(bt_addr_arg->bt_addr[0]), 0, 0);
        }
        
        act_test_report_test_log(ret_val, g_test_info.test_id);
    }

    return ret_val;
}

test_result_e act_test_modify_bt_addr(void *arg_buffer)
{
    int ret_val;

    nvram_param_rw_t param_rw;
    
    if(g_support_norflash_wp == TRUE)
    {
        base_set_disable_write_protect();        
    }

    bt_addr_arg_t *bt_addr_arg = (bt_addr_arg_t *) arg_buffer;

    param_rw.logical_index = PARAM_BT_ADDR;
    param_rw.rw_len = PARAM_BT_ADDR_LEN;
    param_rw.rw_buffer = bt_addr_arg->bt_addr;

    ret_val = write_param_data(&param_rw);

    act_test_report_modify_bt_addr_result(bt_addr_arg);

    act_test_modify_bt_vram();
    
    return ret_val;
}

int32 act_open_att_record_file(void)
{
    int ret_val;
    int32 file_handle;
    int32 file_valid;
    uint32 check_sum;
    uint8 *file_buffer = (uint8 *) LOG_FILE_BUFFER;
    uint32 *pdata = (uint32 *) file_buffer;
    btaddr_log_file_t *btaddr_log = (btaddr_log_file_t *) file_buffer;

    //定位到根目录
    vfs_cd(g_file_sys_id, CD_ROOT, 0);

    ret_val = vfs_file_dir_exist(g_file_sys_id, att_record_filename, 1);

    libc_memset(file_buffer, 0, BT_ADDR_LOG_FILE_LEN);

    file_valid = TRUE;

    if (ret_val == 0)
    {
        file_valid = FALSE;
    }
    else
    {
        file_handle = vfs_file_open(g_file_sys_id, att_record_filename, OPEN_MODIFY);

        if (file_handle == 0)
        {
            libc_print("open file error!\n", 0, 0);

            while (1)
                ;
        }

        vfs_file_read(g_file_sys_id, file_buffer, BT_ADDR_LOG_FILE_LEN, file_handle);

        if ((btaddr_log->magicl != BT_ADDR_LOG_MAGIC_L) || (btaddr_log->magich != BT_ADDR_LOG_MAGIC_H))
        {
            file_valid = FALSE;
        }

        check_sum = cal_att_file_checksum(btaddr_log);

        if (check_sum != btaddr_log->checksum)
        {
            file_valid = FALSE;
        }
    }

    if (file_valid == TRUE)
    {
        vfs_file_close(g_file_sys_id, file_handle);
        return TRUE;
    }
    else
    {
        file_handle = vfs_file_create(g_file_sys_id, att_record_filename, BT_ADDR_LOG_FILE_LEN);

        if (file_handle == 0)
        {
            libc_print("create bt addr log file error!\n", 0, 0);

            while (1)
                ;
        }

        libc_memset(btaddr_log, 0, sizeof(btaddr_log_file_t));
        
        btaddr_log->magicl = BT_ADDR_LOG_MAGIC_L;
        btaddr_log->magich = BT_ADDR_LOG_MAGIC_H;
        btaddr_log->record_cnt = 0;
        btaddr_log->succeed_cnt = 0;
        btaddr_log->failed_cnt = 0;
        btaddr_log->checksum = cal_att_file_checksum(btaddr_log);

        vfs_file_write(g_file_sys_id, file_buffer, BT_ADDR_LOG_FILE_LEN, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);

        return FALSE;

    }
}

int32 act_test_read_bt_addr_from_log(uint8 *byte_buffer)
{
    btaddr_log_file_t *btaddr_log = (btaddr_log_file_t *) LOG_FILE_BUFFER;

    if (act_open_att_record_file() == TRUE)
    {
        //按照高位在前的顺序依次读入
        byte_buffer[2] = btaddr_log->addr[0];
        byte_buffer[1] = btaddr_log->addr[1];
        byte_buffer[0] = btaddr_log->addr[2];
    }
}

int32 act_test_write_att_record_file(uint8 *byte_buffer, log_test_info_t *test_info, uint32 mode)
{
    uint32 i;
    int32 file_handle;
    uint8 *file_buffer = (uint8 *) LOG_FILE_BUFFER;
    uint32 *pdata = (uint32 *) file_buffer;
    btaddr_log_file_t *btaddr_log = (btaddr_log_file_t *) file_buffer;

    //定位到根目录
    vfs_cd(g_file_sys_id, CD_ROOT, 0);

    file_handle = vfs_file_open(g_file_sys_id, att_record_filename, OPEN_MODIFY);

    if (file_handle == 0)
    {
        libc_print("open file error!\n", 0, 0);

        while (1)
            ;
    }

    vfs_file_read(g_file_sys_id, file_buffer, BT_ADDR_LOG_FILE_LEN, file_handle);

    if ((btaddr_log->magicl != BT_ADDR_LOG_MAGIC_L) || (btaddr_log->magich != BT_ADDR_LOG_MAGIC_H))
    {
        libc_print("file unvalid!\n", 0, 0);

        while (1)
            ;
    }

    if (mode == 0)
    {
        for (i = 0; i < 3; i++)
        {
            byte_buffer[i]++;

            if (byte_buffer[i] != 0)
            {
                break;
            }
        }

        //按照高位在前的顺序依次写入
        btaddr_log->addr[0] = byte_buffer[2];
        btaddr_log->addr[1] = byte_buffer[1];
        btaddr_log->addr[2] = byte_buffer[0];
    }
    else if(mode == 1)
    {
        libc_memcpy(&(btaddr_log->record_cnt), test_info, sizeof(log_test_info_t));
    }
    else
    {        
        libc_memcpy(&(btaddr_log->bt_paired_dev_info), byte_buffer, sizeof(bt_paired_dev_info2_t));
        ;//nothing 已更新配对列表信息，直接计算校验和
    }

    btaddr_log->checksum = cal_att_file_checksum(btaddr_log);

    vfs_file_seek(g_file_sys_id, 0, SEEK_SET, file_handle);

    vfs_file_write(g_file_sys_id, file_buffer, BT_ADDR_LOG_FILE_LEN, file_handle);

    vfs_file_close(g_file_sys_id, file_handle);

    return TRUE;
}

uint32 read_att_test_count(void)
{
    btaddr_log_file_t *btaddr_log = (btaddr_log_file_t *) LOG_FILE_BUFFER;

    act_open_att_record_file();

    return btaddr_log->record_cnt;
}

static int32 utf8str_to_unicode(uint8 *utf8, uint32 utf8Len, uint16 *unicode, uint32 *unicode_len)
{
    int32 count = 0;
    uint8 c0, c1;
    uint32 scalar;

    while (--utf8Len >= 0)
    {
        c0 = *utf8;
        utf8++;

        if (c0 < 0x80)
        {
            *unicode = c0;

            if (*unicode == 0x00)
            {
                count += 2;
                break;
            }
            unicode++;
            count += 2;
            continue;
        }

        /*非ascii字符，编码的第一个字节不应该为10xxxxxx*/
        if ((c0 & 0xc0) == 0x80)
        {
            return -1;
        }

        scalar = c0;
        if (--utf8Len < 0)
        {
            return -1;
        }

        c1 = *utf8;
        utf8++;
        /*第2个字节的编码格式应该为10xxxxxx*/
        if ((c1 & 0xc0) != 0x80)
        {
            return -1;
        }
        scalar <<= 6;
        scalar |= (c1 & 0x3f);

        /*如果r0 & 0x20等于0，说明该utf编码长度为2，否则其编码长度大于2*/
        if (!(c0 & 0x20))
        {
            if ((scalar != 0) && (scalar < 0x80))
            {
                /*unicode编码小于0x80，utf8编码长度却为2，over encoding*/
                return -1;
            }
            *unicode = (unsigned short) scalar & 0x7ff;
            if (*unicode == 0x00)
            {
                count += 2;
                break;
            }
            unicode++;
            count += 2;
            continue;
        }

        if (--utf8Len < 0)
        {
            return -1;
        }

        c1 = *utf8;
        utf8++;
        /*第3个字节的编码格式应该为10xxxxxx*/
        if ((c1 & 0xc0) != 0x80)
        {
            return -1;
        }
        scalar <<= 6;
        scalar |= (c1 & 0x3f);

        /*如果r0 & 0x10等于0，说明该utf编码长度为3，否则其编码长度大于3*/
        if (!(c0 & 0x10))
        {
            if (scalar < 0x800)
            {
                return -1;
            }
            if ((scalar >= 0xd800) && (scalar < 0xe000))
            {
                return -1;
            }
            *unicode = (unsigned short) scalar & 0xffff;
            if (*unicode == 0x00)
            {
                count += 2;
                break;
            }
            unicode++;
            count += 2;
            continue;
        }

        return -1;
    }

    //保证字符串四字节对齐
    if ((count % 4) != 0)
    {
        unicode++;
        count += 2;
        *unicode = 0x00;
    }

    *unicode_len = count;
    return 0;
}

int act_test_report_modify_bt_name_result(bt_name_arg_t *bt_name_arg, uint16 write_len)
{
    uint8 cmd_data[TEST_BTNAME_MAX_LEN];
    int ret_val;
    return_result_t *return_data;
    nvram_param_rw_t param_rw;
    uint16 trans_bytes = 0;

    param_rw.logical_index = PARAM_BT_NAME;

    param_rw.rw_buffer = cmd_data;

    param_rw.rw_len = PARAM_BT_NAME_LEN;

    base_param_read(&param_rw);

    //以写入的字节数进行比较
    if (libc_memcmp(bt_name_arg->bt_name, param_rw.rw_buffer, write_len) != 0)
    {
        ret_val = 0;
    }
    else
    {
        ret_val = 1;
    }

    if (ret_val == 1)
    {
        att_write_test_info("modify bt name ok", 0, 0);
    }
    else
    {
        att_write_test_info("modify bt name failed", 0, 0);
    }

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_MODIFY_BTNAME;

        return_data->test_result = ret_val;

        //utf8转unicode可能所需的buffer较多，极端情况下是之前字符串buffer的2倍
        //因此需要一个大一点的buffer缓存字符串
        utf8str_to_unicode(cmd_data, sizeof(cmd_data), return_data->return_arg, &trans_bytes);

        act_test_report_result(return_data, trans_bytes + 4);
    }
    else
    {
        act_test_report_test_log(ret_val, TESTID_MODIFY_BTNAME);    
    }

    return ret_val;
}

test_result_e act_test_modify_bt_name(void *arg_buffer)
{
    int ret_val;

    nvram_param_rw_t param_rw;
    
    if(g_support_norflash_wp == TRUE)
    {
        base_set_disable_write_protect();        
    }    

    bt_name_arg_t *bt_name_arg = (bt_name_arg_t *) arg_buffer;

    param_rw.logical_index = PARAM_BT_NAME;

    //包括结束符在内
    param_rw.rw_len = libc_strlen(bt_name_arg->bt_name) + 1;

    //DEBUG_ATT_PRINT("write bt name len", param_rw.rw_len, 2);

    param_rw.rw_buffer = bt_name_arg->bt_name;

    ret_val = write_param_data(&param_rw);

    act_test_report_modify_bt_name_result(bt_name_arg, param_rw.rw_len);

    act_test_modify_bt_vram();
    
    return ret_val;
}

void act_test_report_modify_ble_name_result(ble_name_arg_t *ble_name_arg, uint16 write_len)
{
    uint8 cmd_data[TEST_BTBLENAME_MAX_LEN];
    int ret_val;
    return_result_t *return_data;
    nvram_param_rw_t param_rw;
    uint16 trans_bytes = 0;

    param_rw.logical_index = PARAM_BT_BLENAME;

    param_rw.rw_buffer = cmd_data;

    param_rw.rw_len = PARAM_BT_BLENAME_LEN;

    base_param_read(&param_rw);

    if (libc_memcmp(ble_name_arg->bt_ble_name, param_rw.rw_buffer, write_len) != 0)
    {
        ret_val = 0;
    }
    else
    {
        ret_val = 1;
    }

    if (ret_val == 1)
    {
        att_write_test_info("modify ble name ok", 0, 0);
    }
    else
    {
        att_write_test_info("modify ble name failed", 0, 0);
    }

    att_write_test_info(cmd_data, 0, 0);

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_MODIFY_BLENAME;

        return_data->test_result = ret_val;

        //由于ble名称字符串较多，也需要先转换再缓存
        utf8str_to_unicode(cmd_data, sizeof(cmd_data), return_data->return_arg, &trans_bytes);

        //按照实际字节数上报结果，比如虽然有30字节数据，但只上报12个有效字节的数据
        act_test_report_result(return_data, trans_bytes + 4);
    }
    else
    {
        act_test_report_test_log(ret_val, TESTID_MODIFY_BLENAME);    
    }    
}

test_result_e act_test_modify_bt_ble_name(void *arg_buffer)
{
    int ret_val;

    nvram_param_rw_t param_rw;
    
    if(g_support_norflash_wp == TRUE)
    {
        base_set_disable_write_protect();        
    }    

    ble_name_arg_t *ble_name_arg = (ble_name_arg_t *) arg_buffer;

    param_rw.logical_index = PARAM_BT_BLENAME;
    param_rw.rw_len = libc_strlen(ble_name_arg->bt_ble_name) + 1;
    param_rw.rw_buffer = ble_name_arg->bt_ble_name;

    ret_val = base_param_write(&param_rw);

    act_test_report_modify_ble_name_result(ble_name_arg, param_rw.rw_len);

    act_test_modify_bt_vram();
    
    return ret_val;
}

