/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：RCP 相关的宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _COMMON_RCP_H
#define _COMMON_RCP_H

#include "psp_includes.h"
#include "case_independent.h"
#include "case_type.h"
#include "gui_msg.h"
#include "app_msg.h"

/*! 远程控制数据包（无格式），简写 rcp */
typedef struct
{
    /*! 数据包数据 */
    void * rcp_data;
    /*! 数据包长度 */
    uint16 rcp_len;
    //uint8 reserve[2];
    /*! 区分服务 */
    uint8 rcp_flag;
    /*! 区分远端设备号 */
    uint8 rcp_remote;
} rmt_ctrl_pkg_t;

/*! rcp 队列相关宏定义 */
#define RCP_QUEUE_SIZE      8 //RCP 数据包长度
#define RCP_QUEUE_DEPTH     5 //RCP 队列深度，与应用消息队列一致

//#define RCP_ACT_SERV_FLAG   0
//#define RCP_TIP_SERV_FLAG   1
//#define RCP_ANCS_SERV_FLAG  2
//for weixin service support
#define RCP_WEIXIN_MASK 0x10
#define RCP_WEIXIN_FLAG 0x10
//for client 
#define RCP_ROLE_CLIENT 0x80

/*! service type枚举 */
typedef enum
{
    RCP_ACT_SERV_FLAG = 0,//actions service
    RCP_TIP_SERV_FLAG,    //ble tip service
    RCP_ANCS_SERVN_FLAG,   //apple ancs service notify
    RCP_ANCS_SERVD_FLAG,   //apple ancs service data source
	
	  // 发送特性
	  RCP_ANCS_CTRL_POINT_FLAG,
    RCP_CTS_SERV_FLAG,   //cts service
    RCP_NDST_SERV_FLAG,   //ndst service
    RCP_BAS_SERV_FLAG,   //bas service 
    RCP_LLS_SERV_FLAG,// 
    RCP_IAS_SERV_FLAG,// 
    RCP_HRS_SERV_FLAG,// 
    RCP_HTS_SERV_FLAG,//     
    RCP_FMS_SERV_FLAG,// 
    RCP_HTS_SERV1_FLAG, 
    RCP_RTUS_SERV_FLAG,   
    RCP_SERVICE_MAX       //invalid service
}rcp_service_type_e;

/*! 命令类枚举 */
typedef enum
{
    RCP_CMD_NONE = 0, //QAC
    RCP_CMD_APSWITCH = 'P', //0x50
    RCP_CMD_CONTROL = 'C', //0x43
    RCP_CMD_SIMKEY = 'K', //0x4B
    RCP_CMD_QUERY = 'Q', //0x51
    RCP_CMD_ANSWER = 'A', //0x41
    RCP_CMD_SETTING = 'S', //0x53
    RCP_CMD_ACK = 'R', //0x52
} rcp_cmd_type_e;

/*远程控制通道类型*/
#define RCP_CHAN_MASK            (0x03 << 0)
#define RCP_CHAN_SPP             (0x00 << 0)
#define RCP_CHAN_I2C_SPI_UART    (0x01 << 0)
#define RCP_CHAN_GPIO            (0x02 << 0)
/*远程控制数据传输分段标志*/
#define RCP_SEGMENT_MASK    (0x03 << 2)
#define RCP_SEGMENT_NONE    (0x00 << 2)
#define RCP_SEGMENT_FIRST   (0x01 << 2)
#define RCP_SEGMENT_CONT    (0x02 << 2)
#define RCP_SEGMENT_LAST    (0x03 << 2)
/*远程控制数据传输分段SEQ*/
#define RCP_SEQ_MASK        (0x0f << 0)

#define RCP_COMMAND_HEAD_SIZE     16 //RCP命令结构体长度
#define RCP_COMMAND_MAGIC_OFFSET  0
#define RCP_COMMAND_FLAGS_OFFSET  2  //FLAGS成员在RCP命令结构体中的偏移地址
#define RCP_COMMAND_LEN_OFFSET    6  //命令长度成员在RCP命令结构体中的偏移地址

