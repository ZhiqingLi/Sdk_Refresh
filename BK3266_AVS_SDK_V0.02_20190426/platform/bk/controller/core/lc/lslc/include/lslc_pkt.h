#ifndef _PARTHUS_LSLC_PKT_
#define _PARTHUS_LSLC_PKT_

/******************************************************************************
 * MODULE NAME:    lslc_pkt.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LSLC_Packet
 * MAINTAINER:     John Nelson
 * DATE:           12 Jun 1999
 *
 * SOURCE CONTROL: $Id: lslc_pkt.h,v 1.33 2013/04/08 11:04:50 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    12 June 1999 -   jn       - Initial Version V0.9
 *
 ******************************************************************************/

#include "sys_types.h"
#include "dl_dev.h"

/************************************************************************
 *
 * Packet Receiver Specific Functions
 *
 ************************************************************************/

DRAM_CODE void LSLCpkt_Prepare_Receiver_ID(const t_devicelink *pDevLink,
    t_state context);
DRAM_CODE void LSLCpkt_Prepare_Receiver_FHS(const t_devicelink *pDevLink,
    t_state context);
RAM_CODE void LSLCpkt_Prepare_Receiver_Connection(const t_devicelink *pDevLink);

#if (PRH_BS_CFG_SYS_LMP_MSS_SUPPORTED==1)
DRAM_CODE void LSLCpkt_Prepare_Receiver_Master_Slave_Switch(const t_devicelink *pDevLink);
#endif

#if (PRH_BS_CFG_SYS_RETURN_TO_PICONET_SUPPORTED==1)
void LSLCpkt_Prepare_Receiver_Return_To_Piconet(
	const t_devicelink *p_dev_link, t_clock rx_freq_clk);
void LSLCpkt_Prepare_Receiver_WinExt_Return_To_Piconet(
	const t_devicelink *p_dev_link, t_clock rx_freq_clk);
#endif

/************************************************************************
 * LSLCpkt_Disable_Receiver
 *
 * Disables receiver, and ensures that no receive will be allowed
 ************************************************************************/
void LSLCpkt_Disable_Receiver(void);

/************************************************************************
 * Interrupt processing routines for Packet/No Packet/Header
 ************************************************************************/
DRAM_CODE void LSLCpkt_Process_Rx_Packet_Header(void);
DRAM_CODE void LSLCpkt_Process_Rx_Packet(void);
RAM_CODE void LSLCpkt_Process_No_Rx_Packet(void);

/************************************************************************
 *
 * Packet Transmitter Specific Functions
 *
 ************************************************************************/

DRAM_CODE void LSLCpkt_Generate_ID(t_devicelink *pDevLink, t_state IDcontext);
void LSLCpkt_Generate_FHS(t_devicelink *pDevLink, t_state FHScontext);
void LSLCpkt_Generate_FHS_Ex(t_devicelink *pDevLink,
    t_state FHScontext, t_clock bt_clk);
RAM_CODE void LSLCpkt_Generate_FirstFHS_MasterPageResponse(t_devicelink *p_dev_link);
void LSLCpkt_Generate_EIR(t_devicelink *p_dev_link);
RAM_CODE void LSLCpkt_Generate_NULL(t_devicelink *pDevLink);
RAM_CODE void LSLCpkt_Generate_POLL(t_devicelink *pDevLink);
RAM_CODE void LSLCpkt_Generate_ACL(t_devicelink *pDevLink);
DRAM_CODE void LSLCpkt_Generate_SCO(t_devicelink *pDevLink);
void LSLCpkt_Generate_DV(t_devicelink *pDevLink);

/*
 * snooper functions for LSLCaccess to use in RF power control
 */
RAM_CODE t_devicelink* LSLCpkt_Get_Active_Rx_Device_Link_Ref(void);
RAM_CODE t_devicelink* LSLCpkt_Get_Active_Tx_Device_Link_Ref(void);

#endif
