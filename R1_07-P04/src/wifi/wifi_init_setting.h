#ifndef _WIFI_INIT_SETTING_H_
#define _WIFI_INIT_SETTING_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"

//****************************************************************************************
//					WiFi产品的相关定制参数设置
//****************************************************************************************
//**MCU+CAP+PRJproject_name&:项目名称定义
#define MCU_CAP_PRJ_PROJECT_NAME	"RHYMEET_R1"

//**MCU+SID+name&:设置设备默认SSID名称
#define MCU_SSID_NAME	"RHYMEET_R1"

//**MCU+PTV+nnn:使用的MCU 协议的版本号设置
#define MCU_PTV_NUM		002		//nnn为0~999的数字，表示使用的MCU 协议的版本号，默认000；此项设定和capacity 的位定义有依赖关系?
								//例如MCU+CAP+001的第8位表示模组的I2S master/slave 模式，在MCU协议001版本加入支持，所以要使能配置cap1 bit8就必须要设定是nnn >=001才支持。如果不发就认为是000了。

#ifdef FUNC_WIFI_ALI_PROJECT_EN
	//**MCU+ALI+PIDproductid&: 阿里小智的项目设置
	#define MCU_ALI_PID_PRODUCTID	"SUNCHAN_ENTERTAINMENT_ATALK_R1"	//如果这是一个阿里小智的项目，请回复此项，productid来自阿里提供的prodconf.json文件里面model字段，例如：EDIFIER_ENTERTAINMENT_ATALK_MA1

	//**MCU+ALV+nnn:阿里小智的项目 协议的版本号设置
	#define MCU_ALV_NUMBER 		001		//nnn为0~999的数字，表示使用的阿里项目协议版本号。
#endif

//**MCU+SPY+NAMmodelname&:Spotify认证，设置modename
#define MCU_SPY_NAM_MODE_NAME	"MV_WiFiDemo" //如果需要Spotify认证，回复此项设置modename，如果不回复默认与project_name一致

//**MCU+SPY+BRNbrandname&:Spotify认证，设置brandname
#define	MCU_SPY_BRN_BRAND_NAME	"MV_WiFiDemo" //如果需要Spotify认证，回复此项设置brandname，如果不回复默认默认与project_name一致

//**MCU+SPY+MODmodedisplaylname&:Spotify认证，设置modedisplayname
#define MCU_SPY_MOD_DISP_NAME	"MV_WiFiDemo" //如果需要Spotify认证，回复此项设置modedisplayname，如果不回复默认默认与modename一致

//**MCU+SPY+BRDbarnddisplayname&:Spotify认证，设置branddisplayname
#define MCU_SPY_BRD_DISP_NAME	"MV_WiFiDemo" //如果需要Spotify认证，回复此项设置branddisplayname，如果不回复默认默认与brandname一致

//**MCU+SPY+TYPdevicetype&:Spotify认证，设置类型
#define	MCU_SPY_TYP_DEVICE_TYPE		1	//如果需要Spotify认证，回复此项设置类型，音箱为1，盒子为0；默认为1

