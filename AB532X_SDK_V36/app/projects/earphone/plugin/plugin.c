#include "include.h"

volatile int pwrkey_detect_flag;
void plugin_init(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    multi_lang_init(sys_cb.lang_id);
#endif

#if ((UART0_PRINTF_SEL == PRINTF_NONE)  || (!EQ_DBG_IN_UART))
    if (!xcfg_cb.eq_dgb_uart_en) {
        CLKGAT0 &= ~BIT(10);    //UART0CLK
    }
#endif
    CLKGAT0 &= ~BIT(9);       //SDCLK
    CLKGAT0 &= ~BIT(11);      //HUART
    CLKGAT0 &= ~BIT(14);      //USBCLK
    CLKGAT0 &= ~BIT(15);      //FMRXCLK
    CLKGAT0 &= ~BIT(21);      //UART1CLK
    CLKGAT0 &= ~BIT(24);      //TIME1
    CLKGAT0 &= ~BIT(25);      //TIME2
    CLKGAT0 &= ~BIT(29);      //SPFCLK
    CLKGAT1 &= ~(BIT(0)|BIT(1)|BIT(3)|BIT(4) |BIT(8)  |BIT(11)  |BIT(12)); //FMAMFDT,FMTX,IRRX,IIS,TIME3,UART2,SPI1
    CLKGAT1 &= ~BIT(13);      //FMLODIV
    CLKGAT1 &= ~BIT(14);      //FMLO_AFCCLK
}

void lopll_2_pll1out_init(void)   //耳机省功耗
{

}

void plugin_var_init(void)
{
    pwrkey_detect_flag = 0;
    xcfg_cb.pwrkey_config_en = xcfg_cb.pwrkey_config_en & xcfg_cb.user_pwrkey_en;
    xcfg_cb.adkey_config_en  = xcfg_cb.adkey_config_en & xcfg_cb.user_adkey_en;
    xcfg_cb.adkey2_config_en = xcfg_cb.adkey2_config_en & xcfg_cb.user_adkey2_en;
    xcfg_cb.iokey_config_en  = xcfg_cb.iokey_config_en & xcfg_cb.user_iokey_en;
    xcfg_cb.key_double_config_en = xcfg_cb.key_double_config_en & xcfg_cb.user_key_double_en;



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
    }

#if LED_DISP_EN
	led_func_init();
#endif // LED_DISP_EN
}

AT(.com_text.plugin)
void plugin_tmr5ms_isr(void)
{

}

AT(.com_text.plugin)
void plugin_tmr1ms_isr(void)
{

}

void plugin_saradc_init(u16 *adc_ch)
{

}

void plugin_saradc_sel_channel(u16 *adc_ch)
{

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
    lefmic_zero_detect_do(ptr, samples);
#endif // MICL_MUX_DETECT_LINEIN
}

bool bt_hfp_ring_number_en(void)
{
    return xcfg_cb.bt_hfp_ring_number_en;
}


AT(.com_text.bsp.sys)
bool is_sd_support(void)
{
    return false;
}

AT(.com_text.bsp.sys)
bool is_sd1_support(void)
{
    return false;
}

AT(.com_text.bsp.sys)
bool is_usb_support(void)
{
    return false;
}

bool is_sleep_dac_off_enable(void)
{
    return true;
}

//设置WAV RES提示音播放的数字音量 (0 ~ 0x7fff)
u32 get_piano_digvol(void)
{
    return 0x6000;
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
    bsp_piano_warning_play(WARNING_TONE, TONE_MAX_VOL);
#endif
}

void minvol_tone_play(void)
{

}

void plugin_playmode_warning(void)
{

}

void plugin_lowbat_vol_reduce(void)
{
#if LOWPWR_REDUCE_VOL_EN
    music_src_set_volume(0x50c0);       //设置音乐源音量达到整体降低系统音量 (范围：0~0x7fff)
#endif // LOWPWR_REDUCE_VOL_EN
}

void plugin_lowbat_vol_recover(void)
{
#if LOWPWR_REDUCE_VOL_EN
    music_src_set_volume(0x7fff);       //还原音量
#endif // LOWPWR_REDUCE_VOL_EN
}

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

}

bool plugin_func_idle_enter_check(void)
{
    //可以在这里决定否需要进入idle
    return true;
}

//用于karaok初始化
void plugin_karaok_init(void)
{

}
