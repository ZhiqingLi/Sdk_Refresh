#ifndef _PARTHUS_BT_FHS_IMPL_
#define _PARTHUS_BT_FHS_IMPL_

/******************************************************************************
 * MODULE NAME:    bt_fhs_impl.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Frequency Hop Structure
 * MAINTAINER:     John Nelson/Ivan Griffin
 * DATE:           27 May 1999
 *
 * SOURCE CONTROL: $Id: bt_fhs_impl.h,v 1.33 2014/05/19 19:25:54 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     27 May 1999 -   jn       - Initial Version V0.9
 *    V1.1     13 Jul 1999 -   ig       - Reimplemented as inlines
 *    v1.2     27 Jul 1999 -   g.f      - Minor Modifications
      v1.3     28 Jul 1999 -   g.f      - Added sets for srMode,spMode,smMode
 *
 * SOURCE: Based on V1_4 Design
 * ISSUES:
 * NOTES TO USERS:
 *
 ******************************************************************************/

#include "hw_register.h"
#include "sys_types.h"
#include "bt_fhs.h"

#if (PRAGMA_INLINE==1)
#pragma inline(FHS_Get_LAP,\
FHS_Set_LAP,\
FHS_Get_UAP,\
FHS_Set_UAP,\
FHS_Get_NAP,\
FHS_Set_NAP,\
FHS_Get_UAP_LAP,\
FHS_Get_CLK,\
FHS_Change_CLK_To_Offset,\
FHS_Get_AM_Addr,\
FHS_Set_Am_Addr,\
FHS_Get_Bd_Addr_Ex,\
FHS_Set_Bd_Addr,\
FHS_Get_Syncword_Ex,\
FHS_Get_EIR,\
FHS_Set_EIR,\
FHS_Get_Page_Scan_Repetition,\
FHS_Set_Page_Scan_Repetition,\
FHS_Get_Page_Scan_Period,\
FHS_Set_Page_Scan_Period,\
FHS_Get_Page_Scan_Mode,\
FHS_Set_Page_Scan_Mode,\
FHS_Get_Device_Class,\
FHS_Set_Device_Class)
#endif

/*
 * Byte array to remove bit field anomalies and possible endianness
 */

#define to_u_int32(addr)   ((u_int32)*(addr) + (*((addr)+1)<<8) + \
                           (*((addr)+2)<<16) + (*((addr)+3)<<24) )

#define to_u_int24(addr)   ((u_int32)*(addr) + (*((addr)+1)<<8) + (*((addr)+2)<<16) )

#define to_u_int16(addr)   ((u_int16)*(addr) + (*((addr)+1)<<8) )

__INLINE__ t_lap FHS_Get_LAP(const t_FHSpacket *fhs)
{
    const u_int8 *p_value32 = fhs->payload + 4;
    return ((t_lap) ( (to_u_int32(p_value32) & 0x03FFFFFC) >> 2  ));
}

__INLINE__ t_uap FHS_Get_UAP(const t_FHSpacket *fhs)
{
    return ((t_uap) fhs->payload[8]);
}

__INLINE__ t_nap FHS_Get_NAP(const t_FHSpacket *fhs)
{
    const u_int8 *p_value16 = fhs->payload + 9;
    return ((t_nap) to_u_int16(p_value16) );
}

__INLINE__ void FHS_Set_LAP(t_FHSpacket *fhs, t_lap lap)
{
    /*
     * LAP in FHS bytes[4:7] = { LAP[5:0] << 2, LAP[13:6], LAP[21:14], LAP[23:22] }
     */
    u_int8 *p_lap = fhs->payload + 4;

    *p_lap   = (u_int8) _SYS_GET_CHAR_8_BITS((*p_lap & 0x03) | (u_int8) (lap<<2));
    *++p_lap = (u_int8) _SYS_GET_CHAR_8_BITS(lap>> 6);
    *++p_lap = (u_int8) _SYS_GET_CHAR_8_BITS(lap>>14);
    ++p_lap;
    *p_lap   = (u_int8) ((*p_lap & 0xFC) | ((u_int8) (lap>>22) & 0x3));
}

__INLINE__ void FHS_Set_UAP(t_FHSpacket *fhs, t_uap uap)
{
    fhs->payload[8] = uap;
}

