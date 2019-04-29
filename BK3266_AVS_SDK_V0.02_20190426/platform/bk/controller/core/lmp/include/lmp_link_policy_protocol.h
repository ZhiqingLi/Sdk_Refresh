
#ifndef _PARTHUS_LM_LINK_POLICY_PROTOCOL_
#define  _PARTHUS_LM_LINK_POLICY_PROTOCOL_

/******************************************************************************
 * MODULE NAME:    lmp_link_policy_protocol.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_link_policy_protocol.h,v 1.34 2014/07/02 15:39:50 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_types.h"
#include "lc_types.h"  /* required for t_LC_Event_Info */
#include "lmp_acl_connection.h"
#include "hc_event_gen.h"

/**************************************************
 * Service Interface to the Link Policy Object
 *************************************************/
void LMpolicy_Initialise(void);
t_error LMpolicy_LM_Read_Hold_Mode_Activity(t_lmp_link* p_link,t_cmd_complete_event* cmd_complete_info);
t_error LMpolicy_LM_Write_Hold_Mode_Activity(t_lmp_link* p_link,u_int8 mode,t_cmd_complete_event* cmd_complete_info);

t_error LMpolicy_LM_Hold_Mode(t_lmp_link* p_link,u_int16 max_interval, u_int16 min_interval);

void    LMpolicy_LM_Role_Discovery(t_lmp_link* p_link,t_cmd_complete_event* cmd_complete_info);

t_error LMpolicy_LM_Exit_Park_Mode(t_lmp_link* p_link,u_int8 automatic);
t_error LMpolicy_LM_Park_Mode(t_lmp_link* p_link,u_int16 max_interval,u_int16 min_interval);

t_error LMpolicy_LM_Sniff_Mode(t_lmp_link* p_link,u_int16 max_interval,
                                  u_int16 min_interval,u_int16 attempt,
                                  u_int16 timeout);
t_error LMpolicy_LM_Exit_Sniff_Mode(t_lmp_link* p_link);

void    LMpolicy_LMP_Unpark_Pmaddr_Req_Ack(t_deviceIndex device_index);
void    LMpolicy_LMP_Unpark_Pmaddr_Accepted_Ack(t_deviceIndex device_index);

void LMpolicy_LMP_Sniff_Req_Accepted_Ack(t_deviceIndex device_index);
void LMpolicy_LMP_Unsniff_Req_Accepted_Ack(t_deviceIndex device_index);

t_error LMpolicy_Enter_Park_Mode_Complete(t_LC_Event_Info* lc_event_info);
t_error LMpolicy_Exit_Park_Mode_Complete(t_LC_Event_Info* lc_event_info);

/***************************************************
 * Layer Internal Interface 
 ***************************************************/

void LMpolicy_LM_Return_From_Hold(t_lmp_link* p_link);
void LMpolicy_LMP_Hold_Accepted_Ack(t_deviceIndex device_index);
void LMpolicy_Enter_Sniff_Mode(t_deviceIndex device_index);
void LMpolicy_LMP_Park_Req_Ack(t_deviceIndex device_index);
void LMpolicy_LMP_Park_Accepted_Ack(t_deviceIndex device_index);

/**************************************************
 * Peer Interface to the Link Policy Object
 *************************************************/
t_error LMpolicy_LMP_Park(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Park_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Sniff_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Hold(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Hold_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Unsniff_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Switch_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Set_Broadcast_Scan_Window(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Modify_Beacon(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Unpark_Bd_Addr_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Unpark_Pm_Addr_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Accepted(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Not_Accepted(t_lmp_link* p_link,u_int16 opcode, t_error reason);
t_error _LMpol_Execute_Unpark_Request(t_lmp_link*  p_link, u_int8  automatic, t_role role);


#if (PRH_BS_CFG_SYS_LMP_SNIFF_SUPPORTED==1)
#if (PRH_BS_CFG_SYS_LMP_SNIFF_SUBRATING_SUPPORTED==1)
t_error LMpolicy_LM_Sniff_Subrating(t_lmp_link* p_link, u_int16 max_latency,
            u_int16 min_remote_timeout, u_int16 min_local_timeout);
t_error LMpolicy_LMP_Sniff_Subrating_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMpolicy_LMP_Sniff_Subrating_Res(t_lmp_link* p_link,t_p_pdu p_payload);
#endif
#endif

#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)
void LMpolicy_Service_Park_Links(void);
#else
#define LMpolicy_Service_Park_Links()
#endif



void LMpolicy_Remove_Link(const t_lmp_link* p_link);
void LMpolicy_Clear_Link_If_Parked(t_lmp_link* p_link);
#endif

