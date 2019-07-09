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
    uint32_t usbcon0, usbcon1,clkcon0,clkcon2,pwrcon0,rtccon5,rtccon4,temp,xo26mcon,clkgat0,clkgat1;
    u8 pa_de, gpioadir, gpioapu, gpioapd, gpioapu200k, gpioapd200k, gpioapu300, gpioapd300;
    u8 pb_de, gpiobdir, gpiobpu, gpiobpd, gpiobpu200k, gpiobpd200k, gpiobpu300, gpiobpd300;
    u8 pe_de, gpioedir, gpioepu, gpioepd, gpioepu200k, gpioepd200k, gpioepu300, gpioepd300;
    u8 pf_de, gpiofdir, gpiofpu, gpiofpd, gpiofpu200k, gpiofpd200k, gpiofpu300, gpiofpd300;
    u8 pg_de, gpiogdir, gpiogpu, gpiogpd, gpiogpu200k, gpiogpd200k, gpiogpu300, gpiogpd300;
	
    u16 adc_ch;
    u8 sys_clk, p111_en;
	uint32_t poweron_cnt = 0;
//    printf("%s\n", __func__);
	
	tm_cb.sleep_mode = 1;

	if ((tm_cb.alarm_poweron == 0) && (sys_cb.power_off_play_tone == 1))
	{
		gui_box_poweroff();
		mp3_res_play(RES_BUF_POWEROFF_MP3, RES_LEN_POWEROFF_MP3);
	}
	
    gui_off();
#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
#if SYS_KARAOK_EN
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif
    dac_power_off();                    //dac power down
    saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

	delay_ms(300);
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
//--------------------------------------------------------------	
//RC clk
	clkcon0 = CLKCON0;
	clkcon2 = CLKCON2;

    CLKCON0 |= BIT(0);                  //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));  //system clk select RC

	pwrcon0 = PWRCON0;
	PWRCON0 = (PWRCON0 & ~0x0f) | (0x05);
	PWRCON0 = (PWRCON0 & ~(0x07<<4)) | (0x04<<4);
	PWRCON0 &= ~BIT(8);
	PWRCON0 &= ~BIT(11);
	
	rtccon5 = irtc_sfr_read(RTCCON5_CMD);
	rtccon4 = irtc_sfr_read(RTCCON4_CMD);

	temp = irtc_sfr_read(RTCCON5_CMD) & ~(0x03<<2);
	temp |= (0x03<<2);
	irtc_sfr_write(RTCCON5_CMD, temp);		

	temp = irtc_sfr_read(RTCCON5_CMD) & ~(0x03<<4);
	temp |= (0x02<<4);
	irtc_sfr_write(RTCCON5_CMD, temp);	

	temp= irtc_sfr_read(RTCCON4_CMD) & ~(0x1f<<0);
	temp |= (0x0B<<0);
	irtc_sfr_write(RTCCON4_CMD, temp);	

	xo26mcon = XO26MCON;
	XO26MCON |= BIT(11);
	XO26MCON &= ~(0x7<<12);
	XO26MCON &= ~(0x7<<15);
	XO26MCON &= ~(0x7<<18);
	XO26MCON &= ~(0x7<<21);
	XO26MCON &= ~(0x7<<2);
	XO26MCON &= ~(0x3<<0);
	XO26MCON |= (0x3<<6);
	XO26MCON &= ~(0x3<<28);
	asm("nop"); asm("nop"); asm("nop");
	
	XO26MCON &= ~BIT(9);
	XO26MCON |= BIT(8);

	clkgat0 = CLKGAT0;
	clkgat1 = CLKGAT1;

	CLKGAT0  &= ~BIT(11);

#if 0
	XO26MCON &= ~(0xf<<24); 			//X26 output DIS
	XO26MCON &= ~BIT(10);				//X26 DIS
	XO26MCON &= ~BIT(9);				//X26 LDO DIS
