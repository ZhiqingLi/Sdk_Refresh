/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   主函数，初始ap，参数和创建后台
 *      <author>       <time>
 *       Wekan   2015-3-27
 *******************************************************************************/

#include  "ap_btcall.h"

/*全局变量定义*/
//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     3

app_timer_t g_mainmenu_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];
//系统变量
comval_t g_comval;

btcall_status_e g_last_btcall_status = BTCALL_IDLE;
bool g_display_flag;
int8 g_display_timer_id = -1;
bool g_need_tts_play = FALSE;
bool g_need_draw = FALSE;
bool g_chf_cag_exch_enable;
bool g_state_call_phone; //是否处于打电话阶段

uint8 g_callin_ring_mode;
uint8 g_callin_ring_space_time;

uint32 g_keytone_conflict_cfg;

bool g_set_mic_mute = FALSE;//设置mic静音--远端没声音

uint8 dae_bypass_status = 0;
    
/*main中要填充这两个值，由系统人员分配*/
OS_STK *ptos = (OS_STK *) AP_FRONT_LOW_STK_POS;
INT8U prio = AP_FRONT_LOW_PRIO;

btcall_info_t g_btcall_cur_info;

bool g_background_call_flag = FALSE;//后台打电话，即属于嵌套子场景，打完电话后回到原先应用

/******************************************************************************
 * \par  Description:   读取应用VM全局变量,初始化全局变量
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/
static void _read_var(void)
{
    //读取common VM公共变量信息
    com_setting_comval_init(&g_comval);

    g_chf_cag_exch_enable = (uint8) com_get_config_default(BTCALL_FRONT_CHF_CAG_EXCH_ENABLE);

    g_callin_ring_mode = (uint8) com_get_config_default(BTCALL_CALLIN_RING_MODE);
    g_callin_ring_space_time = (uint8) com_get_config_default(BTCALL_CALLIN_RING_SPACE_TIME);
    if (g_callin_ring_space_time == 0)
    {
        g_callin_ring_space_time = 1; //至少1S
    }
}

/******************************************************************************
 * \par  Description:  applib初始化
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/
static void _app_init(void)
{
    /*读入VM变量*/
    _read_var();

    /*初始化applib库，前台AP*/
    applib_init(APP_ID_BTCALL, APP_TYPE_GUI);

    /*初始化软定时器*/
    init_app_timers(g_mainmenu_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    /*初始化 applib 消息模块*/
    applib_message_init();

    /*初始化系统计时器*/
    sys_timer_init();

    com_rcp_init();//必须在xxx_rcp_var_init之前

    com_view_manager_init();

    dae_bypass_status = get_dae_bypass_status();
    com_set_dae_onoff(FALSE);
}

/******************************************************************************
 * \par  Description:  applib退出
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/
void _app_exit(void)
{
    
    //libc_print("datebypass",dae_bypass_status,2);
    if(dae_bypass_status == 0)
    {
    	com_set_dae_onoff(TRUE);
    }
        
    com_view_manager_exit();

    /*备份系统计时器*/
    sys_timer_exit();

    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    /*执行applib库的注销操作*/
    applib_quit();
}

/******************************************************************************
 * \par  Description:  处理后台冲突问题，退出不是自己的后台
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

void kill_conflict_console(void)
{
    engine_type_e engine_type = get_engine_type();
    if (engine_type == ENGINE_NULL)
    {
        return;
    }

    if (engine_type != ENGINE_BTCALL)
    {
        msg_apps_t msg;

        msg.type = MSG_KILL_APP_SYNC;
        msg.content.data[0] = APP_ID_THEENGINE;

        send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
    }
}
/******************************************************************************
 * \par  Description:  创建后台
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

void create_console(void)
{
    msg_apps_t msg;
    msg_reply_t temp_reply;
    engine_type_e engine_type = get_engine_type();

    if (engine_type == ENGINE_NULL)
    {
        msg.content.data[0] = APP_ID_BTCALLEG;
        msg.content.data[1] = g_app_info_state.stub_tools_type;
        msg.type = MSG_CREAT_APP_SYNC;

        send_sync_msg(APP_ID_MANAGER, &msg, &temp_reply, 0);
    }
}

void kill_console(void)
{
    engine_type_e engine_type = get_engine_type();

    if (engine_type != ENGINE_NULL)
    {
        msg_apps_t msg;

        msg.type = MSG_KILL_APP_SYNC;
        msg.content.data[0] = APP_ID_THEENGINE;
        msg.content.data[1] = 0;

        send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
    }
}

/******************************************************************************
 * \par  Description:  处理低电退出的情况
 * \param[in]    null
 * \param[out]   none
 * \return
 * \note
 *   <author>    <time>
 *    Wekan   2015-10-30
 *******************************************************************************/

