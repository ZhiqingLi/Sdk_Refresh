#include "include.h"
#include "func.h"

bool power_off_check(void);
void lock_code_pwrsave(void);
void unlock_code_pwrsave(void);

AT(.com_text.sleep)
void lowpwr_tout_ticks(void)
{
    if(sys_cb.sleep_delay != -1L && sys_cb.sleep_delay > 0) {
        sys_cb.sleep_delay--;
    }
    if(sys_cb.pwroff_delay != -1L && sys_cb.pwroff_delay > 0) {
        sys_cb.pwroff_delay--;
    }
}

AT(.com_text.sleep)
bool sys_sleep_check(u32 *sleep_time)
{
    if(*sleep_time > sys_cb.sleep_wakeup_time) {
        *sleep_time = sys_cb.sleep_wakeup_time;
        return true;
    }
    return false;
}

//休眠定时器，500ms进一次
AT(.sleep_text.sleep)
uint32_t sleep_timer(void)
{
    uint32_t ret = 0;
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_detect(0);
    }
#endif // CHARGE_EN
    if (led_bt_sleep()) {
        sys_cb.sleep_wakeup_time = 50000/625;

#if VBAT_DETECT_EN
        bsp_saradc_init(adc_cb.channel);
        if ((xcfg_cb.lowpower_warning_en) && (sys_cb.vbat < ((u16)LPWR_WARNING_VBAT*100 + 2750))) {
            //低电需要唤醒sniff mode
            ret = 2;
        }
        saradc_exit(adc_cb.channel);
#endif // VBAT_DETECT_EN

#if CHARGE_EN
        if (!port_2led_is_sleep_en()) {
            ret = 2;
        }
#endif // CHARGE_EN
    } else {
        sys_cb.sleep_wakeup_time = -1L;
    }

    if(sys_cb.pwroff_delay != -1L) {
        if(sys_cb.pwroff_delay > 5) {
            sys_cb.pwroff_delay -= 5;
        } else {
            sys_cb.pwroff_delay = 0;
            return 1;
        }
    }
    return ret;
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint32_t status, ret;
    uint32_t usbcon0, usbcon1;
    u8 pa_de, pb_de, pe_de, pf_de, pg_de;
    u8 p111_en;
    u16 adc_ch;

#if VBAT_DETECT_EN
    if (is_lowpower_vbat_warning()) {   //低电不进sniff mode
        return;
    }
#endif // VBAT_DETECT_EN

    printf("%s\n", __func__);

#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    bt_enter_sleep();

    gui_off();
    led_off();
    rled_off();

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

    if (SLEEP_DAC_OFF_EN) {
        dac_power_off();                //dac power down
    } else {
        dac_channel_disable();          //only dac channel master disable
    }

    sys_set_tmr_enable(0, 0);

    saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟
    adc_ch = adc_cb.channel;
    adc_cb.channel = BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP);

#if CHARGE_EN
    charge_set_stop_time(3600);
#endif
    if (dev_is_online(DEV_UDISK)) {
        udisk_insert();                 //udisk需要重新enum
    }
    usbcon0 = USBCON0;                  //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;

    //close pll0
    PLL0CON &= ~BIT(18);                //pll10 sdm enable disable
    PLL0CON &= ~BIT(6);                 //pll0 analog diable
    PLL0CON &= ~BIT(12);                //pll0 ldo disable

    //close pll1
    p111_en = pll1_disable();

    //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pe_de = GPIOEDE;
    pf_de = GPIOFDE;
    pg_de = GPIOGDE;
    GPIOADE = 0;
    GPIOBDE = 0;
#if CHARGE_EN
    if (CHARGE_DC_IN() && (xcfg_cb.rled_io_sel == IO_PE0)) {
        GPIOEDE = BIT(0);   //PE0做充电指示灯，保持IO数字输出
    } else {
        GPIOEDE = 0;
    }
#else
    GPIOEDE = 0;
