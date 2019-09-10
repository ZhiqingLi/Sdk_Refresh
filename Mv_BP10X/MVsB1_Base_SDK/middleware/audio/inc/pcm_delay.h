/**
 *************************************************************************************
 * @file	pcm_delay.h
 * @brief	Delay of PCM samples
 *
 * @author	ZHAO Ying (Alfred)
 * @version	V1.1.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *
 *************************************************************************************
 */

#ifndef _PCM_DELAY_H
#define _PCM_DELAY_H

#include <stdint.h>

#define MAX_DELAY_SAMPLES 2400 // Maximum delay in samples, which is about 50ms at 48kHz, or 54ms at 44.1kHz

 /** error code for noise gate */
typedef enum _PCMDELAY_ERROR_CODE
{
    PCMDELAY_ERROR_NUMBER_OF_CHANNELS_NOT_SUPPORTED = -256,
    PCMDELAY_ERROR_INVALID_DELAY_SAMPLES,
    // No Error
    PCMDELAY_ERROR_OK = 0,					/**< no error              */
} PCMDELAY_ERROR_CODE;

typedef struct _PCMDelay
{
	int32_t num_channels;
	int32_t delay_samples;
	int32_t delay_index;
	int32_t delay_buf[MAX_DELAY_SAMPLES];
} PCMDelay;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * @brief Initialization of PCM delay unit.
 * @param ct Pointer to the PCM delay context structure. The structure's memory should be allocated by the calling process.
 * @param num_channels Number of channels. Both 1 channel and 2 channels are supported.
 * @param delay_samples Delay in samples. e.g. 40ms delay @ 44.1kHz = 40*44.1 = 1764 samples. This value should not be greater than MAX_DELAY_SAMPLES.
 * @return error code. 0 means successful, other codes indicate error.
 */
int32_t pcm_delay_init(PCMDelay* ct, int32_t num_channels, int32_t delay_samples);


/**
 * @brief Apply PCM delay to a frame of PCM data.
 * @param ct Pointer to a PCMDelay object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,...; for stereo: L0,R0,L1,R1,L2,R2,...
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,...; for stereo: L0,R0,L1,R1,L2,R2,...
 *        pcm_out can be the same as pcm_in. In this case, the PCM signals are changed in-place.
 * @param n Number of PCM samples to process.
 * @return none
 */
void pcm_delay_apply(PCMDelay* ct, int16_t *pcm_in, int16_t *pcm_out, int32_t n);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif // _PCM_DELAY_H
