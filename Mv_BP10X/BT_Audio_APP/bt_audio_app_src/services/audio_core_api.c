/**
 **************************************************************************************
 * @file    audio_core.c
 * @brief   audio core 
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include <nds32_intrinsic.h>
#include "type.h"
#include "clk.h"
#include "freertos.h"
#include "audio_core_api.h"
#include "app_config.h"
#include "debug.h"
#include "ctrlvars.h"
#include "audio_effect.h"
#include "mode_switch_api.h"
#include "main_task.h"
#include "audio_core_api.h"

#ifdef CFG_APP_BT_MODE_EN
#include "bt_play_api.h"
#if (BT_HFP_SUPPORT == ENABLE)
#include "bt_hf_api.h"
#endif
#endif

typedef enum
{
	AC_RUN_CHECK,//用于检测是否需要暂停任务，如果需要暂停任务，则停留再该状态
	AC_RUN_GET,
	AC_RUN_PROC,
	AC_RUN_PUT,
}AudioCoreRunState;

//ControlVariablesContext gCtrlVars;

//static AudioCoreContext		AudioCore;
AudioCoreContext		AudioCore;

static uint16_t AdjustLevelHigh	= 0;//(CFG_PARA_SAMPLES_PER_FRAME * 3 / 4); //混音前buf两帧大小，获取时机是抽走一帧后。
static uint16_t AdjustLevelLow	= 0;//(CFG_PARA_SAMPLES_PER_FRAME / 4);
static AudioCoreRunState AudioState = AC_RUN_GET;

#ifdef CFG_FUNC_FREQ_ADJUST //硬件调频偏，钳位正负万分之五,(蓝牙因阻塞可加强分频)
#define ADJUST_PERIOD_COUNT		16 //水位次数 求均消抖，推荐16,32/64/128,便于计算，相当于100~2000mS左右调整一次。
#define ADJUST_CLOCK_MAX		6 //调频偏值最大单位数(正负)，对应reg值;480M分频获取audioclock reg为计数器小部分，单位1/256.
static int8_t FreqAsyncSource = 127;	//异步音源通道，127为缺省值，未启用。
static int16_t AsyncRemainLast;
static int32_t AsyncRemain;
static int8_t AdjustValue = 0;	//系统分频数(偏差)调整值 即小数部分(单位是1/256),
static int8_t LastRise = 0;	//上次水位变化情况，1:涨 0：平 -1:跌
static uint8_t Count;

/**
 * @func        AudioCoreSourceFreqAdjustEnable
 * @brief       使能系统音频分频微调，使信道之间同步(与异步音源)
 * @param       uint8_t AsyncIndex   异步音频源混音信道编号
 * @param       uint16_t LevelLow   异步音频源混音信道低水位采样点值
 * @param       uint16_t LevelHigh   异步音频源混音信道高水位采样点值
 * @Output      None
 * @return      None
 * @Others      
 * Record
 * 1.Date        : 20180518
 *   Author      : pi.wang
 *   Modification: Created function
 */
void AudioCoreSourceFreqAdjustEnable(uint8_t AsyncIndex, uint16_t LevelLow, uint16_t LevelHigh)
{
	if(AsyncIndex >= AUDIO_CORE_SOURCE_MAX_MUN)
		return ;
	Count = 0;
	FreqAsyncSource = AsyncIndex;
	AsyncRemainLast = 0xffff;
	AsyncRemain = 0;
	AdjustValue = 0;
	AdjustLevelHigh = LevelHigh;
	AdjustLevelLow = LevelLow;
}

void AudioCoreSourceFreqAdjustDisable(void)
{
	FreqAsyncSource = 127;
	AsyncRemainLast = 0;
	Clock_AudioPllClockAdjust(PLL_CLK_1, 0, 0);//恢复缺省值
	Clock_AudioPllClockAdjust(PLL_CLK_2, 0, 0);
	AdjustValue = 0;
	Count = 0;
	AdjustLevelHigh = 0;//(CFG_PARA_SAMPLES_PER_FRAME * 3 / 4);
	AdjustLevelLow = 0;//(CFG_PARA_SAMPLES_PER_FRAME / 4);
}

