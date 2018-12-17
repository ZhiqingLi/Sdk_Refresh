/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：fm引擎相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __APP_FMENGINE_H__
#define __APP_FMENGINE_H__

#include "psp_includes.h"
#include "case_include.h"
#include "fmengine.h"
#include "audio_device.h"

#ifdef WAVES_ASET_TOOLS
extern waves_t g_waves;
#endif
extern output_channel_e g_output_channel_config;

extern Engine_Status_t g_fmengine_status;
extern FMEngine_cfg_t g_fmengine_cfg;
extern Module_Status_e g_module_status;
extern mmm_pp_ainout_setting_t g_ainout_param;
extern bool g_stop_by_tts_flag;

//保存中间件句柄
extern void *g_mmm_pp_handle;

//fm通道选择
extern uint8 g_fm_chanel;

//for dae adjust freq
extern dae_config_t *g_dae_cfg_shm;

bool play(void);
bool stop(void);
app_result_e fmengine_control_block(void) __FAR__;
app_result_e fm_Ain_open(private_msg_t* cur_msg)__FAR__;
app_result_e fm_module_open(private_msg_t* cur_msg)__FAR__;
app_result_e fm_module_reinstall(private_msg_t* cur_msg) __FAR__;
app_result_e fm_module_standby(void) __FAR__;
app_result_e fmengine_set_freq_sync(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_mute_sync(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_set_band_sync(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_set_throd(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_set_softseek(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_set_hardseek(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_seek_break(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_seekflag(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_status(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_band(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_freq(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_intensity(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_antenna(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_stereo(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_playflag(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_get_modual_status(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_tts_start(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_tts_stop(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_debug(private_msg_t *cur_msg) __FAR__;
app_result_e fmengine_set_eq(private_msg_t* msg_ptr) __FAR__;
uint32 _volume_deal(uint8 vol_in, bool set_mode) __FAR__;

app_result_e fmengine_standby(private_msg_t *cur_msg) __FAR__;
app_result_e fmengine_play(private_msg_t* cur_msg) __FAR__;
app_result_e fmengine_stop(private_msg_t* cur_msg) __FAR__;

void fm_eg_reply_msg(private_msg_t *private_msg) __FAR__;

#endif //__APP_FMENGINE_H__
