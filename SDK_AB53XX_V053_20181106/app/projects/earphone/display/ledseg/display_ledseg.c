#include "include.h"
#include "ledseg/display_ledseg.h"

#if (GUI_SELECT & DISPLAY_LEDSEG)
extern u32 fmam_freq;
extern const u8 ledseg_num_table[10];
typedef void (*PFUNC) (void);

AT(.rodata.ledseg)
const u8 ledseg_eq_table[7][3] =
{
    {T_LEDSEG_N, T_LEDSEG_O, T_LEDSEG_R},   //NORMAL
    {T_LEDSEG_P, T_LEDSEG_O, T_LEDSEG_P},   //POP
    {T_LEDSEG_R, T_LEDSEG_O, T_LEDSEG_C},   //ROCK
    {T_LEDSEG_J, T_LEDSEG_A, T_LEDSEG_Z},   //JAZZ
    {T_LEDSEG_C, T_LEDSEG_L, T_LEDSEG_A},   //CLASSIC
    {T_LEDSEG_C, T_LEDSEG_U, T_LEDSEG_T},   //COUNTRY
    {T_LEDSEG_B, T_LEDSEG_A, T_LEDSEG_S},   //BASS
};

AT(.rodata.ledseg)
const u8 ledseg_playmode_table[4][3] =
{
    {T_LEDSEG_A, T_LEDSEG_L, T_LEDSEG_L},   //全部循环
    {T_LEDSEG_O, T_LEDSEG_N, T_LEDSEG_E},   //单曲循环
    {T_LEDSEG_F, T_LEDSEG_O, T_LEDSEG_D},   //文件夹循环
    {T_LEDSEG_R, T_LEDSEG_D, T_LEDSEG_O},   //随机播放
};

//Display functions icon
AT(.text.ledseg)
static void ledseg_msc_icon(void)
{
    ledseg_buf[4] |= ICON_MP3;
    if (f_msc.cur_dev == DEV_SDCARD) {
        ledseg_buf[4] |= ICON_SD;
    } else if (f_msc.cur_dev == DEV_UDISK) {
        ledseg_buf[4] |= ICON_USB;
    }
}

//mute时全部闪烁
AT(.text.display.ledseg)
static bool is_mute_flicker(void)
{
    if ((box_cb.flicker.sta) && (sys_cb.mute)) {
        return true;
    } else {
        return false;
    }
}

AT(.text.display.ledseg)
void ledseg_disp_welcome(void)
{
    ledseg_buf[1] = T_LEDSEG_H;
    ledseg_buf[2] = T_LEDSEG_I;
}

AT(.text.display.ledseg)
void ledseg_disp_volume(void)
{
    if(bt_get_status() == BT_STA_INCALL) {
        ledseg_disp_number(sys_cb.hfp_vol * sys_cb.hfp2sys_mul);
    } else {
        ledseg_disp_number(sys_cb.vol);
    }
    ledseg_buf[0] = T_LEDSEG_OFF;
    ledseg_buf[1] = T_LEDSEG_U;
}

//Display music playing time
AT(.text.display.ledseg)
void ledseg_disp_playtime(void)
{
    u8 minute = f_msc.curtime.min;

    if (is_mute_flicker()) {
        return;
    }

    if (minute > 100) {
        minute = 99;
    }
    ledseg_buf[0] = ledseg_num_table[minute / 10];
    ledseg_buf[1] = ledseg_num_table[minute % 10];
    ledseg_buf[2] = ledseg_num_table[f_msc.curtime.sec / 10];
    ledseg_buf[3] = ledseg_num_table[f_msc.curtime.sec % 10];
    ledseg_buf[4] |= (ICON_PLAY | ICON_DDOT);
    ledseg_msc_icon();
}

AT(.text.display.ledseg)
void ledseg_disp_pause(void)
{
    ledseg_buf[0] = T_LEDSEG_P;
    ledseg_buf[1] = T_LEDSEG_A;
    ledseg_buf[2] = T_LEDSEG_U;
    ledseg_buf[3] = T_LEDSEG_S;
    ledseg_buf[4] |= ICON_PAUSE;
    ledseg_msc_icon();
}

AT(.text.display.ledseg)
void ledseg_disp_eq(void)
{
    ledseg_buf[1] = ledseg_eq_table[sys_cb.eq_mode][0];
    ledseg_buf[2] = ledseg_eq_table[sys_cb.eq_mode][1];
    ledseg_buf[3] = ledseg_eq_table[sys_cb.eq_mode][2];
}

