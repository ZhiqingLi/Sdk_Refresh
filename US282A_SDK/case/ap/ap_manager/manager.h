/*******************************************************************************
 *                              us212a
 *                            Module: manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-06 15:00     1.0             build this file
 *******************************************************************************/
/*
 * \file     manager.h
 * \brief    manager应用定义
 * \author   zhangxs
 * \version 1.0
 * \date  2011/09/04
 *******************************************************************************/
#ifndef _MANAGER_H
#define _MANAGER_H

#include  "psp_includes.h"
#include  "case_include.h"

typedef struct
{
    uint8 front_ap_id;
    uint8 ap_switch_flag;
}test_share_info_t;

extern test_share_info_t *g_p_test_share_info;
extern uint8 prev_back_ap_id;
extern uint8 prev_front_ap_id;
extern uint8 *pa_thread_task_addr;
extern mem_use_info_t *g_mem_use_info;
extern uint8 support_dev_num;
extern void manager_msg_callback(private_msg_t *pri_msg) __FAR__;
extern void _get_app_name(char *namebuf, uint8 ap_id, uint8 app_type) __FAR__;

extern void * pa_thread_open(void* param);
extern void wait_ap_exit(void) __FAR__;
extern void kill_app(uint8 ap_id, uint8 ap_param) __FAR__;
 
extern bool globe_data_init_applib(void);

extern void ft_test_mode(void);

extern void manager_config_hosc_freq(void) __FAR__;

extern void install_key_drv(void);

extern void manager_mem_manager(uint32 app_id, uint32 exec_ap) __FAR__;

extern void manager_cache_memory_init(void) __FAR__;

#endif

