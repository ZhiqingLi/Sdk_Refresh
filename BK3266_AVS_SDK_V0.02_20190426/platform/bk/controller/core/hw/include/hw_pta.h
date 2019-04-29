/*
 * MODULE NAME:    hw_pta.h
 * PROJECT CODE:   Bluestream BT3.0
 * DESCRIPTION:    PTA Interface Driver
 * MAINTAINER:     Tom Kerwick
 * DATE:           18 October 2010
 *
 * SOURCE CONTROL: $Id: hw_pta.h,v 1.15 2014/06/26 17:39:35 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2010 Ceva Inc.
 *     All rights reserved.
 *
 */

#include "sys_config.h"
#include "hw_macro_defs.h"
#include "hw_radio.h"

#include <limits.h> // CHAR_BIT - Defines the number of bits in a byte - typically 8, but can be 16 (DSPs).

#if (PRH_BS_CFG_SYS_PTA_DRIVER_SUPPORTED==1)

boolean HWpta_Initialise(void);
void HWpta_Return_To_Standby(void);
boolean HWpta_Set_Bt_Req_Lead_Time(u_int16 lead_time);

boolean HWpta_Control_Enabled(boolean enabled);
boolean HWpta_Test_Control_Enabled(boolean enabled);

void HWpta_Tx_Access_Request(boolean priority, boolean freq_overlap, t_role role);
void HWpta_Rx_Access_Request(boolean priority, boolean freq_overlap, t_role role);

void HWpta_Service_Tx_Start(void);
void HWpta_Service_Rx_Start(void);

void HWpta_Service_PKA(void);

#define HWpta_Service_PKD() HWpta_Rx_Access_Complete()
#define HWpta_Service_NoPKD() HWpta_Rx_Access_Complete()

RAM_CODE void HWpta_Rx_Access_Complete(void);
void HWpta_Tx_Access_Complete(void);

void HWpta_Generic_BT_Access_Mode(boolean enabled);
void HWpta_Combine_BT_RX_REQ(boolean enabled);

void HWpta_LE_Access_Request(boolean priority, boolean freq_overlap, t_RadioMode rf_mode);
void HWpta_LE_Access_Complete(void);

void HWpta_Extended_Access_Request(boolean priority, boolean freq_overlap,
    		  				u_int16 number_of_slots);

void HWpta_WinExt_Adjust_En(void);
void HWpta_WinExt_Adjust_Dis(void);


/*
Initially agreed to use PTA_GRANT_RX_DENIED use address 53 bit 6 in the receive status registers,
 and PTA_GRANT_TX_DENIED use address 40 bit 0 in the transmit status registers. However, the EDR status
 registers are in a different location. Following defined in hw_jal_defs.h/hw_lc_impl.h:

#define JAL_PTA_GRANT_TX_DENIED_ADDR   (0x00000734 + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_TX_DENIED_MASK   0x00000001
#define JAL_PTA_GRANT_TX_DENIED_SHFT   0

#define JAL_PTA_GRANT_RX_DENIED_ADDR   (0x00000734 + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_RX_DENIED_MASK   0x00000002
#define JAL_PTA_GRANT_RX_DENIED_SHFT   1

__INLINE__ u_int HW_get_pta_grant_rx_denied(void) { return mGetHWEntry(JAL_PTA_GRANT_RX_DENIED);}
__INLINE__ u_int HW_get_pta_grant_tx_denied(void) { return mGetHWEntry(JAL_PTA_GRANT_TX_DENIED);}

*/