#endif // CHARGE_EN
    GPIOFDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH

    wakeup_disable();
    sleep_wakeup_config();

    sys_cb.sleep_wakeup_time = -1L;
    while(bt_is_sleep()) {
        WDT_CLR();
        status = bt_sleep_proc();

        if(status == 1) {
            ret = sleep_timer();
            if(ret) {
                if (ret == 1) {
                    sys_cb.pwrdwn_tone_en = 1;
                    func_cb.sta = FUNC_PWROFF;
                }
                break;
            }
        }

        if (wakeup_get_status()) {
            break;
        }
    }

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOEDE = pe_de;
    GPIOFDE = pf_de;
    GPIOGDE = pg_de;

    wakeup_disable();
    if (p111_en) {
        pll1_enable();
    }
    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

    //enable adpll
    PLL0CON |= BIT(12);                //PLL1 LDO enable
    delay_us(100);
    PLL0CON |= BIT(18);                //PLL1 SDM enable
    PLL0CON |= BIT(6);                 //PLL1 Analog enable

    adc_cb.channel = adc_ch;
    bsp_saradc_init(adc_cb.channel);
#if CHARGE_EN
    charge_set_stop_time(18000);
    charge_disp_sta(sys_cb.charge_sta);      //update充灯状态
#endif // CHARGE_EN
    sys_set_tmr_enable(1, 1);

    if (DAC_FAST_SETUP_EN) {
        bsp_loudspeaker_mute();
        delay_5ms(2);
    }

    if (SLEEP_DAC_OFF_EN) {
        dac_restart();
    } else {
        dac_channel_enable();
    }
    bsp_change_volume(sys_cb.vol);
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
    bt_exit_sleep();
#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
#endif
    printf("sleep_exit\n");
}

AT(.text.lowpwr.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if ((*is_sleep)()) {
        if ((!sys_cb.sleep_en) || (!port_2led_is_sleep_en())) {
            reset_sleep_delay();
            return false;
        }
        if(sys_cb.sleep_delay == 0) {
            sfunc_sleep();
            reset_sleep_delay();
            reset_pwroff_delay();
            return true;
        }
    } else {
        reset_sleep_delay();
        reset_pwroff_delay();
    }
    return false;
}

//红外关机进入sleep mode
ALIGNED(512)
AT(.text.lowpwr.sleep)
void func_sleepmode(void)
{
    uint32_t usbcon0, usbcon1;
    u8 pa_de, pb_de, pe_de, pf_de, pg_de;
    u16 adc_ch;
    u8 sys_clk, p111_en;
//    printf("%s\n", __func__);

    gui_display(DISP_OFF);
#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    dac_power_off();                    //dac power down
    saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

    gui_off();
    led_off();
    rled_off();
    sys_clk = get_cur_sysclk();
    set_sys_clk(SYS_26M);
    sys_set_tmr_enable(0, 0);

    adc_ch = adc_cb.channel;
    adc_cb.channel = (ADCCH_VBAT|ADCCH_BGOP);

    if (dev_is_online(DEV_UDISK)) {
        udisk_insert();                 //udisk需要重新enum
    }
    usbcon0 = USBCON0;                  //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;

    //close pll0
    PLL0CON &= ~BIT(18);                //pll10 sdm enable disable
    PLL0CON &= ~BIT(6);                 //pll0 analog diable
    PLL0CON &= ~BIT(12);                //pll0 ldo disable

    //close pll1
    p111_en = pll1_disable();

    //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pe_de = GPIOEDE;
    pf_de = GPIOFDE;
    pg_de = GPIOGDE;
    GPIOADE = 0;
    GPIOBDE = 0;
#if CHARGE_EN
    if (CHARGE_DC_IN() && (xcfg_cb.rled_io_sel == IO_PE0)) {
        GPIOEDE = BIT(0);   //PE0做充电指示灯，保持IO数字输出
    } else {
        GPIOEDE = 0;
    }
#else
    GPIOEDE = 0;
#endif // CHARGE_EN
    GPIOFDE = 0;
    GPIOGDE = 0x3F;     //MCP FLASH

#if IRRX_HW_EN
    IR_CAPTURE_PORT();
#endif // IRRX_HW_EN
    wakeup_disable();
//    sleep_wakeup_config();                  //按键唤醒配置
    WDT_DIS();

#if CHARGE_EN
    charge_set_stop_time(1800);
    if (xcfg_cb.charge_en) {
        RTCCPND = BIT(18);                      //clear rtc1s pending
        asm("nop");
        asm("nop");
        asm("nop");
        RTCCON |= BIT(7);                       //RTC 1S wakeup enable
    }
#endif

    while(1) {
        LPMCON |= BIT(0);                   //Sleep mode enable
        asm("nop");asm("nop");asm("nop");

#if IRRX_HW_EN
        if (get_irkey() == KEY_IR_POWER) {
            ir_key_clr();
            break;
        }
#endif // IRRX_HW_EN

#if CHARGE_EN
        if (xcfg_cb.charge_en) {
            if (RTCCON & BIT(18)) {
                RTCCPND = BIT(18);             //clear rtc1s pending
            }
            charge_detect(0);
        }
#endif // CHARGE_EN

//        //io wakeup
//        if (wakeup_get_status()) {
//            break;
//        }
    }

    WDT_EN();

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOEDE = pe_de;
    GPIOFDE = pf_de;
    GPIOGDE = pg_de;

    wakeup_disable();

    //enable adpll
    PLL0CON |= BIT(12);                //PLL1 LDO enable
    delay_us(100);
    PLL0CON |= BIT(18);                //PLL1 SDM enable
    PLL0CON |= BIT(6);                 //PLL1 Analog enable
    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

    set_sys_clk(sys_clk);              //还原系统时钟
    if (p111_en) {
        pll1_enable();
    }

    adc_cb.channel = adc_ch;
    bsp_saradc_init(adc_cb.channel);
#if CHARGE_EN
    charge_set_stop_time(18000);
#endif // CHARGE_EN
    sys_set_tmr_enable(1, 1);

    if (DAC_FAST_SETUP_EN) {
        bsp_loudspeaker_mute();
        delay_5ms(2);
    }
    dac_restart();
    bsp_change_volume(sys_cb.vol);
    gui_display(DISP_POWERON);
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
#endif
    func_cb.sta = func_cb.last;
    func_cb.last = FUNC_NULL;
}

