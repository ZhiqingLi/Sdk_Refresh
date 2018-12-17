#include "include.h"
#include "func.h"

#if FUNC_AUX_EN

AT(.text.func.aux.msg)
void func_aux_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            bsp_clr_mute_sta();
            func_aux_pause_play();
            break;

        case EVT_LINEIN_REMOVE:
            if(func_cb.last == FUNC_AUX){
                func_cb.sta = FUNC_NULL;
            }else{
                func_cb.sta = func_cb.last;
            }
            break;

        case KU_REC:
#if AUX_REC_EN
            if (f_aux.pause) {
                func_aux_pause_play();
            }
            sfunc_record();
    #if REC_AUTO_PLAY
            if (rec_cb.flag_play) {
                rec_cb.flag_play = 0;
                func_aux_stop();
                sfunc_record_play();
                func_aux_start();
            }
    #endif

#endif // AUX_REC_EN
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_AUX_EN
