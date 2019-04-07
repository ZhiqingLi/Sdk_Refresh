#include "include.h"
#include "func.h"
#include "func_aux.h"

#if FUNC_AUX_EN
func_aux_t f_aux;

///AUX立体声直通不用能DAC动态降噪，可以使用下面降噪函数
void aux_dnr_init(u16 v_cnt, u16 v_pow, u16 s_cnt, u16 s_pow);
u8 aux_dnr_process(u8 *ptr, u32 len);       //返回值 1：voice(需要淡入)， 返回2: silence (需要淡出)

#if AUX_REC_EN
AT(.text.bsp.aux)
void aux_rec_start(void)
{
    if (f_aux.aux2adc & AUX2PA_ADC_MASK) {
        aux_shorting_enable();       //L+R叠加录音
    }
    f_aux.rec_en = 1;
}

AT(.text.bsp.aux)
void aux_rec_stop(void)
{
    f_aux.rec_en = 0;
    if (f_aux.aux2adc & AUX2PA_ADC_MASK) {
        aux_shorting_disable();     //恢复模拟直通立体声输出
    }
}

AT(.text.bsp.aux)
void aux_rec_init(void)
{
    rec_src.spr = SPR_44100;
    rec_src.nchannel = 1;
    rec_src.source_start = aux_rec_start;
    rec_src.source_stop  = aux_rec_stop;
    f_aux.rec_en = 0;
}
#endif // AUX_REC_EN


//AT(.com_text)
//void pcm_manual_amplify(void *pcm_buf,u32 len, u16 gain)  //2^13  0DB
//{
//    s32 tem;
//    s16 *buf = (s16*)pcm_buf;
//    for(u32 i=0; i<len/2; i++){
//        tem = (buf[i]* gain)>>13;   //2^13 8192
//         if(tem < -32768){
//            tem = -32768;
//       }else if(tem > 32767){
//            tem = 32767;
//       }
//       buf[i] = tem;
//    }
//}

AT(.com_text.func.aux)
void aux_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if AUX_REC_EN
    if (f_aux.rec_en) {
        puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
    }
#endif // AUX_REC_EN


#if AUX_SNR_EN
    aux_dnr_process(ptr, samples);
#endif // AUX_SNR_EN

    //pcm_manual_amplify(ptr,(samples*2)<<ch_mode,GAIN_DIG_P7DB);  //ch_mode 0 单声道, 1双声道
    if (f_aux.aux2adc & AUX2ADC_MASK) {
        sdadc_pcm_2_dac(ptr, samples, ch_mode);
    }
}

AT(.text.bsp.aux)
void func_aux_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if AUX_REC_EN
    sfunc_record_pause();
#endif // AUX_REC_EN

    if (!f_aux.pause) {
        func_aux_stop();
        mp3_res_play(addr, len);
        func_aux_start();
    } else {
        mp3_res_play(addr, len);
    }

#if AUX_REC_EN
    sfunc_record_continue();
#endif // AUX_REC_EN
}

AT(.text.func.aux)
void func_aux_start(void)
{
    bsp_aux_start(get_aux_channel_cfg(), xcfg_cb.aux_anl_gain, f_aux.aux2adc);
}

AT(.text.func.aux)
void func_aux_stop(void)
{
    bsp_aux_stop(get_aux_channel_cfg(), f_aux.aux2adc);
}

AT(.text.func.aux)
void func_aux_pause_play(void)
{
    if (f_aux.pause) {
        led_aux_play();
        func_aux_start();
    } else {
        led_idle();
        func_aux_stop();
    }
    f_aux.pause ^= 1;
}

AT(.text.func.aux)
void func_aux_setvol_callback(u8 dir)
{
    if (f_aux.pause) {
        func_aux_pause_play();
    }

    if (sys_cb.vol == 0) {
        aux_channel_mute();
    } else if ((sys_cb.vol == 1) && (dir)) {
        aux_channel_unmute();
    }
}

AT(.text.func.aux)
void func_aux_process(void)
{
    func_process();
}

static void func_aux_enter(void)
{
    if (!is_linein_enter_enable()) {
        func_cb.sta = FUNC_NULL;
        return;
    }

    memset(&f_aux, 0, sizeof(func_aux_t));
    f_aux.aux2adc = ((u8)(AUX_2_SDADC_EN & xcfg_cb.aux_2_sdadc_en) << 7) | AUDIO_PATH_AUX;
#if ((!SYS_KARAOK_EN) && (AUX_REC_EN || AUX_SNR_EN))
    if (((f_aux.aux2adc & AUX2ADC_MASK) == 0) && (get_aux_channel_cfg() & CHANNEL_R) && (get_aux_channel_cfg() & CHANNEL_L)) {
        //AUX立体声模拟直通, 需要同时开启SDADC用于录音或能量检测
        f_aux.aux2adc |= AUX2PA_ADC_MASK;
    }
#endif // AUX_REC_EN
    func_cb.mp3_res_play = func_aux_mp3_res_play;
    func_cb.set_vol_callback = func_aux_setvol_callback;
    msg_queue_clear();

#if AUX_SNR_EN
    aux_dnr_init(2, 0x200, 60, 0x180);
#endif // AUX_SNR_EN

#if AUX_REC_EN
    aux_rec_init();
#endif // AUX_REC_EN

    led_aux_play();
    func_aux_enter_display();
#if WARNING_FUNC_AUX
    mp3_res_play(RES_BUF_AUX_MODE_MP3, RES_LEN_AUX_MODE_MP3);
#endif // WARNING_FUNC_AUX

#if SYS_KARAOK_EN
    dac_fade_out();
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif

    func_aux_start();

#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, FUNC_AUX);
#endif

    led_aux_play();
}

static void func_aux_exit(void)
{
#if AUX_REC_EN
    sfunc_record_stop();
#endif // AUX_REC_EN

    func_aux_exit_display();
    func_aux_stop();
    func_cb.last = FUNC_AUX;
}

AT(.text.func.aux)
void func_aux(void)
{
    printf("%s\n", __func__);

    func_aux_enter();

    while (func_cb.sta == FUNC_AUX) {
        func_aux_process();
        func_aux_message(msg_dequeue());
        func_aux_display();
    }

    func_aux_exit();
}
#endif  //FUNC_AUX_EN
