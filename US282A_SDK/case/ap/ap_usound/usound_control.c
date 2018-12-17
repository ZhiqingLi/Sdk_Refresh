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

#include "ap_usound.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_send_msg(void *input_param, void *output_param, uint32 msg_type)
 * \向引擎发送消息
 * \param[in]    input_param 输入参数指针
 * \param[in]    msg_type  消息类型
 * \param[out]   output_param 输出参数指针
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_send_msg(void *input_param, void *output_param, uint32 msg_type)
{
    bool bret = TRUE;
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
    bret = (uint8) (send_sync_msg(APP_ID_UENGINE, &msg, &reply, 0) == ERR_NO_ERR);

    if (reply.type == MSG_REPLY_FAILED)
    {
        bret = FALSE;
    }
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_volume_add(void)
 * \加大音量
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_volume_add(void)
{
    bool bret;

    //发送同步消息
    bret = usound_send_msg(NULL, NULL, MSG_UENGINE_VOL_ADD_SYNC);
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_volume_sub(void)
 * \减小音量
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_volume_sub(void)
{
    bool bret;

    //发送同步消息
    bret = usound_send_msg(NULL, NULL, MSG_UENGINE_VOL_SUB_SYNC);
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_play_pause(void)
 * \播放暂停音乐
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_play_pause(void)
{
    bool bret;
    bret = usound_send_msg(NULL, NULL, MSG_UENGINE_PLAY_PAUSE_SYNC);
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_play_next(void)
 * \播放下一曲
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_play_next(void)
{
    bool bret;

    //发送同步消息
    bret = usound_send_msg(NULL, NULL, MSG_UENGINE_NEXT_SONG_SYNC);
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_play_prev(void)
 * \播放上一曲
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_play_prev(void)
{
    bool bret;

    //发送同步消息
    bret = usound_send_msg(NULL, NULL, MSG_UENGINE_PREV_SONG_SYNC);
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_tts_start(void)
 * \开始播报tts
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_tts_start(void)
{
    bool bret;

    //发送同步消息
    bret = usound_send_msg(NULL, NULL, MSG_TTS_START_SYNC);
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_tts_stop(void)
 * \tts播报完成
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      usound_control.c
 * \note
 */
/*******************************************************************************/
bool usound_tts_stop(void)
{
    bool bret;

    //发送同步消息
    bret = usound_send_msg(NULL, NULL, MSG_TTS_STOP_SYNC);
    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_get_status(uint32* play_status)
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
bool __section__(".rcode") usound_get_status(uengine_status_t* play_status)
{
    if (sys_share_query_read(APP_ID_UENGINE, play_status) == -1)
    {
        PRINT_ERR("usound share query not exist!\n");
        return FALSE;
    }

    return TRUE;
}

