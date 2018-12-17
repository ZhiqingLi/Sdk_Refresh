/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：radio相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __APP_RADIO_H__
#define __APP_RADIO_H__

#include "psp_includes.h"
#include "case_include.h"

#include "fmengine.h"

#define _DEBUG      (0)

#define APP_TIMER_COUNT     2

/* 定义预设电台列表中所支持的保存电台数，需开出相应大小buffer*/
#define MAX_STATION_COUNT   40

/* 定义各波段最小频点*/
//单位kHz
#define MIN_US      (87500) //87500 = 0x155cc
#define MIN_JP      (76000) //76000 = 0x128E0
#define MIN_EU      (87500) //87500 = 0x155cc
/* 定义各波段最大频点*/
#define MAX_US      (108000) //108000 = 0x1a5e0
//#define MAX_JP    (91000) //91000 = 0x16378
#define MAX_JP      (90000) //90000 = 0x15f90
#define MAX_EU      (108000) //108000 = 0x1a5e0
#define SELECT_PREV_DIRECTION   (0)
#define SELECT_NEXT_DIRECTION   (1)

//radio AP场景定义
typedef enum
{
    /* FM 播放场景*/
    FM_SCENE_PLAYING,
    /* FM 搜台场景 */
    FM_SCENE_SEARCH,
    /* 场景退出*/
    FM_SCENE_EXIT,
} radio_scene_e;

//进入voice 的入口
typedef enum
{
    //默认初始状态，未进入voice
    NO_ERTER_VOICE = 0,
    //从radio 主菜单选择FM 录制进入voice
    FROM_RADIO_MAINMENU,
    //从radio 的option 子菜单选择FM 录制进入voice
    FROM_OPTION_MENU
} enter_voice_mode_e;

//radio ui配置参数
typedef struct
{
    /* 魔术数*/
    uint16 magic;
    /* 预设列表( 普通频段)*/
    uint32 fmstation_us[MAX_STATION_COUNT];
    /* 预设列表( 日本频段)*/
    uint32 fmstation_jp[MAX_STATION_COUNT];
    /* 预设列表( 欧洲频段)*/
    uint32 fmstation_eu[MAX_STATION_COUNT];
    /* 电台信息( 普通频段)*/
    fm_play_status_t FMStatus_us;
    /* 电台信息( 日本频段)*/
    fm_play_status_t FMStatus_jp;
    /* 电台信息( 欧洲频段)*/
    fm_play_status_t FMStatus_eu;
    /* 当前播放的电台信息*/
    fm_play_status_t FMStatus;
    /* 当前播放波段, Band_MAX 表示当前播放为用户电台*/
    radio_band_e band_mode;
    /* 进入放音的入口，决定了返回到radioUI 的入口*/
    enter_voice_mode_e enter_voice;
    /* 配置模组采用的搜台模式( 默认模式可通过配置项配置)	*/
    FM_SeekMode_e seek_mode;
    /* 搜台提示方式 */
    uint8 hint_mode_cfg;
    /*! 按键支持个数 */
    uint8 key_number;
} radio_config_t;

extern app_timer_t radio_app_timer_vector[];

//for alarm_fm
//radio进入模式
extern uint8 g_radio_from_alarm;
// alarm_fm的结束时间
extern time_t alarmfm_end_time;

//alarm_fm音量
extern uint8 alarmfm_volume;

//全局变量声明
extern radio_scene_e g_radio_scene;

extern radio_config_t g_radio_config;

extern uint32 *pfreq;

extern uint32 Auto_tab[MAX_STATION_COUNT];

extern Engine_Status_t g_engine_status;

//系统全局变量
extern comval_t g_comval;

extern const uint32 max_freq[];

extern const uint32 min_freq[];

extern uint32 startfreq;

//存储数字输入的buffer
extern uint8 g_digital_buf[MAX_DIGITAL_COUNT];

//当前数字健的个数
extern uint8 g_digital_count;

//搜台门限值
extern uint8 g_search_throd;

//是否从录音返回
extern app_param_e g_enter_param;

//KEY_NEXT_VOLADD 和 KEY_PREV_VOLSUB 长按调音量标志
extern bool g_next_prev_flag;

//for RCP
//是否正在搜台中
extern bool g_searching_flag;
//FM收音是否在播放
extern bool g_playing_flag;

//for set release mute timer
extern int8 g_mute_set_timer;

//函数声明

//需要过滤的频点(因为方案本身的辐射)
bool should_be_passed(uint32 freq) __FAR__;
uint32 get_start_freq(FM_SeekDir_e dir) __FAR__;
void show_error(void) __FAR__;

extern int radio_send_sync_msg(uint8 app_id, msg_apps_t *msg, msg_reply_t *reply, uint32 timeout) __FAR__;
extern bool radio_send_msg(void *input_param, void *output_param, uint32 msg_type) __FAR__;

extern bool radio_set_mute(FM_MUTE_e mode) __FAR__;

extern bool radio_set_band(radio_band_e mode) __FAR__;

extern bool radio_set_freq(uint32 freq) __FAR__;

extern bool radio_set_hardseek(uint32 freq, uint8 dir) __FAR__;

