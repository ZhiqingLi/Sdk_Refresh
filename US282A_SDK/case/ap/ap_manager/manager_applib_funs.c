/*******************************************************************************
 *                              us212A
 *                            Module: manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-28 14:42     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_applib_funs.c
 * \brief    applib 进程管理模块（bank部分）实现
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               实现一套基本 进程管理 接口。
 * \par      EXTERNALIZED FUNCTIONS:
 *               调用 libc 和 psp 的接口实现。
 * \version 1.0
 * \date  2011-9-28
 *******************************************************************************/

#include "applib.h"

/******************************************************************************/
/*!
 * \par  Description:
 *    应用在 applib 模块上的初始化处理，在应用的 main 函数入口处调用
 * \param[in]    app_id：应用ID
 * \param[in]    type：应用类型，参见app_type_e 定义
 * \param[out]   none
 * \return       bool
 * \retval           返回是否初始化成功，true表示成功，false表示失败
 * \note
 *******************************************************************************/
bool applib_init(uint8 app_id, app_type_e type)
{
    app_info_t *cur_app_info;

    //获取 app_info 结构体指针
    cur_app_info = &g_app_info_vector[type];

    //初始化应用信息
    libc_memset(cur_app_info, 0x0, sizeof(app_info_t));
    cur_app_info->used = 1;//标志已被分配使用
    cur_app_info->app_id = app_id;
    cur_app_info->app_type = (uint8) type;
    cur_app_info->mq_id = (uint8) MQ_ID_MNG;

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    以异步方式发送应用私有消息
 * \param[in]    app_id：同步发送应用私有消息的目标应用ID
 * \param[in]    msg：指向要发送的应用消息结构体
 * \param[out]   none
 * \return       bool
 * \retval           返回发送异步消息是否成功，true表示成功，false表示失败
 * \par          exmaple code
 * \code
 *               msg.type = MSG_KILL_APP;
 *               msg.content.data[0] = APP_ID_MUSIC;
 *               result = send_async_msg(APP_ID_MANAGER, &msg);
 *               if(result == TRUE)
 *               {}
 * \endcode
 * \note
 *******************************************************************************/
int send_async_msg(uint8 app_id, msg_apps_t *msg)
{
    btstack_msg_t cur_msg;
    mq_id_e mq_id;

    cur_msg.sem = NULL;
    cur_msg.reply = NULL;
    libc_memset(cur_msg.bd_addr, 0x0, BD_ADDR_LEN);

    //查找目标应用
    if (get_engine_type_by_appid(app_id) != ENGINE_NULL)
    {
        mq_id = MQ_ID_EGN;
    }
    else if (app_id == APP_ID_BTSTACK)
    {
        mq_id = MQ_ID_BT;
    }
    else
    {
        mq_id = MQ_ID_DESK;
    }

    //填充私有消息结构体
    libc_memcpy(&(cur_msg.msg), msg, sizeof(msg_apps_t));

    //发送消息到队列中
    if ((int) sys_mq_send((uint32) (mq_id), (void *) &cur_msg) < 0)
    {
        return ERR_MSGQUEUE_FULL;
    }

    return ERR_NO_ERR;
}

