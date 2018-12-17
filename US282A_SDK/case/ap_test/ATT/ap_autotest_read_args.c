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

uint16 *act_test_read_cfg_file(uint32 line_number)
{
    uint16 *cur;
    uint16 *start;
    uint16 *begin;
    uint32 cur_line;
    uint16 *cfg_file_buffer = (uint16 *) CFG_FILE_BUFFER;

    if (g_test_ap_info->cfg_file_len > MAX_CFG_FILE_LENGTH)
    {
        //DEBUG_ATT_PRINT("cfg file length error!", 0, 0);

        return FALSE;
    }

    vfs_file_seek(g_file_sys_id, g_test_ap_info->cfg_file_offset, SEEK_SET, g_test_file_handle);

    vfs_file_read(g_file_sys_id, cfg_file_buffer, g_test_ap_info->cfg_file_len, g_test_file_handle);

    cur = cfg_file_buffer;

    cur_line = 0;

    while (1)
    {
        start = cur;

        begin = cur;

        //搜索结束标志 00 0d 00 0a
        while (*cur != 0x000a)
        {
            cur++;
        }

        while ((*start != 0x003d) && (start != cur))
        {
            start++;
        }

        //搜索到'='，说明是有效的数据行，否则过滤该行数据
        if (*start == 0x003d)
        {
            cur_line++;
        }

        //搜索到指定行数，退出
        if (cur_line == line_number)
        {
            break;
        }
        else
        {
            cur++;

            if ((uint32)(cur - cfg_file_buffer) > g_test_ap_info->cfg_file_len)
            {
                return 0;
            }
        }
    }

    return begin;
}

static uint8 unicode_to_uint8(uint16 widechar)
{
    uint8 temp_value;

    if ((widechar >= '0') && (widechar <= '9'))
    {
        temp_value = (widechar - '0');
    }
    else if ((widechar >= 'A') && (widechar <= 'F'))
    {
        temp_value = widechar + 10 - 'A';
    }
    else if ((widechar >= 'a') && (widechar <= 'f'))
    {
        temp_value = widechar + 10 - 'a';
    }
    else
    {
        return 0;
    }

    return temp_value;
}

static int32 unicode_to_int(uint16 *start, uint16 *end, uint32 base)
{
    uint32 minus_flag;
    int32 temp_value = 0;

    minus_flag = FALSE;

    while (start != end)
    {
        if (*start == '-')
        {
            minus_flag = TRUE;
        }
        else
        {
            temp_value *= base;

            temp_value += unicode_to_uint8(*start);
        }
        start++;
    }

    if (minus_flag == TRUE)
    {
        temp_value = 0 - temp_value;
    }

    return temp_value;
}

static int32 unicode_encode_utf8(uint8 *s, uint16 widechar)
{
    int32 encode_len;

    if (widechar & 0xF800)
    {
        encode_len = 3;
    }
    else if (widechar & 0xFF80)
    {
        encode_len = 2;
    }
    else
    {
        encode_len = 1;
    }

    switch (encode_len)
    {
        case 1:
        *s = (char) widechar;
        break;

        case 2:
        *s++ = 0xC0 | (widechar >> 6);
        *s = 0x80 | (widechar & 0x3F);
        break;

        case 3:
        *s++ = 0xE0 | (widechar >> 12);
        *s++ = 0x80 | ((widechar >> 6) & 0x3F);
        *s = 0x80 | (widechar & 0x3F);
        break;
    }

    return encode_len;
}

static int32 unicode_to_utf8_str(uint16 *start, uint16 *end, uint8 *utf8_buffer, uint32 utf8_buffer_len)
{
    int32 encode_len;
    int32 encode_total_len;

    encode_len = 0;
    encode_total_len = 0;

    while (start != end)
    {
        encode_len = unicode_encode_utf8(utf8_buffer, *start);

        start++;

        if (encode_len + encode_total_len > utf8_buffer_len)
        {
            return FALSE;
        }

        encode_total_len += encode_len;

        utf8_buffer += encode_len;
    }

    //末尾添加结束符
    *utf8_buffer = 0;

    return TRUE;
}

