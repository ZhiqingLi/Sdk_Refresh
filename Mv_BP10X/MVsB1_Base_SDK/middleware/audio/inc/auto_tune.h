/**
 *************************************************************************************
 * @file	auto_tune.h
 * @brief	Auto-Tune for 44.1kHz signals
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v1.1.0
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __AUTO_TUNE_H__
#define __AUTO_TUNE_H__

#include <stdint.h>


#define AUTOTUNE_FRAME_SIZE 256	// Frame size
#define MAX_NUM_PITCH_LAGS 300	// related to the pitch detection range, sample rate and resolution
#define MAX_PERIOD_SAMPLES 686	// ceil(48000/70)


/** error code for auto tune */
typedef enum _AUTOTUNE_ERROR_CODE
{
    AUTOTUNE_ERROR_UNSUPPORTED_NUMBER_OF_CHANNELS = -256,
	AUTOTUNE_ERROR_SAMPLE_RATE_NOT_SUPPORTED,
	AUTOTUNE_ERROR_UNKNOWN_KEY,
	// No Error
	AUTOTUNE_ERROR_OK = 0,					/**< no error              */
} AUTOTUNE_ERROR_CODE;


/** Auto Tune Context */
typedef struct _AutoTuneContext
{
	int32_t num_channels;
	int32_t sample_rate;
	int32_t frame_size;
	int32_t xlp_size;
	int32_t lp_bb;
	int32_t lp_d;	
	int32_t num_pitch_lags;
	uint32_t key;
	const int16_t *full_notes;
	int32_t num_notes;
	int32_t pitch_ref;	
	int32_t inpitch;	
	int32_t pitch_trend[3];
	int32_t pitch_index[3];
	int16_t pitch_lag_table[MAX_NUM_PITCH_LAGS];
	int32_t ac[MAX_NUM_PITCH_LAGS];
	int16_t xlp[MAX_PERIOD_SAMPLES+AUTOTUNE_FRAME_SIZE];
	int16_t xfwin[AUTOTUNE_FRAME_SIZE];
	int16_t xin[AUTOTUNE_FRAME_SIZE];
	int16_t x[MAX_PERIOD_SAMPLES+AUTOTUNE_FRAME_SIZE*2];
	int16_t y[AUTOTUNE_FRAME_SIZE*2];
	int16_t lastPCM[2];	
} AutoTuneContext;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief Initialize the Auto-Tune module.
 * @param ct Pointer to an AutoTuneContext object.
 * @param num_channels Number of channels. 
 * @param sample_rate Sample rate. Although other sample rates can be set, 44.1kHz is the only sample rate that is currently designed for.
 * @param key key selection. Supported keys (12 major scales + 1 chromatic scale): 'a' for A-flat major, 'A' for A major, 'b' for B-flat major, 'B' for B major, 'C' for C major, 'd' for D-flat major, 'D' for D major, 'e' for E-flat major, 'E' for E major, 'F' for F major, 'g' for G-flat major, 'G' for G major, 'X' for chromatic scale.
 * @return error code. AUTOTUNE_ERROR_OK means successful, other codes indicate error. 
 * @note Due to the voice property and computation limit, although 2 channels are supported, this module treats stereo signals like mono signals by mixing left and right channel first and make mono output in both left and right channels, i.e. the left channel has the same output as the right channel.
 */
int32_t auto_tune_init(AutoTuneContext *ct, int32_t num_channels, int32_t sample_rate, uint8_t key);


/**
 * @brief Apply the Auto-Tune effect.
 * @param ct Pointer to an AutoTuneContext object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,...; for stereo: L0,R0,L1,R1,L2,R2,...
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,...; for stereo: L0,R0,L1,R1,L2,R2,...
 *        pcm_out can be the same as pcm_in. In this case, the PCM is changed in-place.
 * @param snap_mode Snap mode. 'n': near snap, the voice is snapped to the nearest note; 'u': upper snap, the voice is snapped to the upper note; 'l': lower snap, the voice is snapped to the lower note.
 * @param key key selection. Supported keys (12 major scales + 1 chromatic scale): 'a' for A-flat major, 'A' for A major, 'b' for B-flat major, 'B' for B major, 'C' for C major, 'd' for D-flat major, 'D' for D major, 'e' for E-flat major, 'E' for E major, 'F' for F major, 'g' for G-flat major, 'G' for G major, 'X' for chromatic scale.
 * @return error code. AUTOTUNE_ERROR_OK means successful, other codes indicate error.
 * @note The length of PCM samples = num_channels * AUTOTUNE_FRAME_SIZE.
 */
int32_t auto_tune_apply(AutoTuneContext *ct, int16_t *pcm_in, int16_t *pcm_out, uint8_t snap_mode, uint8_t key);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__AUTO_TUNE_H__
