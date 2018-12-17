/*******************************************************************************
 *                              US211A
 *                            Module: music engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     fiona.yang     2011-09-06 15:00     1.0             build this file
 *******************************************************************************/
#include "app_uengine.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \app_result_e uengine_message_deal(private_msg_t* cur_msg)
 * \引擎消息处理,处理引擎接收到的私有消息和系统消息
 * \param[in]    private_msg_t* cur_msg
 * \param[out]   none
 * \return       int the result
 * \retval       RESULT_IGNOR 忽略
 * \retval       RESULT_APP_QUIT 退出app
 * \note
 */
/*******************************************************************************/
app_result_e uengine_message_deal(private_msg_t* cur_msg)
{
    app_result_e ret_value = RESULT_NULL;

    //消息
    switch (cur_msg->msg.type)
    {

        case MSG_APP_QUIT: //退出app
        ret_value = RESULT_APP_QUIT;
        break;

        /* 设置eq*/
        //case MSG_UENGINE_SET_EQ_SYNC:
        case MSG_SET_DAE_CFG_SYNC:
        if(0 == g_uspeaker_24bit)
        {
            uengine_set_eq(cur_msg);
        }
        uengine_reply_msg(cur_msg, TRUE);
        break;

        /* 获取信息相关*/
        case MSG_UENGINE_GET_STATUS_SYNC:
        uengine_reply_msg(cur_msg, TRUE);
        break;
        
        /* 播放暂停*/
        case MSG_UENGINE_PLAY_PAUSE_SYNC:
        uengine_play_pause();
        uengine_reply_msg(cur_msg, TRUE);
        break;

        /* 切换下一曲*/
        case MSG_UENGINE_NEXT_SONG_SYNC:
        uengine_next_song();
        uengine_reply_msg(cur_msg, TRUE);
        break;
        
        /* 切换上一曲*/
        case MSG_UENGINE_PREV_SONG_SYNC:
        uengine_prev_song();
        uengine_reply_msg(cur_msg, TRUE);
        break;
        
        /* 音量加*/
        case MSG_UENGINE_VOL_ADD_SYNC:
        uengine_vol_add();
        uengine_reply_msg(cur_msg, TRUE);
        break;
        
        /* 音量减*/
        case MSG_UENGINE_VOL_SUB_SYNC:
        uengine_vol_sub();
        uengine_reply_msg(cur_msg, TRUE);
        break;

        //准备进入TTS播报
        case MSG_TTS_START_SYNC:
        //准备切换音频输出
        case MSG_SWITCH_AOUT_START_SYNC:
        uengine_tts_start();
        uengine_reply_msg(cur_msg, TRUE);
        break;

        //TTS播报完成
        case MSG_TTS_STOP_SYNC:
        case MSG_SWITCH_AOUT_STOP_SYNC: //完成切换音频输出
        uengine_tts_stop();
        uengine_reply_msg(cur_msg, TRUE);
        break;

        case MSG_UENGINE_VOL_SEND_SYNC:
        g_current_vol = *((uint8 *)cur_msg->msg.content.addr);
        uengine_vol_sync((uint8)g_current_vol);
        uengine_reply_msg(cur_msg, TRUE);
        break;

        default:
        uengine_reply_msg(cur_msg, TRUE);
        break;
    }

    return ret_value;

}
