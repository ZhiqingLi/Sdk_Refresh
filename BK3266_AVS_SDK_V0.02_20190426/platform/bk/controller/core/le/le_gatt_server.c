/***********************************************************************
 *
 * MODULE NAME:    LE_gatt_server.c
 * PROJECT CODE:   CEVA Low Energy Single Mode
 * DESCRIPTION:    Implementation of the GATT Database Server.
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:  Janurary 2012
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2011-2012 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/
#include "config.h"
#ifdef BT_DUALMODE
#include "sys_config.h"
#include "sys_types.h"
#include "sys_features.h"
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)

#include "le_connection.h"
#include "le_att.h"
#include "tra_queue.h"
#include "le_const.h"
#include "le_config.h"
#include "le_gatt_server.h"
#include "lmp_types.h"
#include "hc_const.h"
#include "le_smp.h"
#include "lmp_utils.h"
#include "le_att.h"
#include "sys_mmi.h"
#include "le_cts_app.h"
#include "sys_features.h"
#include "hw_le_lc_impl.h"
#include "hw_radio_defs.h"
#include "hw_radio.h"

#ifdef BEKEN_UP_OTA_3266
#include "app_Oad_target.h"
#endif

#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)

typedef struct t_att_val_service_decl {
    u_int8 service_UUID_len;
    t_UUID service_UUID;
} t_att_val_service_decl;

typedef struct t_att_val_include_decl {
    u_int16 start_handle;
    u_int16 end_handle;
    u_int8 service_UUID_len;
    t_UUID service_UUID;
} t_att_val_include_decl;

typedef struct t_att_val_char_decl {
    u_int8 properties;
    u_int16 char_value_handle;
    u_int8 UUID_len;
    t_UUID char_UUID;
} t_att_val_char_decl;


typedef struct t_att_val_char_value_decl {
    u_int8 val_len;
    u_int8* p_value;
    u_int8 max_len;
    u_int8 local_notification;
} t_att_val_char_or_descr_value_decl;

typedef struct t_att_val_char_extended_properties {
    u_int16 extented_properties_bit_field;
} t_att_val_char_extended_properties;

typedef struct t_att_val_char_user_desc_decl {
    u_int8 string_len;
    u_int8* p_utf_string;
} t_att_val_char_user_desc_decl ;

typedef struct t_att_val_char_format_decl {
    u_int8 format;
    u_int8 exponent;
    u_int16 unit;
    u_int8 name_space;
    u_int16 description;
} t_att_val_char_format_decl;


typedef union t_attrib_value {
    t_att_val_service_decl service;
    t_att_val_include_decl include_service;
    t_att_val_char_decl    char_declaration;
    t_att_val_char_or_descr_value_decl value;
}t_attrib_value;


typedef struct t_attrib {
    u_int16 attrib_handle;
    u_int8 attrib_len;
    t_UUID attrib_UUID;
    t_attrib_value attrib_value;
    u_int8 permissions;
} t_attrib ;

u_int16 GATT_reconnection_address_handle = 0x00;
u_int8 _GATTserver_Check_Space_Available(u_int16 GATTserver_DB_next_index);
u_int16 _GATTserver_Find_End_Group_Handle(u_int16 current_index);
#if 0 /* no used */
u_int8* _GATTserver_add_value_to_val_array(u_int8 val_len,u_int8* p_value);
#endif

u_int8 GATTserver_Check_Is_Service_Authorised(void);
t_GATT_error GATTserver_Check_Encryption_Key_Size(u_int8 permissions);
t_GATT_error GATTserver_Check_Permissions(t_attrib* p_current_attrib,u_int8 action);
#if 0 /* no used */
t_error _GATTserver_Find_Characteristic_Value_Properties(u_int16 value_handle, u_int8 index, u_int8* p_properties);
#endif
u_int8 GATTserver_Find_DB_Index(u_int16 start_handle,u_int16 end_handle,u_int16* p_index);

static u_int16 GATTserver_DB_last_index;

static u_int16 GATTserver_DB_next_index = 0;
static u_int16 GATTserver_DB_last_handle =0;
#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)
static u_int16 GATTserver_DB_Reconnection_Address_Index = 0;
#endif
#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)
u_int16 GATTserver_DB_Peripheral_Privacy_Flag_Index = 0;
#endif

#define MAX_GATT_SERVER_NUM_ENTRIES         100//400

#define GATT_SERVER_MAX_PENDING_INDICATIONS 10

u_int16 GATTserver_Pending_Indications[GATT_SERVER_MAX_PENDING_INDICATIONS];
u_int8  GATTserver_pending_Indications_Index=0;


// GATT Server Global Paramers..
// These are global Variables which are stored in the GATT DataBase
t_attrib GATTserver_DB[MAX_GATT_SERVER_NUM_ENTRIES];

u_int16 GATTserver_Handle_in_Error=0;

#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)
u_int8 GATTprivacy_Flags[2] = {0x00,0x00};
#endif

#if (PRH_BS_CFG_SYS_LE_SERVER_PTS_LARGE_DATABASE_1==1)
static u_int8 string_26_byte[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static u_int8 Value_V1[1] = {0x01};
static u_int8 Value_V2[65] = "11111222223333344444555556666677777888889999900000111112222233333";
static u_int8 Value_V3[1] = {0x03};
static u_int8 Value_V4[1] = {0x04};
static u_int8 Value_V5[1] = {0x05};
static u_int8 Value_V6[1] = {0x06};
static u_int8 Value_V7[1] = {0x07};
static u_int8 Value_V8[1] = {0x08};
static u_int8 Value_V9[1] = {0x09};
static u_int8 Value_V11[1] = {0x0B};
static u_int8 Value_V12[1] = {0x0C};

static u_int8 Service_C_1[16] = { 0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0x0C,0xA0,0x00,0x00};
static u_int8 Service_C_2[16] = { 0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0x0C,0xA0,0x00,0x00};
static u_int8 UUID_Val_11[16] = { 0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0x0B,0xB0,0x00,0x00};
static u_int8 UUID_Val_9[16] =  { 0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0x09,0xB0,0x00,0x00};

u_int8 V5D4[16] =  { 0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0xD4,0xD5,0x00,0x00};
u_int8 V5D4_Descriptor[2] = {0x44,0x00};
u_int8 V9D2[16] =  { 0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0xD2,0xD9,0x00,0x00};
u_int8 V9D2_Descriptor[2] = {0x22,0x00};
u_int8 V9D3[16] =  { 0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01,0x00,0x00,0x00,0x00,0xD3,0xD9,0x00,0x00};
u_int8 V9D3_Descriptor[2] = {0x33,0x00};


u_int8 A00B_format[7];

#endif
// Currently not used
// static u_int8 Service_Change_Range[4] = { 0x01,0x00,0xFF,0xFF };

#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)

u_int8 reconnection_addr_valid = 0x00;
#endif

typedef enum {
    GAP_GATT_DB = 0x00,
    GATT_GATT_DB,
    DEVICE_INFO_GATT_DB,
    IAS_GATT_DB, //IMMEDIATE ALERT PROFILE
    TX_POWER_GATT_DB,
    LINK_LOSS_GATT_DB,
    BATTERY_SERVICE_GATT_DB,
    SCAN_PARAMETERS_GATT_DB,
    CURRENT_TIME_GATT_DB,
    REFERENCE_TIME_GATT_DB,
    NEXT_DST_GATT_DB,
    HID_GATT_DB,
    HRS_GATT_DB,
    HT_GATT_DB,
    CSC_GATT_DB,
    RSC_GATT_DB,
    BLOOD_PRESSURE_GATT_DB,
    USER_DEF_FFE0_GATT_DB,
    USER_DEF_FFA0_GATT_DB,
#ifdef BEKEN_BLE_OTA_3266
    USER_DEF_OAD_GATT_DB,
#endif
    GATT_DB_NUM,
}GATT_DB_t;
//#define CEVA_DEFAULT
//#define _FIND_ME_TI_SIM_
#ifdef CEVA_DEFAULT
#define CEVA_DEFAULT_DB_LEN   17
#define DB_LEN CEVA_DEFAULT_DB_LEN
#elif defined(_FIND_ME_TI_SIM_)
//#define _FIND_ME_TI_SIM_DB_LEN 6
#define _FIND_ME_TI_SIM_DB_LEN 9 //NEW
#define DB_LEN _FIND_ME_TI_SIM_DB_LEN
#else 
//add begin by  ps for OTA
#ifdef BEKEN_BLE_OTA_3266
#define DB_LEN 2/* 4 */
#else
#define DB_LEN 1/* 4 */
#endif
//add end by  ps for OTA
#endif

#ifdef BEKEN_BLE_OTA_3266
#define GATT_NO_OPERATION  20
#else
#define GATT_NO_OPERATION  19
#endif

typedef u_int16 (*GATT_DB_setup_function)(void);//merge CEVA 0517
GATT_DB_setup_function GATT_DB_setup_handlers[GATT_DB_NUM] = {
    App_Setup_GAP_GATT_DB,
    App_Setup_GATT_GATT_DB,
    App_Setup_DEV_INFO_GATT_DB,
    App_Setup_IAS_GATT_DB,
    App_Setup_TX_POWER_GATT_DB,
    App_Setup_LINK_LOSS_GATT_DB,
    App_Setup_BATTERY_SERVICE_GATT_DB,
    App_Setup_SCAN_PARAMETERS_GATT_DB,
    App_Setup_CURRENT_TIME_GATT_DB,
    App_Setup_REFERENCE_TIME_GATT_DB,
    App_Setup_NEXT_DST_GATT_DB,
    App_Setup_HID_GATT_DB,
    App_Setup_HRS_GATT_DB,
    App_Setup_HT_GATT_DB,
    App_Setup_CSC_GATT_DB,
    App_Setup_RSC_GATT_DB,
    App_Setup_BLOOD_PRESSURE_GATT_DB,
    App_Setup_FFE0_GATT_DB, //User define notify Phone alert
    App_Setup_FFA0_GATT_DB,
#ifdef BEKEN_BLE_OTA_3266
    App_Setup_OAD_GATT_DB,
#endif

};
extern t_App_Control LEapp_Config;

t_error GATTserver_Init(void)
{
    // Set up the Data Base

    // First Add the GAP as a primary service !
    //     GAP Characteristics :
    //          Device Name
    //


#ifdef _FIND_ME_TI_SIM_	
    //const GATT_DB_t GATT_DB_SELECT[DB_LEN]={GAP_GATT_DB,GATT_GATT_DB,LINK_LOSS_GATT_DB,
    //                                      IAS_GATT_DB,TX_POWER_GATT_DB,BATTERY_SERVICE_GATT_DB};
    const GATT_DB_t GATT_DB_SELECT[DB_LEN]={GAP_GATT_DB,GATT_GATT_DB,DEVICE_INFO_GATT_DB,IAS_GATT_DB,
                                            TX_POWER_GATT_DB,LINK_LOSS_GATT_DB,BATTERY_SERVICE_GATT_DB,USER_DEF_FFE0_GATT_DB,USER_DEF_FFA0_GATT_DB};//NEW
#else
//const GATT_DB_t GATT_DB_SELECT[DB_LEN]={GAP_GATT_DB,GATT_GATT_DB,DEVICE_INFO_GATT_DB,USER_DEF_FFE0_GATT_DB};
#ifdef BEKEN_BLE_OTA_3266
//add begin by  ps for OTA
const GATT_DB_t GATT_DB_SELECT[DB_LEN]={USER_DEF_FFE0_GATT_DB,USER_DEF_OAD_GATT_DB};
//add end by  ps for OTA
#else
const GATT_DB_t GATT_DB_SELECT[DB_LEN]={USER_DEF_FFE0_GATT_DB};
#endif //BEKEN_UP_OTA_3266

#endif
    u_int8 GATT_DB_SELECT_index = 0;
    Gatt_Set_MTU_Size(255);
    GATTserver_DB_next_index = 0;
    GATTserver_DB_last_handle =0;

#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)
    reconnection_addr_valid = 0x00;
#endif

#if (PRH_BS_CFG_SYS_LE_SERVER_PTS_LARGE_DATABASE_1==1)


    handle = 1;

    _GATTserver_Add_Service(SECONDARY_SERVICE_16BIT,handle,0x00,0xA00D);

    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB00C,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB00C,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,1,Value_V12);
    handle++;

    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle,UUID_Val_11,0x02);
    handle++;
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle,UUID_Val_11,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,1,Value_V11);
    handle++;


    /************************************************************************************************************/
    handle=16;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,0xA00B);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB006,0x3E);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB006,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE|GATT_PERMISSION_AUTHENTICATED | 0xF0,1,Value_V6);
    handle++;

    /************************************************************************************************************/
    handle = 32;

    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,0xA00A);
    handle++;

    _GATTserver_Add_Included_Service(handle,0x0001,0x0005,0xA00D); // All included ATTRIBs are read only GATT_PERMISSION_READ);
    handle++;

    _GATTserver_Add_Included_Service(handle,0x0090,0x0096,0x0000); // No UUID given for included service
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB001,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB001,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ /*| GATT_PERMISSION_READ_AUTHORISED*/,1,Value_V1);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB002,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,50,&Value_V2[0]);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB003,0x08);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB003,NO_LOCAL_NOTIFY,GATT_PERMISSION_WRITE,1,Value_V3);
    handle++;

    /************************************************************************************************************/
    handle = 48;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,0xA00B);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB007,0x08);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB007,NO_LOCAL_NOTIFY,GATT_PERMISSION_WRITE,1,Value_V7);
    handle++;

    /************************************************************************************************************/
    handle = 64;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,GENERIC_ACCESS_PROFILE);
    handle++;

    // GF 04 Nov 2012
    _GATTserver_Add_Characteristic_Declaration(handle,DEVICE_NAME,0x02);
    //_GATTserver_Add_Characteristic_Declaration(handle,DEVICE_NAME,0x42);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,DEVICE_NAME,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ /*|GATT_PERMISSION_WRITE*/,13,name_array);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,APPERANCE,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,APPERANCE,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ/*|GATT_PERMISSION_WRITE*/,2,appearance_array);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,7,connection_params);
    handle++;

