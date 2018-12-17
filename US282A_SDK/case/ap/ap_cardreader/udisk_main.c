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
#include "ap_udisk.h"

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

uint32 g_vctl_back;

uint8 g_info_str[52];

//p_adfu_launcher
p_func_entry p_adfu_launcher;

int32 g_pop_up_timer;

uint32 g_pop_up_time_out;

//uint8 g_key_play;

//系统变量
comval_t g_comval;

uint8 g_reinstall_count;

uint8 g_removal_flag;

uint32 g_wait_time_before_enum = 0;

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

    //set init val
    g_pop_up_timer = -1;

    /*初始化applib库，前台AP*/
    applib_init(APP_ID_UDISK, APP_TYPE_GUI);

    /*初始化软定时器*/
    init_app_timers(g_mainmenu_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    /*初始化 applib 消息模块*/
    applib_message_init();

    /*初始化系统计时器*/
    sys_timer_init();

    com_rcp_init();//必须在xxx_rcp_var_init之前

    com_view_manager_init();

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
    com_view_manager_exit();

    /*备份系统计时器*/
    sys_timer_exit();

    /*保存VM变量*/
    //sys_vm_write(&g_mainmenu_var, VM_AP_MAINMENU);
    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    /*执行applib库的注销操作*/
    applib_quit();
}

/******************************************************************************/
/*!
 * \par  Description:
 * 调节vdd and 调频 forusb
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/
void _rise_vdd_and_freq(void)
{
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL11, FREQ_NULL);//max frequency
    //sys_enter_high_powered(3);
}

/******************************************************************************/
/*!
 * \par  Description:
 * 恢复vdd and freq
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/
void _back_vdd_and_freq(void)
{
    //sys_exit_high_powered();
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
}

/******************************************************************************/
/*!
 * \par  Description:
 * \0x00-0x0f转ASCII码
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/
void rd_itoa(uint8 data, uint8 pos, uint8 *str)
{
    //uint8 data_low,data_high;
    uint8 data_cur = 0;
    
    if(pos != 0)
    {
        data_cur = (uint8)((data & 0xf0) >> 4);
    }
    else
    {
        data_cur = (data & 0x0f);
    }
    
    if(data_cur > 9)
    {
        data_cur += 55;
    }
    else
    {
        data_cur += 48;
    }
    *str = data_cur;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \生成随机的SN并写VM。如果VM里面有SN，则直接使用VM中记录的SN
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/

void sn_random(uint8 *buf)
{
    uint8 i = 0;
    
    uint32 num1,num2 = 0;
    
    testval_t usb_sn = {0};
	
    sys_vm_read(&usb_sn,VM_API_FLAG,sizeof(testval_t)); 
	
    if(usb_sn.usb_cardreader_magic == 0x5A5A)
    {
        num1 = usb_sn.usb_cardreader_low;
        
        num2 = usb_sn.usb_cardreader_high;
    }
    else
    {
        num1 = sys_random();

        num2 = sys_random();

        usb_sn.usb_cardreader_low = num1;
        
        usb_sn.usb_cardreader_high = num2;
        
        usb_sn.usb_cardreader_magic = 0x5A5A;
        
        sys_vm_write(&usb_sn,VM_API_FLAG,sizeof(testval_t));
    }

    for(i=0;i<8;i++)
    {
        rd_itoa((uint8)(num1>>i),0,&buf[i]);
    }

    for(i=0;i<8;i++)
    {
        rd_itoa((uint8)(num2>>i),0,&buf[i+8]);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * 获取USB相关的配置信息
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      udisk_main.c
 * \note
 *******************************************************************************/
