#include "include.h"


#define VBG_VOLTAGE             pmu_get_vbg()

adc_cb_t adc_cb AT(.buf.key.cb);
extern u8 pwr_usage_id;

#if USER_KEY_DOUBLE_EN
const u16 tbl_double_key[] = USER_KEY_DOUBLE;
#endif

bool get_adc_val(void);
u16 get_vbat_val(void);
int s_abs(int x);

#if USER_KEY_THRICE_EN
typedef struct {
    u8  cnt;
    u8  stable;
    u16 val;
} key_thrice_t;
key_thrice_t key_thrice_cb;
#endif // USER_KEY_THRICE_EN

AT(.com_text.key.table)
const key_shake_tbl_t key_shake_table = {
    .scan_cnt = KEY_SCAN_TIMES,
    .up_cnt   = KEY_UP_TIMES,
    .long_cnt = KEY_LONG_TIMES,
    .hold_cnt = KEY_LONG_HOLD_TIMES,
};

AT(.saradc_text.init)
void bsp_saradc_init(u16 adc_ch)
{
//    printf("%s: %04x\n", __func__, adc_ch);
    if (adc_ch == 0) {
        return;
    }
    memset(&adc_cb, 0, sizeof(adc_cb));
    adc_cb.sfr = (psfr_t)(&SADCDAT0);           //SADCDAT SFR
    adc_cb.channel = adc_ch;
    saradc_init(adc_cb.channel);
    saradc_setup_time_set(VBAT2_ADCCH, 3);      //如通过IO检测电池电压, 当然上下拉分压电阻比较大,如500K, 需要加大采样的建立时间(采样延时)
#if USER_ADKEY_MUX_LED
    saradc_baud_set(0x09);
    saradc_setup_time_set(ADKEY_CH, 3);
#endif // USER_ADKEY_MUX_LED

#if USER_ADKEY
    adc_cb.key_val = 0xff;
#endif // USER_ADKEY

#if USER_ADKEY2
    adc_cb.key2_val = 0xff;
#endif // USER_ADKEY2

    saradc_kick_start(adc_cb.channel, USER_ADKEY_MUX_LED);

#if VBAT_DETECT_EN
    while(!get_adc_val());                  //获取一次初值，再kick一次
    adc_cb.vbat_val =  (uint)adc_cb.vbat2 * VBG_VOLTAGE / adc_cb.bg;
    adc_cb.vbat_total = adc_cb.vbat_val << 5;
    sys_cb.vbat = get_vbat_val();
#endif // VBAT_DETECT_EN
}

//AT(.com_rodata.bb)
//const char str_adc_v[] = "SADCDAT: %02x\n";
AT(.com_text.port.key)
bool get_adc_val(void)
{
    u16 channel;

    if (!is_saradc_convert_finish()) {
        return false;
    }

#if USER_ADKEY
    if (xcfg_cb.user_adkey_en) {
        adc_cb.key_val = (u8)(adc_cb.sfr[ADKEY_CH] >> 2);
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (xcfg_cb.user_adkey2_en) {
        adc_cb.key2_val = (u8)(adc_cb.sfr[ADKEY2_CH] >> 2);
    }
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        adc_cb.sdclk_valid = 0;
        if (adc_cb.sdclk_convert) {
            adc_cb.sdclk_val = (u8)(adc_cb.sfr[SDCLK_AD_CH] >> 2);
            adc_cb.sdclk_valid = 1;
        }
    }
#endif // USER_ADKEY_MUX_SDCLK

#if USER_PWRKEY
    adc_cb.vrtc_val = adc_cb.sfr[ADCCH_VRTC];
    adc_cb.wko_val  = adc_cb.sfr[ADCCH_WKO];
#endif // USER_PWRKEY

#if VBAT_DETECT_EN || CHARGE_EN
	adc_cb.bg = (u8)(adc_cb.sfr[ADCCH_BGOP] >> 2);
#if VBAT_DETECT_EN
    adc_cb.vbat2 = (u8)(adc_cb.sfr[VBAT2_ADCCH] >> 2);
#endif // VBAT_DETECT_EN
#if CHARGE_EN
    adc_cb.vusb_val = (u8)(adc_cb.sfr[ADCCH_VUSB] >> 2);
#endif // CHARGE_EN
#endif

    plugin_saradc_sel_channel(&adc_cb.channel);      //其他的ADC通道
    channel = adc_cb.channel;

#if USER_ADKEY_MUX_SDCLK
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        adc_cb.sdclk_convert = 1;
        if (is_det_sdcard_busy()) {
            channel &= ~BIT(SDCLK_AD_CH);
            adc_cb.sdclk_convert = 0;
        }
    }
