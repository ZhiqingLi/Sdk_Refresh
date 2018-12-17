/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_power_phy.c
 * \brief    开关机电源硬件接口
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "ap_config.h"

/************************************** ********************/
/*!
 * \par  Description:
 *  config等待RTCVCC域的寄存器写入
 * \param[in]    none
 * \param[out]   none
 * \return       non
 * \ingroup      config
 * \note
 ******************************************************/
void config_flush_rtc_reg(void)
{
    uint32 i;

    act_writel(0xA596, RTC_REGUPDATA);
    while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ;//nothing for QAC
    }

    for (i = 0; i < 256 * 4; i++)
    {
        act_writel((act_readl(WD_CTL) | 0x01), WD_CTL);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  config检测电源是否插入
 * \param[in]    none
 * \param[out]   none
 * \return       TRUE电源接入FALSE没有电源
 * \ingroup      config
 * \note
 *******************************************************************************/
bool __section__(".rcode") detect_power_on(void)
{
    if((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0)//电源接入
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/************************************** ********************/
/*!
 * \par  Description:
 *  config使能play键的唤醒功能
 默认是电源插入和长按唤醒
 * \param[in]    none
 * \param[out]   none
 * \return       non
 * \ingroup      config
 * \note
 ******************************************************/
void en_play_wake_up(bool standby_enter)
{
    uint32 tmp;
    tmp = act_readl(WKEN_CTL);

    tmp |= (1 << WKEN_CTL_SHORT_WKEN) //play短按唤醒
    | (1 << WKEN_CTL_LONG_WKEN); //play长按唤醒

    if (standby_enter == TRUE)
    {
        tmp |= (1 << WKEN_CTL_BT_WK_EN); //BT唤醒
    }
    else
    {
        tmp &= ~(1 << WKEN_CTL_BT_WK_EN); //禁止BT唤醒
    }

    act_writel(tmp, WKEN_CTL); //设置唤醒方式
    config_flush_rtc_reg();


    /** 清pending
    */
    if(((act_readl(INTC_PD) & 0x01) != 0)&&(standby_enter == TRUE))
    {
    }
    else
    {
        act_writel(act_readl(WAKE_PD), WAKE_PD);
    }
}

//进入standby S3BT/S4模式，DV5V，alarm，外部中断，ADC_KEY唤醒，ON_OFF键唤醒，BT唤醒
//不再返回，通过各种唤醒手段将系统reset
void enter_standby_S3S4(bool standby_enter)
{
    UINT32 i;
    msg_apps_t msg;
    en_play_wake_up(standby_enter);
	
    /** 保证anti-pop之前IC PA 初始化完成
    */
    while (g_app_info_state.inner_pa_inited == FALSE)
    { //等待 IC PA 初始化完成
        sys_os_time_dly(1);
    }
    
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
//ATS2823 肯定不能支持屏幕，不会有时钟的需求，将时钟高校低关闭，降低电流
    act_writel(act_readl(RTC_CTL) | (1 << 2), RTC_CTL);
    act_writel(act_readl(HCL_CTL) & (~(1 << 0)), HCL_CTL);
    act_writel(0xA596, RTC_REGUPDATA);
    while (0x5A69 != act_readl(RTC_REGUPDATA))
    {
        ;//delay for RTC update complete
    }
#endif    

    //antipop，去掉关机杂音
    disable_pa(g_app_info_state.aout_mode);

    if (standby_enter == TRUE)
    {
        PRINT_INFO("ENTER S3BT");

        //设置进入S3BT标志
        act_writel(act_readl(RTC_BAK0) | (1 << MY_RTC_S3BT), RTC_BAK0);
        config_flush_rtc_reg();
        
        //设置进入S3BT标志
        if((act_readl(INTC_PD) & 0x01) != 0)
        {
            PRINT_INFO("reboot exit S3BT");

            //act_writel(0x1e, WD_CTL); //使能S3BT
            //while(1);
           
             //打开PA，PA开机后常开
            if (g_app_info_state.aout_mode == AO_SOURCE_DAC)
            {
                uint8 pa_mode = (uint8) com_get_config_default(SETTING_AUDIO_INNER_PA_SET_MODE);
                enable_pa(pa_mode, 0, AO_SOURCE_DAC);
            }
            else if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
            {
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
                //DEMO IC:A0,A1,A2,VRO_S
                enable_pa(3, 0, AO_SOURCE_I2S);
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)
                //DVB:B7,B8,B9,A6
                enable_pa(4, 0, AO_SOURCE_I2S);
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823)
                //DVB:AOUTL,AOUTR,VRO,VROS
                enable_pa(0, 0, AO_SOURCE_I2S);
#else
                //DEMO IC:A0,A1,A2,A3
                enable_pa(1, 0, AO_SOURCE_I2S);
#endif
            } 
            else
            {
                ;//nothing
            }

            //config ap重新执行一次开机流程
            msg.content.data[0] = APP_ID_CONFIG;
            msg.content.data[1] = SWITCH_ON;
            msg.type = MSG_CREAT_APP;
            
            send_async_msg(APP_ID_MANAGER, &msg);
            return;
        }
        else
        {
            //SPEC上 需要延时后再disable S1
            act_writel(0x03, POWER_CTL); //使能S3BT
            for(i = 0;i < 100;i++)
            {
                ;//delay
            }
            act_writel(0x02, POWER_CTL); //disable S1
        }
    }
    else
    {
        PRINT_INFO("poweroff");
        /** 关机之前需要等待RTC寄存器刷完.
        */
        while (0x5A69 != act_readl(RTC_REGUPDATA))
        {
            ;//delay for RTC update complete
        }
        act_writel(0x00, POWER_CTL); //disable All，休眠进S4
    }

    //设置最长时间的看门狗重启时间，如果这段时间内还未进入S3BT或S4,则系统直接重启
    ENABLE_WATCH_DOG(0x07);
    while (1)
    {
        ;//nothing
    }
}

//从该接口返回时，必须保证系统以正常模式运行，各模块都是正常可用的
app_result_e sys_power_off(bool standby_enter)
{
    app_result_e result = RESULT_NULL;

    //关掉LED数码管，LED灯
#if (SUPPORT_LED_DRIVER == 1)
    if(g_comval.support_led_display == 1)
    {
        led_clear_screen();
        led_sleep_display();
    }
#endif

#if (SUPPORT_OUTER_CHARGE == 1)
    if ((sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER) && (standby_enter == FALSE))
    {
        if (g_app_info_state.extern_bat_in == 1)
        {
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
            key_extern_tk_op(TK_OP_OFF);
#endif
            g_app_info_state.extern_bat_in = 0;
        }
    }
#endif

    while ((act_readl(ONOFF_KEY) & (1 << ONOFF_KEY_ONOFF_PRESS_0)) != 0)
    {
        ;//等待放开
    }

    SPEAKER_OFF();
    SPEAKER_CONTROL_DISABLE();

#if (SUPPORT_OUTER_CHARGE == 1)
    if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        enter_standby_S3S4(standby_enter);
    }
    else
#endif
    {
        if (detect_power_on() == TRUE) //电源插入，进入关机充电
        {
            result = config_charging(SWITCH_OFF);
            if (result == RESULT_POWER_OFF)
            {
                //如果S3BT进入，那么即使这里检测到关机返回值，仍然按照S3BT的流程
                //继续执行，防止按正常关机掉电流程执行，蓝牙controller连接断开的
                //情况
                //standby_enter = FALSE;
                goto enter_power_off;
            }
        }
        else//3种“关机”模式选择一种
        {
            enter_power_off: enter_standby_S3S4(standby_enter);
        }
    }

    SPEAKER_CONTROL_ENABLE();
    SPEAKER_ON();

    //唤醒LED数码管
#if (SUPPORT_LED_DRIVER == 1)
    if(g_comval.support_led_display == 1)
    {
        led_wake_up_display();
        led_clear_screen();
        led_display(0, NUM_ERROR, 1);
        led_display(1, NUM_ERROR, 1);
        led_display(2, NUM_ERROR, 1);
        led_display(3, NUM_ERROR, 1);
    }
#endif

    return result;
}

