#ifndef _PARTHUS_USLC_SP_CTRL_STATES_
#define _PARTHUS_USLC_SP_CTRL_STATES_

/***********************************************************************
 *
 * MODULE NAME:    uslc_sp_ctrl_states.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  26 October 2002
 *
 * SOURCE CONTROL: $Id: uslc_sp_ctrl_states.h,v 1.3 2004/07/07 14:22:00 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    26.Oct.2002 -   IG       - separation of control states enum from
 *                               role switch procedure
 *    
 ***********************************************************************/

typedef enum 
{
    OS_INACTIVE,
    OS_START,

    OS_SWITCH_PICONET_RECOVER_TDD,
    OS_SWITCH_PICONET_RECOVER_SLOT_OFFSET,
    OS_SWITCH_PICONET_RECOVER_CLK,

    OS_END,

    OS_SENTINEL /* NOT A VALID STATE */
} t_USLCsp_Ctrl_State;

#ifdef DEBUG
static char *TESTsp_Ctrl_State_Desc[] =
{
    "OS_INACTIVE",
    "OS_START",

    "OS_SWITCH_PICONET_RECOVER_TDD",
    "OS_SWITCH_PICONET_RECOVER_SLOT_OFFSET",
    "OS_SWITCH_PICONET_RECOVER_CLK",

    "OS_END",

    "OS_SENTINEL" /* NOT A VALID STATE */
};
#endif
/* DEBUG */

#endif
/* _PARTHUS_USLC_SP_CTRL_STATES_*/