#endif // USER_ADKEY_MUX_SDCLK

    saradc_kick_start(channel, USER_ADKEY_MUX_LED);
//    printf(str_adc_v, adc_cb.key_val);
    return true;
}

#if USER_ADKEY_MUX_SDCLK
AT(.com_text.port.key)
void adc_mux_sdclk_w4_convert(void)
{
    if ((xcfg_cb.user_adkey_mux_sdclk_en) && (adc_cb.sdclk_convert)) {
        while (!is_saradc_convert_finish()) {
            WDT_CLR();
        }
    }
}
#endif // USER_ADKEY_MUX_SDCLK

#if (USER_ADKEY || USER_ADKEY_MUX_SDCLK)
AT(.com_text.port.key)
static u8 get_adkey(u8 key_val, u8 key_config_en)
{
    u8 num = 0;
    u8 *ptr;

    //配置工具是否使能
    if (!key_config_en) {
        return NO_KEY;
    }

    while (key_val > adkey_table[num].adc_val) {
        num++;
    }

    //工具配置了第一组ADKEY的按键定义？
    ptr = get_adkey_configure(num);
    if (ptr != NULL) {
        if ((num > 11) || (adkey_table[num].adc_val == 0xff)) {
            return NO_KEY;
        }
        return key_config_table[*(ptr+num)];
    }

    return adkey_table[num].usage_id;
}
#endif // USER_ADKEY

#if USER_ADKEY2
AT(.com_text.port.key)
static u8 get_adkey2(void)
{
    u8 num = 0;
    u8 *ptr;

    //配置工具是否使能了第二组ADKEY2？
    if (!xcfg_cb.user_adkey2_en) {
        return NO_KEY;
    }

    while (adc_cb.key2_val > adkey2_table[num].adc_val) {
        num++;
    }

    //工具配置了第二组ADKEY2的按键定义？
    ptr = get_adkey2_configure(num);
    if (ptr != NULL) {
        if ((num > 11) || (adkey2_table[num].adc_val == 0xff)) {
            return NO_KEY;
        }
        return key_config_table[*(ptr+num)];
    }
    return adkey2_table[num].usage_id;
}
#endif // USER_ADKEY2

#if USER_PWRKEY
AT(.com_text.port.key)
static u8 get_pwrkey(void)
{
    u8 num = 0;
    u16 key_val = ((u32)adc_cb.wko_val << 8) / adc_cb.vrtc_val;
    u8 *ptr;

//    //配置工具是否使能了PWRKEY？
    if (!xcfg_cb.user_pwrkey_en) {
        return NO_KEY;
    }

    if (key_val > 0xff) {
        key_val = 0xff;
    }
    while ((u8)key_val > pwrkey_table[num].adc_val) {
        num++;
    }

    //工具配置了PWRKEY的按键定义？
    ptr = get_pwrkey_configure(num);
    if (ptr != NULL) {
        if (num > 4) {
            return NO_KEY;
        }
        return key_config_table[*(ptr+num)];
    }
    return pwrkey_table[num].usage_id;
}
#endif // USER_PWRKEY

AT(.text.bsp.power)
bool pwrkey_get_status(void)
{
    u32 delay = 20;
    u32 pre_sta = 0;
    u32 sta_cnt = 0;
    while(delay--) {
        u32 tmp_sta = (RTCCON & BIT(19));
        if(pre_sta == tmp_sta) {
            sta_cnt++;
        } else {
            pre_sta = tmp_sta;
            sta_cnt = 0;
        }
        delay_ms(1);
        if(sta_cnt > 4) {
            break;
        }
    }

    return (pre_sta == 0)? true : false;
}

