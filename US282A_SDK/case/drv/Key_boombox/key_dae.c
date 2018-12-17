/*******************************************************************************
 *                              5116
 *                            Module: musicdec
 *                 Copyright(c) 2003-2021 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       fiona     2015-01-11 15:00     1.0             build this file
 *******************************************************************************/

//#include <audio_device.h>
//#include <analog.h>
#include <dae_info_def.h>
#include <dae_standard_mode_info_def.h>
#include <mmm_dae_def.h>
#include <psp_includes.h>
#include <kernel_interface.h>

// dsp address , and ((int) p_left_filter_param - 0x20000)* 2 + 0x9fc20000 for mips address
// dsp address , and ((int) p_right_filter_param - 0x20000)* 2 + 0x9fc20000 for mips address
typedef struct
{
    int max_filter_order;
    int *p_left_filter_param;  
    int *p_right_filter_param; 
} spk_comp_param_t;

#define SPK_COMP_PARAM_ADDR     (0x9fc341f4)

DAE_para_info_t __section__(".share_info_section") g_dae_param_info;

const uint8 spk_comp_file[] = "spk_comp.dat";


//等待DSP音效参数更新完成
static void waiting_dsp_dae_update_compelete(void)
{
   uint8 i = 0;
    
    //音效打开的时候，才会判断上一次DAE参数是否更新完，避免音效关闭的时候，也要延时等待。
   if (g_dae_param_info.bypass == 0)
   {
        //check if dsp has alredy read last update
        if (g_dae_param_info.DAE_change_flag != 0)
        {
            for (i = 0; i < 5; i++)
            {
                sys_os_time_dly(1);
                if (g_dae_param_info.DAE_change_flag == 0)
                {
                    break;
                }
            }
        }
    }
}

static void dae_param_init(void *param_ptr)
{
	init_play_param_t *play_para;
	
	//g_dae_param_info.DAE_init_flag = 0;
    //g_dae_param_info.fade_out_flag = 0;            
    //g_dae_param_info.DAE_change_flag = 0; 
    //g_dae_param_info.mute_flag = 0; 

    libc_memset(&g_dae_param_info, 0, sizeof(DAE_para_info_t));
       
    play_para = (init_play_param_t *) param_ptr;

    g_dae_param_info.channels = (short)play_para->chanel_num;

    if (play_para->sample_rate == 44000)
    {
        play_para->sample_rate = 44100;
    }

    g_dae_param_info.sample_rate = (int)play_para->sample_rate;

    g_dae_param_info.block_size = (short)play_para->block_size;

    g_dae_param_info.fade_handle = (uint32)play_para->fade_handle;

    g_dae_param_info.fade_in_time_ms = DEAULT_FADEIN_TIME;

    g_dae_param_info.fade_in_flag = 1;

    g_dae_param_info.output_channel_config = play_para->output_channel;

	PRINT_INFO_INT("output_channel_config", g_dae_param_info.output_channel_config);
		
    //g_dae_param_info.DAE_change_flag = 1;
}

static void set_bypass_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

	//bypass
    g_dae_param_info.bypass = pdae_cfg_para->bypass;
}



static void set_peq_band_setting(void *param_ptr)
{
    dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    uint8 i = 0;

    for (i = 0; i < MAX_PEQ_SEG; i++)
    {
       libc_memcpy(&g_dae_param_info.band_settings[i], &pdae_cfg_para->peq_bands[i], sizeof(peq_band_t));
    }
}

static void set_peq_enable_type(void *param_ptr)
{
    dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    uint8 i = 0;

    for (i = 0; i < MAX_PEQ_SEG; i++)
    {
       g_dae_param_info.band_enable_type[i] = pdae_cfg_para->peq_band_enable_type[i];
             //!!!注意PEQ是否使能，受gain的控制，高通除外
       if (pdae_cfg_para->peq_bands[i].gain == 0)
       {
           if (2 == g_dae_param_info.band_settings[i].type)
           {

           }
           else
           {
               g_dae_param_info.band_enable_type[i] = 0;            
           }
       }
    }
}