/*
For convenient enable/disable of PTA mode and PTA GRANT test, two additional registers are defined
in the tabasco registers. These are as follows:
 1.	pta_mode_enable use address 738 bit 0. 
2.	pta_grant_test_enable use address 738 bit 1.

#define JAL_PTA_MODE_ENABLE_ADDR   (0x00000738 + JAL_BASE_ADDR)
#define JAL_PTA_MODE_ENABLE_MASK   0x00000001
#define JAL_PTA_MODE_ENABLE_SHFT   0

#define JAL_PTA_GRANT_TEST_ENABLE_ADDR   (0x00000738 + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_TEST_ENABLE_MASK   0x00000002
#define JAL_PTA_GRANT_TEST_ENABLE_SHFT   1

__INLINE__ void HW_set_pta_mode_enable(const u_int mode) { mSetHWEntry(JAL_PTA_MODE_ENABLE);}
__INLINE__ void HW_set_pta_grant_test_enable(const u_int mode) { mSetHWEntry(JAL_PTA_GRANT_TEST_ENABLE);}

*/

/*
 * Tabasco Base Addresses:
 * TAB_BASE_ADDR = Platform dependant (0x03000000 on Chimera).
 * JAL_BASE_ADDR = TAB_BASE_ADDR + 0x6000
 * HAB_BASE_ADDR = TAB_BASE_ADDR + 0x8000
 * PTA_BASE_ADDR = TAB_BASE_ADDR + 0x9000
 */

#if defined(TEAKLITE4) && !defined(TEAKLITE4_IO_BB_PORT)
#include <limits.h> // CHAR_BIT - Defines the number of bits in a byte - typically 8, but can be 16 (DSPs).
#define ADDR_DIV	2 //(CHAR_BIT/8)
#else
#define ADDR_DIV	1
#endif

#ifndef PTA_BASE_ADDR
#define PTA_BASE_ADDR            HAB_BASE_ADDR + 0x1000/ADDR_DIV
#endif

#define HAB_GIO_HIGH_12_ADDR     0x000000B0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_12_MASK     0x0000FFFF
#define HAB_GIO_HIGH_12_SHFT     0

#define HAB_GIO_HIGH_13_ADDR     0x000000B0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_13_MASK     0xFFFF0000
#define HAB_GIO_HIGH_13_SHFT     16

#define HAB_GIO_HIGH_14_ADDR     0x000000B4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_14_MASK     0x0000FFFF
#define HAB_GIO_HIGH_14_SHFT     0

#define HAB_GIO_HIGH_15_ADDR     0x000000B4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_15_MASK     0xFFFF0000
#define HAB_GIO_HIGH_15_SHFT     16

#define HAB_GIO_HIGH_16_ADDR     0x000000B8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_16_MASK     0x0000FFFF
#define HAB_GIO_HIGH_16_SHFT     0

#define HAB_GIO_HIGH_17_ADDR     0x000000B8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_17_MASK     0xFFFF0000
#define HAB_GIO_HIGH_17_SHFT     16

#define HAB_GIO_HIGH_18_ADDR     0x000000BC/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_18_MASK     0x0000FFFF
#define HAB_GIO_HIGH_18_SHFT     0

#define HAB_GIO_HIGH_19_ADDR     0x000000BC/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_19_MASK     0xFFFF0000
#define HAB_GIO_HIGH_19_SHFT     16

#define HAB_GIO_HIGH_20_ADDR     0x000000C0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_20_MASK     0x0000FFFF
#define HAB_GIO_HIGH_20_SHFT     0

#define HAB_GIO_HIGH_21_ADDR     0x000000C0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_21_MASK     0xFFFF0000
#define HAB_GIO_HIGH_21_SHFT     16

#define HAB_GIO_HIGH_22_ADDR     0x000000C4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_22_MASK     0x0000FFFF
#define HAB_GIO_HIGH_22_SHFT     0

#define HAB_GIO_HIGH_23_ADDR     0x000000C4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_HIGH_23_MASK     0xFFFF0000
#define HAB_GIO_HIGH_23_SHFT     16


#define HAB_GIO_LOW_12_ADDR     0x000000C8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_12_MASK     0x0000FFFF
#define HAB_GIO_LOW_12_SHFT     0

#define HAB_GIO_LOW_13_ADDR     0x000000C8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_13_MASK     0xFFFF0000
#define HAB_GIO_LOW_13_SHFT     16

