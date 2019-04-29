#include "driver_beken_includes.h"
#include "app_beken_includes.h"

void disable_audio_ldo(void);

__inline void clear_bit(volatile uint32 *p,uint32 count,...)
{
    uint32 val;
    uint8 i = 0;
    int bit_no;
    va_list args;

    va_start(args, count);

    val = 0;
    for(;i < count;i++)
    {
        bit_no = va_arg(args,int);
        val |= (1 << bit_no);
    }

    (*p) &= (~(val));

    va_end(args);
}

__inline void set_bit(volatile uint32 *p,uint32 count,...)
{
    uint32 val;
    uint8 i = 0;
    int bit_no;
    va_list args;

    va_start(args, count);

    val = 0;
    for(;i < count;i++)
    {
        bit_no = va_arg(args,int);
        val |= (1 << bit_no);
    }

    (*p) |= val;

    va_end(args);
}

#if 0
void ldo_enable(void)
{
	BK3000_A3_CONFIG |= sft_GPIO_LDO_en;
}

void ldo_disable(void)
{
	BK3000_A3_CONFIG &= ~sft_GPIO_LDO_en;
}
#endif

static volatile boolean b_26m_clock_closed   = FALSE;
static volatile u_int32 backup_pmu_peri_pwds = 0XFFFFFFFF;
static u_int8 s_ana_dac_open = 0;
RAM_CODE void BK3000_Ana_Decrease_Current(u_int8 enable)
{

}
RAM_CODE void Close_26M_Clock(void)
{
    u_int32 cpu_flags, mask;
    //uint8 i=0;
#if 0
    app_env_handle_t  env_h = app_env_get_handle();
#endif

    if (b_26m_clock_closed)
        return;

    SYSirq_Disable_Interrupts_Save_Flags(&cpu_flags, &mask);

    CLEAR_SLEEP_TICK;
#if 0
    os_printf("--------------------------------------\r\n");
    os_printf("|RegA0:%08x\r\n",BK3000_A0_CONFIG);
    os_printf("|RegA1:%08x\r\n",BK3000_A1_CONFIG);
    os_printf("|RegA2:%08x\r\n",BK3000_A2_CONFIG);
    os_printf("|RegA3:%08x\r\n",BK3000_A3_CONFIG);
    os_printf("|RegA4:%08x\r\n",BK3000_A4_CONFIG);
    os_printf("|RegA5:%08x\r\n",BK3000_A5_CONFIG);
    os_printf("|RegA6:%08x\r\n",BK3000_A6_CONFIG);
    os_printf("|RegA7:%08x\r\n",BK3000_A7_CONFIG);
    for(i=0;i<16;i++)
        os_printf("|xvr[%d]:%08x\r\n",i,XVR_analog_reg_save[i]);
    os_printf("--------------------------------------\r\n");
#endif
    /* Step 1: close BT xvr */
    BK3000_XVR_REG_0x03 = XVR_analog_reg_save[3] & ~(0x01 << 0);
    BK3000_XVR_REG_0x09 = 0x2BFFF304;
    backup_pmu_peri_pwds = BK3000_PMU_PERI_PWDS;

    if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON)
        && app_env_check_sniff_mode_Enable())
    {
        BK3000_A6_CONFIG |= (1 << 0);
    }

    /* Step 2: close audio */
    BK3000_Ana_Dac_enable(0);

    /* Step 3: set cpu clk = xtal 26M */
    BK3000_set_clock(1, 0);
             
    /* Step 4: set flash parameters */
    set_flash_clk(FLASH_CLK_26mHz);      
    flash_set_line_mode(FLASH_LINE_2);

    /* Step 5: power down peripheral clock*/
#ifdef CONFIG_PWM_NOT_SLEEP
    extern uint8 g_sniff_flag;
    if(g_sniff_flag == 2) // enter sniff mode
    {
        BK3000_PMU_PERI_PWDS |= 0x00703FFF;  
    }
    else
        BK3000_PMU_PERI_PWDS |= 0x00703BFF;  /* if have on connections,open pwm timer for LED */
#else
    BK3000_PMU_PERI_PWDS |= 0x00703FFF;  
#endif

    /* Step 6: Optimizaiotn BUCK */
#if 0
    if( env_h->env_cfg.used == 0x01 )
    {
        char enable;	
        enable = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_BUCK_ENABLE) ? 1:0;

        if(enable)
        {
#if 0
        	/* 20160808 */
	        BK3000_A1_CONFIG  = (BK3000_A1_CONFIG & (~(0x1<<22)));
        	BK3000_A2_CONFIG  = (BK3000_A2_CONFIG & (~(0x1<<22)));
        	BK3000_A1_CONFIG |= (1 << 28);
#endif
            GPIO_Ax.a1->bustconspiEn = 0;// burst mode controlled by digital(auto)
            GPIO_Ax.a5->spiselDigLDO30 = 0x06;
        }
        else
        {
            GPIO_Ax.a5->spiselDigLDO30 = 0x07;
        }
    }
    else
    {
#if ( SYS_CFG_BUCK_ON == 1)
#if 0
        BK3000_A1_CONFIG  = (BK3000_A1_CONFIG & (~(0x1<<22)));
    	BK3000_A2_CONFIG  = (BK3000_A2_CONFIG & (~(0x1<<22)));
    	BK3000_A1_CONFIG |= (1 << 28);
#endif
        GPIO_Ax.a1->bustconspiEn = 0;// burst mode controlled by digital(auto)
        GPIO_Ax.a5->spiselDigLDO30 = 0x06;
#else
        GPIO_Ax.a5->spiselDigLDO30 = 0x07;
#endif
    }
