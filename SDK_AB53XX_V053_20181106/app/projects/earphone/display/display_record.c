#include "include.h"
#include "func.h"
#include "sfunc_record.h"

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
#endif
