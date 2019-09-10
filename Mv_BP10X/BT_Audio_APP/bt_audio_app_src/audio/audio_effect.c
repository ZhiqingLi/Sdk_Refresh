#include <string.h>
#include <nds32_intrinsic.h>
#include "debug.h"
#include "app_config.h"
#include "ctrlvars.h"
#include "watchdog.h"
#include "rtos_api.h"
#include "audio_effect.h"
#include "audio_effect_library.h"
#include "communication.h"
#include "audio_adc.h"
#include "main_task.h"

#include "bt_config.h"

#ifdef CFG_FUNC_AI
#include "ai.h"
#endif



#ifdef CFG_FUNC_AUDIO_EFFECT_EN
extern bool GetWhetherRecMusic(void);
extern uint8_t DecoderTaskState;

uint32_t           DynamicStartAddr;// = (uint32_t)CFG_AUDIO_EFFECT_START_ADDR;
uint32_t           DynamicEndAddr;//	= (uint32_t)CFG_AUDIO_EFFECT_END_ADDR;
uint32_t           DynamicStartAddrBakcup;

AUDIO_EFF_PARAMAS Audio_mode ;//调音参数缓存

//int32_t pcm_buf_1[CFG_PARA_SAMPLES_PER_FRAME];
//int32_t pcm_buf_2[CFG_PARA_SAMPLES_PER_FRAME];
//int32_t pcm_buf_3[CFG_PARA_SAMPLES_PER_FRAME];
//int32_t pcm_buf_4[CFG_PARA_SAMPLES_PER_FRAME];

int32_t pcm_buf_1[512];
int32_t pcm_buf_2[512];
int32_t pcm_buf_3[512];
int32_t pcm_buf_4[512];


void du_efft_fadein_sw(int16_t* pcm_in, uint16_t pcm_length, uint16_t ch)
{
	int16_t* temp = (int16_t *)pcm_in;
	int32_t n = 0, w = 0, step = 32768/pcm_length;

	if(ch == 2)
	{
		for(n = 0;	n < pcm_length; n++)
		{
			temp[n * 2] = (int16_t)(((int32_t)temp[n * 2] * w + 16384) >> 15);
			temp[n * 2 + 1] = (int16_t)(((int32_t)temp[n * 2 + 1] * w + 16384) >> 15);
			w += step;
		}
	}
	else if(ch == 1)
	{
		for(n = 0;	n < pcm_length; n++)
		{
			temp[n] = (int16_t)(((int32_t)temp[n] * w + 16384) >> 15);
			w += step;
		}
	}
}


void du_efft_fadeout_sw(int16_t* pcm_in, uint16_t pcm_length, uint16_t ch)
{
	int16_t* temp = (int16_t *)pcm_in;
	int32_t n = 0, w = (32768/pcm_length)*(pcm_length-1), step = 32768/pcm_length;

	if(ch == 2)
	{
		for(n = 0; n < pcm_length; n++)
		{
			temp[n * 2] = (int16_t)(((int32_t)temp[n * 2] * w + 16384) >> 15);
			temp[n * 2 + 1] = (int16_t)(((int32_t)temp[n * 2 + 1] * w + 16384) >> 15);
			w -= step;
		}
	}
	else if(ch == 1)
	{
		for(n = 0; n < pcm_length; n++)
		{
			temp[n] = (int16_t)(((int32_t)temp[n] * w + 16384) >> 15);
			w -= step;
		}
	}
}

void LoadAudioMode(uint16_t len,const uint8_t *buff, uint8_t init_flag);

/*
****************************************************************
* 分析音效模式最大内存使用率
*
*
****************************************************************
*/
uint32_t GetAudioParamasMaxRam (void)
{
	//uint8_t i = 0;
	uint32_t MaxUseRam = 0;
	//uint32_t MinUseRam = 0;

	DBG("GetAudioParamasMaxRam....\n");

	MaxUseRam = PLATE_REVERB_SIZE + ECHO_SIZE + ECHO_S_SIZE + EQ_SIZE*10;

	return MaxUseRam;
}
/*
****************************************************************
* 音效模式选择函数
* 1.共预留10组调音参数，可由调音工具导出或下载；
* 2.每组调音参数对应1个音效模式；
****************************************************************
*/
void LoadAudioParamas (uint8_t mode)
{
	uint8_t i = 0;

	DBG("[KEY]: FUNC_ID_EFFECT_MODE -> %d\n", mode);

	Audio_mode.EffectParamas = 0;
	Audio_mode.len           = 0;
	Audio_mode.eff_mode      = 0;
   
	if(mode < 10)//从下载的10组参数对应的flash存储空间中获取
	{
		DBG("[SYS]: Get Audio Effect Paramas From SPI FLASH!!!!!!\n");
		//ReadEffectParamas(mode,&Audio_mode_buff[0]);
		return;
	}
	else//从调音数组中获取（由调音工具导出)
	{
		DBG("[SYS]: Get Audio Effect Paramas From Data.c!!!!!!\n");

		i = 0;

		while(1)
		{
			if(EFFECT_TAB[i].eff_mode == 0xff)   
			{
				DBG("No Audio effect File load!!!!\n");
				return;
			} 	  

			if(EFFECT_TAB[i].eff_mode == mode)   
			{
				Audio_mode.eff_mode      = EFFECT_TAB[i].eff_mode;
				Audio_mode.EffectParamas = EFFECT_TAB[i].EffectParamas;
	            Audio_mode.len           = EFFECT_TAB[i].len;
				DBG("number:%d , eff_mode:%d , len:%d name:%s\n",i,EFFECT_TAB[i].eff_mode,EFFECT_TAB[i].len,EFFECT_TAB[i].name);
	            return;
			}
			else
			{
			   i++;
			}
		}
	}
}
/*
****************************************************************
* 音效模式选择函数
* 1.共预留10组调音参数，可由调音工具导出或下载；
* 2.每组调音参数对应1个音效模式；
****************************************************************
*/
void AudioEffectModeSel(uint8_t mode, uint8_t init_flag)//0=hw,1=effect,2=hw+effect ff= no init
{
	WDG_Feed();

	LoadAudioParamas(mode);

	if(init_flag == 0xff)///no init
	{
		return;
	}

	if(Audio_mode.EffectParamas == 0)
	{
		return;
	}

	if(init_flag == 0)///only hardware init
	{
		LoadAudioMode(Audio_mode.len,Audio_mode.EffectParamas , init_flag);//0=hw,1=effect,2=hw+effect
		return;
	}

	if(!AudioEffectListJudge(Audio_mode.len,Audio_mode.EffectParamas))
	{
		return;
	}

	AudioEffectsAllDisable();

	LoadAudioMode(Audio_mode.len,Audio_mode.EffectParamas , init_flag);//0=hw,1=effect,2=hw+effect
//	{
//		void EffectChange(void);
//		EffectChange();
//	}
}
/*
****************************************************************
* 关闭所有音效功能
*
*
****************************************************************
*/
void AudioEffectsAllDisable(void)
{
	uint16_t i;
    
	DBG("AudioEffectsAllDisable \n");
	gCtrlVars.auto_tune_unit.enable 	  = 0;
	gCtrlVars.echo_unit.enable			  = 0;
	gCtrlVars.pitch_shifter_unit.enable   = 0;
	#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN
	gCtrlVars.pitch_shifter_pro_unit.enable = 0;
	#endif
	gCtrlVars.voice_changer_unit.enable   = 0;
	#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
	gCtrlVars.voice_changer_pro_unit.enable = 0;
	#endif
	gCtrlVars.reverb_unit.enable		  = 0;
	gCtrlVars.plate_reverb_unit.enable	  = 0;
	#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
	gCtrlVars.reverb_pro_unit.enable      = 0;
	#endif
	gCtrlVars.freq_shifter_unit.enable	  = 0;
	gCtrlVars.howling_dector_unit.enable  = 0;
	#if CFG_AUDIO_EFFECT_MIC_AEC_EN
	gCtrlVars.mic_aec_unit.enable         = 0;
	#endif
	gCtrlVars.MicAudioSdct_unit.enable    = 0;
	gCtrlVars.MusicAudioSdct_unit.enable  = 0;
	#if CFG_AUDIO_EFFECT_MUSIC_3D_EN
	gCtrlVars.music_threed_unit.enable    = 0;
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
	gCtrlVars.music_vb_unit.enable        = 0;
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
	gCtrlVars.music_vb_classic_unit.enable = 0;
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
	gCtrlVars.music_delay_unit.enable     = 0;
	#endif
	gCtrlVars.music_exciter_unit.enable   = 0;	
	gCtrlVars.vocal_cut_unit.enable       = 0;
	gCtrlVars.vocal_remove_unit.enable    = 0;
	gCtrlVars.chorus_unit.enable          = 0;
	gCtrlVars.phase_control_unit.enable   = 0;

	//expand
	for(i = 0; i < sizeof(expander_unit_aggregate)/sizeof(expander_unit_aggregate[0]); i++)
	{
		expander_unit_aggregate[i]->enable = 0;
	}
    //drc
	for(i = 0; i < sizeof(drc_unit_aggregate)/sizeof(drc_unit_aggregate[0]); i++)
	{
		drc_unit_aggregate[i]->enable		= 0;
	}
	//eq
	for(i = 0; i < sizeof(eq_unit_aggregate)/sizeof(eq_unit_aggregate[0]); i++)
	{
		eq_unit_aggregate[i]->enable		= 0;
	}
    //gain control
	for(i = 0; i < sizeof(gain_unit_aggregate)/sizeof(gain_unit_aggregate[0]); i++)
	{
		gain_unit_aggregate[i]->enable		= 0;
	}
	//eq
	for(i = 0; i < sizeof(eq_unit_aggregate)/sizeof(eq_unit_aggregate[0]); i++)
	{
		eq_unit_aggregate[i]->enable 	= 0;
	}

}

/*
****************************************************************
* 音效模块反初始化
*
*
****************************************************************
*/
void AudioEffectsDeInit(void)
{
	uint8_t i;
#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexLock(AudioEffectMutex);
	}
#endif
    DBG("AudioEffectsDeInit \n");
    #ifdef CFG_USE_OS_MALLOC
	if(gCtrlVars.freq_shifter_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.freq_shifter_unit.ct);
	}
	if(gCtrlVars.howling_dector_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.howling_dector_unit.ct);
	}
	if(gCtrlVars.pitch_shifter_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.pitch_shifter_unit.ct);
	}
	#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN
	if(gCtrlVars.pitch_shifter_pro_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.pitch_shifter_pro_unit.ct);
	}
	#endif
	if(gCtrlVars.auto_tune_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.auto_tune_unit.ct);
	}
	if(gCtrlVars.voice_changer_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.voice_changer_unit.ct);
	}
	#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
	if(gCtrlVars.voice_changer_pro_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.voice_changer_pro_unit.ct);
	}
	#endif
	if(gCtrlVars.echo_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.echo_unit.ct);
	}
	if(gCtrlVars.reverb_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.reverb_unit.ct);
	}
	if(gCtrlVars.plate_reverb_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.plate_reverb_unit.ct);
	}
	#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
	if(gCtrlVars.reverb_pro_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.reverb_pro_unit.ct);
	}
	#endif
	#if CFG_AUDIO_EFFECT_MIC_AEC_EN
	if(gCtrlVars.mic_aec_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.mic_aec_unit.ct);
	}
	#endif
	if(gCtrlVars.MicAudioSdct_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.MicAudioSdct_unit.ct);
	}
	if(gCtrlVars.MusicAudioSdct_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.MusicAudioSdct_unit.ct);
	}
	if(gCtrlVars.vocal_cut_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.vocal_cut_unit.ct);
	}
	if(gCtrlVars.vocal_remove_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.vocal_remove_unit.ct);
	}
	#if CFG_AUDIO_EFFECT_MUSIC_3D_EN
	if(gCtrlVars.music_threed_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.music_threed_unit.ct);
	}
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
	if(gCtrlVars.music_vb_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.music_vb_unit.ct);
	}
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
	if(gCtrlVars.music_vb_classic_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.music_vb_classic_unit.ct);
	}
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
	if(gCtrlVars.music_delay_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.music_delay_unit.ct);
	}
	#endif
	if(gCtrlVars.music_exciter_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.music_exciter_unit.ct);
	}
	if(gCtrlVars.chorus_unit.ct != NULL)
	{
		osPortFree(gCtrlVars.chorus_unit.ct);
	}
    //expand
	for(i = 0; i < sizeof(expander_unit_aggregate)/sizeof(expander_unit_aggregate[0]); i++)
	{
		if(expander_unit_aggregate[i]->ct != NULL)
		{
			osPortFree(expander_unit_aggregate[i]->ct);
		}
	}
    //drc
    for(i = 0; i < sizeof(drc_unit_aggregate)/sizeof(drc_unit_aggregate[0]); i++)
	{
		if(drc_unit_aggregate[i]->ct != NULL)
		{
    		osPortFree(drc_unit_aggregate[i]->ct);
		}
	}
	 //eq
    for(i = 0; i < sizeof(eq_unit_aggregate)/sizeof(eq_unit_aggregate[0]); i++)
	{
		if(eq_unit_aggregate[i]->ct != NULL)
		{
    		osPortFree(eq_unit_aggregate[i]->ct);
		}
	}

    #else
    DynamicStartAddr = DynamicStartAddrBakcup;
    #endif

	gCtrlVars.freq_shifter_unit.ct   =   NULL;
	gCtrlVars.howling_dector_unit.ct =   NULL;
	gCtrlVars.pitch_shifter_unit.ct  =   NULL;
	#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN
	gCtrlVars.pitch_shifter_pro_unit.ct  =   NULL;
	#endif
	gCtrlVars.auto_tune_unit.ct		 =   NULL;
	gCtrlVars.voice_changer_unit.ct  =   NULL;
	#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
	gCtrlVars.voice_changer_pro_unit.ct  =   NULL;
	#endif
	gCtrlVars.echo_unit.ct			 =   NULL;
	gCtrlVars.echo_unit.s_buf		 =   NULL;
	gCtrlVars.reverb_unit.ct		 =   NULL;
	gCtrlVars.plate_reverb_unit.ct	 =   NULL;
	#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
	gCtrlVars.reverb_pro_unit.ct     =   NULL;
	#endif
	#if CFG_AUDIO_EFFECT_MIC_AEC_EN
	gCtrlVars.mic_aec_unit.ct        =   NULL;
	#endif
	gCtrlVars.MicAudioSdct_unit.ct   =   NULL;
	gCtrlVars.MusicAudioSdct_unit.ct =   NULL;
	gCtrlVars.vocal_cut_unit.ct	     =   NULL;
	gCtrlVars.vocal_remove_unit.ct	 =   NULL;
	#if CFG_AUDIO_EFFECT_MUSIC_3D_EN
	gCtrlVars.music_threed_unit.ct   =   NULL;
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
	gCtrlVars.music_vb_unit.ct       =   NULL;
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
	gCtrlVars.music_vb_classic_unit.ct = NULL;
	#endif
	#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
	gCtrlVars.music_delay_unit.ct	 =   NULL;
	#endif
	gCtrlVars.music_exciter_unit.ct	 =   NULL;
	gCtrlVars.chorus_unit.ct         =   NULL;
	
    //expand
	for(i = 0; i < sizeof(expander_unit_aggregate)/sizeof(expander_unit_aggregate[0]); i++)
	{
		expander_unit_aggregate[i]->ct		= NULL;
	}
    //drc
    for(i = 0; i < sizeof(drc_unit_aggregate)/sizeof(drc_unit_aggregate[0]); i++)
	{
		drc_unit_aggregate[i]->ct		    = NULL;
	}
	 //eq
    for(i = 0; i < sizeof(eq_unit_aggregate)/sizeof(eq_unit_aggregate[0]); i++)
	{
		eq_unit_aggregate[i]->ct		    = NULL;
	}

#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexUnlock(AudioEffectMutex);
	}
#endif
}

/*
****************************************************************
* 音效模块初始化
*
*
****************************************************************
*/
void AudioEffectsInit(void)
{
#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexLock(AudioEffectMutex);
	}
