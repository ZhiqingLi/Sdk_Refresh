/***********************************************************************
 *
 * MODULE NAME:    sys_mmi.h
 * PROJECT CODE:   Bluetooth
 * DESCRIPTION:    Hardware MMI Interface
 * MAINTAINER:     Tom Kerwick
 * CREATION DATE:  10 Jan 2011
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/
#ifndef _PARTHUS_SYS_MMI_
#define _PARTHUS_SYS_MMI_

#include "sys_types.h"

typedef enum
{
    eSYSmmi_BTCLK_Tick_Event        = 0,
    eSYSmmi_LC_Connection_Event     = 1,
    eSYSmmi_LM_Connection_Event     = 2,
    eSYSmmi_Scan_Active_Event       = 3,

    eSYSmmi_PKA_Event               = 4,
    eSYSmmi_PKD_Event               = 5,
    eSYSmmi_HCIt_Activity_Event     = 6,
    eSYSmmi_BTms_Sched_Event        = 7,

    eSYSmmi_TX_START_Event          = 8,
    eSYSmmi_TX_MID_Event            = 9,
    eSYSmmi_RX_START_Event          = 10,
    eSYSmmi_RX_MID_Event            = 11,

    eSYSmmi_SYNC_DET_Event          = 12,
    eSYSmmi_PKT_RX_HDR_Event        = 13,
    eSYSmmi_NO_PKT_RCVD_Event       = 14,

    eSYSmmi_LC_Disconnection_Event  = 15,
    eSYSmmi_LM_Disconnection_Event  = 16,
    eSYSmmi_Scan_Inactive_Event     = 17,

    eSYSmmi_HAL_Corruption_Event    = 18,
    eSYSmmi_HAL_Test_Event          = 19,
    eSYSmmi_HAL_Incompatible_Event  = 20,
	eSYSmmi_SPI_Conflict_Event		= 21


} t_SYSmmi_Event;

                                 // 7 6 5 4  3 2 1 0
#define SYS_MMI_7SEG_0		0x3F // 0 0 1 1  1 1 1 1
#define SYS_MMI_7SEG_1		0x06 // 0 0 0 0  0 1 1 0
#define SYS_MMI_7SEG_2		0x5B // 0 1 0 1  1 0 1 1
#define SYS_MMI_7SEG_3		0x4F // 0 1 0 0  1 1 1 1
#define SYS_MMI_7SEG_4      0x66 // 0 1 1 0  0 1 1 0
#define SYS_MMI_7SEG_5      0x6D // 0 1 1 0  1 1 0 1
#define SYS_MMI_7SEG_6      0x7D // 0 1 1 1  1 1 0 1
#define SYS_MMI_7SEG_7      0x07 // 0 0 0 0  0 1 1 1
#define SYS_MMI_7SEG_8      0x7F // 0 1 1 1  1 1 1 1
#define SYS_MMI_7SEG_9      0x6F // 0 1 1 0  1 1 1 1
#define SYS_MMI_7SEG_A      0x77 // 0 1 1 1  0 1 1 1
#define SYS_MMI_7SEG_B      0x7C // 0 1 1 1  1 1 0 0
#define SYS_MMI_7SEG_C      0x39 // 0 0 1 1  1 0 0 1
#define SYS_MMI_7SEG_D      0x5E // 0 1 0 1  1 1 1 0
#define SYS_MMI_7SEG_E      0x79 // 0 1 1 1  1 0 0 1
#define SYS_MMI_7SEG_F      0x71 // 0 1 1 1  0 0 0 1

void SYSmmi_Initialise(void);

void SYSmmi_Display_Event(t_SYSmmi_Event);

void SYSmmi_Display_eSYSmmi_BTCLK_Tick_Event(void);
void SYSmmi_Display_eSYSmmi_Periodic_Event(t_SYSmmi_Event);

void SYSmmi_Display_Numeric(u_int16 data);
void SYSmmi_Display_7SEG_D(u_int8 data);
void SYSmmi_Display_7SEG_C(u_int8 data);
void SYSmmi_Display_7SEG_B(u_int8 data);
void SYSmmi_Display_7SEG_A(u_int8 data);

void SYSmmi_Display_LCD(char* text);

#endif
