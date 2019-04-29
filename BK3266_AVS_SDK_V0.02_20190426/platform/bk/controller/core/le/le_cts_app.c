/***********************************************************************
 *
 * MODULE NAME:    le_cts_app.c
 * PROJECT CODE:   CEVA Low Energy Single Mode
 * DESCRIPTION:    Implementation of the Services required for Simple Server App and HID App.
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:  June 2012
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2011-2012 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/
#include "config.h"
#ifdef BT_DUALMODE
// Only required for Terasic Demo of SM Peripheral
//#include "system.h"
#include "sys_types.h"
#include "sys_features.h"
#include "hc_const.h"

#include "hw_lc.h"
#include "hw_le_lc.h"
#include <stdio.h>

#if (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1)
//#if ((PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1) && (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1) && (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1))

#include "le_connection.h"
#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
#include "le_att.h"
#include "le_gatt_server.h"
#endif
#if(PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
#include "le_gap.h"
#include "le_gap_const.h"
#endif
#include "le_const.h"

#include "hc_const.h"
#include "hc_event_gen.h"
#include "tra_queue.h"
#include "bt_timer.h"
#include "sys_mmi.h"
#include "hw_le_lc_impl.h"


#include "le_link_layer.h"
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
#include "le_smp.h"
#endif
#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
#include "le_gap.h"
#endif
#if (PRH_BS_CFG_SYS_LE_L2CAP_INCLUDED==1)
#include "le_l2cap.h"
#endif
#include "le_config.h"
#include "le_cts_app.h"
#include "lmp_utils.h"
#include "compiler_arm.h"
#include "driver_audio.h"
#include "lmp_sco_container.h"
//#include "BKAPI.h"
#include "lp_pdd_main.h" // add by cai.zhong 20190220 for smartvoice 
#include "hal_print.h"  // add by cai.zhong 20190220 for smartvoice debug

//********************************EQ********************************************
#if (APP_EQ_BLE_CONFIG == 1)
ble_aud_equ_t aud_equ_ble[5];
#endif
//********************************************************************************





/********************************************************************************
 * APPLICATION EVENT IDS
 * These are used for encoding Events which will be encoded for transmission over
 * the Serial to the Applications
 ********************************************************************************/
/* GAP Event Id */
#define APP_ALERTING_EVENT                               0x01
#define APP_CONNECT_EVENT                                0x02
#define APP_DISCONNECT_EVENT                             0x03
#define APP_CONNECTABLE_EVENT                            0x04
#if 0 /* no used */
/* Scan Service Event Id */
#define APP_SCAN_INTERVAL_INDICATION                     0x05

/* Time Services Event Ids */
#define APP_CURRENT_TIME_UPDATE_INDICATION               0x06
#define APP_LOCAL_TIME_INFORMATION_UPDATE_INDICATION     0x07
#define APP_REFERENCE_TIME_INFORMATION_UPDATE_INDICATION 0x08
#define APP_DATE_TIME_WITH_DST_UPDATE_INDICATION         0x09

/* HID Service Event Ids */
#define APP_HID_PROTOCOL_MODE_INDICATION                 0x0A
#define APP_HID_CONTROL_POINT_INDICATION                 0x0B
#define APP_HID_BOOT_KEYBOARD_OUTPUT_INDICATION          0x0C
#define APP_HID_REPORT_OUTPUT_INDICATION                 0x0D

#define APP_HID_BOOT_KEYBOARD_INPUT_INDICATION           0x0E
#define APP_HID_BOOT_MOUSE_INPUT_INDICATION              0x0F
#define APP_HID_REPORT_FEATURE_INDICATION                0x10
#define APP_HID_REPORT_INPUT_INDICATION                  0x11

/* HRS */
#define APP_HRS_CONTROL_POINT_INDICATION                 0x12

/* HT  */
#define APP_HT_MEASUREMENT_INTERVAL_INDICATION           0x13

/* CSC  */
#define APP_CSC_REPORT_CONTROL_POINT_INDICATION          0x14

/* RSC */
#define APP_RSC_REPORT_CONTROL_POINT_INDICATION          0x15

/* BG */
#define APP_BG_REPORT_CONTROL_POINT_INDICATION           0x16

/*************************************************************
 * APPLICATION COMMAND IDs
 ************************************************************/
/* HID Commands */
#define HID_APP_UPDATE_BOOT_KEYBOARD_INPUT  0x01
#define HID_APP_UPDATE_BOOT_MOUSE_INPUT     0x02
#define HID_APP_UPDATE_BOOT_KEYBOARD_OUTPUT 0x03
#define HID_APP_UPDATE_HID_INFO             0x04
#define HID_APP_UPDATE_REPORT_MAP           0x05
#define HID_APP_UPDATE_REPORT_INPUT         0x06
#define HID_APP_UPDATE_REPORT_OUTPUT        0x07
#define HID_APP_UPDATE_REPORT_FEATURE       0x08


/* CTS Commands */
#define CTS_APP_UPDATE_CURRENT_TIME                0x01
#define CTS_APP_UPDATE_LOCAL_TIME                  0x02
#define CTS_APP_UPDATE_REFERENCE_TIME_INFORMATION  0x03
#define CTS_APP_WRITE_CURRENT_TIME_NOTIFICATIONS   0x04

/* NDST Command */
#define NDCS_APP_UPDATE_CURRENT_TIME 0x01

/* Misc Service Commands */

#define APP_INIT                      0x00  // Init the Application
#define TPS_APP_UPDATE_POWER_LEVEL    0x01  // Update Tx Power
#define BAS_APP_UPDATE_BATTERY        0x02  // Update Battery Level
#define IAS_APP_CANCEL_ALERTING       0x03  // Cancle Alerting
#define GAP_APP_MAKE_CONNECTABLE      0x04  // Make Device Connectable
#define SCPS_APP_REQUEST_SCAN_REFRESH 0x05  // Request refresh of Scan Parameters from Peer
#define APP_DISCONNECT                   0x06  // Disconnect the Application
#define APP_LINK_DISCONNECT           0x07  // Disconnect Current Link

/* HRS Commands */

#define HRS_APP_UPDATE_HEART_RATE_MEASUREMENT 0x01
#define HRS_APP_UPDATE_BODY_SENSOR_LOCATION   0x02

/* Health Thermometer */
#define HT_APP_UPDATE_TEMP_MEASUREMENT         0x01
#define HT_APP_UPDATE_TEMP_TYPE                0x02
#define HT_APP_UPDATE_INT_TEMP_MEASUREMENT     0x03
#define HT_APP_UPDATE_MEASUREMENT_INTERVAL     0x04

/* CSC Commands */
#define CSC_APP_UPDATE_CSC_MEASUREMENT              0x01
#define CSC_APP_UPDATE_CSC_FEATURE_SUPPORTED        0x02
#define CSC_APP_UPDATE_CSC_SENSOR_LOCATION          0x03

/* RSC Commands */
#define RSC_APP_UPDATE_RSC_MEASUREMENT              0x01
#define RSC_APP_UPDATE_RSC_FEATURE_SUPPORTED        0x02
#define RSC_APP_UPDATE_RSC_SENSOR_LOCATION          0x03


#define BP_APP_UPDATE_BP_MEASUREMENT                0x01
#define BP_APP_UPDATE_INT_CUFF_PRESSURE_MEASUREMENT 0x02
#define BP_APP_UPDATE_BP_FEATURE                    0x03

#define BG_APP_UPDATE_GLUCOSE_MEASUREMENT            0x01
#define BG_APP_UPDATE_MEASUREMENT_CONTEXT            0x02
#define BG_APP_UPDATE_GLUCOSE_FEATURE                0x03
#define BG_APP_UPDATE_RECORD_ACCESS_CONTROL_POINT    0x04 // Not Sure if Needed

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
#endif

/* Battery and Tx Power */
#define BAS_SERVER_BATTERY_LEVEL_HANDLE               0x00A2
#define TPS_SERVER_TX_POWER_HANDLE                    0x0082
#define NOTIFY_PHONE_HANDLE                           0x00B2


/* Alerting */
#define LINK_LOSS_SERVER_IMMEDIATE_ALERT_HANDLE       0x0072
#define IAS_SERVER_IMMEDIATE_ALERT_HANDLE             0x0052
#if 0
/* Scan Service */
#define SCPS_SERVER_SCAN_REFRESH_HANDLE               0x0094
#define SCPS_SERVER_SCAN_INTERVAL_HANDLE              0x0092

/* Cycling Speed and Cadence Handles */
#define CSC_MEASUREMENT_HANDLE                        0x0112
#define CSC_SUPPORTED_FEATURES_HANDLE                 0x0115
#define CSC_SENSOR_LOCATION_HANDLE                    0x0117
#define CSC_SC_CONTROL_POINT_HANDLE                   0x0119
#define CSC_SC_CONTROL_POINT_CCC_HANDLE               0x011A

/* Running Speed and Cadence Handles */
#define RSC_MEASUREMENT_HANDLE                        0x0122
#define RSC_SUPPORTED_FEATURES_HANDLE                 0x0125
#define RSC_SENSOR_LOCATION_HANDLE                    0x0127
#define RSC_SC_CONTROL_POINT_HANDLE                   0x0129
#define RSC_SC_CONTROL_POINT_CCC_HANDLE               0x012A

/* Blood Pressure */
#define BP_MEASUREMENT_HANDLE                         0x0132
#define BP_INT_CUFF_PRESSURE_MEASUREMENT_HANDLE       0x0135
#define BP_FEATURE_HANDLE                             0x0138

/* Blood Glucose */
#define BG_MEASUREMENT_HANDLE                         0x0142
#define BG_MEASUREMENT_CONTEXT_HANDLE                 0x0145
#define BG_FEATURE_HANDLE                             0x0148

/************************************************************
 * Simple States to track device discovery.
 * This is only relevant for the CTS and NDST services, as the peer
 * device is GATT Server for these services.
 ************************************************************/
#define W4_CTS_DISCOVERY                  0x01
#define W4_CTS_CHAR_DISCOVERY            0x02
#define W4_NDST_DISCOVERY                0x03
#endif


#define  NO_APPLICATION                             0x00
#if 0
#define  PROXIMITY_FIND_ME_APP                        0x01
#define  HID_APPLICATION                            0x04

#define NUM_SENSOR_LOCATIONS 15

// Maps the HID Characteristic to Handles
const u_int16 hid_handles[HID_APP_UPDATE_REPORT_FEATURE+1]
= { 0x00, // Invalid
    HID_BOOT_KEYBOARD_INPUT_HANDLE,
    HID_BOOT_MOUSE_INPUT_HANDLE,
    HID_BOOT_KEYBOARD_OUTPUT_HANDLE,
    HID_HID_INFO_HANDLE,
    HID_REPORT_MAP_HANDLE,
    HID_REPORT_INPUT_HANDLE,
    HID_REPORT_OUTPUT_HANDLE,
    HID_REPORT_FEATURE_HANDLE
};


u_int8 SENSOR_LOCATIONS[NUM_SENSOR_LOCATIONS] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
#endif
const u_int16 ADC_table[11] = {0x3EC,0x380,0x31C,0x2B8,0x254,0x1F1,0x18c,0x129,0x0C8,0x062,0x001};//for battery detect function
/* 1.5v,1.35v,1.2v,1.05v,0.9v,0.75v,0.6v,0.45v,0.3v,0.15v,0v */
#ifdef BATTERY_DETECT
static u_int16 Battery_level_adc;//for battery detect function
#if (ADC_ON == 0)
static u_int8 battery_level_value = 20;//for battery detect function
#endif
#endif

#if 0
static u_int16 Battery_level_adc;//for battery detect function
#endif

#if 0
typedef struct t_App_Control
{
    u_int8 Flash_Type;
    u_int8 Alert_Level;
    u_int32 TAPP_Alerting_Timer;
    u_int32 TAPP_HT_Update_Timer;
    u_int8 HT_Updated;
    float New_Temp;
    float Prev_Temp;
} t_App_Control;
#endif

#define HEART_RATE_SAMPLES 4

#define APP_SIMPLE 0x02
#define APP_HEALTH_SERVICES 0x03
#define APP_HID_SERVICE 0x04
#define APP_FITNESS_SERVICES 0x05
#define APP_FIND_ME_TI_LIKE 0x80 //charles

//t_App_Control LEapp_Config = {0x01,0x02,BT_CLOCK_MAX_TIMEOUT,BT_CLOCK_MAX_TIMEOUT,0,98.4,0.0};//org
#if (APP_DEFINE == APP_FIND_ME)

t_App_Control LEapp_Config;
//= {0x01,0x02,BT_CLOCK_MAX_TIMEOUT,BT_CLOCK_MAX_TIMEOUT,BT_CLOCK_MAX_TIMEOUT};//charles modify

//u_int8 Flash_Type,u_int8 Alert_Level,TAPP_Alerting_Timer,u_int32 TAPP_BatteryDetect_Timer,TAPP_L2CAP_PARA_UPDATE_Timer
#endif
#if 0 //do not used
void User_App_Alerting(u_int8 alerting);
void User_App_Flash_7Segment(u_int8 level);
void User_App_Flash_Leds(u_int8 ON_OFF);
void User_App_Flash_Leds_OFF(void);
#endif

void LE_App_Generate_Event(u_int8 event,u_int8 para1,u_int8* p_data);

u_int8 App_State = 0;
u_int16 LE_App_ACL_Handle = 0;
#if 0
u_int16 LE_CTS_Peer_Server_Current_Time_Handle = 0;
u_int16 LE_CTS_Peer_Server_Current_Time_End_Handle = 0;
u_int16 LE_CTS_Peer_Server_Local_Time_Info_Handle = 0;
u_int16 LE_CTS_Peer_Server_Reference_Time_Handle = 0;
u_int16 LE_NDST_Peer_Server_Time_Handle = 0;
u_int16 LE_CTS_Peer_Server_Current_Time_CC_Descr_Handle = 0;
u_int16 CTS_start_handle = 0;
u_int16 CTS_end_handle = 0;
u_int16 NDST_start_handle = 0;
u_int16 NDST_end_handle = 0;
#endif
u_int16 LE_Pending_Read_Handle=0;
u_int16 LE_Pending_Write_Handle=0;
#if 0
u_int8 LE_CTS_Peer_Server_Current_Time_Valid = 0;
u_int8 LE_CTS_Peer_Server_Local_Time_Info_Valid = 0;
u_int8 LE_CTS_Peer_Server_Reference_Time_Valid = 0;
u_int8 LE_CTS_Peer_Server_Current_Time_CCC_Handle = 0;
u_int8 LE_NDST_Peer_Server_Time_Valid=0;