#endif
    #ifndef CFG_USE_OS_MALLOC
	if(DynamicStartAddrBakcup == 0)///malloc fail
	{
		DBG("Audio malloc fail\n");
		AudioEffectsAllDisable();
		return;
	}
	DynamicStartAddr = DynamicStartAddrBakcup;
    #endif
	
	#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
	AudioEffectReverbProInit(&gCtrlVars.reverb_pro_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif
	
	#if CFG_AUDIO_EFFECT_MUSIC_NOISE_SUPPRESSOR_EN
	AudioEffectExpanderInit(&gCtrlVars.music_expander_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

    #if CFG_AUDIO_EFFECT_MUSIC_SILENCE_DECTOR_EN
	gCtrlVars.MusicAudioSdct_unit.enable = 1;
	AudioEffectSilenceDectorInit(&gCtrlVars.MusicAudioSdct_unit,gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
    #endif

	#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
	AudioEffectPcmDelayInit(&gCtrlVars.music_delay_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MUSIC_EXCITER_EN
	AudioEffectExciterInit(&gCtrlVars.music_exciter_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN
	AudioEffectPitchShifterProInit(&gCtrlVars.pitch_shifter_pro_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif
	
	#if CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
	AudioEffectExpanderInit(&gCtrlVars.mic_expander_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

	#if 0//CFG_AUDIO_EFFECT_MIC_AEC_EN
	gCtrlVars.mic_aec_unit.enable = 1;
	//AudioEffectAecInit(&gCtrlVars.mic_aec_unit, 1, gCtrlVars.sample_rate);
	AudioEffectAecInit(&gCtrlVars.mic_aec_unit, 1, 16000);//固定为16K采样率
	#endif
	
    #if CFG_AUDIO_EFFECT_MIC_FREQ_SHIFTER_EN
	AudioEffectFreqShifterInit(&gCtrlVars.freq_shifter_unit);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_HOWLING_DECTOR_EN
	AudioEffectHowlingSuppressorInit(&gCtrlVars.howling_dector_unit);
	#endif
	
    #if CFG_AUDIO_EFFECT_MIC_SILENCE_DECTOR_EN
	gCtrlVars.MicAudioSdct_unit.enable = 1;
	AudioEffectSilenceDectorInit(&gCtrlVars.MicAudioSdct_unit,gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
    #endif

	#if CFG_AUDIO_EFFECT_MIC_PITCH_SHIFTER_EN
	AudioEffectPitchShifterInit(&gCtrlVars.pitch_shifter_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_AUTO_TUNE_EN
	AudioEffectAutoTuneInit(&gCtrlVars.auto_tune_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_EN
	AudioEffectVoiceChangerInit(&gCtrlVars.voice_changer_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

    #if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
	AudioEffectVoiceChangerProInit(&gCtrlVars.voice_changer_pro_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif
	
	#if CFG_AUDIO_EFFECT_MIC_ECHO_EN
	AudioEffectEchoInit(&gCtrlVars.echo_unit,  gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_REVERB_EN
	AudioEffectReverbInit(&gCtrlVars.reverb_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

    #if CFG_AUDIO_EFFECT_MIC_PLATE_REVERB_EN
	AudioEffectPlateReverbInit(&gCtrlVars.plate_reverb_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif	
	
	#if CFG_AUDIO_EFFECT_VOCAL_CUT_EN
	AudioEffectVocalCutInit(&gCtrlVars.vocal_cut_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_VOCAL_REMOVE_EN
    AudioEffectVocalRemoveInit(&gCtrlVars.vocal_remove_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
    #endif

	#if CFG_AUDIO_EFFECT_CHORUS_EN
    AudioEffectChorusInit(&gCtrlVars.chorus_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
    #endif
	
	#if CFG_AUDIO_EFFECT_MUSIC_3D_EN
	AudioEffectThreeDInit(&gCtrlVars.music_threed_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
	AudioEffectVBInit(&gCtrlVars.music_vb_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
	AudioEffectVBClassicInit(&gCtrlVars.music_vb_classic_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif
	
	#if CFG_AUDIO_EFFECT_MUSIC_DRC_EN
	AudioEffectDRCInit(&gCtrlVars.music_drc_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

    #if CFG_AUDIO_EFFECT_MIC_DRC_EN
	AudioEffectDRCInit(&gCtrlVars.mic_drc_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif
	
	#if CFG_AUDIO_EFFECT_REC_DRC_EN
	AudioEffectDRCInit(&gCtrlVars.rec_drc_unit, 1, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MUSIC_PRE_EQ_EN
	AudioEffectEQInit(&gCtrlVars.music_pre_eq_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MUSIC_OUT_EQ_EN
	AudioEffectEQInit(&gCtrlVars.music_out_eq_unit, gCtrlVars.adc_line_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_BYPASS_EQ_EN
	AudioEffectEQInit(&gCtrlVars.mic_bypass_eq_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_ECHO_EQ_EN
	AudioEffectEQInit(&gCtrlVars.mic_echo_eq_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_REVERB_EQ_EN
	AudioEffectEQInit(&gCtrlVars.mic_reverb_eq_unit, gCtrlVars.adc_mic_channel_num, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_MIC_OUT_EQ_EN
	AudioEffectEQInit(&gCtrlVars.mic_out_eq_unit, 2, gCtrlVars.sample_rate);
	#endif

	#if CFG_AUDIO_EFFECT_REC_EQ_EN
	AudioEffectEQInit(&gCtrlVars.rec_eq_unit, 1, gCtrlVars.sample_rate);
	#endif

#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexUnlock(AudioEffectMutex);
	}
#endif
}

#ifdef CFG_FUNC_MIC_KARAOKE_EN
/*
****************************************************************
* Music+Mic音效处理主函数
*
*
****************************************************************
*/
void AudioEffectProcess(AudioCoreContext *pAudioCore)
{

	int16_t  s;
	int16_t  pcm;
	uint16_t n = mainAppCt.SamplesPreFrame;
	int16_t *mic_pcm    	= NULL;//pBuf->mic_in;///mic input	
	int16_t *bypass_tmp 	= NULL;//pBuf->mic_in;
	int16_t *music_pcm    	= NULL;//pBuf->music_in;///music input
	int16_t *remind_in      = NULL;//pBuf->remind_in;
	int16_t *monitor_out    = NULL;//pBuf->dac0_out; 
	int16_t *record_out     = NULL;//pBuf->dacx_out; 
	int16_t *i2s0_out       = NULL;//pBuf->i2s0_out; 
	int16_t *i2s1_out       = NULL;//pBuf->i2s1_out; 
	int16_t *usb_out        = NULL;//pBuf->usb_out; 
	int16_t *local_rec_out  = NULL;//pBuf->rec_out; 
	#if (BT_HFP_SUPPORT == ENABLE)
	int16_t *hf_mic_in      = NULL;//pBuf->hf_mic_in;//蓝牙通话mic采样buffer
	int16_t *hf_pcm_in      = NULL;//pBuf->hf_pcm_in;//蓝牙通话下传buffer
	int16_t *hf_aec_in		= NULL;//pBuf->hf_aec_in;;//蓝牙通话下传delay buffer,专供aec算法处理
	int16_t *hf_pcm_out     = NULL;//pBuf->hf_pcm_out;//蓝牙通话上传buffer
	int16_t *hf_dac_out     = NULL;//pBuf->hf_dac_out;//蓝牙通话DAC的buffer	
	int16_t *hf_rec_out     = NULL;//pBuf->hf_rec_out;//蓝牙通话送录音的buffer	
	int16_t *u_pcm_tmp      = (int16_t *)pcm_buf_3;
	int16_t *d_pcm_tmp      = (int16_t *)pcm_buf_4;
	#endif
	
	int16_t *echo_tmp   	= (int16_t *)pcm_buf_1;
	int16_t *reverb_tmp 	= (int16_t *)pcm_buf_2;
	int16_t *b_e_r_mix_tmp 	= (int16_t *)pcm_buf_2;
	int16_t *rec_bypass_tmp = (int16_t *)pcm_buf_3;
	int16_t *rec_effect_tmp = (int16_t *)pcm_buf_4;
    int16_t *rec_remind_tmp = (int16_t *)pcm_buf_1;

	if(pAudioCore->AudioSource[MIC_SOURCE_NUM].Enable == TRUE)////mic buff
	{
#if (BT_HFP_SUPPORT == ENABLE)
		if(GetSystemMode() == AppModeBtHfPlay)
		{
			//hf mode
			hf_mic_in = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;
		}
		else
#endif
		{
			bypass_tmp = mic_pcm = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;//双mic输入
		}
	}

#ifdef CFG_FUNC_RECORDER_EN
	if(GetSystemMode() == AppModeCardPlayBack || GetSystemMode() == AppModeUDiskPlayBack || GetSystemMode() == AppModeFlashFsPlayBack)
	{
		if(pAudioCore->AudioSource[PLAYBACK_SOURCE_NUM].Enable == TRUE)
		{
			music_pcm = pAudioCore->AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf;// include usb/sd source 
		}
	}
	else
#endif
	{
		if(pAudioCore->AudioSource[APP_SOURCE_NUM].Enable == TRUE)////music buff
		{
	#if (BT_HFP_SUPPORT == ENABLE) && defined(CFG_APP_BT_MODE_EN)
			if(GetSystemMode() == AppModeBtHfPlay)
			{
				//hf sco: nomo
				hf_pcm_in = pAudioCore->AudioSource[APP_SOURCE_NUM].PcmInBuf;

				//aec process:push the new data, pop the old data
				if(BtHf_AECRingDataSpaceLenGet() > CFG_BTHF_PARA_SAMPLES_PER_FRAME)
					BtHf_AECRingDataSet(hf_pcm_in, CFG_BTHF_PARA_SAMPLES_PER_FRAME);
				hf_aec_in = BtHf_AecInBuf();
			}
			else
	#endif
			{
				music_pcm = pAudioCore->AudioSource[APP_SOURCE_NUM].PcmInBuf;// include line/bt/usb/sd/spdif/hdmi/i2s/radio source
			}
		}
	}	
	
#if defined(CFG_FUNC_REMIND_SOUND_EN)	
	if(pAudioCore->AudioSource[REMIND_SOURCE_NUM].Enable == TRUE)////remind buff
	{
		//audio_src_sink.music_in = 0;
		AudioCoreSourceMute(APP_SOURCE_NUM, 1, 1);
		remind_in = pAudioCore->AudioSource[REMIND_SOURCE_NUM].PcmInBuf;
	}	
#endif

	if(pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].Enable == TRUE)   ////dacx buff
	{
#if (BT_HFP_SUPPORT == ENABLE)
		if(GetSystemMode() == AppModeBtHfPlay)
		{
			//hf mode
			hf_dac_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
			hf_pcm_out = pAudioCore->AudioSink[AUDIO_HF_SCO_SINK_NUM].PcmOutBuf;
		}
		else
#endif
		{
			monitor_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
		}
	}

#ifdef CFG_RES_AUDIO_DACX_EN 	
	if(pAudioCore->AudioSink[AUDIO_DACX_SINK_NUM].Enable == TRUE)   ////dacx buff
	{
		record_out = pAudioCore->AudioSink[AUDIO_DACX_SINK_NUM].PcmOutBuf;
	}	
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN 	
	if(pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].Enable == TRUE)	////dacx buff
	{
#if (CFG_RES_I2S_PORT==1)
		i2s1_out = pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf;
#else
		i2s0_out = pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf;
#endif
	}
#endif

#ifdef CFG_FUNC_RECORDER_EN
	if(pAudioCore->AudioSink[AUDIO_RECORDER_SINK_NUM].Enable == TRUE)
	{
		local_rec_out = pAudioCore->AudioSink[AUDIO_RECORDER_SINK_NUM].PcmOutBuf;
	}
#endif

#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexLock(AudioEffectMutex);
	}
#endif

    if(monitor_out)
	{
		memset(monitor_out, 0, n * 2 * 2);
    }

    if(record_out)
    {
		memset(record_out, 0, n * 2);
    }
	
    if(usb_out)
    {
		memset(usb_out, 0, n * 2 * 2);//mono*2 stereo*4
    }
	
    if(i2s0_out)
    {
		memset(i2s0_out, 0, n * 2 * 2);//mono*2 stereo*4
    }

#if (BT_HFP_SUPPORT == ENABLE)
	if(hf_pcm_out) //nomo
	{
		memset(hf_pcm_out, 0, n * 2);
    }
	
	if(hf_dac_out) //stereo
	{
		memset(hf_dac_out, 0, n * 2 * 2);
    }
	
	if(hf_rec_out) //nomo
	{
		memset(hf_rec_out, 0, n * 2);
    }
#endif

	memset(pcm_buf_1,   0, sizeof(pcm_buf_1));
	memset(pcm_buf_2,   0, sizeof(pcm_buf_2));
	memset(pcm_buf_3,   0, sizeof(pcm_buf_3));
	memset(pcm_buf_4,   0, sizeof(pcm_buf_4));

#if (BT_HFP_SUPPORT == ENABLE)
	if(hf_mic_in && hf_pcm_in && hf_pcm_out && hf_dac_out)
	{
		//hf_pcm_in: 16K nomo //256*2
		//hf_mic_in: 16K stereo (需要转成nomo)  //256*2*2
		//hf_pcm_out: 16K nomo //256*2
		//hf_dac_out: 16K stereo //256*2*2
		
		//手机端通话音频送DAC 16K nomo -> stereo
		for(s = 0; s < n; s++)
		{
			hf_dac_out[s*2 + 0] = hf_pcm_in[s];
			hf_dac_out[s*2 + 1] = hf_pcm_in[s]; 
		}

        //本地MIC采样做降噪处理
		#if CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
		//AudioEffectExpanderApply(&gCtrlVars.mic_expander_unit, hf_mic_in, hf_mic_in, n);
		#endif
	
		//本地MIC采样及手机端通话音频做AEC处理
		//only mic input : stereo -> mono
	    for(s = 0; s < n; s++)
		{
			d_pcm_tmp[s] = __nds32__clips((((int32_t)hf_mic_in[2 * s + 0] + (int32_t)hf_mic_in[2 * s + 1])), 16-1);
			u_pcm_tmp[s] = hf_aec_in[s];
		}
	    #if CFG_AUDIO_EFFECT_MIC_AEC_EN
		AudioEffectAecApply(&gCtrlVars.mic_aec_unit, u_pcm_tmp , d_pcm_tmp, hf_pcm_out, n);
	    #else
		for(s = 0; s < n; s++)
		{
		    hf_pcm_out[s] = d_pcm_tmp[s];
		}
	    #endif

		/*for(s = 0; s < n; s++)
		{
		    pcm   =rec_bypass_tmp[s];//
			hf_pcm_out[2 * s + 0] = pcm; 
			hf_pcm_out[2 * s + 1] = pcm; 
		}*/	
		
		//AEC处理之后的数据做变调处理
		#if CFG_AUDIO_EFFECT_MIC_PITCH_SHIFTER_EN 
		//AudioEffectPitchShifterApply(&gCtrlVars.pitch_shifter_unit, hf_pcm_out, hf_pcm_out, n, gCtrlVars.adc_mic_channel_num);
		#endif
	}
    //本地MIC采样及手机端通话音频做录音处理
	if(hf_rec_out)
	{
		for(s = 0; s < n; s++)
		{
			hf_rec_out[2*s + 0] = __nds32__clips((((int32_t)hf_mic_in[2*s + 0] + (int32_t)hf_pcm_in[2*s + 0])), 16-1);
			hf_rec_out[2*s + 1] = __nds32__clips((((int32_t)hf_mic_in[2*s + 1] + (int32_t)hf_pcm_in[2*s + 1])), 16-1);
		}
	}
#endif
	
	//伴奏信号音效处理
	if(music_pcm)
	{
		if(gCtrlVars.adc_line_channel_num == 1)
		{
			for(s = 0; s < n; s++)
			{
				music_pcm[s] = __nds32__clips((((int32_t)music_pcm[2 * s + 0] + (int32_t)music_pcm[2 * s + 1]) ), 16-1);
			}
		}
		
		#if CFG_AUDIO_EFFECT_MUSIC_NOISE_SUPPRESSOR_EN
		AudioEffectExpanderApply(&gCtrlVars.music_expander_unit, music_pcm, music_pcm, n);
		#endif
		
		#if CFG_AUDIO_EFFECT_MUSIC_SILENCE_DECTOR_EN
		AudioEffectSilenceDectorApply(&gCtrlVars.MusicAudioSdct_unit,  music_pcm,  n);
		#endif
		
		#if CFG_AUDIO_EFFECT_AUX_GAIN_CONTROL_EN
		{
			#ifdef CFG_FUNC_SHUNNING_EN
			uint32_t gain;
			gain = gCtrlVars.aux_gain_control_unit.gain;
			gCtrlVars.aux_gain_control_unit.gain = gCtrlVars.aux_out_dyn_gain;//闪避功能打开时用到
			#endif
			AudioEffectPregainApply(&gCtrlVars.aux_gain_control_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
			#ifdef CFG_FUNC_SHUNNING_EN
			gCtrlVars.aux_gain_control_unit.gain = gain;
			#endif
		}		
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN 
		AudioEffectPitchShifterProApply(&gCtrlVars.pitch_shifter_pro_unit, music_pcm, music_pcm, n);
		#endif	
		
		#if CFG_AUDIO_EFFECT_VOCAL_CUT_EN
        AudioEffectVocalCutApply(&gCtrlVars.vocal_cut_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_VOCAL_REMOVE_EN
        AudioEffectVocalRemoveApply(&gCtrlVars.vocal_remove_unit,  music_pcm, music_pcm, n);
        #endif			
	
		#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
        AudioEffectVBApply(&gCtrlVars.music_vb_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
        AudioEffectVBClassicApply(&gCtrlVars.music_vb_classic_unit, music_pcm, music_pcm, n);
		#endif
		
        #if CFG_AUDIO_EFFECT_MUSIC_3D_EN
        AudioEffectThreeDApply(&gCtrlVars.music_threed_unit, music_pcm, music_pcm, n);
        #endif			

		#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
		AudioEffectPcmDelayApply(&gCtrlVars.music_delay_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_EXCITER_EN
		AudioEffectExciterApply(&gCtrlVars.music_exciter_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_PRE_EQ_EN
		AudioEffectEQApply(&gCtrlVars.music_pre_eq_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_DRC_EN
		AudioEffectDRCApply(&gCtrlVars.music_drc_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_OUT_EQ_EN
		AudioEffectEQApply(&gCtrlVars.music_out_eq_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
		#endif
		
		if(gCtrlVars.adc_line_channel_num == 1)
		{
			memcpy(&music_pcm[n], music_pcm, n*2);
			for(s = 0; s < n; s++)
			{
				music_pcm[2*s + 0] = music_pcm[2*s + 1] = music_pcm[n+s];
			}
		}

	}

    //MIC信号音效处理
	if(mic_pcm)
	{	
	    #ifdef CFG_FUNC_DETECT_MIC_EN
		if(gCtrlVars.MicOnlin && (!gCtrlVars.Mic2Onlin))
		{
			#ifdef CFG_FUNC_DETECT_MIC_SEG_EN
			if(gCtrlVars.MicSegment == 3)
			{
				for(s = 0; s < n; s++)
				{
					mic_pcm[s*2 + 1] = mic_pcm[s*2 + 0] = 0; 
				}
			}
			else
			#endif
			{
				for(s = 0; s < n; s++)
				{
					mic_pcm[s*2 + 0] = mic_pcm[s*2 + 1]; 
				}
			}
		}
		else if((!gCtrlVars.MicOnlin) && gCtrlVars.Mic2Onlin)
		{
			for(s = 0; s < n; s++)
			{
				mic_pcm[s*2 + 1] = mic_pcm[s*2 + 0]; 
			}
		}
		else if(gCtrlVars.MicOnlin && gCtrlVars.Mic2Onlin)
		{
			#ifdef CFG_FUNC_DETECT_MIC_SEG_EN
			if(gCtrlVars.MicSegment == 3)
			{
				for(s = 0; s < n; s++)
				{
					mic_pcm[s*2 + 1] = mic_pcm[s*2 + 0]; 
				}
			}
			else
			#endif
			{
				for(s = 0; s < n; s++)
				{
					pcm	= __nds32__clips((((int32_t)mic_pcm[2 * s + 0] + (int32_t)mic_pcm[2 * s + 1]) ), 16-1); 
					mic_pcm[2 * s + 0] = pcm; 
					mic_pcm[2 * s + 1] = pcm; 
				}
			}
		}
		#else
		if( (gCtrlVars.line3_l_mic1_en) && (gCtrlVars.line3_r_mic2_en) )
		{
			for(s = 0; s < n; s++)
			{
				pcm	= __nds32__clips((((int32_t)mic_pcm[2 * s + 0] + (int32_t)mic_pcm[2 * s + 1]) ), 16-1); 
				mic_pcm[2 * s + 0] = pcm; 
				mic_pcm[2 * s + 1] = pcm; 
			}
		}
		else if(gCtrlVars.line3_l_mic1_en )
		{
			for(s = 0; s < n; s++)
			{
				mic_pcm[s*2 + 1] = mic_pcm[s*2 + 0]; 
			}
		}
		else if( gCtrlVars.line3_r_mic2_en )
		{
			for(s = 0; s < n; s++)
			{
				mic_pcm[s*2 + 0] = mic_pcm[s*2 + 1]; 
			}
		}
		#endif
        else
		{
			for(s = 0; s < n; s++)
			{
				mic_pcm[s*2 + 0] = 0;
				mic_pcm[s*2 + 1] = 0; 
			}
		}
		if(gCtrlVars.adc_mic_channel_num == 1)
		{
			for(s = 0; s < n; s++)
			{
				pcm	= __nds32__clips((((int32_t)mic_pcm[2 * s + 0] + (int32_t)mic_pcm[2 * s + 1]) ), 16-1);
				mic_pcm[s] = pcm;
			}
		}
		
		#ifdef CFG_FUNC_AI
		ai_audio_encode(mic_pcm,n,gCtrlVars.adc_mic_channel_num);
		#endif

		#if CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
		AudioEffectExpanderApply(&gCtrlVars.mic_expander_unit, mic_pcm, mic_pcm, n);
		#endif
		
        #if CFG_AUDIO_EFFECT_MIC_SILENCE_DECTOR_EN
		AudioEffectSilenceDectorApply(&gCtrlVars.MicAudioSdct_unit,  mic_pcm,  n);
		#endif
		
		#if CFG_AUDIO_EFFECT_MIC_FREQ_SHIFTER_EN || CFG_AUDIO_EFFECT_MIC_HOWLING_DECTOR_EN || CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN || CFG_AUDIO_EFFECT_CHORUS_EN
		//freq shifter，howling只支持单声道数据	
		 //********************* stereo 2 mono ****************************  
		if((gCtrlVars.adc_mic_channel_num == 2) && (gCtrlVars.freq_shifter_unit.enable 
			|| gCtrlVars.howling_dector_unit.enable
			#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
			|| gCtrlVars.voice_changer_pro_unit.enable
			#endif
			|| gCtrlVars.chorus_unit.enable)
			)
		{
			for(s = 0; s < n; s++)
			{
				// pcm	= __nds32__clips((((int32_t)mic_pcm[2 * s + 0] + (int32_t)mic_pcm[2 * s + 1]) ), 16-1);

				mic_pcm[s] = mic_pcm[2 * s + 0];

			}
		}
		#endif
				
        #if CFG_AUDIO_EFFECT_MIC_FREQ_SHIFTER_EN
		AudioEffectFreqShifterApply(&gCtrlVars.freq_shifter_unit, mic_pcm, mic_pcm, n);
		#endif
		
        #if CFG_AUDIO_EFFECT_MIC_HOWLING_DECTOR_EN
		AudioEffectHowlingSuppressorApply(&gCtrlVars.howling_dector_unit, mic_pcm, mic_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN 
		AudioEffectVoiceChangerProApply(&gCtrlVars.voice_changer_pro_unit, mic_pcm, mic_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_CHORUS_EN
        AudioEffectChorusApply(&gCtrlVars.chorus_unit,  mic_pcm, mic_pcm, n);
        #endif
		
		#if CFG_AUDIO_EFFECT_MIC_FREQ_SHIFTER_EN || CFG_AUDIO_EFFECT_MIC_HOWLING_DECTOR_EN || CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN || CFG_AUDIO_EFFECT_CHORUS_EN
		 //********************* mono 2 stereo ****************************
		if((gCtrlVars.adc_mic_channel_num == 2) && (
				gCtrlVars.freq_shifter_unit.enable 
			|| gCtrlVars.howling_dector_unit.enable
			#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
			|| gCtrlVars.voice_changer_pro_unit.enable
			#endif
			|| gCtrlVars.chorus_unit.enable))
		{	
		    for(s = 0; s < n; s++)
			{
			    rec_bypass_tmp[s] = mic_pcm[s];
			}
			for(s = 0; s < n; s++)
			{
			    pcm   =rec_bypass_tmp[s];//
				mic_pcm[2 * s + 0] = pcm; 
				mic_pcm[2 * s + 1] = pcm; 
			}		
		}
		#endif

		#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_EN 
		AudioEffectVoiceChangerApply(&gCtrlVars.voice_changer_unit, mic_pcm, mic_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MIC_PITCH_SHIFTER_EN 
		AudioEffectPitchShifterApply(&gCtrlVars.pitch_shifter_unit, mic_pcm, mic_pcm, n, gCtrlVars.adc_mic_channel_num);
		#endif					
		
		#if CFG_AUDIO_EFFECT_MIC_AUTO_TUNE_EN
		AudioEffectAutoTuneApply(&gCtrlVars.auto_tune_unit, mic_pcm, mic_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MIC_ECHO_EN
		AudioEffectEchoApply(&gCtrlVars.echo_unit, mic_pcm, echo_tmp, n);
		#endif

		#if CFG_AUDIO_EFFECT_MIC_REVERB_EN
		AudioEffectReverbApply(&gCtrlVars.reverb_unit, mic_pcm, reverb_tmp, n);
		#endif

        #if CFG_AUDIO_EFFECT_MIC_PLATE_REVERB_EN
		AudioEffectPlateReverbApply(&gCtrlVars.plate_reverb_unit, mic_pcm, reverb_tmp, n);
		#endif

		#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
		AudioEffectReverbProApply(&gCtrlVars.reverb_pro_unit, mic_pcm, reverb_tmp, n);
		#endif
		
		//bypass eq
		#if CFG_AUDIO_EFFECT_MIC_BYPASS_EQ_EN
		AudioEffectEQApply(&gCtrlVars.mic_bypass_eq_unit, bypass_tmp, bypass_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif

        //get bypass data for record 
		//if(gCtrlVars.reverb_pro_unit.enable==0)
		{
			for(s = 0; s < n; s++)
			{
				rec_bypass_tmp[2*s + 0] = bypass_tmp[2*s + 0];
				rec_bypass_tmp[2*s + 1] = bypass_tmp[2*s + 1];
			}

		}
		//else
		//{
		//	for(s = 0; s < n; s++)
		//	{
		//		rec_bypass_tmp[2*s + 0] = 0;///for reberb pro
		//		rec_bypass_tmp[2*s + 1] = 0;
		//	}
		//}
		
        //echo eq
		#if CFG_AUDIO_EFFECT_MIC_ECHO_EQ_EN
		AudioEffectEQApply(&gCtrlVars.mic_echo_eq_unit, echo_tmp, echo_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif		
		
        //reverb eq
		#if CFG_AUDIO_EFFECT_MIC_REVERB_EQ_EN
		AudioEffectEQApply(&gCtrlVars.mic_reverb_eq_unit, reverb_tmp, reverb_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif

        //get echo+reverb data for record 
		//if(gCtrlVars.reverb_pro_unit.enable==0)
		{
			for(s = 0; s < n; s++)
			{
				int32_t Data_L = (int32_t)echo_tmp[2*s+0] + (int32_t)reverb_tmp[2*s+0];
				int32_t Data_R = (int32_t)echo_tmp[2*s+1] + (int32_t)reverb_tmp[2*s+1];

				rec_effect_tmp[2 * s + 0] = __nds32__clips((Data_L >> 0), 16-1);
				rec_effect_tmp[2 * s + 1] = __nds32__clips((Data_R >> 0), 16-1);
			}
		}
		//else
		//{
		//	for(s = 0; s < n; s++)
		//	{
		//		int32_t Data_L = (int32_t)reverb_tmp[2*s+0];///for reberb pro
		//		int32_t Data_R = (int32_t)reverb_tmp[2*s+1];///for reberb pro

		//		rec_effect_tmp[2 * s + 0] = __nds32__clips((Data_L >> 0), 16-1);
		//		rec_effect_tmp[2 * s + 1] = __nds32__clips((Data_R >> 0), 16-1);
		//	}
		//}
		
        //bypass pregain
		#if CFG_AUDIO_EFFECT_MIC_BYPASS_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.mic_bypass_gain_control_unit, bypass_tmp, bypass_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif
		
        //echo pregain
		#if CFG_AUDIO_EFFECT_MIC_ECHO_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.mic_echo_control_unit, echo_tmp, echo_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif

		//reverb pregain
		#if CFG_AUDIO_EFFECT_MIC_REVERB_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.mic_reverb_gain_control_unit, reverb_tmp, reverb_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif
		
		//mux
		//if(gCtrlVars.reverb_pro_unit.enable==0)
		{
			for(s = 0; s < n; s++)
			{
				if(gCtrlVars.adc_mic_channel_num == 2)
				{
					int32_t Data_L = (int32_t)bypass_tmp[2*s+0] + (int32_t)echo_tmp[2*s+0] + (int32_t)reverb_tmp[2*s+0];
					int32_t Data_R = (int32_t)bypass_tmp[2*s+1] + (int32_t)echo_tmp[2*s+1] + (int32_t)reverb_tmp[2*s+1];

					b_e_r_mix_tmp[2 * s + 0] = __nds32__clips((Data_L >> 0), 16-1);
					b_e_r_mix_tmp[2 * s + 1] = __nds32__clips((Data_R >> 0), 16-1);
				}
				else
				{
					int32_t Data_M = (int32_t)bypass_tmp[s] + (int32_t)echo_tmp[s] + (int32_t)reverb_tmp[s];
					b_e_r_mix_tmp[s] = __nds32__clips((Data_M >> 0), 16-1);
				}
			}
		}
		//else
		//{
		//	for(s = 0; s < n; s++)
		//	{
		//		if(gCtrlVars.adc_mic_channel_num == 2)
		//		{
		//			int32_t Data_L = (int32_t)echo_tmp[2*s+0] + (int32_t)reverb_tmp[2*s+0];///for reberb pro
		//			int32_t Data_R = (int32_t)echo_tmp[2*s+1] + (int32_t)reverb_tmp[2*s+1];///for reberb pro

		//			b_e_r_mix_tmp[2 * s + 0] = __nds32__clips((Data_L >> 0), 16-1);
		//			b_e_r_mix_tmp[2 * s + 1] = __nds32__clips((Data_R >> 0), 16-1);
		//		}
		//		else
		//		{
		//			int32_t Data_M = (int32_t)bypass_tmp[s] + (int32_t)echo_tmp[s] + (int32_t)reverb_tmp[s];
		//			b_e_r_mix_tmp[s] = __nds32__clips((Data_M >> 0), 16-1);
		//		}
		//	}
		//}

		if(gCtrlVars.adc_mic_channel_num == 1)
		{
			memcpy(&b_e_r_mix_tmp[n], b_e_r_mix_tmp, n*2);
			memcpy(&bypass_tmp[n], bypass_tmp, n*2);
			for(s = 0; s < n; s++)
			{
				b_e_r_mix_tmp[2*s + 0] = b_e_r_mix_tmp[2*s + 1] = b_e_r_mix_tmp[n+s];
				bypass_tmp[2*s + 0]    = bypass_tmp[2*s + 1]    = bypass_tmp[n+s];
			}
		}
		
		#if CFG_AUDIO_EFFECT_MIC_DRC_EN
		AudioEffectDRCApply(&gCtrlVars.mic_drc_unit, b_e_r_mix_tmp, b_e_r_mix_tmp, n);
		#endif
	}	
	
	#if CFG_DAC0_EN
	//DAC0立体声监听音效处理
	if(monitor_out)
	{			
		#if CFG_AUDIO_EFFECT_MIC_OUT_EQ_EN
		AudioEffectEQApply(&gCtrlVars.mic_out_eq_unit, b_e_r_mix_tmp, b_e_r_mix_tmp, n, 2);
		#endif

		#if CFG_AUDIO_EFFECT_MIC_OUT_GAIN_CONTROL_EN
		#if 0//CFG_USB_MODE == AUDIO_GAME_HEADSET//电竞耳机上用到
		{
			gCtrlVars.mic_out_gain_control_unit.enable = 1;
	        gCtrlVars.mic_out_gain_control_unit.mute = 0;
	        gCtrlVars.mic_out_gain_control_unit.gain   = DigVolTab_64[gCtrlVars.UsbMicToSpeakerVolume];  		
	        if(gCtrlVars.UsbMicToSpeakerMute) gCtrlVars.mic_out_gain_control_unit.gain = 0;
		}
		#endif
		AudioEffectPregainApply(&gCtrlVars.mic_out_gain_control_unit, b_e_r_mix_tmp, b_e_r_mix_tmp, n, 2);
		#endif
				
		for(s = 0; s < n; s++)
		{
			if(music_pcm)
			{
				monitor_out[2*s + 0] = __nds32__clips((((int32_t)music_pcm[2*s + 0] + (int32_t)b_e_r_mix_tmp[2*s + 0])), 16-1);
				monitor_out[2*s + 1] = __nds32__clips((((int32_t)music_pcm[2*s + 1] + (int32_t)b_e_r_mix_tmp[2*s + 1])), 16-1);
			}
			else
			{
				monitor_out[2*s + 0] = b_e_r_mix_tmp[2*s + 0];
				monitor_out[2*s + 1] = b_e_r_mix_tmp[2*s + 1];
			}

		}		
		
        #if defined(CFG_FUNC_REMIND_SOUND_EN)//提示音音效处理
		if(remind_in)
		{
			for(s = 0; s < n; s++)
			{
				monitor_out[2*s + 0] = __nds32__clips((((int32_t)monitor_out[2*s + 0] + (int32_t)remind_in[2*s + 0])), 16-1);
				monitor_out[2*s + 1] = __nds32__clips((((int32_t)monitor_out[2*s + 1] + (int32_t)remind_in[2*s + 1])), 16-1);
			}
		}
        #endif

		if(i2s0_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s0_out[2*s + 0] = monitor_out[2*s + 0];
				i2s0_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}
		
		if(i2s1_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s1_out[2*s + 0] = monitor_out[2*s + 0];
				i2s1_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}	
	}
	#endif
	
    #ifdef CFG_FUNC_RECORDER_EN
	if(local_rec_out)
	{
		for(s = 0; s < n; s++)
		{
			if(GetWhetherRecMusic())
			{
				local_rec_out[2*s + 0] = monitor_out[2*s + 0];
				local_rec_out[2*s + 1] = monitor_out[2*s + 1];
			}
			else
			{
				local_rec_out[2*s + 0] = b_e_r_mix_tmp[2*s + 0];
				local_rec_out[2*s + 1] = b_e_r_mix_tmp[2*s + 1];
			}
		}
	}
	#endif
	
	#ifdef CFG_RES_AUDIO_DACX_EN
	//DAC X单声道录音音效处理
	if(record_out)
	{
	    //if(gCtrlVars.reverb_pro_unit.enable)
		//{
		//	memset(rec_bypass_tmp,   0, n * 4);
		//}

		#if CFG_AUDIO_EFFECT_REC_BYPASS_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.rec_bypass_gain_control_unit, rec_bypass_tmp, rec_bypass_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif
		
		#if CFG_AUDIO_EFFECT_REC_EFFECT_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.rec_effect_gain_control_unit, rec_effect_tmp, rec_effect_tmp, n, gCtrlVars.adc_mic_channel_num);
		#endif

		#if CFG_AUDIO_EFFECT_REC_AUX_GAIN_CONTROL_EN
		if(music_pcm) AudioEffectPregainApply(&gCtrlVars.rec_aux_gain_control_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
		#endif
		
		if(music_pcm)
		{
			for(s = 0; s < n; s++)
			{
				record_out[s] = __nds32__clips((((int32_t)rec_effect_tmp[2*s+0] + (int32_t)rec_bypass_tmp[2*s+0] + (int32_t)music_pcm[2*s+0]           
									    + (int32_t)rec_effect_tmp[2*s+1] + (int32_t)rec_bypass_tmp[2*s+1] + (int32_t)music_pcm[2*s+1])), 16-1);
			}
		}
		else
		{
			for(s = 0; s < n; s++)
			{
				record_out[s] = __nds32__clips((((int32_t)rec_effect_tmp[2*s+0] + (int32_t)rec_bypass_tmp[2*s+0] + 0           
						                + (int32_t)rec_effect_tmp[2*s+1] + (int32_t)rec_bypass_tmp[2*s+1] + 0)), 16-1);
			}
		}
				
		#if CFG_AUDIO_EFFECT_REC_OUT_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.rec_out_gain_control_unit, record_out, record_out, n, 1);
		#endif
		
		#if CFG_AUDIO_EFFECT_REC_EQ_EN
		AudioEffectEQApply(&gCtrlVars.rec_eq_unit, record_out, record_out, n, 1);
		#endif

		#if CFG_AUDIO_EFFECT_REC_DRC_EN
		AudioEffectDRCApply(&gCtrlVars.rec_drc_unit, record_out, record_out, n);
		#endif

        #if CFG_AUDIO_EFFECT_PHASE_EN
		AudioEffectPhaseApply(&gCtrlVars.phase_control_unit, record_out, record_out, n, 1);
		#endif
	}
	#endif

	#if CFG_USB_OUT_EN
    #if CFG_USB_OUT_STEREO_EN
	if(usb_out)
	{
	    if(!record_out)
		{
			#if CFG_AUDIO_EFFECT_REC_BYPASS_GAIN_CONTROL_EN
			AudioEffectPregainApply(&gCtrlVars.rec_bypass_gain_control_unit, rec_bypass_tmp, rec_bypass_tmp, n, gCtrlVars.adc_mic_channel_num);
			#endif
			
			#if CFG_AUDIO_EFFECT_REC_EFFECT_GAIN_CONTROL_EN
			AudioEffectPregainApply(&gCtrlVars.rec_effect_gain_control_unit, rec_effect_tmp, rec_effect_tmp, n, gCtrlVars.adc_mic_channel_num);
			#endif

			#if CFG_AUDIO_EFFECT_REC_AUX_GAIN_CONTROL_EN
			if(music_pcm) AudioEffectPregainApply(&gCtrlVars.rec_aux_gain_control_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
			#endif

    	}
		for(s = 0; s < n; s++)
		{
			if(music_pcm)
			{
				usb_out[2*s + 0] = __nds32__clips((((int32_t)music_pcm[2*s + 0] + (int32_t)rec_effect_tmp[2*s + 0]  + (int32_t)rec_bypass_tmp[2*s + 0])), 16-1);
				usb_out[2*s + 1] = __nds32__clips((((int32_t)music_pcm[2*s + 1] + (int32_t)rec_effect_tmp[2*s + 1]  + (int32_t)rec_bypass_tmp[2*s + 1])), 16-1);
			}
			else
			{
				usb_out[2*s + 0] = __nds32__clips((((int32_t)rec_effect_tmp[2*s + 0]  + (int32_t)rec_bypass_tmp[2*s + 0])), 16-1);
				usb_out[2*s + 1] = __nds32__clips((((int32_t)rec_effect_tmp[2*s + 1]  + (int32_t)rec_bypass_tmp[2*s + 1])), 16-1);
			}
		}
				
		#if CFG_AUDIO_EFFECT_REC_OUT_GAIN_CONTROL_EN
		#if CFG_SUPPORT_USB_VOLUME_SET
        gCtrlVars.rec_usb_out_gain_control_unit.enable = 1;
        gCtrlVars.rec_usb_out_gain_control_unit.mute = 0;
        gCtrlVars.rec_usb_out_gain_control_unit.gain   = DigVolTab_64[gCtrlVars.UsbMicVolume];  		
        if(gCtrlVars.UsbMicMute) gCtrlVars.rec_usb_out_gain_control_unit.gain = 0;
        AudioEffectPregainApply(&gCtrlVars.rec_usb_out_gain_control_unit, usb_out, usb_out, n, 2);			
		#endif
		#endif
		
		#if CFG_AUDIO_EFFECT_REC_OUT_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.rec_out_gain_control_unit, usb_out, usb_out, n, 2);
		#endif

	}
    #endif
	#endif
	#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexUnlock(AudioEffectMutex);
	}
    #endif

}
#else
void AudioMusicProcess(AudioCoreContext *pAudioCore)
{

	int16_t  s;
	int16_t  pcm;
	uint16_t n = mainAppCt.SamplesPreFrame;
	int16_t *music_pcm    	= NULL;//pBuf->music_in;///music input
	int16_t *remind_in      = NULL;//pBuf->remind_in;
	int16_t *monitor_out    = NULL;//pBuf->dac0_out; 
	int16_t *record_out     = NULL;//pBuf->dacx_out; 
	int16_t *i2s0_out       = NULL;//pBuf->i2s0_out; 
	int16_t *i2s1_out       = NULL;//pBuf->i2s1_out; 
	int16_t *usb_out        = NULL;//pBuf->usb_out; 
	int16_t *local_rec_out  = NULL;//pBuf->rec_out; 
	int16_t *rec_music_tmp  = (int16_t *)pcm_buf_1;
	#if BT_HFP_SUPPORT
	int16_t  *hf_mic_in     = NULL;//pBuf->hf_mic_in;//蓝牙通话mic采样buffer
	int16_t  *hf_pcm_in     = NULL;//pBuf->hf_pcm_in;//蓝牙通话下传buffer
	int16_t  *hf_aec_in		= NULL;//pBuf->hf_aec_in;;//蓝牙通话下传delay buffer,专供aec算法处理
	int16_t  *hf_pcm_out    = NULL;//pBuf->hf_pcm_out;//蓝牙通话上传buffer
	int16_t  *hf_dac_out    = NULL;//pBuf->hf_dac_out;//蓝牙通话DAC的buffer	
	int16_t  *hf_rec_out    = pBuf->hf_rec_out;//蓝牙通话送录音的buffer	
	int16_t  *u_pcm_tmp     = (int16_t *)pcm_buf_2;
	int16_t  *d_pcm_tmp     = (int16_t *)pcm_buf_3;
	#endif

	if(pAudioCore->AudioSource[MIC_SOURCE_NUM].Enable == TRUE)////mic buff
	{
#if (BT_HFP_SUPPORT == ENABLE)
		if(GetSystemMode() == AppModeBtHfPlay)
		{
			//hf mode
			hf_mic_in = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;
		}
#endif
	}
	
#ifdef CFG_FUNC_RECORDER_EN
	if(GetSystemMode() == AppModeCardPlayBack || GetSystemMode() == AppModeUDiskPlayBack || GetSystemMode() == AppModeFlashFsPlayBack)
	{
		if(pAudioCore->AudioSource[PLAYBACK_SOURCE_NUM].Enable == TRUE)
		{
			music_pcm = pAudioCore->AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf;// include usb/sd source 
		}
	}
	else
#endif
	{
		if(pAudioCore->AudioSource[APP_SOURCE_NUM].Enable == TRUE)////music buff
		{
	#if (BT_HFP_SUPPORT == ENABLE) && defined(CFG_APP_BT_MODE_EN)
			if(GetSystemMode() == AppModeBtHfPlay)
			{
				//hf sco: nomo
				hf_pcm_in = pAudioCore->AudioSource[APP_SOURCE_NUM].PcmInBuf;

				//aec process:push the new data, pop the old data
				if(BtHf_AECRingDataSpaceLenGet() > CFG_BTHF_PARA_SAMPLES_PER_FRAME)
					BtHf_AECRingDataSet(hf_pcm_in, CFG_BTHF_PARA_SAMPLES_PER_FRAME);
				hf_aec_in = BtHf_AecInBuf();
			}
			else
	#endif
			{
				music_pcm = pAudioCore->AudioSource[APP_SOURCE_NUM].PcmInBuf;// include line/bt/usb/sd/spdif/hdmi/i2s/radio source
			}
		}
	}	
	
#if defined(CFG_FUNC_REMIND_SOUND_EN)	
	if(pAudioCore->AudioSource[REMIND_SOURCE_NUM].Enable == TRUE)////remind buff
	{
		//audio_src_sink.music_in = 0;
		AudioCoreSourceMute(APP_SOURCE_NUM, 1, 1);
		remind_in = pAudioCore->AudioSource[REMIND_SOURCE_NUM].PcmInBuf;
	}	
#endif

    if(pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].Enable == TRUE)   ////dacx buff
	{
	#if (BT_HFP_SUPPORT == ENABLE)
		if(GetSystemMode() == AppModeBtHfPlay)
		{
			//hf mode
			hf_dac_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
			hf_pcm_out = pAudioCore->AudioSink[AUDIO_HF_SCO_SINK_NUM].PcmOutBuf;
		}
		else
	#endif
		{
			monitor_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
		}
	}	
#ifdef CFG_RES_AUDIO_DACX_EN 	
	if(pAudioCore->AudioSink[AUDIO_DACX_SINK_NUM].Enable == TRUE)   ////dacx buff
	{
		record_out = pAudioCore->AudioSink[AUDIO_DACX_SINK_NUM].PcmOutBuf;
	}	
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN 	
	if(pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].Enable == TRUE)	////dacx buff
	{
#if (CFG_RES_I2S_PORT==1)
		i2s1_out = pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf;
#else
		i2s0_out = pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf;
#endif
	}
#endif

#ifdef CFG_FUNC_RECORDER_EN
	if(pAudioCore->AudioSink[AUDIO_RECORDER_SINK_NUM].Enable == TRUE)
	{
		local_rec_out = pAudioCore->AudioSink[AUDIO_RECORDER_SINK_NUM].PcmOutBuf;
	}
#endif

#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexLock(AudioEffectMutex);
	}
#endif

    if(monitor_out)
	{
		memset(monitor_out, 0, n * 2 * 2);
    }

    if(record_out)
    {
		memset(record_out, 0, n * 2);
    }
	
    if(usb_out)
    {
		memset(usb_out, 0, n * 2 * 2);//mono*2 stereo*4
    }
	
    if(i2s0_out)
    {
		memset(i2s0_out, 0, n * 2 * 2);//mono*2 stereo*4
    }

#if (BT_HFP_SUPPORT == ENABLE)
	if(hf_pcm_out) //nomo
	{
		memset(hf_pcm_out, 0, n * 2);
	}
	
	if(hf_dac_out) //stereo
	{
		memset(hf_dac_out, 0, n * 2 * 2);
	}
	
	if(hf_rec_out) //nomo
	{
		memset(hf_rec_out, 0, n * 2);
	}
#endif

	memset(pcm_buf_1,   0, sizeof(pcm_buf_1));
	memset(pcm_buf_2,   0, sizeof(pcm_buf_2));
	memset(pcm_buf_3,   0, sizeof(pcm_buf_3));
	memset(pcm_buf_4,   0, sizeof(pcm_buf_4));

#if (BT_HFP_SUPPORT == ENABLE)
	if(hf_mic_in && hf_pcm_in && hf_pcm_out && hf_dac_out)
	{
		//hf_pcm_in: 16K nomo //256*2
		//hf_mic_in: 16K stereo (需要转成nomo)  //256*2*2
		//hf_pcm_out: 16K nomo //256*2
		//hf_dac_out: 16K stereo //256*2*2
		
		//手机端通话音频送DAC 16K nomo -> stereo
		for(s = 0; s < n; s++)
		{
			hf_dac_out[s*2 + 0] = hf_pcm_in[s];
			hf_dac_out[s*2 + 1] = hf_pcm_in[s]; 
		}

        //本地MIC采样做降噪处理
		#if CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
		//AudioEffectExpanderApply(&gCtrlVars.mic_expander_unit, hf_mic_in, hf_mic_in, n);
		#endif
	
		//本地MIC采样及手机端通话音频做AEC处理
		//only mic input : stereo -> mono
	    for(s = 0; s < n; s++)
		{
			d_pcm_tmp[s] = __nds32__clips((((int32_t)hf_mic_in[2 * s + 0] + (int32_t)hf_mic_in[2 * s + 1])), 16-1);
			u_pcm_tmp[s] = hf_aec_in[s];
		}
	    #if CFG_AUDIO_EFFECT_MIC_AEC_EN
		AudioEffectAecApply(&gCtrlVars.mic_aec_unit, u_pcm_tmp , d_pcm_tmp, hf_pcm_out, n);
	    #else
		for(s = 0; s < n; s++)
		{
		    hf_pcm_out[s] = d_pcm_tmp[s];
		}
	    #endif

		/*for(s = 0; s < n; s++)
		{
		    pcm   =rec_bypass_tmp[s];//
			hf_pcm_out[2 * s + 0] = pcm; 
			hf_pcm_out[2 * s + 1] = pcm; 
		}*/	
		
		//AEC处理之后的数据做变调处理
		#if CFG_AUDIO_EFFECT_MIC_PITCH_SHIFTER_EN 
		//AudioEffectPitchShifterApply(&gCtrlVars.pitch_shifter_unit, hf_pcm_out, hf_pcm_out, n, gCtrlVars.adc_mic_channel_num);
		#endif
	}
    //本地MIC采样及手机端通话音频做录音处理
	if(hf_rec_out)
	{
		for(s = 0; s < n; s++)
		{
			hf_rec_out[2*s + 0] = __nds32__clips((((int32_t)hf_mic_in[2*s + 0] + (int32_t)hf_pcm_in[2*s + 0])), 16-1);
			hf_rec_out[2*s + 1] = __nds32__clips((((int32_t)hf_mic_in[2*s + 1] + (int32_t)hf_pcm_in[2*s + 1])), 16-1);
		}
	}
#endif

	
	//伴奏信号音效处理
	if(music_pcm)
	{
		if(gCtrlVars.adc_line_channel_num == 1)
		{
			for(s = 0; s < n; s++)
			{
				music_pcm[s] = __nds32__clips((((int32_t)music_pcm[2 * s + 0] + (int32_t)music_pcm[2 * s + 1]) ), 16-1);
			}
		}
		
		#if CFG_AUDIO_EFFECT_MUSIC_NOISE_SUPPRESSOR_EN
		AudioEffectExpanderApply(&gCtrlVars.music_expander_unit, music_pcm, music_pcm, n);
		#endif
		
		#if CFG_AUDIO_EFFECT_MUSIC_SILENCE_DECTOR_EN
		AudioEffectSilenceDectorApply(&gCtrlVars.MusicAudioSdct_unit,  music_pcm,  n);
		#endif

		for(s = 0; s < n; s++)
		{
			rec_music_tmp[2*s + 0] = music_pcm[2*s + 0];
			rec_music_tmp[2*s + 1] = music_pcm[2*s + 1];
		}	
		
		#if CFG_AUDIO_EFFECT_AUX_GAIN_CONTROL_EN
		{
			#ifdef CFG_FUNC_SHUNNING_EN
			uint32_t gain;
			gain = gCtrlVars.aux_gain_control_unit.gain;
			gCtrlVars.aux_gain_control_unit.gain = gCtrlVars.aux_out_dyn_gain;//闪避功能打开时用到
			#endif
			AudioEffectPregainApply(&gCtrlVars.aux_gain_control_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
			#ifdef CFG_FUNC_SHUNNING_EN
			gCtrlVars.aux_gain_control_unit.gain = gain;
			#endif
		}		
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN 
		AudioEffectPitchShifterProApply(&gCtrlVars.pitch_shifter_pro_unit, music_pcm, music_pcm, n);
		#endif	
		
		#if CFG_AUDIO_EFFECT_VOCAL_CUT_EN
        AudioEffectVocalCutApply(&gCtrlVars.vocal_cut_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_VOCAL_REMOVE_EN
        AudioEffectVocalRemoveApply(&gCtrlVars.vocal_remove_unit,  music_pcm, music_pcm, n);
        #endif			
	
		#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
        AudioEffectVBApply(&gCtrlVars.music_vb_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
        AudioEffectVBClassicApply(&gCtrlVars.music_vb_classic_unit, music_pcm, music_pcm, n);
		#endif
		
        #if CFG_AUDIO_EFFECT_MUSIC_3D_EN
        AudioEffectThreeDApply(&gCtrlVars.music_threed_unit, music_pcm, music_pcm, n);
        #endif			

		#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
		AudioEffectPcmDelayApply(&gCtrlVars.music_delay_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_EXCITER_EN
		AudioEffectExciterApply(&gCtrlVars.music_exciter_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_PRE_EQ_EN
		AudioEffectEQApply(&gCtrlVars.music_pre_eq_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
		#endif
		
		#if CFG_AUDIO_EFFECT_MUSIC_DRC_EN
		AudioEffectDRCApply(&gCtrlVars.music_drc_unit, music_pcm, music_pcm, n);
		#endif

		#if CFG_AUDIO_EFFECT_MUSIC_OUT_EQ_EN
		AudioEffectEQApply(&gCtrlVars.music_out_eq_unit, music_pcm, music_pcm, n, gCtrlVars.adc_line_channel_num);
		#endif
		
		if(gCtrlVars.adc_line_channel_num == 1)
		{
			memcpy(&music_pcm[n], music_pcm, n*2);
			for(s = 0; s < n; s++)
			{
				music_pcm[2*s + 0] = music_pcm[2*s + 1] = music_pcm[n+s];
			}
		}
	}	
	
	#if CFG_DAC0_EN
	//DAC0立体声监听音效处理
	if(monitor_out)
	{							
		for(s = 0; s < n; s++)
		{
			if(music_pcm)
			{
				monitor_out[2*s + 0] = music_pcm[2*s + 0];
				monitor_out[2*s + 1] = music_pcm[2*s + 1];
			}
			else
			{
				monitor_out[2*s + 0] = 0;
				monitor_out[2*s + 1] = 0;
			}
		}		

		#if defined(CFG_FUNC_REMIND_SOUND_EN)//提示音音效处理
		if(remind_in)
		{
			for(s = 0; s < n; s++)
			{
				monitor_out[2*s + 0] = __nds32__clips((((int32_t)monitor_out[2*s + 0] + (int32_t)remind_in[2*s + 0])), 16-1);
				monitor_out[2*s + 1] = __nds32__clips((((int32_t)monitor_out[2*s + 1] + (int32_t)remind_in[2*s + 1])), 16-1);
			}
		}
        #endif
		
		if(i2s0_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s0_out[2*s + 0] = monitor_out[2*s + 0];
				i2s0_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}
		
		if(i2s1_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s1_out[2*s + 0] = monitor_out[2*s + 0];
				i2s1_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}	
	}
	#endif
	
    #ifdef CFG_FUNC_RECORDER_EN
	if(local_rec_out)
	{
		for(s = 0; s < n; s++)
		{
			local_rec_out[2*s + 0] = monitor_out[2*s + 0];
			local_rec_out[2*s + 1] = monitor_out[2*s + 1];
		}
	}
	#endif
	
	#ifdef CFG_RES_AUDIO_DACX_EN
	//DAC X单声道录音音效处理
	if(record_out)
	{		
		if(music_pcm)
		{
			for(s = 0; s < n; s++)
			{
				record_out[s] = __nds32__clips((((int32_t)rec_music_tmp[2*s+0]/2 + (int32_t)rec_music_tmp[2*s+1]/2)), 16-1);
			}
		}
		else
		{
			for(s = 0; s < n; s++)
			{
				record_out[s] = 0;
			}
		}
				
		#if CFG_AUDIO_EFFECT_REC_OUT_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.rec_out_gain_control_unit, record_out, record_out, n, 1);
		#endif
		
		#if CFG_AUDIO_EFFECT_REC_EQ_EN
		AudioEffectEQApply(&gCtrlVars.rec_eq_unit, record_out, record_out, n, 1);
		#endif

		#if CFG_AUDIO_EFFECT_REC_DRC_EN
		AudioEffectDRCApply(&gCtrlVars.rec_drc_unit, record_out, record_out, n);
		#endif

        #if CFG_AUDIO_EFFECT_PHASE_EN
		AudioEffectPhaseApply(&gCtrlVars.phase_control_unit, record_out, record_out, n, 1);
		#endif
	}
	#endif

	#if CFG_USB_OUT_EN
    #if CFG_USB_OUT_STEREO_EN
	if(usb_out)
	{
		for(s = 0; s < n; s++)
		{
			if(monitor_out)
			{
				usb_out[2*s + 0] = monitor_out[2*s + 0];
				usb_out[2*s + 1] = monitor_out[2*s + 1];
			}
			else
			{
				usb_out[2*s + 0] = 0;
				usb_out[2*s + 1] = 0;
			}
		}
				
		#if CFG_AUDIO_EFFECT_REC_OUT_GAIN_CONTROL_EN
		#if CFG_SUPPORT_USB_VOLUME_SET
        gCtrlVars.rec_usb_out_gain_control_unit.enable = 1;
        gCtrlVars.rec_usb_out_gain_control_unit.mute = 0;
        gCtrlVars.rec_usb_out_gain_control_unit.gain   = DigVolTab_64[gCtrlVars.UsbMicVolume];  		
        if(gCtrlVars.UsbMicMute) gCtrlVars.rec_usb_out_gain_control_unit.gain = 0;
        AudioEffectPregainApply(&gCtrlVars.rec_usb_out_gain_control_unit, usb_out, usb_out, n, 2);			
		#endif
		#endif
		
		#if CFG_AUDIO_EFFECT_REC_OUT_GAIN_CONTROL_EN
		AudioEffectPregainApply(&gCtrlVars.rec_out_gain_control_unit, usb_out, usb_out, n, 2);
		#endif
	}
    #endif
	#endif
	#ifdef FUNC_OS_EN
	if(AudioEffectMutex != NULL)
	{
		osMutexUnlock(AudioEffectMutex);
	}
    #endif

}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
/*
****************************************************************
* Pcm Delay音效初始化
*
*
****************************************************************
*/
void AudioEffectPcmDelayInit(PcmDelayUnit *unit, uint8_t channel, uint32_t sample_rate)
{

	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + PCM_DELAY_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_PCM_DELAY,unit,PCM_DELAY_SIZE);
			
			unit->ct = (PCMDelay *)DynamicStartAddr;
			
			DynamicStartAddr += PCM_DELAY_SIZE;

		}
		else
		{
			PrintMallocFail(EFF_PCM_DELAY,unit,PCM_DELAY_SIZE); 
			unit->enable = 0;
		}
        #else
		unit->ct = (PCMDelay *)osPortMallocFromEnd(PCM_DELAY_SIZE);
		
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("PCMDelay mem malloc err! %ld\n",EXPANDER_SIZE);
		}
        #endif
	}
	
	if(unit->ct != NULL)
	{
		pcm_delay_init(unit->ct, channel, unit->delay_samples);
	}	
}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
/*
****************************************************************
* Pcm Delay音效配置
*
*
****************************************************************
*/
void AudioEffectPcmDelayConfig(PcmDelayUnit *unit, uint8_t channel, uint32_t sample_rate)
{

    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct != NULL)
	{
		pcm_delay_init(unit->ct, channel, unit->delay_samples);
	}
}
#endif	
/*
****************************************************************
* Exciter音效初始化
*
*
****************************************************************
*/
void AudioEffectExciterInit(ExciterUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MUSIC_EXCITER_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + EXCITER_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_EXCITER,unit,EXCITER_SIZE);
			
			unit->ct = (ExciterContext *)DynamicStartAddr;
			
			DynamicStartAddr += EXCITER_SIZE;

		}
		else
		{
			PrintMallocFail(EFF_EXCITER,unit,EXCITER_SIZE); 
			unit->enable = 0;
		}
        #else
		unit->ct = (ExciterContext *)osPortMallocFromEnd(EXCITER_SIZE);
			
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("ExciterContext mem malloc err! %ld\n",EXCITER_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		exciter_init(unit->ct, channel, sample_rate, unit->f_cut);
	}
#endif
}
/*
****************************************************************
* Exciter音效配置
*
*
****************************************************************
*/
void AudioEffectExciterConfig(ExciterUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MUSIC_EXCITER_EN
    if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		exciter_init(unit->ct, channel, sample_rate, unit->f_cut);
	}
#endif
}

/*
****************************************************************
* Aec音效初始化
*
*
****************************************************************
*/
void AudioEffectAecInit(AecUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_AEC_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{	
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + AEC_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_AEC,unit,AEC_SIZE);
			
			unit->ct = (BlueAECContext *)DynamicStartAddr;
			
			DynamicStartAddr += AEC_SIZE;

		}
		else
		{
			PrintMallocFail(EFF_AEC,unit,AEC_SIZE); 
			unit->enable = 0;
		}
        #else
		unit->ct = (BlueAECContext *)osPortMallocFromEnd(AEC_SIZE);
			
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("BlueAECContext mem malloc err! %ld\n",AEC_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		blue_aec_init(unit->ct, unit->es_level, unit->ns_level);
	}
#endif	
}
/*
****************************************************************
* Expander音效初始化
*
*
****************************************************************
*/
void AudioEffectExpanderInit(ExpanderUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MUSIC_NOISE_SUPPRESSOR_EN || CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + EXPANDER_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_EXPAND,unit,EXPANDER_SIZE);
			
			unit->ct = (ExpanderContext *)DynamicStartAddr;
			
			DynamicStartAddr += EXPANDER_SIZE;

		}
		else
		{
			PrintMallocFail(EFF_EXPAND,unit,EXPANDER_SIZE); 
			unit->enable = 0;
		}
        #else
		unit->ct = (ExpanderContext *)osPortMallocFromEnd(EXPANDER_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("ExpanderContext mem malloc err! %ld\n",AEC_SIZE);
		}
        #endif

	}
		
	if(unit->ct != NULL)
	{
		expander_init(unit->ct,  channel, sample_rate,
								 unit->threshold,
								 unit->ratio,
								 unit->attack_time,
								 unit->release_time);
	}
#endif
}
/*
****************************************************************
* Expander音效INIT参数配置
*
*
****************************************************************
*/
void AudioEffectExpanderConfig(ExpanderUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MUSIC_NOISE_SUPPRESSOR_EN || CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
    if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		expander_init(unit->ct,  channel, sample_rate,
								 unit->threshold,
								 unit->ratio,
								 unit->attack_time,
								 unit->release_time);
	}
#endif
}
/*
****************************************************************
* Expander音效配置函数
* 1，适用于实时调节的场合
*
****************************************************************
*/
void AudioEffectExpanderThresholdConfig(ExpanderUnit *unit)
{
#if CFG_AUDIO_EFFECT_MUSIC_NOISE_SUPPRESSOR_EN || CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
	if(!unit->enable)
	{
		return;
	}	
	
	if(unit->ct != NULL)
	{
		expander_set_threshold(unit->ct,  unit->threshold);
	}
#endif
}
/*
****************************************************************
* FreqShifter音效初始化
*
*
****************************************************************
*/
void AudioEffectFreqShifterInit(FreqShifterUnit *unit)
{
#if CFG_AUDIO_EFFECT_MIC_FREQ_SHIFTER_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + FREQSHIFTER_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_FREQ_SHIFTER,unit,FREQSHIFTER_SIZE);
			
			unit->ct = (FreqShifterContext *)DynamicStartAddr;
			
			DynamicStartAddr += FREQSHIFTER_SIZE;

		}
		else
		{
			PrintMallocFail(EFF_FREQ_SHIFTER,unit,FREQSHIFTER_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (FreqShifterContext *)osPortMallocFromEnd(FREQSHIFTER_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("FreqShifterContext mem malloc err! %ld\n",FREQSHIFTER_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		freqshifter_init(unit->ct,unit->deltaf);  
	}
#endif
}
/*
****************************************************************
* FreqShifter音效配置
*
*
****************************************************************
*/
void AudioEffectFreqShifterConfig(FreqShifterUnit *unit)
{
#if CFG_AUDIO_EFFECT_MIC_FREQ_SHIFTER_EN
    if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		freqshifter_init(unit->ct,unit->deltaf);  
	}
#endif
}
/*
****************************************************************
* HowlingDector音效初始化
*
*
****************************************************************
*/
void AudioEffectHowlingSuppressorInit(HowlingDectorUnit *unit)
{
#if CFG_AUDIO_EFFECT_MIC_HOWLING_DECTOR_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + HOWLING_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_HOWLING_DETOR,unit,HOWLING_SIZE);
			unit->ct = (HowlingContext *)DynamicStartAddr;
			DynamicStartAddr += HOWLING_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_HOWLING_DETOR,unit,HOWLING_SIZE);
			unit->enable = 0;
	   	}

        #else
		unit->ct = (HowlingContext *)osPortMallocFromEnd(HOWLING_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("HowlingContext mem malloc err! %ld\n",HOWLING_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		howling_suppressor_init(unit->ct, unit->suppression_mode);
	}
#endif
}
/*
****************************************************************
* HowlingDector音效配置
*
*
****************************************************************
*/
void AudioEffectHowlingSuppressorConfig(HowlingDectorUnit *unit)
{
#if CFG_AUDIO_EFFECT_MIC_HOWLING_DECTOR_EN
    if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		howling_suppressor_init(unit->ct, unit->suppression_mode);
	}
#endif
}

/*
****************************************************************
* SilenceDector音效初始化
*
*
****************************************************************
*/
void AudioEffectSilenceDectorInit(SilenceDetectorUnit *unit,uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MUSIC_SILENCE_DECTOR_EN || CFG_AUDIO_EFFECT_MIC_SILENCE_DECTOR_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

    if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + SILENCE_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_SILENCE_DETOR,unit,SILENCE_SIZE);
			unit->ct = (SilenceDetectorContext *)DynamicStartAddr;
			DynamicStartAddr += SILENCE_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_SILENCE_DETOR,unit,SILENCE_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (SilenceDetectorContext *)osPortMallocFromEnd(SILENCE_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("SilenceDetectorContext mem malloc err! %ld\n",SILENCE_SIZE);
		}	
        #endif
	}
		
	if(unit->ct != NULL)
	{
	   init_silence_detector(unit->ct, channel, sample_rate);
	}
#endif
}
/*
****************************************************************
* PitchShifter音效初始化
*
*
****************************************************************
*/
void AudioEffectPitchShifterInit(PitchShifterUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_PITCH_SHIFTER_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + PS_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_PITCH,unit,PS_SIZE);
			unit->ct = (PSContext *)DynamicStartAddr;
			DynamicStartAddr += PS_SIZE;		
		}
		else
		{
			PrintMallocFail(EFF_PITCH,unit,PS_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (PSContext *)osPortMallocFromEnd(PS_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("PSContext mem malloc err! %ld\n",PS_SIZE);
		}	
        #endif
	}
		
	if(unit->ct != NULL)
	{
		//unit->ct->w = 300;//////改善延时!!!!!!!!!!!!!
		pitch_shifter_init(unit->ct, channel, sample_rate, unit->semitone_steps, CFG_MIC_PITCH_SHIFTER_FRAME_SIZE);//512
		//gCtrlVars.SamplesPerFrame = CFG_MIC_PITCH_SHIFTER_FRAME_SIZE / 2;
	}
#endif
}
#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN
/*
****************************************************************
* PitchShifterPro音效初始化
*
*
****************************************************************
*/
void AudioEffectPitchShifterProInit(PitchShifterProUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if 0
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + PS_PRO_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_PITCH_PRO,unit,PS_PRO_SIZE);
			unit->ct = (PitchShifterProContext *)DynamicStartAddr;
			DynamicStartAddr += PS_PRO_SIZE;		
		}
		else
		{
			PrintMallocFail(EFF_PITCH_PRO,unit,PS_PRO_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (PitchShifterProContext *)osPortMallocFromEnd(PS_PRO_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("PitchShifterProContext mem malloc err! %ld\n",PS_PRO_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		pitch_shifter_pro_init(unit->ct, channel, sample_rate, unit->semitone_steps, CFG_MUSIC_PITCH_SHIFTER_PRO_FRAME_SIZE);
		gCtrlVars.SamplesPerFrame = CFG_MUSIC_PITCH_SHIFTER_PRO_FRAME_SIZE;
	}
#endif
}
#endif
/*
****************************************************************
* PitchShifter音效参数配置
* 1，适用于实时调节的场合
*
****************************************************************
*/
void AudioEffectPitchShifterConfig(PitchShifterUnit *unit)
{
#if CFG_AUDIO_EFFECT_MIC_PITCH_SHIFTER_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct != NULL)
	{
		pitch_shifter_configure(unit->ct, unit->semitone_steps);
	}
#endif
}
#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN
/*
****************************************************************
* PitchShifterPro音效参数配置
* 1，适用于实时调节的场合
*
****************************************************************
*/
void AudioEffectPitchShifterProConfig(PitchShifterProUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if 0
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct != NULL)
	{
		pitch_shifter_pro_init(unit->ct, channel, sample_rate, unit->semitone_steps, CFG_MUSIC_PITCH_SHIFTER_PRO_FRAME_SIZE);
	}
#endif
}
#endif
/*
****************************************************************
* AutoTune音效初始化
*
*
****************************************************************
*/
void AudioEffectAutoTuneInit(AutoTuneUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_AUTO_TUNE_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
    	 unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + AUTOTUNE_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_AUTO_TUNE,unit,AUTOTUNE_SIZE);
			unit->ct = (AutoTuneContext *)DynamicStartAddr;
			DynamicStartAddr += AUTOTUNE_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_AUTO_TUNE,unit,AUTOTUNE_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (AutoTuneContext *)osPortMallocFromEnd(AUTOTUNE_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("AutoTuneContext mem malloc err! %ld\n",AUTOTUNE_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		auto_tune_init(unit->ct, channel, sample_rate, unit->key);///256
	}
#endif
}

/*
****************************************************************
* AutoTune音效配置
*
*
****************************************************************
*/
void AudioEffectAutoTuneConfig(AutoTuneUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_AUTO_TUNE_EN
	if(!unit->enable)
	{
		return;
	}		
	if(unit->ct != NULL)
	{
		auto_tune_init(unit->ct, channel, sample_rate, unit->key);///256
	}
#endif
}

/*
****************************************************************
* VoiceChanger音效初始化
*
*
****************************************************************
*/
void AudioEffectVoiceChangerInit(VoiceChangerUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + VOICECHANGER_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_VOICE_CHANGE,unit,VOICECHANGER_SIZE);
			unit->ct = (VoiceChangerContext *)DynamicStartAddr;
			DynamicStartAddr += VOICECHANGER_SIZE;
			
		}
		else
		{
			PrintMallocFail(EFF_VOICE_CHANGE,unit,VOICECHANGER_SIZE);
			unit->enable = 0;
	   	}

        #else
		unit->ct = (VoiceChangerContext *)osPortMallocFromEnd(VOICECHANGER_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("VoiceChangerContext mem malloc err! %ld\n",VOICECHANGER_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		voice_changer_init(unit->ct, channel, sample_rate, unit->pitch_ratio, unit->formant_ratio);//512

	}
#endif
}
/*
****************************************************************
* VoiceChanger音效配置
*
*
****************************************************************
*/
void AudioEffectVoiceChangerConfig(VoiceChangerUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_EN
	if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		voice_changer_init(unit->ct, channel, sample_rate, unit->pitch_ratio, unit->formant_ratio);//512

	}
#endif
}
#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
/*
****************************************************************
* VoiceChangerPro音效初始化
*
*
****************************************************************
*/
void AudioEffectVoiceChangerProInit(VoiceChangerProUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + VOICECHANGER_PRO_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_VOICE_CHANGE_PRO,unit,VOICECHANGER_PRO_SIZE);
			unit->ct = (VoiceChangerProContext *)DynamicStartAddr;
			DynamicStartAddr += VOICECHANGER_PRO_SIZE;
			
		}
		else
		{
			PrintMallocFail(EFF_VOICE_CHANGE_PRO,unit,VOICECHANGER_PRO_SIZE);
			unit->enable = 0;
	   	}	
        #else
		unit->ct = (VoiceChangerProContext *)osPortMallocFromEnd(VOICECHANGER_PRO_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("VoiceChangerProContext mem malloc err! %ld\n",VOICECHANGER_PRO_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		voice_changer_pro_init(unit->ct, sample_rate, 256, unit->pitch_ratio, unit->formant_ratio);///256				
	}
}
#endif
#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
/*
****************************************************************
* VoiceChangerPro音效配置
*
*
****************************************************************
*/
void AudioEffectVoiceChangerProConfig(VoiceChangerProUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		voice_changer_pro_init(unit->ct, sample_rate, 256, unit->pitch_ratio, unit->formant_ratio);///256				
	}
}
#endif
/*
****************************************************************
* Echo音效初始化
*
*
****************************************************************
*/
void AudioEffectEchoInit(EchoUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_ECHO_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{	
		#ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + ECHO_SIZE + ECHO_S_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_ECHO,unit,ECHO_S_SIZE+ECHO_SIZE);
			unit->ct = (EchoContext *)DynamicStartAddr;
			DynamicStartAddr += ECHO_SIZE;
			unit->s_buf = (uint8_t *)DynamicStartAddr;
			DynamicStartAddr += ECHO_S_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_ECHO,unit,ECHO_S_SIZE+ECHO_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (EchoContext *)osPortMallocFromEnd(ECHO_S_SIZE+ECHO_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("EchoContext mem malloc err! %ld\n",ECHO_S_SIZE+ECHO_SIZE);
		}
		else
		{
		    unit->s_buf = (uint8_t *)unit->ct + ECHO_SIZE;
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		echo_init(unit->ct, channel, sample_rate, unit->fc, 44100, unit->s_buf);
	}
#endif
}
/*
****************************************************************
* Echo音效配置
*
*
****************************************************************
*/
void AudioEffectEchoConfig(EchoUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_ECHO_EN

	if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		echo_init(unit->ct, channel, sample_rate, unit->fc, 44100, unit->s_buf);
	}
#endif
}
/*
****************************************************************
* Reverb音效初始化
*
*
****************************************************************
*/
void AudioEffectReverbInit(ReverbUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_REVERB_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + REVERB_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_REVERB,unit,REVERB_SIZE);
			unit->ct = (ReverbContext *)DynamicStartAddr;
			DynamicStartAddr += REVERB_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_REVERB,unit,REVERB_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (ReverbContext *)osPortMallocFromEnd(REVERB_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("ReverbContext mem malloc err! %ld\n",REVERB_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		reverb_init(unit->ct, channel, sample_rate);
		reverb_configure(unit->ct, unit->dry_scale, unit->wet_scale, unit->width_scale, unit->roomsize_scale, unit->damping_scale);
	}
#endif
}
/*
****************************************************************
* Reverb音效配置函数
* 1，适用于实时调节的场合
*
****************************************************************
*/
void AudioEffectReverbConfig(ReverbUnit *unit)
{
#if CFG_AUDIO_EFFECT_MIC_REVERB_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
	//	return;
	}
	
	if(!unit->enable)
	{
		return;
	}	
	
	if(unit->ct != NULL)
	{
		reverb_configure(unit->ct, unit->dry_scale, unit->wet_scale, unit->width_scale, unit->roomsize_scale, unit->damping_scale);
	}
#endif
}
/*
****************************************************************
* PlateReverb音效初始化
*
*
****************************************************************
*/
void AudioEffectPlateReverbInit(PlateReverbUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_PLATE_REVERB_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + PLATE_REVERB_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_PLATE_REVERB,unit,PLATE_REVERB_SIZE);
			unit->ct = (PlateReverbContext *)DynamicStartAddr;
			DynamicStartAddr += PLATE_REVERB_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_PLATE_REVERB,unit,PLATE_REVERB_SIZE);
			unit->enable = 0;
	   	}
        #else
		unit->ct = (PlateReverbContext *)osPortMallocFromEnd(PLATE_REVERB_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("PlateReverbContext mem malloc err! %ld\n",PLATE_REVERB_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
	    plate_reverb_init(unit->ct, channel, sample_rate,unit->highcut_freq,unit->modulation_en);		
        plate_reverb_configure(unit->ct, unit->predelay, unit->diffusion, unit->decay, unit->damping, unit->wetdrymix);		
	}
#endif
}
/*
****************************************************************
* PlateReverb音效modulatio配置
*
*
****************************************************************
*/
void AudioEffectPlateReverbHighcutModulaConfig(PlateReverbUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MIC_PLATE_REVERB_EN
	if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
	    plate_reverb_init(unit->ct, channel, sample_rate,unit->highcut_freq,unit->modulation_en);		
	}
#endif
}
void AudioEffectPlateReverbConfig(PlateReverbUnit *unit)
{
#if CFG_AUDIO_EFFECT_MIC_PLATE_REVERB_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
	//	return;
	}
	
	if(!unit->enable)
	{
		return;
	}	
	
	if(unit->ct != NULL)
	{
		plate_reverb_configure(unit->ct, unit->predelay, unit->diffusion, unit->decay, unit->damping, unit->wetdrymix);		
	}
#endif
}
#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
/*
****************************************************************
* ReverbPro音效初始化
*
*
****************************************************************
*/
void AudioEffectReverbProInit(ReverbProUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	int32_t real_size=0,len=0;

	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel==0)
    {
    	 unit->channel = channel;
    }

	channel = unit->channel;
	
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
		if(sample_rate>44100)
		{
			DBG("Pro Reverb sample must <= 44100\n");
		}
		if(sample_rate == 44100)
		{
            #ifndef CFG_USE_OS_MALLOC
			if(DynamicStartAddr + MAX_REVERB_PRO_CONTEXT_SIZE_44100 <= DynamicEndAddr)
			{
				PrintMallocMap(EFF_REVERB_PRO,unit,REVERB_PRO_SIZE);
				unit->ct = (uint8_t *)DynamicStartAddr;
				DynamicStartAddr += MAX_REVERB_PRO_CONTEXT_SIZE_44100;

				len = MAX_REVERB_PRO_CONTEXT_SIZE_44100;
			}
			else
			{
				PrintMallocFail(EFF_REVERB_PRO,unit,REVERB_PRO_SIZE);
				unit->enable = 0;
			}
            #else
			len = MAX_REVERB_PRO_CONTEXT_SIZE_44100;
			unit->ct = (uint8_t *)osPortMallocFromEnd(REVERB_PRO_SIZE);
						
			if(unit->ct == NULL)
			{
				unit->enable = 0;
				DBG("ReverbPro 44k mem malloc err! %d\n",REVERB_PRO_SIZE);
			}
            #endif
			
		}
		else if(sample_rate == 32000)
		{
            #ifndef CFG_USE_OS_MALLOC
			if(DynamicStartAddr + MAX_REVERB_PRO_CONTEXT_SIZE_32000 <= DynamicEndAddr)
			{
				PrintMallocMap(EFF_REVERB_PRO,unit,REVERB_PRO_SIZE);
				unit->ct = (uint8_t *)DynamicStartAddr;
				DynamicStartAddr += MAX_REVERB_PRO_CONTEXT_SIZE_32000;

				len = MAX_REVERB_PRO_CONTEXT_SIZE_32000;
			}
			else
			{
				PrintMallocFail(EFF_REVERB_PRO,unit,REVERB_PRO_SIZE);
				unit->enable = 0;
			}
            #else
			len = MAX_REVERB_PRO_CONTEXT_SIZE_32000;
			unit->ct = (uint8_t *)osPortMallocFromEnd(REVERB_PRO_32K_SIZE);
						
			if(unit->ct == NULL)
			{
				unit->enable = 0;
				DBG("ReverbPro 32k mem malloc err! %u\n",REVERB_PRO_32K_SIZE);
			}
            #endif

		}
	}
		
	if(unit->ct != NULL)
	{
		reverb_pro_init(unit->ct,sample_rate,unit->dry,unit->wet,unit->erwet,unit->erfactor,
			            unit->erwidth,unit->ertolate,unit->rt60,unit->delay,unit->width,unit->wander,
			            unit->spin,unit->inputlpf,unit->damplpf,unit->basslpf,unit->bassb,unit->outputlpf);

		real_size  = reverb_pro_context_size(unit->ct);

		len = len - real_size;

		//DynamicStartAddr -= len;

	}
}
#endif
#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
/*
****************************************************************
* ReverProb音效配置函数
* 1，适用于实时调节的场合
* 2.暂时只调节干声或湿声
****************************************************************
*/
void AudioEffectReverProbConfig(ReverbProUnit *unit,uint32_t sample_rate)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
	if((!unit->enable)||(unit->ct == NULL))
	{
		return;
	}	
	
	reverb_pro_init(unit->ct,sample_rate,unit->dry,unit->wet,unit->erwet,unit->erfactor,
							unit->erwidth,unit->ertolate,unit->rt60,unit->delay,unit->width,unit->wander,
							unit->spin,unit->inputlpf,unit->damplpf,unit->basslpf,unit->bassb,unit->outputlpf);

}
#endif
/*
****************************************************************
* VocalCut音效初始化
*
*
****************************************************************
*/
void AudioEffectVocalCutInit(VocalCutUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_VOCAL_CUT_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;
	
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + VOCALCUT_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_VOCALCUT,unit,VOCALCUT_SIZE);
			unit->ct = (VocalCutContext *)DynamicStartAddr;
			DynamicStartAddr += VOCALCUT_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_VOCALCUT,unit,VOCALCUT_SIZE);
			unit->enable = 0;
   	    }
        #else
		unit->ct = (VocalCutContext *)osPortMallocFromEnd(VOCALCUT_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("VocalCutContext mem malloc err! %ld\n",VOCALCUT_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		vocal_cut_init(unit->ct, sample_rate);			
	}
#endif
}
/*
****************************************************************
* VocalRemove音效初始化
*
*
****************************************************************
*/
void AudioEffectVocalRemoveInit(VocalRemoveUnit *unit,  uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_VOCAL_REMOVE_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
	if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;
	
	if(!unit->enable)
	{
		return;
	}

	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + VOCALREMOVE_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_VOCALREMOVE,unit,VOCALREMOVE_SIZE);
			unit->ct = (VocalRemoverContext *)DynamicStartAddr;
			DynamicStartAddr += VOCALREMOVE_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_VOCALREMOVE,unit,VOCALREMOVE_SIZE);
		}
        #else
	    unit->ct = (VocalRemoverContext *)osPortMallocFromEnd(VOCALREMOVE_SIZE);
					
	    if(unit->ct == NULL)
	    {
			unit->enable = 0;
			DBG("VocalRemoverContext mem malloc err! %ld\n",VOCALREMOVE_SIZE);
	    }
        #endif
	}
		
	if(unit->ct != NULL)
	{
		vocal_remover_init(unit->ct, sample_rate,unit->lower_freq,unit->higher_freq,unit->step_size);
	}
#endif
}
/*
****************************************************************
* VocalRemove音效配置
*
*
****************************************************************
*/
void AudioEffectVocalRemoveConfig(VocalRemoveUnit *unit,  uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_VOCAL_REMOVE_EN	
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct != NULL)
	{
		vocal_remover_init(unit->ct, sample_rate,unit->lower_freq,unit->higher_freq,unit->step_size);
	}
#endif
}
/*
****************************************************************
* Chorus音效初始化
*
*
****************************************************************
*/
void AudioEffectChorusInit(ChorusUnit *unit,  uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_CHORUS_EN
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
	if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;
	
	if(!unit->enable)
	{
		return;
	}
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + CHORUS_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_CHORUS,unit,CHORUS_SIZE);
			unit->ct = (ChorusContext *)DynamicStartAddr;
			DynamicStartAddr += CHORUS_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_CHORUS,unit,CHORUS_SIZE);
		}
        #else
	    unit->ct = (ChorusContext *)osPortMallocFromEnd(CHORUS_SIZE);
					
	    if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("ChorusContext mem malloc err! %ld\n",CHORUS_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		chorus_init(unit->ct, sample_rate, unit->delay_length, unit->mod_depth, unit->mod_rate);
	}
#endif
}
#if CFG_AUDIO_EFFECT_MUSIC_3D_EN
/*
****************************************************************
* ThreeD音效初始化
*
*
****************************************************************
*/
void AudioEffectThreeDInit(ThreeDUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + THREED_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_THREED,unit,THREED_SIZE);
			unit->ct = (ThreeDContext *)DynamicStartAddr;
			DynamicStartAddr += THREED_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_THREED,unit,THREED_SIZE);
			unit->enable = 0;
   	    }
        #else
		unit->ct = (ThreeDContext *)osPortMallocFromEnd(THREED_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("ThreeDContext mem malloc err! %ld\n",THREED_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		three_d_init(unit->ct, channel,	sample_rate);	
	}
}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
/*
****************************************************************
* VB音效初始化
*
*
****************************************************************
*/
void AudioEffectVBInit(VBUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + VB_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_VB,unit,VB_SIZE);
			unit->ct = (VBContext *)DynamicStartAddr;
			DynamicStartAddr += VB_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_VB,unit,VB_SIZE);
			unit->enable = 0;
   	    }
        #else
		unit->ct = (VBContext *)osPortMallocFromEnd(VB_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("VBContext mem malloc err! %ld\n",VB_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		vb_init(unit->ct, channel,	sample_rate, unit->f_cut);	
	}
}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
/*
****************************************************************
* VB音效配置
*
*
****************************************************************
*/
void AudioEffectVBConfig(VBUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!unit->enable)
	{
		return;
	}
		
	if(unit->ct != NULL)
	{
		vb_init(unit->ct, channel,	sample_rate, unit->f_cut);	
	}
}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
/*
****************************************************************
* VBClassic音效初始化
*
*
****************************************************************
*/
void AudioEffectVBClassicInit(VBClassicUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + VBCLASSIC_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_VBCLASSIC,unit,VBCLASSIC_SIZE);
			unit->ct = (VBContext *)DynamicStartAddr;
			DynamicStartAddr += VBCLASSIC_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_VBCLASSIC,unit,VBCLASSIC_SIZE);
			unit->enable = 0;
   	    }
        #else
		unit->ct = osPortMallocFromEnd(VBCLASSIC_SIZE);//(VBContext *)
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("VBContext mem malloc err! %ld\n",VBCLASSIC_SIZE);
		}
        #endif
	}
		
	if(unit->ct != NULL)
	{
		vb_classic_init(unit->ct, channel,	sample_rate, unit->f_cut);	
	}
}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
/*
****************************************************************
* VBClassic音效配置
*
*
****************************************************************
*/
void AudioEffectVBClassicConfig(VBClassicUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct != NULL)
	{
		vb_classic_init(unit->ct, channel,	sample_rate, unit->f_cut);	
	}
}
#endif
/*
****************************************************************
* DRC音效初始化
*
*
****************************************************************
*/
void AudioEffectDRCInit(DRCUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MUSIC_DRC_EN || CFG_AUDIO_EFFECT_MIC_DRC_EN || CFG_AUDIO_EFFECT_REC_DRC_EN
	uint16_t q[2];

	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + DRC_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_DRC,unit,DRC_SIZE);
			unit->ct = (DRCContext *)DynamicStartAddr;
			DynamicStartAddr += DRC_SIZE;
		}
		else
		{
			PrintMallocFail(EFF_DRC,unit,DRC_SIZE);
			unit->enable = 0;
   	    }
        #else
		unit->ct = (DRCContext *)osPortMallocFromEnd(DRC_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("DRCContext mem malloc err! %ldu\n",DRC_SIZE);
		}
        #endif
	}
	
	if(unit->ct != NULL)
	{
		q[0] = unit->q[0];
		q[1] = unit->q[1];
		drc_init(unit->ct, channel, sample_rate, unit->fc, unit->mode, q, unit->threshold, unit->ratio,unit->attack_tc, unit->release_tc);
	}
