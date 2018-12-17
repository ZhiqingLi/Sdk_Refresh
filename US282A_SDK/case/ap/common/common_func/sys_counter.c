/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 系统2Hz软定时器HANDLE，监测项目包括：卡热拔插，AUX热拔插，省电关机，
 *       LED数码管关屏，充电和电量，APK/APP状态。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/*! \cond COMMON_API */

static sys_status_e get_cur_sys_status(void);

void __section__(".bank") enter_lower_mode(void)
{
    msg_apps_t msg;

    //调试模式不响应进入S2,S3状态
    if (g_app_info_state.stub_tools_type == 0)
    {
        //广播进入低功耗消息
        if (sys_comval->lowpower_mode == LOW_POWER_MODE_S2)
        {
            msg.type = MSG_SYSTEM_ENTER_S2;
        }
        else
        {
            msg.type = MSG_SYSTEM_ENTER_S3;
            act_writel(act_readl(WAKE_PD), WAKE_PD); //清pending
            sys_mdelay(5);
        }
        broadcast_msg(&msg, TRUE);    
    }
}
/******************************************************************************/
/*!
 * \par  Description:
 *    系统计时软定时器服务例程，处理屏幕背光，屏保，省电关机，睡眠关机，回到正在
 *    播放等。
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 * \li  系统定时功能，包括背光，屏保，省电关机，睡眠关机，app休眠，由前台AP进行计时。
 *      即在前台AP创建一个定时器，专用于上面五个功能计时器的计时，在 get_app_msg 中
 *      调用 handle_timers 去做检测，一旦定时已到，触发对应定时功能。
 * \li  屏幕背光完全由 common 模块控制，无须用户关心，用户可以通过 get_backlight_state
 *      来获取背光状态。
 * \li  屏幕保护同样完全由 common 模块控制，无须用户关心。等需要进入屏幕保护时，先发送
 *      异步消息 MSG_ENTER_SCREEN_SAVE 给当前AP，在 com_message_box 中进入屏保。
 * \li  省电关机和睡眠（定时）关机则是广播 MSG_POWER_OFF 消息，manager ap 在收到该消息
 *      后，发送 MSG_APP_QUIT 让前台和后台AP自己退出。
 * \li  返回正在播放界面通过以"返回正在播放的模式"创建对应的前台AP或者发送异步消息 back_to_playing
 *      让前台AP自己回到正在播放场景。返回正在播放界面需要AP的正在播放场景协助，即在进入场景时
 *      change_app_state(APP_STATE_PLAYING)，在退出场景时 change_app_state(APP_STATE_NO_PLAY)。
 *******************************************************************************/
void sys_counter_handle(void)
{

    bool usb_cable_adapter = FALSE;
    sys_status_e cur_sys_status;
    bool enter_lp_flag = FALSE;
    uint16 cur_poff_timer; //省电关机时间(0.5s单位)

    if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0) //USB电缆或充电适配线插入
    {
        usb_cable_adapter = TRUE;
    }

#if (SUPPORT_OUTER_CHARGE == 1)
    if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if (IS_EXTERN_CAHREGE_IN() == 1)
        {
            usb_cable_adapter = TRUE;
        }
    }
#endif

    //电池及其充电检测处理
    com_battery_charge_deal();

    if ((g_app_last_state.last_ui_status != UI_STATUS_IDLE)
            && (g_app_info_state_all.ui_status == UI_STATUS_IDLE))
    {
        g_sys_counter.ui_idle_counter = 0;
    }

    /* 关闭背光及前台进入功耗优化模式 */
    if (usb_cable_adapter == FALSE)
    {
        if ((sys_comval->light_timer > 0) && (get_backlight_state() == BACKLIGHT_STATE_NORMAL))
        {
            g_sys_counter.light_counter++;
            if (g_sys_counter.light_counter >= (2u * sys_comval->light_timer)) //light_timer 以1S为单位
            {
                com_set_display_onoff(FALSE);
            }
        }

        /* 空闲一段时间就降低频率 */
        if (g_app_info_state_all.ui_status == UI_STATUS_IDLE)
        {
            g_sys_counter.ui_idle_counter++;
            if (g_sys_counter.ui_idle_counter >= UI_IDLE_TIMER)
            {
                adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
                g_app_info_state_all.ui_status = UI_STATUS_IDLE_LOWFREQ;
            }
        }
    }

    /* 获取当前系统工作状态 */
    cur_sys_status = get_cur_sys_status();

    /* 开启省电关机计时 */
    if (((g_app_last_state.last_sys_status != SYS_STATUS_IDLE) && (cur_sys_status == SYS_STATUS_IDLE))
            || ((g_app_last_state.last_sys_status != SYS_STATUS_WAIT_LINK) && (cur_sys_status == SYS_STATUS_WAIT_LINK)))
    {
        g_sys_counter.poweroff_counter = 0;
    }

    /* 省电关机计时器；在USB电缆没有插入时有效 */
    if ((usb_cable_adapter == FALSE) && (cur_sys_status != SYS_STATUS_WORKING))
    {
        if (sys_comval->lowpower_mode != LOW_POWER_MODE_NONE)
        {
            if (sys_comval->lowpower_timer == 0)
            {
                enter_lp_flag = TRUE;
            }
            else
            {
                cur_poff_timer = 120u * sys_comval->lowpower_timer;

                g_sys_counter.poweroff_counter++;
                if (g_sys_counter.poweroff_counter >= cur_poff_timer)
                {
                    enter_lp_flag = TRUE;
                }
            }

            if (enter_lp_flag == TRUE)
            {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
                if(g_bt_stack_cur_info.dev_role==NORMAL_DEV)
                {
                    enter_lower_mode();
                }
#else
                enter_lower_mode();                
#endif      

            }
        }
#if 0
        else
        {
            //poweroff_timer 等于0不省电关机
            if (sys_comval->poweroff_timer > 0)
            {
                if (cur_sys_status == SYS_STATUS_WAIT_LINK)
                {
                    //蓝牙省电时间，一般长于省电关机时间，默认为5分钟
                    cur_poff_timer = 120u * sys_comval->poweroff_bt_waitpair_timer;
                }
                else
                {
                    cur_poff_timer = 120u * sys_comval->poweroff_timer;
                }

                g_sys_counter.poweroff_counter++;
                if (g_sys_counter.poweroff_counter >= cur_poff_timer)
                {
                    //广播关机消息
                    msg.type = MSG_POWER_OFF;
                    broadcast_msg(&msg, TRUE);
                }
            }
        }
#endif
    }

    /* 3秒钟没获取到APK命令，表示APK进入后台模式 */
    g_sys_counter.apk_counter++;
    if (g_sys_counter.apk_counter >= APK_BACKGROND_OVERTIME)
    {
        set_apk_status(APK_BACKGROUND);
    }

    /* 更新最近状态 */
    g_app_last_state.last_sys_status = cur_sys_status;
    g_app_last_state.last_ui_status = g_app_info_state_all.ui_status;
}

