#include "include.h"
#include "func.h"
#include "func_fmrx.h"

#if (GUI_SELECT != GUI_NO)
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

AT(.text.func.fmrx.display)
void func_fmrx_enter_display(void)
{
    gui_display(DISP_FREQ);
}

AT(.text.func.fmrx.display)
void func_fmrx_exit_display(void)
{
}
#endif
