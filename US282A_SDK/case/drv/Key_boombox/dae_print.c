/*******************************************************************************
 *                         
 * daeÊý¾Ý´òÓ¡
 *
 *******************************************************************************/
#include <dae_info_def.h>
#include <dae_standard_mode_info_def.h>
#include <mmm_dae_def.h>

#include <psp_includes.h>

void dae_param_print(uint8 audiopp_type)
{
    uint8 i = 0;
    DAE_para_info_t* p_dae_param = (DAE_para_info_t*)(&g_dae_param_info);

    if (SMART_MODE == audiopp_type)
    {
        PRINT_INFO("SMART_MODE");
    }
    else 
    {
        PRINT_INFO("STANDARD_MODE"); 
    }
    
    PRINT_INFO_INT("sample_rate",               p_dae_param->sample_rate);
    PRINT_INFO_INT("channels",                  p_dae_param->channels);
    PRINT_INFO_INT("block_size",                p_dae_param->block_size);

    PRINT_INFO_INT("fade_in_time_ms",           p_dae_param->fade_in_time_ms);
    PRINT_INFO_INT("fade_out_time_ms",          p_dae_param->fade_out_time_ms);
    PRINT_INFO_INT("mute_time_ms",              p_dae_param->mute_time_ms);
    PRINT_INFO_INT("fade_in_flag",              p_dae_param->fade_in_flag);
    PRINT_INFO_INT("fade_out_flag",             p_dae_param->fade_out_flag);
    PRINT_INFO_INT("mute_flag",                 p_dae_param->mute_flag);

    PRINT_INFO_INT("DAE_init_flag",             p_dae_param->DAE_init_flag);
    PRINT_INFO_INT("DAE_change_flag",           p_dae_param->DAE_change_flag);
    PRINT_INFO_INT("output_channel_config",     p_dae_param->output_channel_config);
    PRINT_INFO_INT("bypass",                    p_dae_param->bypass);

    PRINT_INFO_INT("precut",                    p_dae_param->precut);

    PRINT_INFO_INT("Vbass_enable",              p_dae_param->Vbass_enable);
    PRINT_INFO_INT("Surround_enable",           p_dae_param->Surround_enable);
    PRINT_INFO_INT("TrebleEnhance_enable",      p_dae_param->TrebleEnhance_enable);
    PRINT_INFO_INT("Peq_enable",                p_dae_param->Peq_enable);
    
    PRINT_INFO_INT("SpeakerCompensation_enable",p_dae_param->SpeakerCompensation_enable);
    
    if (SMART_MODE == audiopp_type)
    {
        PRINT_INFO_INT("Limiter_enable",        p_dae_param->Limiter_enable);
    }
    else if(STANDARD_MODE == audiopp_type)
    {
        PRINT_INFO_INT("Compressor_enable",     p_dae_param->Limiter_enable);
    }

    PRINT_INFO_INT("MultibandDRC_enable",       p_dae_param->MultibandDRC_enable);
    PRINT_INFO_INT("DownwardExpander_enable",   p_dae_param->DownwardExpander_enable);
    PRINT_INFO_INT("MultiFreqBandEnergy_enable",p_dae_param->MultiFreqBandEnergy_enable);
    PRINT_INFO_INT("MultiFreqBandEnergy_enable",p_dae_param->MultiFreqBandEnergy_enable);

    PRINT_INFO("noise reduction");
    PRINT_INFO_INT("DownExp_attack_time",        p_dae_param->DownExp_attack_time);
    PRINT_INFO_INT("DownExp_release_time",       p_dae_param->DownExp_release_time);
    PRINT_INFO_INT("DownExp_set0_threshold",     p_dae_param->DownExp_set0_threshold);
    PRINT_INFO_INT("DownExp_threshold",          p_dae_param->DownExp_threshold);
    PRINT_INFO_INT("DownExp_ratio",              p_dae_param->DownExp_ratio);

    PRINT_INFO_INT("Vbass_low_cut_off_frequency",p_dae_param->Vbass_low_cut_off_frequency);
    PRINT_INFO_INT("Vbass_gain",                 p_dae_param->Vbass_gain);     
    PRINT_INFO_INT("Vbass_type",                 p_dae_param->Vbass_type);
    
    PRINT_INFO_INT("Surround_angle",             p_dae_param->Surround_angle);
    PRINT_INFO_INT("Surround_gain",              p_dae_param->Surround_gain);

    PRINT_INFO_INT("Treble_frequency",           p_dae_param->Treble_frequency);
    PRINT_INFO_INT("Treble_gain",                p_dae_param->Treble_gain);

    for(i = 0; i< (MDRC_NUM_BANDS - 1); i++)
    {
        PRINT_INFO_INT("crossover_freqency", p_dae_param->crossover_freqency[i]);
    }
    PRINT_INFO_INT("MDRC_compensation_peak_detect_attack_time",     \
                   p_dae_param->MDRC_compensation_peak_detect_attack_time);
    PRINT_INFO_INT("MDRC_compensation_peak_detect_release_time",    \
                   p_dae_param->MDRC_compensation_peak_detect_release_time);     
    PRINT_INFO_INT("MDRC_compensation_threshold",                   p_dae_param->MDRC_compensation_threshold);
    PRINT_INFO_INT("MDRC_compensation_filter_Q",                    p_dae_param->MDRC_compensation_filter_Q);

    PRINT_INFO_INT("Peq_change_flag",p_dae_param->Peq_change_flag);
    for(i = 0; i< EQ_NUM_BANDS; i++)
    {
        PRINT_INFO_INT("PEQ_cutoff",            p_dae_param->band_settings[i].cutoff);
        PRINT_INFO_INT("PEQ_q",                 p_dae_param->band_settings[i].q);
        PRINT_INFO_INT("PEQ_gain",              p_dae_param->band_settings[i].gain);
        PRINT_INFO_INT("PEQ_type",              p_dae_param->band_settings[i].type);
    }   
    for(i = 0; i< EQ_NUM_BANDS; i++)
    {
        PRINT_INFO_INT("PEQ_enble_type",        p_dae_param->band_enable_type[i]);
    } 

}

