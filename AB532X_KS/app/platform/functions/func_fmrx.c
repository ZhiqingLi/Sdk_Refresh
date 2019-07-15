#include "include.h"
#include "func.h"
#include "func_fmrx.h"

fmrx_cb_t fmrx_cb AT(.buf.fmrx.cb);

#if FUNC_FMRX_EN

AT(.text.func.fmrx)
void func_fmrx_stop(void)
{
#if FMRX_INSIDE_EN
    dac_fade_out();
    dac_fade_wait();
    fmrx_digital_stop();
#else
    bsp_aux_stop(FMRX_CHANNEL_CFG, ((u8)FMRX_2_SDADC_EN << 7) | AUDIO_PATH_FM);
#endif // FMRX_INSIDE_EN
}

AT(.text.func.fmrx)
void func_fmrx_start(void)
{
#if FMRX_INSIDE_EN
    fmrx_digital_start();
    dac_fade_in();
#else
    bsp_aux_start(FMRX_CHANNEL_CFG, FMRX_AUX_ANL_GAIN, ((u8)FMRX_2_SDADC_EN << 7) | AUDIO_PATH_FM);
#endif // FMRX_INSIDE_EN
}

//先暂停FMRX再播放提示音
AT(.text.func.fmrx)
void func_fmrx_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if FMRX_REC_EN
    sfunc_record_pause();
#endif // FMRX_REC_EN

    if (fmrx_cb.sta == FMRX_PAUSE) {
        mp3_res_play(addr, len);
    } else {
        func_fmrx_stop();
        mp3_res_play(addr, len);
        func_fmrx_start();
    }

#if FMRX_REC_EN
    sfunc_record_continue();
#endif // FMRX_REC_EN
}

//FMRX播放暂停控制
AT(.text.func.fmrx)
void func_fmrx_pause_play(void)
{
    if (fmrx_cb.sta == FMRX_PLAY) {
        func_fmrx_stop();
        fmrx_cb.sta = FMRX_PAUSE;
        led_idle();
    } else if (fmrx_cb.sta == FMRX_PAUSE) {
        func_fmrx_start();
        fmrx_cb.sta = FMRX_PLAY;
        led_fm_play();
    }
}

//调音量解除FMRX暂停
AT(.text.func.fmrx)
void func_fmrx_setvol_callback(u8 dir)
{
    if (fmrx_cb.sta == FMRX_PAUSE) {
        func_fmrx_pause_play();
    }
}


AT(.text.func.fmrx)
void save_ch_buf(u16 freq)
{
    unsigned char index, bit_pos;
    index = (freq - 875) / 16;
    bit_pos = (freq - 875) % 16;
    fmrx_cb.buf[index] |= BIT(bit_pos);
}

AT(.text.func.fmrx)
u16 get_ch_freq(u8 ch)
{
    u8 i, j;
    for (i = 0; i < 13; i++) {
        for (j = 0; j < 16; j++) {
            if (fmrx_cb.buf[i] & BIT(j)) {
                ch--;
                if (ch == 0) {
                    return ((j + i*16) + 875)*10;
                }
            }
        }
    }
    return FM_FREQ_MIN;
}

AT(.text.func.fmrx)
void reset_fm_cb(void)
{
    fmrx_cb.freq = FM_FREQ_MIN;
    fmrx_cb.ch_cnt = 0;
    fmrx_cb.ch_cur = 1;
    memset(fmrx_cb.buf, 0, 26);
}

static void fmrx_set_curch_freq(void)
{
    param_fmrx_chcur_write();
    param_sync();
    fmrx_cb.freq = get_ch_freq(fmrx_cb.ch_cur);
    gui_box_show_chan();
    bsp_fmrx_set_freq(fmrx_cb.freq);
    dac_fade_in();
    printf("ch freq: %d,  %d\n", fmrx_cb.freq, fmrx_cb.ch_cur);
}

AT(.text.func.fmrx)
void fmrx_switch_freq(u8 dir)
{
    if (dir > 0) {
        fmrx_cb.freq += 10;
        if (fmrx_cb.freq > 10800) {
            fmrx_cb.freq = 8750;
        }
    } else {
        fmrx_cb.freq -= 10;
        if (fmrx_cb.freq < 8750) {
            fmrx_cb.freq = 10800;
        }
    }
    bsp_fmrx_set_freq(fmrx_cb.freq);
    dac_fade_in();
}

AT(.text.func.fmrx)
void fmrx_switch_channel(u8 dir)
{
    bsp_clr_mute_sta();
    if (fmrx_cb.ch_cnt == 0) {
        return;
    }

    if (dir > 0) {
        fmrx_cb.ch_cur++;
        if (fmrx_cb.ch_cur > fmrx_cb.ch_cnt) {
            fmrx_cb.ch_cur = 1;
        }
    } else {
        fmrx_cb.ch_cur--;
        if (fmrx_cb.ch_cur < 1) {
            fmrx_cb.ch_cur = fmrx_cb.ch_cnt;
        }
    }
    fmrx_set_curch_freq();
}


