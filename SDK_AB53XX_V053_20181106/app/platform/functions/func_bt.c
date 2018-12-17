#include "include.h"
#include "func.h"
#include "func_bt.h"

void sbc_decode_exit(void);
void bt_cvsd_irq_init(void);

func_bt_t f_bt;

ALIGNED(64)
bool func_bt_chkclr_warning(u8 bits)
{
    GLOBAL_INT_DISABLE();
    if(f_bt.warning_status & bits) {
        f_bt.warning_status &= ~bits;
        GLOBAL_INT_RESTORE();
        return true;
    }
    GLOBAL_INT_RESTORE();
    return false;
}

#if FUNC_BT_EN
#if BT_HFP_REC_EN
void bt_sco_rec_start(void);
void bt_sco_rec_stop(void);

AT(.com_text.bt_rec)
void bt_sco_rec_mix(u8 *buf, u32 samples)
{
    bt_sco_rec_mix_do(buf, samples);
}

AT(.text.func.bt)
void bt_sco_rec_init(void)
{
    rec_src.spr = SPR_8000;
    rec_src.nchannel = 0x11;
    rec_src.source_start = bt_sco_rec_start;
    rec_src.source_stop  = bt_sco_rec_stop;
    f_bt.rec_pause = 0;
    rec_cb.sco_flag = 1;
}
#endif

#if BT_REC_EN
void bt_music_rec_start(void);
void bt_music_rec_stop(void);

AT(.text.func.bt)
void bt_music_rec_init(void)
{
    rec_src.spr = SPR_44100;
    rec_src.nchannel = 0x22;
    rec_src.source_start = bt_music_rec_start;
    rec_src.source_stop  = bt_music_rec_stop;
    f_bt.rec_pause = 0;
    rec_cb.sco_flag = 0;
}

AT(.text.func.bt)
bool bt_rec_status_process(void)
{
    if (func_cb.sta == FUNC_BT) {
        func_bt_status();
        if ((f_bt.disp_status > BT_STA_PLAYING) || (f_bt.disp_status < BT_STA_CONNECTED)) {
            if ((f_bt.disp_status > BT_STA_PLAYING) && rec_cb.sco_flag) {
                return true;
            }
            f_bt.rec_pause = 1;
            return false;       //结束录音
        }
    }
    return true;
}

AT(.text.func.bt)
void bt_music_rec_continue(void)
{
#if !BT_HFP_REC_EN
    if ((f_bt.rec_pause) && ((f_bt.disp_status == BT_STA_PLAYING) || (f_bt.disp_status == BT_STA_CONNECTED))) {
        msg_enqueue(KU_REC);    //继续录音
        f_bt.rec_pause = 0;
    }
#endif
}

#endif // BT_REC_EN

void func_bt_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }

#if BT_REC_EN
    sfunc_record_pause();
#endif

    bt_audio_bypass();
    mp3_res_play(addr, len);
    bt_audio_enable();

#if SYS_KARAOK_EN
    if (f_bt.disp_status > BT_STA_CONNECTED) {
        dac_fade_in();
    }
#endif

#if BT_REC_EN
    sfunc_record_continue();
#endif
}

//切换提示音语言
void func_bt_switch_voice_lang(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    if (xcfg_cb.lang_id >= LANG_EN_ZH) {
        sys_cb.lang_id = (sys_cb.lang_id) ? 0 : 1;
        multi_lang_init(sys_cb.lang_id);
        param_lang_id_write();
        param_sync();
        func_bt_mp3_res_play(RES_BUF_LANGUAGE_MP3, RES_LEN_LANGUAGE_MP3);
    }
#endif
}

#if BT_TWS_EN
AT(.text.func.bt)
static void func_bt_status_update(void)
{
    if(f_bt.disp_status == BT_STA_CONNECTED) {
        f_bt.disp_status = 0xff;
    }
}

