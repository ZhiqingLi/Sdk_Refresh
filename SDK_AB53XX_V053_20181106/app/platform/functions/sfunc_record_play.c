#include "include.h"

#if REC_AUTO_PLAY

rec_play_cb_t rec_play_cb;

extern u8 fname_buf[100];

AT(.text.func.music)
static void sfunc_record_play_time(void)
{
    u16 cur_sec;
    cur_sec = music_get_cur_time() / 10;
    rec_play_cb.min = cur_sec / 60;
    rec_play_cb.sec = cur_sec % 60;
}

AT(.text.func.record)
static void sfunc_record_play_process(void)
{
    func_process();
    sfunc_record_play_time();
    if (get_music_dec_sta() == MUSIC_STOP) {
        rec_play_cb.sta = PREC_STOP;
    }
}

AT(.text.func.record)
static void sfunc_record_play_enter(void)
{
    memset(&rec_play_cb, 0, sizeof(rec_play_cb));

    if (fs_open((const char *)fname_buf, FA_READ) == FR_OK) {
        if (music_decode_init()) {
            printf("record playing [%s]\n", f_msc.fname);
            led_music_play();
            rec_play_cb.sta = PREC_PLAYING;
            music_control(MUSIC_MSG_PLAY);             //开始播放
            delay_5ms(1);
        }
    }
}

AT(.text.func.record)
static void sfunc_record_play_exit(void)
{
    if (rec_play_cb.sta == PREC_INIT) {
        return;
    }
    music_control(MUSIC_MSG_STOP);
    if (dev_is_online(DEV_SDCARD)) {
        sd0_stop(1);
#if I2C_MUX_SD_EN
        sd0_go_idle_state();
#endif
    }
    led_idle();
}

AT(.text.func.record)
void sfunc_record_play(void)
{
    sfunc_record_play_enter();
    while (rec_play_cb.sta == PREC_PLAYING) {
        sfunc_record_play_process();
        sfunc_record_play_message(msg_dequeue());
        sfunc_record_play_display();
    }

    sfunc_record_play_exit();
}

#endif
