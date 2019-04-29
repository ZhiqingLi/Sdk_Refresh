#ifndef _CEVA_LE_SMP__
#define _CEVA_LE_SMP__

/*********************************************************************
 * MODULE NAME:     le_smp.h
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
#include "config.h"
#ifdef BT_DUALMODE
/******************************************
 * LE SMP Security Types and #defines
 ******************************************/

typedef enum {
	SMP_RESERVED         = 0x00,
	SMP_PAIRING_REQUEST  = 0x01,
	SMP_PAIRING_RESPONSE = 0x02,
	SMP_PAIRING_CONFIRM  = 0x03,
	SMP_PAIRING_RANDOM   = 0x04,
	SMP_PAIRING_FAILED   = 0x05,
	SMP_ENCRYPTION_INFORMATION = 0x06,
	SMP_MASTER_IDENTIFICATION  = 0x07,
	SMP_IDENTITY_INFORMATION   = 0x08,
	SMP_IDENTITY_ADDRESS_INFORMATION = 0x09,
	SMP_SIGNING_INFORMATION          = 0x0A,
	SMP_SECURITY_REQUEST             = 0x0B
} t_SMP_command_code;

enum t_SMP_pairing_error_codes
{
	SMP_PAIRING_ERROR_RESERVED = 0x00,
	SMP_PAIRING_ERROR_PASSKEY_ENTRY_FAILED = 0x01,
	SMP_PAIRING_ERROR_OOB_NOT_AVAILABLE = 0x02,
	SMP_PAIRING_ERROR_AUTHENTICATION_REQUIRMENTS = 0x03,
	SMP_PAIRING_ERROR_CONFIRM_VALUE_FAILED = 0x04,
	SMP_PAIRING_ERROR_PAIRING_NOT_SUPPORTED = 0x05,
	SMP_PAIRING_ERROR_ENCRYPTION_KEY_SIZE = 0x06,
	SMP_PAIRING_ERROR_COMMAND_NOT_SUPPORTED = 0x07,
	SMP_PAIRING_ERROR_UNSPECIFIED_REASON = 0x08,
	SMP_PAIRING_ERROR_REPEATED_ATTEMPTS = 0x09,
	SMP_PAIRING_ERROR_INVALID_PARAMETERS = 0x0A,
	SMP_PAIRING_ERROR_TIMEOUT            = 0x0C  // Local Error Code - not in spec

} ;
typedef enum t_SMP_pairing_error_codes t_SMP_pairing_error;
typedef enum {
	SMP_STATE_IDLE     = 0x00,
	SMP_STATE_PAIRING_RESP_PENDING = 0x01,
	SMP_W4_INITIATOR_PAIRING_CONFIRM = 0x02,
	SMP_W4_INITIATOR_PASSKEY_ENTRY = 0x03,
	SMP_W4_RESPONDER_PAIRING_CONFIRM = 0x04,
	SMP_W4_RESPONDER_PASSKEY_CONFIRM = 0x05,
	SMP_W4_INITIATOR_PAIRING_RANDOM = 0x06,
	SMP_W4_RESPONDER_PAIRING_RANDOM = 0x07,
	SMP_W4_INITIATOR_OOB_ENTRY = 0x08,
	SMP_W4_RESPONDER_OOB_ENTRY = 0x09,
	SMP_W4_RESPONDER_PASSKEY_ENTRY = 0x0A,
	SMP_W4_ENCRYPTION_CHANGE_EVENT = 0x0B,
	SMP_W4_LTK_REQUEST_EVENT = 0x0C,
	SMP_W4_MASTER_KEY_DISTRIBUTION = 0x0D,
	SMP_W4_SLAVE_KEY_DISTRIBUTION = 0x0F

} t_SMP_states;


typedef enum {
	SMP_PASSKEY_DISPLAY = 0x01,
	SMP_PASSKEY_INPUT = 0x02
} t_SMP_passkey_action;