static void set_dew_band_high_pass_gain(void *param_ptr)
{
    dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    uint8 i = 0;
    
    for (i = 0; i < MAX_PEQ_SEG; i++)
    {
        if (pdae_cfg_para->dew_bands[DEW_BAND_HIGH_PASS].gain != 0)
        {
           if (g_dae_param_info.band_settings[i].type == 2)
          {
                if (pdae_cfg_para->dew_bands[DEW_BAND_HIGH_PASS].gain > 0)
               {
                    g_dae_param_info.band_settings[i].cutoff -= \
                        (short)pdae_cfg_para->dew_bands[DEW_BAND_HIGH_PASS].cutoff;
               }
               else
               {
                    g_dae_param_info.band_settings[i].cutoff += \
                        (short)pdae_cfg_para->dew_bands[DEW_BAND_HIGH_PASS].cutoff;
               }
          }
        }
    }
}


static void set_dew_band_low_freq_gain(void *param_ptr)
{
    dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    uint8 i = 0;
   for (i = 0; i < MAX_PEQ_SEG; i++)
   {
        if (pdae_cfg_para->dew_bands[DEW_BAND_LOW_FREQ].gain != 0)
        {
            if ((g_dae_param_info.band_settings[i].type == 1)
                    && (g_dae_param_info.band_settings[i].cutoff != 0)
                    && (g_dae_param_info.band_settings[i].cutoff == \
                    (pdae_cfg_para->dew_bands[DEW_BAND_LOW_FREQ].cutoff & 0x7fff)))
            {
                g_dae_param_info.band_settings[i].gain += pdae_cfg_para->dew_bands[DEW_BAND_LOW_FREQ].gain;
            }
        }
    }
}




static void set_dew_band_high_freq_gain(void *param_ptr)
{
    dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    uint8 i = 0;
    
    for (i = 0; i < MAX_PEQ_SEG; i++)
    {  
        if (pdae_cfg_para->dew_bands[DEW_BAND_HIGH_FREQ].gain != 0)
        {
            if ((g_dae_param_info.band_settings[i].type == 1)
                    && (g_dae_param_info.band_settings[i].cutoff != 0)
                    && (g_dae_param_info.band_settings[i].cutoff == \
                    (pdae_cfg_para->dew_bands[DEW_BAND_HIGH_FREQ].cutoff & 0x7fff)))
            {
                g_dae_param_info.band_settings[i].gain += pdae_cfg_para->dew_bands[DEW_BAND_HIGH_FREQ].gain;
            }
        }
    }
}


static void set_dew_band_peq_gain(void *param_ptr)
{
    dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    uint8 i = 0;
    uint8 j = DEW_BAND_HIGH_FREQ + 1;
    
    for (i = 0; i < MAX_PEQ_SEG; i++)
    {
        for (j = (DEW_BAND_HIGH_FREQ + 1); j < MAX_DEW_SEG; j++)
        {
            if (pdae_cfg_para->dew_bands[j].gain != 0)
            {
                if ((g_dae_param_info.band_settings[i].type == 1)
                        && (g_dae_param_info.band_settings[i].cutoff != 0)
                        && (g_dae_param_info.band_settings[i].cutoff == pdae_cfg_para->dew_bands[j].cutoff))
                {
                    g_dae_param_info.band_settings[i].gain += pdae_cfg_para->dew_bands[j].gain;
                }
            }
        }
    }
}

static void set_peq_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

    //PEQ
    if (pdae_cfg_para->peq_enable == 1)
    {
        set_peq_band_setting(param_ptr);
        set_peq_enable_type(param_ptr);
        set_dew_band_high_pass_gain(param_ptr);
        set_dew_band_low_freq_gain(param_ptr);
        set_dew_band_high_freq_gain(param_ptr);
        set_dew_band_peq_gain(param_ptr);

        //if last status is close we need update change flag
        if (g_dae_param_info.Peq_enable == 0)
        {
            g_dae_param_info.Peq_enable = 1;
        }
    }
    else
    {
        g_dae_param_info.Peq_enable = 0;
    }
	
    g_dae_param_info.Peq_change_flag = 1;
}

