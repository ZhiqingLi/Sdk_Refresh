/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟闹铃响及闹铃目录扫描功能入口，包括应用初始化，应用退出等。
 * 作者：cailizhen
 ********************************************************************************/

#include "app_alarm.h"


//系统全局变量
comval_t g_comval;

//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     3

/*!
 *  \brief
 *  app_timer_vector：应用软定时器数组指针，指向应用空间的软定时器数组
 */
app_timer_t alarm_app_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

int32 g_fs_mount_id = -1;
uint32 g_dir_file_total;
file_path_info_t g_alarm_path;

time_t g_next_alarm_time;
uint8 g_next_alarm_index;
bool g_has_next_alarm;
bool g_next_alarm_already_ring;

alarm_ring_var_t g_alarm_ring_var;
alarm_ring_var_t *g_p_alarm_ring;

OS_STK *ptos = (OS_STK *) AP_ALARM_STK_POS;
INT8U prio = AP_ALARM_PRIO;
sdd_open_arg_t g_sdd_open_parm;
uint32 g_display_sec = 0;


/******************************************************************************/
/*!
 * \par  Description:
 * \void _load_cfg(void)
 * \读取配置信息
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \ingroup      music_main
 * \note
 */
/*******************************************************************************/
void _load_cfg(void)
{
    //系统变量获取
    com_setting_comval_init(&g_comval);
}

/******************************************************************************/
/*!
 * \par  Description:
 * \void _save_cfg(void)
 * \保存配置信息
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \ingroup      music_main
 * \note
 */
/*******************************************************************************/
void _save_cfg(void)
{
    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool _app_init(void)
 * \读取配置信息
 * \param[in]    void  para1
 * \param[out]   none
 * \return       int the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      music_main
 * \note
 * \li  初始化文件选择器时设置默认盘符为主盘，文件类型为COMMONDIR
 * \li  如果后台有music播放或从browser等进入，同步location
 * \li  music ui默认没有盘符记忆，从playlist进入和退出，盘符写入browser VM变量
 */
/*******************************************************************************/
bool _app_init(void)
{
    //读取配置信息 (包括vm信息)
    _load_cfg();

    //初始化applib库，前台AP
    applib_init(APP_ID_ALARM, APP_TYPE_GUI);

    applib_message_init();

    //初始化软定时器
    init_app_timers(alarm_app_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    //必须在init_app_timers 后面调用
    sys_timer_init();

    com_rcp_init();//必须在xxx_rcp_var_init之前

    com_view_manager_init();

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \bool _app_deinit(void)
 * \退出app的功能函数 保存配置信息
 * \param[in]    void  para1
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      music_main
 * \note
 */
/*******************************************************************************/
bool _app_deinit(void)
{
    com_view_manager_exit();

    //备份系统计时器
    sys_timer_exit();

    //执行applib库的注销操作
    applib_quit();

    //save config 写vram
    _save_cfg();

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \int main(int argc  const char *argv[])
 * \app入口函数
 * \param[in]    argc  para1
 * \param[in]    argc  para2
 * \param[out]   none
 * \return       int the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup      music_main
 * \note
 */
/*******************************************************************************/
int main(int argc, const char *argv[])
{
    app_result_e retval;

    g_standby_exit_flag = (bool) ((argc & PARAM_S3_EXIT) != 0);
    argc = argc & (~PARAM_S3_EXIT);

    //g_p_alarm_ring = &g_alarm_ring_var;
    //g_p_alarm_ring->cur_alarm_record.alarm.ring_type = RING_TYPE_BUILTIN;

    //初始化
    if (_app_init() == FALSE)
    {
        //提示出错(显示ERRO)
        retval = RESULT_NEXT_FUNCTION;
        goto alarm_exit;
    }
    retval = alarm_ring_entry();
    alarm_exit:

    if (retval == RESULT_SYSTEM_ENTER_S3)
    {
        PRINT_INFO("ap enter s3bt");
    }
    retval = RESULT_NEXT_FUNCTION;
    com_ap_switch_deal(retval);

    //退出
    _app_deinit();
    return retval;
}
