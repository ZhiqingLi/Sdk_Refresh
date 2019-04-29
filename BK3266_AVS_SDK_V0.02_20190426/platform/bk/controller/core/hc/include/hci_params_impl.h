#ifndef _PARTHUS_HCI_GET_PARAMS_IMPL_
#define _PARTHUS_HCI_GET_PARAMS_IMPL_


/******************************************************************************
 * MODULE NAME:        hci_params_impl.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:        HCI Parameter Routines (Inline Implementation)
 * MAINTAINER:         Gary Fleming
 * CREATION DATE:      Middle of June 1999 
 *
 * SOURCE CONTROL: $Id: hci_params_impl.h,v 1.21 2007/09/04 17:52:43 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/
#include "hc_const.h"
#include "hci_params.h"

#if (PRAGMA_INLINE==1)
#pragma inline(HCIparam_Get_Uint16,\
HCIparam_Get_Uint24,\
HCIparam_Get_HCI_Opcode,\
HCIparam_Get_Uint32,\
HCIparam_Get_Bd_Addr,\
HCIparam_Get_Lap,\
HCIparam_Get_Connection_Handle,\
HCIparam_Get_Packet_Types,\
HCIparam_Get_Reason,\
HCIparam_Get_Role,\
HCIparam_Get_Length,\
HCIparam_Get_Num_Responses,\
HCIparam_Get_Inquiry_Length,\
HCIparam_Get_SrMode,\
HCIparam_Get_SpMode,\
HCIparam_Get_Timer,\
HCIparam_Get_Scan_Enable,\
HCIparam_Get_Scan_Activity,\
HCIparam_Get_Allow_Role_Switch, \
HCIparam_Get_Scan_Type)
#endif


/*****************************************************************
 *   FUNCTION : HCIparam_Get_Uint16
 *
 *   DESCRIPTION :
 *   retrieves a Uint16 from the PDU Payload
 *
 *****************************************************************/