static void func_bt_tws_set_channel(void)
{
    if(f_bt.tws_status & 0xc0) {   //对箱状态.
        if ((2 == xcfg_cb.bt_tws_lr_mode) || (3 == xcfg_cb.bt_tws_lr_mode)) {  //硬件判断 //2 PWRKEY 820K接地  //3  特定GPIO接地
            tws_lr_xcfg_sel();
            if (sys_cb.tws_left_channel) {
                dac_mono_init(0, true); //void dac_mono_init(bool mix_en, bool lr_sel) //lr_sel true 表示左声道.
            } else {
                dac_mono_init(0, false);
            }
        }else {
            dac_mono_init(0, (f_bt.tws_status & 0x80));
        }
    } else {
        dac_mono_init(1, (f_bt.tws_status & 0x80));
    }
}
#endif

void func_bt_warning(void)
{
    if(func_bt_chkclr_warning(BT_WARN_DISCON)) {
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en) {
            func_bt_status_update();
            if(xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }
        }
#endif

#if WARNING_BT_DISCONNECT
        func_cb.mp3_res_play(RES_BUF_DISCONNECT_MP3, RES_LEN_DISCONNECT_MP3);
#endif // WARNING_BT_DISCONNECT
#if WARNING_BT_WAIT_CONNECT
        func_cb.mp3_res_play(RES_BUF_WAIT4CONN_MP3, RES_LEN_WAIT4CONN_MP3);
#endif // WARNING_BT_WAIT_CONNECT
    }

	if(func_bt_chkclr_warning(BT_WARN_CON)) {
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en) {
            func_bt_status_update();
        }
#endif
#if WARNING_BT_CONNECT
        func_cb.mp3_res_play(RES_BUF_CONNECTED_MP3, RES_LEN_CONNECTED_MP3);
#endif
    }

    if(func_bt_chkclr_warning(BT_WARN_PAIRING)) {
        func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
    }

#if BT_TWS_EN
    if(xcfg_cb.bt_tws_en) {
        if(f_bt.warning_status & (BT_WARN_TWS_MCON|BT_WARN_TWS_SCON)) {
            if(xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }
        }
        bool left_warning = false;
        bool right_warning = false;
        if ((2 == xcfg_cb.bt_tws_lr_mode) || (3 == xcfg_cb.bt_tws_lr_mode)) {  //硬件判断 //2 PWRKEY 820K接地  //3  特定GPIO接地
            if (sys_cb.tws_left_channel) {
                if(func_bt_chkclr_warning(BT_WARN_TWS_SCON | BT_WARN_TWS_MCON ) ) { //left channel
                    left_warning = true;
                }
            } else {
                if(func_bt_chkclr_warning(BT_WARN_TWS_SCON | BT_WARN_TWS_MCON ) ) { //right channel
                    right_warning = true;
                }
            }
        }

        if(func_bt_chkclr_warning(BT_WARN_TWS_SCON) || left_warning) { //left channel       //slave
            func_cb.mp3_res_play(RES_BUF_LEFT_CH_MP3, RES_LEN_LEFT_CH_MP3);
        }

        if(func_bt_chkclr_warning(BT_WARN_TWS_MCON) || right_warning) {  //right channel      //master
            delay_5ms(200);
            func_cb.mp3_res_play(RES_BUF_RIGHT_CH_MP3, RES_LEN_RIGHT_CH_MP3);
        }
    }
#endif

#if BT_HID_MENU_EN
    //按键手动断开HID Profile的提示音
    if (xcfg_cb.bt_hid_menu_en) {
    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_CON)) {
            func_cb.mp3_res_play(RES_BUF_CAMERA_ON_MP3, RES_LEN_CAMERA_ON_MP3);
        }
    #endif

    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_DISCON)) {
            func_cb.mp3_res_play(RES_BUF_CAMERA_OFF_MP3, RES_LEN_CAMERA_OFF_MP3);
        }
    #endif

    #if BT_HID_DISCON_DEFAULT_EN
        if (f_bt.hid_discon_flag) {
            if (bt_hid_is_ready_to_discon()) {
                f_bt.hid_discon_flag = 0;
                bt_hid_disconnect();
            }
        }
    #endif // BT_HID_DISCON_DEFAULT_EN
    }
