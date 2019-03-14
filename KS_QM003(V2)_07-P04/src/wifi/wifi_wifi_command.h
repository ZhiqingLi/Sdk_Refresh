#ifndef _WIFI_WIFI_COMMAND_H_
#define _WIFI_WIFI_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//****************************************************************************************
//				WiFi	端命令集
//****************************************************************************************

//******************************************************************
// 3 出厂开机
//******************************************************************
#define AXX_CAP_GET		0x0370	//出厂开机指恢复出厂设置后的第一次开机，此时WiFi模组会发送指令给MCU获取此产品的相关定制参数并保存；此后正常开机，WiFi模组会直接使用保存的参数初始化，不再询问MCU。MCU主动发送这些回复也无效。
#define AXX_GET_SID		0x0371	//用于设置设备默认SSID名称参数name长度不大于32位，以&字符结束，name不要包含特殊字符，可以包含字母，数字，下划线，中线。

//******************************************************************
// 4 正常开机/重启/恢复出厂/休眠唤醒/电池
//******************************************************************
#define AXX_BOT_DON 	0x0470	//表示设备启动成功，但并没有完全启动，有些功能可能尚不支持，这个指令有时也用于检测WiFi是否死机
#define AXX_BOT_UP0 	0x0471	//由于模组在收到MCU+BOT+DON时会回复AXX+BOT+DON（用于心跳检测），所以额外增加这个指令用于确认设备开机。备注，这个指令在开机音之前，此时有些进程尚未初始化完成。
#define AXX_MCU_RDY  	0x0472	//表示设备已经完全启动，可以正常处理所有MCU的指令了
#define AXX_VOL_GET		0x0473	//获取当前音量
#define AXX_MCU_VER 	0x0474	//获取当前MCU的版本
#define AXX_UID_XXX  	0x0475	//模组回复MCU项目的uuid，是8个字节的ascii码，以FF开头
#define AXX_MAC_XXX  	0x0476	//模组回复MCU当前模块的MAC地址
#define AXX_DEV_RST 	0x0477	//模块告知MCU模组即将重启
#define AXX_POW_OFF 	0x0478	//WiFi模组请求关机
#define AXX_FACTORY 	0x0479	//恢复出厂设置后，设备会自动重启。
#define AXX_POW_STA 	0x047A	//WiFi模组上电状态
#define AXX_MCP_XXX 	0x047B	//APP通过wifi模块通知mcu控制系统其它模块进入电源管理状态,其中数字可能为：000： 休眠状态001： 工作状态;MCU收到wifi模块的通知后, 进行电源管理控制, 结束该操作后反馈wifi模块一个新的状态

//******************************************************************
// 5  WiFi模组固件升级命令
//******************************************************************
#define AXX_BURNING		0x0501	//模块告知MCU设备处于更新状态，请勿操作断电知道设备更新成功
								//升级成功或者失败后，WiFi都会重启，重启之前会发送AXX+DEV+RST给MCU。
#define AXX_BURN002		0x0502	//WiFi模组固件升级成功
#define AXX_BURN003		0x0503	//WiFi模组固件升级失败
#define AXX_SLI_OTA		0x0504	//WIFI模块通知MCU静默升级开始,  MCU 收到这条指令开始，则不显示任何的灯效（如升级时发送的AXX+BURNING，AXX+BURN002,AXX+BURN003等）和不播放任何提示音
#define AXX_SLI_DON		0x0505	//WiFi模组通知MCU静默升级过程结束，之后WIFI进入正常工作模式。

//******************************************************************
// 6  MCU在线升级
//******************************************************************
//#define AXX_MCU_VER 	0x0474	//获取MCU当前的版本号
#define AXX_MCU_UPD 	0x0671	//模组需要升级MCU软件
#define AXX_MCU_RUN 	0x0672	//其中index表示这是第几个包，数字为0~N（N为总的包数-1)，len表示此次发送的实际有效数据长度，一般为1024，data是发送的实际数据内容，最前面4个字节实际有效数据的check-sum。

//******************************************************************
// 7 网络命令
//******************************************************************
//WPS配网
#define AXX_WPS_ON		0x0771	//表示WPS配网模式启动

