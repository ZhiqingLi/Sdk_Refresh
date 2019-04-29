#ifndef _PARTHUS_HC_HCI_EVENTS_
#define _PARTHUS_HC_HCI_EVENTS_

/**********************************************************************
 * MODULE NAME:    hc_hci_events.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Generates the HCI Events
 * AUTHOR:         Gary Fleming
 * DATE:           28 May 1999
 *
 * SOURCE CONTROL: $Id: hc_hci_events.h,v 1.13 2004/07/07 14:21:09 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    xx xxx 1999 -   gf       - Initial Version 
 *
 *    
 **********************************************************************/

#include "sys_types.h"

typedef struct {
    u_int16 timeout;
    } t_Read_Accept_TO_Complete;

typedef struct {
    u_int16 timeout;
    } t_Read_Page_TO_Complete;  

typedef struct {
    t_scanEnable scanEnable;
    } t_Read_Scan_Enable_Complete;    

typedef struct {
    t_scanActivity scanActivity;
    } t_Read_Page_Scan_Activity; 

typedef struct {
    t_scanActivity scanActivity;
    } t_Read_Inquiry_Scan_Activity;      

typedef struct {
    u_int8 loopbackMode;
    } t_Read_Loopback_Mode;    

typedef struct {
    t_bufferSize* p_hc_buffer_size;
    } t_Read_Buffer_Size;  

typedef struct {
    u_int8*  features;
    } t_Read_Local_Features;

typedef struct {
    t_versionInfo*  version;
    } t_Read_Local_Version;

typedef union {
    t_Read_Accept_TO_Complete readAccTO;
    t_Read_Page_TO_Complete   readPageTO;
    t_Read_Scan_Enable_Complete readScanEnable;
    t_Read_Page_Scan_Activity readPageScan;
    t_Read_Inquiry_Scan_Activity readInquiryScan;
    t_Read_Loopback_Mode readLoopbackMode;
    t_Read_Buffer_Size readBufferSize;
    t_Read_Local_Features readLocalFeatures;
    t_Read_Local_Version readLocalVersion;

} t_command_complete_return_params;
   
typedef struct {
    u_int16 opcode;
    t_error status;
    t_command_complete_return_params returnParams; 
 } t_cmd_complete; 

typedef struct {
    t_inquiryResult* p_result_list;
    u_int8 Num_Responses;
} t_inquiry_result_event;

typedef struct {
    u_int8 Num_Responses;
    t_error status;
} t_inquiry_complete_event;

typedef struct {
    t_error status;
} t_hci_eventInfo;

void Generate_Inquiry_Result_Event(t_hci_eventInfo* inqEventInfo);
void Generate_Disconnection_Complete_Event(t_event*);
void Generate_Connection_Request_Event(t_bd_addr*, t_classDevice, t_linkType);
void Generate_Inquiry_Complete_Event(t_hci_eventInfo* inqEventInfo);
void Generate_Connection_Complete_Event(t_event*);
void Generate_Command_Complete_Event(t_cmd_complete* eventInfo);
void Generate_Command_Status_Event(t_error status,u_int16 opcode);

#endif
