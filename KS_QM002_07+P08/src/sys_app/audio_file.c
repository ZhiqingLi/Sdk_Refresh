///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: line_in_control.c
//  maintainer: lilu
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "type.h"
#include "sys_app.h"
#include "sound_remind.h"
#include "sys_vol.h"
#include "adc.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"
#ifdef CFG_WAV_REMIND_IN_CODE_AREA
#include "wav_remind_file.c"
#endif


#ifdef CFG_WAV_REMINDSOUND_MIX_EN

#define 	WAV_REMIND_DET_SCAN_TIME			1
#define 	WAV_REMIND_DET_JITTER_TIME			20
#define		WAV_REMIND_DET_COUNT				11 			//key count per adc channel
#define		WAV_REMIND_DET_FULL_VAL				4096		//

static const uint16_t WavRemindDetEvent[] = 
{
	MSG_NONE,
	MSG_NONE,
	MSG_NONE,
	MSG_NUM_0,			
	MSG_NUM_1,
	MSG_NUM_2,
	MSG_NUM_3,
	MSG_NUM_4,
	MSG_NUM_5,
	MSG_NUM_6,
	MSG_NONE,
};	

typedef enum _WAV_REMIND_DET_STATE
{
    WAV_REMIND_DET_STATE_IDLE,
    WAV_REMIND_DET_STATE_JITTER,

} WAV_REMIND_DET_STATE;


TIMER					WavRemindDetScanTimer;
WAV_REMIND_DET_STATE	WavRemindDetState;

#if defined(CFG_WAV_REMIND_DET_ADC_EN)
// Key process, image key value to key event.
static uint8_t AdcRemindIDDetGet(uint8_t Channel)
{
	uint16_t	Val;
	uint8_t	KeyIndex = 0xFF;

	Val = SarAdcChannelGetValue(Channel);

	if(Val >= (WAV_REMIND_DET_FULL_VAL - (WAV_REMIND_DET_FULL_VAL / WAV_REMIND_DET_COUNT)))
	{
		return 0xFF;	//no pressed key.
	}
	KeyIndex = (Val + (WAV_REMIND_DET_FULL_VAL / WAV_REMIND_DET_COUNT) / 2) * WAV_REMIND_DET_COUNT / WAV_REMIND_DET_FULL_VAL;
	return KeyIndex;
}

// adc key scan
// return: 0---no key, else---key msg
void AdcRemindDetScan(void)
{
	static 	uint8_t 	PreKeyIndex = 0xFF;
	uint8_t				KeyIndex;
	uint16_t			Msg = MSG_NONE;

	if(!IsTimeOut(&WavRemindDetScanTimer))
	{
		return;
	}
	TimeOutSet(&WavRemindDetScanTimer, WAV_REMIND_DET_SCAN_TIME);

	KeyIndex = AdcRemindIDDetGet(ADC_REMIND_DET_PORT);
	
	switch(WavRemindDetState)
	{
		case WAV_REMIND_DET_STATE_IDLE:
			if(KeyIndex != 0xFF)
			{
				PreKeyIndex = KeyIndex;
				APP_DBG("Adc Remind ID Detect Get KeyIndex = %d,GOTO JITTER!\n", KeyIndex);
				WavRemindDetState = WAV_REMIND_DET_STATE_JITTER;
				Msg = WavRemindDetEvent[KeyIndex];
			}
			break;

		case WAV_REMIND_DET_STATE_JITTER:
			if(PreKeyIndex != KeyIndex)
			{
				//DBG("GOTO IDLE Because jitter!\n");
				WavRemindDetState = WAV_REMIND_DET_STATE_IDLE;
			}
			else if(IsTimeOut(&WavRemindDetScanTimer))
			{
				//DBG("GOTO PRESS_DOWN!\n");
				//超时，按键有效
				WavRemindDetState = WAV_REMIND_DET_STATE_JITTER;
			}
			
			TimeOutSet(&WavRemindDetScanTimer, WAV_REMIND_DET_JITTER_TIME);
			Msg = WavRemindDetEvent[0];
			break;

		default:
			Msg = MSG_NONE;
			WavRemindDetState = WAV_REMIND_DET_STATE_IDLE;
			break;
	}

	if(Msg != MSG_NONE)
	{
		APP_DBG("AdcRemindDet Msg : %d\n", Msg);
		MsgSend(Msg);

#ifdef FUNC_SLEEP_EN
		gSys.SleepTimeCnt = FALSE;
#endif

#ifdef FUNC_SLEEP_LEDOFF_EN
		gSys.SleepLedOffCnt = FALSE;
		gSys.SleepLedOffFlag = FALSE;
#endif
	}
}