#endif

    /* Step 7: close saradc */
    #if 0
    GPIO_Ax.a3->spipwdSarADCA = 1;
    GPIO_Ax.a3->spipwdSarADCBuffer = 1;
    GPIO_Ax.a3->spipwdSARADCLDO = 1;
    #endif

    /* Step 8: close RF clk/ldo and DCO */
    BK3000_A0_CONFIG |= (1<<29);   //xtal buff off;
    //BK3000_A0_CONFIG &= ~(1<<20);   //bt ldo off; 
    BK3000_A0_CONFIG |= (1<<26);  // DCO power off;
    BK3000_A0_CONFIG &= ~(1<<15);  // cbias off
    BK3000_A1_CONFIG |= (1<<31);   // BUCKA pfm

    /* Step 9: setting for GPIO wakeup */
#if 0
#if ( BUTTON_DETECT_IN_SNIFF == 1)
    GPIO_Ax.a3->spipwdrsc100K  = 0;    
#else
	GPIO_Ax.a3->spipwdrsc100K = 1;   
#endif
#endif


    b_26m_clock_closed = TRUE;
    SYSirq_Interrupts_Restore_Flags(cpu_flags, mask);
}
#if 0//def BEKEN_DEBUG
extern uint32 sleep_slots;
#endif
RAM_CODE void Open_26M_Clock(void)
{
    u_int32 cpu_flags, mask;

    if (!b_26m_clock_closed)
        return;

    SYSirq_Disable_Interrupts_Save_Flags(&cpu_flags, &mask);
    BK3000_GPIO_47_CONFIG = 0x01;  // clear gpio latch;
    /* Step 0:  */
    BK3000_A0_CONFIG |= (1<<15);  // cbias on;
    BK3000_Ana_PLL_enable(CPU_DCO_CLK);
    BK3000_A1_CONFIG &= ~(1<<31);   // BUCKA pfm
    BK3000_A0_CONFIG &= ~(1<<29);   //xtal buff on;
    //BK3000_A0_CONFIG |= (1<<20);   //bt ldo on; 

             

    /* Step 1: Power on CEVA clock */
    BK3000_PMU_PERI_PWDS &= (~bit_PMU_CEVA_PWD); 
    /* Step 2: Open saradc and ...*/
    #if 0
    GPIO_Ax.a3->spipwdxtalbufa = 0;  
    GPIO_Ax.a3->spipwdSarADCA = 0;
    GPIO_Ax.a3->spipwdSarADCBuffer = 0;
    GPIO_Ax.a3->spipwdSARADCLDO = 0;
    GPIO_Ax.a3->spipwdrsc100K     = 0;    /**< 打开100K时钟 */
    #endif

    /* Step 3: Power on peripheral */
    BK3000_PMU_PERI_PWDS = backup_pmu_peri_pwds; 

    b_26m_clock_closed = FALSE;

    /* Step 5: set cpu clk */
#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(1, 0);				/* 26M xtal */
#else
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif

    /* Step 6: set flash */
	flash_config();

    /* Step 7: Open BT xvr 52M */
    //BK3000_XVR_REG_0x09 = XVR_analog_reg_save[9]&(~(1<<11)); 
    BK3000_XVR_REG_0x09 = XVR_analog_reg_save[9] = 0x23DFF304;//0x23DFF304;//0x2BDFF304;
    BK3000_XVR_REG_0x03 = XVR_analog_reg_save[3] | (0x01 << 0);


    /* Step 8: set uart */
#if (CONFIG_UART_IN_SNIFF == 1)
	BK3000_GPIO_1_CONFIG = 0x7C;
#endif
//    BK3000_GPIO_Initial();
    SYSirq_Interrupts_Restore_Flags(cpu_flags, mask);
}
extern u_int32 XVR_analog_reg_save[];
void BK3000_Ana_PLL_enable( u_int16 freq ) 
{
    u_int16 cnt_i ;
    BK3000_A0_CONFIG &= ~(1<<26);  // DCO power ON;
    delay_us(5);
    XVR_analog_reg_save[6] = 0x02C80400;//default value;
    if(freq>100)
    {
        cnt_i = freq * 64 / 26 ;
        XVR_analog_reg_save[6] = ((~0x01FF3000) & XVR_analog_reg_save[6]) + ((cnt_i & 0x1FF)<<16) + (0x00 << 12);
    }
    else
    {
        cnt_i = freq * 96 / 26 ;
        XVR_analog_reg_save[6] = ((~0x01FF3000) & XVR_analog_reg_save[6]) + ((cnt_i & 0x1FF)<<16) + (0x01 << 12);
    }
    BK3000_XVR_REG_0x06 = XVR_analog_reg_save[6] ;
    BK3000_XVR_REG_0x06 = XVR_analog_reg_save[6] = XVR_analog_reg_save[6] & 0xFFFF7FFF ;
    delay_us(5);
    BK3000_XVR_REG_0x06 = XVR_analog_reg_save[6] = XVR_analog_reg_save[6] | 0x00008000 ;
    delay_us(5);
    BK3000_XVR_REG_0x06 = XVR_analog_reg_save[6] = XVR_analog_reg_save[6] & 0xFFFF7FFF ;
    delay_us(5);
    BK3000_XVR_REG_0x06 = XVR_analog_reg_save[6] = XVR_analog_reg_save[6] | 0x00008000 ;
    delay_us(5);
}
void BK3000_set_ana_dig_voltage(uint8 ana,uint8 dig)
{
    uint32 reg;
    reg = BK3000_A4_CONFIG;
    reg &= ~(0x03 << 10);     //  analog voltage;
    reg &= ~(0x03 << 6);      // digital voltage;
    reg |= (ana << 10);
    reg |= (dig << 6);
    BK3000_A4_CONFIG = reg;
}
void BK3000_ana_buck_enable( char enable)
{
    if(enable)
    {
        BK3000_A2_CONFIG &= ~(1<<10);
        delay_us(10);
        BK3000_A2_CONFIG &= ~(1<<18);
        delay_us(100);
        BK3000_A2_CONFIG |=  (1<<18);
        delay_us(100);
    }
    else
    {
        BK3000_A2_CONFIG |= (1<<10);
    }
    return;
}
void BK3000_Dig_Buck_open( char enable )
{
    if(enable)
    {
        BK3000_A1_CONFIG &= ~(1 << 20);
    }
    else
    {
        BK3000_A1_CONFIG |= (1 << 20);
    }
}
static void BK3000_analog_reg_init( void )
{
#if 0//(CONFIG_APP_TOOLKIT_5 == 0)&&(SYS_CFG_BUCK_ON ==  1)  // buck on
    BK3000_A0_CONFIG = 0xC706B621; //0xE706B621; C706B621 0xC716B621
    delay_us(2000);
    BK3000_A1_CONFIG = 0x1B03FC6C;//0x5C03FC6C;//0x1F03FC6C;//0x9F09FC6C;
    BK3000_A2_CONFIG = 0x7E132904;//0x7E13298C;//0x7E12A98C;//0x7E12A99C;//0x7E12BF9C;
    delay_us(2000);
    BK3000_A0_CONFIG = 0xC716B621;
    delay_us(2000);
    BK3000_A3_CONFIG = 0x180004A0;//0x180004A0;
    BK3000_A4_CONFIG = 0x84200E7F;//0x84200A7F;//0x84200A5F;84200E7F
    BK3000_A5_CONFIG  = 0x80801400 ;
    BK3000_A6_CONFIG  = 0xE83B4068 ;
    BK3000_A7_CONFIG  = 0x80006237 ;
#else  // ldo
    BK3000_A0_CONFIG = 0xC706B621; //0xE706B621;
    delay_us(2000);
    BK3000_A1_CONFIG = 0x1B13FC6C;//0x5C13FC6C;//0x1F13FC6C;//0x9F09FC6C;
    BK3000_A2_CONFIG = 0x7E132D04;//0x7E132D8C;//0x7E12AD8C;//0x7E12AD9C;//0x7E12BF9C;
    delay_us(2000);
    BK3000_A0_CONFIG = 0xC716B621;
    delay_us(2000);
    BK3000_A3_CONFIG = 0x180004A0;//0x180004A0;
    BK3000_A4_CONFIG = 0x84200E7F;//0x84200A7F;//0x84200A5F;84200E7F
    BK3000_A5_CONFIG  = 0x80801400 ;
    BK3000_A6_CONFIG  = 0xE83B4068 ;
    BK3000_A7_CONFIG  = 0x80006237 ;
#endif
#ifdef BT_DUALMODE
    /*Disable CEVA AHB Bus Clock Gating, otherwise BLE PKA will lost sometime, Yangyang, 2018/08/15*/
    BK3000_PMU_GATE_CFG = 0x4800;   
#else
    BK3000_PMU_GATE_CFG = 0x800;
#endif
}
void BK3000_ICU_Initial(void)
{
    BK3000_PMU_CONFIG = (1 << sft_PMU_CPU_CLK_SEL)
                      | (0 << sft_PMU_CPU_CLK_DIV);
	//delay_us(1000);
    BK3000_analog_reg_init();
    set_flash_ctrl_config();	/*Read Flash status register to Flash controller config register*/

//    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);   /**< CPU时钟源的选择--  0: ROSC, 1: XTAL(26MHz), 2:PLL0(39MHz), 3:PLL1(78MHz) */
    /*
    BK3000_PMU_CONFIG = (CPU_CLK_SEL << sft_PMU_CPU_CLK_SEL)
                      | (CPU_CLK_DIV << sft_PMU_CPU_CLK_DIV);
    */
    //set_flash_qe();                               /**< 4线模式打开 */
    //flash_set_line_mode(FLASH_LINE_2);
    //flash_set_line_mode(DEFAULT_LINE_MODE);
    //set_flash_clk(FLASH_CLK_SEL);                 /**< FLASH CLK 4 : 78M, 8 : 26M */
    BK3000_GPIO_DEEP_SLEEP_LATCH = 0x1;


    BK3000_PMU_PERI_PWDS &= ~bit_PMU_UART_PWD;
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_CEVA_PWD;               // enable CEVA baseband clock
    //BK3000_PMU_PERI_PWDS &= ~bit_PMU_SDIO_PWD;               // enable SDCard clock
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_LPO_CLK_STEAL_ENABLE;   // Clock Stealing Enable to get 32k clock
    BK3000_PMU_PERI_PWDS |= bit_PMU_LPO_CLK_SEL_XTAL;        // 1:XTAL分频 0:ROSC输入 32.768
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_LPO_CLK_DISABLE;        // enable LPO clock using for 32K clock
    BK3000_PMU_PERI_PWDS |= bit_PMU_CLK_FREQ_SEL;
}
void BK3000_start_wdt( u_int32 val )
{
#ifdef TRACE32_DEBUG
    return;
#endif
    BK3000_WDT_CONFIG = 0x5A0001|val;
    BK3000_WDT_CONFIG = 0xA50001|val;
}