void AudioCoreSourceFreqAdjust(void)
{
	int8_t 	Adjust = 0;//为了保障调整频率平滑，单次可微调1单位。
	int8_t	LevelRise = 0;
	if(FreqAsyncSource < AUDIO_CORE_SOURCE_MAX_MUN
		&& ((AudioCore.AudioSource[FreqAsyncSource].Enable)))// && AudioCore.AudioSource[FreqAsyncSource].FuncDataGetLen != NULL
	{
#ifdef CFG_APP_BT_MODE_EN
		if(GetSystemMode() == AppModeBtAudioPlay)
		{
			AsyncRemain += GetValidSbcDataSize();
		}
		else
#endif
		{
			AsyncRemain += AudioCore.AudioSource[FreqAsyncSource].FuncDataGetLen();
		}

		if(++Count >= ADJUST_PERIOD_COUNT)
		{
			AsyncRemain = AsyncRemain / Count;//水位消抖。
			if(AsyncRemainLast != 0xffff)//not first
			{
				if(AsyncRemain > AsyncRemainLast)//水位涨
				{
					LevelRise = 1;
					if((AsyncRemain > AdjustLevelHigh) //buf水位 越过缓冲高限位
						|| (AsyncRemain >= AdjustLevelLow && (LastRise == 1)))//波动区间内 连续上涨
					{
						Adjust = -1; //需加快系统时钟/降低分频调整值。
					}
				}
				else if(AsyncRemain < AsyncRemainLast)//水位跌
				{
					LevelRise = -1;
					if((AsyncRemain < AdjustLevelLow)//buf水位 越过缓冲低限位
						|| ((AsyncRemain < AdjustLevelHigh) && (LastRise == -1))) //波动区间内 连续下跌
					{
						Adjust = 1; //需降低系统时钟/提高分频调整值
					}
				}

				Adjust += AdjustValue; //叠加Last频偏调整值
#ifdef CFG_APP_BT_MODE_EN
				if(Adjust > ADJUST_CLOCK_MAX && ((GetSystemMode() == AppModeBtAudioPlay)||(GetSystemMode() == AppModeBtHfPlay))) //调频偏钳位
				{
					if(Adjust > ADJUST_CLOCK_MAX * 2)
					{
						Adjust = ADJUST_CLOCK_MAX * 2;
					}
				}
				else if(Adjust > ADJUST_CLOCK_MAX)
#else
				if(Adjust > ADJUST_CLOCK_MAX)
#endif
				{
					Adjust = ADJUST_CLOCK_MAX;
				}
				else if(Adjust < -ADJUST_CLOCK_MAX)
				{
					Adjust = -ADJUST_CLOCK_MAX;
				}
				if(Adjust != AdjustValue)//分频值需调整
				{
					AdjustValue = Adjust; //保存本次频偏 分频。
					//DBG("R:%d A:%d \n", (int)AsyncRemain, (int)AdjustValue);
					if(AdjustValue > 0) //相对缺省，提高分频值
					{
						Clock_AudioPllClockAdjust(PLL_CLK_1, 0, AdjustValue);
						Clock_AudioPllClockAdjust(PLL_CLK_2, 0, AdjustValue);
					}
					else
					{
						Clock_AudioPllClockAdjust(PLL_CLK_1, 1, -AdjustValue);
						Clock_AudioPllClockAdjust(PLL_CLK_2, 1, -AdjustValue);
					}
				}
			}
			AsyncRemainLast = AsyncRemain;
			AsyncRemain = 0;//累积数据清零
			Count = 0;
			LastRise = LevelRise;//备份本次水位变化。
		}
	}
}
#endif