//获取命令长度
#define COMMAND_LENGTH_HIGH(buf)  ((uint16)(*((uint8*)(buf) + RCP_COMMAND_LEN_OFFSET + 1)) << 8)
#define COMMAND_LENGTH_LOW(buf)   (*((uint8*)(buf) + RCP_COMMAND_LEN_OFFSET))
#define COMMAND_LENGTH(buf)       (COMMAND_LENGTH_HIGH(buf) + COMMAND_LENGTH_LOW(buf))
//获取FLAGS
#define COMMAND_FLAGS(buf,n)      (*((uint8*)(buf) + RCP_COMMAND_FLAGS_OFFSET + (n)))
//获取MAGIC
#define COMMAND_MAGIC_SOH(buf)    (*((uint8*)(buf) + RCP_COMMAND_MAGIC_OFFSET))
#define COMMAND_MAGIC_SOH_R(buf)  (*((uint8*)(buf) + RCP_COMMAND_MAGIC_OFFSET + 1))

#define MAX_SEGMENT_LEN     0x20 //I2C应答命令最大数据包长度

#define PHY_REPLY_HEADER    0  //应答命令头

#define PHY_REPLY_DATA      1  //应答数据段

//对话框消息ID
#define DIALOG_MSG_USB_STICK    0x00  //USB线插入时选择仅充电还是进入USB音箱

//APK进入后台模式超时时间
#define APK_BACKGROND_OVERTIME  6     //3秒钟没收到APK命令则表示APK进入后台模式
//APK/APP验证超时时间
#define APK_APP_VERIFICATION_OVERTIME  10 //连接成功后5秒钟没有收到验证命令，就认为是非法连接

//定义支持特性或功能BITMAP
//low 32bit
#define SUPPORT_FEATURE_A2DP_PLAY     (1 << 0)  //A2DP蓝牙推歌
#define SUPPORT_FEATURE_SDCARD_PLAY   (1 << 1)  //本地插卡播歌
#define SUPPORT_FEATURE_UHOST_PLAY    (1 << 2)  //本地U盘播歌
#define SUPPORT_FEATURE_RECORD_PLAY   (1 << 3)  //录音回放
#define SUPPORT_FEATURE_FOLDER_PLAY   (1 << 4)  //自定义目录快捷播放
#define SUPPORT_FEATURE_FMRADIO       (1 << 5)  //FM收音机
#define SUPPORT_FEATURE_LINEIN_PLAY   (1 << 6)  //AUX音箱
#define SUPPORT_FEATURE_RECORD        (1 << 7)  //MIC录音
#define SUPPORT_FEATURE_REC_FMRADIO   (1 << 8)  //FM收音机录音
#define SUPPORT_FEATURE_REC_LINEIN    (1 << 9)  //AUX音箱录音
#define SUPPORT_FEATURE_ALARM_CLOCK   (1 << 10) //闹钟
#define SUPPORT_FEATURE_ALARM_BUILDIN_RING (1 << 11) //闹钟闹铃支持内置铃声
#define SUPPORT_FEATURE_ALARM_RING_FOLDER  (1 << 12) //闹钟闹铃支持自定义闹铃目录
#define SUPPORT_FEATURE_ALARM_PLAYLIST     (1 << 13) //闹钟闹铃支持播放列表
#define SUPPORT_FEATURE_ALARM_FMRADIO      (1 << 14) //定时FM收音机
#define SUPPORT_FEATURE_ALARM_REC_FMRADIO  (1 << 15) //定时FM收音机录音，必须支持FM收音机录音
#define SUPPORT_FEATURE_USBBOX_PLAY        (1 << 16) //USB音箱播歌
#define SUPPORT_FEATURE_OTA        (1 << 17)         //OTA功能


#define SUPPORT_FEATURE_PLIST_FILE    (1 << 24) //支持文件名列表
#define SUPPORT_FEATURE_PLIST_TITLE   (1 << 25) //支持标题列表
#define SUPPORT_FEATURE_PLIST_ARTIST  (1 << 26) //支持演唱者列表
#define SUPPORT_FEATURE_PLIST_ALBUM   (1 << 27) //支持专辑列表
#define SUPPORT_FEATURE_PLIST_GENRE   (1 << 28) //支持风格列表

/*!
 * \brief
 *  dialog_type_e 对话框类型枚举类型
 */