#if 0
static void set_speaker_compensation_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
	uint8 i = 0;
	
	//Speaker Compensation
    if (pdae_cfg_para->spk_compensation_enable == 1)
    {
        spk_comp_param_t *p_spk_comp_param;
        int *p_left, *p_right;

        p_spk_comp_param = (spk_comp_param_t *) SPK_COMP_PARAM_ADDR;
        p_left  = (int*)(((uint32) p_spk_comp_param->p_left_filter_param  - 0x20000)* 2 + 0x9fc20000);
        p_right = (int*)(((uint32) p_spk_comp_param->p_right_filter_param - 0x20000)* 2 + 0x9fc20000);

        g_dae_param_info.SpeakerCompensation_enable = 1;

        g_dae_param_info.FilterOrder = pdae_cfg_para->spk_comp_filter_order;
        g_dae_param_info.FilterQvalLeft = pdae_cfg_para->spk_comp_filter_QvalLeft;
        g_dae_param_info.FilterQvalRight = pdae_cfg_para->spk_comp_filter_QvalRight;

        if (pdae_cfg_para->spk_comp_source == 0)
        {
            //从SD区文件加载滤波器参数
            sd_file_t *file_fp;

            if (pdae_cfg_para->spk_comp_filter_order > p_spk_comp_param->max_filter_order)
            {
                //PRINTD_ERR("spk_comp filter order error!!");
                while(1)
                {
                    ; //nothing
                }
            }

            file_fp = sys_sd_fopen(spk_comp_file);
            if (file_fp == NULL)
            {
                //PRINTD_ERR("Can't open spk_comp.dat!!");
                while(1)
                {
                    ; //nothing
                }
            }

            if (sys_sd_fread((void*)file_fp, (void*)p_left, sizeof(int)*(pdae_cfg_para->spk_comp_filter_order))
                    < sizeof(int)*pdae_cfg_para->spk_comp_filter_order)
            {
                //PRINTD_ERR("spk_comp.dat length error!!");
                while(1)
                {
                    ; //nothing
                }
            }

            if (sys_sd_fread((void*)file_fp, (void*)p_right, sizeof(int)*(pdae_cfg_para->spk_comp_filter_order))
                    < sizeof(int)*pdae_cfg_para->spk_comp_filter_order)
            {
                //PRINTD_ERR("spk_comp.dat length error!!");
                while(1)
                {
                    ; //nothing
                }
            }

            sys_sd_fclose(file_fp);
        }
        else
        {
            //从VRAM加载滤波器参数
            for (i = 0; i < 4; i++)
            {
                sys_vm_read(p_left + 128 * i,  VM_SPK_COMP_DAT_BASE + 0x10000 * i, 128);
            }

            for (i = 0; i < 4; i++)
            {
                sys_vm_read(p_right + 128 * i, VM_SPK_COMP_DAT_BASE + 0x10000 * (4 + i), 128);
            }
        }

        g_dae_param_info.SpeakerCompensationChangeFlag = 1;
    }
    else
    {
        g_dae_param_info.SpeakerCompensation_enable = 0;
    }
}
#endif

static void set_vbass_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

	//vbass
    g_dae_param_info.Vbass_enable = pdae_cfg_para->vbass_enable;
    g_dae_param_info.Vbass_type = 1;
    
    if (pdae_cfg_para->vbass_enable == 1)
    {
        g_dae_param_info.Vbass_low_cut_off_frequency = (short)pdae_cfg_para->vbass_cut_freq;

        g_dae_param_info.Vbass_gain = pdae_cfg_para->vbass_ratio;

        if (((pdae_cfg_para->dew_bands[DEW_BAND_LOW_FREQ].cutoff & 0x8000) != 0)
                && (pdae_cfg_para->dew_bands[DEW_BAND_LOW_FREQ].gain != 0))
        {
            g_dae_param_info.Vbass_gain += (pdae_cfg_para->dew_bands[DEW_BAND_LOW_FREQ].gain);

            if (g_dae_param_info.Vbass_gain > 120)
            {
                g_dae_param_info.Vbass_gain = 120;
            }
            else if (g_dae_param_info.Vbass_gain < -120)
            {
                g_dae_param_info.Vbass_gain = -120;
            }
            else
            {
                //qac;
            }
        }
    }
}


static void set_surround_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

	//surround
    g_dae_param_info.Surround_enable = pdae_cfg_para->vsurround_enable;

    if (pdae_cfg_para->vsurround_enable == 1)
    {
        g_dae_param_info.Surround_angle = pdae_cfg_para->vsurround_angle;

        g_dae_param_info.Surround_gain = pdae_cfg_para->vsurround_ratio;
    }
}

