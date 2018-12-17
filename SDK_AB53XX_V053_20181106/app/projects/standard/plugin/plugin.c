#include "include.h"

volatile int pwrkey_detect_flag;            //pwrkey 820K用于复用检测的标志。

#if SYS_KARAOK_EN
//{第一级混响幅度[0, 256]， 每级混响衰减系数[0, 256]}
const u16 echo_level_gain_16[16 + 1][2] = {
    {0, 0},
    {45, 20},   {60, 25},   {65, 30},   {65, 51},
    {79, 51},   {79, 72},   {79, 85},   {79, 101},
    {85, 131},  {85, 142},  {86, 147},  {88, 160},
    {88, 163},  {88, 167},  {95, 171},  {170, 150},
};

//{第一级混响幅度[0, 256]， 每级混响衰减系数[0, 256]}
const u16 echo_level_gain_12[12 + 1][2] = {
    {0, 0},
    {45, 20},   {65, 30},   {65, 51},    {79, 51},
    {79, 72},   {79, 85},   {85, 131},   {86, 147},
    {88, 160},  {88, 167},  {95, 171},   {170, 150},
};
#endif

void plugin_init(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    multi_lang_init(sys_cb.lang_id);
#endif
}

void plugin_var_init(void)
{
    pwrkey_detect_flag = 0;
    xcfg_cb.pwrkey_config_en = xcfg_cb.pwrkey_config_en & xcfg_cb.user_pwrkey_en;
    xcfg_cb.adkey_config_en  = xcfg_cb.adkey_config_en & (xcfg_cb.user_adkey_en | xcfg_cb.user_adkey_mux_sdclk_en);
    xcfg_cb.adkey2_config_en = xcfg_cb.adkey2_config_en & xcfg_cb.user_adkey2_en;
    xcfg_cb.iokey_config_en  = xcfg_cb.iokey_config_en & xcfg_cb.user_iokey_en;
    xcfg_cb.key_double_config_en = xcfg_cb.key_double_config_en & xcfg_cb.user_key_double_en;

#if FUNC_AUX_EN
    if (!xcfg_cb.func_aux_en) {
        xcfg_cb.linein_2_pwrdown_en = 0;
        xcfg_cb.linein_det_iosel = 0;
        xcfg_cb.auxl_sel = 0;
        xcfg_cb.auxr_sel = 0;
        xcfg_cb.mode_2_aux_en = 0;
    }
#endif // FUNC_AUX_EN

    if (!xcfg_cb.led_disp_en) {
        xcfg_cb.bled_io_sel = 0;
        xcfg_cb.charge_full_bled = 0;
        xcfg_cb.led_pwron_config_en   =  0;
        xcfg_cb.led_pwroff_config_en  =  0;
        xcfg_cb.led_btpair_config_en  =  0;
        xcfg_cb.led_btconn_config_en  =  0;
        xcfg_cb.led_btmusic_config_en =  0;
        xcfg_cb.led_btring_config_en  =  0;
        xcfg_cb.led_btcall_config_en  =  0;
        xcfg_cb.led_lowbat_config_en  =  0;
    }

    if (!xcfg_cb.func_music_en) {
        xcfg_cb.music_wav_support = 0;
        xcfg_cb.music_wma_support = 0;
        xcfg_cb.music_ape_support = 0;
        xcfg_cb.music_flac_support = 0;
        xcfg_cb.music_sbc_support = 0;
        xcfg_cb.music_udisk_en = 0;
        xcfg_cb.music_sdcard_en = 0;
    }

    if (!xcfg_cb.music_sdcard_en) {
        xcfg_cb.sddet_iosel = 0;
    }

    if (!EARPHONE_DETECT_EN) {
        xcfg_cb.earphone_det_iosel = 0;
    }

    if (!LINEIN_DETECT_EN) {
        xcfg_cb.linein_det_iosel = 0;
    }

    if (!xcfg_cb.led_pwr_en) {
        xcfg_cb.rled_io_sel = 0;
    }

#if LED_DISP_EN
	led_func_init();
#endif // LED_DISP_EN
}

AT(.com_text.plugin)
void plugin_tmr5ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_level_calc();
#endif
}

AT(.com_text.plugin)
void plugin_tmr1ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_scan();
#endif
}

//蓝牙休眠时，唤醒IO配置
void sleep_wakeup_config(void)
{
#if USER_ADKEY
    if (xcfg_cb.user_adkey_en) {
        adcch_io_wakeup_config(ADKEY_CH);           //配置ADKEY IO下降沿唤醒。
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (xcfg_cb.user_adkey2_en) {
        adcch_io_wakeup_config(ADKEY2_CH);          //配置ADKEY1 IO下降沿唤醒。
    }
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        adcch_io_wakeup_config(SDCLK_AD_CH);      //IO下降沿唤醒。
    }
#endif // USER_ADKEY_MUX_SDCLK

#if USER_IOKEY
    if (xcfg_cb.user_iokey_en) {
        if (xcfg_cb.iokey_config_en) {
            wakeup_gpio_config(xcfg_cb.iokey_io0, 1);   //配置IO下降沿唤醒。
            wakeup_gpio_config(xcfg_cb.iokey_io1, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io2, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io3, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io4, 1);
        } else {
            //不用工具配置IOKEY时，根据实际使用的IOKEY IO进行修改。
            wakeup_gpio_config(IO_PF0, 1);
            wakeup_gpio_config(IO_PF1, 1);
            wakeup_gpio_config(IO_PF3, 1);
        }
    }
#endif // USER_IOKEY

#if USER_PWRKEY
    if (xcfg_cb.user_pwrkey_en) {
        wakeup_wko_config();
    }
#endif // USER_PWRKEY
}