#endif
//--------------------------------------------------------------	
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

	gpioadir = GPIOADIR;
	gpioapu  = GPIOAPU;
	gpioapd  = GPIOAPD;
	gpioapu200k = GPIOAPU200K;
	gpioapd200k = GPIOAPD200K;
	gpioapu300  = GPIOAPU300;
	gpioapd300  = GPIOAPD300;

	gpiobdir = GPIOBDIR;
	gpiobpu  = GPIOBPU;
	gpiobpd  = GPIOBPD;
	gpiobpu200k = GPIOBPU200K;
	gpiobpd200k = GPIOBPD200K;
	gpiobpu300  = GPIOBPU300;
	gpiobpd300  = GPIOBPD300;

	gpioedir = GPIOEDIR;
	gpioepu  = GPIOEPU;
	gpioepd  = GPIOEPD;
	gpioepu200k = GPIOEPU200K;
	gpioepd200k = GPIOEPD200K;
	gpioepu300  = GPIOEPU300;
	gpioepd300  = GPIOEPD300;

	gpiofdir = GPIOFDIR;
	gpiofpu  = GPIOFPU;
	gpiofpd  = GPIOFPD;
	gpiofpu200k = GPIOFPU200K;
	gpiofpd200k = GPIOFPD200K;
	gpiofpu300  = GPIOFPU300;
	gpiofpd300  = GPIOFPD300;

	gpiogdir = GPIOGDIR;
	gpiogpu  = GPIOGPU;
	gpiogpd  = GPIOGPD;
	gpiogpu200k = GPIOGPU200K;
	gpiogpd200k = GPIOGPD200K;
	gpiogpu300  = GPIOGPU300;
	gpiogpd300  = GPIOGPD300;

	
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F;     //MCP FLASH

	GPIOADIR = 0xFF;
	GPIOAPU  = 0x00;
	GPIOAPD  = 0x00;
	GPIOAPU200K = 0x00;
	GPIOAPD200K = 0xFF;
	GPIOAPU300  = 0x00;
	GPIOAPD300  = 0x00;

	GPIOBDIR = 0xFF;
	GPIOBPU  = 0x00;
	GPIOBPD  = 0x00;
	GPIOBPU200K = 0x00;
	GPIOBPD200K = 0xFF;
	GPIOBPU300  = 0x00;
	GPIOBPD300  = 0x00;

	GPIOEDIR = 0xFF;
	GPIOEPU  = 0x00;
	GPIOEPD  = 0x00;
	GPIOEPU200K = 0x00;
	GPIOEPD200K = 0xFF;
	GPIOEPU300  = 0x00;
	GPIOEPD300  = 0x00;

	GPIOFDIR = 0xFF;
	GPIOFPU  = 0x00;
	GPIOFPD  = 0x00;
	GPIOFPU200K = 0x00;
	GPIOFPD200K = 0xFF;
	GPIOFPU300  = 0x00;
	GPIOFPD300  = 0x00;

	GPIOGDIR |= 0xC0;
	GPIOGPU  &= ~0xC0;
	GPIOGPD  &= ~0xC0;
	GPIOGPU200K &= ~0xC0;
	GPIOGPD200K |= 0xC0;
	GPIOGPU300  &= ~0xC0;
	GPIOGPD300  &= ~0xC0;

#if PERIPHERAL_POWER_ONOFF_PIN
	gpio_t user_per_io1;

	bsp_gpio_cfg_init(&user_per_io1, PERIPHERAL_POWER_ONOFF_PIN);
	user_per_io1.sfr[GPIOxFEN] &= ~BIT(user_per_io1.num);
	user_per_io1.sfr[GPIOxDE]  |= BIT(user_per_io1.num);
	user_per_io1.sfr[GPIOxDIR] |= BIT(user_per_io1.num);
	user_per_io1.sfr[GPIOxPD200K] &= ~BIT(user_per_io1.num);
