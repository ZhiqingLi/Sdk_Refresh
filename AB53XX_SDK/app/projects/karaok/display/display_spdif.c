#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.spdif.display)
void func_spdif_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_SPDIF);
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.spdif.display)
void func_spdif_enter_display(void)
{
    gui_display(DISP_SPDIF);
}

AT(.text.func.spdif.display)
void func_spdif_exit_display(void)
{
    gui_display(DISP_SPDIF);
}
#endif

