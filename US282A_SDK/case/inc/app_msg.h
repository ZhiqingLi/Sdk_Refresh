/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：应用私有消息和系统消息相关宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _app_msg_H_
#define _app_msg_H_

#include "psp_includes.h"
#include "case_type.h"

/*!
 * \brief
 *  msg_apps_type_e 应用消息枚举类型
 */
typedef enum
{
    /*! 空消息 */
    MSG_NULL = 0x0000,

    /*!进入蓝牙推歌*/
    EVENT_ENTER_BTPLAY,
    /*!进入MUSIC播歌*/
    EVENT_ENTER_MUSIC,
    /*!进入LINEIN播歌*/
    EVENT_ENTER_LINEIN,
    /*!进入fm收音*/
    EVENT_ENTER_RADIO,
    /*!进入闹钟设置，仅用于手机APK切换进入*/
    EVENT_ENTER_ALARM_RCP,
    /*!进入插卡播歌*/
    EVENT_ENTER_MUSIC_CARD,
    /*!进入U盘播歌*/
    EVENT_ENTER_MUSIC_UDISK,
    /*!进入card录音文件播放*/
    EVENT_RECORD_CPLAY_RCP,
    /*!进入udisk录音文件播放*/
    EVENT_RECORD_UPLAY_RCP,
    /*!进入mic录音*/
    EVENT_ENTER_CRECORD_RCP,
    /*!进入mic录音*/
    EVENT_ENTER_URECORD_RCP,
    /*!进入USB音箱*/
    EVENT_ENTER_USOUND,

    /******************************system message****************************/
    MSG_RTC2HZ = MSG_SYS_RTC2HZ, /*0x80*/
    MSG_BAT_V_CHG = MSG_SYS_BAT_V_CHG, /*0x81*/
    MSG_LOW_POWER = MSG_SYS_LOW_POWER, /*0x82*/
    MSG_RTCALARM = MSG_SYS_RTCALARM, /*0x83*/
    MSG_POWER_OFF = MSG_SYS_POWER_OFF, /*0x84*/
    MSG_RECLAIM = MSG_SYS_RECLAIM, /*0x85*/
    MSG_DSP_RESET = MSG_SYS_DSP_RESET, /*0x86*/
    MSG_MPU_ERROR = MSG_SYS_MPU_ERROR, /*0x87*/
    MSG_USB_STICK = MSG_SYS_USB_STICK, /*0x90*/
    MSG_USB_UNSTICK = MSG_SYS_USB_UNSTICK, /*0x91*/
    MSG_ADAPTOR_IN = MSG_SYS_ADAPTOR_IN, /*0xa0*/
    MSG_ADAPTOR_OUT = MSG_SYS_ADAPTOR_OUT, /*0xa1*/
    MSG_SD_IN = MSG_SYS_SD_IN, /*0xb0*/
    MSG_SD_OUT = MSG_SYS_SD_OUT, /*0xb1*/
    MSG_UH_IN = MSG_SYS_UH_IN, /*0xc0*/
    MSG_UH_OUT = MSG_SYS_UH_OUT, /*0xc1*/
    MSG_LINEIN_IN = MSG_SYS_LINEIN_IN, /*0xd0*/
    MSG_LINEIN_OUT = MSG_SYS_LINEIN_OUT, /*0xd1*/
    MSG_HP_IN = MSG_SYS_HP_IN, /*0xe0*/
    MSG_HP_OUT = MSG_SYS_HP_OUT, /*0xe1*/

    /*! 充电已满消息 */
    MSG_FULL_CHARGE = 0xf0,

    /*! 通知应用程序退出消息，应用程序接受到此消息后，进入退出流程，该消息只能由 manager.app 发送 */
    MSG_APP_QUIT,

    /*! 进入TTS播报 */
    MSG_TTS_START_SYNC,
    /*! TTS播报完成 */
    MSG_TTS_STOP_SYNC,

    /*! 开始切换音频输出 */
    MSG_SWITCH_AOUT_START_SYNC,
    /*! 完成切换音频输出 */
    MSG_SWITCH_AOUT_STOP_SYNC,

    /*! 设置DAE参数 */
    MSG_SET_DAE_CFG_SYNC,

    //报曲目号
    MSG_MUSIC_SONG_NUM_TTS,

    MSG_ASQT_TEST,//ASQT调试

    /*! 系统进入S2低功耗 */
    MSG_SYSTEM_ENTER_S2,
    /*! 引擎进入STANDBY */
    MSG_ENGINE_STANDBY_SYNC,
    /*! 系统进入S3低功耗 */
    MSG_SYSTEM_ENTER_S3,
    /*! 进入空闲模式 */
    MSG_IDLE_MODE,

    /*! 蓝牙相关消息： */
    /*! 打电话状态变化消息 */
    MSG_BT_HFP_CALL = 0x140,
    /*! 蓝牙设备连接消息 */
    MSG_BT_CONNECTED,
    /*! 蓝牙设备断开连接消息 */
    MSG_BT_DISCONNECT,

    /******************************manager message****************************/
    /*! process manager消息占位符*/
    MANAGER_BASE = 0x180,
    /*! 异步创建应用程序消息 */
    MSG_CREAT_APP,
    /*! 同步创建应用程序消息 */
    MSG_CREAT_APP_SYNC,
    MSG_CREAT_APP_EXT_SYNC,
    /*! 异步杀死应用程序消息 */
    MSG_KILL_APP,
    /*! 同步杀死应用程序消息 */
    MSG_KILL_APP_SYNC,
    /*! 加载stub驱动 */
    MSG_STUB_INSTALL_SYNC,

    /***************************Music engine message**************************/
    /*! Music engine消息占位符 */
    MENGINE_BASE = 0x200,
    /*! 获取引擎信息，音乐播放界面定时获取引擎信息 */
    MSG_MENGINE_GET_ENGINE_INFO_SYNC,
    /*! 获取当前文件路径 */
    MSG_MENGINE_GET_FILEPATH_SYNC,
    /*! 设置播放模式 */
    MSG_MENGINE_SET_PLAYMODE_SYNC,
    /*! 设置显示模式 */
    MSG_MENGINE_SET_DISPMODE_SYNC,
    /*! 设置淡入淡出 */
    MSG_MENGINE_SET_FADE_SYNC,
    /*! 设置播放速度 */
    MSG_MENGINE_SET_PLAYSPEED_SYNC,
    /*! 设置AB复读模式: 自动或手动 */
    MSG_MENGINE_SET_AB_AUTO_SYNC,
    /*! 设置AB复读次数 */
    MSG_MENGINE_SET_AB_COUNT_SYNC,
    /*! 设置AB复读时间间隔 */
    MSG_MENGINE_SET_AB_TIME_SYNC,
    /*! 开始AB复读 */
    MSG_MENGINE_START_AB_SYNC,
    /*! 取消AB复读 */
    MSG_MENGINE_CLEAR_AB_SYNC,
    /*! 设置AB复读下一步: A点--> B点 */
    MSG_MENGINE_SET_AB_NEXT_SYNC,
    /*! 设置AB复读上一步: A点<-- B点 */
    MSG_MENGINE_SET_AB_PREV_SYNC,
    /*! 播放音乐 */
    MSG_MENGINE_PLAY_SYNC,
    /*! 停止音乐 */
    MSG_MENGINE_STOP_SYNC,
    /*! 暂停音乐 */
    MSG_MENGINE_PAUSE_SYNC,
    /*! 恢复音乐 */
    MSG_MENGINE_RESUME_SYNC,
    /*! 跳到指定时间播放音乐 */
    MSG_MENGINE_SEEK_SYNC,
    /*! 设置音乐播放文件(文件选择器方式) */
    MSG_MENGINE_SET_FILEPATH_SYNC,
    /*! 设置音乐播放文件(playlist方式)  */
    MSG_MENGINE_SET_FILEINDEX_SYNC,
    /*! 切换到下一首 */
    MSG_MENGINE_PLAY_NEXT_SYNC,
    /*! 切换到上一首 */
    MSG_MENGINE_PLAY_PREV_SYNC,
    /*! 删除文件 */
    MSG_MENGINE_DELETE_FILE_SYNC,
    /*! 快进 */
    MSG_MENGINE_FFWD_SYNC,
    /*! 快退 */
    MSG_MENGINE_FBWD_SYNC,
    /*! 取消快进或快退 */
    MSG_MENGINE_CANCEL_FFB_SYNC,
    /*! 设置快进或快退步长 */
    MSG_MENGINE_SET_FFBSTEP_SYNC,
    /*! 设置快进或快退播放时间 */
    MSG_MENGINE_SET_FFBTIME_SYNC,
    /*! 获取当前时间的书签信息 */
    MSG_MENGINE_MAKE_BOOKMARK_SYNC,
    /*! 设置上次播放文件 */
    MSG_MENGINE_SET_LAST_FILE_SYNC,
    /*! 清除错误状态: UI应用处理后清除 */
    MSG_MENGINE_CLEAR_ERR_SYNC,
    /*! 前台在合适时机主动清除FILESWITCH标志 */
    MSG_MENGINE_CLEAR_FILESWITCH_SYNC,
    /*! 删除收藏夹，更改收藏夹序号和总数 */
    MSG_MENGINE_DEL_FAVORITE_SYNC,
    /*! 添加收藏夹，更改收藏夹总数 */
    MSG_MENGINE_ADD_FAVORITE_SYNC,
    //for audible
    /*! 切换到下一首 */
    MSG_MENGINE_CAHPTER_NEXT_SYNC,
    /*! 切换到上一首 */
    MSG_MENGINE_CHAPTER_PREV_SYNC,
    /*! 获取当前的图片信息 */
    MSG_MENGINE_AUDIBLE_GETIMAGE_SYNC,
    //===========music ui 新增消息 ======================

    /*! 获取信息 */
    MSG_MENGINE_GET_ENGINE_CFG_SYNC,
    /*! 获取循环模式+shuffle */
    MSG_MENGINE_GET_PLAYMODE_SYNC,
    /*! 获取播放模式 */
    MSG_MENGINE_GET_ENGINE_PLAYMODE_SYNC,
    /*! 获取文件格式 */
    MSG_MENGINE_GET_FILEINFO_SYNC,
    /*! 获取当前播放状态 */
    MSG_MENGINE_GET_STATUS_SYNC,
    /*! 获取当前播放时间和比特率 */
    MSG_MENGINE_GET_PLAYINFO_SYNC,
    /*! 获取当前播放时间断点 */
    MSG_MENGINE_GET_BKINFO_SYNC,
    /*! 获取当前章节总数和序数 */
    MSG_MENGINE_GET_SECTION_INFO_SYNC,
    /*! 设置audible章节模式 */
    MSG_MENGINE_SET_SECTION_MODE_SYNC,
    /*! 设置AB复读间隔 */
    MSG_MENGINE_SET_AB_GAP_SYNC,
    /*! 设置AB复读A点 */
    MSG_MENGINE_SET_APOINT_SYNC,
    /*! 设置AB复读B点 */
    MSG_MENGINE_SET_BPOINT_SYNC,
    /*! 播放书签 */
    MSG_MENGINE_PLAY_BOOKMARK_SYNC,
    /*! music播放场景删除文件 */
    MSG_MENGINE_DELFILE_MUSUI_SYNC,
    /*! music播放场景删除文件 */
    MSG_MENGINE_DELFILE_OTER_SYNC,
    /*! music改变文件播放路径 */
    MSG_MENGINE_CHANGE_FILEPATH_SYNC,
    /*! music standby播歌 */
    MSG_MENGINE_STANDBY_PLAY_SYNC,
    /*! 切换磁盘后插先播 */
    MSG_MENGINE_SWITCH_DISK_SYNC,
    /*! 按list方式切换歌曲 */
    MSG_MENGINE_LIST_ACCESS_ALLOW_SYNC,
    /*! 数字点歌 */
    MSG_MENGINE_DIGITAL_SONG_SYNC,
    /*! folder切换下一首歌曲 */
    MSG_MENGINE_FOLFER_NEXT_SYNC,
    /*! folder切换上一首歌曲 */
    MSG_MENGINE_FOLFER_PREV_SYNC,
    /*! 获取当前播放模式 */ //0--music,1--sdfile,2--linein
    MSG_MENGINE_GET_PLAYTYPE_SYNC,

    /*! 获取id3信息 */
    MSG_MENGINE_GET_ID3INFO_SYNC,
    /*! 获取目录名信息 */
    MSG_MENGINE_GET_DIRNAME_SYNC,
    /*! 获取文件名称 */
    MSG_MENGINE_GET_FILENAME_SYNC,

    /*! 设置排序后的序号 */
    MSG_MENGINE_SET_SORTNUMBER_SYNC,

    /*! 获取当前序号 */
    MSG_MENGINE_GET_DISKNUMBER_SYNC,
    //保存断点信息
    MSG_MENGINE_SAVE_BKINFO_SYNC,
    /*! 清除断点错误标志 */
    MSG_MENGINE_CLR_BKERR_SYNC,
    //SD卡断电重上电
    MSG_MENGINE_SD_PW_RESET_SYNC,
    MSG_MENGINE_SET_FIO_SYNC,

    /****************************fm engine message****************************/
    /*! fm engine消息占位符*/
    FMENGINE_BASE = 0x300,

    /* 初始化及卸载相关*/
    /* 安装FM drv ,  执行FM 引擎进程后即安装*/
    MSG_FMENGINE_OPEN,
    /* FM 模组打开*/
    MSG_FM_MODE_OPEN,
    /* FM 模拟通道打开*/
    MSG_FM_AIN_OPEN,
    /* FM 驱动IC  初始化配置= MSG_FM_MODE_OPEN + MSG_FM_A_CHANNEL_OPEN*/
    MSG_FMENGINE_INIT,
    /* FM 驱动IC 进入Standby 状态*/
    MSG_FMENGINE_STANDBY,
    /* 接收到此命令后，卸载FM 驱动，并退出引擎*/
    MSG_FMENGINE_CLOSE,

    /* 设置相关*/
    /* 设置频点播放( 异步) */
    MSG_FMENGINE_SETFREQ,
    /* 设置频点播放( 同步)  */
    MSG_FMENGINE_SETFREQ_SYNC,
    /* 设置静音或取消静音*/
    MSG_FMENGINE_SETMUTE,
    /* 设置电台波段*/
    MSG_FMENGINE_SETBAND,
    /* 设置搜台门限值*/
    MSG_FMENGINE_SETTHRED,
    /* 启动软件搜台*/
    MSG_FMENGINE_SET_SOFTSEEK,
    /* 启动硬件搜台*/
    MSG_FMENGINE_SET_HARDSEEK,
    /* 退出硬件搜台*/
    MSG_FMENGINE_SET_SEEKBREAK,

    /* 获取信息相关*/
    /* 获取硬件自动搜台是否完成，对应Engine_Status_t.STC_flag */
    MSG_FMENGINE_AUTO_SEEK_OVER,
    /* 获取全部状态信息，对应Engine_Status_t  结构*/
    MSG_FMENGINE_GET_STATUS,
    /* 获取当前波段，对应Engine_Status_t.FM_CurBand */
    MSG_FMENGINE_GET_BAND,
    /* 获取当前频率，对应Engine_Status_t.FM_CurrentFreq */
    MSG_FMENGINE_GET_FREQ,
    /* 获取电台信号强度，对应Engine_Status_t.FM_SignalStg */
    MSG_FMENGINE_GET_INTENSITY,
    /* 获取天线状态( 耳机是否插入) */
    MSG_FMENGINE_GET_ANTENNA,
    /* 获取立体声状态信息，对应Engine_Status_t.FM_Stereo_Status */
    MSG_FMENGINE_GET_STEREO,
    /* 获取引擎播放状态信息，对应Engine_Status_t.FM_VolStatus */
    MSG_FMENGINE_GET_PLAYSTATUS,
    /* 获取当前模组状态*/
    MSG_FMENGINE_GET_MODUAL_STATUS,
    /* 重建FM模组全局数组内容*/
    MSG_FMENGINE_RESINTALL,
    /*FM play*/
    MSG_FMENGINE_PLAY,
    /*FM pause*/
    MSG_FMENGINE_PAUSE,
    //调试用
    MSG_FMENGINE_DEBUG,

    /****************************autotest message****************************/
    /*! auto test消息占位符 */
    AUTOTEST_BASE = 0x400,
    MSG_AUTOTEST_CREAT_APP_EXT_SYNC,
    MSG_AUTOTEST_SET_PAIRED_DEV_SYNC,
    MSG_AUTOTEST_GET_BTSTACK_STATUS_SYNC,
    MSG_AUTOTEST_CLEAR_BTSTACK_ERR_SYNC,
    MSG_AUTOTEST_HFP_CONNECT_SYNC,
    MSG_AUTOTEST_A2DP_CONNECT_SYNC,
    MSG_AUTOTEST_QUERY_BTSTACK_WORK_SYNC,
    MSG_AUTOTEST_SET_SHARE_DATA_SYNC,
    MSG_AUTOTEST_BTT_TEST_SYNC,
    MSG_AUTOTEST_SWITCH_APP,
    MSG_AUTOTEST_SET_PAIRED_DEV2_SYNC,
    MSG_AUTOTEST_GET_PAIRED_DEV2_SYNC,
    /****************************linein message****************************/
    //LINEIN引擎的消息
    LINEIN_EG_BASE = 0x500,
    //得到状态
    MSG_LINEIN_EG_GET_STATUS_SYNC,
    //播放
    MSG_LINEIN_EG_PLAY_SYNC,
    //停止
    MSG_LINEIN_EG_STOP_SYNC,
     //salve now linein
    MSG_LINEIN_IN_STATUS_SYNC,

    /****************************** BTSTACK ******************************/
    /****************************** BTSTACK A2DP/AVRCP ******************************/
    /*注意：为了更好进行bank划分，要求所有A2DP/AVRCP相关的消息必须分配在[0x600-0x6ff]*/
    MSG_BTSTACK_AVRCP_BASE = 0x600,
    MSG_BTSTACK_AVRCP_TX_OPID_SYNC = 0x601,
    MSG_BTSTACK_AVRCP_VOLUME_SYNC = 0x602,
    MSG_BTSTACK_A2DP_CONNECT_SYNC = 0x603,
    MSG_BTSTACK_A2DP_DISCONNECT_SYNC = 0x604,
    MSG_BTSTACK_AVRCP_CONNECT_SYNC = 0x605,
    MSG_BTSTACK_AVRCP_DISCONNECT_SYNC = 0x606,
#ifdef __BQB_TEST_PROFILE_
    MSG_BTSTACK_DELAY_REPORT_SYNC = 0x607,
#endif
    /****************************** BTSTACK HFP ******************************/
    /*注意：为了更好进行bank划分，要求所有HFP相关的消息必须分配在[0x700-0x7ff]*/
    MSG_BTSTACK_HFP_BASE = 0x700,
    MSG_BTSTACK_HFP_CALL_REQ_SYNC = 0x701,
    MSG_BTSTACK_HFP_VOLUME_SYNC = 0x702,
    MSG_BTSTACK_HFP_BATTERY_SYNC = 0x703,
    MSG_BTSTACK_HFP_SETSYN_SYNC = 0x704,
    MSG_BTSTACK_HFP_RELEASESYN_SYNC = 0x705,
    MSG_BTSTACK_HFP_TX_DTMF_CODE_SYNC = 0x706,
    MSG_BTSTACK_HFP_DIAL_PHONENUM_SYNC = 0x707,
    MSG_BTSTACK_HFP_CALL_LAST_SYNC = 0x708,
    MSG_BTSTACK_HFP_CONNECT_SYNC = 0x709,
    MSG_BTSTACK_HFP_DISCONNECT_SYNC = 0x70a,
    MSG_BTSTACK_HFP_SET_SIRI_SYNC = 0x70b,
    MSG_BTSTACK_HFP_SCO_CHANGE_SYNC = 0x70c,

    /*3方通话处理*/
    MSG_BTENGINE_3WAY_HANDLE_SYNC = 0x720,
    MSG_BTENGINE_3WAY_GET_STATUS_SYNC = 0x721,
	
	
    /****************************** BTSTACK OTHER ******************************/
    MSG_BTSTACK_OTHER_BASE = 0x800,
    MSG_BTSTACK_SET_DEVICE_NAME_SYNC = 0x801,
    MSG_BTSTACK_GET_DEVICE_INFO_SYNC = 0x802,
    MSG_BTSTACK_SET_DISCOVERABLE_SYNC = 0x803,
    MSG_BTSTACK_BQB_TEST_SYNC = 0x804,
    MSG_BTSTACK_GET_PAIRED_LIST_SYNC = 0x805,
    MSG_BTSTACK_FORCE_UNLINK_SYNC = 0x806,
    MSG_BTSTACK_RCP_SEND_SYNC = 0x807,
    MSG_BTSTACK_SET_A2DP_ACTIVE_DEV_SYNC = 0x808,
    MSG_BTSTACK_SET_HFP_ACTIVE_DEV_SYNC = 0x809,
    MSG_BTSTACK_SNIFF_CONTROL_SYNC = 0x80a,
    MSG_BTSTACK_SPP_DISCONNECT_SYNC = 0x80b,
    MSG_BTSTACK_BLE_DISCONNECT_SYNC = 0x80c,
    MSG_BTSTACK_GET_PB_CONTENT_SYNC = 0x80d,
    MSG_BTSTACK_PLAY_PIPE_CTRL_SYNC = 0x80e,
    MSG_BTSTACK_CALL_PIPE_CTRL_SYNC = 0x80f,
    MSG_BTSTACK_SET_BLE_DISCOVERABLE_SYNC = 0x810,
    MSG_BTSTACK_ENTER_BTT_TEST_SYNC = 0x811,
    MSG_BTSTACK_GET_BTSTACK_VER_SYNC = 0x812,
    MSG_BTSTACK_GET_BTSTACK_CONN_INIT_SYNC=0x813,
    MSG_BTSTACK_DECODE_QUIT_SYNC=0x814,
    MSG_BTSTACK_SET_PAGEABLE_SYNC = 0x815,
    MSG_BTSTACK_AG_UNLINK_SYNC = 0x816,
    //for test client 
    MSG_BTSTACK_SET_BLE_WORK_MODE = 0x820,
    //for profile qulification
    MSG_BTSTACK_SET_CTS_NOTIFY = 0x821,    
    //for test client disconect
    MSG_BTSTACK_SET_BLE_DISCONECT = 0x822,
     //for test client discovery
    MSG_BTSTACK_SET_BLE_AUTOCACHE = 0x823,
         //for test client discovery
    MSG_BTSTACK_SET_BLE_READVAL = 0x824,
    //for test tip security
    MSG_BTSTACK_SET_BLE_SECURITY = 0x825,
    //for test fms start alart
    MSG_BTSTACK_SET_BLE_ALART_1 = 0x826,
    //for test fms stop alart
    MSG_BTSTACK_SET_BLE_ALART_0 = 0x827,
    
    //for test hrs notify
    MSG_BTSTACK_SET_BLE_HRS_NOTI = 0x828,
    
    //for test hts notify
    MSG_BTSTACK_SET_BLE_HTS_NOTI = 0x829,
    
    //for hid test 
    MSG_BTSTACK_SET_LM_DISCOVERABLE_SYNC = 0x82a,
    
    //for fast scan br/edr
    MSG_BTSTACK_SET_SCAN_MODE = 0x82b,
    //for force unlink one 
    MSG_BTSTACK_FORCE_DISCONN = 0x82c,
    
    /****************************** BTSTACK TWS ******************************/
    MSG_BTSTACK_TWS_BASE = 0x8a0,
    MSG_BTSTACK_TWS_START_PLAY_SYNC = 0x8a1,
    MSG_BTSTACK_TWS_FORCE_UNLINK_SYNC = 0x8a2,
    MSG_BTSTACK_TWS_CLEAR_ACTIVE_ID=0x8a3,
    MSG_BTSTACK_TWS_SEND_MSG_SYNC=0x8a4,
    MSG_BTSTACK_TWS_SEND_PHONCON_SYNC=0x8a5,
    MSG_BTSTACK_TWS_SUB_MODE_FLAG=0x8a6,
    MSG_BTSTACK_TWS_POWER_OFF=0x8a7,
    MSG_BTSTACK_GET_TWS_PAIRED_LIST_SYNC = 0x8a8,
    MSG_BTSTACK_TWS_VOLUME_SYNC=0x8a9,
    MSG_BTSTACK_TWS_DEAL_MSG_SYNC = 0x8aa,
    MSG_BTSTACK_TWS_UPDATE_PARM = 0x8ab,
    MSG_BTSTACK_TWS_SYNC_CMD = 0x8ac,
    MSG_BTSTACK_CLEAR_TWSPAIR_LIST = 0x8ad,
    MSG_BTSTACK_TWS_FILTER_CTRL_SYNC = 0x8ae,
    MSG_BTSTACK_TWS_SET_START_TIME = 0x8af,
    MSG_BTSTACK_TWS_LINEIN_EXT = 0x8b0,
   
    /****************************** BTSTACK EVENT ******************************/
    MSG_BTSTACK_EVENT_BASE = 0x900,
    MSG_BTSTACK_AVRCP_RX_OPID = 0x901,
    MSG_BTSTACK_AVRCP_VOLUME_ABS = 0x902,
    MSG_BTSTACK_HFP_VOLUME_ABS = 0x903,
    MSG_BTSTACK_ERR_CONNECTION_TIMEOUT = 0x904,
    MSG_BTSTACK_SPPBLE_RCP_PKG = 0x905,
    MSG_BTSTACK_PATCH_DOWNLOAD_FLAG = 0x906,
    MSG_BTSTACK_ERR_HARDWARE_EXCEPTION = 0x907,
    
    MSG_BTSTACK_POWER_OFF=0x908,
    MSG_BTSTACK_ERR_HARDWARE = 0x909,
    MSG_BTSTACK_TWS_APSWITCH_SYNC = 0x90a,
    MSG_BTSTACK_PARSE_GENARAL_CMD=0x90b,
    

    /****************************** BTPLAY ENGINE ******************************/
    MSG_BTPLAYEG_BASE = 0xa00,

    /****************************** BTCALL ENGINE ******************************/
    MSG_BTCALLEG_BASE = 0xb00,
    MSG_BTCALLEG_MIC_MUTE_SYNC, //配置远端表单
    MSG_BTCALLEG_MIC_RELEASEMUTE_SYNC,//关远端静音
    MSG_BTCALLEG_GET_EG_VOLUM_SYNC, //查询后台的音量-asqt配置
    MSG_BTCALLEG_GET_EG_PA_VOLUM_SYNC, //查询后台的PA音量-asqt配置



    /****************************** USOUN ENGINE ******************************/
    MSG_UENGINE_BASE = 0xc00,
    /*!播放暂停*/
    MSG_UENGINE_PLAY_PAUSE_SYNC,
    /*!切换下一曲*/
    MSG_UENGINE_NEXT_SONG_SYNC,
    /*!切换上一曲*/
    MSG_UENGINE_PREV_SONG_SYNC,
    /*!获取状态*/
    MSG_UENGINE_GET_STATUS_SYNC,
    /*!音量加*/
    MSG_UENGINE_VOL_ADD_SYNC,
    /*!音量减*/
    MSG_UENGINE_VOL_SUB_SYNC,
    /*!音量值同步*/
    MSG_UENGINE_VOL_SEND_SYNC,
    
    /****************************** HID ******************************/
    MSG_BTSTACK_HID_BASE                = 0xd00,
    MSG_BTSTACK_HID_CONNECT_SYNC        = 0xd01, 
    MSG_BTSTACK_HID_SEND_SYNC           = 0xd02,
    MSG_BTSTACK_HID_DISCONNECT_SYNC     = 0xd03,
	

} msg_apps_type_e;

