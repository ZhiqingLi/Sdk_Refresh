/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌引擎应用解码线程加载和卸载接口实现。
 * 作者：
 ********************************************************************************/

#include "linein_engine.h"

static void get_noise_reduction_para(noise_reduction_t* p_noise_reduction)
{
    noise_reduction_config_t noise_reduction_temp;
    
    com_get_config_struct((uint16) NOISE_AUX_REDUCTION, (uint8 *) &noise_reduction_temp, sizeof(noise_reduction_config_t));
    libc_memcpy(p_noise_reduction, &noise_reduction_temp.noise_reduction, sizeof(noise_reduction_t));    
}

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
    noise_reduction_t noise_reduction;
    mmm_bp_set_type_param_t type_pram;
    mmm_bp_aout_setting_t mmm_bp_aout_param;
    char *mmm_name =
    { "mmm_bp.al" };

    btplay_gl_var.media_type = media_type;
    
    if(g_app_info_state_all.stub_mode_flag!=0)
    {
        mmm_bp_aout_param.asrc_index = K_OUT1_P2;
        mmm_bp_aout_param.asrc_mode_sel = 0;
        mmm_bp_aout_param.dac_chanel = DAF1_EN; 
    }
    else 
    {
    	  mmm_bp_aout_param.asrc_index = K_OUT0_U0;
        mmm_bp_aout_param.asrc_mode_sel = 2;
        mmm_bp_aout_param.dac_chanel = DAF0_EN; 
    }

    if (media_type == A2DP_CODEC_SBC)
    {
        type_pram.music_type = TYPE_SBC;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
        type_pram.work_mode = (g_p_bt_stack_cur_info->dev_role | 0xf0);
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
    //g_p_btplay_info->status = BTPLAY_PAUSE;
    if (linein_tws_playpause_flag == 0)
    {
    	g_p_btplay_info->status = BTPLAY_PLAY;
    	libc_print("@1",0,0);
    }	
    
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if(g_p_bt_stack_cur_info->dev_role!=0)
    {
        if (type_pram.music_type == TYPE_SBC)
        {
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL11, FREQ_LEVEL11);
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
            adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL11, FREQ_LEVEL11);
            PRINT_INFO("level11:");
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

    libc_print("@tty:",type_pram.work_mode,2);
    //加载中间件驱动
    sys_load_mmm(mmm_name, TRUE);

    //初始化中间件库
    mmm_init_ret = mmm_bp_cmd(&mp_handle, MMM_BP_OPEN, (unsigned int) NULL);
    if (mmm_init_ret == 0)
    {
        //mmm_bp_cmd(&mp_handle, MMM_BP_FADE_IN_TIME, (unsigned int) 0);//设置淡入时间
	    mmm_bp_cmd(&mp_handle, MMM_BP_FADE_OUT_TIME, (unsigned int) 0);//设置淡出时间
        mmm_bp_cmd(mp_handle, MMM_BP_AOUT_OPEN, (unsigned int)& mmm_bp_aout_param);

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
        result = mmm_bp_cmd(mp_handle, MMM_BP_RESTRAIN_NOISE, FALSE);

#ifdef WAVES_ASET_TOOLS
         //发送播放命令
        g_waves.bin_number = g_app_info_state_all.bin_number;
        result = mmm_bp_cmd(mp_handle, MMM_BP_PLAY, (unsigned int) &g_waves); 
#else
		get_noise_reduction_para(&noise_reduction);
        result = mmm_bp_cmd(mp_handle, MMM_BP_SET_NOISE_REDUCTION_PARA, &noise_reduction);
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
    
    //sample_sound_exit();
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
    g_p_btplay_info->status = BTPLAY_PAUSE;
    PRINT_INFO("btplay mmm exit");

    adjust_freq_add_value(0, 0);

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);

    com_set_sound_out(SOUND_OUT_STOP);
    
}


