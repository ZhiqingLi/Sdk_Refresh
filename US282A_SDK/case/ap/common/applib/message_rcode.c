/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用接收系统消息和私有消息接口实现；系统消息会先转发为私有消息并在前台
 *       应用中接受并处理；这类接口仅在前台应用中调用。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    接收系统消息，并广播给所有应用。
 * \param[in]    none
 * \param[out]   sys_msg：返回接收到的系统消息
 * \return       bool
 * \retval           TRUE 接收系统消息成功。
 * \retval           FALSE 接收系统消息失败。
 * \ingroup      applib_msg
 * \note
 * \li  获取系统消息后，会广播给所有应用。
 * \li  该接口只在前台AP的 get_app_msg 中调用，也就是说，系统消息从发出到被处理，
 *      可能会被 delay 一些时间，直到有前台AP进入消息循环。
 *******************************************************************************/
bool get_sys_msg(sys_msg_t *sys_msg)
{
    msg_apps_t msg;

    //须把type清0，因为系统消息长度为2个字节，sys_mq_receive 并不会覆盖到type（4字节）高2字节，会导致type错误
    sys_msg->type = MSG_NULL;
    if (sys_mq_receive(MQ_ID_SYS, (void *) sys_msg) == 0)
    {
        msg.type = sys_msg->type;
        return broadcast_msg(&msg, TRUE);
    }

    //消息队列已空，返回false
    return FALSE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    接收应用私有消息（有些来自于接收系统消息时转发的）。
 * \param[in]    none
 * \param[out]   private_msg：返回接收到的应用私有消息
 * \return       bool
 * \retval           TRUE 接收应用私有消息成功。
 * \retval           FALSE 接收应用私有消息失败。
 * \ingroup      applib_msg
 * \par          exmaple code
 * \code
 例子1：应用私有消息处理
 private_msg_t private_msg;
 app_result_e result;
 bool ret;

 ret = get_app_msg(&private_msg);
 if(ret == TRUE)
 {
 result = msg_callback(&private_msg);
 if(result == RESULT_REDRAW)
 {
 返回重绘
 }
 else
 {
 其他返回值处理
 }
 }
 * \endcode
 * \note
 *******************************************************************************/
bool get_app_msg(private_msg_t *private_msg)
{
    if (g_this_app_info->app_type == APP_TYPE_GUI) //前台应用
    {
        if (g_buffer_app_msg.msg.type != MSG_NULL) //缓冲消息，直接拷贝
        {
            libc_memcpy(private_msg, &g_buffer_app_msg, sizeof(private_msg_t));
            g_buffer_app_msg.msg.type = MSG_NULL;
            return TRUE;
        }
        else
        {
            sys_msg_t sys_msg;
            //获取系统消息并广播
            get_sys_msg(&sys_msg);
        }
    }
    //接收 应用私有消息
    if (sys_mq_receive((uint32) (g_this_app_info->mq_id), (void *) private_msg) == 0)
    {
        g_this_app_msg_hook(private_msg);
        return TRUE;
    }

    //消息队列已空，返回false
    return FALSE;
}

/*! \endcond */