typedef enum
{
    /*! 无效对话框类型 */
    DIALOG_TYPE_NONE = 0x00,
    /*! 含有按键选择的"询问对话框" */
    DIALOG_BUTTON_INCLUDE = 0x01,
    /*! 不含按键的"提示信息对话框"，弹出后按任意按键退出 */
    DIALOG_BUTTON_ANYKEY = 0x02,
    /*! 不含按键的"提示信息对话框"，弹出后等待2秒钟后退出 */
    DIALOG_INFOR_WAIT = 0x03,
    /*! 不含按键的"提示信息对话框"，弹出后立即退出 */
    DIALOG_INFOR_IMMEDIATE = 0x04,
    /*! 取消当前对话框 */
    DIALOG_TYPE_CANCEL = 0x05,
} dialog_type_e;

/*!
 * \brief
 *  button_type_e 对话框按键组合类型枚举类型
 */
typedef enum
{
    /*! 确定和取消 */
    BUTTON_OKCANCEl = 0x00,
    /*! 终止、重试和忽略 */
    BUTTON_ABORTRETRYIGNORE = 0x01,
    /*! 是、否和取消 */
    BUTTON_YESNOCANCEL = 0x02,
    /*! 是和否 */
    BUTTON_YESNO = 0x03,
    /*! 重试和取消 */
    BUTTON_RETRYCANCEL = 0x04,
    /*以下是自定义按钮组合*/
    /*! 充电/USB音箱 */
    BUTTON_USBSOUND_CHARGE = 0x05,
    /*! 没有按钮 */
    BUTTON_NO_BUTTON = 0x06,
} button_type_e;

/*! 对话框状态机 */
typedef enum
{
    /*! 处于空闲状态 */
    DIALOG_STATE_NONE,
    /*! 小机发起对话框请求，等待APK应答 */
    DIALOG_STATE_LAUNCH,
    /*! 小机收到APK应答，等待APK对话框选择 */
    DIALOG_STATE_CHOOSING,
    /*! 小机收到APK对话框选择，确定选项返回 */
    DIALOG_STATE_CONFIRM,
    /*! APK没有做选择自动退出 */
    DIALOG_STATE_CANCEL_AUTO,
} dialog_state_e;

/*! 控制类命令回调函数 */
typedef app_result_e (*rcp_ctrl_func)(uint32 data1, uint32 data2);
/*! 查询类命令回调函数 */
typedef app_result_e (*rcp_get_func)(uint32 data1, uint32 data2);
/*! 设置类命令回调函数 */
typedef app_result_e (*rcp_set_func)(uint32 data1, uint32 data2, void *data, uint16 data_len);

/*! 扩展服务类回调函数 */
typedef app_result_e (*rcp_ext_serv_func)(void *data, uint16 data_len, uint8 serv_flag);

/*set/get/control类命令回调函数注册表*/
typedef struct
{
    rcp_cmd_type_e type;
    uint8 id;
    void * cb;
} rcp_cmd_cb_t;

//get类命令ID号
#define QUERY_ID_GLOBAL_RCP_INFO  0x00  //获取全局状态
/* 应答参数为：0, 0, com_rcp_info_t + XXX_rcp_info_t （各ap自定义，可以不要）*/
#define QUERY_ID_DISK_CAPACITY    0x01  //获取本次磁盘容量
/* 应答参数为：available capacity, total capacity, NULL */
#define QUERY_ID_SUPPORT_FEATURE  0x02  //获取支持特性或功能
/* 应答参数为：support feature bitmap low 32bit, support feature bitmap high 32bit, NULL */
#define QUERY_ID_MUSIC_FOLDER     0x03  //获取本地磁盘特殊目录列表
/* 应答参数为：music folder count, 0, music_folder_infor_t * music folder count */
#define QUERY_ID_BT_DEVICE_INFO   0x80  //用户扩展命令范例，获取蓝牙设备信息，包括名称等
/* 应答参数为：0, 0, bt_device_info_t */

