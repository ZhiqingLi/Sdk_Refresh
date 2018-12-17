/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE DAE设置管理模块，包括加载默认值，初始化和设置接口；通过共享内存进行
 *       间接设置。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "music_common.h"
#include "common_func.h"
#include "stub_command.h"
#include "../aset_common/common_aset_new_func.h"

extern void com_inner_load_mdrc_param(dae_config_t *p_dae_cfg, bool aux_flag) __FAR__;

bool  __set_dae_config(dae_config_t *p_dae_cfg);
uint8 __section__(".BANK47")__calc_dae_freq(dae_config_t *p_dae_cfg);
static void  __update_precut_factor(dae_config_t *p_dae_cfg);
static void  __update_dew_setting(dae_config_t *p_dae_cfg, int16 volume_relative);
static void  __update_mdrc_setting(dae_config_t *p_dae_cfg, int16 volume_relative, \
                int16 signal_energy, int16 vol_adjust_gain_max);
static void  __set_signal_variable_mode(bool variable_mode);
static bool  __load_update_set_dae_config(dae_config_t *p_dae_cfg, bool need_load);
uint32 __section__(".BANK47") update_dae_vol_table(void);


//根据MDRC开关，AUX通道属性，可变信号通道属性等选择DAE配置参数
bool com_load_mdrc_config(dae_config_t *p_dae_cfg, bool load_bin, bool update_signal_detect)
{
    uint8 mdrc_enable = (p_dae_cfg->mdrc_enable == 1) || (p_dae_cfg->mdrc_enable_standard_mode == 1);
         
        
    sys_comval->dae_cfg_variable = (p_dae_cfg->enable == 1) && (p_dae_cfg->bypass == 0)
            && ((mdrc_enable) || (sys_comval->dae_enhanced_enable == 1) \
            || (sys_comval->dae_weaken_enable == 1));

    if (p_dae_cfg->enable == 0)
    {
        return TRUE;
    }

    if (p_dae_cfg->bypass == TRUE)
    {
        p_dae_cfg->equivalent_gain = 0;
        p_dae_cfg->precut_ratio = 0;
        p_dae_cfg->mdrc_precut_ratio = 0;

        return TRUE;
    }

    if (load_bin == TRUE)
    {
        if (p_dae_cfg->mdrc_enable == 1)
        {
            p_dae_cfg->equivalent_gain = (uint16) com_get_config_default(DAE_EQUIVALENT_GAIN);
            p_dae_cfg->precut_ratio = 0;
            p_dae_cfg->mdrc_precut_ratio = 0;
        }
        else
        {
            p_dae_cfg->equivalent_gain = 0;
            p_dae_cfg->precut_ratio = (uint16) com_get_config_default(DAE_PRECUT_RATIO);
            p_dae_cfg->mdrc_precut_ratio = 0;
            p_dae_cfg->post_precut_ratio = 0;
        }
    }
    p_dae_cfg->makeup_gain = 0;

    if (load_bin == TRUE)
    {
        com_inner_load_mdrc_param(p_dae_cfg, sys_comval->aux_flag);
    }

    if (update_signal_detect == TRUE)
    {
        __set_signal_variable_mode(sys_comval->signal_variable_mode);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    通过apk/app设置DAE参数。
 * \param[in]    daemode_param DAE设置参数
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 设置EQ成功
 * \retval           FALSE 设置EQ失败
 * \note
*******************************************************************************/
bool __section__(".bank") com_set_dae_config(daemode_param_t *daemode_param)
{
    dae_config_t *p_dae_cfg = &(sys_comval->dae_cfg);
    bool bret = TRUE;
    uint8 dae_option;

    if (daemode_param == NULL)
    {
        return FALSE;
    }

    if (daemode_param->dae_mode == DAE_MODE_NONE)
    {
        com_set_dae_onoff(FALSE);
    }
    else
    {
        com_set_dae_onoff(TRUE);

        dae_option = daemode_param->param.dae_option;
   
        sys_comval->set_dae_by_app_flag = 1;
        
        //VBASS
        if ((dae_option & (1 << DAE_OPTION_VBASS)) != 0)
        {
            p_dae_cfg->vbass_enable = 1;
        }
        else
        {
            p_dae_cfg->vbass_enable = 0;
        }
        //TREBLE
        if ((dae_option & (1 << DAE_OPTION_TREBLE)) != 0)
        {
            p_dae_cfg->treble_enhance_enable = 1;
        }
        else
        {
            p_dae_cfg->treble_enhance_enable = 0;
        }
        //VSURROUND
        if ((dae_option & (1 << DAE_OPTION_VSURROUND)) != 0)
        {
            p_dae_cfg->vsurround_enable = 1;
        }
        else
        {
            p_dae_cfg->vsurround_enable = 0;
        }
        //SFR
        if ((dae_option & (1 << DAE_OPTION_SFR)) != 0)
        {
            p_dae_cfg->spk_compensation_enable = 1;
        }
        else
        {
            p_dae_cfg->spk_compensation_enable = 0;
        }
        //PEQ
        if ((dae_option & (1 << DAE_OPTION_PEQ)) != 0)
        {
            p_dae_cfg->peq_enable = 1;
        }
        else
        {
            p_dae_cfg->peq_enable = 0;
        }
        //MDRC
        if ((dae_option & (1 << DAE_OPTION_MDRC)) != 0)
        {
            p_dae_cfg->mdrc_enable = 1;
        }
        else
        {
            p_dae_cfg->mdrc_enable = 0;
        }
        //DAE Enhancement
        if ((dae_option & (1 << DAE_OPTION_DAE_ENH)) != 0)
        {
            sys_comval->dae_enhanced_enable = 1;
        }
        else
        {
            sys_comval->dae_enhanced_enable = 0;
        }
        //DAE Weaken
        if ((dae_option & (1 << DAE_OPTION_DAE_WKN)) != 0)
        {
            sys_comval->dae_weaken_enable = 1;
        }
        else
        {
            sys_comval->dae_weaken_enable = 0;
        }
    }

    bret = __load_update_set_dae_config(p_dae_cfg, TRUE);

    return bret;
}

uint8 get_dae_bypass_status(void)
{
    return sys_comval->dae_cfg.bypass;
}

//动态开关DAE模块，用于带耳机输出的音箱方案；该接口仅仅修改sys_comval的参数，仍然没有其作用
bool __section__(".bank") com_set_dae_onoff(bool onoff)
{
    dae_config_t *p_dae_cfg = &(sys_comval->dae_cfg);
    bool bret = TRUE;

    if (FALSE == onoff) 
    {     
        //关闭数字音效
        if (TRUE == p_dae_cfg->bypass)
        {
            //不直接返回，需要重新计算所需要的DSP频率
            //return bret;                    //数字音效已经关闭直接返回
        }
        else
        {
            p_dae_cfg->bypass = TRUE;       //数字音效没关闭，将数字音效关闭
        }
    }
    else  
    {
         //打开数字音效
        if (FALSE == p_dae_cfg->bypass)
        {
            //不直接返回，需要重新计算所需要的DSP频率
            //return bret;                    //数字音效已经打开直接返回
        }
        else
        {
            p_dae_cfg->bypass = FALSE;      //数字音效没打开，将数字音效打开
        }     
    }

    bret = __load_update_set_dae_config(p_dae_cfg, TRUE);

    return bret;
}

//使能/禁止DAE，如果禁止就加载空DAE库以释放DAE库空间
bool __section__(".bank") com_set_dae_enable(bool enable)
{
    dae_config_t *p_dae_cfg = &(sys_comval->dae_cfg);
    bool bret = TRUE;

    p_dae_cfg->enable = enable;

    bret = __load_update_set_dae_config(p_dae_cfg, TRUE);

    return bret;
}

//设置DAE通道属性，分为AUX通道属性和信号可变通道属性：linein和FM为AUX，其它应用为非AUX
bool __section__(".bank") com_set_dae_chan(bool aux_flag, bool variable_mode)
{
    dae_config_t *p_dae_cfg = &(sys_comval->dae_cfg);
    bool bret = TRUE;

    sys_comval->aux_flag = aux_flag;
    sys_comval->signal_variable_mode = variable_mode;

    bret = __load_update_set_dae_config(p_dae_cfg, TRUE);

    return bret;
}

//动态更新DAE配置参数
bool com_set_dae_config_dynamic(void)
{
    bool bret = TRUE;

    bret = __load_update_set_dae_config(&(sys_comval->dae_cfg), FALSE);

    return bret;
}

//动态更新DAE配置参数
void com_update_dae_config(dae_config_t *p_dae_cfg)
{
    int16 volume_relative = sys_comval->volume_relative;
    int16 signal_energy = sys_comval->signal_energy_used;
    int16 vol_adjust_gain_max;
    uint8 i;

    if ((p_dae_cfg->enable == FALSE) || (p_dae_cfg->bypass == TRUE))
    {
        goto update_last;
    }

    //PRINT_INFO_INT("volume_relative",volume_relative);
    PRINT_INFO_INT("signal_energy",signal_energy);
    
    //更新音效增强减弱设置
    __update_dew_setting(p_dae_cfg, volume_relative + signal_energy);

    vol_adjust_gain_max = 0;
    for (i = 1; i < MAX_DEW_SEG; i++) //跳过高通滤波
    {
//        if (p_dae_cfg->dew_bands[i].gain != 0)
//        {
//            PRINT_DBG_INT("cutoff:", p_dae_cfg->dew_bands[i].cutoff);
//            PRINT_DBG_INT("gain:", p_dae_cfg->dew_bands[i].gain);
//        }

        if (p_dae_cfg->dew_bands[i].gain > vol_adjust_gain_max)
        {
            vol_adjust_gain_max = p_dae_cfg->dew_bands[i].gain;
        }
    }

    //PRINT_DBG_INT("volume_relative:", volume_relative);
    //PRINT_DBG_INT("signal_energy:", signal_energy);
    //PRINT_DBG_INT("vol_adjust_gain_max:", vol_adjust_gain_max);

    vol_adjust_gain_max = vol_adjust_gain_max * (int16) (sys_comval->dae_enhanced_peak_ratio) / 10;

    //更新MDRC相关参数
    if (p_dae_cfg->mdrc_enable == 1)
    {
        __update_mdrc_setting(p_dae_cfg, volume_relative, signal_energy, vol_adjust_gain_max);
    }
  
    if (STANDARD_MODE == p_dae_cfg->audiopp_type)
    {
        if (STUB_PC_TOOL_ASET_EQ_MODE == g_app_info_state.stub_tools_type)
        {
            //标准模式下，连接ASET工具时，直接将ASET参数传递给DSP
        }
        else
        {
            //标准模式下，打包成固件时，根据不同音量级别动态更新预衰减 压缩器阈值
            //信号微调 音量微调 增益补偿 补偿滤波器阈值  三段MDRC阈值
            update_dae_vol_table();  
        }  
    }
  
    update_last:

    __update_precut_factor(p_dae_cfg);
    __calc_dae_freq(p_dae_cfg);
}
#if 0
void __section__(".rcode")multi_band_multi_freq_test(void)
{
    short *p_signal_energy_inner_band;//多频段起始地址
    short *p_signal_energy_inner_freq;//多频点起始地址
    uint8 i = 0;
    
    p_signal_energy_inner_band = g_app_info_state_all.p_energy_value_bands;
    p_signal_energy_inner_freq = g_app_info_state_all.p_energy_value_freqs;
    //PRINT_INFO_INT("ad0",p_signal_energy_inner_band);
    //PRINT_INFO_INT("ad1",p_signal_energy_inner_freq);
    for (i=0; i<10;i++)
    {
       PRINT_INFO_INT("freq ",i); 
       PRINT_INFO_INT("f",*(p_signal_energy_inner_freq + i)); 
       PRINT_INFO_INT("b",*(p_signal_energy_inner_band + i));
    }
    
    PRINT_INFO("\n");
}
#endif
//实时能量检测
void __section__(".rcode") com_detect_energy_realtime(void)
{
#if (SUPPORT_DETECT_ENERGY == 1)
    dae_config_t *p_dae_cfg = &(sys_comval->dae_cfg);
    short *p_signal_energy_inner;
    short tmp_signal_energy_inner;
    uint8 vbass_enable;

    p_signal_energy_inner = g_app_info_state_all.p_energy_value - 1;
    tmp_signal_energy_inner = *p_signal_energy_inner;
    
    vbass_enable = p_dae_cfg->vbass_enable;

    sys_comval->signal_energy = tmp_signal_energy_inner;
    
	if (STUB_PC_TOOL_ASET_EQ_MODE != g_app_info_state.stub_tools_type)
	{
		//非调音模式下，规避小信号大音量的时候，虚拟低音打开会产生电流声
	    if(sys_comval->signal_energy<-300)
	    {
	        p_dae_cfg->vbass_enable = 0;        
	    }
	    else
	    {
	        //大信号的时候，通过APP设置过音效，就不做处理;否则恢复默认状态
	        if (sys_comval->set_dae_by_app_flag == 1)
	        {
	            ;//for qac
	        }
	        else
	        {
	            p_dae_cfg->vbass_enable = sys_comval->default_vbass_enable;
	        }
	    }
	}
 
    if (sys_comval->signal_energy < sys_comval->signal_energy_min)
    {
        sys_comval->signal_energy = sys_comval->signal_energy_min;
    }
    else 
    {
        ;//do noting for qac
    }
	
    if (sys_comval->signal_energy_used != sys_comval->signal_energy)
    {
        sys_comval->signal_energy_used = sys_comval->signal_energy;
#ifdef SUPPORT_ASET_TEST
   g_aset_sync_volume = 1;
#endif
        //com_set_dae_config_dynamic();
		com_set_phy_volume(sys_comval->volume_current);
#ifdef SUPPORT_ASET_TEST
   g_aset_sync_volume = 0;
#endif

    }

    p_dae_cfg->vbass_enable = vbass_enable;
#endif
}


//更新PRECUT系数
static void __update_precut_factor(dae_config_t *p_dae_cfg)
{
    int16 precut_ratio = 0;

    if (p_dae_cfg->mdrc_enable == 1)
    {
        int16 limit_threshold = p_dae_cfg->mdrc_threshold_base_max;

        if (p_dae_cfg->limiter_enable == 1)
        {
            limit_threshold += ((int16) p_dae_cfg->limiter_threshold_diff - p_dae_cfg->limiter_threshold);
        }

        precut_ratio = (int16) 0 - limit_threshold;
        precut_ratio -= (int16) p_dae_cfg->makeup_gain;
        precut_ratio += p_dae_cfg->precut_ratio;
    }

    sys_comval->mdrc_percent = (uint8) com_math_exp_fixed(0 - precut_ratio);
}

//加载mdrc参数
static __section__(".bank") void __load_mdrc_param(dae_config_t *p_dae_cfg, bool aux_flag)
{
    uint16 config_id;
    uint8 i;

    if (p_dae_cfg->mdrc_enable == 0)
    {
        return ;
    }

    sys_comval->dae_mdrc_peak_gain  = (uint8) com_get_config_default(DAE_PRECUT_RATIO);
    if (aux_flag == TRUE)
    {
        config_id = DAE_MDRC_AUX_BANK0_SETTING;
        p_dae_cfg->limiter_threshold_diff = (int16) com_get_config_default(DAE_MDRC_AUX_LIMITER_TH_DIFF);
        sys_comval->mdrc_vol_adjust = (int8) (int16) com_get_config_default(DAE_MDRC_AUX_VOLUME_ADJUST);
        p_dae_cfg->post_precut_ratio = (int8) (int16) com_get_config_default(DAE_MDRC_AUX_SIGNAL_ADJUST);
    }
    else
    {
        config_id = DAE_MDRC_BANK0_SETTING;
        p_dae_cfg->limiter_threshold_diff = (int16) com_get_config_default(DAE_MDRC_LIMITER_TH_DIFF);
        sys_comval->mdrc_vol_adjust = (int8) (int16) com_get_config_default(DAE_MDRC_VOLUME_ADJUST);
        p_dae_cfg->post_precut_ratio = (int8) (int16) com_get_config_default(DAE_MDRC_SIGNAL_ADJUST);
    }

    p_dae_cfg->mdrc_threshold_base_max = -600;
    for (i = 0; i < MAX_MDRC_SEG; i++)
    {
        mdrc_config_t tmp_mdrc_config;

        com_get_config_struct((uint16) (config_id + i), (uint8 *) &tmp_mdrc_config, \
                sizeof(tmp_mdrc_config));

        p_dae_cfg->mdrc_bands[i].threshold = tmp_mdrc_config.mdrc_data.threshold;
        p_dae_cfg->mdrc_bands[i].attack_time = tmp_mdrc_config.mdrc_data.attack_time;
        p_dae_cfg->mdrc_bands[i].release_time = tmp_mdrc_config.mdrc_data.release_time;
        p_dae_cfg->mdrc_bands[i].reserve = 0;

        if ((p_dae_cfg->limiter_enable == 1)
                && (p_dae_cfg->mdrc_bands[i].threshold > (0 - p_dae_cfg->limiter_threshold_diff)))
        {
            p_dae_cfg->mdrc_bands[i].threshold = (0 - p_dae_cfg->limiter_threshold_diff);
        }

        p_dae_cfg->mdrc_threshold_base[i] = p_dae_cfg->mdrc_bands[i].threshold;
        if (p_dae_cfg->mdrc_threshold_base[i] > p_dae_cfg->mdrc_threshold_base_max)
        {
            p_dae_cfg->mdrc_threshold_base_max = p_dae_cfg->mdrc_threshold_base[i];
        }

        if (i < (MAX_MDRC_SEG-1))
        {
            p_dae_cfg->mdrc_crossover_freq[i] = (uint16) tmp_mdrc_config.mdrc_data.crossover_freq;
        }
    }
}

//更新音效增强减弱设置
static void __update_dew_setting(dae_config_t *p_dae_cfg, int16 volume_relative)
{
    dew_config_t tmp_dew_cfg;
    dew_band_t *p_dew_band;
    dew_band_t *p_dew_band_enh, *p_dew_band_wk;
    int16 vol_adjust_gain;
    uint8 i;

    p_dew_band = sys_malloc(sizeof(dew_band_t)*MAX_DEW_SEG);
    if (p_dew_band == NULL)
    {
        PRINT_ERR("dew bands malloc fail!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }

    libc_memset(p_dae_cfg->dew_bands, 0x00, sizeof(p_dae_cfg->dew_bands));

    //动态增强效果处理
    if (sys_comval->dae_enhanced_enable == 1)
    {
        sys_vm_read(p_dew_band, VM_DEW_PARAM_BASE, sizeof(dew_band_t)*MAX_DEW_SEG);

        for (i = 0; i < MAX_DEW_SEG; i++)
        {
            p_dew_band_enh = p_dew_band + i;

            vol_adjust_gain = 0;
            if ((p_dew_band_enh->gain != 0) && (p_dew_band_enh->rel_vol_start > p_dew_band_enh->rel_vol_keep))
            {
                if (i == DEW_BAND_HIGH_PASS)
                {
                    if ((sys_comval->highpass_cutoff != 0) && (sys_comval->highpass_cutoff >= p_dew_band_enh->cutoff))
                    {
                        if (volume_relative < p_dew_band_enh->rel_vol_keep)
                        {
                            p_dae_cfg->dew_bands[i].cutoff = sys_comval->highpass_cutoff - p_dew_band_enh->cutoff;
                            p_dae_cfg->dew_bands[i].gain = 1;
                        }
                        else if (volume_relative < p_dew_band_enh->rel_vol_start)
                        {
                            p_dae_cfg->dew_bands[i].cutoff = (sys_comval->highpass_cutoff - p_dew_band_enh->cutoff)
                                    * (p_dew_band_enh->rel_vol_start - volume_relative)
                                    / (p_dew_band_enh->rel_vol_start - p_dew_band_enh->rel_vol_keep);
                            p_dae_cfg->dew_bands[i].gain = 1;
                        }
                        else
                        {
                            ;//for qac
                        }
                    }
                }
                else
                {
                    if (volume_relative < p_dew_band_enh->rel_vol_keep)
                    {
                        vol_adjust_gain = p_dew_band_enh->gain;
                    }
                    else if (volume_relative < p_dew_band_enh->rel_vol_start)
                    {
                        vol_adjust_gain = p_dew_band_enh->gain
                                * (p_dew_band_enh->rel_vol_start - volume_relative)
                                / (p_dew_band_enh->rel_vol_start - p_dew_band_enh->rel_vol_keep);
                    }
                    else
                    {
                        ;//for qac
                    }

                    if (vol_adjust_gain != 0)
                    {
                        p_dae_cfg->dew_bands[i].cutoff = p_dew_band_enh->cutoff;
                        if (((i == DEW_BAND_LOW_FREQ) || (i == DEW_BAND_HIGH_FREQ))
                                && (p_dew_band_enh->reserve_param == 1))
                        {
                            p_dae_cfg->dew_bands[i].cutoff |= (uint16) 0x8000;
                        }
                        p_dae_cfg->dew_bands[i].gain = vol_adjust_gain;
                    }
                }
            }
        }
    }

    //动态减弱效果处理
    if (sys_comval->dae_weaken_enable == 1)
    {
        sys_vm_read(p_dew_band, VM_DEW_PARAM_BASE + 0x10000, sizeof(dew_band_t)*MAX_DEW_SEG);

        for (i = 0; i < MAX_DEW_SEG; i++)
        {
            if (p_dae_cfg->dew_bands[i].gain != 0)
            {
                break;
            }

            p_dew_band_wk = p_dew_band + i;

            vol_adjust_gain = 0;
            if ((p_dew_band_wk->gain != 0) && (p_dew_band_wk->rel_vol_keep > p_dew_band_wk->rel_vol_start))
            {
                if (i == DEW_BAND_HIGH_PASS)
                {
                    if ((sys_comval->highpass_cutoff != 0) && (p_dew_band_wk->cutoff >= sys_comval->highpass_cutoff))
                    {
                        if (volume_relative > p_dew_band_wk->rel_vol_keep)
                        {
                            p_dae_cfg->dew_bands[i].cutoff = p_dew_band_wk->cutoff - sys_comval->highpass_cutoff;
                            p_dae_cfg->dew_bands[i].gain = -1;
                        }
                        else if (volume_relative > p_dew_band_wk->rel_vol_start)
                        {
                            p_dae_cfg->dew_bands[i].cutoff = (p_dew_band_wk->cutoff - sys_comval->highpass_cutoff)
                                    * (volume_relative - p_dew_band_wk->rel_vol_start)
                                    / (p_dew_band_wk->rel_vol_keep - p_dew_band_wk->rel_vol_start);
                            p_dae_cfg->dew_bands[i].gain = -1;
                        }
                        else
                        {
                            ;//for qac
                        }
                    }
                }
                else
                {
                    if (volume_relative > p_dew_band_wk->rel_vol_keep)
                    {
                        vol_adjust_gain = p_dew_band_wk->gain;
                    }
                    else if (volume_relative > p_dew_band_wk->rel_vol_start)
                    {
                        vol_adjust_gain = p_dew_band_wk->gain
                                * (volume_relative - p_dew_band_wk->rel_vol_start)
                                / (p_dew_band_wk->rel_vol_keep - p_dew_band_wk->rel_vol_start);
                    }
                    else
                    {
                        ;//for qac
                    }

                    if (vol_adjust_gain != 0)
                    {
                        p_dae_cfg->dew_bands[i].cutoff = p_dew_band_wk->cutoff;
                        if (((i == DEW_BAND_LOW_FREQ) || (i == DEW_BAND_HIGH_FREQ))
                                && (p_dew_band_wk->reserve_param == 1))
                        {
                            p_dae_cfg->dew_bands[i].cutoff |= (uint16) 0x8000;
                        }
                        p_dae_cfg->dew_bands[i].gain = vol_adjust_gain;
                    }
                }
            }
        }
    }

#if 0
    for (i =0; i < MAX_DEW_SEG; i++)
    {
        PRINT_INFO_INT("cutoff",p_dae_cfg->dew_bands[i].cutoff);
        PRINT_INFO_INT("gain",p_dae_cfg->dew_bands[i].gain);
    }
#endif  
  
    sys_free(p_dew_band);
    p_dew_band = NULL;
}

//更新MDRC相关参数，包括mdrc，limiter，precut，makeup gain等
static void __update_mdrc_setting(dae_config_t *p_dae_cfg, int16 volume_relative, \
                                    int16 signal_energy, int16 vol_adjust_gain_max)
{
    int16 volume_absolute = 0 - volume_relative;
    int16 max_add;
    int16 threshold_add;
    int16 volume_compensate_max;
    int16 limiter_threshold_max;
    uint8 i;

    if (p_dae_cfg->limiter_enable == 1)
    {
        int16 limiter_threshold;

        limiter_threshold = p_dae_cfg->mdrc_threshold_base_max + p_dae_cfg->limiter_threshold_diff;

        limiter_threshold_max = p_dae_cfg->limiter_threshold;
        if (limiter_threshold > limiter_threshold_max)
        {
            limiter_threshold_max = limiter_threshold;
        }
        max_add = limiter_threshold_max - limiter_threshold;
    }
    else
    {
        limiter_threshold_max = 0;
        max_add = 0 - p_dae_cfg->mdrc_threshold_base_max;
    }

    if (volume_absolute >= max_add)
    {
        threshold_add = max_add;
    }
    else if (volume_absolute > 0)
    {
        threshold_add = volume_absolute;
    }
    else
    {
        threshold_add = 0;
    }

    //动态调整Makeup gain
    p_dae_cfg->makeup_gain = (uint16)(max_add - threshold_add);

    //动态调高limiter阈值
    if (p_dae_cfg->limiter_enable == 1)
    {
        p_dae_cfg->limiter_threshold = limiter_threshold_max;
    }
    //动态调高MDRC阈值
    for (i = 0; i < MAX_MDRC_SEG; i++)
    {
        p_dae_cfg->mdrc_bands[i].threshold = p_dae_cfg->mdrc_threshold_base[i] + threshold_add;
    }

    //动态precut，需要通过音量补偿
    if (p_dae_cfg->limiter_enable == 1)
    {
        p_dae_cfg->mdrc_precut_ratio = (uint8)(p_dae_cfg->limiter_threshold_diff + \
            ((int16) 0 - p_dae_cfg->limiter_threshold));
    }
    else
    {
        p_dae_cfg->mdrc_precut_ratio = 0;
    }

    volume_absolute -= threshold_add;

    volume_compensate_max = (int16)(sys_comval->dae_mdrc_peak_gain - p_dae_cfg->equivalent_gain);

    if ((vol_adjust_gain_max > 0) && (vol_adjust_gain_max > (0 - signal_energy)))
    {
        volume_compensate_max += vol_adjust_gain_max - (0 - signal_energy);
    }

    //动态precut，需要通过音量补偿
    p_dae_cfg->precut_ratio = 0;
    if (volume_absolute > 0)
    {
        if (volume_absolute > volume_compensate_max)
        {
            p_dae_cfg->precut_ratio = (uint8)volume_compensate_max;
        }
        else
        {
            p_dae_cfg->precut_ratio = (uint8)volume_absolute;
        }
    }
}

bool __set_dae_config(dae_config_t *p_dae_cfg)
{
    msg_apps_t seteq_msg;
    dae_config_t *p_dae_cfg_shm;
    engine_type_e engine_type;
    bool bret = TRUE;

    //更新DAE配置参数到共享内存
    p_dae_cfg_shm = (dae_config_t *) sys_shm_mount(SHARE_MEM_ID_DAECFG);
    if (p_dae_cfg_shm == NULL)
    {
        PRINT_ERR("dae_cfg shm not exist!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }

#ifdef SUPPORT_ASET_TEST
    if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE) 
        && (g_aset_sync_volume == 0)
        && (g_app_info_vector[APP_TYPE_GUI].app_id != APP_ID_CONFIG))
    {
        //同步当前ASET工具的PEQ设置
        aset_test_set_dae_init();
    }
#endif

    sys_os_sched_lock();
    libc_memcpy(p_dae_cfg_shm, p_dae_cfg, sizeof(dae_config_t));
    sys_os_sched_unlock();

    //发送消息给引擎
    engine_type = get_engine_type();
    if ((engine_type != ENGINE_NULL) && (engine_type != ENGINE_BTCALL))
    {
        seteq_msg.type = MSG_SET_DAE_CFG_SYNC;
        bret = (bool) (send_sync_msg(APP_ID_THEENGINE, &seteq_msg, NULL, 0) == ERR_NO_ERR);
    }

    return bret;
}

static bool __load_update_set_dae_config(dae_config_t *p_dae_cfg, bool need_load)
{
    bool bret = TRUE;

    if (need_load == TRUE)
    {
        com_load_mdrc_config(p_dae_cfg, TRUE, TRUE);
    }

    if (sys_comval->dae_inited_flag == FALSE)
    {
        return bret;
    }

    com_update_dae_config(p_dae_cfg);
    bret = __set_dae_config(p_dae_cfg);

    return bret;
}

static void __set_signal_variable_mode(bool variable_mode)
{
    dae_config_t *p_dae_cfg = &(sys_comval->dae_cfg);

    if (sys_comval->signal_energy_enable == 0)
    {
        variable_mode = 0;
        sys_comval->signal_energy = 0; 
    }

#if (SUPPORT_DETECT_ENERGY == 1)
    if (variable_mode == 1)
    {
        sys_comval->signal_energy = SIGNAL_ENERGY_DEFAULT;
        sys_comval->signal_energy_used = SIGNAL_ENERGY_DEFAULT;

        p_dae_cfg->signal_energy_init = SIGNAL_ENERGY_DEFAULT;
        p_dae_cfg->energy_detect_enable = 1;
    }
    else
#endif
    {
        sys_comval->signal_energy = 0;
        sys_comval->signal_energy_used = 0;

        p_dae_cfg->signal_energy_init = 0;
        p_dae_cfg->energy_detect_enable = 0;
    }
}

uint8 __section__(".BANK47")__calc_dae_freq(dae_config_t *p_dae_cfg)
{
    uint8 dae_freq = 0;
    uint8 band_freq_flag = 0;
    engine_type_e engine_type;
    
#if (SUPPORT_MULTI_FREQ_MULTI_BAND == 1)
    if (p_dae_cfg->MultiFreqBandEnergy_enable == 1)
    {
        dae_freq += 1; //频段能量计算基础频率
        dae_freq += (p_dae_cfg->dae_attributes->num_band*21)/10;
    }

    if (p_dae_cfg->FreqSpetrumDisplay_enable == 1)
    {
        dae_freq += 1; //频点能量计算基础频率
        dae_freq += p_dae_cfg->dae_attributes->num_freq_point*6/10;
    }
#endif
    if(1 == p_dae_cfg->noise_reduction_enable) 
    {
        dae_freq += 5;
    }
#if 0
    if (p_dae_cfg->enable == FALSE)
    {
        p_dae_cfg->run_freq = 0;
        return 0;
    }
#endif
    if (p_dae_cfg->bypass == TRUE)
    {
        p_dae_cfg->run_freq = dae_freq;
        return 0;
    }

    dae_freq += 2; //DAE基本频率

    //避免小米手机蓝牙推歌场景下，在小音量下进行音量调节，性能不足问题
    engine_type = get_engine_type();
    if (engine_type == ENGINE_BTPLAY)
    {
        dae_freq += 9;
    }

    if (p_dae_cfg->peq_enable == 1)
    {
        uint8 peq_band_cnt, i;

        dae_freq += 2;

        peq_band_cnt = 0;
        for (i = 0; i < MAX_PEQ_SEG; i++)
        {
            if (p_dae_cfg->peq_bands[i].gain != 0)
            {
                peq_band_cnt++;
            }
        }

        dae_freq += (uint8) (((uint16) peq_band_cnt*17)/10);
    }

    if (p_dae_cfg->spk_compensation_enable == 1)
    {
        dae_freq += 27;
    }

    //VBASS
    if (p_dae_cfg->vbass_enable == 1)
    {
        dae_freq += 12;            
    }

    //TREBLE
    if (p_dae_cfg->treble_enhance_enable == 1)
    {
        dae_freq += 2;
    }

    //VSURROUND
    if (p_dae_cfg->vsurround_enable == 1)
    {
        dae_freq += 17;
    }
    if (SMART_MODE == p_dae_cfg->audiopp_type)
    {
        //MDRC
        if (p_dae_cfg->mdrc_enable == 1)
        {
            dae_freq += 43;
        }
    }
    else 
    {
         //MDRC增强
        if (p_dae_cfg->mdrc_enable_standard_mode == 1)
        {
            dae_freq += 57;
        }
    }

    if (SMART_MODE == p_dae_cfg->audiopp_type)
    {
        //LIMITER
        if (p_dae_cfg->limiter_enable == 1)
        {
            dae_freq += 6;
        }
    }
    else 
    {
         //compressor
        if (p_dae_cfg->compressor_enable_standard_mode == 1)
        {
            dae_freq += 10;
        }
    }

    p_dae_cfg->run_freq = dae_freq;

    return dae_freq;
}

//DAE 音量系数表更新
uint32 __section__(".BANK47") update_dae_vol_table(void)
{
    vol_dae_para_temp_standard_mode_t vol_dae_para_temp_standard_mode;
    uint16 index = 0;
    uint16 tab_index = 0;
    dae_compressor_temp_standard_mode_t    compressor_temp_standard_mode;
    
    if (1 == sys_comval->aux_flag)
    {
        //设置压缩器(AUX)模式下参数
        com_get_config_struct((uint16) (DAE_COMPRESSOR_AUX_SETTING), (uint8 *)&compressor_temp_standard_mode ,  \
        sizeof(dae_compressor_temp_standard_mode_t));
        libc_memcpy(&(sys_comval->dae_cfg.compressor_standard_mode), &compressor_temp_standard_mode.compressor,      \
        sizeof(dae_compressor_standard_mode_t));

        index = (31 - (uint16)sys_comval->volume_current) + VOL_31_DAE_AUX;
        PRINT_INFO_INT("index_aux",index);
        
        com_get_config_struct(index , (uint8 *)&vol_dae_para_temp_standard_mode ,  \
        sizeof(vol_dae_para_temp_standard_mode_t));
    
        sys_comval->dae_cfg.precut_standard_mode                = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.precut_standard_mode;
        
        sys_comval->dae_cfg.compressor_standard_mode.threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.compressor_th1;
        
        sys_comval->dae_cfg.compressor_standard_mode.threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.compressor_th2;

        sys_comval->dae_cfg.mdrc_extend_para_standard_mode.signal_adjust        = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.signal_fine_tune;
        
        sys_comval->mdrc_vol_adjust_standard_mode                               = \
        (int8)vol_dae_para_temp_standard_mode.vol_dae_para_sm.vol_fine_tune;
        
        sys_comval->dae_cfg.mdrc_extend_para_standard_mode.makeup_gain          = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.gain_compensation;
        
        sys_comval->dae_cfg.mdrc_peak_standard_mode.MDRC_compensation_threshold = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.compensation_filter_threshold;
                                     
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[0].threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_low_freq_th1;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[0].threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_low_freq_th2;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[1].threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_middle_freq_th1;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[1].threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_middle_freq_th2;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[2].threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_high_freq_th1;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[2].threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_high_freq_th2;
    }
    else
    {
        //设置压缩器(非AUX)模式下参数
        com_get_config_struct((uint16) (DAE_COMPRESSOR_SETTING), (uint8 *)&compressor_temp_standard_mode ,  \
        sizeof(dae_compressor_temp_standard_mode_t));
        libc_memcpy(&(sys_comval->dae_cfg.compressor_standard_mode), &compressor_temp_standard_mode.compressor,      \
        sizeof(dae_compressor_standard_mode_t));

        index = (31 - (uint16)sys_comval->volume_current) + VOL_31_DAE;
        PRINT_INFO_INT("index_unaux",index);
        
        com_get_config_struct(index , (uint8 *)&vol_dae_para_temp_standard_mode ,  \
        sizeof(vol_dae_para_temp_standard_mode_t));
    
        sys_comval->dae_cfg.precut_standard_mode                = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.precut_standard_mode;
        
        sys_comval->dae_cfg.compressor_standard_mode.threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.compressor_th1;
        
        sys_comval->dae_cfg.compressor_standard_mode.threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.compressor_th2;

        sys_comval->dae_cfg.mdrc_extend_para_standard_mode.signal_adjust        = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.signal_fine_tune;
        
        sys_comval->mdrc_vol_adjust_standard_mode                               = \
        (int8)vol_dae_para_temp_standard_mode.vol_dae_para_sm.vol_fine_tune;
        
        sys_comval->dae_cfg.mdrc_extend_para_standard_mode.makeup_gain          = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.gain_compensation;
        
        sys_comval->dae_cfg.mdrc_peak_standard_mode.MDRC_compensation_threshold = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.compensation_filter_threshold;
                                     
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[0].threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_low_freq_th1;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[0].threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_low_freq_th2;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[1].threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_middle_freq_th1;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[1].threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_middle_freq_th2;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[2].threshold1 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_high_freq_th1;
        
        sys_comval->dae_cfg.p_mdrc_band_standard_mode[2].threshold2 = \
        vol_dae_para_temp_standard_mode.vol_dae_para_sm.mdrc_high_freq_th2;
    }
}


