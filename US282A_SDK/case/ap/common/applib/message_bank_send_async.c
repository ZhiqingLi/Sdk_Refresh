/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：异步消息发送接口，用于应用之间发送异步消息，也可用于前台COMMON向自身发送
 *       异步消息，以降低COMMON与应用之间的耦合性。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

static app_info_t *__app_running_get_app_info(uint8 app_id);

/******************************************************************************/
/*!
 * \par  Description:
 *    以异步方式发送应用私有消息。
 * \param[in]    app_id 同步发送应用私有消息的目标应用ID
 * \param[in]    msg 指向要发送的应用消息结构体
 * \param[out]   none
 * \return       int
 * \retval           0  发送异步消息成功。
 * \retval           -1 目标应用不存在。
 * \retval           -2 消息队列已满。
 * \ingroup      applib_msg
 * \par          exmaple code
 * \code
 例子1：异步发送消息，无须等待立即返回
 msg_apps_t msg;
 int ret;

 msg.type = MSG_KILL_APP;
 msg.content.data[0] = APP_ID_MUSIC;
 ret = send_async_msg(APP_ID_MANAGER, &msg);
 if (int != ERR_NO_ERR)
 {
 发生错误
 }
 * \endcode
 * \note
 * \li  返回错误原因包括：找不到接收应用，消息队列已满。
 * \li  私有消息异步发送方式，最多只能附带4个字节的消息内容（建议采用双方约定code作为消息内容）。
 *******************************************************************************/
int send_async_msg(uint8 app_id, msg_apps_t *msg)
{
    const app_info_t *target_app_info;
    private_msg_t cur_send_pmsg;
    uint8 mq_id;

    PRINT_INFO_INT("async msg:", msg->type);

    //通用引擎ID转换为具体APP ID
    if (app_id == APP_ID_THEENGINE)
    {
        app_id = get_engine_appid_by_type(get_engine_type());
        if (app_id == APP_ID_MAX)
        {
            goto app_noexist;
        }
    }

    //查找目标应用
    target_app_info = __app_running_get_app_info(app_id);
    if (target_app_info == NULL)
    {
        app_noexist:
        PRINT_ERR("async msg ERR_TARGET_NOEXIST!!");
        return ERR_TARGET_NOEXIST;
    }
    mq_id = target_app_info->mq_id;

    send_msg:
    //填充私有消息结构体
    libc_memcpy(&(cur_send_pmsg.msg), msg, sizeof(msg_apps_t));

    //发送消息到队列中
    if (sys_mq_send((uint32) (mq_id), (void *) &cur_send_pmsg) < 0)
    {
        PRINT_ERR("async msg ERR_MSGQUEUE_FULL!!");
        return ERR_MSGQUEUE_FULL;
    }

    return ERR_NO_ERR;
}

/*! \endcond */

/*! \cond */

//根据应用ID获取应用信息
static app_info_t *__app_running_get_app_info(uint8 app_id)
{
    app_info_t *app;
    uint8 i;

    if (app_id >= APP_ID_MAX)
    {
        return NULL;
    }

    for (i = 0; i < MAX_APP_COUNT; i++)
    {
        app = &g_app_info_vector[i];
        if ((app->used == 1) && (app->app_id == app_id))
        {
            return app;
        }
    }

    return NULL;
}

/*! \endcond */
