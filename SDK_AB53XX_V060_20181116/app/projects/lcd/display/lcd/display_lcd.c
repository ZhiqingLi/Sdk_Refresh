#include "include.h"
#include "ledseg/display_ledseg.h"

#if (GUI_SELECT & DISPLAY_LCD)
u8 lcd_fmrx_buf[168] AT(.fmrx_cb_buf);   //缓存FMRX的图片数据, 只有FMRX模式用到。

typedef void (*PFUNC) (void);
extern u8 fname_buf[100];

AT(.text.lcd.table)
const u8 week_gbk[14] = {0xC8, 0xD5, 0xD2, 0xBB, 0xB6, 0xFE, 0xC8, 0xFD, 0xCB, 0xC4, 0xCE, 0xE5, 0xC1, 0xF9};

AT(.text.lcd.table)
const u32 lcd_bnum_table[10] = {
    BMP_SYS_B0,
    BMP_SYS_B1,
    BMP_SYS_B2,
    BMP_SYS_B3,
    BMP_SYS_B4,
    BMP_SYS_B5,
    BMP_SYS_B6,
    BMP_SYS_B7,
    BMP_SYS_B8,
    BMP_SYS_B9,
};

AT(.text.lcd.table)
const u32 lcd_mnum_table[10] = {
    BMP_SYS_0,
    BMP_SYS_1,
    BMP_SYS_2,
    BMP_SYS_3,
    BMP_SYS_4,
    BMP_SYS_5,
    BMP_SYS_6,
    BMP_SYS_7,
    BMP_SYS_8,
    BMP_SYS_9,
};

AT(.text.lcd.table)
const u32 lcd_snum_table[10] = {
    BMP_SYS_S0,
    BMP_SYS_S1,
    BMP_SYS_S2,
    BMP_SYS_S3,
    BMP_SYS_S4,
    BMP_SYS_S5,
    BMP_SYS_S6,
    BMP_SYS_S7,
    BMP_SYS_S8,
    BMP_SYS_S9,
};

AT(.text.lcd.table)
const u32 lcd_ssnum_table[10] = {
    BMP_SYS_SS0,
    BMP_SYS_SS1,
    BMP_SYS_SS2,
    BMP_SYS_SS3,
    BMP_SYS_SS4,
    BMP_SYS_SS5,
    BMP_SYS_SS6,
    BMP_SYS_SS7,
    BMP_SYS_SS8,
    BMP_SYS_SS9,
};

AT(.text.lcd.table)
const u32 lcd_eq_table[7] = {
    BMP_SYS_EQ_NOR,
    BMP_SYS_EQ_POP,
    BMP_SYS_EQ_ROC,
    BMP_SYS_EQ_JAZZ,
    BMP_SYS_EQ_CLAS,
    BMP_SYS_EQ_COU,
    BMP_SYS_EQ_BAS,
};

AT(.text.lcd.table)
const u32 lcd_msc_type_table[5] = {
    BMP_SYS_WAV,
    BMP_SYS_MP3,
    BMP_SYS_WMA,
    BMP_SYS_APE,
    BMP_SYS_FLAC,
};

AT(.text.lcd.table)
const u32 lcd_menu_repeat_table[] = {
    BMP_MENU_RPT_ALL,
    BMP_MENU_RPT_ONE,
    BMP_MENU_RPT_FLODER,
    BMP_MENU_RANDOM,
};

AT(.text.lcd.table)
const u32 lcd_menu_eq_table[] = {
    BMP_MENU_NORMAL,
    BMP_MENU_POP,
    BMP_MENU_ROCK,
    BMP_MENU_JAZZ,
    BMP_MENU_CLASSIC,
    BMP_MENU_COUNTRY,
};

AT(.text.display.lcd)
void lcd_disp_poweron(void)
{
    lcd_disp_image(RES_BUF_BMP_FUNC_DAT, BMP_FUNC_POWRON, 0, 2);
}

AT(.text.display.lcd)
void lcd_disp_poweroff(void)
{
    lcd_screen_clear();
    lcd_disp_image(RES_BUF_BMP_FUNC_DAT, BMP_FUNC_POWEROFF, 0, 2);
}

