#ifndef _API_BTSTACK_H
#define _API_BTSTACK_H

//蓝牙功能
#define PROF_A2DP                       0x0007  //蓝牙音乐功能
#define PROF_HID                        0x0018  //蓝牙键盘功能
#define PROF_HFP                        0x0060  //蓝牙通话功能
#define PROF_SPP                        0x0080  //蓝牙串口功能
#define PROF_PBAP                       0x0100  //蓝牙电话本功能

//蓝牙特性
#define HFP_BAT_REPORT                  0x01    //是否支持手机电量显示
#define HFP_3WAY_CALL                   0x02    //是否支持三通电话
#define HFP_IOS_RING                    0x04    //是否支持IOS来电铃声
#define HFP_CALL_PRIVATE                0x08    //是否打开强制私密接听
#define HFP_SIRI_CMD                    0x10    //是否打开siri控制命令
#define A2DP_VOL_CTRL                   0x01    //是否支持手机音量控制
#define A2DP_RECON_CTRL                 0x02    //是否打开A2DP控制键回连

//AVRCP消息
#define AVC_VOLUME_UP               0x41
#define AVC_VOLUME_DOWN             0x42
#define AVC_MUTE                    0x43
#define AVC_PLAY                    0x44
#define AVC_STOP                    0x45
#define AVC_PAUSE                   0x46
#define AVC_RECORD                  0x47
#define AVC_REWIND                  0x48
#define AVC_FAST_FORWARD            0x49
#define AVC_EJECT                   0x4a
#define AVC_FORWARD                 0x4b
#define AVC_BACKWARD                0x4c

//扩展标志
#define BT_FLAG0_TWS_SCAN           0x01
#define BT_FLAG0_CLK_ADVANCE        0x02
#define BT_FLAG0_TWS_M_RECON        0x04
#define BT_FLAG0_TWS_FAST_RECON     0x08
#define BT_FLAG0_TWS_MENU_PAIR      0x10

#define BT_FLAG1_LTX_RECON          0x01

typedef struct
{
    u8 txpwr;                       //0=4dbm, 1=3dbm, 2=2dbm, 3=1dbm, 4=0dbm
    u8 packet;                      //0=LQFP48, 1=QFN32, 3=, 4=, 5=TSSOP24, 6=SOP16
    u8 res[6];
} rf_cfg_t;

typedef struct {
    u8 pwrup_connect_times;
    u8 tout_connect_times;
    u16 profile;
    u8 max_acl_link;
    u8 max_nor_link;
    u8 a2dp_feature;
    u8 hfp_feature;
    u8 vx_set;
    u16 scan_timeout;
    u8 res[2];

    u16 dual_mode           : 1;
    u16 tws_mode            : 1;
    u16 fcc_test_mode       : 1;
    u16 cbt_test_mode       : 1;
    u16 simple_pair_mode    : 1;
    u16 scan_ctrl           : 1;

    u16 sco_plc_en          : 1;
    u16 sco_msbc_en         : 1;
} bt_cfg_t;

//蓝牙状态
enum {
    BT_STA_OFF,                                 //蓝牙模块已关闭
    BT_STA_INITING,                             //初始化中
    BT_STA_IDLE,                                //蓝牙模块打开，未连接

    BT_STA_SCANNING,                            //扫描中
    BT_STA_DISCONNECTING,                       //断开中
    BT_STA_CONNECTING,                          //连接中

    BT_STA_CONNECTED,                           //已连接
    BT_STA_PLAYING,                             //播放
    BT_STA_INCOMING,                            //来电响铃
    BT_STA_OUTGOING,                            //正在呼出
    BT_STA_INCALL,                              //通话中
    BT_STA_OTA,                                 //OTA升级中
};

//通话状态
enum {
    BT_CALL_IDLE,                               //
    BT_CALL_INCOMING,                           //来电响铃
    BT_CALL_OUTGOING,                           //正在呼出
    BT_CALL_ACTIVE,                             //通话中
    BT_CALL_3WAY_CALL,                          //三通电话或两部手机通话
};

//蓝牙通知
enum {
    BT_NOTICE_INIT_FINISH,                      //蓝牙初始化完成
    BT_NOTICE_DISCONNECT,                       //蓝牙断开
    BT_NOTICE_LOSTCONNECT,                      //蓝牙连接丢失
    BT_NOTICE_CONNECTED,                        //蓝牙连接成功
    BT_NOTICE_INCOMING,                         //来电
    BT_NOTICE_RING,                             //来电响铃
    BT_NOTICE_OUTGOING,                         //去电
    BT_NOTICE_CALL,                             //建立通话
    BT_NOTICE_SET_SPK_GAIN,                     //设置通话音量
    BT_NOTICE_MUSIC_PLAY,                       //蓝牙音乐开始播放
    BT_NOTICE_MUSIC_STOP,                       //蓝牙音乐停止播放
    BT_NOTICE_MUSIC_CHANGE_VOL,                 //改变蓝牙音乐音量
    BT_NOTICE_MUSIC_SET_VOL,                    //设置蓝牙音乐音量
    BT_NOTICE_HID_CONN_EVT,                     //HID服务连接事件
    BT_NOTICE_KEY_PRESS_EVT,                    //远端按键按下
    BT_NOTICE_TBOX_EVT,
    BT_NOTICE_TWS_SET_VOL,                      //设置TWS音乐音量
    BT_NOTICE_TWS_HID_SHUTTER,                  //远端TWS拍照键
    BT_NOTICE_TWS_INFO,                         //TWS同步信息
    BT_NOTICE_TWS_USER_KEY,                     //TWS自定义按键
};

