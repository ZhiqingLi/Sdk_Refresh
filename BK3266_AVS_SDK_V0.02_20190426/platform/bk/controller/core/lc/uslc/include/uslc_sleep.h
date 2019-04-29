#ifndef _PARTHUS_USLC_SLEEP_
#define _PARTHUS_USLC_SLEEP_

/***********************************************************************
 *
 * HEADER NAME:    uslc_sleep.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LC Portion of Master Slave Switch Procedure
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  17 August 2000
 *
 * SOURCE CONTROL: $Id: uslc_sleep.h,v 1.4 2004/07/07 14:22:00 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    17.Aug.2000 - IG  - Initial LC State Machine for Master-Slave Switch
 *
 ***********************************************************************/

void USLCsleep_Initialise(void);
t_error USLCsleep_Request(t_deviceIndex device_index, t_slots num_slots_to_sleep);
t_error USLCsleep_Cancel(void);
void USLCsleep_State_Dispatcher(boolean);

boolean USLCsleep_Is_Tabasco_Asleep(void);
void USLCsleep_Wakeup(void);

#endif
