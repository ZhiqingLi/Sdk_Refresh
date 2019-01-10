#ifndef _DISPLAY_H
#define _DISPLAY_H

#define BOX_TIMES               20      //20*100=2s

enum {
    GUI_BOX_NULL,                   //主界面

    GUI_BOX_NUMBOX,                 //数显框
    GUI_BOX_VOLBOX,                 //音量框
    GUI_BOX_CHANBOX,                //电台框
    GUI_BOX_EQ,                     //EQ框
    GUI_BOX_MODEPLAY,               //PLAYMODE显示框
    GUI_BOX_INPUTNUM,               //输入数字框
    GUI_BOX_PLAYDEV,                //播放设备
};

typedef struct {
    u8 sta      : 1,                //显示闪烁状态， 0：display off,  1:display on
       pos      : 7;                //显示闪烁的位置
    u8 times;                       //闪烁次数
    u8 counter;                     //显示闪烁周期时间控制 counter*100ms
    u8 preval;                      //闪烁预设周期
} gui_flicker_t;

typedef struct {
    u8  times;                       //控件显示的时间控制
    u8  sta;                         //状态
    u8  disp_sta;                    //当前显示的状态
    u16 number;                      //numbox使用，需要显示的数字
    u8  update      : 1,             //刷新标志
        event       : 1,             //BOX event标志
        res         : 6;             //保留6bits
    gui_flicker_t flicker;           //显示闪烁控制
} gui_box_t;
extern gui_box_t box_cb;

#if (GUI_SELECT & DISPLAY_LEDSEG)
#include "ledseg/ledseg_common.h"
#include "ledseg/display_ledseg.h"

#define gui_scan()                  ledseg_scan()
#define gui_display(n)              ledseg_display(n)
#define gui_off()                   ledseg_off()
#else
#define gui_scan()
#define gui_display(n)
#define gui_off()
#endif

void gui_init(void);
void gui_box_isr(void);
bool gui_box_process(void);
void gui_box_display(void);
void gui_box_clear(void);
void gui_box_show_vol(void);
void gui_box_show_eq(void);
void gui_box_show_playmode(void);
void gui_box_show_num(u16 num);
void gui_box_show_chan(void);
void gui_box_show_inputnum(u8 number);
void gui_box_flicker_set(u8 cnt, u8 times, u8 pos);
void gui_box_flicker_clr(void);
void gui_box_show_playdev(void);

#endif //_DISPLAY_H
