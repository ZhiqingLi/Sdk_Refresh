#ifndef _PARTHUS_HCI_PARAMS_
#define _PARTHUS_HCI_PARAMS_

/******************************************************************************
 * MODULE NAME:     hci_params.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:     HCI Parameter Routines
 * MAINTAINER:      Gary Fleming
 * CREATION DATE:   June 1999     
 *
 * SOURCE CONTROL: $Id: hci_params.h,v 1.13 2007/09/04 17:52:43 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

__INLINE__ u_int16 HCIparam_Get_Uint16(t_p_pdu);
__INLINE__ u_int32 HCIparam_Get_Uint24(t_p_pdu);
__INLINE__ u_int16 HCIparam_Get_HCI_Opcode(t_p_pdu);
__INLINE__ u_int32 HCIparam_Get_Uint32(t_p_pdu p_pdu);
__INLINE__ void HCIparam_Get_Bd_Addr(t_p_pdu, t_bd_addr*);
__INLINE__ t_lap HCIparam_Get_Lap(t_p_pdu);
__INLINE__ t_connectionHandle HCIparam_Get_Connection_Handle(t_p_pdu);
__INLINE__ u_int16 HCIparam_Get_Packet_Types(t_p_pdu);
__INLINE__ t_error HCIparam_Get_Reason(t_p_pdu);
__INLINE__ t_role HCIparam_Get_Role(t_p_pdu);
__INLINE__ u_int8 HCIparam_Get_Length(t_p_pdu);
__INLINE__ u_int8 HCIparam_Get_Num_Responses(t_p_pdu);
__INLINE__ u_int8 HCIparam_Get_Inquiry_Length(t_p_pdu);
__INLINE__ t_pageScanRepit HCIparam_Get_SrMode(t_p_pdu);
__INLINE__ t_pageScanPeriod HCIparam_Get_SpMode(t_p_pdu);
__INLINE__ t_slots HCIparam_Get_Timer(t_p_pdu pdu_ptr);
__INLINE__ t_scanEnable HCIparam_Get_Scan_Enable(t_p_pdu);
__INLINE__ t_scanActivity* HCIparam_Get_Scan_Activity(t_p_pdu, t_scanActivity*);

__INLINE__ u_int8 HCIparam_Get_Allow_Role_Switch(t_p_pdu);
__INLINE__ t_scan_type HCIparam_Get_Scan_Type(t_p_pdu p_pdu);

#ifdef __USE_INLINES__
    #include "hci_params_impl.h"
#endif

#endif
