/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌前台应用相关宏定义，结构体定义，模块内全局变量声明，模块内外部接口
 *       声明等。
 * 作者：
 ********************************************************************************/

#ifndef _AP_BTPLAY_H
#define _AP_BTPLAY_H

#include  "case_include.h"

#include  "btplay_common.h"
#include "app_linein_rcp.h"

extern bool g_need_draw;

extern comval_t g_comval;

extern uint8 g_btplay_tick_250ms;
extern int8 tick_timer_id;
extern uint8 g_btplay_flag;

#ifdef ENABLE_TRUE_WIRELESS_STEREO

extern uint8 flag2;
extern uint8 flag_con_ok_one;

extern uint8 flag_pair;
extern uint8 flag_no_tts;

extern uint8 tws_vol_sync_flag;
extern int8 tws_sync_timer_id;


extern bool g_tws_need_tts_play;


extern uint8 flag_dis_tws_dev;
extern uint8 tws_volume;

extern void tws_btplay_ui_display(void);
extern void linein_tws_tts_play(void);

extern void tws_timer_kill(int8 *timer_id);
#endif
extern uint32 g_last_change_state_time;
extern uint32  g_second_change_state_flag;


extern bool g_btplay_init_flag;


//extern conn_status_e g_last_conn_status;

extern app_result_e get_message_loop(void);



extern uint8 g_last_dev_role;

extern btplay_info_t g_btplay_cur_info;
extern btplay_status_e g_last_btplay_status;
extern const key_event_map_t linein_ke_maplist[];
extern const sys_event_map_t linein_se_maplist[];
extern void linein_main_view(view_update_e mode) __FAR__;
extern void linein_create_main_view(void) __FAR__;


extern btplay_status_e g_btplay_enter_s3bt_status; //进入S3BT前状态
extern a2dp_status_e g_btplay_enter_s3bt_a2dp_status; //进入S3BT前A2DP服务状态
extern hfp_status_e g_btplay_enter_s3bt_hfp_status; //进入S3BT前HFP服务状态

#endif

