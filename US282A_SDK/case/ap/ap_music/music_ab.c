//          AB窗口
//
//1.模式窗口。
//2.任意非PRE/NEXT/PLAY键能关闭它,且这个按键消息被“新的顶层模式窗口”接收。
//  比如V+键将关闭本窗口，同时弹出“音量设置窗口”(模式窗口)，
//  新窗口会处理V+键(音量加1级)。
//
//  如果按下“快进键”(如果有单独的快进键的话)，将关闭本窗口，
//  “播放窗口”成为新的顶层模式窗口，新窗口会处理快进键。
#include "music.h"

#ifdef AB_SUPPORT

MUS_STATIC app_result_e _ab_key_next(void);
MUS_STATIC app_result_e _ab_key_prev(void);
MUS_STATIC app_result_e _ab_key_play(void);

MUS_STATIC const key_event_map_t __section__(".rodata.ke_maplist")
music_ab_key_map_list[] =
{
    { { KEY_NEXT, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _ab_key_next },
    { { KEY_PREV, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _ab_key_prev },
    { { KEY_PLAY, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _ab_key_play },
    { { KEY_PHONEPLAY, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _ab_key_play },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL }, NULL },
};

//闪烁的那个，现在应该灭，还是应该亮
MUS_STATIC bool s_ab_on = TRUE;
MUS_STATIC int8 s_ab_timer_id = -1;

//给方案公司做参考用的代码
#if 0
MUS_STATIC bool _set_ab_count(uint16 count)
{
    bool bret;
    bret = music_send_msg(&count, NULL, MSG_MENGINE_SET_AB_COUNT_SYNC);
    return bret;
}

MUS_STATIC bool _set_ab_gap(uint16 gaptime)
{
    bool bret;
    bret = music_send_msg(&gaptime, NULL, MSG_MENGINE_SET_AB_GAP_SYNC);
    return bret;
}
#endif

MUS_STATIC bool _start_ab(void)
{
    bool bret;
    //如果不设置重复次数，默认的是5次
    //bret = _set_ab_count((uint16)-1);
    bret = music_send_msg(NULL, NULL, MSG_MENGINE_START_AB_SYNC);
    return bret;
}

MUS_STATIC bool _set_a(void)
{
    bool bret;
    bret = music_send_msg(NULL, NULL, MSG_MENGINE_SET_APOINT_SYNC);
    return bret;
}

MUS_STATIC bool _set_b(void)
{
    bool bret;
    bret = music_send_msg(NULL, NULL, MSG_MENGINE_SET_BPOINT_SYNC);
    return bret;
}

MUS_STATIC bool _clear_ab(void)
{
    bool bret = FALSE;
    if (g_music_info.eg_status.ab_status != PlayAB_Null)
    {
        bret = music_send_msg(NULL, NULL, MSG_MENGINE_CLEAR_AB_SYNC);
    }
    return bret;
}

MUS_STATIC app_result_e _ab_key_next(void)
{
    play_get_status(&g_music_info);

    switch (g_music_info.eg_status.ab_status)
    {
        case PlayAB_Null:
        _start_ab(); //变成等待设置A点
        break;
        case PlayAB_A: //等待设置A点
        _set_a(); //变成等待设置B点
        s_ab_on = TRUE;
        break;
        case PlayAB_B: //等待设置B点
        _set_b(); //变成设置完成
        s_ab_on = TRUE;
        break;
        case PlayAB_Do: //A, B都设置好了
        break;
        default:
        break;
    }

    com_view_update(VIEW_ID_AB);

    return RESULT_NULL;
}

MUS_STATIC app_result_e _ab_key_prev(void)
{
    //重新开始设置A点
    _clear_ab();
    _ab_key_next();

    return RESULT_NULL;
}

MUS_STATIC app_result_e _ab_key_play(void)
{
    _clear_ab();
    com_view_remove(VIEW_ID_AB, TRUE);

    return RESULT_NULL;
}

MUS_STATIC void _ab_show(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 0)
    {
        return;
    }

    play_get_status(&g_music_info);

    switch (g_music_info.eg_status.ab_status)
    {
        case PlayAB_Null:
        break;
        case PlayAB_A: //等待设置A点
        //A闪烁
        led_display(NUMBER1, NUM_A, (uint32 )s_ab_on);
        led_display(NUMBER2, NUM_ERROR, TRUE);
        led_display(NUMBER3, NUM_B, TRUE);
        break;
        case PlayAB_B: //等待设置B点
        //显示A-, B闪烁
        led_display(NUMBER1, NUM_A, TRUE);
        led_display(NUMBER2, NUM_ERROR, TRUE);
        led_display(NUMBER3, NUM_B, (uint32 )s_ab_on);
        break;
        case PlayAB_Do: //设置完成
        //显示A-B
        led_display(NUMBER1, NUM_A, TRUE);
        led_display(NUMBER2, NUM_ERROR, TRUE);
        led_display(NUMBER3, NUM_B, TRUE);
        break;
        default:
        break;
    }
#endif
}

MUS_STATIC void _ab_flash(void)
{
    s_ab_on = !s_ab_on;
    com_view_update(VIEW_ID_AB);
}

MUS_STATIC void _cls(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        led_clear_screen();
    }
#endif
}

//视图的生成、更新、销毁处理
MUS_STATIC void _ab_view_handle(view_update_e mode)
{
    _cls();
    switch (mode)
    {
        case VIEW_UPDATE_DESTROY:
        kill_app_timer(s_ab_timer_id);
        s_ab_timer_id = -1;
        _clear_ab();
        break;

        case VIEW_UPDATE_CREATE:
        s_ab_timer_id = set_app_timer(APP_TIMER_ATTRB_UI | (VIEW_ID_AB << 8),
                500, (timer_proc) _ab_flash);
        s_ab_on = TRUE;
        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:
        _ab_show();
        break;
        default:
        break;
    }
}

app_result_e create_view_ab(void)
{
    //创建AB窗口
    create_view_param_t param;

    _clear_ab();
    _ab_key_next(); //start a b
    _ab_key_next(); //设置A点

    param.type = VIEW_TYPE_SUB;
    param.unknown_key_deal = UNKNOWN_KEY_QUIT_REDEAL;
    param.overtime = 0;
    param.ke_maplist = music_ab_key_map_list;
    param.se_maplist = NULL;
    param.view_cb = _ab_view_handle;
    com_view_add(VIEW_ID_AB, &param);

    return RESULT_NULL;
}

#endif //#ifdef AB_SUPPORT
