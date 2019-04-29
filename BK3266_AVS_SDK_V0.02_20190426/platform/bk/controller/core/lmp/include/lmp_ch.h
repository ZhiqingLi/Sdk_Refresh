#ifndef _PARTHUS_LMP_CH_
#define _PARTHUS_LMP_CH_

/*********************************************************************
 * MODULE NAME:     lmp_ch.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION: 
 * MAINTAINER:      Gary Fleming
 * DATE:            17-06-99        
 * SOURCE CONTROL:  $Id: lmp_ch.h,v 1.72 2014/03/11 03:14:05 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/

#include "lmp_acl_connection.h"
#include "lmp_types.h"
#include "lc_interface.h"
#include "hc_event_gen.h"

#define  LMch_NO_CONNECTION        0x00
#define  LMch_TEMPORARY_CONNECTION 0x01
#define  LMch_LMP_CONNECTION       0x02

void LMconnection_Initialise(void);

/**************************************************
 * Higher Layer interface to the LMP Call Handler.
 *
 **************************************************/

t_error LMconnection_LM_Connection_Req(const t_pageParams* p_page_info);
t_error LMconnection_LM_Read_Remote_Name_Cancel(const t_bd_addr *bd_addr);
t_error LMconnection_LM_Connection_Cancel(const t_bd_addr *bd_addr);
t_error LMconnection_LM_Read_Remote_Name(const t_pageParams *p_page_info);

t_error LMconnection_LM_Connection_Accept(t_bd_addr* p_bd_addr,t_role,
    u_int16 voice_setting);
t_error LMconnection_LM_Connection_Reject(t_bd_addr* p_bd_addr,t_error);
t_error LMconnection_LM_Disconnect_Req(t_connectionHandle handle, t_error);
t_error LMconnection_LM_Page_Complete(t_LC_Event_Info* eventInfo);
t_error LMconnection_LM_Auth_Complete(t_lmp_link* p_link,t_error status);
t_error LMconnection_LM_Read_Remote_Supported_Features(t_lmp_link* p_link);
t_error LMconnection_LM_Read_Remote_Extended_Features(t_lmp_link* p_link, u_int8 page);
t_error LMconnection_LM_Encrypt_Complete(t_lmp_link* p_link,t_error status);
t_error LMconnection_LM_Switch_Role(t_lmp_link* p_link, t_role new_role);
void LMconnection_LM_Switch_Req(t_lmp_link *p_link);

t_error LMconnection_LM_Add_SCO_Connection(t_lmp_link* p_link,
    t_packetTypes packet_types, u_int16 voice_setting);
t_error LMconnection_LM_Change_SCO_Packet_Type(
    t_lmp_link* p_link, t_sco_info* p_sco_link, t_packetTypes packet_types);
t_error LMconnection_LM_Disconnect_SCO(t_sco_info *p_sco_link, t_error reason);

t_error LMconnection_LM_Setup_Synchronous_Connection(u_int16 handle,
    u_int32 tx_bandwidth, u_int32 rx_bandwidth, u_int16 max_latency,
    u_int16 voice_setting, u_int8 retransmission_effort, 
    u_int16 esco_packet_types);
t_error LMconnection_LM_Accept_Synchronous_Connection_Request(u_int8* p_bdaddr,
    u_int32 tx_bandwidth, u_int32 rx_bandwidth, u_int16 max_latency,
    u_int16 voice_setting, u_int8 retransmission_effort, 
    u_int16 esco_packet_types);
t_error LMconnection_LM_Reject_Synchronous_Connection_Request(u_int8* p_bdaddr,
    t_error reason);
t_error LMconnection_LM_AutoAccept_Default_eSCO_Link(t_lmp_link* p_link);
t_error LMconnection_LM_Accept_Synchronous_Connection_Request_Common(t_lmp_link* p_link,
    u_int32 tx_bandwidth, u_int32 rx_bandwidth,
    u_int16 max_latency, u_int16 voice_setting, 
    u_int8 retransmission_effort, u_int16 esco_packet_types);

/**************************************************
 * Peer interface to the LMP Call Handler
 * 
 **************************************************/
