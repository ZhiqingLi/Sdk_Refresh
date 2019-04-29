#ifndef LE_L2CAP_H_
#define LE_L2CAP_H_
/*********************************************************************
 * MODULE NAME:     le_l2cap.h
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
typedef struct t_l2cap_event_params {
	u_int8 event;
	u_int16 handle;
	u_int8 status;
}t_l2cap_event_params;


#define L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST    0x12
#define L2CAP_CONNECTION_PARAMETER_UPDATE_RESPONSE   0x13
#define L2CAP_COMMAND_REJECT                         0x01 /* TP/LE/CPU/BI-02-C,TP/LE/REJ/BI-01-C L2cap part*/

void LE_L2CAP_Init(void);
u_int8 LE_L2CAP_Connection_Update(u_int16 handle, u_int16 max_interval,u_int16 min_interval, u_int16 latency,u_int16 timeout);
t_error LE_L2CAP_Decode_Incoming_PDU(t_LE_Connection* p_link,u_int8* p_pdu, u_int8 length);
t_error LE_L2CAP_Generate_Response_Event(u_int16 handle,u_int8 status);
void LE_L2CAP_Connection_Update_Complete(t_LE_Connection* p_link,u_int8 status);
void LE_L2CAP_Register_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr);
void LE_L2CAP_DeRegister_Callbacks(void);

#endif
#endif