#endif
}

/*
****************************************************************
* DRC音效配置函数
*
*
****************************************************************
*/
void AudioEffectDRCConfig(DRCUnit *unit, uint8_t channel, uint32_t sample_rate)
{
#if CFG_AUDIO_EFFECT_MUSIC_DRC_EN || CFG_AUDIO_EFFECT_MIC_DRC_EN || CFG_AUDIO_EFFECT_REC_DRC_EN
	uint16_t q[2];

	if(!unit->enable)
	{
		return;
	}	
	
	if(unit->ct != NULL)
	{
		q[0] = unit->q[0];
		q[1] = unit->q[1];
		drc_init(unit->ct, channel, sample_rate, unit->fc, unit->mode, q, unit->threshold, unit->ratio,unit->attack_tc, unit->release_tc);
	}
#endif
}
/*
****************************************************************
* EQ音效初始化
*
*
****************************************************************
*/
void AudioEffectEQInit(EQUnit *unit, uint8_t channel, uint32_t sample_rate)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		return;
	}
	
    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct == NULL)
	{
        #ifndef CFG_USE_OS_MALLOC
		if(DynamicStartAddr + EQ_SIZE <= DynamicEndAddr)
		{
		    PrintMallocMap(EFF_EQ,unit,EQ_SIZE);
			unit->ct = (EQContext *)DynamicStartAddr;
			DynamicStartAddr += EQ_SIZE;		
		}
		else
		{
			PrintMallocFail(EFF_EQ,unit,EQ_SIZE);
			unit->enable = 0;
   	    }
        #else
		unit->ct = (EQContext *)osPortMallocFromEnd(EQ_SIZE);
				
		if(unit->ct == NULL)
		{
			unit->enable = 0;
			DBG("EQContext mem malloc err! %ld\n",EQ_SIZE);
		}
        #endif
	}
	
	if(unit->ct != NULL)
	{
		eq_clear_delay_buffer(unit->ct);
		eq_init(unit->ct, sample_rate, unit->filter_params, unit->filter_count, unit->pregain, channel);
	}

}
/*
****************************************************************
* EQ预增益配置函数
*
*
****************************************************************
*/
void AudioEffectEQPregainConfig(EQUnit *unit)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		//return;
	}
	
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct != NULL)
	{
		eq_configure_pregain(unit->ct, unit->pregain);
	}
}
/*
****************************************************************
* EQ滤波器配置函数
*
*
****************************************************************
*/
void AudioEffectEQFilterConfig(EQUnit *unit, uint32_t sample_rate)
{
	if(!gCtrlVars.audio_effect_init_flag)
	{
		//return;
	}
	
	if(!unit->enable)
	{
		return;
	}
	
	if(unit->ct != NULL)
	{
		eq_configure_filters(unit->ct, sample_rate, unit->filter_params, unit->filter_count);
	}
}
/*
****************************************************************
* Pcm Delay主循环处理函数
*
*
****************************************************************
*/
#if CFG_AUDIO_EFFECT_MUSIC_DELAY_EN
void AudioEffectPcmDelayApply(PcmDelayUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		pcm_delay_apply(unit->ct, pcm_in, pcm_out, n);
	}
}
#endif
#if CFG_AUDIO_EFFECT_MIC_AEC_EN
/*
****************************************************************
* Aec主循环处理函数
*
*
****************************************************************
*/
void AudioEffectAecApply(AecUnit *unit, int16_t *u_pcm_in, int16_t *d_pcm_in, int16_t *pcm_out, uint32_t n)
{
    uint16_t fram,i,samples;

	fram = n/64;
	if(fram == 0)
	{
		samples = 64;
		fram = 1;
	}
	else
	{
		samples = 64;
	}
			
	if((unit->enable) && (unit->ct != NULL))
	{
        for(i = 0; i < fram; i++)	
    	{
			blue_aec_run(unit->ct,  (int16_t *)(u_pcm_in + i * 64), (int16_t *)(d_pcm_in + i * 64), (int16_t *)(pcm_out + i * 64));
    	}
	}
}
#endif
/*
****************************************************************
* Exciter主循环处理函数
*
*
****************************************************************
*/
void AudioEffectExciterApply(ExciterUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		exciter_apply(unit->ct, pcm_in, pcm_out, n, unit->dry, unit->wet);		
	}
}
/*
****************************************************************
* Expander主循环处理函数
*
*
****************************************************************
*/
void AudioEffectExpanderApply(ExpanderUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		expander_apply(unit->ct, pcm_in, pcm_out, n);
	}
}
/*
****************************************************************
* FreqShifter主循环处理函数
*
*
****************************************************************
*/
void AudioEffectFreqShifterApply(FreqShifterUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{	
	int16_t  s;

	switch (unit->fade_step)
	{
		case 0://normal apply
			if((unit->enable) && (unit->ct != NULL))
			{
				freqshifter_apply(unit->ct, pcm_in, pcm_out, n);
			}
			break;

		case 1://fadeout before off apply, need triggle outside
			if((unit->enable) && (unit->ct != NULL))
			{
				freqshifter_apply(unit->ct, pcm_in, pcm_out, n);
			}
			du_efft_fadeout_sw(pcm_out,n,1);
			unit->fade_step = 2;
			break;

		case 2://zero
			for(s = 0; s < n; s++)
			{
				pcm_out[s] = 0;
			}
			unit->fade_step = 3;
			break;

		case 3://fadein with no apply
			du_efft_fadein_sw(pcm_out,n,1);
			unit->fade_step = 4;
			break;

		case 4://no apply
			break;

		case 5://fadeout before on apply, , need triggle outside
			du_efft_fadeout_sw(pcm_in,n,1);
			unit->fade_step = 6;
            break;
		case 6://zero
			for(s = 0; s < n; s++)
			{
				pcm_out[s] = 0;
			}
			unit->fade_step = 7;
			break;

		case 7://fadein with apply
			if((unit->enable) && (unit->ct != NULL))
			{
				freqshifter_apply(unit->ct, pcm_in, pcm_out, n);
			}
			du_efft_fadein_sw(pcm_out,n,1);
			unit->fade_step = 0;
			break;
	}
}
/*
****************************************************************
* HowlingDector主循环处理函数
*
*
****************************************************************
*/
void AudioEffectHowlingSuppressorApply(HowlingDectorUnit *unit, int16_t *pcm_in, int16_t *pcm_out, int32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		howling_suppressor_apply(unit->ct,  pcm_in, pcm_out, n);
	}
}
/*
****************************************************************
* SilenceDector主循环处理函数
*
*
****************************************************************
*/
#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
uint32_t  Silence_Power_Off_Time = SILENCE_POWER_OFF_DELAY_TIME;
#endif
void AudioEffectSilenceDectorApply(SilenceDetectorUnit *unit, int16_t *pcm_in, int32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		unit->level = apply_silence_detector(unit->ct, pcm_in,n);
		#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
		if(unit->level > SILENCE_THRESHOLD)
			Silence_Power_Off_Time = SILENCE_POWER_OFF_DELAY_TIME;
		#endif
	}
}
/*
****************************************************************
* Phase主循环处理函数
*
*
****************************************************************
*/
void AudioEffectPhaseApply(PhaseControlUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n,  uint8_t channel)
{
	int32_t s;

    if(unit->channel == 0)
    {
    	 unit->channel = channel;
    }


	if(unit->enable && unit->phase_difference)
	{
		channel = unit->channel;

		for(s=0; s < n * channel; s++)		
		{			
			pcm_in[s] = __nds32__clips(((int32_t)pcm_in[s] * (-1)), (16)-1);		
		}	
	}
}

