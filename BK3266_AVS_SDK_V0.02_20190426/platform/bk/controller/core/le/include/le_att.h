#ifndef LE_ATT_H_
#define LE_ATT_H_
/*********************************************************************
 * MODULE NAME:     le_att.h
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
/*************************************************************///merge CEVA 0517
/* GATT Procedures and Subprocedures *///merge CEVA 0517
/*************************************************************///merge CEVA 0517
#define GATT_IDLE                                        0x00
#define GATT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY         0x01
#define GATT_CHARACTERISTIC_DISCOVERY                    0x02
#define GATT_CHARACTERISTIC_VALUE_READ                   0x03
#define GATT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS     0x04
#define GATT_DIS_ALL_CHARS_OF_SERVICE                    0x05//merge CEVA 0517
#define GATT_DISCOVER_ALL_SERVICES                       0x06
#define GATT_DISCOVER_CHARACTERISTICS_BY_UUID            0x07
#define GATT_DISCOVER_SPECIFIC_SERVICE                   0x08
#define GATT_DISCOVER_PRIMARY_SERVICE                    0x09
#define GATT_FIND_INCLUDED_SERVICES                      0x0A
#define GATT_READ_CHARACTERISTIC_VALUE                   0x0B
#define GATT_READ_LONG_CHARACTERISTIC_VALUES             0x0C
#define GATT_READ_MULTIPLE_CHARACTERISTIC_VALUES         0x0D
#define GATT_READ_USING_CHARACTERISTIC_UUID              0x0E
#define GATT_RELATIONSHIP_DISCOVERY                      0x0F
#define GATT_WRITE_CHARACTERISTIC_VALUE                  0x10//merge CEVA 0517
#define GATT_WRITE_WITHOUT_RESPONSE                      0x11//merge CEVA 0517
#define GATT_SIGNED_WRITE_WITHOUT_RESPONSE               0x12//merge CEVA 0517
#define GATT_WRITE_CHARACTERISTIC                        0x13//merge CEVA 0517
#define GATT_WRITE_LONG_CHARACTERISTIC                   0x14//merge CEVA 0517
#define GATT_CHARACTERISTIC_VALUE_NOTIFICATION           0x15//merge CEVA 0517
#define GATT_CHARACTERISTIC_VALUE_INDICATION             0x16//merge CEVA 0517
#define GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITE         0x17//merge CEVA 0517
#define GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_SHORT  0x18//merge CEVA 0517
#define GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_LONG   0x19//merge CEVA 0517
#define GATT_WRITE_CHARACTERISTIC_DESCRIPTOR_VALUE       0x1A//merge CEVA 0517
#define GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_SHORT      0x1B//merge CEVA 0517
#define GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_LONG       0x1C//merge CEVA 0517

/*************************************************************
 * GATT Permissions
 *************************************************************/

#define GATT_PERMISSION_READ                 0x01//merge CEVA 0517
#define GATT_PERMISSION_WRITE                0x02//merge CEVA 0517
#define GATT_PERMISSION_AUTHENTICATED        0x04//merge CEVA 0517
#define GATT_PERMISSION_AUTHORISED           0x08//merge CEVA 0517
#define GATT_PERMISSION_ENCRYPTED            0xF0 /* TP/SEC/AUT/BV-15-C GAP layer */
// For future release
//#define GATT_PERMISSION_WRITE                0x10
//#define GATT_PERMISSION_WRITE_AUTHENTICATED  0x20
//#define GATT_PERMISSION_WRITE_AUTHORISED     0x40



#define GATT_MAX_LONG_WRITE                              100//merge CEVA 0517

/*
 * ATT Error Codes
 */

