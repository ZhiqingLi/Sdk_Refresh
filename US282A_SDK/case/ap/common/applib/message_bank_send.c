/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：同步消息发送接口，用于应用之间发送同步消息，使用同步消息必须谨慎，要保证
 *       消息接收方收到并回复，否则前台应用会陷入假死状态。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

static app_info_t *__app_running_get_app_info(uint8 app_id);

/******************************************************************************/
/*!
 * \par  Description:
 *    以同步方式发送应用私有消息。
 * \param[in]    app_id 同步发送应用私有消息的目标应用ID
 * \param[in]    msg 指向要发送的应用消息结构体
 * \param[in]    reply 指向同步消息应答结构体
 * \param[in]    timeout 超时发送时间，0 表示一直等待不超时返回
 * \param[out]   none
 * \return       int
 * \retval           0  发送同步消息成功。
 * \retval           -1 目标应用不存在。
 * \retval           -2 消息队列已满。
 * \retval           -3 发送同步消息给自身。
 * \retval           -4 同步消息响应超时。
 * \ingroup      applib_msg
 * \par          exmaple code
 * \code
 例子1：阻塞式发送同步消息，直到对方调用 libc_sem_post 应答
 msg_apps_t msg;
 msg_reply_t reply;
 int ret;

 msg.type = MSG_MENGINE_GET_FILEPATH_SYNC;
 reply.content = (void *)file_path;
 ret = send_sync_msg(APP_ID_MANAGER, &msg, &reply, 0);
 if (ret == ERR_NO_ERR)
 {
 if (reply.type == MSG_REPLY_SUCCESS)
 {
 正确返回，获取并处理文件路径
 }
 else
 {
 错误返回
 }
 }
 else
 {
 其他错误
 }
 * \endcode
 * \note
 * \li  返回错误原因包括：找不到接收应用，消息队列已满，发送同步消息给自己，同步消息响应超时。
 * \li  注意消息内容缓冲区 msg.content.addr 和 应答缓冲区 reply.content 的空间都是开在消息发送方的。
 *      由于发送同步消息后会等待返回，如果调用 send_sync_msg 的代码是在 control bank段，那么上面的
 *      缓冲区可以开在bank data中。
 *******************************************************************************/
int send_sync_msg(uint8 app_id, msg_apps_t *msg, msg_reply_t *reply, uint32 timeout)
{
    private_msg_t cur_send_pmsg;
    const app_info_t *target_app_info;
    msg_reply_t temp_reply;
    os_event_t *sem; //同步消息同步信号量指针
    int retvalue = ERR_NO_ERR;

    PRINT_DBG_INT("sync msg:", msg->type);

    //无限时等待信号量，同时只能有1个线程发送同步消息
    libc_sem_wait(thread_mutex, 0);

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
    if (NULL == target_app_info)
    {
        app_noexist:
        libc_sem_post(thread_mutex);
        PRINT_ERR("sync msg ERR_TARGET_NOEXIST!!");
        return ERR_TARGET_NOEXIST;
    }

    //不能发送同步消息给应用自己
    if (target_app_info == g_this_app_info)
    {
        libc_sem_post(thread_mutex);
        PRINT_ERR("sync msg ERR_SYNC_ITSELF!!");
        return ERR_SYNC_ITSELF;
    }

    //填充私有消息结构体
    libc_memcpy(&(cur_send_pmsg.msg), msg, sizeof(msg_apps_t));
    libc_sem_init(&sem, 0);
    cur_send_pmsg.sem = sem;
    if (reply == NULL)
    {
        cur_send_pmsg.reply = &temp_reply;
    }
    else
    {
        cur_send_pmsg.reply = reply;
    }

    //发送消息到队列中
    if (sys_mq_send((uint32) (target_app_info->mq_id), (void *) &cur_send_pmsg) < 0)
    {
        PRINT_ERR("sync msg ERR_MSGQUEUE_FULL!!");
        retvalue = ERR_MSGQUEUE_FULL;
    }
    else
    {
        //等待同步消息回应
        if (libc_sem_wait(sem, timeout) < 0)
        {
            PRINT_ERR("sync msg ERR_SYNC_TIMEOUT!!");
            retvalue = ERR_SYNC_TIMEOUT;
        }
    }

    libc_sem_destroy(&sem);
    libc_sem_post(thread_mutex);

    PRINT_DBG_INT("sync msg ret:", msg->type);

    return retvalue;
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
