#include "include.h"
#include "gui.h"

gui_box_t box_cb;

void gui_init(void)
{
    memset(&box_cb, 0, sizeof(box_cb));

#if (GUI_SELECT == GUI_LEDSEG_7P7S)
    timer1_irq_init();
#endif

#if (GUI_SELECT & DISPLAY_LEDSEG)
    ledseg_init();
#endif
}

//100ms调用一次
AT(.com_text.display)
void gui_box_isr(void)
{
    if (box_cb.times > 0) {
        box_cb.times--;
    }

    if (box_cb.flicker.counter) {
        box_cb.flicker.counter--;
        if (box_cb.flicker.counter == 0) {
            box_cb.flicker.sta = (box_cb.flicker.sta) ? 0 : 1;
            if (box_cb.flicker.times != 0xff) {
                if (box_cb.flicker.times > 0) {
                    box_cb.flicker.times--;
                } else {
                    memset(&box_cb.flicker, 0, sizeof(box_cb.flicker));
                }
            }
            box_cb.flicker.counter = box_cb.flicker.preval;
        }
    }
}

AT(.text.gui)
void gui_box_flicker_set(u8 cnt, u8 times, u8 pos)
{
    box_cb.flicker.times = times;
    box_cb.flicker.counter = cnt;
    box_cb.flicker.preval = cnt;
    box_cb.flicker.sta = 0;
    box_cb.flicker.pos = pos;
}

void gui_box_flicker_clr(void)
{
    memset(&box_cb.flicker, 0, sizeof(box_cb.flicker));
}

AT(.text.gui)
bool gui_box_process(void)
{
    if (box_cb.times == 0) {
        if (box_cb.sta == GUI_BOX_INPUTNUM) {
            if (!box_cb.event) {
                box_cb.event = 1;
                box_cb.sta = GUI_BOX_NULL;
                return false;
            }
        }
        box_cb.sta = GUI_BOX_NULL;                  //控件显示时间到，回到主界面显示
    }
    if (box_cb.sta != box_cb.disp_sta || box_cb.update) {
        box_cb.disp_sta = box_cb.sta;
        box_cb.update = 0;
        return true;
    }
    return false;
}

//显示控件
AT(.text.gui)
void gui_box_display(void)
{
    switch (box_cb.disp_sta) {
    case GUI_BOX_NUMBOX:
    case GUI_BOX_INPUTNUM:
        gui_display(DISP_NUM);
        break;
    case GUI_BOX_VOLBOX:
        gui_display(DISP_VOL);
        break;
    case GUI_BOX_CHANBOX:
        gui_display(DISP_FQ_CHAN);
        break;
    case GUI_BOX_EQ:
        gui_display(DISP_EQ);
        break;
    case GUI_BOX_MODEPLAY:
        gui_display(DISP_MODEPLAY);
        break;
    case GUI_BOX_FREQ:
        gui_display(DISP_FREQ);
        break;
    }
}

//控件清除
AT(.text.gui)
void gui_box_clear(void)
{
    memset(&box_cb, 0, sizeof(box_cb));
}

//显示音量控件
AT(.text.gui)
void gui_box_show_vol(void)
{
    bsp_sys_unmute();
    box_cb.sta = GUI_BOX_VOLBOX;
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
    gui_display(DISP_VOL);   //立刻显示
}

//显示EQ控件
AT(.text.gui)
void gui_box_show_eq(void)
{
    box_cb.sta = GUI_BOX_EQ;
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
}

//显示PLAYMODE控件
AT(.text.gui)
void gui_box_show_playmode(void)
{
    box_cb.sta = GUI_BOX_MODEPLAY;
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
    gui_display(DISP_MODEPLAY);
}

//显示数字控件
AT(.text.gui)
void gui_box_show_num(u16 num)
{
    box_cb.number = num;
    box_cb.sta = GUI_BOX_NUMBOX;
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
    gui_display(DISP_NUM);
}

//显示频道控件
AT(.text.gui)
void gui_box_show_chan(void)
{
    box_cb.sta = GUI_BOX_CHANBOX;
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
}

//显示输入数字控件
AT(.text.gui)
void gui_box_show_inputnum(u8 number)
{
    int max_num = IR_INPUT_NUM_MAX;

    if (number == (KEY_NUM_P100 - KEY_NUM_0)) {
        number = 100;
        max_num *= 10;
    }

    if (box_cb.sta != GUI_BOX_INPUTNUM || box_cb.number > max_num) {
        box_cb.number = number;
        box_cb.sta = GUI_BOX_INPUTNUM;
    } else {
        if (number == 100) {
            box_cb.number += number;
        } else {
            box_cb.number = (box_cb.number * 10) + number;
        }
    }
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
}

//显示频率控件
AT(.text.gui)
void gui_box_show_freq(void)
{
    box_cb.sta = GUI_BOX_FREQ;
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
}

#if MUSIC_PLAYDEV_BOX_EN
//显示播放设备控件
AT(.text.gui)
void gui_box_show_playdev(void)
{
    box_cb.sta = GUI_BOX_PLAYDEV;
    box_cb.times = BOX_TIMES;
    box_cb.update = 1;
    gui_display(DISP_PLAYDEV);
}
#endif // MUSIC_PLAYDEV_BOX_EN
