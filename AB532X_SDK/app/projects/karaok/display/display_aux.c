#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.aux.display)
void func_aux_display(void)
{
    gui_box_process();

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
    gui_display(DISP_AUX);
}

AT(.text.func.aux.display)
void func_aux_exit_display(void)
{
}

#endif // FUNC_AUX_EN
