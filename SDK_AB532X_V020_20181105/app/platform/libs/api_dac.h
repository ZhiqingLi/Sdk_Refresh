#ifndef _API_DAC_H_
#define _API_DAC_H_

void dac_cb_init(u32 dac_cfg);
void dac_set_volume(u8 vol);
void dac_set_dvol(u16 vol);
void dac_fade_process(void);
void dac_fade_out(void);
void dac_fade_in(void);
void dac_fade_wait(void);
void dac_analog_fade_in(void);
void dac_set_analog_vol(u8 vol);
void dac_digital_enable(void);
void dac_digital_disable(void);
void dac_spr_set(uint spr);
void dac_obuf_init(void);
void dac_src1_init(void);
void dac_power_on(void);
void dac_restart(void);
void dac_power_off(void);
void dac_set_mono(void);
void dac_mono_init(bool mix_en, bool lr_sel);
void adpll_init(u8 out_spr);
void adpll_spr_set(u8 out48k_flag);
void obuf_put_one_sample(u32 left, u32 right);
void obuf_put_samples(void *buf, uint len);
void dac_unmute_set_delay(u16 delay);
u16 dac_pcm_pow_calc(void);
void dac_channel_enable(void);
void dac_channel_disable(void);
void dac_dnr_detect(void);
void music_src_set_volume(u16 vol);
#endif
