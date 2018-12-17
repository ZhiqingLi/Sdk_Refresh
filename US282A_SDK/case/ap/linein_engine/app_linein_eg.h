/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：linein引擎相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __APP_LINEIN_EG_H__
#define __APP_LINEIN_EG_H__

#include "psp_includes.h"
#include "case_include.h"
#include "linein_common.h"
#include "mmm_pp.h"

//模拟通道
#define LINEIN_CH_A             (0)

#define BAT_HIGH_VAL            (0x6a)

#define BAT_LOW_VAL             (0x5d)

#define COUNT_BOUND             (100)

/*!
 * \brief
 *  引擎信息结构体变量定义
 */
typedef struct
{
    /******************************************************************
     *          引擎状态信息
     *******************************************************************/
    linein_eg_status_t eg_status;

} linein_eg_info_t;

#ifdef WAVES_ASET_TOOLS
extern  waves_t g_waves;
#endif

extern output_channel_e g_output_channel_config;

//globle variable
//保存中间件句柄
extern void *g_mmm_pp_handle;

//控件返回值
extern app_result_e g_linein_eg_result;

//引擎状态变量结构体指针
extern linein_eg_status_t *g_eg_status_p;

//私有消息全局变量
//extern private_msg_t g_cur_msg;
//eq data 
extern mmm_pp_eq_info_t g_eq_info;

//for clk adjust 
extern uint32 g_save_freq;

extern mmm_pp_ainout_setting_t g_ainout_param;

//因TTS播报而暂停播放，TTS播报结束后需要恢复播放
extern bool g_stop_by_tts_flag;

//linein播放通道选择
extern uint8 g_linein_channel_sel;

//for dae adjust freq
extern dae_config_t *g_dae_cfg_shm;

//if need open play
extern uint32 g_need_play;

//functions
bool play(void) __FAR__;
bool stop(void) __FAR__;
void linein_eg_reply_msg(void* msg_ptr, bool ret_vals) __FAR__;
app_result_e linein_eg_control_block(void) __FAR__;
app_result_e linein_eg_message_done(private_msg_t* cur_msg) __FAR__;
app_result_e linein_eg_message_done_bank(private_msg_t* cur_msg) __FAR__;

//for event
void _error_handle(void) __FAR__;
app_result_e linein_eg_get_status(void* msg_ptr) __FAR__;
app_result_e linein_eg_set_eq(void* msg_ptr);
app_result_e linein_eg_play(void* msg_ptr);
app_result_e linein_eg_stop(void* msg_ptr);

//for linein analog 
bool linein_eg_aa_play(void) __FAR__;
bool linein_eg_aa_stop(void) __FAR__;

app_result_e linein_eg_tts_start(void* msg_ptr) __FAR__;
app_result_e linein_eg_tts_stop(void* msg_ptr) __FAR__;

app_result_e linein_eg_standby(void * msg_ptr) __FAR__;

#endif //__APP_LINEIN_EG_H__