AT(.text.display.ledseg)
void ledseg_disp_playmode(void)
{
    ledseg_buf[1] = ledseg_playmode_table[sys_cb.play_mode][0];
    ledseg_buf[2] = ledseg_playmode_table[sys_cb.play_mode][1];
    ledseg_buf[3] = ledseg_playmode_table[sys_cb.play_mode][2];
    ledseg_msc_icon();
}

AT(.text.display.ledseg)
void ledseg_disp_music_num(void)
{
    ledseg_disp_number(box_cb.number);
    ledseg_msc_icon();
    if (box_cb.disp_sta == GUI_BOX_NUMBOX) {
        ledseg_buf[4] |= ICON_PLAY;
    }
}

AT(.text.display.ledseg)
void ledseg_disp_freq(void)
{
#if FUNC_FMRX_EN
    if (is_mute_flicker()) {
        return;
    }
    ledseg_disp_number(fmrx_cb.freq / 10);
    if (ledseg_buf[0] == T_LEDSEG_0) {
        ledseg_buf[0] = T_LEDSEG_OFF;
    }
    ledseg_buf[4] |= ICON_FM;
    if (fmrx_cb.sta == FMRX_PAUSE) {
        ledseg_buf[4] |= ICON_PAUSE;
    }
#endif // FUNC_FMRX_EN
}

AT(.text.display.ledseg)
void ledseg_disp_freq_ch(void)
{
    ledseg_disp_number(fmrx_cb.ch_cur);
    ledseg_buf[0] = T_LEDSEG_C;
    ledseg_buf[1] = T_LEDSEG_H;
    ledseg_buf[4] |= ICON_FM;
}

AT(.text.display.ledseg)
void ledseg_disp_aux(void)
{
#if FUNC_AUX_EN
    if (is_mute_flicker()) {
        return;
    }
    ledseg_buf[1] = T_LEDSEG_A;
    ledseg_buf[2] = T_LEDSEG_U;
    ledseg_buf[3] = T_LEDSEG_X;
    ledseg_buf[4] |= ICON_AUX;
    if (f_aux.pause) {
        ledseg_buf[4] |= ICON_PAUSE;
    }
#endif // FUNC_AUX_EN
}

AT(.text.display.ledseg)
void ledseg_disp_bt(void)
{
    if (is_mute_flicker()) {
        return;
    }

#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        ledseg_buf[1] = T_LEDSEG_H;
        ledseg_buf[2] = T_LEDSEG_I;
        ledseg_buf[3] = T_LEDSEG_D;
    } else
#endif // FUNC_BTHID_EN
    {
        ledseg_buf[1] = T_LEDSEG_B;
        ledseg_buf[2] = T_LEDSEG_T;
    }
}

AT(.text.display.ledseg)
void ledseg_disp_usbpc(void)
{
    if (is_mute_flicker()) {
        return;
    }
    ledseg_buf[1] = T_LEDSEG_P;
    ledseg_buf[2] = T_LEDSEG_C;
}

AT(.text.display.ledseg)
void ledseg_disp_speaker(void)
{
#if FUNC_SPEAKER_EN
    if (is_mute_flicker()) {
        return;
    }
    ledseg_buf[1] = T_LEDSEG_T;
    ledseg_buf[2] = T_LEDSEG__;
    ledseg_buf[3] = T_LEDSEG_T;
    if (f_spk.pause) {
        ledseg_buf[4] |= ICON_PAUSE;
    }
#endif // FUNC_SPEAKER_EN
}

AT(.text.display.ledseg)
void ledseg_disp_rtctime(void)
{
#if FUNC_CLOCK_EN
    ledseg_buf[0] = ledseg_num_table[rtc_tm.tm_hour / 10];
    ledseg_buf[1] = ledseg_num_table[rtc_tm.tm_hour % 10];
    ledseg_buf[2] = ledseg_num_table[rtc_tm.tm_min / 10];
    ledseg_buf[3] = ledseg_num_table[rtc_tm.tm_min % 10];
    if (rtc_tm.tm_sec % 2) {
        ledseg_buf[4] |= ICON_DDOT;
    }
#endif // FUNC_CLOCK_EN
}

