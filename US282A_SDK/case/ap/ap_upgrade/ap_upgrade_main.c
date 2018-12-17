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
/*!
 * \file     mainmenu_main.c
 * \brief    MainMenu的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_upgrade.h"

/*全局变量定义*/
//应用自己需要的软定时器个数
#define APP_TIMER_COUNT    1
#define    AP_RADIO_STK_POS  AP_FRONT_LOW_STK_POS
#define    AP_RADIO_PRIO     AP_FRONT_LOW_PRIO

app_timer_t g_mainmenu_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

/*main中要填充这两个值，由系统人员分配*/
OS_STK *ptos = (OS_STK *) AP_RADIO_STK_POS;
/* Radio UI 进程的主线程优先级*/
INT8U prio = AP_RADIO_PRIO;

int32 prev_freq = 0;

//3s超时定时器
int8 overtime_timer_id = 0xff;

uint8 g_cur_status;

uint8 g_paint_flag;

uint8 g_display_bitmap;

//标志是否是play按键切换usb应用
uint8 g_key_play;

//系统变量
comval_t g_comval;

uint8 g_led_state;

typedef void (*handler_ker)(void);

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
//static void _read_var(void)
//{
//    //读取common VM公共变量信息
//    //com_setting_comval_init(&g_comval);
//}

//static app_result_e upgrade_create_front_app(uint8 ap_id)
//{
//    msg_apps_t msg;
//
//    msg.type = MSG_CREAT_APP;
//    msg.content.data[0] = ap_id;
//    msg.content.data[1] = 0;
//    send_async_msg(APP_ID_MANAGER, &msg);
//
//    return RESULT_APP_QUIT;
//}

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
static void _app_init(void)
{
    uint32 temp_save;
    //uint8 *p_backdoor_flag = (uint8 *) 0x9fc19a9c;

    // temp_save = sys_local_irq_save();
    // *p_backdoor_flag |= 0x08;

    // sys_local_irq_restore(temp_save);
    /*读入VM变量*/
    //_read_var();

    /*初始化applib库，前台AP*/
    applib_init(APP_ID_UPGRADE, APP_TYPE_GUI);

    /*初始化软定时器*/
    init_app_timers(g_mainmenu_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    /*初始化 applib 消息模块*/
    //applib_message_init();

    /*初始化系统计时器*/
    sys_timer_init();

    g_key_play = FALSE;
    
    g_led_state = 0;
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
static void _app_exit(void)
{
    /*备份系统计时器*/
    sys_timer_exit();

    /*保存VM变量*/
    //sys_vm_write(&g_mainmenu_var, VM_AP_MAINMENU);
    //sys_vm_write(&g_setting_comval, VM_AP_SETTING);

    /*关闭ui资源文件*/
    //ui_res_close(UI_AP);
    //ui_res_close(UI_COM);

    /*关闭菜单配置文件*/
    //com_close_confmenu_file();

    /*执行applib库的注销操作*/
    applib_quit();
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void _scene_result_deal(app_result_e result_val)
 * \场景返回值处理
 * \param[in]    result_val 场景调度返回值 para1
 * \param[out]   none
 * \return       none the result
 * \ingroup      udevice
 * \note
 */
/*******************************************************************************/
void _scene_result_deal(app_result_e result_val)
{
    msg_apps_t msg;
    msg.type = MSG_CREAT_APP; //创建ap

    msg.content.data[1] = PARAM_FROM_UDISK;//param

    switch (result_val)
    {
        case RESULT_RADIO:
        //进入radio应用
        //msg.content.data[0] = APP_ID_RADIO;
        //发送异步消息，创建其他前台进程
        //send_async_msg(APP_ID_MANAGER, &msg);
        break;

        case RESULT_APP_QUIT:
        break;

        default:
        break;
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
static void upgrade_close_engine(void)
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
    int32 wd_ctl_tmp;
    int32 irq_save = 0;

    /*ap初始化*/
    _app_init();

#if(SUPPORT_LED_DRIVER == 1)  
    led_clear_screen();
    led_display(NUMBER1, NUM_U, 1);
    led_display(NUMBER2, NUM_P, 1);
    led_display(NUMBER3, NUM_G, 1);
    led_display(NUMBER4, NUM_E, 1);
#endif

    upgrade_close_engine();

    //杀死蓝牙后台
    com_btmanager_exit(FALSE, TRUE);
    
    irq_save = sys_local_irq_save();

    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL11, FREQ_NULL);//max frequency
    change_ui_status(UI_STATUS_BUSY);

    result = upgrade_deal();

    //创建ap
    com_ap_switch_deal(result);

    _app_exit();

    return result;
}