void User_App_Update_LCD(void);
#endif
u_int8 App_Config_App_Present = 1;
u_int8 App_Config_Type = NO_APPLICATION;

u_int16 g_pulse_count=0;

/*
 * Callback functions which are registered with the lower layers.
 */
#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
void LE_APP_GATT_Event_Handler(u_int8 eventCode, t_gatt_event_params* p_gatt_event_params);
void LE_APP_GATT_Command_Complete(u_int8 operation_id,t_gatt_command_complete_params* p_params);
#endif
#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
void LE_APP_GAP_Connection_Complete(u_int8 eventCode, t_gap_event_params* p_gap_event_params);
void Notify_BLE_Connected(void);
void Notify_BLE_Disconnected(void);

#endif

// add begin by cai.zhong 20190221 for ble server
#define __SMARTVOICE__
#ifdef __SMARTVOICE__
static u_int8 lp_service_uuid_128_content[16] = { 0x00,0x00,0x82,0x6f,0x63,0x2e,0x74,0x6e,0x69,0x6f,0x70,0x6c,0x65,0x63,0x78,0x65};
static u_int8 lp_cmd_tx_char_val_uuid_128_content[16]  = { 0x01,0x00,0x82,0x6f,0x63,0x2e,0x74,0x6e,0x69,0x6f,0x70,0x6c,0x65,0x63,0x78,0x65};
static u_int8 lp_cmd_rx_char_val_uuid_128_content[16]  = { 0x02,0x00,0x82,0x6f,0x63,0x2e,0x74,0x6e,0x69,0x6f,0x70,0x6c,0x65,0x63,0x78,0x65};
static u_int8 lp_data_tx_char_val_uuid_128_content[16] = { 0x03,0x00,0x82,0x6f,0x63,0x2e,0x74,0x6e,0x69,0x6f,0x70,0x6c,0x65,0x63,0x78,0x65};	
static u_int8 lp_data_rx_char_val_uuid_128_content[16] = { 0x04,0x00,0x82,0x6f,0x63,0x2e,0x74,0x6e,0x69,0x6f,0x70,0x6c,0x65,0x63,0x78,0x65};


static u_int8 CMD_UUID_Send_CHAR_Value[DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE] = {0x00,0x00};
static u_int8 CMD_UUID_RX_CHAR_Value_CCC[2] = {0x00,0x00};
static u_int8 CMD_UUID_Receive_CHAR_Value[DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE] = {0x00,0x00};
static u_int8 DATA_UUID_Send_CHAR_Value[DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE] = {0x00,0x00};
static u_int8 DATA_UUID_RX_CHAR_Value_CCC[2] = {0x00,0x00};
static u_int8 DATA_UUID_Receive_CHAR_Value[DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE] = {0x00,0x00};
#endif
// add end by cai.zhong 20190221 for ble server

#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)&&(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
static u_int8 reconnection_addr[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
#endif

u_int8 Service_Change_Range[4] = {0x01,0x00,0x4F,0x01};
#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
static u_int8 gap_name_array[16] = "BK3266 BLE";//"A-CEVA";
static u_int8 gap_appearance_array[2] = {0x03,0xC1} ; // Hid Keyboard
static u_int8 gap_connection_params[7] = {100,0,200,0,0,0xd0,07};


u_int8 manufacturer_name_string[10] = "CEVA";
u_int8 model_number_string[7] = "BT 4.0";
u_int8 serial_number_string[11] = "12x07x2012";
u_int8 fw_revision_string[5] = "01.1";
u_int8 sw_revision_string[5] = "01.1";
u_int8 hw_revision_string[5] = "SM-1";
u_int8 system_id[8] = {0xDE,0xBC,0x9A,0xFE,0xFF,0x56,0x34,0x12};
u_int8 pnp_id[7] = {0x01,0x0E,0x00,0x12,0x34,0x01,0x67};
u_int8 IEEE_Reg_Cert[4] = {0x89,0xAB,0xCD,0xEF};

static u_int8 alert_level_array[1] = {0x00};
static u_int8 tx_power_array[1] = {0};
static u_int8 tx_power_format[7];
static u_int8 link_loss_alert_level_array[1] = {0x00};
static u_int8 battery_level[1] = {100};
#endif
#if (APP_DEFINE == APP_FIND_ME)&&(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
//static u_int8 Notify_Phone_Alert[1] = {2};//02 : turn on phone alert , 00 : turn off phone alert
static u_int8 Range_Alarm_Enable[1] = {0};
static u_int8 Beep_range[1] = {0};
static u_int8 Reserve0[1]={0};
static u_int8 Reserve1[1]={0};
static u_int8 Reserve2[1]={0};
static u_int8 Range_Alarm_Enable_str[18] = {"Range Alarm Enable"};
//static u_int8 Notify_Phone_description_CCC[2] = {0x00,0x00};
//static u_int8 Notify_Phone_Alert_str[15] = {"Key Press State"};
static u_int8 Beep_range_str[10] = {"Beep Range" };
static u_int8 Reserve0_str[9] = {"Reserve 0"};
static u_int8 Reserve1_str[9] = {"Reserve 1"};
static u_int8 Reserve2_str[9] = {"Reserve 2"};
static u_int8 battery_service_format[7];

/* add by sean 11.30*/
static u_int8 EQ_Param0[11] = {0};
static u_int8 EQ_Param1[11] = {0};
static u_int8 EQ_Param2[11] = {0};
//static u_int8 EQ_Param3[11] = {0};
//static u_int8 EQ_Param4[11] = {0};


static u_int8 EQ_Param0_str[9] = {"EQ_Param0"};
static u_int8 EQ_Param1_str[9] = {"EQ_Param1"};
static u_int8 EQ_Param2_str[9] = {"EQ_Param2"};
static u_int8 EQ_Param3_str[9] = {"EQ_Param3"};
//static u_int8 EQ_Param4_str[9] = {"EQ_Param4"};

/*********************************************/
#endif
/* Client configuration characteristics - CCC */
u_int8 GATT_service_change_CCC[2] = {0x00,0x00};
u_int8 TX_Power_Level_CCC[2] = {0x00,0x00};
u_int8 Battery_Service_CCC[2] = {0x00,0x00};

#if 0
const u_int8 Simple_App_Adv_Ind[26] = {
    0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/, /* Flags Field */
    0x02,0x0A,0x00,                        /* Tx Power    */
    0x05,0x08,'C','E','V','A',             /* Local name  */
    0x0D,0x02,0x00,0x18,0x01,0x18,0x02,0x18,0x03,0x18,0x04,0x18,0x05,0x18};
#endif
#if 0
const u_int8  Find_ME_adv_data[14]= {
    /* Flags Field */ 0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/,
    /* Tx Power    */ 0x02,0x0A,0x00,
    /* Services //0x07 data length,0x02 16-bits UUIDS incomplete */ 0x07,0x02,0x03,0x18,0x02,0x18,0x04,0x18
    /* 0x1803 LINK LOSS */
    /* 0x1802 Immediate Alert */
    /* 0x1804 Tx Power */
};
#endif

#if 0
const u_int8  EQ_CONFIG_adv_data[10]= {
    /* Flags Field */ 0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/,
    /* Tx Power    */ 0x02,0x0A,0x00,
    /* Services //0x03 data length,0x02 16-bits UUIDS incomplete */ 0x03,0x02,0xe0,0xff,

};
#else
//0x2A29   ����  ����  ����
const u_int8  EQ_CONFIG_adv_data[]= {
    /* Flags Field */ 0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/,
    	                    // 0x07,0x09,'M','T','1','1','4','7',
    	                      0x0D,0x09,'B','K','3','2','6','6','D','M','_','B','L','E', //0x09:Complete Local Name
    /* Tx Power    */ //0x09,0xff,0xee,0x03,0xb3,0x13,0xc6,0x4c,0x20,0xc9,
    /* Services //0x03 data length,0x02 16-bits UUIDS incomplete */ //0x03,0x02,0x66,0x66,

};
#endif
#if 0
const u_int8  HID_adv_data[22]= {
    /* Flags Field */ 0x02,0x01,0x04 ,/*GAP_LE_BR_EDR_NOT_SUPPORTED*/
    /* Services //0x07 data length,0x02 16-bits UUIDS incomplete */ 0x05,0x02,0x12,0x18,0x0F,0x18,
    /* Appearance */ 0x03,0x19,0xC1,0x03,
    /* Local Name */ 0x08,0x09,'3','4','*','*','K','B','D'
    /* 0x1803 LINK LOSS */
    /* 0x1802 Immediate Alert */
    /* 0x1804 Tx Power */
};
#endif
#if 0
const u_int8 Simple_App_Scan_Resp[21] = {
    0x02,0x11,0x02,      /*Le Host+No OOB+ Public */
    0x03,0x16,0x00,0x18, /*UUID for GAP */
    0x03,0x16,0x01,0x18, /*UUID for GATT*/
    0x09,0x02,0x06,0x18,0x07,0x18,0x0a,0x18,0x0f,0x18 };
// static u_int8  Find_ME_scan_resp_data[12] =
//                        {
//                                                   0x0B,0x09,'A','N','T','I','L','O','S','S','B','L'   //C_Chip sample
//                        };
#endif
#if 0
const u_int8  Find_ME_scan_resp_data[12] = //Find_ME_scan_resp_data[0] : Length of data
    //Find_ME_scan_resp_data[1]= 9 : Complete Name see GAP profile assign number
    {
        0x0B,0x09,'S','e','c','u','r','i','T','a','g','D',
    };
#endif
const u_int8  EQ_CONFIG_scan_resp_data[] = //Find_ME_scan_resp_data[0] : Length of data
    //Find_ME_scan_resp_data[1]= 9 : Complete Name see GAP profile assign number
    {
			0x0D,0x08,'B','K','3','2','6','6','D','M','_','B','L','E',   //0x08:Shortened Local Name,suggest same to adv local name when scaned by Andriod v7.0 above
    };
#if 0
const u_int8  HID_scan_resp_data[9] = //Find_ME_scan_resp_data[0] : Length of data
    //Find_ME_scan_resp_data[1]= 9 : Complete Name see GAP profile assign number
    {
        0x08,0x09,'3','4','*','*','K','B','D'
    };
#endif
#if 0
const u_int8 Health_App_Adv_Ind[26] = {
    0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/, /* Flags Field */
    0x02,0x0A,0x00,                        /* Tx Power    */
    0x05,0x08,'C','E','V','A',             /* Local name  */
    //  0x0D,0x02,0x00,0x18,0x01,0x18,0x02,0x18,0x03,0x18,0x04,0x18,0x05,0x18};
    0x0D,0x02,0x00,0x18,0x01,0x18,0x08,0x18,0x10,0x18,0x09,0x18,0x0D,0x18};
const u_int8 Health_App_Scan_Resp[21] = {
    0x02,0x11,0x02,      /*Le Host+No OOB+ Public */
    0x03,0x16,0x00,0x18, /*UUID for GAP */
    0x03,0x16,0x01,0x18, /*UUID for GATT*/
    0x09,0x02,0x00,0x18,0x09,0x18,0x0d,0x18,0x10,0x18 };


const u_int8 Fitness_App_Adv_Ind[26] = {
    0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/, /* Flags Field */
    0x02,0x0A,0x00,                        /* Tx Power    */
    0x05,0x08,'C','E','V','A',             /* Local name  */
    0x0D,0x02,0x00,0x18,0x01,0x18,0x02,0x18,0x03,0x18,0x04,0x18,0x05,0x18};

const u_int8 Fitness_App_Scan_Resp[21] = {
    0x02,0x11,0x02,      /*Le Host+No OOB+ Public */
    0x03,0x16,0x00,0x18, /*UUID for GAP */
    0x03,0x16,0x01,0x18, /*UUID for GATT*/
    0x05,0x02,0x08,0x18,0x09,0x18 };

const u_int8 HID_App_Adv_Ind[26] = {
    0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/, /* Flags Field */
    0x02,0x0A,0x00,                        /* Tx Power    */
    0x05,0x08,'C','E','V','A',             /* Local name  */
    0x0D,0x02,0x00,0x18,0x01,0x18,0x02,0x18,0x03,0x18,0x04,0x18,0x12,0x18};

const u_int8 HID_App_Scan_Resp[21] = {
    0x02,0x11,0x02,      /*Le Host+No OOB+ Public */
    0x03,0x16,0x00,0x18, /*UUID for GAP */
    0x03,0x16,0x01,0x18, /*UUID for GATT*/
    0x03,0x02,0x12,0x18 };
#endif
#if 0
static u_int8 battery_level_value = 20;//for battery detect function
#endif
#if 0
extern u_int16 LMutils_Get_Uint16(t_p_pdu p_pdu);
extern u_int32 LMutils_Get_Uint32(t_p_pdu p_u_int8);
extern void LMutils_Set_Uint16(t_p_pdu p_buffer, u_int16 value_16_bit);
#endif
#if (ADC_ON == 1)
extern volatile u_int8 ADC_finish;//for battery detect function
extern volatile u_int8 ADC_captrue;
#endif

//#define LONG_BLE_INTERVAL   800
//#define SHORT_BLE_INTERVAL  96
u_int16 BLE_interval = SHORT_BLE_INTERVAL;//LONG_BLE_INTERVAL;//SHORT_BLE_INTERVAL;
u_int8  need_to_reset_l2cap_timer = 0;
extern u_int32 log_current_clk[10];
extern u_int32 log_next_connect_event_start_time[10];

//extern  volatile u_int8 RF_test_mode_enable;
u_int16 Get_BLE_interval(void)
{
	return BLE_interval;
}
void Set_BLE_interval(u_int16 interval)
{
	BLE_interval = interval;
	need_to_reset_l2cap_timer = 1;
	
}

#ifdef BATTERY_DETECT

u_int16 Get_Battery(void)//for battery detect function
{
    return Battery_level_adc;
}
void Save_Power_on_Battery(u_int8 value)//for battery detect function
{
    battery_level[0] = value;
}
void Save_Battery_level(void)//for battery detect function
{
#if (ADC_ON == 1)
    Battery_level_adc = REG_ADC_DAT&0x03FF;
#endif
}
void Restart_Battery_Detect_Timer(void)//for battery detect function
{
    u_int32 current_clk = HW_Get_Native_Clk_Avoid_Race();
    LEapp_Config.TAPP_BatteryDetect_Timer = current_clk+32000;
}
#endif
void LE_APP_Init(void)
{
   #ifdef GAP_MASTER_MODE
   u_int8  peer_address[6] = {0x60, 0x32, 0xC3, 0x60, 0x32, 0xC3};
   #endif
    // Initially for fully embedded Proximity, IAS and Find Me
    App_Config_App_Present = 1;
#ifdef BATTERY_DETECT
    LEapp_Config.TAPP_BatteryDetect_Timer = BT_TIMER_OFF;//for find me
#endif
    //    App_Config_Type = PROXIMITY_FIND_ME_APP;//charles remove
#if 0 //do not used
    User_App_Flash_Leds_OFF();
    LEapp_Config.Flash_Type = 0x00;
#endif
    LEapp_Config.Alert_Level = 0x00;
#if 0 //do not used
    LEapp_Config.TAPP_Alerting_Timer = BT_CLOCK_MAX_TIMEOUT;
#endif
#if (APP_DEFINE == APP_HT_DEVICE)
    LEapp_Config.HT_Updated = 0;
    LEapp_Config.TAPP_HT_Update_Timer = BT_CLOCK_MAX_TIMEOUT;
    LEapp_Config.TAPP_HT_Measurement_Interval  = BT_CLOCK_MAX_TIMEOUT;
#endif

#if (APP_DEFINE == APP_HR_DEVICE)
    LEapp_Config.TAPP_HR_Update_Timer = BT_CLOCK_MAX_TIMEOUT;
#endif

    LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer = BT_TIMER_OFF;

    //.TAPP_DISCOVERABLE_Timer = BT_CLOCK_MAX_TIMEOUT;
    LEapp_Config.TAPP_POWERON_STANDBY_Timer = BT_TIMER_OFF;
    {
#if (PRH_BS_CFG_UART_FOR_SENSOR_ONLY==1)
        volatile u_int32 *pulseOut = (u_int32*) PULSE_TIMER_OUT8BIT_BASE;
        // Init the Pulse Rate/ Heart Rate Time.
        LEapp_Config.HR_Sample_Number = 0;
        LEapp_Config.HR_Updated = 0;
        LEapp_Config.HR_Aggretate_Time_for_10pulse = 0;
        LEapp_Config.HR_BPM = 0x0;

        *pulseOut = 1;
        *pulseOut = 0;
#endif
#if (APP_DEFINE == APP_HR_DEVICE)
        LEapp_Config.TAPP_HR_Update_Timer = HW_Get_Native_Clk_Avoid_Race() + 32 /* 100ms interval */;
#endif
    }
	#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
    // Register Callbacks for GATT Event and GATT Command Complete handling
    GATT_Register_Callbacks((void*)LE_APP_GATT_Event_Handler,(void*)LE_APP_GATT_Command_Complete);
	#endif
	 
    GAP_Register_Callbacks((void*)LE_APP_GAP_Connection_Complete, (void*)0x00);
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)

    //LEsmp_Configure_Security(0x03 /* io_capability - No Input/No Output */, 0x00 /* oob_flag - Data Not Present */,0x00 /* auth_req - No Bonding / No MITM */,
    //                         0x10 /* max_encry_key_size*/,0x07 /* min_encry_key_size */,0x02 /* initiator_key_dist - ID Key */,
    //                         0x02 /* responder_key_dist - ID Key */);
    //LEsmp_Configure_Security(SMP_NoInputNoOutput /* io_capability - SMP_NoInputNoOutput  */, 0x00 /* oob_flag - Data Not Present */,SMP_AUTHREQ_NO_BONDING_NO_MITM /* auth_req - Bonding / !MITM */,
    //                         0x10 /* max_encry_key_size*/,0x07 /* min_encry_key_size */,0x01 /* initiator_key_dist - ID Key Enc Key */,
    //                         0x01 /* responder_key_dist - ID Key Enc Key */);
    //LEsmp_Configure_Security(SMP_DisplayOnly /* io_capability - SMP_NoInputNoOutput  */, 0x00 /* oob_flag - Data Not Present */,SMP_AUTHREQ_BONDING_MITM /* auth_req - Bonding / !MITM */,
    //                         0x10 /* max_encry_key_size*/,0x07 /* min_encry_key_size */,0x02 /* initiator_key_dist - ID Key Enc Key */,
    //                         0x02 /* responder_key_dist - ID Key Enc Key */);
    LEsmp_Configure_Security(SMP_NoInputNoOutput /* io_capability - SMP_NoInputNoOutput  */, 0x00 /* oob_flag - Data Not Present */,SMP_AUTHREQ_BONDING_MITM /* auth_req - Bonding / !MITM */,
                         0x10 /* max_encry_key_size*/,0x07 /* min_encry_key_size */,0x07 /* initiator_key_dist - ID Key Enc Key */,
                         0x07 /* responder_key_dist - ID Key Enc Key */);

#endif
	#if(PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
#ifdef GAP_SLAVE_MODE
    #ifdef __SMARTVOICE__
    linkplay_set_advertising();
    #else
    GAP_Set_AD_Data(sizeof(EQ_CONFIG_adv_data),(u_int8*)EQ_CONFIG_adv_data);	
    GAP_Set_ScanResp_Data(sizeof(EQ_CONFIG_scan_resp_data),(u_int8*)EQ_CONFIG_scan_resp_data);//C_Chip
    GAP_Discoverable_Mode(GAP_GENERAL_DISCOVERABLE,ADV_IND,1);
    #endif
#endif
#ifdef GAP_MASTER_MODE
    /* u_int8 GAP_Connection_Establishment(u_int8 mode, u_int8* p_peer_address,u_int8 peer_address_type,
		                            u_int16 max_conn_int, u_int16 min_conn_int,u_int16 latency, u_int16 super_to) 
		                       max_conn_int * 1.25 ms = */
    //GAP_Connection_Establishment(GAP_DIRECTED_CONNECT,peer_address,PUBLIC_ADDRESS,6,6,0,10);/* inteval = 6 * 1.25ms = 7.5ms */
    //GAP_Connection_Establishment(GAP_DIRECTED_CONNECT,peer_address,PUBLIC_ADDRESS,24,24,0,72);/* interval = 24 * 1.25 = 30 ms */
    GAP_Connection_Establishment(GAP_GENERAL_CONNECT,peer_address,PUBLIC_ADDRESS,24,24,0,72);


#endif
	#endif
}

