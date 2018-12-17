/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用系统消息默认处理，前台应用自己定义的系统消息处理优先于系统消息
 *       默认处理。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_func.h"

app_result_e com_sys_deal_uhost_in(void *ev_param);
app_result_e com_sys_deal_uhost_out(void *ev_param);
app_result_e com_sys_deal_sdcard_in(void *ev_param);
app_result_e com_sys_deal_sdcard_out(void *ev_param);
app_result_e com_sys_deal_linein_in(void *ev_param);
app_result_e com_sys_deal_linein_out(void *ev_param);
app_result_e com_sys_deal_rtc_alarm(void *ev_param);
app_result_e com_sys_deal_usb_in(void *ev_param);
app_result_e com_sys_deal_lowpower(void *ev_param);
app_result_e com_sys_deal_full_charge(void *ev_param);
app_result_e com_sys_deal_power_off(void *ev_param);
app_result_e com_sys_deal_idle_mode(void *ev_param);
app_result_e com_sys_deal_system_enter_s2(void *ev_param);
app_result_e com_sys_deal_system_enter_s3(void *ev_param);
app_result_e com_sys_deal_hfp_call(void *ev_param);
app_result_e com_sys_deal_app_quit(void *ev_param);
app_result_e com_sys_deal_headphone_in(void *ev_param);
app_result_e com_sys_deal_headphone_out(void *ev_param);
app_result_e com_sys_deal_enter_btplay(void *ev_param);
app_result_e com_sys_deal_enter_music(void *ev_param);
app_result_e com_sys_deal_enter_card_music(void *ev_param);
app_result_e com_sys_deal_enter_uhost_music(void *ev_param);
app_result_e com_sys_deal_enter_linein(void *ev_param);
app_result_e com_sys_deal_enter_radio(void *ev_param);
app_result_e com_sys_deal_enter_alarm(void *ev_param);
app_result_e com_sys_deal_enter_record_cplay(void *ev_param);
app_result_e com_sys_deal_enter_record_uplay(void *ev_param);
app_result_e com_sys_deal_enter_crecord(void *ev_param);
app_result_e com_sys_deal_enter_urecord(void *ev_param);
app_result_e com_sys_deal_enter_usound(void *ev_param);
app_result_e com_sys_install_stub(void *ev_param);
app_result_e com_sys_deal_btstack_hd_err(void *ev_param);
app_result_e com_att_switch_app(void *ev_param);
app_result_e com_tws_switch_app(void *ev_param);
app_result_e com_mpu_error_deal(void *ev_param);

