#include "app_fmengine.h"
#include "mmm_mr.h"
#include <analog.h>

//设置频点
app_result_e fmengine_set_freq_sync(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    uint32 freq;
    int result;

    freq = *(uint32*) (data_ptr->msg.content.addr); //单位:kHz
    result = fm_set_freq(freq);

    /* 设置频点成功*/ //0成功 -1失败
    if (result != -1)
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}


//进行软件搜台
app_result_e fmengine_set_softseek(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    //uint16 now_freq;
    int result;
    //uint8 dir;
    seek_param_t soft_param;

    seek_param_t* param_ptr = (seek_param_t*) (data_ptr->msg.content.addr);

    //软件搜台接口函数参数
    soft_param.freq = param_ptr->freq; //软件搜台当前判断频点

    soft_param.dir = param_ptr->dir; //搜台方向
    //for search too long
    change_engine_state(ENGINE_STATE_PLAYING);

    result = fm_soft_seek(soft_param.freq, soft_param.dir);
    //-1:没信号
    //0: 有信号
    //1: 立体声

    g_fmengine_status.FM_CurrentFreq = soft_param.freq; //更新引擎层当前频点值

    if (result != -1) //0成功 -1失败
    {
        //有效电台
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
    }
    else
    {
        //无效电台
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;

}

//启动硬件搜台
app_result_e fmengine_set_hardseek(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    //uint16 now_freq;
    //uint8 dir;
    int result;
    seek_param_t hard_param;

    seek_param_t* param_ptr = (seek_param_t*) (data_ptr->msg.content.addr);

    //软件搜台接口函数参数
    hard_param.freq = param_ptr->freq; //软件搜台当前判断频点
    //dir = data_ptr->msg.content.data[2]; //搜台方向
    hard_param.dir = param_ptr->dir; //搜台方向

    //硬件搜台接口函数参数
    //now_freq = *(uint16*) &(data_ptr->msg.content.data[0]); //硬件搜台起始频点
    //dir = data_ptr->msg.content.data[2]; //搜台方向
    //for search too long
    change_engine_state(ENGINE_STATE_PLAYING);
    //启动硬件搜台
    result = fm_hard_seek(hard_param.freq, hard_param.dir);

    if (result != -1) //0成功 -1失败
    {
        //启动成功
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        g_fmengine_status.STC_flag = HARDSEEK_DOING;
    }
    else
    {
        //启动失败
        data_ptr->reply->type = MSG_REPLY_FAILED;
        g_fmengine_status.STC_flag = HARDSEEK_NOT_START;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

//中断硬件搜台
app_result_e fmengine_seek_break(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;

    result = fm_set_breakseek();

    //成功停止硬件搜台//0成功 -1失败
    if (result != -1)
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        fm_get_freq(&g_fmengine_status.FM_CurrentFreq); //更新当前频率
        g_fmengine_status.STC_flag = HARDSEEK_COMPLETE_NOT_VALID;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;

}