/**************************************************************************
 * FUNCTION :- LE_CTS_APP_Info
 *
 * DESCRIPTION
 * This is the handler for Commands from the Simple Server Application, which
 * are specific to this allocation (GATT interaction for CTS characteristics).
 * It it triggered by the hc_cmd_disp.c on receipt of an Application command over
 * the Serial/UART interface.
 *
 * It supports the initiating of a read of the following characteristic from the peer :
 *   CURRENT_TIME
 *   LOCAL_TIME
 *   REFERENCE TIME INFORMATION
 *
 * The response to each read will result in an update of the peers characteric value
 * displayed on the application.
 *
 * In addition this also supports the Enabling/Disabling of automatic update from the peer
 * for its CURRENT TIME characteristic on the peer by writeing to the relevant
 * Client Characteristic Configuration (CCC) Attribute.
 *
 * INPUTS
 * u_int8 command :-  Instruction on what has to be performed.
 * u_int8 command_val :- Only used in the Characteristic Write - to carry Enable/Disable Flag
 * u_int8* p_command_params :- Current UNUSED.
 *************************************************************************/

t_error LE_CTS_APP_Info(u_int8 command,u_int8 command_val,u_int8* p_command_params)
{

    return NO_ERROR;
}

/**************************************************************************
 * FUNCTION :- LE_NCST_APP_Info
 *
 * DESCRIPTION
 * This is the handler for Commands from the he Simple Server Application, which
 * are specific to this allocation (GATT interaction for the Next DST service change
 * characteristic.
 *
 * It it triggered by the hc_cmd_disp.c on receipt of an Application command over
 * the Serial/UART interface.
 *
 * It supports the initiating of a read of the following characteristic from the peer :
 *   TIME WITH DST
 *
 * The response to  read will result in an update of the peers characteric value
 * displayed on the application.
 *
 * INPUTS
 * u_int8 command :-  Instruction on what has to be performed.
 * u_int8 command_val :- Currently UNUSED
 * u_int8* p_command_params :- Current UNUSED.
 *************************************************************************/


t_error LE_NCST_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_command_params)
{

    return NO_ERROR;
}

/**************************************************************************
 * FUNCTION :- LE_Simple_APP_Info
 *
 * DESCRIPTION
 * This is the handler for Commands which are common to many applications. It is
 * triggered by the hc_cmd_disp.c on receipt of an Application command over
 * the Serial/UART interface.
 *
 *
 * It supports the following :
 *     APP_INIT - Initialise the application.
 *  APP_DISCONNECT             - Disconnect the App from the LE stack - deregisters any callbacks.
 *  GAP_APP_MAKE_CONNECTABLE   - Calls GAP procedures to make device connectable.
 *  TPS_APP_UPDATE_POWER_LEVEL - Updates the Tx Power characteristic in the local DB.
 *  BAS_APP_UPDATE_BATTERY     - Updates the Battery Level characteristic in the local DB.
 *  IAS_APP_CANCEL_ALERTING    - Provides a hook from which platform specific procedures
 *                               can be called
 *  SCPS_APP_REQUEST_SCAN_REFRESH - Updates the Scan Refresh Characteristic in the local DB.
 *
 *
 * INPUTS
 * u_int8 command :-  Instruction on what has to be performed.
 * u_int8 command_len :- Length of data to be written to local DB
 * u_int8 param1 :-  A one byte parameters - to be used in DB writes.
 *************************************************************************/

t_error LE_Simple_APP_Info(u_int8 command,u_int8 command_len,u_int8 param1)
{
    return NO_ERROR;
}

/**************************************************************************
 * FUNCTION :- LE_HID_APP_Info
 *
 * DESCRIPTION
 * This is the handler for Commands which are specific to the HID Application.
 * All these commands perform a local write to a specific characteristic in the
 * local GATT DB Server. It is triggered by the hc_cmd_disp.c on receipt of an
 * Application command over the Serial/UART interface.
 *
 *
 * INPUTS
 * u_int8 command :-  Instruction on what has to be performed.
 * u_int8 command_len :- Length of data to be written to local DB
 * u_int8* p_pdu :-  The Data to be written.
 *************************************************************************/

t_error LE_HID_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu)
{

    return NO_ERROR;
}

/**************************************************************************
 * FUNCTION :- LE_HRS_APP_Info
 * ( Heart Rate Service )
 *
 * DESCRIPTION
 * This is the handler for Commands which are specific to the HRS Application.
 * All these commands perform a local write to a specific characteristic in the
 * local GATT DB Server. It is triggered by the hc_cmd_disp.c on receipt of an
 * Application command over the Serial/UART interface.
 *
 * INPUTS
 * u_int8 command :-  Instruction on what has to be performed.
 * u_int8 command_len :- Length of data to be written to local DB
 * u_int8* p_pdu :-  The Data to be written.
 **************************************************************************/

t_error LE_HRS_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu)
{

    return NO_ERROR;
}

t_error LE_HT_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu)
{

    return NO_ERROR;
}

/********************************************************
 * Arrays mapping CSC and RSC Characteristics to handles
 *
 */
#if 0
const u_int16 CSC_Command_To_Handle_Map[CSC_APP_UPDATE_CSC_SENSOR_LOCATION+1] = {
    0x00,// INVALID
    CSC_MEASUREMENT_HANDLE,// CSC_APP_UPDATE_CSC_MEASUREMENT
    CSC_SUPPORTED_FEATURES_HANDLE, // CSC_APP_UPDATE_CSC_FEATURE_SUPPORTED
    CSC_SENSOR_LOCATION_HANDLE// CSC_APP_UPDATE_CSC_SENSOR_LOCATION
};

const u_int16 RSC_Command_To_Handle_Map[RSC_APP_UPDATE_RSC_SENSOR_LOCATION+1] = {
    0x00,// INVALID
    RSC_MEASUREMENT_HANDLE,// RSC_APP_UPDATE_RSC_MEASUREMENT
    RSC_SUPPORTED_FEATURES_HANDLE, // RSC_APP_UPDATE_RSC_FEATURE_SUPPORTED
    RSC_SENSOR_LOCATION_HANDLE// RSC_APP_UPDATE_RSC_SENSOR_LOCATION
};
#endif

t_error LE_CSC_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu)
{

    return NO_ERROR;
}

t_error LE_RSC_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu)
{

    return NO_ERROR;
}

t_error LE_BP_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu)
{

    return NO_ERROR;
}

t_error LE_BG_APP_Info(u_int8 command,u_int8 command_len,u_int8* p_pdu)
{

    return NO_ERROR;
}

/**************************************************************************
 * FUNCTION :- LE_App_Local_Event_Indication
 *
 * DESCRIPTION
 * This function is triggered on receipt of a Write_Indication Event/Callback from
 * the GATT layer.
 *
 * For each handle the local GATT DB is read to determine the new value which has
 * been written into the DB by the Peer. An Application Event is then generated to
 * inform the App/User of the new value.
 *
 * INPUTS
 * u_int16 attrib_handle - Handle which has been written to.
 * u_int8  status  :- Currently UNUSED
 *************************************************************************/

u_int8 LE_App_Local_Event_Indication(u_int16 attrib_handle, u_int8 status)
{
#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
    u_int8* p_value_array;
    //u_int8 alert_level;
    u_int8 length;

    switch (attrib_handle)
    {
    case IAS_SERVER_IMMEDIATE_ALERT_HANDLE :

        GATTserver_Characteristic_Read_Local_Value(attrib_handle,&length,&p_value_array);

#if 0 // no used
        alert_level = p_value_array[0];
        User_App_Alerting(alert_level);
#endif
        break;

    default :
        break;
    }
#endif
    return NO_ERROR;
}

