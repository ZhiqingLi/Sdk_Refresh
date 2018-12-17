/**
 *******************************************************************************
 * @file    adc.h
 * @brief	SarAdc module driver interface
 
 * @author  Sam
 * @version V1.0.0
 
 * $Created: 2014-12-26 14:01:05$
 * @Copyright (C) 2014, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 ******************************************************************************* 
 */

/**
 * @addtogroup ADC
 * @{
 * @defgroup adc adc.h
 * @{
 */
 
#ifndef __ADC_H__
#define __ADC_H__

#define  AUDIO_SAMPLE_RATE_8KHZ   	8000
#define  AUDIO_SAMPLE_RATE_11KHZ   	11025
#define  AUDIO_SAMPLE_RATE_12KHZ   	12000
#define  AUDIO_SAMPLE_RATE_16KHZ   	16000
#define  AUDIO_SAMPLE_RATE_22KHZ   	22050
#define  AUDIO_SAMPLE_RATE_24KHZ   	24000
#define  AUDIO_SAMPLE_RATE_32KHZ   	32000
#define  AUDIO_SAMPLE_RATE_44KHZ   	44100
#define  AUDIO_SAMPLE_RATE_48KHZ   	48000

typedef enum __ADC_MODE
{
	DC_MODE,
	AUDIO_MMODE
	
}ADC_MODE;


typedef enum __ADC_DC_CLK_DIV
{
	CLK_DIV_NONE,
	CLK_DIV_2,
	CLK_DIV_4,
	CLK_DIV_8,
	CLK_DIV_16,
	CLK_DIV_32,
	CLK_DIV_64,
	CLK_DIV_128,
	CLK_DIV_256
	
}ADC_DC_CLK_DIV;

 
typedef enum __ADC_DC_CHANNEL_SEL
{
	ADC_CHANNEL_VDD33,
	ADC_CHANNEL_VDD_BK,
	ADC_CHANNEL_VDD12,
	ADC_CHANNEL_GPIOA3,
	ADC_CHANNEL_GPIOA4,
	ADC_CHANNEL_GPIOA5,
	ADC_CHANNEL_GPIOA6,
	ADC_CHANNEL_GPIOA9,
	ADC_CHANNEL_GPIOA10, //8
	ADC_CHANNEL_GPIOA11,
	ADC_CHANNEL_GPIOA12,
	ADC_CHANNEL_GPIOA13,
	ADC_CHANNEL_GPIOA14,
	ADC_CHANNEL_GPIOA16,
	ADC_CHANNEL_GPIOA17, //14
	ADC_CHANNEL_GPIOA18,
	ADC_CHANNEL_GPIOB0,
	ADC_CHANNEL_GPIOB1,
	ADC_CHANNEL_GPIOB2,
	ADC_CHANNEL_GPIOB3,
	ADC_CHANNEL_GPIOB24, //20
	ADC_CHANNEL_GPIOB25,
	ADC_CHANNEL_GPIOB26,
	ADC_CHANNEL_GPIOB27,
	ADC_CHANNEL_GPIOB28,
	ADC_CHANNEL_GPIOB29,
	ADC_CHANNEL_GPIOC10, //26
	ADC_CHANNEL_GPIOC11
	
}ADC_DC_CHANNEL_SEL;


typedef enum __ADC_VREF
{
	ADC_VREF_VMID, //2倍VMID
	ADC_VREF_VDDA,
	ADC_VREF_Extern //外部输入
	
}ADC_VREF;

/**
 * @brief  ADC采样使能
 * @param  无
 * @return 无
 */
void ADC_Enable(void);

/**
 * @brief  ADC采样禁止
 * @param  无
 * @return 无
 */
void ADC_Disable(void);

/**
 * @brief  ADC工作模式选择
 * @param  AdcMode 0:DC采样;    1:AC音频采样;  
 * @return NONE
 */
void ADC_ModeSet(ADC_MODE AdcMode);

/**
 * @brief  查询ADC工作模式
 * @param  无
 * @return ADC工作模式。0:DC采样;	1:AC音频采样;
 */
ADC_MODE ADC_ModeGet(void);

/**
 * @brief  ADC工作时钟分频(DC模式下有效)
 * @param  div: 分频系数
 * @return 无
 */
void ADC_DCClkDivSet(ADC_DC_CLK_DIV Div);

/**
 * @brief  ADC模拟通道配置(DC模式下有效)
 * @param  DcChannel DC通道号（请参考说明文档）
 * @return 无
 */
