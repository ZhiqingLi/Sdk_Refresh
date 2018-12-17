/*
 ********************************************************************************
 *                       ACTOS
 *                 show logo when boot
 *
 *                (c) Copyright 2003-2009, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : welcome.c
 * By     : johnsen
 ********************************************************************************
 */

#include "uhost.h"
#include "case_independent.h"
#include "sys_info.h"

//#define FT_MODE

extern void welcome_ejtag(void) __FAR__;
extern void welcome_i2c_tk_init(void) __FAR__;
extern bool welcome_i2c_tk_check(void) __FAR__;

void welcome_delay_ms(uint8 dly_ms)
{
    uint8 i, j;
    while (dly_ms != 0)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 255; j++)
            {
                ; //do nothing
            }
        }
        dly_ms--;
    }
}

void flush_rtc(void)
{
    act_writel(0xA596, RTC_REGUPDATA);
    while (0x5A69 != act_readl(RTC_REGUPDATA))
    {
        ;//delay for RTC update complete
    }
}

void welcome_RTL_poweron(void)
{

    act_writel(act_readl(HOSC_CTL) | 0x001f0000, HOSC_CTL); //HGMC 0x1f
    welcome_delay_ms(5);

    act_writel(act_readl(LDO_CTL) | 0xb, LDO_CTL); //BTVCC enable;BTVCC 3.3v
    welcome_delay_ms(5);
    act_writel((act_readl(LDO_CTL) &0xffe7ffff) | 0x00100000, LDO_CTL); //OSCVDD 1.3v
    //act_writel((act_readl(LDO_CTL) &0xffe7ffff) | 0x00180000, LDO_CTL); //OSCVDD 1.4v
    welcome_delay_ms(5);
    act_writel((act_readl(LDO_CTL)&(0xfffff0ff)) | 0x00001600, LDO_CTL); //BTVDD enable;BTVDD 1.1v
    //act_writel((act_readl(LDO_CTL) & 0xfffc3fff)|0x0001c000, LDO_CTL); //VD15_SET 1.35V
    welcome_delay_ms(20);

    act_writel(act_readl(BT_CFG) | (~0x7fffffff), BT_CFG);//en_chip 0->1
    flush_rtc();

    //shut_dwon_bt_n 1->0
    act_writel(act_readl(BT_CFG) & (~0x20000000), BT_CFG);
    flush_rtc();

    //shut_dwon_bt_n 0->1
    act_writel(act_readl(BT_CFG) | 0x20000000, BT_CFG);
    flush_rtc();

}

void mcu_enter_sleep_S4(void)
{

    act_writel((act_readl(VD15_DCDC_CTL) & (~VD15_DCDC_CTL_VD15_MODE_S1_MASK)), VD15_DCDC_CTL);
    act_writel((act_readl(VD15_DCDC_CTL) | (0x02 << VD15_DCDC_CTL_VD15_MODE_S1_SHIFT)), VD15_DCDC_CTL); //DC-LDO AUTO

    act_writel(0x00, POWER_CTL); //休眠
//  VD15_DCDC_CTL修改无需刷密码  
/*
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ; //nothing
    }
*/
    while (1)
    {
        ; //nothing
    }

}

bool is_usb_cable_in(void)
{
    bool usb_in = FALSE;

    usb_in = (bool) (((act_readb(DPDMCTRL) & (1 << DPDMCTRL_Plugin)) != 0)
            || ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0));

    return usb_in;
}

void disp_starting_board_config(uint8 charge_mode, welcome_argument_t *welcome_arg)
{

#if ((CASE_BOARD_TYPE == CASE_BOARD_EVB)||(CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825))

    //select PMUADC_CTL_LRADC_FS 250khz
    act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_BATADC_FS)), PMUADC_CTL);
    //enable LRADC1
    act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_LRADC1_EN)), PMUADC_CTL);
    //act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_LRADC234_EN)), PMUADC_CTL);
    //mfp switch
    act_writel((act_readl(GPIOB0_CTL) & (~GPIOB0_CTL_GPIOB0_MFP_MASK)), GPIOB0_CTL);

    //rtc register
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ;//wait for register update
    }

    
    //b0 as lradc1
    act_writel((act_readl(GPIOB0_CTL) | (1 << GPIOB0_CTL_GPIOB0_MFP_SHIFT)), GPIOB0_CTL);

    //rtc register
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ; //wait for register update
    }
