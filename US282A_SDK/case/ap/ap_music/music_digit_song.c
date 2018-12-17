//          数字选歌窗口
//
//1.模式窗口。
//2.任意非数字键能关闭它,且这个按键消息被“新的顶层模式窗口”接收。
//  比如V+键将关闭本窗口，同时弹出“音量设置窗口”(模式窗口)，
//  新窗口会处理V+键(音量加1级)。
//
//  如果按下“快进键”(如果有单独的快进键的话)，将关闭本窗口，
//  “播放窗口”成为新的顶层模式窗口，新窗口会处理快进键。

#include "music.h"

//最多几个数字
#define DIGIT_COUNT     (MAX_DIGITAL_COUNT)

MUS_STATIC app_result_e _dig_key_number(void);

MUS_STATIC const key_event_map_t __section__(".rodata.ke_maplist")
music_digit_song_key_map_list[] =
{
    { { KEY_NUMBER, 0, KEY_TYPE_DOWN, KEY_DEAL_FILTER }, _dig_key_number },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL }, NULL },
};

//存储输入的数字(高位在低下标)
MUS_STATIC uint8 s_digit_buf[DIGIT_COUNT];
//当前数字的个数
MUS_STATIC uint8 s_digit_count;

MUS_STATIC void _dig_show(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    uint8 n;

    if (g_comval.support_led_display == 0)
    {
        return;
    }

    for (n = 0; n < s_digit_count; n++)
    {
        led_display(NUMBER1 + (4 - s_digit_count),
                NUM_0 + s_digit_buf[n], TRUE);
    }
#endif
}

MUS_STATIC app_result_e _dig_key_number(void)
{
    s_digit_buf[s_digit_count] = (uint8)(g_p_view_ke->val - KEY_NUM0);
    s_digit_count++;
    com_view_update(VIEW_ID_DIGIT_SONG);
    if (s_digit_count == DIGIT_COUNT)
    {
        com_view_remove(VIEW_ID_DIGIT_SONG, TRUE);
    }

    return RESULT_NULL;
}

MUS_STATIC void _dig_select_song(void)
{
    uint16 song_num = 0;
    uint8 n;
    play_mode_e play_mode;
    if((g_music_info.eg_status.play_status != StopSta)
        && (g_music_info.eg_status.play_status != PauseSta))
    {
        play_mode = PLAY_NORMAL;
    }
    else
    {
        play_mode = PLAY_NO_PLAY;
    }

    for (n = 0; n < s_digit_count; n++)
    {
        song_num *= 10;
        song_num += s_digit_buf[n];
    }
    music_get_filepath(&g_file_path_info);
    if (song_num != g_file_path_info.file_path.dirlocation.file_num)
    {
        music_digital_song(song_num);
        music_switch_song(TTS_SONG_SEQ, play_mode);
    }
}

//视图的生成、更新、销毁处理
MUS_STATIC void _dig_view_handle(view_update_e mode)
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
        break;

        case VIEW_UPDATE_DESTROY:
        //退出本视图，有以下原因
        //1.按任意键退出
        //2.超时退出
        //3.输入了4个数字(输满了)主动退出
        //4.强制退出
        //  4.1其他同类视图的加载(比如：卡拔出，导致弹出hint窗口。
        //      而弹出hint窗口时，按照现有机制可能会导致本窗口被关闭)
        //  4.2比如，apk上按PLAY键，apk的本意是操作主窗口上的PLAY,
        //      所以，本窗口要退出。
        //第1,4种，直接退出，不选歌
        //第2,3种，要选歌
        //注意:强制退出(比如：卡拔出，按POWER键退出时，不会调用到这里)

        _dig_select_song();
        break;

        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:
        _dig_show();
        break;
        default:
        break;
    }
}

app_result_e create_view_digit_song(key_event_t *p_ke)
{
    create_view_param_t param;

    s_digit_count = 0;
    //这里不做按键的检查(客户端已经保证了)
    s_digit_buf[s_digit_count] = (uint8) (p_ke->val - KEY_NUM0);
    s_digit_count++;

    param.type = VIEW_TYPE_SUB;
    param.unknown_key_deal = UNKNOWN_KEY_QUIT_REDEAL;
    param.overtime = 3000;
    param.ke_maplist = music_digit_song_key_map_list;
    param.se_maplist = NULL;
    param.view_cb = _dig_view_handle;
    com_view_add(VIEW_ID_DIGIT_SONG, &param);

    return RESULT_NULL;
}