#if VBAT_DETECT_EN
//AT(.com_rodata.bat)
//const char bat_str[] = "VBAT: %d.%03dV\n";
AT(.com_text.port.vbat)
u16 get_vbat_val(void)
{
    static u16 vbat_bak = 0;
    u32 vbat = (u32)adc_cb.vbat2 * VBG_VOLTAGE / adc_cb.bg;
    //不同方案可能采用不同 vbat 滤波算法, 在方案对应的plugin.c中处理
    plugin_vbat_filter(&vbat);
    //默认的取平均值算法.
    adc_cb.vbat_total = adc_cb.vbat_total - adc_cb.vbat_val + vbat; //均值
    adc_cb.vbat_val = adc_cb.vbat_total>>5;

    if(adc_cb.vbat_val > vbat_bak) {
        vbat = adc_cb.vbat_val - vbat_bak;
    } else {
        vbat = vbat_bak - adc_cb.vbat_val;
    }
    if (vbat >= 30) {   //偏差大于一定值则更新
        vbat_bak = adc_cb.vbat_val;
//        printf(bat_str, adc_cb.vbat_val/1000, adc_cb.vbat_val%1000);
    }

    return vbat_bak;
}

int is_lowpower_vbat_warning(void)
{
    if (sys_cb.vbat <= ((u16)LPWR_OFF_VBAT*100+2700)) {
        if (LPWR_OFF_VBAT) {
            if (!sys_cb.lpwr_cnt) {
                sys_cb.lpwr_cnt = 1;
            } else if (sys_cb.lpwr_cnt >= 10) {
                return 1;       //VBAT低电关机
            }
        }
        return 0;               //VBAT低电不关机
    }
#if WARNING_LOW_BATTERY
    else {
        sys_cb.lpwr_cnt = 0;
        if (sys_cb.vbat < ((u16)LPWR_WARNING_VBAT*100 + 2800)) {
    #if LED_LOWBAT_EN
            if (xcfg_cb.rled_lowbat_en) {
                if ((!CHARGE_DC_IN()) && (!RLED_LOWBAT_FOLLOW_EN)) {
                    led_lowbat();
                } else {
                    led_lowbat_recover();
                }
            }
    #endif // LED_LOWBAT_EN
            if (xcfg_cb.lowpower_warning_en) {
                return 2;       //低电压提示音播报
            }
        } else {
    #if LED_LOWBAT_EN
            if (xcfg_cb.rled_lowbat_en) {
                led_lowbat_recover();
            }
    #endif // LED_LOWBAT_EN
        }
        return 0;
    }
#endif // WARNING_LOW_BATTERY
    sys_cb.lpwr_cnt = 0;
    return 0;
}
#endif  //VBAT_DETECT_EN


AT(.text.bsp.power)
bool power_off_check(void)
{
#if CHARGE_EN
    u16 charge_cnt = 0;
#endif
#if USER_PWRKEY
    int pwrkey_pressed_flag, ticks, up_cnt = 0;
    pwrkey_pressed_flag = 0;
#endif // USER_PWRKEY

    while (1) {
        WDT_CLR();
        delay_ms(5);
#if USER_PWRKEY
        if ((get_pwrkey() & K_PWR_MASK) == K_PWR) {
            up_cnt = 0;

            if (!pwrkey_pressed_flag) {
                ticks = tick_get();
                pwrkey_pressed_flag = 1;
            }
            if (!sys_cb.poweron_flag) {
                if (tick_check_expire(ticks, PWRON_PRESS_TIME)) {                   //长按开机时间配置
                    sys_cb.poweron_flag = 1;
                }
            }
        } else {
            if (up_cnt < 3) {
                up_cnt++;
            }
            if (up_cnt == 3) {
                up_cnt = 10;
                sys_cb.poweron_flag = 0;
                pwrkey_pressed_flag = 0;
            }
        }
#endif // USER_PWRKEY

#if CHARGE_EN
        if (xcfg_cb.charge_en) {
			charge_cnt++;
			if (charge_cnt > 20) {
                charge_cnt = 0;
                charge_detect(0);
            }
        }
#endif // CHARGE_EN

        if (sys_cb.poweron_flag) {
            if ((CHARGE_DC_NOT_PWRON) && CHARGE_DC_IN()) {
                continue;
            }
#if LINEIN_2_PWRDOWN_EN
            if (dev_is_online(DEV_LINEIN)) {
                continue;
            }
#endif // LINEIN_2_PWRDOWN_EN
            //长按PP/POWER开机
            gui_display(DISP_POWERON);
            led_power_up();
            dac_restart();
            bsp_change_volume(sys_cb.vol);
#if WARNING_POWER_ON
            mp3_res_play(RES_BUF_POWERON_MP3, RES_LEN_POWERON_MP3);
#endif // WARNING_POWER_ON

            if (PWRON_ENTER_BTMODE_EN) {
                func_cb.sta = FUNC_BT;
                if (dev_is_online(DEV_UDISK)) {
                    sys_cb.cur_dev = DEV_UDISK;
                } else {
                    sys_cb.cur_dev = DEV_SDCARD;
                }
            } else {
                if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_UDISK)) {
                    func_cb.sta = FUNC_MUSIC;
                    if (dev_is_online(DEV_UDISK)) {
                        sys_cb.cur_dev = DEV_UDISK;
                    } else {
                        sys_cb.cur_dev = DEV_SDCARD;
                    }
                } else {
#if FUNC_BT_EN
                    func_cb.sta = FUNC_BT;
#elif FUNC_CLOCK_EN
                    func_cb.sta = FUNC_CLOCK;
#else
                    func_cb.sta = FUNC_IDLE;
#endif
                }
            }
            return true;
        } else {
            if (CHARGE_DC_IN()) {
                continue;
            } else {
                return false;
            }
        }
    }
}