/**
 * @brief       AudioCore 数据源 块获取
 * @note		只有所有使能的音频流buf数据满足1帧，才会DMA搬运数据帧
*/
static bool AudioCoreSourceDateGet(void)
{
	uint32_t i;
	uint32_t Cnt = 0;
	bool IsSourceDataEnough = TRUE; 

	for(i = 0; i< AUDIO_CORE_SOURCE_MAX_MUN; i++)
	{
		if(AudioCore.AudioSource[i].Enable == FALSE)
		{
			Cnt++;//增加一个计数器，用于统计是否所有source都被禁止
			continue;
		}
		if((AudioCore.AudioSource[i].IsSreamData == FALSE)/**/
		|| (AudioCore.AudioSource[i].FuncDataGetLen == NULL))
		{
			continue;
		}
		//if(AudioCore.AudioSource[i].FuncDataGetLen() < CFG_PARA_SAMPLES_PER_FRAME)
		if(AudioCore.AudioSource[i].FuncDataGetLen() < mainAppCt.SamplesPreFrame)
		{
			IsSourceDataEnough = FALSE; 
			break;
		}
	}
	if(Cnt == AUDIO_CORE_SOURCE_MAX_MUN)//没有一个通道是能数据，退出
	{
		return FALSE;
	}
	if(!IsSourceDataEnough)
	{
		IsSourceDataEnough = TRUE;
		return FALSE;
	}

	for(i = 0; i< AUDIO_CORE_SOURCE_MAX_MUN; i++)
	{
		if(AudioCore.AudioSource[i].Enable == FALSE)
		{
			continue;
		}
		//长度必须是FRAME，不足填0
		//memset(AudioCore.AudioSource[i].PcmInBuf, 0, CFG_PARA_SAMPLES_PER_FRAME * AudioCore.AudioSource[i].PcmFormat * 2);
		memset(AudioCore.AudioSource[i].PcmInBuf, 0, mainAppCt.SamplesPreFrame * AudioCore.AudioSource[i].PcmFormat * 2);
		AudioCore.AudioSource[i].FuncDataGet(AudioCore.AudioSource[i].PcmInBuf, mainAppCt.SamplesPreFrame);
	}
	
#ifdef CFG_FUNC_FREQ_ADJUST
	AudioCoreSourceFreqAdjust();
#endif	
	return TRUE;
}

/**
 * @func        AudioCoreSinkDataSet
 * @brief       AudioCore 音效输出 推数据帧到 音频输出系统的buf
 * @param       None
 * @Output      DMA搬运数据帧到音频输出buf
 * @return      bool
 * @note		音效输出数据buf满足1帧时，dma搬运1帧数据
 * Record
*/
static bool AudioCoreSinkDataSet(void)
{
	uint32_t i;
	
	#ifdef CFG_RES_AUDIO_I2SOUT_EN
	uint32_t SendI2SDataIf=1;
	#endif
	for(i = 0; i< AUDIO_CORE_SINK_MAX_NUM; i++)
	{
		if((AudioCore.AudioSink[i].Enable == TRUE)
		&& (AudioCore.AudioSink[i].FuncDataSpaceLenSet() < mainAppCt.SamplesPreFrame))
		{

		#ifdef CFG_RES_AUDIO_I2SOUT_EN
			if(i==AUDIO_I2SOUT_SINK_NUM)
				{

				DBG("i2s dma full ,check tx pin wave!\n");
				SendI2SDataIf=0;
				continue;
				
				}
		#endif
			return FALSE;
		}
	}

	for(i = 0; i< AUDIO_CORE_SINK_MAX_NUM; i++)
	{
		#ifdef CFG_RES_AUDIO_I2SOUT_EN
		if(SendI2SDataIf==0&&i==AUDIO_I2SOUT_SINK_NUM)continue;
		#endif
	
		if((AudioCore.AudioSink[i].Enable == TRUE) && (AudioCore.AudioSink[i].FuncDataSet != NULL))
		{
			AudioCore.AudioSink[i].FuncDataSet(AudioCore.AudioSink[i].PcmOutBuf, mainAppCt.SamplesPreFrame);
		}
	}
	return TRUE;
}

void AudioCoreSourcePcmFormatConfig(uint8_t Index, uint16_t Format)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_MUN)
	{
		AudioCore.AudioSource[Index].PcmFormat = Format;
	}
}

