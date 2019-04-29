#ifndef _CEVA_LE_GAP_H__
#define _CEVA_LE_GAP_H__

/*********************************************************************
 * MODULE NAME:     le_gap.h
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
#include "le_link_layer.h"
#define GAP_NON_DISCOVERABLE     0x00
#define GAP_LIMITED_DISCOVERABLE 0x01
#define GAP_GENERAL_DISCOVERABLE 0x02
#define GAP_TURNOFF_DISCOVERABLE 0x03

#define GAP_NON_DISCOVERY        0x00
#define GAP_LIMITED_DISCOVERY    0x01
#define GAP_GENERAL_DISCOVERY    0x02


typedef struct t_gap_event_params {
	u_int8 event;
	u_int16 handle;
	u_int8 status;
	u_int8* p_address;
	u_int8 valid_reconn_addr;
	u_int8 encrypt;
	u_int8 role;

} t_gap_event_params;

extern u_int32 gCurrent_ADV_interval;

void GAP_Set_AD_Data(u_int8 length, u_int8* p_data);


void GAP_Set_ScanResp_Data(u_int8 length, u_int8* p_data);

u_int8 GAP_Discoverable_Mode(u_int8 mode,u_int8 adv_type,u_int8 Dual_mode);
u_int8 GAP_Discovery_Mode(u_int8 mode);

u_int8 GAP_Set_Broadcast_Data(u_int8 length,u_int8* p_data);
u_int8 GAP_Set_Broadcast_Mode(u_int8 mode, u_int8 type);

u_int8 GAP_Connectable_Mode(u_int8 mode, u_int8 peer_addr_type,u_int8 own_addr_type, u_int8* p_bdaddr);

#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE == 1)
u_int8 GAP_Connection_Establishment(u_int8 mode, u_int8* peer_address,u_int8 peer_address_type,
		                            u_int16 max_conn_int, u_int16 min_conn_int, u_int16 latency,u_int16 super_to);
//FLASH_LE_CODE u_int8 GAP_Connection_Establishment(u_int8 mode, volatile u_int8* peer_address,u_int8 peer_address_type,
//		                            u_int16 max_conn_int, u_int16 min_conn_int, u_int16 latency,u_int16 super_to);

#endif
u_int8  GAP_Find_Connectable_Devices(void);
u_int8  GAP_Connection_Complete(t_LE_Connection* p_connection, u_int8 status);
t_error LEgap_Handle_Encryption_Change_Event(t_LE_Connection* p_connection,u_int8 status, u_int8 mode );

u_int8 GAP_Init(void);
void   GAP_Check_GAP_Timers(void);
void   GAP_Register_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr);
void   GAP_DeRegister_Callbacks(void);
u_int8 GAP_Handle_Advertising_Events(t_advert_event* p_Advert_Event);
void   GAP_Clear_Connect_Params_Update_Pending(void);
u_int8 GAP_Set_Connect_Params_Update_Pending(u_int16 handle);
void GAP_Disconnect(u_int16 handle);
t_error GAP_Set_Privacy(u_int8 privacy);
void Set_LEgap_Config_state(u_int8 state);
void LEadv_Advertise_Disable_Enable(unsigned char enable);

#endif
#endif