void quit_for_low_power(void)
{
    if (g_btcall_cur_info.status == BTCALL_HFP)
    {
        if (g_chf_cag_exch_enable == 1)//支持切换功能
        {
            hfp_change_to_phone();//音频切到手机
            PRINT_INFO("low power! hfp to phone.");
        }
    }
}

app_result_e att_btcall_quit_deal(void)
{
    com_btmanager_force_unlink();

    return RESULT_BLUETEETH_SOUND;

}
/******************************************************************************
 * \par  Description:  应用的入口函数和场景管理器模块
 * \param[in]    PARAM_FROM_XXX
 * \param[out]   none
 * \return          返回时，退出app
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-27
 *******************************************************************************/

int main(int argc, const char *argv[])
{
    app_result_e result = RESULT_NULL;

    g_standby_exit_flag = (bool) ((argc & PARAM_S3_EXIT) != 0);
    argc = argc & (~PARAM_S3_EXIT);

    if (argc == 1)
    {
        g_background_call_flag = TRUE;
    }

    /*ap初始化*/
    _app_init();

    //创建主视图
    g_last_btcall_status = g_btcall_cur_info.status = BTCALL_IDLE;

    btcall_create_main_view();

    kill_conflict_console();

    //if conflict we need disable keytone when phone calling
    g_keytone_conflict_cfg = com_get_config_default(SETTING_STUB_TEST_DEBUG_ENABLE);

    if (g_keytone_conflict_cfg == 1)
    {
        keytone_set_on_off(FALSE);
    }

    //解除静音并设置音量模式
    com_set_mute(FALSE);

    if ((g_background_call_flag == FALSE) || (g_app_info_vector[APP_TYPE_BTSTACK].used == 0))
    {
        if (g_standby_exit_flag == FALSE)
        {
            com_reset_sound_volume(0);

            com_tts_state_play((uint16) (TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK), (void*) TTS_ENTER_BTCALL);

            //创建蓝牙后台
            com_btmanager_init(FALSE);

            //等待TTS结束
            if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
            {
                sys_os_time_dly(10); //等待100ms，以便短按抬起能够完成
            }
        }
    }
    else
    {
        com_switch_volume_mode(VOLUME_MODE_CALL);
        com_set_volume_gain((uint8) com_get_config_default(SETTING_SOUND_CALL_VOLUME_GAIN));
        com_reset_sound_volume(SET_VOLUME_SYNC_TO_REMOTE);

        //打电话不能进行TTS播报
        com_tts_state_play_control(TTS_CTRL_IGNORE);
    }

    keytone_set_dac_chan(DAF1_EN); //切换按键音通道必须确保中间件不存在

    create_console();

    result = get_message_loop();
    sys_os_time_dly(5);//留一点时间给eg处理
    if ((result == RESULT_POWER_OFF) || (result == RESULT_LOW_POWER))//低电退出
    {
        quit_for_low_power();
    }
    
    com_switch_volume_mode(VOLUME_MODE_NORMAL);
    com_set_volume_gain((uint8) com_get_config_default(SETTING_SOUND_MAIN_VOLUME_GAIN));
    com_reset_sound_volume(0);

    kill_console();

    keytone_set_dac_chan(DAF0_EN); //切换按键音通道必须确保中间件不存在

    //退出打电话开启TTS播报
    com_tts_state_play_control(TTS_CTRL_CANCEL_IGNORE);

    //退出打电话开启keytone
    if (g_keytone_conflict_cfg == 1)
    {
        keytone_set_on_off(TRUE);
    }

    if (result == RESULT_SYSTEM_ENTER_S3)
    {
        PRINT_INFO("ap enter s3bt");
    }

    //ATT工具进入该ap，退出之后都是进入btplay应用
    if (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ATT_MODE)
    {
        result = att_btcall_quit_deal();
    }

    //create new app
    com_ap_switch_deal(result);

    _app_exit();

    return result;
}

