#ifndef _WIFI_CONTROL_H_
#define _WIFI_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"

#ifdef FUNC_WIFI_EN
//WiFi模组工作状态定义
enum
{
    WIFI_STATUS_IDLE = 0, 
    WIFI_STATUS_SD_PLUGOUT,  //SD卡拔出  
    WIFI_STATUS_SD_PLUGIN,  //SD卡插入
    WIFI_STATUS_USB_PLUGOUT,  //USB拔出
    WIFI_STATUS_USB_PLUGIN,  //USB插入  	
    WIFI_STATUS_INITING,  //模块正在初始化    
    WIFI_STATUS_INIT_END,  //模块初始化结束        
    WIFI_STATUS_REBOOT_MCU,  //模块通知MCU重启  
    WIFI_STATUS_STATION_DISCONNECTED, //没连接路由器
    WIFI_STATUS_STATION_CONNECTING,  //正在连接路由器
    WIFI_STATUS_STATION_CONNECTED,  //已连接路由器
    WIFI_STATUS_AP_NO_CONNECTED,	//无人连接
    WIFI_STATUS_AP_CONNECTED,	//已有人连接
    WIFI_STATUS_AP_DISCONNECTED,	//有设备断开连接，但仍然有其他设备连接着
    WIFI_STATUS_AP_HIDDEN,	//AP已经隐藏
    WIFI_STATUS_ETH_PLUGOUT,  //以太网未插入    
    WIFI_STATUS_ETH_PLUGIN,  //以太网已插入
    WIFI_STATUS_ETH_PERMITED,  //以太网已经禁用
    WIFI_STATUS_WWW_DISABLE,  //不能上网
    WIFI_STATUS_WWW_ENABLE,  //能上网    
    WIFI_STATUS_WPS_SCAN_STOP,  //未处于搜索状态
    WIFI_STATUS_WPS_SCANNING,  //正在搜索
    WIFI_STATUS_PLAY_PAUSE,	//暂停播放
    WIFI_STATUS_PLAY_PLAYING, //正在播放
    WIFI_STATUS_LOGIN_SUCCESSFUL, //登录服务商服务器成功
    WIFI_STATUS_LOGIN_FAILED, //登录服务商服务器失败
};

//MCU端播放模式定义
enum
{
	MCU_PLAY_MODE_WIFI = 0,	//切回WiFi模式，并自动重头播放上次的歌单
	MCU_PLAY_MODE_USB = 4,	//U盘本地，如果U盘接在模块上，则同时切换回WiFi模式
	MCU_PLAY_MODE_AUX = 5,	//AUX，目前App显示为line-in
	MCU_PLAY_MODE_BT = 6,	//Bluetooth
	MCU_PLAY_MODE_MIRROR = 7,	// Mirror, 播放Mirror服务的输入，同时切换回WiFi模式
	MCU_PLAY_MODE_OPTICAL = 8,	//Optical
	MCU_PLAY_MODE_RCA = 9,	//RCA, APP不支持
	MCU_PLAY_MODE_COAXIAL = 10,	//同轴, APP不支持
	MCU_PLAY_MODE_SD = 14,	//T卡本地，如果T卡接在模块上，则同时切换回WiFi模式（2016年的版本支持）
};

//WiFi端播放模式定义
enum
{	
	WIFI_PLAY_MODE_IDLE = 0,
	WIFI_PLAY_MODE_AIRPLAY = 1,
	WIFI_PLAY_MODE_DLNA = 2,			//第三方DLNA	
	WIFI_PLAY_MODE_ALEXA = 3,			//Alexa音频	
	WIFI_PLAY_MODE_MENU_PLAY_WIFI = 10,	//歌单方式播放  010 ~ 019，其中 011 表示歌单方式播放模块端的U盘
	WIFI_PLAY_MODE_MENU_PLAY_USB = 11,	//歌单方式播放  010 ~ 019，其中 011 表示歌单方式播放模块端的U盘
	WIFI_PLAY_MODE_MENU_PLAY_SD = 16,	//歌单方式播放  010 ~ 019，其中 016 表示歌单方式播放模块端的U盘
	WIFI_PLAY_MODE_API_PLAY = 20,		//第三方http api播放    020 ~ 029， 其中 021 表示第三方http api播放模块端的U盘
	WIFI_PLAY_MODE_SPOTIFY_PLAY = 31,
	WIFI_PLAY_MODE_END_PLAY = 39,
	WIFI_PLAY_MODE_AUX = 40,
	WIFI_PLAY_MODE_BT = 41,
	WIFI_PLAY_MODE_EXT_USBTF = 42,	//外部TF或USB播放	
	WIFI_PLAY_MODE_OPTICAL = 43,
	WIFI_PLAY_MODE_RCA = 44,
	WIFI_PLAY_MODE_COAXIAL = 45,	//同轴, APP不支持
	WIFI_PLAY_MODE_HIGHT_PLAY = 49,
	WIFI_PLAY_MODE_MIRROR = 50,
	WIFI_PLAY_MODE_SLAVE = 99,	//子音箱播放
	WIFI_PLAY_MODE_AVS = 0xff,				//播放AVS语音音频	
};

