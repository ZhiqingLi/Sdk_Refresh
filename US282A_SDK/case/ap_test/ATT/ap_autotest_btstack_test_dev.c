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

int32 att_test_open_file(bt_paired_dev_info2_t *paired_dev_info, uint8 open_mode)
{
    uint8 *pdata;
    int32 trans_bytes;
    int32 file_length;
    uint8 temp_data[32];

    att_fopen_arg_t *att_fopen;

    uint32 random_val;

    trans_bytes = 0;

    file_length = 0;

    att_fopen = (att_fopen_arg_t *) STUB_ATT_RW_TEMP_BUFFER;

    libc_memset(att_fopen, 0, 128);

    if(open_mode == FILE_OPEN_MODE_RB)
    {
        att_fopen->open_mode[0] = 'r';
        att_fopen->open_mode[1] = 0x00;
        att_fopen->open_mode[2] = 'b';
        att_fopen->open_mode[3] = 0x00;
    }
    else
    {
        att_fopen->open_mode[0] = 'w';
        att_fopen->open_mode[1] = 0x00;
        att_fopen->open_mode[2] = 'b';
        att_fopen->open_mode[3] = 0x00;        
    }
    
    bytes_to_unicode(paired_dev_info->remote_addr, 5, 12, &(att_fopen->file_name[0]), &trans_bytes);
    
    att_fopen->file_name[trans_bytes++] = '.';
    att_fopen->file_name[trans_bytes++] = 0;

    att_fopen->file_name[trans_bytes++] = 'b';
    att_fopen->file_name[trans_bytes++] = 0;

    att_fopen->file_name[trans_bytes++] = 'i';
    att_fopen->file_name[trans_bytes++] = 0;

    att_fopen->file_name[trans_bytes++] = 'n';
    att_fopen->file_name[trans_bytes++] = 0;

    att_fopen->file_name[trans_bytes++] = 0;
    att_fopen->file_name[trans_bytes++] = 0;    
    
    att_fopen->file_name[trans_bytes++] = 0;
    att_fopen->file_name[trans_bytes++] = 0; 

    att_write_data(STUB_CMD_ATT_FILE_OPEN, trans_bytes + 8, STUB_ATT_RW_TEMP_BUFFER);

    att_read_data(STUB_CMD_ATT_FILE_OPEN, 4, temp_data);

    pdata = (uint8 *) temp_data;

    file_length = (pdata[6]) | (pdata[7] << 8);

    att_write_data(STUB_CMD_ATT_ACK, 0, STUB_ATT_RW_TEMP_BUFFER);

    //print_log("file len: %d open_mode %d", file_length, open_mode);    

    if(open_mode == FILE_OPEN_MODE_RB)
    {
        if ((file_length != 0) && (file_length != 0xffff))
        {
            return file_length;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;                
    }
       
}

int32 att_test_open_record_file(bt_paired_dev_info2_t *paired_dev_info)
{
    uint32 file_len;

    uint32 read_len;

    uint8 *pdata;
    
    att_fread_arg_t *att_fread_arg = (att_fread_arg_t *) STUB_ATT_RW_TEMP_BUFFER;

    file_len = att_test_open_file(paired_dev_info, FILE_OPEN_MODE_RB);
    
    //先判断文件是否存在
    if(file_len != 0)
    {     
        att_fread_arg->offsetl = 0;
        att_fread_arg->offseth = 0;
        att_fread_arg->lengthl = file_len & 0xffff;
        att_fread_arg->lengthh = (file_len >> 16);
        
        att_write_data(STUB_CMD_ATT_FILE_READ, 8, (uint8 *)STUB_ATT_RW_TEMP_BUFFER);

        att_read_data(STUB_CMD_ATT_FILE_READ, file_len, STUB_ATT_RW_TEMP_BUFFER);

        pdata = (uint8 *) STUB_ATT_RW_TEMP_BUFFER;

        read_len = (pdata[6]) | (pdata[7] << 8);

        libc_memcpy(paired_dev_info, (uint8 *)(STUB_ATT_RW_TEMP_BUFFER + 6), sizeof(bt_paired_dev_info2_t));

        att_write_data(STUB_CMD_ATT_ACK, 0, STUB_ATT_RW_TEMP_BUFFER);

        return read_len;
    }
    else
    {
        return 0;
    }
}

int32 att_test_close_file(void)
{
    att_write_data(STUB_CMD_ATT_FILE_CLOSE, 0, STUB_ATT_RW_TEMP_BUFFER);

    att_read_data(STUB_CMD_ATT_ACK, 0, STUB_ATT_RW_TEMP_BUFFER);

    return TRUE;

}



//从文件读取配对信息
uint32 test_btstack_read_paired_dev_info(bt_paired_dev_info2_t *paired_dev_info)
{
    uint32 i;

    btaddr_log_file_t *btaddr_log = (btaddr_log_file_t *) LOG_FILE_BUFFER;
    
    if(g_test_mode == TEST_MODE_CARD)
    {      
        act_open_att_record_file();
        
        if(libc_memcmp(&(btaddr_log->bt_paired_dev_info.remote_addr), paired_dev_info->remote_addr, 6) == 0)
        {
            libc_memcpy(paired_dev_info, &(btaddr_log->bt_paired_dev_info), sizeof(bt_paired_dev_info2_t));
        }
        else
        {
            return FALSE;      
        }
    }
    else
    {
        if(att_test_open_record_file(paired_dev_info) != 0)
        {
            att_test_close_file();           
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;     
}

void test_btstack_get_paired_dev_info(bt_paired_dev_info2_t *bt_paired_dev2)
{
    msg_apps_t msg;
    
    msg.type = MSG_AUTOTEST_GET_PAIRED_DEV2_SYNC;
    msg.content.addr = bt_paired_dev2;
    
    send_sync_msg(MSG_TARGET_ID_BTSTACK, &msg, NULL, 0);     

}

//从btstack获取配对信息并更新到文件中
void test_btstack_save_paired_dev_info(bt_paired_dev_info2_t *bt_paired_dev2)
{
    uint32 i;
    
    uint32 file_len;

    bt_paired_dev_info2_t old_paired_dev;

    btaddr_log_file_t *btaddr_log = (btaddr_log_file_t *) LOG_FILE_BUFFER;
    
    att_fwrite_arg_t *att_fwrite_arg = (att_fwrite_arg_t *) STUB_ATT_RW_TEMP_BUFFER;

    libc_memcpy(&(old_paired_dev.remote_addr), bt_paired_dev2->remote_addr, 6);
    
    if(test_btstack_read_paired_dev_info(&old_paired_dev) == TRUE)
    {
        if(libc_memcmp(&old_paired_dev, bt_paired_dev2, sizeof(bt_paired_dev_info2_t)) == 0)
        {
            return;
        }
    }
    
    if(g_test_mode == TEST_MODE_CARD)
    {    
        att_write_test_info("pair list update", 0, 0);
        act_test_write_att_record_file(bt_paired_dev2, 0, 2);
    }
    else
    {
        print_log("pair list update");
        
        //不存在则创建
        att_test_open_file(bt_paired_dev2, FILE_OPEN_MODE_WB);

        file_len = sizeof(bt_paired_dev_info2_t);
        
        libc_memcpy((uint8 *)(STUB_ATT_RW_TEMP_BUFFER + 14), bt_paired_dev2, file_len);
       
        att_fwrite_arg->offsetl = 0;
        att_fwrite_arg->offseth = 0;
        att_fwrite_arg->lengthl = file_len & 0xffff;
        att_fwrite_arg->lengthh = (file_len >> 16); 

        att_write_data(STUB_CMD_ATT_FILE_WRITE, file_len + 8, (uint8 *)STUB_ATT_RW_TEMP_BUFFER);
        
        att_read_data(STUB_CMD_ATT_FILE_WRITE, 4, STUB_ATT_RW_TEMP_BUFFER);      
     
        //关闭配对文件
        att_test_close_file();
    }  

    return;
}