int32_t AdcRemindDetInit(void)
{
	DBG("AdcRemindDetInit()\n");

	WavRemindDetState = WAV_REMIND_DET_STATE_IDLE;
	TimeOutSet(&WavRemindDetScanTimer, 0);

#ifdef CFG_WAV_REMIND_DET_ADC_EN
	SarAdcGpioSel(ADC_REMIND_DET_PORT);
#endif
	return 0;
}
#elif defined(CFG_WAV_REMIND_DET_GPIO_EN)

//GPIO检测初始化
void GpioRemindDetInit(void)
{
	DBG("GpioRemindDetInit()\n");

#ifdef GPIO_KEY1_DETECT_BIT_MASK
	//设为输入，带上拉
	GpioClrRegBits(GPIO_KEY1_DETECT_PORT_OE, GPIO_KEY1_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY1_DETECT_PORT_PU, GPIO_KEY1_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY1_DETECT_PORT_PD, GPIO_KEY1_DETECT_BIT_MASK);
	GpioSetRegBits(GPIO_KEY1_DETECT_PORT_IE, GPIO_KEY1_DETECT_BIT_MASK);
#endif

#ifdef GPIO_KEY2_DETECT_BIT_MASK
	//设为输入，带上拉
	GpioClrRegBits(GPIO_KEY2_DETECT_PORT_OE, GPIO_KEY2_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY2_DETECT_PORT_PU, GPIO_KEY2_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY2_DETECT_PORT_PD, GPIO_KEY2_DETECT_BIT_MASK);
	GpioSetRegBits(GPIO_KEY2_DETECT_PORT_IE, GPIO_KEY2_DETECT_BIT_MASK);
#endif

#ifdef GPIO_KEY3_DETECT_BIT_MASK
	//设为输入，带上拉
	GpioClrRegBits(GPIO_KEY3_DETECT_PORT_OE, GPIO_KEY3_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY3_DETECT_PORT_PU, GPIO_KEY3_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY3_DETECT_PORT_PD, GPIO_KEY3_DETECT_BIT_MASK);
	GpioSetRegBits(GPIO_KEY3_DETECT_PORT_IE, GPIO_KEY3_DETECT_BIT_MASK);
#endif

#ifdef GPIO_KEY4_DETECT_BIT_MASK
	//设为输入，带上拉
	GpioClrRegBits(GPIO_KEY4_DETECT_PORT_OE, GPIO_KEY4_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY4_DETECT_PORT_PU, GPIO_KEY4_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY4_DETECT_PORT_PD, GPIO_KEY4_DETECT_BIT_MASK);
	GpioSetRegBits(GPIO_KEY4_DETECT_PORT_IE, GPIO_KEY4_DETECT_BIT_MASK);
#endif

#ifdef GPIO_KEY5_DETECT_BIT_MASK
	//设为输入，带上拉
	GpioClrRegBits(GPIO_KEY5_DETECT_PORT_OE, GPIO_KEY5_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY5_DETECT_PORT_PU, GPIO_KEY5_DETECT_BIT_MASK);
	GpioClrRegBits(GPIO_KEY5_DETECT_PORT_PD, GPIO_KEY5_DETECT_BIT_MASK);
	GpioSetRegBits(GPIO_KEY5_DETECT_PORT_IE, GPIO_KEY5_DETECT_BIT_MASK);
#endif

	WavRemindDetState = WAV_REMIND_DET_STATE_IDLE;
	TimeOutSet(&WavRemindDetScanTimer, 0);
}

