#include "include.h"
#include "func.h"
#include "func_fmrx.h"

#if (GUI_SELECT != GUI_NO)
#if !GUI_LCD_EN
AT(.text.func.fmrx.display)
void func_fmrx_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_FREQ);
        break;
    default:
        gui_box_display();
        break;
    }
}
#else

AT(.text.func.fmrx.display)
void func_fmrx_freq_display(void)
{
    if (fmrx_cb.disp_freq != fmrx_cb.freq) {
        fmrx_cb.disp_freq = fmrx_cb.freq;
        fmrx_cb.freq_update = 1;
    }

    if (fmrx_cb.freq_update) {
        fmrx_cb.freq_update = 0;
        lcd_disp_fmrx_freq();
    }
}

AT(.text.func.fmrx.display)
void func_fmrx_ch_display(void)
{
    if (fmrx_cb.diap_ch != fmrx_cb.ch_cur) {
        fmrx_cb.diap_ch = fmrx_cb.ch_cur;
        fmrx_cb.ch_update = 1;
    }
    if (fmrx_cb.ch_update) {
        fmrx_cb.ch_update = 0;
        lcd_disp_num(fmrx_cb.ch_cur, 71, 0, DN_RIGHT | DN_ZERO | 2);
        lcd_line_update(0, 2);
    }
}

AT(.text.func.fmrx.display)
void func_fmrx_display_init(void)
{
    lcd_screen_clear();
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_RADIO, 0, 0);
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_CH, 55, 0);
    lcd_disp_num(fmrx_cb.ch_cur, 71, 0, DN_RIGHT | DN_ZERO | 2);
    lcd_line_update(0, 2);
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_FM, 26, 2);
    lcd_line_update(2, 2);
    lcd_disp_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_FMQT, 0, 5);
    fmrx_cb.disp_freq = fmrx_cb.freq;
    fmrx_cb.diap_ch = fmrx_cb.ch_cur;
    fmrx_cb.ch_update = 1;
    fmrx_cb.freq_update = 1;
}

AT(.text.func.fmrx.display)
void func_fmrx_display(void)
{
    if (gui_box_process() && ((!box_cb.disp_sta) || (box_cb.disp_sta == GUI_BOX_CHANBOX))) {
        //从控件显示回到主界面
        func_fmrx_display_init();
    }

    switch (box_cb.disp_sta) {
    case GUI_BOX_CHANBOX:
        fmrx_cb.ch_update = 1;
    case GUI_BOX_NULL:
        func_fmrx_freq_display();
        func_fmrx_ch_display();
        break;
    default:
        gui_box_display();
        break;
    }
}
#endif

AT(.text.func.fmrx.display)
void func_fmrx_enter_display(void)
{
#if GUI_LCD_EN
    int i;
    func_fmrx_display_init();
    for (i = 0; i < 10; i++) {
        memcpy(&lcd_fmrx_buf[MID_NUM_SIZE*i], (u8 *)(RES_BUF_BMP_SYS_DAT + (lcd_mnum_table[i] & 0xfffff)), MID_NUM_SIZE);
    }
    memcpy(&lcd_fmrx_buf[160], (u8 *)(RES_BUF_BMP_SYS_DAT + (BMP_SYS_FMG & 0xfffff)), 8);
    func_fmrx_freq_display();
#else
    gui_display(DISP_FREQ);
#endif // GUI_LCD_EN
}

AT(.text.func.fmrx.display)
void func_fmrx_exit_display(void)
{
}
#endif
