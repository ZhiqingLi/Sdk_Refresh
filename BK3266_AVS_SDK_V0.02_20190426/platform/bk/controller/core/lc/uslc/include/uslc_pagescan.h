#ifndef _PARTHUS_USLC_PAGESCAN_
#define _PARTHUS_USLC_PAGESCAN_

/******************************************************************************
 * MODULE NAME:    uslc_pagescan.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Page Scan Processor Module
 * MAINTAINER:     John Nelson, Conor Morris
 * DATE:           20 Jun 1999
 *
 * SOURCE CONTROL: $Id: uslc_pagescan.h,v 1.21 2014/07/03 18:28:50 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *    
 ******************************************************************************/

t_error USLCpageScan_Request(t_deviceIndex device_index,t_slots scan_timeout,
                             t_scan_type scan_type);
void USLCpageScan_Cancel(void);
void USLCpageScan_Initialise(void);
#if (PRH_BS_DEV_TIM1_CORRECTED_PAGESCAN_WIDTHS==1)
void USLCpageScan_Page_Scan(boolean look_ahead);
#else
void USLCpageScan_Page_Scan(boolean dummy);
#endif
void USLCpageScan_Resume(void);
boolean USLCpageScan_In_Response_State(void);

#endif
