#include "include.h"
#include "func.h"
#include "func_usbdev.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.usbdev.display)
void func_usbdev_display(void)
{
    gui_box_process();

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
    gui_display(DISP_USBPC);
}

AT(.text.func.usbdev.display)
void func_usbdev_exit_display(void)
{
}
#endif
