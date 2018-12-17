/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：外部设备检测，包括USB，UHOST，CARD，AUX等。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "key_interface.h"
#include "uhost.h"

uint8 usb_detect_counter;
uint8 card_detect_counter;

#if (SUPPORT_AUX_DETECT != 0)
static uint8 check_aux(void);
#endif

#if (SUPPORT_HP_DETECT != 0)
static uint8 check_hp(void);
#endif

#if ((SUPPORT_CARD_DETECT != 0) && (SUPPORT_CARD_DETECT != DETECT_BY_CMD))
static uint8 check_card(void);
#endif

//返回值：0表示不在，1表示在，2表示不确定
typedef uint8 (*check_hardware)(void);

//check_in：TRUE表示检测插入，FALSE表示检测拔出
//返回值：0表示没有拔插，1表示插入，2表示拔出
static uint8 check_device_in_out(check_hardware cbk, bool check_in);

void check_card_status(uint32 detect_mode);
void check_linein_status(uint32 detect_mode);
void check_headphone_status(uint32 detect_mode);
void usb_switch_handle(uint32 detect_mode);
void switch_to_uhost_detect(void);
void switch_to_udevice_detect(void);

/******************************************************************************/
/*!
 * \par  Description:
 *    外部设备检测服务函数,200ms执行一次
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 *******************************************************************************/
void key_inner_peripheral_detect_handle(uint32 detect_mode)
{
    //卡检测函数
    check_card_status(detect_mode);

    //LINEIN检测函数
    check_linein_status(detect_mode);

    //HEADPHONE检测函数
    check_headphone_status(detect_mode);

    //USB/UHOST切换以便系统检测
    usb_switch_handle(detect_mode);
}

/******************************************************************************/
/*!
 * \par  Description:
 *  卡检测,由于系统卡检测关闭中断的时间太长,使用应用主动检测方式
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup
 * \note
 *******************************************************************************/
