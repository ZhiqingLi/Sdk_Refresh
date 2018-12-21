#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)

AT(.text.func.aux.display)
void func_aux_display(void)
{
#if GUI_LCD_EN
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        lcd_screen_clear();
        f_aux.disp_update = 1;
    }
#else
    gui_box_process();
#endif // GUI_LCD_EN

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_AUX);
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.aux.display)
void func_aux_enter_display(void)
{
#if GUI_LCD_EN
    lcd_screen_clear();
    f_aux.disp_update = 1;
#endif
    gui_display(DISP_AUX);
}

AT(.text.func.aux.display)
void func_aux_exit_display(void)
{
}

#endif // FUNC_AUX_EN