//硬开关方案，低电时，进入省电状态
ALIGNED(512)
AT(.text.lowpwr.pwroff)
void sfunc_lowbat(void)
{
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_off();
    }
#endif // CHARGE_EN
    WDT_DIS();

    USBCON0 = BIT(5);
    USBCON1 = 0;
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH

    CLKCON0 |= BIT(0);                  //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));  //system clk select RC

    PLL0CON &= ~BIT(13);                //PLL0 SDM Dis
    PLL0CON &= ~BIT(6);                 //PLL0 Analog Dis
    PLL0CON &= ~BIT(12);                //PLL0 LDO DIS

    XO26MCON &= ~(0xf<<24);             //X26 output DIS

    XO26MCON &= ~BIT(10);               //X26 DIS
    XO26MCON &= ~BIT(9);                //X26 LDO DIS

    LPMCON |= BIT(0);                   //Sleep mode enable
    asm("nop");asm("nop");asm("nop");
    while(1);
}

//软开关方案，POWER键/低电时，进入关机状态
ALIGNED(512)
AT(.text.lowpwr.pwroff)
void sfunc_pwrdown(void)
{
    LOUDSPEAKER_MUTE_DIS();
    WDT_DIS();
#if USER_PWRKEY_EXT
    PWRKEY_EXT_POWEROFF();
#endif
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_off();
    }
#endif // CHARGE_EN

    CLKCON0 |= BIT(0);                  //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));  //system clk select RC

    PLL0CON &= ~BIT(13);                //PLL0 SDM Dis
    PLL0CON &= ~BIT(6);                 //PLL0 Analog Dis
    PLL0CON &= ~BIT(12);                //PLL0 LDO DIS

    XO26MCON &= ~(0xf<<24);             //X26 output DIS

    XO26MCON &= ~BIT(10);               //X26 DIS
    XO26MCON &= ~BIT(9);                //X26 LDO DIS
    uint rtccon0 = irtc_sfr_read(RTCCON0_CMD);
    uint rtccon3 = irtc_sfr_read(RTCCON3_CMD);
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        rtccon3 |= BIT(5);
    }
#endif

    irtc_sfr_write(RTCCON3_CMD, rtccon3 & ~BIT(4));  //关WK PIN，再打开，以清除Pending

    irtc_sfr_write(RTCCON0_CMD, rtccon0 & ~0x04);    //只关LVD，保留RTCRAM与RTC

    rtccon3 &= ~0x07;                   //关VDDCORE VDDIO VDDBUCK
    rtccon3 |= BIT(4);                  //WK pin wake up enable

    RTCCON |= BIT(5);                   //PowerDown Reset，如果有Pending，则马上Reset
    irtc_sfr_write(RTCCON3_CMD, rtccon3);
    LPMCON |= BIT(0);
    asm("nop");asm("nop");asm("nop");
    while (1);
}

