#ifndef _PARTHUS_USLC_MSS_CTRL_STATES_
#define _PARTHUS_USLC_MSS_CTRL_STATES_

/***********************************************************************
 *
 * MODULE NAME:    uslc_mss_ctrl_states.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  26 October 2000
 *
 * SOURCE CONTROL: $Id: uslc_mss_ctrl_states.h,v 1.12 2004/07/07 14:21:59 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    26.Oct.2000 -   IG       - separation of control states enum from
 *                               role switch procedure
 *    
 ***********************************************************************/

typedef enum 
{
    OS_INACTIVE,
    OS_START,

    OS_SLAVE_A_TDD_SWITCH_TX,
    OS_SLAVE_A_TDD_SWITCH_RX,
    OS_MASTER_A_PICONET_SWITCH_TX,
    OS_MASTER_A_PICONET_SWITCH_RX,
    OS_SLAVE_A_MSS_FAILURE_RECOVER_TDD,
    OS_SLAVE_A_MSS_FAILURE_RECOVER_SLOT_OFFSET,
    OS_SLAVE_A_MSS_FAILURE_RECOVER_CLK,
    OS_MASTER_A_CONNECTED_TO_SLAVE,
    
    OS_MASTER_B_TDD_SWITCH_RX,
    OS_SLAVE_B_PICONET_SWITCH_INTRASLOT_ADJUST,
    OS_SLAVE_B_PICONET_SWITCH_RX,
    OS_MASTER_B_END,
    OS_SLAVE_B_CONNECTED_TO_MASTER,

    OS_RECOVER_ORIGINAL_PICONET,

    OS_SENTINEL /* NOT A VALID STATE */
} t_USLCmss_Ctrl_State;

#ifdef DEBUG
static char *TESTmss_Ctrl_State_Desc[] =
{
    "OS_INACTIVE",
    "OS_START",

    "OS_SLAVE_A_TDD_SWITCH_TX",
    "OS_SLAVE_A_TDD_SWITCH_RX",
    "OS_MASTER_A_PICONET_SWITCH_TX",
    "OS_MASTER_A_PICONET_SWITCH_RX",
    "OS_SLAVE_A_MSS_FAILURE_RECOVER_SLOT",
    "OS_SLAVE_A_MSS_FAILURE_RECOVER_CLK",
    "OS_MASTER_A_CONNECTED_TO_SLAVE",
    
    "OS_MASTER_B_TDD_SWITCH_RX",
    "OS_SLAVE_B_PICONET_SWITCH_RX",
    "OS_MASTER_B_END",
    "OS_SLAVE_B_CONNECTED_TO_MASTER",

    "OS_RECOVER_ORIGINAL_PICONET",

    "OS_SENTINEL" /* NOT A VALID STATE */
};
#endif
/* DEBUG */

#endif
