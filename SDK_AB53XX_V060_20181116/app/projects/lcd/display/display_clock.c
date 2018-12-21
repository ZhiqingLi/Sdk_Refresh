#include "include.h"
#include "func.h"
#include "func_clock.h"

#if ((GUI_SELECT != GUI_NO) && (FUNC_CLOCK_EN))
AT(.text.func.clock.display)
void func_clock_display(void)
{
#if GUI_LCD_EN
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        lcd_screen_clear();
        tm_cb.disp_update = 1;
    }
#else
    gui_box_process();
#endif // GUI_LCD_EN

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        if (tm_cb.setting) {
            gui_display(DISP_TIME_SET);
        } else {
            gui_display(DISP_RTCTIME);
        }
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.clock.display)
void func_clock_enter_display(void)
{
#if GUI_LCD_EN
    lcd_screen_clear();
    tm_cb.disp_update = 1;
    tm_cb.flicker_sta = 0;
#endif // GUI_LCD_EN
    gui_display(DISP_RTCTIME);
}

AT(.text.func.clock.display)
void func_clock_exit_display(void)
{
}
#endif
