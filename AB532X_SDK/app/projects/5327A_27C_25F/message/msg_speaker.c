#include "include.h"
#include "func.h"
#include "bsp_spiflash1.h"
bool spifalsh_rec_play_last_file(void);

#if FUNC_SPEAKER_EN
AT(.text.func.speaker.msg)
void func_speaker_message(u16 msg)
{
    switch (msg) {
        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
        case KU_PLAY_HSF:
            bsp_clr_mute_sta();
            func_speaker_pause_play();
            break;

#if EQ_MODE_EN
        case KU_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        case KU_REC:
            printf("KU_REC\n");
#if MIC_REC_EN
            if ((!dev_is_online(DEV_SDCARD)) && (!dev_is_online(DEV_UDISK) && (!dev_is_online(DEV_SDCARD1)))) {
                #if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
                if (!is_exspiflash_online()) {
                    break;
                }
                #else
                break;
                #endif
            }
            if (f_spk.pause) {
                func_speaker_pause_play();
            }
            func_cb.mp3_res_play(RES_BUF_REC_START_MP3, RES_LEN_REC_START_MP3);
            sfunc_record();
    #if REC_AUTO_PLAY
            if (rec_cb.flag_play) {
            #if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
                spifalsh_rec_play_last_file();
            #else
                rec_cb.flag_play = 0;
                func_speaker_stop();
                sfunc_record_play();
                func_speaker_start();
            #endif
            }
    #endif
#endif // MIC_REC_EN
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_SPEAKER_EN
