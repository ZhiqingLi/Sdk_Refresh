/**
 **************************************************************************************
 * @file    misc.h
 * @brief   
 * 
 * @author  
 * @version 
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __MISC_H__
#define __MISC_H__

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

void DetectMic(void);
void HWDeviceDected(void);
void HWDeviceDected_Init(void);
void ShunningModeProcess(void);
void SoftPowerInit(void);
void WaitSoftKey(void);
void SoftKeyPowerOff(void);
void DetectEarPhone(void);
void DetectMic3Or4Line(void);
#if 0
#include "adc.h"
//--------------------------------------------------//
void MicVolSmoothProcess(void);///only for knob
void MicVolSmoothInit(void);///only for knob
void DACVolumeSmoothProcess(void);

void Mic_Enable(AUDIO_ADC_INPUT mic_ch,bool En);
void Line_Enable(AUDIO_ADC_INPUT line_ch,bool En);

void Communication_Effect_Config(uint8_t Control, uint8_t *buf, uint32_t len);

void DetectEarPhone(void);

void DetectMic3Or4Line(void);
void DetectLineIn(void);

void MiscDeviceProcess(void);
void AudioEffectModeSel(uint8_t mode);
void DebugUartInit(void);
void Timer2Init(void);
void Communication_Effect_Init(void);
void Communication_Effect_Process(uint32_t sum_len);
uint16_t RemindSound_GetData(void* Buf, uint16_t MaxLen);
uint16_t RemindSound_GetDataLen(void);
void LedDisplay(void);
void LedDisplayOff(void);
bool AdcChannelSel(uint32_t adc_ch);
uint16_t GetAdcVal(uint32_t adc_ch);
void SaveAudioParamasToRam(uint8_t *buf, uint32_t len);///debug
void WriteEffectParamas(uint32_t mode,uint8_t *buff);
void ReadEffectParamas(uint32_t mode, uint8_t *buff);
void LoadAudioMode(uint16_t len,const uint8_t *buff);
void UsbLoadAudioMode(uint16_t len,uint8_t *buff);

bool  AudioEffectListJudge(uint16_t len, uint8_t *buff);
void UpdataParamasToSpi(uint8_t Control);
void GetAudioEffectMaxValue(void);
void EqModeSet(uint8_t EqMode);
void printf_effect_status(void);
void PrintMallocMap(uint8_t mode, void *map,uint32_t RamSize);
void PrintMallocFail(uint8_t mode, void *map,uint32_t RamSize);
void PrintAudioEffectParamaesList(uint32_t  len,uint8_t *buff);
void PrintDmaUseInfo(void);
void DmaFiFoAutoAssociate(void);
void DmaChannelAutoAssociate(void);
void PrintAudioEffectName(const uint8_t *buff);
void PrintKeyStatus(uint16_t KeyType, uint16_t KeyValue);
void PrintAnaInputError(void);
void mv_ram_free(uint32_t Unitaddr,uint8_t EffType);

void DigitalFadeProcess(int16_t *PcmIn, uint16_t n, uint32_t ChNum);
void DmaTimerCircleMode_GPIO_Init(void);
void DmaTimerCircleMode_GPIO_Config(uint32_t R_Duty, uint32_t G_Duty,  uint32_t B_Duty);
void HwPwmInit(void);
void HwPwmConfig(uint16_t Pwm_Duty);

extern const uint32_t ADC_CHANNEL_Select_Tab[16];
extern const uint32_t ADC_CHANNEL_Init_Tab[16*2];
extern const uint32_t SupportSampleRateList[13];
extern uint32_t           DmaDynamicStartAddr;

bool IsPcLink(void);
//--------------------------------------------------//

enum
{
   ADC_VIN       = BIT(ADC_CHANNEL_VIN),	/**channel VIN*/
   ADC_VDDA      = BIT(ADC_CHANNEL_VDDA),	/**channel VDDA*/
   ADC_VDD1V2    = BIT(ADC_CHANNEL_VDD1V2),	/**channel VDD1.2V*/
   ADC0_GPIOA17  = BIT(ADC_CHANNEL_GPIOA17),	/**channel 0*/
   ADC1_GPIOA18  = BIT(ADC_CHANNEL_GPIOA18),	/**channel 1*/
   ADC2_GPIOA23  = BIT(ADC_CHANNEL_GPIOA23),	/**channel 2*/
   ADC3_GPIOA24  = BIT(ADC_CHANNEL_GPIOA24),	/**channel 3*/
   ADC4_GPIOA25  = BIT(ADC_CHANNEL_GPIOA25),	/**channel 4*/
   ADC5_GPIOA26  = BIT(ADC_CHANNEL_GPIOA26),	/**channel 5*/
   ADC6_GPIOA27  = BIT(ADC_CHANNEL_GPIOA27),	/**channel 6*/
   ADC7_GPIOA28  = BIT(ADC_CHANNEL_GPIOA28),	/**channel 7*/
   ADC8_GPIOA29  = BIT(ADC_CHANNEL_GPIOA29),	/**channel 8*/
   ADC9_GPIOA30  = BIT(ADC_CHANNEL_GPIOA30),	/**channel 9*/
   ADC10_GPIOA31 = BIT(ADC_CHANNEL_GPIOA31),	/**channel 10*/
   //注意:若选用8264系列芯片，输入通路选择MIC1,MIC2,则B6,B7无法作为ADC来使用!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   ADC12_GPIOB6  = BIT(ADC_CHANNEL_GPIOB6), 	/**channel 12*/
   ADC13_GPIOB7  = BIT(ADC_CHANNEL_GPIOB7),	    /**channel 13*/
   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
};

//-------only for print audio effect ram malloc---------------//
enum
{
    EFF_AUTO_TUNE=0,
    EFF_DC_BLOCK, 
    EFF_DRC,
    EFF_ECHO,
    EFF_EQ,
    EFF_EXPAND,
    EFF_FREQ_SHIFTER,
    EFF_HOWLING_DETOR,
    EFF_NOISE_GATE,
    EFF_PITCH,
    EFF_REVERB,
    EFF_SILENCE_DETOR,
    EFF_THREED,
    EFF_VB,
    EFF_VOICE_CHANGE,
    EFF_GAIN,
    EFF_VOCALCUT,
    EFF_PLATE_REVERB,
    EFF_REVERB_PRO,
    EFF_VOICE_CHANGE_PRO,
    EFF_PHASE_CTL,
    EFF_VOCALREMOVE,
    EFF_PITCH_PRO,
    EFF_VBCLASSIC,
    EFF_PCM_DELAY,
    EFF_EXCITER,
    EFF_AEC,
    EFF_CHORUS,
	
};
//-------only Sound mode------------------------------------//
enum
{
    SOUND_SONG  = 1,
    SOUND_TALK,
    SOUND_KTV,
    SOUND_AUTO_TUNE,
    SOUND_SHUNNING,
    SOUND_MAGIC,
    SOUND_BURST,
    SOUND_VOICE_CHAGNE,
    SOUND_1,
    SOUND_2,
    SOUND_3,
    SOUND_4,
    SOUND_5,  
    SOUND_6,  
};
//------only hid procotol-don't modify------------------------------//
#define LOAD_START              0x6e
#define LOAD_END                0x6f
#define SAVE_START              0x7e
#define SAVE_END                0x7f
#define SAVE_RUNING             0x01
#define LOAD_BUSY               0x8f
#define LOAD_IDLE               0x00
//----------------------------------------//
#endif
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__CODE_KEY_H__
