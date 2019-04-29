#ifndef _PARTHUS_LM_CONFIG_
#define _PARTHUS_LM_CONFIG_

/******************************************************************************
 * MODULE NAME:    lmp_config.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_config.h,v 1.122 2014/09/16 06:52:27 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_config.h"

#include "lmp_const.h"
#include "lmp_types.h"
#include "lmp_sec_types.h"
#include "lmp_acl_connection.h"

#include "hc_event_gen.h"

/*
 * t_LMconfig g_LM_config_info;
 *
 * Define the lmp data structure to hold all global LMP variables
 */
typedef struct 
{   
    t_lm_callback lmp_event[_LM_MAX_NUM_EVENTS];

    t_string device_name;

#if (PRH_BS_CFG_SYS_REMOTE_NAME_REQUEST_SUPPORTED==1)
    t_string remote_name;
#endif
    u_int32 beacon_instant;
    t_slots accept_timeout;
    u_int8  accept_timeout_index;
    u_int16  operation_pending;
    t_link_key master_key;
    t_rand bcast_encr_rand;
    t_lm_acl_state  state;
    t_slots page_timeout;

    /* Activity Modes of the device
     * Currently not used 
     */
    t_role role;
    u_int8 hold_mode_activity;
    u_int8 num_acl_link_on_hold;

   /* 
    * Number of broadcast Re-Transmissions 
    */
    u_int8 Nbc;

    /* 
     * Security Info 
     */
    t_link_key_type key_type;
    t_pin_type pin_type;
    u_int8 max_encrypt_key_size;
    u_int8 min_encrypt_key_size;
    t_flag authentication_enable;
    t_encrypt_mode  encryption_mode;
    u_int16 bcast_enc_key_size_mask; 
#if (PRH_BS_CFG_SYS_LMP_SECURE_SIMPLE_PAIRING_SUPPORTED==1)
	u_int8 ssp_enabled;
	u_int8 ssp_debug_mode;
#endif
    /*
     * ACL connection related 
     */
    u_int8 new_connections_allowed;
    u_int8 num_acl_links;
    u_int8 num_slave_acl_links;
    u_int8 num_master_acl_links;
    /*
     * SCO connection related
     */
    u_int8 num_slave_sco_links;
    u_int8 num_master_sco_links;

    /* 
     * The Broadcast Connection Handle 
     */
    u_int16 active_broadcast_handle;
    u_int16 piconet_broadcast_handle;

    u_int8 num_bcast_retry;
    u_int8 current_bcast_type;

#if (PRH_BS_CFG_SYS_LMP_HOLD_SUPPORTED==1)
    u_int16 max_hold_time;
    u_int16 min_hold_time;
#endif

    u_int16 num_links_parked;

#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)
    t_lm_park_info park_info;
#if (PRH_BS_CFG_SYS_SCATTERNET_SUPPORTED==1)
    t_piconet_index role_in_park; /* As a scatternet limitation */
#endif
    u_int16 T_bcast;
    u_int16 D_bcast;
    u_int8  N_bcast;
    u_int32 end_beacon;
    u_int32 next_acc_window;  /* The instance of the next access window  */
    u_int32 start_acc_window;
    u_int32 end_acc_window;
    u_int8 T_access;
    u_int8 D_access;
    u_int8 N_access;
    u_int8 M_access;
    u_int8 M_access_rem; /* The number of Access Windows remaining    */
    u_int8 access_scheme;
    u_int8 N_poll_rem;    /* The number of Poll Slots remaining       */
    u_int8 N_poll;        
    u_int8 N_sleep;
    u_int8 D_sleep;
    u_int16 unpark_handle;    /* The handle for the ACL link currently being unparked */
    u_int16 bcast_scan_ext;   /* Broadcast Scan Window Extension */
	u_int8 bcast_scan_ext_unpark;
    u_int8 park_defaults;
#endif
    u_int16 default_t_poll;
    t_packet default_pkt_type;
    t_packet last_broadcast_pkt_rx;
    u_int8 links_in_low_power;
    t_slots next_wakeup_time;  
    t_lmp_link* next_wakeup_link;

#if (PRH_BS_CFG_SYS_SLEEP_IN_STANDBY_SUPPORTED == 1)
    u_int8 standby_power_timer_index;
#endif

#if (PRH_BS_CFG_SYS_LMP_POWER_CONTROL_SUPPORTED==1) 
    u_int8 peer_power_timer_index;
#endif

#if(PRH_BS_CFG_SYS_HCI_V12_FUNCTIONALITY_SUPPORTED==1)
    u_int16 default_link_policy_mode;
#endif

#if ((PRH_BS_CFG_SYS_LMP_SCO_SUPPORTED==1) || (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1))
    u_int16 default_voice_setting;
#endif

#if(PRH_BS_CFG_SYS_BROADCAST_NULL_IN_INQ_PAGE_SUPPORTED==1)
    u_int16 emergency_poll_interval;
#endif
#if (PRH_BS_CFG_SYS_LMP_SECURE_SIMPLE_PAIRING_SUPPORTED==1)
	u_int8 Outgoing_Encapsulated_P192_len_pending;
	u_int8 public_key_x[24];
	u_int8 public_key_y[24];
	u_int8 secret_key[24];


#if (PRH_BS_CFG_SYS_SSP_OOB_SUPPORTED==1)
	u_int8 oob_data_present;
#endif
	u_int8 auth_requirements;
	u_int8 io_cap;
#if (PRH_BS_CFG_SYS_SSP_PASSKEY_SUPPORTED==1)
	u_int8 passKeyCounter;
#endif
#if (PRH_BS_CFG_SYS_SSP_OOB_SUPPORTED==1)
	u_int8 randomizer_R[16];
	u_int8 hash_C[16];
#endif
	//u_int8 Ra[16];
#endif
	u_int8 default_sco_route_via_hci;
	u_int8 inquiry_tx_power_level;
	u_int8 inquiry_response_tx_power_level;
#if 1 // FIX FOR SSP- Blocking
	u_int8 new_public_key_generation; // Key Generation Status
#endif
#if (PRH_BS_CFG_SYS_LE_CONTROLLER_SUPPORTED==1)
	u_int8 le_host_supported;
	u_int8 le_and_bredr_host;
#endif
	u_int8 SCO_blocking_scatternet;
	u_int8 crystal_calibration;
} t_LMconfig;