extern bool radio_set_softseek(uint32 freq, uint8 dir) __FAR__;

extern bool radio_set_throd(uint8 val) __FAR__;

extern bool radio_get_seekflag(void) __FAR__;

extern bool radio_get_freq(void) __FAR__;

extern bool radio_break_hardseek(void) __FAR__;

extern void init_point_freq(void) __FAR__;

extern uint8 whether_in_list(uint32 freq, uint32 *p_freq) __FAR__;

extern bool radio_modul_open(uint8)__FAR__;
extern bool radio_connect_ain(void)__FAR__;

extern bool radio_engine_init(void) __FAR__;

extern uint32 get_end_freq(FM_SeekDir_e dir) __FAR__;

//extern bool radio_modual_reinstall(void);

extern app_result_e radio_scene_playing(void) __FAR__;

extern app_result_e playing_loop_deal(void) __FAR__;

extern app_result_e radio_scene_auto_search(FM_SeekDir_e dir) __FAR__;

extern app_result_e radio_scene_manual_search(FM_SeekDir_e dir) __FAR__;

extern app_result_e select_station(uint8 direction) __FAR__;

extern app_result_e change_freq(FM_SeekDir_e dir) __FAR__;

extern void radio_do_switch_band(radio_band_e old_band, radio_band_e new_band) __FAR__;

extern app_result_e radio_play_key_digital_sure(void) __FAR__;

extern app_result_e radio_play_key_digital_store(uint8 number) __FAR__;

extern app_result_e deal_hard_auto_seek(FM_SeekDir_e dir) __FAR__;
extern app_result_e deal_hard_manual_search(FM_SeekDir_e dir) __FAR__;

extern app_result_e deal_soft_auto_seek(FM_SeekDir_e dir) __FAR__;
extern app_result_e deal_soft_manual_search(FM_SeekDir_e dir) __FAR__;

extern void show_freq_in_play(void) __FAR__;

extern app_result_e radio_digital_to_chanel(uint16 digital) __FAR__;

extern void release_mute_timer(void) __FAR__;

//RCP相关接口（回调函数）
/*FM收音私有信息*/
typedef struct
{
    /*! 当前是否处于搜台中 */
    uint8 searching;
    /*! MUTE状态，请参见 FM_MUTE_e 定义 */
    uint8 mute;
    /*! 是否有效电台 */
    uint8 is_station;
    /*! 当前电台频段，请参见 radio_band_e 定义 */
    uint8 band;
    /*! 当前电台频点，以kHz为单位 */
    uint32 freq;
} radio_rcp_info_t;

/*FM全局信息结构体*/
typedef struct
{
    /*公共信息结构体*/
    com_rcp_info_t com_info;
    /*FM收音私有信息结构体*/
    radio_rcp_info_t app_info;
} radio_global_rcp_info_t;

extern void radio_rcp_var_init(void) __FAR__;

extern void radio_rcp_var_exit(void) __FAR__;

extern app_result_e radio_get_global_rcp_info(uint32 data1, uint32 data2) __FAR__;

extern app_result_e radio_get_station_list(uint32 data1, uint32 data2) __FAR__;

extern app_result_e radio_get_station_name(uint32 data1, uint32 data2) __FAR__;

extern app_result_e radio_set_station_band(uint32 data1, uint32 data2) __FAR__;

extern app_result_e radio_set_station_freq(uint32 data1, uint32 data2) __FAR__;

app_result_e radio_apk_key_search(uint32 data1, uint32 data2) __FAR__;
app_result_e radio_apk_key_stop_search(uint32 data1, uint32 data2) __FAR__;
app_result_e radio_apk_key_chadd(uint32 data1, uint32 data2) __FAR__;
app_result_e radio_apk_key_chsub(uint32 data1, uint32 data2) __FAR__;
app_result_e radio_apk_key_mute(uint32 data1, uint32 data2) __FAR__;

extern void radio_freq_tts_play(uint32 freq) __FAR__;
extern uint8 g_freq_tts_sec_ids[10];
extern uint32 g_search_current_freq;
extern bool g_digit_input_flag;
extern bool g_first_enter_play;

extern bool g_neednot_freq_tts_play;

extern bool g_radio_init_flag;

extern const key_event_map_t radio_play_ke_maplist[];
extern const key_event_map_t radio_search_ke_maplist[];

#define RADIO_MAIN_VIEW_PLAY          0
#define RADIO_MAIN_VIEW_HARD_SEARCH   1
#define RADIO_MAIN_VIEW_SOFT_SEARCH   2
#define RADIO_SUB_VIEW_DIGIT          3
#define RADIO_MSG_VIEW_STATION        4
#define RADIO_MSG_VIEW_ERROR          5

extern void radio_play_create_main_view(void) __FAR__;
extern void radio_hard_search_create_main_view(void) __FAR__;
extern void radio_soft_search_create_main_view(void) __FAR__;
extern void radio_digit_create_sub_view(uint16 overtime) __FAR__;
extern void radio_station_create_msg_view(uint16 overtime) __FAR__;
extern void radio_error_create_msg_view(uint16 overtime) __FAR__;

#endif //__APP_RADIO_H__