typedef enum {

	LE_ATT_INVALID_HANDLE = 0x01,
	LE_ATT_READ_NOT_PERMITED = 0x02,
	LE_ATT_WRITE_NOT_PERMITED = 0x03,
	LE_ATT_INVALID_PDU = 0x04,
	LE_ATT_INSUFFICIENT_AUTHENTICATION = 0x05,
	LE_ATT_REQUEST_NOT_SUPPORTED = 0x06,
	LE_ATT_INVALID_OFFSET = 0x07,
	LE_ATT_INSUFFICIENT_AUTHORISATION = 0x08,
	LE_ATT_PREPARE_QUEUE_FULL = 0x09,
	LE_ATT_ATTRIBUTE_NOT_FOUND = 0x0A,
	LE_ATT_ATTRIBUTE_NOT_LONG = 0x0B,
	LE_ATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE =0x0C,
	LE_ATT_INVALID_ATTRIBUTE_LENGTH = 0x0D,
	LE_ATT_UNLIKELY_ERROR = 0x0E,
	LE_ATT_INSUFFICIENT_ENCRYPTION = 0x0F,
	LE_ATT_UNSUPPORTED_GROUP = 0X10,
	LE_ATT_INSUFFICIENT_RESUORCES = 0X11
} t_ATT_error_code;


typedef struct t_ATT_config
{
	u_int8 current_procedure;
	u_int8 sub_procedure;
#if (PRH_BS_CFG_SYS_LE_ATT_INCLUDED==1)//merge CEVA 0517
	u_int8 MTU_LEN;//merge CEVA 0517
	u_int8 UUID_len;//merge CEVA 0517
	u_int8 pending_ATT_opcode;//merge CEVA 0517
	u_int8 protocol;//merge CEVA 0517
	u_int16 start_handle;
	u_int16 end_handle;
	u_int16 primary_service_UUID;
	u_int16 characteristic_Handle;//merge CEVA 0517
	u_int8  value[PRH_BS_CFG_SYS_LE_CLIENT_GATT_MAX_LONG_WRITE];//merge CEVA 0517
	u_int16 value_offset;//merge CEVA 0517
	u_int16 value_len;//merge CEVA 0517
	u_int8 prepared_write_pending;//merge CEVA 0517
	u_int8 last_prepare_write_error;//merge CEVA 0517
	u_int8 UUID[16];//merge CEVA 0517
	u_int16 characteristic_16bit_UUID;//merge CEVA 0517
	u_int16 connection_handle;//merge CEVA 0517
#endif
} t_ATT_config;
/*
 * ATT Attribute Opcodes
 */

typedef enum {

	ATT_ERROR_RESPONSE                  = 0x01,
	ATT_EXCHANGE_MTU_REQUEST            = 0x02,
	ATT_EXCHANGE_MTU_RESPONSE           = 0x03,
	ATT_FIND_INFORMATION_REQUEST        = 0x04,
	ATT_FIND_INFORMATION_RESPONSE       = 0x05,
	ATT_FIND_BY_TYPE_VALUE_REQUEST      = 0x06,
	ATT_FIND_BY_TYPE_VALUE_RESPONSE     = 0x07,
	ATT_READ_BY_TYPE_REQUEST            = 0x08,
	ATT_READ_BY_TYPE_RESPONSE           = 0x09,
	ATT_READ_REQUEST                    = 0x0A,
	ATT_READ_RESPONSE                   = 0x0B,
	ATT_READ_BLOB_REQUEST               = 0x0C,
	ATT_READ_BLOB_RESPONSE              = 0x0D,
	ATT_READ_MULTIPLE_REQUEST           = 0x0E,
	ATT_READ_MULTIPLE_RESPONSE          = 0x0F,
	ATT_READ_BY_GROUP_TYPE_REQUEST      = 0x10,
	ATT_READ_BY_GROUP_TYPE_RESPONSE     = 0x11,
	ATT_WRITE_REQUEST                   = 0x12,
	ATT_WRITE_RESPONSE                  = 0x13,
	ATT_PREPARE_WRITE_REQUEST           = 0x16,
	ATT_PREPARE_WRITE_RESPONSE          = 0x17,
	ATT_EXECUTE_WRITE_REQUEST           = 0x18,
	ATT_EXECUTE_WRITE_RESPONSE          = 0x19,
	ATT_HANDLE_VALUE_NOTIFICATION       = 0x1B,
	ATT_HANDLE_VALUE_INDICATION         = 0x1D,
	ATT_HANDLE_VALUE_CONFIRMATION       = 0x1E,
	ATT_WRITE_COMMAND                   = 0x52,
	ATT_SIGNED_WRITE_COMMAND            = 0xD2

} t_ATT_Attribute_Opcode;


