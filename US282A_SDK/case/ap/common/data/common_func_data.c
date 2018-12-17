/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用内部 COMMON 全局变量，所有前台应用都（应该）链接到相同的地址，以
 *       保证COMMON BANK 段代码是一致的；这些全局变量每个前台应用加载时都会初始化。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/*!
 *  \brief
 *  sys_comval 系统参数指针，通过 com_set_sys_comval 接口传递给 common 并保存在全局指针
 */
comval_t *sys_comval;

/*!
 *  \brief
 *  this_filter_key_value 过滤掉按键长按动作，要过滤的按键
 */
uint8 this_filter_key_value;

/*!
 *  \brief
 *  sys_counter_timer_id 系统定时功能（包括背光，屏保，省电关机，定时关机，等）计时器 timer id
 */
int8 sys_counter_timer_id;

/*!
 *  \brief
 *  peripheral_detect_timer_id 外设检测（包括USB，UHOST，CARD，AUX）定时器 timer id
 */
int8 peripheral_detect_timer_id;

/*!
 *  \brief
 *  是否需要忽略切换静音
 */
bool g_ignore_switch_mute;

bool g_detect_card_force_flag;

clock_show_var_t *g_p_clock_show;
set_calendar_var_t *g_p_set_calendar;
set_alarm_var_t *g_p_set_alarm;

view_infor_t g_view_infor[MVC_VIEW_DEEPTH];
view_manager_infor_t g_view_manager_infor;
key_event_t * g_p_view_ke;

low_power_var_t g_low_power_var;

standby_enter_hook g_standby_enter_cb;
standby_exit_hook g_standby_exit_cb;
bool g_standby_exit_flag;
bool g_esd_restart_flag;

bool g_need_auto_connect_timeout;  //用于重启controller后继续超时回连

restore_breakpoint_t g_ota_restore_data;//OTA记录在VRAM的数据

uint32 g_APKsend_pakg_num;
#ifdef __ESD_MODE_
uint8 g_end_autoconnt = 0;
#endif
bool g_need_reset_controller_timeout; //用于超时回连连接不上时重启controller
bool g_neednot_tts_play_timeout; //用于超时断开不播报TTS
bool g_neednot_tts_play_reset; //用于重启controller后不播报TTS


uint8  g_prev_next_ch_start=0;
int8  prev_next_ch_timer_id=0;
