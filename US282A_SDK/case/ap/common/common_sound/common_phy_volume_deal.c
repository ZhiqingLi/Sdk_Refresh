/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：物理音量管理接口，包括设置音量和获取音量；支持I2S。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "music_common.h"
#include "common_func.h"
#include "ccd_interface.h"

#if ((CASE_LINEIN_CHANNEL_SEL == 0) || (CASE_FM_CHANNEL_SEL == 0))
extern const int16 g_ana_pa_db[ANA_PA_VOLUME_MAX];
extern const vol_hard_t g_hard_volume_table[VOLUME_TABLE_MAX+1][VOLUME_VALUE_MAX+1];
#else
extern const vol_hard_t g_hard_volume_table[VOLUME_VALUE_MAX+1];
extern const vol_hard_t g_hard_volume_table_sm[VOLUME_VALUE_MAX+1];
#endif
extern const uint16 g_hard_volume_table_i2s_pa[VOLUME_VALUE_MAX+1];

static void _calc_keytone_precut_i2s_pa(uint8 set_vol, uint16 reg_val);
static void _calc_keytone_precut_inner_pa(uint8 set_vol, uint32 vol_hard_pa, uint32 vol_hard_da);
static void _set_phy_volume_i2s_pa(uint16 reg_val_to);
static void _set_phy_volume_inner_pa(uint32 vol_hard_pa_to, uint32 vol_hard_da_to);
uint32 __section__(".BANK46") mdrc_comp_handle(uint32 vol_hard_da);
static void _set_phy_volume_inner_pa_sm(uint32 vol_hard_pa_to, uint32 vol_hard_da_to);
static void _set_ao_source_dac_sm(int tab_index, uint8 vol_to);
static void adjust_input_signal(uint8 aout_mode); 



/******************************************************************************/
/*!
 * \par  Description:
 * \设置硬件音量调节
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      common_func
 * \note
 * \li
 *******************************************************************************/
