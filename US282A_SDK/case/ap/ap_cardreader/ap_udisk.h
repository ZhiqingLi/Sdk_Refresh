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

#ifndef _AP_UDISK_H
#define _AP_UDISK_H

#include  "psp_includes.h"
#include  "case_include.h"
#include "ud_interface.h"

//play界面显示元素标识

typedef void (*p_func_entry)(void);

//extern uint8 g_key_play;
extern int32 g_pop_up_timer;

extern uint32 g_pop_up_time_out;
//系统变量
extern comval_t g_comval;

extern const key_event_map_t udisk_ke_maplist[];

extern const sys_event_map_t udisk_se_maplist[];
extern uint8 g_reinstall_count;
extern uint8 g_removal_flag;

extern uint32 g_wait_time_before_enum;

#if (SUPPORT_OUTER_CHARGE == 1)
extern app_result_e ud_poweroff_msg_deal(void *ev_param) __FAR__;
#endif 

extern app_result_e ud_unstick_msg_deal(void *ev_param) __FAR__;

extern app_result_e ud_sdin_msg_deal(void *ev_param) __FAR__;

extern app_result_e ud_sdout_msg_deal(void *ev_param) __FAR__;

extern void udisk_create_main_view(void) __FAR__;

extern void ud_paint_deal(void) __FAR__;

extern app_result_e udisk_key_deal_quit_udisk(void) __FAR__;

extern bool _config_disk_label(void) __FAR__;

extern app_result_e get_message_loop(void) __FAR__;

#endif //_AP_UDISK_H
