//          提示信息窗口
//
//1.模式窗口。
//2.任意键能关闭它,且这个按键消息被“新的顶层模式窗口”接收。
//  比如V+键将关闭本窗口，同时弹出“音量设置窗口”(模式窗口)，
//  新窗口会处理V+键(音量加1级)。
//
//  如果按下“快进键”(如果有单独的快进键的话)，将关闭本窗口，
//  “播放窗口”成为新的顶层模式窗口，新窗口会处理快进键。
#include "music.h"

MUS_STATIC eg_err_e s_hint_err;

MUS_STATIC void _hint_show(uint8 error_no)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 0)
    {
        return;
    }

    led_display(NUMBER1, NUM_E, TRUE);
    led_display(NUMBER2, NUM_R, TRUE);
    led_display(NUMBER3, NUM_0, TRUE);
    led_display(NUMBER4, NUM_0 + error_no, TRUE);
#endif
}

//视图的生成、更新、销毁处理
MUS_STATIC void _hint_view_handle(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        g_quit_from_hint = FALSE;

#if(SUPPORT_LED_DRIVER == 1)
        if (g_comval.support_led_display == 1)
        {
            led_clear_screen();
        }
#endif
        switch (s_hint_err)
        {
            case EG_ERR_DISK_OUT:
            _hint_show(3);
            break;
            default:
            _hint_show(1);
            break;
        }
        break;

        case VIEW_UPDATE_DESTROY:
        g_quit_from_hint = TRUE;
        break;

        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:
        default:
        break;
    }
}

//err: 错误号
app_result_e create_view_hint(eg_err_e err)
{
    //创建hint窗口
    create_view_param_t param;

    s_hint_err = err;

    param.type = VIEW_TYPE_MSG;
    param.unknown_key_deal = UNKNOWN_KEY_QUIT_REDEAL;
    //因为不能显示，所以就尽快退出
    param.overtime = 100;
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        param.overtime = 3000;
    }
#endif
    param.ke_maplist = NULL;
    param.se_maplist = NULL;
    param.view_cb = _hint_view_handle;
    com_view_add(VIEW_ID_HINT, &param);

    return RESULT_NULL;
}
