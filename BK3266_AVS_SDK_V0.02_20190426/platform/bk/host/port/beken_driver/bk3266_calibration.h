/**
 **************************************************************************************
 * @file    bk3266_calibration.h
 * @brief   Calibrations for BK3266, such as audio dac DC offset, charger, sar-adc, tx, etc
 * 
 * @author  Aixing.Li
 * @version V1.0.0
 *
 * &copy; 2017 BEKEN Corporation Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __BK3266_CALIBRATION_H__
#define __BK3266_CALIBRATION_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "types.h"

/**
 * @brief  Calibration debug enable
 * @param  enable calibration debug enable set, 0: disable, 1: enable
 * @return NULL
 */
void calib_dbg_enable(uint32_t enable);

/**
 * @brief  Audio DAC DC offset calibration
 * @param  type calibration parameter
 * 		   <0>: 0: differential, 1: single-end
 * 		   <1>: 0: DAC PGA chop disable, 1: DAC PGA chop enable
 * @return NULL
 */
void audio_dac_dc_offset_calibration(uint32_t type);

/**
 * @brief  Charger calibration enable
 * @param  enable calibration enable set, 0: disable, 1: enable
 * @return NULL
 */
void charger_calibration_enable(uint32_t enable);

/**
 * @brief  Charger VLCF calibration
 * @param  type calibration parameter, 0: internal hardware calibration, 1: external software calibration
 * @return NULL
 */
void charger_vlcf_calibration(uint32_t type);

/**
 * @brief  Charger ICP calibration
 * @param  type calibration parameter, 0: internal hardware calibration, 1: external software calibration
 * @return NULL
 */
void charger_icp_calibration(uint32_t type);

/**
 * @brief  Charger VCV calibration
 * @param  type calibration parameter, 0: internal hardware calibration, 1: external software calibration
 * @return NULL
 */
void charger_vcv_calibration(uint32_t type);

/**
 * @brief  SAR-ADC enable
 * @brief  channel 0:VBATTERY, others:FIXME
 * @param  enable  0:disable, 1:enable
 * @return NULL
 */
void sar_adc_enable(uint32_t channel, uint32_t enable);

/**
 * @brief  SAR-ADC read
 * @param  NULL
 * @return SAR-ADC value
 */
uint32_t sar_adc_read(void);

/**
 * @brief  SAR-ADC convert digital out to analog in
 * @param  NULL
 * @return NULL
 */
void sar_adc_dout_2_vin(uint32_t dout);

/**
 * @brief  SAR-ADC calibration
 * @param  NULL
 * @return NULL
 */
void sar_adc_calibration(void);

/**
 * @brief  TX calibration initialzation
 * @param  NULL
 * @return NULL
 */
void tx_calibration_init(void);

/**
 * @brief  TX calibration de-initialzation
 * @param  NULL
 * @return NULL
 */
void tx_calibration_deinit(void);

/**
 * @brief  TX DC calibration
 * @param  NULL
 * @return NULL
 */
void tx_dc_calibration(void);

/**
 * @brief  TX_Q_CONST_IQCAL_P calibration
 * @param  NULL
 * @return NULL
 */
void tx_q_const_iqcal_p_calibration(void);

/**
 * @brief  TX IQ gain imbalance calibration
 * @param  NULL
 * @return NULL
 */
void tx_iq_gain_imbalance_calibration(void);

/**
 * @brief  TX IQ phase imbalance calibration
 * @param  NULL
 * @return tx_ty2
 */
uint32_t tx_iq_phase_imbalance_calibration(void);

/**
 * @brief  TX output power calibration
 * @param  NULL
 * @return NULL
 */
void tx_output_power_calibration(void);

/**
 * @brief  RX RSSI calibration
 * @param  NULL
 * @return NULL
 */
void rx_rssi_calibration(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__BK3266_CALIBRATION_H__
