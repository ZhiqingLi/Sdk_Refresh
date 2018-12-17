/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌前台应用相关宏定义，结构体定义，模块内全局变量声明，模块内外部接口
 *       声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _AP_BTPLAY_H
#define _AP_BTPLAY_H

#include  "case_include.h"

#include  "btplay_common.h"

#define LINK_TTS_SEL  0  //连接相关TTS播报条件，0表示A2DP或者HFP其中任何一个连接起来都算连接，全部断开才算断开，
//1表示只有A2DP连接起来才算连接，A2DP断开就算断开

#define AVRCP_PLAY_START_TIME (2000)
#define AVRCP_FAKE_START_TIME (3000)

#define BTPLAY_FAST_NORMAL    0
#define BTPLAY_FAST_FORWARD   1
#define BTPLAY_FAST_BACKWARD  2

extern bool display_flag;
extern int8 display_timer_id;
extern bool g_need_tts_play;
extern bool g_need_draw;
extern bool g_avrcp_tts_enable;
extern comval_t g_comval;

extern uint32 g_btplay_check_btplay_status_timer;
extern btplay_status_e g_last_btplay_status_before_unlink;
extern uint8 g_btplay_autoplay_flag;

extern uint8 g_avrcp_autoplay_cnt;
extern uint8 g_avrcp_autoplay_flag;
extern uint8 g_avrcp_autoplay_try;
extern uint8 g_send_avrcp_play_tick;
extern uint8 g_fake_play_tick; //fake 伪造的
extern uint8 g_btplay_tick_250ms;
extern int8 tick_timer_id;
extern uint8 g_btplay_flag;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
extern uint8 flag1;
extern uint8 flag2;
extern uint8 flag_con_ok_one;

extern uint8 flag_pair;
extern uint8 flag_no_tts;

extern uint8 dev_role_flag;
extern uint8 tws_vol_sync_flag;
extern int8 tws_sync_timer_id;
extern uint8 tws_phcon_sync_flag;
extern int8 tws_phcon_timer_id;
extern uint8 g_auto_btplay_flag;
extern uint8 g_last_stack_play_status;

extern bool g_tws_need_tts_play;
extern bool g_tws_need_tts_play1;

extern uint8 flag_ph_dis_master;
extern uint8 flag_dis_tws_dev;
extern uint8 tws_con_flag;
extern uint8 tws_discon_flag;
extern uint8 tws_volume;
extern uint8 tws_swith_flag;
extern uint8 g_last_role_save;

extern void tws_btplay_ui_display(void);
extern void btplay_tws_tts_play(void);
extern void btplay_tws_tts_play1(void);

extern void tws_timer_kill(int8 *timer_id);
extern void btplay_tws_sync_deal(uint8 sync_val);
#endif
extern uint32 g_last_change_state_time;
extern uint32  g_second_change_state_flag;

extern uint16 disc_sco_num;

extern bool g_btplay_init_flag;

extern uint8 fast_play_flag;

extern conn_status_e g_last_conn_status;

extern app_result_e get_message_loop(void);
extern void btplay_ui_display(void);
extern void btplay_tts_play(void);

#if (LINK_TTS_SEL == 0)
extern uint8 g_connected_flag;
extern uint8 g_last_connected_flag;
#endif

extern btplay_info_t g_btplay_cur_info;
extern btplay_status_e g_last_btplay_status;
extern const key_event_map_t btplay_ke_maplist[];
extern const sys_event_map_t btplay_se_maplist[];
extern void btplay_main_view(view_update_e mode) __FAR__;
extern void btplay_create_main_view(void) __FAR__;
extern void bt_conn_states_init(void);

extern btplay_status_e g_btplay_enter_s3bt_status; //进入S3BT前状态
extern a2dp_status_e g_btplay_enter_s3bt_a2dp_status; //进入S3BT前A2DP服务状态
extern hfp_status_e g_btplay_enter_s3bt_hfp_status; //进入S3BT前HFP服务状态




#endif