typedef struct t_ATT_attribute {

	u_int16 handle;
	u_int16 permissions;

}t_ATT_attribute;


/*
 * GATT UUIDs for Characteristics Descriptions
 */
#define ALERT_CATEGORY_ID					0x2A43
#define ALERT_CATEGORY_ID_BIT_MASK 			0x2A42
#define ALERT_LEVEL	 						0x2A06
#define ALERT_NOTIFICATION_CONTROL_POINT 	0x2A44
#define ALERT_STATUS						0x2A3F
#define APPERANCE							0x2A01
#define BATTERY_LEVEL						0x2A19
#define BLOOD_PRESSURE_FEATURE				0x2A49
#define BLOOD_PRESSURE_MEASUREMENT			0x2A35
#define BODY_SENSOR_LOCATION				0x2A38


#define DATA_TIME							0x2A08
#define DAY_DATE_TIME						0x2A0A
#define DAY_OF_WEEK 						0x2A09
#define DEVICE_NAME							0x2A00
#define DST_OFFSET							0x2A0D
#define EXACT_TIME_256						0x2A0C
#define FIRMWARE_REVISION_STRING			0x2A26
#define HARDWARE_REVISION_STRING			0x2A27
#define HEART_RATE_CONTROL_POINT			0x2A39
#define HEART_RATE_MEASUREMENT				0x2A37

#define IEEE_11073_20601_REGULATORY_CERTIFICATION_DATA_LIST	0x2A2A
#define INTERMEDIATE_CUFF_PRESSURE			0x2A36
#define INTERMEDIATE_TEMPERATURE			0x2A1E
//#define LONG_TIMER_INFORMATION		    0x2A0F//merge CEVA 0517
#define MANUFACTURER_NAME_STRING			0x2A29
#define MEASUREMENT_INTERVAL				0x2A21
#define MODEL_NUMBER_STRING					0x2A24
#define NEW_ALERT							0x2A46
#define PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS	0x2A04
#define PERIPHERAL_PRIVACY_FLAGS					0x2A02
#define PNP_ID										0x2A50
#define GATT_CHAR_PROTOCOL_MODE						0x2A4E//merge CEVA 0517
#define RECONNECTION_ADDRESS						0x2A03
//#define REFERENCE_TIME_INFORMATION				0x2A14//merge CEVA 0517
#define REPORT										0x2A4D
#define REPORT_MAP									0x2A4B
#define RINGER_CONTROL_POINT						0x2A40
#define RINGER_SETTING								0x2A41
#define SCAN_INTERVAL_WINDOW						0x2A4F
#define SCAN_REFRESH								0x2A31
#define SERIAL_NUMBER_STRING						0x2A25
#define SERVICE_CHANGED								0x2A05
#define SOFTWARE_REVISION_STRING					0x2A28
#define SUPPORTED_NEW_ALERT_CATEGORY				0x2A47
#define SUPPORTED_UNREAD_ALERT_CATEGORY				0x2A48
#define SYSTEM_ID									0x2A23
#define TEMPERATURE_MEASUREMENT						0x2A1C
#define TEMPERATURE_TYPE 							0x2A1D
#define TIME_ACCURACY								0x2A12
#define TIME_SOURCE									0x2A13
#define GATT_CHAR_TIME_UPDATE_CONTROL_POINT			0x2A16//merge CEVA 0517
#define GATT_CHAR_TIME_UPDATE_STATE					0x2A17//merge CEVA 0517
#define GATT_CHAR_TIME_WITH_DST						0x2A11//merge CEVA 0517
#define TIME_ZONE									0x2A0E
#define TX_POWER_LEVEL								0x2A07
#define UNREAD_ALERT_STATUS							0x2A45



/*
 * GATT UUIDs for Characteristics Descriptors
 *
 * GATT Characteristic Descriptors
 * «Characteristic Extended Properties?	 	0x2900
 * «Characteristic User Description?		 	0x2901
 * «Client Characteristic Configuration?	 	0x2902
 * «Server Characteristic Configuration?	 	0x2903
 * «Characteristic Format?				 		0x2904
 * «Characteristic Aggregate Format?	 		0x2905
 */