t_error LMconnection_LMP_Host_Connection_Req(t_lmp_link* p_link,t_p_pdu);
t_error LMconnection_LMP_Accepted(t_lmp_link* p_link, u_int16 opcode);
t_error LMconnection_LMP_Not_Accepted(t_lmp_link* p_link,u_int16 opcode, t_error reason);
t_error LMconnection_LMP_Setup_Complete(t_lmp_link* p_link,t_p_pdu);
t_error LMconnection_LMP_Detach(t_lmp_link* p_link,t_p_pdu);
t_error LMconnection_LMP_Name_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Name_Res(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_SCO_Link_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Remove_SCO_Link_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Switch_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Features_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Features_Res(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Features_Req_Ext(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Features_Res_Ext(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Packet_Type_Table_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
void LMconnection_LMP_Packet_Type_Table_Req_Accepted_Ack(t_deviceIndex device_index);
t_error LMconnection_LMP_eSCO_Link_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconnection_LMP_Remove_eSCO_Link_Req(t_lmp_link* p_link,t_p_pdu p_pdu);

/* Note to move to being static in the LMP */
void LMconnection_LMP_Switch_Timeout(t_lmp_link* p_link);

/**************************************************************
 *  Lower Layer interface to the LMP Call Handler 
 **************************************************************/

void LMconnection_Send_LMP_Detach(t_lmp_link* p_link, t_error reason);
void LMconnection_LMP_Detach_Ack(t_deviceIndex device_index);
void LMconnection_LMP_SCO_Accepted_Ack(t_deviceIndex device_index);
//void LMconnection_LMP_Switch_Accepted_Ack(t_deviceIndex device_index); - not used
t_error  LMconnection_LM_Page_Complete(t_LC_Event_Info* eventInfo);
void LMconnection_LMP_eSCO_Accepted_Ack(t_deviceIndex device_index);
void LMch_Send_LMP_eSCO_Link_Req(const t_lmp_link *p_link, t_esco_descr *p_esco_descr);

t_error LMconnection_LM_Switch_Role_Complete(t_LC_Event_Info* event_info);
void LMconnection_Switch_Complete_Event(t_lmp_link *p_link);
void _Send_HC_Role_Change_Event(t_lmp_link *p_link,t_error reason);

void LMconnection_LM_Disconnect(t_lmp_link *p_link, t_error reason);
void LMconnection_LM_Disconnect_Inf(t_lmp_link* p_link);
void LMconnection_LM_Recent_Contact_Timeout(t_lmp_link* p_link);
//void LMconnection_LM_EventInd(int eventCode, t_lm_callback functPtr); - not used
void LMconnection_LM_Incoming_Link_Inf(t_deviceIndex device_index,
                                 t_bd_addr* p_bd_addr,
                                 t_classDevice device_class);
t_error LMconnection_Handle_Incoming_LM_Connection_Request(t_lmp_link* p_link);

void LMch_CB_Enable_L2CAP_Traffic(t_deviceIndex device_index);
void LMch_Change_Piconet(t_lmp_link *p_link);
void LMch_Set_Next_Change_Piconet_Time(t_slots timeout);
void LMch_Adjust_Next_Change_Piconet_Time(t_slots timeout, boolean extend);
void LMch_Clear_Change_Piconet_Timer(void);

void LMch_Send_LMP_Packet_Type_Table_Req(t_lmp_link *p_link, u_int8 ptt);
void LMconnection_LMP_Remove_Sco_Link_Accepted_Ack(t_deviceIndex device_index);

RAM_CODE void LMch_Enable_L2CAP_Traffic(t_lmp_link* p_link, u_int8 l2cap_enable_type);
void LMch_Disable_L2CAP_Traffic(t_lmp_link* p_link, u_int8 l2cap_disable_type);

void LMconnection_Page_Resume(t_lmp_link* p_link);
void LMconnection_Set_Suspend_Timer(u_int8 suspend_time);
/*
 * Combine all L2CAP_EN flags except PTT (where problems ovserved) for now to
 * avoid the risk of introducing any hidden issues...
 */
#if 0
#define LM_CH_L2CAP_EN_AFH      0x01 //0x01
#define LM_CH_L2CAP_EN_PTT      0x02
#define LM_CH_L2CAP_EN_MSS      0x01 //0x04
#define LM_CH_L2CAP_EN_FLUSH    0x01 //0x08
#define LM_CH_L2CAP_EN_PARK     0x01 //0x10
#define LM_CH_L2CAP_EN_SNIFF    0x01 //0x20
#define LM_CH_L2CAP_EN_HOLD     0x01 //0x40
#define LM_CH_L2CAP_EN_ENC      0x01 //0x80
#define LM_CH_L2CAP_EN_ALL      0xFF
#else
#define LM_CH_L2CAP_EN_AFH      0x01 //0x01
#define LM_CH_L2CAP_EN_PTT      0x02
#define LM_CH_L2CAP_EN_MSS      0x04
#define LM_CH_L2CAP_EN_FLUSH    0x08
#define LM_CH_L2CAP_EN_PARK     0x10
#define LM_CH_L2CAP_EN_SNIFF    0x20
#define LM_CH_L2CAP_EN_HOLD     0x40
#define LM_CH_L2CAP_EN_ENC      0x80
#define LM_CH_L2CAP_EN_ALL      0xFF
#endif

#endif
