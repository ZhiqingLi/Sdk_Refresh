/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     ap_udevice.h
 * \brief    MainMenu的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#ifndef _AP_USOUND_H
#define _AP_USOUND_H

#include  "psp_includes.h"
#include  "case_include.h"
#include  "usound_common.h"

//play界面显示元素标识

#define up_usbflag           0x01   //usb
#define up_volflag           0x02   //vol
//-----------------------------清除刷新标志

#define clr_usbflag        0xfe   //清除磁盘拔出
#define clr_volflag        0xfd  //清除出错提示
//
extern uengine_status_t g_play_status;

extern uint32 g_pc_syn_flag;

//for manual set flag
extern uint32 g_manual_set;

extern uengine_sync_volume_t* g_volume_share;

//extern uint8 g_key_play;
//系统变量
extern comval_t g_comval;

extern bool g_usound_init_flag;

//play key flag 
extern uint32 g_play_key_flag;

//timer use 
extern int32 g_play_key_timer;

//for counter
extern uint32 g_last_time_count;

extern const key_event_map_t usound_ke_maplist[];

extern const sys_event_map_t usound_se_maplist[];

extern app_result_e _key_prev_deal(void) __FAR__;

extern app_result_e _key_next_deal(void) __FAR__;

extern app_result_e _key_play_deal(void) __FAR__;

extern app_result_e _key_vol_add_deal(void) __FAR__;

extern app_result_e _key_vol_sub_deal(void) __FAR__;

extern app_result_e ud_unstick_msg_deal(void *ev_param) __FAR__;

extern void ud_paint_deal(void) __FAR__;

extern bool usound_play_pause(void) __FAR__;

extern bool usound_play_next(void) __FAR__;

extern bool usound_play_prev(void) __FAR__;

extern bool usound_tts_start(void) __FAR__;

extern bool usound_tts_stop(void) __FAR__;

typedef struct
{
    uint8 usound_status;
    uint8 reserve[3];
} usound_rcp_info_t;

typedef struct
{
    com_rcp_info_t com_info;
    usound_rcp_info_t app_info;
} usound_global_rcp_info_t;

extern void usound_rcp_var_init(void) __FAR__;

extern void usound_rcp_var_exit(void) __FAR__;

extern app_result_e usound_get_global_rcp_info(uint32 data1, uint32 data2) __FAR__;

extern app_result_e usound_apk_key_play(uint32 data1, uint32 data2) __FAR__;

extern app_result_e usound_apk_key_pause(uint32 data1, uint32 data2) __FAR__;

extern app_result_e usound_apk_key_next(uint32 data1, uint32 data2) __FAR__;

extern app_result_e usound_apk_key_prev(uint32 data1, uint32 data2) __FAR__;

extern bool usound_get_status(uengine_status_t* play_status) __FAR__;

extern void usound_create_main_view(void) __FAR__;

extern app_result_e get_message_loop(void) __FAR__;

#endif