#define CHARACTERISTIC_EXTENDED_PROPERTIES	0x2900
#define CHARACTERISTIC_USER_DESCRIPTION		0x2901
#define CLIENT_CHARACTERISTIC_CONFIGURATION	0x2902
#define SERVER_CHARACTERISTIC_CONFIGURATION	0x2903
#define CHARACTERISTIC_PRESENTATION_FORMAT	0x2904
#define CHARACTERISTIC_AGGREGATE_FORMAT		0x2905


#define VALID_RANGE							0x2906
#define EXTERNAL_REPORT_REFERENCE			0x2907
#define REPORT_REFERENCE					0x2908


/*
 * GATT -  Characteristic Types
 *
 * «Device Name?					 			0x2A00
 * «Appearance?	 							0x2A01
 * «Peripheral Privacy Flag?	 				0x2A02
 * «Reconnection Address?	 					0x2A03
 * «Peripheral Preferred Connection Parameters?0x2A04
 * «Service Changed?	 						0x2A05
 */

#define DEVICE_NAME								0x2A00
#define APPERANCE                               0x2A01
#define PERIPHERAL_PRIVACY_FLAG                 0x2A02
#define RECONNECTION_ADDRESS                    0x2A03
#define PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS 0x2A04
#define SERVICE_CHANGED                         0x2A05
#define GATT_CHAR_ALERT_LEVEL                   0x2A06//merge CEVA 0517

#define GATT_CHAR_LOCAL_TIME                    0x2A0F//merge CEVA 0517
#define GATT_CHAR_REFERENCE_TIME                0x2A14//merge CEVA 0517
#define GATT_CHAR_SYSTEM_ID                     0x2A23//merge CEVA 0517
#define GATT_CHAR_MODEL_NUMBER                  0x2A24//merge CEVA 0517
#define GATT_CHAR_SERIAL_NUMBER			        0x2A25//merge CEVA 0517
#define GATT_CHAR_FW_REVISION			        0x2A26//merge CEVA 0517
#define GATT_CHAR_HW_REVISION			        0x2A27//merge CEVA 0517
#define GATT_CHAR_SW_REVISION			        0x2A28//merge CEVA 0517
#define GATT_CHAR_MANUFACTURER_NAME             0x2A29//merge CEVA 0517
#define GATT_CHAR_REG_CERT                      0x2A2A//merge CEVA 0517
#define GATT_CHAR_CURRENT_TIME					0x2A2B//merge CEVA 0517

#define GATT_CHAR_BOOT_KEYBOARD_INPUT_REPORT			0x2A22//merge CEVA 0517
#define GATT_CHAR_BOOT_KEYBOARD_OUTPUT_REPORT			0x2A32//merge CEVA 0517
#define GATT_CHAR_BOOT_MOUSE_INPUT_REPORT				0x2A33//merge CEVA 0517

#define GATT_CHAR_HID_INFORMATION				0x2A4A//merge CEVA 0517
#define GATT_CHAR_REPORT_MAP					0x2A4B//merge CEVA 0517
#define GATT_CHAR_HID_CONTROL_POINT				0x2A4C//merge CEVA 0517

#define GATT_CHAR_REPORT						0x2A4D//merge CEVA 0517
#define GATT_CHAR_PROTOCOL_MODE					0x2A4E//merge CEVA 0517
#define GATT_CHAR_PnP_ID                        0x2A50//merge CEVA 0517

#define GATT_CHAR_HEART_RATE_MEASUREMENT        0x2A37//merge CEVA 0517
#define GATT_CHAR_HEART_RATE_SENSOR_LOCATION    0x2A38//merge CEVA 0517
#define GATT_CHAR_HEART_RATE_CONTROL_POINT      0x2A39//merge CEVA 0517

#define GATT_CHAR_TEMP_MEASUREMENT              0x2A1C//merge CEVA 0517
#define GATT_CHAR_TEMPERATURE_TYPE              0x2A1D//merge CEVA 0517
#define GATT_CHAR_INT_TEMP_MEASUREMENT          0x2A1E//merge CEVA 0517
#define GATT_CHAR_MEASUREMENT_INTERVAL			0x2A21//merge CEVA 0517

