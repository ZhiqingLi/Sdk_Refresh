/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：APPLIB 全局变量，所有应用（包括ap manager，前台应用，引擎应用，BT STACK）
 *       共享的全局变量，必须保证所有应用都链接到相同的地址；这些变量会在ap manager
 *       初始化阶段进行初始化，并且只会初始化一次，之后就一直常驻内存。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*!
 *  \brief
 *  g_app_info_vector 全局应用信息结构体数组
 */
app_info_t __section__(".applib_globe_data.g_app_info_vector") g_app_info_vector[MAX_APP_COUNT];

/*!
 *  \brief
 *  config_fp 配置脚本文件句柄
 */
sd_handle __section__(".applib_globe_data.config_fp") config_fp;

/*!
 *  \brief
 *  g_app_info_state_all 全局系统当前状态结构体，存放在系统全局空间
 *  NOTE!!! 必须确保 app_info_state_all_t 前面部分成员不变
 */
app_info_state_all_t __section__(".applib_globe_data.g_app_info_state_all") g_app_info_state_all;

uint32 __section__(".applib_globe_data.g_app_info_state_all") g_customer_state;//预留客户全局配置项
bool __section__(".applib_globe_data.g_app_info_state_all")g_neednot_tts_play_timeout1;//规避LT_ADDR


