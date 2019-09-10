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
    u8 min;
    u8 sec;
} rec_play_cb_t;
extern rec_play_cb_t rec_play_cb;

void sfunc_record_play(void);
void sfunc_record_play_message(u16 msg);

#if (GUI_SELECT != GUI_NO)
void sfunc_record_play_display(void);
#else
#define sfunc_record_play_display()
#endif
#endif