#define GATT_CHAR_SENSOR_LOCATION			    0x2A5D//merge CEVA 0517
#define GATT_CHAR_SC_CONTROL_POINT              0x2A55//merge CEVA 0517

#define GATT_CHAR_CSC_MEASUREMENT               0x2A5B//merge CEVA 0517
#define GATT_CHAR_CSC_FEATURES_SUPPORTED		0x2A5C//merge CEVA 0517

#define GATT_CHAR_RSC_MEASUREMENT               0x2A53//merge CEVA 0517
#define GATT_CHAR_RSC_FEATURES_SUPPORTED		0x2A54//merge CEVA 0517


#define GATT_CHAR_BP_MEASUREMENT                0x2A35//merge CEVA 0517
#define GATT_CHAR_BP_INT_CUFF_MEASUREMENT       0x2A36//merge CEVA 0517
#define GATT_CHAR_BP_FEATURES_SUPPORTED         0x2A49//merge CEVA 0517

#define GATT_CHAR_BG_MEASUREMENT                0x2A18//merge CEVA 0517
#define GATT_CHAR_BG_MEASUREMENT_CONTEXT        0x2A34//merge CEVA 0517
#define GATT_CHAR_BG_FEATURES_SUPPORTED         0x2A51//merge CEVA 0517
#define GATT_CHAR_BG_CONTROL_POINT              0x2A52//merge CEVA 0517


/*
 * GATT Services
 */
#define GENERIC_ACCESS_PROFILE 		0x1800
#define GENERIC_ATTRIBUTE_PROFILE 	0x1801
#define IMMEDIATE_ALERT             0x1802//merge CEVA 0517
#define LINK_LOSS                   0x1803//merge CEVA 0517
#define TX_POWER                    0x1804//merge CEVA 0517
#define CURRENT_TIME                0x1805//merge CEVA 0517
#define REFERENCE_TIME_UPDATE       0x1806//merge CEVA 0517
#define NEXT_DST_CHANGE             0x1807//merge CEVA 0517
#define HEALTH_THERMOMETER          0x1809//merge CEVA 0517
#define DEVICE_INFORMATION          0x180A//merge CEVA 0517
#define HEART_RATE                  0x180D//merge CEVA 0517
#define BATTERY_SERVICE             0x180F//merge CEVA 0517
#define HUMAN_INTERFACE_DEVICE      0x1812//merge CEVA 0517
#define SCAN_PARAMETERS             0x1813//merge CEVA 0517
#define CYCLING_SPEED_AND_CADENCE   0x1816//merge CEVA 0517
#define RUNNING_SPEED_AND_CADENCE   0x1814//merge CEVA 0517
#define BLOOD_PRESSURE              0x1810//merge CEVA 0517
#define BLOOD_GLUCOSE               0x1808//merge CEVA 0517


/*
 * GATT Format Types - used in Characteristic Format Presentation////merge CEVA 0517
 */

typedef enum {
	FORMAT_BOOLEAN       = 0x01,//merge CEVA 0517
	FORMAT_2BIT_UINT     = 0x02,//merge CEVA 0517
	FORMAT_4BIT_UINT     = 0x03,//merge CEVA 0517
	FORMAT_8BIT_UINT     = 0x04,//merge CEVA 0517
	FORMAT_12BIT_UINT    = 0x05,//merge CEVA 0517
	FORMAT_16BIT_UINT    = 0x06,//merge CEVA 0517
	FORMAT_24BIT_UINT    = 0x07,//merge CEVA 0517
	FORMAT_32BIT_UINT    = 0x08,//merge CEVA 0517
	FORMAT_48BIT_UINT    = 0x09,//merge CEVA 0517
	FORMAT_64BIT_UINT    = 0x0A,//merge CEVA 0517
	FORMAT_128BIT_UINT   = 0x0B,//merge CEVA 0517
	FORMAT_8BIT_SINT     = 0x0C,//merge CEVA 0517
	FORMAT_12BIT_SINT    = 0x0D,//merge CEVA 0517
	FORMAT_16BIT_SINT    = 0x0E,//merge CEVA 0517
	FORMAT_24BIT_SINT    = 0x0F,//merge CEVA 0517
	FORMAT_32BIT_SINT    = 0x10,//merge CEVA 0517
	FORMAT_48BIT_SINT    = 0x11,//merge CEVA 0517
	FORMAT_64BIT_SINT    = 0x12,//merge CEVA 0517
	FORMAT_128BIT_SINT   = 0x13,//merge CEVA 0517
	FORMAT_32FLOAT       = 0x14,//merge CEVA 0517
	FORMAT_64FLOAT       = 0x15,//merge CEVA 0517
	FORMAT_SFLOAT        = 0x16,//merge CEVA 0517
	FORMAT_FLOAT         = 0x17,//merge CEVA 0517
	FORMAT_DUINT16       = 0x18,//merge CEVA 0517
	FORMAT_UTF8S         = 0x19,//merge CEVA 0517
	FORMAT_UTF16S        = 0x1A,//merge CEVA 0517
	FORMAT_OPAQUE_STRUCT = 0x1B//merge CEVA 0517
} t_char_format;
/*
 * GATT Attribute Types
 */