AT(.text.bsp.power)
bool is_powron_frist_enable(void)
{
    if (!PWRON_FRIST_BAT_EN) {
        return false;
    }

    if (IS_PWRKEY_PRESS()) {
        return false;
    }

    if (CHARGE_DC_IN() && (CHARGE_DC_NOT_PWRON) && (xcfg_cb.charge_en)) {
        return false;
    }
    return true;
}

AT(.text.bsp.power)
void charge_full_bled_on_time_check(void)  //1S调用 一次
{
    if (BLED_CHARGE_FULL_EN && LED_DISP_EN) {
        printf("ckcnt = %d\n",sys_cb.charge_full_bled_on_cnt);
        if (sys_cb.charge_full_bled_on_cnt < 32767) {
            if (sys_cb.charge_full_bled_on_cnt > 0) {
                sys_cb.charge_full_bled_on_cnt--;
            }

            if (sys_cb.charge_full_bled_on_cnt <= 1) {
                sys_cb.charge_bled_flag = 0;
                printf("--->off\n");
                LED_SET_OFF();
            }
        }
    }
}

AT(.text.bsp.power)
void power_on_check(void)
{
    int pwrkey_pressed_flag = 0;

#if CHARGE_EN
    u16 charge_cnt = 0;
#if CHARGE_FINISH_INTO_LOWPOWER
    u32 buf[3] = {0};
    u8 charge_into_rc_flag = 0;
    u8 p111_en = 0;
#endif
#endif
    u8 rtccon8 = irtc_sfr_read(RTCCON8_CMD);
    u8 rtccon3 = irtc_sfr_read(RTCCON3_CMD);

    irtc_sfr_write(RTCCON3_CMD, irtc_sfr_read(RTCCON3_CMD) | 0x7);  //VDDCORE, VDDIO, BUCK enable
    irtc_sfr_write(RTCCON8_CMD, 0x07);                              //Clr pending

#if USER_PWRKEY_EXT
    PWRKEY_EXT_INIT();
#endif
    if (!SOFT_POWER_ON_OFF) {
        return;
    }

#if USER_PWRKEY || USER_PWRKEY_EXT
    int up_cnt = 0, ticks;
    #if USER_PWRKEY
    if (!xcfg_cb.user_pwrkey_en) {
        return;
    }
    //第一次上电是否直接开机
    if (is_powron_frist_enable()) {
        return;
    }
    #endif
    if (!IS_PWRKEY_PRESS()) {
        pwrkey_pressed_flag = 1;
        ticks = sys_cb.ms_ticks;
    }
#endif // USER_PWRKEY
    if (rtccon8 & BIT(0)) {
        return;                                                     //长按PWRKEY 10S复位后直接开机。
    }
#if CHARGE_FINISH_INTO_LOWPOWER
    if ((CHARGE_FINISH_2_LOWPOWER == xcfg_cb.charge_warehouse_set) || (CHARGE_OUT_2_POWERON == xcfg_cb.charge_warehouse_set)) {
        if ((CHARGE_DC_NOT_PWRON) && CHARGE_DC_IN()){
             p111_en = pll1_disable();
            sys_clk_seclet_RC(buf);
            charge_into_rc_flag = 1;
        }
    }
#endif // CHARGE_FINISH_INTO_LOWPOWER
    while (1) {
        WDT_CLR();
#if CHARGE_FINISH_INTO_LOWPOWER
        if(charge_into_rc_flag){
             delay_us(900);
        }else
#endif
		{
			delay_ms(5);
        }
        u8 key_val = bsp_key_scan();
#if USER_PWRKEY || USER_PWRKEY_EXT
        if ((key_val & K_PWR_MASK) == K_PWR) {
            up_cnt = 0;
            if (!pwrkey_pressed_flag) {
                ticks = tick_get();
                pwrkey_pressed_flag = 1;
            }
            if (!sys_cb.poweron_flag) {
                if (tick_check_expire(ticks, PWRON_PRESS_TIME)) {               //长按开机时间配置
                    sys_cb.poweron_flag = 1;
                }
            }
        } else {
            if (up_cnt < 3) {
                up_cnt++;
            }
            if (up_cnt == 3) {
                up_cnt = 10;
                pwrkey_pressed_flag = 0;
                sys_cb.poweron_flag = 0;
            }
        }
#else
        key_val = key_val;      //去warning
#endif // USER_PWRKEY

#if LINEIN_2_PWRDOWN_EN
        linein_detect();
#endif // LINEIN_2_PWRDOWN_EN

#if CHARGE_EN
        if (xcfg_cb.charge_en) {
			charge_cnt++;
			if (charge_cnt > 20) {
                charge_cnt = 0;
                charge_detect(0);
#if CHARGE_FINISH_INTO_LOWPOWER
                if ((CHARGE_FINISH_2_LOWPOWER == xcfg_cb.charge_warehouse_set) || (CHARGE_OUT_2_POWERON == xcfg_cb.charge_warehouse_set)) {
                    if (2 == sys_cb.charge_sta) {  //充电完成.
                        if(charge_into_rc_flag){
                            sys_clk_restore(buf);
                        }
                        printf("charge finish\n");
                        if (charge_finish_into_lowpower_check(1)) {  //充电完成后, 是否要进入低电模式
                            if (p111_en) {
                                pll1_enable();
                            }
                            break;
                        }
                    }
                }
#endif
            } else if (charge_cnt >= 200) {  //1S
                charge_cnt = 0;
                charge_full_bled_on_time_check();
            }
        }
#endif // CHARGE_EN

        if (sys_cb.poweron_flag) {
#if VBAT_DETECT_EN
            if (sys_cb.vbat <= 2950) {  //电压小于2.95v不开机
                continue;
            }
#endif
            if ((CHARGE_DC_NOT_PWRON) && CHARGE_DC_IN()) {
                continue;
            }
#if LINEIN_2_PWRDOWN_EN
            if (dev_is_online(DEV_LINEIN)) {
                continue;
            }
#endif // LINEIN_2_PWRDOWN_EN
            //长按PP/POWER开机
#if CHARGE_FINISH_INTO_LOWPOWER
            if(charge_into_rc_flag){
                sys_clk_restore(buf);
            }
            if (p111_en) {
                pll1_enable();
            }
#endif // CHARGE_FINISH_INTO_LOWPOWER
            break;
        } else {
            //PWKKEY松开，立刻开关
            if (!pwrkey_pressed_flag) {
                if (CHARGE_DC_IN()) {
                    continue;
                } else {
#if CHARGE_OUT_POWER_ON && CHARGE_FINISH_INTO_LOWPOWER
                    if(charge_into_rc_flag){
                        sys_clk_restore(buf);
                    }
                     if (CHARGE_OUT_2_POWERON == xcfg_cb.charge_warehouse_set) {
                        if ((rtccon8 & BIT(4)) || (rtccon3 & BIT(1))) {   //VUSB 唤醒直接开机.
                            if(!sys_cb.rtc_first_pwron){
                                if (p111_en) {
                                    pll1_enable();
                                }
                                return;
                            }
                        }
                    }
#endif
                    rtccon3 = rtccon3;//去除编译警告
                    saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟
                    sfunc_pwrdown();
                }
            }
        }
    }
}