#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#if (SUPPORT_AUX_DETECT == DETECT_BY_GPIOB0)
    //b0 as in for aux detect
    act_writel((act_readl(GPIOB0_CTL) | (1 << GPIOB0_CTL_GPIOB0INEN)), GPIOB0_CTL);

    //rtc register
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ; //wait for register update
    }
#endif
#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#if (SUPPORT_OUTER_CHARGE == 1)
    if (charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        //enable charge and battery check
        act_writel(act_readl(EXTERN_CHARGE_DETECT_GPIOIN) | EXTERN_CHARGE_DETECT_PIN, EXTERN_CHARGE_DETECT_GPIOIN);

        act_writel(act_readl(EXTERN_BATTERY_DETECT_GPIOIN) | EXTERN_BATTER_DETECT_PIN, EXTERN_BATTERY_DETECT_GPIOIN);
    }
#endif

    act_writel(act_readl(EXTERN_TK_PLANE_GPIOOUT) | EXTERN_TK_PLANE_PIN, EXTERN_TK_PLANE_GPIOOUT);
    act_writel(act_readl(EXTERN_TK_PLANE_GPIODAT) & (~EXTERN_TK_PLANE_PIN), EXTERN_TK_PLANE_GPIODAT);

#endif

#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))

    //enable LRADC2 PMUADC_CTL_LRADC2_EN
    act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_LRADC234_EN)), PMUADC_CTL);

    act_writel((act_readl(AD_Select1) & (~AD_Select1_GPIOA22_MASK)), AD_Select1);

    act_writel((act_readl(AD_Select1) | (0x01 << AD_Select1_GPIOA22_SHIFT)), AD_Select1);

    welcome_delay_ms(20);
        
#endif
}

void disp_starting_adfu_check(void(*adfu_launch)(void), welcome_argument_t *welcome_arg)
{
    bool adfu_flag = FALSE;

    uint32 check_cnt;

#if ((CASE_BOARD_TYPE == CASE_BOARD_EVB) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825))

    if (is_usb_cable_in() == TRUE)
    {
        welcome_delay_ms(20);

        //if ((act_readl(ONOFF_KEY) & (1<<ONOFF_KEY_ONOFF_PRESS_0)) != 0) //PLAY键
        //{
        //    key_down = TRUE;
        //}

        if ((act_readb(LRADC1_DATA) & 0x7F) < 0x62)//ADC按键
        {
            adfu_flag = TRUE;
        }
    }

#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)

    if (is_usb_cable_in() == TRUE)
    {
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
        //点亮触摸板灯
        act_writel(act_readl(EXTERN_TK_PLANE_GPIODAT) | EXTERN_TK_PLANE_PIN, EXTERN_TK_PLANE_GPIODAT);

        welcome_delay_ms(20);

        welcome_i2c_tk_init();

        if (welcome_i2c_tk_check() == TRUE)
        {
            adfu_flag = TRUE;
        }
#endif
    }

#endif

//增加ATS2823可通过按键进入ADFU,实际上是进入卡引导程序，从而可以和ATT工具通信
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    //welcome_arg->print("adc data:", (act_readb(LRADC2_DATA) & 0x7F));

    check_cnt = 0;
    
    while(1)
    {
        if ((act_readb(LRADC2_DATA) & 0x7F) < 0x62)//ADC按键
        {
            check_cnt++;

            welcome_delay_ms(100);

            if(check_cnt == 30)
            {
                adfu_flag = TRUE;
                break;
            }
        }
        else
        {
            break;
        }
    }    
#endif

    if (adfu_flag == TRUE)
    {
        welcome_arg->print("enter adfu...", 0);
        
        adfu_launch();
    }
}

