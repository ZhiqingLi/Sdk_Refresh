#ifndef _PARTHUS_USLC_PARK_SLAVE_
#define _PARTHUS_USLC_PARK_SLAVE_

/******************************************************************************
 * MODULE NAME:    uslc_park_slave.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Park Slave Processor Module
 * MAINTAINER:     John Nelson, Conor Morris
 *
 * SOURCE CONTROL: $Id: uslc_park_slave.h,v 1.13 2004/07/07 14:21:59 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *    
 ******************************************************************************/

typedef enum
{ 
    /* Park related states */
    OS_PARK_START=1,      /* Only if Park Request option 2 is selected */
    OS_UNPARK_START,         
    OS_PARK_SLAVE_ACCESS_WINDOW_START,

    /* Unpark related states */
    OS_UNPARK_RX,
    OS_W4_UNPARK_INSTRUCTION,
    OS_END_UNPARK_SUCCESSFUL,
    OS_END_UNPARK_UNSUCCESSFUL,

    /* Access Windows related states */
    OS_PARK_SLAVE_ACCESS_WINDOW_RX,
    OS_PARK_SLAVE_ACCESS_WINDOW_W4_UNPARK_MSG,
    OS_PARK_SLAVE_W4_ACCESS_WINDOW_END,
    OS_PARK_SLAVE_ACCESS_WINDOW_END,

    OS_PARK_END,
    OS_NOT_ACTIVE          /* procedure not active */
    } t_USLCparkSlave_ctrl_state; /* bluetooth device control states */

t_USLCparkSlave_ctrl_state USLCparkSlave_Get_Ctrl_State(void);

t_error USLCparkSlave_Park_Request(t_deviceIndex device_index, t_pm_addr pm_addr);

t_error USLCparkSlave_Unpark_Request(t_deviceIndex* device_index,t_parkDeviceIndex park_device_index, 
                    t_unpark_type unpark_type, t_am_addr am_addr,t_clock start_timer_ref, t_slots n_poll);

t_error USLCparkSlave_Access_Window_Request(t_slots t_access, u_int m_access, 
                 u_int n_acc_slot, u_int8 n_poll, t_deviceIndex device_index, t_ar_addr ar_addr);

t_error USLCparkSlave_Unpark_Commit(void);

t_error USLCparkSlave_Unpark_Rollback(void);

void USLCparkSlave_Initialise(void);

void USLCparkSlave_Park(boolean /*dummy*/);


#endif
