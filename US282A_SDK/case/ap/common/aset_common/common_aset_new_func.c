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

#define UPDATE_PEQ_MODE_PEQ        0
#define UPDATE_PEQ_MODE_VBASS      1
#define UPDATE_PEQ_MODE_TREBLE     2
#define UPDATE_PEQ_MODE_VSURROUND  3
#define UPDATE_PEQ_MODE_LIMITER    4
#define UPDATE_PEQ_MODE_MDRC       5
#define UPDATE_PEQ_MODE_MDRC_OTHER 6
#define UPDATE_PEQ_MODE_SEE        7
#define UPDATE_PEQ_MODE_SEW        8
#define UPDATE_PEQ_MODE_SIGNAL     9
#define UPDATE_PEQ_MODE_SFR        10
#define UPDATE_PEQ_MODE_PEQ2       11
#define UPDATE_PEQ_MODE_PEQ3       12
//兼容标准音效流程增加宏定义
#define UPDATE_PEQ_MODE_MDRC_STANDARD       13
#define UPDATE_PEQ_MODE_MDRC_OTHER_STANDARD 14
#define UPDATE_PEQ_MODE_COMPRESSOR_OTHER_STANDARD 15

extern dae_config_t *g_p_dae_cfg;
extern uint32 g_aset_base_timer;

int32 aset_read_mdrc2_data(void);
static int32  __section__(".bank_2")aset_read_compressor_standard_mode(void);
static int32  __section__(".bank_2")aset_read_multibank_drc_data_standard_mode(void);
static int32 __section__(".bank_2")aset_read_mdrc2_data_standard_mode(void);




int32 aset_read_data(uint16 cmd, void *data_buffer, uint32 data_len)
{
    int32 ret_val;

    stub_ext_param_t ext_param;

    uint8 *read_buffer = (uint8 *) data_buffer;

    ext_param.opcode = cmd;
    ext_param.payload_len = 0;
    ext_param.rw_buffer = sys_malloc_large_data(256);

    ret_val = stub_ext_write(&ext_param);

    if (ret_val == 0)
    {
        ext_param.payload_len = (uint16)data_len;
        ret_val = stub_ext_read(&ext_param);
        if (ret_val != 0)
        {
            libc_print("aset read err.L&C=",(data_len <<16) + cmd, 2);
        }
        if (ret_val == 0)
        {
            libc_memcpy(read_buffer, &(ext_param.rw_buffer[6]), data_len);

            //回复ACK
#ifdef WAVES_ASET_TOOLS
			ext_param.opcode = STUB_CMD_WAVES_ASET_ACK;
#else
            ext_param.opcode = STUB_CMD_ASET_ACK;
#endif  
            ext_param.payload_len = 0;
            ret_val = stub_ext_write(&ext_param);
            //break;
        }
    }

    sys_free_large_data(ext_param.rw_buffer);

    return ret_val;
}

int32 aset_read_volume(void)
{
    int volume;

    int ret_val;

    ret_val = aset_read_data(STUB_CMD_ASET_READ_VOLUME, &volume, sizeof(volume));

    if (ret_val == 0)
    {
        //update volume
        uint8 cur_volume = (uint8) volume;//0-31

        if (cur_volume > VOLUME_VALUE_MAX)
        {
            cur_volume = VOLUME_VALUE_MAX;
        }

        g_aset_sync_volume = 1;
        com_set_sound_volume(cur_volume, SET_VOLUME_SYNC_TO_REMOTE);
        g_aset_sync_volume = 0;
    }

    return 0;
}

int32 aset_read_main_switch(void)
{
    int main_switch;

    int ret_val;

    ret_val = aset_read_data(STUB_CMD_ASET_READ_MAIN_SWITCH, &main_switch, sizeof(main_switch));

    if (ret_val == 0)
    {
        g_aset_main_switch_flag = (bool) main_switch;
    }

    return 0;
}

void __section__(".bank_2") update_peq_point(dae_config_t *p_dae, int8 peq_point_start, \
                                                aset_peq_point_t *peq_points,int8 peq_point_count)
{
    int i;

    for (i = 0; i < peq_point_count; i++)
    {
        p_dae->peq_bands[peq_point_start + i].cutoff = (uint16) peq_points[i].freq;
        p_dae->peq_bands[peq_point_start + i].q = (uint16) peq_points[i].Qval;
        p_dae->peq_bands[peq_point_start + i].gain = peq_points[i].gain;
        p_dae->peq_bands[peq_point_start + i].type = (uint16) peq_points[i].type;

		if ((p_dae->peq_bands[peq_point_start + i].type == 2) && (p_dae->peq_bands[peq_point_start + i].gain != 0))
		{
			sys_comval->highpass_cutoff = (uint8)p_dae->peq_bands[peq_point_start + i].cutoff;
		}

        //libc_print("cutoff: ", p_dae->peq_bands[peq_point_start + i].cutoff, 2);
        //libc_print("Qval: ", p_dae->peq_bands[peq_point_start + i].q, 2);
        //libc_print("gain: ", p_dae->peq_bands[peq_point_start + i].gain, 2);
        //libc_print("type: ", p_dae->peq_bands[peq_point_start + i].type, 2);  
    }

    p_dae->peq_band_enable_type[peq_point_start + i] = (uint8)peq_points[i].enable;

}