void disp_starting_low_power(uint8 charge_mode)
{
    bool enter_s4 = FALSE;
    bool usb_cable_adapter = is_usb_cable_in();

#if (SUPPORT_OUTER_CHARGE == 1)
    if (charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if (IS_EXTERN_CAHREGE_IN() == 1)
        {
            usb_cable_adapter = TRUE;
        }
    }
#endif

    if (usb_cable_adapter == FALSE)
    {
#if (SUPPORT_OUTER_CHARGE == 1)
        if (charge_mode == BAT_CHARGE_MODE_OUTER)
        {
            //for adc charge value use
            act_writel((act_readl(AD_Select) & EXTERN_CHARGE_VAL_MSK)|EXTERN_CHARGE_VAL_SEL, AD_Select);
            welcome_delay_ms(10);
            if ((act_readb(TEMPADC_DATA) & 0x7f) < TEMP_ADC_1_62_V)
            {
                enter_s4 = TRUE;
            }
        }
        else
#endif
        {
            if ((act_readb(BATADC_DATA) & 0x7f) < BAT_ADC_3_30_V)
            {
                enter_s4 = TRUE;
            }
        }
    }

    if (enter_s4 == TRUE)
    {
        mcu_enter_sleep_S4();
    }
}

void disp_starting_show_welcome(void)
{
#if((CASE_BOARD_TYPE == CASE_BOARD_EVB) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825))

    //使能GPIOA4为输出，且输出高电平，以点亮LED灯
    act_writel(act_readl(GPIOAINEN) & (~(uint32) (1 << 4)), GPIOAINEN);
    act_writel(act_readl(GPIOAOUTEN) | (uint32) (1 << 4), GPIOAOUTEN);
    act_writel(act_readl(GPIOADAT) | (uint32) (1 << 4), GPIOADAT);

#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#if 0
    //使能GPIOA13/14/22/23, GPIOB11为输出，且输出低电平，以点亮LED灯
    act_writel(act_readl(GPIOAINEN) & (~(uint32) ((1 << 13) | (1 << 14) | (1 << 22) | (1 << 23))), GPIOAINEN);
    act_writel(act_readl(GPIOAOUTEN) | (uint32) ((1 << 13) | (1 << 14) | (1 << 22) | (1 << 23)), GPIOAOUTEN);
    act_writel(act_readl(GPIOADAT) & (~(uint32) ((1 << 13) | (1 << 14) | (1 << 22) | (1 << 23))), GPIOADAT);
    act_writel(act_readl(GPIOBINEN) & (~(uint32) (1 << 11)), GPIOBINEN);
    act_writel(act_readl(GPIOBOUTEN) | (uint32) (1 << 11), GPIOBOUTEN);
    act_writel(act_readl(GPIOBDAT) & (~(uint32) (1 << 11)), GPIOBDAT);
#endif
#endif
}

/**
LED INIT
*/
void led_init_spio(void)
{
    act_writel(0x00000000,DEBUGSIOIE);
    act_writel(0x00000000,DEBUGSIOOE);
    //act_writel(act_readl(MFP_CTL3) | (uint32) (1 << 4), MFP_CTL3);
    //act_writel(act_readl(MFP_CTL3) | (uint32) (1 << 5), MFP_CTL3);
    act_writel(0x0c,SIO_DAT);
}

/**
LED ON
*/
void led_on_spio(void)
{
    //act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << 2), SIO_OUTEN);
    //act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << 3), SIO_OUTEN);
}

void welcome_flush_rtc(void)
{
    uint32 i;
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ;//qac
    }
    for (i = 0; i < 256 * 4; i++)
    {
        ;//qac
    }
}

static void reset_RTCVDD_reg(void)
{
    act_writel(0X942625,VD15_DCDC_CTL); 
    act_writel(0xd7,PMUADC_CTL); 
    act_writel(0x2d,BDG_CTL); 
    act_writel(0x002aa888,LDO_CTL);  
    act_writel(0x3bf,SYSTEM_SET); 
    act_writel(0x1,POWER_CTL); //Power source as VCC/VDD/BTVCC/SVCC on/off Control Register (RTCVDD)
    act_writel(0x200000,TIMER_CTL);  
    act_writel(0,WAKE_PD); 
    act_writel(0x80d8,ONOFF_KEY);
    act_writel(0,BIAS_CTL);
    act_writel(0x1c,NFC_CTL); 
    act_writel(0Xd00,DCDC_CTL2); 
    act_writel(0X13d,SPD_CTL); 
    act_writel(0x5A69,RTC_REGUPDATA); 
   
   //下面的RTCVDD阈寄存器才需要刷密码
    act_writel(0x6fb,WKEN_CTL); 
    flush_rtc();
    
    act_writel(0x0002a0a0,HOSC_CTL); 
    flush_rtc();
    
    act_writel(0x00000Cbd,HCL_CTL); 
    flush_rtc();

    act_writel(0x80,RTC_CTL); 
    flush_rtc();
    
    act_writel(0,RTC_DHMSALM); 
    flush_rtc();

    act_writel(0,RTC_DHMS); 
    flush_rtc();

    act_writel(0x00000101,RTC_YMD); 
    flush_rtc();

    act_writel(0,RTC_ACCESS); 
    flush_rtc();

    act_writel(0,RTC_BAK0); 
    flush_rtc();

    act_writel(0,RTC_BAK1); 
    flush_rtc();

    act_writel(0,RTC_BAK2);
    flush_rtc();

    act_writel(0,RTC_BAK3); 
    flush_rtc();  

    act_writel(0x00000080,GPIOB0_CTL); 
    flush_rtc();

    act_writel(0x80000080,BT_CFG); 
    flush_rtc();
}