void BK3000_wdt_power_on(void)
{
#ifdef TRACE32_DEBUG
    return;
#endif
    BK3000_WDT_CONFIG = 0x5A0001|0x3fff;
    BK3000_WDT_CONFIG = 0xA50001|0x3fff;
}
#ifdef TRACE32_DEBUG
void BK3000_stop_wdt(void )
{
    BK3000_WDT_CONFIG = 0x5A0000;
    BK3000_WDT_CONFIG = 0xA50000;
}
#endif
void BK3000_wdt_reset(void )
{
#ifdef TRACE32_DEBUG
    return;
#endif
    BK3000_WDT_CONFIG = 0x5A0001;
    BK3000_WDT_CONFIG = 0xA50001;
}

static void app_set_powerdown_gpio(void)
{
    uint8 i=0;
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();

    if((env_h->env_cfg.system_para.system_flag&APP_ENV_SYS_FLAG_LED_REVERSE) >> 9)
    {
        for(i = 0; i < LED_NUM; i++)
        {
            if(app_h->led_map[i] == LED_INVALID_INDEX)
                continue;
            gpio_config(app_h->led_map[i],1);	
            gpio_output(app_h->led_map[i],1);
        }
    }	
    
    if ((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PAMUTE_HIGH) >> 6 )
    {
        gpio_config(env_h->env_cfg.system_para.pamute_pin,1);	
        gpio_output(env_h->env_cfg.system_para.pamute_pin,1);
    }	
}

