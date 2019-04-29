#ifndef _PARTHUS_LM_SEC_UPPER_
#define _PARTHUS_LM_SEC_UPPER_

/******************************************************************************
 * MODULE NAME:   lmp_sec_upper.h 
 * PROJECT CODE:  BlueStream
 * DESCRIPTION:   Definitions for LMP security module upper interface. 
 * MAINTAINER:    Daire McNamara
 * CREATION DATE: 8 February 2001   
 *
 * SOURCE CONTROL: $Id: lmp_sec_upper.h,v 1.8 2004/07/07 14:22:20 namarad Exp $
 *
 * LICENSE:
 *   This source code is copyright (c) 2001-2004 Ceva Inc.
 *   All rights reserved.
 *
 ******************************************************************************/

#include "lmp_sec_types.h"
#include "lmp_acl_container.h"
#include "lmp_const.h"
#include "lmp_config.h"
#include "hc_event_gen.h"

t_error LMsec_upper_LM_Authentication_Requested(t_lmp_link* p_link);
t_error LMsec_upper_LM_Change_Connection_Link_Key(t_lmp_link* p_link);
t_error LMsec_upper_LM_Create_New_Unit_Key(void);
t_error LMsec_upper_LM_Link_Key_Request_Negative_Reply(t_bd_addr* p_bd_addr, 
    t_cmd_complete_event* p_cmd_complete_info);
t_error LMsec_upper_LM_Link_Key_Request_Reply(t_bd_addr* p_bd_addr,
    t_link_key link_key, t_cmd_complete_event* p_cmd_complete_info);
t_error LMsec_upper_LM_Master_Link_Key(t_link_key_life key_flag);
t_error LMsec_upper_LM_Pin_Code_Request_Negative_Reply(t_bd_addr* p_bd_addr, 
    t_cmd_complete_event* p_cmd_complete_info);
t_error LMsec_upper_LM_Pin_Code_Request_Reply(t_bd_addr* p_claimant_bd_addr, 
    t_pin_code pin_code, u_int8 pin_code_length, 
    t_cmd_complete_event* p_cmd_complete_info);
t_error LMsec_upper_LM_Set_Connection_Encryption(t_lmp_link *p_link, 
    t_flag encrypt_enable);

void LMsec_upper_Send_HCI_Event(t_lmp_link *p_link, u_int8 hci_event, 
    t_error status);

#endif /* _PARTHUS_LM_SPEC_UPPER_ */

