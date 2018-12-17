/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                             All Rights Reserved.
 *      Description:按键初始化
 *      <author>       <time>
 *       Wekan       2015-3-26pm
 *******************************************************************************/

#include "key_inner.h"

/******************************************************************************
 * \par  Description:   touch key初始化
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-23pm         1.0             review this file
 *******************************************************************************/
#if (SUPPORT_TK_DRIVER == TK_DRIVER_INNER)

void key_tk_init(void)
{
#if ((CASE_BOARD_TYPE == CASE_BOARD_EVB) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825))
    act_writel(act_readl(VOUT_CTL) | 0x00080000, VOUT_CTL);
#endif
    //reset tk
    act_writel((act_readl(MRCR) & (~(1 << MRCR_TouchKeyReset))), MRCR);

    //wakeup tk
    act_writel((act_readl(MRCR) | (1 << MRCR_TouchKeyReset)), MRCR);

    //module clk enable
    act_writel((act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_TOUCHKEYCLKEN)), CMU_DEVCLKEN);

    //mfp switch

#if((CASE_BOARD_TYPE == CASE_BOARD_EVB) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825))
    // B7=k1   B8=k2   B9=k3   B10=k4   A5=K5  A6=K6
    act_writel((act_readl(AD_Select) & (~(0xFFF << 2))) | (0x555 << 2), AD_Select);
#endif

    //irq enable
    act_writel((act_readl(TK_CTL) | (1 << TK_CTL_TK_INT_EN)), TK_CTL);
    //trouch key(1~6) enable
    act_writel(((act_readl(TK_CTL) & (~(0xFF << TK_CTL_TK0_EN))) | (0x7E << TK_CTL_TK0_EN)), TK_CTL);
    //noise detect enable
    act_writel((act_readl(TK_CTL) | (0x07 << TK_CTL_Noise_det_press_en)), TK_CTL);
    //setup tk_debounce time
    //setup tk key_press_db //bl cal_DB 6 times
    act_writel((act_readl(TK_ND_DB_CTL) & (~TK_ND_DB_CTL_BL_CAL_DB_MASK)), TK_ND_DB_CTL);

    act_writel((act_readl(TK_ND_DB_CTL) | (2 << TK_ND_DB_CTL_BL_CAL_DB_SHIFT)), TK_ND_DB_CTL);

    //setup tk key_press_db //KEY_PRESS_DB 4 times
    act_writel((act_readl(TK_ND_DB_CTL) & (~TK_ND_DB_CTL_KEY_PRESS_DB_MASK)), TK_ND_DB_CTL);

    act_writel((act_readl(TK_ND_DB_CTL) | (4 << TK_ND_DB_CTL_KEY_PRESS_DB_SHIFT)), TK_ND_DB_CTL);

    //setup tk_key_up_db//KEY_UP_DB 2 times
    act_writel((act_readl(TK_ND_DB_CTL) & (~TK_ND_DB_CTL_KEY_UP_DB_MASK)), TK_ND_DB_CTL);

    act_writel((act_readl(TK_ND_DB_CTL) | (2 << TK_ND_DB_CTL_KEY_UP_DB_SHIFT)), TK_ND_DB_CTL);
    //raw counter set as largest
    act_writel((act_readl(TK_ND_DB_CTL) & (~TK_ND_DB_CTL_TK_PWM_CNT_MASK)), TK_ND_DB_CTL);
    //raw counter
    act_writel((act_readl(TK_ND_DB_CTL) | (0xffff << TK_ND_DB_CTL_TK_PWM_CNT_SHIFT)), TK_ND_DB_CTL);

    //触摸门槛300
    act_writel(act_readl(TK_KEY_TH) & (~TK_KEY_TH_TK_PRESS_TH_MASK), TK_KEY_TH);
    act_writel(act_readl(TK_KEY_TH) | (0x14 << TK_KEY_TH_TK_PRESS_TH_SHIFT), TK_KEY_TH);

    //抬升门槛300
    act_writel(act_readl(TK_KEY_TH) & (~TK_KEY_TH_UPLIFT_TH_MASK), TK_KEY_TH);
    act_writel(act_readl(TK_KEY_TH) | (0x14 << TK_KEY_TH_UPLIFT_TH_SHIFT), TK_KEY_TH);

    //基准电容120
    act_writel(act_readl(TK_KEY_TH) & (~TK_KEY_TH_BL_CAL_TH_MASK), TK_KEY_TH);
    act_writel(act_readl(TK_KEY_TH) | (0x0A << TK_KEY_TH_BL_CAL_TH_SHIFT), TK_KEY_TH);

    //TK_IDA_CTL charge as largeest
    act_writel(0xffffffff, TK_IDA_CTL);

    //enable module
    act_writel((act_readl(TK_CTL) | (1 << TK_CTL_TK_EN)), TK_CTL);
}
#endif

