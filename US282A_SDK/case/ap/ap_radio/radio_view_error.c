//显示错误
#include "app_radio.h"

void show_error(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 0)
    {
        return;
    }

    led_clear_screen();

    led_display(LCD_FM, 0xFF, 1);
    led_display(NUMBER1, NUM_E, 1);
    led_display(NUMBER2, NUM_R, 1);
    led_display(NUMBER3, NUM_0, 1);
    led_display(NUMBER4, NUM_5, 1);
#endif
}

void __radio_error_view_redraw(void)
{
    show_error();
}

void radio_error_msg_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            __radio_error_view_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {

        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            __radio_error_view_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            __radio_error_view_redraw();
        }
        break;

        default:
        break;
    }
}

//创建消息视图--显示ERROR
void radio_error_create_msg_view(uint16 overtime)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MSG;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = overtime;
    create_view_param.ke_maplist = NULL;
    create_view_param.se_maplist = NULL;
    create_view_param.view_cb = radio_error_msg_view;
    com_view_add(RADIO_MSG_VIEW_ERROR, &create_view_param);
}