//网络状态通知
#define AXX_STA_XXX		0x0772	//通知MCU有关模组连接路由器的状态，MCU也可以发送MCU+STA+GET来询问
#define AXX_ETH_XXX		0x0773	//以太网状态，MCU也可以发送MCU+ETH+GET来询问000 无001 有插入FFF 已经禁用
#define AXX_WWW_XXX 	0x0774	//模组发给MCU互联网测试状态，000 不能上网001 能上网
#define AXX_RA0_XXX		0x0775	//模组通知有关AP模式热点的被连接情况000  无人连接001  有新设备连接到模块002  有设备断开连接，但仍然有其他设备连接着FFF  AP已经隐藏

//******************************************************************
// 8 播放命令
//******************************************************************
//音量控制
//#define AXX_VOL_GET		0x0473	//获取当前音量
#define AXX_VOL_NNN 	0x0871	//-	音量设置，nnn为0~100  如果音量是由模块自行控制的，则此命令为告知MCU当前音量
#define AXX_MUT_NNN 	0x0872	//001是mute，000是unmute。

//预置歌单和切换
#define AXX_KEY_RDY		0x0873	//模块有对应的预置按键定义
#define AXX_KEY_NIL		0x0874	//模块没有对应的预置按键定义
#define AXX_PRE_OKN		0x0875	//表示在key n预置成功
#define AXX_PRE_FFN		0x0876	//表示在key n预置失败
#define AXX_KEY_NNN		0x0877	//WiFi模组会发送AXX+KEY+nnN表示此歌单属于哪个预置
#define AXX_KEY_FFF		0X0878	//如果播放的歌曲不属于任何预置，则发送AXX+KEY+FFF

//播放状态通知
#define AXX_PLY_XXX		0x0879	//-	播放状态000 不处于播放状态001 处于播放状态		
#define AXX_PLP_XXX		0x087A	//当前循环模式,00 loop all,001 single loop,002 shuffle loop,003 shuffle once,Others: no loop
#define AXX_CHN_XXX		0x087B	//音乐声道001 左声道002 右声道000 立体声

//指定播放USB/TF卡某首歌曲
#define AXX_SEL_PLY		0x087C	//模块收到MCU发送的歌曲号数并且存在，开始播放
#define AXX_SEL_ERR 	0x087D	//模块收到MCU发送的歌曲号数但不存在

//******************************************************************
// 9  时间和定时器命令
//******************************************************************
#define AXX_SET_RTC		0x0971	//AXX+SET+RTCYYYYMMDDHHMMSS&：模块设置MCU当前系统时间，YYYYMMDDHHMMSS为时间格式，UTC时间
#define AXX_SET_WEK 	0x0972	//模块设置MCU 当前系统日期n表示星期几，0~6表示周日到周6
#define AXX_GET_RTC		0x0973	//-	模块请求MCU发送RTC时间
#define AXX_APP_RTC 	0x0974	//AXX+APP+RTCYYYYMMDDHHMMSS&：模块告知MCU当前系统时间，YYYYMMDDHHMMSS为时间格式，UTC时间
#define AXX_APP_WEK 	0x0975	//n表示星期几，0~6表示周日到周6
#define AXX_NXT_ALS		0x0976	//模块告知MCU下一个闹钟的时间. n表示为离当前时间的秒钟数.
#define AXX_NXT_ARS		0X0977	//当下一个闹钟秒钟数小于61秒,模块将会发送本指令告知MCU下一个闹钟的实际时间. n表示为离当前时间的实际秒钟数	
#define AXX_ALM_STT		0x0978	//模块告知MCU模块进入播放闹钟状态
#define AXX_ALM_BEX		0x0979	//模块告知MCU模块进入播放闹钟状态---扩展;nnn定义为十六进制标志位.位0：表示当前闹钟是否打开懒人模式
#define AXX_ALM_STP		0x097A	// 模块告知MCU模块退出播放闹钟状态
#define AXX_NXT_ALM		0x097B	//设置当前闹钟时间.
#define AXX_ALM_VAL		0x097C	//模块通知MCu，语音设置闹钟时间。

//******************************************************************
// 10 多房间功能命令
//******************************************************************
#define AXX_SLV_YES		0x0A71	//模块通知MCU 设备变成子音箱
#define AXX_SLV_NOT		0x0A72	//模块通知MCU 设备退出成子音箱
#define AXX_SLV_XXX		0x0A73	//返回子音箱的数量