// Key process, image key value to key event.
static uint8_t GpioRemindIDDetGet(void)

{
	uint8_t KeyIndex = 0xFF;

	if(!(GpioGetReg(GPIO_KEY1_DETECT_PORT_IN) & GPIO_KEY1_DETECT_BIT_MASK))
	{			
		KeyIndex = 4;
	}
	else if(!(GpioGetReg(GPIO_KEY2_DETECT_PORT_IN) & GPIO_KEY2_DETECT_BIT_MASK))
	{			
		KeyIndex = 4;
	}
	else if(!(GpioGetReg(GPIO_KEY3_DETECT_PORT_IN) & GPIO_KEY3_DETECT_BIT_MASK))
	{			
		KeyIndex = 5;
	}
	else if(!(GpioGetReg(GPIO_KEY4_DETECT_PORT_IN) & GPIO_KEY4_DETECT_BIT_MASK))
	{			
		KeyIndex = 4;
	}
	else if(!(GpioGetReg(GPIO_KEY5_DETECT_PORT_IN) & GPIO_KEY5_DETECT_BIT_MASK))
	{			
		KeyIndex = 4;
	}

	return KeyIndex;
}

// adc key scan
// return: 0---no key, else---key msg
void GpioRemindDetScan(void)
{
	static 	uint8_t 	PreKeyMsg = 0xFF;
	uint8_t				CurKeyMsg;
	uint16_t			Msg = MSG_NONE;

	CurKeyMsg = GpioRemindIDDetGet();
	
	switch(WavRemindDetState)
	{
		case WAV_REMIND_DET_STATE_IDLE:
			if((CurKeyMsg != 0xFF) && IsTimeOut(&WavRemindDetScanTimer))
			{
				PreKeyMsg = CurKeyMsg;
				Msg = WavRemindDetEvent[CurKeyMsg];
				WavRemindDetState = WAV_REMIND_DET_STATE_JITTER;
				TimeOutSet(&WavRemindDetScanTimer, WAV_REMIND_DET_SCAN_TIME);
			}
			break;

		case WAV_REMIND_DET_STATE_JITTER:
			if(PreKeyMsg != CurKeyMsg)
			{
				//DBG("GOTO IDLE Because jitter!\n");
				WavRemindDetState = WAV_REMIND_DET_STATE_IDLE;
			}
			else if(IsTimeOut(&WavRemindDetScanTimer))
			{
				//超时
				WavRemindDetState = WAV_REMIND_DET_STATE_JITTER;
			}
			Msg = WavRemindDetEvent[0];
			TimeOutSet(&WavRemindDetScanTimer, WAV_REMIND_DET_JITTER_TIME);
			break;

		default:
			Msg = MSG_NONE;
			WavRemindDetState = WAV_REMIND_DET_STATE_IDLE;
			break;
	}

	if(Msg != MSG_NONE)
	{
		APP_DBG("GpioRemindDet Msg : %x; CurKeyMsg = %d\n", Msg, CurKeyMsg);
		MsgSend(Msg);

#ifdef FUNC_SLEEP_EN
		gSys.SleepTimeCnt = FALSE;
#endif

#ifdef FUNC_SLEEP_LEDOFF_EN
		gSys.SleepLedOffCnt = FALSE;
		gSys.SleepLedOffFlag = FALSE;
#endif
	}
}

#endif


#if  CFG_WAV_REMIND_IN_CODE_AREA || CFG_WAV_REMIND_IN_CONST_AREA