//**MCU+CAP+001xxxxxxxx&:功能开关变量设置
#define MCU_CAP_001_BIT0	0	//保留
#define MCU_CAP_001_BIT1	0	//是否禁止开机音乐，默认0不禁止
#define MCU_CAP_001_BIT2	0	//开机音乐是否使用user区的音频，默认0；将统一为有User区就使用user区，否则播放默认语音
#define MCU_CAP_001_BIT3	0	//是否禁止禁止语音提示，默认0不禁止
#define MCU_CAP_001_BIT4	0	//保留
#define MCU_CAP_001_BIT5	0	//语音提示是否使用user区的音频，默认0；将统一为有User区就使用user区，否则播放默认语音。
#define MCU_CAP_001_BIT6	0	//插入U盘/T卡是否UDisk/T卡的音频，默认0不自动
#define MCU_CAP_001_BIT7	0	//开机自动播放U盘/T卡是否自动播放，默认0不自动
#define MCU_CAP_001_BIT8	0	//模组I2S 模式，默认0 从模式，1 主模式，要使能这一位设定，必须要同时设定MCU+PTV+nnn：nnn>=001 才生效；只有A31/A28支持这个功能；并且固件需要是标准固件。
#define MCU_CAP_001_BIT9	1	//音量是否由MCU控制，默认有MCU的为1，否则为0,  如果音量由MCU控制，开机后WiFi不会发送音量给MCU，只会读取, 否则，开机时WIFi会发送音量给MCU
#define MCU_CAP_001_BIT10	0	//音量是否自动保存到WiFi端，默认有MCU的为0，否则为1
#define MCU_CAP_001_BIT11	0	//歌曲信息medadata是否默认是GBK编码，默认为0 UTF8编码，否则为1 GBK编码
#define MCU_CAP_001_BIT12	1	//如果MCU切换模式时不想WiFi返回AXX+PLM的指令，请把这1位设置位1，默认为0
#define MCU_CAP_001_BIT13	0	//保留
#define MCU_CAP_001_BIT14	0	//EQ是否由MCU控制，默认有MCU的为1，否则为0
#define MCU_CAP_001_BIT15	0	//EQ是否自动保存，默认有MCU的为0，否则为1
#define MCU_CAP_001_BIT16	0	//MCU是否有RTC，默认为0表示没有
#define MCU_CAP_001_BIT17	0	//保留
#define MCU_CAP_001_BIT18	0	//保留
#define MCU_CAP_001_BIT19	0	//保留
#define MCU_CAP_001_BIT20	0	//静音是否由MCU控制，默认有MCU的为1，否则为0
#define MCU_CAP_001_BIT21	0	//声道是否自动保存，否则为0
#define MCU_CAP_001_BIT22	0	//保留
#define MCU_CAP_001_BIT23	1	//WiFi模块是否有接USB麦克风，默认为没有接0，否则为1，要使能这一位设定，必须要同时设定MCU+PTV+nnn：nnn>=002 才生效；只有A31/A28支持这个功能；并且固件需要是标准固件。
#define MCU_CAP_001_BIT24	0	//WiFi模块是否有接USB无线遥控器，默认为没有接0，否则为1，要使能这一位设定，必须要同时设定MCU+PTV+nnn：nnn>=002 才生效；只有A31/A28支持这个功能；并且固件需要是标准固件。
#define MCU_CAP_001_BIT25	0	//WiFi模块是否从MCU获取时间，默认为0；0：WiFi模块通过SNTP获取时间，1：WiFi模块从MCU或者APP获取时间
#define MCU_CAP_001_BIT26	0	//是否MONO输出，默认0，双声道
#define MCU_CAP_001_BIT27	0	//子音响按ungroup按键只是从组里面解除该主音箱，默认为0；0：解散整个组，1：只是解除该子音响！?
#define MCU_CAP_001_BIT28	0	//按group键不解散其他组音箱，默认0，0：解散该路由器下的其他组，1：不解散其他组
#define MCU_CAP_001_BIT29	0	//保留
#define MCU_CAP_001_BIT30	0	//保留
#define MCU_CAP_001_BIT31	0	//保留