void check_card_status(uint32 detect_mode)
{
    int8 temp_card;
    uint8 ret_val = 0;
    uint16 msg = 0;

    if ((detect_mode & (PER_DETECT_CARD | PER_DETECT_CARD_FORCE)) == 0)
    {
        return;
    }

#if (SUPPORT_CARD_DETECT == DETECT_BY_CMD)
    if ((detect_mode & PER_DETECT_CARD) != 0)
    {
        card_detect_counter++;
    }

    if ((detect_mode & PER_DETECT_CARD_FORCE) != 0)
    {
        card_detect_counter = CARD_DETECT_TIMER;
    }

    if (card_detect_counter >= CARD_DETECT_TIMER)
    {
        card_detect_counter = 0;

        temp_card = (int8) sys_detect_disk(DRV_GROUP_STG_CARD);

        if (((detect_mode & PRE_DETECT_CARD_FOR_IN) != 0) && (temp_card == 0))
        {
            //判断到卡插入,发送消息到系统消息队列
            msg = MSG_SYS_SD_IN;
        }
        else if (((detect_mode & PRE_DETECT_CARD_FOR_IN) == 0) && (temp_card == -1))
        {
            //判断到卡拔出,发送消息到系统消息队列
            msg = MSG_SYS_SD_OUT;
        }
        else
        {
            ; //nothing
        }
    }
#elif (SUPPORT_CARD_DETECT != 0)
    ret_val = check_device_in_out(check_card, ((detect_mode & PRE_DETECT_CARD_FOR_IN) != 0));
    if (ret_val == 1)
    {
        msg = MSG_SYS_SD_IN;
    }
    else if (ret_val == 2)
    {
        msg = MSG_SYS_SD_OUT;
    }
    else
    {
        ; //nothing
    }
#endif

    if (msg != 0)
    {
        sys_mq_send(MQ_ID_SYS, &msg);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \   void check_linein_status(void)
 * \param[in]    none
 * \param[in]    none
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void check_linein_status(uint32 detect_mode)
{
    uint8 ret_val = 0;
    uint16 msg = 0;

    if ((detect_mode & PER_DETECT_AUX) == 0)
    {
        return;
    }

#if (SUPPORT_AUX_DETECT != 0)
    ret_val = check_device_in_out(check_aux, (bool)((detect_mode & PER_DETECT_AUX_FOR_IN) != 0));
    if (ret_val == 1)
    {
        msg = MSG_SYS_LINEIN_IN;
    }
    else if (ret_val == 2)
    {
        msg = MSG_SYS_LINEIN_OUT;
    }
    else
    {
        ; //nothing
    }
#else
    if ((detect_mode & PER_DETECT_AUX_FOR_IN) != 0) //当前为line in线拔开状态
    {
        msg = MSG_SYS_LINEIN_IN;
    }
#endif

    if (msg != 0)
    {
        sys_mq_send(MQ_ID_SYS, &msg);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \   void check_headphone_status(void)
 * \param[in]    none
 * \param[in]    none
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void check_headphone_status(uint32 detect_mode)
{
    uint8 ret_val = 0;
    uint16 msg = 0;

    if ((detect_mode & PER_DETECT_HP) == 0)
    {
        return;
    }

#if (SUPPORT_HP_DETECT != 0)
    ret_val = check_device_in_out(check_hp, ((detect_mode & PER_DETECT_HP_FOR_IN) != 0));
    if (ret_val == 1)
    {
        msg = MSG_SYS_HP_IN;
    }
    else if (ret_val == 2)
    {
        msg = MSG_SYS_HP_OUT;
    }
    else
    {
        ; //nothing
    }
    if (msg != 0)
    {
        sys_mq_send(MQ_ID_SYS, &msg);
    }
#endif


}

/******************************************************************************/
/*!
 * \par  Description:
 *    USB检测切换服务函数,200ms切换一次
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 *******************************************************************************/
void usb_switch_handle(uint32 detect_mode)
{
    if (((detect_mode & PER_DETECT_USBUHOST_SWITCH) == 0) && ((detect_mode & PER_DETECT_UHOST_FORCE) == 0)
            && ((detect_mode & PER_DETECT_USB_FORCE) == 0))
    {
        return;
    }

    if ((detect_mode & PER_DETECT_UHOST_FORCE) != 0)
    {
        DISABLE_USB_DETECT();
        DISABLE_UHOST_DETECT();

        //切换到host检测
        switch_to_uhost_detect();

        ENABLE_UHOST_DETECT();

        //delay 200ms for detect
        sys_os_time_dly(20);

        usb_detect_counter = 1;
        return;
    }

    if ((detect_mode & PER_DETECT_USB_FORCE) != 0)
    {
        DISABLE_USB_DETECT();
        DISABLE_UHOST_DETECT();

        //切换到usb检测
        switch_to_udevice_detect();

        sys_os_time_dly(20);

        ENABLE_USB_DETECT();

        usb_detect_counter = 1;

        //出去后还会延时300ms等待检测

        return;
    }

    if ((GLOBAL_KERNEL_BACKDOOR_FLAG & BACKDOOR_USB_STATUS_CHANGED) != 0)
    {
        return;
    }

    if ((GLOBAL_KERNEL_BACKDOOR_FLAG & BACKDOOR_USB_PLUGIN) == 0)
    {
        if ((GLOBAL_KERNEL_BACKDOOR_FLAG & BACKDOOR_UHOST_PLUGIN) == 0)
        {

            //当前是device状态
            if (get_usb_port_state() == USB_PORT_DEVICE)
            {
                if (usb_detect_counter == USBUHOST_DETECT_TIMER)
                {
                    DISABLE_USB_DETECT();
                    DISABLE_UHOST_DETECT();

                    //切换到host检测
                    switch_to_uhost_detect();

                    usb_detect_counter = 0;
                }
                else
                {
                    if (usb_detect_counter == 0)
                    {
                        ENABLE_USB_DETECT();
                    }
                    usb_detect_counter++;
                }

            }
            else
            {
                if (usb_detect_counter == USBUHOST_DETECT_TIMER)
                {
                    DISABLE_USB_DETECT();
                    DISABLE_UHOST_DETECT();

                    //切换到device检测
                    switch_to_udevice_detect();

                    usb_detect_counter = 0;
                }
                else
                {
                    if (usb_detect_counter == 0)
                    {
                        ENABLE_UHOST_DETECT();
                    }
                    usb_detect_counter++;
                }
            }
        }
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  3?ê??ˉ?ì2ausb??ê?·??óè?￡?′óusb device×ausb host
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
void switch_to_uhost_detect(void)
{
    /* enable softID and softVBUS. SoftID=0, SoftVBUS=1, FSM will go to A_idle.  */
    act_writeb((act_readb(IDVBUSCTRL) & (uint8) 0xf0) | (uint8) 0x07, IDVBUSCTRL);

    /* enable uhost Line status detect, enable 15Kohm DP/DM pull down, disable 500Kohm DP/DM pull up resistor */

    act_writeb((act_readb(DPDMCTRL) & (uint8) 0xf0) | DPDMCTRL_LineDetEN_OR, DPDMCTRL);

    //  UHOST_POWER_ENABLE();

    //  UHOST_POWER_OPEN();
}

/******************************************************************************/
/*!
 * \par  Description:
 *  3?ê??ˉ?ì2ausb??ê?·??óè?￡?′óusb device×ausb host
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
void switch_to_udevice_detect(void)
{
    /* enable softID and softVBUS. SoftID=1, SoftVBUS=1  */
    act_writeb((act_readb(IDVBUSCTRL) & (uint8) 0xf0) | (uint8) 0x0f, IDVBUSCTRL);

    /* enable uhost Line status detect */
    act_writeb((act_readb(DPDMCTRL) & (uint8) 0xf0) | 0x1f, DPDMCTRL);

    //    UHOST_POWER_ENABLE();

    //    UHOST_POWER_CLOSE();
}

////////////////////////////////////////////////////////////

#if (SUPPORT_AUX_DETECT != 0)
static uint8 check_aux(void)
{
    uint8 linein_flag = 0;

#if (SUPPORT_AUX_DETECT == DETECT_BY_ADCKEY)

    uint8 adc_data;
    adc_data = act_readb(LRADC1_DATA) & 0x7F;
    if ((adc_data >= (AUX_ADC_VALUE - 3)) && (adc_data <= (AUX_ADC_VALUE + 3)))
    {
        linein_flag = 1;
    }
    else if (adc_data > (AUX_ADC_VALUE + 3))
    {
        linein_flag = 0;
    }
    else
    {
        linein_flag = 2;
    }

#elif (SUPPORT_AUX_DETECT == DETECT_BY_GPIO)

#if (AUX_GPIO_EN_LEVEL == ENABLE_LEVEL_LOW)
    if ((act_readl(AUX_GPIO_DATA) & AUX_GPIO_PIN) == 0)
    {
        linein_flag = 1;
    }
    else
    {
        linein_flag = 0;
    }
#else
    if ((act_readl(AUX_GPIO_DATA) & AUX_GPIO_PIN) != 0)
    {
        linein_flag = 1;
    }
    else
    {
        linein_flag = 0;
    }
#endif

#elif (SUPPORT_AUX_DETECT == DETECT_BY_GPIOB0)

#if (AUX_GPIO_EN_LEVEL == ENABLE_LEVEL_LOW)
    if ((act_readl(GPIOB0_CTL) & (1 << GPIOB0_CTL_GPIOB0DAT)) == 0)
    {
        linein_flag = 1;
    }
    else
    {
        linein_flag = 0;
    }
#else
    if ((act_readl(GPIOB0_CTL) & (1 << GPIOB0_CTL_GPIOB0DAT)) != 0)
    {
        linein_flag = 1;
    }
    else
    {
        linein_flag = 0;
    }
#endif

#else //DETECT_BY_SIO

#if (AUX_SIO_EN_LEVEL == ENABLE_LEVEL_LOW)
    if ((act_readl(SIO_DAT) & AUX_SIO_PIN) == 0)
    {
        linein_flag = 1;
    }
    else
    {
        linein_flag = 0;
    }
#else
    if ((act_readl(SIO_DAT) & AUX_SIO_PIN) != 0)
    {
        linein_flag = 1;
    }
    else
    {
        linein_flag = 0;
    }
#endif

#endif

    return linein_flag;
}
#endif

#if (SUPPORT_HP_DETECT != 0)
static uint8 check_hp(void)
{
    uint8 hp_flag = 0;

#if (HP_GPIO_EN_LEVEL == ENABLE_LEVEL_LOW)
    if ((act_readl(HP_GPIO_DATA) & HP_GPIO_PIN) == 0)
    {
        hp_flag = 1;
    }
    else
    {
        hp_flag = 0;
    }
#else
    if ((act_readl(HP_GPIO_DATA) & HP_GPIO_PIN) != 0)
    {
        hp_flag = 1;
    }
    else
    {
        hp_flag = 0;
    }
#endif

    return hp_flag;
}
#endif

#if ((SUPPORT_CARD_DETECT != 0) && (SUPPORT_CARD_DETECT != DETECT_BY_CMD))
static uint8 check_card(void)
{
    uint8 card_flag = 0;

#if (CARD_GPIO_EN_LEVEL == ENABLE_LEVEL_LOW)
    if ((act_readl(CARD_GPIO_DATA) & CARD_GPIO_PIN) == 0)
    {
        card_flag = 1;
    }
    else
    {
        card_flag = 0;
    }
#else
    if ((act_readl(CARD_GPIO_DATA) & CARD_GPIO_PIN) != 0)
    {
        card_flag = 1;
    }
    else
    {
        card_flag = 0;
    }
#endif

    return card_flag;
}
#endif

//check_in：TRUE表示检测插入，FALSE表示检测拔出
//返回值：0表示没有拔插，1表示插入，2表示拔出
static uint8 check_device_in_out(check_hardware cbk, bool check_in)
{
    uint8 ret_val = 0;
    uint8 cur_flag = 0;
    uint8 last_flag = 0;
    uint8 debounce = 0;

    while(1)
    {
        cur_flag = cbk();

        //检测插入
        if (check_in == 1)
        {
            if (cur_flag == 1)
            {
                if (last_flag == 0)
                {
                    debounce = 0;
                }
                else
                {
                    debounce++;
                    if (debounce < 2)
                    {
                        sys_os_time_dly(1);
                    }
                    else
                    {
                        ret_val = 1;
                        break;
                    }
                }
            }
            else//包括ADC检测不确定的情况，检测流程结束
            {
                break;
            }
        }
        //检测拔出
        else
        {
            if (cur_flag == 0)
            {
                if (last_flag == 1)
                {
                    debounce = 0;
                }
                else
                {
                    debounce++;
                    if (debounce < 2)
                    {
                        sys_os_time_dly(1);
                    }
                    else
                    {
                        ret_val = 2;
                        break;
                    }
                }
            }
            else//包括ADC检测不确定的情况，检测流程结束
            {
                break;
            }
        }

        last_flag = cur_flag;
    }

    return ret_val;
}
