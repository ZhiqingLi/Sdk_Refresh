//      播放窗口的paint
//
//  这里的代码跟music_play.c放在同一个bank, 这样播放时就不用切bank(在不按按键、不出异常时).
//      但为什么把它单独做一个文件呢？因为有很多方案是没有LED的，
//      这种情况下，只要改这里，就可以省很多代码空间。
//
#include "music.h"


//显示播放时间, 播放状态(播放/暂停), 播放的是哪个盘(U/卡)
void play_view_handle(view_update_e mode)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 0)
    {
        return;
    }

    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        led_clear_screen();
        //显示4个0
        led_display(NUMBER1, NUM_0, TRUE);
        led_display(NUMBER2, NUM_0, TRUE);
        led_display(NUMBER3, NUM_0, TRUE);
        led_display(NUMBER4, NUM_0, TRUE);
        break;

        case VIEW_UPDATE_DESTROY:
        led_clear_screen();
        break;

        case VIEW_UPDATE_REDRAW:
        if (g_quit_from_hint == TRUE)
        {
            break;
        }
        g_play_refresh_flag = PLAY_REFRESH_ALL;
        //跟下面case相同的处理
        case VIEW_UPDATE_STATUS:
        //刷新播放时间，播放状态, 盘符
        if ((g_play_refresh_flag & PLAY_REFRESH_TIME) != 0)
        {
            uint32 cur_time; //单位秒
            uint8 minute; //分钟

            //毫秒转换为秒
            cur_time = g_music_info.eg_playinfo.cur_time / 1000;
            //去掉小时
            cur_time %= 3600;

            minute = (uint8)(cur_time / 60);

            //分钟的十位
            led_display(NUMBER1, NUM_0 + minute / 10, TRUE);
            //分钟的个位
            led_display(NUMBER2, NUM_0 + (minute % 10), TRUE);

            //:
            led_display(LCD_COL, 0xff, TRUE);

            //秒
            cur_time %= 60;
            //秒的十位
            led_display(NUMBER3, NUM_0 + cur_time / 10, TRUE);
            //秒的个位
            led_display(NUMBER4, NUM_0 + (cur_time % 10), TRUE);
        }

        if ((g_play_refresh_flag & PLAY_REFRESH_STATUS) != 0)
        {
            //显示播放状态
            if ((g_music_info.eg_status.play_status == StopSta) || (g_music_info.eg_status.play_status == PauseSta))
            {
                led_display(LCD_PAUSE, 0xff, TRUE);
                led_display(LCD_PLAY, 0xff, FALSE);
            }
            else
            {
                led_display(LCD_PLAY, 0xff, TRUE);
                led_display(LCD_PAUSE, 0xff, FALSE);
            }
        }

        if ((g_play_refresh_flag & PLAY_REFRESH_DISK) != 0)
        {
            //显示盘符
            music_get_filepath(&g_file_path_info); //获取文件路径
            if (g_file_path_info.file_path.dirlocation.disk == DISK_H)
            {
                led_display(LCD_SD, 0xff, TRUE);
                led_display(LCD_USB, 0xff, FALSE);
            }
            else
            {
                led_display(LCD_USB, 0xff, TRUE);
                led_display(LCD_SD, 0xff, FALSE);
            }
        }
        g_play_refresh_flag = 0;
        break;

        default:
        break;
    }
#endif
}