extern t_LMconfig g_LM_config_info;

#if (PRH_BS_CFG_SYS_SLEEP_IN_STANDBY_SUPPORTED == 1)
#define LM_CONFIG_SLEEP_IN_STANDBY_MONITOR_PERIOD 0x7FFF /* 20 sec */
#define LM_CONFIG_AWAKE_IN_STANDBY_MONITOR_PERIOD 0x0640
#endif

#define LM_CONFIG_PEER_POWER_MONITOR_PERIOD         0x200  /* 0x80 */

void LMconfig_Initialise(void);
#if (DEBUG_SCATTERNET_MONITORS == 1)
void TC_Read_Local_LMPconfig_Monitors_Via_App(void);
#endif
t_packet LMconfig_Get_Default_Pkt_Type(void);
void LMconfig_Change_Default_Pkt_Type(t_packet pkt_type);

void LMconfig_LM_Read_Local_Name(t_cmd_complete_event* p_event);
void LMconfig_LM_Change_Local_Name(u_int8* p_name);
void LMconfig_LM_Write_Voice_Settings(u_int16 voice_settings);
void LMconfig_LM_Standby_Low_Power_Timeout(t_lmp_link* p_link);

t_error LMconfig_LMP_Not_Accepted(t_lmp_link* p_link, u_int16 opcode, t_error reason);
t_error LMconfig_LM_Read_Remote_Version(t_lmp_link* p_link);
t_error LMconfig_LMP_Version_Req(t_lmp_link* p_link,t_p_pdu p_pdu);
t_error LMconfig_LMP_Version_Res(t_lmp_link* p_link,t_p_pdu p_pdu);
u_int8 LMconfig_PageInq_PreventingScan(void);
/*
 * LMP state functions
 */
