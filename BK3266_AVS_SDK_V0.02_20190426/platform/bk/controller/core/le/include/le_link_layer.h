
#ifndef _CEVA_LE_LINK_LAYER__
#define _CEVA_LE_LINK_LAYER__

/*********************************************************************
 * MODULE NAME:     le_link_layer.h
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

#include "le_connection.h"

typedef enum
{
	LL_CONNECTION_UPDATE_REQ = 0,
	LL_CHANNEL_MAP_REQ       = 1,
	LL_TERMINATE_IND         = 2,
	LL_ENC_REQ               = 3,
	LL_ENC_RSP               = 4,
	LL_START_ENC_REQ         = 5,
	LL_START_ENC_RSP         = 6,
	LL_UNKNOWN_RSP           = 7,
	LL_FEATURE_REQ           = 8,
	LL_FEATURE_RSP           = 9,
	LL_PAUSE_ENC_REQ         = 10,
	LL_PAUSE_ENC_RSP         = 11,
	LL_VERSION_IND           = 12,
	LL_REJECT_IND            = 13,
	LL_SLAVE_FEATURE_REQ     = 14,
	LL_CONNECTION_PARAM_REQ  = 15,
	LL_CONNECTION_PARAM_RSP  = 16,
	LL_REJECT_IND_EXT        = 17,
	LL_PING_REQ              = 18,
	LL_PING_RSP              = 19,
	LL_NO_OPERATION			 = 20  // For the case when no operation pending
} t_LE_LL_Control_PDU_Opcodes;

#define LL_CONNECTION_UPDATE_PENDING 0x0001
#define LL_CHANNEL_MAP_REQ_PENDING   0x0002
#define LL_TERMINATE_PENDING         0x0004
#define LL_ENC_REQ_PENDING           0x0008
#define LL_ENCRYPTION_TRANSITIONING  0x0010
#define LL_FEATURE_REQ_PENDING       0x0020
#define LL_PAUSE_ENC_REQ_PENDING     0x0040
#define LL_VERSION_IND_PENDING       0x0080
#define LL_DATA_TX_STOPPED           0x0100
#define LL_DATA_RX_STOPPED           0x0200
#define LL_TX_ENCRYPTED              0x0400
#define LL_RX_ENCRYPTED              0x0800
#define LL_AUTONOMOUSLY_INITIATED    0x1000
#define LL_PING_PENDING              0x2000
#define LL_CONNECTION_PARAM_REQ_PENDING 0x4000


typedef enum
{
	ADV_IND                 = 0,
	ADV_DIRECT_IND          = 1,
	ADV_NONCONN_IND         = 2,
	SCAN_REQ                = 3,
	SCAN_RSP                = 4,
	CONNECT_REQ             = 5,
	ADV_SCAN_IND       		= 6,   // Was ADV_DISCOVER_IND
	ADV_DUMMY               = 7

} t_LE_Advertising_Channel_PDU_Types;

typedef enum
{
	SUBEVENT_VOID                 = 0,
	SUBEVENT_CONNECTION_COMPLETE  = 1,
	SUBEVENT_ADVERTISING_REPORT   = 2,
	SUBEVENT_CONNECTION_UPDATE_COMPLETE = 3,
	SUBEVENT_READ_REMOTE_USED_FEATURES_COMPLETE = 4,
	SUBEVENT_LONG_TERM_KEY_REQUEST = 5
} t_LE_subevents;

typedef struct
{
	u_int8 event_type;
	u_int8 addressType;
	u_int8 address[6];
	u_int8 lenData;
	u_int8 data[31];
	s_int8 rssi;
	u_int8 write_complete;

} t_advert_event;


#define LE_LL_RESPONSE_TIMEOUT  0x1F3FF//0x1F400   /* 40.00 Seconds (BT_CLOCK_TICKS_PER_SECOND * 40) */

u_int8 Vaidate_Access_Address(u_int32 proposed_address);
t_error LE_LL_Ctrl_Handle_LE_IRQ(u_int8 IRQ_Type);

void LE_LL_Init_Duplicates_List(void);
//u_int8 LE_LL_Get_CRC_Err(void);
t_error LE_Decode_link_layer_PDU(t_LE_Connection* p_connection, t_p_pdu p_pdu);
void LE_LL_Encode_Own_Address_In_Payload(u_int8* p_payload,u_int8 address_type);
void LE_LL_Handle_LLC_Ack(t_LE_Connection* p_connection, u_int8 opcode);
t_error LE_LL_Check_For_Pending_Advertising_Reports(void);
void LE_LL_Check_For_Pending_Connection_Completes(void);
void LE_LL_Set_Adv_Header(u_int8 type, u_int8 tx_add, u_int8 rx_add, u_int8 length);
t_error LEll_Encode_Advertising_ChannelPDU(u_int8 pdu_type, const u_int8* pAddress,
										 u_int8 RxAdd, u_int8* pLLdata);
u_int8 LEll_Decode_Advertising_ChannelPDU(t_p_pdu p_pdu,u_int8 rssi,u_int8 length);

u_int8 LE_LL_Check_Timers(void);
void LE_LL_Response_Timeout(t_LE_Connection* p_connection);
u_int8 LE_LL_LE_Event_Due(u_int32 current_clk);

#define LE_LL_Get_CRC_Err  HWle_get_crc_err

void LE_LL_Sleep(void);
u_int32 LE_LL_InactiveSlots(void);
u_int8 LE_LL_Is_LE_Active(void);
u_int32 LE_LL_Slots_To_Next_LE_Activity(u_int32 clk);
u_int8 LE_LL_Get_Current_State(void);
u_int8 LE_LL_Determine_Next_LE_Role(void);

void LE_LL_Setup_User_Data_Tx_Complete_Callback(t_deviceIndex device_index,void (*callback)(t_deviceIndex));
void LE_LL_Data_Tx_Stop_Callback_Send_LL_Pause_Enc_Req(t_deviceIndex device_index);
void LE_LL_Data_Tx_Stop_Callback_Send_LL_Pause_Enc_Rsp(t_deviceIndex device_index);
void LE_LL_Data_Tx_Stop_Callback_Send_LL_Enc_Rsp(t_deviceIndex device_index);
void LE_LL_Data_Tx_Stop_Callback_Send_LL_Enc_Req(t_deviceIndex device_index);
void Clr_LE_LL_Immediate_switch_BLE(void);
void Set_LE_LL_Immediate_switch_BLE(void);
u_int8 Get_LE_LL_Immediate_switch_BLE(void);
#endif

