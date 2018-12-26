/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：播放窗口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __MUSIC_PLAY_H__
#define __MUSIC_PLAY_H__

#include "case_include.h"

//刷屏
#define PLAY_REFRESH_ALL     ((uint8)-1)
#define PLAY_REFRESH_TIME    ((uint8)1 << 0)
#define PLAY_REFRESH_STATUS  ((uint8)1 << 1)
#define PLAY_REFRESH_DISK    ((uint8)1 << 2)

//刷新标志
extern uint8 g_play_refresh_flag;

//文件路径来源信息保存，播放时候用
extern file_path_info_t g_file_path_info;
//music总时间信息
extern mmm_mp_file_info_t g_music_fileinfo;
//在提示后，是否退出本窗口
extern bool g_quit_from_hint;

//连续错误歌曲数
extern uint16 g_error_num;
//上次错误的歌曲号
extern uint16 g_error_file_num;
//向前切(如果这一曲刚好错误，要跳到上一个还是下一个)
extern uint16 g_switch_prev;

extern bool g_neednot_song_tts_play;

void play_view_handle(view_update_e mode) __FAR__;

app_result_e music_scene_play(void) __FAR__;
bool play_get_status(mengine_info_t *p_music_info) __FAR__;

#endif //__MUSIC_PLAY_H__
