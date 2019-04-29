#ifndef _PARTHUS_HC_EVENT_DISP
#define _PARTHUS_HC_EVENT_DISP

/**************************************************************************
 * MODULE NAME:    hc_event_disp.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Host Controller event dispatcher
 * AUTHOR:         Gary Fleming
 * DATE:           04-07-1999
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 * SOURCE CONTROL: $Id: hc_event_disp.h,v 1.5 2004/07/07 14:21:09 namarad Exp $
 *
 *************************************************************************/

#include "sys_types.h"

typedef t_error (*t_hci_event_callback)(t_p_pdu, u_int16);

void HC_Event_Handler_Init(t_hci_event_callback* p_funct);
t_error HC_Event_Dispatcher(void);

#endif

