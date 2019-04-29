#ifndef _PARTHUS_BT_FHS_
#define _PARTHUS_BT_FHS_

/******************************************************************************
 * MODULE NAME:    bt_fhs.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Frequency Hop Structure
 * MAINTAINER:     John Nelson, Conor Morris
 * DATE:           27 May 1999
 *
 * SOURCE CONTROL: $Id: bt_fhs.h,v 1.31 2008/10/29 11:55:20 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     27 May 1999 -   jn       - Initial Version V0.9
 *    V1.1     26 July 1999 -  cm       - Added functions to set SR, SP Mode
 *
 * SOURCE: Based on V1_4 Design
 * ISSUES:
 * NOTES TO USERS:
 *    
 ******************************************************************************/
#include "sys_types.h"
#include "sys_config.h"
#include "hw_register.h"

/*
 * Byte array used to avoid bit field anomalies and possible endianness
 * Actual FHS length is 18, but round to fullwords for word copy.
 *
 * Equivalent Definition of Bit-Field based structure, Little Endian
 *
 * typedef struct {
 *   u_int32 parity31_0;
 *
 *   u_int32 parity33_32:2;
 *   u_int32 LAP:24;
 *   u_int32 :2;
 *   u_int32 pageScanRepetition:2;
 *   u_int32 pageScanPeriod:2;
 *
 *   u_int32 UAP:8;
 *   u_int32 NAP:16;
 *   u_int32 deviceClass7_0:8;
 *
 *   u_int32 deviceClass23_8:16;
 *   u_int32 am_addr:3;
 *   u_int32 clk14_2:13;
 *
 *   u_int32 clk27_15:13;
 *   u_int32 pageScanMode:3;
 *   } t_FHSpacket;
 */

/*
 * The FHS packet must be 32 bit aligned for effective memory copy to/from hw.
 */
typedef union {
    u_int8 payload[20];
    u_int32 force_u_int32_alignment;
} t_FHSpacket;

__INLINE__ t_lap     FHS_Get_LAP(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_LAP(t_FHSpacket *fhs, t_lap lap);

__INLINE__ t_uap     FHS_Get_UAP(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_UAP(t_FHSpacket *fhs, t_uap uap);

__INLINE__ t_nap     FHS_Get_NAP(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_NAP(t_FHSpacket *fhs, t_nap nap);

__INLINE__ t_uap_lap FHS_Get_UAP_LAP(const t_FHSpacket *fhs);

__INLINE__ t_clock   FHS_Get_CLK(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_CLK(t_FHSpacket *fhs, t_clock clk);

__INLINE__ t_am_addr FHS_Get_AM_Addr(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_Am_Addr(t_FHSpacket *fhs, t_am_addr am_addr);

__INLINE__ void      FHS_Get_Bd_Addr_Ex
                            (const t_FHSpacket *fhs, t_bd_addr *p_bd_addr);
__INLINE__ void      FHS_Set_Bd_Addr
                            (t_FHSpacket *fhs, const t_bd_addr *p_bd_addr );

__INLINE__ void      FHS_Get_Syncword_Ex
                            (const t_FHSpacket *fhs, t_syncword *p_syncword);
__INLINE__ void      FHS_Set_Syncword_Ex
                            (t_FHSpacket *fhs, const t_syncword *p_syncword);

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
__INLINE__ u_int8 FHS_Get_EIR(const t_FHSpacket *fhs);
__INLINE__ void FHS_Set_EIR(t_FHSpacket *fhs, u_int8 eir);
#endif

__INLINE__ t_pageScanRepit FHS_Get_Page_Scan_Repetition(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_Page_Scan_Repetition
                            (t_FHSpacket *fhs, t_pageScanRepit srMode);

__INLINE__ t_pageScanPeriod FHS_Get_Page_Scan_Period(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_Page_Scan_Period
                            (t_FHSpacket *fhs, t_pageScanPeriod spMode);

__INLINE__ t_clock FHS_Change_CLK_To_Offset(t_FHSpacket *fhs, t_clock base_clk);

__INLINE__ t_pageScanMode   FHS_Get_Page_Scan_Mode(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_Page_Scan_Mode
                            (t_FHSpacket *fhs, t_pageScanMode smMode);

__INLINE__ t_classDevice    FHS_Get_Device_Class(const t_FHSpacket *fhs);
__INLINE__ void      FHS_Set_Device_Class
                            (t_FHSpacket *fhs, t_classDevice class_of_device);

#ifdef __USE_INLINES__
#include "bt_fhs_impl.h"
#endif

#endif