static void set_treble_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

	//treble
    g_dae_param_info.TrebleEnhance_enable = pdae_cfg_para->treble_enhance_enable;

    if (pdae_cfg_para->treble_enhance_enable == 1)
    {
        g_dae_param_info.Treble_frequency = (short)pdae_cfg_para->treble_enhance_cut_freq;

        g_dae_param_info.Treble_gain = pdae_cfg_para->treble_enhance_ratio;

        if (((pdae_cfg_para->dew_bands[DEW_BAND_HIGH_FREQ].cutoff & 0x8000) != 0)
                && (pdae_cfg_para->dew_bands[DEW_BAND_HIGH_FREQ].gain != 0))
        {
            g_dae_param_info.Treble_gain += (pdae_cfg_para->dew_bands[DEW_BAND_HIGH_FREQ].gain);

            if (g_dae_param_info.Treble_gain > 150)
            {
                g_dae_param_info.Treble_gain = 150;
            }
            else if (g_dae_param_info.Treble_gain < 0)
            {
                g_dae_param_info.Treble_gain = 0;
            }
            else
            {
                //qac;
            }
        }
    }
}


static void set_energy_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

	//信号能量检测
	g_dae_param_info.energy_detect_enable = pdae_cfg_para->energy_detect_enable;
	
	if (g_dae_param_info.energy_detect_enable == 1)
	{
		g_dae_param_info.period = pdae_cfg_para->period;
		g_dae_param_info.period_count = pdae_cfg_para->period_count;
		g_dae_param_info.preadjust_db = pdae_cfg_para->preadjust_db;
		g_dae_param_info.preadjust_count = pdae_cfg_para->preadjust_count;
		g_dae_param_info.signal_energy_inner = pdae_cfg_para->signal_energy_init;
	}
}

static void set_multi_freq_band_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

#if (SUPPORT_MULTI_FREQ_MULTI_BAND_SETTING == 1)
	g_dae_param_info.MultiFreqBandEnergy_enable = pdae_cfg_para->MultiFreqBandEnergy_enable;
	if (g_dae_param_info.MultiFreqBandEnergy_enable == 1)
	{
		g_dae_param_info.duration_ms = pdae_cfg_para->dae_attributes->duration_ms;	 
		g_dae_param_info.num_band = pdae_cfg_para->dae_attributes->num_band;
		libc_memcpy(g_dae_param_info.f_c, pdae_cfg_para->dae_attributes->f_c, \
			sizeof(g_dae_param_info.f_c));			  
	}
#endif
}

static void set_multi_freq_point_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

#if (SUPPORT_MULTI_FREQ_MULTI_BAND_SETTING == 1)
	g_dae_param_info.FreqSpetrumDisplay_enable = pdae_cfg_para->FreqSpetrumDisplay_enable;
	if (g_dae_param_info.FreqSpetrumDisplay_enable == 1)
	{
		g_dae_param_info.num_freq_point = pdae_cfg_para->dae_attributes->num_freq_point;
		libc_memcpy(g_dae_param_info.freq_point, pdae_cfg_para->dae_attributes->freq_point, \
			sizeof(g_dae_param_info.freq_point));
	}
#endif
}

static void set_noise_reduction_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

	//噪音消除算法是否使能
    g_dae_param_info.DownwardExpander_enable= pdae_cfg_para->noise_reduction_enable;
}

static void set_smart_mode_precut_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
	//precut
	g_dae_param_info.precut = (short)(pdae_cfg_para->precut_ratio + pdae_cfg_para->equivalent_gain + \
		pdae_cfg_para->mdrc_precut_ratio);
	
	PRINT_INFO_INT("precut_smart_mode",g_dae_param_info.precut);
	PRINT_INFO_INT("equivalent_gain",pdae_cfg_para->equivalent_gain);
}

static void set_smart_mode_makeup_gain_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

    //makeup gain & post precut
    g_dae_param_info.makeup_gain = (int) (pdae_cfg_para->makeup_gain) + pdae_cfg_para->post_precut_ratio; 
}

static void set_smart_mode_mdrc_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
	uint8 i = 0;
	
    //mdrc
    g_dae_param_info.MultibandDRC_enable = pdae_cfg_para->mdrc_enable;

    if (pdae_cfg_para->mdrc_enable == 1)
    {
       for (i = 0; i < (MAX_MDRC_SEG-1); i++)
       {
           g_dae_param_info.crossover_freqency[i] = pdae_cfg_para->mdrc_crossover_freq[i];
       }

       for (i = 0; i < MAX_MDRC_SEG; i++)
       {
           libc_memcpy(&g_dae_param_info.mdrc_band_settings[i], &pdae_cfg_para->mdrc_bands[i],\
                   sizeof(mdrc_band_t));
       }
       
       g_dae_param_info.MDRC_compensation_peak_detect_attack_time = 50;
       g_dae_param_info.MDRC_compensation_peak_detect_release_time = 500;
       g_dae_param_info.MDRC_compensation_threshold = -100;
       g_dae_param_info.MDRC_compensation_filter_Q = 1;
    }
}