extern uint8 app_get_auto_powerdown(void);
static void BK3000_icu_deepsleep(uint8 wakup_pin,t_powerdown_mode pwrdown_mode)
{
    int i,jitter_low_cnt = 0,jitter_high_cnt = 0;
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 high_flag = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH) ? 1:0 ;

    os_printf("icu_deepsleep:%d,%d\r\n",wakup_pin,high_flag);
    Delay(1000);
    set_spr( SPR_VICMR(0), 0); 
    Delay(10);
    cpu_set_interrupts_enabled(0);
    BK3000_PMU_CONFIG = (1 << sft_PMU_CPU_CLK_SEL)
                          | (0 << sft_PMU_CPU_CLK_DIV
                          /* | PMU_AHB_CLK_ALWAYS_ON */);
    set_flash_clk(FLASH_CLK_26mHz);
    flash_set_line_mode(FLASH_LINE_2);
    if(pwrdown_mode == POWERDOWN_SELECT)
    {
        if(GPIO_CHARGER_FLAG)
        {
            bt_flag2_operate(APP_FLAG2_CHARGE_POWERDOWN,0);
           	BK3000_start_wdt(0x1ff);
            while(1);
            //return;
        }
    }
    /* Step 1: close RF cb and 52M pll */
    BK3000_XVR_REG_0x09 = 0x2BFFF304;
    /* Step 2: close ABB CB */
    BK3000_A0_CONFIG &= ~(1<<15);
    /* Step 3: close DCO */
    BK3000_A0_CONFIG |= (1<<26);
    /* Step 4: ana buck pfm mode */
    BK3000_A1_CONFIG &= ~(1<<31);

    /* if system working in LDO MODE */
    /* Step 5: dig buck ramp compensation disable */
    /* Step 6: ana buck ramp compensation disable */
    if(env_h->env_cfg.used == 0x01)
    {
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
        if(!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DIG_BUCK_ENABLE))
        {
            BK3000_A1_CONFIG &= ~(1<<15);
        }
        if(!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_ANA_BUCK_ENABLE))
        {
            BK3000_A2_CONFIG &= ~(1<<7);
        }
    #elif(CHIP_PACKAGE_TSSOP_28 == 1) // LDO MODE
        BK3000_A1_CONFIG &= ~(1<<15);
        BK3000_A2_CONFIG &= ~(1<<7);
    #endif
    }
    else
    {
    #if (SYS_CFG_BUCK_ON == 0)    // LDO MODE
        BK3000_A1_CONFIG &= ~(1<<15);
        BK3000_A2_CONFIG &= ~(1<<7);
    #endif
    }
 
    Delay(100);
    for( i = 0; i <= GPIO_NUM; i++)
    {
        gpio_config(i,0);  // High impedance
    }

    if (high_flag)
    	gpio_config(wakup_pin, 0);
    else
    	gpio_config(wakup_pin, 3);

    /* the elimination of GPIO jitter */
    if(pwrdown_mode == POWERDOWN_SELECT)
    {
    while(app_get_auto_powerdown())
    {
        os_delay_ms(20);
        if(gpio_input(wakup_pin) == (high_flag ^ 0x01))
        {
            jitter_high_cnt = 0;
            jitter_low_cnt++;
            if(jitter_low_cnt > 3) // if gpio detect is anti-high_flag level more than 3 times,then deep sleep!
                break;
        }
        else
        {
            jitter_low_cnt = 0;
            jitter_high_cnt++;
            if(jitter_high_cnt > 0x0f) // if GPIO is high_flag level for a long time,then reset mcu;
            {
                BK3000_start_wdt(0xff);
                while(1);
            }
        }
    }
	
	if(gpio_input(wakup_pin) == high_flag)
	{
		gpio_config(wakup_pin, 5);
		if (DEFAULT_WAKEUP_PIN == wakup_pin)
			gpio_config(7, 5);
    	}
	}

    app_set_powerdown_gpio();
    Delay(10);

    BK3000_A0_CONFIG |= (1<<16);

	BK3000_GPIO_DEEP_SLEEP_LATCH = 0x1;
	BK3000_GPIO_PAD_CTRL |= (0x1f<<3);
    /*
    * Bit 29: wakeup enable;
    * Bit xx: gpio pin(0:26)wakeup;
    * Bit 27: USB plug in wakeup;
    */
    BK3000_GPIO_WKUPEN  = ((1<<29) | (1<<wakup_pin));
    if(pwrdown_mode == POWERDOWN_SELECT)
        BK3000_GPIO_WKUPEN |= (1<<27);
    BK3000_GPIO_DPSLP  = 0x3261;

    while(1);

}
static void BK3000_icu_shutdown( uint8  wakup_pin)
{
    int i;
    app_handle_t app_h = app_get_sys_handler();
    
    os_printf("icu_shutdown:%d\r\n",wakup_pin);
    set_spr( SPR_VICMR(0), 0);
    Delay(10);
    cpu_set_interrupts_enabled(0);
    if(GPIO_CHARGER_FLAG)
    {
        bt_flag2_operate(APP_FLAG2_CHARGE_POWERDOWN,0);
        BK3000_start_wdt(0x1ff);
        while(1);
    }
    for( i = 0; i <= GPIO_NUM; i++)
    {
        gpio_config(i,0);//input & pulldown;
    }

    if(ADC_CHANNEL_NULL != app_h->button_ad_channel)
    {//配置ADC按键为高阻状态
        if(ADC_CHANNEL_1 == app_h->button_ad_channel)
        {
            gpio_config(6,5);//input & pulldown;
        }
        else if(ADC_CHANNEL_2 == app_h->button_ad_channel)
        {    
            gpio_config(7,5);//High impedance
        }
        else if(ADC_CHANNEL_4 == app_h->button_ad_channel)
        {    
            gpio_config(18,5);//High impedance
        }
        else if(ADC_CHANNEL_6 == app_h->button_ad_channel)
        {    
            gpio_config(20,5);//High impedance
        }
        else if(ADC_CHANNEL_7 == app_h->button_ad_channel)
        {   
            gpio_config(21,5);//High impedance
        }
    }
    //gpio_config(6,5); // for 28pin shutdown
    app_set_powerdown_gpio();
    Delay(100);
    //GPIO15 config = input & pulldown;
    BK3000_A0_CONFIG |= (1<<16);
}
void BK3000_icu_sw_powerdown(uint8  wakup_pin,t_powerdown_mode pwrdown_mode)
{
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 high_flag = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH) ? 1:0 ;
    switch(pwrdown_mode)
    {
    case POWERDOWN_SELECT:
#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/&&(CONFIG_SW_SWITCH_KEY_POWER == 1)

        if((SW_PWR_KEY_SWITCH==app_env_check_pwrCtrl_mode())
            && (((gpio_input(wakup_pin)!=high_flag)&&(DEFAULT_WAKEUP_PIN != wakup_pin))
            || (gpio_input(wakup_pin)==high_flag)))
        {
        		BK3000_icu_deepsleep(wakup_pin,POWERDOWN_SELECT);
        }
        else
#endif
        {
            BK3000_icu_shutdown(wakup_pin);
        }
        break;
#if (CONFIG_CHG_DEEPSLEEP == 1)		
    case POWERDOWN_CHG_DEEPSLEEP:
        BK3000_icu_deepsleep(wakup_pin,POWERDOWN_CHG_DEEPSLEEP);
        break;
#endif		
    case POWERDOWN_SHUTDOWN:
        BK3000_icu_shutdown(wakup_pin);
        break;
    default:
        BK3000_icu_deepsleep(wakup_pin,POWERDOWN_SELECT);
        break;
    }
}