#define HAB_GIO_LOW_14_ADDR     0x000000CC/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_14_MASK     0x0000FFFF
#define HAB_GIO_LOW_14_SHFT     0

#define HAB_GIO_LOW_15_ADDR     0x000000CC/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_15_MASK     0xFFFF0000
#define HAB_GIO_LOW_15_SHFT     16

#define HAB_GIO_LOW_16_ADDR     0x000000D0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_16_MASK     0x0000FFFF
#define HAB_GIO_LOW_16_SHFT     0

#define HAB_GIO_LOW_17_ADDR     0x000000D0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_17_MASK     0xFFFF0000
#define HAB_GIO_LOW_17_SHFT     16

#define HAB_GIO_LOW_18_ADDR     0x000000D4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_18_MASK     0x0000FFFF
#define HAB_GIO_LOW_18_SHFT     0

#define HAB_GIO_LOW_19_ADDR     0x000000D4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_19_MASK     0xFFFF0000
#define HAB_GIO_LOW_19_SHFT     16

#define HAB_GIO_LOW_20_ADDR     0x000000D8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_20_MASK     0x0000FFFF
#define HAB_GIO_LOW_20_SHFT     0

#define HAB_GIO_LOW_21_ADDR     0x000000D8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_21_MASK     0xFFFF0000
#define HAB_GIO_LOW_21_SHFT     16

#define HAB_GIO_LOW_22_ADDR     0x000000DC/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_22_MASK     0x0000FFFF
#define HAB_GIO_LOW_22_SHFT     0

#define HAB_GIO_LOW_23_ADDR     0x000000DC/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_LOW_23_MASK     0xFFFF0000
#define HAB_GIO_LOW_23_SHFT     16


#define HAB_GIO_COMBINE_12_ADDR     0x000000E0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_12_MASK     0x000000FF
#define HAB_GIO_COMBINE_12_SHFT     0

#define HAB_GIO_COMBINE_13_ADDR     0x000000E0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_13_MASK     0x0000FF00
#define HAB_GIO_COMBINE_13_SHFT     8

#define HAB_GIO_COMBINE_14_ADDR     0x000000E0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_14_MASK     0x00FF0000
#define HAB_GIO_COMBINE_14_SHFT     16

#define HAB_GIO_COMBINE_15_ADDR     0x000000E0/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_15_MASK     0xFF000000
#define HAB_GIO_COMBINE_15_SHFT     24

#define HAB_GIO_COMBINE_16_ADDR     0x000000E4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_16_MASK     0x000000FF
#define HAB_GIO_COMBINE_16_SHFT     0

#define HAB_GIO_COMBINE_17_ADDR     0x000000E4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_17_MASK     0x0000FF00
#define HAB_GIO_COMBINE_17_SHFT     8

#define HAB_GIO_COMBINE_18_ADDR     0x000000E4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_18_MASK     0x00FF0000
#define HAB_GIO_COMBINE_18_SHFT     16

#define HAB_GIO_COMBINE_19_ADDR     0x000000E4/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_19_MASK     0xFF000000
#define HAB_GIO_COMBINE_19_SHFT     24

#define HAB_GIO_COMBINE_20_ADDR     0x000000E8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_20_MASK     0x000000FF
#define HAB_GIO_COMBINE_20_SHFT     0

#define HAB_GIO_COMBINE_21_ADDR     0x000000E8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_21_MASK     0x0000FF00
#define HAB_GIO_COMBINE_21_SHFT     8

#define HAB_GIO_COMBINE_22_ADDR     0x000000E/ADDR_DIV8 + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_22_MASK     0x00FF0000
#define HAB_GIO_COMBINE_22_SHFT     16

#define HAB_GIO_COMBINE_23_ADDR     0x000000E8/ADDR_DIV + PTA_BASE_ADDR
#define HAB_GIO_COMBINE_23_MASK     0xFF000000
#define HAB_GIO_COMBINE_23_SHFT     24

/***************************************************************************
 *
 * Enable Masks WIN_EXT GIO High/Low Control
 *
 ***************************************************************************/

