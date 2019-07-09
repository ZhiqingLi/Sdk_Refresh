#ifndef _API_KARAOK_H
#define _API_KARAOK_H

#define FIX_BIT(x)          ((int)(x * (1 << 28)))


void karaok_disable_mic_first(void);
void karaok_set_mic_volume(u16 vol);
void karaok_set_music_volume(u16 vol);
void karaok_init(void *cfg);
void karaok_bufmode(uint mode);             //mode=1时使用降采样算法加大混响Buffer
void karaok_var_init(void);
void karaok_exit(void);
void echo_reset_buf(void *cfg);
u8 get_magic_sta(void);

void karaok_mic_mute(void);
void karaok_mic_unmute(void);
void karaok_process(void);

void echo_set_level(const u16 *vol);    //混响等级设置
void echo_set_delay(u16 delay);     //混响间隔

void karaok_voice_rm_enable(void);  //开启消人声
void karaok_voice_rm_disable(void); //关闭消人声
void karaok_voice_rm_switch(void);  //消人声切换
bool karaok_get_voice_rm_sta(void); //获取消人声状态

//void karaok_rec_init(u8 nchannel);
//bool karaok_rec_fill_buf(u8 *buf);
//void karaok_aux_rec_enable(void);
//void karaok_aux_rec_disable(void);
//void karaok_fmrx_rec_enable(void);
//void karaok_fmrx_rec_disable(void);

void karaok_set_mav(u8 type, u8 level);  //type:0(高音), 1(低音)， level[0:7]

void kara_sco_start(void);
void kara_sco_stop(void);

void mic_dnr_init(u8 v_cnt, u16 v_pow, u8 s_cnt, u16 s_pow);   //MIC动态降噪 //10ms检测一次 //连续超过 v_cnt 次大于v_pow就认为有声，连续超过s_cnt次低于s_pow就认为无声
void karaok_enable_mic_first(u16 vol);  //mic优先使能， 同时设置mic_first时， 音量的音量值（0~65535）
void mic_first_init(u8 v_cnt, u16 v_pow, u8 s_cnt, u16 s_pow);  //连续超过 v_cnt 次大于 v_pow 就开启mic_first，连续超过 s_cnt 次低于 s_pow 就关闭 mic_first
u16 get_karaok_mic_maxpow(bool rekick);    //karaok中的mic 能量值
void karaok_mic_first_music_vol_fade_enable(u16 fade_in_step,  u16 fade_out_step);  //ms

#endif
