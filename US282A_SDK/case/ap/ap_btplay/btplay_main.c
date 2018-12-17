/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙推歌前台应用入口，包括应用初始化，应用退出等。
 * 作者：cailizhen
 ********************************************************************************/

#include  "ap_btplay.h"

/*全局变量定义*/
//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     3

app_timer_t g_mainmenu_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];
//系统变量
comval_t g_comval;

uint32 g_btplay_check_btplay_status_timer;
btplay_status_e g_last_btplay_status_before_unlink;
uint8 g_btplay_autoplay_flag;

btplay_status_e g_last_btplay_status = BTPLAY_IDLE;
bool display_flag;
int8 display_timer_id = -1;
bool g_need_tts_play = FALSE;
bool g_need_draw = FALSE;
bool g_avrcp_tts_enable;

uint8 g_avrcp_autoplay_cnt = 0; //连接成功后自动发送avrcp播放的次数
uint8 g_avrcp_autoplay_flag = 0; //可以发送avrcp play自动播放
uint8 g_avrcp_autoplay_try = 0; //发送avrcp play的次数
uint8 g_send_avrcp_play_tick = 0; //延时2秒发送avrcp play计时
uint8 g_fake_play_tick = 0; //为了过滤某些手机回连时有2S的伪start状态
uint8 g_btplay_tick_250ms = 0;
int8 tick_timer_id = -1;
uint8 g_btplay_flag;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
//播报的音量添加
uint8 flag1;//标志播报主箱与手机的连接与断开
uint8 flag2=0;
uint8 flag_con_ok_one=0;
uint8 flag_pair;
uint8 dev_role_flag=0;
uint8 flag_no_tts=0;
uint8 tws_vol_sync_flag = 0;//tws设备间第一次同步音量
int8 tws_sync_timer_id = -1;

uint8 tws_phcon_sync_flag = 0;//tws设备间第一次手机连接通知
int8 tws_phcon_timer_id = -1;
uint8 tws_swith_flag = 0;

//tws下的播放处理
bool g_tws_need_tts_play = FALSE;
bool g_tws_need_tts_play1 = FALSE;

uint8 flag_ph_dis_master=0;
uint8 flag_dis_tws_dev=0;
uint8 tws_con_flag=0;
uint8 tws_discon_flag=0;
uint8 g_auto_btplay_flag = 0;
uint8 g_last_stack_play_status=0;
uint8 tws_volume=0;
uint8 g_last_role_save = 0;
#endif
uint32 g_last_change_state_time=0;
uint32 g_second_change_state_flag=0;

uint16 disc_sco_num = 0;

//初始化阶段，可以过滤掉一些事件处理
bool g_btplay_init_flag;

uint8 fast_play_flag = BTPLAY_FAST_NORMAL; //0表示正常，1表示快进，2表示快退

conn_status_e g_last_conn_status;

#if (LINK_TTS_SEL == 0)
uint8 g_connected_flag;
uint8 g_last_connected_flag;
#endif

/*main中要填充这两个值，由系统人员分配*/
OS_STK *ptos = (OS_STK *) AP_FRONT_LOW_STK_POS;
INT8U prio = AP_FRONT_LOW_PRIO;

btplay_info_t g_btplay_cur_info;
btplay_status_e g_btplay_enter_s3bt_status; //进入S3BT前状态
a2dp_status_e g_btplay_enter_s3bt_a2dp_status; //进入S3BT前A2DP服务状态
hfp_status_e g_btplay_enter_s3bt_hfp_status; //进入S3BT前HFP服务状态
uint8 g_btplay_s3bt_param_buffer[4]; //用来加载和保存参数的临时缓冲区

