/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/

#include "ap_usound.h"

/*全局变量定义*/
//应用自己需要的软定时器个数
#define APP_TIMER_COUNT    2
#define    AP_RADIO_STK_POS  AP_FRONT_LOW_STK_POS
#define    AP_RADIO_PRIO     AP_FRONT_LOW_PRIO

app_timer_t g_mainmenu_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

/*main中要填充这两个值，由系统人员分配*/
OS_STK *ptos = (OS_STK *) AP_RADIO_STK_POS;
/* Radio UI 进程的主线程优先级*/
INT8U prio = AP_RADIO_PRIO;

//uint8 g_key_play;
uengine_status_t g_play_status;

//save enter volume
uint32 g_raw_volume;

uint32 g_pc_syn_flag;

//for manual set flag
uint32 g_manual_set;

uengine_sync_volume_t* g_volume_share;

//系统变量
comval_t g_comval;

//初始化阶段，可以过滤掉一些事件处理
bool g_usound_init_flag;

//play key flag
uint32 g_play_key_flag;

//timer use
int32 g_play_key_timer = -1;

//for counter
uint32 g_last_time_count;

uint8 g_usound_cfg;

uint8 g_eq_temp;

/******************************************************************************/
/*!
 * \par  Description:
 *      MainMenu 读取应用VM全局变量,初始化全局变量
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void _read_var(void)
{
    //读取common VM公共变量信息
    com_setting_comval_init(&g_comval);
    
    g_usound_cfg = (uint8)com_get_config_default(USOUND_TYPE);
}

/******************************************************************************/
/*!
 * \par  Description:
 *      applib初始化
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void _app_init(void)
{

    /*读入VM变量*/
    _read_var();

    /*初始化applib库，前台AP*/
    applib_init(APP_ID_USOUND, APP_TYPE_GUI);

    //sys_enter_high_powered(1);
    /*初始化软定时器*/
    init_app_timers(g_mainmenu_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    /*初始化 applib 消息模块*/
    applib_message_init();

    /*初始化系统计时器*/
    sys_timer_init();

    com_rcp_init();//必须在xxx_rcp_var_init之前
    usound_rcp_var_init();

    com_view_manager_init();

    keytone_set_on_off(FALSE);
    
    sys_set_sys_info(0, SYS_PRINT_ONOFF);

    if(g_usound_cfg != 0)
    {
        g_eq_temp = sys_comval->dae_cfg.bypass;
        com_set_dae_onoff(0);//关音效
    }
    //g_key_play = FALSE;
    com_set_dae_chan(FALSE, FALSE);
}

/******************************************************************************/
/*!
 * \par  Description:
 *      applib退出
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void _app_exit(void)
{
    uint8 uart_enable = 0;
    
    keytone_set_on_off(TRUE);
    
    g_tts_play_info.option &= (~IGNORE_PLAY_TTS);
    
    if((g_usound_cfg != 0)&&(g_eq_temp == 0))
    {
        com_set_dae_onoff(1);//开音效
    }   
    
    uart_enable = (uint8) com_get_config_default(SETTING_UART_PRINT_ENABLE);
    sys_set_sys_info(uart_enable, SYS_PRINT_ONOFF);
    
    usound_rcp_var_exit();

    com_view_manager_exit();

    /*备份系统计时器*/
    sys_timer_exit();

    /*保存VM变量*/
    //sys_vm_write(&g_mainmenu_var, VM_AP_MAINMENU);
    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    /*执行applib库的注销操作*/
    applib_quit();
    //sys_exit_high_powered();
}

/******************************************************************************/
/*!
 * \par  Description:
 *  检测是否有usb插入，确认是否进入U盘应用
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      music
 * \note
 *******************************************************************************/
