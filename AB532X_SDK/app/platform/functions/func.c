#include "include.h"

func_cb_t func_cb AT(.buf.func_cb);

#if VBAT_DETECT_EN
AT(.text.func.msg)
void lowpower_vbat_process(void)
{
    int lpwr_vbat_sta = is_lowpower_vbat_warning();

    if (lpwr_vbat_sta == 1) {
        if (func_cb.mp3_res_play) {
            func_cb.mp3_res_play(RES_BUF_LOW_BATTERY_MP3, RES_LEN_LOW_BATTERY_MP3);
        }
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;     //低电，进入关机或省电模式
        return;
    }
    if ((func_cb.mp3_res_play == NULL) || (lpwr_vbat_sta != 2)) {
        if ((sys_cb.lowbat_flag) && (sys_cb.vbat > 3800)) {
            sys_cb.vbat_nor_cnt++;
            if (sys_cb.vbat_nor_cnt > 40) {
                sys_cb.lowbat_flag = 0;
                sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;
                plugin_lowbat_vol_recover();    //离开低电, 恢复音乐音量
            }
        }
        return;
    }

    //低电提示音播�?
    sys_cb.vbat_nor_cnt = 0;
    if (sys_cb.lpwr_warning_cnt > xcfg_cb.lpwr_warning_period) {
        sys_cb.lpwr_warning_cnt = 0;
        if (sys_cb.lpwr_warning_times) {        //低电语音提示次数
            if (RLED_LOWBAT_FOLLOW_EN) {
                led_lowbat_follow_warning();
            }

            if (sys_cb.lowbat_flag) {			//忽略第一次低电，作为消抖
            	func_cb.mp3_res_play(RES_BUF_LOW_BATTERY_MP3, RES_LEN_LOW_BATTERY_MP3);
            }
            
            plugin_lowbat_vol_reduce();         //低电降低音乐音量

            if (RLED_LOWBAT_FOLLOW_EN) {
                while (get_led_sta(1)) {        //等待红灯闪完
                    delay_5ms(2);
                }
                led_lowbat_recover();
            }
			//忽略第一次低电报警，作为低电消抖
            if ((sys_cb.lpwr_warning_times != 0xff) && sys_cb.lowbat_flag) {
                sys_cb.lpwr_warning_times--;
            }

            sys_cb.lowbat_flag = 1;
            if (!sys_cb.lpwr_warning_times) {
            	sys_cb.pwrdwn_tone_en = 1;
				func_cb.sta = FUNC_PWROFF;     //低电提示音播放完，进入关机或省电模式
				printf("low power warning time out power off;\n");
            }
        }
    }
}
#endif // VBAT_DETECT_EN

