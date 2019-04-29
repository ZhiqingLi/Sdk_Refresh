#ifndef _PARTHUS_LMP_LINK_QOS_
#define _PARTHUS_LMP_LINK_QOS_

/******************************************************************************
 * MODULE NAME:    lmp_link_qos.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_link_qos.h,v 1.30 2009/07/17 16:27:25 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_types.h"
#include "lmp_const.h"
#include "lmp_types.h"
#include "hc_event_gen.h"


void LMqos_Send_HC_Packet_Type_Changed_Event(t_lmp_link* p_link,t_error outcome);
t_error LMqos_LM_Validate_Change_Packet_Type(u_int16 handle, t_HCIpacketTypes packet_types);
t_error LMqos_LM_Change_Validated_Packet_Type(u_int16 handle, t_HCIpacketTypes packet_types);
t_error LMqos_LM_Change_Packet_Type(u_int16 handle,t_HCIpacketTypes packet_types);
t_error LMqos_LM_Change_SCO_Packet_Type(u_int16 handle,t_HCIpacketTypes packet_types);
t_error LMqos_LM_Preferred_Rate(t_lmp_link* p_link,u_int8 rate);
t_error LMqos_LM_Write_Supervision_Timeout(t_lmp_link* p_link, t_slots timeout,
                                                  t_cmd_complete_event* p_cmd_complete_info);
t_error LMqos_LM_Read_Supervision_Timeout(t_lmp_link* p_link,
                                               t_cmd_complete_event* p_cmd_complete_info);
t_error LMqos_LM_QOS_Setup(t_lmp_link* p_link,u_int8 service,u_int32 tk_rate,
                                  u_int32 pk_bandwidth,u_int32 latency,u_int32 delay);

void LMqos_Initialise(void);
t_error LMqos_LMP_Preferred_Rate(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMqos_LMP_Auto_Rate(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMqos_LMP_Set_Supervision_Timeout(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMqos_LMP_Supervision_Timeout(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMqos_LMP_Quality_of_Service_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMqos_LMP_Quality_of_Service(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMqos_LMP_Max_Slot(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMqos_LMP_Max_Slot_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMqos_LMP_Accepted(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMqos_LMP_Not_Accepted(t_lmp_link* p_link, u_int16 rejected_opcode, t_error reason);

void    LMqos_Write_QoS(t_lmp_link* p_link, u_int16 poll_interval,u_int8 num_bcast);
t_error LMqos_Update_QoS(t_lmp_link* p_link, u_int16 poll_interval,u_int8 num_bcast);

#if (PRH_BS_DEV_EXTENDED_QOS_NOTIFICATIONS_SUPPORTED==1)
void LMqos_Notify_QOS_All_Slaves(boolean enable);
boolean LMqos_Is_LMP_QOS_Notification_Ack_Pending(void);
void LMqos_Clear_LMP_QOS_Notification_Ack_Pending(t_deviceIndex device_index);
void LMqos_Clear_LMP_QOS_Notification_Ack_Pending_All_Slaves(void);
#endif

void LMpolicy_LMP_Supervision_Timeout_Ack(t_deviceIndex device_index);

u_int8 LMqos_Convert_To_Max_Slots(t_HCIpacketTypes packet_types);
void LMqos_Channel_Quality_Timeout(t_lmp_link* p_link);

void LMqos_Update_Allowed_Packets(t_lmp_link* p_link,u_int8 max_slots,t_rate rate);
t_error LMqos_LM_Force_Max_Slots_In(t_lmp_link* p_link, u_int8 max_slots);
t_error LMqos_LM_Max_Slot_Req(t_lmp_link* p_link, u_int8 max_slots);

t_packetTypes LMqos_LM_Gen_Allowed_Packet_Types(t_HCIpacketTypes acl_packet_types);
//#if (BUILD_TYPE==UNIT_TEST_BUILD)
t_HCIpacketTypes LMqos_LM_Get_HCI_ACL_Packet_Types(t_packetTypes allowed_packet_types);
//#endif


#if (PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)
t_error LMqos_Send_LMP_Preferred_Rate(t_lmp_link* p_link,t_rate rate);
#endif

#if (PRH_BS_CFG_SYS_LM_AUTO_RATE_SUPPORTED==1)
t_error LMqos_LM_Auto_Rate(t_lmp_link* p_link);
#else
#define LMqos_LM_Auto_Rate(p_link)
#endif

t_error LMqos_LM_Flow_Specification(u_int8 direction, t_lmp_link* p_link,u_int8 service,
                                    u_int32 tk_rate, u_int32 tk_bucket_rate, u_int32 pk_bandwidth,
                                    u_int32 latency);

void LMqos_Adjust_Supervision_Timeout_On_Link(t_lmp_link* p_link,
         t_LM_context context, u_int16 interval);


#endif

