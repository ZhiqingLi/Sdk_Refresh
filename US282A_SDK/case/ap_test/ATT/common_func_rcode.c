/*******************************************************************************
 *                              US212A
 *                            Module: Picture
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-19 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     user1_main.c
 * \brief    picture主模块，负责进程初始化，退出处理，场景调度
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"

//const uint8 key_map_val[]={3,2,7,8,5,6,11};
void tick_ISR(void)
{
    tick_cnt++;
}

int send_sync_msg(uint8 target_id, msg_apps_t *msg, msg_reply_t *reply, uint32 timeout)
{
    private_msg_t cur_send_pmsg;
    msg_reply_t temp_reply;

    os_event_t *sem;//同步消息同步信号量指针
    bool retvalue = TRUE;

    //无限时等待信号量，同时只能有1个线程发送同步消息
    libc_sem_wait(thread_mutex, 0);

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
    if (sys_mq_send((uint32)(target_id), (void *) &cur_send_pmsg) == -1)
    {
        DEBUG_ATT_PRINT("msg full", 0, 0);
        libc_sem_destroy(&sem);
        libc_sem_post(thread_mutex);
        return FALSE;
    }

    //等待同步消息回应
    if (libc_sem_wait(sem, timeout) < 0)
    {
        DEBUG_ATT_PRINT("msg wait timeout", 0, 0);
        retvalue = FALSE;
    }

    libc_sem_destroy(&sem);
    libc_sem_post(thread_mutex);
    return retvalue;
}

int send_async_msg(uint8 target_id, msg_apps_t *msg)
{
    private_msg_t cur_send_pmsg;

    //填充私有消息结构体
    libc_memcpy(&(cur_send_pmsg.msg), msg, sizeof(msg_apps_t));

    //发送消息到队列中
    if (sys_mq_send((uint32)(target_id), (void *) &cur_send_pmsg) == -1)
    {
        DEBUG_ATT_PRINT("msg full", 0, 0);
        return FALSE;
    }
    return TRUE;
}