//WiFi支持的播放重复模式定义
enum 
{
	WIFI_PLAY_MODE_REPEAT_ALL = 0,    // 顺序循环播放
	WIFI_PLAY_MODE_REPEAT_ONE,     // 单曲循环播放
	WIFI_PLAY_MODE_REPEAT_SHUFFLE, // 随机循环播放
	WIFI_PLAY_MODE_ONCE_SHFFLE,	//随机播放一次
	WIFI_PLAY_MODE_NO_REPEAT,	//不循环播放
	WIFI_PLAY_MODE_TOTAL_NUM
};

typedef enum _WIFI_AVS_STATUS_
{
	WIFI_AVS_STATUS_IDLE = 0,
	WIFI_AVS_STATUS_LSN,
	WIFI_AVS_STATUS_THK,
	WIFI_AVS_STATUS_TTS,
	WIFI_AVS_STATUS_ERR,
	WIFI_AVS_STATUS_DIS,
	WIFI_AVS_STATUS_DIALOGUE_END,
	WIFI_AVS_STATUS_CAO,
	WIFI_AVS_STATUS_CAC,
	WIFI_AVS_STATUS_CAI,
	WIFI_AVS_STATUS_CAA,
	WIFI_AVS_STATUS_CAN,
	WIFI_AVS_STATUS_COMMS_END,
}WIFI_AVS_STATUS;

//凯叔智能故事机电台模式
enum 
{
	WIFI_PLAY_KAISHU_RADIO_DEFAULT = 1,    	//凯叔电台
	WIFI_PLAY_KAISHU_RADIO_SLEEP,			//哄睡模式
	WIFI_PLAY_KAISHU_RADIO_TIPS,			//小知识
	WIFI_PLAY_KAISHU_RADIO_CUSTOM,			//自定义
	WIFI_PLAY_KAISHU_RADIO_FAVORITE,		//收藏夹
	WIFI_PLAY_KAISHU_RADIO_MAX
};

//WiFi工作状态变量定义
typedef struct _WIFI_WORK_STATE_
{	
	uint32_t InitState			:2;	//模块初始化状态	
	uint32_t PlayState			:2;	//播放状态
	uint32_t GroupState			:2; //WPS搜索状态	
	uint32_t FactoryState		:2; //出厂恢复状态	
	uint32_t FirmwareState		:2; //WiFi模组固件升级状态	
	uint32_t TestModeState		:2; //产测模式状态	
	uint32_t MutliRoomState		:2; //多房间状态
	uint32_t CardInsertState	:2; //卡插入状态
	uint32_t USBInsertState		:2; //USB插入状态
	uint32_t PowerState			:2; //WiFi上电状态(000 loop all 001 single loop 002 shuffle loop 003 shuffle once Others: no loop)
	uint32_t RepeatMode			:4;	//WiFi播放重复模式
	uint16_t WPSState; 				//WPS搜索状态
	uint16_t StationState;			//路由器连接状态(0-没有连接 1-已经连接   2-正在连接 )
	uint16_t APState;				//AP模式热点被连接状态
	uint16_t ETHState; 				//以太网状态
	uint16_t WWWState; 				//互联网络状态
	uint16_t WiFiAppPlayMode;		//WiFi通知MCU播放模式
	uint16_t SlaveSoundBoxCnt;		//WiFi子音箱个数	
	uint8_t  IotSlaveDeviceID;		//IOT设备ID号	
	uint16_t WiFiSoundRemindState;	//WiFi语音提示状态
	uint8_t  LoginState;			//服务器登录状态
	uint16_t LedState; 				//LED状态(000 表示关闭001 表示常亮002 表示快闪003 表示慢闪)
	uint8_t  KaiShuRadio;			//凯叔故事机当前电台
	uint8_t  KaiShuVolumeMax;		//凯叔音量限制;
	bool	 KaiShuChildLock;		//凯叔儿童锁状态。
	WIFI_AVS_STATUS MicState; 		//MIC状态（000 空闲 001 录音中  002 语义识别中  003 语音回复中 004 MIC错误 ）
	bool     VisInfState; 			//WiFi消息通知状态
	bool	 WiFiNotifyChangeModeFlag;
	bool	 AlarmRemindState;		//WiFi闹钟播放状态
#ifdef FUNC_POWER_MONITOR_EN
#ifdef FUNC_WIFI_BATTERY_POWER_CHECK_EN	
	uint8_t BatPowerPercentBak;
	bool ChargeStatusFlag;
	uint8_t BatPowerPercent;
#endif
#endif
#ifdef FUNC_WIFI_POWER_KEEP_ON
	bool OtherModuleWiFiAudioEn;  //其他模式下打开WiFi声音 
	bool WiFiPowerOffRequestFlag;	
#endif
} WIFI_WORK_STATE;