void BK3000_Ana_Dac_enable(uint8 enable )
{
    app_env_handle_t  env_h = app_env_get_handle();

    //os_printf("【AUDIO】：%s(%d)\r\n", __func__, enable);
    //aud_PAmute_oper(1);
    if( enable) 
    {
        if(s_ana_dac_open == 0)
        {
            s_ana_dac_open = 1;
            audio_dac_analog_enable(1);
        #if 1/*(CONFIG_APP_TOOLKIT_5==1)*/||(CONFIG_EXT_AUDIO_PA_EN == 1)
            if(env_h->env_cfg.feature.pa_unmute_delay_time )
            {
                os_printf("pa_un_delay:%dms\r\n",10*env_h->env_cfg.feature.pa_unmute_delay_time);
                //os_delay_ms((10*env_h->env_cfg.feature.pa_unmute_delay_time));
            }
        #endif
        }
    }
    else 
    {
        if(s_ana_dac_open == 1)
        {
            s_ana_dac_open = 0;
        #if 1/*(CONFIG_APP_TOOLKIT_5==1)*/||(CONFIG_EXT_AUDIO_PA_EN == 1)
            if(env_h->env_cfg.feature.pa_mute_delay_time )
            {
                os_printf("pa_delay:%dms\r\n",10*env_h->env_cfg.feature.pa_mute_delay_time);
                //os_delay_ms((10*env_h->env_cfg.feature.pa_mute_delay_time));
            }
        #endif
            audio_dac_analog_enable(0);
        }
    }
}

#ifdef CONFIG_BLUETOOTH_HFP
void BK3000_Ana_Adc_enable(uint8 enable)
{
    app_env_handle_t  env_h = app_env_get_handle();

    if( enable )
    {
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
	if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_MIC_SINGLE_ENABLE))
		BK3000_A6_CONFIG |= (1<<31);
	else
		BK3000_A6_CONFIG &= ~(1<<31);
    #elif (CONFIG_AUDIO_MIC_DIFF_EN == 1)
        BK3000_A6_CONFIG &= ~(1<<31);
    #else
        BK3000_A6_CONFIG |= (1<<31);
    #endif
        BK3000_A6_CONFIG &= ~(1<<28);
        BK3000_A6_CONFIG &= ~(1<<25);
        BK3000_A6_CONFIG |= (1<<19);
        BK3000_A6_CONFIG &= ~(1<<18);
        BK3000_A6_CONFIG |= (1<<17);
        BK3000_A6_CONFIG |= (1<<16);
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
        if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_MIC_SINGLE_ENABLE))
            BK3000_A7_CONFIG |=  (1<<12);
        else
            BK3000_A7_CONFIG &= ~(1<<12);
    #elif CONFIG_AUDIO_MIC_DIFF_EN == 1
        BK3000_A7_CONFIG &= ~(1<<12);
    #else
        BK3000_A7_CONFIG |=  (1<<12);
    #endif
        BK3000_A6_CONFIG &= ~(1<<24);
        BK3000_A6_CONFIG &= ~(1<<20);
        BK3000_A6_CONFIG |= (1<<21);

    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
        BK3000_A6_CONFIG = (BK3000_A6_CONFIG & ~(0x03<<14))
                |((( env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_MIC_BIAS ) >> 13) << 14);
    #else
        BK3000_A6_CONFIG = (BK3000_A6_CONFIG & ~(0x03<<14)) |(0x01 << 14);
    #endif

    }
    else
    {
        BK3000_A6_CONFIG |= (1<<20);
    }
}

void open_linein2aux_loop(void)
{
    os_printf("open_linein2aux_loop...\r\n");
    BK3000_AUD_FIFO_CONF |=  ( 1 << sft_AUD_LOOP_ADC2DAC);
}

void close_linein2aux_loop(void)
{
    os_printf("close_linein2aux_loop...\r\n");
    BK3000_AUD_FIFO_CONF &= ~(1<<sft_AUD_LOOP_ADC2DAC);
}

