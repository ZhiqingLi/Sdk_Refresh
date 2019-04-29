#ifndef _PARTHUS_LM_LINK_POLICY_
#define  _PARTHUS_LM_LINK_POLICY_

/******************************************************************************
 * MODULE NAME:   lmp_link_policy.h 
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:    Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_link_policy.h,v 1.61 2014/03/11 03:14:05 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_types.h"
#include "lmp_acl_connection.h"
#include "lc_interface.h"
#include "config.h"

void LMpol_Initialise(void);

/*****************************************
 * Lower layer inerface to the link policy
 *****************************************/

RAM_CODE t_devicelink* LMpol_Get_Next_Master_Tx_Activity(void);
RAM_CODE t_devicelink* LMpol_Is_SCO_Next_Activity(void);
RAM_CODE void LMpol_Get_Next_SCO_Tx_Activity(t_devicelink* );

boolean LMpol_Get_Next_Slave_Tx_Activity(t_deviceIndex device_index);
boolean LMpol_Get_Next_Slave_Rx_Activity(t_deviceIndex device_index);

t_devicelink* LMpol_Get_Next_Master_Beacon_Tx(void);

boolean LMpol_Get_Next_Slave_Park_Activity(t_lmp_link* p_link);

t_error LMpol_Max_Slots_Allowed(t_lmp_link* p_link, u_int8 max_slots);
t_error LMpol_Device_Activity(t_LC_Event_Info* lc_event_info);

t_error LMpolicy_Write_Flush_Timeout(t_lmp_link* p_link,u_int16 timeout);
void LMpol_Flush_Timeout(t_lmp_link* p_link);
t_error   LMpol_Flush_Link_Buffer(t_lmp_link* p_link);
void LMpol_Scan_All_Links_For_Pending_Flushes(void);
void LMpol_Scan_For_Pending_Broadcast(void);
void LMpol_Flow_Stop(t_deviceIndex device_index,void (*callback)(t_deviceIndex));
void LMpol_Flow_Start(t_deviceIndex device_index,void (*callback)(t_deviceIndex));
void LMpol_Init_SCO(t_lmp_link* p_link,u_int8 sco_index);
void LMpol_Ack_Last_Tx_Packet(t_deviceIndex device_index);
t_error LMpol_HCI_Flush(t_lmp_link* p_link);
#if(PRH_BS_CFG_SYS_LMP_NONFLUSHABLE_PBF_SUPPORTED==1)
t_error LMpol_HCI_Enhanced_Flush(t_lmp_link* p_link);
t_error   LMpol_Flush_Auto_Link_Buffer(t_lmp_link* p_link);
#endif
void LMpol_Deactivate_SCO(t_lmp_link* p_link, u_int8 sco_index);
void LMpol_LM_SSR_Initiate(t_lmp_link* p_link);
void LMpol_LM_SSR_Timeout(t_lmp_link* p_link);
void LMpol_Init_Sniff_Subrating_Instant(t_lmp_link* p_link, u_int32 ssr_instant);
void LMpol_Sleep(u_int16 interval,t_lmp_link* p_link, u_int32 clock);
void LMpol_Decr_Links_In_Low_Power(void);
void LMpol_Adjust_Wakeup_Slot_Time(t_lmp_link *p_link, u_int32 instant);
#define LMpol_Is_Wakeup_Interval(interval)\
(LMpol_Get_Interval_To_Next_Wakeup() < ((t_slots)(interval)))
t_slots LMpol_Get_Interval_To_Next_Wakeup(void);
void LMpol_Service_Imminent_Wakeup_Control(void);
u_int32 LMpol_Get_Common_Subrate(t_lmp_link* p_link);

void LMpol_Scan_All_Links_For_Pending_Flushes(void);

#define LMP_LOW_POWER_STATE    (LMP_HOLD_MODE | LMP_SNIFF_MODE | LMP_PARK_MODE)
#define LMP_SECURITY_STATE     (W4_AUTHENTICATION | W4_ENCRYPTION)
#define LMP_HOST_ACCEPT        (W4_HOST_ACL_ACCEPT | W4_HOST_SCO_ACCEPT)
#define LMP_ACL_TX             (LMP_ACTIVE | W4_PAGE_OUTCOME | LMP_DETACH_ACK_PENDING | \
                                W4_LMP_ACTIVE | LMP_HOST_ACCEPT | LMP_SECURITY_STATE )