// WiFi启动后WiFi相关参数初始化赋值，可根据需要修改
const static WIFI_WORK_STATE InitgWiFi =
{
	WIFI_STATUS_IDLE,            	    //模块初始化状态	
	FALSE,  	                        //播放状态
	0,                                //组网状态	
	0,                                //出厂恢复状态	
	0,                                //WiFi模组固件升级状态	
	0,                                //产测模式状态	
	0,                                //多房间状态
	0,                                //卡插入状态
	0,                                //USB插入状态
	0,                                //WiFi上电状态(000 loop all 001 single loop 002 shuffle loop 003 shuffle once Others: no loop)
	WIFI_PLAY_MODE_REPEAT_ALL,  	    //WiFi播放重复模式
	WIFI_STATUS_WPS_SCAN_STOP,        //WPS搜索状态
	WIFI_STATUS_STATION_DISCONNECTED,	//路由器连接状态(0-没有连接 1-已经连接   2-正在连接 )
	WIFI_STATUS_AP_NO_CONNECTED,    	//AP模式热点被连接状态
	WIFI_STATUS_ETH_PLUGOUT,          //以太网状态
	WIFI_STATUS_WWW_DISABLE,          //互联网络状态
	MCU_PLAY_MODE_WIFI,         			//WiFi播放模式
	0,                            		//WiFi子音箱个数	
	1,                          	    //IOT设备ID号	
	2,                              	//WiFi语音提示状态
	WIFI_STATUS_LOGIN_FAILED,
	0,                                //LED状态(000 表示关闭001 表示常亮002 表示快闪003 表示慢闪)
	WIFI_PLAY_KAISHU_RADIO_DEFAULT,   //默认凯叔电台
	100,							  //凯叔默认限制音量。
	FALSE,							  //凯叔儿童锁
	WIFI_AVS_STATUS_IDLE,             //MIC状态（000 空闲 001 录音中  002 语义识别中  003 语音回复中 004 MIC错误 ）
	0,                                //WiFi消息通知状态
	0,
	FALSE,							  //WiFi闹钟播放状态
#ifdef FUNC_POWER_MONITOR_EN
#ifdef FUNC_WIFI_BATTERY_POWER_CHECK_EN	
	0,
	FALSE,
	0,
#endif
#endif
#ifdef FUNC_WIFI_POWER_KEEP_ON
	0,                                //其他模式下打开WiFi声音 
	FALSE,
#endif
};

extern WIFI_WORK_STATE gWiFi;
extern uint8_t gCurNumberKeyNum;
extern uint16_t McuSoftSdkVerNum;	
extern TIMER WiFiSoundRemindTimer;

