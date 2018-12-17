#ifndef DU361_H
#define DU361_H

#include "eq.h"

#define DU361_I2C_ADDR							    0x56			

#define DU361_VERSION                               0
#define DU361_PWR_CTL                               1
#define DU361_PATH_MODE                             2
#define DU361_SAMPLERATE                            3
#define DU361_I2S_CTL                               4
#define DU361_PGA_CTL                               5
#define DU361_PGA_LINE1_VOL                         6
#define DU361_PGA_LINE2_VOL                         7
#define DU361_PGA_MIC_VOL                           8
#define DU361_ADC_DIGITAL_VOL                       9
#define DU361_ADC_SR_ADJ                            10
#define DU361_ALC1                                  11
#define DU361_ALC2                                  12
#define DU361_ALC_NOISE_GATE                        13
#define DU361_DAC_CTL                               14
#define DU361_DAC_DIGITAL_VOL                       15
#define DU361_DAC_PGA_VOL                           16
#define DU361_DAC_SR_ADJ                            17
#define DU361_EQ_CTL                                18
#define DU361_EQ_PREGAIN                            19
#define DU361_EQ_F1_TYPE                            20
#define DU361_EQ_F1_F0                              21
#define DU361_EQ_F1_Q                               22
#define DU361_EQ_F1_GAIN                            23
#define DU361_EQ_F2_TYPE                            24
#define DU361_EQ_F2_F0                              25
#define DU361_EQ_F2_Q                               26
#define DU361_EQ_F2_GAIN                            27
#define DU361_EQ_F3_TYPE                            28
#define DU361_EQ_F3_F0                              29
#define DU361_EQ_F3_Q                               30
#define DU361_EQ_F3_GAIN                            31
#define DU361_EQ_F4_TYPE                            32
#define DU361_EQ_F4_F0                              33
#define DU361_EQ_F4_Q                               34
#define DU361_EQ_F4_GAIN                            35
#define DU361_EQ_F5_TYPE                            36
#define DU361_EQ_F5_F0                              37
#define DU361_EQ_F5_Q                               38
#define DU361_EQ_F5_GAIN                            39
#define DU361_EQ_F6_TYPE                            40
#define DU361_EQ_F6_F0                              41
#define DU361_EQ_F6_Q                               42
#define DU361_EQ_F6_GAIN                            43
#define DU361_EQ_F7_TYPE                            44
#define DU361_EQ_F7_F0                              45
#define DU361_EQ_F7_Q                               46
#define DU361_EQ_F7_GAIN                            47
#define DU361_EQ_F8_TYPE                            48
#define DU361_EQ_F8_F0                              49
#define DU361_EQ_F8_Q                               50
#define DU361_EQ_F8_GAIN                            51
#define DU361_EQ_F9_TYPE                            52
#define DU361_EQ_F9_F0                              53
#define DU361_EQ_F9_Q                               54
#define DU361_EQ_F9_GAIN                            55
#define DU361_EQ_F10_TYPE                           56
#define DU361_EQ_F10_F0                             57
#define DU361_EQ_F10_Q                              58
#define DU361_EQ_F10_GAIN                           59
#ifdef FUNC_USE_DU361_MIC_HOWLING_EN
#define DU361_RESOURCE_USAGE                        60
#define DU361_GPIO_CONFIG                           61
#define DU361_GPIO_WRITE                            62
#define DU361_GPIO_DATA                             63
#define DU361_EFFECT_HOWLING_CTRL                   64
#define DU361_EFFECT_ECHO                           65
#define DU361_EFFECT_REVERB1                        66
#define DU361_EFFECT_REVERB2                        67
#define DU361_ADC_LEVEL                             68
#define DU361_I2SIN_LEVEL                           69
#else
#define DU361_DRC1                                  60
#define DU361_DRC2                                  61
#define DU361_DRC3                               	62
#define DU361_EFFECTS_BASS1                         63
#define DU361_EFFECTS_BASS2                         64
#define DU361_EFFECTS_3D                            65
#define DU361_EFFECTS_PITCH_SHIFTER                 66
#endif

#define DU361_EQ_F_NUM0                             0x00
#define DU361_EQ_F_NUM1                             0x02
#define DU361_EQ_F_NUM2                             0x04
#define DU361_EQ_F_NUM3                             0x08
#define DU361_EQ_F_NUM4                             0x10
#define DU361_EQ_F_NUM5                             0x20
#define DU361_EQ_F_NUM6                             0x40
#define DU361_EQ_F_NUM7                             0x80
#define DU361_EQ_F_NUM8                             0x100
#define DU361_EQ_F_NUM9                             0x200
#define DU361_EQ_F_NUM10                            0x400


void DU361_init(void); 
void DU361_EqStyleConfigure(uint16_t SamplingRate, EqFilterParams* FilterParams, uint32_t FilterNum, uint32_t PreGain);
void DU361_EqPreGainSet(uint32_t PreGain);
void DU361_EqEnable(void);
void DU361_EqDisable(void);
//void DU361_EqStyleSelect(uint8_t Id);
void DU361_DrcConfig(uint16_t Drc1Para,uint16_t Drc2Para,uint16_t Drc3Para);
void DU361_EffectsBass(uint16_t EffectsBassOnOff);
void DU361_Effects3D(uint16_t Effects3DOnOff);
void DU361_EffectsPitchShifer(uint16_t EffectsPitchShiferOnOff);
#if 1//def FUNC_USE_NEW_DU361_EQ_MODE_EN
void DU361_EqModeSelect(uint8_t Id);
#endif
#ifdef FUNC_USE_DU361_MIC_HOWLING_EN
void DU361_MicMuteSet(uint8_t MuteState);
void DU361_MicVolumeSet(uint8_t MicVolume);
void DU361_EchoSet(uint8_t EchoStep);
#ifdef FUNC_USE_DU361_GPIO_EN
void DU361_SetLineDetInput(void);
uint8_t DU361_ReadLineDetState(void);
void DU361_SetBtPwrCtlOutput(void);
void DU361_SetBtPwrOn(void);
void DU361_SetBtPwrOff(void);
void DU361_SetBatDetInput(void);
uint8_t DU361_ReadBatDetState(void);
#endif
#ifdef FUNC_USE_DU361_MIC_AUTO_MUTE_EN
uint16_t DU361_ReadAdcLevel(void);
void DU361_MicAutoMuteProc(void);
#endif
#endif
#endif
