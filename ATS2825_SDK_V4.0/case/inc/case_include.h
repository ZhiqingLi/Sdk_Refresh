/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 层所有公共模块头文件集合，前台应用和引擎应用都需要包含此头文件；COMMON
 *       不应该包含此头文件，而是根据自己需要精确选择所依赖模块的头文件。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _CASE_INCLUDE_H
#define _CASE_INCLUDE_H

#include "psp_includes.h"
#include "case_independent.h"
#include "enhanced.h"
#include "led_driver.h"
#include "key_interface.h"
#include "fm_interface.h"
#include "fmengine.h"

#include "app_msg.h"
#include "gui_msg.h"

#include "case_type.h"
#include "applib.h"
#include "common_func.h"
#include "common_rcp.h"
#include "setting_common.h"
#include "music_common.h"
#include "common_btmanager.h"
#include "common_time_alarm.h"

#include "config_id.h"
#include "vm_def.h"

#include "ccd_interface.h"
#include "bt_controller_interface.h"

#endif