void com_set_phy_volume(uint8 set_vol)
{
    int tab_index, tab_index_tmp;
    int db;
    uint8 vol_to = set_vol;
    bool vbass_enable_temp = sys_comval->dae_cfg.vbass_enable;

    PRINT_INFO_INT("bypass",sys_comval->dae_cfg.bypass);
    PRINT_INFO_INT("set_vol",set_vol);

	if (STUB_PC_TOOL_ASET_EQ_MODE != g_app_info_state.stub_tools_type)
	{
		//非调音模式下，规避小信号大音量的时候，虚拟低音打开会产生电流声
		if ((sys_comval->signal_energy_enable == 1)
        && (sys_comval->signal_variable_mode == 1)
        && (g_app_info_state_all.energy_available_flag == TRUE))
	    {
	        sys_comval->signal_energy = *(g_app_info_state_all.p_energy_value - 1);
	        
	        if(sys_comval->signal_energy<-300)
	        {
	            sys_comval->dae_cfg.vbass_enable = 0;        
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
	                sys_comval->dae_cfg.vbass_enable = sys_comval->default_vbass_enable;
	            }
	        }
	    }
	}
	    
    if (sys_comval->volume_gain > VOLUME_TABLE_MAX)
    {
        PRINT_WARNING("Invalid volume gain!!");
        sys_comval->volume_gain = VOLUME_TABLE_MAX;
    }

    if (vol_to > VOLUME_VALUE_MAX)
    {
        vol_to = VOLUME_VALUE_MAX;
    }

    sys_comval->volume_current_phy = set_vol;
 
    tab_index = VOLUME_VALUE_MAX - vol_to;
    
    if (TRUE == sys_comval->w_input_enable)
    {
        adjust_input_signal(g_app_info_state.aout_mode);
        return ;
    }
    
    switch (g_app_info_state.aout_mode)
    {
        case AO_SOURCE_I2S: //i2s
        //case AO_SOURCE_SPDIF: //spdif
        {
            uint16 reg_val_to;

            reg_val_to = g_hard_volume_table_i2s_pa[tab_index];

            //音量限制处理
            db = (int16) g_hard_volume_table_i2s_pa[0] - (int16) reg_val_to; //TO MODIFY CASE BY CASE
            db = db * 10 / 8; //TO MODIFY CASE BY CASE
            if (db > sys_comval->volume_limit)
            {
                reg_val_to = (uint16)(reg_val_to + ((db - sys_comval->volume_limit) * 8 / 10)); //TO MODIFY CASE BY CASE
            }

            _calc_keytone_precut_i2s_pa(vol_to, reg_val_to);

            _set_phy_volume_i2s_pa(reg_val_to);
        }
        break;

        //internel dac & i2s or spdif and external dac both chanel
        case AO_SOURCE_DACI2S: //i2s + internel dac
        case AO_SOURCE_DACSPDIF: //spdif +internel dac
        default:
        {
            if (SMART_MODE == sys_comval->dae_cfg.audiopp_type)
            {              
                uint32 vol_hard_pa_to, vol_hard_da_to;
                uint32 vol_hard_pa_tmp, vol_hard_da_tmp;
               
#if ((CASE_LINEIN_CHANNEL_SEL == 0) || (CASE_FM_CHANNEL_SEL == 0))
                vol_hard_pa_to = g_hard_volume_table[sys_comval->volume_gain][tab_index].vol_pa;
                vol_hard_da_to = g_hard_volume_table[sys_comval->volume_gain][tab_index].vol_da;

                //音量限制处理
                vol_hard_pa_tmp = g_hard_volume_table[sys_comval->volume_gain][0].vol_pa;
                vol_hard_da_tmp = g_hard_volume_table[sys_comval->volume_gain][0].vol_da;

                db = g_ana_pa_db[vol_hard_pa_to] - g_ana_pa_db[vol_hard_pa_tmp];
                db += ((int32) vol_hard_da_to - (int32) vol_hard_da_tmp) * 15 / 4;
#else
                vol_hard_pa_to = g_hard_volume_table[tab_index].vol_pa - sys_comval->volume_gain;

                vol_hard_da_to = g_hard_volume_table[tab_index].vol_da;

                //音量限制处理
                vol_hard_pa_tmp = g_hard_volume_table[0].vol_pa - sys_comval->volume_gain;
                vol_hard_da_tmp = g_hard_volume_table[0].vol_da;

                db = ((int16) vol_hard_da_to - (int16) vol_hard_da_tmp) * 15 / 4;
#endif
                //音量限制处理
                if (db > sys_comval->volume_limit)
                {
                    vol_hard_da_to = vol_hard_da_to - (uint32)((db - sys_comval->volume_limit) * 4 / 15);
                }

                _calc_keytone_precut_inner_pa(vol_to, vol_hard_pa_to, vol_hard_da_to);
                _set_phy_volume_inner_pa(vol_hard_pa_to, vol_hard_da_to);       
            }
            else if (STANDARD_MODE == sys_comval->dae_cfg.audiopp_type)
            {
                _set_ao_source_dac_sm(tab_index, vol_to);
            }
            else
            {
                ;//nothing
            }

        }
        break;
    }
    sys_comval->dae_cfg.vbass_enable = vbass_enable_temp;
}
void com_update_volume_limit(int8 vol_limit)
{
    PRINT_INFO_INT("vol_limit",vol_limit);
    
    sys_comval->volume_limit = vol_limit;
    if(get_mute_enable() == FALSE)//静音时不能调音量
    {
        com_set_phy_volume(sys_comval->volume_current);
    }
}

static void _calc_keytone_precut_i2s_pa(uint8 set_vol, uint16 reg_val)
{
    int db;
    int tab_index_tmp;

    if (set_vol > sys_comval->kt_tts_max_volume)
    {
        tab_index_tmp = VOLUME_VALUE_MAX - sys_comval->kt_tts_max_volume;

        db = (int16) reg_val - (int16) g_hard_volume_table_i2s_pa[tab_index_tmp];
        db = db * 10 / 8; //TO MODIFY CASE BY CASE

        if (db >= 0)
        {
            sys_comval->kt_tts_percent = 100;
        }
        else
        {
            sys_comval->kt_tts_percent = (uint8)(com_math_exp_fixed(db));//db为负数
        }
    }
    else
    {
        sys_comval->kt_tts_percent = 100;
    }
}

