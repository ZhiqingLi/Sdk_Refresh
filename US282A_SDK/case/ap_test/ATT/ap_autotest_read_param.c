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

static uint32 _check_bt_addr_valid(uint8 *bt_addr)
{
    uint32 i;

    for (i = 0; i < 6; i++)
    {
        if ((bt_addr[i] != 0) && (bt_addr[i] != 0xff))
        {
            break;
        }
    }

    if (i == 6)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//该测试项用于读取蓝牙地址，提供给PC工具进行校验，确认地址是否正确
int act_test_read_bt_addr(void *arg_buffer)
{
    uint32 i;
    uint8 cmd_data[16];
    int ret_val;
    return_result_t *return_data;
    int trans_bytes;
    nvram_param_rw_t param_rw;
    uint32 bt_addr_valid;
   
    bt_addr_vram_t bt_addr_vram;

    bt_addr_valid = TRUE;
    
    if(g_support_norflash_wp == TRUE)
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
                bt_addr_valid = FALSE;
            }
        }
   }
   else
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
                bt_addr_valid = FALSE;    
            }            
        }
         
   }
    if(bt_addr_valid = TRUE)
    {
        if (_check_bt_addr_valid(bt_addr_vram.bt_addr) == TRUE)
        {
            ret_val = 1;
        }
        else
        {
            ret_val = 0;
        }
    }
    else
    {
        ret_val = 0;    
    }

    if (ret_val == 1)
    {
        for (i = 0; i < 6; i++)
        {
            cmd_data[i] = bt_addr_vram.bt_addr[i];
        }
        
        if(g_test_mode != TEST_MODE_CARD)
        {    
            print_log("bt addr: %x:%x:%x:%x:%x:%x\n", cmd_data[5], cmd_data[4], cmd_data[3], cmd_data[2], cmd_data[1],
                cmd_data[0]);
        }
        else
        {
            for (i = 0; i < 6; i++)
            {
                att_write_test_info("addr: ", cmd_data[5 - i], 1);
            }                
        }
    }
    else
    {
        libc_memset(cmd_data, 0, sizeof(cmd_data));
    }

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_READ_BTADDR;

        return_data->test_result = ret_val;

        trans_bytes = 0;

        //蓝牙地址转unicode形式
        bytes_to_unicode(cmd_data, 5, 6, return_data->return_arg, &trans_bytes);

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
        act_test_report_test_log(ret_val, TESTID_READ_BTADDR);
    }
    
    return ret_val;
}