#if 0 // Not included for GATT Testing
#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)

    _GATTserver_Add_Characteristic_Declaration(handle,RECONNECTION_ADDRESS,0x02);
    handle++;
    GATT_reconnection_address_handle = handle;
    _GATTserver_Add_Characteristic_Value(handle,RECONNECTION_ADDRESS,USE_LOCAL_NOTIFY,GATT_PERMISSION_READ |GATT_PERMISSION_WRITE ,6,reconnection_addr);
    handle++;
#endif

#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)
    _GATTserver_Add_Characteristic_Declaration(handle,PERIPHERAL_PRIVACY_FLAGS,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,PERIPHERAL_PRIVACY_FLAGS,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ |GATT_PERMISSION_WRITE ,1,GATTprivacy_Flags);
    handle++;
#endif
#endif


    /**************************************************************************************************/
    handle = 80;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,GENERIC_ATTRIBUTE_PROFILE);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,SERVICE_CHANGED,0x20);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,SERVICE_CHANGED,NO_LOCAL_NOTIFY,0x00,4,Service_Change_Range);
    handle++;

    /**************************************************************************************************/
    handle = 96;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,0xA00B);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB004,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB004,NO_LOCAL_NOTIFY,0x00,GATT_PERMISSION_READ |GATT_PERMISSION_WRITE |GATT_PERMISSION_AUTHENTICATED ,Value_V4);
    handle++;

    //_GATTserver_Add_Characteristic_Configuration(handle);
    u_int8 Value_V4_CCC[2] = {0x00,0x00};
    _GATTserver_Add_Char_Descriptor(handle,CLIENT_CHARACTERISTIC_CONFIGURATION,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,0x02,Value_V4_CCC);

    handle++;

    /***PRIMARY SERVICE 0xA00B ******************************************************************************/
    handle = 112;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,0xA00B);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB005,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB005,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,1,Value_V5);
    handle++;

    A00B_ext_properties[0] = 0x03;
    //_GATTserver_Add_Characteristic_Extended_Properties(handle,0x03);
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_EXTENDED_PROPERTIES,GATT_PERMISSION_READ,0x01,A00B_ext_properties);

    handle++;

    //    _GATTserver_Add_Characteristic_User_Description(handle,26,string_26_byte,GATT_PERMISSION_READ /* Permissions */);
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_USER_DESCRIPTION,GATT_PERMISSION_READ,26,string_26_byte);
    handle++;

    //_GATTserver_Add_Characteristic_Configuration(handle);
    u_int8 user_description_CCC[2] = {0x00,0x00};
    _GATTserver_Add_Char_Descriptor(handle,CLIENT_CHARACTERISTIC_CONFIGURATION,GATT_PERMISSION_READ|GATT_PERMISSION_WRITE,0x02,user_description_CCC);

    handle++;

    //_GATTserver_Add_Characteristic_Format(handle , 0x3001 /* Length */,0x04/*FORMAT*/, 0x00 /* Exponent */, 0x00 /* n_sp */, 0x3111 /* Description */);

    A00B_format[0] = 4; // Format
    A00B_format[1] = 0; // Exponent
    LMutils_Set_Uint16(A00B_format+2, 0x3001); /* UUID for Watts */
    A00B_format[4] = 0; // Name Space - Sig Specified
    LMutils_Set_Uint16(A00B_format+5, 0x3111);

    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_PRESENTATION_FORMAT,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,0x07,A00B_format);


    handle++;

    _GATTserver_Add_Characteristic_Propietrary_Descriptor(handle,16,V5D4,GATT_PERMISSION_READ,V5D4_Descriptor);
    handle++;

    /***PRIMARY SERVICE 0xA00B ******************************************************************************/
    handle = 128;
    _GATTserver_Add_Service(PRIMARY_SERVICE_16BIT,handle,0x00,0xA00B);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB008,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB008,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,1,Value_V8);
    handle++;

    /***PRIMARY SERVICE UUID = 16 Bytes **********************************************************************/
    handle = 144;

    _GATTserver_Add_Service(PRIMARY_SERVICE_128BIT,handle,Service_C_1,0x0000);
    handle++;

    _GATTserver_Add_Included_Service(handle,0x0001,0x0005,0xA00D); // All included ATTRIBs are read only GATT_PERMISSION_READ);
    handle++;

    _GATTserver_Add_16ByteUUID_Characteristic_Declaration(handle,UUID_Val_9,0x8A);
    handle++;
    _GATTserver_Add_16ByteUUID_Characteristic_Value(handle,UUID_Val_9,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,1,Value_V9);
    handle++;;

    //_GATTserver_Add_Characteristic_Extended_Properties(handle,0x01);
    UUID_Val_9_ext_properties[0] = 0x01;
    _GATTserver_Add_Char_Descriptor(handle,CHARACTERISTIC_EXTENDED_PROPERTIES,GATT_PERMISSION_READ,0x01,UUID_Val_9_ext_properties);

    handle++;

    _GATTserver_Add_Characteristic_Propietrary_Descriptor(handle,16,V9D2,GATT_PERMISSION_READ,V9D2_Descriptor);
    handle++;

    _GATTserver_Add_Characteristic_Propietrary_Descriptor(handle,16,V9D3,GATT_PERMISSION_WRITE,V9D3_Descriptor);
    handle++;

    /***PRIMARY SERVICE UUID = 16 Bytes **********************************************************************/

    handle = 160;
    _GATTserver_Add_Service(PRIMARY_SERVICE_128BIT,handle,Service_C_2,0x0000);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB00A,0x02);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB00A,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,1,Value_V2);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB002,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,21,&Value_V2[0]);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB002,0x0A);
    handle++;
    //    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ| GATT_PERMISSION_WRITE,22,&Value_V2[5]);
    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ| GATT_PERMISSION_WRITE,28,&Value_V2[5]);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB002,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ | GATT_PERMISSION_WRITE,23,&Value_V2[10]);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB002,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ| GATT_PERMISSION_WRITE,43,&Value_V2[0]);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB002,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ| GATT_PERMISSION_WRITE,44,&Value_V2[5]);
    handle++;

    _GATTserver_Add_Characteristic_Declaration(handle,0xB002,0x0A);
    handle++;
    _GATTserver_Add_Characteristic_Value(handle,0xB002,NO_LOCAL_NOTIFY,GATT_PERMISSION_READ,45,&Value_V2[10]);
    handle++;

    GATTserver_DB_last_index = GATTserver_DB_next_index-1;
    GATTserver_DB_last_handle = handle-1;

#elif (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1)// Profile Databases --

    /*********************************************************************
     * Start of Profile Test Databases - Profile GATT DB definitions are 2k of code.
     *********************************************************************/
    for(GATT_DB_SELECT_index = 0;GATT_DB_SELECT_index< DB_LEN;GATT_DB_SELECT_index++)
    {
        GATTserver_DB_last_handle = GATT_DB_setup_handlers[GATT_DB_SELECT[GATT_DB_SELECT_index]]();
    }
    GATTserver_DB_last_handle = GATTserver_DB_last_handle-1;
    GATTserver_DB_last_index = GATTserver_DB_next_index-1;

    //      App_Setup_GAP_GATT_DB();
    //      App_Setup_GATT_GATT_DB();
    //      App_Setup_DEV_INFO_GATT_DB();
    //      App_Setup_IAS_GATT_DB();
    //      App_Setup_TX_POWER_GATT_DB();
    //      App_Setup_LINK_LOSS_GATT_DB();
    //      App_Setup_BATTERY_SERVICE_GATT_DB();
    //      App_Setup_SCAN_PARAMETERS_GATT_DB();

    //      App_Setup_CURRENT_TIME_GATT_DB();
    //      App_Setup_REFERENCE_TIME_GATT_DB();
    //      App_Setup_NEXT_DST_GATT_DB();
    //      App_Setup_HID_GATT_DB();

    //      App_Setup_HRS_GATT_DB();

    //      App_Setup_HT_GATT_DB();
    //      App_Setup_CSC_GATT_DB();
    //      App_Setup_RSC_GATT_DB();
    //      App_Setup_BLOOD_PRESSURE_GATT_DB();
    //      GATTserver_DB_last_handle = App_Setup_BLOOD_GLUCOSE_GATT_DB();
    //        GATTserver_DB_last_index = GATTserver_DB_next_index-1;
    //          #endif //_DEFAULT_
    //          #ifdef _FIND_ME_TI_SIM_
    //          App_Setup_GAP_GATT_DB();//GERERIC ACCESS PROFILE DEFINITION
    //      App_Setup_GATT_GATT_DB();//GATT PROFILE DEFINITION Generic Attribute Profile
    //      App_Setup_DEV_INFO_GATT_DB();//DERVICE INFORMATION PROFILE DEFINITION
    //      App_Setup_IAS_GATT_DB();//IMMEDIATE ALERT PROFILE
    //      App_Setup_TX_POWER_GATT_DB();//TX POWER PROFILE DEFINITION
    //      App_Setup_LINK_LOSS_GATT_DB();//LINK LOSS PROFILE DEFINITION
    //      GATTserver_DB_last_handle = App_Setup_BATTERY_SERVICE_GATT_DB();//BATTERY SERVICE PROFILE DEFINITION
    //          GATTserver_DB_last_index = GATTserver_DB_next_index-1;
    //
    //          #endif

