/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：循环模式设置窗口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __MUSIC_LOOP_MODE_H__
#define __MUSIC_LOOP_MODE_H__

#include "case_include.h"

//循环模式的最小值
#define MIN_LOOP_MODE   (((g_enter_mode == PARAM_RECORD_CPLAY)\
    || (g_enter_mode == PARAM_RECORD_UPLAY)) ? FSEL_MODE_LOOPONE : FSEL_MODE_LOOPALL)

//循环模式的缺省值
#define DEF_LOOP_MODE   (((g_enter_mode == PARAM_RECORD_CPLAY)\
    || (g_enter_mode == PARAM_RECORD_UPLAY)) ? FSEL_MODE_LOOPONEDIR : FSEL_MODE_LOOPALL)

app_result_e create_view_loop_mode(void);

#endif//__MUSIC_LOOP_MODE_H__
