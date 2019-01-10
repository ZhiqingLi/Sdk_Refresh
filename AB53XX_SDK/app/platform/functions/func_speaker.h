#ifndef _FUNC_SPEAKER_H
#define _FUNC_SPEAKER_H

typedef struct {
#if SPEAKER_ECHO_EN
    u8 echo_step;
#endif
    u8  pause       :   1,
        rec_en      :   1,
        draw_update :   1;
}func_speaker_t;
extern func_speaker_t f_spk;

void func_speaker_message(u16 msg);
void func_speaker_stop(void);
void func_speaker_start(void);
void func_speaker_pause_play(void);
void func_speaker_mp3_res_play(u32 addr, u32 len);

#if SPEAKER_ECHO_EN
#define ECHO_MAX_STEP 16
void speaker_echo_init(u8 *buf, u16 buf_len, u8 echo_step,u16 echo_strength); //echo_step 0~16 //echo_strength  0~16384.
void speaker_echo_step_set(u8 step);    //0~16
void speaker_echo_strength_set(u16 strength);   //混响强度设置, 0~16384/2.
void speaker_echo_process(u8 *buf, u16 len);    //混响缓存处理
#endif // SPEAKER_ECHO_EN

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