#endif
    return NO_ERROR;
}
/*****************************************************************************************************
 * _GATTserver_Add_Characteristic_Declaration
 *
 * Adds a 16 bit characteristic declaration to the Server DB
 *****************************************************************************************************/

u_int8 _GATTserver_Add_Characteristic_Declaration(u_int16 handle,u_int16 char_UUID,u_int8 properties)
{
    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];

    if (_GATTserver_Check_Space_Available(GATTserver_DB_next_index))
    {
        p_current_attrib->attrib_handle = handle;

        p_current_attrib->attrib_len = 0x02;
        p_current_attrib->attrib_UUID.UUID_val = GATT_ATT_TYPE_CHARACTERISTIC;

        p_current_attrib->permissions = GATT_PERMISSION_READ; // Read only ?
        p_current_attrib->attrib_value.char_declaration.UUID_len = 0x02;
        p_current_attrib->attrib_value.char_declaration.char_UUID.UUID_val = char_UUID;
        p_current_attrib->attrib_value.char_declaration.properties = properties;
        p_current_attrib->attrib_value.char_declaration.char_value_handle = handle+1;
        GATTserver_DB_next_index++;
        return NO_ERROR;
    }
    else
    {
        return UNSPECIFIED_ERROR;
    }
}


/*****************************************************************************************************
 * _GATTserver_Add_16ByteUUID_Characteristic_Declaration
 *
 * Adds a 128 bit characteristic declaration to the Server DB
 *****************************************************************************************************/

u_int8 _GATTserver_Add_16ByteUUID_Characteristic_Declaration(u_int16 handle, u_int8* p_UUID,u_int8 properties)
{
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_128_BIT_UUIDS==1)
    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];

    if (_GATTserver_Check_Space_Available(GATTserver_DB_next_index))
    {
        p_current_attrib->attrib_handle = handle;
        p_current_attrib->attrib_len = 0x02;
        p_current_attrib->attrib_UUID.UUID_val = GATT_ATT_TYPE_CHARACTERISTIC;

        p_current_attrib->permissions = GATT_PERMISSION_READ;


        p_current_attrib->attrib_value.char_declaration.UUID_len = 0x10;
        p_current_attrib->attrib_value.char_declaration.char_UUID.p_UUID = p_UUID;
        p_current_attrib->attrib_value.char_declaration.properties = properties;
        p_current_attrib->attrib_value.char_declaration.char_value_handle = handle + 1;
        GATTserver_DB_next_index++;
        return NO_ERROR;
    }
    else
#endif
        return UNSPECIFIED_ERROR;
}



/*****************************************************************************************************
 * _GATTserver_Add_Service
 *
 * Adds a Service Definition to the Service DB
 *****************************************************************************************************/
u_int8 _GATTserver_Add_Service(u_int8 service_type,u_int16 handle,u_int8* p_service_UUID,u_int16 service_UUID)
{
    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];

    if(_GATTserver_Check_Space_Available(GATTserver_DB_next_index))
    {
        p_current_attrib->attrib_handle = handle;
        p_current_attrib->permissions = GATT_PERMISSION_READ;
        if (service_type == PRIMARY_SERVICE_16BIT)
        {
            p_current_attrib->attrib_UUID.UUID_val = GATT_ATT_TYPE_PRIMARY_SERVICE;
        }
        else if (service_type == SECONDARY_SERVICE_16BIT)
        {
            p_current_attrib->attrib_UUID.UUID_val = GATT_ATT_TYPE_SECONDARY_SERVICE;
        }
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_128_BIT_UUIDS==1)
        else if (service_type == PRIMARY_SERVICE_128BIT)
        {
            p_current_attrib->attrib_UUID.UUID_val = GATT_ATT_TYPE_PRIMARY_SERVICE;
        }
        else if (service_type == SECONDARY_SERVICE_128BIT)
        {
            p_current_attrib->attrib_UUID.UUID_val = GATT_ATT_TYPE_SECONDARY_SERVICE;
        }

        if ((service_type == PRIMARY_SERVICE_16BIT) || (service_type == SECONDARY_SERVICE_16BIT))
        {
            p_current_attrib->attrib_value.service.service_UUID_len = 0x02;
            p_current_attrib->attrib_value.service.service_UUID.UUID_val = service_UUID;
        }
        else
        {
            p_current_attrib->attrib_value.service.service_UUID_len = 0x10;
            p_current_attrib->attrib_value.service.service_UUID.p_UUID = p_service_UUID;
        }

#else
        p_current_attrib->attrib_value.service.service_UUID_len = 0x02;
        p_current_attrib->attrib_value.service.service_UUID.UUID_val = service_UUID;
#endif

#if 1 // UPF 44 Added to overcome problems with Find Information Request - TI and CSR
        p_current_attrib->attrib_len = 0x02;
#endif
        GATTserver_DB_next_index++;
        return NO_ERROR;
    }
    else
    {
        return UNSPECIFIED_ERROR;
    }
}


/*****************************************************************************************************
 * _GATTserver_Add_Included_Service
 *
 * Adds an included service definition to the Service DB
 *****************************************************************************************************/
u_int8 _GATTserver_Add_Included_Service(u_int16 handle, u_int16 start_handle,u_int16 end_handle,u_int16 service_UUID)
{
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_INCLUDED_SERVICES==1)

    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];

    _GATTserver_Check_Space_Available(GATTserver_DB_next_index);
    p_current_attrib->attrib_handle = handle;
    p_current_attrib->attrib_len = 0x02;
    p_current_attrib->attrib_UUID.UUID_val = GATT_ATT_TYPE_INCLUDE;

    p_current_attrib->permissions = GATT_PERMISSION_READ; // All included services are Read Only with No Authentication/Authorisation required
    p_current_attrib->attrib_value.include_service.start_handle = start_handle;
    p_current_attrib->attrib_value.include_service.end_handle = end_handle;
    p_current_attrib->attrib_value.include_service.service_UUID_len = 0x02;
    p_current_attrib->attrib_value.include_service.service_UUID.UUID_val = service_UUID;

    GATTserver_DB_next_index++;
    return 0;
#else
    return UNSPECIFIED_ERROR;
#endif
}


/*****************************************************************************************************
 * _GATTserver_Add_Characteristic_Value
 *
 * Adds a characteristic value to the Service DB for a 16 Bit Chararacteristic
 *****************************************************************************************************/

u_int8 _GATTserver_Add_Characteristic_Value(u_int16 handle,u_int16 char_UUID,u_int8 local_notify,u_int8 permissions,u_int8 val_len,u_int8* p_value)
{
    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];

#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) // ????
    if (char_UUID == PERIPHERAL_PRIVACY_FLAG)
        GATTserver_DB_Peripheral_Privacy_Flag_Index = GATTserver_DB_next_index;
#endif
    p_current_attrib->attrib_value.value.p_value = p_value;

#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)
    if (char_UUID == RECONNECTION_ADDRESS)
        GATTserver_DB_Reconnection_Address_Index = GATTserver_DB_next_index;
#endif
    p_current_attrib->attrib_handle = handle;
    p_current_attrib->attrib_len = 0x02;
    p_current_attrib->attrib_UUID.UUID_val = char_UUID;
    p_current_attrib->permissions = permissions;
    p_current_attrib->attrib_value.value.local_notification = local_notify;
    p_current_attrib->attrib_value.value.val_len = val_len;

    p_current_attrib->attrib_value.value.max_len = val_len;

    GATTserver_DB_next_index++;

    return 0;
}

/*****************************************************************************************************
 * _GATTserver_Add_16ByteUUID_Characteristic_Value
 *
 * Adds a characteristic value to the Service DB for a 128 Bit Chararacteristic
 *****************************************************************************************************/
u_int8 _GATTserver_Add_16ByteUUID_Characteristic_Value(u_int16 handle,u_int8* char_UUID,u_int8 local_notify,u_int8 permissions,u_int8 val_len,u_int8* p_value)
{
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_128_BIT_UUIDS==1)
    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];
    // Device Name Characteristic Value

    _GATTserver_Check_Space_Available(GATTserver_DB_next_index);

    p_current_attrib->attrib_handle = handle;
    p_current_attrib->attrib_len = 0x10;
    p_current_attrib->attrib_UUID.p_UUID = char_UUID;

    p_current_attrib->permissions = permissions;
    p_current_attrib->attrib_value.value.local_notification = local_notify;
    p_current_attrib->attrib_value.value.val_len = val_len;
    p_current_attrib->attrib_value.value.max_len = val_len;
#if 0
    p_current_attrib->attrib_value.value.p_value = _GATTserver_add_value_to_val_array(val_len,p_value);
#else
    p_current_attrib->attrib_value.value.p_value = p_value;
#endif
    GATTserver_DB_next_index++;
#endif
    return 0;
}



/*****************************************************************************************************
 * _GATTserver_Add_Characteristic_Propietrary_Descriptor
 *
 * Adds a propietrary characteristic descriptor to the Service DB.
 *****************************************************************************************************/

u_int8 _GATTserver_Add_Characteristic_Propietrary_Descriptor(u_int16 handle,u_int8 UUID_len, u_int8* p_UUID, u_int8 permissions,u_int8* p_value)
{
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_128_BIT_UUIDS==1)
    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];

    _GATTserver_Check_Space_Available(GATTserver_DB_next_index);
    p_current_attrib->attrib_handle = handle;

    p_current_attrib->attrib_len = 0x10;
    p_current_attrib->attrib_UUID.p_UUID = p_UUID ;

    p_current_attrib->permissions = permissions;

    p_current_attrib->attrib_value.value.val_len = 1;
    p_current_attrib->attrib_value.value.max_len = 1;

    p_current_attrib->attrib_value.value.p_value = p_value;

    GATTserver_DB_next_index++;
#endif
    return 0;
}
/*
 * The Client Characteristic Configuration descriptor value shall be persistent
 * across connections for bonded devices. The Client Characteristic Configuration
 * descriptor value shall be set to the default value at each connection with
 * non-bonded devices.The characteristic descriptor value is a bit field. When a
 * bit is set, that action shall be enabled, otherwise it will not be used. The Client
 * Characteristic Configuration descriptor may occur in any position within the
 * characteristic definition after the Characteristic Value. Only one Client Characteristic
 * Configuration declaration shall exist in a characteristic definition.
 */

/*****************************************************************************************************
 * _GATTserver_Add_Char_Descriptor
 *
 * Adds a characteristic descriptor to the Service DB.
 *****************************************************************************************************/

u_int8 _GATTserver_Add_Char_Descriptor(u_int16 handle,u_int16 uuid,u_int8 permission,u_int8 val_len,u_int8* p_value)
{
    t_attrib* p_current_attrib = &GATTserver_DB[GATTserver_DB_next_index];

    if (_GATTserver_Check_Space_Available(GATTserver_DB_next_index))
    {
        p_current_attrib->attrib_handle = handle;
        p_current_attrib->attrib_len = 0x02;
        p_current_attrib->attrib_UUID.UUID_val = uuid;
        p_current_attrib->permissions = permission;
        p_current_attrib->attrib_value.value.val_len = val_len;
        p_current_attrib->attrib_value.value.max_len = val_len;

        p_current_attrib->attrib_value.value.p_value = p_value;

        GATTserver_DB_next_index++;
        return NO_ERROR;
    }
    else
    {
        return UNSPECIFIED_ERROR;
    }
}
/*
 * GATT Functions to search the Server DB.
 */


