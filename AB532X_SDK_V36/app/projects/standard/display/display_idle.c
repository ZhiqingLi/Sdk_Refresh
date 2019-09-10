#include "include.h"
#include "func.h"
#include "func_idle.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.idle.display)
void func_idle_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_IDLE);
        break;
    default:
        gui_box_display();
        break;
    }
}
#endif
