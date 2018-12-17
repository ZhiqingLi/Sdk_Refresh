/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：闹钟闹铃响及闹铃目录扫描功能入口，包括应用初始化，应用退出等。
 * 作者：cailizhen
 ********************************************************************************/

#include "ap_ota.h"


//系统全局变量
comval_t g_comval;
otaval_t g_otaval;

bool display_flag = FALSE;
int8 display_timer_id = -1;
uint8 g_ota_status = OTA_STATUS_IDLE;
bool g_need_draw = FALSE;
uint8 g_tick = 0;

//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     3

/*!
 *  \brief
 *  app_timer_vector：应用软定时器数组指针，指向应用空间的软定时器数组
 */
app_timer_t ota_app_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

OS_STK *ptos = (OS_STK *) AP_OTA_STK_POS;
INT8U prio = AP_OTA_PRIO;



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

void _load_vram_ota(void)
{
    sys_vm_read(&g_otaval, VM_OTA_UPGRADE, sizeof(otaval_t));
}

void _save_vram_ota(void)
{
    sys_vm_write(&g_otaval, VM_OTA_UPGRADE, sizeof(otaval_t));
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
	_load_vram_ota();
	
    //初始化applib库，前台AP
    applib_init(APP_ID_OTA_UPGRADE, APP_TYPE_GUI);

    applib_message_init();

    //初始化软定时器
    init_app_timers(ota_app_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

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


void sys_reboot(void)
{
    sys_local_irq_save();
    ENABLE_WATCH_DOG(1);
    while(1)
    {
        ;
    }
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
    app_result_e retval = RESULT_NULL;
	PRINT_INFO(" OTA_MAIN ");

    //初始化
    if (_app_init() == FALSE)
    {
        //提示出错(显示ERRO)
        goto ota_exit;
    }

	ota_create_main_view();
	
	//创建蓝牙后台
	com_btmanager_init(FALSE);
	
    retval = get_message_loop();
	
ota_exit:
	g_otaval.ota_upgrade_flag = 0;	// 清除
	_save_vram_ota();
	
	PRINT_INFO(" OTA_EXIT ");
	sys_reboot();
    return retval;
}