/*****************************************************************************************************
 * GATTserver_Read_By_Group_Type
 * This function is used to discover all the primary services on a server, within a given handle range.
 *
 * This function is only used for primary service discovery. It is only ever dealing with UUIDs for primary services.
 *
 * INPUTS
 * u_int16  max_result_len : This is the maximum length of data which can be returned.
 * u_int16  start_handle   : The start handle  for the range to be searched
 * u_int16  end_handle     : The end handle  for the range to be searched
 *
 * u_int16  attrib_UUID   : PRIMARY_SERVICE_TYPE | SECONDARY_SERVICE_TYPE
 *
 * OUTPUTS
 * u_int8* p_result       :- A pointer to a list of start,end handle and services UUIDs corresponding to each service matching
 *                              attrib_UUID.
 * u_int8* p_result_len   :- A pointer to the total length of valid data in the result field ( pointed to by p_result ).
 *
 *****************************************************************************************************/

u_int8 GATTserver_Read_By_Group_Type(u_int8 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int16 attrib_UUID,
                                     u_int8* p_result, u_int8* p_result_len )
{
    u_int16 i=0;
    u_int8* p_payload = p_result;
    u_int8 status;
    extern t_ATT_config ATT_config;

    *p_result_len = 0x00;

    status = GATTserver_Find_DB_Index(start_handle,end_handle,&i);
    if (status != NO_ERROR)
    {
       
        if (status == GATT_ERROR_INVALID_HANDLE )
            status = GATT_ERROR_ATTRIBUTE_NOT_FOUND;
        return status;
    }

    // i now is the index to the start handle in the Server DB

    while ((i<=GATTserver_DB_last_index) && (GATTserver_DB[i].attrib_handle <= end_handle) && (((*p_result_len)+6) < max_result_len) )
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];

        // if the attrib_UUID match

        if ((p_current_attrib->attrib_UUID.UUID_val == attrib_UUID) && (p_current_attrib->permissions & GATT_PERMISSION_READ))
        {
            //
            if(((*p_result_len)+4+p_current_attrib->attrib_value.service.service_UUID_len) > ATT_config.MTU_LEN)
            {
                break;
            }

            LMutils_Set_Uint16(p_payload, p_current_attrib->attrib_handle );
            LMutils_Set_Uint16(p_payload+2, _GATTserver_Find_End_Group_Handle(i) );
            if (p_current_attrib->attrib_value.service.service_UUID_len == 0x02)
            {
                LMutils_Set_Uint16(p_payload+4, p_current_attrib->attrib_value.service.service_UUID.UUID_val);
                p_payload+=6;
                *p_result_len+=6;
            }

            if(p_current_attrib->attrib_value.service.service_UUID_len == 0x10)
            {
                LMutils_Set_U128bits(p_payload+4, p_current_attrib->attrib_value.service.service_UUID.p_UUID);
                p_payload+=20;
                *p_result_len+=20;
            }
        }
        i++;
    }

    if (*p_result_len == 0x00)
    {
        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;
    }
    return 0;
}

/*****************************************************************************************************
 * GATTserver_Find_DB_Index
 *
 * Simple function which finds the DB index for a given handle.
 *
 *****************************************************************************************************/
u_int8 GATTserver_Find_DB_Index(u_int16 start_handle,u_int16 end_handle,u_int16* p_index)
{
    u_int16 i=0;

   
    if((start_handle > end_handle) || (start_handle == 0x0000) || (start_handle > GATTserver_DB_last_handle))
        return GATT_ERROR_INVALID_HANDLE;

    while ((i<=GATTserver_DB_last_index) && (GATTserver_DB[i].attrib_handle < start_handle))
    {
        i++;
    }
    *p_index = i;

    return NO_ERROR;
}

/*****************************************************************************************************
 * GATTserver_Find_By_Type_And_Value
 *
 * This function supports the Clients Discover Primary (or Secondary) Services By Service UUID
 *
 * The service may exist multiple times on a server. The primary service being discovered is
 * identified by the service UUID.
 * INPUTS
 * u_int16  max_result_len : This is the maximum length of data which can be returned.
 * u_int16  start_handle   : The start handle  for the range to be searched
 * u_int16  end_handle     : The end handle  for the range to be searched
 *
 * u_int16  attrib_type   : PRIMARY_SERVICE_TYPE | SECONDARY_SERVICE_TYPE
 *
 * u_int16  attrib_value  : The UUID value for the Service
 *
 * OUTPUTS
 * u_int8* p_result       :- A pointer to a list of start and end handle pairs corresponding to each service matching
 *                              attrib_type and value.
 * u_int8* p_result_len   :- A pointer the total length of valid data in the result field ( pointed to by p_result ).
 *
 *****************************************************************************************************/

u_int8 GATTserver_Find_By_Type_And_Value(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,
                                         u_int16 attrib_type,u_int16 attrib_value,u_int8* p_result,u_int8* p_result_len)
{
    u_int16 i=0;
    u_int8 result_len = 0;
    u_int8* p_payload = p_result;
    u_int8 status;

    *p_result_len = 0x00;

    status = GATTserver_Find_DB_Index(start_handle,end_handle,&i);
    if (status != NO_ERROR)
        return status;

    // i now is the index to the start handle in the Server DB

    while ((GATTserver_DB[i].attrib_handle <= end_handle) && ((result_len+6) < max_result_len) && (i<=GATTserver_DB_last_index))
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];

        // if the attrib_UUID match

        if ((p_current_attrib->attrib_UUID.UUID_val == attrib_type) && (p_current_attrib->permissions & GATT_PERMISSION_READ) &&
            (p_current_attrib->attrib_value.service.service_UUID.UUID_val == attrib_value))
        {
            // Matching entry and Readable

            // Check if authentication required
            status = GATTserver_Check_Permissions(p_current_attrib, GATT_READ);

            if (status != NO_ERROR)
                return status;


            LMutils_Set_Uint16(p_payload, p_current_attrib->attrib_handle );
            LMutils_Set_Uint16(p_payload+2, _GATTserver_Find_End_Group_Handle(i) );

            p_payload+=4;
            *p_result_len+=4;
        }
        i++;
    }
    if (*p_result_len == 0x00)
        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;

    return NO_ERROR;
}


/*****************************************************************************************************
 * GATTserver_Find_Included_Services
 *
 * This function is used to find include service declarations within a service definition on a server. The service specified
 * is identified by the service handle range..
 *
 * The Starting Handle shall be set to starting handle of the specified service and the Ending Handle shall be set to the
 * ending handle of the specified service.
 *
 * This function supports the discovery of both 16 Bit and 128 Bit UUIDs.
 *
 * Parameters :
 *     u_int16 max_result_len :- This is the maximum length of data which can be returned.
 *  u_int16 start_handle   :- The Starting handle of a service.
 *  u_int16 end_handle     :- The end handle of the  service.
 *  u_int8* p_result       :- A pointer to a list of Attribute Handle and Attribute Value pairs corresponding to the included
 *                            services in the service definition
 *  u_int8* p_result_len   :- A pointer the total length of valid data in the result field ( pointed to by p_result ).
 *
 *****************************************************************************************************/
u_int8 GATTserver_Find_Included_Services(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int8* p_result,u_int8* p_result_len)
{
#if(PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_INCLUDED_SERVICES==1)
    u_int16 i=0;
    u_int8* p_payload = p_result;
    u_int8 status;

    *p_result_len = 0x00;

    status = GATTserver_Find_DB_Index(start_handle,end_handle,&i);
    if (status != NO_ERROR)
        return status;

    // i now is the index to the start handle in the Server DB

    while ((GATTserver_DB[i].attrib_handle <= end_handle) && ((*p_result_len+8) < max_result_len) && (i<=GATTserver_DB_last_index))
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];
        GATTserver_Handle_in_Error = GATTserver_DB[i].attrib_handle;
        // if the attrib_UUID match
        if ((p_current_attrib->attrib_UUID.UUID_val == GATT_ATT_TYPE_INCLUDE) && (p_current_attrib->permissions & GATT_PERMISSION_READ))
        {
            LMutils_Set_Uint16(p_payload, p_current_attrib->attrib_handle );
            LMutils_Set_Uint16(p_payload+2, p_current_attrib->attrib_value.include_service.start_handle );
            LMutils_Set_Uint16(p_payload+4, p_current_attrib->attrib_value.include_service.end_handle );
            if (p_current_attrib->attrib_len == 02)
            {
                LMutils_Set_Uint16(p_payload+6, p_current_attrib->attrib_value.include_service.service_UUID.UUID_val);
                p_payload+=8;
                *p_result_len+=8;
            }
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_128_BIT_UUIDS == 1)
            else
            {
                p_payload+=6;
                *p_result_len+=6;
            }
#endif
        }
        i++;
    }

    if (*p_result_len == 0x00)
        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;

    return NO_ERROR;
#else
    return GATT_ERROR_ATTRIBUTE_NOT_FOUND;
#endif


}
/*****************************************************************************************************
 * GATTserver_Discover_All_Characteristics_Of_A_Service
 *
 * This function is used to support Characteristic Discovery. Both sub-procedures are supported by this function on the server.
 * The difference between the Discover_All_Characteristics_Of_A_Service and Discover_Characteristics_By_UUID exists on the
 * Client side - no differences on the server side.
 *
 * This function supports the discovery of both 16 Bit and 128 Bit UUIDs.
 *
 * Parameters :
 *     u_int16 max_result_len :- This is the maximum length of data which can be returned.
 *  u_int16 start_handle   :- The Starting handle of a service.
 *  u_int16 end_handle     :- The end handle of the specified service.
 *  u_int8* p_result       :- A pointer to a list of Attribute Handle and Attribute Value pairs corresponding to the characteristics
 *                            in the service definition. The Attribute Handle is the handle for the characteristic
 *                            declaration. The Attribute Value is the Characteristic Properties, Characteristic Value Handle
 *                            and Characteristic UUID.
 *  u_int8* p_result_len   :- A pointer the total length of valid data in the result field ( pointed to by p_result ).
 *
 *****************************************************************************************************/
