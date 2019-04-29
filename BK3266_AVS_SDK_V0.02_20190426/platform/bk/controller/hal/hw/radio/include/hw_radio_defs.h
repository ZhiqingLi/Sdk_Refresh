/********************************************************************************
 *
 * MODULE NAME:    hw_radio_defs.h
 * PROJECT CODE:   XpertBlue
 * DESCRIPTION:    Generic radio driver for Tabasco.
 * MAINTAINER:     Tom Kerwick
 * CREATION DATE:  30.01.14
 *
 * SOURCE CONTROL: $Id: hw_radio_defs.h,v 1.2 2014/02/03 16:10:45 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2014 Ceva Inc.
 *     All rights reserved.
 *
 * Selects appropriate hw_radio_defs.h on HW_RADIO definition - as alternative
 * to selecting these directly at makefile level (simplifies situation for IDEs).
 *
 * Note: Defs common to all radio drivers can now be migrated to this file also.
 *
 ********************************************************************************/
#ifndef HW_RADIO_DEFS
#define HW_RADIO_DEFS

#include "sys_types.h"
#include "config.h"

/*****************************************************************************
* Rohm Radio Power Level Definitions:
******************************************************************************/
#define HW_RADIO_MAX_TX_POWER_LEVEL 2 /* 3 power levels */
#define HW_RADIO_TX_POWER_STEP_SIZE 5 /* 5dBm step sizes */
#define HW_RADIO_MIN_1MBIT_TX_POWER 0
#define HW_RADIO_MIN_2MBIT_TX_POWER 0
#define HW_RADIO_MIN_3MBIT_TX_POWER 0

#define HW_RADIO_MAX_1MBIT_TX_POWER HW_RADIO_MAX_TX_POWER_LEVEL
#define HW_RADIO_MAX_2MBIT_TX_POWER HW_RADIO_MAX_TX_POWER_LEVEL
#define HW_RADIO_MAX_3MBIT_TX_POWER HW_RADIO_MAX_TX_POWER_LEVEL
/*
 * Below: Defs common to all radio drivers can now be migrated to this file also.
 */


/*****************************************************************************

*
* Generic definitions common to all radio drivers / non RFIC specific values.
*
******************************************************************************/

#define mHWradio_RX_TAB_DELAY 5
#define mHWradio_TX_TAB_DELAY 1
//#define mHWradio_RX_TAB_DELAY 3
//#define mHWradio_TX_TAB_DELAY 3
#ifdef BT_DUALMODE
#define mHWradio_RX_DELAY (0x0C)
#else
#define mHWradio_RX_DELAY (0x0B)
#endif
#define mHWradio_TX_DELAY (0x05)


#define HW_RADIO_SLOT_TIME_POS_TX_START         (0)
#define HW_RADIO_SLOT_TIME_POS_TX_MID           (312)
#define HW_RADIO_SLOT_TIME_POS_RX_START         (625)
#define HW_RADIO_SLOT_TIME_POS_RX_MID           (937)
#define HW_RADIO_TOTAL_SLOT_TIME                (1250)
#define HW_RADIO_NORMAL_TIME_FOR_SPI_WRITE      (16)

#define HW_RADIO_LE_TIFS                        (150)
#define HW_RADIO_LE_TIFS_B4_TX                 (0x0700) // LE_check = 3. Tx_next = 1.
#define HW_RADIO_LE_TIFS_B4_RX                 (0x0600) // LE_check = 3. Tx_next = 0.


/*****************************************************************************
*
* Defines to support high/low override control of GIO lines (debug purposes):
*
* HWradio_SetOverrideLow(GIO_N);
* HWradio_SetOverrideHigh(GIO_N);
*
******************************************************************************/
#define HW_RADIO_GIO_LOW_GIO_0              HAB_GIO_HIGH_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_HIGH_GIO_0             HAB_GIO_LOW_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_0    0x00008000

#define HW_RADIO_GIO_LOW_GIO_1              HAB_GIO_HIGH_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_HIGH_GIO_1             HAB_GIO_LOW_CTRL_1_AND_0_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_1    0x80000000

#define HW_RADIO_GIO_LOW_GIO_2              HAB_GIO_HIGH_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_HIGH_GIO_2             HAB_GIO_LOW_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_2    0x00008000

#define HW_RADIO_GIO_LOW_GIO_3              HAB_GIO_HIGH_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_HIGH_GIO_3             HAB_GIO_LOW_CTRL_3_AND_2_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_3    0x80000000

#define HW_RADIO_GIO_LOW_GIO_4              HAB_GIO_HIGH_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_HIGH_GIO_4             HAB_GIO_LOW_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_4    0x00008000

#define HW_RADIO_GIO_LOW_GIO_5              HAB_GIO_HIGH_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_HIGH_GIO_5             HAB_GIO_LOW_CTRL_5_AND_4_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_5    0x80000000

#define HW_RADIO_GIO_LOW_GIO_6              HAB_GIO_HIGH_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_HIGH_GIO_6             HAB_GIO_LOW_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_6    0x00008000

#define HW_RADIO_GIO_LOW_GIO_7              HAB_GIO_HIGH_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_HIGH_GIO_7             HAB_GIO_LOW_CTRL_7_AND_6_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_7    0x80000000

#define HW_RADIO_GIO_LOW_GIO_8              HAB_GIO_HIGH_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_HIGH_GIO_8             HAB_GIO_LOW_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_8    0x00008000

#define HW_RADIO_GIO_LOW_GIO_9              HAB_GIO_HIGH_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_HIGH_GIO_9             HAB_GIO_LOW_CTRL_9_AND_8_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_9    0x80000000

#define HW_RADIO_GIO_LOW_GIO_A              HAB_GIO_HIGH_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_HIGH_GIO_A             HAB_GIO_LOW_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_A    0x00008000

#define HW_RADIO_GIO_LOW_GIO_B              HAB_GIO_HIGH_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_HIGH_GIO_B             HAB_GIO_LOW_CTRL_B_AND_A_REG
#define HW_RADIO_GIO_OVERRIDE_MASK_GIO_B    0x80000000

/*****************************************************************************
* GIO override macros
******************************************************************************/
#define HWradio_SetOverrideLow(GIO_NAME)	\
				mHWreg_Logical_OR_With_Register(HW_RADIO_GIO_LOW_##GIO_NAME, \
					HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME); \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_HIGH_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME))

#define HWradio_SetOverrideHigh(GIO_NAME)	\
				mHWreg_Logical_OR_With_Register(HW_RADIO_GIO_HIGH_##GIO_NAME, \
					HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME); \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_LOW_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME))

#define HWradio_CancelOverride(GIO_NAME) \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_LOW_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME)); \
				mHWreg_Logical_AND_With_Register(HW_RADIO_GIO_HIGH_##GIO_NAME, \
					~(HW_RADIO_GIO_OVERRIDE_MASK_##GIO_NAME))

/*****************************************************************************
*
* Internal function prototypes common to all radio drivers
*
******************************************************************************/

void _HWradio_SetGios_ResetCombines(void);

void _HWhab_Config_PHY(void);
void _HWhab_Config_SYM(void);
void _HWhab_Config_COR(void);

void HWradio_Init_Tx_Power_Level(u_int8 power_level);

void _HWradio_ProgNow(u_int32 in_val);
u_int32 _HWradio_ReadNow(u_int32 Reg_To_Read);

#endif