/******************************************************************************/
/*!
 * \par  Description:
 *    MainMenu 读取应用VM全局变量,初始化全局变量
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
static void _read_var(void)
{
    //读取common VM公共变量信息
    com_setting_comval_init(&g_comval);
    tws_volume=sys_comval->volume_current;

    g_avrcp_tts_enable = (uint8) com_get_config_default(BTPLAY_FRONT_AVRCP_TTS_ENABLE);
    if(g_comval.tts_enable==FALSE)
    {
        g_app_info_state_all.g_no_support_tts_flag=1;
    }
    g_avrcp_autoplay_cnt = (uint8) com_get_config_default(BTPLAY_FRONT_AUTOPLAY_CNT);

    //del  g_btplay_flag = (uint8) ((act_readl(RTC_BAK0) & (0x7 << MY_RTC_BTPLAY_FLAG)) >> MY_RTC_BTPLAY_FLAG);
    sys_vm_read(&g_config_var,VM_AP_CONFIG,sizeof(g_config_var_t));
    g_btplay_flag = g_config_var.engine_state;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    applib初始化
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
static void _app_init(void)
{
    /*读入VM变量*/
    _read_var();

    /*初始化applib库，前台AP*/
    applib_init(APP_ID_BTPLAY, APP_TYPE_GUI);

    /*初始化软定时器*/
    init_app_timers(g_mainmenu_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    /*初始化 applib 消息模块*/
    applib_message_init();

    /*初始化系统计时器*/
    sys_timer_init();

    com_rcp_init();//必须在xxx_rcp_var_init之前

    com_view_manager_init();

#ifdef SUPPORT_ASET_TEST
    if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
    {
        aset_test_init();
    }
#endif
    com_set_dae_chan(FALSE, TRUE);
}

/******************************************************************************/
/*!
 * \par  Description:
 *    applib退出
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      mainmenu
 * \note
 *******************************************************************************/
void _app_exit(void)
{
#ifdef SUPPORT_ASET_TEST
        if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
        {
            aset_test_exit();
        }
#endif

    com_view_manager_exit();

    /*备份系统计时器*/
    sys_timer_exit();

    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));

    /*执行applib库的注销操作*/
    applib_quit();
}

void kill_conflict_console(void)
{
    engine_type_e engine_type = get_engine_type();

    if (engine_type == ENGINE_NULL)
    {
        return;
    }

    if (engine_type != ENGINE_BTPLAY)
    {
        msg_apps_t msg;

        msg.type = MSG_KILL_APP_SYNC;
        msg.content.data[0] = APP_ID_THEENGINE;

        send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
    }
}

