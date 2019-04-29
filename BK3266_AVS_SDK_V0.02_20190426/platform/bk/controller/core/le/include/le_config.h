

#ifndef _CEVA_LE_CONFIG__
#define _CEVA_LE_CONFIG__

/*********************************************************************
 * MODULE NAME:     le_config.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:
 * MAINTAINER:      Gary Fleming
 * DATE:            Jan-2012
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2012 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/
#include "uslc_sleep.h"
#include "sys_features.h"
#include "le_connection.h"


typedef struct
{
	u_int8 event_mask;
	u_int8  own_address_type;
	t_bd_addr random_address;
#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)
	u_int8 auto_advertise;
	u_int8 auto_initiate;
	u_int8 slave_listen_outside_latency;
	u_int8 preConfigured_hop_inc_available;
	u_int8 preConfigured_hop_inc;
	u_int8 preConfigured_access_address_available;
	u_int32 preConfigured_access_address;
#endif
	u_int8 trace_level;
	t_bd_addr advertiser_address;

	u_int8  state; // Main State for LE device
	u_int8  sub_state; // Substate is dependent on what the main state is.

// Initiating Information
	u_int16 initiator_scan_interval;
	u_int16 initiator_scan_window;
	u_int8  initiator_filter_policy;
	u_int8  initiator_link_id;
	u_int8  initiating_active;
	t_timer next_initiator_scan_timer;

	u_int8 resuming_scan;
	u_int8  data_channel_map[5];

#if (PRH_BS_CFG_SYS_CHANNEL_ASSESSMENT_SCHEME_SUPPORTED==1)
	t_timer device_channel_map_update_timer;
	u_int32 device_channel_map_update_timeout; 
#endif

	u_int8  sleep_clock_accuracy;

	u_int32  link_id_pool;

	u_int8 active_link; // Currently Active Link
	u_int8 slave_links_active;
	u_int8 master_link_active;

//u_int8 incoming_connection_device_index;
	u_int8 last_adv_pdu_rxed;
	u_int32 next_connection_event_time;

	u_int8 connection_complete_pending;
	t_LE_Connection* p_connection_complete_pending;

	u_int8 TCI_transmit_win_config;
	u_int16 transmit_win_offset;
	u_int8 transmit_win_size;

	u_int8 device_sleep;
	u_int8 tabasco_asleep;
	u_int8 num_le_links;

	u_int8 SMP_present;
	u_int8 AES_complete;
	u_int8 encrypt_pending_flag;
	u_int8 device_index_pending_data_encrypt;
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)// SMP Supported - Not in HCI Only Build
	u_int8 io_cap;
	u_int8 oob_data_delivery;
	u_int8 auth_requirements;
	u_int8 max_encry_key_size;
	u_int8 min_encry_key_size;
	u_int8 SMP_InitiatorKeyDistribution;
	u_int8 SMP_ResponderKeyDistribution;
	u_int8 SMP_TK[16];
	u_int8 pending_decrypt_link;
	u_int8 pending_decrypt_len;
#endif

	u_int8 test_mode; //1 bit

#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
	// Security variables
	u_int32 link_key_request_timer;
	u_int8  long_term_key[16];
	u_int8  random_number[8];
	u_int16 encrypt_diversifier;
	u_int8  SKD[16]; /* session key identifier */
	u_int8  device_index_for_ltk;
#endif

	//u_int8 SMP_bonded;
	u_int8 hc_data_tester_present; // Simple flag to indicate if the HC Data Tester is driving the HC
	u_int8 Auto_Widen_MultiSlot;

	u_int8 initiating_enable;

	u_int8 sleep_request_pending;
#if (PRH_BS_CFG_SYS_LE_BQB_TESTER_SUPPORT==1)
	u_int32 tester_test_config;
	u_int8 tester_introduce_mic_error;
	u_int8 tester_introduce_header_error;
#endif


} t_LE_Config;


typedef struct {
	u_int8 echo_tr_win;
	u_int8 num_time_master_tx_first_win;
	u_int8 enable_scan_backoff;
} t_LE_TCI_Config;


typedef enum
{
#if (PRH_BS_CFG_SYS_LE_LOW_DUTY_DIRECT_ADVERTISING==1)
	ADV_IND_TYPE = 0,
	ADV_DIRECT_TYPE_HIGH_DUTY_TYPE = 1,
	ADV_SCAN_IND_TYPE = 2,
	ADV_NONCONN_IND_TYPE = 3,
    ADV_DIRECT_TYPE_LOW_DUTY_TYPE = 4
#else
	ADV_IND_TYPE = 0,
    ADV_DIRECT_TYPE = 1,
	ADV_SCAN_IND_TYPE = 2,
	ADV_NONCONN_IND_TYPE = 3
#endif
} t_LE_advertising_type;

typedef enum
{
	ADV_IND_EVENT_TYPE  = 0,
    ADV_DIRECT_EVENT_TYPE = 1,
	ADV_DISCOVER_IND_EVENT_TYPE = 2,
	ADV_NONCONN_IND_EVENT_TYPE = 3,
	SCAN_RSP_EVENT_TYPE = 4
} t_LE_advertising_report_events;



