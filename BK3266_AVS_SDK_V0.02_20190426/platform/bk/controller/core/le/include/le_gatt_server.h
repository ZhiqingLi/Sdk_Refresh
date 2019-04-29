#ifndef _CEVA_LE_GATT_SERVER__
#define _CEVA_LE_GATT_SERVER__

/*********************************************************************
 * MODULE NAME:     LE_gatt_server.h
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
/************************************************************************
 * Handles for Attributes and Characteristic in the Local GATT Server Database
 * This are hardcoded as the layout of our local GATT Database is know.
 *
 * NOTE !! If the handles in the local GATT Database are changed then
 * this values may also need to change.
 *************************************************************************/
/* HID */
#define HID_PROTOCOL_MODE_HANDLE            0x00D2
#define HID_BOOT_KEYBOARD_INPUT_HANDLE      0x00E1
#define HID_BOOT_MOUSE_INPUT_HANDLE         0x00E6
#define HID_BOOT_KEYBOARD_OUTPUT_HANDLE     0x00E4
#define HID_HID_INFO_HANDLE                 0x00E9
#define HID_REPORT_MAP_HANDLE               0x00DE
#define HID_REPORT_INPUT_HANDLE             0x00D4
#define HID_REPORT_OUTPUT_HANDLE            0x00D8
#define HID_REPORT_FEATURE_HANDLE           0x00DB
#define HID_CONTROL_POINT_HANDLE            0x00EB


/* Heart Rate Service */
#define HRS_MEASUREMENT_HANDLE               0x00F2
#define HRS_SENSOR_LOCATION_HANDLE           0x00F5
#define HRS_CONTROL_POINT_HANDLE             0x00F7

/* Health Thermometer */

#define HT_TEMP_MEASUREMENT_HANDLE           0x0102
#define HT_TEMP_TYPE_HANDLE                  0x0105
#define HT_INT_TEMP_MEASUREMENT_HANDLE       0x0107
#define HT_MEASUREMENT_INTERVAL_HANDLE       0x010A

/* Current Time and NDST */
#define CTS_SERVER_CURRENT_TIME_HANDLE                0x00A2
#define CTS_SERVER_LOCAL_TIME_HANDLE                  0x00A5
#define CTS_SERVER_REFERENCE_TIME_INFORMATION_HANDLE  0x00A7
#define NDCS_SERVER_TIME_HANDLE                       0x00C2

/* Battery and Tx Power */
#define BAS_SERVER_BATTERY_LEVEL_HANDLE               0x00A2
#define TPS_SERVER_TX_POWER_HANDLE                    0x0082
#define NOTIFY_PHONE_HANDLE                           0x00B2


/* Alerting */
#define LINK_LOSS_SERVER_IMMEDIATE_ALERT_HANDLE       0x0072
#define IAS_SERVER_IMMEDIATE_ALERT_HANDLE             0x0052
/* Scan Service */
#define SCPS_SERVER_SCAN_REFRESH_HANDLE               0x0094
#define SCPS_SERVER_SCAN_INTERVAL_HANDLE              0x0092


#define CSC_MEASUREMENT_HANDLE                        0x0112
#define CSC_SUPPORTED_FEATURES_HANDLE                 0x0115
#define CSC_SENSOR_LOCATION_HANDLE                    0x0117
#define CSC_SC_CONTROL_POINT_HANDLE                   0x0119

#define EQ_PARAM0_HANDLE                  		  0x00B2
#define EQ_PARAM1_HANDLE                  		  0x00B5
#define EQ_PARAM2_HANDLE                  		  0x00B8
#define EQ_PARAM3_HANDLE                  		  0x00BB
#define EQ_PARAM4_HANDLE                  		  0x00BE

// add begin by cai.zhong 20190215 for linkplay service
#define LP_CMD_SEND_HANDLE                  0x00C2
#define LP_CMD_RECEIVE_HANDLE               0x00C6
#define LP_DATA_SEND_HANDLE                 0x00C9
#define LP_DATA_RECEIVE_HANDLE              0x00CD
// add end by cai.zhong 20190215 for linkplay service

t_error GATTserver_Init(void);

/***********************************************************************/
/* GATT Server Calls exposed via the API to the user
 * These are used to read/write values to the local Server.
 * For test purposes an additional call is included to allow the user
 * to change the permission's on a given attribute.
 *
 ***********************************************************************/

t_GATT_error  GATTserver_Characteristic_Read_Local_Value(u_int16 char_val_handle,u_int8* p_len,/*const*/ u_int8** p_value);
t_GATT_error  GATTserver_Characteristic_Write_Local_Value(u_int16 char_val_handle,u_int8 length, u_int8* p_attrib_value);
u_int8  GATTserver_Write_Permissions(u_int16 handle,u_int8 permissions);

/***********************************************************************/
/* GATT Server Calls used by the le_gap.c
 * These are used to find a reconnection address and read the privact flag
 * in the Server DB
 *
 ***********************************************************************/
