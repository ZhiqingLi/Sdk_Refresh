#include "include.h"
#include "func.h"
#include "func_fmrx.h"

#if FUNC_FMRX_EN
AT(.text.func.fmrx.msg)
void func_fmrx_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
		case KU_PLAY_HSF:
            bsp_clr_mute_sta();
            if (fmrx_cb.sta < FMRX_PLAY) {
                fmrx_cb.sta = FMRX_SEEK_STOP;   //停止搜台
            } else {
                func_fmrx_pause_play();
            }
            break;

        case KL_PLAY:
        case KL_PLAY_HSF:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            bsp_clr_mute_sta();
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
            if ((!dev_is_online(DEV_SDCARD)) && (!dev_is_online(DEV_UDISK) && (!dev_is_online(DEV_SDCARD1)))) {
                break;
            }
            func_cb.mp3_res_play(RES_BUF_REC_START_MP3, RES_LEN_REC_START_MP3);
            sfunc_record();
    #if REC_AUTO_PLAY
            if (rec_cb.flag_play) {
                rec_cb.flag_play = 0;
                func_fmrx_stop();
                sfunc_record_play();
                func_fmrx_start();
            }
    #endif
#endif // FMRX_REC_EN
            break;

        case KL_MODE:
        #if FMRX_RSSI_DEBUG
            bsp_fmrx_rssi_print();
            //bsp_fmrx_logger_out(); //more info, need special lib support;
        #endif
            break;

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
