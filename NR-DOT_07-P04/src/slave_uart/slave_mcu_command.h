#ifndef _SLAVE_MCU_COMMAND_H_
#define _SLAVE_MCU_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//****************************************************************************************
//					MCU 端命令集
//****************************************************************************************

//******************************************************************
//	1 MCU发送给蓝牙
//******************************************************************
#define COM_PP      	0x01	//播放/暂停
#define COM_PA    		0x02	//播放
#define COM_PU    		0x03	//暂停
#define COM_PV      	0x04	//上一曲
#define COM_PN      	0x05	//下一曲
#define COM_VD      	0x06	//音量减
#define COM_VP      	0x07	//音量加
#define COM_MC      	0x08	//模式切换
#define COM_MBT     	0x09	//从当前模式切换到蓝牙模式
#define COM_MAX     	0x0A	//从当前模式切换到AUX模式
#define COM_MFM     	0x0B	//从当前模式切换到FM模式
#define COM_MSD     	0x0C	//从当前模式切换到SD模式
#define COM_MUD     	0x0D	//从当前模式切换到USB模式
#define COM_PWD     	0x0E	//关机
#define COM_BT_PR     0x0F	//蓝牙进入配对状态
#define COM_BT_AC     0x10	//蓝牙正在回连最后配对的设备
#define COM_BT_DC     0x11	//蓝牙断开连接
#define COM_BT_CA     0x12	//蓝牙接听来电
#define COM_BT_CE     0x13	//蓝牙结束通话
#define COM_BT_CJ     0x14	//蓝牙来电拒接
#define COM_BT_CR     0x15	//蓝牙末号重播
#define COM_BT_FR     0x16	//蓝牙快退
#define COM_BT_FF     0x17	//蓝牙快进
#define COM_REC_OK    0x18	//录音启动OK
#define COM_REC_ERR   0x19	//录音启动失败，可能是无TF卡/磁盘满
#define COM_REC_END   0x1A	//录音结束
#define COM_PA_OK     0x1B	//录音播放OK
#define COM_PA_ERR    0x1C	//录音播放错误，无卡/无录音文件
#define COM_SD_PN     0x1D	//SD卡插入
#define COM_SD_PO     0x1E	//SD卡拔出
#define COM_SD_ON     0x1F	//SD在线，这个命令只有在开机时回复主机，SD卡是否在线
#define COM_SD_OFF    0x20	//SD不在线，这个命令只有在开机时回复主机，SD卡是否在线

//******************************************************************
//	2 蓝牙发送给MCU
//******************************************************************
#define SY_PO        	0x60	//系统开机
#define SY_PF        	0x61	//系统关机
#define BT_AC        	0x62	//当前是蓝牙模式，蓝牙正在回连
#define BT_WP        	0x63	//当前是蓝牙模式，蓝牙处于配对状态
#define BT_WC        	0x64	//当前是蓝牙模式，蓝牙处于等待连接状态
#define BT_CN        	0x65	//当前是蓝牙模式，蓝牙已经连接
#define BT_PA       	0x66	//当前是蓝牙模式，蓝牙正在播放
#define BT_IC        	0x67	//当前是蓝牙模式，蓝牙有来电
#define BT_OC        	0x68	//当前是蓝牙模式，蓝牙有电话打出
#define BT_EC        	0x69	//当前是蓝牙模式，蓝牙正在通话中
#define SD_PA        	0x6A	//当前是SD卡模式，SD卡正在播放
#define SD_PU        	0x6B	//当前是SD卡模式，SD卡处于暂停状态
#define AX_PA       	0x6C	//当前是AUX模式，AUX正在播放
#define AX_PU        	0x6D	//当前是AUX模式，AUX处于暂停状态
#define REC_START     0x6E	//蓝牙通知MCU，录音启动/结束
#define REC_PA        0x6F	//蓝牙通知MCU，播放录音文件

//******************************************************************
//	3 蓝牙主机相关状态
//******************************************************************
#define MAIN_SY_OFF   0x00	//主机已经关机
#define MAIN_BT_DN    0x90	//主机在蓝牙模式并且没有蓝牙连接
#define MAIN_BT_CN    0x91	//主机在蓝牙模式并且蓝牙已经连接
#define MAIN_BT_PA    0x92	//主机在蓝牙模式并且蓝牙正在播放音乐
#define MAIN_BT_PU    0x93	//主机在蓝牙模式并且蓝牙音乐暂停
#define MAIN_AX_PA    0x94	//主机在AUX式并且正在播放音乐
#define MAIN_AX_PU    0x95	//主机在AUX式并且播放音乐暂停
#define MAIN_SY_ON    0xFF	//主机已经开机

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

