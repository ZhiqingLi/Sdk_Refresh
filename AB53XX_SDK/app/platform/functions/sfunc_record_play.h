#ifndef _SFUNC_RECORD_PLAY_H
#define _SFUNC_RECORD_PLAY_H

enum {
    PREC_INIT,
    PREC_STOP,
    PREC_PLAYING,
    PREC_PAUSE,
};

typedef struct {
    u8 sta;
    u8 pause;
    u8 type;                    //音乐格式
    u8 draw_update  : 1,
       sta_update   : 1,
       time_update  : 1,
       res          : 5;

    u32 bit_rate;               //音乐码率
    msc_time_t alltime;         //music file total time
    msc_time_t curtime;         //music current playing time
    msc_time_t disptime;
} rec_play_cb_t;
extern rec_play_cb_t rec_play_cb;

void sfunc_record_play(void);
void sfunc_record_play_message(u16 msg);

#if (GUI_SELECT != GUI_NO)
void sfunc_record_play_display(void);
void sfunc_record_play_enter_display(void);
#else
#define sfunc_record_play_display()
#define sfunc_record_play_enter_display()
#endif
#endif
