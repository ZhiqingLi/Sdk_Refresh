/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌前台应用视图及其显示相关接口实现。
 * 作者：cailizhen
 ********************************************************************************/

#include  "ap_btplay.h"

#if (SUPPORT_LED_DRIVER == 1)
void wait_link_display(void)
{

//手机连上不闪，否则不管副箱是否连上继续闪    
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    if((g_bt_stack_cur_info.dev_role!=1)||(g_bt_stack_cur_info.tws_master_phonecon==1))
#endif
    {
            if (g_btplay_cur_info.status != BTPLAY_IDLE)
            {
                return;
            }
    }
    
    if (display_flag == TRUE)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //添加tws显示BU
        if(g_bt_stack_cur_info.dev_role==1)
        {
            led_display(NUMBER2, NUM_b, 1);
            led_display(NUMBER3, NUM_U, 1);    
        }
        else
#endif
        {
           led_display(NUMBER2, NUM_b, 1);
           led_display(NUMBER3, NUM_L, 1);    
        }
        display_flag = FALSE;
    }
    else
    {
        led_clear_screen();
        display_flag = TRUE;
    }
}


#ifdef ENABLE_TRUE_WIRELESS_STEREO
//tws显示调试BU
void tws_btplay_ui_display(void)
{
     led_clear_screen();

    switch (g_btplay_cur_info.status)
    {
        case BTPLAY_IDLE:
        led_display(NUMBER2, NUM_b, 1);
        led_display(NUMBER3, NUM_U, 1);
        break;

        default:
        led_display(NUMBER2, NUM_b, 1);
        led_display(NUMBER3, NUM_U, 1);
        if (g_btplay_cur_info.status == BTPLAY_PLAY)
        {
            led_display(LCD_PLAY, 0xff, 1);
            led_display(NUMBER2, NUM_b, 1);
            led_display(NUMBER3, NUM_U, 1);
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
            led_display(NUMBER3, NUM_U, 1);
        }
        else
        {
            ; //nothing for QAC
        }
        break;
    }    
}

#endif




void btplay_ui_display(void)
{
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
}
#endif

void btplay_main_view(view_update_e mode)
{
    uint32 reg_val;
    
    switch (mode)
    {
        case VIEW_UPDATE_CREATE:
        {
#if (SUPPORT_LED_DRIVER == 1)
            if (g_comval.support_led_display == 1)
            {        
                display_timer_id = set_app_timer(APP_TIMER_ATTRB_UI, 500, (timer_proc) wait_link_display);
                display_flag = FALSE;
            }
#endif
        }
        if((g_btplay_cur_info.status == BTPLAY_PLAY)||(g_btplay_cur_info.status == BTPLAY_PAUSE))
        {
            if(g_btplay_cur_info.status != g_btplay_flag)
            {    
                g_btplay_flag = g_btplay_cur_info.status;

                //del  reg_val = (act_readl(RTC_BAK0) & (~(0x7 << MY_RTC_BTPLAY_FLAG)));

                //del  reg_val |= ((uint32)(g_btplay_flag) << MY_RTC_BTPLAY_FLAG);
     
                //del  act_writel(reg_val , RTC_BAK0);

                //rtc register
                //del  act_writel(0xA596, RTC_REGUPDATA);
                //del  while (act_readl(RTC_REGUPDATA) != 0x5A69)
                {
                    ;//wait for register update
                }
                g_config_var.engine_state = g_btplay_flag;
                sys_vm_write(&g_config_var,VM_AP_CONFIG,sizeof(g_config_var_t));
                
            } 
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
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //添加tws显示BL
        if(g_bt_stack_cur_info.dev_role==1)
        {
            tws_btplay_ui_display();
        }
        else
#endif
        {
               btplay_ui_display();
        }
                
             
            }
#endif
        }
        if((g_btplay_cur_info.status == BTPLAY_PLAY)||(g_btplay_cur_info.status == BTPLAY_PAUSE))
        {
            if(g_btplay_cur_info.status != g_btplay_flag)
            {    
                g_btplay_flag = g_btplay_cur_info.status;
                //sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));
                //del  reg_val = (act_readl(RTC_BAK0) & (~(0x7 << MY_RTC_BTPLAY_FLAG)));

                //del  reg_val |= ((uint32)(g_btplay_flag) << MY_RTC_BTPLAY_FLAG);
     
                //del  act_writel(reg_val , RTC_BAK0);

                //rtc register
                //del  act_writel(0xA596, RTC_REGUPDATA);
                //del  while (act_readl(RTC_REGUPDATA) != 0x5A69)
                {
                    ;//wait for register update
                } 
                g_config_var.engine_state = g_btplay_flag;
                sys_vm_write(&g_config_var,VM_AP_CONFIG,sizeof(g_config_var_t));
            } 
#ifdef __ESD_MODE_
            //esd后，需要上报信息，才能发现异常，因此发送快进退消息
            if(g_btplay_cur_info.status == BTPLAY_PAUSE)
            {
                if (fast_play_flag != BTPLAY_FAST_FORWARD)
                {
                    if (com_btmanager_avrcp_op(BTPLAY_AVRCP_FFORWARD) == 0)
                    {
                        fast_play_flag = BTPLAY_FAST_NORMAL;
                    }
                }
                sys_os_time_dly(100); 
                com_btmanager_avrcp_op(BTPLAY_AVRCP_FFORWARD);
                sys_os_time_dly(100); 
                com_btmanager_avrcp_op(BTPLAY_AVRCP_FFORWARD);
                sys_os_time_dly(100); 
                com_btmanager_avrcp_op(BTPLAY_AVRCP_FFORWARD);
            }
 #endif           
        }
        break;

        default:
        break;
    }
}

//创建主视图
void btplay_create_main_view(void)
{
    create_view_param_t create_view_param;

    create_view_param.type = VIEW_TYPE_MAIN;
    create_view_param.unknown_key_deal = UNKNOWN_KEY_IGNORE;
    create_view_param.overtime = 0;
    create_view_param.ke_maplist = btplay_ke_maplist;
    create_view_param.se_maplist = btplay_se_maplist;
    create_view_param.view_cb = btplay_main_view;
    com_view_add(APP_VIEW_ID_MAIN, &create_view_param);
}