#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
/**************************************************************************
 * FUNCTION :- LE_APP_GATT_Event_Handler
 *
 * DESCRIPTION
 * This key function is used to handle all events raised by the GATT layer.
 *
 *
 *
 * GATT_VALUE_NOTIFICATION_EVENT :
 * This occurs when one of the Current Time Characteristics has been modified by
 * the peer and we have previously enable notifications by writing to it's CCC.
 * In this cases we construct an APP_INDICATION event using the information in the
 * notification and sent it to the application.
 *
 * GATT_READ_RESPONSE_EVENT
 * If a READ_RESPONSE_EVENT is recieved for a handle which is valid (i.e not zero).
 * The the payload in the READ_RESPONSE will be sent to the application in an
 * APP_INDICATION event.
 *
 * The READ_RESPONSE_EVENT is also used to trigger discovery of the peers CTS and NDST
 * handles.
 *
 * GATT_DISCOVER_SERVICES_BY_UUID_EVENT
 * This the response to the Discover Services by UUID request which was issued to determine the Handles
 * ranges in use of the CTS and NDST profiles.
 *
 * GATT_DISCOVER_CHARACTERISTICS_EVENT
 * This the response to the Discover Characteristics request which was issue to determine the values of
 * the handles for the Characterisitics of the CTS and NDST profiles.
 *
 * INPUTS
 * u_int8 eventCode              - Identifier for the Event
 * u_int8* p_gatt_event_params   - pointer to a structure of GATT event Parameters
 *
 *************************************************************************/
u_int8 w_eq_count = 0;

void LE_APP_GATT_Event_Handler(u_int8 eventCode, t_gatt_event_params* p_gatt_event_params)
{
//unsigned char temp_gpio = 0;
    //u_int8 event;

	//u_int8 s[8],d[8];
    //os_printf("LE_APP_GATT_Event_Handler eventCode:0x%.2x\r\n",eventCode);	
    switch(eventCode)
    {
    case GATT_READ_RESPONSE_EVENT :
    case GATT_VALUE_NOTIFICATION_EVENT :
        // Need to Add Error Handling here !!!
        if (p_gatt_event_params->status==0x00)
        {
            if (eventCode == GATT_READ_RESPONSE_EVENT)
            {

            }
            else if (p_gatt_event_params->att_handle == 0x00)
            {

            }
            else
            {

            }
        }
        else
        {
            // Need to determine what to do here
        }
        break;

    case GATT_DISCOVER_SERVICES_BY_UUID_EVENT :
        {


        }
        break;

    case GATT_DISCOVER_CHARACTERISTICS_EVENT :
        {


        }
        break;

    case GATT_WRITE_INDICATION_EVENT :
        LE_App_Local_Event_Indication(p_gatt_event_params->att_handle,p_gatt_event_params->status);
        break;

    case  GATT_WRITE_COMPLETE_EVENT:
    {
				#if 0
                   os_printf("att_handle =0x%x,att_len =0x%x, event_opcode =0x%x,handle =0x%x,payload_length =0x%x payload =0x%x\r\n",\
           			p_gatt_event_params->att_handle,\
           			p_gatt_event_params->att_len,\
           			p_gatt_event_params->event_opcode,\
           			p_gatt_event_params->handle,\
           			p_gatt_event_params->payload_length,\
           			p_gatt_event_params->payload[0]) ;
				#endif
             #if 0
             os_printf("Receive data:\r\n");
             DUMP8("0x%02x ", p_gatt_event_params->payload, p_gatt_event_params->payload_length);
             #endif
    	     //os_printf("handle = %x\r\n",p_gatt_event_params->handle);

             switch(p_gatt_event_params->handle)
           	 {
           	    // add begin by cai.zhong 20190221 for ble server
           	    case LP_CMD_RECEIVE_HANDLE:
                {
                    lp_pdd_receive_cmd_data(p_gatt_event_params->payload, p_gatt_event_params->payload_length);
                    break;
                }
                // add end by cai.zhong 20190221 for ble server
      		    case EQ_PARAM0_HANDLE:
			        break;
           	    case EQ_PARAM1_HANDLE:
			        break;
           		case EQ_PARAM2_HANDLE:
           		{
                    #if (APP_EQ_BLE_CONFIG == 1)
       		    	EQ_Data_Slave_converter(p_gatt_event_params->payload,aud_equ_ble+2);
       		    	w_eq_count++;
                    #endif
                    break;
                }  
       		    case EQ_PARAM3_HANDLE:
       		    {
                    #if (APP_EQ_BLE_CONFIG == 1)
       		    	EQ_Data_Slave_converter(p_gatt_event_params->payload,aud_equ_ble+3);
       		    	w_eq_count++;
                    #endif
                    break;	                    
       		    }  			
       		    case EQ_PARAM4_HANDLE:
       		    {
                    #if (APP_EQ_BLE_CONFIG == 1)
       		        EQ_Data_Slave_converter(p_gatt_event_params->payload,aud_equ_ble+4);
       		    	w_eq_count++;
       		    	if(w_eq_count == 5)
       		    	  {
       		    		Write_EQ_parm2_reg();
       		    	 }
       		    	  w_eq_count = 0;
                    #endif
                    break;
       		    } 
           		default:
                    break;

           	}
        break;
    }
    case  GATT_INFORMATION_READ_COMPLETE_EVENT:
        //os_printf("-------------INFORMATION_READ_COMPLETE-----------\r\n");
        //#ifdef LE_SLEEP_ENABLE
		#if 0
        //LE_App_Set_L2cap_Para_Update_Timer(HW_Get_Native_Clk_Avoid_Race() + 3200);   //wait 5 s to start l2cap parameter update req,added by yangyang
        #endif
        break;

    default:
        break;
    }
}
#endif
void Notify_BLE_Connected(void)
{
    lp_pdd_connected(BLUETOOTH_BLE_CONNECT_EVENT); // add by cai.zhong 20190220 for smartvoice 
}
void Notify_BLE_Disconnected(void)
{
    lp_pdd_disconnected(BLUETOOTH_BLE_CONNECT_EVENT); // add by cai.zhong 20190220 for smartvoice 
}










/**************************************************************************
 * FUNCTION :- LE_App_Local_Disconnect_Indication
 *
 * DESCRIPTION
 * This the Callback function invoked by the Lower layers layer when a connection is disconnected
 * either by the peer or due to link loss.
 *
 * Since the 'Simple Server Application' encompasses the link loss profile, the
 * Link Loss IAS alert characteristic will be read from the local GATT DB and an alerting
 * event will be send to the application.
 *
 * For all applications a Disconnect Event will be sent to the application and the GAP
 * will be instructed to place the device in CONNECTABLE mode. A 'Connectable' event will
 * also be sent to the application.
 *************************************************************************/

u_int8 LE_App_Local_Disconnect_Indication(t_error reason)
{
#if 0
    u_int8* p_value_array;
    u_int8 alert_level;
    u_int8 length;
#endif
#ifdef GAP_MASTER_MODE
  u_int8  peer_address[6] = {0x61, 0x33, 0xcc, 0xdd, 0xee, 0xff};
#endif
    /* u_int8 dummy_addr[6] = { 0x00,0x00,0x00,0x00,0x00,0x00 }; */
    //u_int32 current_clk = HW_Get_Native_Clk_Avoid_Race();
#if (APP_DEFINE == APP_HT_DEVICE)
    LEapp_Config.Prev_Temp = 0xFFFF;
#endif

    if (!App_Config_App_Present)
        return 0;

#if (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1)
#if 0
    if (App_Config_App_Present && (App_Config_Type != HID_APPLICATION))
    {
        // Need to check the Alert Level in the Link Loss Profile on the Server
        // To determine if we should raise and Alerting Event
        GATTserver_Characteristic_Read_Local_Value(LINK_LOSS_SERVER_IMMEDIATE_ALERT_HANDLE,&length,&p_value_array);
        alert_level = p_value_array[0];

        if (alert_level != 0)
        {
            //LE_App_Generate_Event(APP_ALERTING_EVENT,alert_level,0x00);
#if 0 // no used
            User_App_Alerting(alert_level);
#endif
        }
    }
#endif
    //Set_MMI_Cycle_time(25);
    //Set_MMI_Cycle_time(0xFFFF);
    //Set_MMI_State(MMI_DISCONNECTED_STATE);
#if 0
#if (MMI_ON == 1)

    if(Get_MMI_State() != MMI_DEEPSLEEP_STATE)
    {
        Set_MMI_State(MMI_POWER_ON_STATE);//for C_Chip
        //LE_App_Generate_Event(APP_DISCONNECT_EVENT,0x00,0x00);
		#ifdef BATTERY_DETECT
        LEapp_Config.TAPP_BatteryDetect_Timer = BT_TIMER_OFF;
		#endif
        //LEapp_Config.TAPP_DISCOVERABLE_Timer = current_clk+16;//wait 5 msec
        //GAP_Discoverable_Mode(GAP_NON_DISCOVERABLE,ADV_IND);
        GAP_Discoverable_Mode(GAP_GENERAL_DISCOVERABLE,ADV_IND,1);
        //GAP_Connectable_Mode(0x02 /*GAP_UNDIRECTED_CONNECTABLE*/, 0x00 /*PUBLIC_ADDRESS*/, 0x00/*PUBLIC_ADDRESS*/,dummy_addr);
        //LE_App_Generate_Event(APP_CONNECTABLE_EVENT,0x00,0x00);
    }
#endif
#endif
#ifdef GAP_SLAVE_MODE
	#if 1
	//if(LMscoctr_Get_Number_eSCO_Connections() == 0)
	{
		GAP_Discoverable_Mode(GAP_GENERAL_DISCOVERABLE,ADV_IND,1);
	}
	#endif
    DEBUG_MSG(0x99);
	DEBUG_MSG(0x99);
	#if 0
	os_printf("LE_App_Local_Disconnect_Indication\r\n");
	{
		u_int8 i = 0;
		for(i = 0;i < 10;i++)
		{
			os_printf("log_IDX : %x\r\n",i);
			os_printf("log_current_clk : %x\r\n",log_current_clk[i]);
			os_printf("log_next_connect_event_start_time : %x\r\n",log_next_connect_event_start_time[i]);
		}
	}
	#endif
#endif
#ifdef GAP_MASTER_MODE
   //GAP_Connection_Establishment(GAP_DIRECTED_CONNECT,peer_address,PUBLIC_ADDRESS,6,6,0,10);/* hw simulate */
   //GAP_Connection_Establishment(GAP_DIRECTED_CONNECT,peer_address,PUBLIC_ADDRESS,24,24,0,72);/* interval = 24 * 1.25 = 30 ms */
   GAP_Connection_Establishment(GAP_GENERAL_CONNECT,peer_address,PUBLIC_ADDRESS,24,24,0,72);
#endif
#endif
    LE_App_Reset_L2cap_Para_Update_Timer();
	Notify_BLE_Disconnected();
    return NO_ERROR;
}

/**************************************************************************
 * FUNCTION :- LE_APP_GAP_Connection_Complete
 *
 * DESCRIPTION
 * This the Callback function invoked by the GAP layer when a connection complete has been
 * recieved from the lower layers and any necessary processing (i.e setup of an encrypted link)
 * is complete.
 *
 * This is mainly used by the CTS based app (Server Simple Service) to cancle alerting
 * (if the Proximity/Link Loss was in use). Also used to trigger Discovery for the handles
 * of the Timer Characteristics on the Peer device.
 *************************************************************************/
void LE_APP_GAP_Connection_Complete(u_int8 event,t_gap_event_params* p_event_params )
{
    /* u_int8 dummy_addr[6] = { 0x00,0x00,0x00,0x00,0x00,0x00 }; */
	#ifdef GAP_MASTER_MODE
   u_int8  peer_address[6] = {0x61, 0x33, 0xcc, 0xdd, 0xee, 0xff};
   #endif
    //u_int32 current_clk = HW_Get_Native_Clk_Avoid_Race();
    if (!App_Config_App_Present)
        return;

    if (p_event_params->status == NO_ERROR)
    {
#if (APP_DEFINE == APP_HT_DEVICE)
        if  (App_Config_Type == APP_HEALTH_SERVICES)
        {
            LEapp_Config.TAPP_HT_Update_Timer = HW_Get_Native_Clk_Avoid_Race() + 400;
            LEapp_Config.HT_Updated = 1;
        }
#endif
        LE_App_ACL_Handle = p_event_params->handle;
#if 0 // no used
        User_App_Alerting(0);
#endif
        //if (App_Config_App_Present && (App_Config_Type != HID_APPLICATION) && (p_event_params->status == NO_ERROR))
        if(App_Config_App_Present && (p_event_params->status == NO_ERROR))
        {
        	//Save org connection_interval
            // On Connect Stop and Alerting
            //LE_App_Generate_Event(APP_ALERTING_EVENT,0x00,0x00);
            LE_App_Set_L2cap_Para_Update_Timer(HW_Get_Native_Clk_Avoid_Race() + 3200*2);
            //LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer = current_clk+(0x7D00);//charles modify after connection complete wait 10 s to start l2cap parameter change
            LEapp_Config.TAPP_POWERON_STANDBY_Timer = BT_CLOCK_MAX_TIMEOUT;
			#ifdef BATTERY_DETECT
            Restart_Battery_Detect_Timer();
			#endif
            // Indicate to UI (on PC that connection has been setup
            //LE_App_Generate_Event(APP_CONNECT_EVENT,0x00,0x00);
			

        }
		DEBUG_MSG(0x97);
    }
    else
    {
        // Connection Failure
        // Go Back Connectable..!!
        #ifdef BATTERY_DETECT
        LEapp_Config.TAPP_BatteryDetect_Timer = BT_TIMER_OFF;
		#endif
        //LEapp_Config.TAPP_DISCOVERABLE_Timer = current_clk+TGAP_adv_fast_period*2;
        #ifdef GAP_SLAVE_MODE
        GAP_Discoverable_Mode(GAP_GENERAL_DISCOVERABLE,ADV_IND,1);
		#endif
		#ifdef GAP_MASTER_MODE
		 /* u_int8 GAP_Connection_Establishment(u_int8 mode, u_int8* p_peer_address,u_int8 peer_address_type,
		                            u_int16 max_conn_int, u_int16 min_conn_int,u_int16 latency, u_int16 super_to) */
        //GAP_Connection_Establishment(GAP_DIRECTED_CONNECT,peer_address,PUBLIC_ADDRESS,6,6,0,10);/* hw simulate */
		//GAP_Connection_Establishment(GAP_DIRECTED_CONNECT,peer_address,PUBLIC_ADDRESS,24,24,0,72);/* interval = 24 * 1.25 = 30 ms */
		GAP_Connection_Establishment(GAP_GENERAL_CONNECT,peer_address,PUBLIC_ADDRESS,24,24,0,72);
		#endif
        //GAP_Connectable_Mode(0x02 /*GAP_UNDIRECTED_CONNECTABLE*/, 0x00 /*PUBLIC_ADDRESS*/, 0x00/*PUBLIC_ADDRESS*/,dummy_addr);
        //LE_App_Generate_Event(APP_CONNECTABLE_EVENT,0x00,0x00);
		

    }
	Notify_BLE_Connected();
    return;
}
/**************************************************************************
 * FUNCTION :- LE_APP_GATT_Command_Complete
 *
 * DESCRIPTION
 * This the Callback function invoked by the GATT layer when a procedure is complete
 * It is used by the Current Time Application to allow the Discovery of the Handles for
 * Time Characteristics on the Peer Devices GATT DB.
 *
 * It is used in combination with 'LE_APP_GATT_Event_Handler' to progress through the
 * discovery state machine. Once discovery is complete it also writes to the Client
 * Charactaristic Configuration for the Current Time Characteristic
 *************************************************************************/

