#ifndef _PARTHUS_LC_TYPES_
#define _PARTHUS_LC_TYPES_

/*************************************************************************
 * MODULE NAME:    lc_types.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Baseband Link Controller Interface Module
 * MAINTAINER:     Conor Morris
 *
 * SOURCE CONTROL: $Id: lc_types.h,v 1.50 2013/09/24 15:23:16 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 *************************************************************************/

#include "bt_fhs.h"
#include "tra_queue.h"
#include "config.h"

/*
 * Used in the Chan_ctrl procedures
 */
#define NO_LOOK_AHEAD        0
#define LOOK_AHEAD           1
#define NFA_CHAN_MASTER      2

/*
 * t_USLC_Frame_Activity                      Frame activity types.
 *
 * Each type a separate bit for rapid set comparison.
 * (Note an enumerated type due to compiler bug on early target!)
 */
#define SCO_MASTER           0x0001
#define CHAN_MASTER          0x0002
#define TESTMODE_MASTER      0x0004
#define ACL_MASTER           0x0008

#define SCO_SLAVE            0x0010
#define CHAN_SLAVE           0x0020
#define TESTMODE_SLAVE       0x0040
#define ACL_SLAVE            0x0080

#define ACL_MASTER_POTENTIAL 0x0100
#ifdef BT_DUALMODE
#define LE_ACL_MASTER_POTENTIAL 0x1000
#define LE_ACL_SLAVE_POTENTIAL  0x2000
#define LE_ACL_MASTER           0x4000
#define LE_ACL_SLAVE            0x8000

#define LE_SCAN                 0x0100
#define LE_ADVERTISE		    0x0200
#define LE_INITIATE             0x0400
#endif
#define NO_ACTIVITY          0x0000

#define MASTER_ACTIVITES     0x000F
#define SLAVE_ACTIVITIES     0x00F0

#define MSS_RESERVED         0x0200

//#define DEFERRED_ACTIVITY    0x8000
#define DEFERRED_ACTIVITY    0x0800
typedef u_int16 t_USLC_Frame_Activity;

/*************************************************************************
 * LC Enumerated Types
 *
 * t_state                              LC state (Bluetooth Device State)
 * t_ulc_procedure                      USLC procedure type                     
 * t_train                              Frequency Train Selection A/B
 * t_rfMode                             Hop 23/79                           
 * t_device_ctrl_state                  Peer Device Control State
 * t_Receiver_Mode                      Hardware Receive Modes 
 * t_Transmitter_Mode                   Hardware Transmit Modes
 *
 *************************************************************************/
typedef enum
{
    No_State                            = 0x0000,      /* Initial State */
    Standby                             = 0x00001,     /* Standby State */
    Connection                          = 0x00002,     /* Connection State */
    Page                                = 0x00004,     /* Page State */
    MasterPageResponse                  = 0x00008,     /* Master Page Response State */
    Page_Scan                           = 0x00010,     /* Page Scan State */
    SlavePageResponse                   = 0x00020,     /* Slave Page Response State */
    Inquiry                             = 0x00040,     /* Inquiry State */
    InquiryResponse                     = 0x00080,     /* Inquiry Response State */
    Inquiry_Scan                        = 0x00100,     /* Inquiry Scan State */
    Master_Slave_Switch                 = 0x00200,     /* Master Slave Switch State */
    Switch_Piconet                      = 0x00400,     /* InterPiconet Switch */
    Low_Power_Mode                      = 0x00800,     /* Master Slave Switch State */
    Return_To_Piconet                   = 0x01000,     /* Return to Piconet State */
    Connection_2ndHalf_Slot_Tx          = 0x02000,
    Connection_Park_Access_Window_Rx    = 0x04000,
    MasterExtendedInquiryResponse       = 0x08000

} t_state;

typedef enum
 {
    PAGE_PROCEDURE=0,
    PAGESCAN_PROCEDURE,
    INQUIRY_PROCEDURE,
    INQUIRYSCAN_PROCEDURE,

    MSS_PROCEDURE,                /* Master-Slave Switch */
    MSS_PROCEDURE_SLAVE_CONTEXT,  /* Master-Slave Switch */
    MSS_PROCEDURE_MASTER_CONTEXT, /* Master-Slave Switch */

    SP_PROCEDURE,                /* Interpiconet Switch */
    SP_PROCEDURE_SLAVE_CONTEXT,  /* Interpiconet Switch */
    SP_PROCEDURE_MASTER_CONTEXT, /* Interpiconet Switch */

    SLEEP_PROCEDURE,                /* Low Power Mode */

    R2P_PROCEDURE,  /* Return From Hold */

    PARKSLAVE_PARK_PROCEDURE,

    PARKMASTER_PARK_PROCEDURE,

    LC_MAX_NUM_PROCEDURES,
    NO_PROCEDURE
} t_ulc_procedure;

