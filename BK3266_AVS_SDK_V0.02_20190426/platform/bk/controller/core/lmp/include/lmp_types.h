#ifndef _PARTHUS_LMP_TYPES_
#define _PARTHUS_LMP_TYPES_

/******************************************************************************
 * MODULE NAME:    lmp_types.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_types.h,v 1.90 2014/03/11 03:14:05 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_config.h"
#include "sys_types.h"
#include "lmp_sec_types.h"
#include "lmp_const.h"
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1)
#include "tra_queue.h"
#endif
#include "sbc.h"
#include "config.h"

typedef enum { OFF = 0x00, ON = 0x01 } t_flag;

typedef u_int8 t_lmp_pdu_q[MAX_NUM_LMP_PDUS][MAX_LMP_PDU_SIZE];

typedef struct {
    /**************************
     * Link Hold Informaton
     **************************/
#if (PRH_BS_CFG_SYS_LMP_HOLD_SUPPORTED==1)
    u_int32 hold_instant;            /* Instant at which the hold should occur */
    u_int16 max_hold_time;           /* Max hold time - Upper bound received via HCI  */
    u_int16 min_hold_time;           /* Min hold time - Lower bound received via HCI  */
    u_int16 max_previous_hold;       /* Max time the link was previously put on hold for
                                      * - used to determine if forced hold possible 
                                      */

    u_int16 hold_time;               /* The length of time the device is to go on hold for */
    u_int8 num_hold_negotiations;    /* The max number of rounds of negotiations which take place
                                      * during a negotiated hold
                                      */
    u_int8 hold_timeout_index;       /* An identifier for the hold timer */
#else
    u_int8 dummy;
#endif
 } t_lm_hold_info;

typedef struct {
#if (PRH_BS_CFG_SYS_LMP_SNIFF_SUPPORTED==1)
    t_timer end_current_sniff_window;
    t_timer next_sniff_window;
    u_int16 D_sniff;
    u_int16 T_sniff;
    u_int16 T_sniff_min;
    u_int16 T_sniff_max;
    u_int16 N_sniff;
    u_int16 N_sniff_rem;
    u_int16 sniff_timeout;
#if (PRH_BS_CFG_SYS_LMP_SNIFF_SUBRATING_SUPPORTED==1)
    u_int8 sniff_subrating_req_pending;
    u_int16 max_latency;
    u_int16 min_local_timeout;
    u_int16 min_remote_timeout;
    u_int32 sniff_subrating_instant;
    u_int8 local_sniff_subrate;
    u_int8 max_remote_sniff_subrate;
    u_int16 new_local_sniff_timeout;
    u_int16 local_sniff_timeout;
    u_int16 min_remote_sniff_timeout;
    u_int32 next_subrate_window;
    u_int8 subrate_state;
    u_int8 subrate_timeout_index;
#endif
    u_int8 sniff_state;
    u_int8 num_sniff_negotiations;
    u_int8 timing_ctrl;
    //u_int8 sniff_rx_status;
#else
    u_int8 dummy;
#endif
 } t_lm_sniff_info; 

typedef struct {
#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)
    u_int8  delta_bcast;      /* Global only                        */
    u_int8  pm_addr;          /* PM Address of the slave - link specific */
    u_int8  ar_addr;          /* AR Address of the slave - link specific */
    u_int8  am_addr;          /* Link specific            */
    u_int8  park_state;       
    t_deviceIndex  device_index;
    t_deviceIndex  unpark_device_index;
    t_deviceIndex  park_device_index;
    u_int8  unpark_pending; /* Flag to indicate if there is an unpark pending - link specific           */
    u_int8 timing_ctrl;    /* the timing control to be used in the park   - can be global */
    u_int8 unpark_pdu_rx;  /* Indicates if an LMP_Unpark PDU has been received - link specific */
    u_int8 num_unparks_attempts; /* Number of attempts the master will try to unpark */
    t_timer next_unpark_timer;
    t_timer next_park_timer;
    u_int8 mode_change;
    u_int8 unpark_type;
    u_int8 park_timeout_index;
    u_int8 unpark_timeout_index;
#else
    u_int8 dummy;
#endif
} t_lm_park_info;

