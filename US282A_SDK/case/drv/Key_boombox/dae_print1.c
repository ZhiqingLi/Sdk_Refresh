/*******************************************************************************
 *                         
 * dae数据打印
 *
 *******************************************************************************/
#include <dae_info_def.h>
#include <dae_standard_mode_info_def.h>
#include <mmm_dae_def.h>

#include <psp_includes.h>

void dae_param_print1(uint8 audiopp_type)
{
    uint8 i = 0;
    DAE_para_info_t* p_dae_param = (DAE_para_info_t*)(&g_dae_param_info);

    PRINT_INFO_INT("SpeakerCompensationChangeFlag",p_dae_param->SpeakerCompensationChangeFlag);
    PRINT_INFO_INT("FilterOrder",               p_dae_param->FilterOrder);     
    PRINT_INFO_INT("FilterQvalLeft",            p_dae_param->FilterQvalLeft);
    PRINT_INFO_INT("FilterQvalRight",           p_dae_param->FilterQvalRight);

    PRINT_INFO_INT("makeup_gain",               p_dae_param->makeup_gain);

    //多频段信息
    PRINT_INFO("******MULTI_BAND*****");
    PRINT_INFO_INT("duration_ms",               p_dae_param->duration_ms);
    PRINT_INFO_INT("num_band",                  p_dae_param->num_band);   
    for(i = 0; i< MFBE_BANDS; i++)
    {
        PRINT_INFO_INT("band_freq",             p_dae_param->f_c[i]);
    } 
    for(i = 0; i< MFBE_BANDS; i++)
    {
        PRINT_INFO_INT("band_energys",          p_dae_param->energys[i]);
    }


    //多频点信息
    PRINT_INFO("******MULTI_FREQ*****");
    PRINT_INFO_INT("num_freq_point",            p_dae_param->num_freq_point);
    for(i = 0; i< MAX_FREQ_POINT; i++)
    {
        PRINT_INFO_INT("freq_point_freq",       p_dae_param->freq_point[i]);
    } 
    for(i = 0; i< MAX_FREQ_POINT; i++)
    {
        PRINT_INFO_INT("freq_point_energys",    p_dae_param->freq_point_mag[i]);
    }

    //信号检测相关
    PRINT_INFO_INT("energy_detect_enable",      p_dae_param->energy_detect_enable);
    PRINT_INFO_INT("period",                    p_dae_param->period);     
    PRINT_INFO_INT("period_count",              p_dae_param->period_count);
    PRINT_INFO_INT("preadjust_db",              p_dae_param->preadjust_db);
    PRINT_INFO_INT("preadjust_count",           p_dae_param->preadjust_count);
    PRINT_INFO_INT("signal_energy_inner",       p_dae_param->signal_energy_inner);     
    PRINT_INFO_INT("energy",                    p_dae_param->energy);
    PRINT_INFO_INT("energy_max",                p_dae_param->energy_max);

    if (SMART_MODE == audiopp_type)
    {
        PRINT_INFO_INT("Limiter_threshold",     p_dae_param->Limiter_threshold);
        PRINT_INFO_INT("Limiter_attack_time",   p_dae_param->Limiter_attack_time);     
        PRINT_INFO_INT("Limiter_release_time",  p_dae_param->Limiter_release_time);
        PRINT_INFO_INT("reserve",p_dae_param->reserve);

        for(i = 0; i< MDRC_NUM_BANDS; i++)
        {
            PRINT_INFO_INT("mdrc_threshold",    p_dae_param->mdrc_band_settings[i].threshold);
            PRINT_INFO_INT("mdrc_attack_time",  p_dae_param->mdrc_band_settings[i].attack_time);
            PRINT_INFO_INT("mdrc_release_time", p_dae_param->mdrc_band_settings[i].release_time);
            PRINT_INFO_INT("mdrc_reserve",      p_dae_param->mdrc_band_settings[i].reserve);
        } 
    }
    else 
    {

        DAE_para_info_standard_mode_t* p_dae_param_standard_mode = (DAE_para_info_standard_mode_t*)(&g_dae_param_info);
        PRINT_INFO_INT("Compressor_threshold1",       p_dae_param_standard_mode->Compressor_threshold1);
        PRINT_INFO_INT("Compressor_ratio1",           p_dae_param_standard_mode->Compressor_ratio1);
        PRINT_INFO_INT("Compressor_threshold2",       p_dae_param_standard_mode->Compressor_threshold2);
        PRINT_INFO_INT("Compressor_ratio2",           p_dae_param_standard_mode->Compressor_ratio2);
        PRINT_INFO_INT("Compressor_tav",              p_dae_param_standard_mode->Compressor_tav);
        PRINT_INFO_INT("Compressor_attack_time",      p_dae_param_standard_mode->Compressor_attack_time);
        PRINT_INFO_INT("Compressor_release_time",     p_dae_param_standard_mode->Compressor_release_time);
        PRINT_INFO_INT("power_regulation",            p_dae_param_standard_mode->power_regulation);

        for(i = 0; i< MDRC_NUM_BANDS_STANDARD_MODE; i++)
        {
            PRINT_INFO_INT("mdrc_threshold1",         p_dae_param_standard_mode->mdrc_band_settings[i].threshold1);
            PRINT_INFO_INT("mdrc_compress_ratio1",    p_dae_param_standard_mode->mdrc_band_settings[i].compress_ratio1);
            PRINT_INFO_INT("mdrc_threshold2",         p_dae_param_standard_mode->mdrc_band_settings[i].threshold2);
            PRINT_INFO_INT("mdrc_compress_ratio2",    p_dae_param_standard_mode->mdrc_band_settings[i].compress_ratio2);

            PRINT_INFO_INT("mdrc_tav_time",           p_dae_param_standard_mode->mdrc_band_settings[i].tav_time);
            PRINT_INFO_INT("mdrc_attack_time",        p_dae_param_standard_mode->mdrc_band_settings[i].attack_time);
            PRINT_INFO_INT("mdrc_release_time",       p_dae_param_standard_mode->mdrc_band_settings[i].release_time);
            PRINT_INFO_INT("mdrc_reserve",            p_dae_param_standard_mode->mdrc_band_settings[i].reserve);
        } 
      
    } 
}