u_int8 GATTserver_Discover_All_Characteristics_Of_A_Service(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int8* p_result,u_int8* p_result_len,u_int8* p_attrib_val_len)
{
    u_int16 i=0;
    u_int8 attrib_val_len = 0x00;
    u_int8* p_payload = p_result;
    u_int8 status;

    *p_result_len = 0x00;

    GATTserver_Handle_in_Error = start_handle;
    status = GATTserver_Find_DB_Index(start_handle,end_handle,&i);
    if (status != NO_ERROR)
    {
        return status;
    }

    // i now is the index to the start handle in the Server DB

    while ((GATTserver_DB[i].attrib_handle <= end_handle) && ((*p_result_len+*p_attrib_val_len) <= (max_result_len-7)) && (i<=GATTserver_DB_last_index))
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];

        // if the attrib_UUID match
        if (p_current_attrib->attrib_UUID.UUID_val == GATT_ATT_TYPE_CHARACTERISTIC)
        {

            // All Characterisic Declarations are Readable - without Authent/Author
            // See - Section 3.3.1
            // 3.3.1 Characteristic Declaration
            // "A characteristic declaration is an Attribute with the Attribute Type set to the
            // UUID for �Characteristic?and Attribute Value set to the Characteristic Properties,
            // Characteristic Value Attribute Handle and Characteristic UUID. The Attribute
            // Permissions shall be readable and not require authentication or
            // authorization."

            LMutils_Set_Uint16(p_payload,p_current_attrib->attrib_handle);
            p_payload[2] = p_current_attrib->attrib_value.char_declaration.properties;
            LMutils_Set_Uint16(p_payload+3,p_current_attrib->attrib_value.char_declaration.char_value_handle);
            if ((p_current_attrib->attrib_value.char_declaration.UUID_len == 0x02) &&
                (attrib_val_len != 21))
            {
                if (attrib_val_len==0x00) // not_determined yet
                    attrib_val_len = 7;

                LMutils_Set_Uint16(p_payload+5,p_current_attrib->attrib_value.char_declaration.char_UUID.UUID_val);

            }
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_128_BIT_UUIDS == 1)
            else if ((p_current_attrib->attrib_value.char_declaration.UUID_len == 0x10 ) && ((*p_result_len+20) < max_result_len) &&
                     (attrib_val_len != 7))
            {
                u_int8 j;

                if (attrib_val_len==0x00) // not_determined yet
                    attrib_val_len = 21;

                for(j=0;j<16;j++)
                    p_payload[j+5] = p_current_attrib->attrib_value.char_declaration.char_UUID.p_UUID[j];

            }
            else
            {
                break;
            }
#endif
            p_payload+= attrib_val_len;
            *p_result_len+= attrib_val_len;
        }
        i++;
    }
    *p_attrib_val_len = attrib_val_len;

    if (*p_result_len == 0x00)
    {
        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;
    }

    return NO_ERROR;
}

u_int16 Get_GATTserver_DB_Last_Handle(void)
{
    return GATTserver_DB_last_handle;
}

/*****************************************************************************************************
 * GATTserver_Find_Descriptor_Information
 *
 * This function is used to discover all the descriptors for a characteristic. The characteristic is identified by the
 * the handle range.
 *
 * This function supports the discovery of both 16 Bit and 128 Bit UUIDs.
 *
 * Parameters :
 * INPUTS
 *     u_int16 max_result_len :- This is the maximum length of data which can be returned.
 *  u_int16 start_handle   :- The Starting handle of a characteristic + 1.
 *  u_int16 end_handle     :- The end handle of the characteristic.
 * OUTPUTS
 *  u_int8* p_result       :- A pointer to a list of Attribute Handle and Attribute Value pairs corresponding to the
 *                            characteristic descriptors in the characteristic definition. The Attribute Handle is the
 *                            handle for the characteristic descriptor declaration. The Attribute Value is the
 *                            Characteristic Descriptor UUID.
 *
 *  u_int8* p_result_len   :- A pointer the total length of valid data in the result field ( pointed to by p_result ).
 *
 *  u_int8* p_format       :- Is a pointer to a variable indicating the format of the data in the result field (p_result).
 *                            If *p_format = 0x01, then only 16 Bit UUIDs are used in the result field.
 *                            If *p_format = 0x02, then only 128 Bit UUIDs are used in the result field.
 *****************************************************************************************************/

u_int8 GATTserver_Find_Descriptor_Information(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int8* p_result,u_int8* p_result_len,u_int8* p_format)
{
    u_int16 i=0;
    u_int8 result_len = 0;
    u_int8* p_payload = p_result;
    u_int8 status;
    u_int8 attrib_found_len=0;

    status = GATTserver_Find_DB_Index(start_handle,end_handle,&i);
    if (status != NO_ERROR)
        return status;

    while ((GATTserver_DB[i].attrib_handle <= end_handle) && (i<=GATTserver_DB_last_index))
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];
        {

            /* The server shall not respond to the Find Information Request with an Error
             * Response with the �Insufficient Authentication? �Insufficient Authorization?
             * �Insufficient Encryption Key Size?or �Application Error?error code.
             */

            if (attrib_found_len != 0)
            {
                if ((attrib_found_len != p_current_attrib->attrib_len) ||
                    (!(result_len+p_current_attrib->attrib_len+2) <= max_result_len))
                {
                    // Mismatch in lengths - so end search.
                    return NO_ERROR;
                }
            }
            else
            {
                // First Attrib Found
                attrib_found_len = p_current_attrib->attrib_len;
            }
            LMutils_Set_Uint16(p_payload,p_current_attrib->attrib_handle);

            if (attrib_found_len==0x02)
            {
                *p_format = 0x01;
                LMutils_Set_Uint16(p_payload+2,p_current_attrib->attrib_UUID.UUID_val);
            }
            else if (attrib_found_len==0x10)
            {
                u_int8 j;

                *p_format = 0x02;
                for (j=0;j<attrib_found_len;j++)
                    p_payload[j+2] = p_current_attrib->attrib_UUID.p_UUID[j];
            }
            else
            {
                // This is important - required for PTS 4.5.3 GAR_SR_BV06.
                return GATT_ERROR_ATTRIBUTE_NOT_FOUND;
            }

            p_payload+=attrib_found_len+2;
            result_len += attrib_found_len+2;

            *p_result_len = result_len;
        }

        i++;
    }

    if (*p_result_len == 0x00)
        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;

    return NO_ERROR;
}

/*****************************************************************************************************
 * GATTserver_Read_Characteristic_Value
 *
 * This function is used to read the value stored at a given attribute handle. This function can be used to read either
 * a characteristic value or a characteristic descriptor.
 *
 * Parameters :
 * INPUTS
 *     u_int16 max_result_len :- This is the maximum length of data which can be returned.
 *  u_int16 att_handle     :- The handle for the characteristic value to be read.
 *
 * OUTPUTS
 *  u_int8* p_result       :- The result field. A pointer to a characteristic or characteristic descriptor value.
 *
 *
 *  u_int8* p_result_len   :- A pointer the total length of valid data in the result field ( pointed to by p_result ).
 *
 *****************************************************************************************************/

u_int8 GATTserver_Read_Characteristic_Value(u_int16 max_result_len,u_int16 att_handle, u_int8* p_result,u_int8* p_result_len)
{
    u_int16 i = 0;
    u_int16 j = 0;
    t_attrib* p_current_attrib;
    t_error status;
    u_int16  UUID;

    // Find the Attribute Value Handle
    if (att_handle > GATTserver_DB_last_handle)
        return GATT_ERROR_INVALID_HANDLE;

    while ((GATTserver_DB[i].attrib_handle != att_handle) && (i<=GATTserver_DB_last_index))
    {
        i++;
    }

    if (GATTserver_DB[i].attrib_handle != att_handle)
        return GATT_ERROR_INVALID_HANDLE;


    // Now we have found the Attrib Value - 'i' is the index for it.
    p_current_attrib = &GATTserver_DB[i];

    /*
     * Check that this is the UUID for a characteristic or or characteristic descriptor.
     * Basically check it is not the UUID for a Service, Char Declaration,
     */
    status = ( t_error )GATTserver_Check_Permissions(p_current_attrib, GATT_READ);

    if (status != NO_ERROR)
        return status;

    if (p_current_attrib->attrib_UUID.UUID_val)
    {
        UUID = p_current_attrib->attrib_UUID.UUID_val;
#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5!=1)
        // Check is UUID is Primary Service, Secondary Service, Include Definition or Characteristic Declaration
        if ((UUID >= 0x2800) && (UUID <= 0x2803))
            return GATT_ERROR_INVALID_HANDLE;
#endif
    }

    {
        // Dont Know if this is a Read Value for a Characteristic Value or a Characteristic Descriptor ..
        // Only know from the Handle Which Type it is...

        u_int8* p_val = p_current_attrib->attrib_value.value.p_value;
        u_int8 length = p_current_attrib->attrib_value.value.val_len;

        if (length > max_result_len)
            length = max_result_len;

        for (j=0;j<length;j++)
            p_result[j] = p_val[j];

        *p_result_len = length;

    }
    return 0;
}

/*****************************************************************************************************
 * GATTserver_Read_Long_Characteristic_Value_Offset
 * [ Note can be optimised to include the GATTserver_Read_Characteristic_Value function also ]
 *
 * This function is used to read a long value stored at a given attribute handle. This function can be used to read
 * a characteristic value.
 *
 * Parameters :
 * INPUTS
 *     u_int16 max_result_len :- This is the maximum length of data which can be returned.
 *  u_int16 att_handle     :- The handle for the characteristic value to be read.
 *  u_int16 offset         :- The offset into the attribute value of the first byte to be read.
 *
 * OUTPUTS
 *  u_int8* p_result       :- The result field. A pointer to a characteristic or characteristic descriptor value.
 *
 *
 *  u_int8* p_result_len   :- A pointer the total length of valid data in the result field ( pointed to by p_result ).
 *
 *****************************************************************************************************/


u_int8 GATTserver_Read_Long_Characteristic_Value_Offset(u_int16 max_result_len,u_int16 att_handle, u_int16 offset, u_int8* p_result,u_int8* p_result_len)
{
    u_int8 i = 2;
    u_int8 j;
    t_attrib* p_current_attrib;
    t_error status;

    // Find the Attribute Value Handle
    if (att_handle > GATTserver_DB_last_handle)
        return GATT_ERROR_INVALID_HANDLE;

    while ((GATTserver_DB[i].attrib_handle != att_handle) && (i<=GATTserver_DB_last_index))
    {
        i++;
    }


    if (GATTserver_DB[i].attrib_handle != att_handle)
        return GATT_ERROR_INVALID_HANDLE;

    // Now we have found the Attrib Value - 'i' is the index for it.
    p_current_attrib = &GATTserver_DB[i];

    // Check the permissions
    if ((p_current_attrib->attrib_UUID.UUID_val >= 0x2800) && (p_current_attrib->attrib_UUID.UUID_val <= 0x2803))
        return GATT_ERROR_INVALID_HANDLE;

    status = (t_error) GATTserver_Check_Permissions(p_current_attrib, GATT_READ);

    if (status != NO_ERROR)
        return status;

    // Now get the pointer to the Attrib value and check attrib length.
    {
        extern t_ATT_config ATT_config;
        u_int8* p_val = p_current_attrib->attrib_value.value.p_value;
        u_int8 length = p_current_attrib->attrib_value.value.max_len;

        if (length <= (ATT_config.MTU_LEN-1))
            return LE_ATT_ATTRIBUTE_NOT_LONG;

        // Was bug here " >= "..
        // Spec says that if the offset == length the device returns a Read_Blob_Response with
        // zero payload.

        if (offset > length)
            return GATT_ERROR_INVALID_OFFSET;

        if ((length - offset) > max_result_len)
            length = max_result_len;
        else
            length = (length - offset);

        for (j=0;j<length;j++)
            p_result[j] = p_val[j+offset];

        *p_result_len = length;

    }
    return 0;
}

/*****************************************************************************************************
 * GATTserver_Read_Characteristic_Value_By_UUID
 * [ Note can be optimised to include the GATTserver_Read_Characteristic_Value function also ]
 *
 *  This function is used to read a Characteristic Value from a server when the client only knows the characteristic UUID
 *  and does not know the handle of the characteristic. The range is delimited by the start and end handles and is typically
 *  the handle range for the service in which the characteristic belongs.
 *
 * Parameters :
 * INPUTS
 *     u_int16 max_result_len   :- This is the maximum length of data which can be returned.
 *  u_int16 start_handle     :- The start of the range over which this read is to be performed.
 *  u_int16 end_handle       :- The end of the range over which this read is to be performed.
 *  u_int16 attrib_UUID      :- Attribute UUID of the characteristic to be read.
 *
 * OUTPUTS
 *  u_int8* p_result       :- The result field. A pointer to a characteristic or characteristic descriptor value.
 *
 *
 *  u_int8* p_result_len   :- A pointer the total length of valid data in the result field ( pointed to by p_result ).
 *
 *****************************************************************************************************/
