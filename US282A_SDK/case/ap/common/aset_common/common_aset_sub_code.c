/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief
 *      <author>       <time>
 *       wuyufan        2015-05-30       review
 *******************************************************************************/
#include "common_aset_new_func.h"
#include "common_aset_interface.h"


extern dae_config_t *g_p_dae_cfg;

int32 aset_write_data(uint16 cmd, void *data_buffer, uint32 data_len)
{
    int32 ret_val;
    int32 timeout;

    stub_ext_param_t ext_param;

    ext_param.rw_buffer = sys_malloc_large_data(256);
    try_again: timeout = 0;
    ext_param.opcode = cmd;
    ext_param.payload_len = (uint16)data_len;

    libc_memcpy(&(ext_param.rw_buffer[6]), data_buffer, data_len);

    ret_val = stub_ext_write(&ext_param);

    if (ret_val == 0)
    {
        //等待ACK 100ms
        while (1)
        {
            //读取ACK数据,最多等待100ms
            ext_param.payload_len = 0;
            ret_val = stub_ext_read(&ext_param);

            if (ret_val == 0)
            {
                if ((ext_param.rw_buffer[1] == 0x03) && (ext_param.rw_buffer[2] == 0xfe))
                {
                    break;
                }
            }
            else
            {
                sys_mdelay(10);
                timeout++;
                if (timeout == 10)
                {
                    //如果数据接收失败，会重新发送数据
                    goto try_again;
                }
            }
        }
    }

    sys_free_large_data(ext_param.rw_buffer);

    return ret_val;
}

void config_pc_view(aset_interface_config_t *pCfg)
{
//用这个宏的目的:使用waves pc工具时，避免空间不够用
#ifndef   WAVES_ASET_TOOLS
    char fw_version[] = "282A105";

    pCfg->bEQ_v_1_0 = 0;
    pCfg->bVBass_v_1_0 = 0;
    pCfg->bVBass_v_1_1 = 1;
    pCfg->bTE_v_1_0 = 0;
    pCfg->bTE_v_1_1 = 1;
    pCfg->bSurround_v_1_0 = 0;
    //pCfg->bLimiter_v_1_0 = 1;
    //pCfg->bMDRC_v_1_0 = 1;
    pCfg->bSRC_v_1_0 = 0;
    pCfg->bSEE_v_1_0 = 1;
    pCfg->bSEW_v_1_0 = 1;
    pCfg->bSD_v_1_0 = 1;
    pCfg->bEQ_v_1_1 = 0;
    pCfg->bMS_v_1_0 = 1;

    pCfg->bEQ_v_1_2 = 1;
    
    if (SMART_MODE == g_p_dae_cfg->audiopp_type)
    {
        pCfg->bLimiter_v_1_0 = 1;
        pCfg->bMDRC_v_1_0 = 1;

        pCfg->bMDRC_v_1_1 = 0;
        pCfg->bComPressor_v_1_0 = 0;
    }
    else if (STANDARD_MODE == g_p_dae_cfg->audiopp_type)
    {
        pCfg->bLimiter_v_1_0 = 0;
        pCfg->bMDRC_v_1_0 = 0;
        
        pCfg->bMDRC_v_1_1 = 1;
        pCfg->bComPressor_v_1_0 = 1;
    }
    else
    {
        ;//for qac
    }
    libc_memcpy(pCfg->szVerInfo, fw_version, sizeof(fw_version));
    
 #endif   
}


int32 aset_write_case_info(void)
{
    aset_case_info_t *aset_case_info = (aset_case_info_t *) ASET_RW_DATA_BUF;

    libc_memset(aset_case_info, 0x00, sizeof(aset_case_info_t));
    aset_case_info->peq_point_num = EQMAXPOINTCOUNT_ALL;
    config_pc_view(&aset_case_info->stInterface);

#ifdef WAVES_ASET_TOOLS
    
#else
    aset_write_data(STUB_CMD_ASET_WRITE_STATUS, aset_case_info, sizeof(aset_case_info_t));          
#endif

    return 0;
}

