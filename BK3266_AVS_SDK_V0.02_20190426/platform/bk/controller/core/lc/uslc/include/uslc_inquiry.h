#ifndef _PARTHUS_USLC_INQUIRY_
#define _PARTHUS_USLC_INQUIRY_

/******************************************************************************
 * MODULE NAME:    uslc_inquiry.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Page Processor Module
 * MAINTAINER:     John Nelson, Conor Morris
 *
 * SOURCE CONTROL: $Id: uslc_inquiry.h,v 1.18 2014/07/03 17:27:19 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/

t_error USLCinq_Inquiry_Request(t_lap inq_lap,
                        t_slots inquiry_timeout, u_int16 Ninquiry);
void USLCinq_Inquiry_Cancel(void);
void USLCinq_Inquiry(boolean look_ahead_to_next_frame);
void USLCinq_Inquiry_Initialise(void);
void USLCinq_Inquiry_Force_To_Tx_State_If_Req(void);
t_error USLCinq_Inquiry_Resume(t_slots slots);
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
u_int8* USLCinq_Get_Stored_Extended_Inquiry_Response(void);
boolean USLCinq_Is_Extended_Inquiry_Response_Frame(void);
#endif
u_int8 USLCinq_Is_Inquiry_Suspended(void);
u_int8 USLCinq_Get_Inq_State(void);
#endif
