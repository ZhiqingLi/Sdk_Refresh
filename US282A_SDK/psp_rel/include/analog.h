/********************************************************************************
 *                              GL5116
 *                            Module: music_player
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       fiona    2015-01-27 15:00     1.0             build this file
 ********************************************************************************/
#ifndef __ANALOG_DEFINE_H__
#define __ANALOG_DEFINE_H__

#include <ucos/init.h>
#include <attr.h>
#include <typeext.h>

#include <asm-mips/mach-gl5116/GL5116_Design_Specification.h>
//typedef volatile unsigned int*      REG32;

extern int32 audio_device_enable_pa(uint32 ddv_sel, uint32 pa_swing, uint32 aout_type) __FAR__;

extern int32 audio_device_disable_pa(uint32 aout_type, void* null2, void* null3) __FAR__;

extern int32 audio_device_enable_pa_ddv(uint32 ddv_sel, uint32 pa_swing,uint32 aout_type) __FAR__;

extern int32 audio_device_disable_pa_ddv(uint32 aout_type, void* null2, void* null3) __FAR__;

extern int32 audio_device_set_pa_volume(uint32 vol_pa, uint32 vol_da,void* null3) __FAR__;

extern int32 audio_device_get_pa_volume(void* null1, void *null2, void *null3) __FAR__;

extern int32 audio_device_enable_dac(uint32 src_type, uint32 dac_chan, void* null3) __FAR__;

extern int32 audio_device_disable_dac(uint32 dac_chan, void* null2, void* null3) __FAR__;

extern int32 audio_device_set_dac_rate(uint32 dac_rate, uint32 chanel_no, void* null3) __FAR__;

extern int32 audio_device_get_dac_rate(void* null1, void* null2, void* null3) __FAR__;

extern int32 audio_device_get_adc_rate(void* null1, void* null2, void* null3) __FAR__;

extern int32 audio_device_enable_ain(uint32 ain_type, uint32 ain_gain, void* null3) __FAR__;

extern int32 audio_device_disable_ain(uint32 ain_type, void* null2, void* null3) __FAR__;

extern int32 audio_device_disable_ain_out(uint32 out_mode, void *null2, void *null3) __FAR__;

extern int32 audio_device_enable_ain_out(uint32 out_mode, void *null2, void *null3) __FAR__;

extern int32 audio_device_set_ain_gain(uint32 ain_type, uint32 ain_gain, void* null3) __FAR__;

extern int32 audio_device_set_adc_gain(uint32 adc_gain, void* null2, void* null3) __FAR__;

extern int32 audio_device_enable_adc(uint32 src_type, uint32 adc_gain, void *null3) __FAR__;

extern int32 audio_device_disable_adc(void* null1, void* null2, void* null3) __FAR__;

extern int32 audio_device_set_adc_rate(uint32 adc_rate,void* null2, void* null3) __FAR__;

extern int32 audio_device_config_asrc(uint32 nTestSel,uint8 asrc_mode) __FAR__;

extern int32 audio_device_close_asrc(uint32 nTestSel) __FAR__;

extern int32 audio_device_set_asrc_rate(uint32 asrc_rate, uint32 chanel_no, uint32 asrc_offset) __FAR__;

//extern int32 audio_device_set_effect_param(uint32 set_type, void* param_ptr, void* null3) __FAR__;

//extern int32 audio_device_get_feature_info(uint32 set_type, void* info_ptr, void* null3) __FAR__;

#endif