static int cmp_bt_name(uint8 *bt_name0, uint8 *bt_name1, uint32 cmp_len)
{
    uint32 i;

    for (i = 0; i < cmp_len; i++)
    {
        if (bt_name0[i] != bt_name1[i])
        {
            break;
        }

        if (bt_name0[i] == '\0')
        {
            return TRUE;
        }
    }

    if (i < cmp_len)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
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
                //count += 2;
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

int act_test_read_bt_name(void *arg_buffer)
{
    uint32 i;

    uint32 ret_val;

    read_btname_test_arg_t *read_btname_arg;

    nvram_param_rw_t param_rw;

    bt_addr_vram_t bt_addr_vram;

    return_result_t *return_data;

    uint32 trans_bytes;

    uint32 count;
    
    uint32 bt_name_valid = TRUE;
    
    uint32 ble_name_valid = TRUE;

    read_btname_arg = (read_btname_test_arg_t *) arg_buffer;

    libc_memset(&bt_addr_vram, 0, sizeof(bt_addr_vram));
    
    if(g_support_norflash_wp == TRUE)
    {
        sys_vm_read(&bt_addr_vram, VM_BTSTACK, sizeof(bt_addr_vram));

        if (bt_addr_vram.magic != VRAM_MAGIC(VM_BTSTACK))
        {
            param_rw.logical_index = PARAM_BT_NAME;
            param_rw.rw_len = sizeof(bt_addr_vram.device_name);
            param_rw.rw_buffer = &(bt_addr_vram.device_name[0]);
    
            //读取蓝牙名称
            ret_val = base_param_read(&param_rw);
    
            if (ret_val != 0)
            { 
                bt_name_valid = FALSE;         
            }  
            param_rw.logical_index = PARAM_BT_BLENAME;
            param_rw.rw_len = sizeof(bt_addr_vram.ble_device_name);
            param_rw.rw_buffer = &(bt_addr_vram.ble_device_name[0]);

            //读取蓝牙BLE名称
            ret_val = base_param_read(&param_rw);

            if (ret_val != 0)
            {
                ble_name_valid = FALSE;
            }         
        }
    }
    else
    {
        param_rw.logical_index = PARAM_BT_NAME;
        param_rw.rw_len = sizeof(bt_addr_vram.device_name);
        param_rw.rw_buffer = &(bt_addr_vram.device_name[0]);

        //读取蓝牙名称
        ret_val = base_param_read(&param_rw);

        param_rw.logical_index = PARAM_BT_BLENAME;
        
        param_rw.rw_len = sizeof(bt_addr_vram.ble_device_name);
        param_rw.rw_buffer = &(bt_addr_vram.ble_device_name[0]);

        //读取蓝牙BLE名称
        ret_val += base_param_read(&param_rw);       

        if (ret_val != 0)
        {  
            sys_vm_read(&bt_addr_vram, VM_BTSTACK, sizeof(bt_addr_vram));
    
            if (bt_addr_vram.magic != VRAM_MAGIC(VM_BTSTACK))
            {
                bt_name_valid = FALSE; 
                ble_name_valid = FALSE;   
            }
        }          
    }
 
    ret_val = FALSE;
    
    if(bt_name_valid == TRUE)
    {
        if (bt_addr_vram.device_name[0] != 0)
        {
            if(ble_name_valid == TRUE)
            {    
                if (bt_addr_vram.ble_device_name[0] != 0)
                {
                    ret_val = TRUE;
                }
            }    
        }
    }

    if (ret_val == TRUE)
    {
        if(g_test_mode != TEST_MODE_CARD)
        {
            print_log("read bt name: %s\n", bt_addr_vram.device_name);
    
            print_log("read ble name: %s\n", bt_addr_vram.ble_device_name);
        }
        else
        {
            att_write_test_info("read bt name: ", bt_addr_vram.device_name, 1);
            
            att_write_test_info("read ble name: ", bt_addr_vram.ble_device_name, 1);        
        }
    }

    if (read_btname_arg->cmp_btname_flag == TRUE)
    {
        ret_val = cmp_bt_name(bt_addr_vram.device_name, read_btname_arg->cmp_btname, sizeof(bt_addr_vram.device_name));
    }

    if (ret_val == TRUE)
    {
        if (read_btname_arg->cmp_blename_flag == TRUE)
        {
            ret_val = cmp_bt_name(bt_addr_vram.ble_device_name, read_btname_arg->cmp_blename,
                    sizeof(bt_addr_vram.ble_device_name));
        }

    }

    if (g_test_mode != TEST_MODE_CARD)
    {
        trans_bytes = 0;
        
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_READ_BTNAME;

        return_data->test_result = ret_val;

        //utf8转unicode可能所需的buffer较多，极端情况下是之前字符串buffer的2倍
        //因此需要一个大一点的buffer缓存字符串
        utf8str_to_unicode(bt_addr_vram.device_name, sizeof(bt_addr_vram.device_name), return_data->return_arg, &count); 

        trans_bytes += (count >> 1);

        //添加参数分隔符','
        return_data->return_arg[trans_bytes++] = 0x002c;   

        //utf8转unicode可能所需的buffer较多，极端情况下是之前字符串buffer的2倍
        //因此需要一个大一点的buffer缓存字符串
        utf8str_to_unicode(bt_addr_vram.ble_device_name, sizeof(bt_addr_vram.ble_device_name), &(return_data->return_arg[trans_bytes]), &count);        

        trans_bytes += (count >> 1);
                
        //如果参数未四字节对齐，要四字节对齐处理
        if ((trans_bytes % 2) != 0)
        {
            return_data->return_arg[trans_bytes++] = 0x0000;
        }        
        act_test_report_result(return_data, trans_bytes * 2 + 4);
    }
    else
    {
        act_test_report_test_log(ret_val, TESTID_READ_BTNAME);
    }
    
    return ret_val;
}
