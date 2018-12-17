//显示FM频道号
#include "app_radio.h"

void show_station_num(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    uint8 station;

    if (g_comval.support_led_display == 0)
    {
        return;
    }

    led_clear_screen();

    //CH
    led_display(NUMBER1, NUM_C, 1);
    led_display(NUMBER2, NUM_H, 1);
    //电台号
    station = g_radio_config.FMStatus.station / 10;
    led_display(NUMBER3, (uint32 )station, 1);
    station = g_radio_config.FMStatus.station % 10;
    led_display(NUMBER4, (uint32 )station, 1);
#endif
}

void __radio_station_view_redraw(void)
{
    show_station_num();

    //radio 频点语音提示
    radio_freq_tts_play(g_radio_config.FMStatus.freq);
}

void radio_station_msg_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            __radio_station_view_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {

        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            __radio_station_view_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            __radio_station_view_redraw();
        }
        break;

        default:
        break;
    }
}

//创建消息视图--显示FM频道号
//播放主视图下超时时间为2s
//搜台主视图下超时时间为200ms
void radio_station_create_msg_view(uint16 overtime)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MSG;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = overtime;
    create_view_param.ke_maplist = NULL;
    create_view_param.se_maplist = NULL;
    create_view_param.view_cb = radio_station_msg_view;
    com_view_add(RADIO_MSG_VIEW_STATION, &create_view_param);
}