void BK3000_Ana_Line_enable( uint8 enable )
{
    app_env_handle_t  env_h = app_env_get_handle();
    static uint32 backup_dac_config0 = 0;
    static uint32 backup_dac_config1 = 0;
    static uint32 backup_dac_config2 = 0;
    uint8 k = 0;
    
    os_printf("BK3000_Ana_Line_enable(%d)\r\n", enable);

    if(enable)
    {
        /* Warning:
         * 1) Line in R&L and GPIO4&5 are multiplexed,when Line in is used,GPIO4/5 must be config high impedance.
         * 2) GPIO 4/5 address mapping is GPIO 8/9.
         */
        gpio_config(8,5); // for line in
        gpio_config(9,5); // for line in

        {
        	for(k=0;k<16;k++)
        		AUD_WRITE_DACL(0, 0);
        #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
            aud_initial(44100, 2, 16); // include open dac: BK3000_Ana_Dac_enable(1)
        #else
            aud_initial(48000, 2, 16); // include open dac: BK3000_Ana_Dac_enable(1)
        #endif
        }
        //BK3000_PMU_PERI_PWDS &= ~bit_PMU_AUD_PWD; //power on=0， power off=1     
        BK3000_AUD_AUDIO_CONF_BAK &= ~0x0000002C;
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        BK3000_A6_CONFIG &= ~(1<<20);  // Power down of Left channel ADC
        BK3000_A6_CONFIG &= ~(1<<21);  // Power down of Right channel ADC
        BK3000_A6_CONFIG |= (1<<24);  // line_en
        BK3000_A6_CONFIG &= ~(3<<22); //line_gain<23:22>
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
        BK3000_AUD_ADC_CONF0  = (((env_h->env_cfg.feature.vol_linein_dig&0x3f)<<18)|(BK3000_AUD_ADC_CONF0&(~(0x3f<<18))));
	 BK3000_AUD_ADC_CONF0 |= (1<<sft_AUD_ADC_HPF1_BYPASS);
        BK3000_A6_CONFIG |= ((env_h->env_cfg.feature.vol_linein_ana)<<22);  //  line_gain<23:22>= 0/1/2/3  ????0/2/4/6dB
    #else
        BK3000_A6_CONFIG |= (0<<22);  //  line_gain<23:22>= 0/1/2/3  ????0/2/4/6dB
    #endif
        BK3000_A6_CONFIG &= ~(1<<28);  //  micrefbuf_en<28> = 0  
		#if (CONFIG_APP_TOOLKIT_5 == 1)
		//make sure line in Vpp-max almost 1Vrms.

        //audadcvmbuftrm=1（spi_MICTrim<1:0>=3）或者audadcvmbuftrm=0（spi_MICTrim<1:0>=0）
        //这两个匹配才能是运放的共模电压处于vdd/2附近
 	 BK3000_A7_CONFIG &= ~(1<<12);
 	 BK3000_A6_CONFIG &= ~(0x03<<14);
 	 BK3000_A6_CONFIG |= (1<<19);
		#if (CONFIG_CUSTOMER_ADVOID_POP_AUX_MODE == 1)
		if(!app_env_check_Use_ext_PA())
			app_customer_ADC_DC_filter();
		#endif
//        BK3000_A6_CONFIG = (BK3000_A6_CONFIG & ~(0x03<<14))
//			|((( env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_MIC_BIAS ) >> 13) << 14);
		#else
        BK3000_A6_CONFIG = (BK3000_A6_CONFIG & ~(0x03<<14)) |(0x02 << 14);
		//	MicTrm<15:14>=0/1/2/3 ????????2.4/2.2/2.0/1.8
		#endif

    #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
        if (bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
            && bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
        {
            close_linein2aux_loop(); //Ditial: disable ADC-->DAC test loop for linein mode
        }
        else
        {
            BK3000_AUD_AUDIO_CONF_BAK |= 0x0000002C ;
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            open_linein2aux_loop();   // Enable ADC-->DAC test loop
        }
    #else
        BK3000_AUD_AUDIO_CONF_BAK |= 0x0000002C ;
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        //open_linein2aux_loop();   // Enable ADC-->DAC test loop
    #endif
        
        backup_dac_config0 = BK3000_AUD_DAC_CONF0;
        backup_dac_config1 = BK3000_AUD_DAC_CONF1;
        backup_dac_config2 = BK3000_AUD_DAC_CONF2;

//        BK3000_AUD_ADC_CONF0  = (0x2F<<18)|(1<<17)|(1<<16)|0x00003A22;  //ADC 增益bit[23]-bit[18]

        //BK3000_AUD_FIFO_CONF |= ((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN )| (1 << sft_AUD_ADC_INT_EN));
        BK3000_AUD_FIFO_CONF |= (1 << sft_AUD_ADC_INT_EN);
    }
    else
    {
        //BK3000_PMU_PERI_PWDS |= (bit_PMU_AUD_PWD);
        //BK3000_AUD_FIFO_CONF &= ~((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN )| (1 << sft_AUD_ADC_INT_EN));
        BK3000_AUD_FIFO_CONF &= ~(1 << sft_AUD_ADC_INT_EN);
        BK3000_A6_CONFIG |= (1<<20);  // Power down of Left channel ADC
        BK3000_A6_CONFIG |= (1<<21);  // Power down of Right channel ADC
        BK3000_A6_CONFIG &= ~(1<<24);  // line_en
        /* close dac */
        aud_close();

        BK3000_AUD_AUDIO_CONF_BAK &= ~(1<<5);
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        //BK3000_AUD_FIFO_CONF &= ~(1<<25);       //Ditial: disable ADC-->DAC test loop for linein mode

        BK3000_AUD_DAC_CONF0  = backup_dac_config0 ;
        BK3000_AUD_DAC_CONF1  = backup_dac_config1 ;
        BK3000_AUD_DAC_CONF2  = backup_dac_config2 ;

        BK3000_AUD_ADC_CONF0  = 0x00E93A22;
		
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
        BK3000_AUD_ADC_CONF0  = (((env_h->env_cfg.feature.vol_mic_dig&0x3f)<<18)|(BK3000_AUD_ADC_CONF0&(~(0x3f<<18))));
    #endif
    }
}

#endif

void enable_audio_ldo(void)
{
    //GPIO_Ax.a3->pwdAudDigLDO = 0;    // Enable the LDO at Microphone LineIn and audio condition
    //BK3000_A3_CONFIG &= (~(1 << 0));
}

void enable_audio_ldo_for_music_files(void)
{
    //GPIO_Ax.a3->pwdAudDigLDO = 1;       // Disable the LDO at Microphone LineIn and audio condition for mp3&wav
    //BK3000_A3_CONFIG |= (1 << 0);
}

void clear_sco_connection(void)
{
    bt_flag1_operate(APP_FLAG_SCO_CONNECTION, 0);

    disable_audio_ldo();
}
void clear_wave_playing(void)
{
    bt_flag1_operate(APP_FLAG_WAVE_PLAYING, 0);

    disable_audio_ldo();
}
void clear_music_play(void)
{
    bt_flag1_operate(APP_FLAG_MUSIC_PLAY, 0);

    disable_audio_ldo();
}

void clear_line_in(void)
{
    bt_flag1_operate(APP_FLAG_LINEIN, 0);

    disable_audio_ldo();
}

void disable_audio_ldo(void)
{
    return;
/*
    if(0 == (bt_flag1_is_set(APP_FLAG_LINEIN
                                | APP_FLAG_MUSIC_PLAY
                                | APP_FLAG_WAVE_PLAYING
                                | APP_FLAG_SCO_CONNECTION)
#if (CONFIG_APP_MP3PLAYER == 1)
             || player_get_play_status()
#endif
    	))
    {
        os_printf("disable_audio_ldo\r\n");
        //GPIO_Ax.a3->pwdAudDigLDO = 1;       // Enable the LDO at Microphone LineIn and audio condition
        //BK3000_A3_CONFIG |= (1 << 0);       // Enable the LDO at Microphone LineIn and audio condition
    }
*/
}

DRAM_CODE void BK3000_set_clock (int clock_sel, int div) 
{
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    if (b_26m_clock_closed)
    {
        return;
    }
#endif
#if ((CONFIG_CHARGE_EN == 1))
    if(bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN))
    {
        clock_sel = 1;
        div = 0;
    }
#endif	
#ifdef BEKEN_UP_OTA_3266
    /*If ota_update_flag is valid , OTA updating..., stay in BLE always*/
    if(flag_ota_enable) 
    {    
        clock_sel = 3;
        div = 1;
    }
#endif
#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    u_int32 cpu_sel = 0;
    u_int32 div_sel = 0;
    cpu_sel = (BK3000_PMU_CONFIG >> sft_PMU_CPU_CLK_SEL) & 0x03;
    div_sel = (BK3000_PMU_CONFIG >> sft_PMU_CPU_CLK_DIV) & 0x7f;
    if((cpu_sel == clock_sel)&&(div_sel == div))
        return;
#endif	
    //os_printf("%d,%d\r\n",clock_sel,div);

    BK3000_PMU_WAKEUP_INT_MASK = 0;
    BK3000_PMU_CONFIG = (clock_sel << sft_PMU_CPU_CLK_SEL)
                      | (div << sft_PMU_CPU_CLK_DIV
                      /* | PMU_AHB_CLK_ALWAYS_ON */);
}