// Was/Is BUG here with read by type for 16 Byte UUIDs
#if 1
//u_int8 GATTserver_Read_Characteristic_Value_By_UUID(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,u_int16 attrib_UUID,u_int8* p_result,u_int8* p_result_len,u_int8* p_attrib_len)
u_int8 GATTserver_Read_Characteristic_Value_By_UUID(u_int16 max_result_len,u_int16 start_handle,u_int16 end_handle,t_attrib_UUID* p_att_UUID,u_int8* p_result,u_int8* p_result_len,u_int8* p_attrib_len)
{
    u_int16 i=0;
    u_int8 j;
    u_int8 result_len = 0;
    //u_int8* p_payload;
    u_int8 status;

    *p_result_len = 0x00;

    GATTserver_Handle_in_Error = start_handle;
    //p_payload = p_result;
    status = GATTserver_Find_DB_Index(start_handle,end_handle,&i);
    if (status != NO_ERROR)
        return status;

    // i now is the index to the start handle in the Server DB

    while ((GATTserver_DB[i].attrib_handle <= end_handle) && ((result_len+6) < max_result_len) && (i<=GATTserver_DB_last_index))/* TP/GPA/SR/BV-01-C bug fix*/
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];

        GATTserver_Handle_in_Error = GATTserver_DB[i].attrib_handle;
        // if the attrib_UUID match

        if (GATTserver_UUID_Match(&p_current_attrib->attrib_UUID,p_current_attrib->attrib_len,p_att_UUID))
        {

            status = GATTserver_Check_Permissions(p_current_attrib, GATT_READ);

            if (status != NO_ERROR)
                return status;

            // Add the Attribute Handle to the Result

            LMutils_Set_Uint16(p_result, p_current_attrib->attrib_handle);//charles modify
						
					
						
						if( (p_att_UUID->UUID_val == GATT_ATT_TYPE_PRIMARY_SERVICE) || (p_att_UUID->UUID_val == GATT_ATT_TYPE_SECONDARY_SERVICE))/* TP/GPA/SR/BV-01-C bug fix*/
						{
							 
							 u_int8 length = p_current_attrib->attrib_value.service.service_UUID_len ;/* TP/GPA/SR/BV-01-C bug fix*/
							
               // p_current_attrib->attrib_value.service.service_UUID.UUID_val = service_UUID;
							 p_result[2] = (p_current_attrib->attrib_value.service.service_UUID.UUID_val) ;/* TP/GPA/SR/BV-01-C bug fix*/
							 p_result[3] = (p_current_attrib->attrib_value.service.service_UUID.UUID_val) >> 8 ;/* TP/GPA/SR/BV-01-C bug fix*/

							 result_len = *p_result_len += length+2;/* include handle length  TP/GPA/SR/BV-01-C bug fix */
               *p_attrib_len = length+2;/* include handle length TP/GPA/SR/BV-01-C bug fix*/
               //p_payload+=length;/* TP/GPA/SR/BV-01-C bug fix */
							 p_result+=*p_attrib_len;/* TP/GPA/SR/BV-01-C bug fix */
							//return NO_ERROR;
							//if(*p_result_len >= 20)/* until MTU size charles modify*/
						  //{
							//	 return NO_ERROR;
							//}
						}
						else
            // Now get the pointer to the Attrib value and check attrib length.
            {
                u_int8* p_val = p_current_attrib->attrib_value.value.p_value;
                u_int8 length = p_current_attrib->attrib_value.value.val_len;

               
                if (length > max_result_len)
                    length = max_result_len;

                for (j=0;j<length;j++)
								{
                    p_result[j+2] = p_val[j];

								}
								
                result_len = *p_result_len += length+2;/* TP/GPA/SR/BV-01-C bug fix */
                *p_attrib_len = length+2;
					      p_result+=*p_attrib_len;/* TP/GPA/SR/BV-01-C bug fix */
            }
        }
        i++;
    }
    if (*p_result_len == 0x00)
        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;

    return NO_ERROR;

}
#endif
/*****************************************************************************************************
 * GATTserver_Write_Command
 * [ Note can be optimised to include the  GATT_Server_Charactaristic_Write_Local_Value also ]
 *
 *  This function is used to write a characteristic value or a characteristic descriptor value to the local Server DB.
 *  If the attrib_handle is the handle of a characteristic handle then the 'local notify' field is checked and if set
 *  then a Notification is sent locally to indicate the handle has been written to.
 *
 *  If the UUID for the attrib_handle is a RECONNECTION_ADDRESS then the reconnection_address_valid flag is set. This
 *  flag is used by the GAP.
 *
 * Parameters :
 * INPUTS
 *
 *     u_int16 attrib_handle      :- This is the handle for the characteristic or characteristic descriptor to be written
 *  u_int8* p_attrib_value     :- A pointer to the data to be written,
 *  u_int16 length             :- The number of octets of data to be written.
 *
 *
 *****************************************************************************************************/
#ifdef BEKEN_DEBUG
extern void bt_frontend_notification(const char *fmt, ...);
#endif
u_int8 GATTserver_Write_Command(u_int16 attrib_handle, u_int8* p_attrib_value,u_int8 length)
{
    // First find the handle
    u_int8 i=0;
    //t_error status=NO_ERROR;
	  t_GATT_error status = GATT_NO_ERROR;
    u_int8 TX_power_attrib_value[1];//charles add
    
#ifdef LE_DEBUG_ENABLE
    bt_frontend_notification("GATTserver_Write_Command: handle:%x, p_value: %s, length %d",attrib_handle,p_attrib_value, length);
#endif	

    // Find the start handle
    while ((GATTserver_DB[i].attrib_handle != attrib_handle) && (i<=GATTserver_DB_last_index))
    {
        i++;
    }

    if ((GATTserver_DB[i].attrib_handle != attrib_handle) ||
        ((GATTserver_DB[i].attrib_UUID.UUID_val >= 0x2800) && (GATTserver_DB[i].attrib_UUID.UUID_val <= 0x2803)))
    {
        return (t_error) GATT_ERROR_INVALID_HANDLE;
    }
    else
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];

        status = GATTserver_Check_Permissions(p_current_attrib, GATT_WRITE);

        if (status != GATT_NO_ERROR)
            return status;

        if (GATTserver_DB[i].attrib_UUID.UUID_val == GATT_CHAR_MEASUREMENT_INTERVAL)
        {
            u_int16 req_value = ((u_int16)p_attrib_value[0] + ((u_int16) p_attrib_value[1] << 8));

            if (req_value)
            {

                // Check if there is a range associated with the characteristic.
                //   First Step is to find the Range Id
                u_int16 j = i+1;

                while ((GATTserver_DB[j].attrib_UUID.UUID_val != VALID_RANGE) &&
                       (GATTserver_DB[j].attrib_UUID.UUID_val != GATT_ATT_TYPE_CHARACTERISTIC) &&
                       (j<=GATTserver_DB_last_index))
                {
                    j++;

                }

                if (GATTserver_DB[j].attrib_UUID.UUID_val == VALID_RANGE)
                {
                    u_int8* p_min_value = GATTserver_DB[j].attrib_value.value.p_value;
                    u_int8* p_max_value = (GATTserver_DB[j].attrib_value.value.p_value+2);
                    u_int16 min_value = ((u_int16)p_min_value[0] + ((u_int16) p_min_value[1] << 8));
                    u_int16 max_value = ((u_int16)p_max_value[0] + ((u_int16) p_max_value[1] << 8));

			  
                    if ((req_value < min_value) || (req_value > max_value))
                    {
                        return GATT_ERROR_OUT_OF_RANGE;
                    }

                }
            }

        }

        // Now write the Value...
        {
            //    u_int8 value_len  = p_current_attrib->attrib_value.value.val_len;
            u_int8 max_len = p_current_attrib->attrib_value.value.max_len;

            u_int8* p_value = p_current_attrib->attrib_value.value.p_value;
            u_int8 descriptor = 0x00;
            u_int16 UUID_val = p_current_attrib->attrib_UUID.UUID_val;
			
            if ((UUID_val >= 0x2900) && (UUID_val<= 0x2905))/* Characteristic Descriptor */
            {
                descriptor = 0x01;
            }
#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)
            else if(UUID_val == RECONNECTION_ADDRESS)
            {
                if(GATTprivacy_Flags[0] == 0)/* TP/CONN/UCON/BV-05-C GAP layer */
                {
                    status = GATT_ERROR_WRITE_NOT_PERMITTED;
                    return status;
                }
                reconnection_addr_valid = 0x01;
            }
#endif
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
            else if (UUID_val == PERIPHERAL_PRIVACY_FLAGS)
            {
                if (*p_attrib_value == 0x01)
                {
                    // Enable Write of the Reconnection Address
                    GATTserver_DB[GATTserver_DB_Reconnection_Address_Index].permissions |= GATT_PERMISSION_WRITE;
                }
                else
                {
                    // Disable Write of the Reconnection Address
                    GATTserver_DB[GATTserver_DB_Reconnection_Address_Index].permissions &= (~GATT_PERMISSION_WRITE);
                }
            }
#endif
            else if (GATTserver_DB[i].attrib_handle == HRS_CONTROL_POINT_HANDLE)
            {
                if (p_attrib_value[0] > 0x01)
                    //return GATT_ERROR_OUT_OF_RANGE;
                    return 0x80; // Control point not supported -- to comply with Test Spec for HRS./
            }
            //    else if ((GATTserver_DB[i].attrib_handle == CSC_SC_CONTROL_POINT_HANDLE))
            else if (UUID_val == GATT_CHAR_SC_CONTROL_POINT)
            {
#if 0 // GF 28 Jan 2013
                {
                    extern t_ATT_config ATT_config;

                    if(ATT_config.pending_ATT_opcode == ATT_HANDLE_VALUE_INDICATION)
                        return 0x80; /* Procedure Already in Progress */
                }
#endif
                status = GATT_SERVER_HANDLE_WRITE_RESP; // Trigger to send the Write_Response from within this function.

                // It we are writing to a control point we need to determine the the corresponding CCC has been
                // correctly configure for indications.

                while ((GATTserver_DB[i].attrib_UUID.UUID_val != CLIENT_CHARACTERISTIC_CONFIGURATION) && (i<=GATTserver_DB_last_index) &&
                       ((GATTserver_DB[i].attrib_UUID.UUID_val & 0xFF00) != 0x2800))
                {
                    i++;
                }
                if ((i<=GATTserver_DB_last_index) && (GATTserver_DB[i].attrib_UUID.UUID_val == CLIENT_CHARACTERISTIC_CONFIGURATION))
                {
                    // We have found our CCC
                    u_int8* p_config_value = GATTserver_DB[i].attrib_value.value.p_value;
                    u_int16 config_value = ((u_int16)p_config_value[0] + ((u_int16) p_config_value[1] << 8));
#if 1 // UPF 44 - Canon have to stop devices writing to our CCC values which are not allowed by our properties
                    u_int8 properties = GATTserver_DB[(i-2)].attrib_value.char_declaration.properties;
                    // Firstly Cannot write a CCC greater that 0x02
                    if (p_attrib_value[0] > 0x02)
                        return (t_error)GATT_ERROR_OUT_OF_RANGE;
                    if (!(p_attrib_value[0] & (properties >> 4)))
                        return (t_error)GATT_ERROR_OUT_OF_RANGE;
#endif

                    if (config_value != 0x02 /* Indicate - wait for Ack */)
                    {
                        return 0x81; // Application Error
                    }
                }
            }
            if (max_len < length)
            {
                os_printf("GATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH   UUID_val:%x,max_len:%d  length:%d\r\n",UUID_val,max_len,length);

                return GATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
	     }
            //if (length <= value_len)
            {
                u_int8 j;

                for (j=0;j<length;j++)
                {
                    p_value[j] = p_attrib_value[j];
                }

                p_current_attrib->attrib_value.value.val_len = length;

                if ((!descriptor) & (p_current_attrib->attrib_value.value.local_notification))//for ATT_WRITE command
                {
                    if (status == GATT_SERVER_HANDLE_WRITE_RESP)
                    {
                        t_Att_pdu_params  pdu_params;
                        LEatt_Encode_PDU(GATT_Get_Active_Handle(),ATT_WRITE_RESPONSE,&pdu_params);
                    }
                    else
                    {
                    	GATT_Event(GATT_WRITE_COMPLETE_EVENT,attrib_handle,length, 0,p_value, NO_ERROR);
                    	//os_printf("val = %d\r\n",p_attrib_value[0]);
                    }
                    
                #ifdef BEKEN_BLE_OTA_3266
					//add begin by  ps for OTA
                    if(GATTserver_DB[i].attrib_handle == OADIMGIDENTIFY_BLE_HANDLE)
                    {
                        extern u_int8 ota_update_flag;
                        ota_update_flag = 1;
                        //oadImgIdentifyWrite( 0x10,length, p_value );
                        oad_pdu_decode(p_value, length );
                    }
                    if(GATTserver_DB[i].attrib_handle == OADIMGBLOCK_BLE_HANDLE)
                    {
                        //oadImgBlockWrite( 0x10,length,p_value );						
                    }
					//add end by  ps for OTA
                #endif// BEKEN_UP_OTA_3266

                    if( (GATTserver_DB[i].attrib_handle == IAS_SERVER_IMMEDIATE_ALERT_HANDLE)//For Find ME TI Iphone APP charles modify
                        ||(GATTserver_DB[i].attrib_handle == LINK_LOSS_SERVER_IMMEDIATE_ALERT_HANDLE))
                    {
                        // u_int8 temp_REG24;

                        switch (p_attrib_value[0])//For Find ME TI Iphone APP
                        {
                        case 3 : //short distant
                            TX_power_attrib_value[0] = 0xE9;
                            GATTserver_Characteristic_Write_Local_Value( TPS_SERVER_TX_POWER_HANDLE,1,TX_power_attrib_value);//-23db
                            HWradio_Set_Tx_Power_Level(3);//real RF power -17.2 db
                            //                                      temp_REG24 = XVR_REG24;
                            //                        temp_REG24 &= ~0x00000780;
                            //                        temp_REG24 |= ;
                            break;
                        case 4 ://medium distant
                            TX_power_attrib_value[0] = 0xFA;
                            GATTserver_Characteristic_Write_Local_Value( TPS_SERVER_TX_POWER_HANDLE,1,TX_power_attrib_value);//-6db
                            HWradio_Set_Tx_Power_Level(5);//real RF power -7.92db
                            break;
                        case 5 ://long distant
                            TX_power_attrib_value[0] = 0x04;
                            GATTserver_Characteristic_Write_Local_Value( TPS_SERVER_TX_POWER_HANDLE,1,TX_power_attrib_value);//+4db
                            HWradio_Set_Tx_Power_Level(0x0F);//real RF  power +3.68db
                            break;
                        case 0 :
                            //Turn off buzzer
#if (MMI_ON == 1)
                            Set_MMI_Cycle_time(0);
#endif

                            LEapp_Config.Alert_Level = 0;
                            GATTserver_Characteristic_Write_Local_Value( GATTserver_DB[i].attrib_handle,1,&LEapp_Config.Alert_Level);
#if (MMI_ON == 1)
                            Set_MMI_State(ALERT_LEVEL_SETTING_STATE);
#endif
                            break;
                        case 1 :
                            //Mid Alert
                            break;
                        case 2 :
#if (MMI_ON == 1)
                            //High Alert
                            Set_MMI_Cycle_time(0);
#endif
                            LEapp_Config.Alert_Level = 2;
                            GATTserver_Characteristic_Write_Local_Value( GATTserver_DB[i].attrib_handle,1,&LEapp_Config.Alert_Level);
#if (MMI_ON == 1)
                            Set_MMI_Cycle_time(25);
                            Set_MMI_State(ALERT_LEVEL_SETTING_STATE);
#endif
                            break;
                        }
                    }
                    GATT_Event(GATT_WRITE_INDICATION_EVENT,attrib_handle,0x00,0x00,0x00,NO_ERROR);

                    return status;
                }
            }
        }

    }
    return 0;
}


