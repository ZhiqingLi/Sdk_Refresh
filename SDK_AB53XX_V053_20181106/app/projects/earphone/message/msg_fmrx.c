#include "include.h"
#include "func.h"
#include "func_fmrx.h"

#if FUNC_FMRX_EN
AT(.text.func.fmrx.msg)
void func_fmrx_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
            sys_cb.mute = 0;
            if (fmrx_cb.sta < FMRX_PLAY) {
                fmrx_cb.sta = FMRX_SEEK_STOP;   //停止搜台
            } else {
                func_fmrx_pause_play();
            }
            break;

        case KL_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            sys_cb.mute = 0;
            if (fmrx_cb.sta == FMRX_PAUSE) {
                func_fmrx_pause_play();
            }
            if (fmrx_cb.sta == FMRX_PLAY) {
                fmrx_cb.sta = FMRX_SEEK_START;
            } else {
                fmrx_cb.sta = FMRX_SEEK_STOP;
            }
            break;

        case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
            fmrx_switch_channel(0);
            if (fmrx_cb.sta == FMRX_PAUSE) {
                func_fmrx_pause_play();
            }
            break;

        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
            fmrx_switch_channel(1);
            if (fmrx_cb.sta == FMRX_PAUSE) {
                func_fmrx_pause_play();
            }
            break;

#if FMRX_HALF_SEEK_EN
        case KL_PREV:
            fmrx_half_seek_start(0);
            break;

        case KL_NEXT:
            fmrx_half_seek_start(1);
            break;
#endif // FMRX_HALF_SEEK_EN

        case KU_REC:
#if FMRX_REC_EN
            sfunc_record();
#endif // FMRX_REC_EN
            break;

//        case KU_EQ:
//            bsp_fmrx_logger_out();
//            break;

#if IR_NUMKEY_EN
        case KEY_NUM_0:
        case KEY_NUM_1:
        case KEY_NUM_2:
        case KEY_NUM_3:
        case KEY_NUM_4:
        case KEY_NUM_5:
        case KEY_NUM_6:
        case KEY_NUM_7:
        case KEY_NUM_8:
        case KEY_NUM_9:
            gui_box_show_inputnum(msg - KEY_NUM_0);
            break;
#endif // IR_NUMKEY_EN

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_FMRX_EN