AT(.text.func.fmrx)
void fmrx_seek_start(void)
{
#if FMRX_REC_EN
    sfunc_record_stop();
#endif // FMRX_REC_EN
#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    reset_fm_cb();
    fmrx_cb.seek_start = 1;
    fmrx_cb.sta = FMRX_SEEKING;
    dac_fade_out();
}

AT(.text.func.fmrx)
void fmrx_seek_stop(void)
{
    fmrx_cb.sta = FMRX_PLAY;
    fmrx_cb.freq = FM_FREQ_MIN;
    if (fmrx_cb.ch_cnt > 0) {
        fmrx_cb.ch_cur = 1;
        fmrx_cb.freq = get_ch_freq(fmrx_cb.ch_cur);
    }
    param_fmrx_chcur_write();
    param_fmrx_chcnt_write();
    param_fmrx_chbuf_write();
    param_sync();

    bsp_fmrx_set_freq(fmrx_cb.freq);
    dac_fade_in();
#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, FUNC_FMRX);
#endif
}

#if FMRX_HALF_SEEK_EN
AT(.text.func.fmrx)
void fmrx_half_seek_start(u8 dir)
{
    bsp_clr_mute_sta();
    if (fmrx_cb.sta != FMRX_PLAY) {
        return;
    }
#if FMRX_REC_EN
    sfunc_record_stop();
#endif // FMRX_REC_EN
    fmrx_cb.seek_start = 2;
    fmrx_cb.hstep = -10;
    if (dir) {
        fmrx_cb.hstep = 10;
    }
    fmrx_cb.fcnt = 0;
    fmrx_cb.freq += fmrx_cb.hstep;
    if (fmrx_cb.freq > FM_FREQ_MAX) {
        fmrx_cb.freq = FM_FREQ_MIN;
    } else if (fmrx_cb.freq < FM_FREQ_MIN) {
        fmrx_cb.freq = FM_FREQ_MAX;
    }
    fmrx_cb.sta = FMRX_SEEKING_HALF;
    led_fm_scan();
    dac_fade_out();
}

AT(.text.func.fmrx)
void fmrx_half_seek_stop(void)
{
    fmrx_cb.seek_start = 0;
    fmrx_cb.sta = FMRX_PLAY;
    led_fm_play();
    bsp_fmrx_set_freq(fmrx_cb.freq);
    dac_fade_in();
}
#endif // FMRX_HALF_SEEK_EN

static void func_fmrx_box_event(void)
{
#if IR_NUMKEY_EN
    if (box_cb.event) {
        box_cb.event = 0;
        bsp_clr_mute_sta();
        if ((box_cb.number > 0) && (box_cb.number < fmrx_cb.ch_cnt)) {
            fmrx_cb.ch_cur = box_cb.number;
            fmrx_set_curch_freq();
        } else if ((box_cb.number >= FM_FREQ_MIN/10) && (box_cb.number <= FM_FREQ_MAX/10)) {
            fmrx_cb.freq = box_cb.number*10;
            bsp_fmrx_set_freq(fmrx_cb.freq);
        }
    }
#endif // IR_NUMKEY_EN
}

AT(.text.func.fmrx)
void func_fmrx_process(void)
{
    func_process();
    func_fmrx_box_event();
    switch (fmrx_cb.sta) {
        case FMRX_PLAY:
//            fmrx_cb.st_flag = fmrx_cur_freq_is_stereo();  //判断当前电台是否为立体声电台
            break;

#if FMRX_HALF_SEEK_EN
        case FMRX_SEEKING_HALF:
            if (bsp_fmrx_check_freq(fmrx_cb.freq)) {
                fmrx_half_seek_stop();
                break;
            }
            fmrx_cb.freq += fmrx_cb.hstep;
            if (fmrx_cb.freq > FM_FREQ_MAX) {
                fmrx_cb.freq = FM_FREQ_MIN;
            } else if (fmrx_cb.freq < FM_FREQ_MIN) {
                fmrx_cb.freq = FM_FREQ_MAX;
            }
            fmrx_cb.fcnt++;
            if (fmrx_cb.fcnt > 205) {
                fmrx_half_seek_stop();
            }
            break;
#endif // FMRX_HALF_SEEK_EN

        case FMRX_SEEK_START:
            led_fm_scan();
            fmrx_seek_start();
            break;

        case FMRX_SEEKING:
            if (bsp_fmrx_check_freq(fmrx_cb.freq)) {
                fmrx_cb.ch_cnt++;
                save_ch_buf(fmrx_cb.freq / 10);
//                printf("ch freq: %d\n", fmrx_cb.freq);
                fmrx_cb.ch_cur = fmrx_cb.ch_cnt;
                bsp_fmrx_set_freq(fmrx_cb.freq);
#if FMRX_SEEK_DISP_CH_EN
                gui_display(DISP_FQ_CHAN);
#endif // FMRX_SEEK_DISP_CH_EN
                dac_fade_in();
                delay_5ms(300);
                dac_fade_out();
            }
            fmrx_cb.freq += 10;
            if (fmrx_cb.freq > FM_FREQ_MAX) {
                fmrx_cb.sta = FMRX_SEEK_STOP;
            }
            break;

        case FMRX_SEEK_STOP:
            led_fm_play();
            fmrx_seek_stop();
            break;
    }
}