typedef enum
{
    NoTrain=0,                          /* No Train */
    Atrain=24,                          /* A Train */
    Btrain=8,                           /* B Train */
    Standard_Scan=1,                    /* Standard Scan Alternative */
    Alternative_Scan=2                  /* Interleaved Scan Alternative */
} t_train;                              /* train options */


typedef enum
{
    RF23=23,    /* 23 hop mode */
    RF79=79     /* 79 hop mode */
} t_rfMode;     /* rf hopping mode options */

typedef enum
{
    NOT_CONNECTED=0,
    CONNECTED_TO_MASTER,
    CONNECTED_TO_SLAVE,
    CONNECTED_TO_MASTER_INACTIVE,
    CONNECTED_TO_SLAVE_INACTIVE
} t_device_ctrl_state;

typedef enum
{
    RXm_RECEIVER_DISABLED=0,
    RXm_SINGLE_WIN=1,
    RXm_DOUBLE_WIN=2,
    RXm_FULL_WIN=3
} t_Receiver_Mode;

typedef enum
{
    TXm_TRANSMITTER_DISABLED = 0,
    TXm_NORMAL=1,
    TXm_SLAVE_1ST_HALF__MASTER_BOTH=2,
    TXm_2ND_HALF_SLOT=3
} t_Transmitter_Mode;


#if (PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)
/*************************************************************************
 * For Channel Driven Quality Support must monitor number Tx/Rx 
 * retransmissions of CRC based packets (data packets)
 *************************************************************************/
typedef struct s_ch_quality
{
    u_int16 num_tx_acl_packets;
    u_int16 num_tx_acl_packets_acked;
    u_int16 num_rx_acl_packets;
    u_int16 num_rx_acl_packets_nacked;
    u_int16 num_tx_syn_packets;
    u_int16 num_tx_syn_packets_acked;
    u_int16 num_rx_syn_packets;
    u_int16 num_rx_syn_packets_nacked;
} t_ch_quality;
#endif


/*************************************************************************
 * LC Event Types    Support of call back to higher layers.
 *************************************************************************/

/*
 * t_LC_Event_Id     The information that can be carried on an LC Event
 */

typedef enum e_LC_Event_Id
{
    /*
     * Indicate to the higher layer that there is activity on this device
     */
    LC_EVENT_REPORT_DEVICE_ACTIVITY,

    /*
     * Indicate to the higher layer that Page procedure has completed
     * successfully (i.e. a baseband connection has been established)
     */
    LC_EVENT_PAGE_RESULT,

    /*
     * Indicate to the higher layer that Page procedure has completed
     * unsuccessfully (i.e. no connection establishment)
     */
    LC_EVENT_PAGE_COMPLETE,

    /*
     * Indicate to the higher layer that Page Scan Incoming Connection
     */
    LC_EVENT_PAGE_SCAN_INCOMING,

    /*
     * Indicate to the higher layer that Page Scan has timed out (no Connection)
     */
    LC_EVENT_PAGE_SCAN_TIMEOUT,

    /*
     * Indicate to the higher layer that Inquiry has completed.
     */
    LC_EVENT_INQUIRY_COMPLETE,

    /*
     * Indicate to the higher layer an Inquiry Result
     */
    LC_EVENT_INQUIRY_RESULT,

    /*
     * Indicate to the higher layer that Inquiry Scan has timed out
     */
    LC_EVENT_INQUIRY_SCAN_TIMEOUT,

    /*
     * Indicate to the higher layers that the Inquiry Scan procedure
     * has entered the 'random wait period'.
     */
    LC_EVENT_INQUIRY_SCAN_RAND_WAIT,

    /*
     * Indicate to the higher layer that MSS procedure has completed
     */
    LC_EVENT_MSS_COMPLETE,

#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)
    /*
     * Indicate to the higher layers that the Park procedure has been completed
     */
    LC_EVENT_PARK_COMPLETE,

    /*
     * Indicate to the higher layer that the Park Wakeup procedure has 
     * completed.
     */
    LC_EVENT_UNPARK_COMPLETE,

    /*
     * Indicate to the higher layer that a Slave-init Unpark has been requested
     * 
     */
    LC_EVENT_PARK_SLAVE_INIT_UNPARK_REQUEST,

    /*
     * Indicate to the higher layer that the Access Window has finished.
     * 
     */
    LC_EVENT_ACCESS_WINDOW_COMPLETE,

#endif

#if (PRH_BS_CFG_SYS_RETURN_TO_PICONET_SUPPORTED==1)
    /*
     * Return to Piconet Timeout callback
     */
    LC_EVENT_R2P_TIMEOUT,
#endif

    /*
     * Indicate to the higher layer that an Access Request has been 
     * received from the Slave.
     */
#if (PRH_BS_DEV_LC_NEXT_ACTIVITY_CALLBACKS_SUPPORTED==1)
    /*
     * Get Next Master Activity
     */
     LC_GET_NEXT_MASTER_TX_ACTIVITY,
    /*
     * Get Next Slave Transmit Activity
     */
     LC_GET_NEXT_SLAVE_TX_ACTIVITY,
    /*
     * Get Next Slave Receive Activity
     */
     LC_GET_NEXT_SLAVE_RX_ACTIVITY,
     /*
      * Get next SCO Transmit Activity
      */
     LC_GET_NEXT_SCO_TX_ACTIVITY,
      /*
       * Is SCO next Transmit or Receive Activity
       */
     LC_IS_SCO_NEXT_ACTIVITY,
     /*
      * Ack Last Dequeued Packet, Necessary for effective flushing
      */
     LC_ACK_LAST_TX_PACKET,
#endif

	 LC_EVENT_INQUIRY_SUSPENDED,
	 LC_EVENT_PAGE_SUSPENDED,

    /*
     * End marker to indicate how many events there are
     */
    LC_MAX_NUMBER_OF_EVENTS

} t_LC_Event_Id;
/*
 * t_LC_Event_Id     The information that can be carried on an LC Event
 */

