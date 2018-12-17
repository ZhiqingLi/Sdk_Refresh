/**
 *************************************************************************************
 * @file	drc.h
 * @brief	DRC (Dynamic Range Control) fixed-point implementation
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v2.0.0
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __DRC_H__
#define __DRC_H__

#include <stdint.h>


/** error code for DRC*/
typedef enum _DRC_ERROR_CODE
{
    DRC_ERROR_SAMPLE_RATE_NOT_SUPPORTED = -256,
    DRC_ERROR_NUM_CHANNELS_NOT_SUPPORTED,
	DRC_ERROR_THRESHOLD_OUT_OF_RANGE,
	DRC_ERROR_RATIO_OUT_OF_RANGE,
	// No Error
	DRC_ERROR_OK = 0,					/**< no error              */
} DRC_ERROR_CODE;

/** Crossover filter type */
typedef enum _DRC_CROSSOVER_FILTER_TYPE
{
	DRC_CROSSOVER_FILTER_NONE,	/** no crossover filters */
	DRC_CROSSOVER_FILTER_B1,	/** Butterworth filters, order = 1 */
	DRC_CROSSOVER_FILTER_LR4,	/** Linkwitz-Riley filters, order = 4 */
	DRC_CROSSOVER_FILTER_Q4,	/** Q controlled filters, order = 4 */
} DRC_CROSSOVER_FILTER_TYPE;

/** DRC context */
typedef struct _DRCContext
{
	int32_t num_channels;		// number of channels
	int32_t sample_rate;		// sample rate	
	int32_t fc;					// cut-off frequency for 2-bands mode	
	int32_t filter_type;
	int32_t filter_order;
	//int32_t knee;				// knee width in dB (0: hard knee; other values: soft knee)
	//int32_t maxdb;
	//int32_t ratio;				// compression ratio
	//int32_t attack_tc;			// attack time constant in millisecond
	//int32_t release_tc;			// release time constant in millisecond		
	int32_t slope[2];
	int32_t alpha_attack[2];
	int32_t alpha_release[2];
	int32_t threshold[2];
	int32_t state[2];
	int32_t bl[2];
	int32_t bh[2];
	int32_t a[2];
	int32_t ah[2];
	int32_t d[2][14];		// filter delays of x & y
} DRCContext;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * @brief Initialize Dynamic Range Control (DRC) module
 * @param ct Pointer to a DRCContext object.
 * @param num_channels Number of channels. 
 * @param sample_rate Sample rate.
 * @param fc Crossover frequency in Hz to seperate the full band into 2 subbands. Set 0 to apply DRC for the full band. Set other values (20 ~ 20000) to apply DRC for the 2 subbands seperatedly.
 * @param order The order of crossover filters. Either 1 or 2 is supported.
 * @param threshold A compressor reduces the level of an audio signal if its amplitude exceeds a certain threshold. Set the threshold in step of 0.01dB. Allowable range: -9000 ~ 0 to cover -90.00dB ~ 0.00dB. For example, -2550 stands for -25.50dB threshold
 * @param ratio The amount of gain reduction is determined by ratio: a ratio of 4:1 means that if input level is 4 dB over the threshold, the output signal level will be 1 dB over the threshold. The gain (level) has been reduced by 3 dB.
 * @param attack_tc Amount of time (in millisecond) it will take for the gain to decrease to a set level.
 * @param release_tc Amount of time (in millisecond) it will take for the gain to restore to 0dB.
 * @return error code. DRC_ERROR_OK means successful, other codes indicate error.
 */
int32_t init_drc(DRCContext *ct, int32_t num_channels, int32_t sample_rate, 
				 int32_t fc, DRC_CROSSOVER_FILTER_TYPE filter_type, uint16_t q[2],
				 int32_t threshold[2], int32_t ratio[2], int32_t attack_tc[2], int32_t release_tc[2]);


/**
 * @brief Apply Dynamic Range Control (DRC) to a frame of PCM data.
 * @param ct Pointer to a DRCContext object.
 * @param pcm_in Address of the PCM input. The PCM layout must be the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 * @param pcm_out Address of the PCM output. The PCM layout is the same as in Microsoft WAVE format, i.e. for mono: M0,M1,M2,... ("mono-S"); for stereo: L0,R0,L1,R1,L2,R2,... ("stereo-R+L")
 *        pcm_out can be the same as pcm_in. In this case, the PCM signals are changed in-place.
 * @param n Number of PCM samples to process.
 * @param pregain Pregain applied before DRC processing. Q4.12 format to represent value in range [0.000244140625, 8) to cover -72dB ~ 18dB. For example, 2052 represents x0.501 (-6dB), 2900 represents x0.708 (-3dB), 4096 represents x1.0 (0dB),  5786 represents x1.413 (+3dB), 16306 represents x3.981 (+12dB)
 * @return error code. DRC_ERROR_OK means successful, other codes indicate error.
 */
int32_t apply_drc(DRCContext *ct, int16_t *pcm_in, int16_t *pcm_out, int32_t n, int32_t pregain1, int32_t pregain2);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DRC_H__
