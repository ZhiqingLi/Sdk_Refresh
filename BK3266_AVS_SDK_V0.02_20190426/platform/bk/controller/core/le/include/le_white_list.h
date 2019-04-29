#ifndef _CEVA_LE_WHITE_LIST__
#define _CEVA_LE_WHITE_LIST__

/*********************************************************************
 * MODULE NAME:     le_white_list.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:     low Energy White List header file
 * MAINTAINER:      Gary Fleming
 * DATE:            August-2011
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2012 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/


typedef struct {

	u_int8  address[6];
	u_int8  address_type;
	u_int8  used;
} t_ListEntry;


u_int8 LEwl_Read_White_List_Size(void);
t_error LEwl_Clear_White_List(void);
t_error LEwl_Add_Device_To_White_List(u_int8 address_type, t_p_pdu p_pdu);
t_error LEwl_Remove_Device_From_White_List(u_int8 address_type,t_p_pdu p_pdu);
u_int8 LEwl_Search_White_List(u_int8 address_type,const u_int8* p_address);
u_int8 LEwl_address_compare(const u_int8* addr1,const u_int8* addr2);
u_int8 LEwl_Add_Device_To_Duplicate_List(u_int8 event_type,u_int8 address_type,const u_int8* p_address);
void LEwl_Init_Duplicates_List(void);
u_int8 LEwl_Can_I_Modify_White_Lists(void);
#endif

