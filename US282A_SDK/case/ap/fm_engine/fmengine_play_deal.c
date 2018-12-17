#include "app_fmengine.h"
#include <analog.h>

//MMM_PP_PLAY是否发出了
static bool s_mmm_pp_played = FALSE;

//ADDA模式时才用: 给中间件发送PLAY
bool play(void)
{
    int result;

    if (s_mmm_pp_played == TRUE)
    {
        return TRUE;
    }

    //重新调整频率，线程与TTS复用
    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL2);

    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

    com_set_sound_out(SOUND_OUT_RESUME);

    mmm_pp_cmd(&g_mmm_pp_handle, MMM_PP_OPEN, (unsigned int) NULL);
    /*
    if(1 == g_app_info_state_all.fix_sample_rate_flag)
    {
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_FIX_SAMPLE_RATE, (unsigned int) NULL);
    }
    */
    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_OPEN, (unsigned int) &g_ainout_param);

    //输出声道处理选择
    //#define OUTPUT_CHANNEL_NORMAL      0 //正常，中间件会初始化为正常
    //#define OUTPUT_CHANNEL_L_R_SWITCH  1 //左右声道互换 L = R0, R = L0
    //#define OUTPUT_CHANNEL_L_R_MERGE   2 //左右声道合并后输出到左声道和右声道 L = R = (L0 + R0) / 2
    //#define OUTPUT_CHANNEL_L_R_ALL_L   3 //左右声道都输出左声道 L = R = L0
    //#define OUTPUT_CHANNEL_L_R_ALL_R   4 //左右声道都输出右声道 L = R = R0
    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_OUTPUT_CHANNEL, g_output_channel_config);

#ifdef WAVES_ASET_TOOLS
    g_waves.bin_number = g_app_info_state_all.bin_number;
    result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY,(unsigned int) &g_waves);
#else
    //发送播放命令
    result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, (unsigned int) NULL);
#endif

    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_ENERGY_INFO, (unsigned int) &(g_app_info_state_all.p_energy_value));
    g_app_info_state_all.energy_available_flag = TRUE;

    s_mmm_pp_played = TRUE;

    return result;
}

//ADDA模式时才用: 给中间件发送CLOSE
bool stop(void)
{
    bool bret = TRUE;
    int result = 0;

    if (s_mmm_pp_played != TRUE)
    {
        return bret;
    }

    g_app_info_state_all.energy_available_flag = FALSE;
    
#ifdef WAVES_ASET_TOOLS
    result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) &g_waves);
#else
    //停止播放
    result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) NULL);
#endif

    if (result != 0)
    {
        bret = FALSE;
    }
    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_CLOSE, (unsigned int) NULL);
    s_mmm_pp_played = FALSE;

    //关闭中间件设备
    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_CLOSE, (unsigned int) NULL);

    //关闭音频输出
    com_set_sound_out(SOUND_OUT_PAUSE);

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL2, FREQ_NULL);
    adjust_freq_add_value(0, 0);

    return bret;
}
//封装play接口
app_result_e fmengine_play(private_msg_t* cur_msg)
{
    play();

    cur_msg->reply->type = MSG_REPLY_SUCCESS;
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}
//封装STOP接口
app_result_e fmengine_stop(private_msg_t* cur_msg)
{
    stop();

    cur_msg->reply->type = MSG_REPLY_SUCCESS;
    fm_eg_reply_msg(cur_msg);
    return RESULT_IGNORE;
}