/*
****************************************************************
* Pregain主循环处理函数
*
*
****************************************************************
*/
void AudioEffectPregainApply(GainControlUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n,  uint8_t channel)
{
	int32_t s;
	int32_t pregain;

    if(unit->channel == 0)
    {
    	 unit->channel = channel;
    }

	if(unit->enable)
	{
		channel = unit->channel;

		pregain = unit->mute? 0 : unit->gain;

		for(s = 0; s < n; s++)
		{
			if(channel == 2)
			{
				pcm_out[2 * s + 0] = __nds32__clips((((int32_t)pcm_in[2 * s + 0] * pregain) >> 12), 16-1);
				pcm_out[2 * s + 1] = __nds32__clips((((int32_t)pcm_in[2 * s + 1] * pregain) >> 12), 16-1);
			}
			else
			{
				pcm_out[s] = __nds32__clips((((int32_t)pcm_in[s] * pregain) >> 12), 16-1);
			}
		}
	}
}
/*
****************************************************************
* PitchShifter主循环处理函数
*
*
****************************************************************
*/
void AudioEffectPitchShifterApply(PitchShifterUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n, uint8_t channel)
{

	if((unit->enable) && (unit->ct != NULL))
	{
		uint32_t PSSample = (CFG_MIC_PITCH_SHIFTER_FRAME_SIZE >> 1);
		uint32_t Cnt = n / PSSample;
		uint16_t iIdx;

		channel = unit->channel;

		for(iIdx = 0; iIdx < Cnt; iIdx++)
		{
			pitch_shifter_apply(unit->ct, (int16_t *)(pcm_in  + (PSSample * channel) * iIdx),
									  	  (int16_t *)(pcm_out + (PSSample * channel) * iIdx));
		}
	}
}
#if CFG_AUDIO_EFFECT_MUSIC_PITCH_SHIFTER_PRO_EN
/*
****************************************************************
* PitchShifterPro主循环处理函数
*
*
****************************************************************
*/
void AudioEffectPitchShifterProApply(PitchShifterProUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{

	if((unit->enable) && (unit->ct != NULL))
	{
		//pitch_shifter_pro_apply(unit->ct, pcm_in, pcm_out);
	}
}
#endif
/*
****************************************************************
* AutoTune主循环处理函数
*
*
****************************************************************
*/
void AudioEffectAutoTuneApply(AutoTuneUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		auto_tune_apply(unit->ct, pcm_in, pcm_out, unit->snap_mode,	unit->key);
	}
}
/*
****************************************************************
* VoiceChanger主循环处理函数
*
*
****************************************************************
*/
void AudioEffectVoiceChangerApply(VoiceChangerUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		voice_changer_apply(unit->ct, pcm_in, pcm_out);
	}
}
#if CFG_AUDIO_EFFECT_MIC_VOICE_CHANGER_PRO_EN
/*
****************************************************************
* VoiceChanger主循环处理函数
*
*
****************************************************************
*/
void AudioEffectVoiceChangerProApply(VoiceChangerProUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		//voice_changer_pro_apply(unit->ct, pcm_in, pcm_out);
	}
}
#endif
/*
****************************************************************
* Echo主循环处理函数
*
*
****************************************************************
*/
void AudioEffectEchoApply(EchoUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	uint32_t delay_sample;
	if((unit->enable) && (unit->ct != NULL))
	{
		delay_sample = unit->delay_samples * gCtrlVars.sample_rate / 1000;
		//对于电位器或按键调节delay参数时的杂音问题，可打开如下屏蔽的代码来完善
		//if(unit->cur_delay_samples != delay_sample)
		//{
		//	if((delay_sample > unit->cur_delay_samples) && ((delay_sample - unit->cur_delay_samples) >= 2))
		//	{
		//		unit->cur_delay_samples+=2;
		//	}
		//	else if((delay_sample < unit->cur_delay_samples) && ((unit->cur_delay_samples - delay_sample) >= 2))
		//	{
		//		unit->cur_delay_samples-=2;
		//	}
		//	echo_apply(unit->ct, pcm_in, pcm_out, n, unit->attenuation, unit->cur_delay_samples, unit->direct_sound);
		//}
		//else
		{
			echo_apply(unit->ct, pcm_in, pcm_out, n, unit->attenuation, delay_sample, unit->direct_sound);
		}
	}
}
/*
****************************************************************
* Reverb主循环处理函数
*
*
****************************************************************
*/
void AudioEffectReverbApply(ReverbUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		reverb_apply(unit->ct, pcm_in, pcm_out, n);
	}
}
/*
****************************************************************
* PlateReverb主循环处理函数
*
*
****************************************************************
*/
void AudioEffectPlateReverbApply(PlateReverbUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		plate_reverb_apply(unit->ct, pcm_in, pcm_out, n);
	}
}
#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
/*
****************************************************************
* ReverbPro主循环处理函数
*
*
****************************************************************
*/
void AudioEffectReverbProApply(ReverbProUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		//reverb_pro_apply(unit->ct, pcm_in, pcm_out, n);
	}
}
#endif
/*
****************************************************************
* VocalCut主循环处理函数
*
*
****************************************************************
*/
void AudioEffectVocalCutApply(VocalCutUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
    if(unit->vocal_cut_en == 0) return;
	
	if((unit->enable) && (unit->ct != NULL))
	{
		vocal_cut_apply(unit->ct, pcm_in, pcm_out, n,	unit->wetdrymix);
	}
}
/*
****************************************************************
* VocalRemovet主循环处理函数
*
*
****************************************************************
*/
void AudioEffectVocalRemoveApply(VocalRemoveUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
    if(unit->vocal_remove_en == 0) return;

	if((unit->enable) && (unit->ct != NULL))
	{
		//vocal_remover_apply(unit->ct, pcm_in, pcm_out);
	}
}
/*
****************************************************************
* Chorus主循环处理函数
*
*
****************************************************************
*/
void AudioEffectChorusApply(ChorusUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		chorus_apply(unit->ct, pcm_in, pcm_out, n, unit->feedback, unit->dry, unit->wet, unit->mod_rate);
	}
}
#if CFG_AUDIO_EFFECT_MUSIC_3D_EN
/*
****************************************************************
* ThreeD主循环处理函数
*
*
****************************************************************
*/
void AudioEffectThreeDApply(ThreeDUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{	
    if(unit->three_d_en == 0) return;
	if((unit->enable) && (unit->ct != NULL))
	{
		three_d_apply(unit->ct, pcm_in, pcm_out, n, unit->intensity);
	}
}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_EN
/*
****************************************************************
* VB主循环处理函数
*
*
****************************************************************
*/
void AudioEffectVBApply(VBUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{	
    if(unit->vb_en == 0) return;
	if((unit->enable) && (unit->ct != NULL))
	{
		vb_apply(unit->ct, pcm_in, pcm_out, n, unit->intensity, unit->enhanced);
	}
}
#endif
#if CFG_AUDIO_EFFECT_MUSIC_VIRTUAL_BASS_CLASSIC_EN
/*
****************************************************************
* VBClassic主循环处理函数
*
*
****************************************************************
*/
void AudioEffectVBClassicApply(VBClassicUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{	
    if(unit->vb_en == 0) return;
	if((unit->enable) && (unit->ct != NULL))
	{
		vb_classic_apply(unit->ct, pcm_in, pcm_out, n, unit->intensity);
	}
}
#endif
/*
****************************************************************
* DRC主循环处理函数
*
*
****************************************************************
*/
void AudioEffectDRCApply(DRCUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		drc_apply(unit->ct, pcm_in, pcm_out, n, unit->pregain1, unit->pregain2);
	}
}
/*
****************************************************************
* EQ主循环处理函数
*
*
****************************************************************
*/
void AudioEffectEQApply(EQUnit *unit, int16_t *pcm_in, int16_t *pcm_out, uint32_t n, uint8_t channel)
{
	uint32_t cnt = n / MAX_EQ_FRAME_SAMPLES;
	uint32_t RemainLen = n - cnt * MAX_EQ_FRAME_SAMPLES;
	uint32_t i = 0;

	if((unit->enable) && (unit->ct != NULL))
	{
		channel = unit->channel;

		for(i = 0; i < cnt; i++)
		{
			eq_apply(unit->ct, (int16_t *)(pcm_in + i * MAX_EQ_FRAME_SAMPLES * channel), (int16_t *)(pcm_out + i * MAX_EQ_FRAME_SAMPLES * channel), MAX_EQ_FRAME_SAMPLES);
		}
		if(RemainLen > 0)
		{
			eq_apply(unit->ct, (int16_t *)(pcm_in + i * MAX_EQ_FRAME_SAMPLES * channel), (int16_t *)(pcm_out + i * MAX_EQ_FRAME_SAMPLES * channel), RemainLen);
		}
	}
}
/*
****************************************************************
* 实时获取reverb相关参数，以此为电位器调节的最大值
* 只是在装载参数时，才可以获取最大值，用于调节参数
* 用户根据需要获取相应的值
****************************************************************
*/
void GetAudioEffectMaxValue(void)
{
	//if(StartWriteCmd == LOAD_BUSY)//////
	{	
		gCtrlVars.max_chorus_wet               = gCtrlVars.chorus_unit.wet;
		
	    gCtrlVars.max_plate_reverb_roomsize    = gCtrlVars.plate_reverb_unit.decay;

		gCtrlVars.max_plate_reverb_wetdrymix   = gCtrlVars.plate_reverb_unit.wetdrymix;

		gCtrlVars.max_reverb_wet_scale         = gCtrlVars.reverb_unit.wet_scale;

		gCtrlVars.max_reverb_roomsize    = gCtrlVars.reverb_unit.roomsize_scale;
		
		gCtrlVars.max_echo_delay        = gCtrlVars.echo_unit.delay_samples;
	
        gCtrlVars.max_echo_depth        = gCtrlVars.echo_unit.attenuation;
	
		//gCtrlVars.ReverbRoom            = 0;

		//gCtrlVars.max_dac0_dig_l_vol    = gCtrlVars.dac0_dig_l_vol;

		//gCtrlVars.max_dac0_dig_r_vol    = gCtrlVars.dac0_dig_r_vol;

		//gCtrlVars.max_dac1_dig_vol      = gCtrlVars.dac1_dig_vol;  
        #if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
		gCtrlVars.max_reverb_pro_wet     = gCtrlVars.reverb_pro_unit.wet ;
		gCtrlVars.max_reverb_pro_erwet   = gCtrlVars.reverb_pro_unit.erwet ;
		#endif

    }
}
/*
****************************************************************
* treb,bass调节函数
*
*
****************************************************************
*/
#ifdef CFG_FUNC_TREB_BASS_EN
void BassTrebAjust(int16_t 	BassGain,	int16_t TrebGain)
{
	gCtrlVars.mic_out_eq_unit.filter_params[2].gain = gCtrlVars.mic_out_eq_unit.eq_params[2].gain = BassTrebGainTable[BassGain]; 
	gCtrlVars.mic_out_eq_unit.filter_params[3].gain = gCtrlVars.mic_out_eq_unit.eq_params[3].gain=  BassTrebGainTable[TrebGain];
	AudioEffectEQFilterConfig(&gCtrlVars.mic_out_eq_unit, gCtrlVars.sample_rate);
}
#endif

/*************************************************
 *  混响大小调节函数
 *
 *
 ***************************************************/
#ifdef CFG_FUNC_MIC_KARAOKE_EN
void ReverbStepSet(uint8_t ReverbStep)
{
	uint16_t step,r;
	
    step = gCtrlVars.max_chorus_wet / MAX_MIC_REVB_STEP;
	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.chorus_unit.wet = gCtrlVars.max_chorus_wet;
	}
	else
	{
		gCtrlVars.chorus_unit.wet = ReverbStep * step;
	}
	step = gCtrlVars.max_plate_reverb_roomsize / MAX_MIC_REVB_STEP;
	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.plate_reverb_unit.decay = gCtrlVars.max_plate_reverb_roomsize;
	}
	else
	{
		gCtrlVars.plate_reverb_unit.decay = ReverbStep * step;
	}
	//DBG("mic_wetdrymix   = %d\n",gCtrlVars.max_plate_reverb_wetdrymix);
	step = gCtrlVars.max_plate_reverb_wetdrymix / MAX_MIC_REVB_STEP;
	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.plate_reverb_unit.wetdrymix = gCtrlVars.max_plate_reverb_wetdrymix;
	}
	else
	{
		gCtrlVars.plate_reverb_unit.wetdrymix = ReverbStep * step;
	}			
    step = gCtrlVars.max_echo_delay / MAX_MIC_REVB_STEP;
	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.echo_unit.delay_samples = gCtrlVars.max_echo_delay;
	}
	else
	{
		gCtrlVars.echo_unit.delay_samples = ReverbStep * step;
	}
	step = gCtrlVars.max_echo_depth/ MAX_MIC_REVB_STEP;
	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.echo_unit.attenuation= gCtrlVars.max_echo_depth;
	}
	else
	{
		gCtrlVars.echo_unit.attenuation = ReverbStep * step;
	}

    step = gCtrlVars.max_reverb_wet_scale/ MAX_MIC_REVB_STEP;
	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.reverb_unit.wet_scale = gCtrlVars.max_reverb_wet_scale;
	}
	else
	{
		gCtrlVars.reverb_unit.wet_scale = ReverbStep * step;
	}
    step = gCtrlVars.max_reverb_roomsize/ MAX_MIC_REVB_STEP;
	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.reverb_unit.roomsize_scale = gCtrlVars.max_reverb_roomsize;
	}
	else
	{
		gCtrlVars.reverb_unit.roomsize_scale = ReverbStep * step;
	}
