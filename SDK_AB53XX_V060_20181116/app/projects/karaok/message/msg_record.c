#include "include.h"
#include "func.h"
#include "sfunc_record.h"

#if FUNC_REC_EN
AT(.text.func.record.msg)
void sfunc_record_message(u16 msg)
{
    switch (msg) {
        case KU_MODE:
        case KU_MODE_POWER:
        case KL_PLAY_MODE:
            msg_enqueue(msg);
        case KL_REC:
            sfunc_record_stop();
            break;

#if SYS_MAGIC_VOICE_EN
        case KU_REC:
            magic_voice_switch();
            break;
#endif

        case KU_PLAY:
#if SYS_KARAOK_EN
            //只暂停音乐
            if (func_cb.sta == FUNC_BT) {
                bt_music_play_pause();
            } else if (func_cb.sta == FUNC_FMRX) {
                func_fmrx_pause_play();
            } else if (func_cb.sta == FUNC_AUX) {
                func_aux_pause_play();
            }
#else
            if (sfunc_is_recording()) {
                sfunc_record_pause();
            } else {
                sfunc_record_start();
            }
#endif // SYS_KARAOK_EN
            break;

#if SYS_KARAOK_EN
        case KL_PLAY:
        	karaok_voice_rm_switch();
            break;
#endif // SYS_KARAOK_EN

		case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
            if (func_cb.sta == FUNC_BT) {
                bt_music_prev();
            } else if (func_cb.sta == FUNC_FMRX) {
                fmrx_switch_channel(0);
            }
            break;

        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
            if (func_cb.sta == FUNC_BT) {
                bt_music_next();
            } else if (func_cb.sta == FUNC_FMRX) {
                fmrx_switch_channel(1);
            }
            break;

        case MSG_SYS_1S:
            if (!sfunc_is_recording()) {
                break;
            }
            if(!sfunc_fwrite_sync()) {
                sfunc_record_stop();
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

#if REC_AUTO_PLAY
AT(.text.func.record.msg)
void sfunc_record_play_message(u16 msg)
{
    switch (msg) {
        case KU_MODE:
        case KU_MODE_POWER:
        case KL_PLAY_MODE:
            msg_enqueue(msg);
            break;

        case KU_PLAY:
            if (rec_play_cb.pause) {
                music_control(MUSIC_MSG_PLAY);
                led_music_play();
                rec_play_cb.pause = 0;
            } else {
                music_control(MUSIC_MSG_PAUSE);
                led_idle();
                rec_play_cb.pause = 1;
            }
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif

#endif //FUNC_REC_EN