#endif // BT_HID_MENU_EN
}

void func_bt_disp_status(void)
{
    uint status = bt_get_disp_status();

    if(f_bt.disp_status != status) {

        f_bt.disp_status = status;
        if (bt_pb_check()) {
            led_bt_pb_sleep();
            return;
        }
        if(!bt_is_connected()) {
            en_auto_pwroff();
            sys_cb.sleep_en = BT_PAIR_SLEEP_EN;
        } else {
            dis_auto_pwroff();
            sys_cb.sleep_en = 1;
        }

        switch (f_bt.disp_status) {
        case BT_STA_CONNECTING:
            if (BT_RECONN_LED_EN) {
                led_bt_reconnect();
                break;
            }
        case BT_STA_INITING:
        case BT_STA_IDLE:
            led_bt_idle();
#if WARNING_BT_PAIR
            if(f_bt.need_pairing && f_bt.disp_status == BT_STA_IDLE) {
                f_bt.need_pairing = 0;
                if(xcfg_cb.warning_bt_pair && xcfg_cb.bt_tws_en) {
                    f_bt.warning_status |= BT_WARN_PAIRING;
                }
            }
#endif
            break;
        case BT_STA_SCANNING:
            led_bt_scan();
            break;

        case BT_STA_DISCONNECTING:
            led_bt_connected();
            break;

        case BT_STA_CONNECTED:
#if BT_TWS_EN
            if(!bt_is_connected()) {
                led_bt_idle();
                break;
            }
#endif
            led_bt_connected();
            break;
        case BT_STA_INCOMING:
            led_bt_ring();
            break;
        case BT_STA_PLAYING:
            led_bt_play();
            break;
        case BT_STA_OUTGOING:
        case BT_STA_INCALL:
            led_bt_call();
            break;
        }

        if(f_bt.disp_status >= BT_STA_CONNECTED) {
            f_bt.need_pairing = 1;
        }

#if BT_BACKSTAGE_EN
        if (f_bt.disp_status < BT_STA_PLAYING && func_cb.sta_break != FUNC_NULL) {
            func_cb.sta = func_cb.sta_break;
        }
#endif
    }
}

AT(.text.func.bt)
void func_bt_status(void)
{
    while(1) {
        func_bt_disp_status();

#if FUNC_BTHID_EN
        if(is_bthid_mode()) {
            func_bt_hid_warning();
        } else
#endif
        {
            func_bt_warning();
        }

        if(f_bt.disp_status != 0xff) {
            break;
        }
    }
}

AT(.text.func.bt)
static void func_bt_process(void)
{
    func_process();
    func_bt_status();

    if(f_bt.disp_status == BT_STA_INCOMING) {
        sfunc_bt_ring();
        reset_sleep_delay();
        reset_pwroff_delay();
#if BT_REC_EN
        bt_music_rec_continue();
#endif
    } else if(f_bt.disp_status == BT_STA_OTA) {
        sfunc_bt_ota();
        reset_sleep_delay();
        reset_pwroff_delay();
    } else if(f_bt.disp_status >= BT_STA_OUTGOING) {
        sfunc_bt_call();
        reset_sleep_delay();
        reset_pwroff_delay();
#if BT_REC_EN
        bt_music_rec_continue();
#endif
    }

    if(sys_cb.pwroff_delay == 0) {
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
        return;
    }
    if(sleep_process(bt_is_sleep)) {
        f_bt.disp_status = 0xff;
    }
}