/*****************************************************************************************************
 * GATTserver_Characteristic_Read_Local_Value
 *
 *  This function is used to read a characteristic value or a characteristic descriptor value from the local Server DB.
 *  This is invoked by the Local Host ( not the Client ).
 *
 * Parameters :
 * INPUTS
 *
 *     u_int16 char_val_handle      :- This is the handle for the characteristic or characteristic descriptor to be written
 *  u_int8** p_value             :- A pointer to pointer which will be set to the data being read,
 *  u_int8*  p_len               :- A pointer to the length of data which is read.
 *
 *
 *****************************************************************************************************/

t_GATT_error GATTserver_Characteristic_Read_Local_Value(u_int16 char_val_handle,u_int8* p_len,/*const*/ u_int8** p_value)
{
    u_int8 i=0;

    // Find the handle
    while ((GATTserver_DB[i].attrib_handle != char_val_handle) && (i<=GATTserver_DB_last_index))
    {
        i++;
    }

    if (GATTserver_DB[i].attrib_handle != char_val_handle)
    {
        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;
    }

    else if ((GATTserver_DB[i].attrib_UUID.UUID_val >= 0x2800) && (GATTserver_DB[i].attrib_UUID.UUID_val <= 0x2803))
    {
        return GATT_ERROR_INVALID_HANDLE;
    }
    else
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];

        *p_value = p_current_attrib->attrib_value.value.p_value;
        *p_len = p_current_attrib->attrib_value.value.val_len;

    }
	return GATT_NO_ERROR;
}

/*****************************************************************************************************
 * GATTserver_Characteristic_Write_Local_Value
 *
 *  This function is used to write a characteristic value or a characteristic descriptor value to the local Server DB.
 *  This is invoked by the Local Host ( not the Client ).
 *
 *  If the char_val_handle is that of a Reconnection_address_descriptor the 'reconnection_addr_valid' flag is set.
 *
 *  If the handle is the handle of a characteristic value, then we search for a Client Configuration Characteristic (CCC) Descriptor
 *  included in the Characteristic Definition. If one is found then we check the value of the CCC and determine if a ATT_NOTIFY
 *  or ATT_INDICATION should be sent to the peer.
 *
 * Parameters :
 * INPUTS
 *
 *     u_int16 char_val_handle      :- This is the handle for the characteristic or characteristic descriptor to be written
 *  u_int8* p_attrib_value       :- A pointer to the data which is to be written.
 *  u_int8  length               :- length of data which is to be written.
 *
 *
 *****************************************************************************************************/

t_GATT_error GATTserver_Characteristic_Write_Local_Value(u_int16 char_val_handle,u_int8 length, u_int8* p_attrib_value)
{
    u_int8 i=0;
    u_int8 j=0;
    u_int16 char_UUID;

    // Find the start handle
    while ((GATTserver_DB[i].attrib_handle != char_val_handle) && (i<=GATTserver_DB_last_index))
    {
        i++;
    }

    char_UUID = GATTserver_DB[i].attrib_UUID.UUID_val;

    if (GATTserver_DB[i].attrib_handle != char_val_handle)
    {

        return GATT_ERROR_ATTRIBUTE_NOT_FOUND;
    }
    else if ((char_UUID >= 0x2800) && (char_UUID <= 0x2803))
    {

        return GATT_ERROR_INVALID_HANDLE;
    }
    else
    {
        t_attrib* p_current_attrib = &GATTserver_DB[i];


        // Now write the Value...
        {
            //u_int8 value_len = p_current_attrib->attrib_value.value.val_len;
            u_int8 value_len = p_current_attrib->attrib_value.value.max_len;
            u_int8* p_value = p_current_attrib->attrib_value.value.p_value;


            // Now get the pointer to the Attrib value and check attrib length.
#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)
            if(char_UUID == RECONNECTION_ADDRESS)
            {
                reconnection_addr_valid = 0x01;
            }
#endif
#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)
            else if (char_UUID == PERIPHERAL_PRIVACY_FLAGS)
            {
                if (*p_attrib_value == 0x01)
                {
                    // Enable Write of the Reconnection Address
                    GATTserver_DB[GATTserver_DB_Reconnection_Address_Index].permissions |= GATT_PERMISSION_WRITE;
                }
                else
                {
                    // Disable Write of the Reconnection Address
                    GATTserver_DB[GATTserver_DB_Reconnection_Address_Index].permissions &= (~GATT_PERMISSION_WRITE);
                }
            }
#endif


            {
                if (length > value_len)
                {
                    length = value_len;
                }

                for (j=0;j<length;j++)
                {
                    p_value[j] = p_attrib_value[j];
                }
                for (j=length;j<value_len;j++)
                {
                    p_value[j]=0x00;
                }
            }
            //       if (char_UUID == TX_POWER_LEVEL)
            //           {
            //                 t_Att_pdu_params  pdu_params;
            //                   t_Att_pdu_params* p_pdu_params = &pdu_params;
            //                   u_int8 opcode;
            //
            //                     p_pdu_params->attribute.handle = char_val_handle;
            //                     p_pdu_params->attribute.p_value = p_attrib_value;
            //                     p_pdu_params->attribute.value_len = length;
            //                   opcode = ATT_HANDLE_VALUE_NOTIFICATION;
            //                   LEatt_Encode_PDU(GATT_Get_Active_Handle() /*connection_handle*/,opcode,p_pdu_params);
            //           }
        }

        // Firstly Find the Client Characteristic Configuration (CCC) - if one exists
        // As Variable - i already is the index to the Characterisic value we scan the remainder of the
        // Characteristic descriptors until we find the CCC. Note the termination condictions of the search
        // are the end of the char descriptor declarations ( i.e UUID = 28XX or last_DB_index)

        i++; // To move one handle over the Characteristic Value.
        while ((GATTserver_DB[i].attrib_UUID.UUID_val != CLIENT_CHARACTERISTIC_CONFIGURATION) && (i<=GATTserver_DB_last_index) &&
               ((GATTserver_DB[i].attrib_UUID.UUID_val & 0xFF00) != 0x2800))//0x2800 Primary Service
        {
            i++;
        }
        if ((i<=GATTserver_DB_last_index) && (GATTserver_DB[i].attrib_UUID.UUID_val == CLIENT_CHARACTERISTIC_CONFIGURATION))
        {
            extern t_LE_Config LE_config;
            // We have found our CCC
            if (LE_config.num_le_links /* Connection Active */)
            {
                u_int8* p_config_value = GATTserver_DB[i].attrib_value.value.p_value;
                u_int16 config_value = ((u_int16)p_config_value[0] + ((u_int16) p_config_value[1] << 8));

                t_Att_pdu_params  pdu_params;
                t_Att_pdu_params* p_pdu_params = &pdu_params;


                if ((config_value == 0x01 /* Notify */) ||
                    (config_value == 0x02 /* Indicate - wait for Ack */))//master use Att write Client charcteristic configuration to set notify or indicate
                {
                    u_int8 opcode;

                    p_pdu_params->attribute.handle = char_val_handle;
                    p_pdu_params->attribute.p_value = p_attrib_value;
                    p_pdu_params->attribute.value_len = length;

                    if (config_value == 0x01 /* Notify */)
                        opcode = ATT_HANDLE_VALUE_NOTIFICATION;
                    else
                        opcode = ATT_HANDLE_VALUE_INDICATION;

                    // This is the only case where the PDU encoding is performed within
                    // the GATT server.
                    LEatt_Encode_PDU(16 /*connection_handle*/,opcode,p_pdu_params);

                    if (config_value == 0x02 /* Indication */)
                        return GATT_INDICATION_PENDING;
                }
            }
            else /* No Connection Active */
            {
                /* Here I store the Handle in an array of pending indications.
                 * When the link is re-established the Application should call "service_pending_indications"
                 * Which will cycle through any pending indications or notifications.
                 */
                if (GATTserver_pending_Indications_Index < GATT_SERVER_MAX_PENDING_INDICATIONS)
                    GATTserver_Pending_Indications[GATTserver_pending_Indications_Index++] = char_val_handle;
            }
        }

    }
	return GATT_NO_ERROR;
}

