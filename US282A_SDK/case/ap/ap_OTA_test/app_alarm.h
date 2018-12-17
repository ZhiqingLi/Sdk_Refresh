/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟闹铃响及闹铃目录扫描功能相关宏定义，结构体定义，模块内全局变量声明，
 *       模块内外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef __APP_ALARM_H__
#define __APP_ALARM_H__

#include "psp_includes.h"
#include "case_include.h"
#include "music_common.h"
#include "enhanced.h"
#include "storageio.h"

/*!定义音乐应用的任务栈和主线程优先级*/
#define    AP_ALARM_STK_POS  AP_FRONT_LOW_STK_POS
#define    AP_ALARM_PRIO     AP_FRONT_LOW_PRIO

typedef app_result_e (*key_func)(void);

//系统全局变量
extern comval_t g_comval;

extern int32 g_fs_mount_id;
extern uint32 g_dir_file_total;
extern file_path_info_t g_alarm_path;

extern time_t g_next_alarm_time;
extern uint8 g_next_alarm_index;
extern bool g_has_next_alarm;
extern bool g_next_alarm_already_ring;

extern bool g_rcp_ctrl_quit_flag;

typedef struct
{
    date_t alarm_date;
    time_t alarm_time;
    bool overtime_flag;
    bool twinkling_flag;
    int8 twinkling_timer_id;
    int8 alarm_overtime_timer_id;
    alarm_record_t cur_alarm_record;
} alarm_ring_var_t;

extern alarm_ring_var_t g_alarm_ring_var;
extern alarm_ring_var_t *g_p_alarm_ring;
extern uint32 g_display_sec;

extern app_result_e alarm_ring_entry(void) __FAR__;
extern bool alarm_fsel_init(uint8 disk_type, uint8 ring_type, uint16 ring_file_no) __FAR__;
extern bool alarm_play_init(alarm_ring_var_t *alarm_ring_var) __FAR__;
extern bool alarm_play_deinit(void) __FAR__;
extern bool alarm_check_status(void) __FAR__;

extern void alarm_scan_ring_folder(void) __FAR__;

extern int32 alarm_fs_init(uint8 disk_type) __FAR__;
extern bool alarm_fs_deinit(uint8 disk_type, int32 file_sys_id) __FAR__;

#endif //__APP_ALARM_H__