typedef enum {
        SWITCH_IDLE = 0,
        SWITCH_PEER_ACTIVATION_PENDING = 1,
        SWITCH_LOCAL_ACTIVATION_PENDING = 2,
        SWITCH_LOCAL_COMPLETION_PENDING = 3,
        SWITCH_COMPLETE_SUCCESS = 4,
        SWITCH_COMPLETE_FAIL = 5
} t_lm_switch_states;
    
typedef struct {
   t_lm_sniff_info sniff_info;
   t_lm_hold_info hold_info;
   t_lm_park_info park_info;
  } t_lm_policy_info; 


typedef enum {
       SCO_UNUSED = 0,
	   SCO_MASTER_DEACTIVATION_PENDING = 1,
       SCO_IDLE = 2,
       SCO_ACTIVATION_PENDING = 3,
       SCO_ACTIVE = 4,
       SCO_CHANGE_PKT_ACCEPT_ACK_PENDING = 5
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1)
       ,
       eSCO_MASTER_ACTIVATION_PENDING = 6,
       eSCO_MASTER_INACTIVE = 7,    
       eSCO_MASTER_RESERVED_SLOT_TX = 8,
       eSCO_MASTER_RESERVED_SLOT_RX = 9,
       eSCO_MASTER_RETRANSMISSION_WINDOW_TX = 10,
       eSCO_MASTER_RETRANSMISSION_WINDOW_RX = 11,
       eSCO_SLAVE_ACTIVATION_PENDING = 12,
       eSCO_SLAVE_INACTIVE = 13,    
       eSCO_SLAVE_RESERVED_SLOT_TX = 14,
       eSCO_SLAVE_RESERVED_SLOT_RX = 15,
       eSCO_SLAVE_RETRANSMISSION_WINDOW_TX = 16,
       eSCO_SLAVE_RETRANSMISSION_WINDOW_RX = 17
#endif
} t_lm_sco_states;

typedef struct {
    u_int32  next_activity_time;          /* For SCO scheduling             */   
    t_packetTypes packet_types;        /* Bit vector of allowed HV types */
    t_connectionHandle connection_handle; /* SCO Connection Handle          */
    t_linkType link_type;                 /* SCO or eSCO                    */

    u_int8  timing_ctrl;                  /* SCO Initialisation Mechanism   */
    u_int8  d_sco;
    u_int8  t_sco;

    u_int8  lm_sco_handle;                /* SCO Air Handle                  */
    u_int8  lm_sco_packet;                /* SCO Packet: sco HV1, HV2, HV3   */
    u_int8  lm_air_mode;                  /* SCO Air Mode: CVSD, u-Law, A-Law*/
    u_int16 voice_setting;                /* Per-link voice setting          */

    u_int8  sco_index;                    /* Fast access to index via ptr    */
    u_int8  sco_cfg_hardware;             /* Baseband configuration (Tabasco)*/
    t_lm_sco_states state;                /* Current State of the SCO rf type*/
    t_deviceIndex device_index;           /* The LC link index for this SCO  */
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1)
    u_int8 w_esco;
    t_packet tx_packet_type;
    t_packet rx_packet_type;
    t_timer esco_end_retransmission_window_timer;
    u_int16 tx_packet_length;
    u_int16 rx_packet_length;
    t_lt_addr esco_lt_addr;

    /*
     *
     * CM: 22 SEP 03
     * 
     * This is required as the the allocation of the LT container
     * for the device_link (ie the lt_index) is done at a point
     * where the lt_addr is not known. Once the lt_addr is known
     * the LM must ensure that the association between the 
     * lt_index and lt_addr is made.
     * 
     * Would like to get rid of the requirement for this element !
     */
    u_int8 esco_lt_index;

    /*
     * Indicates whether the negotiated packet was transmitted in the reserved 
     * slot. Ie the event that there is no eSCO data available, then a POLL/NULL
     * packet will be substituted accordingly. 
     */
    u_int8 esco_negotiated_pkt_tx_in_reserved_slot;