__INLINE__ u_int16 HCIparam_Get_Uint16(t_p_pdu p_pdu)
{
    return ((u_int16)p_pdu[0] + ((u_int16) p_pdu[1] << 8));
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Lap
 *
 *   DESCRIPTION :
 *   retrieves a LAP (part of the BD_Address) from a buffer of u_int8's
 *
 *****************************************************************/

__INLINE__ t_lap HCIparam_Get_Lap(t_p_pdu p_pdu)
{
    return  (t_lap) (*p_pdu) + ( (t_lap) (*(p_pdu+1) ) << 8) + 
           ((t_lap) (*(p_pdu+2)) <<16 );
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Bd_Addr
 *
 *   DESCRIPTION :
 *   retrieves a BD_ADDRESS from a buffer of u_int8's
 *
 *****************************************************************/

__INLINE__ void HCIparam_Get_Bd_Addr(t_p_pdu p_pdu, t_bd_addr* p_bd_addr)
{
    BDADDR_Assign_from_Byte_Array(p_bd_addr, p_pdu);
}


/*****************************************************************
 *   FUNCTION : HCIparam_Get_Packet_Type
 *
 *   DESCRIPTION :
 *   retrieves the packet type from the PDU payload
 *
 *****************************************************************/

__INLINE__ u_int16 HCIparam_Get_Packet_Types(t_p_pdu p_pdu)
{
   return HCIparam_Get_Uint16(p_pdu);
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Uint24
 *
 *****************************************************************/

__INLINE__ u_int32 HCIparam_Get_Uint24(t_p_pdu p_pdu)
{
    return ( (u_int32)(*p_pdu) + ((u_int32)(*(p_pdu+1)) << 8) 
           + ((u_int32)(*(p_pdu+2)) <<16 ));
}


/*****************************************************************
 *   FUNCTION : HCIparam_Get_Uint32
 *
 *****************************************************************/

__INLINE__ u_int32 HCIparam_Get_Uint32(t_p_pdu p_pdu)
{
    return ( (u_int32)(*p_pdu) + ((u_int32)(*(p_pdu+1)) << 8) + 
           ( (u_int32)(*(p_pdu+2)) <<16 ) + ((u_int32)(*(p_pdu+3)) <<24 ));
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Reason
 *
 *   DESCRIPTION :
 *   retrieves an error code from the PDU payload
 *
 *****************************************************************/

__INLINE__ t_error HCIparam_Get_Reason(t_p_pdu pdu_ptr)
{
   return (t_error)*pdu_ptr;
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Length
 *
 *   DESCRIPTION :
 *   retrieves a length field from the PDU payload
 *
 *****************************************************************/

__INLINE__ u_int8 HCIparam_Get_Length(t_p_pdu pdu_ptr)
{
   return *pdu_ptr;
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Inquiry_Length
 *
 *   DESCRIPTION :
 *   retrieves the inquiry length from the PDU p_payload
 *
 *****************************************************************/

__INLINE__ u_int8 HCIparam_Get_Inquiry_Length(t_p_pdu p_pdu)
{
   return (u_int8)*p_pdu;
}


/*****************************************************************
 *   FUNCTION : HCIparam_Get_Num_Responses
 *
 *   DESCRIPTION :
 *   retrieves the number of Inquiry Responses from the PDU payload
 *
 *****************************************************************/

__INLINE__ u_int8 HCIparam_Get_Num_Responses(t_p_pdu p_pdu)
{
   return (u_int8)*p_pdu;
}


/*****************************************************************
 *   FUNCTION : HCIparam_Get_Connection_handle
 *
 *   DESCRIPTION :
 *   retrieves a connection handle from the PDU payload
 *
 *****************************************************************/

__INLINE__ t_connectionHandle HCIparam_Get_Connection_Handle(t_p_pdu p_pdu)
{
    return (t_connectionHandle)HCIparam_Get_Uint16(p_pdu);
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Role
 *
 *   DESCRIPTION :
 *   retrieves the device role from the PDU payload
 *
 *****************************************************************/

__INLINE__ t_role HCIparam_Get_Role(t_p_pdu p_pdu)
{
   return (t_role)*p_pdu;
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_srMode
 *
 *   DESCRIPTION :
 *   retrieves the Scan Repitition Mode from the PDU Payload
 *
 *****************************************************************/

__INLINE__ t_pageScanRepit HCIparam_Get_SrMode(t_p_pdu p_pdu)
{
    return (t_pageScanRepit)*p_pdu;
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_spMode
 *
 *   DESCRIPTION :
 *   retrieves the Scan Period Mode from the PDU Payload
 *
 *****************************************************************/

__INLINE__ t_pageScanPeriod HCIparam_Get_SpMode(t_p_pdu p_pdu)
{
    return (t_pageScanPeriod)*p_pdu;
}

/*****************************************************************
 *   FUNCTION :  HCIparam_Get_Scan_Enable
 *
 *   DESCRIPTION :
 *   retrieves the Scan Enable Field from the PDU Payload
 *
 *****************************************************************/

__INLINE__ t_scanEnable HCIparam_Get_Scan_Enable(t_p_pdu p_pdu)
{
    return (t_scanEnable)*p_pdu;
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Clock_Timer
 *
 *   DESCRIPTION :
 *   retrieves the Timer Field from the PDU Payload
 *
 *****************************************************************/
__INLINE__ t_slots HCIparam_Get_Timer(t_p_pdu p_pdu)
{
   return HCIparam_Get_Uint16(p_pdu);
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_Scan_Activity
 *
 *   DESCRIPTION :
 *   retrieves the Scan Activity from the PDU Payload
 *   NOT FINISHED !!!!!!
 *****************************************************************/

__INLINE__ t_scanActivity* HCIparam_Get_Scan_Activity(t_p_pdu p_pdu, 
    t_scanActivity*  scanAct)
{
   /*
    * need to get from p_pdu here!!
    */
   
   scanAct->interval = HCIparam_Get_Uint16(p_pdu);
   scanAct->window = HCIparam_Get_Uint16(p_pdu+2);
   return scanAct;
}

/*****************************************************************
 *   FUNCTION : HCIparam_Get_HCI_Opcode
 *
 *   DESCRIPTION :
 *   retrieves the HCI Opcode from the PDU payload
 *
 *****************************************************************/

__INLINE__ u_int16 HCIparam_Get_HCI_Opcode(t_p_pdu p_pdu)
{
   return HCIparam_Get_Uint16(p_pdu);
}


/*****************************************************************
 *   FUNCTION : HCIparam_Get_Allow_Role_Switch
 *
 *   DESCRIPTION :
 *   retrieves the allow role switch field from PDU payload
 *   
 *****************************************************************/
__INLINE__ u_int8 HCIparam_Get_Allow_Role_Switch(t_p_pdu p_pdu)
{
    return (*p_pdu);
}

/*****************************************************************
 *   FUNCTION :  HCIparam_Get_Scan_Enable
 *
 *   DESCRIPTION :
 *   retrieves the Scan Enable Field from the PDU Payload
 *
 *****************************************************************/

__INLINE__ t_scan_type HCIparam_Get_Scan_Type(t_p_pdu p_pdu)
{
    return (t_scan_type)*p_pdu;
}

#endif