AT(.text.display.lcd)
void lcd_disp_bt(void)
{
    if (f_bt.draw_update) {
        lcd_screen_clear();
        lcd_disp_image(RES_BUF_BMP_FUNC_DAT, BMP_FUNC_BLUETOOTH, 0, 3);
        f_bt.draw_update = 0;
        f_bt.sta_update = 1;
    }

    if (f_bt.sta_update) {
        f_bt.sta_update  = 0;
        if (f_bt.disp_status < BT_STA_CONNECTED) {
            lcd_line_clear(0);
            lcd_disp_str("Disconnect", LINE_CENTER, 0, 0);
        } else {
            lcd_line_clear(0);
            if (f_bt.disp_status == BT_STA_PLAYING) {
                lcd_disp_str("Playing", LINE_CENTER, 0, 0);
            } else if (f_bt.disp_status == BT_STA_INCALL) {
                lcd_disp_str("Calling", LINE_CENTER, 0, 0);
            } else {
                lcd_disp_str("Connected", LINE_CENTER, 0, 0);
            }
        }
    }
}

AT(.text.display.lcd)
void lcd_disp_music(void)
{
    if (f_msc.draw_update) {
        lcd_screen_clear();
        f_msc.draw_update = 0;
        f_msc.sta_update = 1;
        f_msc.fname_update = 1;
    }
    func_music_disp_stauts();

    if (!bsp_lrc_is_ready()) {
        func_music_disp_fname();
        func_music_disp_freq_spectrum();
    } else {
        func_music_disp_lrc();
    }
}

AT(.text.display.lcd)
void lcd_disp_fmrx_freq(void)
{
#if FUNC_FMRX_EN
    u8 *buf, x = 50, num;
    u16 freq = fmrx_cb.disp_freq / 10;

    //FM频率显示
    buf = &lcd_buf[COLUMN_MAX * 2];
    memset(buf + x, 0, COLUMN_MAX - x);
    memset(buf + x + COLUMN_MAX, 0, COLUMN_MAX - x);
    num = freq / 1000;
    if (num) {
        memcpy(buf + x, lcd_fmrx_buf + num*MID_NUM_SIZE, MID_NUM_WIDTH);
        memcpy(buf + x + COLUMN_MAX, lcd_fmrx_buf + num*MID_NUM_SIZE + MID_NUM_WIDTH, MID_NUM_WIDTH);
    }
    x += MID_NUM_WIDTH;
    freq %= 1000;

    num = freq / 100;
    memcpy(buf + x, lcd_fmrx_buf + num*MID_NUM_SIZE, MID_NUM_WIDTH);
    memcpy(buf + x + COLUMN_MAX, lcd_fmrx_buf + num*MID_NUM_SIZE + MID_NUM_WIDTH, MID_NUM_WIDTH);
    x += MID_NUM_WIDTH;
    freq %= 100;

    num = freq / 10;
    memcpy(buf + x, lcd_fmrx_buf + num*MID_NUM_SIZE, MID_NUM_WIDTH);
    memcpy(buf + x + COLUMN_MAX, lcd_fmrx_buf + num*MID_NUM_SIZE + MID_NUM_WIDTH, MID_NUM_WIDTH);
    x += MID_NUM_WIDTH;
    freq %= 10;

    x += 2;
    *(buf + x) = 0;
    *(buf + x + COLUMN_MAX) = 0x18;
    x += 3;

    num = freq;
    memcpy(buf + x, lcd_fmrx_buf + num*MID_NUM_SIZE, MID_NUM_WIDTH);
    memcpy(buf + x + COLUMN_MAX, lcd_fmrx_buf + num*MID_NUM_SIZE + MID_NUM_WIDTH, MID_NUM_WIDTH);


    //FM频率箭头
    buf = &lcd_buf[COLUMN_MAX * 4];
    memset(buf, 0, COLUMN_MAX);
    x = (fmrx_cb.disp_freq/10 - 861) >> 1;
    memcpy(buf + x, lcd_fmrx_buf + 10*MID_NUM_SIZE, 8);
    lcd_line_update(2, 3);
#endif // FUNC_FMRX_EN
}

AT(.text.display.lcd)
void lcd_disp_aux(void)
{
#if FUNC_AUX_EN
    if (f_aux.disp_update) {
        f_aux.disp_update = 0;
        lcd_buf_clear();

        lcd_disp_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_LINEIN, 3, 3);
        if (f_aux.pause) {
            lcd_disp_str("Pause", LINE_CENTER, 0, 0);
        } else {
            lcd_disp_str("Playing", LINE_CENTER, 0, 0);
        }
    }
