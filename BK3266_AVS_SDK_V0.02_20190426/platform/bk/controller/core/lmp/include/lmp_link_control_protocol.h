#ifndef _PARTHUS_LMP_LINK_CONTROL_
#define  _PARTHUS_LMP_LINK_CONTROL_

/******************************************************************************
 * MODULE NAME:   lmp_link_control_protocol.h 
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:    Gary Fleming
 * CREATION DATE:     
 *
 * SOURCE CONTROL: $Id: lmp_link_control_protocol.h,v 1.13 2009/06/03 15:41:43 morrisc Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_types.h"
#include "lmp_types.h"
#include "lmp_const.h"
#include "lmp_acl_connection.h"
#include "hc_event_gen.h"


t_error LMlc_LM_Clock_Offset_Req(t_lmp_link* p_link);
t_error LMlc_LP_Inc_Power(t_lmp_link* p_link, u_int8 power_units);
t_error LMlc_LP_Dec_Power(t_lmp_link* p_link, u_int8 power_units);

t_error LMlc_LM_Inc_Peer_Power(t_lmp_link* p_link, u_int8 power_units);
t_error LMlc_LM_Dec_Peer_Power(t_lmp_link* p_link, u_int8 power_units);
t_error LMlc_LM_Read_Transmit_Power_Level(t_lmp_link* p_link, u_int8 power_type,
                                                  t_cmd_complete_event* p_cmd_complete);
#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
t_error LMlc_LMP_Power_Control_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMlc_LMP_Power_Control_Resp(t_lmp_link* p_link,t_p_pdu p_payload);
#endif
/* It is assumed that the Clock Offset Req is triggered from the HCI  */
#ifdef NEW_DECODE_ROUTINE
t_error LMlc_LMP_Decode(t_lmp_link* p_link,t_p_pdu p_payload);
#else
t_error LMlc_LMP_Clock_Offset_Req(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMlc_LMP_Clock_Offset_Res(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMlc_LMP_Slot_Offset(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMlc_LMP_Timing_Accuracy_Req(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMlc_LMP_Timing_Accuracy_Res(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMlc_LMP_Supervision_Timeout(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMlc_LMP_Test_Activate(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMlc_LMP_Test_Control(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMlc_LMP_Incr_Power_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMlc_LMP_Decr_Power_Req(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMlc_LMP_Max_Power(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMlc_LMP_Min_Power(t_lmp_link* p_link,t_p_pdu p_payload);
t_error LMlc_LMP_Page_Mode_Req(t_lmp_link* p_link, t_p_pdu p_pdu);
t_error LMlc_LMP_Page_Scan_Mode_Req(t_lmp_link* p_link, t_p_pdu p_pdu);
t_error LMlc_LMP_Not_Accepted(t_lmp_link* p_link, u_int16 opcode, t_error reason);

#if (PRH_BS_CFG_SYS_LMP_TIMING_INFO_SUPPORTED==1)
t_error LMlc_LM_Timing_Accuracy_Req(t_lmp_link* p_link);
#else
#define LMlc_LM_Timing_Accuracy_Req(p_link)
#endif

#endif

#endif

