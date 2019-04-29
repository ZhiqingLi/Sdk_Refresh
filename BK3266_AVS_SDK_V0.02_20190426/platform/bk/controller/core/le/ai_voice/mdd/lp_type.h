#ifndef __LP_TYPE_H__
#define __LP_TYPE_H__

#define SDK_VERSION                     "0.02"

typedef unsigned char                   uint8_t;
typedef char                            int8_t;
typedef unsigned short                  uint16_t;
typedef short                           int16_t;
typedef unsigned int                    uint32_t;
typedef int                             int32_t;
typedef unsigned long long              uint64_t;
typedef long long                       int64_t;
typedef unsigned char                   BOOL;

#if !defined(TRUE)
#define TRUE                            1
#endif

#if !defined(FALSE)
#define FALSE                           0
#endif

#if !defined(NULL)
#define NULL                            ((void *)0)
#endif

typedef void * LPHANDLE;
typedef void * os_wait_event;

typedef struct{
    uint8_t         event;// event类型，参考消息类型定义
    uint8_t         type;//event消息子类型
    uint16_t        Inparam;// 消息值
    os_wait_event   *wait_sem;
    uint32_t        *reply;
    uint32_t        msg_length;
    uint8_t         msg_buf[0]; //消息buffer内容实体//明确消息类型，转成预定义的结构
}LPMSG; //消息数据类型

typedef char  LPStatus;

#define LP_ERR_OK           0
#define LP_ERR_FAIL        -1
#define LP_ERR_FULL        -2
#define LP_ERR_NOMEMORY    -3
#define LP_ERR_NONE        -4
#define LP_ERR_SKIP_EVENT  -5


enum{
    LP_PROTOCOL_EVENT,//LINKPLAY协议栈event
    BLUETOOTH_EVENT, //蓝牙连接event
    MODE_EVENT,//模式切换event
    BUTTON_EVENT,//按键event
    VOLUME_EVENT,//音量event
    PLAY_EVENT,//播放event
    RECORD_EVENT,//录音数据event
    RAW_DATA_EVENT,//透传数据event
    TIMER_EVENT//Timer event
};

enum{
  BUTTON_PLAY,
  BUTTON_PAUSE,
  BUTTON_MUTE,
  BUTTON_NEXT,
  BUTTON_PREV,
  BUTTON_FFORWARD,//快进
  BUTTON_FBACKWARD,//快退
  BUTTON_RECONGINZE,//按键唤醒
  BUTTON_VOICE_RECONGINZE,//Alexa语音唤醒
  BUTTON_ABORT_ALEXA,//取消当前alexa对话
};

enum {
    BLUETOOTH_BLE_CONNECT_EVENT,
    BLUETOOTH_SPP_CONNECT_EVENT,
    BLUETOOTH_A2DP_CONNECT_EVENT,
};

/*
 * callback for user operation
 */
typedef int (*_pf_lp_callback)(LPHANDLE handle, uint32_t type, uint32_t param, void *buf, uint32_t size);
enum {
    LP_CB_BLE_SPP_SEND,
    LP_CB_START_RECORD,
    LP_CB_STOP_RECORD,
    LP_CB_RESET_OPUS_BUF,
    LP_CB_START_TRANS_OPUS_DATA,
    LP_CB_STOP_TRANS_OPUS_DATA,
    LP_CB_A2DP_CONTROL,                 // 1: play, 0: pause
    LP_CB_PASSTHOUGH_DATA,
    LP_CB_GET_DEVICE_INFO,
    LP_CB_NOTIFY_ALEXA_UI,
    LP_CB_NOTIFY_PROMPT_VOICE,
    LP_CB_NOTIFY_MIC_MUTE_STATUS,       //有些项目有单独mic指示灯，通过这个消息
    LP_CB_I2C_WRITE_READ,
    LP_CB_RESET_DSP,
    LP_CB_RESTORE_TO_DEFAULT,
    LP_CB_READ_WRITE_SOC_REGISTER,
    LP_CB_SPP_OTA_HANDLE,
};

enum {
    ALEXA_TRIGGER_BY_BTN,//按键唤醒
    ALEXA_TRIGGER_BY_VOICE,//Alexa语音唤醒
    ALEXA_TRIGGER_BY_APP,//App唤醒
};

