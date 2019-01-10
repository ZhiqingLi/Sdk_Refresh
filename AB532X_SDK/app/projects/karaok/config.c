#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {
#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif // FUNC_MUSIC_EN

#if FUNC_FMRX_EN
    FUNC_FMRX,
#endif

#if FUNC_BT_EN
    FUNC_BT,
#endif

#if FUNC_BTHID_EN
    FUNC_BTHID,
#endif // FUNC_BTHID

#if FUNC_AUX_EN
    FUNC_AUX,
#endif // FUNC_AUX_EN

#if FUNC_CLOCK_EN
    FUNC_CLOCK,
#endif

#if FUNC_USBDEV_EN
    FUNC_USBDEV,
#endif // FUNC_USBDEV_EN

#if FUNC_FMAM_FREQ_EN
    FUNC_FMAM_FREQ,
#endif // FUNC_FMAM_FREQ_EN

#if FUNC_SPEAKER_EN
    FUNC_SPEAKER,
#endif // FUNC_SPEAKER_EN

#if FUNC_IDLE_EN
    FUNC_IDLE,
#endif
};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

