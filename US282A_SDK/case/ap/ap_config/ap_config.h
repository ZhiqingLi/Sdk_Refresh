/*******************************************************************************
 *                              us212a
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-06 15:00     1.0             build this file
 *******************************************************************************/
/*
 * \file     ap_config.h
 * \brief    config应用的头文件
 * \author   zhangxs
 * \version 1.0
 * \date  2011/09/04
 *******************************************************************************/
#ifndef _AP_CONFIG_H
#define _AP_CONFIG_H

#include  "psp_includes.h"
#include  "case_include.h"
#include  "card.h"

#define SWITCH_ON           0 //开机
#define SWITCH_OFF          1 //关机
#define SWITCH_LOWPOWER     2 //低电关机
#define SWITCH_STANDBY      3 //超低功耗模式
#define SWITCH_IDLE_MODE    4 //外部充电方案的空闲模式
#define POWER_ON            5 //ap_manager传参，上电开机

typedef struct
{
    uint16 active; //此参数没用到，只是保持与config.bin中的数据结构一致
    uint16 total; //按键总数
    uint16 keyID[16]; //按键定义数组
} cfg_key_var_t;

typedef struct
{
    uint16 active_item;//活动AP菜单项编号
    uint16 total_item; //AP菜单总数
    uint16 ap_funcs[MAX_FUNCTON_CYCLE]; //支持应用编号的数组
} cfg_loop_func_var_t;

/*wav录音掉电回写文件头结构体*/
typedef struct
{
    /*长名*/
    uint8 rec_filename[28];
    /*文件偏移信息*/
    pfile_offset_t ptr_file_offset;
    /*已录文件大小*/
    uint32 file_bytes;
    /*录音比特率*/
    uint32 bitrate;
    /*目录信息,73byte,需要做对齐*/
    pdir_layer_t ptr_layer;
    /*盘符信息*/
    uint8 disk_no;
    /*录音格式，0-wav, 1-mp3*/
    uint8 rec_format;
    /*需要重写标志*/
    bool rewrite_need;
} record_writehead_t;

extern record_writehead_t g_writehead;
extern comval_t g_comval;
extern bool usb_in_flag;
extern bool uhost_in_flag;
extern bool adaptor_in_flag;
extern bool sd_in_flag;
extern bool linein_in_flag;
extern bool rtcalarm_flag;
extern bool need_lowpower_tts_flag;
extern bool g_config_standby_exit;
extern bool g_config_bt_flag;
extern bool g_config_esd_restart;
extern bool g_config_volume_init;

extern uint8 g_uhost_delay;

extern app_result_e config_charging(int param) __FAR__;
extern app_result_e check_enter_test_mode(void) __FAR__;
extern void en_play_wake_up(bool standby_enter) __FAR__;
extern app_result_e sys_power_off(bool standby_enter) __FAR__;
extern app_result_e config_poweron_option(void) __FAR__;
extern void config_flush_rtc_reg(void) __FAR__;

extern app_result_e config_idle_mode_loop(void) __FAR__;

extern bool detect_power_on(void) __FAR__;
extern void config_record_writehead(void) __FAR__;
extern app_result_e open_stub(void) __FAR__;

extern void card_set_drv_power(void) __FAR__;
extern int config_key_module(void) __FAR__;
extern void config_sys_init(void) __FAR__;
extern void config_sys_init_display(void) __FAR__;

extern app_result_e load_att_code(int32 file_sys_id, uint8 stg_base) __FAR__;
extern OTA_malloc_param_t *g_OTA_var __FAR__;

extern void config_power_on_dispatch(app_result_e result) __FAR__;
extern g_config_var_t g_config_var;


#endif

