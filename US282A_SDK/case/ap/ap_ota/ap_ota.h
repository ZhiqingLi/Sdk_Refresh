/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟闹铃响及闹铃目录扫描功能相关宏定义，结构体定义，模块内全局变量声明，
 *       模块内外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef __APP_OTA_H__
#define __APP_OTA_H__

#include "psp_includes.h"
#include "case_include.h"
#include "music_common.h"
#include "enhanced.h"

/*!定义音乐应用的任务栈和主线程优先级*/
#define    AP_OTA_STK_POS  AP_FRONT_LOW_STK_POS
#define    AP_OTA_PRIO     AP_FRONT_LOW_PRIO

typedef app_result_e (*key_func)(void);

//系统全局变量
extern comval_t g_comval;

extern bool display_flag;
extern int8 display_timer_id;
extern uint8 g_ota_status;
extern bool g_need_draw;
extern uint8 g_tick;

enum
{
	OTA_STATUS_IDLE = 0,
	OTA_STATUS_CONNECT = 1,
	OTA_STATUS_UPGRADE = 2,
};

extern app_result_e get_message_loop(void) __FAR__;
extern void ota_main_view(view_update_e mode) __FAR__;
extern void ota_create_main_view(void) __FAR__;

extern const sys_event_map_t ota_se_maplist[];
extern const key_event_map_t ota_ke_maplist[];

#if (SUPPORT_LED_DRIVER == 1)
extern void wait_ota_display(void) __FAR__;
extern void ota_ui_display(void) __FAR__;
#endif

#endif //__APP_OTA_H__