const sys_event_map_t __section__(".rodata.se_maplist") com_se_maplist[] =
{
    { { MSG_UH_IN, SYSMSG_STOP_TTS_YES }, com_sys_deal_uhost_in },
    { { MSG_UH_OUT, 0 }, com_sys_deal_uhost_out },
    { { MSG_SD_IN, SYSMSG_STOP_TTS_YES }, com_sys_deal_sdcard_in },
    { { MSG_SD_OUT, 0 }, com_sys_deal_sdcard_out },
    { { MSG_LINEIN_IN, SYSMSG_STOP_TTS_YES }, com_sys_deal_linein_in },
    { { MSG_LINEIN_OUT, 0 }, com_sys_deal_linein_out },
    { { MSG_RTCALARM, SYSMSG_STOP_TTS_YES }, com_sys_deal_rtc_alarm },
    { { MSG_USB_STICK, SYSMSG_STOP_TTS_YES }, com_sys_deal_usb_in },
    { { MSG_LOW_POWER, SYSMSG_STOP_TTS_YES }, com_sys_deal_lowpower },
    { { MSG_FULL_CHARGE, SYSMSG_STOP_TTS_YES }, com_sys_deal_full_charge },
    { { MSG_POWER_OFF, SYSMSG_STOP_TTS_YES }, com_sys_deal_power_off },
    { { MSG_IDLE_MODE, SYSMSG_STOP_TTS_YES }, com_sys_deal_idle_mode },
    { { MSG_SYSTEM_ENTER_S2, SYSMSG_STOP_TTS_YES }, com_sys_deal_system_enter_s2 },
    { { MSG_SYSTEM_ENTER_S3, SYSMSG_STOP_TTS_YES }, com_sys_deal_system_enter_s3 },
    { { MSG_BT_HFP_CALL, SYSMSG_STOP_TTS_YES }, com_sys_deal_hfp_call },
    { { MSG_APP_QUIT, SYSMSG_STOP_TTS_YES }, com_sys_deal_app_quit },
    { { MSG_HP_IN, 0 }, com_sys_deal_headphone_in },
    { { MSG_HP_OUT, 0 }, com_sys_deal_headphone_out },
    { { EVENT_ENTER_BTPLAY, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_btplay },
    { { EVENT_ENTER_MUSIC, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_music },
    { { EVENT_ENTER_MUSIC_CARD, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_card_music },
    { { EVENT_ENTER_MUSIC_UDISK, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_uhost_music },
    { { EVENT_ENTER_LINEIN, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_linein },
    { { EVENT_ENTER_RADIO, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_radio },
    { { EVENT_ENTER_ALARM_RCP, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_alarm },
    { { EVENT_RECORD_CPLAY_RCP, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_record_cplay },
    { { EVENT_RECORD_UPLAY_RCP, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_record_uplay },
    { { EVENT_ENTER_CRECORD_RCP, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_crecord },
    { { EVENT_ENTER_URECORD_RCP, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_urecord },
    { { EVENT_ENTER_USOUND, SYSMSG_STOP_TTS_YES }, com_sys_deal_enter_usound },
    { { MSG_AUTOTEST_BTT_TEST_SYNC, SYSMSG_STOP_TTS_YES},  com_sys_install_stub},
    { { MSG_BTSTACK_ERR_HARDWARE, SYSMSG_STOP_TTS_YES}, com_sys_deal_btstack_hd_err},
    { { MSG_AUTOTEST_SWITCH_APP, SYSMSG_STOP_TTS_YES}, com_att_switch_app},
    { { MSG_BTSTACK_TWS_APSWITCH_SYNC, SYSMSG_STOP_TTS_YES}, com_tws_switch_app},
    { { MSG_MPU_ERROR, SYSMSG_STOP_TTS_YES}, com_mpu_error_deal},
    /*! 结束标志 */
    { { MSG_NULL, 0 }, NULL },
};

app_result_e com_sys_deal_uhost_in(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if ((sys_comval->hard_support_uhost == 1) && (sys_comval->support_uhostplay == 1))
    {
        //进入MUSIC应用
        result = RESULT_MUSIC_UPLAY;
    }

    return result;
}

app_result_e com_sys_deal_uhost_out(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    //已经在MUSIC中处理

    return result;
}

app_result_e com_sys_deal_sdcard_in(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if ((sys_comval->hard_support_card == 1) && (sys_comval->support_cardplay == 1))
    {
        //进入MUSIC应用
#if (SUPPORT_RING_FOLDER == 1)
        if ((int8) sys_detect_disk(DRV_GROUP_STG_CARD) != -1)
        {
#ifdef __ESD_MODE_
            //g_ap_switch_var.result_alarm_ring_scan_backup = RESULT_MUSIC_CPLAY;
            //result = RESULT_ALARM_RING_FOLDER_SCAN;
#else
            g_ap_switch_var.result_alarm_ring_scan_backup = RESULT_MUSIC_CPLAY;
            result = RESULT_ALARM_RING_FOLDER_SCAN;
#endif
        }
#else

#ifdef __ESD_MODE_
        //result = RESULT_MUSIC_CPLAY;
#else
        result = RESULT_MUSIC_CPLAY;
#endif

#endif
    }

    return result;
}

app_result_e com_sys_deal_sdcard_out(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    //已经在MUSIC中处理

    return result;
}

app_result_e com_sys_deal_linein_in(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (sys_comval->support_linein == 1)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        if((g_bt_stack_cur_info.dev_role != 2)&&(get_cur_func_id() != APP_FUNC_PLAYLINEIN))
#endif
        {
            //进入MUSIC应用
            result = RESULT_ENTER_LINEIN;
        }    
       
    }

    return result;
}

app_result_e com_sys_deal_linein_out(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    //已经在LINEIN中处理

    return result;
}

app_result_e com_sys_deal_rtc_alarm(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    //响应闹钟
    result = tm_alarm_alarm_ring_deal(ALARM_RING_ENTER);

    return result;
}

app_result_e com_sys_deal_usb_in(void *ev_param)
{
    app_result_e result;

    //进入USB音箱应用
    if (get_apk_available() == TRUE)
    {
        result = com_rcp_dialog(DIALOG_BUTTON_INCLUDE, BUTTON_USBSOUND_CHARGE, DIALOG_MSG_USB_STICK, 1,
                com_dialog_check_usbdialog);
        if (result == RESULT_DIALOG_USB_SOUND)
        {
            if (get_cable_state() == CABLE_STATE_CABLE_IN)
            {
                result = RESULT_USB_SOUND;
            }
            else
            {
                result = RESULT_NULL;
            }
        }
        else if ((result == RESULT_DIALOG_ONLY_CHARGE) || (result == RESULT_DIALOG_CANCEL_AUTO))
        {
            result = RESULT_NULL;
        }
        else
        {
            //其他返回结果照样返回
        }
    }
    else
    {
#ifdef __ESD_MODE_
        result = RESULT_NULL;
#else
        result = RESULT_USB_SOUND;
#endif
    }

    return result;
}

app_result_e com_sys_deal_lowpower(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (g_app_info_state.bat_value == 1)
    {
        //电量低，请充电
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        if(g_bt_stack_cur_info.dev_role!=0)
        {
            g_app_info_state_all.g_send_exit_mmm_tts=1;
        }
#endif
        com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_USEFIFO, (void *) TTS_BAT_LOW_CHARGE);
    
    }
    else
    {
        //闪屏4次
        if (sys_comval->support_led_display == 1)
        {
            uint8 twinkling_cnt;

            for (twinkling_cnt = 0; twinkling_cnt < 8; twinkling_cnt++)
            {
                if ((twinkling_cnt % 2) == 0)
                {
                    //清除led屏
                    led_flash_screen(0);
                }
                else
                {
                    //恢复led内容
                    led_flash_screen(1);
                }
                sys_os_time_dly(50);
            }
        }

        result = RESULT_LOW_POWER;
    }

    return result;
}

app_result_e com_sys_deal_full_charge(void *ev_param)
{
    //充电已满
    if (sys_comval->bat_charge_full_tts == 1)
    {
        com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_USEFIFO, (void *) TTS_CHARGE_FULL);
    }
    PRINT_INFO("charge full!");

    return RESULT_NULL;
}

app_result_e com_sys_deal_power_off(void *ev_param)
{
    return RESULT_POWER_OFF;
}

app_result_e com_sys_deal_idle_mode(void *ev_param)
{
    return RESULT_IDLE_MODE;
}

app_result_e com_sys_deal_system_enter_s2(void *ev_param)
{
    app_result_e result = RESULT_NULL;
    uint32 wake_type = 0;

    wake_type = wake_type
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
              | WAKE_TYPE_I2C_KEY
#endif
#else
              | WAKE_TYPE_ONOFF_KEY
              | WAKE_TYPE_PHY_KEY
#endif
#if 0 //默认不支持红外遥控唤醒，否则必须一直跑高频
              | WAKE_TYPE_IR_KEY
#endif
#if 0 //默认不支持插拔AUX和插拔卡唤醒
              | WAKE_TYPE_AUX_DETECT
              | WAKE_TYPE_CARD_DETECT
#endif
              | WAKE_TYPE_BT_RX;

    result = com_standby(wake_type);

    return result;
}

app_result_e com_sys_deal_system_enter_s3(void *ev_param)
{
    sys_disable_use_temp_pool();
   
    sys_base_set_enter_s3bt_scene();
    
    return RESULT_SYSTEM_ENTER_S3;
}

app_result_e com_sys_deal_hfp_call(void *ev_param)
{
    if (g_this_app_info->app_id != APP_ID_BTCALL)
    {
        return RESULT_BLUETEETH_CALL_BACKGROUND;
    }
    else
    {
        return RESULT_NULL; //用来终止来电电话本播报TTS
    }
}

app_result_e com_sys_deal_app_quit(void *ev_param)
{
    return RESULT_APP_QUIT;
}

app_result_e com_sys_deal_headphone_in(void *ev_param)
{
    SPEAKER_OUT_SWITCH(TRUE);

    return RESULT_NULL;
}

app_result_e com_sys_deal_headphone_out(void *ev_param)
{
    SPEAKER_OUT_SWITCH(FALSE);

    return RESULT_NULL;
}

app_result_e com_sys_deal_enter_btplay(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_BTPLAY)
    {
        result = RESULT_BLUETEETH_SOUND;
    }

    return result;
}

app_result_e com_sys_deal_enter_music(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if ((get_cur_func_id() != APP_FUNC_PLAYUHOST) && (get_cur_func_id() != APP_FUNC_PLAYCARD))
    {
        if ((int8) sys_detect_disk(DRV_GROUP_STG_UHOST) != (int8) (-1))
        {
            result = RESULT_MUSIC_UPLAY;
        }
        else if ((int8) sys_detect_disk(DRV_GROUP_STG_CARD) != (int8) (-1))
        {
            result = RESULT_MUSIC_CPLAY;
        }
        else
        {
            //do nothing for QAC
        }
    }

    return result;
}

app_result_e com_sys_deal_enter_card_music(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_PLAYCARD)
    {
        if ((int8) sys_detect_disk(DRV_GROUP_STG_CARD) != (int8) (-1))
        {
            result = RESULT_MUSIC_CPLAY;
        }
    }

    return result;
}

app_result_e com_sys_deal_enter_uhost_music(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_PLAYUHOST)
    {
        if ((int8) sys_detect_disk(DRV_GROUP_STG_UHOST) != (int8) (-1))
        {
            result = RESULT_MUSIC_UPLAY;
        }
    }

    return result;
}

app_result_e com_sys_deal_enter_linein(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_PLAYLINEIN)
    {
        result = RESULT_ENTER_LINEIN;
    }

    return result;
}

app_result_e com_sys_deal_enter_radio(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_RADIO)
    {
        g_ap_switch_var.switch_to_radio_by_apkapp = TRUE;
        result = RESULT_RADIO;
    }

    return result;
}

app_result_e com_sys_deal_enter_alarm(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    g_ap_switch_var.apk_alarm_flag = 1;
    g_ap_switch_var.apk_alarm_last_func = get_cur_func_id();
    g_ap_switch_var.apk_alarm_backup_cb = (void *) g_app_rcp_cmd_cb_tbl;
    g_ap_switch_var.apk_alarm_backup_global_cb = (void *) g_app_get_global_info;
    com_rcp_set_callback(NULL, NULL);

#if (SUPPORT_LED_DRIVER == 1)
    if (sys_comval->support_led_display == 1)
    {
        tm_alarm_create_clock_view();
    }
#endif

    return result;
}

app_result_e com_sys_deal_enter_record_cplay(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_PLAYCRECORD)
    {
        result = RESULT_RECORD_CPLAY;
    }

    return result;
}

app_result_e com_sys_deal_enter_record_uplay(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_PLAYURECORD)
    {
        result = RESULT_RECORD_UPLAY;
    }

    return result;
}

app_result_e com_sys_deal_enter_crecord(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_MICREC_TOC)
    {
        result = RESULT_RECORD_MIC_TOC;
    }

    return result;
}

