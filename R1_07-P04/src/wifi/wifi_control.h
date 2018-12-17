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
	WIFI_PLAY_MODE_DLNA = 2,	//第三方DLNA	
	WIFI_PLAY_MODE_MENU_PLAY_WIFI = 10,	//歌单方式播放  010 ~ 019，其中 011 表示歌单方式播放模块端的U盘
	WIFI_PLAY_MODE_MENU_PLAY_USB = 11,	//歌单方式播放  010 ~ 019，其中 011 表示歌单方式播放模块端的U盘
	WIFI_PLAY_MODE_MENU_PLAY_SD = 16,	//歌单方式播放  010 ~ 019，其中 016 表示歌单方式播放模块端的U盘
	WIFI_PLAY_MODE_API_PLAY = 20,	//第三方http api播放    020 ~ 029， 其中 021 表示第三方http api播放模块端的U盘
	WIFI_PLAY_MODE_ALIAPP_PLAY = 22,	//阿里智能项目，手机播放歌曲
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
	uint16_t WPSState; //WPS搜索状态
	uint16_t StationState;	//路由器连接状态(0-没有连接 1-已经连接   2-正在连接 )
	uint16_t APState;	//AP模式热点被连接状态
	uint16_t ETHState; //以太网状态
	uint16_t WWWState; //互联网络状态
	uint16_t WiFiPlayMode;			//WiFi播放模式
	uint16_t SlaveSoundBoxCnt;		//WiFi子音箱个数	
	uint16_t WiFiSoundRemindState;	//WiFi语音提示状态
	uint16_t LedState; //LED状态(000 表示关闭001 表示常亮002 表示快闪003 表示慢闪)
	uint16_t MicState; //MIC状态（000 空闲 001 录音中  002 语义识别中  003 语音回复中 004 MIC错误 ）
	bool     MicMuteState; //MIC Mute状态（ FALSE MIC不禁止   TRUE MIC禁止）
	bool     VisInfState; //WiFi消息通知状态
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
	2,                              	//WiFi语音提示状态
	0,                                //LED状态(000 表示关闭001 表示常亮002 表示快闪003 表示慢闪)
	0,                                //MIC状态（000 空闲 001 录音中  002 语义识别中  003 语音回复中 004 MIC错误 ）
	0,                                //MIC Mute状态（ FALSE MIC不禁止   TRUE MIC禁止）
	0,                                //WiFi消息通知状态
#ifdef FUNC_POWER_MONITOR_EN
#ifdef FUNC_WIFI_BATTERY_POWER_CHECK_EN	
	0,
	FALSE,
	0,
#endif
#endif
#ifdef FUNC_WIFI_POWER_KEEP_ON
	0,                                //其他模式下打开WiFi声音 
	0,
#endif
};

extern WIFI_WORK_STATE gWiFi;
extern uint8_t gCurNumberKeyNum;
extern uint16_t McuSoftSdkVerNum;	
extern TIMER WiFiSoundRemindTimer;
extern bool IsBurningSilentState;

extern void WiFiUpgradeMcuSoftMsg(uint8_t* MsgData);
extern void WiFiUpgradeMcuSoftRunning(uint8_t* UpdateData);
extern bool WiFiPowerOnInitStateGet(void);
extern void WiFiWorkStateSet(uint8_t State);
extern void WiFiWpsStateSet(uint8_t State);
extern void WiFiStationStateSet(uint8_t State);
extern void WiFiEthStateSet(uint8_t State);
extern void WiFiWwwStateSet(uint8_t State);
extern void WiFiRa0StateSet(uint8_t State);
extern void WiFiFactoryStateSet(bool State);
extern bool WiFiFactoryStateGet(void);
extern void WiFiFirmwareUpgradeStateSet(uint8_t State);
extern uint8_t WiFiFirmwareUpgradeStateGet(void);
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
extern void WiFiAppSetPlayMode(uint8_t Mode);
extern void McuSetPlayMode(uint8_t ChangeMode);
extern void WiFiGetMcuCurPlayMode(void);
extern void WiFiSetRepeatMode(uint8_t RepeatMode);
extern void WiFiTalkStateSet(bool State);
extern void WiFiSetMcuSystemTime(uint8_t* DateData);
extern void WiFiSetMcuWeekDay(uint8_t* DayData);
extern void WiFiNoticeMcuNextAlarmTime(uint8_t* SecondData);
extern void MasterMcuRevPassThroughCmd(uint16_t Cmd);
extern void SlaveMcuRevPassThroughCmd(uint16_t Cmd);
extern void McuRevPassThroughData(uint8_t* RevData);
extern void McuSendPassThroughData(uint8_t* SendData, uint16_t DataLen);
extern void WiFiStateCheck(void);
extern void WiFiSetMcuLedState(uint16_t State);
extern void WiFiControlGpioInit(void);
extern void WiFiPowerOnInitProcess(void);
extern uint32_t WiFiControl(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
#endif


