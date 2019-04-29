/******************************************************************************
 * MODULE NAME:    sys_hal_config.h
 * PROJECT CODE:   Bluetooth
 * DESCRIPTION:    HAL Configuration
 * MAINTAINER:     Tom Kerwick
 * DATE:           10 Jan 2011
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ceva Inc.
 *     All rights reserved.
 *
******************************************************************************/
#ifndef _PARTHUS_SYS_HAL_CONFIG_H
#define _PARTHUS_SYS_HAL_CONFIG_H

#include "sys_types.h"
#include "hw_radio_defs.h"

#if defined(__USE_INLINES__)
#define __INLINE__ __inline__ extern
#define PRAGMA_INLINE 0
#else
#define __INLINE__
#define PRAGMA_INLINE 0
#endif

void SYShalconfig_Initialise_EEPROM(void);
void SYShal_config_set_EDR3_feature(void);

void SYShalconfig_BD_ADDR_Was_Set(void);
void SYShalconfig_Get_Stored_BD_ADDR(void);

void SYShalconfig_Device_Class_Was_Set(void);
void SYShalconfig_Get_Stored_Device_Class(void);

void SYShalconfig_Unit_Key_Was_Set(void);
void SYShalconfig_Get_Stored_Unit_Key(void);

void SYShalconfig_Set_System_Hardware_Configuration(u_int32 info);

#define SYS_HAL_USE_RADIO_CLOCK_AS_TAB_CLOCK 1 
#define SYS_HAL_USE_RADIO_CLOCK_AS_CPU_CLOCK 0 

/* 
 * MAX_POWER_LEVEL_UNITS
 * This value indicates the number of power steps available. Ie for n power 
 * steps, there are n+1 freqs required.
 */
#define MAX_POWER_LEVEL_UNITS HW_RADIO_MAX_TX_POWER_LEVEL


//#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)

/*
 * EPC_MAX_TX_POWER_DIFFERENCE
 * The transmit power level difference between the packet headers of all supported
 * packet types at any given power step shall not exceed 10dB.
 */
#if (CONFIG_CTRL_BQB_TEST_SUPPORT == 1)
#define RF_POWER_LEVEL0   0
#define RF_POWER_LEVEL1   1
#define RF_POWER_LEVEL2   2

#if 0
#define RF_LEVEL0_VAL   0x05
#define RF_LEVEL1_VAL   0x09
#define RF_LEVEL2_VAL   0x13
#else
#define RF_LEVEL0_VAL   0x08
#define RF_LEVEL1_VAL   0x0D
#define RF_LEVEL2_VAL   0x13
#endif
#endif

#define EPC_MAX_TX_POWER_DIFFERENCE 10

#define MIN_POWER_LEVEL_1MBITS HW_RADIO_MIN_1MBIT_TX_POWER
#define MIN_POWER_LEVEL_2MBITS HW_RADIO_MIN_2MBIT_TX_POWER
#define MIN_POWER_LEVEL_3MBITS HW_RADIO_MIN_3MBIT_TX_POWER

#define MAX_POWER_LEVEL_1MBITS HW_RADIO_MAX_1MBIT_TX_POWER
#define MAX_POWER_LEVEL_2MBITS HW_RADIO_MAX_2MBIT_TX_POWER
#define MAX_POWER_LEVEL_3MBITS HW_RADIO_MAX_3MBIT_TX_POWER

#define TX_POWER_STEP_SIZE HW_RADIO_TX_POWER_STEP_SIZE

//#endif // PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED
extern const s_int8 Power_control_table_dBm[];
#define TX_POWER_LEVEL_Pmax 0//(Power_control_table_dBm[MAX_POWER_LEVEL_UNITS])
#define TX_POWER_LEVEL_Pmin 0//(Power_control_table_dBm[0])

#ifdef CONFIG_EN_AFH_FUNC
#define GOLDEN_RECEIVER_RSSI_MIN -75//-56
#define GOLDEN_RECEIVER_RSSI_MAX -68//-30
#else
#define GOLDEN_RECEIVER_RSSI_MIN -56
#define GOLDEN_RECEIVER_RSSI_MAX -30
#endif

#define SYS_LF_OSCILLATOR_PRESENT 1


#endif /* _PARTHUS_SYS_HAL_CONFIG_H */