static uint8 detect_usb_line_status(void)
{
    uint32 plug_in = 0;
    if (get_usb_port_state() == USB_PORT_HOST)
    {
        //检测到uhost插入
        if (sys_detect_disk(DRV_GROUP_STG_UHOST) == 0)
        {
            return FALSE;
        }

        key_peripheral_detect_handle(PER_DETECT_USB_FORCE);

        sys_os_time_dly(30);
    }

    //get usbcable plug status
    plug_in = (act_readb(DPDMCTRL) & (1 << DPDMCTRL_Plugin));

    if (plug_in != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  杀死后台引擎
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      music
 * \note
 *******************************************************************************/
void udevice_close_engine(void)
{
    msg_apps_t msg;
    engine_type_e engine_type = get_engine_type();

    if (engine_type != ENGINE_NULL)
    {
        msg.type = MSG_KILL_APP_SYNC;
        msg.content.data[0] = APP_ID_THEENGINE;

        send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
    }

    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool usound_open_engine(uint8 engine_id)
 * \装载引擎,如果引擎存在，则不再安装
 * \param[in]    bool  para1
 * \param[out]   none
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      music_scene_playing.c
 * \note
 */
/*******************************************************************************/
bool usound_open_engine(uint8 engine_id)
{
    bool bret = FALSE;
    msg_apps_t msg;
    //msg_reply_t temp_reply;

    //无消息内容
    msg.content.data[0] = engine_id;
    msg.content.data[1] = sys_comval->volume_current;

    //消息类型(即消息名称)
    msg.type = MSG_CREAT_APP_SYNC;
    //发送同步消息
    if (send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0) == ERR_NO_ERR)
    {
        bret = TRUE;
    }

    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool music_close_engine(uint8 engine_id)
 * \关闭引擎
 * \param[in]    bool  para1
 * \param[out]   none
 * \return       void the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      music_scene_playing.c
 * \note
 */
/*******************************************************************************/
bool usound_close_engine(uint8 engine_id)
{
    bool bret = FALSE;
    msg_apps_t msg;
    //注销后台，后台engine_id
    msg.type = MSG_KILL_APP_SYNC; //MSG_KILL_APP;
    msg.content.data[0] = engine_id;
    //发送同步消息
    if (send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0) == ERR_NO_ERR)
    {
        bret = TRUE;
    }

    return bret;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      MainMenu 应用的入口函数和场景管理器模块
 * \param[in]    PARAM_FROM_XXX
 * \param[out]   none
 * \return       the result
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
int main(int argc, const char *argv[])
{
    app_result_e result = RESULT_NULL;
    //void (*p_adfu_launcher)(void) = 0xbfc004b9;
	
    /*ap初始化*/
    _app_init();

    if (detect_usb_line_status() != FALSE)
    {
#if (SUPPORT_LED_DRIVER == 1)
        if (g_comval.support_led_display == 1)
        {
            ud_paint_deal();
        }
#endif
        //解除静音模式
        com_set_mute(FALSE);
        
        //创建主视图
        usound_create_main_view();

        udevice_close_engine();

        g_usound_init_flag = TRUE;
        if (g_ap_switch_var.call_background_status != CALL_BACK_STATUS_BACK)
        {
            com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, (void*) TTS_ENTER_USBBOX);

            PRINT_INFO("ttsover");

            if (g_comval.support_bt_inback == 1)
            {
                //创建蓝牙后台
                com_btmanager_init(FALSE);
            }
            else
            {
                //杀死蓝牙后台
                com_btmanager_exit(FALSE, TRUE);
            }

            //等待TTS结束
            if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
            {
                sys_os_time_dly(10); //等待100ms，以便短按抬起能够完成
            }
        }
        
        g_tts_play_info.option |= IGNORE_PLAY_TTS;        //播报完进入usb音箱后关闭按键音TTS

        //for key play record
        g_play_key_flag = 0;

        while (1)
        {
            result = com_view_loop();
            if (result == RESULT_NONE_EVENT)
            {
                result = RESULT_NULL;
                break;
            }
            else if (result > RESULT_COMMON_RESERVE)
            {
                goto usound_main_exit;
            }
            else
            {
                ; //nothing
            }
        }

        g_usound_init_flag = FALSE;

        usound_open_engine(APP_ID_UENGINE);
        
        //save volume for exit restore
        g_raw_volume = com_get_sound_volume();

        g_manual_set = 0;

        com_reset_sound_volume(0);

        result = get_message_loop();

        usound_close_engine(APP_ID_UENGINE);

        if ((g_pc_syn_flag == 1) && (g_manual_set == 0))
        {
            //设置音量
            com_set_sound_volume((uint8) g_raw_volume, 0);
        }
    }
    else
    {
        result = RESULT_NEXT_FUNCTION;
    }
    usound_main_exit:     
    
    if (result == RESULT_ENTER_STUB_DEBUG)
    {
        //杀死蓝牙后台
        com_btmanager_exit(FALSE, TRUE);
        
        libc_print("open stub", 0, 0);

        result = com_sys_install_stub();
    }    

    com_ap_switch_deal(result);

    _app_exit();

    return result;
}