void create_console(void)
{
    msg_apps_t msg;
    msg_reply_t temp_reply;
    engine_type_e engine_type = get_engine_type();

    if (engine_type == ENGINE_NULL)
    {
        msg.content.data[0] = APP_ID_BTPLAYEG;
        msg.content.data[1] = 0;
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

#ifdef SUPPORT_BTT_TEST
void enter_btt_test(void)
{
    msg_apps_t msg;
    //消息类型(即消息名称)
    msg.type = MSG_BTSTACK_ENTER_BTT_TEST_SYNC;

    //发送同步消息
    send_sync_msg_btmanager(NULL, &msg, NULL, 0);
}
#endif

#ifdef ENABLE_TRUE_WIRELESS_STEREO
void tws_timer_kill(int8 *timer_id)
{
    if (*timer_id != -1)
    {
        kill_app_timer(*timer_id);
    }
    *timer_id = -1;
}
#endif

/******************************************************************************/
/*!
 * \par  Description:
 *    MainMenu 应用的入口函数和场景管理器模块
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

    g_standby_exit_flag = (bool) ((argc & PARAM_S3_EXIT) != 0);
    argc = argc & (~PARAM_S3_EXIT);

    /*ap初始化*/
      //PRINT_INFO("-d-");
    _app_init();

    if (g_standby_exit_flag == TRUE)
    {      
        if(sys_vm_read(g_btplay_s3bt_param_buffer, VM_S3BT_APP_STATUS, 3) == 0)
        {
            g_btplay_enter_s3bt_status = g_btplay_s3bt_param_buffer[0];
            g_btplay_enter_s3bt_a2dp_status = g_btplay_s3bt_param_buffer[1];
            g_btplay_enter_s3bt_hfp_status = g_btplay_s3bt_param_buffer[2];
        }
        else
        {
            g_standby_exit_flag = FALSE;    
        }
    }

    //创建主视图
    g_last_btplay_status = g_btplay_cur_info.status = BTPLAY_IDLE;
    btplay_create_main_view();

    kill_conflict_console();

    g_btplay_init_flag = TRUE;
    
   // bt_conn_states_init();

#ifdef ENABLE_TRUE_WIRELESS_STEREO
#if 1
    if( g_app_info_state_all.power_on_flag==1)
    {
        g_app_info_state_all.power_on_flag=0;
        tws_swith_flag = 0;
        libc_print("p_join:",0,0);
    }
    else
    {
        tws_con_flag= (( g_bt_stack_cur_info.tws_hfp_con_flag== 1) ||(g_bt_stack_cur_info.phonetomastercon == 1));
        tws_discon_flag=(( g_bt_stack_cur_info.tws_hfp_con_flag== 0) &&(g_bt_stack_cur_info.phonetomasterdis== 1));
        flag1=tws_con_flag;
        flag_pair=g_bt_stack_cur_info.pair;
        flag_dis_tws_dev=g_bt_stack_cur_info.tws_dev_discon;
        //flag_ph_dis_master=g_bt_stack_cur_info.phonetomasterdis;
        flag_ph_dis_master=tws_discon_flag;  
        tws_swith_flag = 1; 
    }
    //主箱控制从箱切进应用
    //将变量清0
    g_tws_m_switchap_flag = 0; 
#endif
#endif

    //解除静音并设置音量模式
    com_set_mute(FALSE);
    com_reset_sound_volume(SET_VOLUME_SYNC_TO_REMOTE);

    if ((g_ap_switch_var.call_background_status != CALL_BACK_STATUS_BACK) && (g_standby_exit_flag == FALSE))
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO        
        if (g_bt_stack_cur_info.dev_role != TWS_SLAVE)
#endif
        {
            com_tts_state_play(TTS_MODE_ONLYONE | TTS_MODE_NOBLOCK, TTS_ENTER_BTPLAY);
        }
        //创建蓝牙后台
        com_btmanager_init(FALSE);
        //等待TTS结束
        if (com_tts_state_play_wait() == TTS_PLAY_RET_BY_KEY)
        {
            sys_os_time_dly(10); //等待100ms，以便短按抬起能够完成
        }
    }
    bt_conn_states_init();
#if 0
#ifdef ENABLE_TRUE_WIRELESS_STEREO
   // flag1=g_bt_stack_cur_info.phonetomastercon;
   if( g_app_info_state_all.power_on_flag==1)
   {
        g_app_info_state_all.power_on_flag=0;
    }
   else
   {
        tws_con_flag= (( g_bt_stack_cur_info.tws_hfp_con_flag== 1) ||(g_bt_stack_cur_info.phonetomastercon == 1));
        tws_discon_flag=(( g_bt_stack_cur_info.tws_hfp_con_flag== 0) &&(g_bt_stack_cur_info.phonetomasterdis== 1));
        flag1=tws_con_flag;
        flag_pair=g_bt_stack_cur_info.pair;
        flag_dis_tws_dev=g_bt_stack_cur_info.tws_dev_discon;
        //flag_ph_dis_master=g_bt_stack_cur_info.phonetomasterdis;
        flag_ph_dis_master=tws_discon_flag;  
    }

#endif
#endif

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
 #ifdef ENABLE_TRUE_WIRELESS_STEREO
            if (g_bt_stack_cur_info.dev_role == TWS_SLAVE)
            {
                //从箱切换应用,通知主箱暂停音乐播放
                send_cmd_to_other(1);
                
            }
            
/*            if (g_bt_stack_cur_info.dev_role != 0)
            {
                uint8 val_tmp = 3;
                btplay_tws_sync_deal(val_tmp);                
            }
*/            
 #endif
            goto btplay_main_exit;
            // PRINT_INFO("-a-");
        }
        else
        {
            ; //nothing
        }
    }

    g_btplay_init_flag = FALSE;