//控制消息
enum bt_msg_t {
    BT_MSG_OFF          = 0,                    //关闭蓝牙
    BT_MSG_ON,                                  //打开蓝牙
    BT_MSG_SCAN_ENABLE,                         //打开扫描
    BT_MSG_SCAN_DISABLE,                        //关闭扫描
    BT_MSG_CONNECT,                             //连接蓝牙
    BT_MSG_DISCONNECT,                          //断开蓝牙
    BT_MSG_PLAY_PAUSE,                          //切换播放、暂停
    BT_MSG_VOL_CHANGE,                          //音量调整
    BT_MSG_CALL_REDIAL,                         //回拨电话（最后一次通话）
    BT_MSG_CALL_ANSWER_INCOM,                   //接听来电
    BT_MSG_CALL_TERMINATE,                      //挂断通话或来电
    BT_MSG_CALL_SWAP,                           //切换三通电话
    BT_MSG_HFP_REPORT_BAT,                      //报告电池电量
    BT_MSG_HFP_SPK_GAIN,                        //设置通话扬声器音量
    BT_MSG_HFP_MIC_GAIN,                        //设置通话麦克风音量
    BT_MSG_HFP_PRIVATE_SWITCH,                  //切换私密通话
    BT_MSG_HID_CONNECT,                         //连接HID
    BT_MSG_HID_DISCONNECT,                      //断开HID
    BT_MSG_AUDIO_BYPASS,                        //忽略蓝牙SBC/SCO AUDIO
    BT_MSG_AUDIO_ENABLE,                        //使能蓝牙SBC/SCO AUDIO
    BT_MSG_TWS_SCAN_ENABLE,                     //打开TWS扫描，连接后会自动关闭
    BT_MSG_TWS_SCAN_DISABLE,                    //关闭TWS扫描
    BT_MSG_TWS_DISCONNECT,                      //断开TWS连接
    BT_MSG_TWS_CONNECT,                         //建立TWS连接
    BT_MSG_TWS_SEARCH_SLAVE,                    //搜索并建立TWS连接
    BT_MSG_RES2,
    BT_MSG_SWITCH_SIRI,
    BT_MSG_OTA_READ,
    BT_MSG_OTA_STATUS,
    BT_MSG_CALL_REDIAL_NUMBER,                  //回拨电话（从hfp_get_outgoing_number获取号码）
    BT_MSG_HFP_AT_CMD,
    BT_MSG_MAX,

    BT_MSG_HID_KEY      = 0x30,                 //HID按键
    BT_MSG_HID_CONSUMER,                        //CONSUMER按键
    BT_MSG_TWS_USER_KEY,                        //TWS用户自定义消息
    BT_MSG_PARAM_MAX,

    BT_MSG_PLAY         = AVC_PLAY,             //播放
    BT_MSG_PAUSE        = AVC_PAUSE,            //暂停
    BT_MSG_STOP         = AVC_STOP,             //停止
    BT_MSG_PREV         = AVC_BACKWARD,         //上一曲
    BT_MSG_NEXT         = AVC_FORWARD,          //下一曲
    BT_MSG_REWIND       = AVC_REWIND,           //开始快退
    BT_MSG_FAST_FORWARD = AVC_FAST_FORWARD,     //开始快进
    BT_MSG_REWIND_END       = AVC_REWIND|0x80,  //结束快退
    BT_MSG_FAST_FORWARD_END = AVC_FAST_FORWARD|0x80, //结束快进
};

extern bt_cfg_t bt_cfg;

//control
void bt_fcc_init(void);
void bt_init(void);                             //初始化蓝牙变量
int bt_setup(void);                             //打开蓝牙模块
void bt_off(void);                              //关闭蓝牙模块
void bt_wakeup(void);                           //唤醒蓝牙模块
void bt_send_msg(uint msg);                     //蓝牙控制消息，参数详见bt_msg_t
void bt_audio_bypass(void);                     //蓝牙SBC/SCO通路关闭
void bt_audio_enable(void);                     //蓝牙SBC/SCO通路使能
u8 bt_get_packet(u8 index);
uint8_t bt_get_hvbko(u8 rf_packet);