void AudioCoreSourceEnable(uint8_t Index)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_MUN)
	{
		AudioCore.AudioSource[Index].Enable = TRUE;
	}
}

void AudioCoreSourceDisable(uint8_t Index)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_MUN)
	{
		AudioCore.AudioSource[Index].Enable = FALSE;
	}
}

void AudioCoreSourceMute(uint8_t Index, bool IsLeftMute, bool IsRightMute)
{
	if(IsLeftMute)
	{
		AudioCore.AudioSource[Index].LeftMuteFlag = TRUE;
	}
	if(IsRightMute)
	{
		AudioCore.AudioSource[Index].RightMuteFlag = TRUE;
	}
}

void AudioCoreSourceUnmute(uint8_t Index, bool IsLeftUnmute, bool IsRightUnmute)
{
	if(IsLeftUnmute)
	{
		AudioCore.AudioSource[Index].LeftMuteFlag = FALSE;
	}
	if(IsRightUnmute)
	{
		AudioCore.AudioSource[Index].RightMuteFlag = FALSE;
	}
}

void AudioCoreSourceVolSet(uint8_t Index, uint16_t LeftVol, uint16_t RightVol)
{
	AudioCore.AudioSource[Index].LeftVol = LeftVol;
	AudioCore.AudioSource[Index].RightVol = RightVol;
}

void AudioCoreSourceVolGet(uint8_t Index, uint16_t* LeftVol, uint16_t* RightVol)
{
	*LeftVol = AudioCore.AudioSource[Index].LeftCurVol;
	*RightVol = AudioCore.AudioSource[Index].RightCurVol;
}

void AudioCoreSourceConfig(uint8_t Index, AudioCoreSource* Source)
{
	memcpy(&AudioCore.AudioSource[Index], Source, sizeof(AudioCoreSource));
}

void AudioCoreSinkEnable(uint8_t Index)
{
	AudioCore.AudioSink[Index].Enable = TRUE;
}

void AudioCoreSinkDisable(uint8_t Index)
{
	AudioCore.AudioSink[Index].Enable = FALSE;
}

void AudioCoreSinkMute(uint8_t Index, bool IsLeftMute, bool IsRightMute)
{
	if(IsLeftMute)
	{
		AudioCore.AudioSink[Index].LeftMuteFlag = TRUE;
	}
	if(IsRightMute)
	{
		AudioCore.AudioSink[Index].RightMuteFlag = TRUE;
	}
}

void AudioCoreSinkUnmute(uint8_t Index, bool IsLeftUnmute, bool IsRightUnmute)
{
	if(IsLeftUnmute)
	{
		AudioCore.AudioSink[Index].LeftMuteFlag = FALSE;
	}
	if(IsRightUnmute)
	{
		AudioCore.AudioSink[Index].RightMuteFlag = FALSE;
	}
}

void AudioCoreSinkVolSet(uint8_t Index, uint16_t LeftVol, uint16_t RightVol)
{
	AudioCore.AudioSink[Index].LeftVol = LeftVol;
	AudioCore.AudioSink[Index].RightVol = RightVol;
}

void AudioCoreSinkVolGet(uint8_t Index, uint16_t* LeftVol, uint16_t* RightVol)
{
	*LeftVol = AudioCore.AudioSink[Index].LeftCurVol;
	*RightVol = AudioCore.AudioSink[Index].RightCurVol;
}

void AudioCoreSinkConfig(uint8_t Index, AudioCoreSink* Sink)
{
	memcpy(&AudioCore.AudioSink[Index], Sink, sizeof(AudioCoreSink));
}


void AudioCoreProcessConfig(AudioCoreProcessFunc AudioEffectProcess)
{
	AudioCore.AudioEffectProcess = AudioEffectProcess;
}

/**
 * @func        AudioCoreConfig
 * @brief       AudioCore参数块，本地化API
 * @param       AudioCoreContext *AudioCoreCt
 * @Output      None
 * @return      None
 * @Others      外部配置的参数块，复制一份到本地
 */
void AudioCoreConfig(AudioCoreContext *AudioCoreCt)
{
	memcpy(&AudioCore, AudioCoreCt, sizeof(AudioCoreContext));
}

