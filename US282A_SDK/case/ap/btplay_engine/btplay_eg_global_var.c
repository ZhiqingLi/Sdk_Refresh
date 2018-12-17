/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌引擎应用全局变量定义。
 * 作者：cailizhen
 ********************************************************************************/

#include  "btplay_engine.h"

btplay_global_variable_struct btplay_gl_var =
{ 0 };
btplay_config_t g_btplay_cfg;

uint16 pause_cnt = 0;
uint16 play_cnt = 0;
uint8 change_flag = 0;

void *mp_handle = NULL;

bt_stack_pipe_t *g_p_play_pipe;
bt_stack_info_t *g_p_bt_stack_cur_info;
bt_stack_twsinfo_t *g_p_tws_info;

btplay_info_t g_btplay_info[2];
btplay_info_t *g_p_btplay_info;
#ifdef WAVES_ASET_TOOLS
    waves_t  g_waves;
#endif