void __section__(".bank_2") update_peq_value(dae_config_t *p_dae, void *aset_data, uint8 update_mode)
{
    int i;

    sys_os_sched_lock();

    if (update_mode == UPDATE_PEQ_MODE_PEQ)
    {
        aset_peq_data_t *aset_peq_data = (aset_peq_data_t *) aset_data;

        //判断是否使能PEQ
        p_dae->peq_enable = (bool)aset_peq_data->peq_enable;

        //均衡器使能
        if (aset_peq_data->peq_enable == 1)
        {
            update_peq_point(p_dae, 0, aset_peq_data->peq_array, aset_peq_data->nPointTotal);
        }

        if (aset_peq_data->main_gain_enable == 1)
        {
            g_aset_main_gain_value = aset_peq_data->main_gain_value;
            g_aset_equivalent_gain = aset_peq_data->equivalent_gain_value;
            if (p_dae->mdrc_enable == 1)
            {
                p_dae->equivalent_gain = g_aset_equivalent_gain;
                p_dae->precut_ratio = 0;
                p_dae->mdrc_precut_ratio = 0;
                sys_comval->dae_mdrc_peak_gain = g_aset_main_gain_value;
            }
            else
            {
                p_dae->equivalent_gain = 0;
                p_dae->precut_ratio = g_aset_main_gain_value;
                p_dae->mdrc_precut_ratio = 0;
            }

            p_dae->precut_standard_mode = g_aset_main_gain_value; //标准模式

			//PRINT_INFO_INT("aset_precut_ratio_sm", p_dae->precut_ratio);
			//PRINT_INFO_INT("aset_precut_ratio_st", p_dae->precut_standard_mode);
        }
        else
        {
            p_dae->precut_ratio = 0;
            p_dae->mdrc_precut_ratio = 0;
            p_dae->equivalent_gain = 0;

            p_dae->precut_standard_mode = 0;                            //标准模式
        }
    }
    else if (update_mode == UPDATE_PEQ_MODE_PEQ2)
    {
        aset_peq_data_2_3_t *aset_eq_data_2_3 = (aset_peq_data_2_3_t *) aset_data;
#if 0
        if (EQMAXPOINTCOUNT_ALL >= (EQMAXPOINTCOUNT*2))
        {
            update_peq_point(p_dae, EQMAXPOINTCOUNT, aset_eq_data_2_3->peq_array, EQMAXPOINTCOUNT);
        }
#endif        
        update_peq_point(p_dae, EQMAXPOINTCOUNT, aset_eq_data_2_3->peq_array, \
            EQMAXPOINTCOUNT_ALL - EQMAXPOINTCOUNT);
    }
    else if (update_mode == UPDATE_PEQ_MODE_PEQ3)
    {
        aset_peq_data_2_3_t *aset_eq_data_2_3 = (aset_peq_data_2_3_t *) aset_data;

        update_peq_point(p_dae, EQMAXPOINTCOUNT*2, aset_eq_data_2_3->peq_array, \
            EQMAXPOINTCOUNT_ALL - EQMAXPOINTCOUNT*2);
    }
    else if (update_mode == UPDATE_PEQ_MODE_VBASS)
    {
        aset_vbass_data_t *aset_vbass_data = (aset_vbass_data_t *) aset_data;

        p_dae->vbass_enable = (bool)aset_vbass_data->vbass_enable;
        p_dae->vbass_cut_freq = (uint16) aset_vbass_data->cutoff_freq;
        if ((aset_vbass_data->vbass_gain >= 0) && (aset_vbass_data->vbass_gain <= 240))
        {
            p_dae->vbass_ratio = (int16)(aset_vbass_data->vbass_gain) - 120; //虚拟低音增益范围转换成(-12.0dB - 12.0dB)
        }
        else
        {
            p_dae->vbass_ratio = 0;
        }
    }
    else if (update_mode == UPDATE_PEQ_MODE_TREBLE)
    {
        aset_te_data_t *aset_te_data = (aset_te_data_t *) aset_data;

        p_dae->treble_enhance_enable = (bool)aset_te_data->te_enable;
        p_dae->treble_enhance_cut_freq = (uint16) aset_te_data->cutoff_freq;
        p_dae->treble_enhance_ratio = (int16)(aset_te_data->te_gain);
    }
    else if (update_mode == UPDATE_PEQ_MODE_VSURROUND)
    {
        aset_surr_data_t *aset_surr_data = (aset_surr_data_t *) aset_data;

        p_dae->vsurround_angle = (uint8)aset_surr_data->head_angle;
        p_dae->vsurround_ratio = (int8)aset_surr_data->surr_gain;
        p_dae->vsurround_enable = (bool)aset_surr_data->surr_enable;
    }
    else if (update_mode == UPDATE_PEQ_MODE_LIMITER)
    {
        aset_limiter_data_t *aset_limiter_data = (aset_limiter_data_t *) aset_data;

        p_dae->limiter_threshold = aset_limiter_data->threhold;
        p_dae->limiter_attack_time = (uint16)aset_limiter_data->attack_time;
        p_dae->limiter_release_time = (uint16)aset_limiter_data->release_time;
        p_dae->limiter_enable = (bool)aset_limiter_data->limiter_enable;
    }
    else if (update_mode == UPDATE_PEQ_MODE_MDRC)
    {
        aset_mdrc_data_t *aset_mdrc_data = (aset_mdrc_data_t *) aset_data;

        p_dae->mdrc_threshold_base_max = -600;
        for (i = 0; i < MAX_MDRC_SEG; i++)
        {
            libc_memcpy(&(p_dae->mdrc_bands[i]), &(aset_mdrc_data->mdrc_band[i]), sizeof(aset_mdrc_band_t));
            p_dae->mdrc_bands[i].reserve = 0;
            p_dae->mdrc_threshold_base[i] = p_dae->mdrc_bands[i].threshold;
            if (p_dae->mdrc_threshold_base[i] > p_dae->mdrc_threshold_base_max)
            {
                p_dae->mdrc_threshold_base_max = p_dae->mdrc_threshold_base[i];
            }
        }

        p_dae->mdrc_enable = (bool)aset_mdrc_data->mdrc_enable;

        if (p_dae->mdrc_enable == 1)
        {
            p_dae->equivalent_gain = g_aset_equivalent_gain;
            p_dae->precut_ratio = 0;
            p_dae->mdrc_precut_ratio = 0;
            sys_comval->dae_mdrc_peak_gain = g_aset_main_gain_value;
        }
        else
        {
            p_dae->equivalent_gain = 0;
            p_dae->precut_ratio = g_aset_main_gain_value;
            p_dae->mdrc_precut_ratio = 0;
        }
    }
    else if (update_mode == UPDATE_PEQ_MODE_MDRC_OTHER)
    {
        aset_mDRC2_data_t *aset_mdrc2_data = (aset_mDRC2_data_t *) aset_data;

        p_dae->mdrc_crossover_freq[0] = (uint16)aset_mdrc2_data->nMidMin;
        p_dae->mdrc_crossover_freq[1] = (uint16)aset_mdrc2_data->nMaxMid;
        p_dae->limiter_threshold_diff = (int16)aset_mdrc2_data->limiterThresholdDiff;
        p_dae->post_precut_ratio = (int8)aset_mdrc2_data->signalPrecut;
        sys_comval->mdrc_vol_adjust = (int8)aset_mdrc2_data->fMVolume;
    }
    else if (update_mode == UPDATE_PEQ_MODE_SEE)
    {
        aset_SEEnhance_data_t *aset_SEEnhance_data = (aset_SEEnhance_data_t *) aset_data;
        dew_band_t *p_dew_band;

        p_dew_band = sys_malloc(sizeof(dew_band_t)*MAX_DEW_SEG);
        sys_comval->dae_enhanced_enable = aset_SEEnhance_data->bEnableSEE;
        sys_comval->dae_enhanced_peak_ratio = aset_SEEnhance_data->peak_ratio;

        libc_memcpy(p_dew_band, aset_SEEnhance_data, sizeof(dew_band_t)*MAX_DEW_SEG);

        for (i = 0; i < MAX_DEW_SEG; i++)
        {
            p_dew_band[i].rel_vol_start *= 10; //单位从1db转换为0.1db
            p_dew_band[i].rel_vol_keep *= 10; //单位从1db转换为0.1db
        }

        sys_vm_write(p_dew_band, VM_DEW_PARAM_BASE, sizeof(dew_band_t)*MAX_DEW_SEG);

        sys_free(p_dew_band);
        p_dew_band = NULL;
    }
    else if (update_mode == UPDATE_PEQ_MODE_SEW)
    {
        aset_SEWeaken_data_t *aset_SEWeaken_data = (aset_SEWeaken_data_t *) aset_data;
        dew_band_t *p_dew_band;

        p_dew_band = sys_malloc(sizeof(dew_band_t)*MAX_DEW_SEG);

        sys_comval->dae_weaken_enable = aset_SEWeaken_data->bEnableSEW;

        libc_memcpy(p_dew_band, aset_SEWeaken_data, sizeof(dew_band_t)*MAX_DEW_SEG);

        for (i = 0; i < MAX_DEW_SEG; i++)
        {
            p_dew_band[i].rel_vol_start *= 10; //单位从1db转换为0.1db
            p_dew_band[i].rel_vol_keep *= 10; //单位从1db转换为0.1db
        }

        sys_vm_write(p_dew_band, VM_DEW_PARAM_BASE + 0x10000, sizeof(dew_band_t)*MAX_DEW_SEG);

        sys_free(p_dew_band);
        p_dew_band = NULL;
    }
    else if (update_mode == UPDATE_PEQ_MODE_SIGNAL)
    {
        aset_SignalDete_data_t *aset_SignalDete_data = (aset_SignalDete_data_t *) aset_data;

        sys_comval->signal_energy_enable = aset_SignalDete_data->bEnableSD;
        if (sys_comval->signal_energy_enable == 0)
        {
            p_dae->energy_detect_enable = 0;
            sys_comval->signal_energy = 0; 
        }

        p_dae->period = aset_SignalDete_data->fSignalDetePer * 100; //单位从0.1S转换为1ms
        p_dae->period_count = (uint8)aset_SignalDete_data->nSignalDetePerNum;
        p_dae->preadjust_db = aset_SignalDete_data->fSignalDesTVal;
        p_dae->preadjust_count = (uint8)aset_SignalDete_data->nSignalDesPerNum;
        sys_comval->signal_energy_min = aset_SignalDete_data->nSignalMinVal;
    }
    else if (update_mode == UPDATE_PEQ_MODE_SFR)
    {
        aset_FRCurve_data_t *aset_FRCurve_data = (aset_FRCurve_data_t *) aset_data;
        uint8 *buff = aset_FRCurve_data->stFirInfo.fir_coeff;
        uint8 index = aset_FRCurve_data->index;

        p_dae->spk_compensation_enable = aset_FRCurve_data->bEnFRCurve;

        p_dae->spk_comp_source = 1;
        p_dae->spk_comp_filter_order = (short)aset_FRCurve_data->stFirInfo.fir_order;
        if (index == 0)
        {
            p_dae->spk_comp_filter_QvalLeft = (uint8)aset_FRCurve_data->stFirInfo.fir_coeff_Q;
        }
        if (index == 4)
        {
            p_dae->spk_comp_filter_QvalRight = (uint8)aset_FRCurve_data->stFirInfo.fir_coeff_Q;
        }
        sys_vm_write(buff, VM_SPK_COMP_DAT_BASE + (((uint32)index) << 16), 128);
    }
    else if (update_mode == UPDATE_PEQ_MODE_COMPRESSOR_OTHER_STANDARD)
    {
        Compressor_TP_Info_M *pCompressor_TP_Info = (Compressor_TP_Info_M *) aset_data;
       
        p_dae->compressor_enable_standard_mode = pCompressor_TP_Info->bLmEnable;

        p_dae->compressor_standard_mode.threshold1 = pCompressor_TP_Info->nThreshold_01;
        p_dae->compressor_standard_mode.ratio1 = pCompressor_TP_Info->nCpRatio_01;
        p_dae->compressor_standard_mode.threshold2 = pCompressor_TP_Info->nThreshold_02;
        p_dae->compressor_standard_mode.ratio2 = pCompressor_TP_Info->nCpRatio_02;
        p_dae->compressor_standard_mode.tav = pCompressor_TP_Info->nRmsAvgTime;
        p_dae->compressor_standard_mode.attack_time = pCompressor_TP_Info->fAttack_time;
        p_dae->compressor_standard_mode.release_time = pCompressor_TP_Info->nRelease_time;    
    }
    else if (update_mode == UPDATE_PEQ_MODE_MDRC_STANDARD)
    {
        DRC_CP_Info_M *pDRC_CpVal_Info = (DRC_CP_Info_M *) aset_data;
        int16 mdrc_threshold_max = -600;
                      
        for (i = 0; i < MDRC_NUM_BANDS_STANDARD_MODE; i++)
        {
            libc_memcpy((p_dae->p_mdrc_band_standard_mode) + i, &(pDRC_CpVal_Info->stDrcVal_M[i]), \
                sizeof(DRC_CpVal_Info_M));
            p_dae->p_mdrc_band_standard_mode[i].reserve = 0;
        }       
        p_dae->mdrc_enable_standard_mode = (bool)pDRC_CpVal_Info->bDrcEnable;
    }
    else if (update_mode == UPDATE_PEQ_MODE_MDRC_OTHER_STANDARD)
    {
        DRC_TP_Info_M3 *pDRC_TP_Info_M3 = (DRC_TP_Info_M3 *) aset_data;
        
        if (p_dae->mdrc_enable_standard_mode == 1)
        {
            p_dae->mdrc_extend_para_standard_mode.signal_adjust = (int16)pDRC_TP_Info_M3->fSignalPrecut;//功率调节
            p_dae->mdrc_extend_para_standard_mode.mdrc_crossover_freq0 = pDRC_TP_Info_M3->nMidMin; 
            p_dae->mdrc_extend_para_standard_mode.mdrc_crossover_freq1 = pDRC_TP_Info_M3->nMaxMid; 
            p_dae->mdrc_extend_para_standard_mode.makeup_gain = (int16)pDRC_TP_Info_M3->fGainCom; //灵敏度
            sys_comval->mdrc_vol_adjust_standard_mode = (int8)pDRC_TP_Info_M3->fMVolume;
            
            p_dae->mdrc_peak_standard_mode.MDRC_compensation_peak_detect_attack_time = pDRC_TP_Info_M3->fStTime; 
            p_dae->mdrc_peak_standard_mode.MDRC_compensation_peak_detect_release_time = pDRC_TP_Info_M3->nReleaseTime;  
            p_dae->mdrc_peak_standard_mode.MDRC_compensation_threshold = pDRC_TP_Info_M3->fTsVal;  
            p_dae->mdrc_peak_standard_mode.MDRC_compensation_filter_Q = pDRC_TP_Info_M3->fQVal;
#if 0
            PRINT_INFO_INT("signal_adjust",pDRC_TP_Info_M3->fSignalPrecut);
            PRINT_INFO_INT("nMidMin",pDRC_TP_Info_M3->nMidMin);
            PRINT_INFO_INT("nMaxMid",pDRC_TP_Info_M3->nMaxMid);
            PRINT_INFO_INT("fGainCom",pDRC_TP_Info_M3->fGainCom);
            PRINT_INFO_INT("fMVolume",pDRC_TP_Info_M3->fMVolume);
            PRINT_INFO_INT("fStTime",pDRC_TP_Info_M3->fStTime);
#endif            
        }
    }
    else
    {
        ;//nothing
    }

    sys_os_sched_unlock();
}

