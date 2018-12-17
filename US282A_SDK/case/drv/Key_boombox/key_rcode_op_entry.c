
/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                             All Rights Reserved.
 *      Description:接口
 *      <author>       <time>           
 *       Wekan       2015-3-26pm            
 *******************************************************************************/

#include "key_inner.h"

typedef void* (*key_op_i)(void*, void*, void*);
/*1.驱动内部实现的具体接口函数声明*/
typedef struct 
{
    key_op_i __key_battery_charge_open;
    key_op_i __key_battery_charge_stop;
    key_op_i __key_battery_get_status;
    key_op_i __key_adjust_freq_op;
    key_op_i __key_peripheral_detect;
    key_op_i __key_register_scan_isr_cbk;
    key_op_i __key_discrete_led_op;
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
    key_op_i __key_extern_tk_op;  
#endif
    key_op_i __key_set_effect_param;
    key_op_i __key_get_feature_info;
    key_op_i __key_set_waves_effect_param;
    key_op_i __key_address_translation;
}block_key_operations;

/*2.定义接口操作集结构体，并进行初始化*/
block_key_operations blk_op =
{
    (key_op_i)key_inner_battery_charge_open,
    (key_op_i)key_inner_battery_charge_stop,
    (key_op_i)key_inner_battery_get_status,
    (key_op_i)adjust_freq_op_entry,
    (key_op_i)key_inner_peripheral_detect_handle,
    (key_op_i)key_inner_register_scan_isr_cbk,
    (key_op_i)key_inner_discrete_led_op,
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
    (key_op_i)key_inner_extern_tk_op, 
#endif
    (key_op_i)key_inner_set_effect_param,
    (key_op_i)key_inner_get_feature_info,
    (key_op_i)key_inner_set_waves_effect_param,
    (key_op_i)key_inner_address_translation,
};


