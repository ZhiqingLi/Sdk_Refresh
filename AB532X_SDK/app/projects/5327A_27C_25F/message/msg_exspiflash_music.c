#include "include.h"
#include "func.h"
#include "func_music.h"
#include "func_exspiflash_music.h"

#if (EX_SPIFLASH_SUPPORT)
AT(.text.func.music.msg)
void func_exspifalsh_music_message(u16 msg)
{
#if SPIFALSH_MUSIC_PLAY_REC
    u32 rec_total = 0;
#endif
    switch (msg) {
        case KU_PREV_VOL_DOWN:
        case KL_VOL_DOWN_PREV:
        case KU_PREV:
        #if SPIFALSH_MUSIC_PLAY_REC
            exspifalsh_rec_switch_file(0);
        #else
            exspifalsh_music_switch_file(0);
        #endif
            break;

        case KU_NEXT_VOL_UP:
        case KL_VOL_UP_NEXT:
        case KU_NEXT:
        #if SPIFALSH_MUSIC_PLAY_REC
            exspifalsh_rec_switch_file(1);
        #else
            exspifalsh_music_switch_file(1);
        #endif
            break;

#if SPIFALSH_MUSIC_PLAY_REC
        case KL_REC:
            printf("delete cur file\n");
            music_control(MUSIC_MSG_STOP);
            spifalsh_rec_delete_curplay_file();
            rec_total = spifalsh_rec_get_total_file();
            printf("rec_total = %d\n",rec_total);
            if (0 == rec_total) {
                func_cb.sta = FUNC_NULL;
            }
#endif
            break;

        case KU_PLAY:
        case KU_PLAY_POWER:
        case KU_PLAY_MODE:
              if (exspi_msc.pause) {
                exspi_msc.pause = false;
                music_control(MUSIC_MSG_PLAY);
                printf("music continue playing\n");
            } else {
                exspi_msc.pause = true;
                bsp_clr_mute_sta();
                music_control(MUSIC_MSG_PAUSE);
                printf("music pause\n");
            }
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif //(EX_SPIFLASH_SUPPORT & EXSPI_MUSIC)

