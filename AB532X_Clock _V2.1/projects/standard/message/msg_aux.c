#include "include.h"
#include "func.h"

#if FUNC_AUX_EN

AT(.text.func.aux.msg)
void func_aux_message(u16 msg)
{
	if(!clock_message(msg)){
		return;
	}
	
    switch (msg) {
		case KU_NEXT_VOL_UP:
			bsp_set_volume(bsp_volume_inc(sys_cb.vol));
			if (func_cb.set_vol_callback) {
				func_cb.set_vol_callback(1);
			}
		#if WARNING_MAX_VOLUME
			if (sys_cb.vol == VOL_MAX) {
				maxvol_tone_play();
			}
		#endif 
			break;
		case KU_PREV_VOL_DOWN:
			bsp_set_volume(bsp_volume_dec(sys_cb.vol));
			if (func_cb.set_vol_callback) {
				func_cb.set_vol_callback(0);
			}
			bsp_bt_vol_change();
		#if WARNING_MIN_VOLUME
			if (sys_cb.vol == 0) {
				if (func_cb.mp3_res_play) {
					func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
				}
			}
		#endif
			break;
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
        case KU_PLAY_HSF:
            bsp_clr_mute_sta();
            func_aux_pause_play();
            break;

        case EVT_LINEIN_REMOVE:
            func_cb.sta = func_cb.last;
            break;

        case KU_REC:
#if AUX_REC_EN
            if ((!dev_is_online(DEV_SDCARD)) && (!dev_is_online(DEV_UDISK) && (!dev_is_online(DEV_SDCARD1)))) {
                break;
            }
            if (f_aux.pause) {
                func_aux_pause_play();
            }
            func_cb.mp3_res_play(RES_BUF_REC_START_MP3, RES_LEN_REC_START_MP3);
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