//******************************************************************
// 11 AUX/BT/RCA/USB 模式切换命令
//******************************************************************
#define AXX_PLM_GET		0x0B71	//获取当前播放模式（WIFI, BT, AUX...）
#define AXX_PLM_XXX		0x0B72	//通过App切换模式时，模组可以发送AXX+PLM+nnn给MCU
#define AXX_USB_XXX		0x0B73	//模块通知MCU 000: USB拔出001: USB插入
#define AXX_MMC_XXX 	0x0B74	//模块通知MCU 000: 卡拔出001: 卡插入
#define AXX_PLM_AVS     0x0B75  //模块通知MCU临时切换WiFi播放AVS语音。

//******************************************************************
// 12  MCU配置
//******************************************************************
#define AXX_WAN_LST 	0x0C71	//返回网络中的AP列表
#define AXX_STA_INF		0x0C72	//返回模组当前的网络连接信息

//******************************************************************
// 13  LCD显示屏
//******************************************************************
#define AXX_MEA_RDY 	0x0D71	//通知MCU可以调用MCU+MEA+GET获取当前歌曲信息了。
#define AXX_MEA_DAT 	0x0D72	//模块回复歌曲信息，以AXX+MEA+DAT开头，&结束，metadata 是json格式：{ "title": "hex(歌名)", "artist": "hex(演唱者)", "album": "hex(专辑)", "vendor": "hex(来源)" }
#define AXX_ASR_DAT 	0x0D73	//模块主动上报语音识别结果给MCU，以AXX+ASR+DAT开头，&结束，asrdata 编码格式由MCU+CAP+001xxxxxxxx& bit11 决定，目前只有阿里项目支持这个接口，可以结合AXX+TLK++ON和AXX+TLK+OFF这两个状态使用
#define AXX_PLY_POS 	0x0D74	//time单位毫秒

//******************************************************************
// 14. 阿里小智平台特殊命令
//******************************************************************
//阿里平台有一个规则：MCU发送配网命令进入配网后，模块会回馈发送AXX+WPS++ON，模块配网结束不管成功或失败或超时会发送AXX+WPS+END，在这两条回馈状态期间MCU需要不响应任何按键。
#define AXX_WPS_END 	0x0E71	//模块通知MCU,配网结束
#define AXX_NET_ONN 	0x0E72	//当设备连接到阿里服务器之后，模块会发送AXX+NET+ONN
#define AXX_NET_OFF 	0x0E73	//断开后会发送AXX+NET+OFF，（AXX+WWW+001表示能联网，而NET+ONN表示已经连接到阿里服务器）
#define AXX_LED_XXX		0x0E74	//模块通知MCU，订阅更新指示灯(000 表示关闭001 表示常亮002表示快闪003 表示慢闪)
#define AXX_REC_ON		0x0E75	//在MCU发送MCU+REC++ON命令进入录音流程后，模块会回馈发送AXX+REC++ON
#define AXX_REC_OFF		0x0E76	//模块结束录音时会发送AXX+REC+OFF
#define AXX_AIM_ON		0x0E77	//在MCU发送MCU+AIM++ON命令进入群聊后，模块会回馈发送AXX+AIM++ON
#define AXX_AIM_OFF		0x0E78	//模块结束群聊时会发送AXX+AIM+OFF
#define AXX_CHT_XXX		0x0E79	//群聊更新指示灯：000 表示关闭:001 表示常亮:002 表示快闪:003 表示慢闪
#define AXX_AST_GET		0x0E7A	// WIFI模组发送向MCU请求设备状态；

//******************************************************************
// 15 产测命令
//******************************************************************
#define	AXX_TST__ON		0x0F71	//当进入产测模式，所有的按键屏蔽逻辑应该都要去掉以便于测试按键是否正常
#define	AXX_TST_OFF		0x0F72	//退出产测模式：
#define AXX_LED_TES		0x0F73	//进入产测模式，MCU可以闪烁所有的灯，并标记自己进入产测模式；直到发现模组的重启指令才清除产测模式状态

//******************************************************************
// 16 透传
//******************************************************************
//处于同一个多房间的MCU之间可以传递指令
#define AXX_M2S_NNN 	0x1071	//Slave的MCU会收到AXX+M2S+nnn
#define AXX_S2M_NNN 	0x1072	//Master的MCU会收到AXX+S2M+nnn

