#ifndef _FUNC_SPEAKER_H
#define _FUNC_SPEAKER_H

typedef struct {
    u8  pause   :   1,
        rec_en  :   1;
}func_speaker_t;
extern func_speaker_t f_spk;

void func_speaker_message(u16 msg);
void func_speaker_stop(void);
void func_speaker_start(void);
void func_speaker_pause_play(void);
void func_speaker_mp3_res_play(u32 addr, u32 len);

#if (GUI_SELECT != GUI_NO)
void func_speaker_display(void);
void func_speaker_enter_display(void);
void func_speaker_exit_display(void);
#else
#define func_speaker_display()
#define func_speaker_enter_display()
#define func_speaker_exit_display()
#endif

#endif // _FUNC_SPEAKER_H
