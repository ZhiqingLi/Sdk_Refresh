/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：播放窗口(因为一个bank放不下，所以，部分代码放到这里来)
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __MUSIC_PLAY2_H__
#define __MUSIC_PLAY2_H__

#include "case_include.h"

app_result_e play_init(void) __FAR__;
bool play_exit(void) __FAR__;

void music_open_scan(uint8 scan_disk) __FAR__;
void music_close_scan(void) __FAR__;

app_result_e play_err_deal(eg_err_e error_number) __FAR__;

app_result_e play_key_play(void) __FAR__;
app_result_e play_key_next(void) __FAR__;
app_result_e play_key_prev(void) __FAR__;
app_result_e play_key_mute(void);
app_result_e play_key_folderadd(void);
app_result_e play_key_foldersub(void);
bool play_next(void) __FAR__;
bool play_prev(void) __FAR__;

#endif //__MUSIC_PLAY2_H__
