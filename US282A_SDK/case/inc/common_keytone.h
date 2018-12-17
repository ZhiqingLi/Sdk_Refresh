/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：KEYTONE 相关的宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _common_keytone_H_
#define _common_keytone_H_

#include "psp_includes.h"
#include "case_independent.h"
#include "applib.h"
#include "setting_common.h"
#include "config_id.h"
#include "common_btmanager.h"
#include "app_view.h"

#define KEYTONE_KEYTONE        0  //按键音
#define KEYTONE_WARNING        1  //警告声，用于提示最大音量，最小音量等
#define KEYTONE_BTLINK         2  //蓝牙连接成功
#define KEYTONE_BTUNLINK       3  //蓝牙断开

#define KEYTONE_SAMPLE_RATE    8  //8K采样率
#define KEYTONE_FILELIST_MAX   4
#define KEYTONE_FILENAME_LEN   16

#define KEYTONE_NOBLOCK        0 //非阻塞模式，默认为非阻塞模式
#define KEYTONE_BLOCK          1 //阻塞模式

#define KEYTONE_FIFO_DEPTH     4

typedef struct
{
    uint8 count;
    dac_chenel_e dac_chan;
    uint8 dac_sample;
    uint8 enable:1;
    uint8 playing:1;
    uint8 thread_eixt:1;
    uint8 kt_mute:1;
    uint8 reserved:4;
    uint8 kt_fifo[KEYTONE_FIFO_DEPTH];
    uint8 *keytone_thread_addr;
} keytone_infor_t;

extern keytone_infor_t g_keytone_infor;
extern const uint8 g_keytone_filelist[KEYTONE_FILELIST_MAX][KEYTONE_FILENAME_LEN];

extern void keytone_set_dac_chan(dac_chenel_e dac_chan) __FAR__;
extern void keytone_set_dac_sample(uint8 dac_chan) __FAR__;
extern void keytone_set_on_off(bool on_off) __FAR__;
extern void keytone_play_deal(void) __FAR__;
extern void keytone_play_deal_wait(void) __FAR__;
extern bool keytone_play(uint8 kt_id, uint8 mode) __FAR__;
extern void com_start_key_tone(uint8 mode) __FAR__;

#endif