//**MCU+CAP+002xxxxxxxx&:功能开关变量设置
#define MCU_CAP_002_BIT0	0	//保留
#define MCU_CAP_002_BIT1	0	//保留
#define MCU_CAP_002_BIT2	0	//保留
#define MCU_CAP_002_BIT3	0	//保留
#define MCU_CAP_002_BIT4	0	//保留
#define MCU_CAP_002_BIT5	0	//保留
#define MCU_CAP_002_BIT6	0	//保留
#define MCU_CAP_002_BIT7	0	//多房间是否优先路由器转发，（备注：当主音箱和子音响在同一局域网时，不需要直连），默认0不支持
#define MCU_CAP_002_BIT8	0	//多房间是否根据路由器和主音箱的信号自动判断是否需要直连（即自动判断子音箱是否需要连接主音箱），默认0不支持，目前不建议打开
#define MCU_CAP_002_BIT9	0	//是否支持快捷连接路由器(即通过环境中其他设备获取路由器名字和密码，当用户有两个音箱，其中一个音箱已经连接路由器，另一个音箱可以不需要app就能连接路由器了。)，默认为0不支持
#define MCU_CAP_002_BIT10	0	//是否支持自动密码（当连上路由器时，设备的密码是路由器的密码，直到用户手动设置固定密码），默认为0不支持
#define MCU_CAP_002_BIT11	1	//连接路由器后，是否自动隐藏AP，默认为0，不隐藏
#define MCU_CAP_002_BIT12	0	//USB是接在MCU上还是WiFi上，默认为0表示在WiFi上或者没有USB
#if (defined(FUNC_WIFI_PLAY_CARD_EN) || defined(FUNC_WIFI_PLAY_USB_EN))
#define MCU_CAP_002_BIT13	1	//是否有T卡模式，默认为0，表示否（所谓T卡模式，是表示模式切换需不需要特别有一个T卡模式；实际上App可以直接选择T卡歌曲，所以大部分情况下不需要选择）
#else
#define MCU_CAP_002_BIT13	0
#endif
#define MCU_CAP_002_BIT14	1	//是否有语音识别功能，默认0无，设置为1表示有
#define MCU_CAP_002_BIT15	0	//是否关闭intercom功能，默认0不关闭；设置为1关闭
#ifdef FUNC_WIFI_I2S_BYPASS_NEED_EN
#define MCU_CAP_002_BIT16	1
#else
#define MCU_CAP_002_BIT16	0	//是否打开I2S Bypass Auto功能，如果打开，则处于Aux-In, BT等外部输入模式时，如果没有子音箱，则打开bypass，I2S in的数据直接I2s out；如果有子音箱，则关闭bypass，WiFi把I2S in的数据转发给子音箱（会增加1.5秒的缓冲）。
#endif
#define MCU_CAP_002_BIT17	0	//是否关闭multiroom多音箱功能，默认0不关闭
#define MCU_CAP_002_BIT18	0	//MCU是否支持同时播放和录音，不支持为1，默认0支持
#define MCU_CAP_002_BIT19	1	//是否有外部音源输入（AUX/BT...) ，如果有，还需要回复MCU+CAP+PLM
#define MCU_CAP_002_BIT20	0	//保留
#ifdef FUNC_WIFI_SUPPORT_RTC_EN
#define MCU_CAP_002_BIT21	1
#else
#define MCU_CAP_002_BIT21	0	//有无闹钟支持，默认为0，不支持
#endif
#define MCU_CAP_002_BIT22	0	//有无定时停止播放功能，默认为0，没有
#define MCU_CAP_002_BIT23	0	//有无外部定时关机功能，默认为0 （如果支持，WiFi会在定时点发送AXX+POW+OFF给MCU，MCU可回复MCU+POW+OFF给WiFi确认，如果WiFi仍然回复AXX+POW+OFF或者2秒内无回复，即可关机；如果WiFi回复AXX+BURNING，不可关机）
#define MCU_CAP_002_BIT24	0	//保留
#define MCU_CAP_002_BIT25	0	//保留
#define MCU_CAP_002_BIT26	0	//保留
#define MCU_CAP_002_BIT27	1	//MCU是否支持在线升级协议，默认为0，不支持
#define MCU_CAP_002_BIT28	0	//是否禁用以太网，默认为0不禁用，如果禁用，则即使插入网线也不能用
#define MCU_CAP_002_BIT29	1	//是否禁止WIFI模式自动播放功能，默认为0开启WIFI模式自动播放（自动播放上次播放的歌单）
#define MCU_CAP_002_BIT30	0	//保留
#define MCU_CAP_002_BIT31	0	//保留

//**MCU+CAP+LAUlanguage&:语音播报语言设置
#if (defined(FUNC_WIFI_ALI_PROJECT_EN) || defined(FUNC_WIFI_DUMI_PROJECT_EN))
	#define MCU_CAP_LAU_ENGLISH		0	//英文
	#define MCU_CAP_LAU_CHINESE		1 //中文
	#define MCU_CAP_LAU_FRENCH		0	//法文
	#define MCU_CAP_LAU_PORTUGUESE	0	//葡萄牙
	#define MCU_CAP_LAU_ITALIAN		0	//意大利
	#define MCU_CAP_LAU_GERMANY		0	//德文
	#define MCU_CAP_LAU_SPANISH		0	//西班牙
	
#elif defined(FUNC_WIFI_ALEXA_PROJECT_EN)
	#define MCU_CAP_LAU_ENGLISH		1	//英文
	#define MCU_CAP_LAU_CHINESE		0 //中文
	#define MCU_CAP_LAU_FRENCH		0	//法文
	#define MCU_CAP_LAU_PORTUGUESE	0	//葡萄牙
	#define MCU_CAP_LAU_ITALIAN		0	//意大利
	#define MCU_CAP_LAU_GERMANY		0	//德文
	#define MCU_CAP_LAU_SPANISH		0	//西班牙
#endif

