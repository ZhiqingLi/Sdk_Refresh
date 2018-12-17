/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌引擎应用全局变量定义。
 * 作者：
 ********************************************************************************/

#include  "linein_engine.h"

btplay_global_variable_struct btplay_gl_var =
{ 0 };
btplay_config_t g_btplay_cfg;



void *mp_handle = NULL;

bt_stack_pipe_t *g_p_play_pipe;
bt_stack_info_t *g_p_bt_stack_cur_info;
bt_stack_twsinfo_t *g_p_tws_info;

btplay_info_t g_btplay_info[2];
btplay_info_t *g_p_btplay_info;

uint16 mov_ptr = 0;
uint8 linein_tws_playpause_flag = 0;
uint8 adc_init_flag = 0;
uint8 sample_start_flag = 0;


uint8 g_need_adc_flag = 0;
uint8 enable_unfilter_flag = 0;
update_pipe_entry update_pipe;

#ifdef WAVES_ASET_TOOLS
waves_t  g_waves;
#endif