static int32 unicode_to_uint8_bytes(uint16 *start, uint16 *end, uint8 *byte_buffer, uint8 byte_index, uint8 byte_len)
{
    while (start != end)
    {
        byte_buffer[byte_index] = (unicode_to_uint8(*start) << 4);

        byte_buffer[byte_index] |= unicode_to_uint8(*(start + 1));

        byte_index--;

        byte_len--;

        if (byte_len == 0)
        {
            break;
        }

        start += 2;
    }

    return TRUE;
}

uint32 act_test_read_test_id(uint16 *line_buffer)
{
    uint16 *start;
    uint16 *cur;
    uint32 test_id;

    start = line_buffer;
    cur = start;

    while (*cur != 0x003d)
    {
        cur++;
    }

    //test id都是十进制的字符串
    test_id = unicode_to_int(start, cur, 10);

    return test_id;
}

static uint8 *act_test_parse_test_arg(uint16 *line_buffer, uint8 arg_number, uint16 **start, uint16 **end)
{
    uint16 *cur;
    //uint32 test_id;
    uint8 cur_arg_num;

    cur = line_buffer;

    //卡模式需要过滤包括'='的前面字符
    //USB模式直接截取字符串即可
    if (g_test_mode == TEST_MODE_CARD)
    {
        while (*cur != 0x003d)
        {
            cur++;
        }

        //skip '='
        cur++;
    }

    cur_arg_num = 0;

    while (cur_arg_num < arg_number)
    {
        *start = cur;

        //遇到','表明解析一个参数，遇到0x0d0a表示一行结束
        while ((*cur != 0x002c) && (*cur != 0x000d) && (*cur != 0x0000))
        {
            cur++;
        }

        *end = cur;

        cur_arg_num++;

        cur++;
    }

    return 0;
}

static int32 act_test_read_bt_name_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start;
    uint16 *end;

    bt_name_arg_t *bt_name_arg = (bt_name_arg_t *) arg_buffer;

    //if(arg_len < sizeof(bt_name_arg_t))
    //{
    //    DEBUG_ATT_PRINT("argument too long", 0, 0);
    //    while(1);
    //}

    act_test_parse_test_arg(line_buffer, 1, &start, &end);

    unicode_to_utf8_str(start, end, bt_name_arg->bt_name, TEST_BTNAME_MAX_LEN);

    //DEBUG_ATT_PRINT(bt_name_arg->bt_name, 0, 0);

    return TRUE;
}

static int32 act_test_read_ble_name_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start;
    uint16 *end;

    ble_name_arg_t *ble_name_arg = (ble_name_arg_t *) arg_buffer;

    //if(arg_len < sizeof(ble_name_arg_t))
    //{
    //    DEBUG_ATT_PRINT("argument too long", 0, 0);
    //    while(1);
    //}

    act_test_parse_test_arg(line_buffer, 1, &start, &end);

    unicode_to_utf8_str(start, end, ble_name_arg->bt_ble_name, TEST_BTBLENAME_MAX_LEN);

    //DEBUG_ATT_PRINT(ble_name_arg->bt_ble_name, 0, 0);

    return TRUE;
}