#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
	//+0  ~~~ -70
    r = abs(gCtrlVars.max_reverb_pro_wet);
    r = 70-r;
    step = r / MAX_MIC_REVB_STEP;

	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.reverb_pro_unit.wet = gCtrlVars.max_reverb_pro_wet;
	}
	else
	{
		r = MAX_MIC_REVB_STEP - 1 - ReverbStep;

		r*= step;

		gCtrlVars.reverb_pro_unit.wet = gCtrlVars.max_reverb_pro_wet - r;

		if(ReverbStep == 0) gCtrlVars.reverb_pro_unit.wet = -70;
	}


    r = abs(gCtrlVars.max_reverb_pro_erwet);
    r = 70-r;
    step = r / MAX_MIC_REVB_STEP;

	if(ReverbStep >= (MAX_MIC_REVB_STEP-1))
	{
		gCtrlVars.reverb_pro_unit.erwet = gCtrlVars.max_reverb_pro_erwet;
	}
	else
	{
		r = MAX_MIC_REVB_STEP - 1 - ReverbStep;

		r*= step;

		gCtrlVars.reverb_pro_unit.erwet = gCtrlVars.max_reverb_pro_erwet - r;

		if(ReverbStep == 0) gCtrlVars.reverb_pro_unit.erwet = -70;
	}