AT(.text.bsp.key.init)
void key_init(void)
{
    u16 adc_ch = 0;

    key_var_init();

#if USER_KEY_THRICE_EN
    memset(&key_thrice_cb, 0, sizeof(key_thrice_cb));
    key_thrice_cb.stable = get_double_key_time();
#endif // USER_KEY_THRICE_EN

#if USER_IOKEY
    io_key_init();
#endif

#if USER_ADKEY
    if (xcfg_cb.user_adkey_en) {
        adc_ch |= BIT(ADKEY_CH);
    #if ADKEY_PU10K_EN
        adcch_io_pu10k_enable(ADKEY_CH);        //开内部10K上拉
    #endif // ADKEY_PU10K_EN
    }
#endif // USER_ADKEY

#if USER_ADKEY2
    if (xcfg_cb.user_adkey2_en) {
        adc_ch |= BIT(ADKEY2_CH);
    }
#endif // USER_ADKEY2

#if USER_ADKEY_MUX_SDCLK
    if (xcfg_cb.user_adkey_mux_sdclk_en) {
        adc_ch |= BIT(SDCLK_AD_CH);
    }
#endif // USER_ADKEY_MUX_SDCLK

#if USER_PWRKEY
    adc_ch |= (BIT(ADCCH_VRTC) | BIT(ADCCH_WKO));
    pwr_usage_id = pwrkey_table[0].usage_id;
    if (xcfg_cb.pwrkey_config_en) {
        pwr_usage_id = key_config_table[xcfg_cb.pwrkey_num0];
    }
#endif // USER_PWRKEY

#if CHARGE_EN || VBAT_DETECT_EN
	adc_ch |= BIT(ADCCH_BGOP);
#if VBAT_DETECT_EN
    adc_ch |=  BIT(VBAT2_ADCCH);
#endif // VBAT_DETECT_EN
#if CHARGE_EN
    adc_ch |= BIT(ADCCH_VUSB);
#endif // CHARGE_EN
#endif

    plugin_saradc_init(&adc_ch);   //其他的ADC通道
    bsp_saradc_init(adc_ch);
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_init();
    }