AT(.text.display.ledseg)
void ledseg_disp_time_set(void)
{
#if FUNC_CLOCK_EN
    ledseg_disp_rtctime();
    ledseg_buf[4] |= ICON_DDOT;
    if (box_cb.flicker.sta == 0) {
        if (box_cb.flicker.pos == 1) {              //分钟闪烁
            ledseg_buf[2] = 0x00;
            ledseg_buf[3] = 0x00;
        } else if (box_cb.flicker.pos == 2) {       //小时闪烁
            ledseg_buf[0] = 0x00;
            ledseg_buf[1] = 0x00;
        }
    }
#endif // FUNC_CLOCK_EN
}

AT(.text.display.ledseg)
void ledseg_disp_rectime(void)
{
#if FUNC_REC_EN
    u8 sec, minute;
    sec = rec_cb.tm_sec % 60;
    minute = rec_cb.tm_sec / 60;
    if (minute > 99) {
        minute = 99;
    }
    ledseg_buf[0] = ledseg_num_table[minute / 10];
    ledseg_buf[1] = ledseg_num_table[minute % 10];
    ledseg_buf[2] = ledseg_num_table[sec / 10];
    ledseg_buf[3] = ledseg_num_table[sec % 10];
    if (rec_cb.tm_sec % 2)
        ledseg_buf[4] |= ICON_DDOT;
    if (sys_cb.cur_dev == DEV_SDCARD) {
        ledseg_buf[4] |= ICON_SD;
    } else if (sys_cb.cur_dev == DEV_UDISK) {
        ledseg_buf[4] |= ICON_USB;
    }
#endif // FUNC_REC_EN
}

AT(.text.display.ledseg)
void ledseg_disp_idle(void)
{
    ledseg_buf[0] = T_LEDSEG_I;
    ledseg_buf[1] = T_LEDSEG_D;
    ledseg_buf[2] = T_LEDSEG_L;
    ledseg_buf[3] = T_LEDSEG_E;
}

AT(.text.display.ledseg)
void ledseg_disp_off(void)
{
    ledseg_buf[1] = T_LEDSEG_O;
    ledseg_buf[2] = T_LEDSEG_F;
    ledseg_buf[3] = T_LEDSEG_F;
}

AT(.text.display.ledseg)
void ledseg_disp_fmam(void)
{
#if FUNC_FMAM_FREQ_EN
    if (fmam_freq > 9999) {
        ledseg_disp_number(fmam_freq/100);      //FM显示100K为单位
        if (ledseg_buf[0] == T_LEDSEG_0) {
            ledseg_buf[0] = T_LEDSEG_OFF;
        }
        ledseg_buf[4] |= ICON_FM;
    } else {
        ledseg_disp_number(fmam_freq);          //AM显示1K为单位
        if (ledseg_buf[0] == T_LEDSEG_0) {
            ledseg_buf[0] = T_LEDSEG_OFF;
        }
        ledseg_buf[4] |= ICON_MP3;
    }
#endif // FUNC_FMAM_FREQ_EN
}

AT(.text.display.ledseg)
void ledseg_disp_playdev(void)
{
    if (sys_cb.cur_dev == DEV_SDCARD) {
        ledseg_buf[1] = T_LEDSEG_S;
        ledseg_buf[2] = T_LEDSEG_D;
    } else {
        ledseg_buf[1] = T_LEDSEG_U;
        ledseg_buf[2] = T_LEDSEG_S;
        ledseg_buf[3] = T_LEDSEG_B;
    }
}

AT(.text.display.ledseg)
void ledseg_disp_dummy(void)
{

}

AT(.text.display.ledseg.table)
const PFUNC ledseg_disp_pfunc[] = {
    ledseg_disp_welcome,
    ledseg_disp_volume,
    ledseg_disp_playtime,
    ledseg_disp_pause,
    ledseg_disp_eq,
    ledseg_disp_playmode,
    ledseg_disp_music_num,
    ledseg_disp_freq,
    ledseg_disp_freq_ch,
    ledseg_disp_aux,
    ledseg_disp_bt,
    ledseg_disp_usbpc,
    ledseg_disp_rtctime,
    ledseg_disp_time_set,
    ledseg_disp_rectime,
    ledseg_disp_fmam,
    ledseg_disp_speaker,
    ledseg_disp_playdev,
    ledseg_disp_idle,
    ledseg_disp_off,
};
#endif