static void _calc_keytone_precut_inner_pa(uint8 set_vol, uint32 vol_hard_pa, uint32 vol_hard_da)
{
    int db;
    int tab_index_tmp;
    uint32 vol_hard_pa_tmp, vol_hard_da_tmp;

#if ((CASE_LINEIN_CHANNEL_SEL == 0) || (CASE_FM_CHANNEL_SEL == 0))

    if (set_vol > sys_comval->kt_tts_max_volume)
    {
        tab_index_tmp = VOLUME_VALUE_MAX - sys_comval->kt_tts_max_volume;

        vol_hard_pa_tmp = g_hard_volume_table[sys_comval->volume_gain][tab_index_tmp].vol_pa;
        vol_hard_da_tmp = g_hard_volume_table[sys_comval->volume_gain][tab_index_tmp].vol_da;

        db = (g_ana_pa_db[vol_hard_pa_tmp] + ((int16) vol_hard_da_tmp * 15 / 4))
                - (g_ana_pa_db[vol_hard_pa] + ((int16) vol_hard_da * 15 / 4));

        if (db >= 0)
        {
            sys_comval->kt_tts_percent = 100;
        }
        else
        {
            sys_comval->kt_tts_percent = (uint8)(com_math_exp_fixed(db));//db为负数
        }
    }
    else
    {
        sys_comval->kt_tts_percent = 100;
    }

#else

    if (set_vol > sys_comval->kt_tts_max_volume)
    {
        tab_index_tmp = VOLUME_VALUE_MAX - sys_comval->kt_tts_max_volume;

        vol_hard_da_tmp = g_hard_volume_table[tab_index_tmp].vol_da;

        db = ((int16) vol_hard_da_tmp - (int16) vol_hard_da) * 15 / 4;

        if (db >= 0)
        {
            sys_comval->kt_tts_percent = 100;
        }
        else
        {
            sys_comval->kt_tts_percent = (uint8)(com_math_exp_fixed(db));//db为负数
        }
    }
    else
    {
        sys_comval->kt_tts_percent = 100;
    }

#endif
}

//注意：该接口需要根据PA音量单位修改，修改点注释为 //TO MODIFY CASE BY CASE
static void _set_phy_volume_i2s_pa(uint16 reg_val_to)
{
    uint16 reg_val = reg_val_to;
    int db;
    uint8 tab_index = 0;
        
    db = (int16) g_hard_volume_table_i2s_pa[0] - (int16) reg_val_to; //TO MODIFY CASE BY CASE
    db = db * 10 / 8; //TO MODIFY CASE BY CASE
    sys_comval->volume_relative = (int16)db;

    if (reg_val < 0x3ff) //TO MODIFY CASE BY CASE
    {
        reg_val += ((uint16) sys_comval->volume_gain * 8); //TO MODIFY CASE BY CASE
    }

    if (FALSE == sys_comval->dae_cfg.bypass)
    {
        com_set_dae_config_dynamic();

        if ((SMART_MODE == sys_comval->dae_cfg.audiopp_type) && (sys_comval->dae_cfg.mdrc_enable == 1)) 
        {
            int16 limit_threshold;
            int16 vol_offset, tmp_vol_offset;

            limit_threshold = sys_comval->dae_cfg.mdrc_threshold_base_max;
            if (sys_comval->dae_cfg.limiter_enable == 1)
            {
                limit_threshold += ((int16) sys_comval->dae_cfg.limiter_threshold_diff \
                    - sys_comval->dae_cfg.limiter_threshold);
            }

            vol_offset  = (int16) 0 - limit_threshold;
            vol_offset -= (int16)sys_comval->dae_cfg.makeup_gain;
            vol_offset += sys_comval->dae_cfg.precut_ratio;
            vol_offset += sys_comval->mdrc_vol_adjust;

            if (vol_offset > 0)
            {
                reg_val -= vol_offset * 8 / 10; //TO MODIFY CASE BY CASE
            }
            else
            {
                reg_val += ((int16) 0 - vol_offset) * 8 / 10; //TO MODIFY CASE BY CASE
            }

            //保证PA音量递增
            tmp_vol_offset = vol_offset * 8 / 10; //TO MODIFY CASE BY CASE
            tmp_vol_offset = tmp_vol_offset * 10 / 8; //TO MODIFY CASE BY CASE

            if (vol_offset != tmp_vol_offset)
            {
                reg_val--; //TO MODIFY CASE BY CASE
            }

            if (reg_val < g_hard_volume_table_i2s_pa[0]) //TO MODIFY CASE BY CASE
            {
                reg_val = g_hard_volume_table_i2s_pa[0];
            }
        }
        else if(STANDARD_MODE == sys_comval->dae_cfg.audiopp_type)
        {
            //标准模式不根据音效参数修改PA寄存器值
        }
        else
        {
            ;//nothing
        }
    }
    
    if (0 == sys_comval->volume_current_phy)
    {
        reg_val = 0; //避免使用智能模式音效时，当音量为0，由于对PA的补偿，使得PA数字音量不为0
    }
    
    PRINT_INFO_INT("i2s_reg_val",reg_val);
    ccd_i2s_pa_set_volume(reg_val);        
}