//App和MCU之间也可以透传数据
#define AXX_PAS_DATA	0x1073	//App发给MCU, WiFi模组收到App发过来的含有MCU+PAS+的tcp指令时，会继续接收数据直到发现 & 符号，然后修改成 AXX+PAS+data&指令给MCU

//******************************************************************
// 18 语音提示命令
//******************************************************************
#define AXX_LAU_GET		0x1271	//模块查询MCU语言，模块默认设置成英文，开机后会发送这个指令询问MCU默认开机语言，MCU回复MCU+LAU+XXX:
#define AXX_LAU_XXX		0x1272	//回复当前语言
#define AXX_PMT_XXX		0x1273	//模组播放语音提示时也会主动发送消息000 开始001 结束002 禁用003 可由MCU触发语音

//******************************************************************
// 19 I2S bypass
//******************************************************************
#define AXX_BYP_NNN 	0x1371	//回复I2S bypass 模式

//******************************************************************
// 20 语音识别命令
//******************************************************************
#define AXX_WAK_UP		0x1471	//模块开始语音识别
#define AXX_TLK_ON		0x1472	//模块开始语音识别
#define AXX_TLK_OFF		0x1473	//模块结束语音识别

//******************************************************************
// 21 EQ设置
//******************************************************************
#define AXX_PAS_EQ_TREBLE 	0x1571	//设置EQ Treble 
#define AXX_PAS_EQ_BASS 	0x1572	//设置EQ Bass
#define AXX_PAS_EQ_GET 		0x1573	//获取EQ 设置

//******************************************************************
//	22 灯光控制
//******************************************************************
#define AXX_PAS_LIGHTON 		0x1671	//- 暖光开
#define AXX_PAS_LIGHTOF 		0x1672	//- 暖光关
#define AXX_PAS_COLDON 			0x1673	//- 冷光开
#define AXX_PAS_COLDOF 			0x1674	//- 冷光关
#define AXX_PAS_LIGHTXX 		0x1675	//- 亮度设置。0-100
#define AXX_PAS_LIGHTDU 		0x1676	//- 亮度连续加
#define AXX_PAS_LIGHTDP 		0x1677	//- 亮度连续减
#define AXX_PAS_LIGHTSET 		0x1678	//- 颜色设置
#define AXX_PAS_STYLESET    	0x1679	//- 风格设置
#define AXX_PAS_LIGHTSENSON 	0x167A	//- 光感打开
#define AXX_PAS_LIGHTSENSOF 	0x167B	//- 光感关闭
#define AXX_PAS_VIBRASENSON 	0x167C	//- 震动打开
#define AXX_PAS_VIBRASENSOF 	0x167D	//- 震动关闭
#define AXX_PAS_LIGHTGET   		0x167E	//- 获取信息
#define AXX_PAS_STYLEGET	    0x167F	//- 风格获取
#define AXX_PAS_LIGHTSENSGET 	0x1680	//- 光感获取
#define AXX_PAS_VIBRASENSGET	0x1681	//- 震动获取
#define AXX_PAS_AUTO_ONNX		0x1682	//- 设置倒计时开灯
#define AXX_PAS_AUTO_OFFX		0x1683	//- 设置倒计时关灯
#define AXX_PAS_AUTO_GET		0x1684	//- 获取倒计时时间

//******************************************************************
//	24 alexa平台
//******************************************************************
#define AXX_TLK_ENA 			0x1871	//-MCU收到AXX+TLK+ENA回复后, LED不显示录音禁止的状态
#define AXX_TLK_DIS 			0x1872	//-MCU收到AXX+TLK+DIS后, LED显示录音禁止状态
#define AXX_VIS_XXX 			0x1873	//-语音状态
#define AXX_AXA_NNN 			0x1874	//-亚马逊登录状态，000成功，001失败

