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
 *    接收应用私有消息（有些来自于接收系统消息时转发的），专给后台引擎使用的精简版本。
 * \param[in]    none
 * \param[out]   private_msg：返回接收到的应用私有消息
 * \return       bool
 * \retval           TRUE 接收应用私有消息成功。
 * \retval           FALSE 接收应用私有消息失败。
 * \ingroup      applib_msg
 * \par          exmaple code
 * \code
 例子1：后台引擎应用私有消息处理
 private_msg_t private_msg;
 app_result_e result;
 bool ret;

 ret = get_app_msg(&private_msg);
 if(ret == TRUE)
 {
 result = msg_callback(&private_msg);
 if(result != RESULT_NULL)
 {
 非空返回值处理
 }
 }
 * \endcode
 * \note
 *******************************************************************************/
bool get_app_msg(private_msg_t *private_msg)
{
    //软定时器扫描
    if (g_app_timer_count > 0)
    {
        handle_timers(g_app_timer_vector, g_app_timer_count);
    }

    //sys_dsp_print();

    //接收 应用私有消息
    if (sys_mq_receive((uint32) (g_this_app_info->mq_id), (void *) private_msg) == 0)
    {
        return TRUE;
    }

    //消息队列已空，返回false
    return FALSE;
}

/*! \endcond */