#define PTA_GIO_HIGH_EARLY_EN_ADDR   0x000000F0/ADDR_DIV + PTA_BASE_ADDR
#define PTA_GIO_HIGH_EARLY_EN_MASK   0x00000FFF
#define PTA_GIO_HIGH_EARLY_EN_SHFT   0

#define PTA_GIO_HIGH_LATE_EN_ADDR   0x000000F0/ADDR_DIV + PTA_BASE_ADDR
#define PTA_GIO_HIGH_LATE_EN_MASK   0x00FFF000
#define PTA_GIO_HIGH_LATE_EN_SHFT   12

#define PTA_GIO_LOW_EARLY_EN_ADDR   0x000000F4/ADDR_DIV + PTA_BASE_ADDR
#define PTA_GIO_LOW_EARLY_EN_MASK   0x00000FFF
#define PTA_GIO_LOW_EARLY_EN_SHFT   0

#define PTA_GIO_LOW_LATE_EN_ADDR   0x000000F4/ADDR_DIV + PTA_BASE_ADDR
#define PTA_GIO_LOW_LATE_EN_MASK   0x00FFF000
#define PTA_GIO_LOW_LATE_EN_SHFT   12

/*
 Definition of Slot Timings:
 */

#define HW_GIO_TOTAL_FRAME_TIME (1250)
#define HW_GIO_TOTAL_SLOT_TIME  (625)
#define HW_GIO_FRAME_START      (0)
#define HW_GIO_TX_START         (0)
#define HW_GIO_TX_SYNC_OUT      (HW_GIO_TX_START+80)
#define HW_GIO_TX_MID           (312)
#define HW_GIO_TX_END           (624)
#define HW_GIO_RX_START         (625)
#define HW_GIO_RX_SYNC_DET      (HW_GIO_RX_START+80)
#define HW_GIO_RX_MID           (937)
#define HW_GIO_RX_END           (1249)
#define HW_GIO_FRAME_END        (1249)

#define HW_PTA_ASSERTED_ON_TX 1
#define HW_PTA_ASSERTED_ON_RX 2

/*
 Allocation of PTA signalling lines shall be as follows:
 BT_REQ (BT_TX_REQ)                    - gio[12]
 BT_PRIORITY (BT_TX_PRIORITY)          - gio[13]
 BT_TX_ACCESS                          - gio[14]
 BT_RX_ACCESS                          - gio[15]
 BT_FREQ_OVERLAP (BT_TX_FREQ_OVERLAP)  - gio[16]
 PTA_GRANT_TEST  (PTA_TX_GRANT_TEST)   - gio[17]
 Useful shadow signals for use with the COMBINE feature:
 BT_EPTA_REQ                           - gio[18]
 PTA_RX_GRANT_TEST                     - gio[19]
 BT_RX_ACCESS_SHADOW                   - gio[20]
 BT_RX_REQ                             - gio[21]
 BT_RX_PRIORITY                        - gio[22]
 BT_RX_FREQ_OVERLAP                    - gio[23]
*/

#define HW_OVERRIDE_HIGH 0x8000 /* otherwise set an intraslot value from 0 to 1249 */
#define HW_ACTIVE_OVERRIDE_HIGH 0x9000 /* override via bit15 & bit12 */

#if defined(__USE_INLINES__) || defined(_PARTHUS_HW_PTA_IMPL_)

__INLINE__ void HW_set_GIO_HIGH_BT_TX_REQ(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_12, value); }
__INLINE__ void HW_set_GIO_HIGH_BT_TX_PRIORITY(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_13, value); }
__INLINE__ void HW_set_GIO_HIGH_TX_ACCESS(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_14, value); 
                  /*mSetHWEntry(HAB_GIO_HIGH_19, value);*/ } /* shadow combine */
__INLINE__ void HW_set_GIO_HIGH_RX_ACCESS(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_15, value); 
                   mSetHWEntry(HAB_GIO_HIGH_20, value); } /* shadow combine */
