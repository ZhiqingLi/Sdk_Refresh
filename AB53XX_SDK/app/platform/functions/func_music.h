#ifndef _FUNC_MUSIC_H
#define _FUNC_MUSIC_H

typedef struct {
    u8 min;                     //minute
    u8 sec;                     //second
} msc_time_t;

typedef struct {
    u32 frame_count;            //current frame count
    u32 file_ptr;               //file ptr of current frame
    u16 fname_crc;              //file name crc
} msc_breakpiont_t;

typedef struct {
    u8 pause        : 1,
       file_change  : 1,
       dev_change   : 2,
       cur_dev      : 2,
       brkpt_flag   : 1,
       prev_flag    : 1;

    u8 rec_type     : 1,
       rec_scan     : 2,
       encrypt      : 1;

    u8 type;                    //音乐格式
    u32 bit_rate;               //音乐码率

    char *fname;

    msc_time_t alltime;         //music file total time
    msc_time_t curtime;         //music current playing time

    u16 file_num;
    u16 file_total;

    u16 dir_num;                //directory current number
    u16 dir_total;              //directory total number

#if MUSIC_BREAKPOINT_EN
    msc_breakpiont_t brkpt;     //music breakpoint info
#endif // MUSIC_BREAKPOINT_EN

#if MUSIC_LRC_EN
    u8 lrc_sta;
    u8 lrc_encoding;
#endif // MUSIC_LRC_EN

    ///LCD点阵屏控制显示变量
#if GUI_LCD_EN
    msc_time_t disptime;
#endif // GUI_LCD_EN
    u8 draw_update  : 1,
       sta_update   : 1,
       time_update  : 1,
       fname_update : 1,
       lrc_update   : 1;
} func_msc_t;
extern func_msc_t f_msc;

void func_music_message(u16 msg);
void func_music_switch_file(u8 direction);
void func_music_switch_dir(u8 direction);
void func_music_select_dir(u16 sel_num);
bool func_music_auto_next_device(void);
void func_music_mp3_res_play(u32 addr, u32 len);
bool func_music_filter_switch(u8 rec_type);
void func_music_filter_set(void);
void func_music_insert_device(u8 dev);
void func_music_remove_device(u8 dev);

#if (GUI_SELECT != GUI_NO)
void func_music_display(void);
void func_music_enter_display(void);
void func_music_exit_display(void);
void func_music_disp_stauts(void);
void func_music_disp_fname(void);
void func_music_disp_freq_spectrum(void);
void func_music_disp_lrc(void);
#else
#define func_music_display()            gui_box_process()
#define func_music_enter_display()
#define func_music_exit_display()
#endif

#endif // _FUNC_MUSIC_H