#define SYSMSG_STOP_TTS_OPTION_MASK  (1 << 0)
#define SYSMSG_STOP_TTS_NO           (0 << 0)
#define SYSMSG_STOP_TTS_YES          (1 << 0)

typedef struct
{
    /*! 系统消息类型 */
    msg_apps_type_e msg_type;
    /*! 系统消息处理选项 */
    uint8 msg_option;
} sys_event_t;

/*创建应用传递结构体，先用在bt_stack*/
typedef struct
{
    uint8 app_id;
    uint8 byte_param[3];
    void *data;
} create_app_param_t;

/*!
 * \brief
 *  sys_msg_t 系统消息结构体
 */
typedef struct
{
    /*! 应用消息类型 */
    msg_apps_type_e type;
} sys_msg_t;

/*!
 * \brief
 *  msg_apps_t 应用消息结构体
 */
typedef struct
{
    /*! 应用消息类型，参见 msg_apps_type_e 定义 */
    uint32 type;
    /*! 应用消息内容 */
    union
    {
        /*! 消息内容真实数据 */
        uint8 data[4];
        /*! 消息内容缓冲区指针，指向消息发送方的地址空间 */
        void *addr;
    } content;
} msg_apps_t;

/*!
 * \brief
 *  msg_reply_type_e 同步消息回执枚举类型
 */