#define QUERY_ID_MUSIC_INFO       0x10  //获取本地音乐当前歌曲文件名和ID3信息
/* 应答参数为：file seq, 0, music_rcp_id3_info_t */
#define QUERY_ID_MUSIC_LYRIC      0x11  //获取本地音乐当前歌曲歌词文件
/* 应答参数为：file seq, 0, lyric file （分段传输，每段512字节）*/
#define QUERY_ID_MUSIC_PLIST      0x12  //获取指定序号本地音乐的播放列表信息
/* 命令参数为：file seq, file count, NULL */
/* 应答参数为：file count, 0, music_rcp_plist_info_t */
#define QUERY_ID_MUSIC_STATUS    0x13   //获取本地音乐当前目录的状态信息
/* 应答参数为：dir_status, 0, random */
#define QUERY_ID_MUSIC_DLIST       0x14  //获取指定序号本地目录的列表信息
/* 命令参数为：dir seq, dir count, NULL */
/* 应答参数为：dir seq, dir count, music_rcp_dlist_info_t */

#define QUERY_ID_STATION_LIST     0x20  //获取收音机电台列表
/* 应答参数为：freq count, 0, 频点uint32 * freq count*/
#define QUERY_ID_STATION_NAME     0x21  //获取当前电台名字
/* 应答参数为：0, 0, 64字节名字 */

#define QUERY_ID_ALARM_LIST       0x30  //获取闹钟记录列表
/* 应答参数为：alarm count, 0, alarm_alarm_t * alarm count */
#define QUERY_ID_RING_LIST        0x31  //获取闹铃列表
/* 命令参数为：ring type, 0, NULL ；ring type -1表示获取全部闹铃，至于全部包含哪些由音箱决定*/
/* 应答参数为：ring count, 0, ring_song_t * ring count */
#define QUERY_ID_RING_FOLDER      0x32  //获取本地磁盘闹铃目录（类型）列表
/* 应答参数为：ring folder count, 0, ring_folder_infor_t * ring folder count */
#define QUERY_ID_ALARM_PARAM      0x33  //获取闹钟参数，包括snooze时间和次数，超时时间等
/* 应答参数为：0, 0, alarm_param_t */
#define QUERY_ID_OTA_VER      0x70  //获取FW版本号
/* 应答参数为： */
#define QUERY_ID_OTA_RECEIVE_STATE      0x71  //获取小机收数据的状态
/* 应答参数为： */
#define QUERY_ID_OTA_CONNECT      0x72  //获取小机握手



//set类命令ID号
//NOTE!!! 用户保留命令区间：[0x80-0xbf] 共64个ID，其他ID保留给炬力标案扩展
#define SETTING_ID_SYSTEM_SETTING 0x00  //系统设置
#define SETTING_ID_SS_DATETIME    0x00  //日期时间
#define SETTING_ID_SS_VOLUME      0x01  //音量
#define SETTING_ID_SS_EQ          0x02  //EQ模式
#define SETTING_ID_SS_USREQ       0x03  //用户EQ
#define SETTING_ID_SS_DAEMODE     0x04  //DAE模式
/* 命令参数为：setting id, value, data
 【1】 设置日期和时间：0, 0, system_datetime_t
 【2】 设置音量：1, volume, NULL
 【3】 设置EQ模式：2, eq type, NULL
 【4】 设置用户EQ：3, 0, uint8 eq_param[7] + uint8 reserve[1]
 【5】 设置DAE模式：4, 0, daemode_param_t
 */
#define SETTING_ID_SYSTEM_STATUS  0x01  //APK状态
/* 命令参数为：apk_status_e, 0, NULL */
#define SETTING_ID_SYSTEM_DIALOG  0x02  //对话框提示控制
#define SETTING_ID_SD_ANSWER      0x00  //回应请求
#define SETTING_ID_SD_CONFIRM     0x01  //对话框确认
#define SETTING_ID_SD_FAIL        0x02  //确认取消
/* 命令参数为：control id, value data
 【1】回应请求：0, overtime/-1 for error, NULL
 【2】对话框返回：1, button id, NULL
 【3】确认取消：2, 0, NULL
 */
#define SETTING_ID_DEVICE_NAME    0x80  //用户扩展命令范例，设置设备名称
/* 命令参数为：0, 0, bt_device_name_t */

#define SETTING_ID_MUSIC_SETTING  0x10  //音乐参数设置
/* 命令参数为：setting id, value, data 
 【1】 设置循环模式：0, loop mode, NULL
 */
