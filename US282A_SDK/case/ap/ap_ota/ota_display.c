#include "ap_ota.h"

#if (SUPPORT_LED_DRIVER == 1)
void wait_ota_display(void)
{
	//if (g_ota_status != OTA_STATUS_IDLE)
	if (g_ota_restore_data.dowmload_statue == OTA_DOWNLOAD_STATE_NULL)
	{		
		if (display_flag == TRUE)
		{
			led_display(NUMBER1, NUM_O, 1);
			led_display(NUMBER2, NUM_T, 1);
			led_display(NUMBER3, NUM_A, 1);
			display_flag = FALSE;
		}
		else
		{
			led_clear_screen();
			display_flag = TRUE;
		}
	}
	else if (g_ota_restore_data.dowmload_statue == OTA_DOWNLOAD_STATE_START)
	{
		led_clear_screen();
		led_display(NUMBER1 + g_tick, NUM_ERROR, 1);

		g_tick = (g_tick + 1) % 4;
	}
	else if (g_ota_restore_data.dowmload_statue == OTA_DOWNLOAD_STATE_END)
	{
	}
}

void ota_ui_display(void)
{
#if 0
    led_clear_screen();

    switch (g_btplay_cur_info.status)
    {
        case BTPLAY_IDLE:
        led_display(NUMBER2, NUM_b, 1);
        led_display(NUMBER3, NUM_L, 1);
        break;

        default:
        led_display(NUMBER2, NUM_b, 1);
        led_display(NUMBER3, NUM_L, 1);
        if (g_btplay_cur_info.status == BTPLAY_PLAY)
        {
            led_display(LCD_PLAY, 0xff, 1);
            led_display(NUMBER2, NUM_b, 1);
            led_display(NUMBER3, NUM_L, 1);
            if (g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_attr.media_type == A2DP_CODEC_AAC)
            {
                led_display(NUMBER1, NUM_ERROR, 1);
                led_display(NUMBER4, NUM_ERROR, 1);
            }
        }
        else if (g_btplay_cur_info.status == BTPLAY_PAUSE)
        {
            led_display(LCD_PAUSE, 0xff, 1);
            led_display(NUMBER2, NUM_b, 1);
            led_display(NUMBER3, NUM_L, 1);
        }
        else
        {
            ; //nothing for QAC
        }
        break;
    }
#endif
}
#endif

void ota_main_view(view_update_e mode)
{
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {		
                display_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 500, (timer_proc) wait_ota_display);
                display_flag = FALSE;
            }
#endif
        }
        break;

        case VIEW_UPDATE_DESTROY:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
                if (display_timer_id != -1)
                {
                    kill_app_timer(display_timer_id);
                }
            }
#endif
        }
        break;

        case VIEW_UPDATE_REDRAW:
        case VIEW_UPDATE_STATUS:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {
			    ota_ui_display();
            }
#endif
        }
        break;

        default:
        break;
    }
}


//创建主视图
void ota_create_main_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MAIN;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 0;
    create_view_param.ke_maplist = ota_ke_maplist;
    create_view_param.se_maplist = ota_se_maplist;
    create_view_param.view_cb = ota_main_view;
    com_view_add(APP_VIEW_ID_MAIN, &create_view_param);
}