AT(.text.func.fmrx)
void func_fmrx_enter(void)
{
    memset(&fmrx_cb, 0, sizeof(fmrx_cb));
    if (!is_func_fmrx_en()) {
        func_cb.sta = FUNC_NULL;
        return;
    }
    func_cb.mp3_res_play = func_fmrx_mp3_res_play;
    func_cb.set_vol_callback = func_fmrx_setvol_callback;

    param_fmrx_chcur_read();
    param_fmrx_chcnt_read();
    param_fmrx_chbuf_read();
    fmrx_cb.freq = get_ch_freq(fmrx_cb.ch_cur);
    msg_queue_clear();

    led_fm_play();
    func_fmrx_enter_display();

    AMPLIFIER_SEL_AB();

#if WARNING_FUNC_FMRX
    mp3_res_play(RES_BUF_FM_MODE_MP3, RES_LEN_FM_MODE_MP3);
#endif // WARNING_FUNC_FMRX

#if SYS_KARAOK_EN
    dac_fade_out();
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
#if BT_BACKSTAGE_EN
  if((func_cb.sta==FUNC_MUSIC)&&(dev_is_online(DEV_UDISK))&&(sys_cb.cur_dev==DEV_UDISK)) {
    }else
#endif // BT_BACKSTAGE_EN
    {
        bsp_fmrx_init();
    }

    MEMCON &= ~0x3ff;                   // ram & rom memory always work

//外置收音配置AUX通路
#if !FMRX_INSIDE_EN
    WDT_CLR();
    #if (FMRX_CLOCK == FM_USE_IO_12MHZ_CLK)
    FUNCMCON0 = 0x0f;
    sys_clk_output(2, FMRX_CLKOMAP);
    #endif
    bsp_fmrx_set_volume(16);
    bsp_aux_start(FMRX_CHANNEL_CFG, FMRX_AUX_ANL_GAIN, ((u8)FMRX_2_SDADC_EN << 7) | AUDIO_PATH_FM);
#endif
    led_fm_play();

    fmrx_cb.sta = FMRX_PLAY;
#if BT_BACKSTAGE_EN
  if((func_cb.sta==FUNC_MUSIC)&&(dev_is_online(DEV_UDISK))&&(sys_cb.cur_dev==DEV_UDISK)) {
    }else
#endif // BT_BACKSTAGE_EN
    {
        bsp_fmrx_set_freq(fmrx_cb.freq);
    }
    dac_fade_in();
#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, FUNC_FMRX);
#endif
    printf("%s: %d\n", __func__, fmrx_cb.freq);
}

AT(.text.func.fmrx)
void func_fmrx_exit(void)
{
#if FMRX_REC_EN
    sfunc_record_stop();
#endif // FMRX_REC_EN

#if !FMRX_INSIDE_EN
    bsp_aux_stop(FMRX_CHANNEL_CFG, ((u8)FMRX_2_SDADC_EN << 7) | AUDIO_PATH_FM);
#endif

    dac_fade_out();
    func_fmrx_exit_display();
    if (fmrx_cb.sta > FMRX_IDLE) {
#if BT_BACKSTAGE_EN
  if((func_cb.sta==FUNC_MUSIC)&&(dev_is_online(DEV_UDISK))&&(sys_cb.cur_dev==DEV_UDISK)) {
    }else
#endif // BT_BACKSTAGE_EN
    {
        bsp_fmrx_exit();
    }
#if I2C_MUX_SD_EN
        sd_gpio_init(2);
#endif // I2C_MUX_SD_EN
    }
    MEMCON |= 0x3ff;                   // ram & rom memory standby when no peripheral access
    AMPLIFIER_SEL_D();
    func_cb.last = FUNC_FMRX;
}

AT(.text.func.fmrx)
void func_fmrx(void)
{
    printf("%s\n", __func__);

    func_fmrx_enter();

    while (func_cb.sta == FUNC_FMRX) {
        func_fmrx_process();
        func_fmrx_message(msg_dequeue());
        func_fmrx_display();
    }

    func_fmrx_exit();
}
#endif // FUNC_FMRX_EN
