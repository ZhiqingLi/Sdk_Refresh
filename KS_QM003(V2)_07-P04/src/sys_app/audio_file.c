///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: line_in_control.c
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////
#include "app_config.h"
#include "breakpoint.h"
#include "i2s.h"
#include "audio_path.h"
#include "pcm_fifo.h"
#include "task_decoder.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "dev_detect_driver.h"
#include "sd_card.h"
#include "eq.h"
#include "eq_params.h"
#include "clk.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "mixer.h"
#include "dac.h"
#include "breakpoint.h"
#include "recorder_control.h"
#include "nvm.h"
#include "sys_app.h"
#include "browser.h"
#include "lrc.h"
#include "eq.h"
#include "eq_params.h"
#include "timer_api.h"
#include "string_convert.h"
#include "i2s_in_control.h"
#include "player_control.h"
#include "type.h"
#include "gpio.h"
#include "uart.h"
#include "watchdog.h"
#include "get_bootup_info.h"
#include "rtc_control.h"

#ifdef CFG_WAV_REMINDSOUND_MIX_EN

typedef struct _MIX_SOUND_TABLE_ITEM
{
 	uint16_t Id;
	uint32_t IdChn;
	uint32_t IdEng;

} MIX_SOUND_TABLE_ITEM;

#define STRING_TO_UINT(s)	((s[0] << 0) | (s[1] << 8) | (s[2] << 16) | (s[3] << 24))	
static const MIX_SOUND_TABLE_ITEM SoundTable[] = 
{
//   ID                                 中文ID                		  英文ID
	{SOUND_WIFI_LOAD,				STRING_TO_UINT("WFLD"),			STRING_TO_UINT("LOAD")},	//WIFI启动中
	{SOUND_ALARM_RING1,				STRING_TO_UINT("NZS1"),			STRING_TO_UINT("NZS1")},	//50
	{SOUND_ALARM_RING2,				STRING_TO_UINT("NZS2"),			STRING_TO_UINT("NZS2")},	//100
	{SOUND_ALARM_RING3,				STRING_TO_UINT("NZS3"),			STRING_TO_UINT("NZS3")},	//150
	{SOUND_ALARM_RING4,				STRING_TO_UINT("NZS4"),			STRING_TO_UINT("NZS4")},	//200  	
	{SOUND_ALARM_RING5,				STRING_TO_UINT("NZS5"),			STRING_TO_UINT("NZS5")},	//200  	
};

typedef struct _MIXER_SOUND_CONTROL
{
	bool     IsRunning;
	uint32_t RemindSoundAddr;
	uint32_t RemindSoundSize;
	uint8_t  RemindSoundChannel;

} MIXER_SOUND_CONTROL;

static MIXER_SOUND_CONTROL gMixer;

//检查是否正在播放提示音
void IsRmsPcmDataRemindInit(void)
{
	memset (&gMixer, 0x00, sizeof(MIXER_SOUND_CONTROL));
}

//检查是否正在播放提示音
bool IsRmsPcmDataReminding(void)
{
	if(gSys.CurModuleID >= MODULE_ID_IDLE)
	{
		return FALSE;
	}
	return gMixer.IsRunning;
}

void RmsPcmDataRemindingSet(bool State)
{
	if(State)
	{		 
		PhubPathSel(ADCIN2PMEM_PCMFIFO2DACOUT);
		MixerUnmute(MIXER_SOURCE_MIC);
		gSys.MicEnable = TRUE;		
		//打开MIX提示音播放
		gMixer.IsRunning = TRUE;
		gMixer.RemindSoundChannel = 1;
	}
	else
	{	
		//关闭MIX提示音播放
		gMixer.IsRunning = FALSE;
		gMixer.RemindSoundChannel = 1;
		MixerMute(MIXER_SOURCE_MIC);	
		gSys.MicEnable = FALSE;
	}
}

//查找提示音存放位置
void MixerSoundRemind(uint16_t SoundId)
{
	uint16_t i;
	uint32_t ConstDataId;

	gMixer.IsRunning = FALSE;
    
    APP_DBG("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	//通过SoundId查找ConstDataId
	for(i = 0; i < sizeof(SoundTable)/sizeof(MIX_SOUND_TABLE_ITEM); i++)
	{
		if(SoundTable[i].Id == SoundId)
		{
			ConstDataId = (gSys.LanguageMode == LANG_ZH) ? SoundTable[i].IdChn : SoundTable[i].IdEng; 
			break;	//找到
		}
	}	
	if(i >= sizeof(SoundTable)/sizeof(MIX_SOUND_TABLE_ITEM))
	{
		APP_DBG("Can not get the SoundId : %d\n", SoundId);
		gMixer.IsRunning = 0;
		return;		//没有找到
	}
    
	if(SpiFlashConstGetInfo(ConstDataId, &gMixer.RemindSoundSize, &gMixer.RemindSoundAddr) < 0)
	{
		APP_DBG("Get const data info error!\n");
		gMixer.IsRunning = 0;
		return;
	}
    
    APP_DBG("ConstDataSize = %d\n", gMixer.RemindSoundSize);
    APP_DBG("ConstDataAddr = 0x%x\n", gMixer.RemindSoundAddr);
	APP_DBG("Get flash const data OK! gSys.LanguageMode = %d;\n", gSys.LanguageMode);
    APP_DBG("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

	//打开MIC通路音量，开始播放提示音
	RmsPcmDataRemindingSet(TRUE);
}

//MIX音频数据填充
uint32_t RmsPcmDataRead(uint8_t* Buffer, uint32_t NumSamplesRequired)
{
    uint32_t Required;
    uint32_t ReadSize;
    
    if(gMixer.RemindSoundChannel == 2)
	{
		Required = NumSamplesRequired * 4;
	}
	else
	{
		Required = NumSamplesRequired * 2;
	}
	ReadSize = gMixer.RemindSoundSize >= Required ? Required : gMixer.RemindSoundSize;
	//APP_DBG("RmsPcmDataRead = %d; %d;\n", ReadSize, Required);
	
    if(ReadSize)
    {
        SpiFlashConstDataRead(Buffer, ReadSize, gMixer.RemindSoundAddr);

        if(ReadSize < Required)
        {
            memset(Buffer + ReadSize, 0, Required - ReadSize);
        }

        gMixer.RemindSoundAddr += ReadSize;
        gMixer.RemindSoundSize -= ReadSize;
    }
    else 
    {
    	//关闭MIX提示音播放
		RmsPcmDataRemindingSet(FALSE);
		APP_DBG ("Stop rms pcm data reminding!!!\n");
    	return FALSE;
    }

    return ReadSize / 2;
}

#endif