#endif

	AudioEffectReverbConfig(&gCtrlVars.reverb_unit);
    AudioEffectPlateReverbConfig(&gCtrlVars.plate_reverb_unit);
#if CFG_AUDIO_EFFECT_MIC_REVERB_PRO_EN
	AudioEffectReverProbConfig(&gCtrlVars.reverb_pro_unit,gCtrlVars.sample_rate);
#endif
}
#endif
#else
int32_t pcm_buf_3[512];
int32_t pcm_buf_4[512];

/*
****************************************************************
* Aec音效初始化
*
*
****************************************************************
*/
void AudioEffectAecInit(AecUnit *unit, uint8_t channel, uint32_t sample_rate)
{
}

/*
****************************************************************
* 音效模块反初始化
*
*
****************************************************************
*/
void AudioEffectsDeInit(void)
{
}

/*
****************************************************************
* 音效模块初始化
*
*
****************************************************************
*/
void AudioEffectsInit(void)
{
}

/*
****************************************************************
* 无音效处理主函数
*
*
****************************************************************
*/
void AudioBypassProcess(AudioCoreContext *pAudioCore)
{
	//int16_t  s;
	int32_t pregain;
	uint16_t n = mainAppCt.SamplesPreFrame;
	int16_t *mic_pcm    	= NULL;//pBuf->mic_in;///mic input	
	int16_t *music_pcm    	= NULL;//pBuf->music_in;///music input
	int16_t *remind_in      = NULL;//pBuf->remind_in;
	int16_t *monitor_out    = NULL;//pBuf->dac0_out; 
	int16_t *record_out     = NULL;//pBuf->dacx_out; 
	int16_t *i2s0_out       = NULL;//pBuf->i2s0_out; 
	int16_t *i2s1_out       = NULL;//pBuf->i2s1_out; 
	int16_t *usb_out        = NULL;//pBuf->usb_out; 
	int16_t *local_rec_out  = NULL;//pBuf->rec_out; 
	#if (BT_HFP_SUPPORT == ENABLE)
	int16_t  *hf_mic_in     = NULL;//pBuf->hf_mic_in;//蓝牙通话mic采样buffer
	int16_t  *hf_pcm_in     = NULL;//pBuf->hf_pcm_in;//蓝牙通话下传buffer
	int16_t  *hf_aec_in		= NULL;//pBuf->hf_aec_in;;//蓝牙通话下传delay buffer,专供aec算法处理
	int16_t  *hf_pcm_out    = NULL;//pBuf->hf_pcm_out;//蓝牙通话上传buffer
	int16_t  *hf_dac_out    = NULL;//pBuf->hf_dac_out;//蓝牙通话DAC的buffer	
	int16_t  *hf_rec_out    = NULL;//pBuf->hf_rec_out;//蓝牙通话送录音的buffer	
	int16_t  *u_pcm_tmp     = (int16_t *)pcm_buf_3;
	int16_t  *d_pcm_tmp     = (int16_t *)pcm_buf_4;
	#endif

	if(pAudioCore->AudioSource[MIC_SOURCE_NUM].Enable == TRUE)////mic buff
	{
#if (BT_HFP_SUPPORT == ENABLE)
		if(GetSystemMode() == AppModeBtHfPlay)
		{
			//hf mode
			hf_mic_in = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;
		}
		else
#endif
		{
			mic_pcm = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;//双mic输入
		}
	}

#ifdef CFG_FUNC_RECORDER_EN
	if(GetSystemMode() == AppModeCardPlayBack || GetSystemMode() == AppModeUDiskPlayBack || GetSystemMode() == AppModeFlashFsPlayBack)
	{
		if(pAudioCore->AudioSource[PLAYBACK_SOURCE_NUM].Enable == TRUE)
		{
			music_pcm = pAudioCore->AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf;// include usb/sd source
		}
	}
	else
#endif
	{
		if(pAudioCore->AudioSource[APP_SOURCE_NUM].Enable == TRUE)////music buff
		{
#if (BT_HFP_SUPPORT == ENABLE) && defined(CFG_APP_BT_MODE_EN)
			if(GetSystemMode() == AppModeBtHfPlay)
			{
				//hf sco: nomo
				hf_pcm_in = pAudioCore->AudioSource[APP_SOURCE_NUM].PcmInBuf;

				//aec process:push the new data, pop the old data
				if(BtHf_AECRingDataSpaceLenGet() > CFG_BTHF_PARA_SAMPLES_PER_FRAME)
					BtHf_AECRingDataSet(hf_pcm_in, CFG_BTHF_PARA_SAMPLES_PER_FRAME);
				hf_aec_in = BtHf_AecInBuf();
			}
			else
#endif
			{
				music_pcm = pAudioCore->AudioSource[APP_SOURCE_NUM].PcmInBuf;// include line/bt/usb/sd/spdif/hdmi/i2s/radio source
			}
		}
	}	
	
#if defined(CFG_FUNC_REMIND_SOUND_EN)	
	if(pAudioCore->AudioSource[REMIND_SOURCE_NUM].Enable == TRUE)////remind buff
	{
		//audio_src_sink.music_in = 0;
		AudioCoreSourceMute(APP_SOURCE_NUM, 1, 1);
		remind_in = pAudioCore->AudioSource[REMIND_SOURCE_NUM].PcmInBuf;
	}	
#endif

    if(pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].Enable == TRUE)   ////dacx buff
	{
	#if (BT_HFP_SUPPORT == ENABLE)
		if(GetSystemMode() == AppModeBtHfPlay)
		{
			//hf mode
			hf_dac_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
			hf_pcm_out = pAudioCore->AudioSink[AUDIO_HF_SCO_SINK_NUM].PcmOutBuf;
		}
		else
	#endif
		{
			monitor_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
		}
	}
	
