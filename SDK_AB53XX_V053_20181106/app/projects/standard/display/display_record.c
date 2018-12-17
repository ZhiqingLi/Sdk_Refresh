#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.record.display)
void sfunc_record_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        gui_display(DISP_REC_TIME);
        break;
    default:
        gui_box_display();
        break;
    }
}

#if REC_AUTO_PLAY
AT(.text.func.record.display)
void sfunc_record_play_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        if (rec_play_cb.pause) {
            gui_display(DISP_REC_PAUSE);
        } else {
            gui_display(DISP_REC_PLAYTIME);
        }
        break;
    default:
        gui_box_display();
        break;
    }
}
#endif

#endif
