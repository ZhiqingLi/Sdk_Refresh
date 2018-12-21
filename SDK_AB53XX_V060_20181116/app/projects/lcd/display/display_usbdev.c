#include "include.h"
#include "func.h"
#include "func_usbdev.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.usbdev.display)
void func_usbdev_display(void)
{
#if GUI_LCD_EN
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        lcd_screen_clear();
        f_ude.disp_update = 1;
    }
#else
    gui_box_process();
#endif // GUI_LCD_EN

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_USBPC);
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.usbdev.display)
void func_usbdev_enter_display(void)
{
#if GUI_LCD_EN
    f_ude.disp_sta = RW_IDLE;
    f_ude.disp_update = 1;
    lcd_screen_clear();
#endif // GUI_LCD_EN
    gui_display(DISP_USBPC);
}

AT(.text.func.usbdev.display)
void func_usbdev_exit_display(void)
{
}
#endif