#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
void LE_APP_GATT_Command_Complete(u_int8 operation_id,t_gatt_command_complete_params* p_params)
{

    //u_int8 dummy_UUID[2] = { 0x00, 0x00 };

    switch(operation_id)
    {
    case 0x01 :// Primary Service Discovery :

        break;


    case 0x03 : // GATT_CHARACTERISTIC_DISCOVERY :


        break;
    }
}
#endif
/**************************************************************************
 * FUNCTION :- LE_App_Generate_Event
 *
 * DESCRIPTION
 * Simple function for encoding Events which have to be sent to the Application
 * over the Serial/UART IF.
 *
 *************************************************************************/

void LE_App_Generate_Event(u_int8 event,u_int8 para1,u_int8* p_data)
{
#if 0
    u_int8* p_buffer;
    u_int8  i;
    struct q_desc* qd;
    /*
     * Determine event length:
     *     2 bytes for event header
     *     1 byte for event
     *     1 byte for param
     */
    if (!App_Config_App_Present)
        return;

    if (event == APP_SCAN_INTERVAL_INDICATION)
    {
        qd=BTq_Enqueue(HCI_EVENT_Q, 0, (t_length)
                       (6+SIZE_OF_EVENT_HEADER ) );
    }
    else if (( event ==  APP_LOCAL_TIME_INFORMATION_UPDATE_INDICATION ) ||
             ( event == APP_CURRENT_TIME_UPDATE_INDICATION ) ||
             ( event == APP_REFERENCE_TIME_INFORMATION_UPDATE_INDICATION ) ||
             ( event == APP_DATE_TIME_WITH_DST_UPDATE_INDICATION) ||
             ( event == APP_HID_REPORT_OUTPUT_INDICATION) ||
             ( event == APP_HID_BOOT_KEYBOARD_OUTPUT_INDICATION)||
             ( event ==APP_HID_BOOT_KEYBOARD_INPUT_INDICATION)||
             ( event ==APP_HID_BOOT_MOUSE_INPUT_INDICATION)||
             ( event ==APP_HID_REPORT_FEATURE_INDICATION)||
             ( event ==APP_HID_REPORT_INPUT_INDICATION) ||
             ( event ==APP_HID_CONTROL_POINT_INDICATION) ||
             ( event ==APP_HRS_CONTROL_POINT_INDICATION) ||
             ( event ==APP_CSC_REPORT_CONTROL_POINT_INDICATION) ||
             ( event ==APP_RSC_REPORT_CONTROL_POINT_INDICATION) ||
             ( event ==APP_HID_PROTOCOL_MODE_INDICATION) ||
             ( event ==APP_HT_MEASUREMENT_INTERVAL_INDICATION)
             )
    {
        qd=BTq_Enqueue(HCI_EVENT_Q, 0, (t_length)
                       (para1+2+SIZE_OF_EVENT_HEADER ) );

    }
    else
    {
        qd=BTq_Enqueue(HCI_EVENT_Q, 0, (t_length)
                       (2+SIZE_OF_EVENT_HEADER ) );
    }


    if(qd)
    {
        p_buffer = qd->data;
    }
    else
    {
        return;
    }

    switch(event)
    {

    case APP_ALERTING_EVENT :
    case APP_CONNECT_EVENT :
    case APP_DISCONNECT_EVENT :
    case APP_CONNECTABLE_EVENT :
        p_buffer[2] = event;
        p_buffer[3] = para1;
        break;


    case APP_SCAN_INTERVAL_INDICATION :
        p_buffer[2] = event;
        p_buffer[3] = para1;
        for (i=0;i<para1;i++)
        {
            p_buffer[4+i] = p_data[i];
        };
        break;

    case APP_HID_CONTROL_POINT_INDICATION :
    case APP_HID_PROTOCOL_MODE_INDICATION :
    case APP_HID_REPORT_INPUT_INDICATION :
    case APP_HID_REPORT_FEATURE_INDICATION :
    case APP_HID_BOOT_MOUSE_INPUT_INDICATION :
    case APP_HID_BOOT_KEYBOARD_INPUT_INDICATION :
    case APP_HID_BOOT_KEYBOARD_OUTPUT_INDICATION :
    case APP_HID_REPORT_OUTPUT_INDICATION :

    case APP_LOCAL_TIME_INFORMATION_UPDATE_INDICATION :
    case APP_CURRENT_TIME_UPDATE_INDICATION :
    case APP_REFERENCE_TIME_INFORMATION_UPDATE_INDICATION :
    case APP_DATE_TIME_WITH_DST_UPDATE_INDICATION :
    case APP_HRS_CONTROL_POINT_INDICATION :
    case APP_CSC_REPORT_CONTROL_POINT_INDICATION :
    case APP_RSC_REPORT_CONTROL_POINT_INDICATION :
    case APP_HT_MEASUREMENT_INTERVAL_INDICATION :
        p_buffer[2] = event;
        p_buffer[3] = para1;
        for (i=0;i<para1;i++)
        {
            p_buffer[4+i] = p_data[i];
        }
        break;

    }
    p_buffer[0] = HCI_LE_APP_EVENT;

    if (event == APP_SCAN_INTERVAL_INDICATION)
        p_buffer[1] = 6;
    else if (( event ==  APP_LOCAL_TIME_INFORMATION_UPDATE_INDICATION ) ||
             ( event == APP_CURRENT_TIME_UPDATE_INDICATION ) ||
             ( event == APP_REFERENCE_TIME_INFORMATION_UPDATE_INDICATION ) ||
             ( event == APP_DATE_TIME_WITH_DST_UPDATE_INDICATION) ||
             ( event == APP_HID_BOOT_KEYBOARD_OUTPUT_INDICATION) ||
             ( event == APP_HID_REPORT_OUTPUT_INDICATION) ||
             ( event == APP_HID_REPORT_INPUT_INDICATION ) ||
             ( event == APP_HID_REPORT_FEATURE_INDICATION ) ||
             ( event == APP_HID_BOOT_MOUSE_INPUT_INDICATION ) ||
             ( event == APP_HID_BOOT_KEYBOARD_INPUT_INDICATION ) ||
             ( event == APP_HID_CONTROL_POINT_INDICATION) ||
             ( event == APP_HRS_CONTROL_POINT_INDICATION) ||
             ( event == APP_CSC_REPORT_CONTROL_POINT_INDICATION ) ||
             ( event == APP_RSC_REPORT_CONTROL_POINT_INDICATION ) ||
             ( event == APP_HID_PROTOCOL_MODE_INDICATION) ||
             ( event == APP_HT_MEASUREMENT_INTERVAL_INDICATION)

             )
    {
        p_buffer[1] = para1+2;
    }
    else
        p_buffer[1] = 2;

    BTq_Commit(HCI_EVENT_Q, 0);
#endif
}

#if 0 // no used
void User_App_Alerting(u_int8 alerting)
{
    u_int32 current_clk = HW_Get_Native_Clk_Avoid_Race();

    if (alerting < 0x04)
        //LE_App_Generate_Event(APP_ALERTING_EVENT,alerting,0x00);

    switch(alerting)
    {
    case 0 :
        // Disable All Alerting
#if 0 // Use for PC
        //LE_App_Generate_Event(APP_ALERTING_EVENT,alert_level,0x00);//no used #if 0
#endif

        LEapp_Config.TAPP_Alerting_Timer = BT_CLOCK_MAX_TIMEOUT;

        LEapp_Config.Alert_Level = 0x00;

        User_App_Flash_Leds_OFF();
        User_App_Flash_7Segment(LEapp_Config.Alert_Level);


        break;

    case 1 :
        // Medium level alerting

        LEapp_Config.TAPP_Alerting_Timer = current_clk + 800;

        LEapp_Config.Alert_Level = 0x01;
        break;

    case 2 :
        // High Level Alerting
        LEapp_Config.TAPP_Alerting_Timer = current_clk + 200;
        LEapp_Config.Alert_Level = 0x02;
        break;

    }
}
#endif
#if (ADC_ON == 1)
u_int8 Battery_level_to_Percentage(u_int16 level)//for battery detect function
{
    u_int8 i;

    level &= ~0x03;
    for(i = 0;i < 11;i++)
    {
        if(level >= ADC_table[i])
            break;
    }
    return (100-(i*10));
}
/*************************************************************
 * SUPPORT FUNCTIONS FOR ALERTING
 *
 *************************************************************/
void Trans_ADC_Battery_Cap(void)
{
    u_int8 Battery_Capacity=0;
    if (ADC_captrue == TRUE)
    {
        if(ADC_finish == TRUE)
        {
            ADC_captrue = FALSE;
            ADC_finish = FALSE;

            BKADC_DISABLE();
            BKADC_CLK_PowerDown();
            BKADC_INT_DISABLE();

            Battery_Capacity = Battery_level_to_Percentage(Get_Battery());
            Save_Power_on_Battery(Battery_Capacity);
            GATTserver_Characteristic_Write_Local_Value( BAS_SERVER_BATTERY_LEVEL_HANDLE,1,&Battery_Capacity);
          
        }
    }
}
#endif
void User_App_Check_Timers(void)
{
    u_int32 current_clk = HW_Get_Native_Clk_Avoid_Race();
	//extern t_LE_Config LE_config;
	//t_LE_Connection* p_connection = LEconnection_Find_Link_Entry(LE_config.active_link);
#if 0 // no used
    if ((LEapp_Config.TAPP_Alerting_Timer != BT_CLOCK_MAX_TIMEOUT) &&
        (((LEapp_Config.TAPP_Alerting_Timer - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
    {
        // Flash Leds and 7 Segment
        if (LEapp_Config.Flash_Type == 0x01)
        {
            LEapp_Config.Flash_Type = 0x00;
        }
        else
        {
            LEapp_Config.Flash_Type = 0x01;
        }
        User_App_Flash_Leds(LEapp_Config.Flash_Type);
        User_App_Flash_7Segment(LEapp_Config.Alert_Level);

        if (LEapp_Config.Alert_Level == 0x01)
            LEapp_Config.TAPP_Alerting_Timer = current_clk + 800;
        else if (LEapp_Config.Alert_Level == 0x02)
            LEapp_Config.TAPP_Alerting_Timer = current_clk + 300;
    }
#endif
#if (APP_DEFINE == APP_HT_DEVICE)
    if ((LEapp_Config.TAPP_HT_Update_Timer !=  BT_CLOCK_MAX_TIMEOUT) &&
        (((LEapp_Config.TAPP_HT_Update_Timer - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
    {
#if (PRH_BS_CFG_UART_FOR_SENSOR_ONLY==1)
        if (LEapp_Config.HT_Updated)
        {
            // Update the Temperature..
            LEapp_Config.HT_Updated = 0;
            {
                u_int32 mantissa = (u_int32)(LEapp_Config.New_Temp*10);
                u_int8 p_pdu[8] = {0x00 };

                p_pdu[4] = 0xFF; // Exponent = -1 = 0xFF
                p_pdu[3] = (mantissa & 0x00FF0000)>>16;
                p_pdu[2] = (mantissa & 0x0000FF00)>>8;
                p_pdu[1] = mantissa & 0xFF;

                LEapp_Config.Prev_Temp = LEapp_Config.New_Temp;
                GATTserver_Characteristic_Write_Local_Value(HT_TEMP_MEASUREMENT_HANDLE,0x05,p_pdu);
                User_App_Update_LCD();
            }
            LEapp_Config.TAPP_HT_Update_Timer = current_clk + 400;
        }
#else
        {
            if (LEapp_Config.TAPP_HT_Measurement_Interval!=0)
                LEapp_Config.TAPP_HT_Update_Timer = current_clk + (LEapp_Config.TAPP_HT_Measurement_Interval*3200);
            else
                LEapp_Config.TAPP_HT_Update_Timer = BT_CLOCK_MAX_TIMEOUT;
            // Read and Write the Measurement !!
            GATTserver_Characteristic_Write_Local_Value(HT_TEMP_MEASUREMENT_HANDLE,13,ht_temperature_measurement);
        }
#endif
    }







	
#endif/* #if (APP_DEFINE == APP_HT_DEVICE) */
#if 0
		if(LEconnection_LE_Connected())
		{
			//DEBUG_MSG(0x3E);
			if((p_connection->connInterval_org == p_connection->connInterval) && (LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer == BT_TIMER_OFF))
			{
				DEBUG_MSG(0x3F);
				//LE_App_Set_L2cap_Para_Update_Timer(HW_Get_Native_Clk_Avoid_Race() + 3200);
				//os_printf("Set_L2cap_Para_Update_Timer\r\n");
			}
		}
#endif
		#if 0
		if((p_connection->ll_action_pending & LL_CONNECTION_UPDATE_PENDING) == LL_CONNECTION_UPDATE_PENDING )
		{
			if(LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer !=  BT_TIMER_OFF)
			{
				//LE_App_Set_L2cap_Para_Update_Timer(LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer+ 200);
			}
		}
		#endif
		if(need_to_reset_l2cap_timer == 1)
		{
			need_to_reset_l2cap_timer = 0;
			//LE_App_Set_L2cap_Para_Update_Timer(HW_Get_Native_Clk_Avoid_Race() + 800);
			
		}
    if ((LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer !=  BT_TIMER_OFF) &&
        (((LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
    {
        LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer = BT_TIMER_OFF;
#if 1//def ENABLE_L2CAP_UPDATE_PARAMETER
       if(BLE_interval == LONG_BLE_INTERVAL)
       {
       		//DEBUG_MSG(0xE7);
       		//os_printf("call LE_L2CAP_Connection_Update\r\n");
       		//LE_L2CAP_Connection_Update(u_int16 handle, u_int16 max_interval,u_int16 min_interval, u_int16 latency,u_int16 timeout)
			 
			 LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*10,0x0048*10,0,600);//1000MS unit : 1.25ms
			 //LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*20,0x0048*20,0,600);//2000MS
			 //LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*20,0x0048*20,0,600);//2000MS
			 //LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,798,0x0048*10,0,600);
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*3,0x0048*3,0,0x0258);//300MS
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050,0x0048,0,0x0258);//
	   }
	   else
	   {
	   		/* Iphone limit  max > min, max - min ==  10ms ,timeout do not > 8 sec,Iphone will take max value */
			//DEBUG_MSG(0xE8);
			//LE_L2CAP_Connection_Update(0xFFFF,60,52,0,0x0258);
             LE_L2CAP_Connection_Update(0xFFFF,24,(24-8),0,0x0258);//30ms
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*3,(0x0050*3-8),0,0x0258);//300ms
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050,(0x0050-8),0,0x0258);//100ms
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*2,((0x0050*2)-8),0,0x0258);//200ms
			
			//LE_L2CAP_Connection_Update(0xFFFF,24*4,((24*4)-8),0,0x0258);
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*3,0x0048*3,0,0x0258);
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050,0x0048,0,0x0258);
			//LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x002E,0x0024,0,0x0258);
	   }
#ifdef CONNECT_INTERVAL_50MS
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x002E,0x0024,0,0x0258);
#endif				 
#ifdef CONNECT_INTERVAL_100MS
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050,0x0048,0,0x0258);
#endif	
#ifdef CONNECT_INTERVAL_200MS
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*2,0x0048*2,0,0x0258);
#endif			
#ifdef CONNECT_INTERVAL_300MS  
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*3,0x0048*3,0,0x0258);/*interval:270ms~300ms,latency:0,timeout:6s*/
#endif
#ifdef CONNECT_INTERVAL_400MS  
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*4,0x0048*4,0,0x0258);
#endif
#ifdef CONNECT_INTERVAL_500MS  
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0050*5,0x0048*5,0,0x0258);
#endif
#ifdef CONNECT_INTERVAL_600MS
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x01E0,0x0190,1,0x0258);//600ms
#endif
#ifdef CONNECT_INTERVAL_1000MS
        LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,800,800,0,600);//600ms
