//数字选台视图
#include "app_radio.h"

app_result_e radio_digit_key_deal_number_input(void);
app_result_e radio_digit_key_deal_number_play(void);
const key_event_map_t __section__(".rodata.ke_maplist") radio_digit_ke_maplist[] =
{
    /*! 数字频点 */
    { { KEY_NUMBER, 0, KEY_TYPE_SHORT_UP | KEY_TYPE_LONG_UP, KEY_DEAL_NULL }, radio_digit_key_deal_number_input },
    /*! 切换到下一电台 */
    { { KEY_PLAY, 0, KEY_TYPE_SHORT_UP, KEY_DEAL_NULL }, radio_digit_key_deal_number_play },

    /*! 结束标志 */
    { { KEY_NULL, 0, KEY_TYPE_NULL, KEY_DEAL_NULL }, NULL },
};

app_result_e radio_digit_key_deal_number_input(void)
{
    app_result_e result;
    uint8 number = (uint8)(g_p_view_ke->val - KEY_NUM0);

    result = radio_play_key_digital_store(number);

    return result;
}

app_result_e radio_digit_key_deal_number_play(void)
{
    app_result_e result;

    result = radio_play_key_digital_sure();
    g_digit_input_flag = TRUE;

    return result;
}

void show_digital_set(void)
{
#if(SUPPORT_LED_DRIVER == 1)
    uint8 i, dis_val;

    //数字转换
    uint16 input_num = 0;
    uint16 multis = 1;

    if (g_comval.support_led_display == 0)
    {
        return;
    }

    for (i = 1; i < g_digital_count; i++)
    {
        multis = multis * 10;
    }
    for (i = g_digital_count; i > 0; i--)
    {
        input_num = input_num + g_digital_buf[g_digital_count - i] * multis;
        multis = multis / 10;
    }

    led_clear_screen();

    //显示序号

    //最高位
    dis_val = (uint8) (input_num / 1000);

    led_display(NUMBER1, (uint32 )dis_val, 1);

    //第二位
    input_num = input_num % 1000;
    dis_val = (uint8) (input_num / 100);

    led_display(NUMBER2, (uint32 )dis_val, 1);

    //第三位
    input_num = input_num % 100;
    dis_val = (uint8) (input_num / 10);

    led_display(NUMBER3, (uint32 )dis_val, 1);

    //第四位
    dis_val = (uint8) (input_num % 10);
    led_display(NUMBER4, (uint32 )dis_val, 1);
#endif
}

void __radio_digit_view_redraw(void)
{
    show_digital_set();
}

void radio_digit_sub_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
            __radio_digit_view_redraw();
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
            radio_play_key_digital_sure();
            g_digit_input_flag = FALSE;
        }
        break;

        case VIEW_UPDATE_REDRAW:
        {
            __radio_digit_view_redraw();
        }
        break;

        case VIEW_UPDATE_STATUS:
        {
            __radio_digit_view_redraw();
        }
        break;

        default:
        break;
    }
}

//创建视图
void radio_digit_create_sub_view(uint16 overtime)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_SUB;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = overtime;
    create_view_param.ke_maplist = radio_digit_ke_maplist;
    create_view_param.se_maplist = NULL;
    create_view_param.view_cb = radio_digit_sub_view;
    com_view_add(RADIO_SUB_VIEW_DIGIT, &create_view_param);
}
