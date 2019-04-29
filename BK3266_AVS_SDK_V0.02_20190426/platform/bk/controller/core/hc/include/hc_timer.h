#ifndef _PARTHUS_HC_TIMER
#define _PARTHUS_HC_TIMER

/**********************************************************************
 * MODULE NAME:    hc_timer.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Host Controller Event Driven Timer Functionality
 * MAINTAINER:     Gary Fleming
 * DATE:           03 August 1999
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    xx xxx 1999 -   gf       - Initial Version 
 *
 * SOURCE CONTROL: $Id: hc_timer.h,v 1.11 2004/07/07 14:21:10 namarad Exp $
 *    
 **********************************************************************/

#include "sys_types.h"

u_int8 HCtmr_Set_Timer(t_slots timer_value, 
                       void ((*p_funct)(t_deviceIndex)), 
                       t_deviceIndex device_index);
void HCtmr_Check_All_Timers(void);
void HCtmr_Clear_Timer(u_int8 timer_index);
void HCtmr_Initialise(void);


#endif