__INLINE__ void LMconfig_LM_Clear_Device_State(u_int32 state);
__INLINE__ void LMconfig_LM_Set_Device_State(u_int32 state);
__INLINE__ u_int32 LMconfig_LM_Page_Scans_Allowed(void);
__INLINE__ u_int32 LMconfig_LM_Inquiry_Scans_Allowed(void);
__INLINE__ u_int32 LMconfig_LM_Periodic_Inquiry_Allowed(void);
__INLINE__ u_int32 LMconfig_LM_Check_Device_State(u_int32 query_state);
__INLINE__ u_int32 LMconfig_LM_Enforce_Scans_Disabled(void);

__INLINE__ boolean LMconfig_LM_Connected_As_Master(void);
__INLINE__ boolean LMconfig_LM_Connected_As_Slave(void);
__INLINE__ boolean LMconfig_LM_Connected_As_Scatternet(void);
__INLINE__ u_int8 LMconfig_LM_Num_Piconets(void);
__INLINE__ boolean LMconfig_LM_Connected_As_SCO_Master(void);
__INLINE__ boolean LMconfig_LM_Connected_As_SCO_Slave(void);
#ifdef BT_DUALMODE

__INLINE__ boolean LMconfig_LM_Connected_As_Classic_LE_Scatternet(void);
#endif
/*
 * System based
 */
#define PRH_MAX_ACL_LINKS               PRH_BS_CFG_SYS_MAX_DEVICE_CONTAINERS


/* 
 * Define number of timers available as sum of:
 *      8 Timers for local device +
 *      6 for each link + 
 *      1 per piconet if AFH supported +
 *      5 spare 
 */
#define PRH_MAX_LM_TIMERS                     ( 8 + (PRH_MAX_ACL_LINKS*6)  + \
    ((PRH_BS_CFG_SYS_LMP_AFH_CAPABLE_SUPPORTED!=0)*PRH_BS_CFG_SYS_MAX_PICONETS) + 5 )

#if ((PRH_BS_CFG_SELECT_LEVEL==0) || (PRH_BS_CFG_SELECT_LEVEL==1))
#define PRH_MAX_NAME_LENGTH                   55
#else
/***** URAM reduce  *******/
#if ((defined REDUCE_RAM_USAGE_1) || (defined REDUCE_RAM_USAGE_2)) && (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
#define PRH_MAX_NAME_LENGTH                   124
#else 
#define PRH_MAX_NAME_LENGTH                   248
#endif
/*************************/
#endif

#define PRH_LMP_MSG_TIMEOUT                  (30 * 1600)
#define PRH_SCO_CONNECTION_HANDLE_OFFSET      0x0e00 
#define PRH_ACL_CONNECTION_HANDLE_OFFSET      50

/*
 * Device Security defaults
 */
#define PRH_DEFAULT_MAX_ENCRYPT_KEY_SIZE       16

/*
 * PRH_DEFAULT_MIN_ENCRYPT_KEY_SIZE MUST be at least 1,
 * as the LMP_encryption_key_size_mask_req PDU
 * cannot carry the value of 0.
 */
#define PRH_DEFAULT_MIN_ENCRYPT_KEY_SIZE       1 /* #2786 */
#define PRH_DEFAULT_ENCRYPTION_MODE            ENCRYPT_NONE

/* 
 * Parthus specific Park Defaults 
 */