#if (SYS_DEV_ESCO_SLAVE_TX_AFTER_ESCO_MASTER==1)
	/* 
	 * Indicates whether we have started to receive esco in the reserved slot.
	 */
	u_int8 esco_negotiated_pkt_rx_in_reserved_slot;
#endif
    /*
     * Used by the Master during eSCO window initialisation in the 
     * scenario where the eSCO channel uses the full bandwidth 
     * (ie Width(Reserved Slot) + Width (Retransmission) = TeSCO.
     * The Master will not use the retransmission window
     * until it has received a packet from the slave on the
     * eSCO channel. Not using the retransmission window
     * ensures that the Master will POLL the slave on the ACL
     * channel, thus allowing the ACK (for the LMP_accept)
     * to be sent to the Slave (on receiving this ACK the slave
     * will start his eSCO window).
     * In the scenario where max. bandwidth is not used, this bit will
     * be initialised to "0" (ie no need to wait for esco rx) as there
     * should be an oppotunity to TX the ACK on the ACL channel in 
     * between the eSCO windows.
     */
    t_q_descr *tx_descriptor;
    u_int8 negotiation_state;
    u_int32 tx_bandwidth;
    u_int32 rx_bandwidth;
    u_int16 max_latency;
    u_int8 retransmission_effort;
    u_int16 esco_packet_types;
	//u_int16 esco_packet_types_req; -- Seems redundant

   // t_error reject_reason; -- Not used..

    u_int8 renegotiated_params_available;
#if 1 // NEW_TRANSCODE_CONTROL
	  // GF 18 Feb - eSCO retransmission

	u_int32	Start_Tx_Hpf_Filt;
	u_int32 Start_Tx_Pf1_Filt_A;
	u_int32 Start_Tx_Pf1_Filt_B;
	u_int32 Start_Tx_Pf2_Filt;
	u_int32 Start_Tx_Pf3_Filt;
	u_int32 Start_Tx_Cvsd_Filt_A;
	u_int32 Start_Tx_Cvsd_Filt_B;

	u_int32	Start_Rx_Hpf_Filt;
	u_int32 Start_Rx_Pf1_Filt_A;
	u_int32 Start_Rx_Pf1_Filt_B;
	u_int32 Start_Rx_Pf2_Filt;
	u_int32 Start_Rx_Pf3_Filt;
	u_int32 Start_Rx_Cvsd_Filt_A;
	u_int32 Start_Rx_Cvsd_Filt_B;

	u_int32	End_Tx_Hpf_Filt;
	u_int32 End_Tx_Pf1_Filt_A;
	u_int32 End_Tx_Pf1_Filt_B;
	u_int32 End_Tx_Pf2_Filt;
	u_int32 End_Tx_Pf3_Filt;
	u_int32 End_Tx_Cvsd_Filt_A;
	u_int32 End_Tx_Cvsd_Filt_B;

	u_int32	End_Rx_Hpf_Filt;
	u_int32 End_Rx_Pf1_Filt_A;
	u_int32 End_Rx_Pf1_Filt_B;
	u_int32 End_Rx_Pf2_Filt;
	u_int32 End_Rx_Pf3_Filt;
	u_int32 End_Rx_Cvsd_Filt_A;
	u_int32 End_Rx_Cvsd_Filt_B;

#endif

#if(CONFIG_HFP17_MSBC_SUPPORTED == 1)
    u_int8 msbc_seqn;
    sbc_t sbc_codec_ptr;
#endif
#ifdef BT_DUALMODE

    u_int8 next_activity_flag;     //esco next activity flag for encode tx data