app_result_e com_sys_deal_enter_urecord(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_MICREC_TOU)
    {
        result = RESULT_RECORD_MIC_TOU;
    }

    return result;
}

app_result_e com_sys_deal_enter_usound(void *ev_param)
{
    app_result_e result = RESULT_NULL;

    if (get_cur_func_id() != APP_FUNC_USOUND)
    {
        result = RESULT_USB_SOUND;
    }

    return result;
}

app_result_e com_sys_deal_btstack_hd_err(void *ev_param)
{
    //先卸载掉BT STACK，并断电
    com_btmanager_exit(FALSE, TRUE);

    //再装载BT STACK
    g_need_auto_connect_timeout = 1;
    g_neednot_tts_play_reset = 1;
    com_btmanager_init(FALSE);
    g_need_auto_connect_timeout = 0;

    return RESULT_NULL;
}

app_result_e com_att_switch_app(void *ev_param)
{
    uint32 ret_val;
    uint32 app_func = *(uint32 *)ev_param;  

    if(app_func == APP_ATT_FUNC_ID_BTPLAY)
    {
        ret_val = RESULT_BLUETEETH_SOUND;
    }
    else if(app_func == APP_ATT_FUNC_ID_BTCALL)
    {
        ret_val = RESULT_BLUETEETH_CALL;
    }
    else if(app_func == APP_ATT_FUNC_ID_CARDPLAY)
    {
        ret_val = RESULT_MUSIC_CPLAY;
    }
    else if(app_func == APP_ATT_FUNC_ID_LINEIN)
    {
        ret_val = RESULT_ENTER_LINEIN;
    }
    else if(app_func == APP_ATT_FUNC_ID_RADIO)
    {
        ret_val = RESULT_RADIO;
    }
    else
    {
        ret_val = RESULT_NULL;
    }

    return ret_val;
}

app_result_e com_tws_switch_app(void *ev_param)
{
    uint8 next_func_id;

    next_func_id = com_ap_switch_ask_next_func(RESULT_NEXT_FUNCTION);
    if (next_func_id == get_cur_func_id())
    {
        PRINT_WARNING("NO next APP to switch!!");
        return RESULT_NULL;
    }
    else
    {
        //过滤来不及响应的短按按键
        com_filter_keymsg_in_queue(g_p_view_ke->val, KEY_TYPE_SHORT_UP);
        return RESULT_NEXT_FUNCTION;
    }    
}

app_result_e com_mpu_error_deal(void *ev_param)
{
    //出现mpu error,表明临时缓冲区有错误，需要先清除临时缓冲区
    sys_clear_temp_pool();

    //出现mpu error后系统会禁止使用临时缓冲区，这里重新使能这部分空间的使用
    sys_enable_use_temp_pool();

    return RESULT_NULL;
}