bool AudioCoreInit(void)
{
	return TRUE;
}

void AudioCoreDeinit(void)
{
	AudioState = AC_RUN_GET;
}

/**
 * @func        AudioCoreRun
 * @brief       音源拉流->音效处理+混音->推流
 * @param       None
 * @Output      None
 * @return      None
 * @Others      当前由audioCoreservice任务保障此功能有效持续。
 * Record
 */
extern uint32_t 	IsAudioCorePause;
extern uint32_t 	IsAudioCorePauseMsgSend;
void AudioProcessMain(void);
void AudioCoreRun(void)
{
	bool ret;
	switch(AudioState)
	{
		case AC_RUN_CHECK:
			if(IsAudioCorePause == TRUE)
			{
				if(IsAudioCorePauseMsgSend == TRUE)
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_AUDIO_CORE_HOLD;
					MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);

					IsAudioCorePauseMsgSend = FALSE;
				}
				return;
			}
		case AC_RUN_GET:
			ret = AudioCoreSourceDateGet();
			if(ret == FALSE)
			{
				return;
			}
		case AC_RUN_PROC:
			//AudioCore.AudioProcess();
			AudioProcessMain();
			AudioState = AC_RUN_PUT;

		case AC_RUN_PUT:
			ret = AudioCoreSinkDataSet();
			if(ret == FALSE)
			{
				return;
			}
			//AudioState = AC_RUN_GET;
			AudioState = AC_RUN_CHECK;
			break;
		default:
			break;
	}
}

//音效处理函数，主入口
//将mic通路数据剥离出来统一处理
//mic通路数据和具体模式无关
//提示音通路无音效，剥离后在sink端混音。
void AudioProcessMain(void)
{	
	AduioCoreSourceVolSet();

#ifdef CFG_FUNC_RECORDER_EN
	if(AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].PcmFormat == 1)
		{
			uint16_t i;
			for(i = mainAppCt.SamplesPreFrame * 2 - 1; i > 0; i--)
			{
				AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf[i / 2];
			}
		}
	}
#endif

	if(AudioCore.AudioSource[APP_SOURCE_NUM].Enable == TRUE)////music buff
	{
		#if (BT_HFP_SUPPORT == ENABLE) && defined(CFG_APP_BT_MODE_EN)
		if(GetSystemMode() != AppModeBtHfPlay)
		#endif
		{
			if(AudioCore.AudioSource[APP_SOURCE_NUM].PcmFormat == 1)
			{
				uint16_t i;
				for(i = mainAppCt.SamplesPreFrame * 2 - 1; i > 0; i--)
				{
					AudioCore.AudioSource[APP_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[APP_SOURCE_NUM].PcmInBuf[i / 2];
				}
			}
		}
	}	
		
#if defined(CFG_FUNC_REMIND_SOUND_EN)	
	if(AudioCore.AudioSource[REMIND_SOURCE_NUM].Enable == TRUE)////remind buff
	{
		if(AudioCore.AudioSource[REMIND_SOURCE_NUM].PcmFormat == 1)
		{
			uint16_t i;
			for(i = mainAppCt.SamplesPreFrame * 2 - 1; i > 0; i--)
			{
				AudioCore.AudioSource[REMIND_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[REMIND_SOURCE_NUM].PcmInBuf[i / 2];
			}
		}
	}	
#endif

	if(AudioCore.AudioEffectProcess != NULL)
	{
		AudioCore.AudioEffectProcess((AudioCoreContext*)&AudioCore);
	}
	//else
	//{
		//return;
	//}
	
    #ifdef CFG_FUNC_BEEP_EN
    if(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].Enable == TRUE)   ////dacx buff
	{
		Beep(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf, mainAppCt.SamplesPreFrame);
	}
    #endif

	AduioCoreSinkVolSet();//调音量
}

//注意:
//帧长度大于192 sample

//音量淡入淡出					
#define MixerFadeVolume(a, b, c)  	\
    if(a > b + c)		    \
	{						\
		a -= c;				\
	}						\
	else if(a + c < b)	   	\
	{						\
		a += c;				\
	}						\
	else					\
	{						\
		a = b;				\
	}

