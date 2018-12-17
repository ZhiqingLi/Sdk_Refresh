#include "app_fmengine.h"

//用于硬件搜台过程中，获取硬件搜台是否结束标识
app_result_e fmengine_get_seekflag(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;
    uint8 over_flag;

    result = fm_get_seekflag(&over_flag);

    //成功获取到标识
    if (result != -1)
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;

        //硬件搜台结束
        if ((over_flag & 0x01) == 1) //bit0  为1
        {
            if ((over_flag & 0x02) != 0) //bit1 为1
            {
                //有效台
                *(hardseek_status_e*) data_ptr->reply->content = HARDSEEK_COMPLETE;
                g_fmengine_status.STC_flag = HARDSEEK_COMPLETE;
            }
            else
            {
                //未找到有效台
                *(hardseek_status_e*) data_ptr->reply->content = HARDSEEK_COMPLETE_NOT_VALID;
                g_fmengine_status.STC_flag = HARDSEEK_COMPLETE_NOT_VALID;
            }
        }
        else if ((over_flag & 0x04) != 0)
        {
            *(hardseek_status_e*) data_ptr->reply->content = HARDSEEK_FALSE;
            g_fmengine_status.STC_flag = HARDSEEK_FALSE;
        }
        else
        {
            *(hardseek_status_e*) data_ptr->reply->content = HARDSEEK_DOING;
            g_fmengine_status.STC_flag = HARDSEEK_DOING;
        }
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

//获取当前频率
app_result_e fmengine_get_freq(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;
    result = fm_get_freq(&g_fmengine_status.FM_CurrentFreq);

    if (result != -1) //0成功 -1失败
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        *(uint32*) data_ptr->reply->content = g_fmengine_status.FM_CurrentFreq;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

