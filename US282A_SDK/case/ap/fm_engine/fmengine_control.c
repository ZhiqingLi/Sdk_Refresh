#include "app_fmengine.h"

app_result_e fmengine_message_deal_bank(private_msg_t* cur_msg) __FAR__;

void fm_eg_reply_msg(private_msg_t *private_msg)
{
    libc_sem_post(private_msg->sem);
}

//处理从前台发来的消息
app_result_e __section__(".bank") fmengine_message_deal_bank(private_msg_t* cur_msg)
{
    app_result_e ret_value = RESULT_NULL;

    switch (cur_msg->msg.type)
    {
        /* 模组初始化及卸载相关*/
        /*打开Fm模组*/
        case MSG_FM_MODE_OPEN:
        ret_value = fm_module_open(cur_msg);
        break;

        /*打开Fm音频输入通道*/
        case MSG_FM_AIN_OPEN:
        ret_value = fm_Ain_open(cur_msg);
        break;

        /* FM 模组进入Standby */
        case MSG_FMENGINE_STANDBY:
        ret_value = fm_module_standby();
        break;

        case MSG_ENGINE_STANDBY_SYNC:
        ret_value = fmengine_standby(cur_msg);
        break;

        case MSG_APP_QUIT: //退出app
        /* 要求引擎退出，返回后卸载驱动*/
        if ((cur_msg->msg.content.data[0] & 0x01) != 0)
        {
            PRINT_INFO("eg enter s3bt");
        }
        case MSG_FMENGINE_CLOSE:
        /*其他进程也可向FM 引擎发MSG_FMENGINE_CLOSE  消息要求FM 引擎退出*/
        if (g_module_status != MODULE_STANDBY)
        {
            fm_module_standby();
        }
        ret_value = RESULT_APP_QUIT;
        break;

        /* 参数设置相关*/

        /* 静音或解除静音*/
        case MSG_FMENGINE_SETMUTE:
        ret_value = fmengine_mute_sync(cur_msg);
        break;

        /* 设置电台波段*/
        case MSG_FMENGINE_SETBAND:
        ret_value = fmengine_set_band_sync(cur_msg);
        break;

        /* 设置搜台门限*/
        case MSG_FMENGINE_SETTHRED:
        ret_value = fmengine_set_throd(cur_msg);
        break;


        /* */
        case MSG_FMENGINE_PLAY:
        ret_value = fmengine_play(cur_msg);
        break;

        /* */
        case MSG_FMENGINE_PAUSE:
        ret_value = fmengine_stop(cur_msg);
        break;

        /* 获取信息相关*/

        /* 获取当前状态信息*/
        case MSG_FMENGINE_GET_STATUS:
        ret_value = fmengine_get_status(cur_msg);
        break;

        /* 获取当前波段信息*/
        case MSG_FMENGINE_GET_BAND:
        ret_value = fmengine_get_band(cur_msg);
        break;

        /* 获取当前电台信号强度信息*/
        case MSG_FMENGINE_GET_INTENSITY:
        ret_value = fmengine_get_intensity(cur_msg);
        break;

        /* 获取天线状态( 耳机是否插入)*/
        case MSG_FMENGINE_GET_ANTENNA:
        ret_value = fmengine_get_antenna(cur_msg);
        break;

        /* 获取立体声状态信息*/
        case MSG_FMENGINE_GET_STEREO:
        ret_value = fmengine_get_stereo(cur_msg);
        break;

        /* 获取当前引擎状态*/
        case MSG_FMENGINE_GET_PLAYSTATUS:
        ret_value = fmengine_get_playflag(cur_msg);
        break;

        /* 获取当前模组状态*/
        case MSG_FMENGINE_GET_MODUAL_STATUS:
        ret_value = fmengine_get_modual_status(cur_msg);
        break;

        /*!设置eq*/
        case MSG_SET_DAE_CFG_SYNC:
        //if (g_fm_chanel == 1)
        //{
        ret_value = fmengine_set_eq(cur_msg);
        //}
        break;

        case MSG_TTS_START_SYNC: //准备进入TTS播报
        case MSG_SWITCH_AOUT_START_SYNC: //准备切换音频输出
        ret_value = fmengine_tts_start(cur_msg);
        break;

        case MSG_TTS_STOP_SYNC: //TTS播报完成
        case MSG_SWITCH_AOUT_STOP_SYNC: //完成切换音频输出
        ret_value = fmengine_tts_stop(cur_msg);
        break;

        case MSG_FMENGINE_DEBUG:
        ret_value = fmengine_debug(cur_msg);
        break;

        default:
        break;
    }
    return ret_value;
}

//处理从前台发来的消息
app_result_e fmengine_message_deal(private_msg_t* cur_msg)
{
    app_result_e ret_value = RESULT_NULL;

    switch (cur_msg->msg.type)
    {
        /* 参数设置相关*/

        /* 同步设置频点播放*/
        case MSG_FMENGINE_SETFREQ_SYNC:
        ret_value = fmengine_set_freq_sync(cur_msg);
        break;

        /* 启动软件搜台*/
        case MSG_FMENGINE_SET_SOFTSEEK:
        ret_value = fmengine_set_softseek(cur_msg);
        break;

        /* 启动硬件搜台*/
        case MSG_FMENGINE_SET_HARDSEEK:
        ret_value = fmengine_set_hardseek(cur_msg);
        break;

        /* 退出硬件seek 过程*/
        case MSG_FMENGINE_SET_SEEKBREAK:
        ret_value = fmengine_seek_break(cur_msg);
        break;

        /* 获取信息相关*/

        /* 获取硬件自动搜台是否完成标识*/
        case MSG_FMENGINE_AUTO_SEEK_OVER:
        ret_value = fmengine_get_seekflag(cur_msg);
        break;

        /* 获取当前频率信息*/
        case MSG_FMENGINE_GET_FREQ:
        ret_value = fmengine_get_freq(cur_msg);
        break;

        default:
        ret_value = fmengine_message_deal_bank(cur_msg);
        break;
    }
    return ret_value;
}

//FM 引擎应用为被动模块，也是其他应用进程和FM 驱动通讯的接口模块
app_result_e fmengine_control_block(void)
{
    app_result_e result = RESULT_NULL;
    private_msg_t fmengine_msg;
    //uint32 msg_type;

    while (1)
    {
        //获取并处理消息
        if (get_app_msg(&fmengine_msg) == TRUE)
        {
            result = fmengine_message_deal(&fmengine_msg);
            if (result == RESULT_APP_QUIT)
            {
                break;
            }
        }
        //挂起10ms 多任务交互
        sys_os_time_dly(1);
    }
    return result;
}
