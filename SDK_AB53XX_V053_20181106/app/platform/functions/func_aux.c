#include "include.h"
#include "func.h"
#include "func_aux.h"

#if FUNC_AUX_EN
func_aux_t f_aux;

#if AUX_REC_EN
AT(.text.bsp.aux)
void aux_rec_start(void)
{
    f_aux.rec_en = 1;
    karaok_aux_rec_enable();
}

AT(.text.bsp.aux)
void aux_rec_stop(void)
{
    f_aux.rec_en = 0;
    karaok_aux_rec_disable();
}

AT(.text.bsp.aux)
void aux_rec_init(void)
{
    rec_src.spr = SPR_44100;
    rec_src.source_start = aux_rec_start;
    rec_src.source_stop  = aux_rec_stop;
    f_aux.rec_en = 0;
    if (xcfg_cb.auxr_sel && xcfg_cb.auxl_sel) {
        rec_src.nchannel = 2;
    } else {
        rec_src.nchannel = 1;
    }
}
#endif // AUX_REC_EN

AT(.com_text.func.aux)
void aux_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if AUX_REC_EN
    if (f_aux.rec_en) {
#if KARAOK_REC_EN
        if (!karaok_rec_fill_buf(ptr))
#endif
        {
            puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
        }
    }
#endif // AUX_REC_EN
    sdadc_pcm_2_dac(ptr, samples, ch_mode);
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
    func_cb.mp3_res_play = func_aux_mp3_res_play;
    func_cb.set_vol_callback = func_aux_setvol_callback;
    msg_queue_clear();

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
