#include "include.h"
#include "func.h"

#if FUNC_SPEAKER_EN
AT(.text.func.speaker.msg)
void func_speaker_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            bsp_clr_mute_sta();
            func_speaker_pause_play();
            break;

#if SPEAKER_ECHO_EN
        case K_PREV:
        case KL_PREV:
        case KH_PREV:
        case KU_PREV_VOL_DOWN:
            if (f_spk.echo_step > 0) {
                f_spk.echo_step --;
            }
            printf("prev %d\n",f_spk.echo_step);
            speaker_echo_step_set(f_spk.echo_step);
            break;

        case K_NEXT:
        case KL_NEXT:
        case KH_NEXT:
        case KU_NEXT_VOL_UP:
            f_spk.echo_step++;
            if (f_spk.echo_step > ECHO_MAX_STEP) {
                f_spk.echo_step = ECHO_MAX_STEP;
            }
            printf("next: %d\n",f_spk.echo_step);
            speaker_echo_step_set(f_spk.echo_step);
            break;
#endif // SPEAKER_ECHO_EN
#if EQ_MODE_EN
        case KU_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        case KU_REC:
#if MIC_REC_EN
            if (f_spk.pause) {
                func_speaker_pause_play();
            }
            sfunc_record();
    #if REC_AUTO_PLAY
            if (rec_cb.flag_play) {
                rec_cb.flag_play = 0;
                func_speaker_stop();
                sfunc_record_play();
                func_speaker_start();
            }
    #endif
            f_spk.draw_update = 1;
#endif // MIC_REC_EN
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_SPEAKER_EN
