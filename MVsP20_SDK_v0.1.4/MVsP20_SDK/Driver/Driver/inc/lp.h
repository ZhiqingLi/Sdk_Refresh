/**
 *******************************************************************************
 * @file    lp.h
 * @brief	Low power module driver interface
 
 * @author  Peter Zhai
 * @version V1.0.0
 
 * $Created: 2016-12-26 13:25:07$
 * @Copyright (C) 2014, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 ******************************************************************************* 
 */
 
 /**
 * @addtogroup LP
 * @{
 * @defgroup lp lp.h
 * @{
 */

#ifndef __LP_H__
#define __LP_H__

typedef enum
{
	LVD_THR_2P3V = 0,
	LVD_THR_2P4V,
	LVD_THR_2P5V,
	LVD_THR_2P6V,
	LVD_THR_2P7V,
	LVD_THR_2P8V,
	LVD_THR_2P9V,
	LVD_THR_3P0V,
}LVD_THRESHOLD;

/**
 * @brief	Set LVD threshold	
 * LVD circuit would be triggered when VDD33 is below this threshould.
 *
 * @param	Value	LVD threshold
 *
 * @arg		LVD_THR_2P3V: 2.3V
 * @arg		LVD_THR_2P4V: 2.4V
 * @arg			...
 * @arg		LVD_THR_3P0V: 3.0V
 * @return
 *   		NONE
 * @note 
 */
void LP_LVDThresholdSet(LVD_THRESHOLD Value);

/**
 * @brief	Get LVD threshold
 *
 * @param
 *					NONE
 *
 * @return
 * @arg		LVD_THR_2P3V: 2.3V
 * @arg		LVD_THR_2P4V: 2.4V
 * @arg			...
 * @arg		LVD_THR_3P0V: 3.0V
 *
 * @note
 */
LVD_THRESHOLD LP_LVDThresholdGet(void);

/**
 * @brief	Enable fast isolation for LP_SEL_VIN1 power fail event
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_SelVin1EventFastISOEnable(void);

/**
 * @brief	Disable fast isolation for LP_SEL_VIN1 power fail event
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_SelVin1EventFastISODisable(void);

/**
 * @brief	Enable fast isolation for LVD power fail event
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_LVDEventFastISOEnable(void);

/**
 * @brief	Enable fast isolation for LVD power fail event
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_LVDEventFastISODisable(void);

/**
 * @brief	Clear glitch flag
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note	Since there's no initial value for this flag bit, so clear flag first when system powerup. 
 */
void LP_GlitchFlagClear(void);

/**
 * @brief	Get glitch flag
 *
 * @param	
 *   		NONE
 * @return	
 *			0bit: 1 LVD has been detected
 *			1bit: 1 POR has been detected
 *			2bit: 1 LP_SEL_VIN1 has been detected
 *			3bit: 1 UVLO has been detected
 *   		
 * @note 
 */
uint8_t LP_GlitchFlagGet(void);

/**
 * @brief	Enable fast power down
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_FastPowerDownEnable(void);

/**
 * @brief	Enable normal power down
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_NormalPowerDownEnable(void);

/**
 * @brief	Enable glitch filter for LP_POR signal
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_PorGlitchFilterEnable(void);

/**
 * @brief	Disable glitch filter function for LP_POR signal
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_PorGlitchFilterDisable(void);

/**
 * @brief	Enable glitch filter for LVD signal
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_LVDGlitchFilterEnable(void);

/**
 * @brief	Disable glitch filter for LVD signal
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_LVDGlitchFilterDisable(void);

/**
 * @brief	Enable glitch filter for Vin_Sel1 signal
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_SelVin1GlitchFilterEnable(void);

/**
 * @brief	Disable glitch filter for Vin_Sel1 signal
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_SelVin1GlitchFilterDisable(void);

/**
 * @brief	Set width of POR glitch to be filtered
 *
 * @param	Time	unit: ms
 *   		
 * @return
 *   		NONE
 * @note 
 */
void LP_PorGlitchWidthSet(uint16_t Time);

/**
 * @brief	Set width of LVD glitch to be filtered
 *
 * @param	Time	unit: ms
 *   		
 * @return
 *   		NONE
 * @note 
 */
void LP_LVDGlitchWidthSet(uint16_t Time);

/**
 * @brief	Set width of Vin_sel glitch to be filtered
 *
 * @param	Time	unit: ms
 *   		
 * @return
 *   		NONE
 * @note 
 */
void LP_SelVin1GlitchWidthSet(uint16_t Time);

/**
 * @brief	Enable trigger power down when VIN0 < VIN1
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_SelVin1PowerDownEnable(void);

/**
 * @brief	Disable trigger power down when VIN0 < VIN1
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_SelVin1PowerDownDisable(void);

/**
 * @brief	Enable trigger power down when low voltage detected on VDD33
 *
 * @param	
 *   		NONE
 * @return
 *   		NONE
 * @note 
 */
void LP_LVDPowerDownEnable(void);

/**
 * @brief	Disable trigger power down when low voltage detected on VDD33
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void LP_LVDPowerDownDisable(void);

/**
 * @brief	 Get the cause of last power down event
 *
 * @param	
 *   		NONE
 * @return
 *   		0 : ST1_RESET                   (indicate system come from initial power on)
 *          1 : ST1_WORK_SW_PD              (caused by software trigger power down during w
 *          3 : ST1_BOOT_VIN_SWITCH_FAIL    (caused by lp_sel_vin1 during boot state)
 *          4 : ST1_PU_TIMEOUT_FAIL
 *          5 : ST1_WORK_VDD33_FAIL         (caused by lvd during work state)
 *          6 : ST1_WORK_VIN_SWITCH_FAIL    (caused by lp_sel_vin1 during work state)
 *          7 : ST1_WORK_VIN_LOW_FAIL       (caused by uvlo during work state)
 * @note 
 */
uint8_t LP_PowerDownStateGet(void);

/**
 * @brief	Set LP_TRIM_BANDGAP_CTRL value
 *
 * @param
 *   		sel   0: use efused configured trim value, after reset over
 *                1: use SREG_LP_TRIM_BANDGAP_CTRL.TRIM_BANDGAP_CFG instead. It is mainly for test purpose.
 *          cfg   Trim output voltage of reference voltage generator.
 *                4mV/step, MSB is sign bit
 * @return
 *   		NONE
 * @note
 */
void LP_TrimBandGapSet(uint8_t sel,uint8_t cfg);


#endif

/**
 * @}
 * @}
 */