AT(.text.lowpwr.pwroff)
void func_pwroff(int pwroff_tone_en)
{
    printf("%s\n", __func__);
    led_power_down();
#if WARNING_POWER_OFF
    if (SOFT_POWER_ON_OFF) {
        if (pwroff_tone_en == 1) {
            mp3_res_play(RES_BUF_POWEROFF_MP3, RES_LEN_POWEROFF_MP3);
        } else if (pwroff_tone_en == 2) {
#if WARNING_FUNC_AUX
            mp3_res_play(RES_BUF_AUX_MODE_MP3, RES_LEN_AUX_MODE_MP3);
#endif // WARNING_POWER_OFF
        }
    }
#endif // WARNING_POWER_OFF

    gui_off();

    if (SOFT_POWER_ON_OFF) {
        while (IS_PWRKEY_PRESS()) { //等待PWRKWY松开
            delay_5ms(1);
            WDT_CLR();
        }
#if SYS_KARAOK_EN
        bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
        dac_power_off();                    //dac power down
        if (CHARGE_DC_IN()) {
            if (power_off_check()) {
                return;
            }
        }
        saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

        sfunc_pwrdown();
    } else {
#if SYS_KARAOK_EN
        bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
        dac_power_off();                    //dac power down
        saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

        sfunc_lowbat();                     //低电关机进入Sleep Mode
    }
}


//-----------------------------------------
#if 0   //DEBUG
#define DEGIO_1()  {GPIOFFEN &= ~BIT(5); GPIOFDE |= BIT(5); GPIOFDIR &= ~BIT(5); GPIOFSET = BIT(5);}
#define DEGIO_0()  {GPIOFFEN &= ~BIT(5); GPIOFDE |= BIT(5); GPIOFDIR &= ~BIT(5); GPIOFCLR = BIT(5);}
#define DEG_IO(X);  {while(1) {WDT_CLR() ;DEGIO_1();delay_us(X);DEGIO_0();delay_us(X);}}
#define DEG_N(X);  {delay_us(5000*3);  u32 n = X;  while(n--) {WDT_CLR() ;DEGIO_1();delay_us(5000);DEGIO_0();delay_us(5000);}}
#else
#define DEGIO_1()
#define DEGIO_0()
#define DEG_IO(...);
#define DEG_N(...);
#endif

#if CHARGE_FINISH_INTO_LOWPOWER

#define RC_100MS  4608L
#define CHARGE_FULL_POWER_ON_TIME      0XFFFFFFFFL   //长期开机   //(8*60*1000/200)/*充满电后8分钟内拿起可以开机*/
void charge_stop(u8 mode);

AT(.text.bsp.power)
void sys_clk_seclet_RC(u32 *buf)
{
    buf[0] = get_cur_sysclk();
    buf[1] = CLKCON0;
    CLKCON0  &= ~BIT(28);                //select saradc_clk  1: x26m_clkdiv4  //0 rc clk
    CLKCON0 |= BIT(0);                  //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));  //system clk select RC

    XO26MCON &= ~(0xf<<24);             //X26 output DIS

    XO26MCON &= ~BIT(10);               //X26 DIS
    XO26MCON &= ~BIT(9);                //X26 LDO DIS
}

AT(.text.bsp.power)
void sys_clk_restore(u32 *buf)
{
    XO26MCON |= BIT(9);                //X26 LDO en
    delay_us(100);
    XO26MCON |= BIT(10);               //X26 en
    XO26MCON |= (0xf<<24);             //X26 output en

    delay_us(100);
    CLKCON0 = buf[1];
    asm("nop");asm("nop");asm("nop");
    set_sys_clk(buf[0]);
}


