#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.record.display)
void sfunc_record_display(void)
{
#if GUI_LCD_EN
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        rec_lcd.disp_draw = 1;
    }
#else
    gui_box_process();
#endif // GUI_LCD_EN

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_REC_TIME);
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.record.display)
void sfunc_record_display_enter(void)
{
#if GUI_LCD_EN
    lcd_screen_clear();
    lcd_disp_msgbox(NULL, BMP_MENU_RECORDING);
    memset(&rec_lcd, 0, sizeof(rec_lcd_t));
    rec_lcd.disp_draw = 1;
    rec_lcd.tm_sec = 0;
    rec_lcd.bps = 0;
#endif // GUI_LCD_EN
}

#if REC_AUTO_PLAY
void sfunc_record_play_enter_display(void)
{
#if GUI_LCD_EN
    lcd_screen_clear();
    rec_play_cb.draw_update = 1;
    rec_play_cb.disptime.min = 0;
    rec_play_cb.disptime.sec = 0;
#endif // GUI_LCD_EN
}

#if GUI_LCD_EN
AT(.text.func.record.display)
void sfunc_record_play_display(void)
{
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        rec_play_cb.draw_update = 1;
    }

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_REC_PLAYTIME);
        break;
    default:
        gui_box_display();
        break;
    }
}
#else

AT(.text.func.record.display)
void sfunc_record_play_enter_display(void)
{
}

AT(.text.func.record.display)
void sfunc_record_play_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        if (rec_play_cb.pause) {
            gui_display(DISP_REC_PAUSE);
        } else {
            gui_display(DISP_REC_PLAYTIME);
        }
        break;
    default:
        gui_box_display();
        break;
    }
}
#endif // GUI_LCD_EN
#endif

#endif
