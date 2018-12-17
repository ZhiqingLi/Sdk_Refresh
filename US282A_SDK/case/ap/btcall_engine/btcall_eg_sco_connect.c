/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   处理消息
 *      Cailizhen          2015-1          creat
 *       Wekan        2015-3-30       review
 *******************************************************************************/

#include "btcall_engine.h"


static sd_handle* get_asqt_peq_para_from_firmware(void)
{
    sd_handle asqt_para_bin_handle = NULL;
    char *p_asqt_para_bin = { "asqtpara.bin" };
    asqt_para_head_t 	asqt_para_head;
    
    asqt_para_bin_handle = sys_sd_fopen(p_asqt_para_bin);
	if (NULL == asqt_para_bin_handle)
	{
		PRINT_INFO("open asqtpara.bin fail");
	}
	else
	{
		sys_sd_fread(asqt_para_bin_handle, &asqt_para_head, sizeof(asqt_para_head_t));
		//PRINT_DATA(&asqt_para_head,sizeof(asqt_para_head_t));
		sys_sd_fread(asqt_para_bin_handle, asqt_peq_addr.band_peq1_addr, asqt_para_head.package_len_array[0]);
		//PRINT_DATA(asqt_peq_addr.band_peq1_addr,asqt_para_head.package_len_array[0]);
		sys_sd_fread(asqt_para_bin_handle, asqt_peq_addr.band_peq2_addr, asqt_para_head.package_len_array[1]);
		//PRINT_DATA(asqt_peq_addr.band_peq2_addr,asqt_para_head.package_len_array[1]);				
		g_p_share_mem->peq_info.band_peq1   = ADDR_M4K2DSP((uint32)asqt_peq_addr.band_peq1_addr);
		g_p_share_mem->peq_info.band_peq2   = ADDR_M4K2DSP((uint32)asqt_peq_addr.band_peq2_addr);
		g_p_share_mem->peq_info.change_flag = 1;
	}

    return asqt_para_bin_handle;
}


