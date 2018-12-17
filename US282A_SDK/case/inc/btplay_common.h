/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌相关宏定义，枚举类型，结构体定义等，为蓝牙推歌前台和引擎共享。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef __BTPLAY_COMMON_H__
#define __BTPLAY_COMMON_H__

#include "psp_includes.h"
#include "common_btmanager.h"
#include "common_func.h"

typedef enum
{
    BTPLAY_IDLE  = 0,
    BTPLAY_STOP  = 1,
    BTPLAY_PLAY  = 2,
    BTPLAY_PAUSE = 3,
    //后续考虑添加快进退
} btplay_status_e;

typedef struct
{
    btplay_status_e status;
    uint8 reserve[3];//共享查询结构体需要4字节对齐
} btplay_info_t;

#endif
