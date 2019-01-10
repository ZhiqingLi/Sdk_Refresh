#include "include.h"
#include "func.h"
#include "func_clock.h"

#if ((GUI_SELECT != GUI_NO) && (FUNC_CLOCK_EN))
AT(.text.func.clock.display)
void func_clock_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        if (tm_cb.setting) {
            gui_display(DISP_TIME_SET);
        } else {
            gui_display(DISP_RTCTIME);
        }
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.clock.display)
void func_clock_enter_display(void)
{
    gui_display(DISP_RTCTIME);
}

AT(.text.func.clock.display)
void func_clock_exit_display(void)
{
}
#endif
