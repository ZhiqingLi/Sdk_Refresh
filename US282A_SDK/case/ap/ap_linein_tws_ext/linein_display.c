/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：linein tws推歌前台应用视图及其显示相关接口实现。
 * 作者：
 ********************************************************************************/

#include  "ap_linein.h"


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

    if(g_bt_stack_cur_info.dev_role !=2)
    {
        //icon
        led_display(LCD_AUX, 0xff, 1);
    }

    //显示播放状态
    if (g_btplay_cur_info.status == BTPLAY_PAUSE)
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

        linein_ui_display();
        break;

        case VIEW_UPDATE_DESTROY:

        break;

        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:

        linein_ui_display();
        break;

        default:
        break;
    }
}

//创建主视图
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
