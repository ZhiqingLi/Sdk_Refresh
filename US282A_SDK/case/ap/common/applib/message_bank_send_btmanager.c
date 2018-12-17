/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：BT MANAGER专用同步事件发送接口，用于向BT STACK发送BT命令或事件应答包。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    以同步方式发送应用私有消息。
 * \param[in]    bd_addr 命令对象的设备地址
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
 * \note
 * \li  返回错误原因包括：消息队列已满，同步消息响应超时。
 * \li  注意消息内容缓冲区 msg.content.addr 和 应答缓冲区 reply.content 的空间都是开在消息发送方的。
 *      由于发送同步消息后会等待返回，如果调用 send_sync_msg 的代码是在 control bank段，那么上面的
 *      缓冲区可以开在bank data中。
 *******************************************************************************/
int send_sync_msg_btmanager(uint8 *bd_addr, msg_apps_t *msg, msg_reply_t *reply, uint32 timeout)
{
    btstack_msg_t cur_msg;
    msg_reply_t temp_reply;
    os_event_t *sem; //同步消息同步信号量指针
    int retvalue = ERR_NO_ERR;
    uint8 mq_id = MQ_ID_BT;

    PRINT_DBG_INT("sync msg:", msg->type);

    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 0)
    {
        return 0;
    }

    //无限时等待信号量，同时只能有1个线程发送同步消息
    libc_sem_wait(thread_mutex, 0);

    //填充私有消息结构体
    libc_memcpy(&(cur_msg.msg), msg, sizeof(msg_apps_t));
    if (bd_addr != NULL)
    {
        libc_memcpy(cur_msg.bd_addr, bd_addr, BD_ADDR_LEN);
    }
    else
    {
        libc_memset(cur_msg.bd_addr, 0x0, BD_ADDR_LEN);
    }

    libc_sem_init(&sem, 0);
    cur_msg.sem = sem;
    if (reply == NULL)
    {
        cur_msg.reply = &temp_reply;
    }
    else
    {
        cur_msg.reply = reply;
    }

    //发送消息到队列中
    if (sys_mq_send((uint32) (mq_id), (void *) &cur_msg) < 0)
    {
        //PRINT_ERR("sync msg ERR_MSGQUEUE_FULL!!");
        retvalue = ERR_MSGQUEUE_FULL;
    }
    else
    {
        //等待同步消息回应
        if (libc_sem_wait(sem, timeout) < 0)
        {
            //PRINT_ERR("sync msg ERR_SYNC_TIMEOUT!!");
            retvalue = ERR_SYNC_TIMEOUT;
        }
    }

    libc_sem_destroy(&sem);
    libc_sem_post(thread_mutex);

    PRINT_DBG_INT("sync msg ret:", msg->type);

    return retvalue;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    以异步方式发送应用私有消息。
 * \param[in]    bd_addr 命令对象的设备地址
 * \param[in]    msg 指向要发送的应用消息结构体
 * \param[out]   none
 * \return       int
 * \retval           0  发送异步消息成功。
 * \retval           -1 目标应用不存在。
 * \retval           -2 消息队列已满。
 * \note
 * \li  返回错误原因包括：找不到接收应用，消息队列已满。
 * \li  私有消息异步发送方式，最多只能附带4个字节的消息内容（建议采用双方约定code作为消息内容）。
 *******************************************************************************/
int __section__(".bank") send_async_msg_btmanager(uint8 *bd_addr, msg_apps_t *msg)
{
    btstack_msg_t cur_msg;
    uint8 mq_id = MQ_ID_BT;

    PRINT_INFO_INT("async msg:", msg->type);

    //填充私有消息结构体
    libc_memcpy(&(cur_msg.msg), msg, sizeof(msg_apps_t));
    if (bd_addr != NULL)
    {
        libc_memcpy(cur_msg.bd_addr, bd_addr, BD_ADDR_LEN);
    }
    else
    {
        libc_memset(cur_msg.bd_addr, 0x0, BD_ADDR_LEN);
    }

    //发送消息到队列中
    if (sys_mq_send((uint32) (mq_id), (void *) &cur_msg) < 0)
    {
        PRINT_ERR("async msg ERR_MSGQUEUE_FULL!!");
        return ERR_MSGQUEUE_FULL;
    }

    return ERR_NO_ERR;
}

/*! \endcond */
