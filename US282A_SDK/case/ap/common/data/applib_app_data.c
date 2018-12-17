/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用内部 APPLIB 全局变量，所有前台应用都（应该）链接到相同的地址，以
 *       保证COMMON BANK 段代码是一致的；这些全局变量每个前台应用加载时都会初始化。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*!
 *  \brief
 *  thread_mutex 线程互斥信号量指针
 */
os_event_t *thread_mutex;

/*!
 *  \brief
 *  g_this_app_info 当前应用信息结构体指针
 */
const app_info_t *g_this_app_info;

/*!
 *  \brief
 *  g_app_timer_vector 应用软定时器数组指针，指向应用空间的软定时器数组
 */
app_timer_t *g_app_timer_vector;

/*!
 *  \brief
 *  g_app_timer_count 应用软定时器数目，为 0 表示应用不需要使用定时器
 */
uint8 g_app_timer_count = 0;
uint8 g_app_timer_id;

/*!
 *  \brief
 *  g_this_app_timer_tag 软定时器场景标志，AP场景为0
 */
uint8 g_this_app_timer_tag;

/*!
 *  \brief
 *  g_this_gui_msg_hook 应用私有消息勾函数
 */
gui_msg_hook g_this_gui_msg_hook;

/*! 
 *  \brief
 *  g_this_app_msg_hook 按键消息特殊处理勾函数
 */
app_msg_hook g_this_app_msg_hook;