#ifdef CFG_RES_AUDIO_DACX_EN 	
	if(pAudioCore->AudioSink[AUDIO_DACX_SINK_NUM].Enable == TRUE)   ////dacx buff
	{
		record_out = pAudioCore->AudioSink[AUDIO_DACX_SINK_NUM].PcmOutBuf;
	}	
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN 	
	if(pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].Enable == TRUE)	////dacx buff
	{
#if (CFG_RES_I2S_PORT==1)
		i2s1_out = pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf;
#else
		i2s0_out = pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf;
#endif
	}
#endif

#ifdef CFG_FUNC_RECORDER_EN
	if(pAudioCore->AudioSink[AUDIO_RECORDER_SINK_NUM].Enable == TRUE)
	{
		local_rec_out = pAudioCore->AudioSink[AUDIO_RECORDER_SINK_NUM].PcmOutBuf;
	}
#endif

    if(monitor_out)
	{
		memset(monitor_out, 0, n * 2 * 2);
    }

    if(record_out)
    {
		memset(record_out, 0, n * 2);
    }
	
    if(usb_out)
    {
		memset(usb_out, 0, n * 2 * 2);//mono*2 stereo*4
    }
	
    if(i2s0_out)
    {
		memset(i2s0_out, 0, n * 2 * 2);//mono*2 stereo*4
	}
	
