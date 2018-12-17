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

int32 byte_to_unicode(uint8 byte_value, uint16 *unicode_buffer)
{
    if((byte_value >= 0) && (byte_value <= 9))
    {
        byte_value = (byte_value + '0');    
    }
    else if((byte_value >= 10) && (byte_value <= 15))
    {
        byte_value = byte_value - 10 + 'A';
    }
    else
    {
        byte_value = 0;
    }  

    *unicode_buffer = byte_value; 

    return 1;
}

int32 two_bytes_to_unicode(uint8 byte_value, uint16 *unicode_buffer, uint32 base)
{
    uint8 temp_value;
    uint32 i;
    int32 trans_len = 0;

    for(i = 0; i < 2; i++)
    {
        if(i == 0)
        {
            temp_value = byte_value / base;
        }
        else
        {
            temp_value = byte_value % base; 
        }

        trans_len++;
    
        byte_to_unicode(temp_value, &(unicode_buffer[i]));
    }

    return trans_len;
}

void bytes_to_unicode(uint8 *byte_buffer, uint8 byte_index, uint8 byte_len, uint16 *unicode_buffer, uint16 *unicode_len)
{
    uint32 i;

    int32 trans_len;
    
    for(i = 0; i < byte_len; i++)
    {
        trans_len = two_bytes_to_unicode(byte_buffer[byte_index - i], unicode_buffer, 16);

        unicode_buffer += trans_len;

        *unicode_len += trans_len;
    }

    return;
}


void uint32_to_unicode(uint32 value, uint16 *unicode_buffer, uint16 *unicode_len, uint32 base)
{
    uint32 i;
    uint32 trans_byte;
    uint8 temp_data[12];
    uint32 div_val;

    i = 0;

    trans_byte = 0;

    div_val = value;
    
    while(div_val != 0)
    {
        temp_data[i] = div_val % base;
        div_val = div_val / base;
        i++;
    }
    
    while(i != 0)
    {
        trans_byte = byte_to_unicode(temp_data[i-1], unicode_buffer);
        unicode_buffer += trans_byte;
        *unicode_len += trans_byte;
        i--;
    }

    return; 
}

void int32_to_unicode(int32 value, uint16 *unicode_buffer, uint16 *unicode_len, uint32 base)
{
    uint32 i;
    uint32 trans_byte;
    uint8 temp_data[12];
    uint32 div_val;

    if(value == 0)
    {
        *unicode_buffer = '0';
        *unicode_len += 1;
        return;
    }

    i = 0;
    trans_byte = 0;

    if(value < 0)
    {
        *unicode_buffer = '-';
        unicode_buffer++;
        *unicode_len += 1;
        value = 0 - value;
    }

    div_val = value;
    
    while(div_val != 0)
    {
        temp_data[i] = div_val % base;
        div_val = div_val / base;
        i++;
    }
    
    while(i != 0)
    {
        trans_byte = byte_to_unicode(temp_data[i-1], unicode_buffer);
        unicode_buffer += trans_byte;
        *unicode_len += trans_byte; 
        i--;
    }

    return; 
}


int32 act_test_report_result(uint8 *write_data, uint16 payload_len)
{
    int ret_val;
    uint8 cmd_data[8];
    
    stub_ext_param_t ext_param;

    ((return_result_t*)write_data)->timeout = 5;  // 获取测试项超时
    
    while(1)
    {
        //ext_param.opcode = STUB_CMD_ATT_REPORT_TRESULT;
        //ext_param.payload_len = payload_len;
        //ext_param.rw_buffer = write_data; 
        
        //ret_val = stub_ext_write(&ext_param);
        
        ret_val = att_write_data(STUB_CMD_ATT_REPORT_TRESULT, payload_len, write_data);
          
        if(ret_val == 0)
        {   
            //ext_param.payload_len = 0;
            //ext_param.rw_buffer = cmd_data;
            
            //ret_val = <i>stub_ext_read</i>(&ext_param);
            ret_val = att_read_data(STUB_CMD_ATT_REPORT_TRESULT, 0, cmd_data);
        
            if(ret_val == 0)
            {
                //确认是ACK数据
                if (cmd_data[1] == 0x04 && cmd_data[2] == 0xfe)
                {
                    break;
                }       
            }
        }
    }

    return ret_val;
}



