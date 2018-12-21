#ifndef _DISPLAY_LCD_H
#define _DISPLAY_LCD_H

#if (GUI_SELECT & DISPLAY_LCD)

#include "bmp_sys.h"
#include "bmp_func.h"
#include "bmp_menu.h"

enum {
    DISP_POWERON = 0,
    DISP_VOL,
    DISP_MUSIC_PLAY,
    DISP_MUSIC_PAUSE,
    DISP_EQ,
    DISP_MODEPLAY,
    DISP_NUM,
    DISP_INPUTNUM,
    DISP_FREQ,
    DISP_FQ_CHAN,
    DISP_AUX,
    DISP_BT,
    DISP_USBPC,
    DISP_RTCTIME,
    DISP_TIME_SET,
    DISP_REC_TIME,
    DISP_REC_PLAYTIME,
    DISP_REC_PAUSE,
    DISP_SPDIF,
    DISP_FMAM,
    DISP_SPEAKER,
    DISP_PLAYDEV,
    DISP_IDLE,
    DISP_OFF,
};

extern const u32 lcd_bnum_table[10];
extern const u32 lcd_mnum_table[10];
extern const u32 lcd_snum_table[10];
extern const u32 lcd_ssnum_table[10];
extern const u32 lcd_eq_table[7];
extern const u32 lcd_msc_type_table[5];

extern u8 lcd_fmrx_buf[168];

void lcd_display(u8 disp_num);
void lcd_disp_music(void);
void lcd_disp_bt(void);
void lcd_disp_fmrx_freq(void);
#endif

#endif // _DISPLAY_LCD_H