/******************************************************************************
 * \  par  Description:   加载中间件，包括初始化dac等接口和启动dsp
 * \  param[in]     null0
 * \  param[out]   null
 * \  return           null
 * \  note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/
void btcalleg_load_hfp_mmm(void)
{
    int mr_ret;
    char *mmm_name =
    { "mmm_hfp.al" };
    sv_configs_t *asqt_parm;
    //for mic config
    mmm_mr_ain_setting_t hfp_mic_cfg;
    mmm_hfp_aout_setting_t hfp_dac_cfg;
    sd_handle asqt_para_bin_handle = NULL;
    asqt_peq_status_t	peq_status; 
    uint32 hfp_dec_mode=0;

    hfp_mic_cfg.input_source = (uint8) com_get_config_default(SETTING_AUDIO_INPUT_CHANNAL);

    hfp_mic_cfg.input_gain = (uint8) com_get_config_default(BTCALL_MICIN_OP_GAIN);

    hfp_mic_cfg.adc_gain = (uint8) com_get_config_default(BTCALL_MICIN_ADC_GAIN);

    if(g_p_bt_stack_cur_info->msbc_code_flag == 1)
    {
        hfp_dac_cfg.default_samplerate = 16;
		PRINT_INFO("--MSBC--");
    }
    else 
    {
    	hfp_dac_cfg.default_samplerate = 8;
		PRINT_INFO("--CVSD--");
    }

    if ((btcall_gl_var.stub_asqt_en == 0) && (g_stub_tools_type == 0))
    {
        hfp_dac_cfg.drive_mode = K_OUT0_U0_IN_U1;
    }
    else
    {
        hfp_dac_cfg.drive_mode = K_OUT0_U0_IN_P2;
    }
    asqt_parm = asqt_set_dsp_init_parm();
    if(asqt_parm != NULL)
    {
        //如果ASQT使能，使用pc-tool的参数
        hfp_mic_cfg.input_gain = asqt_parm->mic_op_gain;
        hfp_mic_cfg.adc_gain  =   asqt_parm->mic_adc_gain;
    }
    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_LEVEL5, FREQ_LEVEL8);
    //加载HFP中间件驱动
    sys_load_mmm(mmm_name, TRUE);

    mr_ret = (int) mmm_hfp_cmd(&mp_handle, MMM_HFP_OPEN, 0);

    if (mr_ret == -1)
    {
        PRINT_ERR("hfp open fail!\n");
    }
    
    if(1 == g_app_info_state_all.fix_sample_rate_flag)
    {
        mmm_hfp_cmd(&mp_handle, MMM_HFP_FIX_SAMPLE_RATE, 0);
        hfp_dec_mode = 1;
    }

    if(g_stub_tools_type == STUB_PC_TOOL_ATT_MODE)
    {
        hfp_dec_mode = 2;
    }

    //must aout open first then ain open

    mr_ret = (int) mmm_hfp_cmd(mp_handle, MMM_HFP_AOUT_OPEN, (unsigned int) &hfp_dac_cfg);

    if (mr_ret == -1)
    {
        PRINT_ERR("hfp aoutopen fail!\n");
    }

    mr_ret = (int) mmm_hfp_cmd(mp_handle, MMM_HFP_AIN_OPEN, (unsigned int) &hfp_mic_cfg);

    if (mr_ret == -1)
    {
        PRINT_ERR("hfp ainopen fail!\n");
    }

    if (g_stub_tools_type != STUB_PC_TOOL_ASQT_MODE)
    {   
        //asqt固件模式下，加载完音效库后，从固件的asqtpara.bin文件读取PEQ参数
        asqt_para_bin_handle = get_asqt_peq_para_from_firmware();
    }
    else
    {
        //asqt调试模式下，加载完音效库后，从PC工具再强制读取下PEQ参数
        stub_asqt_read_data(STUB_CMD_ASQT_READ_PEQ_STATUS, 	&peq_status, sizeof(asqt_peq_status_t));
        get_asqt_peq_para_from_pc(&peq_status);
    }
    
    mr_ret = (int) mmm_hfp_cmd(mp_handle, MMM_HFP_PLAY, (unsigned int)&hfp_dec_mode);

    if (g_stub_tools_type != STUB_PC_TOOL_ASQT_MODE)
    {   
        sys_sd_fclose(asqt_para_bin_handle);
    }
	
	btcall_gl_var.dsp_load_flag = 1;
	
    if (mr_ret == -1)
    {
        PRINT_ERR("hfp play fail!\n");
    }
    else
    {
        PRINT_INFO("hfp play ok!\n");
    }

    btcall_gl_var.g_sco_deal_flag = 1;
    btcall_gl_var.need_exit_thread = FALSE;

}

/******************************************************************************
 * \  par  Description:   卸载中间件
 * \  param[in]     null0
 * \  param[out]   null
 * \  return           null
 * \  note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

void btcalleg_free_hfp_mmm(void)
{
    if (mp_handle != NULL)
    {
        mmm_hfp_cmd(mp_handle, MMM_HFP_STOP_PLAY, (unsigned int) NULL);
		
		btcall_gl_var.dsp_load_flag = 0;
		
        mmm_hfp_cmd(mp_handle, MMM_HFP_AIN_CLOSE, (unsigned int) NULL);

        mmm_hfp_cmd(mp_handle, MMM_HFP_AOUT_CLOSE, (unsigned int) NULL);

        //关闭中间件设备
        mmm_hfp_cmd(mp_handle, MMM_HFP_CLOSE, (unsigned int) NULL);

        PRINT_INFO("hfp close!\n");

        //卸载hfp驱动
        sys_free_mmm(TRUE);
        mp_handle = NULL;
    }

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);

    PRINT_INFO("hfp exit!\n");

    btcall_gl_var.g_sco_deal_flag = 0;
}

/******************************************************************************
 * \  par  Description:   创建soc链路
 * \  param[in]     null0
 * \  param[out]   null
 * \  return           null
 * \  note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

void bt_engine_hfp_sco_connect_ind_handle(void)
{
    if ((btcall_gl_var.g_sco_deal_flag == 1) || (btcall_gl_var.tts_play_flag == 1))
    {
        return;
    }

    if (btcall_gl_var.callin_ring_mode != CALLIN_RING_MODE_ONLY_PB)
    {
        if (g_callring_data.callring_timer_id != -1)
        {
            kill_app_timer(g_callring_data.callring_timer_id);
            g_callring_data.callring_timer_id = -1;
        }

        if (g_callring_data.callring_thread_exist == 1)
        {
            btcall_gl_var.need_exit_thread = TRUE;
            while (g_callring_data.callring_thread_exist != 0)
            {
                sys_os_time_dly(1);
            }
        }
    }
    btcalleg_load_hfp_mmm();
    /*有些手机创建了sco链路,但是controller没有发sco包,
     加个定时器判断是否有sco包,如果没有,主动断开sco连接*/
    //    set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL, 2000, (timer_proc) check_disconnect_sco_link);
}

/******************************************************************************
 * \  par  Description:  断开soc链路
 * \  param[in]     null0
 * \  param[out]   null
 * \  return           null
 * \  note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

void bt_engine_hfp_sco_disconnect_ind_handle(void)
{
    if (btcall_gl_var.g_sco_deal_flag != 0)
    {
        btcalleg_free_hfp_mmm();
    }
}

