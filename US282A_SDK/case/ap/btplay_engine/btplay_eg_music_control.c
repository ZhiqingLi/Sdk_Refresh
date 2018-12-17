/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌引擎应用解码线程加载和卸载接口实现。
 * 作者：cailizhen
 ********************************************************************************/

#include "btplay_engine.h"

static const uint8 s_output_channel[] =
{ OUTPUT_CHANNEL_NORMAL, //正常，中间件会初始化为正常
        OUTPUT_CHANNEL_L_R_SWITCH, //左右声道互换 L = R0, R = L0
        OUTPUT_CHANNEL_L_R_MERGE,//左右声道合并后输出到左声道和右声道 L = R = (L0 + R0) / 2
        OUTPUT_CHANNEL_L_R_ALL_L,//左右声道都输出左声道 L = R = L0
        OUTPUT_CHANNEL_L_R_ALL_R, //左右声道都输出右声道 L = R = R0
        };

void a2dp_decode_init(uint8 media_type)
{
    int mmm_init_ret;
    int result;
    mmm_bp_set_type_param_t type_pram;
    mmm_bp_aout_setting_t mmm_bp_aout_param;
    char *mmm_name =
    { "mmm_bp.al" };

    btplay_gl_var.media_type = media_type;
    {
    	  mmm_bp_aout_param.asrc_index = K_OUT0_U0;
        mmm_bp_aout_param.asrc_mode_sel = 2;
        mmm_bp_aout_param.dac_chanel = DAF0_EN; 
    }

    if (media_type == A2DP_CODEC_SBC)
    {
        type_pram.music_type = TYPE_SBC;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
        type_pram.work_mode = g_p_bt_stack_cur_info->dev_role;
#else
        type_pram.work_mode = 0;
#endif
        type_pram.play_delay_start = btplay_gl_var.sbc_delay_start;
        type_pram.play_delay_lowest = btplay_gl_var.sbc_delay_lowest;
    }
    else if (media_type == A2DP_CODEC_AAC)
    {
        type_pram.music_type = TYPE_AAC;
        type_pram.play_delay_start = btplay_gl_var.aac_delay_start;
        type_pram.play_delay_lowest = btplay_gl_var.aac_delay_lowest;
    }
    else if (media_type == A2DP_CODEC_MPEG12)
    {
        type_pram.music_type = TYPE_MP3;
        type_pram.play_delay_start = 161;
        type_pram.play_delay_lowest = 161;
    }
    else
    {
        PRINT_ERR("btplay engine error media type!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }

    com_set_sound_out(SOUND_OUT_START);

    btplay_gl_var.codec_flag = TRUE;
    g_p_btplay_info->status = BTPLAY_PAUSE;
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if(g_p_bt_stack_cur_info->dev_role!=0)
    {
        if (type_pram.music_type == TYPE_SBC)
        {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL8, FREQ_LEVEL6);
            PRINT_INFO("level8:");
        }
        else if (type_pram.music_type == TYPE_AAC)
        {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL8, FREQ_LEVEL3);
        }
        else //TYPE_MP3

        {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL8, FREQ_LEVEL3);
        }
    }
    else