AT(.pwrsave_text.func)
u8 charge_finish_into_lowpower_check(bool stop_charge)   //程序进入RC低功耗, 如果检测到DC_OUT, 则关机.
{
    printf("charge_finish_into_lowpower_check = %d\n",stop_charge);
    u32 dc_out_cnt, dc_in_cnt,charge_full_cnt,dc_out_power_on_cnt;
    u32 io_ade, io_bde, io_ede, io_fde,io_gde,gie;
    u32 io_fpu,io_fpd, io_fpd200k, io_fpu200k,io_fpd300, io_fpu300;
    u32 save_buf[10];
    WDT_DIS();
    saradc_exit(adc_cb.channel);    //close saradc及相关通路模拟

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (stop_charge) {
        charge_stop(0);    //关充电.
        CHAGCON |= BIT(5);
    }

    lock_code_pwrsave();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    gie =  PICCON & BIT(0);
    PICCONCLR = BIT(0);                 //Globle IE disable
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    io_ade = GPIOADE;
    io_bde = GPIOBDE;
    io_ede = GPIOEDE;
    io_fde = GPIOFDE;
    io_fpu = GPIOFPU;
    io_fpd = GPIOFPD;
    io_fpd200k = GPIOFPD200K;
    io_fpu200k = GPIOFPU200K;
    io_fpd300 = GPIOFPD300;
    io_fpu300 = GPIOFPU300;
    io_gde = GPIOGDE;
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOFPU = 0;
    GPIOFPD = 0;
    GPIOFPD200K = 0;
    GPIOFPU200K = 0;
    GPIOFPD300 = 0;
    GPIOFPU300 = 0;
    GPIOGDE = BIT(2);       //SPICS

    charge_into_lowpower(save_buf);

    dc_out_cnt = 0;
    dc_in_cnt = 0;
    charge_full_cnt = 0;
    while(1) {
        DEG_N(1);
        delay_us(RC_100MS* 2);  //200ms
        charge_full_cnt++;
        if (CHARGE_DC_IN()) {
            dc_in_cnt++;
            dc_out_cnt = 0;
        } else {
            dc_in_cnt = 0;
            dc_out_cnt++;
        }

        if (dc_in_cnt >= 3) {  //有DC插入, 不断检测电池电压,如果电池电压低于3.9V,则复位继续充电.
            if (bat_lowpower_recharge(3900)) {
                WDT_RST();     //看门复位, 继续充电.
            }
        }

        if (charge_full_cnt < (xcfg_cb.charge_full_bled_on_time*5)) {   //充满电后1分钟内亮绿灯
            LED_SET_ON();
        }else {
            LED_SET_OFF();
        }
        if (dc_out_cnt >= 2) {
#if  CHARGE_OUT_POWER_ON
            //检测到DC拔出, 则开机.
            if (CHARGE_OUT_2_POWERON == xcfg_cb.charge_warehouse_set) {  //充满电后前 CHARGE_FULL_POWER_ON_TIME 时间内拿起可以开机.
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                DEG_N(5);
                dc_out_power_on_cnt = 0;
               while(1)
                {
                    delay_us(RC_100MS* 2);  //200ms
                    if(check_vusb_voltage(4000)){  //VUSB拔出开机
                        DEG_N(2);
                        break;
                    }else{
                        dc_out_power_on_cnt++;
                    }
                    if(dc_out_power_on_cnt >= 10){
                        DEG_N(4);
                        charge_lowpower_power_down();     //VUSB电压大于2V，仍然在充电仓内，关机
                    }
                }
                DEG_N(3);
                GPIOADE = io_ade;
                GPIOBDE = io_bde;
                GPIOEDE = io_ede;
                GPIOFDE = io_fde;
                GPIOFPU = io_fpu;
                GPIOFPD = io_fpd;
                GPIOFPD200K = io_fpd200k;
                GPIOFPU200K = io_fpu200k;
                GPIOFPD300 = io_fpd300;
                GPIOFPU300 = io_fpu300;
                GPIOGDE = io_gde;
                charge_outof_lowpower(save_buf);
                bsp_saradc_init(adc_cb.channel);
                PICCON |= gie;
                DEG_N(1);
                unlock_code_pwrsave();
                return 1;
            } else
#endif
            {
                DEG_N(2);
                charge_lowpower_power_down();  //DC拔出, 或DC电压低于4V, 关机.
            }
        }
    }

    unlock_code_pwrsave();
    return 0;
}
#endif   //CHARGE_FINISH_INTO_LOWPOWER