#define GATT_ATT_TYPE_PRIMARY_SERVICE 			    0x2800
#define GATT_ATT_TYPE_SECONDARY_SERVICE 			0x2801
#define GATT_ATT_TYPE_INCLUDE						0x2802
#define GATT_ATT_TYPE_CHARACTERISTIC		 	 	0x2803
#define GATT_ATT_TYPE_DEVICE_NAME       0x2A00

/*
 * GATT Propertiese//merge CEVA 0517
 */

#define GATT_PROPERTIES_BROADCAST       		0x01//merge CEVA 0517
#define GATT_PROPERTIES_READ            		0x02//merge CEVA 0517
#define GATT_PROPERTIES_WRITE_WITHOUT_RESPONSE 	0x04//merge CEVA 0517
#define GATT_PROPERTIES_WRITE                   0x08//merge CEVA 0517
#define GATT_PROPERTIES_NOTIFY                  0x10//merge CEVA 0517
#define GATT_PROPERTIES_INDICATE                0x20//merge CEVA 0517
#define GATT_PROPERTIES_AUTH_SIGNED_WRITE       0x40//merge CEVA 0517
#define GATT_PROPERTIES_EXTENDED_PROPERTIES     0x80//merge CEVA 0517


/*
 * ATT Errors
 */

enum GATT_error_codes
{
GATT_NO_ERROR = 0,
GATT_ERROR_INVALID_HANDLE = 0x01,//merge CEVA 0517
GATT_ERROR_READ_NOT_PERMITTED,          //0x02//merge CEVA 0517
GATT_ERROR_WRITE_NOT_PERMITTED,         //0x03//merge CEVA 0517
GATT_ERROR_INVALID_PDU,                 //0x04//merge CEVA 0517
GATT_ERROR_INSUFFICIENT_AUTHENTICATION, //0x05//merge CEVA 0517
GATT_ERROR_REQUEST_NOT_SUPPORTED,       //0x06//merge CEVA 0517
GATT_ERROR_INVALID_OFFSET,              //0x07//merge CEVA 0517
GATT_ERROR_INSUFFICIENT_AUTHORISATION,  //0x08//merge CEVA 0517
GATT_ERROR_PREPARE_QUEUE_FULL,          //0x09//merge CEVA 0517
GATT_ERROR_ATTRIBUTE_NOT_FOUND,         //0x0A//merge CEVA 0517
GATT_ERROR_ATTRIBUTE_NOT_LONG,          //0x0B//merge CEVA 0517
GATT_ERROR_INSUFFICIENT_ENCRYPTION_KEY_SIZE, //0x0C//merge CEVA 0517
GATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH,   //0x0D//merge CEVA 0517
GATT_ERROR_UNLIKELY_ERROR,              //0x0E//merge CEVA 0517
GATT_ERROR_INSUFFICIENT_ENCRYPTION,     //0x0F//merge CEVA 0517
GATT_ERROR_UNSUPPORTED_GROUP_TYPE,      //0x10//merge CEVA 0517
GATT_ERROR_INSUFFICIENT_RESOURCES,      //0x11//merge CEVA 0517
GATT_ERROR_TIMEOUT,                     //0x12//merge CEVA 0517
GATT_ERROR_OUT_OF_RANGE,                //0x13//merge CEVA 0517 NOT in spec define Attribute Protocol (ATT) chapter,3.4 ATTRIBUTE PROTOCOL PDUS
GATT_INDICATION_PENDING = 0x22,         //NOT in spec define Attribute Protocol (ATT) chapter,3.4 ATTRIBUTE PROTOCOL PDUS
GATT_SERVER_HANDLE_WRITE_RESP=0x99,     //NOT in spec define Attribute Protocol (ATT) chapter,3.4 ATTRIBUTE PROTOCOL PDUS
};
typedef enum GATT_error_codes t_GATT_error ;
// Propietrary GATT error
//#define GATT_INDICATION_PENDING                0x22//merge CEVA 0517


