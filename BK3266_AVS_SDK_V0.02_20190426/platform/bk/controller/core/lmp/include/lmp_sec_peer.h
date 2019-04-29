#ifndef _PARTHUS_LM_SEC_PEER_
#define _PARTHUS_LM_SEC_PEER_

/******************************************************************************
 * MODULE NAME:   lmp_sec_peer.h 
 * PROJECT CODE:  BlueStream
 * DESCRIPTION:   Definitions for LMP peer security module 
 * MAINTAINER:    Daire McNamara
 * CREATION DATE: 08 February 2001   
 *
 * SOURCE CONTROL: $Id: lmp_sec_peer.h,v 1.9 2008/11/25 16:45:34 tomk Exp $
 *
 * LICENSE:
 *   This source code is copyright (c) 2001-2004 Ceva Inc.
 *   All rights reserved.
 *
 ******************************************************************************/

#include "lmp_sec_types.h"
#include "lmp_acl_connection.h"

t_error LMsec_peer_LMP_Accepted(t_lmp_link* p_link, t_p_pdu p_pdu);
t_error LMsec_peer_LMP_Au_Rand(t_lmp_link* p_link, t_p_pdu p_pdu);
t_error LMsec_peer_LMP_Comb_Key(t_lmp_link *p_link, t_p_pdu p_pdu);
t_error LMsec_peer_LMP_Encryption_Key_Size_Req(t_lmp_link* p_link, 
    t_p_pdu p_payload);
t_error LMsec_peer_LMP_Encryption_Mode_Req(t_lmp_link* p_link, 
    t_p_pdu p_payload);
t_error LMsec_peer_LMP_In_Rand(t_lmp_link* p_link , t_p_pdu p_pdu);
t_error LMsec_peer_LMP_Not_Accepted(t_lmp_link* p_link, u_int16 opcode,
    t_error reason);
t_error LMsec_peer_LMP_Sres(t_lmp_link* p_link,  t_p_pdu p_pdu);
t_error LMsec_peer_LMP_Start_Encryption_Req(t_lmp_link* p_link, 
    t_p_pdu p_payload);
t_error LMsec_peer_LMP_Stop_Encryption_Req(t_lmp_link* p_link, 
    t_p_pdu p_payload);
#if (PRH_BS_CFG_SYS_LMP_PAUSE_ENCRYPTION_SUPPORTED==1)
void LMsec_peer_LMP_Stop_Encryption_Req_Accepted_Ack(t_deviceIndex device_index);
void LMsec_peer_LMP_Start_Encryption_Req_Accepted_Ack(t_deviceIndex device_index);
#endif
t_error LMsec_peer_LMP_Temp_Key(t_lmp_link* p_link, t_p_pdu p_pdu);
t_error LMsec_peer_LMP_Temp_Rand(t_lmp_link* p_link, t_p_pdu p_pdu);
t_error LMsec_peer_LMP_Unit_Key(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMsec_peer_LMP_Use_Semi_Permanent_Key(t_lmp_link* p_link, 
    t_p_pdu p_payload);
#if (PRH_BS_CFG_SYS_LMP_BROADCAST_ENCRYPTION_SUPPORTED==1)
t_error LMsec_peer_LMP_Encryption_Key_Size_Mask_Req(t_lmp_link* p_link, 
    t_p_pdu p_payload);
t_error LMsec_peer_LMP_Encryption_Key_Size_Mask_Res(t_lmp_link* p_link, 
    t_p_pdu p_payload);
#endif
#if (PRH_BS_CFG_SYS_LMP_PAUSE_ENCRYPTION_SUPPORTED==1)
t_error LMsec_peer_LMP_Pause_Encryption_Req(t_lmp_link* p_link, 
    t_p_pdu p_payload);
t_error LMsec_peer_LMP_Resume_Encryption_Req(t_lmp_link* p_link, 
    t_p_pdu p_payload);
#endif

void LMsec_peer_Send_LMP_Accepted(t_lmp_link* p_link, u_int8 opcode, 
    u_int8 tid_role);
void LMsec_peer_Send_LMP_Message(t_lmp_link *p_link, u_int8 opcode, 
    u_int8 tid_role, void *param);
void LMsec_peer_Send_LMP_Not_Accepted(t_lmp_link* p_link, u_int8 opcode, 
    u_int8 tid_role, t_error reason);

#if (PRH_BS_CFG_SYS_LMP_BROADCAST_ENCRYPTION_SUPPORTED==1)
void LMsec_peer_Send_LMP_Encryption_Key_Size_Mask_Req(t_lmp_link* p_link);
void LMsec_peer_Send_LMP_Encryption_Key_Size_Mask_Res(t_lmp_link* p_link);
#else
#define LMsec_peer_Send_LMP_Encryption_Key_Size_Mask_Req(p_link);
#define LMsec_peer_Send_LMP_Encryption_Key_Size_Mask_Res(p_link);
#endif
#if (PRH_BS_CFG_SYS_LMP_PAUSE_ENCRYPTION_SUPPORTED==1)
t_error LMsec_peer_Send_LMP_Pause_Encryption_Req(t_lmp_link* p_link);
t_error LMsec_peer_Resume_Encryption(t_lmp_link* p_link);
t_error LMsec_peer_Send_LMP_Resume_Encryption_Req(t_lmp_link* p_link);
#else
#define LMsec_peer_Send_LMP_Pause_Encryption_Req(p_link);
#define LMsec_peer_Resume_Encryption(p_link);
#define LMsec_peer_Send_LMP_Resume_Encryption_Req(p_link);
#endif

#endif /* _PARTHUS_LM_SEC_PEER_ */