void enable_aset_sound_effect(void)
{
    msg_apps_t seteq_msg;
    dae_config_t *p_dae_cfg_shm;

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

    sys_os_sched_lock();
    libc_memcpy(p_dae_cfg_shm, g_p_dae_cfg, sizeof(dae_config_t));
    sys_os_sched_unlock();

    //发送消息给引擎
    if (get_engine_type() != ENGINE_NULL)
    {
        seteq_msg.type = MSG_SET_DAE_CFG_SYNC;
        send_sync_msg(APP_ID_THEENGINE, &seteq_msg, NULL, 0);
    }
}

int32 aset_read_eq_data(void)
{
    int ret_val;

    aset_peq_data_t *eq_data_buffer = (aset_peq_data_t *) (ASET_READ_DATA_BUF);
    aset_peq_data_2_3_t *eq_data_2_3_buffer = (aset_peq_data_2_3_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_EQ_DATA, eq_data_buffer, sizeof(aset_peq_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, eq_data_buffer, UPDATE_PEQ_MODE_PEQ);
    }

    if (EQMAXPOINTCOUNT_ALL > EQMAXPOINTCOUNT)
    {
        ret_val = aset_read_data(STUB_CMD_ASET_READ_EQ2_DATA, eq_data_2_3_buffer, sizeof(aset_peq_data_2_3_t));

        if (ret_val == 0)
        {
            //update eq
            update_peq_value(g_p_dae_cfg, eq_data_2_3_buffer, UPDATE_PEQ_MODE_PEQ2);
        }
    }

#if 0
    if (EQMAXPOINTCOUNT_ALL > (EQMAXPOINTCOUNT*2))
    {
        ret_val = aset_read_data(STUB_CMD_ASET_READ_EQ3_DATA, eq_data_2_3_buffer, sizeof(aset_peq_data_2_3_t));

        if (ret_val == 0)
        {
            //update eq
            update_peq_value(g_p_dae_cfg, eq_data_2_3_buffer, UPDATE_PEQ_MODE_PEQ3);
        }
    }
#endif

    return 0;
}