__section__(".FIRST") void disp_starting(void (*adfu_launch)(void), void (*read_res)(uint8*, uint16 , uint16),
        welcome_argument_t *welcome_arg)
{
    uint16 *cfg_value = (uint16 *) welcome_arg->cfg_buffer;
    uint8 charge_mode;
    uint8 capacitor_mode;
    uint8 debug_mode;
    
   //RTCVDD阈寄存器值为0的话，恢复成默认值
    if (0 == act_readl(WKEN_CTL))
    {
        reset_RTCVDD_reg();
    }
    
    //ejtag 口配置
    welcome_ejtag();

    //获取充电模式配置项默认值
    charge_mode = (uint8) cfg_value[CONFIG_ID_80_CHARGE_MODE]; //index 0 -> 充电模式
    capacitor_mode = (uint8) cfg_value[CONFIG_ID_110_CYRSTAL_CAP];
    debug_mode = (uint8)cfg_value[CONFIG_ID_121_DEBUG_MODE];

    if (capacitor_mode == 1)//使用外部电容，将内部电容设置为1.6pf
    {
        act_writel(((act_readl(HOSC_CTL)&(~0xffff))|0x1010), HOSC_CTL);
        welcome_flush_rtc();
    }

    if(debug_mode == 0)
    {
        ENABLE_WATCH_DOG(6);
    }


    //step 1: 板子初始化
    disp_starting_board_config(charge_mode, welcome_arg);
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    led_init_spio();
    led_on_spio();
#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#if (SUPPORT_OUTER_CHARGE == 1)
    if (charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if (IS_EXTERN_BATTERY_IN() == 0)
        {
            goto do_nothing;
        }
    }
#endif
#endif

//#if ((CASE_BOARD_TYPE != CASE_BOARD_ATS2823) && (CASE_BOARD_TYPE != CASE_BOARD_DVB_ATS2823))
    //step 2: 是否进入ADFU升级，如果进入ADFU升级，函数调用就不会返回
    disp_starting_adfu_check(adfu_launch, welcome_arg);
//#endif

    //step 3: 低电判断，如果低电就关机，函数调用就不会返回
    disp_starting_low_power(charge_mode);

#if ((CASE_BOARD_TYPE != CASE_BOARD_ATS2823) && (CASE_BOARD_TYPE != CASE_BOARD_DVB_ATS2823))
    //step 4: 显示 welcome UI
    disp_starting_show_welcome();
#endif

    do_nothing:

#ifdef FT_MODE
    act_writel((act_readl(HOSC_CTL) & 0xffff0000), HOSC_CTL);
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ;//qac
    }
    welcome_delay_ms(20);
    //act_writel((act_readl(HOSC_CTL) | 0x4646), HOSC_CTL);
    welcome_delay_ms(10);
    act_writel(act_readl(0xc01b0000) | 0x00000014, 0xc01b0000); //ft test mode
    welcome_delay_ms(20);
#endif
        
    //step 5: BT Controller 上电
    if((act_readl(POWER_CTL)&(1<<1)) == 0)//check is s3bt POWER ON
    {
        welcome_RTL_poweron();
    }

#ifdef FT_MODE    
    act_writel((act_readl(WD_CTL) & (~0x10)), WD_CTL);
    while(1)
    {
      ;	
    }
#endif    
}

