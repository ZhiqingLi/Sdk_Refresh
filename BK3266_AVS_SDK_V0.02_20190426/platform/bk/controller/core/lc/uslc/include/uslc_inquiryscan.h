#ifndef _PARTHUS_USLC_INQUIRYSCAN_
#define _PARTHUS_USLC_INQUIRYSCAN_

/******************************************************************************
 * MODULE NAME:    uslc_inquiryscan.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Page Processor Module
 * MAINTAINER:     John Nelson, Conor Morris
 *
 * SOURCE CONTROL: $Id: uslc_inquiryscan.h,v 1.17 2004/07/07 14:21:59 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/

t_error USLCinqScan_Request(t_lap inquiry_lap,t_slots scan_timeout,
										 				t_scan_type scan_type);
void USLCinqScan_Cancel(void);
void USLCinqScan_Initialise(void);
void USLCinqScan_Inquiry_Scan(boolean /*dummy*/);

#endif
