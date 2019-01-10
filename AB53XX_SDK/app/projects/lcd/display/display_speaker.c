#include "include.h"
#include "func.h"

#if (GUI_SELECT != GUI_NO)
AT(.text.func.speaker.display)
void func_speaker_display(void)
{
#if GUI_LCD_EN
    if (gui_box_process() && (!box_cb.disp_sta)) {
        //从控件显示回到主界面
        f_spk.draw_update = 1;
    }
#else
    gui_box_process();
#endif // GUI_LCD_EN

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
#if GUI_LCD_EN
    f_spk.draw_update = 1;
#endif // GUI_LCD_EN
    gui_display(DISP_SPEAKER);
}

AT(.text.func.speaker.display)
void func_speaker_exit_display(void)
{
}
#endif