static void set_smart_mode_limiter_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;

    //mdrc
    g_dae_param_info.MultibandDRC_enable = pdae_cfg_para->mdrc_enable;

    //limiter release
    g_dae_param_info.Limiter_enable = pdae_cfg_para->limiter_enable;

    if (pdae_cfg_para->limiter_enable == 1)
    {
       g_dae_param_info.Limiter_threshold = pdae_cfg_para->limiter_threshold + \
        pdae_cfg_para->post_precut_ratio;

       g_dae_param_info.Limiter_attack_time = (short)pdae_cfg_para->limiter_attack_time;

       g_dae_param_info.Limiter_release_time = (short)pdae_cfg_para->limiter_release_time;
    }
}

static void set_standard_mode_precut_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    DAE_para_info_standard_mode_t* p_dae_param_info_standard_mode = \
    (DAE_para_info_standard_mode_t*)(&g_dae_param_info);
	
	//precut
	if (1 == pdae_cfg_para->bypass)
	{
	   p_dae_param_info_standard_mode->precut = 0; 
	}
	else
	{
	   p_dae_param_info_standard_mode->precut = pdae_cfg_para->precut_standard_mode;  
	   PRINT_INFO_INT("precut_standard_mode",p_dae_param_info_standard_mode->precut);
	}
}

static void set_standard_mode_makeup_gain_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    DAE_para_info_standard_mode_t* p_dae_param_info_standard_mode = \
    (DAE_para_info_standard_mode_t*)(&g_dae_param_info);
	
	//makeup_gain
	if (pdae_cfg_para->mdrc_enable_standard_mode == 1)
	{
	   p_dae_param_info_standard_mode->makeup_gain = \
		pdae_cfg_para->mdrc_extend_para_standard_mode.makeup_gain;
	   
	   p_dae_param_info_standard_mode->power_regulation = \
		pdae_cfg_para->mdrc_extend_para_standard_mode.signal_adjust;			
	}
	else
	{
	   p_dae_param_info_standard_mode->makeup_gain = 0;
	   p_dae_param_info_standard_mode->power_regulation = 0;
	}
}

static void set_standard_mode_mdrc_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    DAE_para_info_standard_mode_t* p_dae_param_info_standard_mode = \
    (DAE_para_info_standard_mode_t*)(&g_dae_param_info);
	uint8 i = 0;
	
	//mdrc
	p_dae_param_info_standard_mode->MultibandDRC_enable = pdae_cfg_para->mdrc_enable_standard_mode;
	
	if (pdae_cfg_para->mdrc_enable_standard_mode == 1)
	{  
	   p_dae_param_info_standard_mode->crossover_freqency[0] = \
		pdae_cfg_para->mdrc_extend_para_standard_mode.mdrc_crossover_freq0;
	   
	   p_dae_param_info_standard_mode->crossover_freqency[1] = \
		pdae_cfg_para->mdrc_extend_para_standard_mode.mdrc_crossover_freq1; 
	
	   for (i = 0; i < MDRC_NUM_BANDS_STANDARD_MODE; i++)
	   {
		   libc_memcpy(&p_dae_param_info_standard_mode->mdrc_band_settings[i], \
			&pdae_cfg_para->p_mdrc_band_standard_mode[i],sizeof(dae_mdrc_band_standard_mode_t));
	   }
	
	   p_dae_param_info_standard_mode->MDRC_compensation_peak_detect_attack_time = \
		pdae_cfg_para->mdrc_peak_standard_mode.MDRC_compensation_peak_detect_attack_time;
	   
	   p_dae_param_info_standard_mode->MDRC_compensation_peak_detect_release_time = \
		pdae_cfg_para->mdrc_peak_standard_mode.MDRC_compensation_peak_detect_release_time;
	   p_dae_param_info_standard_mode->MDRC_compensation_threshold = \
		pdae_cfg_para->mdrc_peak_standard_mode.MDRC_compensation_threshold;
	   
	   p_dae_param_info_standard_mode->MDRC_compensation_filter_Q = \
		pdae_cfg_para->mdrc_peak_standard_mode.MDRC_compensation_filter_Q;			
	}
}

