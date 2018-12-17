#ifndef _API_KARAOK_H
#define _API_KARAOK_H

void karaok_enable_mic_first(u16 vol);
void karaok_disable_mic_first(void);
void karaok_set_mic_volume(u16 vol);       //MIC总音量
void karaok_set_mic_left_volume(u16 vol);  //MICL音量,32767(0DB) //双MIC时有效
void karaok_set_mic_right_volume(u16 vol); //MICR音量,32767(0DB) //双MIC时有效
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

u16 karaok_mic_pow_calc(void);      //Karaok MIC能量检测
void mic_dnr_init(u8 v_cnt, u16 v_pow, u8 s_cnt, u16 s_pow);    //MIC动态降噪

void karaok_rec_init(u8 nchannel);
bool karaok_rec_fill_buf(u8 *buf);
void karaok_aux_rec_enable(void);
void karaok_aux_rec_disable(void);
void karaok_fmrx_rec_enable(void);
void karaok_fmrx_rec_disable(void);

void magic_voice_set_level(u8 type, u8 level);  //type:0(高音), 1(低音)， level[0:7]
void magic_voice_process(void);
void mav_kick_start(void);

void kara_sco_start(void);
void kara_sco_stop(void);
void dual_mic_channel_select(u8 left_ch,u8 right_ch);
#endif