#endif
        //LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x00F0,0x00C8,0,0x12C);//300ms
        //LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x00FF,0x00C8,0,0x0258);//310ms
        /* max_interval = 0x00F0 * 1.25 ms,min_interval = 0x00C8 * 1.25 ms,latency = 0,supervision timeout : 0x0258 * 10 ms = 6000 ms */
        //LE_L2CAP_Connection_Update(0xFFFF/*use internal valid handle*/,0x0010,0x0008,0,0x0064);//20ms
#endif   
        //GATTserver_Characteristic_Write_Local_Value( KEY_PRESS_HANDLE,1,&Keypress_state);//charles test
    }


       



	
#ifdef BATTERY_DETECT
    if ((LEapp_Config.TAPP_BatteryDetect_Timer !=  BT_TIMER_OFF) &&
        (((LEapp_Config.TAPP_BatteryDetect_Timer - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
    {

        u_int8 Battery_Capacity;


        Restart_Battery_Detect_Timer();
#if (ADC_ON == 1)//for battery detect function
        read_adc_start();
        ADC_captrue = TRUE;
#endif
#if (ADC_ON == 0)//for battery detect function

        if(battery_level_value < 100)
        {
            battery_level_value+= 20;
        }
        else
        {
            battery_level_value = 20;
        }
        Battery_level_adc = battery_level_value;
        Battery_Capacity = Battery_level_adc;
#if(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
        GATTserver_Characteristic_Write_Local_Value( BAS_SERVER_BATTERY_LEVEL_HANDLE,1,&Battery_Capacity);
#endif
#endif
    }
#endif //#ifdef BATTERY_DETECT
    if ((LEapp_Config.TAPP_POWERON_STANDBY_Timer !=  BT_TIMER_OFF) &&
        (((LEapp_Config.TAPP_POWERON_STANDBY_Timer - current_clk) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
    {
        LEapp_Config.TAPP_POWERON_STANDBY_Timer = BT_TIMER_OFF;
#if (MMI_ON == 1)
#if (AUTO_ADVERTISING == 0)
        Set_MMI_State(MMI_DEEPSLEEP_STATE);
#endif
#endif
    }
    //      if ((LEapp_Config.TAPP_DISCOVERABLE_Timer !=  BT_CLOCK_MAX_TIMEOUT) &&
    //             (((LEapp_Config.TAPP_DISCOVERABLE_Timer - current_clk) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
    //     {
    //           LEapp_Config.TAPP_DISCOVERABLE_Timer = BT_CLOCK_MAX_TIMEOUT;
    //           GAP_Discoverable_Mode(GAP_GENERAL_DISCOVERABLE,ADV_IND);
    //     }
#if 0
    if(LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer == BT_TIMER_OFF)
    {
    	if(LMscoctr_Get_Number_eSCO_Connections() > 0)
    	{
    		if(BLE_interval == SHORT_BLE_INTERVAL)
    		{
				BLE_interval = LONG_BLE_INTERVAL;
				//LE_App_Set_L2cap_Para_Update_Timer(HW_Get_Native_Clk_Avoid_Race() + 1600);
    		}
    	}
		else
		{
			if(BLE_interval == LONG_BLE_INTERVAL)
    		{
				BLE_interval = SHORT_BLE_INTERVAL;
				//LE_App_Set_L2cap_Para_Update_Timer(HW_Get_Native_Clk_Avoid_Race() + 1600);
    		}
		}
    }
#endif
#if (PRH_BS_CFG_EMBEDDED_PULSE_SENSOR==1)
    if ((LEapp_Config.TAPP_HR_Update_Timer !=  BT_TIMER_OFF) &&
        (((LEapp_Config.TAPP_HR_Update_Timer - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
    {

        // When the Timer for Pulse Rate Measurement expires, we read the Pulse Rate In Register ( PULSE_TIMER_IN32BIT_BASE)
        // This is a 32-bit input register with the following fields :
        //                 bit31: value valid,
        //              bit30: no signal for 10 sec +,
        //              bit29: pulse duration overflow,
        //              bits 28-15: pulse duration in 100us steps,
        //              bit14: pulse interval overflow,
        //              bits13-0: pulse interval in 100us steps.
        // So the critical bits are bit31     - value valid
        //                      and bits13-0  - pulse interval in 100us steps

        // I store a reading of the last 20 samples. I store a reading for the total aggregate time for the last
        // 20 samples - and used this to calculate the BPM.

        volatile u_int32 *pulseIn = (u_int32*) PULSE_TIMER_IN32BIT_BASE;
        volatile u_int32 *pulseOut = (u_int32*) PULSE_TIMER_OUT8BIT_BASE;
        u_int32 pulseValue;
        char str[40];

        pulseValue = *pulseIn;

        if(pulseValue & 0x40000000)
        {
            // No Signal for 10 seconds +
            LEapp_Config.HR_Sample_Number=0x00;
            *pulseOut = 1;
            *pulseOut = 0;
            *pulseIn = 0x00;

            sprintf(str,"BMP :- No Input");

            LCD_Init();
            LCD_Show_Text(str);

            LEapp_Config.HR_Sample_Number=0;
            LEapp_Config.HR_Updated = 1;
            LEapp_Config.HR_BPM = 0;

        }
        else if (pulseValue & 0x00004000)
        {
            // Pulse Interval Overflow
            LEapp_Config.HR_Sample_Number=0x00;
            *pulseOut = 1;
            *pulseOut = 0;
            *pulseIn = 0x00;

            sprintf(str,"BMP :- Overflow");
            LCD_Init();
            LCD_Show_Text(str);

            LEapp_Config.HR_Sample_Number=0;
            LEapp_Config.HR_Updated = 1;
            LEapp_Config.HR_BPM = 0;

        }
        else if(pulseValue & 0x80000000)
        {
            u_int32 num_clks_forNpulses;
            u_int32 num_clks_per_pulse;
            u_int32 pulse_interval = pulseValue & 0x3FFF;
            u_int32 heart_rate =  (600000/pulse_interval);
            LEapp_Config.HR_Sample_Number++;
#if 0
            g_pulse_count++;
            *pulseIn = 0x00;
            sprintf(str,"BPM :- %5d    ", heart_rate);
            LCD_Init();
            LCD_Show_Text(str);
#endif
            // Store the Time of the first Pulse
            if (LEapp_Config.HR_Sample_Number==0x01)
                LEapp_Config.HR_Clk_FirstPulse = current_clk;


            if (LEapp_Config.HR_Sample_Number == HEART_RATE_SAMPLES)
            {
                // Store the Time of the Tenth Pulse
                LEapp_Config.HR_Clk_NthPulse = current_clk;

#if 0
                // Reset the Aggregate interval Timer
                LEapp_Config.HR_Updated = 0x01;
                LEapp_Config.HR_Aggretate_Time_for_10pulse += pulse_interval;

                LEapp_Config.HR_BPM = (u_int16) (60000/LEapp_Config.HR_Aggretate_Time_for_100pulse);
                LEapp_Config.HR_Sample_Number = 0x0;
                LEapp_Config.HR_Aggretate_Time_for_10pulse = 0x00;
#else
                {


                    num_clks_forNpulses = LEapp_Config.HR_Clk_NthPulse - LEapp_Config.HR_Clk_FirstPulse;

                    // Determine the num clk ticks per pulse.
                    num_clks_per_pulse = num_clks_forNpulses/HEART_RATE_SAMPLES;

                    // Now Determine num pulses per min
                    LEapp_Config.HR_BPM = (u_int16)((60*3200)/num_clks_per_pulse);
                    //        User_App_Update_LCD();
                    LEapp_Config.HR_Sample_Number = 0x0;
                    //
                    // Now Update the GATT DB
                    LEapp_Config.HR_Updated = 0x01;
                }
#endif

                //
                //SYSmmi_Display_Numeric((u_int16)LEapp_Config.HR_BPM);
                //SYSmmi_Display_Numeric2((u_int16)LEapp_Config.HR_BPM);
                //SYSmmi_Display_Decimal2((u_int16)LEapp_Config.HR_BPM,0);
                //SYSmmi_Display_Numeric2(0x0000);
            }
            else
            {
                LEapp_Config.HR_Aggretate_Time_for_10pulse += pulse_interval;
            }
            *pulseOut = 1;
            *pulseOut = 0;
        }


        LEapp_Config.TAPP_HR_Update_Timer = current_clk + 32;
        if (LEapp_Config.HR_Updated)
        {
            // Update the Heart Rate Updated..
            LEapp_Config.HR_Updated = 0;

            {
                u_int8 p_pdu[8] = {0x01 };

                p_pdu[2] = (LEapp_Config.HR_BPM & 0x0000FF00)>>8;
                p_pdu[1] = LEapp_Config.HR_BPM & 0xFF;

                //SYSmmi_Display_Decimal2((u_int16)LEapp_Config.HR_BPM,0);
                GATTserver_Characteristic_Write_Local_Value(HRS_MEASUREMENT_HANDLE,0x03,p_pdu);
                User_App_Update_LCD();
            }
        }
    }
#endif/* #if (PRH_BS_CFG_EMBEDDED_PULSE_SENSOR==1) */
}

#if 0 //no used
void User_App_Flash_Leds(u_int8 ON_OFF)
{
    u_int8 i;
    for(i=0; i<16; i++)
    {
        if (ON_OFF==1)
            HWled_Toggle(i);
        else
            HWled_Toggle(i);
    }
}

void User_App_Flash_Leds_OFF(void)
{
    u_int8 i;
    for(i=0; i<16; i++)
    {
        HWled_Off(i);
    }
}
#endif

void LE_App_Set_L2cap_Para_Update_Timer(u_int32 time)
{
    LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer =  time;
}

void LE_App_Reset_L2cap_Para_Update_Timer(void)
{
    LEapp_Config.TAPP_L2CAP_PARA_UPDATE_Timer =  BT_TIMER_OFF;
}

void User_App_Flash_7Segment(u_int8 level)
{
#if 0
    switch(level)
    {
    case 0x00 :
        SYSmmi_Display_Numeric(0x0000);
        break;

    case 0x01 :
        SYSmmi_Display_Numeric(0x1111);
        break;

    case 0x02 :
        SYSmmi_Display_Numeric(0x2222);
        break;
    }
#endif
}

void User_App_Update_LCD(void)
{
#ifdef LCD_ENABLED
    char str[40];
    sprintf(str,"BPM :- %5d    ", LEapp_Config.HR_BPM);
    LCD_Init();
    LCD_Show_Text(str);
    LCD_Line2();
    sprintf(str,"Temp :- %4f    ", LEapp_Config.New_Temp);
    LCD_Show_Text(str);
#endif
}


#endif

#if (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1) && (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
/********************************************************************
 * GATT Server Database setup functions.
 * Following functions are used to configure the GATT DATABASE
 */

/*******************************************************
 * GERERIC ACCESS PROFILE DEFINITION
 * Handle 0x10 to 0x1A
 *
 * Peripheral Privacy is at 0x18
 * ReConnection Addr  is at 0x1A
 ******************************************************/

u_int16 App_Setup_GAP_GATT_DB(void)
{
    u_int16 handle = 0x10; /* GAP handles goes from 0x10- 0x1B   */
    extern u_int8 GATTprivacy_Flags[];
    extern u_int16 GATT_reconnection_address_handle;

    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,GENERIC_ACCESS_PROFILE);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,DEVICE_NAME,0x08);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,DEVICE_NAME,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ |GATT_PERMISSION_WRITE,13,gap_name_array);
    //_GATTserver_Add_Characteristic_Value(handle,DEVICE_NAME,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ |GATT_PERMISSION_WRITE|GATT_PERMISSION_AUTHENTICATED,16,gap_name_array);//charles modify
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,APPERANCE,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,APPERANCE,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,1,gap_appearance_array);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,7,gap_connection_params);
    handle++;


#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)
    _GATTserver_Add_Characteristic_Declaration(handle,PERIPHERAL_PRIVACY_FLAGS,0x02);
    // Value handle = 0x18
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,PERIPHERAL_PRIVACY_FLAGS,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,1,GATTprivacy_Flags);
    handle++;
#endif

    // handle = 0x19
    _GATTserver_Add_Characteristic_Declaration(handle,RECONNECTION_ADDRESS,0x02);
    handle++;
    // Value handle = 0x1A
    GATT_reconnection_address_handle = handle;
    _GATTserver_Add_Characteristic_Value(handle,RECONNECTION_ADDRESS,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,6,reconnection_addr);
    //0x10+0x0A = 0x1A Enablehandle
    return handle;
}


/*******************************************************
 * GATT PROFILE DEFINITION
 * Handle 0x20 to 0x22
 *
 * Service Changed is at 0x22
 *
 ******************************************************/
u_int16 App_Setup_GATT_GATT_DB(void)
{
    u_int16 handle = 0x20;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,GENERIC_ATTRIBUTE_PROFILE);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,SERVICE_CHANGED,0x20);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,SERVICE_CHANGED,NO_LOCAL_NOTIFY,0x00,4,Service_Change_Range);
    handle++;

    _GATTserver_Add_Char_Descriptor(handle,CLIENT_CHARACTERISTIC_CONFIGURATION,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,0x02,GATT_service_change_CCC);
    //0x20+0x03 = 0x23 Enable handle
    return handle;
}


/*******************************************************
 * DERVICE INFORMATION PROFILE DEFINITION
 * Handle 0x30 to 0x42
 *
 ******************************************************/
u_int16 App_Setup_DEV_INFO_GATT_DB(void)
{
    u_int16 handle = 0x30;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,DEVICE_INFORMATION);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_MANUFACTURER_NAME,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_MANUFACTURER_NAME,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,10,manufacturer_name_string);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_MODEL_NUMBER,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_MODEL_NUMBER,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,6,model_number_string);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_SERIAL_NUMBER,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_SERIAL_NUMBER,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,10,serial_number_string);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_FW_REVISION,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_FW_REVISION,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,4,fw_revision_string);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_HW_REVISION,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_HW_REVISION,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,4,hw_revision_string);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_SW_REVISION,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_SW_REVISION,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,4,sw_revision_string);
    handle++;


    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_SYSTEM_ID,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_SYSTEM_ID,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,8,system_id);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_REG_CERT,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_REG_CERT,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,4,IEEE_Reg_Cert);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_PnP_ID,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_PnP_ID,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,7,pnp_id);
    //0x30+0x12 = 0x66 End handle
    return handle;
}

