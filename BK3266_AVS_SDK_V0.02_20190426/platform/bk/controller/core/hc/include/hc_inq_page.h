#ifndef _PARTHUS_HC_INQ_PAGE_
#define _PARTHUS_HC_INQ_PAGE_

/******************************************************************************
 * MODULE NAME:    hc_inq_page.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    xx xxx 1999 -   gf       - Initial Version 
 *
 * SOURCE CONTROL: $Id: hc_inq_page.h,v 1.11 2004/07/07 14:21:10 namarad Exp $
 *
 ******************************************************************************/

#include "sys_types.h"
#include "hc_const.h"


void HC_Scan_Initialise(void);

t_scanActivity HC_Read_Page_Scan_Activity(void);
t_error HC_Write_Page_Scan_Activity(t_scanActivity*);

t_scanActivity HC_Read_Inquiry_Scan_Activity(void);
void HC_Write_Inquiry_Scan_Activity(t_scanActivity*);

void HC_Write_Scan_Enable(t_scanEnable scanEnable);
t_scanEnable HC_Read_Scan_Enable(void);


#endif