#endif // FUNC_AUX_EN
}

AT(.text.display.lcd)
void lcd_disp_rtc(void)
{
#if FUNC_CLOCK_EN
    char char_buf[8];
    u32 time = rtc_tm_to_time(&rtc_tm);
    if (tm_cb.disp_time != time) {
        tm_cb.disp_time = time;
        tm_cb.disp_update = 1;
    }

    if (tm_cb.setting) {
        if (tm_cb.flicker_sta != box_cb.flicker.sta) {
            tm_cb.flicker_sta = box_cb.flicker.sta;
            tm_cb.disp_update = 1;
        }
    } else {
        tm_cb.flicker_sta = 0;
    }

    if (tm_cb.disp_update) {
        tm_cb.disp_update = 0;
        lcd_buf_clear();

        //显示星期
        memset(char_buf, 0, sizeof(char_buf));
        char_buf[0] = 0xD0;
        char_buf[1] = 0xC7;
        char_buf[2] = 0xC6;
        char_buf[3] = 0xDA;
        char_buf[4] = week_gbk[rtc_tm.tm_wday*2];
        char_buf[5] = week_gbk[rtc_tm.tm_wday*2 + 1];
        lcd_draw_text(char_buf, 80, 0, 0);
        lcd_line_update(0, 2);

        //显示时间
        lcd_disp_num(rtc_tm.tm_hour, 5, 2, DN_BIG | DN_RIGHT | DN_ZERO | 2);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_BCOLON, 37, 2);
        lcd_disp_num(rtc_tm.tm_min, 48, 2, DN_BIG | DN_RIGHT | DN_ZERO | 2);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_BCOLON, 80, 2);
        lcd_disp_num(rtc_tm.tm_sec, 0 + 91, 2, DN_BIG | DN_RIGHT | DN_ZERO | 2);
        if ((tm_cb.flicker_sta) && (tm_cb.setting)) {
            if (tm_cb.index == 1) {
                lcd_disp_invert(48, 2, 32, 3);      //min
            } else if (tm_cb.index == 2) {
                lcd_disp_invert(5, 2, 32, 3);       //hour
            }
        }
        lcd_line_update(2, 3);

        //显示日期
        lcd_disp_num(rtc_tm.tm_year+1970, 5, 6, DN_RIGHT | DN_ZERO | 4);
        memset(char_buf, 0, sizeof(char_buf));
        char_buf[0] = 0xC4;
        char_buf[1] = 0xEA;
        lcd_draw_text(char_buf, 39, 6, 0);
        lcd_disp_num(rtc_tm.tm_mon, 55, 6, DN_RIGHT | DN_ZERO | 2);
        char_buf[0] = 0xD4;
        char_buf[1] = 0xC2;
        lcd_draw_text(char_buf, 73, 6, 0);
        lcd_disp_num(rtc_tm.tm_mday, 89, 6, DN_RIGHT | DN_ZERO | 2);
        char_buf[0] = 0xC8;
        char_buf[1] = 0xD5;
        lcd_draw_text(char_buf, 107, 6, 0);
        if ((tm_cb.flicker_sta) && (tm_cb.setting)) {
            if (tm_cb.index == 3) {
                lcd_disp_invert(89, 6, 16, 2);      //day
            } else if (tm_cb.index == 4) {
                lcd_disp_invert(55, 6, 16, 2);       //month
            } else if (tm_cb.index == 5) {
                lcd_disp_invert(5, 6, 32, 2);       //year
            }
        }
        lcd_line_update(6, 2);
    }
#endif // FUNC_CLOCK_EN
}

AT(.text.display.lcd)
void lcd_disp_speaker(void)
{
#if FUNC_SPEAKER_EN
    if (f_spk.draw_update) {
        lcd_screen_clear();
        lcd_disp_str("Speaker", LINE_CENTER, 0, 0);
        f_spk.draw_update = 0;
        nmv_cb.ms_cnt = 0;
    }
    if (!nmv_cb.ms_cnt) {
        lcd_line_clear(3);
        lcd_disp_image(RES_BUF_BMP_FUNC_DAT, BMP_FUNC_SPK2, 48, 3);
    } else if (nmv_cb.ms_cnt == 10) {
        lcd_line_clear(3);
        lcd_disp_image(RES_BUF_BMP_FUNC_DAT, BMP_FUNC_SPK1, 48, 3);
    } else if (nmv_cb.ms_cnt == 20) {
        lcd_line_clear(3);
        lcd_disp_image(RES_BUF_BMP_FUNC_DAT, BMP_FUNC_SPK3, 48, 3);
    } else if (nmv_cb.ms_cnt >= 30) {
        nmv_cb.ms_cnt = 0;
    }
#endif // FUNC_SPEAKER_EN
}

