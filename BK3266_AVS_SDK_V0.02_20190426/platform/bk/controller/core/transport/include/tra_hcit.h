#ifndef _PARTHUS_TRA_HCIT
#define _PARTHUS_TRA_HCIT
/*****************************************************************************
 * MODULE NAME:    tra_hcit.h
 * PROJECT CODE:   Bluetooth
 * DESCRIPTION:    HCI Generic Transport Interface Header
 * MAINTAINER:     Tom Kerwick
 * CREATION DATE:  19 April 2000
 *
 * SOURCE CONTROL: $Id: tra_hcit.h,v 1.29 2009/04/28 16:08:44 morrisc Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * SOURCE:         
 *
 *****************************************************************************/

#include "sys_config.h"

/* Transport access mechanisms (with regards to interrupt/polled operation) */

enum 
{
    TRA_HCIT_FAST_IRQ,
    TRA_HCIT_SLOW_IRQ,
    TRA_HCIT_POLLED
};

/*
 * HCI transport type bytes
 */

enum
{
    TRA_HCIT_COMMAND = 1,
    TRA_HCIT_ACLDATA = 2,
    TRA_HCIT_SCODATA = 3,
    TRA_HCIT_EVENT   = 4
};

/*
 * This structure contains information required to be persistent
 * during the operation of the HCI transport subsystem. This info
 * can be changed by calling another (platform dependent) register
 * function, passing this structure as the argument.
 * It is a union of common members and transport specific structures.
 */

struct tra_hcit_info
{
	/*
     * tx_tra_source indicates the queue (e.g. HCI/L2CAP/SCO) the PDU was removed
     * from - used to inform the queue that the PDU is fully transmitted.
     */

	u_int8 tx_tra_source;

    /*
     * Device indices held during transmit and 
     * receive (this info is required both at the
     * start and end of transmit or receive operation.
     */

    t_deviceIndex tx_device_index;
    t_deviceIndex rx_device_index;

    t_deviceIndex tx_sco_device_index;
    t_deviceIndex rx_sco_device_index;
}; 


u_int8* TRAhcit_Generic_Get_Rx_Buf(u_int8 ptype,int size, u_int8 *hci_header);
void TRAhcit_Generic_Commit_Rx_Buf(u_int8 tra_source);
void TRAhcit_Generic_Acknowledge_Complete_Tx(u_int8 tra_source, u_int32 length);

/*
 * Interfaces to System Scheduler
 */

t_error TRAhcit_Dispatch_Pending_Data(void);
t_error TRAhcit_Dispatch_Pending_Event(void);
t_error TRAhcit_Dispatch_Pending_SCO(void);

#define HCIT_TX_FIFO_LENGTH 120

void TRAhcit_Rx_Char(u_int8 ch);
void TRAhcit_Tx_PDU(void);

#endif /* _PARTHUS_TRA_HCIT */
