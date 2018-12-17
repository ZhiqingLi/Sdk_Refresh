/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：BT STACK专用事件发送接口，用于向BT MANAGER发送BT事件，BT事件都是以异步方式
 *       发送。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    bt stack 发送事件到 bt manager。
 * \param[in]    app_id 同步发送应用私有消息的目标应用ID
 * \param[in]    msg 指向要发送的应用消息结构体
 * \param[out]   none
 * \return       int
 * \retval           0  发送异步消息成功。
 * \retval           -1 目标应用不存在。
 * \retval           -2 消息队列已满。
 * \note
 * \li  返回错误原因包括：找不到接收应用，消息队列已满。
 *******************************************************************************/
int send_async_event(uint8 app_id, btstack_event_t *ev)
{
    uint8 mq_id = MQ_ID_RES;

    PRINT_DBG_INT("btstack event:", ev->msg.type);

    //if (app_id != APP_ID_BT_MANAGER)
    //{
    //    return ERR_TARGET_NOEXIST;
    //}

    //发送消息到队列中
    if (sys_mq_send((uint32) (mq_id), (void *) ev) < 0)
    {
        //PRINT_ERR("bt evt MSG FULL!!");
        return ERR_MSGQUEUE_FULL;
    }

    return ERR_NO_ERR;
}

/*! \endcond */
