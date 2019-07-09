#include "include.h"
#include "func.h"
#include "func_music.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.music.display)
void func_music_display(void)
{
    gui_box_process();

    switch (box_cb.disp_sta) {
    case GUI_BOX_NULL:
        /*if(f_msc.pause)
            gui_display(DISP_MUSIC_PAUSE);
        else
            gui_display(DISP_MUSIC_PLAY);
        break;*/
        gui_disp_clock();
    default:
        gui_box_display();
        break;
    }
}

AT(.text.func.music.display)
void func_music_enter_display(void)
{
}

AT(.text.func.music.display)
void func_music_exit_display(void)
{
}

#endif