void AduioCoreSourceVolSet(void)
{
	uint32_t i, j;
    
	uint16_t LeftVol, RightVol, VolStep = 4096 / mainAppCt.SamplesPreFrame + 1;

	if(GetSystemMode() == AppModeBtHfPlay)
	{
		AudioCore.AudioSource[0].PreGain = BT_HFP_MIC_DIGIT_GAIN;
		AudioCore.AudioSource[1].PreGain = BT_HFP_INPUT_DIGIT_GAIN;
	}
	else
	{
		AudioCore.AudioSource[0].PreGain = 4095;//0db
		AudioCore.AudioSource[1].PreGain = 4095;
	}
	
	for(j=0; j<AUDIO_CORE_SOURCE_MAX_MUN; j++)
	{
		if(!AudioCore.AudioSource[j].Enable)
		{
			continue;
		}
		if(AudioCore.AudioSource[j].LeftMuteFlag == TRUE)
		{
			LeftVol = 0;
		}
		else
		{
			LeftVol = AudioCore.AudioSource[j].LeftVol;
		}
		if(AudioCore.AudioSource[j].RightMuteFlag == TRUE)
		{
			RightVol = 0;
		}
		else
		{
			RightVol = AudioCore.AudioSource[j].RightVol;
		}
		if(AudioCore.AudioSource[j].PcmFormat == 2)//立体声
		{
			for(i=0; i<mainAppCt.SamplesPreFrame; i++)
			{
				AudioCore.AudioSource[j].PcmInBuf[2 * i + 0] = __nds32__clips((((((int32_t)AudioCore.AudioSource[j].PcmInBuf[2 * i + 0]) * AudioCore.AudioSource[j].LeftCurVol) >> 12) * AudioCore.AudioSource[j].PreGain) >> 12, (16)-1);//__SSAT((((((int32_t)AudioCore.AudioSource[j].PcmInBuf[2 * i] * AudioCore.AudioSource[j].LeftCurVol) >> 12) * AudioCore.AudioSource[j].PreGain) >> 12), 16);
				AudioCore.AudioSource[j].PcmInBuf[2 * i + 1] = __nds32__clips((((((int32_t)AudioCore.AudioSource[j].PcmInBuf[2 * i + 1]) * AudioCore.AudioSource[j].RightCurVol) >> 12) * AudioCore.AudioSource[j].PreGain) >> 12, (16)-1);//__SSAT((((((int32_t)AudioCore.AudioSource[j].PcmInBuf[2 * i + 1] * AudioCore.AudioSource[j].RightCurVol) >> 12) * AudioCore.AudioSource[j].PreGain) >> 12), 16);
				
				MixerFadeVolume(AudioCore.AudioSource[j].LeftCurVol, LeftVol, VolStep);
				MixerFadeVolume(AudioCore.AudioSource[j].RightCurVol, RightVol, VolStep);
			}
		}
		else
		{
			for(i=0; i<mainAppCt.SamplesPreFrame; i++)
			{
				AudioCore.AudioSource[j].PcmInBuf[i] = __nds32__clips((((((int32_t)AudioCore.AudioSource[j].PcmInBuf[i]) * AudioCore.AudioSource[j].LeftCurVol) >> 12) * AudioCore.AudioSource[j].PreGain) >> 12, (16)-1);//__SSAT((((((int32_t)AudioCore.AudioSource[j].PcmInBuf[i] * AudioCore.AudioSource[j].LeftCurVol) >> 12) * AudioCore.AudioSource[j].PreGain) >> 12), 16);
				MixerFadeVolume(AudioCore.AudioSource[j].LeftCurVol, LeftVol, VolStep);
			}
		}
	}
}


