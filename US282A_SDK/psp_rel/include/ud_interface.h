/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2013-01-18 16:40     1.0                build this file
 ********************************************************************************/
#ifndef __USOUND_INTERFACE_H__
#define __USOUND_INTERFACE_H__

#include "typeext.h"

typedef void* (*ud_op_i)(void*, void*, void*);

typedef enum
{
    CMD_GET_STATUS = 0, //sratus
    CMD_INIT, //init start
    CMD_EXIT, //init exit
    CMD_SET_CFG, //some config data set
    CMD_GET_INFO,//iso data get use
    CMD_SET_INFO,//iso data set use
    CMD_SET_CMD, //can be extension
    CMD_HID_DEAL,
//hid deal
} ud_cmd_e;

typedef struct
{
    ud_op_i get_stauts;
    ud_op_i module_start;
    ud_op_i module_stop;
    ud_op_i set_config;
    ud_op_i get_info;
    ud_op_i set_info;
    ud_op_i set_cmd;
    ud_op_i hid_deal;
} usound_operations_t;//audio up and down or audio down only


typedef struct
{
    ud_op_i get_stauts;
    ud_op_i module_start;
    ud_op_i module_stop;
    ud_op_i set_config;
    ud_op_i msc_send;
    ud_op_i msc_recive;
    ud_op_i set_cmd;
    ud_op_i msc_deal;
} ureader_operations_t;

typedef struct
{
    ud_op_i get_stauts;
    ud_op_i module_start;
    ud_op_i module_stop;
    ud_op_i set_config;
    ud_op_i hid_get;
    ud_op_i hid_set;
    ud_op_i set_cmd;
    ud_op_i hid_deal;
} uhid_operations_t;

//for comunication
typedef struct
{
    uint8 set_sample;//sample flag
    uint8 set_adc_sample;//adc sample flag
    uint8 set_dac_sample;//dac sample flag
    uint8 no_empty_fla;
    uint8 data_in_flag;
    uint16 line_sta;//if is insert
    uint16 switch_flag;//if need switch_to stub 
    uint32 sample_rate;//rate
    uint32 adc_sample_rate;//adc rate
    uint32 dac_sample_rate;//dac rate
    uint32 start_play;//if is start play
    uint32 start_record;//if is start record
    uint32 volume_chg;//if voluem syn
    uint32 volume_value;//syn volume value
    uint32 hid_idle_time;//hid idle time
    uint32 uspeaker24_energy;
} usound_status_t;

//for comunication
typedef struct
{
    uint32 line_sta;//if is insert
    uint32 card_sta;//if card is exist
    uint32 suspend_flag;//if is suspend now
    uint32 switch_to_adfu;//need to switch toa adfu
    uint32 reset_count;//reset times
    uint32 remove_card;//need remove card
    uint32 pop_out;
} ureader_status_t;

//for comunication
typedef struct
{
    uint32 line_sta;//if is insert
} uhid_status_t;

//for commond set
typedef enum
{
    SET_PLAY_FLAG = 0,//play status set
    SET_HID_OPERS, //cmd use
    SET_SAMPLE_RATE, //sample set
    SET_ADJUST_TIMER, //adjust
    SET_LINE_STATUS,//status
    SET_VOLUME_FLAG,//volume syn flag
    SET_HID_CHANGE,//hid need report
    SET_CARD_INOUT,
    CLR_REMOVAL,
    SET_TTS_FLAG,
    SET_USPEAKER_24BIT,
    FIX_SAMPLE_RATE,
    SET_VOLUME,
    CLR_SWITCH_FLAG,
    SET_MUTE_FLAG,
//deal card pull plug
} usound_set_cmd_e;

//for 24bit usb speaker
typedef enum
{
    USPEAKER24_PLAY = 0,
    USPEAKER24_PAUSE,
    USPEAKER24_SAMPLE_RATE,
    USPEAKER24_TTS_START,
    USPEAKER24_TTS_STOP,
} uspeaker_24bit_set_cmd_e;

/*3.对上层提供的总接口函数*/
extern void * ud_op_entry(void *param1, void *param2, void *param3, ud_cmd_e cmd)__FAR__;

/*获取当前usb状态函数原型 */
/*int usound_inner_get_status(usound_status_t *pstatus)*/
/*int ureader_inner_get_status(ureader_status_t *pstatus)*/
/*int uhid_inner_get_status(uhid_status_t *pstatus)*/
#define ud_get_status(a)       ud_op_entry((void*)(a), (void*)(0), (void*)(0), CMD_GET_STATUS)

/*启动usb模块,注册中断等初始化操作，USOUND驱动会返回 usound_isr_cbk  */
#define ud_module_start(p_usound_isr_cbk)      ud_op_entry((void*)(p_usound_isr_cbk), (void*)(0), (void*)(0), CMD_INIT)

/*退出usb模块,注销中断、释放资源等  */
#define ud_module_stop()       ud_op_entry((void*)(0), (void*)(0), (void*)(0), CMD_EXIT)

/*设置usb的配置项  cfg_index --modify项 */
/*int usound_inner_set_cfg(uint32 cfg_index, uint32* pcfg, uint32 cfg_len)*/
#define ud_set_config(a,b,c)   ud_op_entry((void*)(a), (void*)(b), (void*)(c), CMD_SET_CFG)

/*获取usb信息,暂未使用 */
#define ud_get_info(a,b,c)     ud_op_entry((void*)(a), (void*)(b), (void*)(c), CMD_GET_INFO)

/*配置usb,暂未使用 */
#define ud_set_info(a,b,c)     ud_op_entry((void*)(a), (void*)(b), (void*)(c), CMD_SET_INFO)

/*预留暂未使用 */
#define ud_msc_send(a,b)       ud_op_entry((void*)(a), (void*)(b), (void*)(0), CMD_GET_INFO)

/*预留暂未使用 */
#define ud_msc_receive(a,b)    ud_op_entry((void*)(a), (void*)(b), (void*)(0), CMD_SET_INFO)

/*设置 cmd 见usound_set_cmd_e定义*/
/*int ureader_inner_set_cmd(uint32 cmd, uint32 cmd_param)*/
#define ud_set_cmd(a,b)        ud_op_entry((void*)(a), (void*)(b), (void*)(0), CMD_SET_CMD)

/*预留暂未使用 */
#define ud_hid_deal()          ud_op_entry((void*)(0), (void*)(0), (void*)(0), CMD_HID_DEAL)

/*预留暂未使用 */
#define ud_msc_deal()          ud_op_entry((void*)(0), (void*)(0), (void*)(0), CMD_HID_DEAL)

/*预留暂未使用 */
#define ud_hid_get()           ud_op_entry((void*)(0), (void*)(0), (void*)(0), CMD_GET_INFO)

/*预留暂未使用 */
#define ud_hid_set()           ud_op_entry((void*)(0), (void*)(0), (void*)(0), CMD_SET_INFO)

#endif  //__USOUND_INTERFACE_H__
