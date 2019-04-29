#ifndef _PARTHUS_LM_TIMER
#define _PARTHUS_LM_TIMER

/**********************************************************************
 * MODULE NAME:    lmp_timer.h
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
 * SOURCE CONTROL: $Id: lmp_timer.h,v 1.15 2009/08/06 15:52:37 tomk Exp $
 *    
 **********************************************************************/

#include "sys_types.h"
#include "lmp_types.h"
#include "lmp_acl_connection.h"

typedef struct {
      u_int8 used;
      u_int8 dynamic; /* Indicates if the timer should be freed after firing */
      u_int8 next;
      void  (*function)(t_lmp_link*);
      t_lmp_link* p_link;
      t_timer timer;
} t_lm_timer;

u_int8 LMtmr_Set_Timer(t_slots timer_value, 
                       void ((*p_funct)(t_lmp_link*)),t_lmp_link* p_link, u_int8 dynamic);
RAM_CODE boolean LMtmr_Timer_Expired(void);
RAM_CODE void LMtmr_Handle_Expired_Timers(void);
boolean LMtmr_Clear_Timer(u_int8 timer_index);
void LMtmr_Reset_Timer(u_int8 timer_index,t_slots timer_value);
void LMtmr_Modify_Timer(u_int8 timer_index, t_slots timer_value, boolean timer_extend);
t_slots LMtmr_Get_Residual_Slots(u_int8 timer_index);
void LMtmr_Initialise(void);

#define LMTMR_TIMEOUT_EXTEND (TRUE)
#define LMTMR_TIMEOUT_CURTAIL (FALSE)

#if (BUILD_TYPE!=UNIT_TEST_BUILD)
#define	LM_DEFER_FROM_INTERRUPT_CONTEXT(callback, p_lmp_link) \
	        LMtmr_Set_Timer(0x01, callback, p_lmp_link, 1);
#else /* use of timer here can throw ATS expected results... */
#define	LM_DEFER_FROM_INTERRUPT_CONTEXT(callback, p_lmp_link) \
            callback(p_lmp_link);
#endif

#endif

