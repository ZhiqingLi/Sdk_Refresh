#ifndef _PLUGIN_H
#define _PLUGIN_H

#include "multi_lang.h"
#include "port_pwm.h"
#include "port_linein.h"
#include "port_led.h"

void plugin_init(void);
void plugin_var_init(void);
void plugin_tmr5ms_isr(void);
void plugin_tmr1ms_isr(void);
void maxvol_tone_play(void);
bool adjvol_tone_play(void);

void loudspeaker_mute_init(void);
void loudspeaker_mute(void);
void loudspeaker_unmute(void);
void loudspeaker_disable(void);
void plugin_music_eq(void);

bool bt_hfp_ring_number_en(void);
bool is_func_fmrx_en(void);
bool is_sd_support(void);
bool is_usb_support(void);
bool is_led_scan_enable(void);

extern volatile int adc_linein_flag;
void sleep_wakeup_config(void);
void plugin_vbat_filter(u32 *vbat);
void plugin_saradc_init(u16 *adc_ch);
void plugin_saradc_sel_channel(u16 *adc_ch);
void plugin_hfp_karaok_configure(void);
void plugin_lowbat_vol_reduce(void);
void plugin_lowbat_vol_recover(void);
#endif
