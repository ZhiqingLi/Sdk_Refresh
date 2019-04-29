#ifndef _PARTHUS_LMP_ENCODE_PDU_
#define _PARTHUS_LMP_ENCODE_PDU_

/******************************************************************************
 * MODULE NAME:    lmp_encode_pdu.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "lmp_acl_connection.h"

t_error LM_Encode_LMP_PDU(
    t_deviceIndex device_index, t_lmp_pdu_info* pdu_info);

t_error LM_Encode_LMP_PDU_Fully_Encoded(
    t_deviceIndex device_index, u_int16 opcode, const u_int8 *p_pdu);

void LM_Encode_Msg_Timeout(t_lmp_link* p_link);

void LM_Encode_LMP_Accepted_PDU(
    t_lmp_link *p_link, u_int8 opcode);
void LM_Encode_LMP_Not_Accepted_PDU(
    t_lmp_link *p_link, u_int8 opcode, t_error reason);

void LM_Encode_LMP_Accepted_Ext_PDU(
    t_lmp_link *p_link, u_int16 opcode);
void LM_Encode_LMP_Not_Accepted_Ext_PDU(
    t_lmp_link *p_link, u_int16 opcode, t_error reason);

#endif