#if 1 // GF 2 June 2010 - Changed to Defaults in Test Spec
#define PRH_DEFAULT_DELTA_BEACON 2
#define PRH_DEFAULT_N_BEACON    24
#define PRH_DEFAULT_D_ACCESS    (PRH_DEFAULT_N_BEACON * PRH_DEFAULT_DELTA_BEACON + 32)
#define PRH_DEFAULT_N_ACCESS    10
#define PRH_DEFAULT_T_ACCESS    (2*(PRH_DEFAULT_N_ACCESS) + 6)
#define PRH_DEFAULT_T_BEACON   800
#define PRH_DEFAULT_D_BEACON    20
#define PRH_DEFAULT_N_B_SLEEP    1 /* No Sleep */
#define PRH_DEFAULT_D_B_SLEEP    0

#else  // Alternative Park Parameters ! Used during BQB 2 June

#define PRH_DEFAULT_DELTA_BEACON 2
#define PRH_DEFAULT_N_BEACON     4
#define PRH_DEFAULT_D_ACCESS    (PRH_DEFAULT_N_BEACON * PRH_DEFAULT_DELTA_BEACON + 32)
#define PRH_DEFAULT_N_ACCESS    10
#define PRH_DEFAULT_T_ACCESS    (2*(PRH_DEFAULT_N_ACCESS) + 6)
#define PRH_DEFAULT_T_BEACON   0x160
#define PRH_DEFAULT_D_BEACON     0
#define PRH_DEFAULT_N_B_SLEEP    1 /* No Sleep */
#define PRH_DEFAULT_D_B_SLEEP    0



#endif

#define PRH_DEFAULT_N_POLL      32
#define PRH_DEFAULT_M_ACCESS     1
#define PRH_LMP_UNPARK_TIMER    0x100
#define PRH_PARK_BROADCAST_Q     0
#define PRH_DEFAULT_D_SNIFF      0

#if (PRH_BS_DEV_ALIGN_PARK_BEACON_WITH_SYNCHRONOUS_LINKS==1)
/*
 * Align the d beacon with the 4 escos (0,2,4,6)
 */
#undef PRH_DEFAULT_D_BEACON
#define PRH_DEFAULT_D_BEACON    8
/*
 * Align the t beacon with t_esco of 12
 * 720 is 60*t_esco. Chosen as being
 * reasonably close to 800 and as being
 * a nice clean multiple of t_esco.
 */
#undef PRH_DEFAULT_T_BEACON
#define PRH_DEFAULT_T_BEACON   720
/*
 * Choose delta_b to align with a t_esco
 * of 12.
 */
#undef PRH_DEFAULT_DELTA_BEACON
#define PRH_DEFAULT_DELTA_BEACON 12
/*
 * Reduce n_beacon to 12 to allow
 * it to complete in a reasonable
 * time frame
 */
#undef PRH_DEFAULT_N_BEACON
#define PRH_DEFAULT_N_BEACON    12
/*
 * Set up d_access to allow
 * ar_addrs 1,2,3,4, 13,14,15,16
 * be used without clashing with
 * the 4 x eSCO links
 *
 * 188 is n_beacon (12) * delta_beacon (12)
 *  = 144, + 32 guard = 176, + shuffled
 * forward until the periodicity has minimal
 * clash with 4 x eSCOs.
 */
#undef PRH_DEFAULT_D_ACCESS    
#define PRH_DEFAULT_D_ACCESS    180

/*
 * Set up n_access to 4 to
 * prevent park beacon from interfering
 * with 4 x eSCO connections
 *
 * This imposes a limitation of 4 slaves in
 * park at any one time.
 */
#undef PRH_DEFAULT_N_ACCESS
#define PRH_DEFAULT_N_ACCESS    4

#endif
    
#define PRH_DEFAULT_MIN_HOLD_INTERVAL  0x14
#define PRH_HOLD_INSTANT_DELAY  10 /* 10 * Tpoll */
#define PRH_MIN_HOLD_INSTANT_DELAY  0x64 /* Aligned to new instance offset which was increased 
                                            post UPF7 to improve interoperability of hold */

