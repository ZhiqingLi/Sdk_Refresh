#ifndef _PARTHUS_USLC_PAGE_
#define _PARTHUS_USLC_PAGE_

/*********************************************************************
 * MODULE NAME:    uslc_page.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Baseband Paging procedure
 * MAINTAINER:     John Nelson, Conor Morris
 *
 * SOURCE CONTROL: $Id: uslc_page.h,v 1.18 2013/09/24 15:24:27 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 *      
 *********************************************************************/

t_error USLCpage_Request(t_deviceIndex device_index, boolean allow_fhs,
                          u_int16 train_repetitions, t_slots page_timeout);
void USLCpage_Cancel(void);
void USLCpage_Initialise(void);
void USLCpage_Page(boolean look_ahead_to_next_frame);
RAM_CODE void USLCpage_Potential_Early_Prepare_MasterPageResponse(void);
void USLCpage_Page_Force_To_Tx_State_If_Req(void);
t_error USLCpage_Page_Resume(t_slots slots);
u_int8 USLCpage_Is_Page_Suspended(void);
boolean USLCpage_expired(void);
#endif
