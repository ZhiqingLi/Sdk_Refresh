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

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_SPDIF_EN
