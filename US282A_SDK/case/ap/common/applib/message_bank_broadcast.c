/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：消息广播接口，只能用于异步消息广播，现在系统消息就是通过广播到前台应用，
 *       再由前台应用处理。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    以广播方式（也属于异步发送）发送应用私有消息。
 * \param[in]    msg 指向要发送的应用消息结构体
 * \param[in]    only_front 仅发送给前台应用
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 广播消息成功。
 * \retval           FALSE 广播消息失败。
 * \ingroup      applib_msg
 * \par          exmaple code
 * \code
 例子1：广播系统消息，见 get_sys_msg 实现
 * \endcode
 * \note
 * \li  广播应用消息最多只能附带4个字节的消息内容（建议采用双方约定code作为消息内容）。
 * \li  该接口不会把系统消息广播到 manager ap 私有消息队列中。
 *******************************************************************************/
bool broadcast_msg(msg_apps_t *msg, bool only_front)
{
    app_info_t *app;
    uint8 i;

    //广播该消息给所有应用
    for (i = 0; i < MAX_APP_COUNT; i++)
    {
        app = &g_app_info_vector[i];
        if (app->used == 1)
        {
            if ((only_front == FALSE) || (app->app_type == APP_TYPE_GUI))
            {
                send_async_msg(app->app_id, msg);
            }
        }
    }

    return TRUE;
}

/*! \endcond */
