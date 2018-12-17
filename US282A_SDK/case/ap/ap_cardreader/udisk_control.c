/*******************************************************************************
 *                              US212A
 *                            Module: music ui
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2011-09-22          1.0              create this file
 *******************************************************************************/

#include "ap_udisk.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \bool udisk_send_msg(void *input_param, void *output_param, uint32 msg_type)
 * \向引擎发送消息
 * \param[in]    input_param 输入参数指针
 * \param[in]    msg_type  消息类型
 * \param[out]   output_param 输出参数指针
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      udisk_control.c
 * \note
 */
/*******************************************************************************/
bool udisk_send_msg(void *input_param, void *output_param, uint32 msg_type)
{
    int32 bret = TRUE;
    msg_apps_t msg;
    //返回消息内容存放变量
    msg_reply_t reply;

    //存放输入参数的地址
    msg.content.addr = input_param;

    //存放获取参数的地址
    reply.content = output_param;

    //消息类型(即消息名称)
    msg.type = msg_type;

    //发送同步消息
    bret = (send_sync_msg(APP_ID_UENGINE, &msg, &reply, 0) == ERR_NO_ERR);

    if (reply.type == MSG_REPLY_FAILED)
    {
        bret = FALSE;
    }
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_get_status(mengine_status_t* play_status)
 * \获取当前状态
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
/*bool usound_get_status(mengine_status_t* play_status)
 {
 bool bret;
 bret = udisk_send_msg(NULL, play_status, MSG_UENGINE_GET_STATUS_SYNC);
 return bret;
 }*/