void _set_usb_info(void)
{
    //uint8 info_str[16];
    bool sn_default = 0;

    //vid
    com_get_config_struct(USB_VID, g_info_str, 6);

    ud_set_config(0, g_info_str, 6);

    //pid
    com_get_config_struct(USB_CARDREADER_PID, g_info_str, 6);

    ud_set_config(1, g_info_str, 6);

    //vendor
    com_get_config_struct(INF_USB_VENDOR, g_info_str, 8);

    ud_set_config(2, g_info_str, 8);

    //product name
    com_get_config_struct(INF_CARDREADER_PRODUCT_NAME, g_info_str, 16);

    ud_set_config(3, g_info_str, 16);

    ////CMD_Inquiry_information_FOB
    com_get_config_struct(INF_INQUIRY_INFORMATION_FOB, g_info_str, 52);

    ud_set_config(4, g_info_str, 52);
    
    com_get_config_struct(USB_CARDREADER_BCD_DEVICE, g_info_str, 6);
    
    ud_set_config(5, g_info_str, 6);

    sn_default = (bool)com_get_config_default(USB_SN_TYPE);
    
    if(sn_default == 1)
    {
        com_get_config_struct(USB_CARDREADER_SN, g_info_str, 16);
    }
    else
    {
        sn_random(&g_info_str);
    }
    
    ud_set_config(6, g_info_str, 16);

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
uint8 _detect_usb_line_status(void)
{
    //uint32 temp_save;
    //private_msg_t pri_msg;
    unsigned char plug_in = 0;
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
 *  切换到adfu的处理
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      usb
 * \note
 *******************************************************************************/
void _switch_to_adfu(void)
{

    //void (*p_adfu_launcher)(void) = *(uint32 *)0xbfc002a0;

    p_adfu_launcher = *(uint32*) 0xbfc002a0;

    //select hosc
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);

    //set vcc to 3.2 and vdd to 1.2
    act_writel(((act_readl(VOUT_CTL) & 0xFFFFFF80) | 0x58), VOUT_CTL);
    sys_mdelay(3);
#if (SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        led_clear_screen();
    }
#endif
    act_writel(0, INTC_CFG0);
    act_writel(0, INTC_CFG1);
    act_writel(0, INTC_CFG2); //assgin usb intrrupt to IP2

    act_writel(0x58, WD_CTL);

    p_adfu_launcher();

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
void _udevice_close_engine(void)
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

#if (SUPPORT_LED_DRIVER == 1)
    if (g_comval.support_led_display == 1)
    {
        led_clear_screen();
    }
#endif

    /*ap初始化*/
    _app_init();

    _rise_vdd_and_freq();//_app_init之后

    if (_detect_usb_line_status() != FALSE)
    {

        _config_disk_label();

#if (SUPPORT_LED_DRIVER == 1)
        if (g_comval.support_led_display == 1)
        {
            ud_paint_deal();
        }
#endif

        keytone_set_on_off(FALSE);

        //创建主视图
        udisk_create_main_view();

        _udevice_close_engine();

        com_tts_state_play((TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK | TTS_MODE_SYS_NORECV), (void *) TTS_ENTER_UDISK);

        //杀死蓝牙后台
        com_btmanager_exit(FALSE, TRUE);

        //等待TTS结束
        if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
        {
            sys_os_time_dly(10); //等待100ms，以便短按抬起能够完成
        }

        //U盘模式下不能进行TTS播报
        com_tts_state_play_control(TTS_CTRL_IGNORE);

        //usound_open_engine(APP_ID_UENGINE);

        sys_drv_install(DRV_GROUP_UD, 0, "udisk.drv");

        //some config set
        _set_usb_info();

        //disable detect when init
        DISABLE_USB_DETECT();
        //start init
        ud_module_start(NULL);
        //enable detect after init
        ENABLE_USB_DETECT();

        result = get_message_loop();

#if (SUPPORT_OUTER_CHARGE == 1)
        if (sys_comval->bat_charge_mode == BAT_CHARGE_MODE_OUTER)
        {
            if ((IS_EXTERN_BATTERY_IN () == 0) && (IS_EXTERN_CAHREGE_IN() == 1))
            {
                result = RESULT_IDLE_MODE; //进入空闲模式
            }
        }
#endif

        //disable keytone
        keytone_set_on_off(TRUE);

        //退出U盘模式开启TTS播报
        com_tts_state_play_control(TTS_CTRL_CANCEL_IGNORE);

        //disable detect when exit (need init)
        DISABLE_USB_DETECT();
        //stop and exit
        ud_module_stop();
        //enable detect after init
        ENABLE_USB_DETECT();

        sys_drv_uninstall(DRV_GROUP_UD);

    }
    else        //正常流程不进入，除非ESD
    {
        libc_print("reader esd restart",0,0);
        //del act_writel((act_readl(RTC_BAK0) & (~(0xff << MY_RTC_FUNC_INDEX)))|(0x07 <<MY_RTC_FUNC_INDEX), RTC_BAK0);
        //del act_writel(0xA596, RTC_REGUPDATA);
        //del while (act_readl(RTC_REGUPDATA) != 0x5A69)
        {
            ;//nothing for QAC
        }

        g_config_var.ap_id = APP_FUNC_UDISK;
        sys_vm_write(&g_config_var,VM_AP_CONFIG,sizeof(g_config_var_t));
        
        act_writel((0 << WD_CTL_CLKSEL_SHIFT) | (1 << WD_CTL_WDEN) | (1 << WD_CTL_CLR), WD_CTL);
        while (1)
        {
            ;//等待重启
        }
    }

    //switch to adfu and upgrade
    if (result == RESULT_ENTER_UPGRADE)
    {
        _switch_to_adfu();
    }

    if (result == RESULT_ENTER_STUB_DEBUG)
    {
        libc_print("open stub", 0, 0);

        result = com_sys_install_stub();
    }

    if ((result == RESULT_LASTPLAY) || (result == RESULT_NEXT_FUNCTION))
    {
        result = RESULT_NULL;
    }

#if (SUPPORT_RING_FOLDER == 1)
    if ((int8) sys_detect_disk(DRV_GROUP_STG_CARD) != -1)
    {
        g_ap_switch_var.result_alarm_ring_scan_backup = result;
        result = RESULT_ALARM_RING_FOLDER_SCAN;
    }
#endif

    com_ap_switch_deal(result);

    _back_vdd_and_freq();

    _app_exit();

    g_app_info_state.need_update_playlist = UHOST_NEED_MASK | CARD_NEED_MASK;

    //for disk play to clr bk info
    g_ap_switch_var.card_in_out_flag = 1;

    g_ap_switch_var.uhost_in_out_flag = 1;

    return result;
}