__INLINE__ void FHS_Set_NAP(t_FHSpacket *fhs, t_nap nap)
{
    u_int8 *p_nap = fhs->payload + 9;
    *p_nap++ = (u_int8) (nap & 0xFF);
    *p_nap   = (u_int8) (nap >> 8);
}


__INLINE__ t_uap_lap FHS_Get_UAP_LAP(const t_FHSpacket *fhs)
{
    return ( ((t_uap_lap) fhs->payload[8] << 24) | FHS_Get_LAP(fhs) );
}

__INLINE__ t_clock FHS_Get_CLK(const t_FHSpacket *fhs)
{
    const u_int8 *p_value32 = fhs->payload + 14;

    /*
     * CLK[27:2] bits is stored in fhs[14:17] bytes (over bits 3:28)
     */
    return ((t_clock) ( (to_u_int32(p_value32) & 0x1FFFFFF8) >> 1  ) );
}

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
__INLINE__ u_int8 FHS_Get_EIR(const t_FHSpacket *fhs)
{
    return (u_int8) ((fhs->payload[7] >> 2) & 0x1 );
}
#endif

__INLINE__ t_pageScanRepit FHS_Get_Page_Scan_Repetition(const t_FHSpacket *fhs)
{
    return (t_pageScanRepit) ((fhs->payload[7] >> 4) & 0x3 );
}

__INLINE__ t_pageScanPeriod FHS_Get_Page_Scan_Period(const t_FHSpacket *fhs)
{
    return (t_pageScanPeriod) ((fhs->payload[7] >> 6) & 0x3 );
}

__INLINE__ t_pageScanMode FHS_Get_Page_Scan_Mode(const t_FHSpacket *fhs)
{
    return (t_pageScanMode) ((fhs->payload[17] >> 5) & 0x7);
}

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
__INLINE__ void FHS_Set_EIR(t_FHSpacket *fhs, u_int8 eir)
{
    u_int8 *p_fhs_byte7 = fhs->payload + 7;
    *p_fhs_byte7 = (u_int8) ((*p_fhs_byte7 & 0xFB) | (eir << 2 ));
}
#endif

__INLINE__ void FHS_Set_Page_Scan_Repetition(t_FHSpacket *fhs, t_pageScanRepit srMode)
{
    u_int8 *p_fhs_byte7 = fhs->payload + 7;
    *p_fhs_byte7 = (u_int8) ((*p_fhs_byte7 & 0xCF) | (srMode << 4 ));
}

__INLINE__ void FHS_Set_Page_Scan_Period(t_FHSpacket *fhs, t_pageScanPeriod spMode)
{
    u_int8 *p_fhs_byte7 = fhs->payload + 7;
    *p_fhs_byte7 = (u_int8) ((*p_fhs_byte7 & 0x3F) | (spMode << 6 ));
}

__INLINE__ void FHS_Set_Page_Scan_Mode(t_FHSpacket *fhs, t_pageScanMode smMode)
{
    u_int8 *p_fhs_byte17 = fhs->payload + 17;
    *p_fhs_byte17 = (u_int8) ((*p_fhs_byte17 & 0x1F) | (smMode << 5 ));
}

__INLINE__ t_am_addr FHS_Get_AM_Addr(const t_FHSpacket *fhs)
{

    return (t_am_addr) (fhs->payload[14] & 0x07 );
}

__INLINE__ void FHS_Set_Am_Addr(t_FHSpacket *fhs, t_am_addr am_addr)
{
    u_int8 *p_fhs_byte14 = fhs->payload + 14;
    *p_fhs_byte14 = (u_int8) ((*p_fhs_byte14 & 0xF8) | (am_addr));
}
          
__INLINE__ void FHS_Get_Bd_Addr_Ex(const t_FHSpacket *fhs, t_bd_addr *p_bd_addr)
{
    BDADDR_Set_LAP(p_bd_addr, FHS_Get_LAP(fhs));
    BDADDR_Set_UAP(p_bd_addr, FHS_Get_UAP(fhs));
    BDADDR_Set_NAP(p_bd_addr, FHS_Get_NAP(fhs));
}

