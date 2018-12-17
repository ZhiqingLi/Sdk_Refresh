/********************************************************************************
 *                 Copyright(c) 2003-2015 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙免提相关的宏、枚举类型、结构体等定义
 * 作者：cailizhen
 ********************************************************************************/

#ifndef __BTCALL_COMMON_H__
#define __BTCALL_COMMON_H__

#include "psp_includes.h"
#include "common_btmanager.h"
#include "common_func.h"

#define CALLIN_RING_MODE_NORMAL   0 //正常模式，仅有铃声
#define CALLIN_RING_MODE_ONLY_PB  1 //仅有电话本
#define CALLIN_RING_MODE_RING_PB  2 //铃声+电话本

typedef enum
{
    BTCALL_IDLE    = 0,
    BTCALL_STOP    = 1,
    BTCALL_CALLIN  = 2,
    BTCALL_CALLOUT = 3,
    BTCALL_HFP     = 4,
    BTCALL_PHONE   = 5,
	BTCALL_SIRI    = 6,
} btcall_status_e;

typedef struct
{
    btcall_status_e status;
    uint8 reserve[3];//共享查询结构体需要4字节对齐
} btcall_info_t;

#endif
