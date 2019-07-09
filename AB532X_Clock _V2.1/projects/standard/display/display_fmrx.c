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
        if(fmrx_cb.sta < FMRX_PLAY){
			gui_display(DISP_FREQ);
		}else {
			gui_disp_clock();
		}
        break;
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.fmrx.display)
void func_fmrx_enter_display(void)
{
	box_cb.sta = GUI_BOX_FMFREQ;
    box_cb.times = BOX_TIMES*2;
    box_cb.update = 1;
    gui_display(DISP_FREQ);
}

AT(.text.func.fmrx.display)
void func_fmrx_exit_display(void)
{
}
#endif