static sys_status_e get_cur_sys_status(void)
{
    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 1)
    {
        if (g_btmanager_gl_var.btmanager_status != BTMANAGER_STATUS_IDLE)
        {
            return SYS_STATUS_WORKING;
        }
    }

    if ((g_app_info_state_all.engine_state == ENGINE_STATE_PLAYING)
            || (g_app_info_state_all.app_state != APP_STATE_NO_PLAY)
            || (g_app_info_state_all.ui_status != UI_STATUS_IDLE_LOWFREQ))
    {
        return SYS_STATUS_WORKING;
    }

    //后台引擎与前台应用保持空闲状态；蓝牙没有通信；前台没有操作持续7秒钟以上
    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 1)
    {
        if ((g_bt_stack_cur_info.conn_status == CONN_STATUS_WAIT_PAIR)
                && ((get_cur_func_id() == APP_FUNC_BTPLAY) || (get_cur_func_id() == APP_FUNC_BTCALL)))
        {
            return SYS_STATUS_WAIT_LINK;
        }
    }

    return SYS_STATUS_IDLE;
}

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
void peripheral_detect_handle(void)
{
    uint32 detect_mode = 0;
    
#if (SUPPORT_CARD_DETECT != 0)
    if (sys_comval->hard_support_card == 1)
    {
        detect_mode |= PER_DETECT_CARD;
        if (get_card_state() == CARD_STATE_CARD_NULL)
        {
            detect_mode |= PRE_DETECT_CARD_FOR_IN;
        }
        if (g_detect_card_force_flag == 1)
        {
            g_detect_card_force_flag = 0;
            detect_mode |= PER_DETECT_CARD_FORCE;
        }
    }
#endif

    if (sys_comval->support_linein == 1)
    {
        detect_mode |= PER_DETECT_AUX;
        if (get_linein_state() == LINEIN_STATE_LINEIN_NULL)
        {
            detect_mode |= PER_DETECT_AUX_FOR_IN;
        }
    }

#if (SUPPORT_HP_DETECT != 0)
    if (sys_comval->hard_support_headphone == 1)
    {
        detect_mode |= PER_DETECT_HP;
        if (get_headphone_state() == HEADPHONE_STATE_NULL)
        {
            detect_mode |= PER_DETECT_HP_FOR_IN;
        }
    }
#endif

#if ((CASE_BOARD_TYPE != CASE_BOARD_ATS2823) && (CASE_BOARD_TYPE != CASE_BOARD_DVB_ATS2823))
    //只有支持U盘播放功能才动态检测usb和u盘插入；否则只检测usb插入
    if ((sys_comval->hard_support_uhost == 1) && (sys_comval->support_uhostplay == 1))
    {
        detect_mode |= PER_DETECT_USBUHOST_SWITCH;
    }
#endif

    if (detect_mode != 0)
    {
        key_peripheral_detect_handle(detect_mode);
    }
}

//LED数码管关屏及恢复
void __section__(".bank") com_set_display_onoff(bool onoff)
{
    if (onoff == TRUE)
    {
        led_wake_up_display();
        g_app_info_state.backlight_state = BACKLIGHT_STATE_NORMAL;
    }
    else
    {
        led_sleep_display();
        g_app_info_state.backlight_state = BACKLIGHT_STATE_OFF;
    }
}

/*! \endcond */