int32 aset_write_peq_data(dae_config_t *p_dae)
{
    int i;

    aset_peq_data_t *aset_peq_data = (aset_peq_data_t *) ASET_RW_DATA_BUF;

    aset_peq_data->main_gain_value = p_dae->precut_ratio;

    aset_peq_data->main_gain_enable = 1;

    aset_peq_data->nPointTotal = EQMAXPOINTCOUNT;

    aset_peq_data->peq_enable = (int8)(p_dae->peq_enable);

    for (i = 0; i < EQMAXPOINTCOUNT; i++)
    {
        aset_peq_data->peq_array[i].freq = (int16)p_dae->peq_bands[i].cutoff;
        aset_peq_data->peq_array[i].Qval = p_dae->peq_bands[i].q;
        aset_peq_data->peq_array[i].gain = p_dae->peq_bands[i].gain;
        aset_peq_data->peq_array[i].type = (int16)p_dae->peq_bands[i].type;

        if (p_dae->peq_bands[i].gain != 0)
        {
            aset_peq_data->peq_array[i].enable = 1;
        }
        else
        {
            aset_peq_data->peq_array[i].enable = 0;
        }
    }

    aset_write_data(STUB_CMD_ASET_WRITE_EQ_DATA, aset_peq_data, sizeof(aset_peq_data_t));

    return 0;
}

int32 aset_write_vbass_data(dae_config_t *p_dae)
{
    aset_vbass_data_t *aset_vbass_data = (aset_vbass_data_t *) ASET_RW_DATA_BUF;

    aset_vbass_data->cutoff_freq = (int16)p_dae->vbass_cut_freq;

    aset_vbass_data->vbass_gain = p_dae->vbass_ratio;

    aset_vbass_data->vbass_enable = (int8)p_dae->vbass_enable;

    aset_write_data(STUB_CMD_ASET_WRITE_VBASS_DATA, aset_vbass_data, sizeof(aset_vbass_data_t));

    return 0;
}

int32 aset_write_te_data(dae_config_t *p_dae)
{
    aset_te_data_t *aset_te_data = (aset_te_data_t *) ASET_RW_DATA_BUF;

    aset_te_data->cutoff_freq = (int16)p_dae->treble_enhance_cut_freq;

    aset_te_data->te_gain = p_dae->treble_enhance_ratio;

    aset_te_data->te_enable = (int8)p_dae->treble_enhance_enable;

    aset_write_data(STUB_CMD_ASET_WRITE_TE_DATA, aset_te_data, sizeof(aset_te_data_t));

    return 0;
}

int32 aset_write_surr_data(dae_config_t *p_dae)
{
    aset_surr_data_t *aset_surr_data = (aset_surr_data_t *) ASET_RW_DATA_BUF;

    aset_surr_data->head_angle = p_dae->vsurround_angle;

    aset_surr_data->surr_gain = p_dae->vsurround_ratio;

    aset_surr_data->surr_enable = (int8)p_dae->vsurround_enable;

    aset_write_data(STUB_CMD_ASET_WRITE_SURR_DATA, aset_surr_data, sizeof(aset_surr_data_t));

    return 0;
}

int32 aset_write_limiter_data(dae_config_t *p_dae)
{
    aset_limiter_data_t *aset_limiter_data = (aset_limiter_data_t *) ASET_RW_DATA_BUF;

    aset_limiter_data->attack_time = (int16)p_dae->limiter_attack_time;

    aset_limiter_data->release_time = (int16)p_dae->limiter_release_time;

    aset_limiter_data->threhold = p_dae->limiter_threshold;

    aset_limiter_data->limiter_enable = (int8)p_dae->limiter_enable;

    aset_write_data(STUB_CMD_ASET_WRITE_LIMITER_DATA, aset_limiter_data, sizeof(aset_limiter_data_t));

    return 0;
}

int32 aset_write_volume_data(void)
{
    int volume;

    volume = com_get_sound_volume();

    aset_write_data(STUB_CMD_ASET_WRITE_VOLUME, &volume, sizeof(volume));

    return 0;
}

int32 aset_upload_sound_param(void)
{
//用这个宏的目的:使用waves pc工具时，避免空间不够用
#ifndef   WAVES_ASET_TOOLS

    aset_write_volume_data();

    aset_write_peq_data(g_p_dae_cfg);

    aset_write_vbass_data(g_p_dae_cfg);

    aset_write_te_data(g_p_dae_cfg);

    aset_write_surr_data(g_p_dae_cfg);

    aset_write_limiter_data(g_p_dae_cfg);

#endif
    return 0;
}

