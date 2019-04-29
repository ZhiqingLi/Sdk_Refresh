#ifndef _CEVA_LE_SCAN__
#define _CEVA_LE_SCAN__

/*********************************************************************
 * MODULE NAME:     le_scan.h
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


t_error LEscan_Init(void);

u_int8 LEscan_Get_Scanning_Channel_Index(void);
u_int8 LEscan_Get_Initiating_Channel_Index(void);
u_int8 LEscan_Get_Scan_Filter_Policy(void);
u_int8 LEscan_Get_Scan_Type(void);
u_int8 LEscan_Get_Scan_Enable(void);
u_int8 LEscan_Get_Scan_Backoff_Count(void);
u_int8 LEscan_Get_Scan_Upper_Limit(void);
u_int8 LEscan_Get_Scanning_Own_Address_Type(void);
u_int8 LEscan_Ok_To_Send_Scan_Req(void);
u_int8 LEscan_Is_Next_Scan_Due(u_int32 current_clk);
u_int16 LEscan_Get_Scan_Window(void);
u_int16 LEscan_Get_Scan_Interval(void);
void LEscan_Prep_For_LE_Scan_Rx(void);
void LEscan_Set_Current_Window_Timer(t_timer end_scan_window);

void LEscan_Update_Initiator_Scan_Freq(void);
void LEscan_Advance_Initiating_Frequency(void);
void LEscan_Advance_Scan_Frequency(void);
void LEscan_Backoff_Successfull_Scan_Rsp_Rxed(void);
void LEscan_Set_Adv_Header(u_int8 pdu_type,u_int8 rxAddr);
void LEscan_Encode_Own_Address_In_Payload(u_int8* p_payload);
void LEscan_Scan_Interval_Timeout(u_int32 current_clk);

RAM_CODE u_int32 LEscan_Get_Slots_To_Next_Scanning_Timer(u_int32 current_clk);
u_int32 LEscan_Get_Slots_To_Next_Initiating_Scan_Timer(u_int32 current_clk);
t_error LEscan_Check_For_Pending_Advertising_Reports(void);
t_error LEscan_Ctrl_Handle_Scanning(u_int8 IRQ_Type,u_int32 current_clk);
t_error LEscan_Set_Scan_Parameters(t_p_pdu p_pdu);
t_error LEscan_Set_Scan_Enable(u_int8 enable,u_int8 filter_duplicates);

t_error	LEscan_Genertate_Advertising_Report_Event(u_int8 num_reports,u_int8 event_type,u_int8 addressType,
												  u_int8* p_address,u_int8 lenData,u_int8* p_data,s_int8 rssi);

void LEscan_Set_Window_Expired(u_int16 time_window_expired);
u_int16 LEscan_Get_Window_Expired(void);
void LEscan_Delay_Next_Scan(u_int8 delay);
t_timer LEscan_Get_Scan_Timer(void);
void LEscan_Set_Scan_Timer(t_timer new_value);

#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)
t_error LEscan_TCI_Write_Scan_Freqs(u_int8 freqs);
t_error LEscan_TCI_Write_Initiating_Freqs(u_int8 freqs);
t_error LEscan_TCI_Read_Scan_Freqs(void);
t_error LEscan_TCI_Read_Initiating_Freqs(void);
t_error LEscan_TCI_Read_Scan_Params(void);
t_error LEscan_TCI_Enable_Scan_Backoff(u_int8 backoff_enable);
t_error LEscan_TCI_Read_Scan_Backoff_Info(void);
#endif
#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
t_error LEscan_Set_GAP_Scan_Parameters(u_int16 scan_interval, u_int16 scan_window, u_int8 scan_type, u_int8 scan_own_address_type,
		                           u_int8 scan_filter_policy);
#endif
#endif