int32 aset_read_vbass_data(void)
{
    int ret_val;

    aset_vbass_data_t *vbass_data_buffer = (aset_vbass_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_VBASS_DATA, vbass_data_buffer, sizeof(aset_vbass_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, vbass_data_buffer, UPDATE_PEQ_MODE_VBASS);
    }

    return 0;

}

int32 aset_read_te_data(void)
{
    int ret_val;

    aset_te_data_t *te_data_buffer = (aset_te_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_TE_DATA, te_data_buffer, sizeof(aset_te_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, te_data_buffer, UPDATE_PEQ_MODE_TREBLE);
    }

    return 0;

}

int32 aset_read_surr_data(void)
{
    int ret_val;

    aset_surr_data_t *surr_data_buffer = (aset_surr_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_SURR_DATA, surr_data_buffer, sizeof(aset_surr_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, surr_data_buffer, UPDATE_PEQ_MODE_VSURROUND);
    }

    return 0;

}

int32 aset_read_limiter_data(void)
{
    int ret_val;

    aset_limiter_data_t *limiter_data_buffer = (aset_limiter_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_LIMITER_DATA, limiter_data_buffer, sizeof(aset_limiter_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, limiter_data_buffer, UPDATE_PEQ_MODE_LIMITER);
    }

    return 0;

}

