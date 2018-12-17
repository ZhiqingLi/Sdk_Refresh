/*******************************************************************************
 *                              us212A
 *                            Module: manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-10-8 11:21     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_config.c
 * \brief    应用配置脚本操作接口实现
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               实现了一套简单的应用配置接口，包括读取配置项等。
 * \par      EXTERNALIZED FUNCTIONS:
 *               直接调用sd 文件系统接口进行定位和读取。
 * \version 1.0
 * \date  2011-10-8
 *******************************************************************************/

#include  "manager.h"

uint32 get_true_trim_cap_value(uint32 cap_value, uint32 r_value)
{
    bool overflow_flag = 0;
    uint16 bc_value = 0;
    uint16 tc_value = 0;
    uint32 true_value = 0;
    bc_value = (uint16)((cap_value & (0x07 << 5)) >> 5);
    tc_value = (uint16)((cap_value & (0x1f << 0)) >> 0);
    true_value = bc_value * 30 + tc_value * 1;
    true_value = true_value * r_value / 10000;
    bc_value = (uint16)(true_value / 30);
    tc_value = (uint16)(true_value % 30);
    if(bc_value > 0x07)
    {
        overflow_flag = 1;
        bc_value = 0x07;        //bc电容最大值为07
    }
    if((bc_value == 0x04) || (bc_value == 0x05))
    {
        bc_value = 0x03;        //没有04,05对应的电容值
    }
    if(tc_value > 0x1f)
    {
        overflow_flag = 1;
        tc_value = 0x1f;        //tc电容最大值对应1f
    }
    if(overflow_flag != 0)
    {
        //libc_print("wrong value",0,0);  //系数过大
    }
    cap_value = (bc_value<<5) | (bc_value<<13) | (tc_value<<0) | (tc_value<<8);
    return  cap_value;

}

//HOSC 电容配置
void manager_config_hosc_freq(void)
{
    uint8 use_r_value = 0;
    int32 ret_val;
    int32 trim_cap_offset;
    uint32 trim_cap_value = 0;
    uint32 r_value = 0;
    uint32 adjust_value;   

    com_get_config_struct(USING_HARDWARE_FREQ_R_VALUE,(void *)&use_r_value,4);
    
    ret_val = base_trim_cap_read(&trim_cap_value, RW_TRIM_CAP_SNOR);

    if(ret_val == TRIM_CAP_READ_NO_ERROR)
    {
        ;//读取到正确的频偏数值
    }
    else if(ret_val == TRIM_CAP_READ_ADJUST_VALUE)
    {
        //读取到频偏校准值，需要乘以R值
        if(use_r_value == 1)
        {
            com_get_config_struct(SETTING_HARDWARE_FREQ_R_VALUE,(void *)&r_value,4);
            if(r_value != 0)
            {
                if(r_value > 0xffff)
                {
                    //参数无效
                    return;
                }
                trim_cap_value = get_true_trim_cap_value(trim_cap_value,r_value);
            }
            else
            {
                //参数无效
                return;
            }
        }
        //读取到频偏校准值，需要加上偏移量
        else if(use_r_value == 2)
        {
            com_get_config_struct(SETTING_HARDWARE_FREQ_COMPENSATION_OFFSET, (void*) &adjust_value, 4);

            if(adjust_value > 0x1FFFE)
            {
                //参数无效
                return;
            }

            //频偏偏移值范围为-65535-65535,为避免出现负数，將所有数值
            //先加上65535,所以真实的数值要减去65535
            trim_cap_value = trim_cap_value + adjust_value - 65535;

            if(trim_cap_value > 0x10000)
            {
                //参数无效
                return;
            }
        }
        else
        {
            com_get_config_struct(SETTING_HARDWARE_FREQ_COMPENSATION_DEFAULT_VAL, (void*) &trim_cap_value, 4);
        }
      
    }
 	
    else
    {
        com_get_config_struct(SETTING_HARDWARE_FREQ_COMPENSATION_DEFAULT_VAL, (void*) &trim_cap_value, 4);
    }

    //读取到正确的频偏数值
    libc_print("set hosc value:", trim_cap_value, 2);

    sys_set_hosc_param(trim_cap_value);    
}