static void start_sample_sound(uint32 dst_addr, uint32 block_length)
{

    //abort DMA stransfer and reset config
    *((REG32) (DMA3CTL)) = 0x00;

    //data width 16bit
    act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_DATAWIDTH_SHIFT), DMA3CTL);

    //source addr type:adc fifo
    act_writel(act_readl(DMA3CTL) | (0x0b << DMA3CTL_SRCTYPE_SHIFT), DMA3CTL);

    //dest addr type:memory
    act_writel(act_readl(DMA3CTL) | (0 << DMA3CTL_DSTTYPE_SHIFT), DMA3CTL);


    //dest address
    act_writel(dst_addr, DMA3DADDR0);

    //dma length
    act_writel(block_length / 2, DMA3FrameLen);
   
    
    //dm3 half & complete int disable
    act_writel(act_readl(DMAIE) & (~0x00000808), DMAIE);

    //clear pending，注意不能使用读-或-写这种方式，避免清掉其他pending位
    act_writel(0x00000808, DMAIP);

    //临时注释掉
    //reload enable
    act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_reload), DMA3CTL);
    
    //enable transfer
    act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_DMA3START), DMA3CTL);

    //挂接中断服务程序
    sys_request_irq(IRQ_DMA3, pcm_data_deal);

    //dm3 half & complete int enable
    act_writel(act_readl(DMAIE) | 0x00000808, DMAIE);
}


/******************************************************************************/
/*!
 * \par  Description:
 * \初始化ADC函数及AIN通道
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      bt_common_gen_addr.c
 * \note
 */
/*******************************************************************************/
void sample_sound_init(void)
{
    if (sample_start_flag == 0)
    {	  
       
//        act_writel(act_readl(GPIOAOUTEN) | (1<<22), GPIOAOUTEN);
//        act_writel(act_readl(GPIOADAT) | (1<<22), GPIOADAT);
//        act_writel(act_readl(GPIOADAT) & (~(1<<22)), GPIOADAT);

        if(g_p_bt_stack_cur_info->dev_role == TWS_MASTER)
        {
        	set_notice_func(TWS_SYNC_START_CNT);
        }              
        
//        act_writel(act_readl(GPIOAOUTEN) | (1<<22), GPIOAOUTEN);
//        act_writel(act_readl(GPIOADAT) | (1<<22), GPIOADAT);
//        act_writel(act_readl(GPIOADAT) & (~(1<<22)), GPIOADAT);
                  
        start_sample_sound(ADC_SAMPLE_BUFFER,ADC_SAMPLE_BUFFER_LEN);
              
        sample_start_flag = 1;
        //libc_print("adc en",0,0);
        
//        act_writel(act_readl(GPIOAOUTEN) | (1<<22), GPIOAOUTEN);
//        act_writel(act_readl(GPIOADAT) | (1<<22), GPIOADAT);
//        act_writel(act_readl(GPIOADAT) & (~(1<<22)), GPIOADAT);

    } 
}
void adc_set_init(void)
{
    if((adc_init_flag == 0)&&(g_p_bt_stack_cur_info->dev_role != TWS_SLAVE))
    {
        uint8 ain_gain_tmp = (uint8) com_get_config_default(LINEIN_GAIN_INPUT);
        uint8 adc_gain_tmp = (uint8) com_get_config_default(LINEIN_GAIN_ADC); 
        
        enable_adc(1, adc_gain_tmp);
        
        set_adc_rate(44);
        
        enable_ain(AI_SOURCE_LINEIN, ain_gain_tmp); 
        
        adc_init_flag = 1; 
    }
 	
}

void adc_set_exit(void)
{
    if(adc_init_flag == 1)
    {
        adc_init_flag = 0;
        disable_ain(AI_SOURCE_LINEIN);
        disable_adc();      	
    }
}
void sample_sound_exit(void)
{

    uint32 temp_flag;
    temp_flag = sys_local_irq_save();
    //exit for slave
//    if(g_p_bt_stack_cur_info->dev_role == TWS_SLAVE)
//    {
//    	adc_set_exit();
//    }   
    //*((REG32) (DMA3CTL)) = 0x00;
    act_writel(0x00,DMA3CTL);
    act_writel(0x00000808, DMAIP);
    sys_free_irq(IRQ_DMA3);
    sample_start_flag = 0;
    sys_local_irq_restore(temp_flag);    
    libc_print("adc disen",0,0);
}

void sbc_encode_init(void)
{
	 act_writel(act_readl(CMU_MEMCLKSEL) & 0xfcfeffcf, CMU_MEMCLKSEL);//uarm1,pcmram1 card fifo cpu clock
   
   sys_drv_install(DRV_GROUP_STG_UHOST, NULL, "sbc_enc.drv");
        
   sbc_drv_encode_init(&sbc_obj, 0);	
}

void sbc_encode_exit(void)
{	
    bt_drv_encode_finish(&sbc_obj);
	 sys_drv_uninstall(DRV_GROUP_STG_UHOST);
}


