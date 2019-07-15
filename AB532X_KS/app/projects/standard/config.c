#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {
#if FUNC_BT_EN
	FUNC_BT,
#endif

#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif // FUNC_MUSIC_EN

#if FUNC_AUX_EN
	FUNC_AUX,
#endif // FUNC_AUX_EN

#if FUNC_FMRX_EN
    FUNC_FMRX,
#endif

#if FUNC_USBDEV_EN
	FUNC_USBDEV,
#endif // FUNC_USBDEV_EN

#if FUNC_CLOCK_EN
    FUNC_CLOCK,
#endif

#if FUNC_BTHID_EN
	FUNC_BTHID,
#endif // FUNC_BTHID

#if FUNC_SPEAKER_EN
	FUNC_SPEAKER,
#endif // FUNC_SPEAKER_EN

#if FUNC_SPDIF_EN
    FUNC_SPDIF,
#endif

#if FUNC_FMAM_FREQ_EN
    FUNC_FMAM_FREQ,
#endif // FUNC_FMAM_FREQ_EN

#if FUNC_IDLE_EN
    FUNC_IDLE,
#endif
};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

AT(.text.func)
bool is_next_func_switch(void)
{
    u8 func_num;
    bool is_func_exist = 0;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
    	switch(func_sort_table[func_num]) {
#if (FUNC_BT_EN || FUNC_CLOCK_EN || FUNC_SPEAKER_EN || FUNC_FMAM_FREQ_EN)
#if FUNC_BT_EN
			case FUNC_BT:
#endif
#if FUNC_CLOCK_EN
			case FUNC_CLOCK:
#endif
#if FUNC_SPEAKER_EN
			case FUNC_SPEAKER:
#endif
#if FUNC_FMAM_FREQ_EN
			case FUNC_FMAM_FREQ:
#endif
				is_func_exist = 1;
				break;
#endif

#if FUNC_MUSIC_EN
			case FUNC_MUSIC:
				if (dev_is_online(DEV_UDISK) || dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
					is_func_exist = 1;
				}
				break;
#endif // FUNC_MUSIC_EN

#if FUNC_AUX_EN
			case FUNC_AUX:
				if (is_linein_enter_enable()) {
					is_func_exist = 1;
				}
				break;
#endif // FUNC_AUX_EN

#if FUNC_FMRX_EN
			case FUNC_FMRX:
				if (is_func_fmrx_en()) {
					is_func_exist = 1;
				}
				break;
#endif

#if FUNC_USBDEV_EN
			case FUNC_USBDEV:
				if (dev_is_online(DEV_USBPC)) {
					is_func_exist = 1;
				}
				break;
#endif // FUNC_USBDEV_EN

#if FUNC_SPDIF_EN
			case FUNC_SPDIF:
				if (spdif_is_online()) {
					is_func_exist = 1;
				}
				break;
#endif

#if FUNC_BTHID_EN
			case FUNC_BTHID:
				if (xcfg_cb.func_bthid_en) {
					is_func_exist = 1;
				}
				break;
#endif
			default:
				is_func_exist = 0;
				break;
    	}

    	if (is_func_exist) {
	        if (func_cb.sta != func_sort_table[func_num]) {
	            return 1;
	        }
	        is_func_exist = 0;
        }
    }

	return 0;
}