/*****************************************************************************************************
 * _GATTserver_Service_Pending_Indications
 *
 * This function is used to service any pending indications when a link is re-established.
 *
 *****************************************************************************************************/
t_GATT_error _GATTserver_Service_Pending_Indications(void)
{
    u_int16 i;


    for (i=0;i<GATTserver_pending_Indications_Index;i++)
    {
        u_int16 j=0;
        u_int16 char_UUID;

        // First get the first handle
        u_int16 char_val_handle = GATTserver_Pending_Indications[i];

        // Find the Data associated with the handle.
        while ((GATTserver_DB[j].attrib_handle != char_val_handle) && (j<=GATTserver_DB_last_index))
        {
            j++;
        }

        char_UUID = GATTserver_DB[j].attrib_UUID.UUID_val;

        if (GATTserver_DB[j].attrib_handle != char_val_handle)
        {
			return GATT_ERROR_ATTRIBUTE_NOT_FOUND;
        }
        else if ((char_UUID >= 0x2800) && (char_UUID <= 0x2803))
        {
			return GATT_ERROR_INVALID_HANDLE;
        }
        else
        {
            t_attrib* p_current_attrib = &GATTserver_DB[i];

            u_int8 value_len = p_current_attrib->attrib_value.value.max_len;
            u_int8* p_value = p_current_attrib->attrib_value.value.p_value;

            GATTserver_Characteristic_Write_Local_Value(char_val_handle, value_len, p_value);
        }
    }
    GATTserver_pending_Indications_Index = 0;

	return GATT_NO_ERROR;
}

/*****************************************************************************************************
 * _GATTserver_Find_End_Group_Handle
 *
 * This is a support function used to find the end handle of a PRIMARY or SECONDARY service definition
 *
 * INPUT
 * u_int16 current_index   :- The index into the database from which the search for the end handle should begin
 *****************************************************************************************************/
u_int16 _GATTserver_Find_End_Group_Handle(u_int16 current_index)
{
    u_int16 end_group_handle;

    current_index++;

    while((current_index <= GATTserver_DB_last_index) &&
          (GATTserver_DB[current_index].attrib_UUID.UUID_val != GATT_ATT_TYPE_PRIMARY_SERVICE) && (GATTserver_DB[current_index].attrib_UUID.UUID_val != GATT_ATT_TYPE_SECONDARY_SERVICE))
    {
        current_index++;
    }

    end_group_handle = GATTserver_DB[current_index-1].attrib_handle;

    return end_group_handle;
}
#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)
/*****************************************************************************************************
 * GATTserver_Is_Privacy_Flag_Enabled
 *
 * Simple function used to return the setting of the PERIPHERAL_PRIVACY_FLAGS in the Sever DB
 *
 *****************************************************************************************************/

u_int8 GATTserver_Is_Privacy_Flag_Enabled()
{
#if 0
    if (GATTserver_DB_Peripheral_Privacy_Flag_Index == 0)
    {
        return 0;
    }
    else
    {
        return *(GATTserver_DB[GATTserver_DB_Peripheral_Privacy_Flag_Index].attrib_value.value.p_value);
    }
#else
    return GATTprivacy_Flags[0];
#endif
}
#endif

#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)

/*****************************************************************************************************
 * GATTserver_Find_Reconnection_Address
 *
 * Simple function used to find the RECONNECTION_ADDRESS (if one is present) in the Server DB
 *
 *****************************************************************************************************/

u_int8* GATTserver_Find_Reconnection_Address()
{
    if (!reconnection_addr_valid)
    {
        return 0;
    }
    else
    {
        return GATTserver_DB[GATTserver_DB_Reconnection_Address_Index].attrib_value.value.p_value;
    }
}

#endif

#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_PREPARE_WRITE==1)
/*****************************************************************************************************
 * GATTserver_Find_Reconnection_Address
 *
 * Simple function used to determine if a specific handle in the Server DB can be written.
 * Only required to support the ATT_PREPARED_WRITE capability
 *
 *****************************************************************************************************/

u_int8 GATTserver_Can_I_Write(u_int16 handle)
{
    u_int16 i;
    u_int8 status;
    t_attrib* p_current_attrib;

    // First Find the index for the handle
    status = GATTserver_Find_DB_Index(handle,0xFFFF,&i);
    if (status != NO_ERROR)
        return status;

    p_current_attrib = &GATTserver_DB[i];

    return GATTserver_Check_Permissions(p_current_attrib,GATT_WRITE);

}
#endif

#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_LOCAL_MODIFICATION_OF_PERMISSIONS==1)
/*****************************************************************************************************
 * GATTserver_Write_Permissions
 *
 * This is used modify the permissions on an attribute handle. This is normally only
 * required for conformance tests (to allow permissions to be changed without re-building SW)
 *****************************************************************************************************/
u_int8 GATTserver_Write_Permissions(u_int16 handle,u_int8 permissions)
{
    u_int16 i=0;
    u_int8 status;

    status = GATTserver_Find_DB_Index(handle,0xFFFF,&i);
    if (status == NO_ERROR)
        GATTserver_DB[i].permissions = permissions;

    return status;
}
#endif
/*****************************************************************************************************
 * GATTserver_Check_Permissions
 *
 * Simple function used to check the permissions on a given attribute. This function is compliant with
 * the permissions as defined in Core Specification Ammendment 3  (CSA-3)
 *
 * INPUTS
 * t_attrib*  p_current_attrib  :- pointer to the attribute whose permissions are to be checked.
 * u_int8     action            :- The action the permissions are to be checked against.
 *                                  ( GATT_READ, GATT_WRITE )
 *
 *****************************************************************************************************/

t_GATT_error GATTserver_Check_Permissions(t_attrib* p_current_attrib,u_int8 action)
{
    if ((action == GATT_READ) && (!(p_current_attrib->permissions & GATT_PERMISSION_READ)))
        return GATT_ERROR_READ_NOT_PERMITTED;

    if ((action == GATT_WRITE) && (!(p_current_attrib->permissions & GATT_PERMISSION_WRITE)))
        return GATT_ERROR_WRITE_NOT_PERMITTED;

    // Check if authentication required
    if (p_current_attrib->permissions & GATT_PERMISSION_AUTHENTICATED)
    {
        /* if such a request is issued when the physical link is unauthenticated
         * or unencrypted, the server shall send an Error Response with the status code
         * set to Insufficient Authentication.
         */
         #if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
        if(!LEsmp_Check_Is_Link_Authenticated(GATT_Get_Active_Handle())) // ATT_config.connection_handle))
            return GATT_ERROR_INSUFFICIENT_AUTHENTICATION;
		#endif

    }
    if (p_current_attrib->permissions & GATT_PERMISSION_AUTHORISED)
    {
        if(!GATTserver_Check_Is_Service_Authorised())
            return GATT_ERROR_INSUFFICIENT_AUTHORISATION;
    }

    if (p_current_attrib->permissions & 0xF0 /* Check if Encryption is needed - Top nibble of permissions used for Encry Key */)
    {
		t_GATT_error status = GATTserver_Check_Encryption_Key_Size(p_current_attrib->permissions);

        return status;
    }
	return GATT_NO_ERROR;
}


u_int8 GATTserver_UUID_Match(t_UUID* attrib_UUID,u_int8 attrib_len,t_attrib_UUID* p_att_UUID)
{
    if (attrib_len != p_att_UUID->len)
    {
        return 0;
    }
    else if (attrib_len == 0x02)
    {
        if (attrib_UUID->UUID_val != p_att_UUID->UUID_val)
        {
            return 0;
        }
    }
    else if (attrib_len == 0x10)
    {
        u_int8 i;

        for(i=0;i<16;i++)
        {
            if (attrib_UUID->p_UUID[i] != p_att_UUID->p_UUID[i])
                return 0;
        }
    }
    return 1;
}

#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_PREPARE_WRITE==1)
/*****************************************************************************************************
 * GATTserver_Get_Value_Length
 *
 * Returns the maximum length of an attribute value.
 *
 * ONLY required if PREPARED_WRITE is supported
 *****************************************************************************************************/
u_int8 GATTserver_Get_Value_Length(u_int16 handle)
{
    u_int16 i = 0;

    // First Find the index for the handle
    GATTserver_Find_DB_Index(handle,0xFFFF,&i);
    return GATTserver_DB[i].attrib_value.value.max_len;
}


#endif

/*****************************************************************************************************
 * GATTserver_Get_Reconnection_Address_Handle
 *
 * Simple function which returns the handle of the Reconnection Address, if available
 *****************************************************************************************************/

u_int16 GATTserver_Get_Reconnection_Address_Handle(void)
{
    return GATT_reconnection_address_handle;
}

/*****************************************************************************************************
 * _GATTserver_Check_Space_Available
 *
 * Simple function which checks is space is available for a new attribute.
 *****************************************************************************************************/


u_int8 _GATTserver_Check_Space_Available(u_int16 GATTserver_DB_next_index)
{

    return ((GATTserver_DB_next_index+1) <  MAX_GATT_SERVER_NUM_ENTRIES);
}

/***************************************
 * TO DO - TO BE COMPLETED
 ***************************************/
u_int8 GATTserver_Check_Is_Service_Authorised(void)
{
    return 0;
}
/*******************************************************************************
 * Function
 * GATTserver_Check_Encryption_Key_Size
 *
 * Description
 * Checks if the current key size is sufficient to access an attribute in the
 * GATT server database. The required key size is stored in the upper nibble of the
 * permissions.
 ********************************************************************************/

t_GATT_error GATTserver_Check_Encryption_Key_Size(u_int8 permissions)
{
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)

    u_int8 min_key_size = (permissions/16)+1;

	return (t_GATT_error) LEsmp_Check_Key_Size(GATT_Get_Active_Handle(),min_key_size);
#else
    return GATT_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE;
#endif
}

#endif

#endif

#endif