typedef enum
{
    /*! 同步消息无回复，用于应用退出时应答尚未处理的同步消息 */
    MSG_REPLY_NO_REPLY,
    /*! 回复同步消息发送者，同步消息要求功能成功完成 */
    MSG_REPLY_SUCCESS,
    /*! 回复同步消息发送者，同步消息要求功能完成失败 */
    MSG_REPLY_FAILED,
} msg_reply_type_e;

/*!
 * \brief
 *  msg_reply_t 同步消息回执结构体
 */
typedef struct
{
    /*! 同步消息回执枚举类型，参见 msg_reply_type_e 定义 */
    uint8 type;
    uint8 reserve[3];
    /*! 回执内容缓冲区指针，指向消息发送方的地址空间 */
    void *content;
} msg_reply_t;

/*!
 * \brief
 *  private_msg_t （应用间）私有消息结构体
 */
typedef struct
{
    /*! 私有消息消息内容 */
    msg_apps_t msg;
    /*! 同步信号量 */
    os_event_t *sem;
    /*! 同步消息回执指针 */
    msg_reply_t *reply;
} private_msg_t;

#define BD_ADDR_LEN  6

/*!
 * \brief
 *  btstack_msg_t BT MANAGER控制BT STACK的消息结构体
 */
typedef struct
{
    /*! 私有消息消息内容 */
    msg_apps_t msg;
    /*! 同步信号量 */
    os_event_t *sem;
    /*! 同步消息回执指针 */
    msg_reply_t *reply;
    /*! 保留字节 */
    uint8 reserve[2];
    /*! 命令对象的设备地址；并非所有命令都需要设备地址 */
    uint8 bd_addr[BD_ADDR_LEN];
} btstack_msg_t;