AT(.text.func.process)
void func_process(void)
{
    WDT_CLR();
#if VBAT_DETECT_EN
    lowpower_vbat_process();
#endif // VBAT_DETECT_EN

#if BT_BACKSTAGE_EN
    if (func_cb.sta != FUNC_BT) {
        func_bt_warning();
        uint status = bt_get_status();
#if BT_BACKSTAGE_PLAY_DETECT_EN
        if (status >= BT_STA_PLAYING) {
#else
        if (status > BT_STA_PLAYING) {
#endif
            func_cb.sta_break = func_cb.sta;
            func_cb.sta = FUNC_BT;
        }
    }
#endif

#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_detect(1);
    }
#endif // CHARGE_EN

#if SYS_KARAOK_EN
    karaok_process();
#endif

#if LE_EN
    bsp_ble_process();
#endif
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    switch (msg) {
        case KL_NEXT_VOL_UP:
        case KH_NEXT_VOL_UP:
        case KL_VOL_UP:
        case KH_VOL_UP:
            sys_cb.maxvol_fade = 1;
        case KU_VOL_UP_NEXT:
        case KU_VOL_UP:
        case KU_VOL_NEXT_PREV:
            if (msg == KU_VOL_NEXT_PREV) {
                //循环音量�? 最大音量后提示�? 然后�?开始�?
                if (sys_cb.vol == VOL_MAX) {
                    bsp_set_volume(0);
                } else {
                    bsp_set_volume(bsp_volume_inc(sys_cb.vol));
                }
            } else {
                bsp_set_volume(bsp_volume_inc(sys_cb.vol));
            }
            if (func_cb.set_vol_callback) {
                func_cb.set_vol_callback(1);
            }
            bsp_bt_vol_change();
            printf("current volume: %d\n", sys_cb.vol);
#if WARNING_MAX_VOLUME
            if (sys_cb.vol == VOL_MAX) {
                maxvol_tone_play();
            }
#endif // WARNING_MAX_VOLUME
            break;

        case KLU_VOL_UP:
        case KLU_NEXT_VOL_UP:
            if (sys_cb.maxvol_fade == 2) {
                dac_fade_in();
            }
            sys_cb.maxvol_fade = 0;
            break;

        case KL_PREV_VOL_DOWN:
        case KH_PREV_VOL_DOWN:
        case KU_VOL_DOWN_PREV:
        case KU_VOL_DOWN:
        case KL_VOL_DOWN:
        case KH_VOL_DOWN:
            bsp_set_volume(bsp_volume_dec(sys_cb.vol));
            if (func_cb.set_vol_callback) {
                func_cb.set_vol_callback(0);
            }
            bsp_bt_vol_change();
            printf("current volume: %d\n", sys_cb.vol);
#if WARNING_MIN_VOLUME
            if (sys_cb.vol == 0) {
                if (func_cb.mp3_res_play) {
                    func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
                }
            }
#endif // WARNING_MIN_VOLUME
            break;


        //长按PP/POWER软关�?通过PWROFF_PRESS_TIME控制长按时间)
        case KLH_PLAY_POWER:
        case KLH_MODE_POWER:
        case KLH_HSF_POWER:
        case KLH_POWER:
        	if (SOFT_POWER_ON_OFF) {
		        sys_cb.pwrdwn_tone_en = 1;
		        func_cb.sta = FUNC_PWROFF;
            }
            break;

#if IRRX_HW_EN
        case KU_IR_POWER:
            func_cb.sta = FUNC_SLEEPMODE;
            break;
#endif

        case KU_MODE:
        case KU_MODE_POWER:
        case KL_PLAY_MODE:
        	if (is_next_func_switch()) {
            	func_cb.sta = FUNC_NULL;
            }
            else if (xcfg_cb.bt_key_discon_en && (xcfg_cb.bt_key_discon_mode == 0)) {
            	if (bt_nor_is_connected()) {
	            	printf ("bt_nor_disconnect();\n");
	            	bt_nor_disconnect();
            	}
            }
            break;

#if EQ_MODE_EN
        case KU_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        case KU_MUTE:
            if (sys_cb.mute) {
                bsp_sys_unmute();
            } else {
                bsp_sys_mute();
            }
            break;

#if SYS_KARAOK_EN
        case KU_VOICE_RM:
            karaok_voice_rm_switch();
            break;
#if SYS_MAGIC_VOICE_EN
        case KL_VOICE_RM:
            magic_voice_switch();
            break;
#endif
#endif

        case MSG_SYS_500MS:
#if SD_SOFT_DETECT_EN
            if (SD_IS_SOFT_DETECT()) {
                if (sd_soft_detect()) {
                    if (dev_online_filter(DEV_SDCARD)) {
                        sd_insert();
                        msg_enqueue(EVT_SD_INSERT);
//                        printf("sd insert\n");
                    }
                }
            }
#endif // SD_SOFT_DETECT_EN
            break;

#if MUSIC_UDISK_EN
        case EVT_UDISK_INSERT:
            if (dev_is_online(DEV_UDISK)) {
#if BT_BACKSTAGE_EN && FMRX_INSIDE_EN
                if(func_cb.sta == FUNC_FMRX) {
                    if(fmrx_cb.sta>FMRX_IDLE) {
                    	func_fmrx_stop();
                   	    fmrx_power_off();
                    }
#if SD_USB_MUX_IO_EN
               		 sys_cb.cur_dev = DEV_UDISK;
#endif // SD_USB_MUX_IO_EN
               		 if (dev_udisk_activation_try(0)) {
                  		 sys_cb.cur_dev = DEV_UDISK;
                    	 func_cb.sta = FUNC_MUSIC;
                	}
                	if((fmrx_cb.sta>FMRX_IDLE)&&(func_cb.sta != FUNC_MUSIC)) {
                  	 	 fmrx_power_on(BT_BACKSTAGE_EN << 31);
                  	 	 if ((fmrx_cb.sta <FMRX_PAUSE)&&(fmrx_cb.sta >FMRX_IDLE)) {
                      		  func_fmrx_start();
                       	 }
              		}
			}else
#endif // BT_BACKSTAGE_EN&&FMRX_INSIDE_EN
            {
#if SD_USB_MUX_IO_EN
                sys_cb.cur_dev = DEV_UDISK;
#endif // SD_USB_MUX_IO_EN
                if (dev_udisk_activation_try(0)) {
                    sys_cb.cur_dev = DEV_UDISK;
                    func_cb.sta = FUNC_MUSIC;
                }
            }
            }
            break;
#endif // MUSIC_UDISK_EN

#if MUSIC_SDCARD_EN
        case EVT_SD_INSERT:
            if (dev_is_online(DEV_SDCARD)) {
                sys_cb.cur_dev = DEV_SDCARD;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
        case EVT_SD1_INSERT:
            if (dev_is_online(DEV_SDCARD1)) {
                sys_cb.cur_dev = DEV_SDCARD1;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD1_EN

#if FUNC_USBDEV_EN
        case EVT_PC_INSERT:
            if (dev_is_online(DEV_USBPC)) {
                func_cb.sta = FUNC_USBDEV;
            }
            break;
#endif // FUNC_USBDEV_EN

#if LINEIN_DETECT_EN
        case EVT_LINEIN_INSERT:
            if (dev_is_online(DEV_LINEIN)) {
#if LINEIN_2_PWRDOWN_EN
                sys_cb.pwrdwn_tone_en = LINEIN_2_PWRDOWN_TONE_EN;
                func_cb.sta = FUNC_PWROFF;
#else
                func_cb.sta = FUNC_AUX;
#endif // LINEIN_2_PWRDOWN_EN
            }
            break;
#endif // LINEIN_DETECT_EN

        case EVT_TWS_SET_VOL:
        case EVT_A2DP_SET_VOL:
            if(bt_get_status() != BT_STA_INCALL) {
                printf("change volume: %d\n", sys_cb.vol);
                bsp_change_volume(sys_cb.vol);
                gui_box_show_vol();
            }
            break;
        case EVT_BT_SCAN_START:
            if (bt_get_status() < BT_STA_SCANNING) {
                bt_scan_enable();
            }
            break;
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
        case EVT_ONLINE_SET_EQ:
            eq_parse_cmd();
            break;
#endif
#if LANG_SELECT == LANG_EN_ZH
        case EVT_BT_SET_LANG_ID:
            param_lang_id_write();
            param_sync();
            break;
#endif

#if EQ_MODE_EN
        case EVT_BT_SET_EQ:
            music_set_eq_by_num(sys_cb.eq_mode);
            break;
#endif
#if SYS_KARAOK_EN
        case EVT_ECHO_LEVEL:
//            printf("echo level:%x\n", sys_cb.echo_level);
            bsp_echo_set_level();
            break;

        case EVT_MIC_VOL:
//            printf("mic vol:%x\n", sys_cb.mic_vol);
            bsp_karaok_set_mic_volume();
            break;

        case EVT_MUSIC_VOL:
//            printf("music vol:%x\n", sys_cb.music_vol);
            bsp_karaok_set_music_volume();
            break;
#endif
#if MIC_DETECT_EN
        case EVT_MIC_INSERT:
            karaok_mic_unmute();
            break;

        case EVT_MIC_REMOVE:
            karaok_mic_mute();
            break;
#endif

		case EVT_SYS_FACTORY:
			param_reset_factory();
			if (func_cb.mp3_res_play) {
				func_cb.mp3_res_play(RES_BUF_FACTORY_MP3, RES_LEN_FACTORY_MP3);
			}
			func_cb.sta = FUNC_BT;			//回到蓝牙状态
			break;
			
    }

    //调节音量�?秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6)) {
        sys_cb.cm_vol_change = 0;
        cm_sync();
    }
}

///进入一个功能的总入�?
AT(.text.func)
void func_enter(void)
{
    gui_box_clear();
    param_sync();
    reset_sleep_delay();
    reset_pwroff_delay();
    func_cb.mp3_res_play = NULL;
    func_cb.set_vol_callback = NULL;
    bsp_clr_mute_sta();
    sys_cb.voice_evt_brk_en = 1;    //播放提示音时，快速响应事件�?
    AMPLIFIER_SEL_D();
#if SYS_KARAOK_EN
    karaok_voice_rm_disable();
    bsp_karaok_echo_reset_buf(func_cb.sta);
#endif
}

AT(.text.func)
void func_exit(void)
{
    u8 func_num;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
        if (func_cb.last == func_sort_table[func_num]) {
            break;
        }
    }
    func_num++;                                     //切换到下一个任�?
    if (func_num >= funcs_total) {
        func_num = 0;
    }
    func_cb.sta = func_sort_table[func_num];        //新的任务
#if SYS_MODE_BREAKPOINT_EN
    param_sys_mode_write(func_cb.sta);
#endif // SYS_MODE_BREAKPOINT_EN
}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);
    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_MUSIC_EN
        case FUNC_MUSIC:
            func_music();
            break;