AT(.text.display.lcd)
void lcd_disp_usbdev(void)
{
    if (f_ude.rw_sta != f_ude.disp_sta) {
        f_ude.disp_sta = f_ude.rw_sta;
        f_ude.disp_update = 1;
    }

    if (f_ude.disp_update) {
        f_ude.disp_update = 0;
        lcd_line_clear(2);
        if (f_ude.rw_sta == RW_WRITE) {
            lcd_disp_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_UDE_WRITE, 0, 2);
        } else if (f_ude.rw_sta == RW_READ) {
            lcd_disp_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_UDE_READ, 0, 2);
        } else {
            lcd_disp_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_UDE_IDLE, 0, 2);
        }
    }
}

AT(.text.display.lcd)
void lcd_disp_spdif(void)
{
    if (f_spdif.disp_update) {
        f_spdif.disp_update = 0;
        lcd_buf_clear();
        lcd_draw_image(RES_BUF_BMP_FUNC_DAT, BMP_FUNC_SPDIF, 8, 2);
        lcd_line_update(0, 8);
    }
}

AT(.text.display.lcd)
void lcd_disp_record(void)
{
#if FUNC_REC_EN
    if (rec_lcd.disp_draw) {
        rec_lcd.disp_draw = 0;
        rec_lcd.disp_sta = 1;
        rec_lcd.time_update = 1;
        rec_lcd.tm_sec = rec_cb.tm_sec;

        lcd_buf_clear();
        u8 pos = fname_buf[99];
        lcd_draw_text((char *)&fname_buf[pos], LINE_CENTER, 4, 0);
        lcd_draw_text("Recording", LINE_CENTER, 6, 0);
        lcd_disp_invert(0, 6, COLUMN_MAX, 2);
        lcd_line_update(4, 4);
    }

    if (rec_lcd.tm_sec != rec_cb.tm_sec) {
        rec_lcd.tm_sec = rec_cb.tm_sec;
        rec_lcd.time_update = 1;
    }

    if (rec_lcd.bps != get_rec_mp3_bps()) {
        rec_lcd.bps = get_rec_mp3_bps();
        rec_lcd.disp_sta = 1;
    }

    if (rec_lcd.disp_sta) {
        rec_lcd.disp_sta = 0;
        lcd_line_clear(0);
        if (func_cb.sta == FUNC_FMRX) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_RADIO, 0, 0);
        } else if (func_cb.sta == FUNC_AUX) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_ICON_AUX, 0, 0);
        } else if (func_cb.sta == FUNC_SPEAKER) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_ICON_MIC, 0, 0);
        } else if (func_cb.sta == FUNC_BT) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_ICON_BT, 0, 0);
        } else if (func_cb.sta == FUNC_MUSIC) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_ICON_MSC, 0, 0);
        }
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_KBPS, 60, 0);
        lcd_disp_num(rec_lcd.bps, 62, 0, DN_RIGHT | DN_ZERO | DN_SSMALL | 3);
        if (sys_cb.cur_dev == DEV_UDISK) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_UDISK, 100, 0);
        } else {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_SDCARD, 100, 0);
        }
        lcd_line_update(0, 2);
    }

    if (rec_lcd.time_update) {
        rec_lcd.time_update = 0;
        lcd_line_clear(2);
        lcd_disp_medium_atime(rec_lcd.tm_sec / 3600, (rec_lcd.tm_sec / 60) % 60, rec_lcd.tm_sec % 60, 32, 2);
        lcd_line_update(2, 2);
    }
#endif // FUNC_REC_EN
}

