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
            sys_cb.mute = 0;
            func_speaker_pause_play();
            break;

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
#endif // MIC_REC_EN
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_SPEAKER_EN