#if CFG_WAV_REMIND_IN_CONST_AREA
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

	{SOUND_KS_KNOCK_SOUND_RING1,	STRING_TO_UINT("KSR1"),			STRING_TO_UINT("KSR1")},	//POWER_ON  
	{SOUND_KS_KNOCK_SOUND_RING2,	STRING_TO_UINT("KSR2"),			STRING_TO_UINT("KSR2")},	//POWER_OFF
	{SOUND_KS_KNOCK_SOUND_RING3,	STRING_TO_UINT("KSR3"),			STRING_TO_UINT("KSR3")},	//LOW_POWER  
	{SOUND_KS_KNOCK_SOUND_RING4,   	STRING_TO_UINT("KSR4"),		   	STRING_TO_UINT("KSR4")},    //POWER_ON  
	{SOUND_KS_KNOCK_SOUND_RING5,    STRING_TO_UINT("KSR5"),		   	STRING_TO_UINT("KSR5")},    //POWER_OFF
	{SOUND_KS_KNOCK_SOUND_RING6,    STRING_TO_UINT("KSR6"),		   	STRING_TO_UINT("KSR6")},    //LOW_POWE
	{SOUND_KS_KNOCK_SOUND_RING7,	STRING_TO_UINT("KSR7"),			STRING_TO_UINT("KSR7")},	//POWER_ON  
	{SOUND_KS_KNOCK_SOUND_RING8,	STRING_TO_UINT("KSR8"),			STRING_TO_UINT("KSR8")},	//POWER_OFF
	{SOUND_KS_KNOCK_SOUND_RING9,	STRING_TO_UINT("KSR9"),			STRING_TO_UINT("KSR9")},	//LOW_POWER  
};
#endif

#if CFG_WAV_REMIND_IN_CODE_AREA
typedef struct _MIX_SOUND_TABLE_ITEM
{
 	uint16_t Id;
	uint8_t *Addr;
	uint32_t Size;

} MIX_SOUND_TABLE_ITEM;

static const MIX_SOUND_TABLE_ITEM SoundIDTable[] = 
{
//   ID                                 地址                		       文件大小

	{SOUND_KS_KNOCK_SOUND_RING1,	(uint8_t *)KS_KnockSound1,		sizeof(KS_KnockSound1)		},	//POWER_ON  
	{SOUND_KS_KNOCK_SOUND_RING2,	(uint8_t *)KS_KnockSound2,		sizeof(KS_KnockSound2)		},	//POWER_ON
	{SOUND_KS_KNOCK_SOUND_RING3,	(uint8_t *)KS_KnockSound3,		sizeof(KS_KnockSound3)		},	//POWER_ON  
	{SOUND_KS_KNOCK_SOUND_RING4,	(uint8_t *)KS_KnockSound4,		sizeof(KS_KnockSound4)		},	//POWER_ON  
	{SOUND_KS_KNOCK_SOUND_RING5,	(uint8_t *)KS_KnockSound5,		sizeof(KS_KnockSound5)		},	//POWER_ON
	{SOUND_KS_KNOCK_SOUND_RING6,	(uint8_t *)KS_KnockSound6,		sizeof(KS_KnockSound6)		},	//POWER_ON
	{SOUND_KS_KNOCK_SOUND_RING7,	(uint8_t *)KS_KnockSound7,		sizeof(KS_KnockSound7)		},	//POWER_ON  
	{SOUND_KS_KNOCK_SOUND_RING8,	(uint8_t *)KS_KnockSound8,		sizeof(KS_KnockSound8)		},	//POWER_ON
	{SOUND_KS_KNOCK_SOUND_RING9,	(uint8_t *)KS_KnockSound9,		sizeof(KS_KnockSound9)		},	//POWER_ON 
};

#endif

typedef struct _MIXER_SOUND_CONTROL
{
	bool     IsRunning;

#if CFG_WAV_REMIND_IN_CODE_AREA
    uint8_t *RemindSoundAddr;
#else
	uint32_t RemindSoundAddr;
#endif
	uint32_t RemindSoundSize;
	uint8_t  RemindSoundChannel;

} MIXER_SOUND_CONTROL;

static MIXER_SOUND_CONTROL gMixer;

//检查是否正在播放提示音
bool IsRmsPcmDataReminding(void)
{
	if(gSys.CurModuleID >= MODULE_ID_IDLE)
	{
		return FALSE;
	}
	return gMixer.IsRunning;
}