#endif
    power_on_check();
}

#if USER_KEY_DOUBLE_EN
AT(.com_text.bsp.key)
bool check_key_return(u16 key_return)
{
    u8 i;
    if (key_return == NO_KEY) {
        return false;
    }

    //工具配置了哪些按键支持双击功能？
    i = check_key_double_configure(key_return);
    if (i > 0) {
        return (i - 1);
    }

    for (i=0; i<sizeof(tbl_double_key)/2; i++) {
        if (key_return == tbl_double_key[i]) {
            return true;
        }
    }
    return false;
}
#else
bool check_key_return(u16 key_return)
{
    return false;
}
#endif // USER_KEY_DOUBLE_EN

AT(.com_text.bsp.key)
void pwrkey10s_counter_clr(void)
{
    if (POWKEY_10S_RESET) {
        irtc_sfr_write(RTCCON8_CMD, 1);                 //clear pwrkey10s pending and counter
    }
}

AT(.text.bsp.key)
u8 get_pwroff_pressed_time(void)
{
    return PWROFF_PRESS_TIME;
}

AT(.text.bsp.key)
u8 get_double_key_time(void)
{
    if(DOUBLE_KEY_TIME > 7) {
        return 60;
    } else {
        return (u8)((u8)(DOUBLE_KEY_TIME + 2) * 20);
    }
}

AT(.com_text.bsp.key)
bool get_poweron_flag(void)
{
    return sys_cb.poweron_flag;
}

AT(.com_text.bsp.key)
void set_poweron_flag(bool flag)
{
    sys_cb.poweron_flag = flag;
}

#if USER_KEY_KNOB_EN
AT(.com_text.bsp.key)
void key_knob_process(u16 adc_val, const u8 *knob_level, u8 *key_val)
{
    u8 step = 0;
    if (s_abs((int)knob_level[*key_val] - (int)adc_val) > 4) {    //取绝对值消抖
        while (adc_val > knob_level[step]) {
            step++;
        }
        *key_val = step;
    }
}
#endif

#if USER_KEY_THRICE_EN
//按键三击检测
AT(.com_text.bsp.key)
u16 key_three_press_process(u16 key_val)
{
    key_thrice_t *s = &key_thrice_cb;
    u16 key_return = key_val;

    if (s->cnt) {
        s->cnt--;
    }
    if (s->cnt == 1) {
        return s->val;
    }

    if (((key_val & KEY_TYPE_MASK) == KEY_DOUBLE) && (!s->cnt)) {
        s->val = key_val;
        s->cnt = s->stable;
        key_return = NO_KEY;
    } else if ((s->cnt) && ((key_val & KEY_TYPE_MASK) == KEY_SHORT_UP) && ((key_val & 0xff) == (s->val & 0xff))) {
        s->cnt = 0;
        s->val = NO_KEY;
        //printf("thrice pressed\n");
        return (KEY_THREE | (key_val & 0xff));      //三击按键
    }
    return key_return;
}
#endif // USER_KEY_THRICE_EN