static void _set_phy_volume_inner_pa(uint32 vol_hard_pa_to, uint32 vol_hard_da_to)
{
    uint32 vol_hard_pa, vol_hard_da;
    uint32 vol_hard_pa_tmp, vol_hard_da_tmp;
    int db;


    vol_hard_pa = vol_hard_pa_to;
    vol_hard_da = vol_hard_da_to;

#if ((CASE_LINEIN_CHANNEL_SEL == 0) || (CASE_FM_CHANNEL_SEL == 0))
    vol_hard_pa_tmp = g_hard_volume_table[sys_comval->volume_gain][0].vol_pa;
    vol_hard_da_tmp = g_hard_volume_table[sys_comval->volume_gain][0].vol_da;

    db = g_ana_pa_db[vol_hard_pa_to] - g_ana_pa_db[vol_hard_pa_tmp];
    db += ((int32) vol_hard_da_to - (int32) vol_hard_da_tmp) * 15 / 4;

    sys_comval->volume_relative = (int16)db;
#endif

#if ((CASE_LINEIN_CHANNEL_SEL == 1) && (CASE_FM_CHANNEL_SEL == 1))

    db = ((int16) vol_hard_da_to - (int16) g_hard_volume_table[0].vol_da) * 15 / 4;
    sys_comval->volume_relative = (int16)db;
    
    if (FALSE == sys_comval->dae_cfg.bypass)
    {
        com_set_dae_config_dynamic();
        
        if (sys_comval->dae_cfg.mdrc_enable == 1)
        {
            int16 limit_threshold;
            int16 vol_offset, tmp_vol_offset;

            limit_threshold = sys_comval->dae_cfg.mdrc_threshold_base_max;
            if (sys_comval->dae_cfg.limiter_enable == 1)
            {
                limit_threshold += ((int16) sys_comval->dae_cfg.limiter_threshold_diff \
                    - sys_comval->dae_cfg.limiter_threshold);
            }

            vol_offset  = (int16) 0 - limit_threshold;
            vol_offset -= (int16)sys_comval->dae_cfg.makeup_gain;
            vol_offset += sys_comval->dae_cfg.precut_ratio;
            vol_offset += sys_comval->mdrc_vol_adjust;

            if (vol_offset > 0)
            {
                vol_hard_da += (uint32)(vol_offset * 4 / 15);
            }
            else
            {
                vol_hard_da -= (uint32)(((int16) 0 - vol_offset) * 4 / 15);
            }

            //保证PA音量递增
            tmp_vol_offset = vol_offset * 4 / 15;
            tmp_vol_offset = tmp_vol_offset * 15 / 4;

            if (vol_offset != tmp_vol_offset)
            {
                vol_hard_da++;
            }

            if (vol_hard_da > g_hard_volume_table[0].vol_da)
            {
                vol_hard_da = g_hard_volume_table[0].vol_da;
            }
        }
    }
  
#endif

    if (0 == sys_comval->volume_current_phy)
    {
        vol_hard_da = 0; //避免使用智能模式音效时，当音量为0，由于对PA的补偿，使得PA数字音量不为0
    }
    
    PRINT_INFO_INT("vol_hard_da",vol_hard_da);   
    while(set_pa_volume(vol_hard_pa, vol_hard_da) != 0)
    {
        sys_os_time_dly(10);
    }    
}

//小米模型DAC输出 音量处理
static void _set_ao_source_dac_sm(int tab_index, uint8 vol_to)
{
    uint32 vol_hard_pa_to, vol_hard_da_to;  
    uint32 vol_hard_pa_tmp, vol_hard_da_tmp;
    int db;
    
    vol_hard_pa_to = g_hard_volume_table_sm[tab_index].vol_pa - sys_comval->volume_gain;
    vol_hard_da_to = g_hard_volume_table_sm[tab_index].vol_da;

    //音量限制处理
    vol_hard_pa_tmp = g_hard_volume_table_sm[0].vol_pa - sys_comval->volume_gain;
    vol_hard_da_tmp = g_hard_volume_table_sm[0].vol_da;

    db = ((int16) vol_hard_da_to - (int16) vol_hard_da_tmp) * 15 / 4;
    
    //音量限制处理
    if (db > sys_comval->volume_limit)
    {
        vol_hard_da_to = vol_hard_da_to - (uint32)((db - sys_comval->volume_limit) * 4 / 15);
    }

    _calc_keytone_precut_inner_pa(vol_to, vol_hard_pa_to, vol_hard_da_to);
    
    _set_phy_volume_inner_pa_sm(vol_hard_pa_to, vol_hard_da_to);
}