//初始化
void MixerSoundRemindInit(void)
{
	//关闭MIC通路音量
	gSys.MicVolume = FALSE;
#ifdef CFG_WAV_REMIND_DET_ADC_EN
	AdcRemindDetInit();
#endif
#ifdef CFG_WAV_REMIND_DET_GPIO_EN
	GpioRemindDetInit();
#endif
	memset(&gMixer, 0, sizeof(MIXER_SOUND_CONTROL));
}

//查找提示音存放位置
void MixerSoundRemind(uint16_t SoundId)
{
	uint16_t i;
	uint32_t ConstDataId;

	gMixer.IsRunning = FALSE;
    
    APP_DBG("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
#if CFG_WAV_REMIND_IN_CONST_AREA
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
    
    APP_DBG("From flash const area get ConstDataId = 0x%X\n", ConstDataId);
	if(SpiFlashConstGetInfo(ConstDataId, &gMixer.RemindSoundSize, &gMixer.RemindSoundAddr) < 0)
	{
		APP_DBG("Get const data info error!\n");
		gMixer.IsRunning = 0;
		return;
	}

#endif
	
#if CFG_WAV_REMIND_IN_CODE_AREA
	//通过SoundId查找ConstData
	for(i = 0; i < sizeof(SoundIDTable)/sizeof(MIX_SOUND_TABLE_ITEM); i++)
	{
		if(SoundIDTable[i].Id == SoundId)
		{
			ConstDataId = i;
			gMixer.RemindSoundAddr = SoundIDTable[ConstDataId].Addr;
			gMixer.RemindSoundSize = SoundIDTable[ConstDataId].Size;
			break;	//找到
		}
	}

	if(i >= sizeof(SoundIDTable)/sizeof(MIX_SOUND_TABLE_ITEM))
	{
		APP_DBG("Can not get the SoundId : %d\n", SoundId);
		gMixer.IsRunning = 0;
		return;		//没有找到
	}

     APP_DBG("From flash code area get ConstDataId = 0x%X\n", ConstDataId);
#endif
    
    APP_DBG("ConstDataSize = %d\n", gMixer.RemindSoundSize);
    APP_DBG("ConstDataAddr = 0x%x\n", gMixer.RemindSoundAddr);
	APP_DBG("Get flash const data OK! gSys.LanguageMode = %d;\n", gSys.LanguageMode);
    APP_DBG("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

	//打开MIC通路音量，开始播放提示音
	gSys.MicVolume = MAX_MICIN_VOLUME;
	SetSysVol();
    gMixer.IsRunning = TRUE;
	gMixer.RemindSoundChannel = 1;
	
}

//MIX音频数据填充
uint32_t RmsPcmDataRead(uint8_t* Buffer, uint32_t NumSamplesRequired)
{
    uint32_t Required;
    uint32_t ReadSize;

    if(gMixer.RemindSoundSize == FALSE)
    {
    	//关闭MIC通路音量
		gSys.MicVolume = FALSE;
		SetSysVol();
		gMixer.IsRunning = FALSE;
    	return FALSE;
    }

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
    #if CFG_WAV_REMIND_IN_CONST_AREA
        SpiFlashConstDataRead(Buffer, ReadSize, gMixer.RemindSoundAddr);
	#endif
	#if CFG_WAV_REMIND_IN_CODE_AREA
        memcpy(Buffer, gMixer.RemindSoundAddr, ReadSize);
    #endif
        if(ReadSize < Required)
        {
            memset(Buffer + ReadSize, 0, Required - ReadSize);
        }

        gMixer.RemindSoundAddr += ReadSize;
        gMixer.RemindSoundSize -= ReadSize;
    }
    else
    {
    #if CFG_WAV_REMIND_IN_CONST_AREA
        gMixer.RemindSoundAddr = 0;
	#else
	    gMixer.RemindSoundAddr = NULL;
	#endif
		gMixer.IsRunning = FALSE;
		gMixer.RemindSoundSize = 0;
		gMixer.RemindSoundChannel = 1;
    }

    return ReadSize / 2;
}

#endif
#endif

