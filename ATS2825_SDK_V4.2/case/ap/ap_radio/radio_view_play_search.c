//搜台视图
#include "app_radio.h"

static void show_freq(uint32 freq)
{
#if(SUPPORT_LED_DRIVER == 1)
    uint8 dis_num;

    if(g_comval.support_led_display == 0) {
        return;
    }

    led_clear_screen();

    //定位到mhz
    freq = freq / 100;

    led_display(LCD_FM, 0xff, 1);

    //最高位
    dis_num = (uint8)(freq / 1000);
    if(dis_num != 0) {
        led_display(NUMBER1, (uint32)dis_num, 1);
    } else {
        led_display(NUMBER1, 0, 0);
    }

    freq = freq % 1000;
    //第二位
    dis_num = (uint8)(freq / 100);

    led_display(NUMBER2, (uint32)dis_num, 1);

    //第三位
    freq = freq % 100;
    dis_num = (uint8)(freq / 10);

    led_display(NUMBER3, (uint32)dis_num, 1);

    //点
    led_display(LCD_NO1, 0xff, 1);

    //第四位
    dis_num = (uint8)(freq % 10);
    led_display(NUMBER4, (uint32)dis_num, 1);
#endif
}

static void __radio_play_view_redraw(void)
{
    show_freq(g_radio_config.FMStatus.freq);
}

void radio_play_main_view(view_update_e mode)
{
    switch(mode) {
        case VIEW_UPDATE_CREATE: {
            __radio_play_view_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY: {

        }
        break;

        case VIEW_UPDATE_REDRAW: {
            __radio_play_view_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS: {
            __radio_play_view_redraw();
        }
        break;

        default:
            break;
    }
}

//创建主视图--收音播放
void __section__(".bank") radio_play_create_main_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MAIN;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 0;
    create_view_param.ke_maplist = radio_play_ke_maplist;
    create_view_param.se_maplist = NULL;
    create_view_param.view_cb = radio_play_main_view;
    com_view_add(RADIO_MAIN_VIEW_PLAY, &create_view_param);
}
