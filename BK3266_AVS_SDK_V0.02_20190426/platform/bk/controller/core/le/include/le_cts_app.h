#ifndef _CEVA_LE_CTS_APP_H__
#define _CEVA_LE_CTS_APP_H__

#include "config.h"
#ifdef BT_DUALMODE
#define DEBUG_FOR_LOW_POWER 0
#define ENABLE_L2CAP_UPDATE_PARAMETER
//#define CONNECT_INTERVAL_300MS
//#define CONNECT_INTERVAL_100MS
//#define CONNECT_INTERVAL_1000MS


#define APP_FIND_ME    0
#define APP_HT_DEVICE  1
#define APP_HR_DEVICE  2
#define APP_DEFINE FIND_ME

#define APP_EQ_BLE_CONFIG  0
/*********************************************************************
 * MODULE NAME:     le_cts_app.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:
 * MAINTAINER:      Gary Fleming
 * DATE:            June-2012
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2012 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/

typedef struct t_App_Control
{
	
	//u_int8 Flash_Type;// no used
	#if (APP_DEFINE == APP_FIND_ME)
	u_int8 Alert_Level;
	//u_int32 TAPP_Alerting_Timer;// no used
	#ifdef  BATTERY_DETECT
	u_int32 TAPP_BatteryDetect_Timer;
	#endif
	//u_int32 TAPP_DISCOVERABLE_Timer;//for connectable mode transfer to discoverable timer
	u_int32 TAPP_POWERON_STANDBY_Timer;
    u_int32 MMI_Timer[3];
	#endif


	u_int32 TAPP_L2CAP_PARA_UPDATE_Timer;
	
	



} t_App_Control;

/*-------------ble audio equalizer--------------*/
typedef struct
{
    u_int16    a[2];
    u_int16    b[3];
    uint16 flag_enable;
}ble_aud_equ_t;
/*-------------ble audio equalizer--------------*/
#define LONG_BLE_INTERVAL   800
#define SHORT_BLE_INTERVAL  96

t_error LE_Simple_APP_Info(u_int8 para1,u_int8 para2,u_int8 para3);
t_error LE_CTS_APP_Info(u_int8 command,u_int8 command_val,u_int8* p_command_params);
t_error LE_NCST_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_command_params);
t_error LE_HID_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu);
t_error LE_HT_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu);
t_error LE_HRS_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu);
t_error LE_CSC_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu);
t_error LE_RSC_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu);
t_error LE_BP_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu);
t_error LE_BG_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu);

u_int16 App_Setup_GAP_GATT_DB(void);
u_int16 App_Setup_GATT_GATT_DB(void);
u_int16 App_Setup_DEV_INFO_GATT_DB(void);
u_int16 App_Setup_IAS_GATT_DB(void);
u_int16 App_Setup_TX_POWER_GATT_DB(void);
u_int16 App_Setup_LINK_LOSS_GATT_DB(void);
u_int16 App_Setup_BATTERY_SERVICE_GATT_DB(void);
u_int16 App_Setup_SCAN_PARAMETERS_GATT_DB(void);

u_int16 App_Setup_CURRENT_TIME_GATT_DB(void);
u_int16 App_Setup_REFERENCE_TIME_GATT_DB(void);
u_int16 App_Setup_NEXT_DST_GATT_DB(void);

u_int16 App_Setup_HID_GATT_DB(void);
u_int16 App_Setup_HRS_GATT_DB(void);
u_int16 App_Setup_HT_GATT_DB(void);
u_int16 App_Setup_CSC_GATT_DB(void);
u_int16 App_Setup_RSC_GATT_DB(void);
u_int16 App_Setup_BLOOD_PRESSURE_GATT_DB(void);
u_int16 App_Setup_BLOOD_GLUCOSE_GATT_DB(void);
u_int16 App_Setup_FFE0_GATT_DB(void);
u_int16 App_Setup_FFA0_GATT_DB(void);
void LE_App_Discover_Services_By_UUID_Event(u_int8 attrib_len, u_int8 value_payload_len, u_int8* p_pdu);
void LE_App_Command_Complete(u_int16 handle,u_int8 operation_id,u_int8 status);
void LE_App_Discover_Chars_Event(u_int8 attrib_len, u_int8 value_payload_len, u_int8* p_pdu);
u_int8 LE_App_Local_Event_Indication(u_int16 attrib_handle, u_int8 status);
u_int8 LE_App_Peer_Event_Indication(u_int16 attrib_handle, u_int8 status, u_int8 length, u_int8* p_data);
void LE_App_Read_Response(u_int8 length,u_int8* p_data);
u_int8 LE_App_Local_Disconnect_Indication(t_error reason);
void LE_App_Set_L2cap_Para_Update_Timer(u_int32 time);
void LE_App_Reset_L2cap_Para_Update_Timer(void);
void Trans_ADC_Battery_Cap(void);
void User_App_Check_Timers(void);
#ifdef BATTERY_DETECT
void Restart_Battery_Detect_Timer(void);
#endif
void LE_APP_Init(void);
void Set_BLE_interval(u_int16 interval);
u_int16 Get_BLE_interval(void);


#ifdef BATTERY_DETECT
void Save_Battery_level(void);
u_int16 Get_Battery(void);
void Save_Power_on_Battery(u_int8 value);
u_int8 Battery_level_to_Percentage(u_int16 level);
#endif


#if (APP_EQ_BLE_CONFIG == 1)
void EQ_test_RVCE_Param(u_int8 ch);
void EQ_Data_Slave_converter( u_int8 *pdu,ble_aud_equ_t *aud);
void Write_EQ_parm2_reg();

int atoH(u_int8 *c);

void aud_filt_enable_use_ble( uint8 index, uint8 enable );
int aud_filt_conf_use_ble( uint8 index, ble_aud_equ_t *conf );
void EQ_Data_Print(void);

int16 f_sat1(int din);
int16 f_inv1(int din);
#endif //(APP_EQ_BLE_CONFIG == 1)

#endif

#endif