void AduioCoreSinkVolSet(void)
{
	uint32_t i;

	uint16_t VolStep = 4096 / mainAppCt.SamplesPreFrame + 1;

	if(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].LeftMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].LeftVol = 0;
		}
		if(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].RightMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].RightVol = 0;
		}

		for(i=0; i<mainAppCt.SamplesPreFrame; i++)
		{
			AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf[2 * i + 0] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf[2 * i + 0]) * AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].LeftCurVol) >> 12, (16)-1);//__SSAT((((int32_t)AudioCore.AudioSink[AUDIO_SINK_NUM].PcmOutBuf[2 * i] * AudioCore.AudioSink[AUDIO_SINK_NUM].LeftCurVol) >> 12), 16);
			AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf[2 * i + 1] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf[2 * i + 1]) * AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].RightCurVol) >> 12, (16)-1);//__SSAT((((int32_t)AudioCore.AudioSink[AUDIO_SINK_NUM].PcmOutBuf[2 * i + 1] * AudioCore.AudioSink[AUDIO_SINK_NUM].RightCurVol) >> 12), 16);

			MixerFadeVolume(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].LeftCurVol, AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].LeftVol, VolStep);
			MixerFadeVolume(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].RightCurVol, AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].RightVol, VolStep);
		}
	}
	if(AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].LeftMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].LeftVol = 0;
		}
		if(AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].RightMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].RightVol = 0;
		}

		for(i=0; i<mainAppCt.SamplesPreFrame; i++)
		{
			AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].PcmOutBuf[2 * i + 0] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].PcmOutBuf[2 * i + 0]) * AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].LeftCurVol) >> 12, (16)-1);//
			AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].PcmOutBuf[2 * i + 1] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].PcmOutBuf[2 * i + 1]) * AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].RightCurVol) >> 12, (16)-1);//

			MixerFadeVolume(AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].LeftCurVol, AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].LeftVol, VolStep);
			MixerFadeVolume(AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].RightCurVol, AudioCore.AudioSink[AUDIO_RECORDER_SINK_NUM].RightVol, VolStep);
		}
	}

#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT == ENABLE))
	//sink2: hfp sco send
	if(AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].LeftMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].LeftVol = 0;
		}
		if(AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].RightMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].RightVol = 0;
		}

		for(i=0; i<mainAppCt.SamplesPreFrame; i++)
		{
			AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].PcmOutBuf[i] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].PcmOutBuf[i]) * AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].LeftCurVol) >> 12, (16)-1);

			MixerFadeVolume(AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].LeftCurVol, AudioCore.AudioSink[AUDIO_HF_SCO_SINK_NUM].LeftVol, VolStep);
		}
	}
#endif

#ifdef CFG_RES_AUDIO_DACX_EN 	
	if(AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].LeftMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].LeftVol = 0;
		}
		if(AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].RightMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].RightVol = 0;
		}

		for(i=0; i<mainAppCt.SamplesPreFrame; i++)
		{
			AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].PcmOutBuf[i] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].PcmOutBuf[i]) * AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].LeftCurVol) >> 12, (16)-1);//__SSAT((((int32_t)AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].PcmOutBuf[i] * AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].LeftCurVol) >> 12), 16);

			MixerFadeVolume(AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].LeftCurVol, AudioCore.AudioSink[AUDIO_DACX_SINK_NUM].LeftVol, VolStep);
		}
	}
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN
	if(AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].LeftMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].LeftVol = 0;
		}
		if(AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].RightMuteFlag == TRUE)
		{
			AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].RightVol = 0;
		}

		for(i=0; i<mainAppCt.SamplesPreFrame; i++)
		{		
			AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf[2 * i + 0] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf[2 * i + 0]) * AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].LeftCurVol) >> 12, (16)-1);//__SSAT((((int32_t)AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf[2 * i] * AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].LeftCurVol) >> 12), 16);
			AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf[2 * i + 1] = __nds32__clips((((int32_t)AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf[2 * i + 1]) * AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].RightCurVol) >> 12, (16)-1);//__SSAT((((int32_t)AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf[2 * i + 1] * AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].RightCurVol) >> 12), 16);
		
			MixerFadeVolume(AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].LeftCurVol, AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].LeftVol, VolStep);
			MixerFadeVolume(AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].RightCurVol, AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].RightVol, VolStep);
		}
	}
#endif
}