AT(.com_text.bsp.sys)
void lefmic_zero_detect(u8 *ptr, u32 samples)
{
#if MICL_MUX_DETECT_LINEIN
    if (is_linein_det_mux_micl()) {
        lefmic_zero_detect_do(ptr, samples);
    }
#endif // MICL_MUX_DETECT_LINEIN
}

AT(.com_text.bsp.sys)
bool bt_hfp_ring_number_en(void)
{
    return xcfg_cb.bt_hfp_ring_number_en;
}

AT(.com_text.bsp.sys)
bool is_func_fmrx_en(void)
{
    return xcfg_cb.func_fmrx_en;
}

AT(.com_text.bsp.sys)
bool is_sd_support(void)
{
#if (SD0_MAPPING == SD0MAP_G2) && EQ_DBG_IN_UART
    if (xcfg_cb.eq_dgb_uart_en && xcfg_cb.eq_uart_sel == 1) {
        return false;
    }
#endif
    return (SD_SUPPORT_EN * xcfg_cb.music_sdcard_en);
}

AT(.com_text.bsp.sys)
bool is_usb_support(void)
{
#if EQ_DBG_IN_UART
    if (xcfg_cb.eq_dgb_uart_en && xcfg_cb.eq_uart_sel == 2) {
        return false;
    }
#endif
    return ((MUSIC_UDISK_EN * xcfg_cb.music_udisk_en) | FUNC_USBDEV_EN);
}

AT(.com_text.plugin)
bool is_sleep_dac_off_enable(void)
{
    if (xcfg_cb.linein_det_iosel == IO_MUX_MICL) {
        return false;
    }

    return true;
}

//设置WAV RES提示音播放的数字音量 (0 ~ 0x7fff)
u32 get_wav_res_digvol(void)
{
    return 0x5000;
}

void maxvol_tone_play(void)
{
#if WARING_MAXVOL_MP3
    if (func_cb.mp3_res_play) {
        func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
        if (sys_cb.maxvol_fade) {
            sys_cb.maxvol_fade = 2;
            dac_fade_out();     //抬键的时候再淡入
        }
    }
#else
#if SYS_KARAOK_EN
#if KARAOK_REC_EN
    sfunc_record_pause();
#endif // KARAOK_REC_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
    wav_res_play(RES_BUF_MAX_VOL_WAV, RES_LEN_MAX_VOL_WAV);
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
#if KARAOK_REC_EN
    sfunc_record_continue();
#endif // KARAOK_REC_EN
#else
    wav_res_play(RES_BUF_MAX_VOL_WAV, RES_LEN_MAX_VOL_WAV);
#endif // SYS_KARAOK_EN
#endif // WARING_MAXVOL_MP3
}

void minvol_tone_play(void)
{
//    if (func_cb.mp3_res_play) {
//        func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
//    }
}

#if WARNING_ADJ_VOLUME
bool adjvol_tone_play(void)
{
    if(RES_LEN_ADJ_VOL_WAV == 0) {
        return false;
    } else {
        wav_res_play(RES_BUF_ADJ_VOL_WAV, RES_LEN_ADJ_VOL_WAV);
        return true;
    }
}
#endif

//用于调DAC音质接口函数
void plugin_music_eq(void)
{
    bsp_eq_init();
    sys_cb.eq_mode = 0;
    music_set_eq_by_num(sys_cb.eq_mode);
//   mic_set_eq_by_res(&RES_BUF_EQ_MIC_NORMAL_EQ, &RES_LEN_EQ_MIC_NORMAL_EQ);

//#if AUCAR_EQ_LOUD_BASS_TREBLE_EN
//    aucar_music_loud_set(4);
//    aucar_music_bass_set(8);
//    aucar_music_treble_set(12);
//    printf("eq_loud_bass_tre test\n");
//#endif
}