static int32 act_test_read_bt_addr_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start;
    uint16 *end;

    bt_addr_arg_t *bt_addr_arg = (bt_addr_arg_t *) arg_buffer;

    //if(arg_len < sizeof(bt_addr_arg_t))
    //{
    //    DEBUG_ATT_PRINT("argument too long", 0, 0);
    //    while(1);
    //}

    act_test_parse_test_arg(line_buffer, 1, &start, &end);

    //读取byte5, byte4, byte3数据，一般为蓝牙地址固定字节
    unicode_to_uint8_bytes(start, end, bt_addr_arg->bt_addr, 5, 3);

    //对于卡测试模式，首先从创建的bt_addr.log文件读取蓝牙地址，如果没有
    //再从配置文件读取
    if (g_test_mode == TEST_MODE_CARD)
    {
        //读取byte2, byte1, byte0数据，一般为会修改的字节
        if (act_test_read_bt_addr_from_log(bt_addr_arg->bt_addr) == FALSE)
        {
            act_test_parse_test_arg(line_buffer, 2, &start, &end);

            unicode_to_uint8_bytes(start, end, bt_addr_arg->bt_addr, 2, 3);
        }

        act_test_parse_test_arg(line_buffer, 4, &start, &end);

        bt_addr_arg->bt_addr_add_mode = unicode_to_int(start, end, 16);

        act_test_parse_test_arg(line_buffer, 5, &start, &end);

        bt_addr_arg->bt_burn_mode = unicode_to_int(start, end, 16);        
    }
    else
    {
        //非卡测试方案，直接解析pc发过来的数据
        act_test_parse_test_arg(line_buffer, 2, &start, &end);

        unicode_to_uint8_bytes(start, end, bt_addr_arg->bt_addr, 2, 3);
    }

    return TRUE;
}

static int32 act_test_read_bt_addr_arg2(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start;
    uint16 *end;

    bt_addr_arg_t *bt_addr_arg = (bt_addr_arg_t *) arg_buffer;
    
    libc_memset(bt_addr_arg, 0, sizeof(bt_addr_arg_t));

    act_test_parse_test_arg(line_buffer, 1, &start, &end);

    //读取byte5, byte4, byte3数据，一般为蓝牙地址固定字节
    unicode_to_uint8_bytes(start, end, bt_addr_arg->bt_addr, 5, 6);
    
    return TRUE;
}

static int32 act_test_read_btplay_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start = 0;
    uint16 *end = 0;
    uint8 arg_num = 1;

    btplay_test_arg_t *btplay_arg = (btplay_test_arg_t *) arg_buffer;

    //if(arg_len < sizeof(btplay_test_arg_t))
    //{
    //    DEBUG_ATT_PRINT("argument too long", 0, 0);
    //    while(1);
    //}

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    //读取6字节的蓝牙地址
    unicode_to_uint8_bytes(start, end, btplay_arg->bt_transmitter_addr, 5, 6);

    //卡测试模式需要略过后面的5个蓝牙地址参数，取最后一个测试模式参数
    //PC测试模式，参数由PC分配，不需要进行处理
    if (g_test_mode == TEST_MODE_CARD)
    {
        arg_num += 5;

        //DEBUG_ATT_PRINT("byte0", btplay_arg->bt_transmitter_addr[0], 2);
        //DEBUG_ATT_PRINT("byte1", btplay_arg->bt_transmitter_addr[1], 2);
        //DEBUG_ATT_PRINT("byte2", btplay_arg->bt_transmitter_addr[2], 2);
        //DEBUG_ATT_PRINT("byte3", btplay_arg->bt_transmitter_addr[3], 2);
        //DEBUG_ATT_PRINT("byte4", btplay_arg->bt_transmitter_addr[4], 2);
        //DEBUG_ATT_PRINT("byte5", btplay_arg->bt_transmitter_addr[5], 2);
    }
    else
    {
        //    print_log("bt addr: %02x:%02x:%02x:%02x:%02x:%02x\n",  btplay_arg->bt_transmitter_addr[5], \
    //        btplay_arg->bt_transmitter_addr[4], btplay_arg->bt_transmitter_addr[3], \
    //        btplay_arg->bt_transmitter_addr[2], btplay_arg->bt_transmitter_addr[1], \
    //        btplay_arg->bt_transmitter_addr[0]);
    }

    //读取测试模式
    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    btplay_arg->bt_test_mode = unicode_to_int(start, end, 16);  

    if (g_test_mode != TEST_MODE_CARD)
    {
        //if (btplay_arg->bt_test_mode == 0)
        //{
            //测试时间90s
            act_test_change_test_timeout(90);    
        //} 
        //else   
        //{
        //    //测试时间90s
        //    act_test_change_test_timeout(90);
        //}
    }

    return TRUE;
}

