#include "include.h"
#include "func.h"
#include "func_speaker.h"

#if FUNC_SPEAKER_EN

func_speaker_t f_spk;

#if MIC_REC_EN
AT(.text.bsp.mic)
void mic_rec_start(void)
{
    f_spk.rec_en = 1;
}

AT(.text.bsp.mic)
void mic_rec_stop(void)
{
    f_spk.rec_en = 0;
}

AT(.text.bsp.mic)
void mic_rec_init(void)
{
    rec_src.spr = SPR_16000;
    rec_src.nchannel = 1;
    rec_src.source_start = mic_rec_start;
    rec_src.source_stop  = mic_rec_stop;
    f_spk.rec_en = 0;
}
#endif // MIC_REC_EN

AT(.text.bsp.speaker)
void func_speaker_stop(void)
{
    dac_fade_out();
    dac_fade_wait();                    //等待淡出完成
    audio_path_exit(AUDIO_PATH_SPEAKER);
}

AT(.text.bsp.speaker)
void func_speaker_start(void)
{
    dac_fade_wait();                    //等待淡出完成
    audio_path_init(AUDIO_PATH_SPEAKER);
    audio_path_start(AUDIO_PATH_SPEAKER);
    dac_fade_in();
}

AT(.text.bsp.speaker)
void func_speaker_pause_play(void)
{
    if (f_spk.pause) {
        led_music_play();
        func_speaker_start();
    } else {
        led_idle();
        func_speaker_stop();
    }
    f_spk.pause ^= 1;
}

AT(.text.bsp.speaker)
void func_speaker_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if MIC_REC_EN
    sfunc_record_pause();
#endif // MIC_REC_EN

    if (!f_spk.pause) {
        func_speaker_stop();
        mp3_res_play(addr, len);
        func_speaker_start();
    } else {
        mp3_res_play(addr, len);
    }

#if MIC_REC_EN
    sfunc_record_continue();
#endif // MIC_REC_EN
}

AT(.text.bsp.speaker)
void func_speaker_setvol_callback(u8 dir)
{
    if (f_spk.pause) {
        func_speaker_pause_play();
    }
}

AT(.com_text.func.speaker)
void speaker_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if SPEAKER_ECHO_EN
    speaker_echo_process(ptr,samples);
#endif

#if MIC_REC_EN
    if (f_spk.rec_en) {
        puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
    }
#endif //MIC_REC_EN

    sdadc_pcm_2_dac(ptr, samples, ch_mode);
}

AT(.text.func.speaker)
void func_speaker_process(void)
{
    func_process();
}

#if SPEAKER_ECHO_EN
static u8 echo_buf[1024*12] AT(.echo_buf);

#endif // SPEAKER_ECHO_EN


static void func_speaker_enter(void)
{
    memset(&f_spk, 0, sizeof(f_spk));
    func_cb.mp3_res_play = func_speaker_mp3_res_play;
    func_cb.set_vol_callback = func_speaker_setvol_callback;

    func_speaker_enter_display();
    led_music_play();
#if WARNING_FUNC_SPEAKER
    mp3_res_play(RES_BUF_SPK_MODE_MP3, RES_LEN_SPK_MODE_MP3);
#endif // WARNING_FUNC_SPEAKER

#if MIC_REC_EN
    mic_rec_init();
#endif // MIC_REC_EN

#if SPEAKER_ECHO_EN
    f_spk.echo_step = 8;
    speaker_echo_init(echo_buf,sizeof(echo_buf),f_spk.echo_step, 2^12);
#endif
    func_speaker_start();
}

static void func_speaker_exit(void)
{
#if MIC_REC_EN
    sfunc_record_stop();
#endif // MIC_REC_EN

    func_speaker_exit_display();
    led_idle();
    func_speaker_stop();
    func_cb.last = FUNC_SPEAKER;
}

AT(.text.func.speaker)
void func_speaker(void)
{
    printf("%s\n", __func__);

    func_speaker_enter();

    while (func_cb.sta == FUNC_SPEAKER) {
        func_speaker_process();
        func_speaker_message(msg_dequeue());
        func_speaker_display();
    }

    func_speaker_exit();
}

#endif // FUNC_SPEAKER_EN