/*
 * t_LC_Event_Info     The information that can be carried on an LC Event
 */
typedef struct s_lc_eventInfo
{
     t_error status;
     t_FHSpacket*   fhs_packet;
     t_deviceIndex deviceIndex;
     t_parkDeviceIndex parkDeviceIndex;
     s_int8 rssi;
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
     u_int8 extension_length;
#endif
} t_LC_Event_Info;

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
typedef struct s_EIRpacket
{
    u_int8 data[240];
    u_int8 fec_required;
    t_packet optimal_packet_type;
    u_int8 length;
} t_EIRpacket;
#endif

#define ACL_SCO_LT      0
#define ESCO_LT    1
#define is_eSCO_LT(lt) (lt != ACL_SCO_LT)
#define is_ACL_LT(lt)  (lt == ACL_SCO_LT)


/*
 * Data Structures
 */
typedef struct logical_transport_info
{
    u_int8 lt_addr;
    u_int8 tx_seqn;
    u_int8 rx_previous_arqn;
    u_int8 rx_expected_seqn;
    u_int8 tx_ack_pending;
} t_lt_info;

typedef struct eSCO_tx_descriptor
{
    u_int8 *data;
    u_int16 tx_length;
} t_eSCO_tx_descriptor;

typedef struct devlink
{
    t_deviceIndex device_index;         /* Fast access to index via ptr */
    u_int8        piconet_index;       
#if(PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==0)
    u_int8        am_addr;
#endif
    u_int8        role_of_peer;
    
    t_syncword    access_syncword;      /* Current channel syncword     */

    /*
     * FHS related attributes necessary for LC
     */
    t_clock   clock_offset;             /* BT Clock Offset              */
    t_uap_lap uap_lap;                  /* UAP[31:24]/LAP[23:0]         */
    t_nap     nap;                      /* Nap [BD_ADDR] for encryption */

    /*
     * For multi-slot return to piconet, need time of last known correlation
     */
    u_int32 piconet_clock_last_access;  /* bt_clk of last correlation */

    /*
     * ACL related transmit descriptor
     */
    t_q_descr *tx_descriptor;
    u_int8 packet_type; 

    u_int8  local_tx_L2CAP_flow;        /* Local L2CAP flow  | Stop 0 */
    u_int8  local_rx_L2CAP_flow;        /* Peer  L2CAP flow  | Go   1 */

    /*
     * SCO related
     */
    t_sco_fifo active_sco_fifo;         /* SCO fifo (==index) 0, 1, 2 */
    t_packet   active_sco_pkt;          /* HV1, 2 3                   */

    t_device_ctrl_state link_ctrl_state;

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    /*
     *  Multiple Logical Transports support
     */
#if (ESCO_LINK_CRC_ERROR_CHANGE_TO_GOOD == 1)
    u_int8 rx_crc_err2good;
#endif
    u_int8 nb_active_lt;        /* number of active Logical Transports */
    u_int8 tx_lt_index;     /* 0: ACL/SCO link, 1: eSCO link */
    u_int8 rx_lt_index;
    t_lt_info lt_info[1+PRH_BS_CFG_SYS_NB_SUPPORTED_EXTENDED_SCO_CHANNEL];
    
    /*
     * eSCO related
     */
     u_int16 esco_rx_len;
     u_int8 rx_esco_pending; /* 
                         * Indicates whether the device is expecting an
                         * esco packet from the peer device.
                         */
     t_eSCO_tx_descriptor eSCO_tx_descr;
#endif

#if (PRH_BS_CFG_SYS_LMP_ERRONOUS_DATA_REPORTING_SUPPORTED == 1)

#if (PRH_BS_CFG_SYS_SYN_VIA_HCI_SUPPORTED==1)
    t_q_descr *p_CurrentSCOdescriptor;
	t_timer esco_end_retransmission_window_timer;
	boolean sco_commit_pending;
#endif
#endif
    /*
     * Encryption
     */
    u_int8  *p_encryption_key;          /* Pointer to encryption key  */
    t_encrypt_mode encrypt_mode;        /* Default set to Zero        */
    u_int8  hw_encryption_key_len;      /* Actual key length - 1      */

    u_int16 local_slot_offset_us;       /* Timing Slot offset in us   */
    u_int16 peer_slot_offset_us;        /* Timing Slot offset in us   */

    u_int8  power_level;                /* Current power level        */
#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
	u_int8  power_level_2Mbits;         /* Current power level for DQPSK */
	u_int8  power_level_3Mbits;         /* Current power level for 8DPSK */
	boolean epc_enabled;
#endif

    u_int8  clock_jitter_us;            /* Clock jitter in us         */
    u_int8  clock_drift_ppm;            /* Clock drift in ppm         */

/*
 * ACL Transmission control
 * arqn rxPreviousArqn  previous received packet ARQ
 *          ACK/NAK:    Used to informs source that the payload data
 *                      with CRC was transferred successfully.
 * flow local_rx_buffer_full
 *          FLOW/STOP:  if receive buffer can/cannot be emptied
 *                      (i.e. after checking the transport queue availability).
 * seqn
 *          0/1:        For each new transmitted packet that contains data
 *                      with CRC, invert SEQN.
 *
 * no transmission if remote buffer full
 */
    u_int8  local_rx_buffer_full;       /* rx_flow True if rx_flow==STOP*/

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 0)
    u_int8  tx_seqn;                    /* tx_seqn Invert each new pkt  */
    u_int8  rx_previous_arqn;           /* Status of rx packet          */
