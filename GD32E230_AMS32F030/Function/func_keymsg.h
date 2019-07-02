////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:app_msg.h
//
//            maintainer: Halley
//
//		Description	:
//					用户可以修改该文件中的消息集和消息
//					消息集是消息值的高6Bits相同的一类消息的集合
//					每个消息集最多包含255个消息
///////////////////////////////////////////////////////////////////////////////

#ifndef	__APP_MSG_H__
#define __APP_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

	
// 播放模块消息定制区
enum
{	
	MSG_NONE				= 0x0000,	
	
	////控制消息集，各模式下的按键控制消息///////////////////////////////////////
	MSG_MAIN_CLASS			= 0x1000, 	
	MSG_COMMON_CLOSE,				// 通用模块运行退出消息，类似于WIN32中WM_CLOSE
	MSG_STOP,						// 停止
    MSG_EQ,               			// EQ切换
	MSG_REPEAT,
    MSG_FOLDER_MODE,				// 打开、关闭文件夹播放模式
    MSG_FOLDER_NEXT,				// 上一个文件夹
    MSG_FOLDER_PRE,					// 下一个文件夹
    MSG_BROWSE,						// 文件浏览
    MSG_VOL_UP,
    MSG_VOL_DW,
    MSG_PLAY_PAUSE,
    MSG_FF_START,
    MSG_FB_START,
	MSG_FF_FB_END,
    MSG_PRE,
    MSG_NEXT,
    MSG_REC,
    MSG_REC_PLAYBACK,
    MSG_REPEAT_AB,
    MSG_MUTE,
    MSG_DISK,
    MSG_POWER,
    MSG_REMIND,
	MSG_DRC,
    MSG_3D,
    MSG_VB,    
    MSG_LANG,
    MSG_MODE,
	MSG_REC_FILE_DEL,	
	MSG_MENU,	
	MSG_MIC_VOL_UP,
	MSG_MIC_VOL_DW,
	MSG_ECHO_DELAY_UP,
	MSG_ECHO_DELAY_DW,
	MSG_ECHO_DEPTH_UP,
	MSG_ECHO_DEPTH_DW,
	MSG_TREB_UP,
	MSG_TREB_DW,
	MSG_BASS_UP,
	MSG_BASS_DW,
	MSG_DUTY_UP,
    MSG_DUTY_DW,
	
	MSG_NUM_0,	
	MSG_NUM_1,	
	MSG_NUM_2,	
	MSG_NUM_3,	
	MSG_NUM_4,	
	MSG_NUM_5,	
	MSG_NUM_6,	
	MSG_NUM_7,	
	MSG_NUM_8,	
	MSG_NUM_9,	

	/* 警告类消息 */
	MSG_ERRO_CLASS			= 0x1100,
	MSG_VAL_HIGHT,
	MSG_VAL_LOW,
	MSG_VAL_NORMAL,
};

#pragma pack()

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__APP_MSG_H__
