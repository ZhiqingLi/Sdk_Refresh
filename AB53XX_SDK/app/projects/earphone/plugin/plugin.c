#include "include.h"

void plugin_init(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    multi_lang_init(sys_cb.lang_id);
#endif

    CLKGAT0 &= ~BIT(11);                    //HUART CLK Disable
    CLKGAT0 &= ~(BIT(15) | BIT(26));        //FMRX CLK Disable
    CLKGAT0 &= ~BIT(18);                    //mbist clk disable
    CLKGAT0 &= ~BIT(21);                    //uart1 clk disable
    CLKGAT0 &= ~BIT(28);                    //pwm clk disable
    if (xcfg_cb.bt_ch_mic <= 1) {
        CLKGAT0 &= ~BIT(23);                //sdadcr clk disable
    } else {
        CLKGAT0 &= ~BIT(22);                //sdadcl clk disable
    }
    CLKGAT0 &= ~BIT(29);

    CLKGAT1 &= ~BIT(0);                     //fmamfdt clk disable
    CLKGAT1 &= ~BIT(1);                     //
    CLKGAT1 &= ~BIT(2);                     //src clk disable
    CLKGAT1 &= ~BIT(3);                     //irrx clk disable
    CLKGAT1 &= ~BIT(4);                     //i2s clk disable
    CLKGAT1 &= ~(BIT(8) | BIT(9) | BIT(10)); //timer3, timer4, timer5 clk disable
    CLKGAT1 &= ~BIT(11);                    //uart2 clk disable
    CLKGAT1 &= ~BIT(12);                    //spi1 clk disable
    CLKGAT1 &= ~(BIT(13) | BIT(14));        //fmlo clk disable

    dac_set_dvol(DAC_MAX_DIGVOL);
}

void plugin_var_init(void)
{
    adc_linein_flag = 0;
    xcfg_cb.pwrkey_config_en = xcfg_cb.pwrkey_config_en & xcfg_cb.user_pwrkey_en;
    xcfg_cb.adkey_config_en  = xcfg_cb.adkey_config_en & xcfg_cb.user_adkey_en;
    xcfg_cb.adkey2_config_en = xcfg_cb.adkey2_config_en & xcfg_cb.user_adkey2_en;
    xcfg_cb.iokey_config_en  = xcfg_cb.iokey_config_en & xcfg_cb.user_iokey_en;
    xcfg_cb.key_double_config_en = xcfg_cb.key_double_config_en & xcfg_cb.user_key_double_en;

    if (!xcfg_cb.iokey_config_en) {
        xcfg_cb.iokey_io0 = IO_NONE;
        xcfg_cb.iokey_io1 = IO_NONE;
        xcfg_cb.iokey_io2 = IO_NONE;
        xcfg_cb.iokey_io3 = IO_NONE;
        xcfg_cb.iokey_num4 = NO_KEY;
    }

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
    if (!xcfg_cb.led_pwr_en) {
        xcfg_cb.rled_io_sel = 0;
        xcfg_cb.rled_lowbat_follow_en = 0;
    }

    if (!xcfg_cb.bt_pwrkey_nsec_discover) {
        xcfg_cb.led_btpair_5s_en = 0;
    }

    //PWRKEY模拟硬开关,需要关闭长按10S复位， 第一次上电开机， 长按5秒进配对等功能。
    if (PWRKEY_2_HW_PWRON) {
        xcfg_cb.powkey_10s_reset = 0;
        xcfg_cb.pwron_frist_bat_en = 0;
        xcfg_cb.bt_pwrkey_nsec_discover = 0;
        xcfg_cb.led_btpair_5s_en = 0;
    }
#if LED_DISP_EN
    led_func_init();
#endif // LED_DISP_EN
    memset(&f_bt, 0, sizeof(f_bt));
}

AT(.com_text.plugin)
void plugin_tmr5ms_isr(void)
{
#if LED_DISP_EN
    bled_fft_dac_pow();
#endif // LED_DISP_EN
}

AT(.com_text.plugin)
void plugin_tmr1ms_isr(void)
{

}

//蓝牙休眠时，唤醒IO配置
void sleep_wakeup_config(void)
{
#if USER_ADKEY
    if (xcfg_cb.user_adkey_en) {
        adcch_io_wakeup_config(ADKEY_CH);
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (xcfg_cb.user_adkey2_en) {
        adcch_io_wakeup_config(ADKEY2_CH);
    }
#endif // USER_ADKEY2

#if USER_IOKEY
    if (xcfg_cb.user_iokey_en) {
        if (xcfg_cb.iokey_config_en) {
            wakeup_gpio_config(xcfg_cb.iokey_io0, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io1, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io2, 1);
            wakeup_gpio_config(xcfg_cb.iokey_io3, 1);
        } else {
            //IOKEY不配置时，默认TSSOP24的IOKEY
            wakeup_gpio_config(IO_PA6, 1);
            wakeup_gpio_config(IO_PE7, 1);
        }
    }
#endif // USER_IOKEY

#if USER_PWRKEY
    if ((xcfg_cb.user_pwrkey_en) && (!PWRKEY_2_HW_PWRON)) {
        wakeup_wko_config();
    }
#endif // USER_PWRKEY

#if LINEIN_2_PWRDOWN_EN
    if (xcfg_cb.linein_2_pwrdown_en) {
        if (!xcfg_cb.linein_det_io) {
            wakeup_gpio_config(IO_PF2, 1);
        } else {
            wakeup_gpio_config(xcfg_cb.linein_det_io, 1);
        }
    }
#endif // LINEIN_2_PWRDOWN_EN
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
    return FUNC_FMRX_EN;
}

AT(.com_text.bsp.sys)
bool is_sd_support(void)
{
    return SD_SUPPORT_EN;
}

AT(.com_text.bsp.sys)
bool is_usb_support(void)
{
    return USB_SUPPORT_EN;
}

//设置WAV RES提示音播放的数字音量 (0 ~ 0x7fff)
u32 get_wav_res_digvol(void)
{
    return 0x3800;
}

void maxvol_tone_play(void)
{
    if(WARING_MAXVOL_MP3) {
        if (func_cb.mp3_res_play) {
            func_cb.mp3_res_play(RES_BUF_MAX_VOL_MP3, RES_LEN_MAX_VOL_MP3);
            if (sys_cb.maxvol_fade) {
                sys_cb.maxvol_fade = 2;
                dac_fade_out();     //抬键的时候再淡入
            }
        }
    } else {
        wav_res_play(RES_BUF_MAX_VOL_WAV, RES_LEN_MAX_VOL_WAV);
    }
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
}

AT(.com_text.port.vbat)
void plugin_vbat_filter(u32 *vbat)
{
}

void plugin_saradc_init(u16 *adc_ch)
{
}

AT(.com_text.port.key)
void plugin_saradc_sel_channel(u16 *adc_ch)
{
}
void plugin_lowbat_vol_reduce(void)
{
}

void plugin_lowbat_vol_recover(void)
{
}

void plugin_hfp_karaok_configure(void)
{
}