/******************************************************************************
 * \par  Description:   红外按键初始化
 * \param[in]     ir_code 解码类型,在config.txt中配置
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-23pm         1.0             review this file
 *******************************************************************************/
#if(SUPPORT_HPY_DRIVER  == 1)
void key_gpio_init(void)
{
    uint8 i;
    //在check中初始化
    for (i = 0; i < 3; i++)
    {
        //input enable
        act_writel((act_readl(in_reg_arrary[i][0]) | in_reg_arrary[i][2]), in_reg_arrary[i][0]);
        if(i != 0)
        {
            //output enable
            act_writel((act_readl(out_reg_arrary[i][0]) | out_reg_arrary[i][2]), out_reg_arrary[i][0]);
            // out low
            act_writel((act_readl(out_reg_arrary[i][1]) & (~out_reg_arrary[i][3])), out_reg_arrary[i][1]);
        }
        else
        {
            act_writel((act_readl(out_reg_arrary[i][1]) | out_reg_arrary[i][2]|out_reg_arrary[i][3]), out_reg_arrary[i][1]);
            act_writel(0xA596, RTC_REGUPDATA);
            while(act_readl(RTC_REGUPDATA) != 0x5A69)//写密码

            {
                ;//nothing for QAC
            }
        }

    }
}
#endif

/******************************************************************************
 * \par  Description:   红外按键初始化
 * \param[in]     ir_code 解码类型,在config.txt中配置
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
void key_ir_init(uint8 ir_code)
{
#if(CASE_BOARD_TYPE == CASE_BOARD_EVB)
    //mfp switch
    act_writel((act_readl(MFP_CTL0) & (~MFP_CTL0_GPIOA22_MASK)), MFP_CTL0);
    // ir rx mode
    act_writel((act_readl(MFP_CTL0) | (6 << MFP_CTL0_GPIOA22_SHIFT)), MFP_CTL0);
#else
    //mfp switch
    act_writel((act_readl(MFP_CTL2) & (~MFP_CTL2_GPIO_B11_MASK)), MFP_CTL2);
    // ir rx mode
    act_writel((act_readl(MFP_CTL2) | (4 << MFP_CTL2_GPIO_B11_SHIFT)), MFP_CTL2);
#endif
    // ir clk enable
    act_writel((act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_IRCLKEN)), CMU_DEVCLKEN);

    //act_writel((act_readl(MRCR) & (~(1 << MRCR_UART1Reset))), MRCR);

    //set IR normal mode;
    act_writel((act_readl(MRCR) | (1 << MRCR_UART1Reset)), MRCR);

    //过滤毛刺15*5us
    //act_writel((act_readl(IRC_CTL) & (~IRC_CTL_DBC_MASK)), IRC_CTL);
    //act_writel((act_readl(IRC_CTL) | (15 << IRC_CTL_DBC_SHIFT)), IRC_CTL);
    //bit 16 set as 1过滤毛刺,默认200us
#if((CASE_BOARD_TYPE == CASE_BOARD_EVB)||(CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825))
    act_writel((act_readl(IRC_CTL) | 0x100), IRC_CTL);
#else
    act_writel((act_readl(IRC_CTL) | (1<<16)), IRC_CTL);
#endif

    //IRMS--nec;
    act_writel((act_readl(IRC_CTL) & (~IRC_CTL_ICMS_MASK)), IRC_CTL);

    //nec
    act_writel((act_readl(IRC_CTL) | (1 << IRC_CTL_ICMS_SHIFT)), IRC_CTL);

    //IRC enable;
    act_writel((act_readl(IRC_CTL) | (1 << IRC_CTL_IRE)), IRC_CTL);
    //IRC IRQ enable;
    act_writel((act_readl(IRC_CTL) | (1 << IRC_CTL_IIE)), IRC_CTL);

    //clear error pendings and irq pending and User code don‘t match pending bit and
    //Key data code don‘t match pending bit
    act_writel((act_readl(IRC_STA) | (uint32) 0x75), IRC_STA);
    //init user code
    if (ir_code == IR_CODE_FF00)
    {
        g_current_usercode = IR_CODE_FF00;
        act_writel((uint32) 0xff00, IRC_CC);
    }
    else if (ir_code == IR_CODE_BF00)
    {
        g_current_usercode = IR_CODE_BF00;
        act_writel((uint32) 0xbf00, IRC_CC);
    }
    else //==IR_CODE_7F80
    {
        g_current_usercode = IR_CODE_7F80;
        act_writel((uint32) 0x7f80, IRC_CC);
    }

}

/******************************************************************************
 * \par  Description:   adc按键初始化
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-23pm         1.0             review this file
 *******************************************************************************/
#if (SUPPORT_ADC_KEY == 1)
void key_adc_init(void)
{
    //初始化线控按键 LRADC1234 设置后需要等待10ms才能读出正确的数据，
    //否则可能全0,建议放到welcome进行初始化，则不需要等待10ms

    //select PMUADC_CTL_LRADC_FS 250khz
    act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_BATADC_FS)), PMUADC_CTL);

