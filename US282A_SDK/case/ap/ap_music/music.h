/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：music相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __MUSIC_H__
#define __MUSIC_H__

#include "psp_includes.h"
#include "case_include.h"
#include "music_common.h"
#include "enhanced.h"
#include "music_ab.h"
#include "music_common_inner.h"
#include "music_digit_song.h"
#include "music_hint.h"
#include "music_loop_mode.h"
#include "music_play.h"
#include "music_play2.h"
#include "music_rcp.h"
#include "music_song_number.h"

/*!定义音乐应用的任务栈和主线程优先级*/
#define AP_MUSIC_STK_POS    (AP_FRONT_LOW_STK_POS)
#define AP_MUSIC_PRIO       (AP_FRONT_LOW_PRIO)

//注意：这个宏定义不能跟common_func.h里面的TTS_xxx(如TTS_ENTER_BT)重复
#define TTS_SONG_SEQ        (0x80)//曲目号
#define TTS_INVALID         (0xff)

//如果感觉看map不方便，请如下定义
#define MUS_STATIC
//#define MUS_STATIC          static

#define MIN(a, b)           (((a) > (b)) ? (b) : (a))
#define MAX(a, b)           (((a) < (b)) ? (b) : (a))

//各view的ID号(注意不要与APP_VIEW_ID_MAIN和common用的ID号重复)
typedef enum
{
    VIEW_ID_SONG_NUMBER = 1,
    VIEW_ID_LOOP_MODE,
    VIEW_ID_DIGIT_SONG,
    #ifdef AB_SUPPORT
    VIEW_ID_AB,
    #endif
    VIEW_ID_HINT,
}
music_view_id_e;

typedef struct
{
    /*! 魔数 */
    uint16 magic;
    /*! 循环模式 */
    fsel_mode_e repeat_mode;
    /*! shuffle 开关 取高4bit*/
    uint8 shuffle_flag;
   /*! 当前歌曲总时间*/
    int total_time;
} music_config_t;

//进入music的方式
extern app_param_e g_enter_mode;

//初始化阶段，可以过滤掉一些事件处理
extern bool g_musicplay_init_flag;

//主动切歌到第几首
extern uint16 g_manul_switch_song_seq;

//当前播放状态信息；当前播放时间和比特率信息
extern mengine_info_t g_music_info;

//music ui的配置项信息
extern music_config_t g_music_config;

//系统全局变量
extern comval_t g_comval;

//保存扫描线程句柄
extern void *g_scan_handle;

//扫描标记
extern uint8 g_scan_flag;

//rcp save some file switch info
extern rcp_file_status_t g_rcp_file_status;

#ifdef PRINT_BANK_INFO
extern int print_label;
#endif

extern uint8 g_seq_tts_sec_ids[10];

//rcp通讯中得到播放列表时的定时器
extern int8 g_timer_id_rcp_getplist;

extern app_result_e music_get_dir_list_status(uint32 data1, uint32 data2) __FAR__;
extern app_result_e music_get_music_dir_list(uint32 data1, uint32 data2) __FAR__;
extern bool get_dir_by_dirno(uint16 dir_no, list_dir_info_t *p_dir) __FAR__;
extern void update_dir_list_random(void) __FAR__;

extern bool g_music_restore_from_s3bt;

#endif //__MUSIC_H__