//**MCU+CAP+STMxxxxxxxx&:音源开关变量设置
#if (defined(FUNC_WIFI_ALI_PROJECT_EN) || defined(FUNC_WIFI_DUMI_PROJECT_EN))
	#define MCU_CAP_STM_BIT0	0	//有无MFI airplay，默认0
	#define MCU_CAP_STM_BIT1	1	//有无Airplay，默认1，不可与位1同时为1
	#define MCU_CAP_STM_BIT2	1	//有无DLNA，默认1
	#define MCU_CAP_STM_BIT3	1	//有无QPlay，默认1
	#define MCU_CAP_STM_BIT4	0	//保留
	#define MCU_CAP_STM_BIT5	1	//局域网DMS支持，默认1
	#define MCU_CAP_STM_BIT6	0	//保留
	#define MCU_CAP_STM_BIT7	0	//保留
	#define MCU_CAP_STM_BIT8	0	//保留
	#define MCU_CAP_STM_BIT9	0	//保留
	#define MCU_CAP_STM_BIT10	1	//天天动听，默认1
	#define MCU_CAP_STM_BIT11	1	//豆瓣FM，默认1
	#define MCU_CAP_STM_BIT12	0	//保留
	#define MCU_CAP_STM_BIT13	0	//保留
	#define MCU_CAP_STM_BIT14	1	//蜻蜓，默认1
	#define MCU_CAP_STM_BIT15	1	//喜马拉雅，默认1
	#define MCU_CAP_STM_BIT16	0	//TuneIn（网络电台），默认1
	#define MCU_CAP_STM_BIT17	0	//iHeartRadio，默认1
	#define MCU_CAP_STM_BIT18	0	//Tidal，默认1
	#define MCU_CAP_STM_BIT19	0	//保留
	#define MCU_CAP_STM_BIT20	0	//保留
	#define MCU_CAP_STM_BIT21	0	//Pandora，默认1
	#define MCU_CAP_STM_BIT22	0	//Spotify，默认1
	#define MCU_CAP_STM_BIT23	0	//RHAPSODY,默认1
	#define MCU_CAP_STM_BIT24	0	// QQFM
	#define MCU_CAP_STM_BIT25	0	// VTUNER
	#define MCU_CAP_STM_BIT26	0	//ALEXA
	#define MCU_CAP_STM_BIT27	0	//保留
	#define MCU_CAP_STM_BIT28	0	//保留
	#define MCU_CAP_STM_BIT29	0	//保留
	#define MCU_CAP_STM_BIT30	0	//保留
	#define MCU_CAP_STM_BIT31	0	//保留

#elif defined(FUNC_WIFI_ALEXA_PROJECT_EN)
	#define MCU_CAP_STM_BIT0	0	//有无MFI airplay，默认0
	#define MCU_CAP_STM_BIT1	1	//有无Airplay，默认1，不可与位1同时为1
	#define MCU_CAP_STM_BIT2	1	//有无DLNA，默认1
	#define MCU_CAP_STM_BIT3	1	//有无QPlay，默认1
	#define MCU_CAP_STM_BIT4	0	//保留
	#define MCU_CAP_STM_BIT5	1	//局域网DMS支持，默认1
	#define MCU_CAP_STM_BIT6	0	//保留
	#define MCU_CAP_STM_BIT7	0	//保留
	#define MCU_CAP_STM_BIT8	0	//保留
	#define MCU_CAP_STM_BIT9	0	//保留
	#define MCU_CAP_STM_BIT10	0	//天天动听，默认1
	#define MCU_CAP_STM_BIT11	0	//豆瓣FM，默认1
	#define MCU_CAP_STM_BIT12	0	//保留
	#define MCU_CAP_STM_BIT13	0	//保留
	#define MCU_CAP_STM_BIT14	0	//蜻蜓，默认1
	#define MCU_CAP_STM_BIT15	0	//喜马拉雅，默认1
	#define MCU_CAP_STM_BIT16	1	//TuneIn（网络电台），默认1
	#define MCU_CAP_STM_BIT17	1	//iHeartRadio，默认1
	#define MCU_CAP_STM_BIT18	1	//Tidal，默认1
	#define MCU_CAP_STM_BIT19	0	//保留
	#define MCU_CAP_STM_BIT20	0	//保留
	#define MCU_CAP_STM_BIT21	1	//Pandora，默认1
	#define MCU_CAP_STM_BIT22	1	//Spotify，默认1
	#define MCU_CAP_STM_BIT23	1	//RHAPSODY,默认1
	#define MCU_CAP_STM_BIT24	0	// QQFM
	#define MCU_CAP_STM_BIT25	0	// VTUNER
	#define MCU_CAP_STM_BIT26	1	//ALEXA
	#define MCU_CAP_STM_BIT27	0	//保留
	#define MCU_CAP_STM_BIT28	0	//保留
	#define MCU_CAP_STM_BIT29	0	//保留
	#define MCU_CAP_STM_BIT30	0	//保留
	#define MCU_CAP_STM_BIT31	0	//保留
