/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：music相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#include "music.h"
#include <uhost.h>

//music_maic.c和music.h, music_common*.c是应用级的
//music_play*.*是播放视图，是主视图
//music_ab*.*是处理AB视图的
//music_digit_song*.*是数字选歌
//music_hint*.*是提示信息
//music_loop_mode*.*是循环模式
//music_song_number*.*是曲目号
//musc_rcp*.*是处理rcp相关的

//进入music的方式
app_param_e g_enter_mode;

//初始化阶段，可以过滤掉一些事件处理
bool g_musicplay_init_flag;

//主动切歌到第几首
uint16 g_manul_switch_song_seq;

//系统全局变量
comval_t g_comval;

//保存扫描线程句柄
void *g_scan_handle = NULL;

//扫描标记
uint8 g_scan_flag = 0;

#ifdef PRINT_BANK_INFO
//打印标志位
int print_label;
#endif

//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     (3)
//应用软定时器数组指针，指向应用空间的软定时器数组
app_timer_t music_app_timer_vector[COMMON_TIMER_COUNT + APP_TIMER_COUNT];

bool g_music_restore_from_s3bt;

MUS_STATIC void _load_cfg(void)
{
    //系统变量获取
    com_setting_comval_init(&g_comval);

    //不用恢复AB播放
    //重复模式、随机播放要恢复

    sys_vm_read(&g_music_config, VM_AP_MUSIC, sizeof(music_config_t));
    if (g_music_config.magic != VRAM_MAGIC(VM_AP_MUSIC))
    {
        g_music_config.magic = VRAM_MAGIC(VM_AP_MUSIC);
        //循环模式
        if ((g_enter_mode == PARAM_RECORD_UPLAY) || (g_enter_mode == PARAM_RECORD_CPLAY))
        {
            g_music_config.repeat_mode = FSEL_MODE_LOOPONEDIR;
        }
        else
        {
            g_music_config.repeat_mode = FSEL_MODE_LOOPALL;
        }

        sys_vm_write(&g_music_config, VM_AP_MUSIC, sizeof(music_config_t));
    }
}

//保存配置信息
void _save_cfg(void)
{
    sys_vm_write(&g_music_config, VM_AP_MUSIC, sizeof(music_config_t));
    sys_vm_write(&g_comval, VM_AP_SETTING, sizeof(comval_t));
}

bool _app_init(void)
{
    //读取配置信息
    _load_cfg();
    //循环模式
    if (g_music_config.repeat_mode < MIN_LOOP_MODE)
    {
        g_music_config.repeat_mode = DEF_LOOP_MODE;
    }
    libc_memset(&g_rcp_file_status, 0, sizeof(g_rcp_file_status));

    //初始化applib库，前台AP
    applib_init(APP_ID_MUSIC, APP_TYPE_GUI);

    applib_message_init();

    //初始化软定时器
    init_app_timers(music_app_timer_vector, COMMON_TIMER_COUNT + APP_TIMER_COUNT);

    //必须在init_app_timers 后面调用
    sys_timer_init();

    com_rcp_init();//必须在xxx_rcp_var_init之前
    //初始化RCP回调函数
    music_rcp_var_init();

    com_view_manager_init();

#ifdef SUPPORT_ASET_TEST
    if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) 
        || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
    {
        aset_test_init();
    }
#endif
    com_set_dae_chan(FALSE, FALSE);
    return TRUE;
}

//保存配置信息
bool _app_deinit(void)
{
#ifdef SUPPORT_ASET_TEST
    if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) 
        || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
    {
        aset_test_exit();
    }
#endif

    //注销RCP回调函数
    music_rcp_var_exit();

    com_view_manager_exit();

    //备份系统计时器
    sys_timer_exit();

    //执行applib库的注销操作
    applib_quit();

    //写vram
    _save_cfg();

    return TRUE;
}

//入口参数说明：
//argc:
//  PARAM_RECORD_CPLAY: 卡上录音播放
//  PARAM_RECORD_UPLAY: U上录音播放
//  PARAM_MUSIC_SDCARD: 卡播放
//  PARAM_MUSIC_UHOST: U播放
//  注：闹钟播放不进入这里
int main(int argc, const char *argv[])
{
    app_result_e retval = RESULT_NEXT_FUNCTION;

    g_standby_exit_flag = (bool) ((argc & PARAM_S3_EXIT) != 0);
    g_music_restore_from_s3bt = g_standby_exit_flag;
    argc = argc & (~PARAM_S3_EXIT);

    g_esd_restart_flag = (bool) ((act_readl(RTC_BAK0) & (1 << MY_RTC_ESD_FLAG)) != 0);

    g_enter_mode = argc;

    //要播放U盘还是卡
    if ((argc == PARAM_MUSIC_UHOST) || (argc == PARAM_RECORD_UPLAY))
    {
        g_file_path_info.file_path.dirlocation.disk = DISK_U;
    }
    else
    {
        g_file_path_info.file_path.dirlocation.disk = DISK_H;
    }

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    discrete_led_set(LED_ID_MUSIC_PLAY, LED_ALWAYS_ON, NULL);
#endif

    //初始化
    _app_init();
    if ((com_get_config_default(SETTING_APP_SUPPORT_CARDPLAY)
            | com_get_config_default(SETTING_APP_SUPPORT_UHOSTPLAY)) != 0)
    {
        retval = music_scene_play();
    }

    //如果是录音播放,则对于进入下一个的处理变成回到上一次播放
    if (retval == RESULT_NEXT_FUNCTION)
    {
        if ((g_enter_mode == PARAM_RECORD_CPLAY) || (g_enter_mode == PARAM_RECORD_UPLAY))
        {
            retval = RESULT_LASTPLAY;
        }
    }

    if (retval == RESULT_SYSTEM_ENTER_S3)
    {
        PRINT_INFO("ap enter s3bt");
    }

    com_ap_switch_deal(retval);

    //退出
    _app_deinit();

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    discrete_led_set(LED_ID_MUSIC_PLAY, LED_ALWAYS_OFF, NULL);
#endif

    PRINT_DBG_INT("music exit:", retval);
    return retval;
}
