//          循环模式视图
//
//1.模式窗口。
//2.任意键能关闭它,且这个按键消息被“新的顶层模式窗口”接收。
//  比如V+键将关闭本窗口，同时弹出“音量设置窗口”(模式窗口)，
//  新窗口会处理V+键(音量加1级)。
//
//  如果按下“快进键”(如果有单独的快进键的话)，将关闭本窗口，
//  “播放窗口”成为新的顶层模式窗口，新窗口会处理快进键。
#include "music.h"

MUS_STATIC app_result_e _loop_key_next(void);
MUS_STATIC app_result_e _loop_key_prev(void);

MUS_STATIC const key_event_map_t __section__(".rodata.ke_maplist")
music_loop_mode_key_map_list[] =
{
    { { KEY_NEXT, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _loop_key_next },
    { { KEY_PREV, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _loop_key_prev },
    { { KEY_REPEAT, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _loop_key_next },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL }, NULL },
};

MUS_STATIC app_result_e _loop_key_next(void)
{
    music_get_loop_mode(&g_music_config.repeat_mode);

    //切换循环模式
    g_music_config.repeat_mode++;
    //切换到最大返回到第一个
    if (g_music_config.repeat_mode > FSEL_MODE_RANOM)
    {
        g_music_config.repeat_mode = MIN_LOOP_MODE;
    }

    music_set_loop_mode(g_music_config.repeat_mode);
    com_view_update(VIEW_ID_LOOP_MODE);

    return RESULT_NULL;
}

MUS_STATIC app_result_e _loop_key_prev(void)
{
    music_get_loop_mode(&g_music_config.repeat_mode);

    if (g_music_config.repeat_mode > MIN_LOOP_MODE)
    {
        //切换循环模式
        g_music_config.repeat_mode--;
    }
    else
    {
        //切换到第一个返回到最大
        g_music_config.repeat_mode = FSEL_MODE_RANOM;
    }

    music_set_loop_mode(g_music_config.repeat_mode);
    com_view_update(VIEW_ID_LOOP_MODE);

    return RESULT_NULL;
}

MUS_STATIC void _loop_show(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 0)
    {
        return;
    }

    led_display(NUMBER2, NUM_L, TRUE);
    led_display(NUMBER3, NUM_P, TRUE);
    led_display(NUMBER4, NUM_0 + g_music_config.repeat_mode + 1, TRUE);
#endif
}

//视图的生成、更新、销毁处理
MUS_STATIC void _loop_view_handle(view_update_e mode)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        led_clear_screen();
    }
#endif

    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:
        _loop_show();
        break;
        case VIEW_UPDATE_DESTROY:
        break;
        default:
        break;
    }
}

app_result_e create_view_loop_mode(void)
{
    create_view_param_t param;

    param.type = VIEW_TYPE_SUB;
    param.unknown_key_deal = UNKNOWN_KEY_QUIT_REDEAL;
    param.overtime = 100;
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        param.overtime = 3000;
    }
#endif
    param.ke_maplist = music_loop_mode_key_map_list;
    param.se_maplist = NULL;
    param.view_cb = _loop_view_handle;
    com_view_add(VIEW_ID_LOOP_MODE, &param);

    return RESULT_NULL;
}
