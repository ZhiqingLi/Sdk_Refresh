#ifndef _PARTHUS_SYS_MMI_IMPL_
#define _PARTHUS_SYS_MMI_IMPL_

/***********************************************************************
 *
 * MODULE NAME:    sys_mmi.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    generic debug event man-machine feedback
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  20 September 2000
 *
 * SOURCE CONTROL: $Id: sys_mmi_impl.h,v 1.5 2011/12/12 16:29:30 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    20 Sept 2000 - IG  - initial version
 *
 *    
 ***********************************************************************/

#include "sys_config.h"
#include "sys_types.h"

#include "sys_mmi.h"

#include "hw_leds.h"

#define PKD_TOGGLE_MASK (64-1)
#define BTCLK_TICK_TOGGLE_MASK (256-1)

__INLINE__ void SYSmmi_Display_eSYSmmi_TX_START_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_BTCLK_Tick_Event(void)
{
    static u_int32 btclk_tick_toggle = 1;

    ++btclk_tick_toggle;

    if (!(btclk_tick_toggle & BTCLK_TICK_TOGGLE_MASK))
    {
        HWled_Toggle(HWled_RED);
    }
}

__INLINE__ void SYSmmi_Display_eSYSmmi_TX_MID_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_RX_START_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_RX_MID_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_PKA_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_SYNC_DET_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_NO_PKT_RCVD_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_PKT_RX_HDR_Event(void)
{
}

__INLINE__ void SYSmmi_Display_eSYSmmi_PKD_Event(void)
{
    static u_int32 pkd_toggle = 1;

    ++pkd_toggle;

    if (!(pkd_toggle & PKD_TOGGLE_MASK))
    {
        HWled_Toggle(HWled_GREEN);
    }
}


__INLINE__ void SYSmmi_Display_eSYSmmi_HCIt_Activity_Event(void)
{
   HWled_Toggle(HWled_RED);
}

__INLINE__ void SYSmmi_Display_eSYSmmi_LC_Connection_Event(void)
{
    HWled_On(HWled_RED);
}

__INLINE__ void SYSmmi_Display_eSYSmmi_LM_Connection_Event(void)
{
    HWled_On(HWled_AMBER);
}

__INLINE__ void SYSmmi_Display_eSYSmmi_LC_Disconnection_Event(void)
{
    HWled_Off(HWled_RED);
}

__INLINE__ void SYSmmi_Display_eSYSmmi_LM_Disconnection_Event(void)
{
    HWled_Off(HWled_AMBER);
}

__INLINE__ void SYSmmi_Display_eSYSmmi_Scan_Active_Event(void)
{
    HWled_On(HWled_GREEN);
}

__INLINE__ void SYSmmi_Display_eSYSmmi_Scan_Inactive_Event(void)
{
    HWled_Off(HWled_GREEN);
}

__INLINE__ void SYSmmi_Display_eSYSmmi_SPI_Conflict_Event(void)
{
}

#endif
