#ifndef _PARTHUS_LMP_LC_EVENT_DISP_
#define _PARTHUS_LMP_LC_EVENT_DISP_

/*********************************************************************
 * MODULE NAME:     
 * PROJECT CODE:    BlueStream
 * DESCRIPTION: 
 * MAINTAINER:      Gary Fleming
 * DATE:              
 * SOURCE CONTROL: 
 *
 * LICENSE:
 *     This source code is copyright (c) 2001-2004 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/

#include "lmp_types.h"
#include "lc_interface.h"


t_error LMP_LC_Event_Dispatcher(void);
t_error LMP_LC_Event(t_LC_Event_Info* p_event_info);

#endif

