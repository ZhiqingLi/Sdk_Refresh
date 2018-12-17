/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：所有前台应用共享的全局变量，必须保证所有前台应用都链接到相同的地址；这些
 *       变量会在ap manager 初始化阶段进行初始化，并且只会初始化一次，之后就一直
 *       常驻内存。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/*!
 *  \brief
 *  g_app_info_state 全局系统当前状态结构体，也会在 MANAGER 应用中访问部分变量
 */
app_info_state_t __section__(".g_app_info_state") g_app_info_state;

/*!
 *  \brief
 *  g_app_last_state 全局系统最近状态结构体
 */
app_last_state_t __section__(".gl_com_data") g_app_last_state;

/*!
 *  \brief
 *  g_sys_counter 系统计时器结构体
 */
sys_counter_t __section__(".gl_com_data") g_sys_counter;

/*!
 *  \brief
 *  g_buffer_gui_msg 缓冲gui消息
 */
input_gui_msg_t __section__(".gl_com_data") g_buffer_gui_msg;

/*!
 *  \brief
 *  g_buffer_app_msg 缓冲系统消息
 */
private_msg_t __section__(".gl_com_data") g_buffer_app_msg;

/*!
 *  \brief
 *  g_com_app_timer_vct COMMON 全局软定时器数组
 */
app_timer_t __section__(".gl_com_data") g_com_app_timer_vct[COM_APP_TIMER_MAX];

/*!
 *  \brief
 *  g_discrete_led_info COMMON 分立LED灯数组
 */
discrete_led_info_t __section__(".gl_com_data") g_discrete_led_info[LED_ID_MAX];
bool __section__(".gl_com_data") g_discrete_led_a23_available;

/*! 前台应用切换相关全局变量 */
ap_switch_var_t __section__(".gl_com_data") g_ap_switch_var;


OTA_malloc_param_t __section__(".gl_com_data") *g_OTA_var;
bool __section__(".gl_com_data")g_rcp_ctrl_quit_flag;

//ESD VRAM变量
g_config_var_t __section__(".gl_com_data")g_config_var;

//读卡器流程分支标志
bool __section__(".gl_com_data")g_esd_cardreader_flag;

uint8 __section__(".gl_rcp_buffer") *g_rcp_default_long_buffer;