/*******************************************************
 * IMMEDIATE ALERT PROFILE DEFINITION
 * Handle 0x50 to 0x52
 *
 *
 ******************************************************/
u_int16 App_Setup_IAS_GATT_DB(void)
{
    u_int16 handle = 0x70;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,IMMEDIATE_ALERT);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_ALERT_LEVEL,0x04);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_ALERT_LEVEL,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,1,alert_level_array);
    //0x70+0x02 = 0x72 Enable handles
    return handle;
}

/*******************************************************
 * TX POWER PROFILE DEFINITION
 * Handle 0x60 to 0x62
 *
 ******************************************************/

u_int16 App_Setup_TX_POWER_GATT_DB(void)
{
    u_int16 handle = 0x80;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,TX_POWER);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,TX_POWER_LEVEL,0x12);//Read,Notify
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,TX_POWER_LEVEL,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,1,tx_power_array);
    handle++;

    _GATTserver_Add_Char_Descriptor(handle,CLIENT_CHARACTERISTIC_CONFIGURATION,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,0x02,TX_Power_Level_CCC);

    handle++;

    tx_power_format[0] = 12; // Format
    tx_power_format[1] = 0; // Exponent
    LMutils_Set_Uint16(tx_power_format+2, 0x2726); /* UUID for Watts */
    tx_power_format[4] = 1; // Name Space - Sig Specified
    LMutils_Set_Uint16(tx_power_format+5, 0x000E);

    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_PRESENTATION_FORMAT,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,0x07,tx_power_format);
    //0x70+0x04 = 0x84 Enable handles
    return handle;
}

/********************************************************
 * LINK LOSS PROFILE DEFINITION
 * Handle 0x70 to 0x72
 ********************************************************/
u_int16 App_Setup_LINK_LOSS_GATT_DB(void)
{
    u_int16 handle = 0x90;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,LINK_LOSS);
    handle++;
    _GATTserver_Add_Characteristic_Declaration(handle,GATT_CHAR_ALERT_LEVEL,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,GATT_CHAR_ALERT_LEVEL,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,1,link_loss_alert_level_array);
    //0x80+0x02 = 0x92 Enable handles
    return handle;
}

/********************************************************
 * BATTERY SERVICE PROFILE DEFINITION
 * Handle 0x80 to 0x82
 ********************************************************/
u_int16 App_Setup_BATTERY_SERVICE_GATT_DB(void)
{
    u_int16 handle = 0xA0;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,BATTERY_SERVICE);
    handle++;
    //handle = 0xA1
    _GATTserver_Add_Characteristic_Declaration(handle,BATTERY_LEVEL ,0x12);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    //bit 7 : Extended Properties

    handle++;
    //handle = 0xA2
    _GATTserver_Add_Characteristic_Value(handle,BATTERY_LEVEL,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,1,battery_level);//battery level initial value for var declare
    handle++;
    //handle = 0xA3
    _GATTserver_Add_Char_Descriptor(handle,CLIENT_CHARACTERISTIC_CONFIGURATION,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,0x02,Battery_Service_CCC);

    handle++;
    //handle = 0xA4
    battery_service_format[0] = 4; // Format
    battery_service_format[1] = 0; // Exponent
    LMutils_Set_Uint16(battery_service_format+2, 0x27AD); /* UUID for Percentage */
    battery_service_format[4] = 1; // Name Space - Sig Specified
    LMutils_Set_Uint16(battery_service_format+5, 0x000E);

    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_PRESENTATION_FORMAT,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,0x07,battery_service_format);
    //0xA0+0x04 = 0xA4 Enable handles
    return handle;
}
#if 1
/*******************************************************
 * KEY PRESS STATE DEFINITION (USER DEFINE)
 * Handle
 *
 * Service Changed is at
 *
 ******************************************************/
u_int16 App_Setup_FFE0_GATT_DB(void)
{
// add begin by cai.zhong 20190221 for ble server
#ifdef __SMARTVOICE__
	u_int16 handle = 0xC0;
	_GATTserver_Add_Service(PRIMARY_SERVICE_128BIT, handle, lp_service_uuid_128_content, 0);
    handle++;

    //_GATTserver_Add_Characteristic_Declaration(handle,0xFFE1,0x10);
    //Characteristic Properties
    // bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    //bit 7 : Extended Properties
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, lp_cmd_tx_char_val_uuid_128_content,0x12);
    handle++;
	
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, lp_cmd_tx_char_val_uuid_128_content, USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE,CMD_UUID_Send_CHAR_Value);
	handle++;
	
	_GATTserver_Add_Char_Descriptor(handle,CLIENT_CHARACTERISTIC_CONFIGURATION,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,0x02,CMD_UUID_RX_CHAR_Value_CCC);	
    handle++;
    
	_GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param0_str);
    handle++;
	
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, lp_cmd_rx_char_val_uuid_128_content,0x08);//0x04
    handle++;
	
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, lp_cmd_rx_char_val_uuid_128_content, USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE,CMD_UUID_Receive_CHAR_Value);	
	handle++;
	
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param1_str);
    handle++;

    //_GATTserver_Add_Characteristic_Declaration(handle,0xFFE1,0x10);
    //Characteristic Properties
    // bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    //bit 7 : Extended Properties
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, lp_data_tx_char_val_uuid_128_content,0x12);
    handle++;
	
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, lp_data_tx_char_val_uuid_128_content, USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE,DATA_UUID_Send_CHAR_Value);
	handle++;
	
	_GATTserver_Add_Char_Descriptor(handle,CLIENT_CHARACTERISTIC_CONFIGURATION,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,0x02,DATA_UUID_RX_CHAR_Value_CCC);	
    handle++;
    
	_GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param2_str);
    handle++;
	
    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle, lp_data_rx_char_val_uuid_128_content,0x08);//0x04
    handle++;
	
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle, lp_data_rx_char_val_uuid_128_content, USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,DEFAULT_LP_PDD_BLE_DATA_SEC_SIZE,DATA_UUID_Receive_CHAR_Value);	
	handle++;
	
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param3_str);
    handle++;
    
#else // __SMARTVOICE__
    //u_int16 handle = 0xB0;
	u_int16 handle = 0x01;//only used this database
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,0xFFE0);
    handle++;
    //handle = 0xB1;
    					/**************EQ_Param0*************/
    _GATTserver_Add_Characteristic_Declaration(handle,0xFFE1,0x06);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    //bit 7 : Extended Properties
    handle++;
    //handle = 0xB2;

    _GATTserver_Add_Characteristic_Value(handle,0xFFE1,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,11,EQ_Param0);
    handle++; //handle = 0xB3;

    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param0_str);
    handle++; //handle = 0xB4;
    /*********END*****EQ_Param0*************/


		/**************EQ_Param1*************/
	_GATTserver_Add_Characteristic_Declaration(handle,0xFFE2,0x06);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
	// bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
	//bit 7 : Extended Properties
	handle++; //handle = 0xB5;

	_GATTserver_Add_Characteristic_Value(handle,0xFFE2,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,11,EQ_Param1);
	handle++; //handle = 0xB6;

	_GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param1_str);
	handle++; //handle = 0xB7;
		/*********END*****EQ_Param1_*************/



	/**************EQ_Param2*************/
	_GATTserver_Add_Characteristic_Declaration(handle,0xFFE3,0x06);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
	// bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
	//bit 7 : Extended Properties
	handle++; //handle = 0xB8;

	_GATTserver_Add_Characteristic_Value(handle,0xFFE3,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,11,EQ_Param2);
	handle++; //handle = 0xB9;

	_GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param2_str);
	handle++; //handle = 0xBA;
	/*********END*****EQ_Param2*************/
	#if 0
	/**************EQ_Param3*************/
	_GATTserver_Add_Characteristic_Declaration(handle,0xFFE4,0x06);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
	// bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
	//bit 7 : Extended Properties
	handle++; //handle = 0xBB;

	_GATTserver_Add_Characteristic_Value(handle,0xFFE4,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,11,EQ_Param3);
	handle++; //handle = 0xBC;

	_GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param3_str);
	handle++; //handle = 0xBD;
	/*********END*****EQ_Param3*************/

	/**************EQ_Param4*************/
	_GATTserver_Add_Characteristic_Declaration(handle,0xFFE5,0x06);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
	// bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
	//bit 7 : Extended Properties
	handle++; //handle = 0xBE;

	_GATTserver_Add_Characteristic_Value(handle,0xFFE5,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,11,EQ_Param4);
	handle++; //handle = 0xBF;

	_GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,EQ_Param4_str);
	handle++; //handle = 0xC0;
	/*********END*****EQ_Param4*************/
	#endif
#endif // __SMARTVOICE__
// add end by cai.zhong 20190221 for ble server
    return handle;
}
#endif
#if 1
/*******************************************************
 * MISC STATE DEFINITION (USER DEFINE)
 * Handle
 *
 * Service Changed is at
 *
 ******************************************************/
u_int16 App_Setup_FFA0_GATT_DB(void)
{
    u_int16 handle = 0xf0;

    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT, handle,0x00,0xFFA0);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xFFA1,0x0A);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    //bit 7 : Extended Properties
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xFFA1,NO_LOCAL_NOTIFY,0x00,1,Range_Alarm_Enable);
    handle++;
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,18,Range_Alarm_Enable_str);
    handle++;


    _GATTserver_Add_Characteristic_Declaration(handle,0xFFA2,0x0A);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
                                                                   // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xFFA2,NO_LOCAL_NOTIFY,0x00,1,Beep_range);
    handle++;
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,10,Beep_range_str);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xFFA3,0x02);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xFFA3,NO_LOCAL_NOTIFY,0x00,1,Reserve0);
    handle++;
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,Reserve0_str);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xFFA4,0x02);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xFFA4,NO_LOCAL_NOTIFY,0x00,1,Reserve1);
    handle++;
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,Reserve1_str);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xFFA5,0x02);//Characteristic Properties// bit 0 :Broadcast, bit 1 : Read , bit 2 : Write Without Response, bit 3 : Write
    // bit 4 : Notify, bit 5 : Indicate, bit 6 : Authenticated Signed Writes,
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xFFA5,NO_LOCAL_NOTIFY,0x00,1,Reserve2);
    handle++;
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,9,Reserve2_str);

    //0xC0+0x0F = 0xCF Enable handle
    return handle;
}
#endif




/********************************************************
 * SCAN PRAMETERS PROFILE SERVICE DEFINITION
 * Handle 0x90 to 0x95
 ********************************************************/

u_int16 App_Setup_SCAN_PARAMETERS_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}

/********************************************************
 * CURRENT TIME PROFILE SERVICE DEFINITION
 * Handle 0xA0 to 0xA7
 ********************************************************/

u_int16 App_Setup_CURRENT_TIME_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}


/********************************************************
 * REFERENCE TIME PROFILE SERVICE DEFINITION
 *  Handle 0xB0 to 0xB4
 ********************************************************/

u_int16 App_Setup_REFERENCE_TIME_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}

/********************************************************
 * NEXT DST CHANGE PROFILE SERVICE DEFINITION
 * Handle 0xC0 to 0xB4
 ********************************************************/

u_int16 App_Setup_NEXT_DST_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;

}


/********************************************************
 * HID - PROFILE SERVICE DEFINITION
 * Handle 0xD0 to 0xD
 ********************************************************/

u_int16 App_Setup_HID_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}


/********************************************************
 * HRS - Heart Rate ServicePROFILE SERVICE DEFINITION
 * Handle 0xF0 to 0xFF
 ********************************************************/
u_int16 App_Setup_HRS_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}

/********************************************************
 * HT - Health Thermometer
 * Handle 0x100 to 0x110
 ********************************************************/
u_int16 App_Setup_HT_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}


/********************************************************
 * CSC - Cycling Speed and Cadence - PROFILE SERVICE DEFINITION
 * Handle 0x110 to 0x120
 ********************************************************/
u_int16 App_Setup_CSC_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}