AT(.text.func.bt)
static void func_bt_enter(void)
{
#if !BT_BACKSTAGE_EN
    bt_init();
#endif

    bt_cvsd_irq_init();
    func_cb.mp3_res_play = func_bt_mp3_res_play;
    msg_queue_clear();
    func_bt_enter_display();
    led_bt_init();

#if WARNING_FUNC_BT
    mp3_res_play(RES_BUF_BT_MODE_MP3, RES_LEN_BT_MODE_MP3);
#endif // WARNING_FUNC_BT

#if WARNING_BT_WAIT_CONNECT
    mp3_res_play(RES_BUF_WAIT4CONN_MP3, RES_LEN_WAIT4CONN_MP3);
#endif // WARNING_BT_WAIT_CONNECT

#if !BT_BACKSTAGE_EN
    bsp_bt_init();
    dis_auto_pwroff();
#endif
    f_bt.disp_status = 0xfe;
    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;

    bt_audio_enable();
#if DAC_DNR_EN
    dac_dnr_set_sta(1);
    sys_cb.dnr_sta = 1;
#endif

#if BT_PWRKEY_5S_DISCOVER_EN
    if(bsp_bt_pwrkey5s_check()) {
        f_bt.need_pairing = 0;  //已经播报了
        func_bt_disp_status();
        func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
    } else {
        func_bt_disp_status();
#if WARNING_BT_PAIR
        if (xcfg_cb.warning_bt_pair && !xcfg_cb.bt_tws_en) {
            func_bt_mp3_res_play(RES_BUF_PAIRING_MP3, RES_LEN_PAIRING_MP3);
        }
#endif // WARNING_BT_PAIR
    }
#endif

#if BT_REC_EN
    bt_music_rec_init();
#endif // BT_REC_EN
#if BT_TWS_EN
    if(xcfg_cb.bt_tws_pair_mode == 1 || xcfg_cb.bt_tws_pair_mode == 2) {
        bt_set_tws_scan(1);
    }
#endif
#if BT_SCO_DBG_EN
    f_bt.mic_anl_gain   = BT_ANL_GAIN;
    f_bt.mic_dig_gain   = BT_DIG_GAIN;
    f_bt.aec_level      = BT_ECHO_LEVEL;
    f_bt.aec_offset     = BT_FAR_OFFSET;
    f_bt.noise          = BT_NOISE_THRESHOID;
#endif
}

AT(.text.func.bt)
static void func_bt_exit(void)
{
#if BT_REC_EN
    sfunc_record_stop();
#endif // BT_REC_EN

    dac_fade_out();
#if DAC_DNR_EN
    dac_dnr_set_sta(0);
    sys_cb.dnr_sta = 0;
#endif
#if BT_PWRKEY_5S_DISCOVER_EN
    bsp_bt_pwrkey5s_clr();
#endif
    func_bt_exit_display();
    bt_audio_bypass();
#if !BT_BACKSTAGE_EN
    bt_disconnect();
    bt_off();
#else
    if (f_bt.disp_status == BT_STA_PLAYING) {       //蓝牙退出停掉音乐
        bt_music_pause();
        while (bt_get_status() == BT_STA_PLAYING) {
            delay_5ms(2);
        }
    }
#endif
    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;
    func_cb.last = FUNC_BT;
}

AT(.text.func.bt)
static void func_bt_pb_display(void)
{
    u8 pb_sta = bt_pb_check();
    static u8 pb_sta_pre = 0;
    if (!pb_sta) {
        func_bt_display();
        if (pb_sta_pre) {
            f_bt.disp_status = 0xff;        //重新获取一下状态
        }
    }
    pb_sta_pre = pb_sta;
}

AT(.text.func.bt)
void func_bt(void)
{
    printf("%s\n", __func__);

    func_bt_enter();

    while (func_cb.sta == FUNC_BT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
        func_bt_pb_display();
    }

    func_bt_exit();
}

#endif //FUNC_BT_EN