#define GATT_READ                              0x00//merge CEVA 0517
#define GATT_WRITE                             0x01//merge CEVA 0517

/* GATT UUIDs for UNITS *
 */
// http://developer.bluetooth.org/gatt/units/Pages/default.aspx
#define CELSIUS_TEMPERATURE                          0x272F//merge CEVA 0517


typedef struct t_handle_uuid
{
	u_int16 handle;
	u_int16 uuid;
}  t_handle_uuid;

typedef struct t_attribute
{
	u_int16 type;
	u_int16 handle;
	u_int8* p_value;
	u_int8 value_len;
} t_attribute;

typedef struct t_gatt_event_params//merge CEVA 0517
{
	u_int8   event_opcode;//merge CEVA 0517
	u_int16  handle;//merge CEVA 0517
	u_int8   status;//merge CEVA 0517
	u_int8   payload_length;//merge CEVA 0517
	u_int8*  payload;//merge CEVA 0517
	u_int16  att_handle;//merge CEVA 0517
	u_int8   att_len;//merge CEVA 0517
} t_gatt_event_params;//merge CEVA 0517

typedef struct t_gatt_command_complete_params//merge CEVA 0517
{
	u_int8   operation_id;//merge CEVA 0517
	u_int16  handle;//merge CEVA 0517
	u_int8   status;//merge CEVA 0517
	u_int16  mtu_len;//merge CEVA 0517

} t_gatt_command_complete_params;//merge CEVA 0517

typedef struct t_Att_pdu_params {
	u_int8 error_opcode;
	u_int8 error_code;
	u_int16 attribute_handle;
	u_int16 attribute_type;
	u_int16 mtu;
	u_int16 start_handle;
	u_int16 end_handle;

	//merge CEVA 0517 remove u_int8 num_handle_uuid_pairs;//merge CEVA 0517
	//merge CEVA 0517 remove t_handle_uuid handle_uuid_pairs[10];//merge CEVA 0517

	u_int8 attribute_value_len;
	u_int8* attribute_value;

	u_int8 num_handles;
	u_int16 handle[5];//merge CEVA 0517

	u_int16 attribute_type16;

	u_int8 length;
    
    t_attribute attribute;//merge CEVA 0517
	//merge CEVA 0517 remove u_int8 num_attributes;//merge CEVA 0517
	//merge CEVA 0517 remove t_attribute attribute[10];//merge CEVA 0517

	u_int16 value_offset;

	u_int8 flags;
	u_int8 signature[12];

	u_int8 format;
	u_int8* p_result;

	u_int8* p_UUID;//merge CEVA 0517
	u_int8  UUID_len;//merge CEVA 0517

}t_Att_pdu_params;

typedef struct t_attrib_UUID//merge CEVA 0517
{
	u_int8 len;//merge CEVA 0517
	u_int16 UUID_val;//merge CEVA 0517
	u_int8* p_UUID;//merge CEVA 0517
} t_attrib_UUID;//merge CEVA 0517

typedef union t_UUID {//merge CEVA 0517
	u_int16 UUID_val;//merge CEVA 0517
	u_int8* p_UUID;//merge CEVA 0517
} t_UUID;//merge CEVA 0517


#define NO_LOCAL_NOTIFY   0x00//merge CEVA 0517
#define USE_LOCAL_NOTIFY 0x01//merge CEVA 0517

