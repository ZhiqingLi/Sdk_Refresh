#ifndef _FUNC_FMRX_H
#define _FUNC_FMRX_H

enum {
    FMRX_IDLE = 0,
    FMRX_SEEK_START,
    FMRX_SEEKING,
    FMRX_SEEK_STOP,
    FMRX_SEEKING_HALF,              //半自动搜台
    FMRX_PLAY,
    FMRX_PAUSE,
};

typedef struct {
    u8   st_flag        : 1,
         rec_en         : 1;
    u8   sta;
    u8   seek_start;
    s8   hstep;
    u16  freq;
    u8   ch_cur;
    u8   ch_cnt;
    u16  fcnt;
    u16  buf[13];

#if GUI_LCD_EN
    u8  freq_update     : 1,
        ch_update       : 1;
    u8  diap_ch;
    u16 disp_freq;
#endif // GUI_LCD_EN

} fmrx_cb_t;
extern fmrx_cb_t fmrx_cb;

u16 get_ch_freq(u8 ch);
void fmrx_half_seek_start(u8 dir);
void fmrx_switch_channel(u8 dir);
void fmrx_switch_freq(u8 dir);
void func_fmrx_message(u16 msg);
void func_fmrx_mp3_res_play(u32 addr, u32 len);
void func_fmrx_pause_play(void);
void func_fmrx_stop(void);
void func_fmrx_start(void);
void fmrx_set_curch_freq(void);

#if (GUI_SELECT != GUI_NO)
void func_fmrx_display(void);
void func_fmrx_enter_display(void);
void func_fmrx_exit_display(void);
void func_fmrx_freq_display(void);
void func_fmrx_ch_display(void);
void func_fmrx_display_init(void);
#else
#define func_fmrx_display()             gui_box_process()
#define func_fmrx_enter_display()
#define func_fmrx_exit_display()
#endif

#endif // _FUNC_FMRX_H