/********************************************************
 * RSC - Running Speed and Cadence - PROFILE SERVICE DEFINITION
 * Handle 0x120 to 0x12F
 ********************************************************/
u_int16 App_Setup_RSC_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}

/********************************************************
 * BP - Blood Pressure - PROFILE SERVICE DEFINITION
 * Handle 0x130 to 0x13F
 ********************************************************/

u_int16 App_Setup_BLOOD_PRESSURE_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}

/********************************************************
 * BG - Blood GLucose - PROFILE SERVICE DEFINITION
 * Handle 0x140 to 0x14F
 ********************************************************/

u_int16 App_Setup_BLOOD_GLUCOSE_GATT_DB(void)
{
    u_int16 handle = 0xFFFF;

    return handle;
}


#if (APP_EQ_BLE_CONFIG == 1)
void EQ_test_RVCE_Param(u_int8 ch)
{
	//os_printf("ch = %d\r\n",ch);
	if(ch == '2')
		EQ_Data_Print();

}



int atoH(u_int8 *c)
{
	int value;
	if(*c >= '0' && *c <= '9')
	{
		value = *c - 0x30;
		return value;
	}else if((*c >= 'a' && *c <= 'f'))
	{
		value = *c - 87;
		return value;
	}
	else if( (*c >= 'A' && *c <= 'F'))
	{
		value = *c - 55;
		return value;
	}else
		return -1;
}
void EQ_Data_Slave_converter( u_int8 *pdu,ble_aud_equ_t *aud)
{

	aud->a[0] = LMutils_Get_Uint16(pdu);
	aud->a[1] = LMutils_Get_Uint16(pdu+2);
	aud->b[0] = LMutils_Get_Uint16(pdu+4);
	aud->b[1] = LMutils_Get_Uint16(pdu+6);
	aud->b[2] = LMutils_Get_Uint16(pdu+8);
	aud->flag_enable = *(pdu+10);


}
void EQ_Data_Print(void)
{

  int i;
  os_printf("aud_equ_ble Data:\r\n");
  for(i=0;i<5;i++)
    {


      	os_printf("A2:%02X A1:%02X enable:%02X    \r\n",aud_equ_ble[i].a[1],aud_equ_ble[i].a[0],aud_equ_ble[i].flag_enable);
      	os_printf("B1:%02X B0:%02X B2:%02X",aud_equ_ble[i].b[1],aud_equ_ble[i].b[0],aud_equ_ble[i].b[2]);
  		os_printf("\t");

        os_printf("\r\n");
    }

  os_printf("converter aud_equ_ble  Data:\r\n");
  for(i=0;i<5;i++)
     {


       	os_printf("A2:%02X A1:%02X enable:%02X    \r\n",f_inv1(aud_equ_ble[i].a[1]),f_inv1(aud_equ_ble[i].a[0]),aud_equ_ble[i].flag_enable);
       	os_printf("B1:%02X B0:%02X B2:%02X",f_sat1(aud_equ_ble[i].b[1]),f_sat1(aud_equ_ble[i].b[0]),f_sat1(aud_equ_ble[i].b[2]));
   		os_printf("\t");

         os_printf("\r\n");
     }
}


void Write_EQ_parm2_reg()
{
	u_int8 Filter_count = 0;
	u_int8 i;
//	volatile u_int32 reg_value;
	for(i = 0;i < 5;i++)
		{
			if(aud_equ_ble[i].flag_enable == 1) Filter_count++;
		}
	os_printf("Write_EQ_parm2_reg Filter_count = %d\r\n ",Filter_count);
#if 0
		// DAC HPF2 ����λ��1���ƿ�HPF2  д1 �ƿ�HPF2
		reg_value = Beken_Read_Register(BK3000_AUD_BASE_ADDR+0x7*4);
		reg_value &= ~( 1 << 16 );
		reg_value  |= ( ( 1 - 0) << 16 );
		aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+7*4,reg_value);


		// DAC��Ƶ���˲���ʹ��  д0 ���ر� 1��ʹ��
		//aud_filt_enable(4, 0); delay_ms(50);
		reg_value = Beken_Read_Register(BK3000_AUD_BASE_ADDR+0x18*4);
		reg_value &= ~( 1 << 16 );
		reg_value  |= ((0) << 16 );
		//aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x18*4,reg_value);


		if(Filter_count == 5)
		{
			os_printf("Write_EQ_parm2_reg Filter_count = %d\r\n ",Filter_count);

			//DAC HPF2��ϵ�� B2
			reg_value = Beken_Read_Register(BK3000_AUD_BASE_ADDR+0x7*4);
			reg_value &= ~0xFFFF;
			reg_value |= f_sat1(aud_equ[0].b[2]) & 0XFFFF ;
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x7*4,reg_value);

			//DAC HPF2��ϵ�� B1 B0
			//reg_value = Beken_Read_Register(BK3000_AUD_BASE_ADDR+0x8*4);
			reg_value  = ((f_sat1(aud_equ[0].b[1])& 0xFFFF) << 16) | (f_sat1(aud_equ[0].b[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x8*4,reg_value);


			//DAC HPF2��ϵ��A2 A1
			reg_value  = ((f_inv1(aud_equ[0].a[1])& 0xFFFF) << 16) | (f_inv1(aud_equ[0].a[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x9*4,reg_value);



			//flt0 ��ϵ�� A2 A1
			reg_value  = ((f_inv1(aud_equ[1].a[1])& 0xFFFF) << 16) | (f_inv1(aud_equ[1].a[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x20*4,reg_value);

			//flt0 ��ϵ��B1 B0
			reg_value  = ((f_sat1(aud_equ[1].b[1])& 0xFFFF) << 16) | (f_sat1(aud_equ[1].b[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x21*4,reg_value);

			//flt0 ��ϵ��B2
			reg_value  = f_sat1(aud_equ[1].b[2]) & 0xFFFF;
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x22*4,reg_value);

			//flt1 ��ϵ�� A2 A1
			reg_value  = ((f_inv1(aud_equ[2].a[1])& 0xFFFF) << 16) | (f_inv1(aud_equ[2].a[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x23*4,reg_value);

			//flt1 ��ϵ��B1 B0
			reg_value  = ((f_sat1(aud_equ[2].b[1])& 0xFFFF) << 16) | (f_sat1(aud_equ[2].b[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x24*4,reg_value);

			//flt1 ��ϵ��B2
			reg_value  = f_sat1(aud_equ[2].b[2]) & 0xFFFF;
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x25*4,reg_value);

			//flt2 ��ϵ�� A2 A1
			reg_value  = ((f_inv1(aud_equ[3].a[1])& 0xFFFF) << 16) | (f_inv1(aud_equ[3].a[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x26*4,reg_value);

			//flt2 ��ϵ��B1 B0
			reg_value  = ((f_sat1(aud_equ[3].b[1])& 0xFFFF) << 16) | (f_sat1(aud_equ[3].b[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x27*4,reg_value);

			//flt2 ��ϵ��B2
			reg_value  = f_sat1(aud_equ[3].b[2]) & 0xFFFF;
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x28*4,reg_value);

			//flt3 ��ϵ�� A2 A1
			reg_value  = ((f_inv1(aud_equ[4].a[1])& 0xFFFF) << 16) | (f_inv1(aud_equ[4].a[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x29*4,reg_value);

			//flt3 ��ϵ��B1 B0
			reg_value  = ((f_sat1(aud_equ[4].b[1])& 0xFFFF) << 16) | (f_sat1(aud_equ[4].b[0])&0xFFFF);
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x2A*4,reg_value);

			//flt3 ��ϵ��B2
			reg_value  = f_sat1(aud_equ[4].b[2]) & 0xFFFF;
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x2B*4,reg_value);



			// DAC HPF2   ����λ��1���ƿ�HPF2  д1 �ƿ�HPF2
			reg_value = Beken_Read_Register(BK3000_AUD_BASE_ADDR+0x7*4);
			reg_value &= ~( 1 << 16 );
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x7*4,reg_value);


			// DAC��Ƶ���˲���ʹ��  д0 ���ر� 1��ʹ��
			//aud_filt_enable(4, 0); delay_ms(50);
			reg_value = Beken_Read_Register(BK3000_AUD_BASE_ADDR+0x18*4);
			reg_value  |= ((1) << 16 );
			aud_filt_write_use_hci(BK3000_AUD_BASE_ADDR+0x18*4,reg_value);



		}

#endif

#if 1


	aud_filt_enable_use_ble(0, 0);
	aud_filt_enable_use_ble(4, 0);
	if(Filter_count == 5)
	{
		aud_filt_conf_use_ble( 4,  &aud_equ_ble[0]);

		aud_filt_conf_use_ble( 0,  &aud_equ_ble[1]);
		aud_filt_conf_use_ble( 1,  &aud_equ_ble[2]);
		aud_filt_conf_use_ble( 2,  &aud_equ_ble[3]);
		aud_filt_conf_use_ble( 3,  &aud_equ_ble[4]);

		aud_filt_enable_use_ble(0, 1);
		aud_filt_enable_use_ble(4, 1);
	}

	if(Filter_count < 5 && Filter_count > 0)
	{
		aud_filt_conf_use_ble( 0,  &aud_equ_ble[0]);
		aud_filt_conf_use_ble( 1,  &aud_equ_ble[1]);
		aud_filt_conf_use_ble( 2,  &aud_equ_ble[2]);
		aud_filt_conf_use_ble( 3,  &aud_equ_ble[3]);
		aud_filt_conf_use_ble( 4,  &aud_equ_ble[4]);

		aud_filt_enable_use_ble(0, 1);
		 aud_filt_enable_use_ble(4, 0);
	}

	if(Filter_count == 0)
	{
		aud_filt_conf_use_ble( 0,  &aud_equ_ble[0]);
		aud_filt_conf_use_ble( 1,  &aud_equ_ble[1]);
		aud_filt_conf_use_ble( 2,  &aud_equ_ble[2]);
		aud_filt_conf_use_ble( 3,  &aud_equ_ble[3]);
		aud_filt_conf_use_ble( 4,  &aud_equ_ble[4]);

		aud_filt_enable_use_ble(0, 0);
		aud_filt_enable_use_ble(4, 0);

	}
	os_printf("Write_EQ_parm2_reg Filter_count = %d\r\n ",Filter_count);
#endif



}


 int16 f_sat1(int din)
{
    if (din>32767)
        return 32767;
    if (din<-32767)
        return -32767;
    else
        return(din);
}

 int16 f_inv1( int din )
{
    int16 out = f_sat1( din );
    int tempout;

    if( out == 0 )
        tempout = out;
    else
        tempout = 0x10000 - out + 1;

    return (int16)tempout;
}


void aud_filt_enable_use_ble( uint8 index, uint8 enable )
{
	u_int32 tmp = 0;
    if( index < 4 )
    {
    	tmp = BK3000_AUD_AUDIO_CONF2;

    	tmp &= ~( 1 << 2 );
    	tmp |= ( enable << 2 );

    	BK3000_AUD_AUDIO_CONF2 = tmp;

       // BK3000_AUD_AUDIO_CONF2 &= ~( 1 << 2 );
      //  BK3000_AUD_AUDIO_CONF2 |= ( enable << 2 );
    }
    else
    {
    	tmp = BK3000_AUD_DAC_CONF0;
    	tmp &= ~( 1 << sft_AUD_DAC_HPF2_BYPASS );
    	tmp |= ( ( 1 - enable) << sft_AUD_DAC_HPF2_BYPASS );

    	BK3000_AUD_DAC_CONF0 = tmp;

      //  BK3000_AUD_DAC_CONF0 &= ~( 1 << sft_AUD_DAC_HPF2_BYPASS );
      //  BK3000_AUD_DAC_CONF0 |= ( ( 1 - enable) << sft_AUD_DAC_HPF2_BYPASS );
    }
}
int aud_filt_conf_use_ble( uint8 index, ble_aud_equ_t *conf ) //mod sean 12.02
{
    volatile uint32 *a1a2ptr;
    volatile uint32 *b0b1ptr;
    volatile uint32 *b2ptr;

    if( index < 0 || index >= AUDIO_MAX_FILT_NUM )
        return -1;

    if( index < 4 )
    {
        a1a2ptr = (volatile uint32 *)((uint32)&BK3000_AUD_FLT0_A1_A2 + index*12);
        b0b1ptr = (volatile uint32 *)((uint32)&BK3000_AUD_FLT0_B0_B1 + index*12);
        b2ptr = (volatile uint32 *)((uint32)&BK3000_AUD_FLT0_B2 + index*12);

        //os_printf("%x %x %x \r\n", a1a2ptr, b0b1ptr, b2ptr );
        //os_printf("%x %x %x %x %x\r\n", f_inv(conf->a[0]), f_inv(conf->a[1]),
        //f_sat(conf->b[0]), f_sat(conf->b[1]), f_sat(conf->b[2]));

        *a1a2ptr = ((f_inv1(conf->a[1])& 0xFFFF) << 16)|(f_inv1(conf->a[0])& 0xFFFF );
        *b0b1ptr = ((f_sat1(conf->b[1])& 0xFFFF) << 16)|(f_sat1(conf->b[0])& 0xFFFF );
        *b2ptr = f_sat1(conf->b[2]);
    }
    else
    {
        a1a2ptr = (volatile uint32 *)(&BK3000_AUD_DAC_CONF2);
        b0b1ptr = (volatile uint32 *)(&BK3000_AUD_DAC_CONF1);
        b2ptr = (volatile uint32 *)(&BK3000_AUD_DAC_CONF0);

        //os_printf("%x %x %x \r\n", a1a2ptr, b0b1ptr, b2ptr );
        //os_printf("%x %x %x %x %x\r\n", f_inv(conf->a[0]), f_inv(conf->a[1]),
        //f_sat(conf->b[0]), f_sat(conf->b[1]), f_sat(conf->b[2]));

        *a1a2ptr = ((f_inv1(conf->a[1])& 0xFFFF) << 16)|(f_inv1(conf->a[0])& 0xFFFF );
        *b0b1ptr = ((f_sat1(conf->b[1])& 0xFFFF) << 16)|(f_sat1(conf->b[0])& 0xFFFF );
        *b2ptr &= ~0xFFFF;
        *b2ptr |= f_sat1(conf->b[2]) & 0xFFFF;
    }

    return 0;
}
#endif //(APP_EQ_BLE_CONFIG == 1)

#endif

#endif