static void _set_phy_volume_inner_pa_sm(uint32 vol_hard_pa_to, uint32 vol_hard_da_to)
{
    uint32 vol_hard_pa, vol_hard_da;
    int db;

    vol_hard_pa = vol_hard_pa_to;
    vol_hard_da = vol_hard_da_to;

#if ((CASE_LINEIN_CHANNEL_SEL == 1) && (CASE_FM_CHANNEL_SEL == 1))

    db = ((int16) vol_hard_da_to - (int16) g_hard_volume_table_sm[0].vol_da) * 15 / 4;
    sys_comval->volume_relative = (int16)db;

    if (FALSE == sys_comval->dae_cfg.bypass)
    {
        com_set_dae_config_dynamic();
        
        vol_hard_da = mdrc_comp_handle(vol_hard_da);        
    }

#endif

    PRINT_INFO_INT("vol_hard_da_standard_mode",vol_hard_da);

    while(set_pa_volume(vol_hard_pa, vol_hard_da) != 0)
    {
        sys_os_time_dly(10);
    }
}

uint32 __section__(".BANK46") mdrc_comp_handle(uint32 vol_hard_da)
{
        
    uint32 vol_hard_da_temp = 0;

    vol_hard_da_temp = vol_hard_da;
         
    if (sys_comval->dae_cfg.mdrc_enable_standard_mode == 1)
    {
        int16 compressor_threshold;
        int16 vol_offset, tmp_vol_offset;

        vol_offset = 0;

        vol_offset += sys_comval->mdrc_vol_adjust_standard_mode;            

        if (vol_offset > 0)
        {
            vol_hard_da_temp += (uint32)(vol_offset * 4 / 15);
        }
        else
        {
            vol_hard_da_temp -= (uint32)(((int16) 0 - vol_offset) * 4 / 15);
        }

        //保证PA音量递增
        tmp_vol_offset = vol_offset * 4 / 15;
        tmp_vol_offset = tmp_vol_offset * 15 / 4;

        if (vol_offset != tmp_vol_offset)
        {
            vol_hard_da_temp++;
        }

        if (vol_hard_da_temp > 0xbf)
        {
            vol_hard_da_temp = 0xbf;
        }
    }
   
    return vol_hard_da_temp;
}


/************按下音量调节音量时，实际调节的是信号大小**************
 ***********需要算法的支持，方案中只有使用WAVES音效时才会用*******/
static void adjust_input_signal(uint8 aout_mode) 
{
    uint8  tab_index = 0;
    uint32 vol_hard_da = 0;
    uint16 w_reg_val = 0;
    
    //为避免按键音太大，对按键音做衰减
    sys_comval->mdrc_percent = sys_comval->volume_current + 2;

    switch (aout_mode)
    {
        case AO_SOURCE_I2S: 
        {
            if (0 == sys_comval->volume_current)
            {
                ccd_i2s_pa_set_volume(0x03ff);   
            }
            else
            {       
                if (TTS_STATUS_PLAYING == g_tts_play_info.status)
                {
                    //避免TTS音量过大
                    tab_index = 31 - sys_comval->volume_current;
                    w_reg_val = g_hard_volume_table_i2s_pa[tab_index];
                }
                else
                {
                    w_reg_val = 0x00aa;
                }
                
                ccd_i2s_pa_set_volume(w_reg_val);
            }
        }
        break;

        //internel dac & i2s or spdif and external dac both chanel
        case AO_SOURCE_DACI2S: //i2s + internel dac
        case AO_SOURCE_DACSPDIF: //spdif +internel dac
        default:
        {
            if (0 == sys_comval->volume_current)
            {
                while(set_pa_volume(VOL_HARD_PA, 0) != 0)
                {
                    sys_os_time_dly(10);
                }   
            }
            else
            {       
                if (TTS_STATUS_PLAYING == g_tts_play_info.status)
                {
                    //避免TTS音量过大
                    tab_index = 31 - sys_comval->volume_current;
                    vol_hard_da = g_hard_volume_table[tab_index].vol_da;
                }
                else
                {
                    vol_hard_da = 0xbf;
                }
                
                while(set_pa_volume(VOL_HARD_PA, vol_hard_da) != 0)
                {
                    sys_os_time_dly(10);
                }
            }
        }
        break;
    }  
     
    sys_disable_mem_use(0x37000,0x37000 + 0x650);
    waves_set_effect_param(SET_WAVES_INPUT_PARAM, &sys_comval->volume_current);
    sys_comval->dae_cfg.volume_current = sys_comval->volume_current;
    __set_dae_config(&(sys_comval->dae_cfg));
}

/*! \endcond */