#endif
#endif
    struct list_head header_list;
}   t_sco_info;
#if (ESCO_LINK_CRC_ERROR_CHANGE_TO_GOOD == 1)
typedef struct s_eSCO_Filt {
#if 0
	u_int32	Start_Tx_Hpf_Filt;
	u_int32 Start_Tx_Pf1_Filt_A;
	u_int32 Start_Tx_Pf1_Filt_B;
	u_int32 Start_Tx_Pf2_Filt;
	u_int32 Start_Tx_Pf3_Filt;
	u_int32 Start_Tx_Cvsd_Filt_A;
	u_int32 Start_Tx_Cvsd_Filt_B;

	u_int32	Start_Rx_Hpf_Filt;
	u_int32 Start_Rx_Pf1_Filt_A;
	u_int32 Start_Rx_Pf1_Filt_B;
	u_int32 Start_Rx_Pf2_Filt;
	u_int32 Start_Rx_Pf3_Filt;
	u_int32 Start_Rx_Cvsd_Filt_A;
	u_int32 Start_Rx_Cvsd_Filt_B;

	u_int32	End_Tx_Hpf_Filt;
	u_int32 End_Tx_Pf1_Filt_A;
	u_int32 End_Tx_Pf1_Filt_B;
	u_int32 End_Tx_Pf2_Filt;
	u_int32 End_Tx_Pf3_Filt;
	u_int32 End_Tx_Cvsd_Filt_A;
	u_int32 End_Tx_Cvsd_Filt_B;

	u_int32	End_Rx_Hpf_Filt;
	u_int32 End_Rx_Pf1_Filt_A;
	u_int32 End_Rx_Pf1_Filt_B;
	u_int32 End_Rx_Pf2_Filt;
	u_int32 End_Rx_Pf3_Filt;
	u_int32 End_Rx_Cvsd_Filt_A;
	u_int32 End_Rx_Cvsd_Filt_B;
#else
	u_int32	Start_Rx_Hpf_Filt;
	u_int32 Start_Rx_Pf1_Filt_A;
	u_int32 Start_Rx_Pf1_Filt_B;
	u_int32 Start_Rx_Pf2_Filt;
	u_int32 Start_Rx_Pf3_Filt;
	u_int32 Start_Rx_Cvsd_Filt_A;
	u_int32 Start_Rx_Cvsd_Filt_B;
#endif
    }t_eSCO_Filt;
#endif

typedef struct s_esco_descr {
    u_int8 t_esco;
    u_int8 w_esco;
    u_int8 d_esco;
    u_int8 timing_ctrl_flag;
    t_packet tx_pkt_type;
    t_packet rx_pkt_type;
    u_int16 tx_pkt_len;
    u_int16 rx_pkt_len;
    u_int16 voice_setting;
    u_int8 lm_sco_handle;
    u_int8 esco_lt_addr;
    u_int8 negotiation_state;
} t_esco_descr;

typedef struct t_lm_event_info {
    t_error status;
    t_linkType link_type;
    t_deviceIndex device_index;
    u_int8 number;
    u_int8  mode;
    u_int8  role;
    t_connectionHandle handle;
    t_bd_addr* p_bd_addr;
    t_classDevice cod;
    t_error reason;
    u_int8* p_u_int8;
    u_int16 value16bit;
    u_int8 lmp_version;
    u_int16 lmp_subversion;
    u_int16 comp_id;
    t_inquiryResult* p_result_list;
    u_int32 latency;
    u_int32 token_rate;
	u_int32 numeric_value;
    u_int8 key_type;
#if ((PRH_BS_CFG_SYS_HCI_V12_FUNCTIONALITY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LMP_SCO_SUPPORTED==1))
    u_int8 transmission_interval;
    u_int8 retransmission_window;
    u_int16 rx_packet_length;
    u_int16 tx_packet_length;
    u_int8 air_mode;
#endif
#if (PRH_BS_CFG_SYS_HCI_V12_FUNCTIONALITY_SUPPORTED==1)
    u_int8 page;
    u_int8 max_page;
#endif
    u_int8 ack_required;

	u_int8 io_cap;
	u_int8 oob_data_present;
	u_int8 auth_requirements;
#if (PRH_BS_CFG_SYS_LMP_INQUIRY_RESPONSE_NOTIFICAITON_EVENT_SUPPORTED==1)
	u_int32 lap;
	u_int8 rssi;
#endif
} t_lm_event_info;

