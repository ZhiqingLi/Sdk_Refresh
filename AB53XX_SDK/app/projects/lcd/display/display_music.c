#include "include.h"
#include "func.h"
#include "func_music.h"

#if (GUI_SELECT != GUI_NO)

#if !GUI_LCD_EN
AT(.text.func.music.display)
void func_music_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        if(f_msc.pause)
            gui_display(DISP_MUSIC_PAUSE);
        else
            gui_display(DISP_MUSIC_PLAY);
        break;
    default:
        gui_box_display();
        break;
    }
}
#endif

AT(.text.func.music.display)
void func_music_enter_display(void)
{
#if GUI_LCD_EN
    f_msc.draw_update = 1;
#endif
}

AT(.text.func.music.display)
void func_music_exit_display(void)
{
}

#if GUI_LCD_EN
AT(.text.func.music.display)
void func_music_disp_stauts(void)
{
    u32 bit_rate = get_music_bitrate();
    if (bit_rate != f_msc.bit_rate) {
        f_msc.bit_rate = bit_rate;
        f_msc.sta_update = 1;
    }
    if ((f_msc.disptime.min != f_msc.curtime.min) || (f_msc.disptime.sec != f_msc.curtime.sec)) {
        f_msc.disptime.min = f_msc.curtime.min;
        f_msc.disptime.sec = f_msc.curtime.sec;
        f_msc.time_update = 1;
    }
    if (f_msc.sta_update) {
        f_msc.sta_update = 0;
        lcd_line_clear(0);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_SING, 0, 0);
        if (sys_cb.play_mode == SINGLE_MODE) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_PM_ONE,     17, 0);
        } else if (sys_cb.play_mode == FLODER_MODE) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_PM_FLODER,  17, 0);
        } else if (sys_cb.play_mode == RANDOM_MODE) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_PM_RANDOM,  17, 0);
        } else {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_PM_ALL,     17, 0);
        }
        lcd_disp_num(f_msc.file_num, 18+15, 0, DN_RIGHT | DN_ZERO | DN_SMALL | 4);
        lcd_disp_num(f_msc.file_total, 18+15, 1, DN_RIGHT | DN_ZERO | DN_SMALL | 4);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, lcd_eq_table[sys_cb.eq_mode], 17+15+34, 0);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, lcd_msc_type_table[f_msc.type - 1], 86, 0);
        lcd_disp_num(f_msc.bit_rate/1000, 88, 0, DN_RIGHT | DN_ZERO | DN_SSMALL | 3);
        if (f_msc.cur_dev == DEV_UDISK) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_UDISK, 110, 0);
        } else {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_SDCARD, 110, 0);
        }
        lcd_line_update(0, 2);
    }

    if ((f_msc.time_update) && (!bsp_lrc_is_ready())) {
        f_msc.time_update = 0;
        lcd_line_clear(2);
        if (f_msc.pause) {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_PAUSE, 0, 2);
        } else {
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_PLAY, 0, 2);
        }
        lcd_disp_medium_time(f_msc.curtime.min, f_msc.curtime.sec, 21, 2);
        lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_SPRIT, 60, 2);
        if (f_msc.alltime.min != 0xff) {
            lcd_disp_medium_time(f_msc.alltime.min, f_msc.alltime.sec, 68, 2);
        }
        lcd_line_update(2, 2);
    }
}

AT(.text.func.music.display)
void func_music_disp_fname(void)
{
    if (f_msc.fname_update) {
        f_msc.fname_update = 0;
        memset(&nmv_cb, 0, sizeof(nmv_cb));
        nmv_cb.wide = get_name_str_width(f_msc.fname);
        if (nmv_cb.wide > LCD_WIDTH) {
            nmv_cb.en = 1;
        }
        lcd_line_clear(4);
        lcd_disp_str(f_msc.fname, 0, 4, 1);
    }

    if (nmv_cb.en) {
        if (nmv_cb.ms_cnt >= 15) {
            nmv_cb.ms_cnt = 0;
            if ((nmv_cb.x + (s16)nmv_cb.wide) > 106) {
                nmv_cb.x--;
            } else {
                nmv_cb.x = 0;
            }
            lcd_line_clear(4);
            lcd_disp_str(f_msc.fname, nmv_cb.x, 4, 1);
        }
    }
}

AT(.text.func.music.display)
void func_music_disp_freq_spectrum(void)
{
    if ((nmv_cb.ms_cnt % 5) == 0) {
        lcd_line_clear(6);
        lcd_disp_freq_spectrum_line(6);
        lcd_line_update(6, 2);
    }
}


AT(.text.func.music.display)
void func_music_disp_lrc(void)
{
    char *str = f_msc.fname;
    if (f_msc.lrc_update) {
        f_msc.lrc_update = 0;
        lcd_line_clear(2);
        str = lcd_draw_text(str, 0, 2, 0);
        lcd_line_clear(4);
        str = lcd_draw_text(str, 0, 4, 0);
        lcd_line_clear(6);
        str = lcd_draw_text(str, 0, 6, 0);
        lcd_line_update(2, 6);
    }
}


AT(.text.func.music.display)
void func_music_display(void)
{
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        f_msc.sta_update = 1;
        f_msc.time_update = 1;
        f_msc.fname_update = 1;
        f_msc.lrc_update = 1;
    }

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
    case GUI_BOX_NUMBOX:
        lcd_disp_music();
        break;
    default:
        gui_box_display();
        break;
    }
}
#endif

#endif

