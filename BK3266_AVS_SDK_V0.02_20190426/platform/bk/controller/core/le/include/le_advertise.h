
#ifndef _CEVA_LE_ADVERTISE__
#define _CEVA_LE_ADVERTISE__

/*********************************************************************
 * MODULE NAME:     le_advertise.h
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
typedef struct t_adv_params {
	u_int32 min_interval;
	u_int32 max_interval;
	u_int8  adv_type;
	u_int8  adv_add_type;
	u_int8  init_add_type;
	//u_int8* p_adv_add;
	u_int8* p_init_add;
	u_int8  channel_map;
	u_int8  filter_policy;
} t_adv_params;

t_error LEadv_Set_Advertising_Params(t_p_pdu p_pdu);
u_int8 LEadv_Get_Advertising_Channel_Tx_Power(void);
t_error LEadv_Set_Advertising_Data(u_int8 length,t_p_pdu p_pdu);
t_error LEadv_Set_Scan_Response_Data(u_int8 length,t_p_pdu p_pdu );
t_error LEadv_Set_Advertise_Enable(u_int8 advertise_enable);
void LEadv_Adverting_Event_Begin(u_int32 current_clk);
u_int8 LEadv_Get_Advertising_Channel_Index(void);
t_error LEadv_Init(void);
u_int8 LEadv_Get_Advertising_Own_Address_Type(void);
void LEadv_Disable_Connectable_Advertising(void);
u_int8 LEadv_Get_Advertising_Channel_Index(void);
u_int8 LEadv_Get_Scan_Response_Data_Len(void);
u_int8 LEadv_Get_Advertising_Filter_Policy(void);
void LEadv_Set_Adv_Header(u_int8 pdu_type,u_int8 rxAddr);
void LEadv_Encode_Own_Address_In_Payload(u_int8* p_payload);
u_int8 LEadv_Write_Adv_Data(u_int8* p_payload);
u_int8 LEadv_Write_Scan_Resp_Data(u_int8* p_payload);
u_int8 LEadv_Is_Next_Adv_Due(u_int32 current_clk);
void LEadv_Advertising_Event_Begin(u_int32 current_clk);
u_int32 LEadv_Get_Slots_To_Next_Advertising_Timer(u_int32 current_clk);
t_error LEadv_Handle_Advertising(u_int8 IRQ_Type,u_int32 current_clk);
u_int8 LEadv_Get_Advertising_Enable(void);
u_int8 LEadv_Get_Adv_Channel_Map(void);
u_int8 LEadv_Get_Adv_Type(void);
#if (PRH_BS_CFG_SYS_LE_CSA==4)
void LEadv_GAP_Update_Adv_Interval(u_int16 interval_min, u_int16 interval_max);
#endif
void LEadv_Prep_For_LE_Scan_Resp_Tx(void);
void LEadv_Prep_For_LE_Advert_Tx(void);
void LEadv_Reset_Adv_Frequency(void);
void LEadv_Adjust_Advertising_Event_Start_Timer(u_int32 num);
void LEadv_TCI_Set_Direct_Adv_Timeout(u_int16 timeout);
t_error LEadv_TCI_Write_Advertising_Delta(u_int8 delta);
t_error LEadv_TCI_Read_Advertising_Params(void);

u_int8 LEadv_Get_Direct_Adv_Type(void);
u_int8* LEadv_Get_Direct_Adv_Address(void);
void Set_Dual_mode(u_int8 value);
u_int8  Get_Dual_mode(void);
u_int8 Get_ADV_interval_count(void);

void LE_Set_Immediate_Next_Advertising_Event_Start_Timer(u_int8 reset_timer);
t_timer LE_Get_Next_ADV_Timer_value(void);


#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
t_error LEadv_Set_GAP_Advertising_Params(t_adv_params* p_adv_params);
u_int8 Get_LE_adv_data_len(void);

#endif
#endif
