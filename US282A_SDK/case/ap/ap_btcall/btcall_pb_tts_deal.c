/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   处理tts播报
 *      <author>       <time>
 *       Wekan   2015-3-27
 *******************************************************************************/

#include "ap_btcall.h"

const uint8 pb_info_ext[] =
{ "  来电话了" };

uint8 tts_info_buf[MAX_TTS_INFO_LENGTH];

bool g_call_phone_num = FALSE;//是否在电话本播报中
pb_result_e g_pb_result = PB_RESULT_NULL;//电话本播报类型


/******************************************************************************
 * \par  Description:   处理电话本非数字的电话信息
 * \param[in]    str_info :需要转换的字符串内容
 * \param[out]   null
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/
bool btcall_pb_tts_deal_name(uint8 *str_info)
{
    bool ret;

    utf8_to_unicode(str_info);

    ret = unicode_to_char(str_info);

    return ret;
}

/******************************************************************************
 * \par  Description:   处理电话号码信息
 * \param[in]   str_info :需要转换的字符串内容
 * \param[out]   null
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

uint8 btcall_pb_tts_deal_num(uint8 *str_info)
{
    uint8 *p_str_info_tts = str_info;
    uint8 pb_num;
    uint8 pb_num_len = 0;

    while (1)
    {
        pb_num = *str_info;
        str_info++;
        //结束 or不是ascill码
        if ((pb_num == 0x0) || (pb_num > 0x80))
        {
            break;
        }
        //数字在0-9之间存在到tts段缓存buf中。
        if ((pb_num >= 0x30) && (pb_num <= 0x39))
        {
            *p_str_info_tts = (TTS_SECTION_NUM_0 + pb_num - 0x30);
            p_str_info_tts++;
            pb_num_len++;
        }
    }

    *p_str_info_tts = 0xff;

    return pb_num_len;
}

/******************************************************************************
 * \par  Description:   处理电话本的语音播报信息
 * \param[in]   null
 * \param[out]   null
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

bool btcall_pb_tts_init(void)
{
    msg_apps_t msg;
    pb_data_t pb_content;
    uint8 str_len;

    libc_memset(&pb_content, 0x00, sizeof(pb_data_t));

    msg.content.addr = &pb_content;
    msg.type = MSG_BTSTACK_GET_PB_CONTENT_SYNC;

    send_sync_msg_btmanager(NULL, &msg, NULL, 0);

    libc_memset(tts_info_buf, 0x0, MAX_TTS_INFO_LENGTH);
    if (g_pb_result == PB_RESULT_NAME)
    {
        //对姓名信息进行检测
        libc_memcpy(tts_info_buf, pb_content.name, PB_MAX_NUM_LEN);
        if (btcall_pb_tts_deal_name(tts_info_buf) == FALSE)
        {//编码转换表打开失败，只播报电话号码
            g_pb_result = PB_RESULT_NUM;
        }
        else
        {
            str_len = (uint8) libc_strlen(tts_info_buf);
            //电话号码 or name+来电话了
            libc_memcpy(&tts_info_buf[str_len], pb_info_ext, sizeof(pb_info_ext));
        }
    }

    if (g_pb_result == PB_RESULT_NUM)
    {
        //对电话号码进行检测
        libc_memcpy(tts_info_buf, pb_content.phonenum, PB_MAX_NUM_LEN);
        if (btcall_pb_tts_deal_num(tts_info_buf) == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/******************************************************************************
 * \par  Description:   播报电话本信息函数
 * \param[in]   null
 * \param[out]   null
 * \return      null
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

void btcall_pb_tts_play(void)
{
    if (g_pb_result == PB_RESULT_NUM)
    {
        //只有电话号码时使用tts_rc.lib播报
        com_tts_state_play(TTS_MODE_SECLIST | TTS_MODE_PB_PLAY, tts_info_buf);
    }
    else if (g_pb_result == PB_RESULT_NAME)
    {
        tts_info_t pb_info;

        //有电话本信息时使用Package.dat语音库播报
        pb_info.str_buf = tts_info_buf;
        com_tts_state_play(TTS_MODE_STRING | TTS_MODE_PB_PLAY, &pb_info);
    }
    else
    {
        ;//nothing
    }
}