void BK3000_GPIO_Initial (void) 
{
    /// UART
    BK3000_GPIO_0_CONFIG  = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_1_CONFIG  = 1<<sft_GPIO_INPUT_MONITOR;
#if 0//def BEKEN_DEBUG
    /// CEVA Tx/Rx
    BK3000_GPIO_4_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_5_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_6_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE; // baseband rx bit
    BK3000_GPIO_7_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE; // baseband tx bit
    /// GPIO Debug Message
    BK3000_GPIO_8_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_9_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_12_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_13_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_14_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_15_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_16_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_17_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;

    BK3000_GPIO_18_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE; //radio on

    BK3000_GPIO_GPIODCON |= 0x7f3f0;  // one bit for one gpio(test function)
    //HW_set_test_msg(1);// for debug message function
	/// Enable Debug Pin
//#ifndef CONFIG_APP_SDCARD
//    BK3000_GPIO_GPIODCON = (((0X3F<<4)+0X03)<<8)+0XF0;
//#endif
#else //GPIO配置为高阻态
/*
    BK3000_GPIO_2_CONFIG  = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_3_CONFIG  = 1<<sft_GPIO_INPUT_MONITOR;

    BK3000_GPIO_4_CONFIG  = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_5_CONFIG  = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_6_CONFIG  = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_7_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_8_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_9_CONFIG  = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_10_CONFIG = 0x08;
    BK3000_GPIO_11_CONFIG = 0x08;

    BK3000_GPIO_12_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_13_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_14_CONFIG = 1<<sft_GPIO_FUNCTION_ENABLE;
    BK3000_GPIO_15_CONFIG = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_16_CONFIG = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_17_CONFIG = 1<<sft_GPIO_INPUT_MONITOR;

    BK3000_GPIO_18_CONFIG = 0x08;
    BK3000_GPIO_19_CONFIG = 0x08;
    BK3000_GPIO_20_CONFIG = 0x08;
    BK3000_GPIO_21_CONFIG = 0x08;
    BK3000_GPIO_22_CONFIG = 0x08;
    BK3000_GPIO_23_CONFIG = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_24_CONFIG = 0x08;
    BK3000_GPIO_25_CONFIG = 1<<sft_GPIO_INPUT_MONITOR;
    BK3000_GPIO_26_CONFIG = 1<<sft_GPIO_INPUT_MONITOR;
*/
    //BK3000_GPIO_2_CONFIG  = 0x08;
    //BK3000_GPIO_3_CONFIG  = 0x08;

    //BK3000_GPIO_4_CONFIG  = 0x08;
    //BK3000_GPIO_5_CONFIG  = 0x08;
    //BK3000_GPIO_6_CONFIG  = 0x08;
    BK3000_GPIO_6_CONFIG  = 0x2c;   // for 28pin package,shutdown
    BK3000_GPIO_7_CONFIG  = 0x2c;
    BK3000_GPIO_8_CONFIG  = 0x2c;
    BK3000_GPIO_9_CONFIG  = 0x2c;
    BK3000_GPIO_10_CONFIG = 0x2c;
    BK3000_GPIO_11_CONFIG = 0x2c;

    BK3000_GPIO_12_CONFIG = 0x2c;
    BK3000_GPIO_13_CONFIG = 0x2c;
    BK3000_GPIO_14_CONFIG = 0x2c;
    BK3000_GPIO_15_CONFIG = 0x2c;
    BK3000_GPIO_16_CONFIG = 0x2c;
    BK3000_GPIO_17_CONFIG = 0x2c;

    BK3000_GPIO_18_CONFIG = 0x2c;
    BK3000_GPIO_19_CONFIG = 0x2c;
    BK3000_GPIO_20_CONFIG = 0x2c;
    BK3000_GPIO_21_CONFIG = 0x2c;
    BK3000_GPIO_22_CONFIG = 0x2c;
    BK3000_GPIO_23_CONFIG = 0x2c;
    BK3000_GPIO_24_CONFIG = 0x2c;
    BK3000_GPIO_25_CONFIG = 0x2c;
    BK3000_GPIO_26_CONFIG = 0x2c;

    BK3000_GPIO_GPIODCON = 0;
#endif

	/// Perial Mode 1 function
    BK3000_GPIO_GPIOCON = 0;
}