typedef struct t_smp_event_params {
	u_int8 event;
	u_int16 handle;
	u_int8 status;
	u_int32 passkey;
}t_smp_event_params;

// Pairing Methods
#define LE_NO_PAIRING           0x00
#define LE_OUT_OF_BAND          0x01
#define LE_NUMERIC_COMPARISON   0x02
#define LE_PASSKEY_ENTRY        0x03
#define LE_JUST_WORKS           0x04

#define SMP_DisplayOnly       0x00
#define SMP_DisplayYesNo      0x01
#define SMP_KeyboardOnly      0x02
#define SMP_NoInputNoOutput   0x03
#define SMP_KeyboardDisplay   0x04

#define SMP_AUTHREQ_NO_BONDING_NO_MITM   0x00
#define SMP_AUTHREQ_BONDING_NO_MITM      0x01
#define SMP_AUTHREQ_NO_BONDING_MITM      0x04
#define SMP_AUTHREQ_BONDING_MITM         0x05

// Key Types
#define SMP_SHORT_TERM_KEY 0x01
#define SMP_LONG_TERM_KEY 0x02


/* SMP API Functions */

t_error LEsmp_Initiate_Security(u_int16 handle,u_int8 auth_req);

void LEsmp_Register_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr);
void LEsmp_DeRegister_Callbacks(void);
void LEsmp_Configure_Security(u_int8 io_capability,u_int8 oob_flag,u_int8 auth_req,
		u_int8 max_encry_key_size,u_int8 min_encry_key_size,u_int8 initiator_key_dist,
		u_int8 responder_key_dist);

t_error LEsmp_Passkey_Entry(u_int16 handle,u_int32 passKey /* PassKey */);
t_error LEsmp_OOB_Data_Entry(u_int16 handle,u_int8* p_pdu /* OOB Data */);


t_error LEsmp_Validate_Pairing_Msg(u_int8* p_pdu);
void LEsmp_Check_Pairing_Timers(void);
t_error LEsmp_Pairing_Failed(u_int8 link_id,t_error error_reason);

t_error LEsmp_Decode_Incoming_PDU(t_LE_Connection* p_link,u_int8* p_pdu, u_int8 length);
void LEsmp_Init_SMP(void);
t_error LEsmp_Generate_Request_Event(u_int8 event,u_int16 handle,u_int32 passKey,u_int8 status);
u_int8 LEsmp_Generate_Non_Resolvable_Address(u_int8* p_address);
u_int8 LEsmp_Get_NonResolvable_Address(u_int8* p_address);
u_int8 LEsmp_Confirm_ReConnection_Address(u_int16 handle,u_int8* p_address);
void LEsmp_Generate_Resolvable_Address(u_int8* p_bdaddr);
t_error LEsmp_Handle_HCI_Long_Term_Key_Request(t_LE_Connection* p_connection, u_int8* p_random,u_int16 Ediv);
t_error LEsmp_Handle_Encryption_Key_Refresh_Complete_Event(t_LE_Connection* p_connection,u_int8 status, u_int8 mode);
t_error LEsmp_Handle_Encryption_Change_Event(t_LE_Connection* p_connection,u_int8 status, u_int8 mode );

u_int8 LEsmp_Verify_Signature(u_int8* p_pdu,u_int8 length,u_int8* p_CSRK);
u_int8* LEsmp_Get_Signature(u_int8* p_attrib, u_int8* p_signature);

u_int8 LEsmp_Check_Is_Link_Authenticated(u_int16 handle);
u_int8 LEsmp_Resolve_Address(u_int8 role, u_int8* p_discovered_address);
u_int8 LEsmp_IRK_Distributed(void);
u_int8 LEsmp_Check_Key_Size(u_int16 handle, u_int8 min_key_size);
void LEsmp_Create_Signature(u_int8* payload,u_int8 length,u_int8* p_signature);

#endif
#endif
