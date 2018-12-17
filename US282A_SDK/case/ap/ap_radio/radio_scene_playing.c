#include "app_radio.h"

void radio_engine_kill(uint8 eg_id)
{
    msg_apps_t msg;
    //注销music engine
    msg.type = MSG_KILL_APP_SYNC;
    msg.content.data[0] = eg_id;
    //发送同步消息
    send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
}

bool radio_engine_create(uint8 eg_id)
{
    //消息返回
    msg_reply_t msg_reply;
    msg_apps_t msg;
    //创建radio 后台引擎进程
    msg.type = MSG_CREAT_APP_SYNC;
    msg.content.data[0] = eg_id;
    //发送同步消息
    return (send_sync_msg(APP_ID_MANAGER, &msg, &msg_reply, 0) == ERR_NO_ERR);

}

//引擎初始化
bool radio_engine_init(void)
{
    engine_type_e engine_type;
    bool result = TRUE;
    bool need_init = FALSE;

    //获取当前后台引擎类型
    engine_type = get_engine_type();

    //当前未安装引擎，需安装
    if (engine_type != ENGINE_RADIO)
    {
        if (engine_type != ENGINE_NULL)
        {
            radio_engine_kill(APP_ID_THEENGINE);
        }

        //创建radio 后台引擎进程
        //实际只执行了fmdrv_init(即提供了FM模块的时钟)
        result = radio_engine_create(APP_ID_FMENGINE);
        if (result == TRUE)
        {
            //需初始化模组
            need_init = TRUE;
        }
    }

    if (need_init == TRUE)
    {
        radio_set_mute(SetMUTE);
        result = radio_modul_open((uint8) g_radio_config.band_mode);
    }

    if (result == TRUE)
    {
        //获取当前频点
        radio_get_freq();
    }

    return result;
}

//播放场景初始化
bool scene_play_init(void)
{
    bool bret;

    //安装引擎,初始化fm模组
    bret = radio_engine_init();
    if (bret == FALSE)
    {
        return FALSE;
    }

    //radio_set_mute(SetMUTE);
    //sys_os_time_dly(10);
    //com_set_mute(FALSE);//解除静音

    //设置音量
    com_reset_sound_volume(0);

    //进入播放场景，初始化频率表指针
    init_point_freq();

    return TRUE;
}


uint8 fm_eg_channel_sel = 0xff;

//播放场景
app_result_e radio_scene_playing(void)
{
    app_result_e loop_deal_ret = RESULT_NULL;
    bool bret;
    bool need_reset_freq = FALSE;
    if(fm_eg_channel_sel == 0xFF)
    {
       fm_eg_channel_sel = (uint8) com_get_config_default(FMENGINE_CHANNEL_SEL);
    }
    bool no_need_play;

    no_need_play = g_standby_exit_flag;

    //AA通道时要关掉按键音
    if (fm_eg_channel_sel == 0)
    {
        keytone_set_on_off(FALSE);
    }

    bret = scene_play_init();
    if (bret == FALSE)
    {
#if(SUPPORT_LED_DRIVER == 1)
        if (g_comval.support_led_display == 1)
        {
            //显示Error，等待2秒钟，期间有按键就提前退出到下一个应用，且可以继续响应按键
            show_error(); //仅显示，没有其他任何副作用；这种方法要慎用，它会破坏由视图管理器刷新的UI
            com_app_sleep(2000); //延时，显示错误，任意按键返回
        }

        return RESULT_NEXT_FUNCTION;
#endif
    }

    //for RCP, 同步电台频点，必须在第一次com_tts_state_play之前做，否则APK会获取到错误的频点
    if (g_radio_config.FMStatus.freq != g_engine_status.FM_CurrentFreq)
    {
        need_reset_freq = TRUE;
    }
    //使用radio_get_freq会获取到错误的频点
    g_engine_status.FM_CurrentFreq = g_radio_config.FMStatus.freq;

    //创建主视图
    radio_play_create_main_view();

    //tts
    if (g_first_enter_play == TRUE)
    {
        g_radio_init_flag = TRUE;

        if ((g_ap_switch_var.call_background_status != CALL_BACK_STATUS_BACK) && (g_standby_exit_flag == FALSE))
        {
            com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_ENTER_RADIO);

            if (g_comval.support_bt_inback == 1)
            {
                //创建蓝牙后台
                com_btmanager_init(FALSE);
            }
            else
            {
                //杀死蓝牙后台
                com_btmanager_exit(FALSE, TRUE);
            }

            //等待TTS结束
            if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
            {
                sys_os_time_dly(10); //等待100ms，以便短按抬起能够完成
            }
        }

        while (1)
        {
            loop_deal_ret = com_view_loop();
            if (loop_deal_ret == RESULT_NONE_EVENT)
            {
                loop_deal_ret = RESULT_NULL;
                break;
            }
            else if (loop_deal_ret > RESULT_COMMON_RESERVE)
            {
                goto radio_playing_exit;
            }
            else
            {
                ; //nothing
            }
        }

        g_radio_init_flag = FALSE;
    }

    if (g_ap_switch_var.switch_to_radio_by_apkapp == TRUE)
    {
        g_neednot_freq_tts_play = TRUE;
        g_ap_switch_var.switch_to_radio_by_apkapp = FALSE;
    }

    if (g_radio_config.FMStatus.station > 0)
    {
        //刷新电台号
        radio_station_create_msg_view(2000);
    }
    else
    {
        radio_freq_tts_play(g_radio_config.FMStatus.freq);
    }

    g_neednot_freq_tts_play = FALSE;

    //当前获取的频点和要设置的频点不一致时，都需要强制设置频点
    if (need_reset_freq == TRUE)
    {
        //更新FM的播放状态
        g_playing_flag = TRUE;
        
        //选择正在播放进入, 不重设频点, 避免出现一下短暂杂音
        radio_set_freq(g_radio_config.FMStatus.freq);

        need_reset_freq = FALSE;
    }
    //for RCP, 同步电台频点
    radio_get_freq();

    radio_connect_ain();

    if (no_need_play == 0)
    {
        radio_set_mute(releaseMUTE);
        g_playing_flag = TRUE;
    }
    else 
    {
    	  g_playing_flag = FALSE;
    }

    g_searching_flag = FALSE;
    loop_deal_ret = playing_loop_deal();
    radio_playing_exit: 
    if((loop_deal_ret >= RESULT_APP_QUIT) && (loop_deal_ret < RESULT_AUTO_SEARCH))
    {
        com_set_mute(TRUE); //静音
    }
    return loop_deal_ret;
}