#define SETTING_ID_SONG_SEQ       0x11  //曲目号点播
/* 命令参数为：seq, 0, 0 */
#define SETTING_ID_MUSIC_PLIST    0x12  //播放列表，即文件序号表
/* 命令参数为：0, 0, file index array NOTE!!! 长度必须4字节对齐，如不对齐要填充对齐 */
#define SETTING_ID_MUSIC_DISK     0x13  //切换磁盘
/* 命令参数为：1 for card/2 for udisk, 0, NULL */

#define SETTING_ID_STATION_BAND   0x20  //设置电台频段
/* 命令参数为：band, 0, NULL */
#define SETTING_ID_STATION_FREQ   0x21  //设置电台频点
/* 命令参数为：freq, 0, NULL */

#define SETTING_ID_ALARM_ADD      0x30  //添加或更新闹钟记录
/* 命令参数为：0, 0, alarm_alarm_t*/
#define SETTING_ID_ALARM_DELETE   0x31  //删除闹钟记录
/* 命令参数为：alarm id（-1表示清空）, 0, NULL */
#define SETTING_ID_ALARM_PARAM    0x32  //设置闹钟参数，包括snooze时间和次数，超时时间等
/* 命令参数为：0, 0, alarm_param_t */

#define SETTING_ID_AUX_SETTING    0x40  //LINEIN参数设置
/* 命令参数为：setting id, value, data 
 暂不使用*/
#define SETTING_ID_OTA_CONNECT_SETTING    0x70  //OTA握手(参数设置)
/* 命令参数为：  */
#define SETTING_ID_OTA_RECIEVE_DATA    0x71  //OTA FW数据接收
/* 命令参数为：  */
#define SETTING_ID_OTA_RESTAT_UD    0x72  //开机重启升级
/* 命令参数为：  */
#define SETTING_ID_OTA_RESTAT_CD    0x73  //重启清空备份区
/* 命令参数为：  */
#define SETTING_ID_SEND_COMPLETE          0x74  //完成发送命令
/* 命令参数为：  */
#define SETTING_ID_OTA_RECIEVE_VRAM_DATA  0x75  //OTA VRAM数据接收


/*ACK类命令ID*/
#define RCP_ACK_ID_ACK            0x00  //ACK 应答，默认命令处理OK后自动 ACK 应答
#define RCP_ACK_ID_NACK           0x01  //NACK应答

#define CONTROL_KEY_PLAY        (0x0b)
#define CONTROL_KEY_PAUSE       (0x0c)
#define CONTROL_KEY_NEXT        (0x0d)
#define CONTROL_KEY_PREV        (0x0e)
#define CONTROL_KEY_DISMUTE     (0x0f)
#define CONTROL_KEY_VADD        (0x10)
#define CONTROL_KEY_VSUB        (0x11)
#define CONTROL_KEY_MUTE        (0x12)
#define CONTROL_KEY_STOP        (0x18)
#define CONTROL_FM_SEARCH       (0x23)
#define CONTROL_FM_STOP_SCH     (0x24)
#define CONTROL_FM_CHADD        (0x25)
#define CONTROL_FM_CHSUB        (0x26)
#define CONTROL_FM_MUTE         (0x27)
#define CONTROL_BT_DISCON       (0x28)
#define CONTROL_BT_ACCEPT       (0x29)
#define CONTROL_BT_HANG_UP      (0x2a)
#define CONTROL_BT_CALL_LAST    (0x2b)
#define CONTROL_ALARM_SNOOZE    (0x2c)
#define CONTROL_ALARM_SHUTOFF   (0x2d)