void bt_set_scan(bool enable);                  //打开/关闭发现与连接状态
void bt_connect(void);                          //蓝牙设备回连
void bt_disconnect(void);                       //蓝牙设备断开
void bt_hid_connect(void);                      //蓝牙HID服务回连
void bt_hid_disconnect(void);                   //蓝牙HID服务断开
int bt_hid_is_connected(void);
bool bt_hid_is_ready_to_discon(void);
bool sco_is_bypass(void);

//status
uint bt_get_disp_status(void);                  //获取蓝牙的当前显示状态, V060
uint bt_get_status(void);                       //获取蓝牙的当前状态
uint bt_get_call_indicate(void);                //获取通话的当前状态
bool bt_is_calling(void);                       //判断是否正在通话
bool bt_is_playing(void);                       //判断是否正在播放
bool bt_is_ring(void);                          //判断是否正在响铃
bool bt_is_testmode(void);                      //判断当前蓝牙是否处于测试模式
bool bt_is_sleep(void);                         //判断蓝牙是否进入休眠状态
bool bt_is_connected(void);                     //判断蓝牙是否已连接（TWS副耳被连接，或主耳与手机已连接）
bool bt_is_ios_device(void);                    //判断当前播放的是否ios设备
uint32_t bt_sleep_proc(void);
void bt_enter_sleep(void);
void bt_exit_sleep(void);

//tws api
bool bt_tws_is_connected(void);                 //tws设备是否已连接
bool bt_tws_is_scaning(void);                   //tws是否已打开可被搜索与连接
void bt_set_tws_scan(bool enable);              //打开/关闭tws可被搜索与连接
void bt_tws_disconnect(void);                   //断开tws设备
void bt_tws_connect(void);                      //回连tws设备
void bt_tws_search_slave(void);                 //搜索tws设备，并建立连接
bool bt_tws_get_link_info(uint8_t *bd_addr);    //获取tws配对信息，可作为是否配对过TWS的判断
void bt_tws_delete_link_info(void);             //删除tws配对信息
bool bt_tws_user_key(uint keycode);             //发送tws自定义按键, 10bit有效
bool bt_nor_is_connected(void);                 //是否已连接手机
void bt_nor_connect(void);                      //断开手机
void bt_nor_disconnect(void);                   //回连手机

//蓝牙连接
#define bt_scan_enable()                        bt_send_msg(BT_MSG_SCAN_ENABLE)     //打开扫描
#define bt_scan_disable()                       bt_send_msg(BT_MSG_SCAN_DISABLE)    //关闭扫描

//蓝牙音乐
#define bt_music_play()                         bt_send_msg(BT_MSG_PLAY)            //播放
#define bt_music_pause()                        bt_send_msg(BT_MSG_PAUSE)           //暂停
#define bt_music_play_pause()                   bt_send_msg(BT_MSG_PLAY_PAUSE)      //切换播放/暂停
#define bt_music_stop()                         bt_send_msg(BT_MSG_STOP)            //停止
#define bt_music_prev()                         bt_send_msg(BT_MSG_PREV)            //上一曲
#define bt_music_next()                         bt_send_msg(BT_MSG_NEXT)            //下一曲
#define bt_music_rewind()                       bt_send_msg(BT_MSG_REWIND)          //开始快退
#define bt_music_rewind_end()                   bt_send_msg(BT_MSG_REWIND_END)      //结束快退
#define bt_music_fast_forward()                 bt_send_msg(BT_MSG_FAST_FORWARD)    //开始快进
#define bt_music_fast_forward_end()             bt_send_msg(BT_MSG_FAST_FORWARD_END)//结束快进

//蓝牙通话
#define bt_call_redial_last_number()            bt_send_msg(BT_MSG_CALL_REDIAL)     //电话回拨（最后一次通话）
#define bt_call_answer_incoming()               bt_send_msg(BT_MSG_CALL_ANSWER_INCOM) //接听电话
#define bt_call_terminate()                     bt_send_msg(BT_MSG_CALL_TERMINATE)  //挂断电话
#define bt_call_swap()                          bt_send_msg(BT_MSG_CALL_SWAP)       //切换三通电话
#define bt_call_private_switch()                bt_send_msg(BT_MSG_HFP_PRIVATE_SWITCH)       //切换三通电话
#define bt_siri_switch()                        bt_send_msg(BT_MSG_SWITCH_SIRI)     //调用SIRI, android需要在语音助手中打开“蓝牙耳机按键启动”, ios需要打开siri功能

uint bt_get_hfp_feature(void);
int bt_spp_tx(uint8_t *packet, uint16_t len);

//hid
bool bt_hid_key(int keycode);                 //标准HID键, 如Enter
bool bt_hid_consumer(int keycode);            //自定义HID键, 如VOL+ VOL-

/*****************************************************************************
 * BLE API函数接口
 *****************************************************************************/
u8 le_get_status(void);
bool le_is_connect(void);
void le_set_adv(u8 chanel, u8 type);
void le_set_adv_interval(u16 interval);
void le_enable_adv(u8 allow);
void le_disable_adv(void);
void le_disconnect(void);
bool le_tx_notify(u8 index, u8* buf, u8 len);

#endif //_API_BTSTACK_H