#ifdef SUPPORT_BTT_TEST
    if (g_app_info_state.stub_tools_type == STUB_PC_TOOL_BTT_MODE)
    {
        while (1)
        {
            if (g_bt_stack_cur_info.conn_status != CONN_STATUS_NONE)
            {
                enter_btt_test();
                break;
            }

            sys_os_time_dly(1);

            result = com_view_loop(); //更新 g_bt_stack_cur_info 状态
            if (result> RESULT_COMMON_RESERVE)
            {
                goto btplay_main_exit;
            }
        }
    }
#endif

    keytone_set_dac_chan(DAF1_EN); //切换按键音通道必须确保中间件不存在
    create_console();
#ifdef ENABLE_TRUE_WIRELESS_STEREO
#if 0	

    if((g_ap_switch_var.call_background_status == CALL_BACK_STATUS_BACK)&&(g_ap_switch_var.tws_phon_come_flag==1))
    {
        //g_ap_switch_var.tws_phon_come_flag=0;
        com_btmanager_call_back_auto_connect();
        flag_no_tts=1;
    }
#endif
    //副箱从别的应用切回来，如果这时候还处在对箱状态 ，需要通知主箱
    //
    if ((g_standby_exit_flag ==FALSE)&&(g_bt_stack_cur_info.dev_role == TWS_SLAVE))
    {
        send_cmd_to_other(2);
        g_last_role_save = g_bt_stack_cur_info.dev_role;
    }

    if ((g_standby_exit_flag ==FALSE)
    	   &&(g_bt_stack_cur_info.dev_role == TWS_MASTER)
    	   &&(tws_swith_flag == 1))
    {
        uint8 val_tmp = 4;
        btplay_tws_sync_deal(val_tmp);
    }
#endif
    
    result = get_message_loop();

    kill_console();

    keytone_set_dac_chan(DAF0_EN); //切换按键音通道必须确保中间件不存在

    btplay_main_exit:

    if (result == RESULT_SYSTEM_ENTER_S3)
    {
        sd_sec_param_t sd_sec_param;

        libc_memset(SRAM_S3BT_BUFFER, 0x0, 0x200);

        //if (g_ap_switch_var.s3bt_nor_erase_flag == FALSE)
        //{
        //    sd_sec_param.file_offset = NOR_S3BT_ERASE_WRITE;
        //    sd_sec_param.sram_addr = g_btplay_s3bt_param_buffer;
        //    sd_sec_param.sec_num = 1;
        //    base_ext_vram_write(&sd_sec_param);

        //    g_ap_switch_var.s3bt_nor_erase_flag = TRUE;
        //}

        g_btplay_s3bt_param_buffer[0] = g_btplay_cur_info.status;
        g_btplay_s3bt_param_buffer[1] = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.a2dp_active_id].a2dp_status;
        g_btplay_s3bt_param_buffer[2] = g_bt_stack_cur_info.rmt_dev[g_bt_stack_cur_info.hfp_active_id].hfp_status;

        //sd_sec_param.file_offset = NOR_S3BT_APP_STATUS;
        //sd_sec_param.sram_addr = g_btplay_s3bt_param_buffer;
        //sd_sec_param.sec_num = 1;
        //base_ext_vram_write(&sd_sec_param);

        sys_vm_write(g_btplay_s3bt_param_buffer, VM_S3BT_APP_STATUS, 3);

        PRINT_INFO("ap enter s3bt");
    }
    //create new app
    com_ap_switch_deal(result);

    _app_exit();

    return result;
}
