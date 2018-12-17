#include "app_linein_eg.h"

static void get_noise_reduction_para(noise_reduction_t* p_noise_reduction)
{
    noise_reduction_config_t noise_reduction_temp;
    
    com_get_config_struct((uint16) NOISE_AUX_REDUCTION, (uint8 *) &noise_reduction_temp, sizeof(noise_reduction_config_t));
    libc_memcpy(p_noise_reduction, &noise_reduction_temp.noise_reduction, sizeof(noise_reduction_t));    
}

//数字通道时的播放
bool play(void)
{
    int result;

    bool bret = TRUE;
    noise_reduction_t noise_reduction;
       
    //与TTS复用线程，必须重新设置频率
    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL2);

    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);
    
    //停止 or 暂停时 发送播放命令
    if (g_eg_status_p->play_status == linein_play_status_stop)
    {
        //初始化中间件库
        mmm_pp_cmd(&g_mmm_pp_handle, MMM_PP_OPEN, (unsigned int) NULL);

        /*
        if(1 == g_app_info_state_all.fix_sample_rate_flag)
        {
            mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_FIX_SAMPLE_RATE, (unsigned int) NULL);
        }
        */
        //set sound open
        com_set_sound_out(SOUND_OUT_RESUME);

        g_eg_status_p->play_status = linein_play_status_play; //设置播放状态

        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_OPEN, (unsigned int) &g_ainout_param);

	    //输出声道处理选择
	    //#define OUTPUT_CHANNEL_NORMAL      0 //正常，中间件会初始化为正常
	    //#define OUTPUT_CHANNEL_L_R_SWITCH  1 //左右声道互换 L = R0, R = L0
	    //#define OUTPUT_CHANNEL_L_R_MERGE   2 //左右声道合并后输出到左声道和右声道 L = R = (L0 + R0) / 2
	    //#define OUTPUT_CHANNEL_L_R_ALL_L   3 //左右声道都输出左声道 L = R = L0
        //#define OUTPUT_CHANNEL_L_R_ALL_R   4 //左右声道都输出右声道 L = R = R0
        
	    mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_OUTPUT_CHANNEL, g_output_channel_config);
		
#ifdef WAVES_ASET_TOOLS
        //发送播放命令
        g_waves.bin_number = g_app_info_state_all.bin_number;
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, (unsigned int) &g_waves);
#else
        get_noise_reduction_para(&noise_reduction);
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_SET_NOISE_REDUCTION_PARA, &noise_reduction);
        //发送播放命令
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_PLAY, (unsigned int) NULL);     
#endif

        if (result != 0)
        {
            bret = FALSE;
            _error_handle();
        }
        else
        {
            mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_ENERGY_INFO, (unsigned int) &(g_app_info_state_all.p_energy_value));
            g_app_info_state_all.energy_available_flag = TRUE;

            bret = TRUE;
        }
    }

    return bret;
}

//数字通道时的停止
bool stop(void)
{
    bool bret = TRUE;
    int result = 0;
    
    //mmm_pp_status_t play_status;

    g_app_info_state_all.energy_available_flag = FALSE;
    
    /*mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_GET_STATUS, (unsigned int) &play_status); //获取后暂时没用

    if(play_status.status == MMM_PP_ENGINE_STOPPED)
    {
    	 //return bret;
    	 g_eg_status_p->play_status = linein_play_status_stop;
    }*/

    //正常播放才会停止
    if (g_eg_status_p->play_status == linein_play_status_play)
    {
#ifdef WAVES_ASET_TOOLS
        //停止播放
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) &g_waves);     
#else
        //停止播放
        result = mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_STOP, (unsigned int) NULL);
#endif
        if (result != 0)
        {
            bret = FALSE;
        }
        else
        {           
            bret = TRUE;
        }

        /*if (bret == FALSE)
        {
        }*/ 
        //设置状态
        g_eg_status_p->play_status = linein_play_status_stop;
        //关闭声音设备
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_AINOUT_CLOSE, (unsigned int) NULL);
        //关闭中间件设备
        mmm_pp_cmd(g_mmm_pp_handle, MMM_PP_CLOSE, (unsigned int) NULL);

        //关闭音频输出
        com_set_sound_out(SOUND_OUT_PAUSE);
    }

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL2, FREQ_NULL);
    adjust_freq_add_value(0, 0);

    return bret;
}