// Advertising Substates
#define SUBSTATE_IDLE        0x00
#define W4_T2_PRE_ADV_TX     0x01
#define W4_ADV_TX            0x02
#define W4_ADV_RX            0x03
#define W4_SCAN_RESP_TX      0x04
#define W4_N_x_T2            0x05
#define W4_N_x_T0            0x06
#define W4_T0_PRE_ADV_TX     0x07

// Scanning Substates
#define W4_T2_PRE_SCAN       0x10
#define W4_SCAN_RX           0x20
#define W4_SCAN_TX           0x30
#define W4_SCAN_RESP_RX      0x40
#define W4_SCAN_RESUME       0x50
#define W4_SCAN_PKD          0x60
#define W4_SCAN_RESP_PKD     0x70
#define W4_SCAN_DUMMY_TX     0x80
#define W4_DUMMY_SCAN_RESP_TX 0x90
#define W4_RESUME_SCAN       0xA0
#define W4_RESUME_SCAN_NEXT_TIM 0xB0
#define W4_RESUME_FROM_CLASSIC 0xC0

// Connection Event Substates
#define W4_T2_MASTER_CONN_FIRST_TX      0x01
#define W4_MASTER_CONN_FIRST_TX         0x02
#define W4_MASTER_CONN_RX               0x03
#define W4_MASTER_CONN_TX               0x04
#define W4_T0_PRE_MASTER_FIRST_CONN_TX  0x05


#define W4_T0_PRE_SLAVE_CONN_FIRST_RX      0x10
#define W4_T2_SLAVE_CONN_FIRST_RX          0x11
#define W4_SLAVE_CONN_FIRST_RX             0x12
#define W4_SLAVE_CONN_TX                   0x13
#define W4_SLAVE_CONN_RX                   0x14
#define W4_T1_SLAVE_CONN_FIRST_FULL_RX     0x15
#define W4_T2_SLAVE_CONN_FIRST_FULL_RX     0x16
#define W4_T0_PRE_SLAVE_CE                 0x17
#define W4_T2_PRE_SLAVE_CE                 0x18
#define W4_MULTISLOT_WINDOW_WIDENING_RX    0x19
#define W4_T0_PRE_MULTISLOT_WINDOW_WIDENING_RX 0x20
#define W4_T2_PRE_MULTISLOT_WINDOW_WIDENING_RX 0x21

#define TIFS_TX_ADJUSTMENT 31
#define TIMED_SPI_LE
#define LE_INCLUDED 1
#define LE_SW_AES 1
//Feature set

#define DEFAULT_MAX_NUM_LE_LINKS 1


#define EEPROM_BASE_ADDR_FOR_BONDING_INFO 0x10
#define DEFAULT_LE_DEVICE_CHANNEL_MAP_UPDATE_TIMEOUT 0x1000


t_error LEconfig_Init(void);
void LEconfig_Scan_Interval_Timeout(u_int32 current_clk);
u_int8 LEconfig_Check_Current_Address(u_int8 RxAdd,t_p_pdu p_pdu);
u_int8 LEconfig_Get_Addvertising_Own_Address_Type(void);
void LEconfig_Set_Event_Mask(u_int8* p_pdu);
const u_int8* LEconfig_Get_LE_Features_Ref(void);
u_int8 LEconfig_Is_Features_Encryption_Supported(void);
void LEconfig_Set_Random_Address(u_int8* p_random_address);

t_error LEconfig_Set_Scan_Parameters(t_p_pdu p_pdu);
t_error LEconfig_Set_Scan_Enable(u_int8 enable,u_int8 filter_duplicates);
const u_int8* LEconfig_Read_Supported_States(void);
u_int8 LEconfig_Check_Current_Address(u_int8 RxAdd,t_p_pdu p_pdu);
u_int8 LEconfig_Allocate_Link_Id(u_int8* link_id);
void LEconfig_Free_Link_Id(t_LE_Connection* p_connection);
void LEconfig_Disable_Connectable_Advertising(void);
#if (PRH_BS_CFG_SYS_LE_DUAL_MODE==1)
void LEconfig_Delay_Next_Initiating(u_int8 delay);
void LEconfig_Switch_To_LE(u_int8 device_index,u_int8 immediate);

void LEconfig_Switch_To_BT_Classic(void);
#if (SEAN_MODIFY_REF_BQB == 0)
void LEconfig_Complete_Switch_To_Classic(void);
#endif
#endif
#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)
t_error LEconfig_TCI_Set_Search_Window_Delay(u_int8 delay);
t_error LEconfig_TCI_Set_TIFS_Tx_Adjustment(u_int8 delay);
#endif
#define LEconnection_Determine_Connection_Handle(p_connection)  (p_connection->link_id + PRH_BS_CFG_LE_CONN_HANDLE_OFFSET)

void LEconfig_Complete_Switch_To_LE(void);
extern u_int8 LEconfig_device_address[6];
#endif

