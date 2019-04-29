#ifndef _PARTHUS_SYS_POST_
#define _PARTHUS_SYS_POST_

/*****************************************************************************
 *
 * MODULE NAME:    sys_post.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    SRAM / LED Sanity tester prototypes
 * MAINTAINER:     Ivan Griffin
 * DATE:           19 May 2000
 *
 * SOURCE CONTROL: $Id: sys_post.h,v 1.10 2004/07/07 14:22:30 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    21 Jan 2000  -   IG       - Initial Version 
 *    19 May 2000  -   IG       - rewritten to test AEB-1 SRAM
 *    15 Jun 2000  -   IG       - modified and integrated into HC stack
 *    08 Mar 2001  -   JS       - Moved constants to sys_hal_features.h
 *    
 * SOURCE: 
 * ISSUES:
 * 
 *****************************************************************************/



/*
 * The LSRAM_ and SRAM_ constants are defined in the platform specific
 * file sys_hal_features.h
 */


void SYSpost_Test(void);

#endif
