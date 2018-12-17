#include "app_linein_eg.h"
#include <analog.h>

app_result_e linein_eg_message_done_bank(private_msg_t* cur_msg)
{
    g_linein_eg_result = RESULT_NULL;

    switch (cur_msg->msg.type)
    {
        /*设置参数相关*/
        //case MSG_LINEIN_EG_SET_EQ_SYNC://设置eq +eq curve data
        case MSG_SET_DAE_CFG_SYNC:
        linein_eg_set_eq(cur_msg);
        break;

        /*播放相关*/
        case MSG_LINEIN_EG_PLAY_SYNC: //开始播放
        linein_eg_play(cur_msg);
        break;

        case MSG_LINEIN_EG_STOP_SYNC: //停止播放
        linein_eg_stop(cur_msg);
        break;

        case MSG_ENGINE_STANDBY_SYNC:
        g_linein_eg_result = linein_eg_standby(cur_msg);
        break;

        case MSG_APP_QUIT: //退出app
        if ((cur_msg->msg.content.data[0] & 0x01) != 0)
        {
            PRINT_INFO("eg enter s3bt");
        }
        g_linein_eg_result = RESULT_APP_QUIT;
        break;

        case MSG_TTS_START_SYNC: //准备进入TTS播报
        case MSG_SWITCH_AOUT_START_SYNC: //准备切换音频输出
        g_linein_eg_result = linein_eg_tts_start(cur_msg);
        break;

        case MSG_TTS_STOP_SYNC: //TTS播报完成
        case MSG_SWITCH_AOUT_STOP_SYNC: //完成切换音频输出
        g_linein_eg_result = linein_eg_tts_stop(cur_msg);
        break;

        default:
        break;
    }

    return g_linein_eg_result;
}

//播放
app_result_e linein_eg_play(void* msg_ptr)
{
    bool ret_vals;

    if (g_linein_channel_sel == LINEIN_CH_A)
    {
        ret_vals = linein_eg_aa_play();
        g_eg_status_p->play_status = linein_play_status_play; //设置播放状态
    }
    else
    {
        //设置文件播放
        ret_vals = play(); //播放
    }

    linein_eg_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

//暂停
app_result_e linein_eg_stop(void* msg_ptr)
{
    bool ret_vals;

    if (g_linein_channel_sel == LINEIN_CH_A)
    {
        ret_vals = linein_eg_aa_stop();
        g_eg_status_p->play_status = linein_play_status_stop;
    }
    else
    {
        ret_vals = stop(); //停止播放
    }

    linein_eg_reply_msg(msg_ptr, ret_vals);

    return RESULT_IGNORE;
}

void _error_handle(void)
{
    //关闭解码中间件
    if (g_linein_channel_sel != LINEIN_CH_A)
    {
        stop();
    }
}

//模拟通道时，播放
bool linein_eg_aa_play(void)
{
    uint8 gain_val = 2;
    gain_val = (uint8) com_get_config_default(LINEIN_GAIN_INPUT);


    enable_ain(AI_SOURCE_LINEIN, (uint32) gain_val);

    sys_mdelay(200);

    //enable_adc(2, com_get_config_default(LINEIN_GAIN_ADC));

    //disable_aa_mute();
    enable_ain_out(AI_SOURCE_LINEIN);

    return TRUE;
}

//模拟通道时，暂停
bool linein_eg_aa_stop(void)
{
    //enable_aa_mute();
    disable_ain_out(AI_SOURCE_LINEIN);

    disable_ain(AI_SOURCE_LINEIN);

    //disable_adc();

    return TRUE;
}