u_int8  GATTserver_Is_Privacy_Flag_Enabled(void);
u_int8* GATTserver_Find_Reconnection_Address(void);
u_int16 GATTserver_Get_Reconnection_Address_Handle(void);


/***********************************************************************/
/* GATT Server Calls used by the le_gatt.c
 * These are used to Read/Write and Search the GATT Server database
 *
 ***********************************************************************/

u_int8  GATTserver_Read_By_Group_Type(u_int8 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int16 attrib_UUID,
		                             u_int8* p_result, u_int8* p_result_len );
u_int8  GATTserver_Find_By_Type_And_Value(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,
										  u_int16 attrib_type,u_int16 attrib_value,u_int8* p_result,u_int8* p_result_len);
u_int8  GATTserver_Find_Included_Services(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int8* p_result,u_int8* p_result_len);
u_int16 Get_GATTserver_DB_Last_Handle(void);
u_int8  GATTserver_Find_Descriptor_Information(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int8* p_result,u_int8* p_result_len,u_int8* p_format);
u_int8  GATTserver_Discover_All_Characteristics_Of_A_Service(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int8* p_result,u_int8* p_result_len, u_int8* p_attrib_val_len);
u_int8  GATTserver_Get_Value_Length(u_int16 handle);
//u_int8  GATTserver_Read_Characteristic_Value_By_UUID(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int16 attrib_UUID,u_int8* p_result,u_int8* p_result_len,u_int8* p_attrib_len);
u_int8  GATTserver_Read_Characteristic_Value(u_int16 max_result_len,u_int16 att_handle, u_int8* p_result,u_int8* p_result_len);
u_int8  GATTserver_Read_Long_Characteristic_Value_Offset(u_int16 max_result_len,u_int16 att_handle, u_int16 offset, u_int8* p_result,u_int8* p_result_len);
u_int8 GATTserver_Write_Command(u_int16 attrib_handle, u_int8* p_attrib_value,u_int8 length);
u_int8  GATTserver_Can_I_Write(u_int16 handle);
u_int8  GATTserver_UUID_Match(t_UUID* attrib_UUID,u_int8 attrib_len,t_attrib_UUID* p_att_UUID);

u_int8 GATTserver_Read_Characteristic_Value_By_UUID(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle, t_attrib_UUID* p_att_UUID,u_int8* p_result,u_int8* p_result_len,u_int8* p_attrib_len);

u_int8 _GATTserver_Add_Characteristic_Declaration(u_int16 handle,u_int16 char_UUID,u_int8 properties);
u_int8 _GATTserver_Add_16ByteUUID_Characteristic_Declaration(u_int16 handle, u_int8* p_UUID,u_int8 properties);
u_int8 _GATTserver_Add_Service(u_int8 service_type,u_int16 handle,u_int8* p_service_uuid,u_int16 service_uuid);
u_int8 _GATTserver_Add_Included_Service(u_int16 handle, u_int16 start_handle,u_int16 end_handle,u_int16 service_UUID);
u_int8 _GATTserver_Add_Characteristic_Value(u_int16 handle,u_int16 char_UUID,u_int8 local_notify,u_int8 permissions,u_int8 val_len,u_int8* p_value);
u_int8 _GATTserver_Add_16ByteUUID_Characteristic_Value(u_int16 handle,u_int8* char_UUID,u_int8 local_notify,u_int8 permissions,u_int8 val_len,u_int8* p_value);
u_int8 _GATTserver_Add_Characteristic_Extended_Properties(u_int16 handle,u_int8 ext_properties);
u_int8 _GATTserver_Add_Characteristic_Format(u_int16 handle, u_int16 unit_UUID,t_char_format format, s_int8 exponent, u_int8 n_sp, u_int16 description);
//u_int8 _GATTserver_Add_Characteristic_User_Description(u_int16 handle,u_int8 string_len,u_int8* p_string,u_int8 permissions);
u_int8 _GATTserver_Add_Client_Characteristic_Description(u_int16 handle,u_int8 permissions,u_int8* p_val);
u_int8 _GATTserver_Add_Characteristic_Propietrary_Descriptor(u_int16 handle,u_int8 UUID_len, u_int8* p_UUID, u_int8 permissions,u_int8* p_value);
//u_int8 _GATTserver_Add_Characteristic_Configuration(u_int16 handle);
//u_int8 _GATTserver_Add_Characteristic_External_Report_Reference(u_int16 handle,u_int16 external_UUID);
//u_int8 _GATTserver_Add_Characteristic_Report_Reference(u_int16 handle,u_int8 report_id,u_int8 report_ref);
u_int8 _GATTserver_Add_Char_Descriptor(u_int16 handle,u_int16 uuid,u_int8 permission,u_int8 val_len,u_int8* p_value);


#endif

#endif
