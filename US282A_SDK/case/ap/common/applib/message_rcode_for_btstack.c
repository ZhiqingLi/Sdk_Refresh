/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：引擎应用私有消息接收接口实现，在引擎应用和BT STACK中调用，该接口会执行
 *       软定时器，并且会代理DSP打印和中断打印。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    接收应用私有消息（有些来自于接收系统消息时转发的），专给BTSTACK使用的精简版本。
 * \param[in]    none
 * \param[out]   btstack_msg 返回接收到的应用私有消息
 * \return       bool
 * \retval           TRUE 接收应用私有消息成功。
 * \retval           FALSE 接收应用私有消息失败。
 * \note
 *******************************************************************************/
bool get_app_msg_btstack(btstack_msg_t *btstack_msg)
{
    //软定时器扫描
    if (g_app_timer_count > 0)
    {
        handle_timers(g_app_timer_vector, g_app_timer_count);
    }

    //接收 应用私有消息
    if (sys_mq_receive(MQ_ID_BT, (void *) btstack_msg) == 0)
    {
        return TRUE;
    }

    //消息队列已空，返回false
    return FALSE;
}

/*! \endcond */