#if (BT_HFP_SUPPORT == ENABLE)
	if(hf_pcm_out)
	{
		memset(hf_pcm_out, 0, n * 2);
	}
	
	if(hf_dac_out)
	{
		memset(hf_dac_out, 0, n * 2 * 2);
	}
	
	if(hf_rec_out)
	{
		memset(hf_rec_out, 0, n * 2);
	}

	if(hf_mic_in && hf_pcm_in && hf_pcm_out && hf_dac_out)
	{
		//hf_pcm_in: 16K nomo //256*2
		//hf_mic_in: 16K stereo (需要转成nomo)  //256*2*2
		//hf_pcm_out: 16K nomo //256*2
		//hf_dac_out: 16K stereo //256*2*2
		
		//手机端通话音频送DAC 16K nomo -> stereo
		for(s = 0; s < n; s++)
		{
			hf_dac_out[s*2 + 0] = hf_pcm_in[s];
			hf_dac_out[s*2 + 1] = hf_pcm_in[s]; 
		}

        //本地MIC采样做降噪处理
		#if CFG_AUDIO_EFFECT_MIC_NOISE_SUPPRESSOR_EN
		//AudioEffectExpanderApply(&gCtrlVars.mic_expander_unit, hf_mic_in, hf_mic_in, n);
		#endif

		#if 1
		for(s = 0; s < n; s++)
		{
			hf_pcm_out[2 * s + 0] = hf_mic_in[2 * s + 0]; 
			hf_pcm_out[2 * s + 1] = hf_mic_in[2 * s + 1]; 
		}	
		#else
		//本地MIC采样及手机端通话音频做AEC处理
		//only mic input : stereo -> mono
	    for(s = 0; s < n; s++)
		{
			d_pcm_tmp[s] = __nds32__clips((((int32_t)hf_mic_in[2 * s + 0] + (int32_t)hf_mic_in[2 * s + 1])), 16-1);
			u_pcm_tmp[s] = hf_aec_in[s];
		}
	    #if CFG_AUDIO_EFFECT_MIC_AEC_EN
		AudioEffectAecApply(&gCtrlVars.mic_aec_unit, u_pcm_tmp , d_pcm_tmp, hf_pcm_out, n);
	    #else
		for(s = 0; s < n; s++)
		{
		    hf_pcm_out[s] = d_pcm_tmp[s];
		}
	    #endif

		#endif

		/*for(s = 0; s < n; s++)
		{
		    pcm   =rec_bypass_tmp[s];//
			hf_pcm_out[2 * s + 0] = pcm; 
			hf_pcm_out[2 * s + 1] = pcm; 
		}*/	
		
		//AEC处理之后的数据做变调处理
		#if CFG_AUDIO_EFFECT_MIC_PITCH_SHIFTER_EN 
		//AudioEffectPitchShifterApply(&gCtrlVars.pitch_shifter_unit, hf_pcm_out, hf_pcm_out, n, gCtrlVars.adc_mic_channel_num);
		#endif
	}
    //本地MIC采样及手机端通话音频做录音处理
	if(hf_rec_out)
	{
		for(s = 0; s < n; s++)
		{
			hf_rec_out[2*s + 0] = __nds32__clips((((int32_t)hf_mic_in[2*s + 0] + (int32_t)hf_pcm_in[2*s + 0])), 16-1);
			hf_rec_out[2*s + 1] = __nds32__clips((((int32_t)hf_mic_in[2*s + 1] + (int32_t)hf_pcm_in[2*s + 1])), 16-1);
		}
	}
#endif

	#if CFG_DAC0_EN
	//DAC0立体声监听音效处理
	if(monitor_out)
	{							
		for(s = 0; s < n; s++)
		{
			if(mic_pcm && music_pcm)
			{
				monitor_out[2*s + 0] = __nds32__clips((((int32_t)mic_pcm[2*s + 0] + (int32_t)music_pcm[2*s + 0] )), 16-1);
				monitor_out[2*s + 1] = __nds32__clips((((int32_t)mic_pcm[2*s + 1] + (int32_t)music_pcm[2*s + 1] )), 16-1);
			}
			else if(mic_pcm)
			{				
				monitor_out[2*s + 0] = mic_pcm[2*s + 0];
				monitor_out[2*s + 1] = mic_pcm[2*s + 1];
			}
			else if(music_pcm)
			{				
				monitor_out[2*s + 0] = music_pcm[2*s + 0];
				monitor_out[2*s + 1] = music_pcm[2*s + 1];
			}
			else
			{				
				monitor_out[2*s + 0] = 0;
				monitor_out[2*s + 1] = 0;
			}
		}
		
        #if defined(CFG_FUNC_REMIND_SOUND_EN)//提示音音效处理
		if(remind_in)
		{
			for(s = 0; s < n; s++)
			{
				monitor_out[2*s + 0] = __nds32__clips((((int32_t)monitor_out[2*s + 0] + (int32_t)remind_in[2*s + 0])), 16-1);
				monitor_out[2*s + 1] = __nds32__clips((((int32_t)monitor_out[2*s + 1] + (int32_t)remind_in[2*s + 1])), 16-1);
			}
		}
        #endif

		if(i2s0_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s0_out[2*s + 0] = monitor_out[2*s + 0];
				i2s0_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}
		if(i2s1_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s1_out[2*s + 0] = monitor_out[2*s + 0];
				i2s1_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}	
	}
	#endif

    #ifdef CFG_FUNC_RECORDER_EN
	if(local_rec_out)
	{
		for(s = 0; s < n; s++)
		{
			local_rec_out[2*s + 0] = monitor_out[2*s + 0];
			local_rec_out[2*s + 1] = monitor_out[2*s + 1];
		}
	}
	#endif
	
	#ifdef CFG_RES_AUDIO_DACX_EN
	//DAC X单声道录音音效处理
	if(record_out)
	{		
		if(music_pcm)
		{
			for(s = 0; s < n; s++)
			{
				record_out[s] = __nds32__clips((( (int32_t)music_pcm[2*s+0] + (int32_t)music_pcm[2*s+1])), 16-1);
									    
			}
		}
	}
	#endif

}
#endif