static int32 act_test_read_gpio_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start = 0;
    uint16 *end = 0;

    gpio_test_arg_t *gpio_arg = (gpio_test_arg_t *) arg_buffer;

    //if(arg_len < sizeof(gpio_test_arg_t))
    //{
    //    DEBUG_ATT_PRINT("argument too long", 0, 0);
    //    while(1);
    //}

    act_test_parse_test_arg(line_buffer, 1, &start, &end);

    gpio_arg->gpioA_value = unicode_to_int(start, end, 16);

    act_test_parse_test_arg(line_buffer, 2, &start, &end);

    gpio_arg->gpioB_value = unicode_to_int(start, end, 16);

    act_test_parse_test_arg(line_buffer, 3, &start, &end);

    gpio_arg->gpioSIO_value = unicode_to_int(start, end, 16);

    return TRUE;
}

int32 act_test_read_mptest_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start = 0;
    uint16 *end = 0;
    uint8 arg_num;

    mp_test_arg_t *mp_arg = (mp_test_arg_t *) arg_buffer;

    //if(arg_len < sizeof(mp_test_arg_t))
    //{
    //    DEBUG_ATT_PRINT("argument too long", 0, 0);
    //    while(1);
    //}

    arg_num = 1;

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_channel_low = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_channel_mid = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_channel_high = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_test = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_index_low = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_index_high = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_index_changed = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_threshold_low = (int8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_threshold_high = (int8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_write_mode = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->cfo_upt_offset = (int32) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->pwr_test = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->pwr_threshold_low = (int8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    mp_arg->pwr_threshold_high = (int8) unicode_to_int(start, end, 10);

    return TRUE;
}

int32 act_test_read_btname_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *start = 0;
    uint16 *end = 0;
    uint8 arg_num;

    read_btname_test_arg_t *read_btname_arg = (read_btname_test_arg_t *) arg_buffer;

    //if(arg_len < sizeof(mp_test_arg_t))
    //{
    //    DEBUG_ATT_PRINT("argument too long", 0, 0);
    //    while(1);
    //}

    arg_num = 1;

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    read_btname_arg->cmp_btname_flag = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    unicode_to_utf8_str(start, end, read_btname_arg->cmp_btname, TEST_BTNAME_MAX_LEN);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    read_btname_arg->cmp_blename_flag = (uint8) unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    unicode_to_utf8_str(start, end, read_btname_arg->cmp_blename, TEST_BTBLENAME_MAX_LEN);

    return TRUE;
}

int32 act_test_read_ber_test_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16*  start = NULL;
    uint16*  end   = NULL;
    uint8 arg_num;
    ber_test_arg_t *ber_arg = (ber_test_arg_t *)arg_buffer;

    arg_num = 1;
    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    ber_arg->ber_channel_low = (uint8)unicode_to_int(start, end, 10);
    //print_log("channel low:%d", ber_arg->ber_channel_low);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    ber_arg->ber_channel_mid = (uint8)unicode_to_int(start, end, 10);    

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    ber_arg->ber_channel_high = (uint8)unicode_to_int(start, end, 10);  

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    ber_arg->ber_thr_low = (int8)unicode_to_int(start, end, 10);  

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    ber_arg->ber_thr_high = (int8)unicode_to_int(start, end, 10); 

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    ber_arg->rssi_thr_low = (int8)unicode_to_int(start, end, 10);  

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);

    ber_arg->rssi_thr_high = (int8)unicode_to_int(start, end, 10);    
    
    return TRUE;
}

