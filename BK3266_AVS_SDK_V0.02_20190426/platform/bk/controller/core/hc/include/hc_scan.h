#ifndef _PARTHUS_HC_SCAN_
#define _PARTHUS_HC_SCAN_

/**********************************************************************
 * MODULE NAME:    hc_scan.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Host Controller Page / Inquiry Scan Functionality
 * MAINTAINER:     Gary Fleming
 * DATE:           21 June 1999
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    xx xxx 1999 -   gf       - Initial Version 
 *
 * SOURCE CONTROL: $Id: hc_scan.h,v 1.13 2004/07/07 14:21:10 namarad Exp $
 *
 **********************************************************************/

#include "sys_types.h"
#include "lc_interface.h"
#include "hc_const.h"


void HCscan_Initialise(void);

t_scanActivity HCscan_Read_Page_Scan_Activity(void);
t_error HCscan_Write_Page_Scan_Activity(t_scanActivity*);

t_scanActivity HCscan_Read_Inquiry_Scan_Activity(void);
t_error HCscan_Write_Inquiry_Scan_Activity(t_scanActivity*);

t_error HCscan_Write_Scan_Enable(t_scanEnable scan_enable);
t_scanEnable HCscan_Read_Scan_Enable(void);

void HCscan_Page_Scan_Start(t_deviceIndex dummy_device_index);
t_error HCscan_Page_Scan_Incoming(t_LC_Event_Info* eventInfo);
void HCscan_Inquiry_Scan_Start(t_deviceIndex device_index);

#endif



