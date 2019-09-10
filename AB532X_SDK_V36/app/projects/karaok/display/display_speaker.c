#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.speaker.display)
void func_speaker_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_SPEAKER);
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.speaker.display)
void func_speaker_enter_display(void)
{
    gui_display(DISP_SPEAKER);
}

AT(.text.func.speaker.display)
void func_speaker_exit_display(void)
{
}
#endif