typedef struct
{
    /*! 事件内容 */
    msg_apps_t msg;
    /*! 保留字节 */
    uint8 reserve[2];
    /*! 事件发起的设备地址；并非所有事件都有设备地址 */
    uint8 bd_addr[BD_ADDR_LEN];
} btstack_event_t;

/*!
 * \brief
 *  app_msg_hook 前台应用私有消息勾函数指针
 */
typedef void (*app_msg_hook)(private_msg_t *private_msg);

/*!
 *  \brief
 *  g_this_app_msg_hook 按键消息特殊处理勾函数
 */
extern app_msg_hook g_this_app_msg_hook;

/*!
 *  \brief
 *  g_buffer_app_msg 缓冲系统消息
 */
extern private_msg_t g_buffer_app_msg;

//消息通信管理模块接口声明
//接收消息接口
//bool get_sys_msg(sys_msg_t *sys_msg);（内部接口，由get_app_msg调用）
bool get_app_msg(private_msg_t *private_msg) __FAR__;
bool get_app_msg_btstack(btstack_msg_t *btstack_msg) __FAR__;

//发送消息接口
#define ERR_NO_ERR         0
#define ERR_TARGET_NOEXIST -1
#define ERR_MSGQUEUE_FULL  -2
#define ERR_SYNC_ITSELF    -3
#define ERR_SYNC_TIMEOUT   -4
int send_sync_msg(uint8 app_id, msg_apps_t *msg, msg_reply_t *reply, uint32 timeout) __FAR__;
int send_async_msg(uint8 app_id, msg_apps_t *msg) __FAR__;
bool broadcast_msg(msg_apps_t *msg, bool only_front) __FAR__;

//BT专用
int send_sync_msg_btmanager(uint8 *bd_addr, msg_apps_t *msg, msg_reply_t *reply, uint32 timeout) __FAR__;
int send_async_msg_btmanager(uint8 *bd_addr, msg_apps_t *msg) __FAR__;
int send_async_event(uint8 app_id, btstack_event_t *ev) __FAR__;

//消息通信管理模块初始化接口
bool applib_message_init(void) __FAR__;

#endif