int32 act_test_read_lradc_test_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16*  start = NULL;
    uint16*  end   = NULL;
    uint8 arg_num;
    lradc_test_arg_t *lradc_arg = (lradc_test_arg_t *)arg_buffer;

    arg_num = 1;

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc1_test = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc1_thr_low = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc1_thr_high = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc2_test = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc2_thr_low = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc2_thr_high = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc4_test = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc4_thr_low = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    lradc_arg->lradc4_thr_high = (uint8)unicode_to_int(start, end, 10);    
    
    return TRUE;
}

int32 act_test_read_channel_test_arg(uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint16*  start = NULL;
    uint16*  end   = NULL;
    uint8 arg_num;
    channel_test_arg_t *channel_test_arg = (channel_test_arg_t *)arg_buffer;

    arg_num = 1;

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->test_left_ch = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->left_ch_power_threadshold = (uint32)unicode_to_int(start, end, 10);    

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->test_left_ch_SNR = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->left_ch_SNR_threadshold = (uint32)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->left_ch_max_sig_point = unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->test_right_ch = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->right_ch_power_threadshold = (uint32)unicode_to_int(start, end, 10);    

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->test_right_ch_SNR = (uint8)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->right_ch_SNR_threadshold = (uint32)unicode_to_int(start, end, 10);

    act_test_parse_test_arg(line_buffer, arg_num++, &start, &end);
    channel_test_arg->right_ch_max_sig_point = (uint8)unicode_to_int(start, end, 10);

    return TRUE;
}


const att_task_read_arg_t autotest_readarg_ops[] =
{
    { TESTID_MODIFY_BTNAME, act_test_read_bt_name_arg },

    { TESTID_MODIFY_BLENAME, act_test_read_ble_name_arg },

    { TESTID_MODIFY_BTADDR, act_test_read_bt_addr_arg },
    
    { TESTID_MODIFY_BTADDR2, act_test_read_bt_addr_arg2 },

    { TESTID_BT_TEST, act_test_read_btplay_arg },

    { TESTID_GPIO_TEST, act_test_read_gpio_arg },

    { TESTID_GPIO_TEST_ATS2823, act_test_read_gpio_arg },

    { TESTID_MP_TEST, act_test_read_mptest_arg },

    { TESTID_MP_READ_TEST, act_test_read_mptest_arg },

    { TESTID_READ_BTNAME, act_test_read_btname_arg },

    { TESTID_BER_TEST, act_test_read_ber_test_arg},

    { TESTID_LRADC_TEST, act_test_read_lradc_test_arg},

    { TESTID_LINEIN_CH_TEST_ATS2825, act_test_read_channel_test_arg},

    { TESTID_LINEIN_CH_TEST_ATS2823, act_test_read_channel_test_arg},    

    { TESTID_MIC_CH_TEST, act_test_read_channel_test_arg},

    { TESTID_RSSI_TEST, act_test_read_ber_test_arg},    
};

int32 act_test_read_test_arg(uint16 test_id, uint16 *line_buffer, uint8 *arg_buffer, uint32 arg_len)
{
    uint32 i;

    //DEBUG_ATT_PRINT("test id:", test_id, 2);

    for (i = 0; i < sizeof(autotest_readarg_ops) / sizeof(att_task_read_arg_t); i++)
    {
        if (autotest_readarg_ops[i].test_id == test_id)
        {
            autotest_readarg_ops[i].read_arg_func(line_buffer, arg_buffer, arg_len);
            return TRUE;
        }
    }

    return FALSE;
}

int32 act_test_read_test_info(uint8 read_line, uint8 *test_id, uint8 *arg_buffer, uint32 arg_len)
{
    uint16 *line_buffer;

    line_buffer = act_test_read_cfg_file(read_line);

    //文件解析完毕，测试完成
    if (line_buffer == 0)
    {
        *test_id = 0xff;
        return;
    }

    *test_id = act_test_read_test_id(line_buffer);

    return act_test_read_test_arg(*test_id, line_buffer, arg_buffer, arg_len);
}
#pragma __PRQA_IGNORE_END__
