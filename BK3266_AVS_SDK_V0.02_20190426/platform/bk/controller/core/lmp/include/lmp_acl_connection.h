#ifndef __LM_ACL_CONNECTION
#define __LM_ACL_CONNECTION

/******************************************************************************
 * MODULE NAME:    lm_acl_connection.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_acl_connection.h,v 1.121 2013/09/24 15:05:38 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_config.h"    
#include "lmp_sec_types.h"
#include "lmp_types.h"
//#include "lmp_ecc.h"


enum e_lm_acl_state {
       LMP_ACTIVE                 = 0x0001, 
       LMP_HOLD_MODE              = 0x0002,
       LMP_SNIFF_MODE             = 0x0004, 
       LMP_PARK_MODE              = 0x0008,
       LMP_IDLE                   = 0x0010,
       W4_PAGE_OUTCOME            = 0x0020,
       W4_HOST_ACL_ACCEPT         = 0x0040,
       W4_HOST_SCO_ACCEPT         = 0x0080,
       W4_AUTHENTICATION          = 0x0100,
       W4_ENCRYPTION              = 0x0200,
       LMP_W4_SUPERVISION_TIMEOUT = 0x0400,
       LMP_DETACH_ACK_PENDING     = 0x0800,
       W4_LMP_ACTIVE              = 0x1000,
       W4_FEATURES_RES            = 0x2000,
	   W4_HOST_ESCO_ACCEPT        = 0x4000,
	   W4_SNIFF_EXIT			  = 0x8000
};
/*
 * Note: there is a coupling between the
 * size of the largest enum in the list above
 * and the size of t_lm_acl_state below.
 */
typedef u_int16 t_lm_acl_state;

typedef u_int32  t_policy_settings;

/*******************************************
 *  Start of the Code
 *  
 *******************************************/

