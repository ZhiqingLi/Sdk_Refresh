/**
 **************************************************************************************
 * @file    main_task.h
 * @brief   Program Entry 
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__


#include "type.h"
#include "rtos_api.h"
#include "app_config.h"
#include "mode_switch_api.h"
#include "audio_core_api.h"
#include "app_message.h"

#ifdef CFG_APP_USB_AUDIO_MODE_EN //此设备 detect需2S
#define	MODE_WAIT_DEVICE_TIME			1900
#else
#define	MODE_WAIT_DEVICE_TIME			500
#endif

typedef enum
{
	ModeStateNone	= 0,//首次启动前 状态。
	ModeStateEnter,
	ModeStateWork,
	ModeStateExit,
	ModeStatePause,//暂停态，应用：1、模式进入idle，2、deepsleep 退出到此状态，醒来再进。
}ModeState;

typedef struct _SysVolContext
{
	bool		MuteFlag;	//AudioCore软件mute
	int8_t	 	AudioSourceVol[AUDIO_CORE_SOURCE_MAX_MUN];	//Source增益控制step，小于等于32	
	int8_t	 	AudioSinkVol[AUDIO_CORE_SINK_MAX_NUM];		//Sink增益控制step，小于等于32	
	
}SysVolContext;

typedef struct _MainAppContext
{
	xTaskHandle			taskHandle;
	MessageHandle		msgHandle;
	TaskState			state;
	ModeState			MState;
	AppMode				appCurrentMode;
	AppMode				appTargetMode;//for scan /detect
	AppMode				appBackupMode;//for App backup

	uint16_t			SamplesPreFrame;
	
	uint32_t			*DACFIFO;
#ifdef CFG_RES_AUDIO_DACX_EN
	uint32_t			*DACXFIFO;
#endif
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	uint32_t			*I2SFIFO;
#endif

	uint32_t			*ADCFIFO;
	AudioCoreContext 	*AudioCore;
	uint16_t			SourcesMuteState;//纪录source源mute使能情况,目前只判断left
	
	uint16_t 			*Source0Buf_ADC1;//ADC1 取mic数据
	uint16_t 			*Sink0Buf_DAC0;
#ifdef CFG_RES_AUDIO_DACX_EN
	uint16_t			*Sink2Buf_DACX;
#endif
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	uint16_t			*Sink2Buf_I2S;
#endif

	uint32_t 			SampleRate;
	bool				DeviceSync;
	bool				AudioCoreSync;
#ifdef CFG_FUNC_DISPLAY_EN
	bool				DisplaySync;
#endif
#ifdef CFG_FUNC_REMIND_SOUND_EN
	bool				RemindSoundSync;
#endif
#ifdef CFG_FUNC_ALARM_EN
	uint32_t 			AlarmID;//闹钟ID对应bit位
	bool				AlarmFlag;
#endif
	SysVolContext		gSysVol;
    uint8_t     MusicVolume;
	uint8_t     EffectMode;
    uint8_t     MicVolume;
#ifdef CFG_FUNC_EQ_MODE_EN
	uint8_t     EqMode;
#endif
    uint8_t  	ReverbStep;	
#ifdef CFG_FUNC_TREB_BASS_EN	
	uint8_t 	BassStep;
    uint8_t 	TrebStep;
#endif
#ifdef  CFG_APP_HDMIIN_MODE_EN
	uint8_t  	hdmiArcOnFlg;
#endif
}MainAppContext;

extern MainAppContext	mainAppCt;

#define SoftFlagNoRemind		BIT(0)	//提示音故障
#define	SoftFlagDecoderRun		BIT(1)	
#define SoftFlagDecoderSwitch	BIT(2) //解码器预备切出，目的是过滤消息
#define SoftFlagDecoderApp		BIT(3) //App复用解码器
#define SoftFlagDecoderRemind	BIT(4)	//提示音占用解码器
#define SoftFlagDecoderMask		(SoftFlagDecoderRemind | SoftFlagDecoderApp | SoftFlagDecoderSwitch)//解码器掩码
#define SoftFlagRecording		BIT(5)	//录音进行标记，禁止后插先播，模式切换需清理
#define SoftFlagPlayback		BIT(6)	//回放进行标记，禁止后插先播，模式切换需清理

#if FLASH_BOOT_EN 
#define SoftFlagMvaInCard		BIT(7)	//文件预搜索发现SD卡有MVA包 卡拔除时清理
#define SoftFlagMvaInUDisk		BIT(8)	//文件预搜索发现U盘有Mva包 U盘拔除时清理
#endif
//标记上电第一次选择模式前，由于Standbylay的存在，需标记第一个模式实施状态
#define SoftFlagFirstMode		BIT(9)//SoftFlagWaitDevice 
#define SoftFlagMediaToLineRecMode	BIT(10)//SoftFlagWaitDevice 


#define SoftFlagMask			0xFFFFFFFF
void SoftFlagRegister(uint32_t SoftEvent);
void SoftFlagDeregister(uint32_t SoftEvent);
bool SoftFlagGet(uint32_t SoftEvent);

#define MEDIA_VOLUME_STR_C		("0:/")
#define MEDIA_VOLUME_STR_U		("1:/")
#define MEDIA_VOLUME_C			0
#define MEDIA_VOLUME_U			1

#ifdef CFG_COMMUNICATION_BY_UART
extern uint8_t UartRxBuf[1024];
extern uint8_t UartTxBuf[1024];
#endif

/**
 * @brief	Start a main program task.
 * @param	NONE
 * @return	
 */
int32_t MainAppTaskStart(void);


/**
 * @brief	Get message receive handle of main app
 * @param	NONE
 * @return	MessageHandle
 */
MessageHandle GetMainMessageHandle(void);

uint32_t GetSystemMode(void);

//void AudioDACInit(uint32_t sampleRate);

/**
 * @brief	clear audio core sink buffer(DAC)
 * @param	NONE
 * @return	NONE
 */
void AudioDACSinkBufClear(void);

void AudioADC1ParamsSet(uint32_t sampleRate, uint16_t gain, uint8_t gainBoostSel);

void ResumeAudioCoreMicSource(void);


#endif /*__MAIN_TASK_H__*/
