#ifndef _PARTHUS_LMP_INIT_
#define _PARTHUS_LMP_INIT_

/******************************************************************************
 * MODULE NAME:    lmp_debug.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_debug.h,v 1.11 2004/07/07 14:22:17 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "bt_timer.h"

#ifndef LM_DEBUG_INFO
#define LM_DEBUG_INFO                   0
#endif

#if (LM_DEBUG_INFO==1)
#include <stdio.h>
#define pDebugPrint(format_string)  (printf(format_string))
#define pDebugInc(var) (var++)
void pDebugAssert(u_int8 condition,u_int8 error_no);
void pDebugPrintParkGlobals();

#else
#define pDebugPrint(format_string)
#define pDebugInc(var) 
#define pDebugPrintClk()
#define pDebugAssert(condition,error_no)
#define pDebugPrintParkGlobals() 
#endif

#endif