__INLINE__ void HW_set_GIO_HIGH_BT_TX_FREQ_OVERLAP(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_16, value); }
__INLINE__ void HW_set_GIO_HIGH_PTA_TX_GRANT_TEST(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_17, value); }
__INLINE__ void HW_set_GIO_HIGH_PTA_RX_GRANT_TEST(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_19, value); }
__INLINE__ void HW_set_GIO_HIGH_BT_EPTA_REQ(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_18, value); }
__INLINE__ void HW_set_GIO_HIGH_BT_RX_REQ(const u_int value) {mSetHWEntry(HAB_GIO_HIGH_21, value); }
__INLINE__ void HW_set_GIO_HIGH_BT_RX_PRIORITY(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_22, value); }
__INLINE__ void HW_set_GIO_HIGH_BT_RX_FREQ_OVERLAP(const u_int value)  {mSetHWEntry(HAB_GIO_HIGH_23, value); }

__INLINE__ void HWpta_Set_GIOs_Hi_Early(const u_int16 mask)  {mSetHWEntry(PTA_GIO_HIGH_EARLY_EN, mask); }
__INLINE__ void HWpta_Set_GIOs_Hi_Late(const u_int16 mask)  {mSetHWEntry(PTA_GIO_HIGH_LATE_EN, mask); }
__INLINE__ void HWpta_Set_GIOs_Lo_Early(const u_int16 mask)  {mSetHWEntry(PTA_GIO_LOW_EARLY_EN, mask); }
__INLINE__ void HWpta_Set_GIOs_Lo_Late(const u_int16 mask)  {mSetHWEntry(PTA_GIO_LOW_LATE_EN, mask); }

#else /* declare prototypes */

void HW_set_GIO_HIGH_BT_TX_REQ(const u_int value);
void HW_set_GIO_HIGH_BT_TX_PRIORITY(const u_int value);
void HW_set_GIO_HIGH_TX_ACCESS(const u_int value);
void HW_set_GIO_HIGH_RX_ACCESS(const u_int value);
void HW_set_GIO_HIGH_BT_TX_FREQ_OVERLAP(const u_int value);
void HW_set_GIO_HIGH_PTA_TX_GRANT_TEST(const u_int value);
void HW_set_GIO_HIGH_PTA_RX_GRANT_TEST(const u_int value);
void HW_set_GIO_HIGH_BT_EPTA_REQ(const u_int value);
void HW_set_GIO_HIGH_BT_RX_REQ(const u_int value);
void HW_set_GIO_HIGH_BT_RX_PRIORITY(const u_int value);
void HW_set_GIO_HIGH_BT_RX_FREQ_OVERLAP(const u_int value);

void HWpta_Set_GIOs_Hi_Early(const u_int16 mask);
void HWpta_Set_GIOs_Hi_Late(const u_int16 mask);
void HWpta_Set_GIOs_Lo_Early(const u_int16 mask);
void HWpta_Set_GIOs_Lo_Late(const u_int16 mask);

#endif

#define HW_OVERRIDE_LOW 0x8000 /* otherwise set an intraslot value from 0 to 1249 */

#if defined(__USE_INLINES__) || defined(_PARTHUS_HW_PTA_IMPL_)
__INLINE__ void HW_set_GIO_LOW_BT_TX_REQ(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_12, value); }
__INLINE__ void HW_set_GIO_LOW_BT_TX_PRIORITY(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_13, value); }
__INLINE__ void HW_set_GIO_LOW_TX_ACCESS(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_14, value); 
                 /*mSetHWEntry(HAB_GIO_LOW_19, value);*/ } /* shadow combine */
__INLINE__ void HW_set_GIO_LOW_RX_ACCESS(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_15, value); 
                   mSetHWEntry(HAB_GIO_LOW_20, value); } /* shadow combine */
