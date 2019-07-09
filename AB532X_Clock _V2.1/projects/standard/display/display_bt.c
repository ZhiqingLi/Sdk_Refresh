#include "include.h"
#include "func.h"
#include "func_bt.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.bt.display)
void func_bt_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
		gui_disp_clock();
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.bt.display)
void func_bt_enter_display(void)
{
    gui_display(DISP_BT);
}

AT(.text.func.bt.display)
void func_bt_exit_display(void)
{
}
#endif

