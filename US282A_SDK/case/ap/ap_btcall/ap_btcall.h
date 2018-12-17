

/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*        brief    蓝牙免提
*      <author>       <time>           
*       Wekan   2015-3-27  review    
*******************************************************************************/


#ifndef _AP_BLUETOOTH_H
#define _AP_BLUETOOTH_H

#include  "psp_includes.h"
#include  "case_include.h"
#include  "btcall_common.h"

extern bool g_display_flag;
extern int8 g_display_timer_id;
extern bool g_need_tts_play;
extern bool g_need_draw;
extern bool g_chf_cag_exch_enable;
extern bool g_state_call_phone;
extern comval_t g_comval;

extern uint8 g_callin_ring_mode;
extern uint8 g_callin_ring_space_time;

extern app_result_e get_message_loop(void);
extern void btcall_ui_display(void);
extern void btcall_tts_play(void);
extern void wait_link_display(void);

extern btcall_info_t g_btcall_cur_info;
extern btcall_status_e g_last_btcall_status;
extern bool g_background_call_flag;

extern const sys_event_map_t btcall_se_maplist[];
extern const key_event_map_t btcall_ke_maplist[];
extern void btcall_main_view(view_update_e mode) __FAR__;
extern void btcall_create_main_view(void) __FAR__;

/////////////////  电话本相关  //////////////////

//播报电话本的缓冲buffer长度
#define MAX_TTS_INFO_LENGTH 48

extern uint8 TTS_info_buf[MAX_TTS_INFO_LENGTH];
extern bool g_call_phone_num;
extern pb_result_e g_pb_result;

extern bool g_set_mic_mute;//设置mic静音--远端没声音

extern bool btcall_pb_tts_init(void);
extern void btcall_pb_tts_play(void);

extern void utf8_to_unicode(uint8 *tts_info);
extern uint8 unicode_to_char(uint8 *str_info);

extern bool btcall_send_sync_msg(void *input_param, void *output_param, uint32 msg_type);


extern void get_eg_asqt_vol(void);
extern void volume_cfg(bool set);


#endif

