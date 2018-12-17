/*******************************************************************************
 *                              us212A
 *                            Module: Manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       lzcai     2011-9-27 20:38     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_message_send.c
 * \brief    applib 消息通信机制 发送类（bank） 接口实现
 * \author   lzcai
 * \par      GENERAL DESCRIPTION:
 *               实现一套基本消息通信机制，主要包括消息发送机制。
 * \par      EXTERNALIZED FUNCTIONS:
 *               调用 libc 和 psp 的接口，以及applib 进程管理模块接口实现消息机制。
 * \version 1.0
 * \date  2011-9-27
 *******************************************************************************/

#include "applib.h"

/******************************************************************************/
/*!
 * \par  Description:
 *  等待前后或后台ap退出
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      music
 * \note
 *******************************************************************************/
void wait_ap_exit(void)
{
    uint8 ap_ret;
    int32 waitpid_ret;

    while (1)
    {
        waitpid_ret = libc_waitpid(&ap_ret, 0);
        if ((waitpid_ret >= 0) && (waitpid_ret != WAITPID_NO_PROCESS_EXIT))
        {
            break;
        }
    }

    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *  杀死ap进程
 * \param[in]    ap_id ap编号
 * \param[out]   none
 * \return       none
 * \ingroup      music
 * \note
 *******************************************************************************/
void kill_app(uint8 ap_id, uint8 ap_param)
{
    msg_apps_t async_msg;

    async_msg.type = MSG_APP_QUIT;
    async_msg.content.data[0] = ap_param;
    send_async_msg(ap_id, &async_msg);

    wait_ap_exit();

    return;
}
