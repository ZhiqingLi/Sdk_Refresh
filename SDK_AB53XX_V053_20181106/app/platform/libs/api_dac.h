#ifndef _API_DAC_H_
#define _API_DAC_H_

#define PA1_ANL_SET          0x40
#define PA2_ANL_SET          0x80

void dac_cb_init(u16 dac_cfg);
void dac_set_volume(u8 vol);
void dac_set_dvol(u16 vol);
void dac_fade_process(void);
void dac_fade_out(void);
void dac_fade_in(void);
void dac_fade_wait(void);
void dac_analog_fade_in(void);
void dac_set_analog_vol(u16 vol);
void dac_digital_enable(void);
void dac_digital_disable(void);
void dac_spr_set(uint spr);
void dac_obuf_init(void);
void dac_src1_init(void);
void dac_power_on(void);
void dac_restart(void);
void dac_power_off(void);
void dac_mono_init(bool dual_en, bool lr_sel);
void adpll_init(u8 out_spr);
void adpll_spr_set(u8 out48k_flag);
void obuf_put_one_sample(u32 left, u32 right);
void obuf_put_samples(void *buf, uint len);
void dac_unmute_set_delay(u16 delay);
u16 dac_pcm_pow_calc(void);
void dac_channel_enable(void);
void dac_channel_disable(void);
void dac_set_balance(u16 l_vol, u16 r_vol);
void dac_drc_init(void *cfg);
void dac_drc_exit(void);
void dac_high_impedance(u8 enable);
void dac_channel_exchange(void);    //L与R对换

//EQ
struct eq_coef_tbl_t {
    const int * coef_0;            //tbl_alpha
    const int * coef_1;            //tbl_cos_w0
};

void music_set_eq_by_res(u32 *addr, u32 *len);
void music_set_eq_by_num(u8 num);
void music_eq_off(void);
void music_set_eq(u8 band_cnt, const u32 *eq_param);
void music_set_eq_gain(u32 gain);
bool music_set_eq_is_done(void);    //判断上一次设置EQ是否完成

void mic_set_eq(u8 band_cnt, const u32 *eq_param);
void mic_set_eq_by_res(u32 *addr, u32 *len);
void mic_set_eq_gain(u32 gain);
bool mic_set_eq_is_done(void);      //判断上一次设置EQ是否完成

void bass_treble_coef_cal(int *coef, int gain, int mode);                   //gain:-12dB~12dB, mode:0(bass), 1(treble)
void eq_coef_cal(u8 index, int *coef, int gain, struct eq_coef_tbl_t *tbl); //index:0~7（8条EQ）, gain:-12dB~12dB
void eq_fix_disable(void);          //打开12条EQ，会有切换EQ有杂音的风险
#endif