#define PRH_TX_CHN_QUALITY_DM_PACKET_THRESHOLD 0x10  /* One in 0x0010 (or less) Tx Errors causes the Packet Type of DMx to be selected */
#define PRH_TX_CHN_QUALITY_DH_PACKET_THRESHOLD 0x20  /* One in 0x0020 (or greater) Tx errors causes the Packet Type of DHx to be selected */
#define PRH_RX_CHN_QUALITY_DM_PACKET_THRESHOLD 0x10  /* One in 0x0010 (or less) Rx Errors causes the Packet Type of DMx to be selected */
#define PRH_RX_CHN_QUALITY_DH_PACKET_THRESHOLD 0x20  /* One in 0x0020 (or greater) Rx errors causes the Packet Type of DHx to be selected */

#define PRH_CHN_QUALITY_MIN_NUM_PACKETS ((PRH_TX_CHN_QUALITY_DH_PACKET_THRESHOLD+1))

#define PRH_DEFAULT_JITTER  10
#define PRH_DEFAULT_DRIFT   250


#ifndef PRH_BS_CFG_SYS_LINK_KEY_REQUEST_TIMEOUT
#define PRH_BS_CFG_SYS_LINK_KEY_REQUEST_TIMEOUT  0x4000 /* Keep below 4500 */
#endif

#ifndef PRH_BS_CFG_SYS_PIN_CODE_REQUEST_TIMEOUT
#define PRH_BS_CFG_SYS_PIN_CODE_REQUEST_TIMEOUT  0x4500 /* Keep below 4500 */
#endif


#define PRH_DEV_STATE_ALL_SCANS_ALLOWED           0x00000000
#define PRH_DEV_STATE_INQUIRY_PENDING             0x00000001
#define PRH_DEV_STATE_PAGE_PENDING                0x00000002
#define PRH_DEV_STATE_MAX_ACL_CONTAINERS          0x00000004
#define PRH_DEV_STATE_TEST_MODE                   0x00000008
#define PRH_DEV_STATE_LOOPBACK_MODE               0x00000010
#define PRH_DEV_STATE_GUARANTEED_QOS              0x00000020
#define PRH_DEV_STATE_MAX_SCO_BANDWIDTH           0x00000040
#define PRH_DEV_STATE_ACLS_ACTIVE                 0x00000080
#define PRH_DEV_STATE_SCOS_ACTIVE                 0x00000100
#define PRH_DEV_STATE_MSS_ACTIVE                  0x00000200

#define PRH_DEV_STATE_MAX_SLAVE_ACLS              0x00000400
#define PRH_DEV_STATE_MAX_MASTER_ACLS             0x00000800

#define PRH_DEV_STATE_SUSPEND_PAGE_SCANS          0x00001000
#define PRH_DEV_STATE_SUSPEND_INQUIRY_SCANS       0x00002000
#define PRH_DEV_STATE_SUSPEND_PERIODIC_INQUIRY    0x00004000

#define PRH_DEV_STATE_PARK_PENDING                0x00008000
#define PRH_DEV_STATE_UNPARK_PENDING              0x00010000
#define PRH_DEV_STATE_SCO_PREVENTING_SCANS        0x00020000
#define PRH_DEV_STATE_SNIFF_WINDOW_ACTIVE         0x00040000
#define PRH_DEV_STATE_WAKEUP_IMMINENT             0x00080000

#define PRH_DEV_STATE_CREATE_CONN_PENDING         0x00100000
#define PRH_DEV_STATE_CANCEL_CONN_PENDING         0x00200000
#define PRH_DEV_STATE_RNR_CONN_PENDING            0x00400000
#define PRH_DEV_STATE_RNR_CANCEL_PENDING          0x00800000

#define PRH_DEV_STATE_TRUNCATED_PAGE_PENDING      0x01000000
#define PRH_DEV_STATE_W4_ROLE_SWITCH              0x02000000

#ifdef __USE_INLINES__
#include "lmp_config_impl.h"
#endif

#endif