#define PRIMARY_SERVICE_16BIT      0x01//merge CEVA 0517
#define SECONDARY_SERVICE_16BIT    0x02//merge CEVA 0517
#define PRIMARY_SERVICE_128BIT     0x03//merge CEVA 0517
#define SECONDARY_SERVICE_128BIT   0x04//merge CEVA 0517

/*
 * Multi-octet fields within the attribute protocol shall be sent least significant octet
 * first (little endian) with the exception of the Attribute Value field. The endianness
 * of the Attribute Value field is defined by a higher layer specification.
 */


/********************************************************************************************
 *  GATT API
 *******************************************************************************************/
void GATT_init(void);//merge CEVA 0517

void GATT_Register_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr);//merge CEVA 0517
void GATT_DeRegister_Callbacks(void);//merge CEVA 0517

t_error GATT_Discover_Primary_Service(u_int16 connection_handle,u_int8 UUID_type, u_int8* p_UUID, u_int8 protocol_id);//merge CEVA 0517
t_error GATT_Relationship_Discovery(u_int16 connection_handle, u_int16 start_handle, u_int16 end_handle );//merge CEVA 0517
t_error GATT_Characteristic_Discovery(u_int16 connection_handle, u_int8 uuid_type, u_int8* p_UUID, u_int16 start_handle, u_int16 end_handle, u_int8 protocol_id);//merge CEVA 0517
t_GATT_error GATT_Characteristic_Descriptor_Discovery(u_int16 connection_handle,u_int16 start_handle, u_int16 end_handle );//merge CEVA 0517
t_error GATT_Characteristic_Value_Read(u_int16 connection_handle, u_int8 control, u_int8 offset, u_int8* p_UUID,//merge CEVA 0517
		u_int16 start_handle, u_int16 end_handle, u_int8 protocol_id );//merge CEVA 0517
//t_error GATT_Characteristic_Value_Read_Multiple(u_int16 connection_handle,u_int16 handle1,u_int16 handle2,u_int16 handle3,u_int16 handle4,u_int16 handle5);
t_error GATT_Characteristic_Value_Read_Multiple(u_int16 connection_handle,u_int8* p_handle);//merge CEVA 0517
t_error GATT_Characteristic_Value_Write( u_int16 connection_handle, u_int8 write_instruct, u_int8 offset, u_int16 characteristic_value_handle, u_int8 characteristic_len, u_int8* p_characteristic, u_int8 protocol_id );//merge CEVA 0517
t_error GATT_Client_Charactaristic_Descriptor_Write(u_int16 connection_handle,  u_int16 char_desc_value_handle, u_int8 char_desc_value_len, u_int8* p_value );//merge CEVA 0517
t_error GATT_Client_Execute_Write(u_int16 handle,u_int8 flags);//merge CEVA 0517
t_error GATT_Client_Charactaristic_Descriptor_Read(u_int16 connection_handle /*,u_int8 long_val*/, u_int16 char_desc_handle);//merge CEVA 0517

u_int8 GATT_Event(u_int8 event_opcode, u_int16 handle, u_int8 payload_len, u_int8 attrib_len, u_int8* p_pdu,u_int8 status);//merge CEVA 0517
u_int8 GATT_Command_Complete_Event(u_int16 handle,u_int8 operation_id,u_int8 status);//merge CEVA 0517

void Gatt_Set_MTU_Size(u_int8 mtu_size);//merge CEVA 0517

u_int8 GATT_Check_Is_Link_Encrypted(void);//merge CEVA 0517
void GATT_Check_GATT_Timers(void);//merge CEVA 0517
u_int8 GATT_Exchange_MTU(u_int16 handle);//merge CEVA 0517
u_int16 GATT_Get_Active_Handle(void);//merge CEVA 0517
void GATT_Register_GAP_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr);//merge CEVA 0517

void LEatt_Decode_PDU(t_LE_Connection* p_connection,u_int8 mtu_length,u_int8* p_pdu);//merge CEVA 0517
void LEatt_Encode_PDU(u_int16 connection_handle, u_int8 opcode,t_Att_pdu_params* p_pdu_params);//merge CEVA 0517
void GATT_Discover_DEVICE_NAME(void);
#endif

#endif