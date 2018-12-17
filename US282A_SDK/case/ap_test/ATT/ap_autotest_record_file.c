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

#pragma __PRQA_IGNORE_START__

static uint8 file_name_buffer[80] _BANK_DATA_ATTR_;

#if 0
const uint8 bt_name_tips_ok[] = "modify bt name OK";
const uint8 bt_name_tips_fail[] = "modify bt name FAILED";

const uint8 ble_name_tips_ok[] = "modify ble name OK";
const uint8 ble_name_tips_fail[] = "modify ble name FAILED";

const uint8 bt_addr_tips_ok[] = "modify bt addr OK";
const uint8 bt_addr_tips_fail[] = "modify bt addr FAILED";
#endif

static void create_file_name(uint8 *file_name, log_test_info_t *test_info, uint32 mode)
{
    int i = 0;
    date_t cur_date;
    time_t cur_time;
    uint8 byte_number[6];
    uint32 record_count = test_info->record_cnt;

    read_bt_addr(byte_number, 1);

    file_name[i++] = 0xff;
    file_name[i++] = 0xfe;

    bytes_to_unicode(byte_number, 5, 12, &file_name[i], &i);

    file_name[i++] = '_';
    file_name[i++] = 0x0;

    //显示count
    file_name[i++] = (record_count/1000000) + '0';
    file_name[i++] = 0x0;

    file_name[i++] = ((record_count%1000000)/100000) + '0';
    file_name[i++] = 0x0;

    file_name[i++] = ((record_count%100000)/10000) + '0';
    file_name[i++] = 0x0;

    file_name[i++] = ((record_count%10000)/1000) + '0';
    file_name[i++] = 0x0;

    file_name[i++] = ((record_count%1000)/100) + '0';
    file_name[i++] = 0x0;

    file_name[i++] = ((record_count%100)/10) + '0';
    file_name[i++] = 0x0;

    file_name[i++] = (record_count%10) + '0';
    file_name[i++] = 0x0;

    file_name[i++] = '_';
    file_name[i++] = 0x0;

    if(mode == TRUE)
    {
        //测试成功文件名显示P
        file_name[i++] = 'P';
        file_name[i++] = 0x0;        
    }
    else
    {
        //测试失败文件名显示F
        file_name[i++] = 'F';
        file_name[i++] = 0x0;         
    }

    file_name[i++] = 0x2e;
    file_name[i++] = 0x0;

    file_name[i++] = 'L';
    file_name[i++] = 0x0;

    file_name[i++] = 'O';
    file_name[i++] = 0x0;

    file_name[i++] = 'G';
    file_name[i++] = 0x0;

    file_name[i++] = 0x0;
    file_name[i++] = 0x0;    
}

int32 create_record_file(void)
{
    int ret_val;
    int32 file_handle;
    uint8 *file_buffer = (uint8 *)LOG_FILE_BUFFER;

    //定位到根目录
    //vfs_cd(g_file_sys_id, CD_ROOT, 0);

    ret_val = vfs_file_dir_exist(g_file_sys_id, file_name_buffer, 1);

    libc_memset(file_buffer, 0, LOG_FILE_LEN);

    if(ret_val == 0)
    {
        file_handle = vfs_file_create(g_file_sys_id, file_name_buffer, LOG_FILE_LEN);

        if(file_handle == 0)
        {
            libc_print("create recrod file error!\n", 0, 0);

            while(1);
        }

        vfs_file_write(g_file_sys_id, file_buffer, LOG_FILE_LEN, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);
    }
    else
    {
        file_handle = vfs_file_open(g_file_sys_id, file_name_buffer, OPEN_MODIFY);

        if(file_handle == 0)
        {
            libc_print("open file error!\n", 0, 0);

            while(1);
        }

        vfs_file_write(g_file_sys_id, file_buffer, LOG_FILE_LEN, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);
    }

    return file_handle;
}

void close_log_file(uint32 file_handle)
{
    uint8 *file_buffer = (uint8 *)ATT_LOG_FILE_BUFFER;

    vfs_file_write(g_file_sys_id, file_buffer, g_write_file_len, file_handle);

    DEBUG_ATT_PRINT("write file over!\n", 0, 0);

    vfs_file_close(g_file_sys_id, file_handle);

    return;
}

uint32 read_att_test_info(log_test_info_t *test_info)
{
    btaddr_log_file_t *btaddr_log = (log_test_info_t *) LOG_FILE_BUFFER;

    act_open_att_record_file();

    libc_memcpy(test_info, &(btaddr_log->record_cnt), sizeof(log_test_info_t));
}

void make_log_dir(uint8 *dir_name, uint32 record_cnt)
{
    uint32 i;
    uint32 min_num, max_num;

    i = 0;
    dir_name[i++] = 0xff;
    dir_name[i++] = 0xfe;

    dir_name[i++] = 'A';
    dir_name[i++] = 0;
    dir_name[i++] = 'T';
    dir_name[i++] = 0;
    dir_name[i++] = 'T';
    dir_name[i++] = 0; 

    //每1000个日志文件一个目录
    max_num = (record_cnt + 999)/1000 * 1000;

    min_num = max_num - 999;

    dir_name[i++] = (min_num/1000000) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((min_num%1000000)/100000) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((min_num%100000)/10000) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((min_num%10000)/1000) + '0';
    dir_name[i++] = 0x0;
    

    dir_name[i++] = ((min_num%1000)/100) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((min_num%100)/10) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = (min_num%10) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = '~';
    dir_name[i++] = 0; 

    dir_name[i++] = (max_num/1000000) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((max_num%1000000)/100000) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((max_num%100000)/10000) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((max_num%10000)/1000) + '0';
    dir_name[i++] = 0x0;
    

    dir_name[i++] = ((max_num%1000)/100) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = ((max_num%100)/10) + '0';
    dir_name[i++] = 0x0;

    dir_name[i++] = (max_num%10) + '0';
    dir_name[i++] = 0x0;

    return;
}

void enter_log_dir(uint32 record_cnt)
{
    uint32 dir_name[48];

    make_log_dir(dir_name, record_cnt);

    if(vfs_file_dir_exist(g_file_sys_id, dir_name, 0) == 0)
    {
        vfs_make_dir(g_file_sys_id, dir_name);
    }

    vfs_cd(g_file_sys_id, CD_SUB, dir_name);
}

void write_log_file(uint32 mode)
{
    int32 file_handle;
    log_test_info_t test_info;

    sys_os_sched_lock();

    read_att_test_info(&test_info);

    if(mode == TRUE)
    {
        test_info.succeed_cnt++;
    }
    else
    {
        test_info.failed_cnt++;
    }

    test_info.record_cnt++;

    att_write_test_info("test count: ", test_info.record_cnt,1);

    att_write_test_info("succeed: ", test_info.succeed_cnt, 1);

    att_write_test_info("failed: ", test_info.failed_cnt, 1);

    att_write_test_info("Test time: ", sys_get_ab_timer() - g_test_base_time, 1);

    enter_log_dir(test_info.record_cnt);

    //创建文件名
    create_file_name(file_name_buffer, &test_info, mode);

    create_record_file();

    file_handle = vfs_file_open(g_file_sys_id, file_name_buffer, OPEN_MODIFY);

    close_log_file(file_handle);

    vfs_cd(g_file_sys_id, CD_ROOT, 0);

    act_test_write_att_record_file(0, &test_info, 1);  

    DEBUG_ATT_PRINT("write log file ok!\n", 0, 0);

    sys_os_sched_unlock();

    return;
}
#pragma __PRQA_IGNORE_END__


