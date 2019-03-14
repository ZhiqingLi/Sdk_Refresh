#ifndef _WIFI_FUNCTION_CONFIG_H_
#define _WIFI_FUNCTION_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//****************************************************************************************
//					WiFi 模组功能模块选择配置
//****************************************************************************************
#define FUNC_WIFI_EN   //打开此宏定义，支持 WiFi 功能

#ifdef FUNC_WIFI_EN
//************************************************
//	以下宏定义必须保持打开
//************************************************
#define FUNC_OUTPUT_CHANNEL_AUTO_CHANGE		//打开此宏定义，支持输出模式自动切换功能开关

//************************************************
//	以下宏定义可以根据功能需要开关
//************************************************
#define FUNC_WIFI_POWER_KEEP_ON						//打开此宏定义，WiFi模组开机后电源一直保持
#define FUNC_WIFI_PLAY_CARD_EN						//打开此宏定义，支持卡端口接在WiFi端播放
//#define FUNC_WIFI_PLAY_USB_EN						//打开此宏定义，USB端口接在WiFi端播放
//#define FUNC_WIFI_BT_CONTROL_EN						//打开此宏定义，WiFi控制蓝牙功能
#define FUNC_WIFI_UART_UPGRADE						//选择通过WiFi 串口升级MCU软件功能
#define FUNC_WIFI_BATTERY_POWER_CHECK_EN			//打开此宏定义，支持电池电流检测功能
#define FUNC_WIFI_MIC_TALK_EN						//打开此宏定义，支持语音识别功能
#ifdef FUNC_WIFI_MIC_TALK_EN
	#define FUNC_WIFI_TALK_AND_AUDIO_EFFECT_EN			//打开此宏定义，WiFi有Mic语音功能同时音效对I2S In音源也有效
//	#define FUNC_KEEP_ON_MIC_IN_I2S_OUT_EN				//打开此宏定义，在WiFi模式下随时可以实现语音功能不需要按键，但是I2S In输入信号没有VB/3D等音效功能
	#define FUNC_WIFI_TALK_QUICK_OPEN_MIC_EN 			//打开此宏定义，可以实现WiFi talk快速打开关闭MIC通道,从而实现同时录放功能
#endif
//#define FUNC_WIFI_MULTIROOM_PLAY_EN					//打开此宏定义，支持多房间播放功能，所有音源通过通过I2S 输出到WiFi
#define FUNC_WIFI_SUPPORT_RTC_EN					//打开此宏定义，支持RTC功能
#define FUNC_WIFI_SUPPORT_ALARM_EN				//打开此宏定义，支持闹钟功能
//#define FUNC_WIFI_ALI_PROJECT_EN	 				//打开此宏定义，支持阿里项目特殊功能
//#define FUNC_WIFI_ALEXA_PROJECT_EN	 			//打开此宏定义，支持亚马逊项目
#define FUNC_WIFI_DUMI_PROJECT_EN	 				//打开此宏定义，支持百度度秘项目
//#define FUNC_WIFI_SPOTIFY_NEED_EN 					//打开此宏定义，支持Spotify认证
//#define FUNC_WIFI_I2S_BYPASS_NEED_EN 				//打开此宏定义，支持I2SBypass模式
#define FUNC_WIFI_PASS_THROUGH_DATA_EN				//打开此宏定义，支持WiFi 透传数据功能

#endif
#endif


