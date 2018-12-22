#include "include.h"
#include "func.h"
#include "func_spdif.h"

#if FUNC_SPDIF_EN
AT(.text.func.spdif.msg)
void func_spdif_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
            bsp_clr_mute_sta();
            spdif_play_pause();
            break;

        case KU_REC:
#if SPF_REC_EN
            sfunc_record();
    #if REC_AUTO_PLAY
            if (rec_cb.flag_play) {
                rec_cb.flag_play = 0;
                spdif_exit();
                sfunc_record_play();
                spdif_init(&spd_cfg);
            }
    #endif
#endif
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_SPDIF_EN
