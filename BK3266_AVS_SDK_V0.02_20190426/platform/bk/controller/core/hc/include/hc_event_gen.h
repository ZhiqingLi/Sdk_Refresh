#ifndef _PARTHUS_HC_HCI_EVENTS_
#define _PARTHUS_HC_HCI_EVENTS_

/**********************************************************************
 * MODULE NAME:    hc_event_gen.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Generates the HCI Events
 * MAINTAINER:     Gary Fleming
 * DATE:           28 May 1999
 *
 * SOURCE CONTROL: $Id: hc_event_gen.h,v 1.38 2010/05/11 14:33:42 nicolal Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     xx xxx 1999 -   gf       - Initial Version 
 *
 **********************************************************************/

#include "sys_types.h"
#include "lmp_types.h"
  

typedef struct {
    const t_bufferSize* p_hc_buffer_size;
    } t_Read_Buffer_Size;  

typedef struct {
    const t_versionInfo*  version;
    } t_Read_Local_Version;

typedef struct {
    t_classDevice  cod;
    } t_Read_Class_Device;


typedef struct {
    u_int16 max_num_keys;
    u_int16  num_keys_read;
} t_Read_Stored_Link_Key;

typedef struct {
     u_int8 page;
     u_int8 max_page;
} t_Read_Local_Extended_Features;

typedef union {
    t_Read_Buffer_Size readBufferSize;
    t_Read_Local_Version readLocalVersion;
    t_Read_Stored_Link_Key  readStoredLinkKey;
    t_Read_Class_Device readClassDevice;
    t_Read_Local_Extended_Features readLocalExtendedFeatures;
} t_command_complete_return_params;

typedef struct {
    u_int16 opcode;
    t_connectionHandle handle;
    t_slots timeout;
    u_int8 mode;
    const u_int8* p_u_int8;
    t_error status;
    const t_bd_addr* p_bd_addr;
    u_int16 value16bit;
    u_int8 number;
    u_int32 u_int32val;
    t_scanActivity scan_activity;
    t_command_complete_return_params returnParams; 
 } t_cmd_complete_event; 

typedef struct {
    const t_inquiryResult* p_result_list;
    u_int8 Num_Responses;
} t_inquiry_result_event;

typedef struct {
    u_int8 Num_Responses;
    t_error status;
} t_inquiry_complete_event;

void HCeg_Initialise(void);
t_error HCeg_Set_Event_Mask(t_p_pdu p_pdu);

t_error HCeg_Inquiry_Result_Event(u_int8 event_cdoe, t_lm_event_info* p_inq_event_info);
void HCeg_Command_Complete_Event(t_cmd_complete_event* event_nfo);
void HCeg_Command_Status_Event(t_error status, u_int16 opcode);
void HCeg_Loopback_Command_Event(t_p_pdu p_command_buff, u_int16 opcode);

boolean HCeg_Number_Of_Completed_Packets_Event(u_int8 num_handles, 
     const u_int16* handles, const u_int16 *completed_packets); 

void HCeg_Hardware_Error_Event(u_int8 hardware_code);

/*
 * Helper function used to determine if an event has been masked.
 */
u_int32 HCeg_Is_Event_Masked_On(u_int32 event_code);
/* 
 * Helper function which prevents overflow of the HCI_Event_Q.
 */
u_int8 HCeg_Prevent_HCI_Event_Queue_from_Overfill(void);


/*
 * Handles all other events
 */
t_error HCeg_Generate_Event(u_int8 event_code,t_lm_event_info* p_event_info);

void _Insert_Uint8(t_p_pdu p_buffer, u_int8 value_8_bit);
void _Insert_Uint16(t_p_pdu p_buffer, u_int16 value_16_bit);
void _Insert_Uint24(t_p_pdu p_buffer, u_int32 value_24_bit);
void _Insert_Uint32(t_p_pdu p_buffer, u_int32 value_32_bit);

#endif