#endif

    wakeup_disable();
    //sleep_wakeup_config();                  //按键唤醒配置
    wakeup_wko_config();
    WDT_DIS();
	
	{
        RTCCPND = BIT(18);                      //clear rtc1s pending
        asm("nop");
        asm("nop");
        asm("nop");
        RTCCON |= BIT(7);                       //RTC 1S wakeup enable
	}
	
	tm_cb.alarm_poweron = 0;
	while(1)
	{
		LPMCON |= BIT(0);                   //Sleep mode enable
		asm("nop");asm("nop");asm("nop");

		//io wakeup
		if (wakeup_get_status())
		{
			poweron_cnt = 0;
			while(IS_PWRKEY_PRESS())
			{
				asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
				asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
				asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
				poweron_cnt++;
				if(poweron_cnt >= 0x4000)
				{
					break;
				}
			}

			if(poweron_cnt >= 0x4000)
			{
				break;
			}
		}
		wakeup_wko_config();

		if (RTCCON & BIT(18))
		{
			RTCCPND = BIT(18);			   //clear rtc1s pending
		}

		if (RTCCON & BIT(17))
		{
			if(tm_cb.alarm_type != 0)
			{
				tm_cb.alarm_time_out = 1;
				tm_cb.alarm_poweron  = 1;
				tm_cb.alarm_play_cnt = 0;
				func_cb.last = FUNC_BT;
				break;
			}
			else
			{
				RTCCPND = BIT(17);						//clear RTC alarm pending
				irtc_sfr_write(RTCCON8_CMD, 0x02);		//clear alarm pending
			}
		}

		{
			GPIOBDE |= BIT(1);
			GPIOBDIR |= BIT(1);
			GPIOBPU &= ~BIT(1);
			GPIOBPD |= BIT(1);

			if (GPIOB & BIT(1))
			{
				sys_cb.power_off_dc_in = 1;
				break;
			}
		}
	}

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOEDE = pe_de;
    GPIOFDE = pf_de;
    GPIOGDE = pg_de;

	GPIOADIR = gpioadir;
	GPIOAPU  = gpioapu;
	GPIOAPD  = gpioapd;
	GPIOAPU200K = gpioapu200k;
	GPIOAPD200K = gpioapd200k;
	GPIOAPU300  = gpioapu300;
	GPIOAPD300  = gpioapd300;

	GPIOBDIR = gpiobdir;
	GPIOBPU  = gpiobpu;
	GPIOBPD  = gpiobpd;
	GPIOBPU200K = gpiobpu200k;
	GPIOBPD200K = gpiobpd200k;
	GPIOBPU300  = gpiobpu300;
	GPIOBPD300  = gpiobpd300;

	GPIOEDIR = gpioedir;
	GPIOEPU  = gpioepu;
	GPIOEPD  = gpioepd;
	GPIOEPU200K = gpioepu200k;
	GPIOEPD200K = gpioepd200k;
	GPIOEPU300  = gpioepu300;
	GPIOEPD300  = gpioepd300;

	GPIOFDIR = gpiofdir;
	GPIOFPU  = gpiofpu;
	GPIOFPD  = gpiofpd;
	GPIOFPU200K = gpiofpu200k;
	GPIOFPD200K = gpiofpd200k;
	GPIOFPU300  = gpiofpu300;
	GPIOFPD300  = gpiofpd300;

	GPIOGDIR = gpiogdir;
	GPIOGPU  = gpiogpu;
	GPIOGPD  = gpiogpd;
	GPIOGPU200K = gpiogpu200k;
	GPIOGPD200K = gpiogpd200k;
	GPIOGPU300  = gpiogpu300;
	GPIOGPD300  = gpiogpd300;

    wakeup_disable();

//--------------------------------------------------------------	
	XO26MCON = xo26mcon;
	CLKCON0 = clkcon0;
	CLKCON2 = clkcon2;
    delay_us(100);
	CLKGAT0 = clkgat0;
	CLKGAT1 = clkgat1;
	irtc_sfr_write(RTCCON5_CMD, rtccon5);	
	irtc_sfr_write(RTCCON4_CMD, rtccon4);	
	PWRCON0 = pwrcon0;
//--------------------------------------------------------------	
    WDT_EN();
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
    //gui_display(DISP_POWERON);
    func_clock_enter_display();
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
#if SYS_KARAOK_EN
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_cb.sta);
#endif
	msg_queue_clear();
#if PERIPHERAL_POWER_ONOFF_PIN
	gpio_t user_per_io;

	bsp_gpio_cfg_init(&user_per_io, PERIPHERAL_POWER_ONOFF_PIN);
	user_per_io.sfr[GPIOxFEN] &= ~BIT(user_per_io.num);
	user_per_io.sfr[GPIOxDE]  |= BIT(user_per_io.num);
	user_per_io.sfr[GPIOxDIR] &= ~BIT(user_per_io.num);
	user_per_io.sfr[GPIOxCLR] = BIT(user_per_io.num);
#endif
	ledseg_7p3s_HBS7642_on();
	
	sys_cb.spoweron = 1;
	if (sys_cb.power_off_dc_in == 1)
	{
		func_cb.sta = FUNC_IDLE;
	}
	else
	{
		func_cb.sta = func_cb.last;
		func_cb.last = FUNC_NULL;
	}
	tm_cb.sleep_mode = 0;
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

	if (SOFT_POWER_ON_OFF)
	{
		while (IS_PWRKEY_PRESS())
		{ //等待PWRKWY松开
			delay_5ms(1);
			WDT_CLR();
		}
	#if SYS_KARAOK_EN
		bsp_karaok_exit(AUDIO_PATH_KARAOK);
	#endif
		dac_power_off();                    //dac power down
		if (CHARGE_DC_IN())
		{
			if (power_off_check())
			{
				return;
			}
		}
		saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

		sfunc_pwrdown();
	}
	else
	{
	#if SYS_KARAOK_EN
		bsp_karaok_exit(AUDIO_PATH_KARAOK);
	#endif
		dac_power_off();                    //dac power down
		saradc_exit(adc_cb.channel);        //close saradc及相关通路模拟

		sfunc_lowbat();                     //低电关机进入Sleep Mode
	}
}