//RAM_CODE unsigned char BK3000_hfp_set_powercontrol(void)
//{
//    unsigned char set_power = 0;
//    set_power = 0x18;
//    set_power = set_power &0x1f;  
//    return set_power;
//}

void VICMR_enable_intr_src(uint32 mask)
{
    uint32 int_mask;

    int_mask = get_spr(SPR_VICMR(0));
    int_mask |= mask;
    set_spr( SPR_VICMR(0), int_mask);
}

void VICMR_disable_intr_src(uint32 mask)
{
    uint32 int_mask;

    int_mask = get_spr(SPR_VICMR(0));
    int_mask &= (~mask);
    set_spr( SPR_VICMR(0), int_mask);
}

void VICMR_usb_chief_intr_enable(void)
{
    VICMR_enable_intr_src(1<<VIC_USB_ISR_INDEX);
}

void VICMR_usb_chief_intr_disable(void)
{
    VICMR_disable_intr_src(1<<VIC_USB_ISR_INDEX);
}

void ba22_disable_intr_exception(void)
{
    cpu_set_interrupts_enabled(0);
}

void ba22_enable_intr_exception(void)
{
    cpu_set_interrupts_enabled(1);
}

DRAM_CODE void  VICMR_disable_interrupts(uint32 *interrupts_info_ptr, uint32 *mask)
{
    uint32 oldmask = get_spr(SPR_VICMR(0));

    set_spr(SPR_VICMR(0), 0);

    *interrupts_info_ptr = oldmask;
}
DRAM_CODE void VICMR_restore_interrupts(uint32 interrupts_info, uint32 mask)
{

    if(interrupts_info != 0)
    {
        uint32 trg_flags = get_spr(SPR_VICTR(0));
        set_spr(SPR_VICTR(0), trg_flags &(~(1<<VIC_CEVA_ISR_INDEX)));
        set_spr(SPR_VICMR(0), interrupts_info);
    }
    //set_spr( SPR_VICMR(0), interrupts_info);
}

#ifdef	WROK_AROUND_DCACHE_BUG
inline void app_Dcache_disable(void)
{
    //disable dcache
	unsigned int sr ;
	sr = get_spr(SPR_SR);
	sr = sr & (~SPR_SR_DCE);
	set_spr(SPR_SR, sr);

	//gpio_output( 9, 1 );
	//os_printf("Dcache_disable\r\n");
}

inline void app_Dcache_enable(void)
{
	//enable dcache
	unsigned int sr ;
	sr = get_spr(SPR_SR);
	sr = sr | SPR_SR_DCE;
	set_spr(SPR_SR, sr);
	//gpio_output( 9, 0 );
	//os_printf("old\r\n");
}

inline void app_Dcache_initial(void)
{
	unsigned int sr ;

	//disable dcache
	sr = get_spr(SPR_SR);
	sr = sr & (~SPR_SR_DCE);
	set_spr(SPR_SR, sr);

	//initial dcache
	set_spr(SPR_RIR_MIN, 0);
	set_spr(SPR_RIR_MAX, 0xfffffff4);

	//enable dcache
	sr = get_spr(SPR_SR);
	sr = sr | SPR_SR_DCE;
	set_spr(SPR_SR, sr);

	//disable dcache
	sr = get_spr(SPR_SR);
	sr = sr & (~SPR_SR_DCE);
	set_spr(SPR_SR, sr);
}
#endif

RAM_CODE uint8 bk3000_set_afc_disable(void)
{ // 0:  频偏自动调整使能    1:禁止使用频偏自动调整
#ifdef CONFIG_EN_AFC
    return 0;
#else
    return 1;
#endif
}

#ifdef CONFIG_PRODUCT_TEST_INF
uint8 aver_rssi = 0;
int16 freq_offset = 0,aver_offset=0;

uint8 rssi = 0;
inline void get_freqoffset_rssi(void)
{
     rssi = BK3000_XVR_REG_0x12 & 0xff;
     freq_offset = BK3000_XVR_REG_0x14 & 0x1ff;
}

void average_freqoffset_rssi(void)
{
    static uint16 sum_rssi=0;
    static int16 sum_offset=0;
    static uint8 i = 0;
    int16 temp = freq_offset;
    sum_rssi += rssi;
    temp <<= 7;
    temp >>= 7;
    sum_offset += temp;
    i++;
    if(i==5)
    {
        aver_rssi=sum_rssi/i;
        aver_offset = sum_offset/i;
        i = 0;
        sum_rssi = 0;
        sum_offset = 0;
    }

}
#else
inline void get_freqoffset_rssi(void)
{

}
#endif
// end of file
