#include "include.h"

#define FS_CRC_SEED         0xffff

uint calc_crc(void *buf, uint len, uint seed);
void mp3_res_play_kick(u32 addr, u32 len);
void wav_res_play_kick(u32 addr, u32 len);
void wav_res_dec_process(void);
bool wav_res_is_play(void);
bool wav_res_stop(void);
void mp3_res_play_exit(void);

#if FUNC_MUSIC_EN

#if BT_BACKSTAGE_EN
const u32 *res_addr[] = {
#if WARNING_FUNC_MUSIC
    &RES_BUF_MUSIC_MODE_MP3,
#endif
#if WARNING_USB_SD
    &RES_BUF_SDCARD_MODE_MP3,
    &RES_BUF_USB_MODE_MP3,
#endif
#if WARNING_FUNC_BT
    &RES_BUF_BT_MODE_MP3,
#endif
#if WARNING_FUNC_FMRX
    &RES_BUF_FM_MODE_MP3,
#endif
#if WARNING_FUNC_AUX
    &RES_BUF_AUX_MODE_MP3,
#endif
};
#endif

//扫描全盘文件
bool pf_scan_music(u8 new_dev)
{
    if (new_dev) {
#if USB_SD_UPDATE_EN
        func_update();                                  //尝试升级
#endif // USB_SD_UPDATE_EN
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = BIT(0);
    sys_cb.rec_num = 0;
    sys_cb.ftime = 0;
#endif // REC_FAST_PLAY

    f_msc.file_total = fs_get_total_files();
    if (!f_msc.file_total) {
        f_msc.dir_total = 0;
        return false;
    }

#if REC_FAST_PLAY
    f_msc.rec_scan = 0;
#endif // REC_FAST_PLAY

    if (new_dev) {
#if MUSIC_PLAYDEV_BOX_EN
        gui_box_show_playdev();
#endif // MUSIC_PLAYDEV_BOX_EN

#if WARNING_USB_SD
        if (sys_cb.cur_dev <= DEV_SDCARD1) {
            mp3_res_play(RES_BUF_SDCARD_MODE_MP3, RES_LEN_SDCARD_MODE_MP3);
        } else {
            mp3_res_play(RES_BUF_USB_MODE_MP3, RES_LEN_USB_MODE_MP3);
        }
#endif // WARNING_USB_SD
    }

    f_msc.dir_total = fs_get_dirs_count();          //获取文件夹总数
    return true;
}

#if MUSIC_PLAYMODE_NUM
//music播放模式切换
void music_playmode_next(void)
{
    sys_cb.play_mode++;
    if (sys_cb.play_mode >= MUSIC_PLAYMODE_NUM) {
        sys_cb.play_mode = NORMAL_MODE;
    }
    gui_box_show_playmode();
    plugin_playmode_warning();
}
#endif // MUSIC_PLAYMODE_NUM

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_clr(void)
{
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    param_msc_breakpoint_write();
    param_sync();
    //printf("%s\n", __func__);
}

void bsp_music_breakpoint_init(void)
{
    int clr_flag = 0;
    if (f_msc.brkpt_flag) {
        f_msc.brkpt_flag = 0;
        param_msc_breakpoint_read();
        //printf("restore: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
        if (calc_crc(f_msc.fname, 8, FS_CRC_SEED) == f_msc.brkpt.fname_crc) {
            music_set_jump(&f_msc.brkpt);
        } else {
            clr_flag = 1;
        }
    }
    f_msc.brkpt.fname_crc = calc_crc(f_msc.fname, 8, FS_CRC_SEED);
    f_msc.brkpt.file_ptr = 0;
    f_msc.brkpt.frame_count = 0;
    if (clr_flag) {
        param_msc_breakpoint_write();
    }
}

void bsp_music_breakpoint_save(void)
{
    music_get_breakpiont(&f_msc.brkpt);
    param_msc_breakpoint_write();
    param_sync();
    //printf("save: %d, %d, %04x\n", f_msc.brkpt.file_ptr, f_msc.brkpt.frame_count, f_msc.brkpt.fname_crc);
}
#endif // MUSIC_BREAKPOINT_EN
#endif // FUNC_MUSIC_EN

void mp3_res_play(u32 addr, u32 len)
{
    u16 msg;
    u8 mute_bak;
    //printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
#if BT_BACKSTAGE_EN
    if (func_cb.sta_break != FUNC_NULL) {
        for (u32 i = 0; i < sizeof(res_addr) / sizeof(u32); i++) {
            if (*res_addr[i] == addr) {
                printf("addr equal, sta = %d_%d\n",func_cb.sta,func_cb.sta_break);

                if (func_cb.sta != FUNC_BT) {
                    func_cb.sta_break = FUNC_NULL;
                }
                return;
            }
        }
    }
#endif

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
#if SYS_KARAOK_EN
    u8 voice_bak = 0, func_sta = func_cb.sta;
    if (karaok_get_voice_rm_sta()) {
        voice_bak = 1;
        karaok_voice_rm_disable();
    }
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    mute_bak = sys_cb.mute;
    if (sys_cb.mute) {
        bsp_loudspeaker_unmute();
    }
    if(get_music_dec_sta() != MUSIC_STOP) { //避免来电响铃/报号未完成，影响get_music_dec_sta()状�?
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);
    mp3_res_play_kick(addr, len);
    while (get_music_dec_sta() != MUSIC_STOP) {
        WDT_CLR();
        msg = msg_dequeue();
        if (sys_cb.voice_evt_brk_en) {
            if (((msg == EVT_SD_INSERT) || (msg == EVT_UDISK_INSERT)) && (func_cb.sta != FUNC_MUSIC)) {
                func_message(msg);
                break;
            }
        }
#if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
        if ((msg == KU_REC) || (msg == KL_REC)) {
            break;
        }
#endif

#if LINEIN_DETECT_EN
        if ((msg == EVT_LINEIN_INSERT) && ((sys_cb.voice_evt_brk_en) || (LINEIN_2_PWRDOWN_EN))) {
            func_message(msg);
            break;
        }
#endif // LINEIN_DETECT_EN
#if		1
		//已经最大音量时，提示音中忽略音量按键
		if (((msg == KL_NEXT_VOL_UP) || (msg == KH_NEXT_VOL_UP) || (msg == KL_VOL_UP)
        || (msg == KH_VOL_UP) || (msg == KU_VOL_UP_NEXT) || (msg == KU_VOL_UP)) 
        && (sys_cb.vol >= VOL_MAX)) {
        	printf ("res player ignore msg VOL_UP;\n");
        	continue;
		}
		if (((msg == KL_PREV_VOL_DOWN) || (msg == KH_PREV_VOL_DOWN) || (msg == KU_VOL_DOWN_PREV)
        || (msg == KH_VOL_DOWN) || (msg == KL_VOL_DOWN) || (msg == KU_VOL_DOWN)) 
        && (sys_cb.vol == 0)) {
        	printf ("res player ignore msg VOL_DOWN;\n");
        	continue;
		}
#endif
		
        if ((msg != NO_MSG) && ((msg & KEY_TYPE_MASK) != KEY_HOLD)) {
            msg_enqueue(msg);       //还原未处理的消息
        }
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    mp3_res_play_exit();
    sys_cb.mute = mute_bak;
    if (sys_cb.mute) {
        bsp_loudspeaker_mute();
    }
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
#if SYS_KARAOK_EN
    if (voice_bak) {
        karaok_voice_rm_enable();
    }
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_sta);
#endif
}

//void wav_res_play(u32 addr, u32 len)
//{
//    if (len == 0) {
//        return;
//    }
//
//#if DAC_DNR_EN
//    u8 sta = dac_dnr_get_sta();
//    dac_dnr_set_sta(0);
//#endif
//
//    wav_res_play_kick(addr, len);
//    while (wav_res_is_play()) {
//        wav_res_dec_process();
//        WDT_CLR();
//    }
//
//#if DAC_DNR_EN
//    dac_dnr_set_sta(sta);
//#endif
//}
