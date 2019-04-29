#ifndef _PARTHUS_LM_SEC_CORE_
#define _PARTHUS_LM_SEC_CORE_

/******************************************************************************
 * MODULE NAME:   lmp_sec_core.h 
 * PROJECT CODE:  BlueStream
 * DESCRIPTION:   
 * MAINTAINER:    Daire McNamara
 * CREATION DATE: 8 February 2001   
 *
 * SOURCE CONTROL: $Id: lmp_sec_core.h,v 1.11 2008/11/28 17:38:40 tomk Exp $
 *
 * LICENSE:
 *   This source code is copyright (c) 2001-2004 Ceva Inc.
 *   All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "lmp_sec_types.h"
#include "lmp_acl_connection.h"


#define MAX_REPEATATTEMPTENTRIES          10
#define INITIAL_REPEATATTEMPTWAITINTERVAL 0x0640  
/* BEST FREQ.FAILED ATTACKS:1/sec */
#define MAX_REPEATATTEMPTWAITINTERVAL     0xC800 
/* 320 times INITIAL_REPEAT..*/

typedef enum {
  LINK_KEY_EXISTS,
  NO_LINK_KEY_EXISTS
}t_link_key_search_res;

struct repeatAttemptEntry {
  t_clock waitInterval;
  t_clock lastFailureUpdate;
  t_bd_addr address;
};

void LMsec_core_Array_Xor(u_int8 ARRAY_SIZE, u_int8* result, 
    u_int8* dest, u_int8* src);
u_int8 LMsec_core_Auth_Allowed(t_bd_addr *address);
void LMsec_core_Auth_Challenge(t_lmp_link *p_link, u_int8 tid_role);
void LMsec_core_Auth_Complete(t_lmp_link *p_link, t_error status);
void LMsec_core_Auth_Response(t_lmp_link *p_link, u_int8 tid_role, 
    t_rand au_rand);
t_link_key_search_res LMsec_core_Get_Link_Key(t_lmp_link* p_link);
void LMsec_core_Init_CombKey_Setup(t_lmp_link* p_link);
void LMsec_core_Initialise(t_link_key_type new_key_type);
t_error LMsec_core_Init_Authentication(t_lmp_link* p_link);
t_error LMsec_core_Init_Encryption(t_lmp_link* p_link, t_encrypt_mode mode);
void LMsec_core_Init_LinkKey_Negotiation(t_lmp_link *p_link);
void LMsec_core_Init_UnitKey_Setup(t_lmp_link* p_link);
void LMsec_core_PeerData_Tx_Go_Callback(t_deviceIndex device_index);
void LMsec_core_PeerData_Tx_Stop_Callback(t_deviceIndex device_index);
t_error LMsec_core_RecordAuthFailure(t_bd_addr *address);
t_error LMsec_core_RecordAuthSuccess(t_bd_addr *address);
t_error LMsec_core_Set_Encryption(t_lmp_link *p_link, t_flag encrypt_enable);
void LMsec_core_Set_Encryption_Complete(t_lmp_link *p_link, t_error status);
void LMsec_core_Enable_Rx_Encryption(t_deviceIndex device_index);
void LMsec_core_Change_To_Semi_Permanent_Key(t_lmp_link* p_link);
void LMsec_core_Change_To_Temporary_Key(t_lmp_link* p_link);
void LMsec_core_TimeoutHandler(t_lmp_link *p_link);
u_int8 LMsec_core_Get_Broadcast_Key_Length(void);
boolean LMsec_core_Is_Broadcast_Key_Length_Possible(void);
void LMsec_core_Clean_On_Detach(t_lmp_link* p_link);
void LMsec_core_Encryption_Key_Refresh(t_lmp_link *p_link);

#endif /* _PARTHUS_LM_SEC_CORE_ */