int32 aset_read_multibank_drc_data(void)
{

    int ret_val;
    uint8 mDRC2_en;
    aset_mdrc_data_t *mdrc_data_buffer = (aset_mdrc_data_t *) (ASET_READ_DATA_BUF);
    mdrc_data_buffer->mDRC2_enable = 0;
    ret_val = aset_read_data(STUB_CMD_ASET_READ_MDRC_DATA, mdrc_data_buffer, sizeof(aset_mdrc_data_t));
    mDRC2_en = (uint8)mdrc_data_buffer->mDRC2_enable;//避免数据被改掉
    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, mdrc_data_buffer, UPDATE_PEQ_MODE_MDRC);
        if(mDRC2_en == 0x5A)//跟PC工具商议好，标志为5A
        {
            aset_read_mdrc2_data();
        }
    }
    return 0;
}

int32 aset_read_mdrc2_data(void)
{

    int ret_val;

    aset_mDRC2_data_t *aset_buff = (aset_mDRC2_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_MDRC2_DATA, aset_buff, sizeof(aset_mDRC2_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, aset_buff, UPDATE_PEQ_MODE_MDRC_OTHER);
    }

    return 0;
}

int32 aset_read_frcurve_data(uint16 cmd)
{
    int ret_val;

    aset_FRCurve_data_t *aset_buff = (aset_FRCurve_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(cmd, aset_buff, sizeof(aset_FRCurve_data_t));

    if (ret_val == 0)
    {
        //update eq
        aset_buff->index = (uint8)((cmd & 0x0F) - 1);
        update_peq_value(g_p_dae_cfg, aset_buff, UPDATE_PEQ_MODE_SFR);
    }
    return ret_val;
}


int32 __section__(".bank_2")aset_read_see_data(void)
{
    int ret_val;

    aset_SEEnhance_data_t *aset_buff = (aset_SEEnhance_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_SEE_DATA, aset_buff, sizeof(aset_SEEnhance_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, aset_buff, UPDATE_PEQ_MODE_SEE);
    }

    return 0;

}

int32 __section__(".bank_2")aset_read_sew_data(void)
{
    int ret_val;

    aset_SEWeaken_data_t *aset_buff = (aset_SEWeaken_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_SEW_DATA, aset_buff, sizeof(aset_SEWeaken_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, aset_buff, UPDATE_PEQ_MODE_SEW);
    }

    return 0;
}

int32 __section__(".bank_2")aset_read_SignalD_data(void)
{
    int ret_val;

    aset_SignalDete_data_t *aset_buff = (aset_SignalDete_data_t *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_SD_DATA, aset_buff, sizeof(aset_SignalDete_data_t));

    if (ret_val == 0)
    {
        //update eq
        update_peq_value(g_p_dae_cfg, aset_buff, UPDATE_PEQ_MODE_SIGNAL);
    }

    return 0;
}


int32 aset_cmd_deal(aset_status_t *aset_status)
{
    bool sound_effect_changed = FALSE;
    bool aset_signal_detect_update = FALSE;
    
    //PRINT_DATA(aset_status,sizeof(aset_status_t));    
    if (aset_status->upload_case_info == TRUE)
    {
        PRINT_INFO_INT("update_audiopp",aset_status->update_audiopp);
        if (aset_status->update_audiopp == TRUE)
        {
            sound_effect_changed = TRUE;
            aset_update_audiopp();
        }
        aset_write_case_info();
    }
    else if (aset_status->upload_data == TRUE)
    {
        aset_upload_sound_param();
    }
    else
    {
        ;//for qac
    }
    
    if (aset_status->state == 1)
    {
        
        if (aset_status->volume_changed == TRUE)
        {
            aset_read_volume();

            sound_effect_changed = TRUE;
        }

        if (aset_status->main_switch_changed == TRUE)
        {
            aset_read_main_switch();

            sound_effect_changed = TRUE;
        }

        if (aset_status->eq_data_changed == TRUE)
        {
            aset_read_eq_data();

            sound_effect_changed = TRUE;
        }

        if (aset_status->vbass_data_changed == TRUE)
        {
            aset_read_vbass_data();

            sound_effect_changed = TRUE;
        }

        if (aset_status->te_data_changed == TRUE)
        {
            aset_read_te_data();

            sound_effect_changed = TRUE;
        }

        if (aset_status->surr_data_changed == TRUE)
        {
            aset_read_surr_data();

            sound_effect_changed = TRUE;
        }

        if (aset_status->limiter_data_changed == TRUE)
        {
            aset_read_limiter_data();

            sound_effect_changed = TRUE;
        }

        if (aset_status->multiband_drc_data_changed == TRUE)
        {
            aset_read_multibank_drc_data();

            sound_effect_changed = TRUE;
        }

        if (aset_status->Left_frcurve_data_changed == TRUE)
        {
            uint8 i;
            sound_effect_changed = TRUE;
            for(i = 0; i < 4;i++)
            {
                aset_read_frcurve_data(STUB_CMD_ASET_READ_LFRC_DATA_BASE + (uint16)i);
            }
        }

        if (aset_status->Right_frcurve_data_changed == TRUE)
        {
            uint8 i;
            sound_effect_changed = TRUE;
            for(i = 0; i < 4;i++)
            {
                aset_read_frcurve_data(STUB_CMD_ASET_READ_RFRC_DATA_BASE + (uint16)i);
            }
        }

        if (aset_status->see_data_changed == TRUE)
        {
            sound_effect_changed = TRUE;
            aset_read_see_data();
        }

        if (aset_status->sew_data_changed == TRUE)
        {
            sound_effect_changed = TRUE;
            aset_read_sew_data();
        }

        if (aset_status->SignalD_data_changed == TRUE)
        {
            sound_effect_changed = TRUE;
            aset_signal_detect_update = TRUE;
            aset_read_SignalD_data();
        }

        if (aset_status->standard_mode_mdrc_changed == TRUE)
        {
            sound_effect_changed = TRUE;
        }
        
        if (aset_status->compressor_changed == TRUE)
        {
            sound_effect_changed = TRUE;
            aset_read_compressor_standard_mode();
        }

        if (aset_status->standard_mode_mdrc_changed == TRUE)
        {
            sound_effect_changed = TRUE;
            aset_read_multibank_drc_data_standard_mode();
        }

        if (sound_effect_changed == TRUE)
        {
            bool signal_detect_flag = FALSE;

            if (aset_status->main_switch_changed == 1)
            {
                if (g_aset_main_switch_flag == 0)
                {
                    com_set_dae_onoff(FALSE);
                }
                else
                {
                    com_set_dae_onoff(TRUE);
                }
            }

            if ((g_aset_neednot_take_effect == FALSE) && (aset_signal_detect_update == TRUE))
            {
                if (sys_comval->signal_energy_enable == 1)
                {
                    signal_detect_flag = TRUE; //如果信号检测更新且开启，那么要先关闭使之还原为初始状态，然后再恢复
                    sys_comval->signal_energy_enable = 0;
                    g_p_dae_cfg->energy_detect_enable = 0;
                }

                com_load_mdrc_config(g_p_dae_cfg, FALSE, TRUE);
            }
            else
            {
                com_load_mdrc_config(g_p_dae_cfg, FALSE, FALSE);
            }

            com_update_dae_config(g_p_dae_cfg);
            if (g_aset_neednot_take_effect == FALSE)
            {
                enable_aset_sound_effect();
            }

            if (signal_detect_flag == TRUE)
            {
                sys_os_time_dly(10); //delay 100ms,确保信号检测已经关闭

                sys_comval->signal_energy_enable = 1;

                com_load_mdrc_config(g_p_dae_cfg, FALSE, TRUE);
                com_update_dae_config(g_p_dae_cfg);
                if (g_aset_neednot_take_effect == FALSE)
                {
                    enable_aset_sound_effect();
                }
            }

            g_aset_sync_volume = 1;
            com_reset_sound_volume(0);
            g_aset_sync_volume = 0;
        }
    }
    else
    {
        ; //nothing for QAC
    }

    return 0;
}

void aset_set_dae_init(void)
{
    int ret_val;

    aset_status_t aset_status;

    g_aset_neednot_take_effect = TRUE;

    if (g_aset_run_state.run_state == ASET_TOOLS_NOT_WORK)
    {
        #ifdef SUPPORT_ASET_TEST
        aset_reconnect_deal();
        #endif
    }

    if (g_aset_run_state.run_state == ASET_TOOLS_WORK)
    {
        ret_val = aset_read_data(STUB_CMD_ASET_READ_STATUS, &aset_status, sizeof(aset_status));

        //PRINT_INFO_INT("aset_status.state",aset_status.state);
        if (aset_status.state == 1)
        {
            aset_status.volume_changed = 1;
            aset_status.eq_data_changed = 1;
            //aset_status.limiter_data_changed = 1;
            aset_status.surr_data_changed = 1;
            aset_status.te_data_changed = 1;
            aset_status.vbass_data_changed = 1;
            //aset_status.multiband_drc_data_changed = 1;

            aset_status.Left_frcurve_data_changed = 1;
            aset_status.Right_frcurve_data_changed = 1;
            aset_status.see_data_changed = 1;
            aset_status.sew_data_changed = 1;
            aset_status.SignalD_data_changed = 1;

            if (SMART_MODE == sys_comval->dae_cfg.audiopp_type)
            {
                aset_status.limiter_data_changed = 1;
                aset_status.multiband_drc_data_changed = 1;  

                aset_status.compressor_changed = 0;
                aset_status.standard_mode_mdrc_changed = 0;
            }
            else 
            {
                aset_status.limiter_data_changed = 0;
                aset_status.multiband_drc_data_changed = 0;  

                aset_status.compressor_changed = 1;
                aset_status.standard_mode_mdrc_changed = 1;
            }

            //强制读一次ASET数据到小机
            aset_cmd_deal(&aset_status);
        }
    }

    g_aset_neednot_take_effect = FALSE;
}

int32  __section__(".bank_2")aset_update_audiopp(void)
{

    uint8 audiopp_type;

    audiopp_type = get_audiopp_type();

    if (g_p_dae_cfg->audiopp_type  != audiopp_type)
    {
        g_p_dae_cfg->audiopp_type = audiopp_type;
        g_p_dae_cfg->audiopp_type_update_flag = 1;
    }

    return 0;
}

static int32  __section__(".bank_2")aset_read_compressor_standard_mode(void)
{
    int ret_val;

    Compressor_TP_Info_M *vbass_data_buffer = (Compressor_TP_Info_M *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_COMPRESSOR_DATA_STANDARD_MODE, vbass_data_buffer,           \
        sizeof(Compressor_TP_Info_M));

    if (ret_val == 0)
    {
        update_peq_value(g_p_dae_cfg, vbass_data_buffer, UPDATE_PEQ_MODE_COMPRESSOR_OTHER_STANDARD);
    }

    return 0;
}

static int32 __section__(".bank_2")aset_read_multibank_drc_data_standard_mode(void)
{
    int ret_val;
    uint8 cDataFlag;
    
    DRC_CP_Info_M *mdrc_data_buffer = (DRC_CP_Info_M *) (ASET_READ_DATA_BUF);
    mdrc_data_buffer->bDrcEnable = 0;
    ret_val = aset_read_data(STUB_CMD_ASET_READ_MDRC_DATA_STANDARD_MODE, mdrc_data_buffer, sizeof(DRC_CP_Info_M));
    cDataFlag = (uint8)mdrc_data_buffer->cDataFlag;//避免数据被改掉
      
    if (ret_val == 0)
    {
        update_peq_value(g_p_dae_cfg, mdrc_data_buffer, UPDATE_PEQ_MODE_MDRC_STANDARD);
        if(cDataFlag == 0x5A)//跟PC工具商议好，标志为5A
        {
            aset_read_mdrc2_data_standard_mode();        
        }
    }
    return 0;
}

static int32 __section__(".bank_2")aset_read_mdrc2_data_standard_mode(void)
{

    int ret_val;

    DRC_TP_Info_M3 *aset_buff = (DRC_TP_Info_M3 *) (ASET_READ_DATA_BUF);

    ret_val = aset_read_data(STUB_CMD_ASET_READ_MDRC2_DATA_STANDARD_MODE, aset_buff, sizeof(DRC_TP_Info_M3));

    if (ret_val == 0)
    {
        update_peq_value(g_p_dae_cfg, aset_buff, UPDATE_PEQ_MODE_MDRC_OTHER_STANDARD);
    }

    return 0;
}




