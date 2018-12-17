#include "app_fmengine.h"


//获取状态信息
app_result_e fmengine_get_status(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;
    Engine_Status_t engine_status_tmp;
    uint8 mode;

    //获取状态节点:   0-- 正常播放时取   1-- 硬件seek 过程中取
    mode = data_ptr->msg.content.data[0];
    result = fm_get_status(&engine_status_tmp, mode);

    //获取状态成功//0成功 -1失败
    if (result != -1)
    {
        engine_status_tmp.FM_VolStatus = g_fmengine_status.FM_VolStatus;
        libc_memcpy(&g_fmengine_status, &engine_status_tmp, sizeof(Engine_Status_t));

        //消息应答
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        //将引擎状态回复给RadioUI
        libc_memcpy(data_ptr->reply->content, &g_fmengine_status, sizeof(Engine_Status_t));
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

//获取波段
app_result_e fmengine_get_band(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;
    result = fm_get_band(&g_fmengine_status.FM_CurBand);

    if (result != -1) //0成功 -1失败
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        *(radio_band_e*) data_ptr->reply->content = g_fmengine_status.FM_CurBand;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;

}


//获取当前信号强度
app_result_e fmengine_get_intensity(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;

    result = fm_get_intensity(&g_fmengine_status.FM_SignalStg);
    if (result != -1) //0成功 -1失败
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        *(uint16*) data_ptr->reply->content = g_fmengine_status.FM_SignalStg;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

//获取天线状态
app_result_e fmengine_get_antenna(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;
    uint8 antena_status;
    result = fm_get_antena(&antena_status);

    if (result != -1) //0成功 -1失败
    {
        //获取状态成功
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        //回执，天线是否插入
        *(uint8*) data_ptr->reply->content = antena_status;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

//获取立体声状态
app_result_e fmengine_get_stereo(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;
    int result;
    result = fm_get_stereo(&g_fmengine_status.FM_Stereo_Status);

    if (result != -1) //0成功 -1失败
    {
        data_ptr->reply->type = MSG_REPLY_SUCCESS;
        *(FM_Audio_e*) data_ptr->reply->content = g_fmengine_status.FM_Stereo_Status;
    }
    else
    {
        data_ptr->reply->type = MSG_REPLY_FAILED;
    }
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

//获取当前引擎状态(播放or  暂停)
app_result_e fmengine_get_playflag(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;

    *(FM_Playstatus_e*) data_ptr->reply->content = g_fmengine_status.FM_VolStatus;
    data_ptr->reply->type = MSG_REPLY_SUCCESS;
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

//获取FM 模组当前状态(Work/Standby)
app_result_e fmengine_get_modual_status(private_msg_t* cur_msg)
{
    private_msg_t* data_ptr = (private_msg_t*) cur_msg;

    *(Module_Status_e*) data_ptr->reply->content = g_module_status;
    data_ptr->reply->type = MSG_REPLY_SUCCESS;
    //回复同步消息(发送信号量)
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

