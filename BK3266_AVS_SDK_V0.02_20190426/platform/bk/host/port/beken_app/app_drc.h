/**
 * @file   drc.c
 * @author Aixing.Li
 *
 * @brief  A fixed-point implementation for Dynamic Range Compression
 *
 * (c) 2018 BEKEN Corporation, Ltd. All Rights Reserved
 */

#ifndef __DRC_H__
#define __DRC_H__

#include "types.h"
#include "config.h"

#if (CONFIG_DRC == 1)

enum DRC_CTRL_CMD
{
    DRC_CTRL_CMD_NULL = 0,
    DRC_CTRL_CMD_GAIN_MIN_SET,              /**< arg: -60 ~ +24 dB */
    DRC_CTRL_CMD_GAIN_MAX_SET,              /**< arg: -60 ~ +24 dB */
    DRC_CTRL_CMD_GAIN_ATTACK_SET,           /**< arg: 0 ~ 1, Q1.15 */
    DRC_CTRL_CMD_GAIN_DECAY_SET,            /**< arg: 0 ~ 1, Q1.15 */
    DRC_CTRL_CMD_GAIN_RELEASE_SET = DRC_CTRL_CMD_GAIN_DECAY_SET,
    DRC_CTRL_CMD_GATING_DECAY_SET,          /**< arg: 0 ~ 1, Q1.15 */
    DRC_CTRL_CMD_PEAK_DECAY_SET,            /**< arg: 0 ~ 1, Q1.15 */
    DRC_CTRL_CMD_MAX_SAMPLE_SET,            /**< arg: 0 ~ 32767    */
    DRC_CTRL_CMD_PEAK_THRESHOLD_SET,        /**< arg: 0 ~ 32767    */
    DRC_CTRL_CMD_RSSI_THRESHOLD_HIGH_SET,   /**< arg: 0 ~ 32767    */
    DRC_CTRL_CMD_RSSI_THRESHOLD_LOW_SET,    /**< arg: 0 ~ 32767    */
    DRC_CTRL_CMD_RSSI_THRESHOLD_GATING_SET, /**< arg: 0 ~ 32767    */
    DRC_CTRL_CMD_RSSI_ATTACK_SET,			/**< arg: 0 ~ 1, Q1.15 */
    DRC_CTRL_CMD_RSSI_DECAY_SET,			/**< arg: 0 ~ 1, Q1.15 */
    DRC_CRTL_CMD_RMS_DETECT_TIME_SET		/**< arg: 0 ~ 1000 ms  */
};

#define __PACKED_POST__  __attribute__((packed))

typedef struct _DRCContext
{
	uint8_t  init;
	uint8_t  flag;
	uint8_t  reserved[2];
	uint16_t pre_vol;
	uint16_t post_vol;

    uint32_t rssi;
	uint16_t rssi_attack;
	uint16_t rssi_decay;

	uint32_t rms_value;
	uint32_t rms_detect_cnt;
	uint32_t rms_detect_time;

    uint16_t gain;
    uint16_t gain_min;
    uint16_t gain_max;
    uint16_t channels;        /**< number of PCM channels    */

    uint32_t sample_rate;

    uint16_t _gain_attack;    /**< equal to 1 + gain_attack  */
    uint16_t _gain_decay;     /**< equal to 1 - gain_decay   */

    uint16_t _gating_decay;   /**< equal to 1 - gating_decay */
    uint16_t _peak_decay;     /**< equal to 1 - peak_decay   */

    uint32_t max_sample;
    uint32_t peak_threshold;

    uint32_t rssi_threshold_high;
    uint32_t rssi_threshold_low;
    uint32_t rssi_threshold_gating;

}__PACKED_POST__ DRCContext;

/**
 * @brief  DRC initialize
 * @param  drc DRC struct pointer
 * @param  sample_rate sample rate in Hz
 * @param  channels    number of input PCM channels, MUST be 1 (MONO) or 2 (STEREO)
 * @return NULL
 */
void drc_init(DRCContext* drc, uint32_t sample_rate, uint32_t channels);

/**
 * @brief  DRC control
 * @param  drc DRC struct pointer
 * @param  cmd control command, @see DRC_CTRL_CMD
 * @param  cmd control argument
 * @return NULL
 */
void drc_ctrl(DRCContext* drc, uint32_t cmd, uint32_t arg);

/**
 * @brief  DRC process
 * @param  drc      DRC struct pointer
 * @param  pcm_in   input pcm data pointer.  The PCM data layout is M0.M1.M2... for mono and L0.R0.L1.R1.L2.R2... for stereo
 * @param  pcm_out  output pcm data pointer. The PCM data layout is M0.M1.M2... for mono and L0.R0.L1.R1.L2.R2... for stereo
 *                  The pcm_out can be set same as pcm_in.
 * @param  samples  number of PCM samples to be processed.
 * @return NULL
 */
void drc_apply(DRCContext* drc, int16_t* pcm_in, int16_t* pcm_out, uint32_t samples);

typedef struct _app_drc_para_s
{
	uint8 flag;
	uint16 pre_vol;
	uint16 post_vol;
	uint16 rms_time;
	uint16 threshold;
	uint16 attack_time;
	uint16 release_time;
}__PACKED_POST__ app_drc_para_t;
void app_set_drc_para(uint8 *para);
void app_show_drc_para(void);
#endif
#endif//__DRC_H__