#endif
    {
        if (type_pram.music_type == TYPE_SBC)
        {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL3);
        }
        else if (type_pram.music_type == TYPE_AAC)
        {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL3);
        }
        else //TYPE_MP3
        {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL3);
        }
    }

    adjust_freq_add_value(0, g_dae_cfg_shm->run_freq);

    //加载中间件驱动
    sys_load_mmm(mmm_name, TRUE);

    //初始化中间件库
    mmm_init_ret = mmm_bp_cmd(&mp_handle, MMM_BP_OPEN, (unsigned int) NULL);
    if (mmm_init_ret == 0)
    {
        if(1 == g_app_info_state_all.fix_sample_rate_flag)
        {
            mmm_bp_cmd(mp_handle, MMM_BP_FIX_SAMPLE_RATE, (unsigned int) NULL);
        }
        
        mmm_bp_cmd(mp_handle, MMM_BP_AOUT_OPEN, (unsigned int) &mmm_bp_aout_param);

        mmm_bp_cmd(mp_handle, MMM_BP_SET_MUSIC_TYPE, (unsigned int) &type_pram);

        //输出声道处理选择
        //#define OUTPUT_CHANNEL_NORMAL      0 //正常，中间件会初始化为正常
        //#define OUTPUT_CHANNEL_L_R_SWITCH  1 //左右声道互换 L = R0, R = L0
        //#define OUTPUT_CHANNEL_L_R_MERGE   2 //左右声道合并后输出到左声道和右声道 L = R = (L0 + R0) / 2
        //#define OUTPUT_CHANNEL_L_R_ALL_L   3 //左右声道都输出左声道 L = R = L0
        //#define OUTPUT_CHANNEL_L_R_ALL_R   4 //左右声道都输出右声道 L = R = R0
        //mmm_bp_cmd(mp_handle, MMM_BP_SET_OUTPUT_CHANNEL, (unsigned int) OUTPUT_CHANNEL_NORMAL);
        
		 mmm_bp_cmd(mp_handle, MMM_BP_SET_OUTPUT_CHANNEL,btplay_gl_var.output_channel_config);
       
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //临时这样处理
        if (g_p_bt_stack_cur_info->dev_role == 1)
        {
            //左声道
            mmm_bp_cmd(mp_handle, MMM_BP_SET_OUTPUT_CHANNEL,
                    (unsigned int) s_output_channel[btplay_gl_var.tws_m_output_channel]);

            PRINT_INFO_INT("channel:",s_output_channel[btplay_gl_var.tws_m_output_channel]);
        }
        else if (g_p_bt_stack_cur_info->dev_role == 2)
        {
            //右声道
            mmm_bp_cmd(mp_handle, MMM_BP_SET_OUTPUT_CHANNEL,
                    (unsigned int) s_output_channel[btplay_gl_var.tws_s_output_channel]);

            PRINT_INFO_INT("channel:",s_output_channel[btplay_gl_var.tws_s_output_channel]);
        }
        else
        {
            ;// do nothing
        }
#endif
        //SBC编解码时是否抑制噪声
        result = mmm_bp_cmd(mp_handle, MMM_BP_RESTRAIN_NOISE,TRUE);

#ifdef WAVES_ASET_TOOLS
         //发送播放命令
        g_waves.bin_number = g_app_info_state_all.bin_number;
        result = mmm_bp_cmd(mp_handle, MMM_BP_PLAY, (unsigned int) &g_waves); 
#else
        //发送播放命令
        result = mmm_bp_cmd(mp_handle, MMM_BP_PLAY, (unsigned int) NULL);      
#endif

        if (result == 0)
        {
            PRINT_INFO("btplay mmm init");
#ifdef ENABLE_TRUE_WIRELESS_STEREO
            uint32 temp_flag;
            temp_flag = sys_local_irq_save();
            g_p_tws_info->mmm_init_flag = 1;
            sys_local_irq_restore(temp_flag);
#endif            
            mmm_bp_cmd(mp_handle, MMM_BP_GET_ENERGY_INFO, (unsigned int) &(g_app_info_state_all.p_energy_value));
            g_app_info_state_all.energy_available_flag = TRUE;
        }
    }
}

void a2dp_decode_quit(void)
{
    uint32 temp_flag;
    
    if (mp_handle != NULL)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //卸载中间件,提前关闭bt wake host中断
        if(g_p_bt_stack_cur_info->dev_role!=0)
        {        	    
          temp_flag = sys_local_irq_save();
          
          act_writel(act_readl(INTC_MSK)& 0xfffffffe, INTC_MSK);
        	act_writel(act_readl(BIAS_CTL) & 0xfffffffb, BIAS_CTL);
        	act_writel(0x00000005, WAKE_PD); //clear bt onoff pending
        	g_p_tws_info->mmm_init_flag = 0;
        	g_p_tws_info->start_flag = 0;
        	sys_local_irq_restore(temp_flag);
        }	
#endif        
        g_app_info_state_all.energy_available_flag = FALSE;
#ifdef WAVES_ASET_TOOLS
        mmm_bp_cmd(mp_handle, MMM_BP_STOP, (unsigned int) &g_waves);    
#else
        mmm_bp_cmd(mp_handle, MMM_BP_STOP, (unsigned int) NULL);    
#endif
        mmm_bp_cmd(mp_handle, MMM_BP_AOUT_CLOSE, (unsigned int) NULL);
        //关闭中间件设备
        mmm_bp_cmd(mp_handle, MMM_BP_CLOSE, (unsigned int) NULL);

        //卸载解码驱动
        sys_free_mmm(TRUE);
        mp_handle = NULL;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
#if 0
       temp_flag = sys_local_irq_save();
       g_p_tws_info->tws_send_buf_base = 0x9fc38000;
       sys_local_irq_restore(temp_flag);
#endif
#endif        
    }
    btplay_gl_var.codec_flag = FALSE;
    PRINT_INFO("btplay mmm exit");

    adjust_freq_add_value(0, 0);

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);

    com_set_sound_out(SOUND_OUT_STOP);
}

