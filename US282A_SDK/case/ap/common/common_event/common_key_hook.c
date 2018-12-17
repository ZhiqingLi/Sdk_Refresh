/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：按键消息处理勾子函数，在按键消息响应之前执行一些公共处理，包括过滤掉按键
 *       消息，解除静音，发出按键音，系统计时器清零，唤醒UI显示等。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    按键发生特殊处理，作为 gui 消息处理的回调函数。
 * \param[in]    input_msg 获取到的 gui消息指针
 * \param[out]   input_msg 可能会对input_msg进行修改，比如将val修改为KEY_HOLD虚拟按键
 * \return       bool
 * \retval           TRUE 获取到的按键有效，没有被过滤掉。
 * \retval           FALSE 获取到的按键无效，被过滤掉了。
 * \ingroup      common_func
 * \note
 *******************************************************************************/
bool com_gui_msg_hook(input_gui_msg_t *input_msg)
{
    bool ret;
    key_value_e key_val = input_msg->data.kmsg.val;
    key_type_e key_type = input_msg->data.kmsg.type;

    //1.超长按按键后过滤掉后续超长按键（检测到按键抬起，或者其他按键按下，或该按键的新周期，就算过滤完成）
    if (key_val == g_key_infor.filter_key_itself)
    {
        if ((key_type == KEY_TYPE_DOWN) //收到该按键的新周期，过滤完成
            || (key_type == KEY_TYPE_DBL_CLICK))//双击该按键也算新周期，过滤完成 //第二次超长按也算过滤完成
        {
            g_key_infor.filter_key_itself = KEY_NULL;
        }
        else
        {
           //过滤LONG和HOLD按键
            if (key_type == KEY_TYPE_LONG_10S)
            {
                return FALSE;
            }
            if ((key_type != KEY_TYPE_SHORT_UP) && (key_type != KEY_TYPE_LONG_UP))                    
            {
                return FALSE;
            }
            else //过滤完成，但还是不响应SHORT_UP按键
            {
                this_filter_key_value = KEY_NULL;
                g_key_infor.filter_key_itself = KEY_NULL;
                return FALSE;
            }
        }
    }
    else //收到新按键，过滤完成
    {
        g_key_infor.filter_key_itself = KEY_NULL;
    }

    //1.长按按键后过滤掉后续按键（检测到按键抬起，或者其他按键按下，或该按键的新周期，就算过滤完成）
    if (key_val == g_key_infor.filter_key)
    {
        //过滤还没完成就收到DOWN消息，表明SHORTUP消息在切换应用时被清除掉了，
        //那么DOWN消息就属于新的按键周期的消息，不应该被过滤
        if ((key_type == KEY_TYPE_DOWN) //收到该按键的新周期，过滤完成
                || (key_type == KEY_TYPE_DBL_CLICK)) //双击该按键也算新周期，过滤完成
        {
            g_key_infor.filter_key = KEY_NULL;
        }
        else
        {
            if(key_type == KEY_TYPE_LONG_10S)
            {
                g_key_infor.filter_key = KEY_NULL;
                this_filter_key_value = KEY_NULL;
                goto LONG10S_DEAL;
            }
            //过滤LONG和HOLD按键
            if ((key_type != KEY_TYPE_SHORT_UP)
                    && (key_type != KEY_TYPE_LONG_UP))
            {
                return FALSE;
            }
            else //过滤完成，但还是不响应SHORT_UP按键
            {
                this_filter_key_value = KEY_NULL;
                g_key_infor.filter_key = KEY_NULL;
                return FALSE;
            }
        }
    }
    else //收到新按键，过滤完成
    {
        g_key_infor.filter_key = KEY_NULL;
    }

    //2.长按按键后过滤掉后续抬起按键（检测到按键抬起，或者其他按键按下，或该按键的新周期，就算过滤完成）
    if (key_val == g_key_infor.filter_key_up)
    {
        //过滤还没完成就收到DOWN消息，表明SHORTUP消息在切换应用时被清除掉了，
        //那么DOWN消息就属于新的按键周期的消息，不应该被过滤
        if ((key_type == KEY_TYPE_DOWN) //收到该按键的新周期，过滤完成
                || (key_type == KEY_TYPE_DBL_CLICK)) //双击该按键也算新周期，过滤完成
        {
            g_key_infor.filter_key_up = KEY_NULL;
        }
        else
        {
            //仅过滤抬起按键
            if ((key_type == KEY_TYPE_SHORT_UP)
                    || (key_type == KEY_TYPE_LONG_UP))
            {
                this_filter_key_value = KEY_NULL;
                g_key_infor.filter_key_up = KEY_NULL;
                return FALSE;
            }
        }
    }
    else //收到新按键，过滤完成
    {
        g_key_infor.filter_key_up = KEY_NULL;
    }

    LONG10S_DEAL:

    //更新当前按键值
    if ((key_type == KEY_TYPE_SHORT_UP) //如果按键已经抬起，那么设置过滤按键无效
    || (key_type == KEY_TYPE_LONG_UP))
    {
        this_filter_key_value = KEY_NULL;
    }
    else
    {
        this_filter_key_value = key_val;
    }

    //3.有按键动作要解除静音
    g_ignore_switch_mute = FALSE;
    if (get_mute_enable() == TRUE)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        com_switch_mute(1,0,NULL);
#else
        change_mute_enable(FALSE);
        g_ignore_switch_mute = TRUE;
#endif
        com_reset_sound_volume(0);
        
    }

    //4.按键音处理，仅在按键按下时发出按键音
    if ((key_type == KEY_TYPE_DOWN) && (input_msg->data.kmsg.option == 0))
    {
        //没音频输出，非静音状态，且按键音使能，才发出按键音
        com_start_key_tone(KEYTONE_NOBLOCK);
    }

    ret = TRUE;

    //5.省电关机,屏保计时清0
    g_sys_counter.poweroff_counter = 0;
    g_sys_counter.light_counter = 0;

    if (get_backlight_state() == BACKLIGHT_STATE_OFF)
    {
        com_set_display_onoff(TRUE);
    }

    //6.调高频率
    g_sys_counter.ui_idle_counter = 0;
    if (g_app_info_state_all.ui_status == UI_STATUS_IDLE_LOWFREQ)
    {
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
        g_app_info_state_all.ui_status = UI_STATUS_IDLE;
    }

    return ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    过滤掉当前按键后续动作，即相同值按键消息。
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      common_func
 * \note
 * \li  该接口用于过滤当前按键值的后续动作，如需过滤掉任意指定按键，需直接调用
 *      change_filter_key 设置。
 * \li  在检测到当前按键抬起消息或者其他按键消息后，由 com_gui_msg_hook 自动调用解除。
 * \li  如果此时没有按键按下，该接口调用不会产生真正的按键过滤效果。
 *******************************************************************************/
void com_filter_key_hold(void)
{
    g_key_infor.filter_key = this_filter_key_value;
}

void com_filter_key_up(void)
{
    g_key_infor.filter_key_up = this_filter_key_value;
}

void com_filter_key_itself(void)
{
    //libc_print("this_filter_key_value",this_filter_key_value,2);
    g_key_infor.filter_key_itself = this_filter_key_value;
}

/*! \endcond */