#define mPol_Check_Master_Slave_Switch(policy)    ( policy & LINK_POLICY_SWITCH_ENABLED )
#define mPol_Check_Hold_Mode(policy)              ( policy & LINK_POLICY_HOLD_ENABLED   )
#define mPol_Check_Sniff_Mode(policy)             ( policy & LINK_POLICY_SNIFF_ENABLED  )
#define mPol_Check_Park_Mode(policy)              ( policy & LINK_POLICY_PARK_ENABLED )
#define mPol_Check_Automatic_Unpark_Mode(policy)  ( policy & LINK_POLICY_AUTOMATIC_UNPARK_ENABLED )
#define mPol_Check_Automatic_Rehold_Mode(policy)  ( policy & LINK_POLICY_AUTOMATIC_REHOLD_ENABLED )

#define mPol_Check_Hold_Occured(previous_modes)   ( previous_modes & LINK_POLICY_HOLD_ENABLED  )
#define mPol_Check_Sniff_Occured(previous_modes)  ( previous_modes & LINK_POLICY_SNIFF_ENABLED )
#define mPol_Check_Park_Occured(previous_modes)   ( previous_modes & LINK_POLICY_PARK_ENABLED  )

#define mPol_Set_Hold_Occured(previous_modes)   ( previous_modes |= LINK_POLICY_HOLD_ENABLED  )
#define mPol_Set_Sniff_Occured(previous_modes)  ( previous_modes |= LINK_POLICY_SNIFF_ENABLED )
#define mPol_Set_Park_Occured(previous_modes)   ( previous_modes |= LINK_POLICY_PARK_ENABLED  )

#define mLMpol_Is_SCO_Packet(pkt)               (pkt>=HV1 && pkt<=DV)
#define mLMpol_Is_Transmission_State(state)     ( state & (LMP_ACL_TX))
#define mLMpol_Is_Low_Power_State(state)        ( state & (LMP_LOW_POWER_STATE))

#define mLMpol_Get_Clk_27_1(current_clk)         ((current_clk & 0x0FFFFFFF) >> 1)
#define mLMpol_Get_Clk_26_1(current_clk)         ((current_clk & 0x07FFFFFF) >> 1)
#define mLMpol_Get_Clk_27(current_clk)           (((current_clk & 0x0FFFFFFF) >> 1) &  0x04000000)
#define mLMpol_Get_Sco_Q_Id(sco_handle)          ((sco_handle-1))
#define mLMpol_Get_Sco_Packet_Length(sco_packet) ((sco_packet+1)*10)
#define mLMpol_Get_BB_Packet_Type(sco_packet)    ((t_packet)(sco_packet + 5))
#define mLMpol_Clk_2_Slots(clk)                  ((clk  & 0xFFFFFFFF) >> 1)
#define mLMpol_Get_Timing_Ctrl(device_index)     ((LC_Get_Piconet_Clock(device_index) & 0x08000000) >> 27);

void LMpol_Adjust_ACL_Packet_Types_On_All_Links(t_LM_context context);
void LMpol_Adjust_Poll_Position_On_All_Links(s_int16 poll_position);

void LMpol_Adjust_ACL_Packet_Types_On_Link(t_lmp_link* p_link, t_LM_context context);
boolean LMpol_Sniff_Activation_On_Next_Frame(void);

#if (PRH_BS_CFG_SYS_SCATTERNET_SUPPORTED==1)
void LMpol_Change_Piconet(t_deviceIndex pending_device, t_slots timeout);
#endif

#if (PRH_BS_CFG_SYS_LMP_ERRONOUS_DATA_REPORTING_SUPPORTED == 1) && (PRH_BS_CFG_SYS_SYN_VIA_HCI_SUPPORTED==1)
boolean LMpol_Retransmission_Allowed(t_sco_info *p_sco_link,t_devicelink* pDL, t_clock  current_clk); 
#endif

t_slots LMpol_Check_Slots_To_Next_Classic_Activity(u_int32 current_clk);
t_slots LMpol_Check_Slots_To_Next_Classic_High_Priority_Activity(u_int32 current_clk);
#ifdef BT_DUALMODE

boolean LMpol_BLE_Check_Need_Switch_To_Classic_Bt(void);
boolean LMpol_BLE_Check_BT_Is_Scan(void);
#endif
#endif

