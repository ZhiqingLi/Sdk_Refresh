#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.fmam.display)
void func_fmam_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_FMAM);
        break;
    default:
        gui_box_display();
        break;
    }
}
#endif
