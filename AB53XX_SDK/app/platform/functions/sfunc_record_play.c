#include "include.h"

#if REC_AUTO_PLAY

rec_play_cb_t rec_play_cb;

extern u8 fname_buf[100];

AT(.text.func.music)
static void sfunc_record_play_time(void)
{
    u16 cur_sec;
    cur_sec = music_get_cur_time() / 10;
    rec_play_cb.curtime.min = cur_sec / 60;
    rec_play_cb.curtime.sec = cur_sec % 60;
    rec_play_cb.bit_rate = get_music_bitrate();
}

AT(.text.func.record)
static void sfunc_record_play_process(void)
{
    u16 total_time;

    func_process();
    sfunc_record_play_time();
    if (get_music_dec_sta() == MUSIC_STOP) {
        rec_play_cb.sta = PREC_STOP;
    }
    if (rec_play_cb.alltime.min == 0xff) {
        total_time = music_get_total_time();
        if (total_time != 0xffff) {
            rec_play_cb.alltime.min = total_time / 60;
            rec_play_cb.alltime.sec = total_time % 60;
        }
    }
}

AT(.text.func.record)
static void sfunc_record_play_enter(void)
{
    rec_play_cb_t *s = &rec_play_cb;
    memset(&rec_play_cb, 0, sizeof(rec_play_cb));
    sfunc_record_play_enter_display();
    if (fs_open((const char *)fname_buf, FA_READ) == FR_OK) {
        s->alltime.min = 0xff;
        s->type = music_decode_init();
        if(s->type != NONE_TYPE) {
            printf("record playing [%s]\n", fname_buf);
            led_music_play();
            s->sta = PREC_PLAYING;
            music_control(MUSIC_MSG_PLAY);             //开始播放
            delay_5ms(1);
        }
    }
#if BT_APP_EN
    bsp_app_rec_start(0, 1);
#endif
}

AT(.text.func.record)
static void sfunc_record_play_exit(void)
{
    if (rec_play_cb.sta == PREC_INIT) {
        return;
    }
    music_control(MUSIC_MSG_STOP);
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        sd0_stop(1);
#if I2C_MUX_SD_EN
        sd0_go_idle_state();
#endif
    }
    led_idle();
#if BT_APP_EN
    bsp_app_rec_start(0, 0);
#endif
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
