/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用私有（系统）消息处理勾子函数，在私有（系统）消息响应之前执行一些
 *       公共处理，包括转换消息，设置系统状态，系统计时器清零，唤醒UI显示等。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/******************************************************************************/
/*!
 * \par  Description:
 *    前台应用私有消息勾函数。
 * \param[in]    private_msg 获取到的私有消息指针
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 *******************************************************************************/
void com_app_msg_hook(private_msg_t *private_msg)
{
    msg_apps_type_e msg_type = private_msg->msg.type;
    bool vip_event_flag = FALSE;

    switch (msg_type)
    {
        case MSG_SD_IN:
        if (get_card_state() == CARD_STATE_CARD_NULL)
        {
            change_card_state(CARD_STATE_CARD_IN);
            g_app_info_state.need_update_playlist |= CARD_NEED_MASK;
            //for disk play to clr bk info
            g_ap_switch_var.card_in_out_flag = 1;
            vip_event_flag = TRUE;
        }
        else
        {
            //无效的消息
            private_msg->msg.type = MSG_NULL;
        }
        break;
        case MSG_SD_OUT:
        if (get_card_state() != CARD_STATE_CARD_NULL)
        {
            change_card_state(CARD_STATE_CARD_NULL);
            g_app_info_state.need_update_playlist |= CARD_NEED_MASK;
            vip_event_flag = TRUE;
            g_ap_switch_var.card_in_out_flag = 1;
        }
        else
        {
            //无效的消息
            private_msg->msg.type = MSG_NULL;
        }
        break;

        case MSG_UH_IN:
        if (get_uhost_state() == UHOST_STATE_UHOST_NULL)
        {
            change_uhost_state(UHOST_STATE_UHOST_IN);
            g_app_info_state.need_update_playlist |= UHOST_NEED_MASK;
            vip_event_flag = TRUE;
            GLOBAL_KERNEL_BACKDOOR_FLAG &= ~(BACKDOOR_USB_STATUS_CHANGED);
            ENABLE_UHOST_DETECT();
            g_ap_switch_var.uhost_in_out_flag = 1;
        }
        else
        {
            //无效的消息
            private_msg->msg.type = MSG_NULL;
        }
        break;
        case MSG_UH_OUT:
        if (get_uhost_state() != UHOST_STATE_UHOST_NULL)
        {
            change_uhost_state(UHOST_STATE_UHOST_NULL);
            g_app_info_state.need_update_playlist |= UHOST_NEED_MASK;
            vip_event_flag = TRUE;
            GLOBAL_KERNEL_BACKDOOR_FLAG &= ~(BACKDOOR_USB_STATUS_CHANGED);
            g_ap_switch_var.uhost_in_out_flag = 1;
        }
        else
        {
            //无效的消息
            private_msg->msg.type = MSG_NULL;
        }
        break;

        case MSG_LINEIN_IN:
        if (get_linein_state() == LINEIN_STATE_LINEIN_NULL)
        {
            change_linein_state(LINEIN_STATE_LINEIN_IN);
            vip_event_flag = TRUE;
        }
        else
        {
            //无效的消息
            private_msg->msg.type = MSG_NULL;
        }
        break;
        case MSG_LINEIN_OUT:
        if (get_linein_state() != LINEIN_STATE_LINEIN_NULL)
        {
            change_linein_state(LINEIN_STATE_LINEIN_NULL);
            vip_event_flag = TRUE;
        }
        else
        {
            //无效的消息
            private_msg->msg.type = MSG_NULL;
        }
        break;

        case MSG_USB_STICK:
        /* wait 50ms for USB-connect stably. */
        sys_os_time_dly(20);

        if (get_cable_state() == CABLE_STATE_CABLE_NULL)
        {
            if ((act_readb(DPDMCTRL) & (1 << DPDMCTRL_Plugin)) == 0)
            {
                //ADAPTOR适配线插入
                PRINT_INFO_INT("async msg hook:", MSG_ADAPTOR_IN);
                private_msg->msg.type = MSG_ADAPTOR_IN;
                change_cable_state(CABLE_STATE_ADAPTOR_IN);
            }
            else
            {
                //有usb线插入
                change_cable_state(CABLE_STATE_CABLE_IN);
            }

            if ((g_this_app_info->app_id != APP_ID_CONFIG) && ((g_ap_switch_var.app_func_support & (uint32) (1
                    << APP_FUNC_USOUND)) == 0))
            {
                private_msg->msg.type = MSG_NULL;
            }
            vip_event_flag = TRUE;

            GLOBAL_KERNEL_BACKDOOR_FLAG &= ~(BACKDOOR_USB_STATUS_CHANGED);

            ENABLE_USB_DETECT();
        }
        else
        {
            //无效的device消息
            private_msg->msg.type = MSG_NULL;
        }
        break;
        case MSG_USB_UNSTICK:
        if (get_cable_state() != CABLE_STATE_CABLE_NULL)
        {
            if (get_cable_state() == CABLE_STATE_ADAPTOR_IN) //ADAPTOR适配线拔出
            {
                PRINT_INFO_INT("async msg hook:", MSG_ADAPTOR_OUT);
                private_msg->msg.type = MSG_ADAPTOR_OUT; //改为MSG_ADAPTOR_OUT消息
            }
            change_cable_state(CABLE_STATE_CABLE_NULL);

            if ((g_this_app_info->app_id != APP_ID_CONFIG) && ((g_ap_switch_var.app_func_support & (uint32) (1
                    << APP_FUNC_USOUND)) == 0))
            {
                private_msg->msg.type = MSG_NULL;
            }
            vip_event_flag = TRUE;

            GLOBAL_KERNEL_BACKDOOR_FLAG &= ~(BACKDOOR_USB_STATUS_CHANGED);
        }
        else
        {
            //无效的device消息
            private_msg->msg.type = MSG_NULL;
        }
        break;

        /*耳机（天线）插入*/
        case MSG_HP_IN:
        change_headphone_state(HEADPHONE_STATE_IN);
        break;

        /*耳机（天线）拔出*/
        case MSG_HP_OUT:
        change_headphone_state(HEADPHONE_STATE_NULL);
        break;

        case MSG_POWER_OFF:
        case MSG_RTCALARM:
        case MSG_BT_CONNECTED:
        case MSG_BT_DISCONNECT:
        vip_event_flag = TRUE;
        break;

        case MSG_LOW_POWER:
        if (g_app_info_state.bat_value == 0)
        {
            vip_event_flag = TRUE;
        }
        break;

        default:
        break;
    }

    if (vip_event_flag == TRUE)
    {
        //省电关机,屏保计时清0
        g_sys_counter.poweroff_counter = 0;
        g_sys_counter.light_counter = 0;

        if (get_backlight_state() == BACKLIGHT_STATE_OFF)
        {
            com_set_display_onoff(TRUE);
        }

        //提高频率
        g_sys_counter.ui_idle_counter = 0;
        if (g_app_info_state_all.ui_status == UI_STATUS_IDLE_LOWFREQ)
        {
            adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
            g_app_info_state_all.ui_status = UI_STATUS_IDLE;
        }
    }
}

/*! \endcond */