static void set_standard_mode_compressor_param(void *param_ptr)
{
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
    DAE_para_info_standard_mode_t* p_dae_param_info_standard_mode = \
    (DAE_para_info_standard_mode_t*)(&g_dae_param_info);
	
	//compressor
	  p_dae_param_info_standard_mode->Compressor_enable = pdae_cfg_para->compressor_enable_standard_mode;
	
	  if (pdae_cfg_para->compressor_enable_standard_mode == 1)
	  {
		 p_dae_param_info_standard_mode->Compressor_threshold1	 = \
		  pdae_cfg_para->compressor_standard_mode.threshold1;
		 
		 p_dae_param_info_standard_mode->Compressor_threshold2	 = \
		  pdae_cfg_para->compressor_standard_mode.threshold2;
		 
		 p_dae_param_info_standard_mode->Compressor_tav 		 = \
		  pdae_cfg_para->compressor_standard_mode.tav;
		 
		 p_dae_param_info_standard_mode->Compressor_attack_time  = \
		  pdae_cfg_para->compressor_standard_mode.attack_time;
		 
		 p_dae_param_info_standard_mode->Compressor_release_time = \
		  pdae_cfg_para->compressor_standard_mode.release_time;
		 
		 p_dae_param_info_standard_mode->Compressor_ratio1		 = \
		  pdae_cfg_para->compressor_standard_mode.ratio1;
		 
		 p_dae_param_info_standard_mode->Compressor_ratio2		 = \
		  pdae_cfg_para->compressor_standard_mode.ratio2;
	  }
}

//打印出所有的DAE参数信息
static void dae_param_print(void *param_ptr)
{ 
	dae_config_t *pdae_cfg_para;
	pdae_cfg_para = (dae_config_t *) param_ptr;
	
	if (1 == pdae_cfg_para->dae_print_enable)
    {
        dae_param_print(pdae_cfg_para->audiopp_type);
        dae_param_print1(pdae_cfg_para->audiopp_type);
    }
    //PRINT_DATA(&g_dae_param_info,sizeof(DAE_para_info_t));
    //PRINT_DATA(&g_dae_param_info,sizeof(DAE_para_info_standard_mode_t));
}



/******************************************************************************/
/*!
 * \par  Description:
 * \int32 audio_device_set_effect_param(uint32 set_type, void* param_ptr, void* null3)
 * \设置音效相关参数
 * \param[in]    set_type  para1 命令类型,见set_effect_type_e定义
 * \param[in]    param_ptr para2 参数指针(根据命令类型需要转换指针类型)
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       0 sucesse
 * \retval       -1 fail
 * \ingroup
 * \note
 */