AT(.com_text.port.vbat)
void plugin_vbat_filter(u32 *vbat)
{
#if  VBAT_FILTER_USE_PEAK
    //电源波动比较大的音箱方案, 取一定时间内的电量"最大值"或"次大值",更能真实反应电量.
    #define VBAT_MAX_TIME  (3000/5)   //电量峰值判断时间 3S
    static u16 cnt = 0;
	static u16 vbat_max_cnt = 0;
    static u32 vbat_max[2] = {0,0};
    static u32 vbat_ret = 0;
    u32 vbat_cur = *vbat;
    if (cnt++  < VBAT_MAX_TIME) {
        if (vbat_max[0] < vbat_cur) {
            vbat_max[1] = vbat_max[0];  //vbat_max[1] is less max (次大值)
            vbat_max[0] = vbat_cur;     //vbat_max[0] is max(最大值)
            vbat_max_cnt = 0;
        } else if (vbat_max[0] == vbat_cur) {
            vbat_max_cnt ++;
        }
    } else {
        if (vbat_max_cnt >= VBAT_MAX_TIME/5) {  //总次数的(1/5)都采到最大值,则返回最大值.
            vbat_ret = vbat_max[0];
        } else if (vbat_max[1] != 0) {   //最大值次数较少,则返回次大值(舍弃最大值)
            vbat_ret = vbat_max[1];
        }
        vbat_max[0] = 0;
        vbat_max[1] = 0;
        vbat_max_cnt = 0;
        cnt = 0;
    }
    //返回值
    if (vbat_ret != 0) {
        *vbat = vbat_ret;
    }
#endif
}

//初始化完成, 各方案可能还有些不同参数需要初始化,预留接口到各方案
void plugin_sys_init_finish_callback(void)
{
#if ENERGY_LED_EN
    energy_led_init();
#endif
}

bool plugin_func_idle_enter_check(void)
{
    //可以在这里决定否需要进入idle
    return true;
}

void plugin_saradc_init(u16 *adc_ch)
{
#if USER_KEY_KNOB_EN
    *adc_ch |= BIT(ADCCH_PE5);       //旋钮1
    *adc_ch |= BIT(ADCCH_PE6);       //旋钮2
#endif
}

#if USER_KEY_KNOB_EN
//16级旋钮
AT(.com_rodata.port.key)
const u8 tbl_knob_level_16[16 + 1] = {
    0x02, 0x13, 0x24, 0x36, 0x47, 0x59, 0x6A, 0x7B,
    0x8B, 0x9A, 0xA0, 0xB2, 0xC3, 0xD5, 0xE6, 0xF8,
    0xFF,
};
#endif

AT(.com_text.port.key)
void plugin_saradc_sel_channel(u16 *adc_ch)
{
#if USER_KEY_KNOB_EN
    u8 mic_vol_val = (u8)(adc_cb.sfr[ADCCH_PE5] >> 2);
    u8 echo_level_val = (u8)(adc_cb.sfr[ADCCH_PE6] >> 2);

    static u8 echo_level = USER_KEY_KNOB_LEVEL, mic_vol = USER_KEY_KNOB_LEVEL;

    key_knob_process(echo_level_val, tbl_knob_level_16, &echo_level);
    if (echo_level != sys_cb.echo_level) {
        sys_cb.echo_level = echo_level;
        msg_enqueue(EVT_ECHO_LEVEL);
    }

    key_knob_process(mic_vol_val, tbl_knob_level_16, &mic_vol);
    if (mic_vol != sys_cb.mic_vol) {
        sys_cb.mic_vol = mic_vol;
        msg_enqueue(EVT_MIC_VOL);
    }
#endif
}

void plugin_lowbat_vol_reduce(void)
{
//    karaok_set_music_volume(0x50c0);                //0~0x7fff
}

void plugin_lowbat_vol_recover(void)
{
//    karaok_set_music_volume(0x7fff);
}

void plugin_hfp_karaok_configure(void)
{
}
//用于karaok初始化
void plugin_karaok_init(void)
{
#if SYS_KARAOK_EN
    sys_cb.echo_delay = SYS_ECHO_DELAY;
#if !USER_KEY_KNOB_EN   //没有旋钮的话就固定一个值
    sys_cb.music_vol = USER_KEY_KNOB_LEVEL - 1;
    sys_cb.echo_level = USER_KEY_KNOB_LEVEL - 1;
    sys_cb.mic_vol = USER_KEY_KNOB_LEVEL - 1;
#endif

#if DUAL_MIC_CHANNEL_EN
    sys_cb.mic_left_vol = USER_KEY_KNOB_LEVEL - 1;
    sys_cb.mic_right_vol = USER_KEY_KNOB_LEVEL - 1;
#endif

#endif
}


#if SYS_MAGIC_VOICE_EN
const u8 magic_table[4][2] = {
    {0, 5},
    {0, 2},
    {1, 1},
    {1, 4},
};

void magic_voice_switch(void)
{
#if SYS_KARAOK_EN
    static int magic_sel = 0;
    magic_sel++;
    if (magic_sel > 4) {
        magic_sel = 0;
    }
#if KARAOK_REC_EN
    sfunc_record_pause();
#endif // KARAOK_REC_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
    if (!magic_sel) {
        //close
        sys_cb.magic_en = 0;
        sys_cb.magic_type = 0;
        sys_cb.magic_level = 0;
        bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
    } else {
        sys_cb.magic_en = 1;
        sys_cb.magic_type = magic_table[magic_sel - 1][0];
        sys_cb.magic_level = magic_table[magic_sel - 1][1];
        bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
    }
#if KARAOK_REC_EN
    sfunc_record_continue();
#endif // KARAOK_REC_EN
#endif // SYS_KARAOK_EN
}
#endif // SYS_MAGIC_VOICE_EN