#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    //enable LRADC2 PMUADC_CTL_LRADC2_EN
    act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_LRADC234_EN)), PMUADC_CTL);

    act_writel((act_readl(AD_Select1) & (~AD_Select1_GPIOA22_MASK)), AD_Select1);

    act_writel((act_readl(AD_Select1) | (0x01 << AD_Select1_GPIOA22_SHIFT)), AD_Select1);

#else
    //enable LRADC1
    act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_LRADC1_EN)), PMUADC_CTL);
    //enable LRADC234
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
    act_writel((act_readl(GPIOB0_CTL) | (1<<GPIOB0_CTL_GPIOB0_MFP_SHIFT)), GPIOB0_CTL);

#endif
    //rtc register
    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ;//wait for register update
    }

    sys_mdelay(20);
}
#endif

/******************************************************************************
 * \par  Description:   按键初始化入口
 * \param[in]     null0,null1保留参数，
 * \ param[in]    key_para 是在config.txt中配置按键信息
 * \param[out]   none
 * \return           返回0
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-23pm         1.0             review this file
 *******************************************************************************/

int key_init(void* null0, void *null1, uint32 key_para)
{
#ifdef SUPPORT_UART_SIM_KEY
    uart1_rx_init(115200);
#endif
    //gpio key init
#if( SUPPORT_HPY_DRIVER  == 1)
    key_gpio_init();
#endif

    //ir key init
#if (SUPPORT_IR_DRIVER == 1)
    key_ir_init((uint8)(key_para>>16));
#endif

    //lradc key init
#if (SUPPORT_ADC_KEY == 1)
    key_adc_init();
#endif

    //touch key init
#if (SUPPORT_TK_DRIVER == TK_DRIVER_INNER)
    key_tk_init();
#endif

#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)

    //触摸屏断电
    act_writel(act_readl(EXTERN_TK_PLANE_GPIOOUT) | EXTERN_TK_PLANE_PIN, EXTERN_TK_PLANE_GPIOOUT);
    act_writel(act_readl(EXTERN_TK_PLANE_GPIODAT) & (~EXTERN_TK_PLANE_PIN), EXTERN_TK_PLANE_GPIODAT);

#endif

    //电池充电方式
    g_key_bat_charge_mode = (uint8) (key_para >> 24);

    key_bat_charge_init((uint8) (key_para >> 8), (uint8) key_para);
    key_sampling_battery_voltage_init();

    //for scan irq 20ms
    g_time_irq = sys_set_irq_timer1(key_scan_int_process, 20);

    //request ir irq
#if (SUPPORT_IR_DRIVER == 1)
    sys_request_irq(IRQ_UART1, key_ir_int_process);
#endif

#if (SUPPORT_TK_DRIVER == TK_DRIVER_INNER)
    //request tk irq
    sys_request_irq(IRQ_TOUCHKEY, key_tk_int_process);
#endif

    //使能AUX GPIO IN
#if (SUPPORT_AUX_DETECT == DETECT_BY_GPIO)
    act_writel((act_readl(AUX_GPIO_UPEN) | AUX_GPIO_PIN), AUX_GPIO_UPEN);
    act_writel((act_readl(AUX_GPIO_IN) | AUX_GPIO_PIN), AUX_GPIO_IN);
#endif
#if (SUPPORT_AUX_DETECT == DETECT_BY_SIO)
    act_writel((act_readl(MFP_CTL3) & (~MFP_CTL3_VRO_MASK) | (2 << MFP_CTL3_VRO_SHIFT)), MFP_CTL3); //MFP选择SIO7
    act_writel((act_readl(DAC_ANACTL) & (~(1 << DAC_ANACTL_OPVROEN))), DAC_ANACTL); //VRO与SIO7复用，需关闭VRO
    act_writel((act_readl(SIO_INEN) | AUX_SIO_PIN), SIO_INEN);
#endif

    //使能HP GPIO IN
#if (SUPPORT_HP_DETECT == DETECT_BY_GPIO)
    act_writel((act_readl(HP_GPIO_UPEN) | HP_GPIO_PIN), HP_GPIO_UPEN);
    act_writel((act_readl(HP_GPIO_IN) | HP_GPIO_PIN), HP_GPIO_IN);
#endif

    //使能SD卡GPIO IN
#if (SUPPORT_CARD_DETECT == DETECT_BY_GPIO)
    act_writel((act_readl(CARD_GPIO_UPEN) | CARD_GPIO_PIN), CARD_GPIO_UPEN);
    act_writel((act_readl(CARD_GPIO_IN) | CARD_GPIO_PIN), CARD_GPIO_IN);
#endif

    adjust_freq_do_init();

    return 0;
}

module_init(key_init)

