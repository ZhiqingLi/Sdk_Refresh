#ifndef _PARTHUS_LSLC_CLK_
#define _PARTHUS_LSLC_CLK_

/*************************************************************************
 *
 * HEADER NAME:    lslc_clk.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Bluetooth Clock Management in Software
 *
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  23 September 2001
 *
 * SOURCE CONTROL: $Id: lslc_clk.h,v 1.14 2005/01/21 16:43:19 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2001-2004 Ceva Inc.
 *     All rights reserved.
 *
 ***************************************************************************/

void LSLCclk_Initialise(void);

#ifndef _PARTHUS_SYS_TYPES_
#include "sys_types.h"
#endif

#ifndef _PARTHUS_LC_TYPE_
#include "lc_types.h"
#endif

boolean LSLCclk_Is_Safe_To_Read_Native_Clk(t_frame_pos frame_posn);

void LSLCclk_Wait_For(t_frame_pos frame_posn);

#endif