#if (SUPPORT_SLAVE_CONTROL == 1)
/*COMMON全局状态结构体，预留到4字节*/
typedef struct
{
    /*! RESERVE */
    uint8 reserve_bytes[4];
}com_rcp_info_t;
#else //SUPPORT_SLAVE_CONTROL
/*COMMON全局状态结构体，预留到24字节*/
typedef struct
{
    /*! 当前应用类型，暂时以APP ID代替 */
    uint8 app_type;
    /*! 最大音量 */
    uint8 max_volume;
    /*! 最小音量 */
    uint8 min_volume;
    /*! 当前音量 */
    uint8 cur_volume;
    /*! 当前EQ模式*/
    uint8 cur_eqtype;
    /*! 是否静音状态 */
    uint8 mute_flag;
    /*! 当前电池状态，无充电，充电中，充满电，无电池 */
    uint8 battery_state;
    /*! 当前电池电量，0-4格电量 */
    uint8 battery_value;
    /*! 当前SD卡状态，无插卡，插卡，卡播放 */
    uint8 sdcard_in;
    /*! 当前UHOST状态，无UHOST，插UHOST，UHOST播放 */
    uint8 uhost_in;
    /*! 是否连接USB电缆，没有连接，连接USB线，连接充电适配线 */
    uint8 usb_in;
    /*! 是否连接LINEIN线 */
    uint8 linein_in;
    /*! 是否连接耳机线*/
    uint8 hp_in;
    /*! 是否完成配置，进入应用会先进行配置，完成之后才保证应用状态正确性；
     比如SDCARD播歌，要先进行目录扫描等 */
    uint8 config_flag;
    /*! 是否正在闹钟中 */
    uint8 alarm_clock;
    /*! 当应用完成配置时，传递给APK的应用相关参数；比如音乐应用可以由该参数决定是否需要更新列表 */
    uint8 app_argv;
    /*! 对话框类型，参见 dialog_type_e 定义 */
    uint8 dialog_type;
    /*! 对话框按键组合类型，参见 button_type_e 定义 */
    uint8 dialog_button_type;
    /*! 对话框描述信息ID */
    uint8 dialog_desc_id;
    /*! 默认激活按键项 */
    uint8 dialog_active_default;
    /*! 当前数字音效模式，0表示无数字音效，1表示EQ音效，2表示DAE音效 */
    uint8 dae_mode;
    /*! DAE音效选项，bit 0表示VBASS开关，bit 1表示treble开关，bit 2表示虚拟环绕开关 */
    uint8 dae_option;
    /*! RESERVE */
    uint8 reserve_bytes[2];
} com_rcp_info_t;
#endif //SUPPORT_SLAVE_CONTROL

/*当前系统日期时间*/
typedef struct
{
    /*! 当前系统日期 */
    date_t date;
    /*! 当前系统时间 */
    time_t time;
    uint8 reserve;
} system_datetime_t;

/*DAE mode参数结构体*/
typedef struct
{
    /*! 当前数字音效模式 */
    uint8 dae_mode;
    /*! 参数 */
    union
    {
        /*! DAE音效选项 */
        uint8 dae_option;
        /*! EQ音效类型 */
        uint8 cur_eqtype;
    } param;
    /*! 如果打开GEQ，那么还需要GEQ参数表 */
    uint8 eq_para[7];
} daemode_param_t;