extern void WiFiUpgradeMcuSoftMsg(uint8_t* MsgData);
extern void WiFiUpgradeMcuSoftRunning(uint8_t* UpdateData);
extern bool WiFiPowerOnInitStateGet(void);
extern void WiFiWorkStateSet(uint8_t State);
extern void WiFiWpsStateSet(uint8_t State);
extern void WiFiStationStateSet(uint8_t State);
extern void WiFiEthStateSet(uint8_t State);
extern void WiFiWwwStateSet(uint8_t State);
extern uint8_t WiFiWwwStateGet(void);
extern void WiFiRa0StateSet(uint8_t State);
extern uint8_t WiFiRa0StateGet(void);
extern void WiFiFactoryStateSet(bool State);
extern bool WiFiFactoryStateGet(void);
extern void WiFiFirmwareUpgradeStateSet(uint8_t State);
extern uint8_t WiFiFirmwareUpgradeStateGet(void);
extern void WiFiLoginStateSet(uint8_t State);
extern uint8_t WiFiLoginStateGet(void);
extern void WiFiRequestMcuPowerOff(void);
extern void WiFiPowerStateSet(uint8_t* State);
extern void WiFiTestModeStateSet(void);
extern void WiFiSlaveSounBoxCntSet(uint16_t SoundBoxCnt);
extern uint8_t WiFiSlaveSounBoxCntGet(void);
extern void WiFiMutliRoomStateSet(bool State);
extern bool WiFiMutliRoomStateGet(void);
extern void WiFiUSBStateSet(uint8_t State);
extern bool IsWiFiUSBLink(void);
extern void WiFiCardStateSet(uint8_t State);
extern bool IsWiFiCardLink(void);
extern void WiFiSetMcuVolume(uint8_t Vol);
extern void WiFiGetMcuVolume(void);
extern void WiFiMuteStateSet(uint8_t State);
extern void WiFiGetMcuSoundRemindLanguage(void);
#ifdef FUNC_WIFI_POWER_KEEP_ON
extern void OtherModulePlayWiFiSoundRemind(bool State);
#endif
extern void WiFiSoundRemindStateSet(uint16_t State);
extern bool IsWiFiSoundRemindPlaying(void);
extern void McuSyncWiFiVolume(uint8_t Vol);
extern void McuGetWiFiPlayStateParams(void);
extern void WiFiGetProjectParams(void);
extern void WiFiControlGpioInit(void);
extern void WiFiAppSetPlayMode(uint8_t Cmd,uint8_t Mode);
extern void McuSetPlayMode(uint8_t ChangeMode);
extern void WiFiGetMcuCurPlayMode(void);
extern void WiFiSetRepeatMode(uint8_t RepeatMode);
extern void WiFiTalkStateSet(bool State);
extern void WiFiSetMcuSystemTime(uint8_t* DateData);
extern void WiFiSetMcuWeekDay(uint8_t* DayData);
extern void WiFiNoticeMcuNextAlarmTime(uint8_t* SecondData);
extern void WiFiSetMcuAlarmTime(uint8_t* AlarmTimeData);
extern void WiFiSetAlarmRemindState(bool State);
extern bool GetWiFiAlarmRemindState(void);
extern void MasterMcuRevPassThroughCmd(uint16_t Cmd);
extern void SlaveMcuRevPassThroughCmd(uint16_t Cmd);
extern void McuRevPassThroughData(uint8_t* RevData);
extern void McuSendPassThroughData(uint8_t* SendData, uint16_t DataLen);
extern void WiFiSetMicState(WIFI_AVS_STATUS State);
extern void WiFiStateCheck(void);
extern void WiFiSetMcuLedState(uint16_t State);
extern void WiFiPowerOnInitProcess(void);
extern uint32_t WiFiControl(void);
extern void WiFiKaiShuChildLockStateSet(bool State);
extern bool WiFiKaiShuChildLockStateGet(void);
extern void WiFiKaiShuVolumeMaxSet(uint8_t Vol);
extern uint8_t WiFiKaiShuVolumeMaxGet(void);
extern void WiFiKaiShuRadioSet(  uint8_t val);
extern void GpioRedWhiteLedControl(void);
#ifdef __cplusplus
}
#endif//__cplusplus

#endif

typedef uint8_t BtLinkedProfile;

//蓝牙相关
extern uint32_t FlshUpdatePairedProfile(uint8_t *addr, BtLinkedProfile profile);
extern uint32_t FlshUpdatePairedName(uint8_t *addr, uint8_t *name);
extern uint8_t ClearPairedList(uint8_t *addr);
extern void BtTryToConnect(void);

#ifdef FUNC_WIFI_BT_CONTROL_EN
typedef enum _BT_CTRL_STATUS_
{
	BT_CTRL_STATE_IDLE = 0,
	BT_CTRL_STATE_SCAN,
	BT_CTRL_STATE_PAIR,
	BT_CTRL_STATE_UNPR,
	BT_CTRL_STATE_CTDI,
	BT_CTRL_STATE_CTPL,
	BT_CTRL_STATE_DSCT,
}BT_CTRL_STATUS;

typedef struct
{
	uint8_t 		index;			//1byte
	uint8_t         bdAddr[6];		//6bytes
	uint8_t			deviceName[60];	//60bytes
    BtLinkedProfile linkedProfile;	//1byte  //note:为了节省对FLASH空间的擦写操作,PROFILE对应的保存的数据标志位,0=有效,1=无效

}BT_PAIRED_LIST_INFO;  //68byts

typedef struct _WIFI_CTRL_BT_
{	
	BT_CTRL_STATUS State;
	uint8_t ScanBtDevice[67];
	uint32_t ScanEnable_Timer;						//蓝牙进入可发现时间，单位mS	
	bool    IsScanEnableDone;
	bool    Scan_Enable;
} WIFI_CTRL_BT;

#ifndef BT_DEVICE_NUM
#define BT_DEVICE_NUM				8 //支持最多8个蓝牙设备信息保存
#endif
extern uint8_t BtDevName[50];
extern WIFI_CTRL_BT BtCtrl;
extern BT_PAIRED_LIST_INFO btPairedList[BT_DEVICE_NUM];

extern void WiFi_SendBtCmdToMcu(uint16_t WiFiCmd, uint8_t* CmdData);
extern void Mcu_SendBtCmdToWiFi(uint16_t McuCmd, uint8_t* DataBuf);
extern void WiFi_CtrlBtCmdProcess(uint8_t* gCtrlCmd);

#endif
#endif