typedef struct t_lmp_link 
{    
   /*************************
    * normal ACL information
    *************************/
    /*
     * Encryption key used in baseband, for optimisation must be full word aligned
     */
    t_link_key encry_key;

    t_lm_acl_state  state;
    t_deviceIndex device_index;
    t_role role;
    t_rate rate;
    t_rate peer_rate;
    t_rate preferred_rate;
    t_bd_addr bd_addr;
    t_classDevice device_class;

    t_packetTypes allowed_pkt_types;
    u_int16 proposed_packet_types;
    u_int16 proposed_poll_interval;

    u_int16 operation_pending;
    u_int8  remote_features[8];

    u_int8  context;
    u_int8  setup_complete_bit_mask;
	u_int8  host_connection_request_sent;
	u_int8  host_connection_request_accepted;
    u_int8  features_complete_bit_mask;
    u_int8  index; /* Index into the ACL container */
    u_int8  Nbc;
    t_connectionHandle handle; /* Handle for the ACL Link */

   /***********************************
    * tid_tole_last_rcvd_pkt
    *      ROLE_TID(0):      transaction initiated locally
    *      PEER_ROLE_TID(1): transaction initiated by peer
    * current_proc_tid_role
    *      role of currently initiated procedure, 
    *      determines whether proc. initiated locally or by peer
    ***********************************/

    u_int8 tid_role_last_rcvd_pkt; 
    u_int8 current_proc_tid_role;  
    u_int8 sec_current_proc_tid_role;
     
   /***********************************
    * normal ACL Security Information
    ***********************************/
    t_sec_super_state auth_super_state;
    t_sec_sub_state  auth_sub_state;

    t_sec_super_state encr_super_state;
    t_sec_sub_state  encr_sub_state;
    t_link_key link_key; 
    u_int8 link_key_exists;
    t_link_key_type key_type ;          /* COMBINATION_KEY | LOCAL_UNIT_KEY | REMOTE_UNIT_KEY */
    t_link_key_life link_key_persistance;     /* Semi-Permanent or Temporary */
    u_int8 pin_code_length;
    t_pin_code pin_code;
    u_int8 encrypt_key_size;
    u_int8 rand[16];
    u_int8 local_combkey_part[16];
    t_sres sres;
    t_aco aco;
    t_encrypt_mode encrypt_mode;
    t_encrypt_mode restore_current_encrypt_mode; /* 2825 */
    t_encrypt_enable encrypt_enable;
#if (PRH_BS_CFG_SYS_LMP_PAUSE_ENCRYPTION_SUPPORTED==1)
    u_int8 encrypt_paused;
    u_int8 event_status;
    u_int8 encryption_key_refresh_index;
#endif
    t_lm_policy_info pol_info;
    u_int8 switch_state;
   /********************************
    * Indicator of the current power
    * level in the peer 
    ********************************/

    t_power_level  peer_power_status;
    u_int8 max_power_level;
    volatile boolean peer_power_req_tx_pending;
    s_int8 peer_power_counter;
    
   /******************************
    *  QoS Value for an LMP link. 
    ******************************/
    u_int8 service_type;
#if (PRH_BS_CFG_SYS_QOS_SUPPORTED==1)
    u_int32 token_rate;
    u_int32 peak_bandwidth;
    u_int32 latency;
    u_int32 delay_variation;

    u_int8  proposed_service_type;
    u_int32 proposed_token_rate;
    u_int32 proposed_peak_bandwidth;
    u_int32 proposed_latency;
    u_int32 proposed_delay_variation;
#endif

#if (PRH_BS_CFG_SYS_FLOW_SPECIFICATION_SUPPORTED==1)
    u_int8 direction;

    u_int8 in_service_type;
    u_int32 in_token_rate;
    u_int32 in_peak_bandwidth;
    u_int32 in_latency;
    u_int32 in_token_bucket_size;

    u_int8  in_proposed_service_type;
    u_int32 in_proposed_token_rate;
    u_int32 in_proposed_peak_bandwidth;
    u_int32 in_proposed_latency;
    u_int32 in_proposed_token_bucket_size;

    u_int8  out_service_type;
    u_int32 out_token_rate;
    u_int32 out_peak_bandwidth;
    u_int32 out_latency;
    u_int32 out_token_bucket_size;

    u_int8  out_proposed_service_type;
    u_int32 out_proposed_token_rate;
    u_int32 out_proposed_peak_bandwidth;
    u_int32 out_proposed_latency;
    u_int32 out_proposed_token_bucket_size;

    t_slots transmission_interval;
    s_int16 transmission_position;

    u_int8 flow_spec_pending;
    u_int16 proposed_tx_interval;

    u_int8 out_qos_active;
    u_int8 in_qos_active;
#endif

   /*****************
    *  Link timeouts 
    *****************/
   u_int16 channel_quality_timeout;
   u_int16 flush_timeout;
   u_int16 supervision_timeout; /* negotiated by LM */
   u_int16 link_supervision_timeout; /* configured by HCI */
   u_int8 flush_timeout_index;
   u_int8 flush_execut_timeout_index;
   u_int8 supervision_timeout_index;
   u_int8 recent_contact_timeout_index;
   u_int8 channel_quality_timeout_index;
#if (LMP_ESCO_LINK_REQ_TIMEOUT == 1)
   u_int8 lmp_esco_req_timeout_index;
#endif

#ifdef LMP_LINK_L2CAL_TIMEOUT_ENABLE
   u_int8 l2cap_timeout_index;
#endif
#if (PRH_BS_CFG_SYS_LMP_MSS_SUPPORTED==1)
   u_int8  allow_switch;
   u_int8 change_link_for_role_switch_timer_index;
#endif
   u_int8 flush_status;
   u_int8 flush_type;
   u_int8 queue_flush_pending;
   u_int8 sched_queue_flush_pending;
   u_int8 failed_contact_counter;
   u_int8 flush_packet_type;  // Indicates AUTO_FLUSHABLE / NON_AUTO_FLUSHABLEz
   u_int8 enhanced_queue_flush_pending;
   /***************************
    *  Link Policy Information 
    ***************************/

    t_HCIpacketTypes  packet_types;  /* Master Only */
    t_slots poll_interval;
    s_int16 poll_position;

    u_int16 l2cap_pkt_len_pending;
    u_int16 current_l2cap_pkt_length;
    u_int8  previous_modes; /* Stores the previous modes [ Hold/Sniff/Park ] a link has entered */
    u_int8 packet_type_changed_event;
    u_int8 max_slots_out;
    u_int8 max_slots_out_pending;
    u_int8 max_slots_in;
    u_int8 prev_slots_tx;
    u_int8 l2cap_ack_pending;

    /*************************
     * Timing Related Aspects 
     *************************/
    u_int16 slot_offset;

    u_int8 gen_detach_timeout_index;
    u_int8 gen_switch_timeout_index;
    u_int8 gen_security_timeout_index;

    u_int16 link_policy_mode;
    u_int8 msg_timer;
#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)
    t_deviceIndex park_device_index;
    u_int8 num_park_negotiations;
    u_int8 automatic_park_enabled;

#if (PRH_BS_DEV_SLAVE_FORCE_STOP_AUTOPARK_SUPPORTED==1)
    u_int8 force_stop_autopark;
#endif
#endif
    t_packet default_pkt_type;
    t_error detach_reason;
    t_error disconnect_req_reason;
    u_int8 l2cap_tx_enable;
    u_int8 link_quality;
    u_int8 sec_timer;
    u_int16 bcast_enc_key_size_mask;
    u_int8 ptt_req;
	u_int8 connection_request_send_via_hci;

#if (PRH_BS_CFG_SYS_LMP_SECURE_SIMPLE_PAIRING_SUPPORTED==1)
	u_int8 peer_public_key_x[24];
	u_int8 peer_public_key_y[24];
	u_int8 Incoming_Encapsulated_P192_len_pending;
	u_int8 ssp_initiator;
	u_int8 peer_commitment[16];
	u_int8 peer_random[16];
	u_int8 DHkey[24];
	u_int8 DHkeyCheck[16];
	u_int8 peer_key_check[16];

	u_int8 DHkeyCheckComplete;
	u_int8 DHkeyCalculationComplete;
#if (PRH_BS_CFG_SYS_SSP_OOB_SUPPORTED==1)
	u_int8 oob_data_present;
#endif
	u_int8 auth_requirements;
	u_int8 io_cap;
	u_int8 auth_type;
	u_int8 ssp_tid;
	u_int8 peer_user_authenticated;
	u_int8 local_user_authenticated;
#if (PRH_BS_CFG_SYS_SSP_PASSKEY_SUPPORTED==1)
	u_int32 passKey;
	u_int8 passKeyCount;
#endif
	u_int8 Rpeer[16];
#if (PRH_BS_CFG_SYS_SSP_OOB_SUPPORTED==1)
	u_int8 local_oob_verified;
#endif
	u_int8 rxed_simple_pairing_number;
	u_int8 ssp_host_support;
	u_int8 ssp_debug_mode;
	u_int8 ssp_hci_timer_index;
	u_int8 peer_user_dh_key_check_rxed;
#endif
	boolean  call_lc_wakeup;
    u_int8 LSLCacc_last_rx_LMP_msg;
	
	u_int8 remote_features_pg1[1];
	u_int8 remote_features_pg2[2];
	

} t_lmp_link;

#define PTT_REQ_IDLE 0xFF
#define PTT_REQ_PENDING 0x70

void LMacl_Initialise(t_lmp_link* p_link);
void LMacl_Write_CombKey_Part(t_lmp_link *p_link, u_int8 *local_comb_key_part);
void LMacl_Write_Bd_Addr(t_lmp_link* current_link, t_bd_addr* p_bd_addr);

void LMacl_Add_Sco(t_lmp_link* p_link,t_sco_info* p_sco_link);
void LMacl_Remove_Sco(t_lmp_link* p_link,t_sco_info* p_sco_link);
u_int8 LMacl_Sco_Exists(t_lmp_link* p_link);

void LMacl_Set_Key_Persistance(t_lmp_link* p_link, t_link_key_life key_persistance);
t_link_key_life LMacl_Get_Current_Key_Persistance(t_lmp_link* p_link);
t_link_key_life LMacl_Get_Previous_Key_Persistance(t_lmp_link* p_link);

#endif