#endif // FUNC_MUSIC_EN

#if EX_SPIFLASH_SUPPORT
        case FUNC_EXSPIFLASH_MUSIC:
            func_exspifalsh_music();
            break;
#endif

#if FUNC_FMRX_EN
        case FUNC_FMRX:
            func_fmrx();
            break;
#endif // FUNC_FMRX_EN

#if FUNC_CLOCK_EN
        case FUNC_CLOCK:
            func_clock();
            break;
#endif // FUNC_CLOCK_EN

#if FUNC_BT_EN
        case FUNC_BT:
            func_bt();
            break;
#endif

#if FUNC_BTHID_EN
        case FUNC_BTHID:
            func_bthid();
            break;
#endif // FUNC_BTHID_EN

#if FUNC_AUX_EN
        case FUNC_AUX:
            func_aux();
            break;
#endif // FUNC_AUX_EN

#if FUNC_USBDEV_EN
        case FUNC_USBDEV:
            func_usbdev();
            break;
#endif

#if FUNC_SPDIF_EN
        case FUNC_SPDIF:
            func_spdif();
            break;
#endif

#if FUNC_FMAM_FREQ_EN
        case FUNC_FMAM_FREQ:
            func_fmam_freq();
            break;
#endif // FUNC_FMAM_FREQ_EN

#if FUNC_SPEAKER_EN
        case FUNC_SPEAKER:
            func_speaker();
            break;
#endif // FUNC_SPEAKER_EN

#if FUNC_IDLE_EN
        case FUNC_IDLE:
            func_idle();
            break;
#endif // FUNC_IDLE_EN

        case FUNC_PWROFF:
            func_pwroff(sys_cb.pwrdwn_tone_en);
            break;

        case FUNC_SLEEPMODE:
            func_sleepmode();
            break;

        default:
            func_exit();
            break;
        }
    }
}