enum {
    ALEXA_STATE_TYPE_SPCH,
    ALEXA_STATE_TYPE_NOTIFICATIONS,
    ALEXA_STATE_TYPE_ACCOUNT,
    ALEXA_STATE_TYPE_ALARM,
    ALEXA_STATE_TYPE_NETWORK,
    ALEXA_STATE_TYPE_MIC_MUTE = 10,
    ALEXA_STATE_TYPE_NOT_CONNECT_APP,
    ALEXA_STATE_TYPE_BLE_DISCONNECT,
    ALEXA_STATE_TYPE_SPP_DISCONNECT,
    ALEXA_STATE_TYPE_MODE_CHANGE,
    ALEXA_STATE_TYPE_NETWORK_DISCONNECT,
    ALEXA_STATE_TYPE_ACCOUNT_INVALID,
    ALEXA_STATE_TYPE_COMMS,
};

enum {
    ALEXA_SPEECH_STATE_IDLE,
    ALEXA_SPEECH_STATE_LISTENING,
    ALEXA_SPEECH_STATE_PROCESSING,
    ALEXA_SPEECH_STATE_SPEAKING,
    ALEXA_SPEECH_STATE_ERROR,
};

enum {
    ALEXA_NOTIFICATIONS_STATE_IDLE,
    ALEXA_NOTIFICATIONS_STATE_START,
    ALEXA_NOTIFICATIONS_STATE_CLEAR,
};

enum {
    ALEXA_ACCOUNT_STATE_VALID,
    ALEXA_ACCOUNT_STATE_INVALID,
    ALEXA_ACCOUNT_STATE_NOTLOGIN,
};

enum {
    ALEXA_ALARM_STATE_IDLE,
    ALEXA_ALARM_STATE_PLAYING,
    ALEXA_ALARM_STATE_STOPPED,
};

enum {
    ALEXA_NETWORK_STATE_DISCONNECTED,
    ALEXA_NETWORK_STATE_CONNECTED,
};

enum {
    LP_ALEXA_PLAYER_PLAY,
    LP_ALEXA_PLAYER_PAUSE,
    LP_ALEXA_PLAYER_PREV,
    LP_ALEXA_PLAYER_NEXT,
    LP_ALEXA_PLAYER_ABORT,
};

enum {
    PROMPT_VOICE_START_RECORD,
    PROMPT_VOICE_NOT_CONNECT_APP,
    PROMPT_VOICE_NOT_CONNECT_NETWORK,
    PROMPT_VOICE_ACCOUNT_ERROR,
    PROMPT_VOICE_MIC_MUTE,
    PROMPT_VOICE_MIC_UNMUTE,
};

enum {
    BT_AVRCP_PLAY_CMD,
    BT_AVRCP_PAUSE_CMD
};

enum {
    ALEXA_COMMS_IDLE,
    ALEXA_COMMS_OUTBOUNDING,
    ALEXA_COMMS_INBOUNDING,
    ALEXA_COMMS_ACTIVE,
    ALEXA_COMMS_STOP,
};

enum {
    LP_ALEXA_UI_IDLE,
    LP_ALEXA_UI_SPEECH_LISTENING,
    LP_ALEXA_UI_SPEECH_THINKING,
    LP_ALEXA_UI_SPEECH_SPEAKING,
    LP_ALEXA_UI_SPEECH_ERROR,
    LP_ALEXA_UI_NOTIFICATION_ACTIVED,
    LP_ALEXA_UI_NOTIFICATION_WITH_MUTE,
    LP_ALEXA_UI_ALARM,
    LP_ALEXA_UI_MIC_MUTE,
    LP_ALEXA_UI_LOST_CONNECTION,
    LP_ALEXA_UI_LOST_NETWORK,
    LP_ALEXA_UI_LOST_ACCOUNT,
    LP_ALEXA_UI_COMMS_CALLOUT,
    LP_ALEXA_UI_COMMS_CALLIN,
    LP_ALEXA_UI_COMMS_TALKING,
    LP_ALEXA_UI_COMMS_STOP,
};

#endif //
