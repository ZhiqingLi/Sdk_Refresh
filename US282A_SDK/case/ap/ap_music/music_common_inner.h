/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：music公共函数
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __MUSIC_COMMON_INNER_H__
#define __MUSIC_COMMON_INNER_H__

#include "case_include.h"

bool music_send_sync_msg(msg_apps_t *msg, msg_reply_t *reply, uint32 timeout) __FAR__;
bool music_send_msg(void *input_param, void *output_param, uint32 msg_type) __FAR__;

bool music_play(void) __FAR__;
bool music_pause(void) __FAR__;
bool music_resume(play_mode_e play_mode) __FAR__;
bool music_stop(void) __FAR__;

bool music_digital_song(uint16 song_num) __FAR__;
void music_switch_song(uint8 tts_id, play_mode_e play_mode) __FAR__;
bool music_get_filepath(file_path_info_t *filepath) __FAR__;
bool music_get_fileinfo(mmm_mp_file_info_t* fileinfo) __FAR__;
bool music_clear_fileswitch(void) __FAR__;

bool music_get_loop_mode(uint8* loop_mode) __FAR__;
bool music_set_loop_mode(uint8 loop_mode) __FAR__;

//播“第xx首”
uint8 music_tts_song_seq(uint16 value, uint8 *tts_sec_ids) __FAR__;
//弹出曲目号界面
void music_create_song_num_view(view_type_e view_type, bool tts, play_mode_e play_mode) __FAR__;
//播曲目号
app_result_e music_song_num_tts(void *ev_param) __FAR__;
void music_file_switch_save(void) __FAR__;

#endif//__MUSIC_COMMON_INNER_H__