AT(.com_text.bsp.key)
u16 bsp_key_process(u16 key_val)
{
    u16 key_return = NO_KEY;

    key_return = key_process(key_val);

    //双击处理
#if USER_KEY_DOUBLE_EN
    //配置工具是否使能了按键双击功能？
    if (xcfg_cb.user_key_double_en) {
    #if USER_KEY_THRICE_EN
        if (!key_thrice_cb.cnt) {
            key_return = key_double_process(key_return);
        }
        key_return = key_three_press_process(key_return);
    #else
        key_return = key_double_process(key_return);
    #endif // USER_KEY_THRICE_EN
    }
#endif // USER_KEY_DOUBLE_EN

    return key_return;
}

AT(.com_text.bsp.key)
u16 bsp_reset_key_process(u16 key_msg, u8 key_val)
{
    u16 key_return = key_msg;
    u16 reset_key = key_config_table[xcfg_cb.reset_key_num];
	static uint32_t reset_key_timer;
	static bool		reset_state = 0;
	
	if (NO_KEY != key_val) {
		if (!xcfg_cb.reset_key_mode) {
			if ((key_msg == (reset_key|KEY_DOUBLE)) && !reset_state) {
				key_return = NO_KEY;
				reset_state = 1;
				msg_enqueue(EVT_SYS_FACTORY);
			}
		}
		else if (tick_check_expire(reset_key_timer, 1000*xcfg_cb.reset_key_mode)) {
			if (((key_msg&0xff) == reset_key) && !reset_state) {
				key_return = NO_KEY;
				reset_state = 1;
				msg_enqueue(EVT_SYS_FACTORY);
			}
		}
	}
	else {
		reset_key_timer = tick_get();
		reset_state = 0;
	}

    return key_return;
}

AT(.com_text.bsp.key)
u8 bsp_key_scan(void)
{
    u8 key_val = NO_KEY;
    u16 key = NO_KEY;

    if (!get_adc_val()) {
        return NO_KEY;
    }

#if USER_ADKEY
    key_val = get_adkey(adc_cb.key_val, xcfg_cb.user_adkey_en);
#endif // USER_ADKEY

#if USER_ADKEY2
    if (key_val == NO_KEY) {
        key_val = get_adkey2();
    }
#endif // USER_ADKEY2

#if USER_PWRKEY
    if (key_val == NO_KEY) {
        key_val = get_pwrkey();
    }
#endif // USER_PWRKEY

#if USER_IOKEY
    if (key_val == NO_KEY) {
        key_val = get_iokey();
    }
#endif // USER_IOKEY

#if VBAT_DETECT_EN
    sys_cb.vbat = get_vbat_val();
#endif // VBAT_DETECT_EN

#if (IRRX_SW_EN || IRRX_HW_EN)
    if (key_val == NO_KEY) {
        key_val = get_irkey();
    }
#endif // (IRRX_SW_EN || IRRX_HW_EN)

#if USER_ADKEY_MUX_SDCLK
    //需要放到最后处理,当没进行adc convert需要返回
    if (key_val == NO_KEY) {
        if (!adc_cb.sdclk_valid) {
            return NO_KEY;
        }
        key_val = get_adkey(adc_cb.sdclk_val, xcfg_cb.user_adkey_mux_sdclk_en);
    }
#endif // USER_ADKEY_MUX_SDCLK

    key = bsp_key_process(key_val);
	//恢复出厂设置按键逻辑转换。
    if (xcfg_cb.reset_factory_en) {
		bsp_reset_key_process(key, key_val);
    }
    
    if (key != NO_KEY) {
		printf("current enqueue key msg: %x\n", key);
        if ((key & KEY_TYPE_MASK) == KEY_LONG_UP) {
            msg_queue_detach(key | KEY_HOLD);       //长按抬键，先清掉HOLD按键消息
        }
        msg_enqueue(key);
    }
    return key_val;
}