#endif

#if (PRH_BS_DEV_TOGGLE_TX_SEQN_ON_TX_CRC_PACKET ==1)
    u_int8  tx_seqn_active;             /* Set to TRUE when the current value
                                         * for tx_seqn has been used in a 
                                         * transmitted DM$/DH$/DV packet
                                         */
#endif
    u_int8 remote_rx_buffer_full;       /* rx_flow True if rx_flow==STOP*/
/*
 * ACL Reception Control
 */
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 0)
    u_int8 tx_ack_pending;              /* True if Ack pending for last tx */
    u_int8 rx_expected_seqn;            /* Updated on valid received ACL pkt*/
#endif
    u_int8 rx_expected_broadcast_seqn;

    t_TXRXstatus rx_status;             /* Status of last receive       */

#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)
    u_int8 pm_addr;
#endif

#if (PRH_BS_CFG_SYS_LMP_RSSI_SUPPORTED==1)
    volatile s_int8 rssi_average;
    volatile s_int8 rssi_lastpkt;

#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
	volatile s_int8 rssi_1Mbits_average;
	volatile s_int8 rssi_2Mbits_average;
    volatile s_int8 rssi_3Mbits_average;
#endif
#endif

#if (PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)
    t_ch_quality channel_quality;       /* Monitors for channel quality */
#endif

    t_slots next_wakeup_time;
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
	boolean ptt_acl;
	boolean ptt_esco;
#endif

	u_int8 ErroneousDataStatus;

#if (PRH_BS_CFG_SYS_RETURN_TO_PICONET_SUPPORTED==1) // GF 26 Jan - Moved to device Link for Scatternet
    u_int8  prev_R2P_failed; /* not_received_corr_need; if previous R2P was unsuccessful*/
    u_int32 not_received_correction;
#endif

} t_devicelink;


#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)

/*
 * Data Structures.
 */
typedef struct parkdevlink
{
    t_parkDeviceIndex park_device_index;         /* Fast access to index via ptr */

    t_encrypt_mode encrypt_mode;         /* Default set to Zero        */
    u_int8  hw_encryption_key_len;      /* Actual key length - 1      */
    u_int8  *p_encryption_key;          /* Pointer to encryption key  */

    u_int8  power_level;                /* Current power level        */
    u_int16 peer_slot_offset_us;        /* Timing Slot offset in us   */

    u_int8  clock_jitter_us;            /* Clock jitter in us         */
    u_int8  clock_drift_ppm;            /* Clock drift in ppm         */

} t_parkdevicelink;

#endif

#endif