//RCP命令解析接口
typedef enum
{
    /*!无效RCP包*/
    RCP_STATE_INVALID,
    /*!当前RCP包已经包含完整的命令*/
    RCP_STATE_OK,
    /*!当前RCP包仅包含命令头部*/
    RCP_STATE_HEAD,
} rcp_state_e;
extern rcp_state_e com_check_valid_rcp(uint8 *rcp_head, uint16 rcp_len) __FAR__;
extern app_result_e com_rcp_dispatch(rmt_ctrl_pkg_t *p_rcp) __FAR__;
extern void com_rcp_state_init(void) __FAR__;
extern void com_rcp_init(void) __FAR__;
extern void com_rcp_set_callback(const rcp_cmd_cb_t * app_rcp_cmd_cb_tbl, rcp_get_func app_get_global_info_cb) __FAR__;
extern bool com_rcp_check_for_tts(rmt_ctrl_pkg_t *p_rcp) __FAR__;
extern app_result_e com_rcp_cmd_dispatch(rmt_ctrl_pkg_t *p_rcp) __FAR__;
extern void com_rcp_send_pkg(uint32 data1, uint32 data2, void *data, uint16 data_len, uint16 data_total) __FAR__;
extern void com_rcp_send_pkg_cont(void *data, uint16 data_len) __FAR__;
extern void com_rcp_send_ack_pkg(uint8 ack) __FAR__;
extern void com_rcp_send_cmd_pkg(uint8 cmd_id, uint8 cmd_type,uint8 dev_id) __FAR__;
extern void com_get_com_info(com_rcp_info_t *p_com_rcp_info) __FAR__;
extern app_result_e com_get_global_rcp_info(uint32 data1, uint32 data2) __FAR__;
extern app_result_e com_get_bt_device_info(uint32 data1, uint32 data2) __FAR__;
extern app_result_e com_get_support_feature(uint32 data1, uint32 data2) __FAR__;
extern app_result_e com_set_system_setting(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e com_set_system_status(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e com_set_system_dialog(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
typedef bool (*dialog_check_callback)(void);
extern app_result_e com_rcp_dialog(uint8 type, uint8 button_type, uint8 desc_id, uint8 active_default,
        dialog_check_callback check_cb) __FAR__;
extern bool com_dialog_check_usbdialog(void) __FAR__;
extern app_result_e com_set_device_name(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;

extern app_result_e com_ctrl_switch_mute(uint32 data1, uint32 data2) __FAR__;

extern app_result_e time_alarm_get_alarm_list(uint32 data1, uint32 data2) __FAR__;
extern app_result_e time_alarm_get_ring_list(uint32 data1, uint32 data2) __FAR__;
extern app_result_e time_alarm_get_ring_folder(uint32 data1, uint32 data2) __FAR__;
extern app_result_e time_alarm_get_alarm_param(uint32 data1, uint32 data2) __FAR__;
extern app_result_e time_alarm_set_alarm_add(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e time_alarm_set_alarm_delete(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e time_alarm_set_alarm_param(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
extern app_result_e alarm_alarm_snooze(uint32 data1, uint32 data2) __FAR__;
extern app_result_e alarm_alarm_close(uint32 data1, uint32 data2)__FAR__;
extern bool g_rcp_ctrl_quit_flag;

//rcp dialog
extern uint8 g_dialog_state;
extern uint8 g_dialog_overtime_flag;
extern uint8 g_dialog_overtime;
extern uint8 g_dialog_choosed;
extern uint8 g_dialog_type;
extern uint8 g_dialog_button_type;
extern uint8 g_dialog_desc_id;
extern uint8 g_dialog_active_default;
//用于rcp服务类型保存
extern uint8 g_rcp_service_type;
//用于rcp远端设备号保存
extern uint8 g_rcp_remote_number;

extern const rcp_cmd_cb_t *g_app_rcp_cmd_cb_tbl;
extern rcp_get_func g_app_get_global_info;

extern rcp_ext_serv_func g_app_ext_serv_cb[]; //扩展服务响应函数

extern const msg_apps_type_e rcp_cmd_apswitch_event[];
extern const key_value_e rcp_cmd_simkey_key[];
extern void* com_rcp_cmd_find_cb(uint8 cmd_type, uint8 cmd_id) __FAR__;
extern void com_apswitch_hook(void) __FAR__;
extern void com_rcp_cmd_dispatch_before_hook(void) __FAR__;
extern void com_rcp_cmd_dispatch_after_hook(void) __FAR__;

extern uint8 g_rcp_cmd_flags[2]; //SPP命令标志字
extern uint8 g_rcp_cmd_id; //当前正在处理命令ID
extern uint8 g_rcp_next_seq; //下一次分段合法SEQ
extern bool g_rcp_seg_flag; //RCP分段标志
extern uint8 *g_rcp_long_buffer; //超长RCP缓冲区，超过256字节都属于超长RCP
extern uint16 g_rcp_long_length; //超长RCP长度
extern uint16 g_rcp_long_index; //超长RCP缓冲区INDEX
extern bool g_rcp_long_buffer_flag; //正在缓冲完整RCP标志
extern uint16 g_rcp_pkg_max_len; //RCP最大长度，受限于缓冲区大小

#define RCP_PKG_MAX_PAYLOAD          (520) //RCP最大负载为520字节，允许在data区中存放512数据及8个字节的描述
#define RCP_LONG_COMMAND_BUFFER_LEN  (RCP_PKG_MAX_PAYLOAD + RCP_COMMAND_HEAD_SIZE) //最多为 0x220 字节
extern uint8 *g_rcp_default_long_buffer;

#endif
