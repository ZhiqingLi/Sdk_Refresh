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
            f_aux.disp_update = 1;
            break;

        case EVT_LINEIN_REMOVE:
            func_cb.sta = func_cb.last;
#if BT_APP_EN
            bsp_app_msc_send_dev_info();
#endif
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
    #if GUI_LCD_EN
            lcd_screen_clear();
            f_aux.disp_update = 1;
    #endif
#endif // AUX_REC_EN
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_AUX_EN