AT(.text.display.lcd)
void lcd_disp_record_play(void)
{
#if REC_AUTO_PLAY
    rec_play_cb_t *s = &rec_play_cb;

    if (s->draw_update) {
        s->draw_update = 0;
        s->sta_update = 1;
        s->time_update = 1;

        lcd_buf_clear();
        u8 pos = fname_buf[99];
        lcd_draw_text((char *)&fname_buf[pos], LINE_CENTER, 4, 0);
        lcd_draw_text("Playback", LINE_CENTER, 6, 0);
        lcd_disp_invert(0, 6, COLUMN_MAX, 2);
        lcd_line_update(4, 4);
    }

    if ((s->disptime.min != s->curtime.min) || (s->disptime.sec != s->curtime.sec)) {
        s->disptime.min = s->curtime.min;
        s->disptime.sec = s->curtime.sec;
        s->time_update = 1;
    }

    if (s->sta_update) {
        s->sta_update = 0;

        lcd_line_clear(0);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_ICON_PLAYBACK, 0, 0);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, lcd_eq_table[sys_cb.eq_mode], 17+15+34, 0);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, lcd_msc_type_table[s->type - 1], 86, 0);
        lcd_disp_num(s->bit_rate/1000, 88, 0, DN_RIGHT | DN_ZERO | DN_SSMALL | 3);
        if (sys_cb.cur_dev == DEV_UDISK) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_UDISK, 110, 0);
        } else {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_SDCARD, 110, 0);
        }
        lcd_line_update(0, 2);

        lcd_line_clear(6);
        if (!rec_play_cb.pause) {
            lcd_draw_text("Playback", LINE_CENTER, 6, 0);
        } else {
            lcd_draw_text("Pause", LINE_CENTER, 6, 0);
        }
        lcd_disp_invert(0, 6, COLUMN_MAX, 2);
        lcd_line_update(6, 2);
    }

    if (s->time_update) {
        s->time_update = 0;
        lcd_line_clear(2);
        lcd_disp_medium_time(s->curtime.min, s->curtime.sec, 21, 2);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_SPRIT, 60, 2);
        if (s->alltime.min != 0xff) {
            lcd_disp_medium_time(s->alltime.min, s->alltime.sec, 68, 2);
        }
        lcd_line_update(2, 2);
    }
#endif // REC_AUTO_PLAY
}

AT(.text.display.lcd)
void lcd_disp_playmode(void)
{
    lcd_disp_msgbox("Play Mode", lcd_menu_repeat_table[sys_cb.play_mode]);
}

AT(.text.display.lcd)
void lcd_disp_eq(void)
{
    lcd_disp_msgbox("Eq Mode", lcd_menu_eq_table[sys_cb.eq_mode]);
}

AT(.text.display.lcd)
void lcd_disp_playdev(void)
{
    lcd_disp_msgbox(NULL, BMP_MENU_DEVICE);
}

AT(.text.display.lcd)
void lcd_disp_volume(void)
{
    lcd_disp_progressbox("Volume", sys_cb.vol, 0, VOL_MAX);
}

void lcd_disp_inputnum(void)
{
    u32 number = box_cb.number;
    if (number > 9999) {
        number = 9999;
    }
    lcd_buf_clear();
    lcd_disp_num(number, 32, 2, DN_BIG | DN_RIGHT | DN_ZERO | 4);
    lcd_line_update(0, 8);
}

AT(.text.display.lcd)
void lcd_disp_dummy(void)
{

}

AT(.text.display.lcd.table)
const PFUNC lcd_disp_pfunc[] = {
    lcd_disp_poweron,
    lcd_disp_volume,
    lcd_disp_dummy,
    lcd_disp_dummy,
    lcd_disp_eq,
    lcd_disp_playmode,
    lcd_disp_dummy,
    lcd_disp_inputnum,
    lcd_disp_dummy,
    lcd_disp_dummy,
    lcd_disp_aux,
    lcd_disp_bt,
    lcd_disp_usbdev,
    lcd_disp_rtc,
    lcd_disp_rtc,
    lcd_disp_record,
    lcd_disp_record_play,
    lcd_disp_dummy,
    lcd_disp_spdif,
    lcd_disp_dummy,
    lcd_disp_speaker,
    lcd_disp_playdev,
    lcd_disp_dummy,
    lcd_disp_poweroff,
};

AT(.text.lcd)
void lcd_display(u8 disp_num)
{
    void (*pfunc)(void);
    pfunc = lcd_disp_pfunc[disp_num];
    (*pfunc)();
}
#endif
