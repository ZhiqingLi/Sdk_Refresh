#ifndef __SPEECH_ARM_BIQUAD_CASCADE_DF1_F32_H__
#define __SPEECH_ARM_BIQUAD_CASCADE_DF1_F32_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Instance structure for the floating-point Biquad cascade filter.
*/
typedef struct
{
    uint32_t numStages;         /**< number of 2nd order stages in the filter.  Overall order is 2*numStages. */
    float   *pState;          /**< Points to the array of state coefficients.  The array is of length 4*numStages. */
    float   *pCoeffs;         /**< Points to the array of coefficients.  The array is of length 5*numStages. */
} speech_arm_biquad_casd_df1_inst_f32;

/**
* @brief  Initialization function for the floating-point Biquad cascade filter.
* @param[in,out] *S           points to an instance of the floating-point Biquad cascade structure.
* @param[in]     numStages    number of 2nd order stages in the filter.
* @param[in]     *pCoeffs     points to the filter coefficients.
* @param[in]     *pState      points to the state buffer.
* @return        none
*/

void speech_arm_biquad_cascade_df1_init_f32(
    speech_arm_biquad_casd_df1_inst_f32 * S,
    uint8_t numStages,
    float * pCoeffs,
    float * pState);

/**
* @brief Processing function for the floating-point Biquad cascade filter.
* @param[in]  *S         points to an instance of the floating-point Biquad cascade structure.
* @param[in]  *pSrc      points to the block of input data.
* @param[out] *pDst      points to the block of output data.
* @param[in]  blockSize  number of samples to process.
* @return     none.
*/

void speech_arm_biquad_cascade_df1_f32(
    const speech_arm_biquad_casd_df1_inst_f32 * S,
    int16_t * pSrc,
    int16_t * pDst,
    uint32_t blockSize);

#ifdef __cplusplus
}
#endif

#endif