#endif

//**MCU+CAP+PLMxxxxxxxx&:模式开关变量设置
#define MCU_CAP_PLM_BIT0	0	//保留
#define MCU_CAP_PLM_BIT1	1	//有无Aux-In，默认0（如果App不需要显示当前处于Aux-In状态，例如切换到Aux状态WiFi已经关闭，则此处不需要回复1）
#define MCU_CAP_PLM_BIT2	1	//有无蓝牙，默认0（如果App不需要显示当前处于蓝牙状态，例如切换到蓝牙状态WiFi已经关闭，则此处不需要回复1）
#define MCU_CAP_PLM_BIT3	0	//有无外部插卡输入，默认0（即外部插卡接在其他芯片上不是接在WiFi模组上）
#define MCU_CAP_PLM_BIT4	0	//有无光纤输入，默认0
#define MCU_CAP_PLM_BIT5	0	//有无RCA输入，APP尚未支持
#define MCU_CAP_PLM_BIT6	0	//有无同轴输入，APP尚未支持
#define MCU_CAP_PLM_BIT7	0	//有无FM输入
#define MCU_CAP_PLM_BIT8	0	//有无第二个AUX IN
#define MCU_CAP_PLM_BIT9	0	//保留
#define MCU_CAP_PLM_BIT10	0	//保留
#define MCU_CAP_PLM_BIT11	0	//保留
#define MCU_CAP_PLM_BIT12	0	//保留
#define MCU_CAP_PLM_BIT13	0	//保留
#define MCU_CAP_PLM_BIT14	0	//保留
#define MCU_CAP_PLM_BIT15	0	//保留
#define MCU_CAP_PLM_BIT16	0	//保留
#define MCU_CAP_PLM_BIT17	0	//保留
#define MCU_CAP_PLM_BIT18	0	//保留
#define MCU_CAP_PLM_BIT19	0	//保留
#define MCU_CAP_PLM_BIT20	0	//保留
#define MCU_CAP_PLM_BIT21	0	//保留
#define MCU_CAP_PLM_BIT22	0	//保留
#define MCU_CAP_PLM_BIT23	0	//保留
#define MCU_CAP_PLM_BIT24	0	//保留
#define MCU_CAP_PLM_BIT25	0	//保留
#define MCU_CAP_PLM_BIT26	0	//保留
#define MCU_CAP_PLM_BIT27	0	//保留
#define MCU_CAP_PLM_BIT28	0	//保留
#define MCU_CAP_PLM_BIT29	0	//保留
#define MCU_CAP_PLM_BIT30	0	//保留
#define MCU_CAP_PLM_BIT31	0	//保留

//MCU+PARAMS+m:nnnn&:设置MCU升级格式和每次发送升级包的长度
#define MCU_PARAMS_FOR    0      //为0表示使用escape方式，是默认方式，为1表示使用HEX编码（数据为两倍）
#define MCU_PARAMS_VAL    1024 //每次给MCU发送升级包的数据长度，默认为1024，通常这个值请设置为MCU 数据缓存区长度的一半，但最低不能小于128，最大不能大于4096，必须是16的倍数。

//**MCU+PRESET+n&：设置预置按键数量
#define MCU_PRESET_VAL		6	//设置预置按键数量，最多为10个，为了向前兼容，0表示默认6个预置（不回复默认为0），-1表示没有预置功能。注意n前面有个+，这个指令在正常开机后发送也有效。

//MCU+CAP+PWDpassword&: 设置WiFi AP密码
#define MCU_CAP_PWD       ""     //密码为明文，不要包含特殊字符和与MCU协议冲突，长度8-15字节，这个命令在正常开机后发送无效。

//**MCU+VMX+nnn:语音提示最高允许的音量设置
#define MCU_VMX_VAL		80	       //nnn为0~100的数字，如001, 022, 100；语音提示最高允许的音量，一旦当前音量超过允许音量，软件会做降音处理，这个值设置<10或者大于90无效。

//MCU+FMT+nnn:设置WiFi模块IIS数据格式，
#define MCU_FMT_VAL   000     //nnn为000设定标准格式，nnn=001为设定Left justified格式，默认000，标准IIS格式。
#ifdef __cplusplus
}
#endif//__cplusplus

#endif