/*******************************************************************************/
int32 key_inner_set_effect_param(uint32 set_type, void *param_ptr, void *null3)
{

    int32 ret = 0;

    dae_config_t *pdae_cfg_para;

    //init_play_param_t *play_para;

    uint32 *single_para;

    dae_noise_reduction_t* p_dae_noise_reduction;

    do
    {
        //param is not ok
        if (param_ptr == NULL)
        {
            ret = -1;
            break;
        }

        if (set_type == SET_PARAM_INIT)
        {
			dae_param_init(param_ptr);
            break;
        }
        
        waiting_dsp_dae_update_compelete();
        
        switch (set_type)
        {

            case SET_EFFECT_PARAM:  
                
            pdae_cfg_para = (dae_config_t *) param_ptr;

			set_bypass_param(param_ptr);

			set_peq_param(param_ptr);

			//set_speaker_compensation_param(param_ptr);

            set_vbass_param(param_ptr);

            set_surround_param(param_ptr);

			set_treble_param(param_ptr);
		
 			set_energy_param(param_ptr);

			set_multi_freq_band_param(param_ptr);

			set_multi_freq_point_param(param_ptr);

			set_noise_reduction_param(param_ptr);
            
            if (SMART_MODE == pdae_cfg_para->audiopp_type)
            {
				set_smart_mode_precut_param(param_ptr);
				
				set_smart_mode_makeup_gain_param(param_ptr);
				
				set_smart_mode_mdrc_param(param_ptr);

        		set_smart_mode_limiter_param(param_ptr);
            }
            else 
            {     
				set_standard_mode_precut_param(param_ptr);
				
    			set_standard_mode_makeup_gain_param(param_ptr);

				set_standard_mode_mdrc_param(param_ptr);

				set_standard_mode_compressor_param(param_ptr);
            }
			
			dae_param_print(param_ptr);
			
            break;

            case SET_SAMPLE_INFO:
            single_para = (uint32 *) param_ptr;

            if (*single_para == 44000)
            {
                *single_para = 44100;
            }

            g_dae_param_info.sample_rate = (int)(*single_para);

            break;

            case SET_FADE_OUT:

            single_para = (uint32 *) param_ptr;

            g_dae_param_info.fade_in_time_ms = 0;

            g_dae_param_info.fade_out_time_ms = (short)(*single_para);

            g_dae_param_info.fade_in_flag = 0;

            g_dae_param_info.fade_out_flag = 1;
            //change flag
            break;
            case SET_FADE_IN:

            single_para = (uint32 *) param_ptr;

            g_dae_param_info.fade_in_time_ms = (short)(*single_para);

            g_dae_param_info.fade_out_time_ms = 0;

            g_dae_param_info.fade_in_flag = 1;

            g_dae_param_info.fade_out_flag = 0;
            break;
            
            case SET_NOISE_REDUCTION_PARA:
            p_dae_noise_reduction = (dae_noise_reduction_t*)param_ptr;

            g_dae_param_info.DownwardExpander_enable = p_dae_noise_reduction->enable;
            g_dae_param_info.DownExp_attack_time     = p_dae_noise_reduction->DownExp_attack_time;
            g_dae_param_info.DownExp_release_time    = p_dae_noise_reduction->DownExp_release_time;
            g_dae_param_info.DownExp_set0_threshold  = p_dae_noise_reduction->DownExp_set0_threshold;
            g_dae_param_info.DownExp_threshold       = p_dae_noise_reduction->DownExp_threshold;
            g_dae_param_info.DownExp_ratio           = p_dae_noise_reduction->DownExp_ratio;
            
            break;
            
            default:
            break;
        }
        //set param update flag
        if (SET_NOISE_REDUCTION_PARA != set_type)
        {
            g_dae_param_info.DAE_change_flag = 1;
        }

    } while (0);

    return ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \int32 audio_device_get_feature_info(uint32 get_type, void* info_ptr, void* null3)
 * \获取音频特征值
 * \param[in]    get_type  para1 命令类型
 * \param[in]    info_ptr para2 存放信息数据的指针
 * \param[in]
 * \param[out]   none
 * \return       int the result
 * \retval       0 sucesse
 * \retval       -1 fail
 * \ingroup
 * \note
 */
/*******************************************************************************/
int32 key_inner_get_feature_info(uint32 set_type, void *info_ptr, void *null3)
{

    //get fade out status
    if (set_type == GET_FADE_OUT_STATUS)
    {
        uint32 *get_val;

        get_val = (uint32 *) info_ptr;

        *get_val = g_dae_param_info.fade_out_flag;
    }
    else if (set_type == GET_FADE_PARAM_ADDR)
    {
        dae_fade_param_addr_t *p_dae_fade_param_addr = (dae_fade_param_addr_t *) info_ptr;

        p_dae_fade_param_addr->p_fade_in_time_ms = &(g_dae_param_info.fade_in_time_ms);
        p_dae_fade_param_addr->p_fade_out_time_ms = &(g_dae_param_info.fade_out_time_ms);
        p_dae_fade_param_addr->p_fade_in_flag = &(g_dae_param_info.fade_in_flag);
        p_dae_fade_param_addr->p_fade_out_flag = &(g_dae_param_info.fade_out_flag);
        p_dae_fade_param_addr->p_DAE_change_flag = &(g_dae_param_info.DAE_change_flag);
    }
    else if (set_type == GET_ENERGY_VALUE_ADDR)
    {
        short ** p_energy_value_addr = (short **) info_ptr;
        *p_energy_value_addr = &(g_dae_param_info.energy);
        
#if (SUPPORT_MULTI_FREQ_MULTI_BAND_SETTING == 1)        
        *((short **)((uint8 *)p_energy_value_addr+4)) = g_dae_param_info.energys;
        *((short **)((uint8 *)p_energy_value_addr+8)) = g_dae_param_info.freq_point_mag;
#endif  

    }
    else
    {
        ; //nothing
    }

    return 0;
}
