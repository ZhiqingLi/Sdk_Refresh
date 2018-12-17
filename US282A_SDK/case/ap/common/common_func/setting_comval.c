/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 系统配置项默认值加载接口。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"
#include "vm_def.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    从config.bin中读取默认设置
 * \param[in]    setting_comval 公共配置信息结构体指针
 * \param[out]   none
 * \return       none
 * \ingroup      misc_func
 * \note
 *******************************************************************************/
void com_setting_comval_init(comval_t *setting_comval)
{
    sys_comval = setting_comval;

    sys_vm_read(setting_comval, VM_AP_SETTING, sizeof(comval_t));

    if (setting_comval->magic == VRAM_MAGIC(VM_AP_SETTING))
    {
        if(g_app_info_state.stub_tools_type != 0)
        {
            //调试模式不支持led显示
            setting_comval->support_led_display = FALSE;

            //调试模式不保存setting VRAM，每次都从配置项读取
            setting_comval->magic = 0x0;
            
            if(g_app_info_state.stub_tools_type == STUB_PC_TOOL_ATT_MODE)
            {
                setting_comval->dae_cfg.enable = FALSE;  
            }        
        }

        return;
    }

    setting_comval->magic = VRAM_MAGIC(VM_AP_SETTING);

    //应用配置
    setting_comval->support_cardplay = (uint8) com_get_config_default(SETTING_APP_SUPPORT_CARDPLAY);
    setting_comval->support_uhostplay = (uint8) com_get_config_default(SETTING_APP_SUPPORT_UHOSTPLAY);
    setting_comval->support_linein = (uint8) com_get_config_default(SETTING_APP_SUPPORT_LINEIN);
    setting_comval->support_fmradio = (uint8) com_get_config_default(SETTING_APP_SUPPORT_FMRADIO);
    setting_comval->support_bt_inback = (uint8) com_get_config_default(SETTING_APP_SUPPORT_BT_INBACK);
    setting_comval->support_btplay = (uint8) com_get_config_default(SETTING_APP_SUPPORT_BTPLAY);
    setting_comval->support_btcall = (uint8) com_get_config_default(SETTING_APP_SUPPORT_BTCALL);
    setting_comval->support_usbbox = (uint8) com_get_config_default(SETTING_APP_SUPPORT_USBBOX);
    setting_comval->support_usbmsc = (uint8) com_get_config_default(SETTING_APP_SUPPORT_USBMSC);
    //setting_comval->support_ota = (uint8) com_get_config_default(SETTING_APP_SUPPORT_OTA);
    sys_get_fw_info(&setting_comval->support_ota, FW_INFO_OTA_MODE_ADDR, FW_INFO_OTA_MODE_LEN);
    
    setting_comval->support_ota_song_test = (uint8) com_get_config_default(SETTING_APP_SUPPORT_OTA_SONG_TEST);

    

    //低功耗和省电关机
    setting_comval->lowpower_mode  = (uint8) com_get_config_default(SETTING_ONOFF_LOWPOWER_MODE);
    setting_comval->lowpower_timer = (uint8) com_get_config_default(SETTING_ONOFF_LOWPOWER_TIMER);
    setting_comval->poweroff_timer = (uint8) com_get_config_default(SETTING_ONOFF_POWEROFF_TIMER);
    setting_comval->poweroff_bt_waitpair_timer = (uint8) com_get_config_default(
            SETTING_ONOFF_POWEROFF_BT_WAITPAIR_TIMER);

    //双击按键
    setting_comval->double_click_enable = (uint8) com_get_config_default(SETTING_INPUT_ENABLE_DOUBLE_CLICK);
    setting_comval->double_click_delay = (uint16) com_get_config_default(SETTING_INPUT_DOUBLE_CLICK_DELAY_TIMER);

    //按键音配置项
    setting_comval->key_tone = (uint8) com_get_config_default(SETTING_SOUND_ENABLE_KEYTONE);
    setting_comval->key_tone_overlay = (uint8) com_get_config_default(SETTING_SOUND_ENABLE_KEYTONE_OVERLAY);
    //TTS配置项
    setting_comval->tts_enable = (uint8) com_get_config_default(SETTING_SOUND_ENABLE_TTS);
    setting_comval->tts_language = (uint8) com_get_config_default(SETTING_SOUND_TTS_LANGUAGE);
    setting_comval->kt_tts_min_volume = (uint8) com_get_config_default(SETTING_SOUND_KT_TTS_MIN_VOLUME);
    setting_comval->kt_tts_max_volume = (uint8) com_get_config_default(SETTING_SOUND_KT_TTS_MAX_VOLUME);

    //音量配置项
    setting_comval->volume[VOLUME_MODE_NORMAL] = (uint8) com_get_config_default(SETTING_SOUND_DEFAULT_MAIN_VOLUME);
    setting_comval->volume[VOLUME_MODE_CALL] = (uint8) com_get_config_default(SETTING_SOUND_DEFAULT_CALL_VOLUME);
    setting_comval->volume_mode = VOLUME_MODE_NORMAL;
    setting_comval->volume_current = setting_comval->volume[VOLUME_MODE_NORMAL];
    setting_comval->volume_gain = (uint8) com_get_config_default(SETTING_SOUND_MAIN_VOLUME_GAIN);

    //显示配置项
    setting_comval->support_led_display = (uint8) com_get_config_default(SETTING_DISPLAY_SUPPORT_LED_SEGMENT);
    setting_comval->light_timer = (uint8) com_get_config_default(SETTING_DISPLAY_LED_SEGMENT_LIGHT_TIMER);

    //电池充电
    setting_comval->bat_charge_mode = (uint8) com_get_config_default(SETTING_BAT_CHARGE_MODE);
    setting_comval->bat_charge_full_tts = (uint8) com_get_config_default(SETTING_BAT_CHARGE_FULL_TTS);

    setting_comval->snooze_timer = (uint8) com_get_config_default(SETTING_TIMEALARM_SNOOZE_TIMER);
    setting_comval->snooze_max = (uint8) com_get_config_default(SETTING_TIMEALARM_SNOOZE_MAX_TIMES);
    setting_comval->alarm_overtime = (uint8) com_get_config_default(SETTING_TIMEALARM_ALARM_OVERTIME);

    setting_comval->hard_support_card = (uint8) com_get_config_default(SETTING_HARDWARE_SUPPORT_CARD);
    setting_comval->hard_support_uhost = (uint8) com_get_config_default(SETTING_HARDWARE_SUPPORT_UHOST);
    setting_comval->hard_support_headphone = (uint8) com_get_config_default(SETTING_HARDWARE_SUPPORT_HEADPHONE);
    setting_comval->stub_enable = (uint8) com_get_config_default(SETTING_STUB_TEST_DEBUG_ENABLE);
    setting_comval->stub_print = (uint8) com_get_config_default(SETTING_STUB_PRINT_ENABLE);

    setting_comval->default_vbass_enable = (uint8) com_get_config_default(DAE_VIRTUAL_BASS_ENABLE);
    
    //setting_comval->bt_play = 0;
    //DAE配置项
    com_load_dae_config(setting_comval);

    if(g_app_info_state.stub_tools_type != 0)
    {
        //调试模式不支持led显示
        setting_comval->support_led_display = FALSE;

        //调试模式不保存setting VRAM，每次都从配置项读取
        setting_comval->magic = 0x0;
        
        if(g_app_info_state.stub_tools_type == STUB_PC_TOOL_ATT_MODE)
        {
            setting_comval->dae_cfg.enable = FALSE;  
        }        
    }  

    //用config.bin的默认值写VRAM
    sys_vm_write(setting_comval, VM_AP_SETTING, sizeof(comval_t));
}

void com_enable_jtag_function(uint32 group_number)
{
    if(group_number == 3)
    {
        //for ATS2825,默认第三组    
        act_writel(act_readl(MFP_CTL2) & (~0x30000000), MFP_CTL2); 
        act_writel(act_readl(MFP_CTL2) | (0xa0000000), MFP_CTL2); 
    }
    else if(group_number == 4)
    {
        //for ATS2823,默认第四组
        act_writel(act_readl(AD_Select1) & (~0x06), AD_Select1);
        act_writel(act_readl(MFP_CTL2) & (~0x30000000), MFP_CTL2);
        act_writel(act_readl(MFP_CTL2) | (0xb0000000), MFP_CTL2); 
    }
    else
    {
        ;//nothing for QAC
    }
}

/*! \endcond */
