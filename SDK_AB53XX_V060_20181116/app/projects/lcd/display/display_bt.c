#include "include.h"
#include "func.h"
#include "func_bt.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.bt.display)
void func_bt_display(void)
{
#if GUI_LCD_EN
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        f_bt.draw_update = 1;
    }
#else
    gui_box_process();
#endif // GUI_LCD_EN

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_BT);
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.bt.display)
void func_bt_enter_display(void)
{
#if GUI_LCD_EN
    f_bt.draw_update = 1;
    f_bt.sta_update = 0;
#endif // GUI_LCD_EN
    gui_display(DISP_BT);
}

AT(.text.func.bt.display)
void func_bt_exit_display(void)
{
}
#endif

