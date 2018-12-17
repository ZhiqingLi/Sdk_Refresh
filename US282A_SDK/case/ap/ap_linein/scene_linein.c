/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：LineIn的场景的处理
 * 作者：Gary Wang
 ********************************************************************************/
#include "app_linein.h"

bool linein_close_engine(uint8 engine_id)
{
    bool bret = FALSE;
    msg_apps_t msg;
    //注销后台，后台engine_id
    msg.type = MSG_KILL_APP_SYNC; //MSG_KILL_APP;
    msg.content.data[0] = engine_id;
    //发送同步消息
    if (send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0) == ERR_NO_ERR)
    {
        bret = TRUE;
    }

    return bret;
}

app_result_e _scene_linein_init(void)
{
    app_result_e init_result = RESULT_NULL;
    engine_type_e engine_type;
    msg_apps_t msg;

    msg.content.data[1] = !g_standby_exit_flag;

    //查看当前引擎是否为linein
    engine_type = get_engine_type();

    if ((engine_type != ENGINE_NULL) && (engine_type != ENGINE_LINEIN))
    {
        linein_close_engine(APP_ID_THEENGINE);
    }

    com_set_mute(FALSE); //解除静音
    //语音提示
    if ((g_ap_switch_var.call_background_status != CALL_BACK_STATUS_BACK) && (g_standby_exit_flag == FALSE))
    {
        com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void *) TTS_PLAY_LINEIN);

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

    while (g_standby_exit_flag == 0)
    {
        init_result = com_view_loop();
        if (init_result == RESULT_NONE_EVENT)
        {
            init_result = RESULT_NULL;
            break;
        }
        else if (init_result > RESULT_COMMON_RESERVE)
        {
            return init_result;
        }
        else
        {
            ; //nothing
        }
    }

    g_standby_exit_flag = FALSE;

    //AA通道时要关掉按键音
    if ((uint8) com_get_config_default(LINEIN_CHANNEL_SEL) == 0)
    {
        keytone_set_on_off(FALSE);
    }
    else
    {
	    com_set_dae_chan(TRUE, TRUE);
	}

    //linein引擎已经存在则不需要下列操作
    if (engine_type != ENGINE_LINEIN)
    {
        //打开引擎
        //消息内容
        msg.content.data[0] = APP_ID_LINEIN_EG;
        //消息类型(即消息名称)
        msg.type = MSG_CREAT_APP_SYNC;
        //发送同步消息
        if (send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0) == ERR_NO_ERR)
        {
            //set volume
            com_reset_sound_volume(0);
        }
        else
        {
            init_result = RESULT_NEXT_FUNCTION;
        }
    }
    else
    {
        //play
        linein_play();
        //set volume
        com_reset_sound_volume(0);
        com_set_mute(FALSE);
    }

    return init_result;
}

bool _scene_linein_exit(void)
{
    return TRUE;
}

void linein_ui_display(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 0)
    {
        return;
    }

    //clear screen;
    led_clear_screen();

    //line in
    led_display(NUMBER1, NUM_L, 1);
    led_display(NUMBER2, NUM_I, 1);
    led_display(NUMBER3, NUM_N, 1);
    led_display(NUMBER4, NUM_E, 1);
    //icon
    led_display(LCD_AUX, 0xff, 1);

    //显示播放状态
    if (g_linein_eg_status.play_status == linein_play_status_stop)
    {
        led_display(LCD_PAUSE, 0xff, 1);
        led_display(LCD_PLAY, 0xff, 0);
    }
    else
    {
        led_display(LCD_PLAY, 0xff, 1);
        led_display(LCD_PAUSE, 0xff, 0);
    }
#endif
}

void linein_main_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            linein_ui_display();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            linein_ui_display();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            linein_ui_display();
        }
        break;

        default:
        break;
    }
}

void linein_create_main_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MAIN;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 0;
    create_view_param.ke_maplist = linein_ke_maplist;
    create_view_param.se_maplist = linein_se_maplist;
    create_view_param.view_cb = linein_main_view;
    com_view_add(APP_VIEW_ID_MAIN, &create_view_param);
}

//场景的入口
app_result_e scene_linein(void)
{
    app_result_e ret_vals = RESULT_NULL;

    g_linein_init_flag = TRUE;

    //创建主视图
    linein_create_main_view();

    //场景初始化
    ret_vals = _scene_linein_init();

    g_linein_init_flag = FALSE;

    if (ret_vals > RESULT_COMMON_RESERVE)
    {
        return ret_vals;
    }

    //等待LINEIN状态可用
    while (linein_get_status(&g_linein_eg_status) == FALSE)
    {
        sys_os_time_dly(1);
    }
    
    //common使用
    set_play_disk_type(PLAY_DISK_LINEIN);

    //场景循环
    ret_vals = _linein_loop_deal();

    set_play_disk_type(PLAY_DISK_NULL);

    if ((uint8) com_get_config_default(LINEIN_CHANNEL_SEL) == 1)
    {
	    if (get_engine_type() == ENGINE_LINEIN)
	    {
	        linein_close_engine(APP_ID_LINEIN_EG);
	    }
	
	    //com_set_dae_chan(FALSE, FALSE);
	}

    //调用场景退出函数
    _scene_linein_exit();

    return ret_vals;
}