//******************************************************************
//	25 AVS蓝牙控制命令
//******************************************************************
#define AXX_BT_CON1 	  		0x1971	//- 通知MCU状态，蓝牙已连接
#define AXX_BT_CON0 	  		0x1972	//-  通知MCU状态 ，蓝牙已断开
//以上两个命令用于模块自带蓝牙的情况
#define AXX_BT_ENDM 	  		0x1973	//- WiFi通知MCU蓝牙进入可搜索状态
#define AXX_BT_EXDM 	  		0x1974	//- WiFi通知MCU蓝牙退出可搜索状态
#define AXX_BT_PAIR 	 		0x1975	//- WiFi通知MCU蓝牙对指定地址设备配对
#define AXX_BT_UNPR 	 		0x1976	//- WiFi通知MCU蓝牙与指定设备解除配对
#define AXX_BT_CTDI   			0x1977	//- WiFi通知MCU蓝牙与指定地址设备连接
#define AXX_BT_CTPL  			0x1978	//- WiFi通知MCU蓝牙与指定profile进行连接
#define AXX_BT_DSCT 			0x1979	//- WiFi通知MCU蓝牙与指定设备断开连接
#define AXX_BT_GPRL 			0x197A	//- WiFi通知MCU获取蓝牙配对历史信息列表
#define AXX_BT_PLAY 			0x197B	//- WiFi通知MCU控制蓝牙开始播放
#define AXX_BT_STOP 			0x197C	//- WiFi通知MCU控制蓝牙暂停播放
#define AXX_BT_PREV 			0x197D	//- WiFi通知MCU控制蓝牙上一曲
#define AXX_BT_NEXT 			0x197E	//- WiFi通知MCU控制蓝牙下一曲
#define AXX_BT_GETS 			0x197F	//- WiFi通知MCU获取蓝牙播放状态
#define AXX_BT_SACN 			0x1980	//- WiFi通知MCU控制蓝牙扫描附近设备列表
#define AXX_BT_NAME 			0x1981	//- WiFi通知MCU获取蓝牙设备名称

//******************************************************************
//26 UVS IOT指令，目前 UVS IOT只集成了Kohler项目，主要用于控制LED的亮灭、色温、亮度调节，其他功能暂未实现：
//备注：MCU指令中的 i 对应为子设备ID，如果当前设备被注册为子设备时，收到的AlexaSkill消息中会带上设备ID。
//******************************************************************
#define AXX_IOTN__ON 	  		0x1A71	//- 通知MCU状态，开启设备
#define AXX_IOTN_OFF	  		0x1A72	//- 通知MCU状态 ，关闭设备
#define AXX_IOTN_COLOR 	  		0x1A73	//- 通知MCU状态，设置相应颜色
#define AXX_IOTN_LIGHT	  		0x1A74	//- 通知MCU状态 ，设置设备亮度
#define AXX_IOTN_PER_ADD	  	0x1A75	//- 通知MCU状态 ，亮度连续加
#define AXX_IOTN_PER_SUB	  	0x1A76	//- 通知MCU状态 ，亮度连续减

//******************************************************************
//27 腾讯小微项目
//******************************************************************
#define AXX_I2S_INF		  		0x1B71	//- 当新的歌曲开始播放时,通知mcu新的I2S格式，其中：xxx ： 采样率;nn ： 位宽 
//WAC (Apple WAC & Airplay认证项目使用，其它项目不需要实现）
#define AXX_WAC__ON 	  		0x1B72	//- 进入WAC状态,，必须有状态一直显示在WAC状态（LED灯或者显示屏）
#define AXX_WAC_OFF 	  		0x1B73	//- 退出WAC状态，不能显示在WAC状态

//******************************************************************
//28 凯叔智能故事机特殊命令
//******************************************************************
#define AXX_RAD_XXX 	  		0x1C71	//- WiFi通知MCU当前播放的凯叔电台
#define AXX_LGT__ON 	  		0x1C72	//- WiFi通知MCU小夜灯打开
#define AXX_LGT_OFF 	 		0x1C73	//- WiFi通知MCU小夜灯关闭
#define AXX_LCK__ON 	 		0x1C74	//- WiFi通知MCU开启儿童锁
#define AXX_LCK_OFF   			0x1C75	//- WiFi通知MCU关闭儿童锁
#define AXX_VLM_XXX  			0x1C76	//- WiFi通知MCU最大音量限制
#define AXX_SLP__ON				0x1C77	//- WiFi通知MCU，进入哄睡模式
#define AXX_SLP_OFF				0x1C78	//- WiFi通知MCU，退出哄睡模式

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