__INLINE__ void HW_set_GIO_LOW_BT_TX_FREQ_OVERLAP(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_16, value); }
__INLINE__ void HW_set_GIO_LOW_PTA_TX_GRANT_TEST(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_17, value); }
__INLINE__ void HW_set_GIO_LOW_PTA_RX_GRANT_TEST(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_19, value); }
__INLINE__ void HW_set_GIO_LOW_BT_EPTA_REQ(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_18, value); }
__INLINE__ void HW_set_GIO_LOW_BT_RX_REQ(const u_int value) {mSetHWEntry(HAB_GIO_LOW_21, value); }
__INLINE__ void HW_set_GIO_LOW_BT_RX_PRIORITY(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_22, value); }
__INLINE__ void HW_set_GIO_LOW_BT_RX_FREQ_OVERLAP(const u_int value)  {mSetHWEntry(HAB_GIO_LOW_23, value); }
#else /* declare prototypes */
void HW_set_GIO_LOW_BT_TX_REQ(const u_int value);
void HW_set_GIO_LOW_BT_TX_PRIORITY(const u_int value);
void HW_set_GIO_LOW_TX_ACCESS(const u_int value);
void HW_set_GIO_LOW_RX_ACCESS(const u_int value);
void HW_set_GIO_LOW_BT_TX_FREQ_OVERLAP(const u_int value);
void HW_set_GIO_LOW_PTA_TX_GRANT_TEST(const u_int value);
void HW_set_GIO_LOW_PTA_RX_GRANT_TEST(const u_int value);
void HW_set_GIO_LOW_BT_EPTA_REQ(const u_int value);
void HW_set_GIO_LOW_BT_RX_REQ(const u_int value);
void HW_set_GIO_LOW_BT_RX_PRIORITY(const u_int value);
void HW_set_GIO_LOW_BT_RX_FREQ_OVERLAP(const u_int value);
#endif


#if 1 // if HW_PTA_ALT_3WIRE_INTERFACE, these spare PTA signals can be used for debug:

#define HW_set_BT_PRIO_HI() \
		{\
		HW_set_GIO_HIGH_BT_TX_PRIORITY(HW_OVERRIDE_HIGH); \
		HW_set_GIO_LOW_BT_TX_PRIORITY(0);\
		}

#define HW_set_BT_PRIO_LO() \
		{\
		HW_set_GIO_HIGH_BT_TX_PRIORITY(0); \
		HW_set_GIO_LOW_BT_TX_PRIORITY(HW_OVERRIDE_LOW);\
		}

#define HW_set_BT_FREQ_HI() \
		{\
		HW_set_GIO_HIGH_BT_TX_FREQ_OVERLAP(HW_OVERRIDE_HIGH); \
		HW_set_GIO_LOW_BT_TX_FREQ_OVERLAP(0);\
		}

#define HW_set_BT_FREQ_LO() \
		{\
		HW_set_GIO_HIGH_BT_TX_FREQ_OVERLAP(0); \
		HW_set_GIO_LOW_BT_TX_FREQ_OVERLAP(HW_OVERRIDE_LOW);\
		}

#endif



#else // PRH_BS_CFG_SYS_PTA_DRIVER_SUPPORTED==0

/*
 * Empty function calls
 */
#define HWpta_Initialise()
#define HWpta_Return_To_Standby()
#define HWpta_Set_Bt_Req_Lead_Time(lead_time)

#define HWpta_Tx_Access_Request(priority, freq_overlap, role)
#define HWpta_Rx_Access_Request(priority, freq_overlap, role)

#define HWpta_Service_Tx_Start()
#define HWpta_Service_PKA()
#define HWpta_Service_Rx_Start()
#define HWpta_Service_PKD()
#define HWpta_Service_NoPKD()

#define HWpta_Generic_BT_Access_Mode(enabled)

#define HWpta_LE_Access_Request(priority, freq_overlap, role)
#define HWpta_LE_Access_Complete()

#define HWpta_Extended_Access_Request(priority, freq_overlap, number_of_slots)

#define HWpta_WinExt_Adjust_En()
#define HWpta_WinExt_Adjust_Dis()

#endif
