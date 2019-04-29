#ifndef _PARTHUS_USLC_PARK_MASTER_
#define _PARTHUS_USLC_PARK_MASTER_

/******************************************************************************
 * MODULE NAME:    uslc_park_master.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Park Master Module
 * MAINTAINER:     Conor Morris
 *
 * SOURCE CONTROL: $Id: uslc_park_master.h,v 1.18 2004/07/07 14:21:59 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *    
 ******************************************************************************/


typedef enum
{
    OS_PARK_START_, 
    OS_UNPARK_START_,
    OS_PARK_MASTER_ACCESS_WINDOW_START,

    OS_PARK_END_,

    OS_UNPARK_TX,
    OS_UNPARK_RX_,
    OS_W4_UNPARK_INSTRUCTION_,
    OS_END_UNPARK_SUCCESSFUL_,
    OS_END_UNPARK_UNSUCCESSFUL_,

    OS_PARK_MASTER_ACCESS_WINDOW_TX,
    OS_PARK_MASTER_ACCESS_WINDOW_RX,
    OS_PARK_MASTER_END_ACCESS_WINDOW,
    OS_PARK_LOOK_AHEAD_MASTER_ACCESS_WINDOW_RX,

    OS_NOT_ACTIVE_          /* procedure not active */
} t_USLCparkMaster_ctrl_state; /* bluetooth device control states */

t_USLCparkMaster_ctrl_state USLCparkMaster_Get_Ctrl_State(void);

t_error USLCparkMaster_Park_Request(t_deviceIndex device_index, t_pm_addr pm_addr);

t_error USLCparkMaster_Unpark_Request(t_deviceIndex* device_index,t_parkDeviceIndex park_device_index, 
                    t_unpark_type unpark_type, t_am_addr am_addr,
                    t_slots end_of_beacon_train, t_slots n_poll);

t_error USLCparkMaster_Access_Window_Request(u_int n_acc_slot);

t_error USLCparkMaster_Unpark_Commit(void);
t_error USLCparkMaster_Unpark_Rollback(void);


void USLCparkMaster_Initialise(void);

void USLCparkMaster_Park(boolean look_ahead_to_next_frame);

t_USLCparkMaster_ctrl_state USLCparkMaster_Get_Ctrl_State(void);

void USLCparkMaster_Register_Access_Window_Callback_Handler(t_callback);

#if (PRH_BS_DEV_TIM3_IDENTIFIES_AR_ADDR_HALF_SLOT==1)
void USLCparkMaster_Second_Half_Slot(void);
#endif

#endif