typedef t_error (*t_lm_callback)(u_int8, t_lm_event_info*);


/*
 * t_lmp_pdu_info
 *      Holds all the parameters for LM_Encode_LMP_PDU() to encode the LMP pdu
 */

typedef struct s_lmp_pdu_info {
    /*
     * Only one of these pointers required at a time, hence union
     */
    union {
        const t_versionInfo  *p_local_version;
        const u_int8         *p_uint8;
        const t_sco_info     *p_sco_link;
        const t_lm_park_info *p_park;
    } ptr;

   t_error  reason;

   u_int8   tid_role; 
   u_int8   length;
   u_int16  return_opcode;   /* Supports extended opcodes */
   u_int8   name_offset;
   u_int8   name_length;
   u_int8   Nbc;
   u_int8   max_slots;
   u_int8   key_size;
   u_int8   drift;
   u_int8   jitter;  
   u_int8   mode;
   u_int8   timing_control;
   u_int8   number_of_poll_slots;
   u_int8   number;

   u_int16  opcode;                 /* Supports extended opcodes */
   u_int16  slot_offset;
   u_int16  interval;
   u_int16  offset;
   u_int16  attempt;
   u_int16  timeout;
   u_int32  instant;

   u_int8   settings;
   u_int8   ack_required;
   u_int8   Db_present;
   u_int16  enc_key_len_mask;

#if(PRH_BS_CFG_SYS_TEST_MODE_TESTER_SUPPORTED == 1)
   u_int8   tm_scenario;
   u_int8   tm_hop_mode;
   u_int8   tm_tx_freq;
   u_int8   tm_rx_freq;
   u_int8   tm_power_mode;
   u_int8   tm_poll_period;
   u_int8   tm_pkt_type;
   u_int16  tm_pkt_len;
#endif
}t_lmp_pdu_info;


typedef enum {
    MAX_POWER,
    MIN_POWER,
    INTERMEDIATE_POWER
 } t_power_level;

typedef enum {
    MIN=0, /* used to signify DM1-only in EDR mode */
    MEDIUM=1, /* with FEC for BR or 2MBPS in EDR mode */
    HIGH=2, /* without FEC for BR or 3MBPS in EDR mode */
    AUTO=3 /* auto rate on all packets in active ptt */
 } t_rate;


typedef enum {
     REJECT = 0,
     DONT_AUTO_ACCEPT = 1,
     AUTO_ACCEPT = 2,
     AUTO_ACCEPT_WITH_MSS = 3
     
} t_filter_settings;


/*
 * Enum to ensure both setup completes processed before sending
 * HCI_Connection_Complete
 */
typedef enum
{
    LMP_Remote_Setup_Complete = 1,
    LMP_Local_Setup_Complete = 2,
    LMP_No_Setup_Complete = 0,
    LMP_Both_Setup_Completes = 3
} t_LMP_Setup_Bitmask;

typedef enum
{
    LMP_No_Features_Complete = 0,
    LMP_Remote_Features_Complete = 1,
    LMP_Local_Features_Complete = 2,
    LMP_Both_Features_Complete = 3
} t_LMP_Features_Bitmask;

typedef enum
{
   LMcontxt_ENTERING_HOLD = 0x01,
   LMcontxt_LEAVING_HOLD = 0x02,
   LMcontxt_ENTERING_SNIFF = 0x04,
   LMcontxt_LEAVING_SNIFF = 0x08,
   LMcontxt_ENTERING_SCO = 0x10,
   LMcontxt_LEAVING_SCO = 0x20,
   LMcontxt_CHANGED_SCO = 0x40
} t_LM_context;

typedef enum
{
    NOT_ACTIVE,
    ACTIVE_RESERVED_SLOT,
    ACTIVE_RETRANSMISSION
} t_eSCO_frame_activity;

#endif