void ADC_DCChannelSet(ADC_DC_CHANNEL_SEL ChannelIndex);

/**
 * @brief  ADC DC采样一次转换数据完成
 * @param  无
 * @return 完成标志 1:数据转换完成； 0:数据转换未完成
 */
bool ADC_DCReady(void);

/**
 * @brief  获取ADC采样数据
 * @param  无
 * @return ADC采样数据[0-4095]
 */
uint16_t ADC_DCDataGet(void);

/**
 * @brief  获取一个ADC DC采样数据，阻塞式完成
 * @param  ChannelNum ADC DC采样通道		
 * @return ADC采样数据[0-4095]
 * @note   该函数可以由ADC_DCChannelSet(),ADC_DCReady(),ADC_DCDataGet()3个函数组合完成
 */         
uint16_t ADC_DCChannelDataGet(ADC_DC_CHANNEL_SEL ChannelIndex);
            
/**         
 * @brief  ADC清楚标志
 * @param  无
 * @return 无
 */         
void ADC_Clr(void);
            
/**         
 * @brief  ADC参考电压选择
 * @param  Mode 2:Extern Vref; 1:VDDA; 0: 2*VMID
 * @return 无
 */         
void ADC_VrefSet(ADC_VREF Mode);

/**
 * @brief  ADC采样率设置
 * @param  SampleRate 9种采样率
  * @arg	8000:8000Hz
  * @arg	11025:11025Hz
  * @arg	12000:12000Hz
  * @arg	16000:16000Hz
  * @arg	22050:22050Hz
  * @arg	24000:24000Hz
  * @arg	32000:32000Hz
  * @arg	44100:44100Hz
  * @arg	48000:48000Hz
 * @return 无
 */
void ADC_AudioSampleRateSet(uint16_t SampleRate);

/**
 * @brief  获取ADC采样率
 * @param  无
 * @return SampleRate 9种采样率
 */
uint32_t ADC_AudioSampleRateGet(void);

/**
 * @brief  ADC高通滤波器配置(音频去除DC)
 * @param  IsHpfEn 1:使能分量，去除DC; 0:禁止，不去除DC分量;
 * @return 无
 */
void ADC_AudioHighPassFilterSet(bool IsHpfEn);

/**
 * @brief  ADC数字音量配置
 * @param  Vol 数字音量[0-4095]
 * @return 无
 */
void ADC_AudioVolSet(uint16_t Vol);

/**
 * @brief  ADC模拟增益配置
 * @param  IsGainBoostEn 前端20dB放大器是否使能，TRUE，使能；FALSE：禁止
 * @param  Vol 模拟增益放大器选择，0:38dB；61:0.2dB；63:-1dB。step：-0.6dB
 * @return 无
 */
void ADC_AudioMicInGainSet(bool IsGainBoostEn, uint32_t Vol);

/**
 * @brief  ADC模拟增益电路进入低功耗，使用于低功耗模式
 * @param  无
 * @return 无
 */
void ADC_AudioMicInPoweDown(void);

/**
 * @brief  ADC淡入淡出时间配置(0dB到0的时间)
 * @param  FadeTime 时间，单位Ms
 * @return 无
 */
void ADC_FadeTimeSet(uint8_t FadeTime);

/**
 * @brief  ADC淡入淡出使能
 * @param  无
 * @return 无
 */
void ADC_FadeEnable(void);

/**
 * @brief  ADC淡入淡出禁止
 * @param  无
 * @return 无
 */
void ADC_FadeDisable(void);

/**
 * @brief  ADC静音使能
 * @param  无
 * @return 无
 */
void ADC_AudioDigitalMuteEnable(void);

/**
 * @brief  ADC静音禁能
 * @param  无
 * @return 无
 */
void ADC_AudioDigitalMuteDisable(void);


/**
 * @brief  ADC静音使能(内部有延时，等到音量为0时才推出函数)
 * @param  无
 * @return 无
 */
void ADC_AudioSoftMuteEnable(void);

/**
 * @brief  ADC静音禁能
 * @param  无
 * @return 无
 */
void ADC_AudioSoftMuteDisable(void);

/**
 * @brief  ADC Mic输入偏置电压使能
 * @param  无
 * @return 无
 */
void ADC_MicBiasEnable(void);

/**
 * @brief  ADC Mic输入偏置电压禁止
 * @param  无
 * @return 无
 */
void ADC_MicBiasDisable(void);

#endif

/**
 * @}
 * @}
 */