__INLINE__ void FHS_Set_Bd_Addr(t_FHSpacket *fhs, const t_bd_addr *p_bd_addr)
{
    FHS_Set_LAP(fhs, BDADDR_Get_LAP(p_bd_addr) );
    FHS_Set_UAP(fhs, BDADDR_Get_UAP(p_bd_addr) );
    FHS_Set_NAP(fhs, BDADDR_Get_NAP(p_bd_addr) );
}

__INLINE__ void FHS_Get_Syncword_Ex(const t_FHSpacket *fhs, t_syncword *p_syncword)
{
    /* Includes barker sequence check ??? Bit 25 is correct   */
    p_syncword->low  = to_u_int32(fhs->payload);
    p_syncword->high = to_u_int32(fhs->payload+4) & 0x03FFFFFF;
    p_syncword->high |= (u_int32)(((p_syncword->high)&BIT25) ? 0x4C000000L : 0xB0000000L);
}

__INLINE__ void FHS_Set_Syncword_Ex(t_FHSpacket *fhs, const t_syncword *p_syncword)
{
    u_int8 *p_fhs_payload = fhs->payload;
    u_int8 fhs_byte7 = fhs->payload[7];    /*Retain Page settings         */

    *p_fhs_payload++ = (u_int8) _SYS_GET_CHAR_8_BITS(p_syncword->low);
    *p_fhs_payload++ = (u_int8) _SYS_GET_CHAR_8_BITS(p_syncword->low>> 8);
    *p_fhs_payload++ = (u_int8) _SYS_GET_CHAR_8_BITS(p_syncword->low>> 16);
    *p_fhs_payload++ = (u_int8) (p_syncword->low>> 24);

    *p_fhs_payload++ = (u_int8) _SYS_GET_CHAR_8_BITS(p_syncword->high);
    *p_fhs_payload++ = (u_int8) _SYS_GET_CHAR_8_BITS(p_syncword->high >> 8);
    *p_fhs_payload++ = (u_int8) _SYS_GET_CHAR_8_BITS(p_syncword->high >> 16);
    *p_fhs_payload++ = (u_int8) (((p_syncword->high >> 24) & 0x3) |
                                 (fhs_byte7&0xFC));

}

__INLINE__ t_classDevice FHS_Get_Device_Class(const t_FHSpacket *fhs)
{
    return (t_classDevice) to_u_int24(fhs->payload+11);
}

__INLINE__ void FHS_Set_CLK(t_FHSpacket *fhs, t_clock clk)
{
    /*
     *clk14_2  = ((clk)>>2);   clk27_15 = ((clk)>>15);
     */
    u_int8 *p_fhs_clk = fhs->payload+14;
    clk >>= 2;
    *p_fhs_clk = (u_int8) ((*p_fhs_clk & 0x07 ) | ((clk & 0x1F)<< 3));
    clk >>= 5;
    *++p_fhs_clk = (u_int8) (clk & 0xFF);
    clk >>= 8;
    *++p_fhs_clk = (u_int8) (clk & 0xFF);
    clk >>= 8;
    ++p_fhs_clk;
    *p_fhs_clk = (u_int8) ((*p_fhs_clk & 0xE0) | (clk & 0x1F));
}

/*
 * FHS_Change_CLK_To_Offset
 *
 * The clock in FHS is the peer's absolute value, must store
 * the relative value (to the local's native clock).
 */
__INLINE__ t_clock FHS_Change_CLK_To_Offset(t_FHSpacket *fhs, t_clock base_clk)
{
    t_clock remoteCLK = FHS_Get_CLK(fhs);
    t_clock clk = (remoteCLK - (base_clk&0x0FFFFFFC)) & 0x0FFFFFFF;
    FHS_Set_CLK(fhs, clk);
    return clk;
}

__INLINE__ void FHS_Set_Device_Class(t_FHSpacket *fhs, t_classDevice class_of_device)
{
    u_int8 *p_fhs_device_class = fhs->payload+11;
    *p_fhs_device_class++ = (u_int8) (class_of_device & 0xFF);
    class_of_device >>=8;
    *p_fhs_device_class++ = (u_int8) (class_of_device & 0xFF);
    class_of_device >>=8;
    *p_fhs_device_class++ = (u_int8) (class_of_device & 0xFF);
}

#endif

