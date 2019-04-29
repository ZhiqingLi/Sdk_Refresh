/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_HCI_TYPES_H_
#define _BT_HCI_TYPES_H_

#include <bluetooth.h>

/**************************************************************************
 **                         Events and event parameters
 **************************************************************************/

enum hci_error {
    HCI_ERR_SUCCESS = 0x00,                                             //0x00
    HCI_ERR_UNKNOWN_HCI_COMMAND,                                        //0x01
    HCI_ERR_UNKNOWN_CONNECTION_IDENTIFIER,                              //0x02
    HCI_ERR_HARDWARE_FAILURE,                                           //0x03
    HCI_ERR_PAGE_TIMEOUT,                                               //0x04
    HCI_ERR_AUTHENTICATION_FAILURE,                                     //0x05
    HCI_ERR_PIN_OR_KEY_MISSING,                                         //0x06
    HCI_ERR_MEMORY_CAPACITY_EXCEEDED,                                   //0x07
    HCI_ERR_CONNECTION_TIMEOUT,                                         //0x08
    HCI_ERR_CONNECTION_LIMIT_EXCEEDED,                                  //0x09
    HCI_ERR_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED,          //0x0a
    HCI_ERR_ACL_CONNECTION_ALREADY_EXISTS,                              //0x0b
    HCI_ERR_COMMAND_DISALLOWED,                                         //0x0c
    HCI_ERR_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES,               //0x0d
    HCI_ERR_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS,                //0x0e
    HCI_ERR_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR,            //0x0f
    HCI_ERR_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED,                         //0x10
    HCI_ERR_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE,                     //0x11
    HCI_ERR_INVALID_HCI_COMMAND_PARAMETERS,                             //0x12
    HCI_ERR_REMOTE_USER_TERMINATED_CONNECTION,                          //0x13
    HCI_ERR_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES,   //0x14
    HCI_ERR_REMOTE_DEVICE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF,       //0x15
    HCI_ERR_CONNECTION_TERMINATED_BY_LOCAL_HOST,                        //0x16
    HCI_ERR_REPEATED_ATTEMPTS,                                          //0x17
    HCI_ERR_PAIRING_NOT_ALLOWED,                                        //0x18
    HCI_ERR_UNKNOWN_LMP_PDU,                                            //0x19
    HCI_ERR_UNSUPPORTED_REMOTE_OR_LMP_FEATURE,                          //0x1a
    HCI_ERR_SCO_OFFSET_REJECTED,                                        //0x1b
    HCI_ERR_SCO_INTERVAL_REJECTED,                                      //0x1c
    HCI_ERR_SCO_AIR_MODE_REJECTED,                                      //0x1d
    HCI_ERR_INVALID_LMP_PARAMETERS,                                     //0x1e
    HCI_ERR_UNSPECIFIED_ERROR,                                          //0x1f
    HCI_ERR_UNSUPPORTED_LMP_PARAMETER_VALUE,                            //0x20
    HCI_ERR_ROLE_CHANGE_NOT_ALLOWED,                                    //0x21
    HCI_ERR_LMP_RESPONSE_TIMEOUT,                                       //0x22
    HCI_ERR_LMP_ERROR_TRANSACTION_COLLISION,                            //0x23
    HCI_ERR_LMP_PDU_NOT_ALLOWED,                                        //0x24
    HCI_ERR_ENCRYPTION_MODE_NOT_ACCEPTABLE,                             //0x25
    HCI_ERR_LINK_KEY_CAN_NOT_BE_CHANGED,                                //0x26
    HCI_ERR_REQUESTED_QOS_NOT_SUPPORTED,                                //0x27
    HCI_ERR_INSTANT_PASSED,                                             //0x28
    HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED,                        //0x29
    HCI_ERR_DIFFERENT_TRANSACTION_COLLISION,                            //0x2a
    HCI_ERR_RESERVED_0,                                                 //0x2b
    HCI_ERR_QOS_UNACCEPTABLE_PARAMETER,                                 //0x2c
    HCI_ERR_QOS_REJECTED,                                               //0x2d
    HCI_ERR_CHANNEL_CLASSIFICATION_NOT_SUPPORTED,                       //0x2e
    HCI_ERR_INSUFFICIENT_SECURITY,                                      //0x2f
    HCI_ERR_PARAMETER_OUT_OF_MANDATORY_RANGE,                           //0x30
    HCI_ERR_RESERVED_1,                                                 //0x31
    HCI_ERR_ROLE_SWITCH_PENDING,                                        //0x32
    HCI_ERR_RESERVED_2,                                                 //0x33
    HCI_ERR_RESERVED_SLOT_VIOLATION,                                    //0x34
    HCI_ERR_ROLE_SWITCH_FAILED,                                         //0x35
    HCI_ERR_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE,                        //0x36
    HCI_ERR_SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST,                //0x37
    HCI_ERR_HOST_BUSY_PAIRING,                                          //0x38
    HCI_ERR_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND        //0x39
};
#ifdef CONFIG_TWS
#define 	HCI_ERR_CONNECTION_REJECTED_DUE_TO_STEREO_ROLE_SWITCH 0xF0
#endif

typedef uint8_t hci_error_t;

#ifdef J_DEBUG
const char *hci_err_str(hci_error_t hci_err);
#endif

/* HCI Security definitions */
#define HCI_SEC_NEG_PASSKEY 0xFFFFFFFF
#define HCI_SEC_PASSKEY_MAX 0x000F423F

/**************************************************************************
 **                   Common defines and types (HCI)
 **************************************************************************/
#define HCI_PAIRING_SIZE                16

#define HCI_LAP_SIZE                    3   /* unit LAP */
#define HCI_KEY_SIZE                    HCI_PAIRING_SIZE /* link key */
#define HCI_PIN_SIZE                    HCI_PAIRING_SIZE /* link PIN */
#define HCI_EVENT_MASK_SIZE             8   /* event mask */
#define HCI_CLASS_SIZE                  BT_CLASS_SIZE /* unit class */
#define HCI_FEATURES_SIZE               8   /* LMP features */
#define HCI_UNIT_NAME_SIZE              48 //248 /* unit name size */
#define HCI_DEVNAME_SIZE                16  /* same as dv_xname */
#define HCI_COMMANDS_SIZE               64  /* supported commands size */

typedef uint8_t btkey_t[HCI_KEY_SIZE];
typedef uint8_t btpin_t[HCI_PIN_SIZE];

/* HCI specification */
#define HCI_SPEC_V10                    0x00 /* v1.0 */
#define HCI_SPEC_V11                    0x01 /* v1.1 */
#define HCI_SPEC_V12                    0x02 /* v1.2 */
#define HCI_SPEC_V20                    0x03 /* v2.0 */
#define HCI_SPEC_V21                    0x04 /* v2.1 */
/* 0x05 - 0xFF - reserved for future use */

/* LMP features (and page 0 of extended features) */
/* ------------------- byte 0 --------------------*/
#define HCI_LMP_3SLOT                   0x01
#define HCI_LMP_5SLOT                   0x02
#define HCI_LMP_ENCRYPTION              0x04
#define HCI_LMP_SLOT_OFFSET             0x08
#define HCI_LMP_TIMIACCURACY            0x10
#define HCI_LMP_ROLE_SWITCH             0x20
#define HCI_LMP_HOLD_MODE               0x40
#define HCI_LMP_SNIFF_MODE              0x80
/* ------------------- byte 1 --------------------*/
#define HCI_LMP_PARK_MODE               0x01
#define HCI_LMP_RSSI                    0x02
#define HCI_LMP_CHANNEL_QUALITY         0x04
#define HCI_LMP_SCO_LINK                0x08
#define HCI_LMP_HV2_PKT                 0x10
#define HCI_LMP_HV3_PKT                 0x20
#define HCI_LMP_ULAW_LOG                0x40
#define HCI_LMP_ALAW_LOG                0x80
/* ------------------- byte 2 --------------------*/
#define HCI_LMP_CVSD                    0x01
#define HCI_LMP_PAGISCHEME              0x02
#define HCI_LMP_POWER_CONTROL           0x04
#define HCI_LMP_TRANSPARENT_SCO         0x08
#define HCI_LMP_FLOW_CONTROL_LAG0       0x10
#define HCI_LMP_FLOW_CONTROL_LAG1       0x20
#define HCI_LMP_FLOW_CONTROL_LAG2       0x40
#define HCI_LMP_BC_ENCRYPTION           0x80
/* ------------------- byte 3 --------------------*/
/* reserved                             0x01 */
#define HCI_LMP_EDR_ACL_2MBPS           0x02
#define HCI_LMP_EDR_ACL_3MBPS           0x04
#define HCI_LMP_ENHANCED_ISCAN          0x08
#define HCI_LMP_INTERLACED_ISCAN        0x10
#define HCI_LMP_INTERLACED_PSCAN        0x20
#define HCI_LMP_RSSI_INQUIRY            0x40
#define HCI_LMP_EV3_PKT                 0x80
/* ------------------- byte 4 --------------------*/
#define HCI_LMP_EV4_PKT                 0x01
#define HCI_LMP_EV5_PKT                 0x02
/* reserved                             0x04 */
#define HCI_LMP_AFH_CAPABLE_SLAVE       0x08
#define HCI_LMP_AFH_CLASS_SLAVE         0x10
/* reserved                             0x20 */
/* reserved                             0x40 */
#define HCI_LMP_3SLOT_EDR_ACL           0x80
/* ------------------- byte 5 --------------------*/
#define HCI_LMP_5SLOT_EDR_ACL           0x01
#define HCI_LMP_SNIFF_SUBRATING         0x02
#define HCI_LMP_PAUSE_ENCRYPTION        0x04
#define HCI_LMP_AFH_CAPABLE_MASTER      0x08
#define HCI_LMP_AFH_CLASS_MASTER        0x10
#define HCI_LMP_EDR_eSCO_2MBPS          0x20
#define HCI_LMP_EDR_eSCO_3MBPS          0x40
#define HCI_LMP_3SLOT_EDR_eSCO          0x80
/* ------------------- byte 6 --------------------*/
#define HCI_LMP_EXTENDED_INQUIRY        0x01
/* reserved                             0x02 */
/* reserved                             0x04 */
#define HCI_LMP_SIMPLE_PAIRING          0x08
#define HCI_LMP_ENCAPSULATED_PDU        0x10
#define HCI_LMP_ERRDATA_REPORTING       0x20
#define HCI_LMP_NOFLUSH_PB_FLAG         0x40
/* reserved                             0x80 */
/* ------------------- byte 7 --------------------*/
#define HCI_LMP_LINK_SUPERVISION_TO     0x01
#define HCI_LMP_INQ_RSP_TX_POWER        0x02
#define HCI_LMP_EXTENDED_FEATURES       0x80
/* ------- byte 8 (0'byte of extended features ----*/
#define HCI_LMP_HOST_SIMPLE_PAIRING   0x01

typedef uint8_t bt_lmp_features_t[HCI_FEATURES_SIZE]; /* Bytes 0 to 7 */

/* HCI supported commands
 * v2.1 Core Spec, Vol 2, Part E, Section 6.26 */
/* ------------------------ byte 0 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_0                   0
#define HCI_COMMAND_INQUIRY                             0x01
#define HCI_COMMAND_INQUIRY_CANCEL                      0x02
#define HCI_COMMAND_PERIODIC_INQUIRY                    0x04
#define HCI_COMMAND_EXIT_PERIODIC_INQUIRY               0x08
#define HCI_COMMAND_CREATE_CON                          0x10
#define HCI_COMMAND_DISCONNECT                          0x20
#define HCI_COMMAND_ADD_SCO_CON                         0x40
#define HCI_COMMAND_CREATE_CON_CANCEL                   0x80
/* ------------------------ byte 1 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_1                   1
#define HCI_COMMAND_ACCEPT_CON                          0x01
#define HCI_COMMAND_REJECT_CON                          0x02
#define HCI_COMMAND_LINK_KEY_REP                        0x04
#define HCI_COMMAND_LINK_KEY_NEG_REP                    0x08
#define HCI_COMMAND_PIN_CODE_REP                        0x10
#define HCI_COMMAND_PIN_CODE_NEG_REP                    0x20
#define HCI_COMMAND_CHANGE_CON_PACKET_TYPE              0x40
#define HCI_COMMAND_AUTH_REQ                            0x80
/* ------------------------ byte 2 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_2                   2
#define HCI_COMMAND_SET_CON_ENCRYPTION                  0x01
#define HCI_COMMAND_CHANGE_CON_LINK_KEY                 0x02
#define HCI_COMMAND_MASTER_LINK_KEY                     0x04
#define HCI_COMMAND_REMOTE_NAME_REQ                     0x08
#define HCI_COMMAND_REMOTE_NAME_REQ_CANCEL              0x10
#define HCI_COMMAND_READ_REMOTE_FEATURES                0x20
#define HCI_COMMAND_READ_REMOTE_EXTENDED_FEATURES       0x40
#define HCI_COMMAND_READ_REMOTE_VER_INFO                0x80
/* ------------------------ byte 3 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_3                   3
#define HCI_COMMAND_READ_CLOCK_OFFSET                   0x01
#define HCI_COMMAND_READ_LMP_HANDLE                     0x02
/* reserved                                             0x04 */
/* reserved                                             0x08 */
/* reserved                                             0x10 */
/* reserved                                             0x20 */
/* reserved                                             0x40 */
/* reserved                                             0x80 */
/* ------------------------ byte 4 ----------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_4                   4
/* reserved                                             0x01 */
#define HCI_COMMAND_HOLD_MODE                           0x02
#define HCI_COMMAND_SNIFF_MODE                          0x04
#define HCI_COMMAND_EXIT_SNIFF_MODE                     0x08
#define HCI_COMMAND_PARK_MODE                           0x10
#define HCI_COMMAND_EXIT_PARK_MODE                      0x20
#define HCI_COMMAND_QOS_SETUP                           0x40
#define HCI_COMMAND_ROLE_DISCOVERY                      0x80
/* ------------------------ byte 5 ----------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_5                   5
#define HCI_COMMAND_SWITCH_ROLE                         0x01
#define HCI_COMMAND_READ_LINK_POLICY_SETTINGS           0x02
#define HCI_COMMAND_WRITE_LINK_POLICY_SETTINGS          0x04
#define HCI_COMMAND_READ_DEFAULT_LINK_POLICY_SETTINGS   0x08
#define HCI_COMMAND_WRITE_DEFAULT_LINK_POLICY_SETTINGS  0x10
#define HCI_COMMAND_FLOW_SPECIFICATION                  0x20
#define HCI_COMMAND_SET_EVENT_MASK                      0x40
#define HCI_COMMAND_RESET                               0x80
/* ------------------------ byte 6 ----------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_6                   6
#define HCI_COMMAND_SET_EVENT_FILTER                    0x01
#define HCI_COMMAND_FLUSH                               0x02
#define HCI_COMMAND_READ_PIN_TYPE                       0x04
#define HCI_COMMAND_WRITE_PIN_TYPE                      0x08
#define HCI_COMMAND_CREATE_NEW_UNIT_KEY                 0x10
#define HCI_COMMAND_READ_STORED_LINK_KEY                0x20
#define HCI_COMMAND_WRITE_STORED_LINK_KEY               0x40
#define HCI_COMMAND_DELETE_STORED_LINK_KEY              0x80
/* ------------------------ byte 7 ----------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_7                   7
#define HCI_COMMAND_WRITE_LOCAL_NAME                    0x01
#define HCI_COMMAND_READ_LOCAL_NAME                     0x02
#define HCI_COMMAND_READ_CON_ACCEPT_TIMEOUT             0x04
#define HCI_COMMAND_WRITE_CON_ACCEPT_TIMEOUT            0x08
#define HCI_COMMAND_READ_PAGE_TIMEOUT                   0x10
#define HCI_COMMAND_WRITE_PAGE_TIMEOUT                  0x20
#define HCI_COMMAND_READ_SCAN_ENABLE                    0x40
#define HCI_COMMAND_WRITE_SCAN_ENABLE                   0x80
/* ------------------------ byte 8 ----------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_8                   8
#define HCI_COMMAND_READ_PAGE_SCAN_ACTIVITY             0x01
#define HCI_COMMAND_WRITE_PAGE_SCAN_ACTIVITY            0x02
#define HCI_COMMAND_READ_INQUIRY_SCAN_ACTIVITY          0x04
#define HCI_COMMAND_WRITE_INQUIRY_SCAN_ACTIVITY         0x08
/* reserved                                             0x10 */
/* reserved                                             0x20 */
/* the following are reserved in V2.1+EDR and deprecated in V4.0 */
#define HCI_COMMAND_READ_ENCRYPTION_MODE                0x40
#define HCI_COMMAND_WRITE_ENCRYPTION_MODE               0x80
/* ------------------------ byte 9 ----------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_9                   9
#define HCI_COMMAND_READ_UNIT_CLASS                     0x01
#define HCI_COMMAND_WRITE_UNIT_CLASS                    0x02
#define HCI_COMMAND_READ_VOICE_SETTING                  0x04
#define HCI_COMMAND_WRITE_VOICE_SETTING                 0x08
#define HCI_COMMAND_READ_AUTO_FLUSH_TIMEOUT             0x10
#define HCI_COMMAND_WRITE_AUTO_FLUSH_TIMEOUT            0x20
#define HCI_COMMAND_READ_NUM_BROADCAST_RETRANS          0x40
#define HCI_COMMAND_WRITE_NUM_BROADCAST_RETRANS         0x80
/* ------------------------ byte 10 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_10                  10
#define HCI_COMMAND_READ_HOLD_MODE_ACTIVITY             0x01
#define HCI_COMMAND_WRITE_HOLD_MODE_ACTIVITY            0x02
#define HCI_COMMAND_READ_XMIT_LEVEL                     0x04
#define HCI_COMMAND_READ_SCO_FLOW_CONTROL               0x08
#define HCI_COMMAND_WRITE_SCO_FLOW_CONTROL              0x10
#define HCI_COMMAND_HC2H_FLOW_CONTROL                   0x20
#define HCI_COMMAND_HOST_BUFFER_SIZE                    0x40
#define HCI_COMMAND_HOST_NUM_COMPL_PKTS                 0x80
/* ------------------------ byte 11 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_11                  11
#define HCI_COMMAND_READ_LINK_SUPERVISION_TIMEOUT       0x01
#define HCI_COMMAND_WRITE_LINK_SUPERVISION_TIMEOUT      0x02
#define HCI_COMMAND_READ_NUM_SUPPORTED_IAC              0x04
#define HCI_COMMAND_READ_IAC_LAP                        0x08
#define HCI_COMMAND_WRITE_IAC_LAP                       0x10
/* reserved                                             0x20 */
/* reserved                                             0x40 */
#define HCI_COMMAND_READ_PAGE_SCAN_MODE                 0x80
/* ------------------------ byte 12 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_12                  12
#define HCI_COMMAND_WRITE_PAGE_SCAN_MODE                0x01
#define HCI_COMMAND_SET_AFH_CLASSIFICATION              0x02
/* reserved                                             0x04 */
/* reserved                                             0x08 */
#define HCI_COMMAND_READ_INQUIRY_SCAN_TYPE              0x10
#define HCI_COMMAND_WRITE_INQUIRY_SCAN_TYPE             0x20
#define HCI_COMMAND_READ_INQUIRY_MODE                   0x40
#define HCI_COMMAND_WRITE_INQUIRY_MODE                  0x80
/* ------------------------ byte 13 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_13                  13
#define HCI_COMMAND_READ_PAGE_SCAN_TYPE                 0x01
#define HCI_COMMAND_WRITE_PAGE_SCAN_TYPE                0x02
#define HCI_COMMAND_READ_AFH_ASSESSMENT                 0x04
#define HCI_COMMAND_WRITE_AFH_ASSESSMENT                0x08
/* reserved                                             0x10 */
/* reserved                                             0x20 */
/* reserved                                             0x40 */
/* reserved                                             0x80 */
/* ------------------------ byte 14 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_14                  14
/* reserved                                             0x01 */
/* reserved                                             0x02 */
/* reserved                                             0x04 */
#define HCI_COMMAND_READ_LOCAL_VER                      0x08
/* reserved                                             0x10 */
#define HCI_COMMAND_READ_LOCAL_FEATURES                 0x20
#define HCI_COMMAND_READ_LOCAL_EXTENDED_FEATURES        0x40
#define HCI_COMMAND_READ_BUFFER_SIZE                    0x80
/* ------------------------ byte 15 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_15                  15
#define HCI_COMMAND_READ_COUNTRY_CODE                   0x01 /* deprecated */
#define HCI_COMMAND_READ_BTADDR                         0x02
#define HCI_COMMAND_READ_FAILED_CONTACT_CNTR            0x04
#define HCI_COMMAND_RESET_FAILED_CONTACT_CNTR           0x08
#define HCI_COMMAND_READ_LINK_QUALITY                   0x10
#define HCI_COMMAND_READ_RSSI                           0x20
#define HCI_COMMAND_READ_AFH_CHANNEL_MAP                0x40
#define HCI_COMMAND_READ_CLOCK                          0x80
/* ------------------------ byte 16 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_16                  16
#define HCI_COMMAND_READ_LOOPBACK_MODE                  0x01
#define HCI_COMMAND_WRITE_LOOPBACK_MODE                 0x02
#define HCI_COMMAND_ENABLE_UNIT_UNDER_TEST              0x04
#define HCI_COMMAND_SETUP_SCO_CON                       0x08
#define HCI_COMMAND_ACCEPT_SCO_CON_REQ                  0x10
#define HCI_COMMAND_REJECT_SCO_CON_REQ                  0x20
/* reserved                                             0x40 */
/* reserved                                             0x80 */
/* ------------------------ byte 17 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_17                  17
#define HCI_COMMAND_READ_EXTENDED_INQUIRY_RSP           0x01
#define HCI_COMMAND_WRITE_EXTENDED_INQUIRY_RSP          0x02
#define HCI_COMMAND_REFRESH_ENCRYPTION_KEY              0x04
/* reserved                                             0x08 */
#define HCI_COMMAND_SNIFF_SUBRATING                     0x10
#define HCI_COMMAND_READ_SIMPLE_PAIRING_MODE            0x20
#define HCI_COMMAND_WRITE_SIMPLE_PAIRING_MODE           0x40
#define HCI_COMMAND_READ_LOCAL_OOB_DATA                 0x80
/* ------------------------ byte 18 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_18                  18
#define HCI_COMMAND_READ_INQUIRY_RSP_XMIT_POWER         0x01
#define HCI_COMMAND_WRITE_INQUIRY_RSP_XMIT_POWER        0x02
#define HCI_COMMAND_READ_DEFAULT_ERRDATA_REPORTING      0x04
#define HCI_COMMAND_WRITE_DEFAULT_ERRDATA_REPORTING     0x08
/* reserved                                             0x10 */
/* reserved                                             0x20 */
/* reserved                                             0x40 */
#define HCI_COMMAND_IO_CAPABILITY_REP                   0x80
/* ------------------------ byte 19 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_19                  19
#define HCI_COMMAND_USER_CONFIRM_REP                    0x01
#define HCI_COMMAND_USER_CONFIRM_NEG_REP                0x02
#define HCI_COMMAND_USER_PASSKEY_REP                    0x04
#define HCI_COMMAND_USER_PASSKEY_NEG_REP                0x08
#define HCI_COMMAND_OOB_DATA_REP                        0x10
#define HCI_COMMAND_WRITE_SIMPLE_PAIRING_DEBUG_MODE     0x20
#define HCI_COMMAND_ENHANCED_FLUSH                      0x40
#define HCI_COMMAND_OOB_DATA_NEG_REP                    0x80
/* ------------------------ byte 20 ---------------------------------------*/
#define HCI_SUPPORTED_COMMAND_OCTET_20                  20
/* reserved                                             0x01 */
/* reserved                                             0x02 */
#define HCI_COMMAND_SEND_KEYPRESS_NOTIFICATION          0x04
#define HCI_COMMAND_IO_CAPABILITY_NEG_REP               0x08
/* reserved                                             0x10 */
/* reserved                                             0x20 */
/* reserved                                             0x40 */
/* reserved                                             0x80 */

/* Link types */
#define HCI_LINK_SCO                    0x00 /* Voice */
#define HCI_LINK_ACL                    0x01 /* Data */
#define HCI_LINK_eSCO                   0x02 /* Voice */
/* 0x03 - 0xFF - reserved for future use */

/*
 * ACL/SCO packet type bits are set to enable the
 * packet type, except for 2MBPS and 3MBPS when they
 * are unset to enable the packet type.
 */
/* ACL Packet types for "Create Connection"
 * v2.1 Core Spec, Vol 2, Table 6.2 */
#define HCI_PKT_2MBPS_DH1               0x0002	//0
#define HCI_PKT_3MBPS_DH1               0x0004	//1
#define HCI_PKT_DM1                     0x0008	//1
#define HCI_PKT_DH1                     0x0010	//1
#define HCI_PKT_2MBPS_DH3               0x0100	//0
#define HCI_PKT_3MBPS_DH3               0x0200	//1
#define HCI_PKT_DM3                     0x0400	//1
#define HCI_PKT_DH3                     0x0800	//1
#define HCI_PKT_2MBPS_DH5               0x1000	//0
#define HCI_PKT_3MBPS_DH5               0x2000	//1
#define HCI_PKT_DM5                     0x4000	//1
#define HCI_PKT_DH5                     0x8000	//1
/* Attention: EDR packet types use inverted logic */
#define HCI_PKT_BR_2MBPS_ALL				0xEE1C

/* SCO and eSCO Packet types for "Setup Synchronous Connection".
 * v2.1 Core Spec, Vol 2, Table 6.2 */
#define HCI_PKT_HV1                     0x0001  /* SCO */
#define HCI_PKT_HV2                     0x0002  /* SCO */
#define HCI_PKT_HV3                     0x0004  /* SCO */
#define HCI_PKT_EV3                     0x0008  /* eSCO */
#define HCI_PKT_EV4                     0x0010  /* eSCO */
#define HCI_PKT_EV5                     0x0020  /* eSCO */
#define HCI_PKT_2MBPS_EV3               0x0040  /* eSCO */
#define HCI_PKT_3MBPS_EV3               0x0080  /* eSCO */
#define HCI_PKT_2MBPS_EV5               0x0100  /* eSCO */
#define HCI_PKT_3MBPS_EV5               0x0200  /* eSCO */

#define HCI_PKT_SCO_ALL                 0x03ff
#define HCI_PKT_ESCO_ALL                0x038C  /* 0x038f 0x03cf 0x0007 0x03c7 0x000f*/

/* Voice Settings - 2 bytes (only 10 bits are used)
 * v2.1 Core Spec, Vol 2, Part E, Section 6.12 */

#define HCI_PCM_INPUT_CODING_LINEAR     0x0   /* xxxxxx00xxxxxxxx */
#define HCI_PCM_INPUT_CODING_ULAW       0x1   /* xxxxxx01xxxxxxxx */
#define HCI_PCM_INPUT_CODING_ALAW       0x2   /* xxxxxx10xxxxxxxx */

#define HCI_PCM_INPUT_FORMAT_1COMP      0x0   /* xxxxxxxx00xxxxxx */
#define HCI_PCM_INPUT_FORMAT_2COMP      0x1   /* xxxxxxxx01xxxxxx */
#define HCI_PCM_INPUT_FORMAT_SIGN_MAG   0x2   /* xxxxxxxx10xxxxxx */
#define HCI_PCM_INPUT_FORMAT_UNSIGNED   0x3   /* xxxxxxxx11xxxxxx */

#define HCI_PCM_SAMPLE_SIZE_8BIT        0x0   /* xxxxxxxxxx0xxxxx */
#define HCI_PCM_SAMPLE_SIZE_16BIT       0x1   /* xxxxxxxxxx1xxxxx */

#define HCI_PCM_AIR_FORMAT_CVSD         0x0   /* xxxxxxxxxxxxxx00 */
#define HCI_PCM_AIR_FORMAT_ULAW         0x1   /* xxxxxxxxxxxxxx01 */
#define HCI_PCM_AIR_FORMAT_ALAW         0x2   /* xxxxxxxxxxxxxx10 */
#define HCI_PCM_AIR_FORMAT_TRANSPARENT  0x3   /* xxxxxxxxxxxxxx11 */

typedef struct {
    uint8_t input_coding;
    uint8_t input_format;
    uint8_t input_sample_size;
    uint8_t linear_bit_position;
    uint8_t air_coding_format;
} hci_pcm_settings_t;

/*
 * Connection modes/Unit modes
 *
 * This is confusing. It means that one of the units change its mode
 * for the specific connection. For example one connection was put on
 * hold (but i could be wrong :)
 */

/* Page scan modes (are deprecated) */
#define HCI_MANDATORY_PAGE_SCAN_MODE            0x00
#define HCI_OPTIONAL_PAGE_SCAN_MODE1            0x01
#define HCI_OPTIONAL_PAGE_SCAN_MODE2            0x02
#define HCI_OPTIONAL_PAGE_SCAN_MODE3            0x03
/* 0x04 - 0xFF - reserved for future use */

/* Page scan repetition modes */
#define HCI_SCAN_REP_MODE0                      0x00
#define HCI_SCAN_REP_MODE1                      0x01
#define HCI_SCAN_REP_MODE2                      0x02
/* 0x03 - 0xFF - reserved for future use */

/* Page scan period modes */
#define HCI_PAGE_SCAN_PERIOD_MODE0              0x00
#define HCI_PAGE_SCAN_PERIOD_MODE1              0x01
#define HCI_PAGE_SCAN_PERIOD_MODE2              0x02
/* 0x03 - 0xFF - reserved for future use */

/* Scan enable */
#define HCI_NO_SCAN_ENABLE                      0x00
#define HCI_INQUIRY_SCAN_ENABLE                 0x01
#define HCI_PAGE_SCAN_ENABLE                    0x02
/* 0x04 - 0xFF - reserved for future use */

/* Hold mode activities */
#define HCI_HOLD_MODE_NO_CHANGE                 0x00
#define HCI_HOLD_MODE_SUSPEND_PAGE_SCAN         0x01
#define HCI_HOLD_MODE_SUSPEND_INQUIRY_SCAN      0x02
#define HCI_HOLD_MODE_SUSPEND_PERIOD_INQUIRY    0x04
/* 0x08 - 0x80 - reserved for future use */

/* Connection roles */
#define HCI_ROLE_MASTER                         0x00
#define HCI_ROLE_SLAVE                          0x01
/* 0x02 - 0xFF - reserved for future use */

/* Key flags */
#define HCI_USE_SEMI_PERMANENT_LINK_KEYS        0x00
#define HCI_USE_TEMPORARY_LINK_KEY              0x01
/* 0x02 - 0xFF - reserved for future use */

/* Pin types */
#define HCI_PIN_TYPE_VARIABLE                   0x00
#define HCI_PIN_TYPE_FIXED                      0x01

/* Link key types */
#define HCI_LINK_KEY_TYPE_COMBINATION_KEY                   0x00
#define HCI_LINK_KEY_TYPE_LOCAL_UNIT_KEY                    0x01
#define HCI_LINK_KEY_TYPE_REMOTE_UNIT_KEY                   0x02
#define HCI_LINK_KEY_TYPE_DEBUG_COMBINATION_KEY             0x03
#define HCI_LINK_KEY_TYPE_UNAUTHENTICATED_COMBINATION_KEY   0x04
#define HCI_LINK_KEY_TYPE_AUTHENTICATED_COMBINATION_KEY     0x05
#define HCI_LINK_KEY_TYPE_CHANGED_COMBINATION_KEY           0x06
/* 0x07 - 0xFF - reserved for future use */

/* Encryption modes */
#define HCI_ENCRYPTION_MODE_NONE                0x00
#define HCI_ENCRYPTION_MODE_P2P                 0x01
#define HCI_ENCRYPTION_MODE_ALL                 0x02
/* 0x03 - 0xFF - reserved for future use */

/* Quality of service types */
#define HCI_SERVICE_TYPE_NO_TRAFFIC             0x00
#define HCI_SERVICE_TYPE_BEST_EFFORT            0x01
#define HCI_SERVICE_TYPE_GUARANTEED             0x02
/* 0x03 - 0xFF - reserved for future use */

/* Link policy settings */
#define HCI_LINK_POLICY_DISABLE_ALL_LM_MODES    0x0000
#define HCI_LINK_POLICY_ENABLE_ROLE_SWITCH      0x0001 /* Master/Slave switch */
#define HCI_LINK_POLICY_ENABLE_HOLD_MODE        0x0002
#define HCI_LINK_POLICY_ENABLE_SNIFF_MODE       0x0004
#define HCI_LINK_POLICY_ENABLE_PARK_MODE        0x0008
/* 0x0010 - 0x8000 - reserved for future use */

/* Event masks */
/* TODO: Convert to shift notation? if we do, can abuse the fact that each
 * event value maps to its corresponding bit, though some do not exist in the
 * mask */
#define HCI_EVMSK_ALL                           0xffffffff
#define HCI_EVMSK_NONE                          0x00000000
/* Lower 32 bits of mask */
#define HCI_EVMSK_INQUIRY_COMPL                 0x00000001
#define HCI_EVMSK_INQUIRY_RESULT                0x00000002
#define HCI_EVMSK_CON_COMPL                     0x00000004
#define HCI_EVMSK_CON_REQ                       0x00000008
#define HCI_EVMSK_DISCON_COMPL                  0x00000010
#define HCI_EVMSK_AUTH_COMPL                    0x00000020
#define HCI_EVMSK_REMOTE_NAME_REQ_COMPL         0x00000040
#define HCI_EVMSK_ENCRYPTION_CHANGE             0x00000080
#define HCI_EVMSK_CHANGE_CON_LINK_KEY_COMPL     0x00000100
#define HCI_EVMSK_MASTER_LINK_KEY_COMPL         0x00000200
#define HCI_EVMSK_READ_REMOTE_FEATURES_COMPL    0x00000400
#define HCI_EVMSK_READ_REMOTE_VER_INFO_COMPL    0x00000800
#define HCI_EVMSK_QOS_SETUP_COMPL               0x00001000
/* Reserved                                     0x00002000 */
/* Reserved                                     0x00004000 */
#define HCI_EVMSK_HARDWARE_ERROR                0x00008000
#define HCI_EVMSK_FLUSH_OCCUR                   0x00010000
#define HCI_EVMSK_ROLE_CHANGE                   0x00020000
/* Reserved                                     0x00040000 */
#define HCI_EVMSK_MODE_CHANGE                   0x00080000
#define HCI_EVMSK_RETURN_LINK_KEYS              0x00100000
#define HCI_EVMSK_PIN_CODE_REQ                  0x00200000
#define HCI_EVMSK_LINK_KEY_REQ                  0x00400000
#define HCI_EVMSK_LINK_KEY_NOTIFICATION         0x00800000
#define HCI_EVMSK_LOOPBACK_COMMAND              0x01000000
#define HCI_EVMSK_DATA_BUFFER_OVERFLOW          0x02000000
#define HCI_EVMSK_MAX_SLOT_CHANGE               0x04000000
#define HCI_EVMSK_READ_CLOCK_OFFSET_COMLETE     0x08000000
#define HCI_EVMSK_CON_PKT_TYPE_CHANGED          0x10000000
#define HCI_EVMSK_QOS_VIOLATION                 0x20000000
#define HCI_EVMSK_PAGE_SCAN_MODE_CHANGE         0x40000000
#define HCI_EVMSK_PAGE_SCAN_REP_MODE_CHANGE     0x80000000
/* High 32 bits of mask */
/* TODO: Some of my names don't match the command name exactly, mostly when I
 * think it might be unclear, FIXME */
#define HCI_EVMSK_FLOW_SPECIFICATION_COMPL          0x00000001
#define HCI_EVMSK_RSSI_INQUIRY_RESULT               0x00000002
#define HCI_EVMSK_READ_REMOTE_EXT_FEATURES_COMPL    0x00000004
/* Reserved                                         0x00000008 */
/* Reserved                                         0x00000010 */
/* Reserved                                         0x00000020 */
/* Reserved                                         0x00000040 */
/* Reserved                                         0x00000080 */
/* Reserved                                         0x00000100 */
/* Reserved                                         0x00000200 */
/* Reserved                                         0x00000400 */
#define HCI_EVMSK_SCO_CON_COMPL                     0x00000800
#define HCI_EVMSK_SCO_CON_CHANGED                   0x00001000
#define HCI_EVMSK_SNIFF_SUBRATING                   0x00002000
#define HCI_EVMSK_EXTENDED_INQUIRY_RESULT           0x00004000
#define HCI_EVMSK_ENCRYPTION_KEY_REFRESH            0x00008000
#define HCI_EVMSK_IO_CAPABILITY_REQ                 0x00010000
#define HCI_EVMSK_IO_CAPABILITY_RSP                 0x00020000
#define HCI_EVMSK_USER_CONFIRM_REQ                  0x00040000
#define HCI_EVMSK_USER_PASSKEY_REQ                  0x00080000
#define HCI_EVMSK_REMOTE_OOB_DATA_REQ               0x00100000
#define HCI_EVMSK_SIMPLE_PAIRING_COMPL              0x00200000
/* Reserved                                         0x00400000 */
#define HCI_EVMSK_LINK_SUPERVISION_TO_CHANGED       0x00800000
#define HCI_EVMSK_ENHANCED_FLUSH_COMPL              0x01000000
/* Reserved                                         0x02000000 */
#define HCI_EVMSK_USER_PASSKEY_NOTIFICATION         0x04000000
#define HCI_EVMSK_KEYPRESS_NOTIFICATION             0x08000000
#define HCI_EVMSK_REMOTE_HOST_FEATURES_NOTIFICATION 0x10000000

/* Filter types */
#define HCI_FILTER_TYPE_NONE                    0x00
#define HCI_FILTER_TYPE_INQUIRY_RESULT          0x01
#define HCI_FILTER_TYPE_CON_SETUP               0x02
/* 0x03 - 0xFF - reserved for future use */

/* Filter condition types for HCI_FILTER_TYPE_INQUIRY_RESULT */
#define HCI_FILTER_COND_INQUIRY_NEW_UNIT        0x00
#define HCI_FILTER_COND_INQUIRY_UNIT_CLASS      0x01
#define HCI_FILTER_COND_INQUIRY_BTADDR          0x02
/* 0x03 - 0xFF - reserved for future use */

/* Filter condition types for HCI_FILTER_TYPE_CON_SETUP */
#define HCI_FILTER_COND_CON_ANY_UNIT            0x00
#define HCI_FILTER_COND_CON_UNIT_CLASS          0x01
#define HCI_FILTER_COND_CON_BTADDR              0x02
/* 0x03 - 0xFF - reserved for future use */

/* Xmit level types */
#define HCI_XMIT_LEVEL_CURRENT                  0x00
#define HCI_XMIT_LEVEL_MAXIMUM                  0x01
/* 0x02 - 0xFF - reserved for future use */

/* Host Controller to Host flow control */
#define HCI_HC2H_FLOW_CONTROL_NONE              0x00
#define HCI_HC2H_FLOW_CONTROL_ACL               0x01
#define HCI_HC2H_FLOW_CONTROL_SCO               0x02
#define HCI_HC2H_FLOW_CONTROL_BOTH              0x03
/* 0x04 - 0xFF - reserved future use */

/* Loopback modes */
#define HCI_LOOPBACK_NONE                       0x00
#define HCI_LOOPBACK_LOCAL                      0x01
#define HCI_LOOPBACK_REMOTE                     0x02
/* 0x03 - 0xFF - reserved future use */

/* IO Capabilities */
#define HCI_IO_CAPABILITY_DISPLAY_ONLY          0x00
#define HCI_IO_CAPABILITY_DISPLAY_YESNO         0x01
#define HCI_IO_CAPABILITY_KEYBOARD_ONLY         0x02
#define HCI_IO_CAPABILITY_NO_IO                 0x03
/* 0x04 - 0xFF reserved for future use */

/* Authentication requirements */
#define HCI_AUTH_NO_BONDING             0x00
#define HCI_AUTH_NO_BONDING_MITM        0x01
#define HCI_AUTH_DEDICATED_BONDING      0x02
#define HCI_AUTH_DEDICATED_BONDING_MITM 0x03
#define HCI_AUTH_GENERAL_BONDING        0x04
#define HCI_AUTH_GENERAL_BONDING_MITM   0x05
/* 0x06-0xFF Reserved for future use */
/* An invalid mode setting, used as unknown remote value, no MITM */
#define HCI_AUTH_INVALID_BONDING_MODE   0xFE

/* Used to check auth req for MITM, currently MITM assumed to be odd */
#define HCI_AUTH_MITM_MASK              0x01

/* Keypress Notification types */
#define HCI_KEYPRESS_ENTRY_STARTED              0
#define HCI_KEYPRESS_DIGIT_ENTERED              1
#define HCI_KEYPRESS_DIGIT_ERASED               2
#define HCI_KEYPRESS_PASSKEY_CLEARED            3
#define HCI_KEYPRESS_ENTRY_COMPLETED            4
/* 5 - 255 reserved for future use */

/* Extended Inquiry Response */
#define HCI_MAX_EXTENDED_INQ_RSP_LEN    240
#define HCI_EIR_DATA_TYPE_SIZE          1    /* size of data type */
#define HCI_EIR_DATA_LEN_AND_TYPE_SIZE  2    /* size of data type + length */
#define HCI_EIR_MAX_NAME_LEN            64   /* internal limit of name length */
#define HCI_EIR_TYPE_FLAGS              0x01 /* flags */
#define HCI_EIR_TYPE_UUID16_PART        0x02 /* 16-bit UUID, more available */
#define HCI_EIR_TYPE_UUID16_ALL         0x03 /* 16-bit UUID, all listed */
#define HCI_EIR_TYPE_UUID32_PART        0x04 /* 32-bit UUID, more available */
#define HCI_EIR_TYPE_UUID32_ALL         0x05 /* 32-bit UUID, all listed */
#define HCI_EIR_TYPE_UUID128_PART       0x06 /* 128-bit UUID, more available */
#define HCI_EIR_TYPE_UUID128_ALL        0x07 /* 128-bit UUID, all listed */
#define HCI_EIR_TYPE_NAME_SHORTENED     0x08 /* shortened local name */
#define HCI_EIR_TYPE_NAME_COMPLETE      0x09 /* complete local name */
#define HCI_EIR_TYPE_TX_POWER_LEVEL     0x0A /* transmit power level */

/**************************************************************************
 **                 Link level defines, headers and types
 **************************************************************************/

/*
 * Macro(s) to combine OpCode and extract OGF (OpCode Group Field)
 * and OCF (OpCode Command Field) from OpCode.
 */

#define HCI_OPCODE(gf,cf)               ((((gf) & 0x3f) << 10) | ((cf) & 0x3ff))
#define HCI_OCF(op)                     ((op) & 0x3ff)
#define HCI_OGF(op)                     (((op) >> 10) & 0x3f)

/*
 * Macro(s) to extract/combine connection handle, BC (Broadcast) and
 * PB (Packet boundary) flags.
 */

#define HCI_CON_HANDLE(h)               ((h) & 0x0fff)
#define HCI_PB_FLAG(h)                  (((h) & 0x3000) >> 12)
#define HCI_BC_FLAG(h)                  (((h) & 0xc000) >> 14)
#define HCI_MK_CON_HANDLE(h, pb, bc) \
    (((h) & 0x0fff) | (((pb) & 3) << 12) | (((bc) & 3) << 14))

/* PB flag values */
/* 00 - reserved for future use */
#define HCI_PACKET_FRAGMENT             0x1
#define HCI_PACKET_START                0x2
/* 11 - reserved for future use */

/* BC flag values */
#define HCI_POINT2POINT                 0x0 /* only Host controller to Host */
#define HCI_BROADCAST_ACTIVE            0x1 /* both directions */
#define HCI_BROADCAST_PICONET           0x2 /* both directions */
/* 11 - reserved for future use */

/* A "packet type" byte is prepended in HCI's that transfer different packet
 * types over a single channel (such as RS232 HCI).
 *
 * USB HCI does not use the "packet type" byte, because each type has a
 * dedicated USB pipe.
 */
#define HCI_CMD_PKT                     0x01
#define HCI_ACL_DATA_PKT                0x02
#define HCI_SCO_DATA_PKT                0x03
#define HCI_EVENT_PKT                   0x04
typedef uint8_t hci_pkt_type_t;

/* HCI command packet header
 * Core spec, 5.4.1 HCI Command Packet */
typedef   struct {
    uint16_t    opcode; /* OpCode */
    uint8_t     length; /* parameter(s) length in bytes */
} __PACKED_POST__ hci_cmd_hdr_t;

#define HCI_CMD_MAX_PARAM_SIZE 0xff

#define HCI_CMD_PKT_SIZE \
    (sizeof(hci_pkt_type_t) + sizeof(hci_cmd_hdr_t) + HCI_CMD_MAX_PARAM_SIZE)

/* ACL data packet header
 * Core spec, 5.4.2 HCI ACL Data Packets */
typedef   struct {
    uint16_t    con_handle;     /* connection handle + PB + BC flags */
    uint16_t    length;         /* payload length in bytes */
} __PACKED_POST__ hci_acldata_hdr_t;

#define HCI_ACL_PKT_SIZE \
    (sizeof(hci_pkt_type_t) + sizeof(hci_acldata_hdr_t) + 0xffff)

/* SCO and eSCO data packet header
 * Core spec, 5.4.3 HCI Synchronous Data Packets */
typedef   struct {
    uint16_t    con_handle;     /* connection handle + status flags +
                                   reserved bits */
    uint8_t     length;         /* payload length in bytes */
} __PACKED_POST__ hci_scodata_hdr_t;

#define HCI_SCO_PKT_SIZE \
    (sizeof(hci_pkt_type_t) + sizeof(hci_scodata_hdr_t) + 0xff)

/* HCI event packet header
 * Core spec, 5.4.4 HCI Event Packet */
typedef   struct {
    uint8_t     event;  /* event */
    uint8_t     length; /* parameter(s) length in bytes */
} __PACKED_POST__ hci_event_hdr_t;

#define HCI_EVENT_PKT_SIZE \
    (sizeof(hci_pkt_type_t) + sizeof(hci_event_hdr_t) + 0xff)

/* HCI status return parameter */
typedef   struct {
    uint8_t     status; /* 0x00 - success */
} __PACKED_POST__ hci_status_rp;

/* HCI key pair */
typedef   struct {
    btaddr_t btaddr;
    uint8_t  key[HCI_KEY_SIZE];
} __PACKED_POST__ hci_key_pair_t;

/**************************************************************************
 ** OGF 0x01    Link control commands and return parameters
 **************************************************************************/

#define HCI_OGF_LINK_CONTROL            0x01

#define HCI_OCF_INQUIRY                 0x0001
#define HCI_CMD_INQUIRY                 0x0401
typedef   struct {
    uint8_t     lap[HCI_LAP_SIZE]; /* LAP */
    uint8_t     inquiry_length;    /* (N x 1.28) sec */
    uint8_t     num_responses;     /* Max. # of responses */
} __PACKED_POST__ hci_inquiry_cp;
/* No return parameter(s) */

#define HCI_OCF_INQUIRY_CANCEL          0x0002
#define HCI_CMD_INQUIRY_CANCEL          0x0402
/* No command parameter(s) */
typedef hci_status_rp   hci_inquiry_cancel_rp;

#define HCI_OCF_PERIODIC_INQUIRY        0x0003
#define HCI_CMD_PERIODIC_INQUIRY        0x0403
typedef   struct {
    uint16_t    max_period_length; /* Max. and min. amount of time */
    uint16_t    min_period_length; /* between consecutive inquiries */
    uint8_t     lap[HCI_LAP_SIZE]; /* LAP */
    uint8_t     inquiry_length;    /* (inquiry_length * 1.28) sec */
    uint8_t     num_responses;     /* Max. # of responses */
} __PACKED_POST__ hci_periodic_inquiry_cp;

typedef hci_status_rp   hci_periodic_inquiry_rp;

#define HCI_OCF_EXIT_PERIODIC_INQUIRY                   0x0004
#define HCI_CMD_EXIT_PERIODIC_INQUIRY                   0x0404
/* No command parameter(s) */
typedef hci_status_rp   hci_exit_periodic_inquiry_rp;

#define HCI_OCF_CREATE_CON                              0x0005
#define HCI_CMD_CREATE_CON                              0x0405
typedef   struct {
    btaddr_t    btaddr;             /* destination address */
    uint16_t    pkt_type;           /* packet type */
    uint8_t     page_scan_rep_mode; /* page scan repetition mode */
    uint8_t     page_scan_mode;     /* reserved - set to 0x00 */
    uint16_t    clock_offset;       /* clock offset */
    uint8_t     accept_role_switch; /* accept role switch? 0x00 == No */
} __PACKED_POST__ hci_create_con_cp;
/* No return parameter(s) */

#define HCI_OCF_DISCONNECT                              0x0006
#define HCI_CMD_DISCONNECT                              0x0406
typedef   struct {
    uint16_t    con_handle; /* connection handle */
    uint8_t     reason;     /* reason to disconnect */
} __PACKED_POST__ hci_discon_cp;
/* No return parameter(s) */

/* Add SCO Connection is deprecated */
#define HCI_OCF_ADD_SCO_CON                             0x0007
#define HCI_CMD_ADD_SCO_CON                             0x0407
typedef   struct {
    uint16_t    con_handle; /* connection handle */
    uint16_t    pkt_type;   /* packet type */
} __PACKED_POST__ hci_add_sco_con_cp;
/* No return parameter(s) */

#define HCI_OCF_CREATE_CON_CANCEL                       0x0008
#define HCI_CMD_CREATE_CON_CANCEL                       0x0408
typedef   struct {
    btaddr_t    btaddr;         /* destination address */
} __PACKED_POST__ hci_create_con_cancel_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* destination address */
} __PACKED_POST__ hci_create_con_cancel_rp;

#define HCI_OCF_ACCEPT_CON                              0x0009
#define HCI_CMD_ACCEPT_CON                              0x0409
typedef   struct {
    btaddr_t    btaddr; /* address of unit to be connected */
    uint8_t     role;   /* connection role */
} __PACKED_POST__ hci_accept_con_cp;
/* No return parameter(s) */

#define HCI_OCF_REJECT_CON                              0x000a
#define HCI_CMD_REJECT_CON                              0x040A
typedef   struct {
    btaddr_t    btaddr; /* remote address */
    uint8_t     reason; /* reason to reject */
} __PACKED_POST__ hci_reject_con_cp;
/* No return parameter(s) */

#define HCI_OCF_LINK_KEY_REP                            0x000b
#define HCI_CMD_LINK_KEY_REP                            0x040B
typedef   struct {
    btaddr_t    btaddr;            /* remote address */
    uint8_t     key[HCI_KEY_SIZE]; /* key */
} __PACKED_POST__ hci_link_key_rep_cp;

typedef   struct {
    uint8_t     status; /* 0x00 - success */
    btaddr_t    btaddr; /* unit address */
} __PACKED_POST__ hci_link_key_rep_rp;

#define HCI_OCF_LINK_KEY_NEG_REP                        0x000c
#define HCI_CMD_LINK_KEY_NEG_REP                        0x040C
typedef   struct {
    btaddr_t    btaddr; /* remote address */
} __PACKED_POST__ hci_link_key_neg_rep_cp;

typedef   struct {
    uint8_t     status; /* 0x00 - success */
    btaddr_t    btaddr; /* unit address */
} __PACKED_POST__ hci_link_key_neg_rep_rp;

#define HCI_OCF_PIN_CODE_REP                            0x000d
#define HCI_CMD_PIN_CODE_REP                            0x040D
typedef   struct {
    btaddr_t    btaddr;               /* remote address */
    uint8_t     pin_size;             /* pin code length (in bytes) */
    uint8_t     pin[HCI_PIN_SIZE];    /* pin code */
} __PACKED_POST__ hci_pin_code_rep_cp;

typedef   struct {
    uint8_t     status; /* 0x00 - success */
    btaddr_t    btaddr; /* unit address */
} __PACKED_POST__ hci_pin_code_rep_rp;

#define HCI_OCF_PIN_CODE_NEG_REP                        0x000e
#define HCI_CMD_PIN_CODE_NEG_REP                        0x040E
typedef   struct {
    btaddr_t    btaddr; /* remote address */
} __PACKED_POST__ hci_pin_code_neg_rep_cp;

typedef   struct {
    uint8_t     status; /* 0x00 - success */
    btaddr_t    btaddr; /* unit address */
} __PACKED_POST__ hci_pin_code_neg_rep_rp;

#define HCI_OCF_CHANGE_CON_PACKET_TYPE                  0x000f
#define HCI_CMD_CHANGE_CON_PACKET_TYPE                  0x040F
typedef   struct {
    uint16_t    con_handle; /* connection handle */
    uint16_t    pkt_type;   /* packet type */
} __PACKED_POST__ hci_change_con_pkt_type_cp;
/* No return parameter(s) */

#define HCI_OCF_AUTH_REQ                                0x0011
#define HCI_CMD_AUTH_REQ                                0x0411
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_auth_req_cp;
/* No return parameter(s) */

#define HCI_OCF_SET_CON_ENCRYPTION                      0x0013
#define HCI_CMD_SET_CON_ENCRYPTION                      0x0413
typedef   struct {
    uint16_t    con_handle;        /* connection handle */
    uint8_t     encryption_enable; /* 0x00 - disable, 0x01 - enable */
} __PACKED_POST__ hci_set_con_encryption_cp;
/* No return parameter(s) */

#define HCI_OCF_CHANGE_CON_LINK_KEY                     0x0015
#define HCI_CMD_CHANGE_CON_LINK_KEY                     0x0415
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_change_con_link_key_cp;
/* No return parameter(s) */

#define HCI_OCF_MASTER_LINK_KEY                         0x0017
#define HCI_CMD_MASTER_LINK_KEY                         0x0417
typedef   struct {
    uint8_t     key_flag; /* key flag */
} __PACKED_POST__ hci_master_link_key_cp;
/* No return parameter(s) */

#define HCI_OCF_REMOTE_NAME_REQ                         0x0019
#define HCI_CMD_REMOTE_NAME_REQ                         0x0419
typedef   struct {
    btaddr_t    btaddr;             /* remote address */
    uint8_t     page_scan_rep_mode; /* page scan repetition mode */
    uint8_t     page_scan_mode;     /* page scan mode */
    uint16_t    clock_offset;       /* clock offset */
} __PACKED_POST__ hci_remote_name_req_cp;
/* No return parameter(s) */

#define HCI_OCF_REMOTE_NAME_REQ_CANCEL                  0x001a
#define HCI_CMD_REMOTE_NAME_REQ_CANCEL                  0x041A
typedef   struct {
    btaddr_t    btaddr;             /* remote address */
} __PACKED_POST__ hci_remote_name_req_cancel_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_remote_name_req_cancel_rp;

#define HCI_OCF_READ_REMOTE_FEATURES                    0x001b
#define HCI_CMD_READ_REMOTE_FEATURES                    0x041B
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_remote_features_cp;
/* No return parameter(s) */

#define HCI_OCF_READ_REMOTE_EXTENDED_FEATURES           0x001c
#define HCI_CMD_READ_REMOTE_EXTENDED_FEATURES           0x041C
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
    uint8_t     page;           /* page number */
} __PACKED_POST__ hci_read_remote_extended_features_cp;
/* No return parameter(s) */

#define HCI_OCF_READ_REMOTE_VER_INFO                    0x001d
#define HCI_CMD_READ_REMOTE_VER_INFO                    0x041D
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_remote_ver_info_cp;
/* No return parameter(s) */

#define HCI_OCF_READ_CLOCK_OFFSET                       0x001f
#define HCI_CMD_READ_CLOCK_OFFSET                       0x041F
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_clock_offset_cp;
/* No return parameter(s) */

#define HCI_OCF_READ_LMP_HANDLE                         0x0020
#define HCI_CMD_READ_LMP_HANDLE                         0x0420
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_lmp_handle_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint8_t     lmp_handle; /* LMP handle */
    uint32_t    reserved;   /* reserved */
} __PACKED_POST__ hci_read_lmp_handle_rp;

#define HCI_OCF_SETUP_SCO_CON                           0x0028
#define HCI_CMD_SETUP_SCO_CON                           0x0428
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
    uint32_t    tx_bandwidth;   /* transmit bandwidth */
    uint32_t    rx_bandwidth;   /* receive bandwidth */
    uint16_t    latency;        /* maximum latency */
    uint16_t    voice;          /* voice setting */
    uint8_t     rt_effort;      /* retransmission effort */
    uint16_t    pkt_type;       /* packet types */
} __PACKED_POST__ hci_setup_sco_con_cp;
/* No return parameter(s) */

#define HCI_OCF_ACCEPT_SCO_CON_REQ                      0x0029
#define HCI_CMD_ACCEPT_SCO_CON_REQ                      0x0429
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
    uint32_t    tx_bandwidth;   /* transmit bandwidth */
    uint32_t    rx_bandwidth;   /* receive bandwidth */
    uint16_t    latency;        /* maximum latency */
    uint16_t    content;        /* voice setting */
    uint8_t     rt_effort;      /* retransmission effort */
    uint16_t    pkt_type;       /* packet types */
} __PACKED_POST__ hci_accept_sco_con_req_cp;
/* No return parameter(s) */

#define HCI_OCF_REJECT_SCO_CON_REQ                      0x002a
#define HCI_CMD_REJECT_SCO_CON_REQ                      0x042a
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
    uint8_t     reason;         /* reject error code */
} __PACKED_POST__ hci_reject_sco_con_req_cp;
/* No return parameter(s) */

#define HCI_OCF_IO_CAPABILITY_REP                       0x002b
#define HCI_CMD_IO_CAPABILITY_REP                       0x042b
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
    uint8_t     io_cap;         /* IO capability */
    uint8_t     oob_data;       /* OOB data present */
    uint8_t     auth_req;       /* auth requirements */
} __PACKED_POST__ hci_io_capability_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_io_capability_rep_rp;

#define HCI_OCF_USER_CONFIRM_REP                        0x002c
#define HCI_CMD_USER_CONFIRM_REP                        0x042c
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_confirm_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_confirm_rep_rp;

#define HCI_OCF_USER_CONFIRM_NEG_REP                    0x002d
#define HCI_CMD_USER_CONFIRM_NEG_REP                    0x042d
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_confirm_neg_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_confirm_neg_rep_rp;

#define HCI_OCF_USER_PASSKEY_REP                        0x002e
#define HCI_CMD_USER_PASSKEY_REP                        0x042e
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
    uint32_t    value;          /* 000000 - 999999 */
} __PACKED_POST__ hci_user_passkey_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_passkey_rep_rp;

#define HCI_OCF_USER_PASSKEY_NEG_REP                    0x002f
#define HCI_CMD_USER_PASSKEY_NEG_REP                    0x042f
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_passkey_neg_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_passkey_neg_rep_rp;

#define HCI_OCF_OOB_DATA_REP                            0x0030
#define HCI_CMD_OOB_DATA_REP                            0x0430
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
    uint8_t     c[HCI_PAIRING_SIZE];    /* pairing hash */
    uint8_t     r[HCI_PAIRING_SIZE];    /* pairing randomizer */
} __PACKED_POST__ hci_user_oob_data_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_oob_data_rep_rp;

#define HCI_OCF_OOB_DATA_NEG_REP                        0x0033
#define HCI_CMD_OOB_DATA_NEG_REP                        0x0433
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_oob_data_neg_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_user_oob_data_neg_rep_rp;

#define HCI_OCF_IO_CAPABILITY_NEG_REP                   0x0034
#define HCI_CMD_IO_CAPABILITY_NEG_REP                   0x0434
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
    uint8_t     reason;         /* error code */
} __PACKED_POST__ hci_io_capability_neg_rep_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_io_capability_neg_rep_rp;

/**************************************************************************
 ** OGF 0x02    Link policy commands and return parameters
 **************************************************************************/

#define HCI_OGF_LINK_POLICY                     0x02

#define HCI_OCF_HOLD_MODE                               0x0001
#define HCI_CMD_HOLD_MODE                               0x0801
typedef   struct {
    uint16_t    con_handle;   /* connection handle */
    uint16_t    max_interval; /* (max_interval * 0.625) msec */
    uint16_t    min_interval; /* (max_interval * 0.625) msec */
} __PACKED_POST__ hci_hold_mode_cp;
/* No return parameter(s) */

#define HCI_OCF_SNIFF_MODE                              0x0003
#define HCI_CMD_SNIFF_MODE                              0x0803
typedef   struct {
    uint16_t    con_handle;   /* connection handle */
    uint16_t    max_interval; /* (max_interval * 0.625) msec */
    uint16_t    min_interval; /* (max_interval * 0.625) msec */
    uint16_t    attempt;      /* (2 * attempt - 1) * 0.625 msec */
    uint16_t    timeout;      /* (2 * attempt - 1) * 0.625 msec */
} __PACKED_POST__ hci_sniff_mode_cp;
/* No return parameter(s) */

#define HCI_OCF_EXIT_SNIFF_MODE                         0x0004
#define HCI_CMD_EXIT_SNIFF_MODE                         0x0804
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_exit_sniff_mode_cp;
/* No return parameter(s) */

#define HCI_OCF_PARK_MODE                               0x0005
#define HCI_CMD_PARK_MODE                               0x0805
typedef   struct {
    uint16_t    con_handle;   /* connection handle */
    uint16_t    max_interval; /* (max_interval * 0.625) msec */
    uint16_t    min_interval; /* (max_interval * 0.625) msec */
} __PACKED_POST__ hci_park_mode_cp;
/* No return parameter(s) */

#define HCI_OCF_EXIT_PARK_MODE                          0x0006
#define HCI_CMD_EXIT_PARK_MODE                          0x0806
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_exit_park_mode_cp;
/* No return parameter(s) */

#define HCI_OCF_QOS_SETUP                               0x0007
#define HCI_CMD_QOS_SETUP                               0x0807
typedef   struct {
    uint16_t    con_handle;      /* connection handle */
    uint8_t     flags;           /* reserved for future use */
    uint8_t     service_type;    /* service type */
    uint32_t    token_rate;      /* bytes per second */
    uint32_t    peak_bandwidth;  /* bytes per second */
    uint32_t    latency;         /* microseconds */
    uint32_t    delay_variation; /* microseconds */
} __PACKED_POST__ hci_qos_setup_cp;
/* No return parameter(s) */

#define HCI_OCF_ROLE_DISCOVERY                          0x0009
#define HCI_CMD_ROLE_DISCOVERY                          0x0809
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_role_discovery_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint8_t     role;       /* role for the connection handle */
} __PACKED_POST__ hci_role_discovery_rp;

#define HCI_OCF_SWITCH_ROLE                             0x000b
#define HCI_CMD_SWITCH_ROLE                             0x080B
typedef   struct {
    btaddr_t    btaddr; /* remote address */
    uint8_t     role;   /* new local role */
} __PACKED_POST__ hci_switch_role_cp;
/* No return parameter(s) */

#define HCI_OCF_READ_LINK_POLICY_SETTINGS               0x000c
#define HCI_CMD_READ_LINK_POLICY_SETTINGS               0x080C
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_link_policy_settings_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint16_t    settings;   /* link policy settings */
} __PACKED_POST__ hci_read_link_policy_settings_rp;

#define HCI_OCF_WRITE_LINK_POLICY_SETTINGS              0x000d
#define HCI_CMD_WRITE_LINK_POLICY_SETTINGS              0x080D
typedef   struct {
    uint16_t    con_handle; /* connection handle */
    uint16_t    settings;   /* link policy settings */
} __PACKED_POST__ hci_write_link_policy_settings_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_write_link_policy_settings_rp;

#define HCI_OCF_READ_DEFAULT_LINK_POLICY_SETTINGS       0x000e
#define HCI_CMD_READ_DEFAULT_LINK_POLICY_SETTINGS       0x080E
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    settings;   /* link policy settings */
} __PACKED_POST__ hci_read_default_link_policy_settings_rp;

#define HCI_OCF_WRITE_DEFAULT_LINK_POLICY_SETTINGS      0x000f
#define HCI_CMD_WRITE_DEFAULT_LINK_POLICY_SETTINGS      0x080F
typedef   struct {
    uint16_t    settings;   /* link policy settings */
} __PACKED_POST__ hci_write_default_link_policy_settings_cp;

typedef hci_status_rp   hci_write_default_link_policy_settings_rp;

#define HCI_OCF_FLOW_SPECIFICATION                      0x0010
#define HCI_CMD_FLOW_SPECIFICATION                      0x0810
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
    uint8_t     flags;          /* reserved */
    uint8_t     flow_direction;
    uint8_t     service_type;
    uint32_t    token_rate;
    uint32_t    token_bucket;
    uint32_t    peak_bandwidth;
    uint32_t    latency;
} __PACKED_POST__ hci_flow_specification_cp;
/* No return parameter(s) */

#define HCI_OCF_SNIFF_SUBRATING                         0x0011
#define HCI_CMD_SNIFF_SUBRATING                         0x0810
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
    uint16_t    max_latency;
    uint16_t    max_timeout;    /* max remote timeout */
    uint16_t    min_timeout;    /* min local timeout */
} __PACKED_POST__ hci_sniff_subrating_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
} __PACKED_POST__ hci_sniff_subrating_rp;

/**************************************************************************
 ** OGF 0x03    Host Controller and Baseband commands and return parameters
 **************************************************************************/

#define HCI_OGF_HC_BASEBAND                     0x03

#define HCI_OCF_SET_EVENT_MASK                          0x0001
#define HCI_CMD_SET_EVENT_MASK                          0x0C01
typedef   struct {
    uint8_t     event_mask[HCI_EVENT_MASK_SIZE]; /* event_mask */
} __PACKED_POST__ hci_set_event_mask_cp;

typedef hci_status_rp   hci_set_event_mask_rp;

#define HCI_OCF_RESET                                   0x0003
#define HCI_CMD_RESET                                   0x0C03
/* No command parameter(s) */
typedef hci_status_rp   hci_reset_rp;

#define HCI_OCF_SET_EVENT_FILTER                        0x0005
#define HCI_CMD_SET_EVENT_FILTER                        0x0C05
typedef   struct {
    uint8_t     filter_type;           /* filter type */
    uint8_t     filter_condition_type; /* filter condition type */
    /* variable size condition
       uint8_t  condition[]; -- conditions */
} __PACKED_POST__ hci_set_event_filter_cp;

typedef hci_status_rp   hci_set_event_filter_rp;

#define HCI_OCF_FLUSH                                   0x0008
#define HCI_CMD_FLUSH                                   0x0C08
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_flush_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_flush_rp;

#define HCI_OCF_READ_PIN_TYPE                           0x0009
#define HCI_CMD_READ_PIN_TYPE                           0x0C09
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;   /* 0x00 - success */
    uint8_t     pin_type; /* PIN type */
} __PACKED_POST__ hci_read_pin_type_rp;

#define HCI_OCF_WRITE_PIN_TYPE                          0x000a
#define HCI_CMD_WRITE_PIN_TYPE                          0x0C0A
typedef   struct {
    uint8_t     pin_type; /* PIN type */
} __PACKED_POST__ hci_write_pin_type_cp;

typedef hci_status_rp   hci_write_pin_type_rp;

#define HCI_OCF_CREATE_NEW_UNIT_KEY                     0x000b
#define HCI_CMD_CREATE_NEW_UNIT_KEY                     0x0C0B
/* No command parameter(s) */
typedef hci_status_rp   hci_create_new_unit_key_rp;

#define HCI_OCF_READ_STORED_LINK_KEY                    0x000d
#define HCI_CMD_READ_STORED_LINK_KEY                    0x0C0D
typedef   struct {
    btaddr_t    btaddr;   /* address */
    uint8_t     read_all; /* read all keys? 0x01 - yes */
} __PACKED_POST__ hci_read_stored_link_key_cp;

typedef   struct {
    uint8_t     status;        /* 0x00 - success */
    uint16_t    max_num_keys;  /* Max. number of keys */
    uint16_t    num_keys_read; /* Number of stored keys */
} __PACKED_POST__ hci_read_stored_link_key_rp;

#define HCI_OCF_WRITE_STORED_LINK_KEY                   0x0011
#define HCI_CMD_WRITE_STORED_LINK_KEY                   0x0C11
typedef   struct {
    uint8_t     num_keys_write; /* # of keys to write */
    /* these are repeated "num_keys_write" times
       btaddr_t btaddr;             --- remote address(es)
       uint8_t  key[HCI_KEY_SIZE];  --- key(s) */
} __PACKED_POST__ hci_write_stored_link_key_cp;

typedef   struct {
    uint8_t     status;           /* 0x00 - success */
    uint8_t     num_keys_written; /* # of keys successfully written */
} __PACKED_POST__ hci_write_stored_link_key_rp;

#define HCI_OCF_DELETE_STORED_LINK_KEY                  0x0012
#define HCI_CMD_DELETE_STORED_LINK_KEY                  0x0C12
typedef   struct {
    btaddr_t    btaddr;     /* address */
    uint8_t     delete_all; /* delete all keys? 0x01 - yes */
} __PACKED_POST__ hci_delete_stored_link_key_cp;

typedef   struct {
    uint8_t     status;           /* 0x00 - success */
    uint16_t    num_keys_deleted; /* Number of keys deleted */
} __PACKED_POST__ hci_delete_stored_link_key_rp;

#define HCI_OCF_WRITE_LOCAL_NAME                        0x0013
#define HCI_CMD_WRITE_LOCAL_NAME                        0x0C13
typedef   struct {
    char                name[HCI_UNIT_NAME_SIZE]; /* new unit name */
} __PACKED_POST__ hci_write_local_name_cp;

typedef hci_status_rp   hci_write_local_name_rp;

#define HCI_OCF_READ_LOCAL_NAME                         0x0014
#define HCI_CMD_READ_LOCAL_NAME                         0x0C14
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;                   /* 0x00 - success */
    char                name[HCI_UNIT_NAME_SIZE]; /* unit name */
} __PACKED_POST__ hci_read_local_name_rp;

#define HCI_OCF_READ_CON_ACCEPT_TIMEOUT                 0x0015
#define HCI_CMD_READ_CON_ACCEPT_TIMEOUT                 0x0C15
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;  /* 0x00 - success */
    uint16_t    timeout; /* (timeout * 0.625) msec */
} __PACKED_POST__ hci_read_con_accept_timeout_rp;

#define HCI_OCF_WRITE_CON_ACCEPT_TIMEOUT                0x0016
#define HCI_CMD_WRITE_CON_ACCEPT_TIMEOUT                0x0C16
typedef   struct {
    uint16_t    timeout; /* (timeout * 0.625) msec */
} __PACKED_POST__ hci_write_con_accept_timeout_cp;

typedef hci_status_rp   hci_write_con_accept_timeout_rp;

#define HCI_OCF_READ_PAGE_TIMEOUT                       0x0017
#define HCI_CMD_READ_PAGE_TIMEOUT                       0x0C17
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;  /* 0x00 - success */
    uint16_t    timeout; /* (timeout * 0.625) msec */
} __PACKED_POST__ hci_read_page_timeout_rp;

#define HCI_OCF_WRITE_PAGE_TIMEOUT                      0x0018
#define HCI_CMD_WRITE_PAGE_TIMEOUT                      0x0C18
typedef   struct {
    uint16_t    timeout; /* (timeout * 0.625) msec */
} __PACKED_POST__ hci_write_page_timeout_cp;

typedef hci_status_rp   hci_write_page_timeout_rp;

#define HCI_OCF_READ_SCAN_ENABLE                        0x0019
#define HCI_CMD_READ_SCAN_ENABLE                        0x0C19
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;      /* 0x00 - success */
    uint8_t     scan_enable; /* Scan enable */
} __PACKED_POST__ hci_read_scan_enable_rp;

#define HCI_OCF_WRITE_SCAN_ENABLE                       0x001a
#define HCI_CMD_WRITE_SCAN_ENABLE                       0x0C1A

#define SCAN_DISABLE                                    0x000
#define SCAN_ENABLE_JUST_INQUIRY                        0x001
#define SCAN_ENABLE_JUST_PAGE                           0x002
#define SCAN_ENABLE_INQUIRY_PAGE                        0x003

typedef   struct {
    uint8_t     scan_enable; /* Scan enable */
} __PACKED_POST__ hci_write_scan_enable_cp;

typedef hci_status_rp   hci_write_scan_enable_rp;

#define HCI_OCF_READ_PAGE_SCAN_ACTIVITY                 0x001b
#define HCI_CMD_READ_PAGE_SCAN_ACTIVITY                 0x0C1B
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;             /* 0x00 - success */
    uint16_t    page_scan_interval; /* interval * 0.625 msec */
    uint16_t    page_scan_window;   /* window * 0.625 msec */
} __PACKED_POST__ hci_read_page_scan_activity_rp;

#define HCI_OCF_WRITE_PAGE_SCAN_ACTIVITY                0x001c
#define HCI_CMD_WRITE_PAGE_SCAN_ACTIVITY                0x0C1C
typedef   struct {
    uint16_t    page_scan_interval; /* interval * 0.625 msec */
    uint16_t    page_scan_window;   /* window * 0.625 msec */
} __PACKED_POST__ hci_write_page_scan_activity_cp;

typedef hci_status_rp   hci_write_page_scan_activity_rp;

#define HCI_OCF_READ_INQUIRY_SCAN_ACTIVITY              0x001d
#define HCI_CMD_READ_INQUIRY_SCAN_ACTIVITY              0x0C1D
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;                /* 0x00 - success */
    uint16_t    inquiry_scan_interval; /* interval * 0.625 msec */
    uint16_t    inquiry_scan_window;   /* window * 0.625 msec */
} __PACKED_POST__ hci_read_inquiry_scan_activity_rp;

#define HCI_OCF_WRITE_INQUIRY_SCAN_ACTIVITY             0x001e
#define HCI_CMD_WRITE_INQUIRY_SCAN_ACTIVITY             0x0C1E
typedef   struct {
    uint16_t    inquiry_scan_interval; /* interval * 0.625 msec */
    uint16_t    inquiry_scan_window;   /* window * 0.625 msec */
} __PACKED_POST__ hci_write_inquiry_scan_activity_cp;

typedef hci_status_rp   hci_write_inquiry_scan_activity_rp;

#define HCI_OCF_READ_AUTH_ENABLE                        0x001f
#define HCI_CMD_READ_AUTH_ENABLE                        0x0C1F
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;      /* 0x00 - success */
    uint8_t     auth_enable; /* 0x01 - enabled */
} __PACKED_POST__ hci_read_auth_enable_rp;

#define HCI_OCF_WRITE_AUTH_ENABLE                       0x0020
#define HCI_CMD_WRITE_AUTH_ENABLE                       0x0C20
typedef   struct {
    uint8_t     auth_enable; /* 0x01 - enabled */
} __PACKED_POST__ hci_write_auth_enable_cp;

typedef hci_status_rp   hci_write_auth_enable_rp;

/* Read Encryption Mode is deprecated */
#define HCI_OCF_READ_ENCRYPTION_MODE                    0x0021
#define HCI_CMD_READ_ENCRYPTION_MODE                    0x0C21
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;          /* 0x00 - success */
    uint8_t     encryption_mode; /* encryption mode */
} __PACKED_POST__ hci_read_encryption_mode_rp;

/* Write Encryption Mode is deprecated */
#define HCI_OCF_WRITE_ENCRYPTION_MODE                   0x0022
#define HCI_CMD_WRITE_ENCRYPTION_MODE                   0x0C22
typedef   struct {
    uint8_t     encryption_mode; /* encryption mode */
} __PACKED_POST__ hci_write_encryption_mode_cp;

typedef hci_status_rp   hci_write_encryption_mode_rp;

#define HCI_OCF_READ_UNIT_CLASS                         0x0023
#define HCI_CMD_READ_UNIT_CLASS                         0x0C23
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;                 /* 0x00 - success */
    btclass_t   uclass;                 /* unit class */
} __PACKED_POST__ hci_read_unit_class_rp;

#define HCI_OCF_WRITE_UNIT_CLASS                        0x0024
#define HCI_CMD_WRITE_UNIT_CLASS                        0x0C24
typedef   struct {
    btclass_t   uclass;                 /* unit class */
} __PACKED_POST__ hci_write_unit_class_cp;

typedef hci_status_rp   hci_write_unit_class_rp;

#define HCI_OCF_READ_VOICE_SETTING                      0x0025
#define HCI_CMD_READ_VOICE_SETTING                      0x0C25
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;   /* 0x00 - success */
    uint16_t    settings; /* voice settings */
} __PACKED_POST__ hci_read_voice_setting_rp;

#define HCI_OCF_WRITE_VOICE_SETTING                     0x0026
#define HCI_CMD_WRITE_VOICE_SETTING                     0x0C26
typedef   struct {
    uint16_t    settings; /* voice settings */
} __PACKED_POST__ hci_write_voice_setting_cp;

typedef hci_status_rp   hci_write_voice_setting_rp;

#define HCI_OCF_READ_AUTO_FLUSH_TIMEOUT                 0x0027
#define HCI_CMD_READ_AUTO_FLUSH_TIMEOUT                 0x0C27
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_auto_flush_timeout_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint16_t    timeout;    /* 0x00 - no flush, timeout * 0.625 msec */
} __PACKED_POST__ hci_read_auto_flush_timeout_rp;

#define HCI_OCF_WRITE_AUTO_FLUSH_TIMEOUT                0x0028
#define HCI_CMD_WRITE_AUTO_FLUSH_TIMEOUT                0x0C28
typedef   struct {
    uint16_t    con_handle; /* connection handle */
    uint16_t    timeout;    /* 0x00 - no flush, timeout * 0.625 msec */
} __PACKED_POST__ hci_write_auto_flush_timeout_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_write_auto_flush_timeout_rp;

#define HCI_OCF_READ_NUM_BROADCAST_RETRANS              0x0029
#define HCI_CMD_READ_NUM_BROADCAST_RETRANS              0x0C29
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;  /* 0x00 - success */
    uint8_t     counter; /* number of broadcast retransmissions */
} __PACKED_POST__ hci_read_num_broadcast_retrans_rp;

#define HCI_OCF_WRITE_NUM_BROADCAST_RETRANS             0x002a
#define HCI_CMD_WRITE_NUM_BROADCAST_RETRANS             0x0C2A
typedef   struct {
    uint8_t     counter; /* number of broadcast retransmissions */
} __PACKED_POST__ hci_write_num_broadcast_retrans_cp;

typedef hci_status_rp   hci_write_num_broadcast_retrans_rp;

#define HCI_OCF_READ_HOLD_MODE_ACTIVITY                 0x002b
#define HCI_CMD_READ_HOLD_MODE_ACTIVITY                 0x0C2B
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;             /* 0x00 - success */
    uint8_t     hold_mode_activity; /* Hold mode activities */
} __PACKED_POST__ hci_read_hold_mode_activity_rp;

#define HCI_OCF_WRITE_HOLD_MODE_ACTIVITY                0x002c
#define HCI_CMD_WRITE_HOLD_MODE_ACTIVITY                0x0C2C
typedef   struct {
    uint8_t     hold_mode_activity; /* Hold mode activities */
} __PACKED_POST__ hci_write_hold_mode_activity_cp;

typedef hci_status_rp   hci_write_hold_mode_activity_rp;

#define HCI_OCF_READ_XMIT_LEVEL                         0x002d
#define HCI_CMD_READ_XMIT_LEVEL                         0x0C2D
typedef   struct {
    uint16_t    con_handle; /* connection handle */
    uint8_t     type;       /* Xmit level type */
} __PACKED_POST__ hci_read_xmit_level_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    char                level;      /* -30 <= level <= 30 dBm */
} __PACKED_POST__ hci_read_xmit_level_rp;

#define HCI_OCF_READ_SCO_FLOW_CONTROL                   0x002e
#define HCI_CMD_READ_SCO_FLOW_CONTROL                   0x0C2E
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;       /* 0x00 - success */
    uint8_t     flow_control; /* 0x00 - disabled */
} __PACKED_POST__ hci_read_sco_flow_control_rp;

#define HCI_OCF_WRITE_SCO_FLOW_CONTROL                  0x002f
#define HCI_CMD_WRITE_SCO_FLOW_CONTROL                  0x0C2F
typedef   struct {
    uint8_t     flow_control; /* 0x00 - disabled */
} __PACKED_POST__ hci_write_sco_flow_control_cp;

typedef hci_status_rp   hci_write_sco_flow_control_rp;

#define HCI_OCF_HC2H_FLOW_CONTROL                       0x0031
#define HCI_CMD_HC2H_FLOW_CONTROL                       0x0C31
typedef   struct {
    uint8_t     hc2h_flow; /* Host Controller to Host flow control */
} __PACKED_POST__ hci_hc2h_flow_control_cp;

typedef hci_status_rp   hci_h2hc_flow_control_rp;

#define HCI_OCF_HOST_BUFFER_SIZE                        0x0033
#define HCI_CMD_HOST_BUFFER_SIZE                        0x0C33
typedef   struct {
    uint16_t    max_acl_size; /* Max. size of ACL packet (bytes) */
    uint8_t     max_sco_size; /* Max. size of SCO packet (bytes) */
    uint16_t    num_acl_pkts;  /* Max. number of ACL packets */
    uint16_t    num_sco_pkts;  /* Max. number of SCO packets */
} __PACKED_POST__ hci_host_buffer_size_cp;

typedef hci_status_rp   hci_host_buffer_size_rp;

#define HCI_OCF_HOST_NUM_COMPL_PKTS                     0x0035
#define HCI_CMD_HOST_NUM_COMPL_PKTS                     0x0C35
typedef   struct {
    uint8_t     nu_con_handles; /* # of connection handles */
    /* these are repeated "num_con_handles" times
       uint16_t con_handle;    --- connection handle(s)
       uint16_t compl_pkts;    --- # of completed packets */
} __PACKED_POST__ hci_host_num_compl_pkts_cp;
/* No return parameter(s) */

#define HCI_OCF_READ_LINK_SUPERVISION_TIMEOUT           0x0036
#define HCI_CMD_READ_LINK_SUPERVISION_TIMEOUT           0x0C36
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_link_supervision_timeout_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint16_t    timeout;    /* Link supervision timeout * 0.625 msec */
} __PACKED_POST__ hci_read_link_supervision_timeout_rp;

#define HCI_OCF_WRITE_LINK_SUPERVISION_TIMEOUT          0x0037
#define HCI_CMD_WRITE_LINK_SUPERVISION_TIMEOUT          0x0C37
typedef   struct {
    uint16_t    con_handle; /* connection handle */
    uint16_t    timeout;    /* Link supervision timeout * 0.625 msec */
} __PACKED_POST__ hci_write_link_supervision_timeout_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_write_link_supervision_timeout_rp;

#define HCI_OCF_READ_NUM_SUPPORTED_IAC                  0x0038
#define HCI_CMD_READ_NUM_SUPPORTED_IAC                  0x0C38
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;  /* 0x00 - success */
    uint8_t     num_iac; /* # of supported IAC during scan */
} __PACKED_POST__ hci_read_num_supported_iac_rp;

#define HCI_OCF_READ_IAC_LAP                            0x0039
#define HCI_CMD_READ_IAC_LAP                            0x0C39
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;  /* 0x00 - success */
    uint8_t     num_iac; /* # of IAC */
    /* these are repeated "num_iac" times
       uint8_t  laps[HCI_LAP_SIZE]; --- LAPs */
} __PACKED_POST__ hci_read_iac_lap_rp;

#define HCI_OCF_WRITE_IAC_LAP                           0x003a
#define HCI_CMD_WRITE_IAC_LAP                           0x0C3A
typedef   struct {
    uint8_t     num_iac; /* # of IAC */
    /* these are repeated "num_iac" times
       uint8_t  laps[HCI_LAP_SIZE]; --- LAPs */
} __PACKED_POST__ hci_write_iac_lap_cp;

typedef hci_status_rp   hci_write_iac_lap_rp;

/* Read Page Scan Period Mode is deprecated */
#define HCI_OCF_READ_PAGE_SCAN_PERIOD                   0x003b
#define HCI_CMD_READ_PAGE_SCAN_PERIOD                   0x0C3B
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;                /* 0x00 - success */
    uint8_t     page_scan_period_mode; /* Page scan period mode */
} __PACKED_POST__ hci_read_page_scan_period_rp;

/* Write Page Scan Period Mode is deprecated */
#define HCI_OCF_WRITE_PAGE_SCAN_PERIOD                  0x003c
#define HCI_CMD_WRITE_PAGE_SCAN_PERIOD                  0x0C3C
typedef   struct {
    uint8_t     page_scan_period_mode; /* Page scan period mode */
} __PACKED_POST__ hci_write_page_scan_period_cp;

typedef hci_status_rp   hci_write_page_scan_period_rp;

/* Read Page Scan Mode is deprecated */
#define HCI_OCF_READ_PAGE_SCAN                          0x003d
#define HCI_CMD_READ_PAGE_SCAN                          0x0C3D
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     page_scan_mode; /* Page scan mode */
} __PACKED_POST__ hci_read_page_scan_rp;

/* Write Page Scan Mode is deprecated */
#define HCI_OCF_WRITE_PAGE_SCAN                         0x003e
#define HCI_CMD_WRITE_PAGE_SCAN                         0x0C3E
typedef   struct {
    uint8_t     page_scan_mode; /* Page scan mode */
} __PACKED_POST__ hci_write_page_scan_cp;

typedef hci_status_rp   hci_write_page_scan_rp;

#define HCI_OCF_SET_AFH_CLASSIFICATION                  0x003f
#define HCI_CMD_SET_AFH_CLASSIFICATION                  0x0C3F
typedef   struct {
    uint8_t     classification[10];
} __PACKED_POST__ hci_set_afh_classification_cp;

typedef hci_status_rp   hci_set_afh_classification_rp;

#define HCI_OCF_READ_INQUIRY_SCAN_TYPE                  0x0042
#define HCI_CMD_READ_INQUIRY_SCAN_TYPE                  0x0C42
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     type;           /* inquiry scan type */
} __PACKED_POST__ hci_read_inquiry_scan_type_rp;

#define HCI_OCF_WRITE_INQUIRY_SCAN_TYPE                 0x0043
#define HCI_CMD_WRITE_INQUIRY_SCAN_TYPE                 0x0C43
typedef   struct {
    uint8_t     type;           /* inquiry scan type */
} __PACKED_POST__ hci_write_inquiry_scan_type_cp;

typedef hci_status_rp   hci_write_inquiry_scan_type_rp;

#define HCI_OCF_READ_INQUIRY_MODE                       0x0044
#define HCI_CMD_READ_INQUIRY_MODE                       0x0C44
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     mode;           /* inquiry mode */
} __PACKED_POST__ hci_read_inquiry_mode_rp;

#define HCI_OCF_WRITE_INQUIRY_MODE                      0x0045
#define HCI_CMD_WRITE_INQUIRY_MODE                      0x0C45
typedef   struct {
    uint8_t     mode;           /* inquiry mode */
} __PACKED_POST__ hci_write_inquiry_mode_cp;

typedef hci_status_rp   hci_write_inquiry_mode_rp;

#define HCI_OCF_READ_PAGE_SCAN_TYPE                     0x0046
#define HCI_CMD_READ_PAGE_SCAN_TYPE                     0x0C46
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     type;           /* page scan type */
} __PACKED_POST__ hci_read_page_scan_type_rp;

#define HCI_OCF_WRITE_PAGE_SCAN_TYPE                    0x0047
#define HCI_CMD_WRITE_PAGE_SCAN_TYPE                    0x0C47
typedef   struct {
    uint8_t     type;           /* page scan type */
} __PACKED_POST__ hci_write_page_scan_type_cp;

typedef hci_status_rp   hci_write_page_scan_type_rp;

#define HCI_OCF_READ_AFH_ASSESSMENT                     0x0048
#define HCI_CMD_READ_AFH_ASSESSMENT                     0x0C48
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     mode;           /* assessment mode */
} __PACKED_POST__ hci_read_afh_assessment_rp;

#define HCI_OCF_WRITE_AFH_ASSESSMENT                    0x0049
#define HCI_CMD_WRITE_AFH_ASSESSMENT                    0x0C49
typedef   struct {
    uint8_t     mode;           /* assessment mode */
} __PACKED_POST__ hci_write_afh_assessment_cp;

typedef hci_status_rp   hci_write_afh_assessment_rp;

#define HCI_OCF_READ_EXTENDED_INQUIRY_RSP               0x0051
#define HCI_CMD_READ_EXTENDED_INQUIRY_RSP               0x0C51
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     fec_required;
    uint8_t     response[HCI_MAX_EXTENDED_INQ_RSP_LEN];
} __PACKED_POST__ hci_read_extended_inquiry_rsp_rp;

#define HCI_OCF_WRITE_EXTENDED_INQUIRY_RSP              0x0052
#define HCI_CMD_WRITE_EXTENDED_INQUIRY_RSP              0x0C52
typedef   struct {
    uint8_t     fec_required;
    uint8_t     response[HCI_MAX_EXTENDED_INQ_RSP_LEN];
} __PACKED_POST__ hci_write_extended_inquiry_rsp_cp;

typedef hci_status_rp   hci_write_extended_inquiry_rsp_rp;

#define HCI_OCF_REFRESH_ENCRYPTION_KEY                  0x0053
#define HCI_CMD_REFRESH_ENCRYPTION_KEY                  0x0C53
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
} __PACKED_POST__ hci_refresh_encryption_key_cp;

typedef hci_status_rp   hci_refresh_encryption_key_rp;

#define HCI_OCF_READ_SIMPLE_PAIRING_MODE                0x0055
#define HCI_CMD_READ_SIMPLE_PAIRING_MODE                0x0C55
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     mode;           /* simple pairing mode */
} __PACKED_POST__ hci_read_simple_pairing_mode_rp;

#define HCI_OCF_WRITE_SIMPLE_PAIRING_MODE               0x0056
#define HCI_CMD_WRITE_SIMPLE_PAIRING_MODE               0x0C56
typedef   struct {
    uint8_t     mode;           /* simple pairing mode */
} __PACKED_POST__ hci_write_simple_pairing_mode_cp;

typedef hci_status_rp   hci_write_simple_pairing_mode_rp;

#define HCI_OCF_READ_LOCAL_OOB_DATA                     0x0057
#define HCI_CMD_READ_LOCAL_OOB_DATA                     0x0C57
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     c[HCI_PAIRING_SIZE];    /* pairing hash */
    uint8_t     r[HCI_PAIRING_SIZE];    /* pairing randomizer */
} __PACKED_POST__ hci_read_local_oob_data_rp;

#define HCI_OCF_READ_INQUIRY_RSP_XMIT_POWER             0x0058
#define HCI_CMD_READ_INQUIRY_RSP_XMIT_POWER             0x0C58
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    int8_t      power;          /* TX power */
} __PACKED_POST__ hci_read_inquiry_rsp_xmit_power_rp;

#define HCI_OCF_WRITE_INQUIRY_RSP_XMIT_POWER            0x0059
#define HCI_CMD_WRITE_INQUIRY_RSP_XMIT_POWER            0x0C59
typedef   struct {
    int8_t      power;          /* TX power */
} __PACKED_POST__ hci_write_inquiry_rsp_xmit_power_cp;

typedef hci_status_rp   hci_write_inquiry_rsp_xmit_power_rp;

#define HCI_OCF_READ_DEFAULT_ERRDATA_REPORTING          0x005A
#define HCI_CMD_READ_DEFAULT_ERRDATA_REPORTING          0x0C5A
/* No command parameter(s) */

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     reporting;      /* erroneous data reporting */
} __PACKED_POST__ hci_read_default_errdata_reporting_rp;

#define HCI_OCF_WRITE_DEFAULT_ERRDATA_REPORTING         0x005B
#define HCI_CMD_WRITE_DEFAULT_ERRDATA_REPORTING         0x0C5B
typedef   struct {
    uint8_t     reporting;      /* erroneous data reporting */
} __PACKED_POST__ hci_write_default_errdata_reporting_cp;

typedef hci_status_rp   hci_write_default_errdata_reporting_rp;

#define HCI_OCF_ENHANCED_FLUSH                          0x005F
#define HCI_CMD_ENHANCED_FLUSH                          0x0C5F
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
    uint8_t     packet_type;
} __PACKED_POST__ hci_enhanced_flush_cp;

/* No response parameter(s) */

#define HCI_OCF_SEND_KEYPRESS_NOTIFICATION              0x0060
#define HCI_CMD_SEND_KEYPRESS_NOTIFICATION              0x0C60
typedef   struct {
    btaddr_t    btaddr;         /* remote address */
    uint8_t     type;           /* notification type */
} __PACKED_POST__ hci_send_keypress_notification_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote address */
} __PACKED_POST__ hci_send_keypress_notification_rp;

/**************************************************************************
 ** OGF 0x04    Informational commands and return parameters
 **************************************************************************/

#define HCI_OGF_INFO                            0x04

#define HCI_OCF_READ_LOCAL_VER                          0x0001
#define HCI_CMD_READ_LOCAL_VER                          0x1001
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     hci_version;    /* HCI version */
    uint16_t    hci_revision;   /* HCI revision */
    uint8_t     lmp_version;    /* LMP version */
    uint16_t    manufacturer;   /* Hardware manufacturer name */
    uint16_t    lmp_subversion; /* LMP sub-version */
} __PACKED_POST__ hci_read_local_ver_rp;

#define HCI_OCF_READ_LOCAL_COMMANDS                     0x0002
#define HCI_CMD_READ_LOCAL_COMMANDS                     0x1002
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     commands[HCI_COMMANDS_SIZE];    /* opcode bitmask */
} __PACKED_POST__ hci_read_local_commands_rp;

#define HCI_OCF_READ_LOCAL_FEATURES                     0x0003
#define HCI_CMD_READ_LOCAL_FEATURES                     0x1003
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;                      /* 0x00 - success */
    uint8_t     features[HCI_FEATURES_SIZE]; /* LMP features bitmask*/
} __PACKED_POST__ hci_read_local_features_rp;

#define HCI_OCF_READ_LOCAL_EXTENDED_FEATURES            0x0004
#define HCI_CMD_READ_LOCAL_EXTENDED_FEATURES            0x1004
typedef   struct {
    uint8_t     page;           /* page number */
} __PACKED_POST__ hci_read_local_extended_features_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint8_t     page;           /* page number */
    uint8_t     max_page;       /* maximum page number */
    uint8_t     features[HCI_FEATURES_SIZE];    /* LMP features */
} __PACKED_POST__ hci_read_local_extended_features_rp;

#define HCI_OCF_READ_BUFFER_SIZE                        0x0005
#define HCI_CMD_READ_BUFFER_SIZE                        0x1005
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;       /* 0x00 - success */
    uint16_t    max_acl_size; /* Max. size of ACL packet (bytes) */
    uint8_t     max_sco_size; /* Max. size of SCO packet (bytes) */
    uint16_t    num_acl_pkts;  /* Max. number of ACL packets */
    uint16_t    num_sco_pkts;  /* Max. number of SCO packets */
} __PACKED_POST__ hci_read_buffer_size_rp;

/* Read Country Code is deprecated */
#define HCI_OCF_READ_COUNTRY_CODE                       0x0007
#define HCI_CMD_READ_COUNTRY_CODE                       0x1007
/* No command parameter(s) */
typedef   struct {
    uint8_t     status;       /* 0x00 - success */
    uint8_t     country_code; /* 0x00 - NAM, EUR, JP; 0x01 - France */
} __PACKED_POST__ hci_read_country_code_rp;

#define HCI_OCF_READ_BTADDR                             0x0009
#define HCI_CMD_READ_BTADDR                             0x1009
/* No command parameter(s) */
typedef   struct {
    uint8_t     status; /* 0x00 - success */
    btaddr_t    btaddr; /* unit address */
} __PACKED_POST__ hci_read_btaddr_rp;

/**************************************************************************
 ** OGF 0x05    Status commands and return parameters
 **************************************************************************/

#define HCI_OGF_STATUS                          0x05

#define HCI_OCF_READ_FAILED_CONTACT_CNTR                0x0001
#define HCI_CMD_READ_FAILED_CONTACT_CNTR                0x1401
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_failed_contact_cntr_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint16_t    counter;    /* number of consecutive failed contacts */
} __PACKED_POST__ hci_read_failed_contact_cntr_rp;

#define HCI_OCF_RESET_FAILED_CONTACT_CNTR               0x0002
#define HCI_CMD_RESET_FAILED_CONTACT_CNTR               0x1402
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_reset_failed_contact_cntr_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_reset_failed_contact_cntr_rp;

#define HCI_OCF_READ_LINK_QUALITY                       0x0003
#define HCI_CMD_READ_LINK_QUALITY                       0x1403
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_link_quality_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint8_t     quality;    /* higher value means better quality */
} __PACKED_POST__ hci_read_link_quality_rp;

#define HCI_OCF_READ_RSSI                               0x0005
#define HCI_CMD_READ_RSSI                               0x1405
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_rssi_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    char                rssi;       /* -127 <= rssi <= 127 dB */
} __PACKED_POST__ hci_read_rssi_rp;

#define HCI_OCF_READ_AFH_CHANNEL_MAP                    0x0006
#define HCI_CMD_READ_AFH_CHANNEL_MAP                    0x1406
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_read_afh_channel_map_cp;

typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint8_t     mode;       /* AFH mode */
    uint8_t     map[10];    /* AFH Channel Map */
} __PACKED_POST__ hci_read_afh_channel_map_rp;

#define HCI_OCF_READ_CLOCK                              0x0007
#define HCI_CMD_READ_CLOCK                              0x1407
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
    uint8_t     clock;          /* which clock */
} __PACKED_POST__ hci_read_clock_cp;

typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
    uint32_t    clock;          /* clock value */
    uint16_t    accuracy;       /* clock accuracy */
} __PACKED_POST__ hci_read_clock_rp;


/**************************************************************************
 ** OGF 0x06    Testing commands and return parameters
 **************************************************************************/

#define HCI_OGF_TESTING                         0x06

#define HCI_OCF_READ_LOOPBACK_MODE                      0x0001
#define HCI_CMD_READ_LOOPBACK_MODE                      0x1801
/* No command parameter(s) */
typedef   struct {
    uint8_t     status; /* 0x00 - success */
    uint8_t     lbmode; /* loopback mode */
} __PACKED_POST__ hci_read_loopback_mode_rp;

#define HCI_OCF_WRITE_LOOPBACK_MODE                     0x0002
#define HCI_CMD_WRITE_LOOPBACK_MODE                     0x1802
typedef   struct {
    uint8_t     lbmode; /* loopback mode */
} __PACKED_POST__ hci_write_loopback_mode_cp;

typedef hci_status_rp   hci_write_loopback_mode_rp;

#define HCI_OCF_ENABLE_UNIT_UNDER_TEST                  0x0003
#define HCI_CMD_ENABLE_UNIT_UNDER_TEST                  0x1803
/* No command parameter(s) */
typedef hci_status_rp   hci_enable_unit_under_test_rp;

#define HCI_OCF_WRITE_SIMPLE_PAIRING_DEBUG_MODE         0x0004
#define HCI_CMD_WRITE_SIMPLE_PAIRING_DEBUG_MODE         0x1804
typedef   struct {
    uint8_t     mode;   /* simple pairing debug mode */
} __PACKED_POST__ hci_write_simple_pairing_debug_mode_cp;

typedef hci_status_rp   hci_write_simple_pairing_debug_mode_rp;

/**************************************************************************
 ** OGF 0x3e    Bluetooth Logo Testing
 ** OGF 0x3f    Vendor Specific
 **************************************************************************/

#define HCI_OGF_BT_LOGO                         0x3e
#define HCI_OGF_VENDOR                          0x3f

/* Ericsson specific FC */
#define HCI_CMD_ERICSSON_WRITE_PCM_SETTINGS             0xFC07
#define HCI_CMD_ERICSSON_SET_UART_BAUD_RATE             0xFC09
#define HCI_CMD_ERICSSON_SET_SCO_DATA_PATH              0xFC1D

/* Cambridge Silicon Radio specific FC */
#define HCI_CMD_CSR_EXTN                                0xFC00


/**************************************************************************
 **                         Events and event parameters
 **************************************************************************/

#define HCI_EVENT_INQUIRY_COMPL                 0x01
typedef   struct {
    uint8_t     status; /* 0x00 - success */
} __PACKED_POST__ hci_inquiry_compl_ep;

#define HCI_EVENT_INQUIRY_RESULT                0x02
typedef   struct {
    uint8_t     num_responses;      /* number of responses */
    /*  hci_inquiry_result[num_responses]   -- see below */
} __PACKED_POST__ hci_inquiry_result_ep;

typedef   struct {
    btaddr_t    btaddr;                   /* unit address */
    uint8_t     page_scan_rep_mode;       /* page scan rep. mode */
    uint8_t     page_scan_period_mode;    /* page scan period mode */
    uint8_t     page_scan_mode;           /* page scan mode */
    btclass_t   uclass;                   /* unit class */
    uint16_t    clock_offset;             /* clock offset */
} __PACKED_POST__ hci_inquiry_result;

#define HCI_EVENT_CON_COMPL                     0x03
typedef   struct {
    uint8_t     status;          /* 0x00 - success */
    uint16_t    con_handle;      /* Connection handle */
    btaddr_t    btaddr;          /* remote unit address */
    uint8_t     link_type;       /* Link type */
    uint8_t     encryption_mode; /* Encryption mode */
} __PACKED_POST__ hci_con_compl_ep;

#define HCI_EVENT_CON_REQ                       0x04
typedef   struct {
    btaddr_t    btaddr;                 /* remote unit address */
    btclass_t   uclass;                 /* remote unit class */
    uint8_t     link_type;              /* link type */
} __PACKED_POST__ hci_con_req_ep;

#define HCI_EVENT_DISCON_COMPL                  0x05
typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint8_t     reason;     /* reason to disconnect */
} __PACKED_POST__ hci_discon_compl_ep;

#define HCI_EVENT_AUTH_COMPL                    0x06
typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_auth_compl_ep;

#define HCI_EVENT_REMOTE_NAME_REQ_COMPL         0x07
typedef   struct {
    uint8_t     status;                   /* 0x00 - success */
    btaddr_t    btaddr;                   /* remote unit address */
    char        name[HCI_UNIT_NAME_SIZE]; /* remote unit name */
} __PACKED_POST__ hci_remote_name_req_compl_ep;

#define HCI_EVENT_ENCRYPTION_CHANGE             0x08
typedef   struct {
    uint8_t     status;            /* 0x00 - success */
    uint16_t    con_handle;        /* Connection handle */
    uint8_t     encryption_enable; /* 0x00 - disable */
} __PACKED_POST__ hci_encryption_change_ep;

#define HCI_EVENT_CHANGE_CON_LINK_KEY_COMPL     0x09
typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* Connection handle */
} __PACKED_POST__ hci_change_con_link_key_compl_ep;

#define HCI_EVENT_MASTER_LINK_KEY_COMPL         0x0a
typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* Connection handle */
    uint8_t     key_flag;   /* Key flag */
} __PACKED_POST__ hci_master_link_key_compl_ep;

#define HCI_EVENT_READ_REMOTE_FEATURES_COMPL    0x0b
typedef   struct {
    uint8_t     status;                      /* 0x00 - success */
    uint16_t    con_handle;                  /* Connection handle */
    uint8_t     features[HCI_FEATURES_SIZE]; /* LMP features bitmsk*/
} __PACKED_POST__ hci_read_remote_features_compl_ep;

#define HCI_EVENT_READ_REMOTE_VER_INFO_COMPL    0x0c
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* Connection handle */
    uint8_t     lmp_version;    /* LMP version */
    uint16_t    manufacturer;   /* Hardware manufacturer name */
    uint16_t    lmp_subversion; /* LMP sub-version */
} __PACKED_POST__ hci_read_remote_ver_info_compl_ep;

#define HCI_EVENT_QOS_SETUP_COMPL               0x0d
typedef   struct {
    uint8_t     status;          /* 0x00 - success */
    uint16_t    con_handle;      /* connection handle */
    uint8_t     flags;           /* reserved for future use */
    uint8_t     service_type;    /* service type */
    uint32_t    token_rate;      /* bytes per second */
    uint32_t    peak_bandwidth;  /* bytes per second */
    uint32_t    latency;         /* microseconds */
    uint32_t    delay_variation; /* microseconds */
} __PACKED_POST__ hci_qos_setup_compl_ep;

#define HCI_EVENT_COMMAND_COMPL                 0x0e
typedef   struct {
    uint8_t     num_cmd_pkts; /* # of HCI command packets */
    uint16_t    opcode;       /* command OpCode */
    /* command return parameters (if any) */
} __PACKED_POST__ hci_command_compl_ep;

#define HCI_EVENT_COMMAND_STATUS                0x0f
typedef   struct {
    uint8_t     status;       /* 0x00 - pending */
    uint8_t     num_cmd_pkts; /* # of HCI command packets */
    uint16_t    opcode;       /* command OpCode */
} __PACKED_POST__ hci_command_status_ep;

#define HCI_EVENT_HARDWARE_ERROR                0x10
typedef   struct {
    uint8_t     hardware_code; /* hardware error code */
} __PACKED_POST__ hci_hardware_error_ep;

#define HCI_EVENT_FLUSH_OCCUR                   0x11
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_flush_occur_ep;

#define HCI_EVENT_ROLE_CHANGE                   0x12
typedef   struct {
    uint8_t     status; /* 0x00 - success */
    btaddr_t    btaddr; /* address of remote unit */
    uint8_t     role;   /* new connection role */
} __PACKED_POST__ hci_role_change_ep;

#define HCI_EVENT_NUM_COMPL_PKTS                0x13
typedef   struct {
    uint8_t     num_con_handles; /* # of connection handles */
    /* these are repeated "num_con_handles" times
       uint16_t con_handle; --- connection handle(s)
       uint16_t compl_pkts; --- # of completed packets */
} __PACKED_POST__ hci_num_compl_pkts_ep;

#define HCI_EVENT_MODE_CHANGE                   0x14
typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint8_t     unit_mode;  /* remote unit mode */
    uint16_t    interval;   /* interval * 0.625 msec */
} __PACKED_POST__ hci_mode_change_ep;

#define HCI_EVENT_RETURN_LINK_KEYS              0x15
typedef   struct {
    uint8_t     num_keys; /* # of keys */
    /* these are repeated "num_keys" times
       btaddr_t btaddr;               --- remote address(es)
       uint8_t  key[HCI_KEY_SIZE]; --- key(s) */
} __PACKED_POST__ hci_return_link_keys_ep;

#define HCI_EVENT_PIN_CODE_REQ                  0x16
typedef   struct {
    btaddr_t    btaddr; /* remote unit address */
} __PACKED_POST__ hci_pin_code_req_ep;

#define HCI_EVENT_LINK_KEY_REQ                  0x17
typedef   struct {
    btaddr_t    btaddr; /* remote unit address */
} __PACKED_POST__ hci_link_key_req_ep;

#define HCI_EVENT_LINK_KEY_NOTIFICATION         0x18
typedef   struct {
    btaddr_t    btaddr;            /* remote unit address */
    uint8_t     key[HCI_KEY_SIZE]; /* link key */
    uint8_t     key_type;          /* type of the key */
} __PACKED_POST__ hci_link_key_notification_ep;

#define HCI_EVENT_LOOPBACK_COMMAND              0x19
typedef hci_cmd_hdr_t   hci_loopback_command_ep;

#define HCI_EVENT_DATA_BUFFER_OVERFLOW          0x1a
typedef   struct {
    uint8_t     link_type; /* Link type */
} __PACKED_POST__ hci_data_buffer_overflow_ep;

#define HCI_EVENT_MAX_SLOT_CHANGE               0x1b
typedef   struct {
    uint16_t    con_handle;    /* connection handle */
    uint8_t     lmp_max_slots; /* Max. # of slots allowed */
} __PACKED_POST__ hci_max_slot_change_ep;

#define HCI_EVENT_READ_CLOCK_OFFSET_COMPL       0x1c
typedef   struct {
    uint8_t     status;       /* 0x00 - success */
    uint16_t    con_handle;   /* Connection handle */
    uint16_t    clock_offset; /* Clock offset */
} __PACKED_POST__ hci_read_clock_offset_compl_ep;

#define HCI_EVENT_CON_PKT_TYPE_CHANGED          0x1d
typedef   struct {
    uint8_t     status;     /* 0x00 - success */
    uint16_t    con_handle; /* connection handle */
    uint16_t    pkt_type;   /* packet type */
} __PACKED_POST__ hci_con_pkt_type_changed_ep;

#define HCI_EVENT_QOS_VIOLATION                 0x1e
typedef   struct {
    uint16_t    con_handle; /* connection handle */
} __PACKED_POST__ hci_qos_violation_ep;

/* Page Scan Mode Change Event is deprecated */
#define HCI_EVENT_PAGE_SCAN_MODE_CHANGE         0x1f
typedef   struct {
    btaddr_t    btaddr;         /* destination address */
    uint8_t     page_scan_mode; /* page scan mode */
} __PACKED_POST__ hci_page_scan_mode_change_ep;

#define HCI_EVENT_PAGE_SCAN_REP_MODE_CHANGE     0x20
typedef   struct {
    btaddr_t    btaddr;             /* destination address */
    uint8_t     page_scan_rep_mode; /* page scan repetition mode */
} __PACKED_POST__ hci_page_scan_rep_mode_change_ep;

#define HCI_EVENT_FLOW_SPECIFICATION_COMPL      0x21
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
    uint8_t     flags;          /* reserved */
    uint8_t     direction;      /* flow direction */
    uint8_t     type;           /* service type */
    uint32_t    token_rate;     /* token rate */
    uint32_t    bucket_size;    /* token bucket size */
    uint32_t    peak_bandwidth; /* peak bandwidth */
    uint32_t    latency;        /* access latency */
} __PACKED_POST__ hci_flow_specification_compl_ep;

#define HCI_EVENT_RSSI_RESULT                   0x22
typedef   struct {
    uint8_t     num_responses;      /* number of responses */
    /*  hci_rssi_result[num_responses]   -- see below */
} __PACKED_POST__ hci_rssi_result_ep;

typedef   struct {
    btaddr_t    btaddr;                 /* unit address */
    uint8_t     page_scan_rep_mode;     /* page scan rep. mode */
    uint8_t     blank;                  /* reserved */
    btclass_t   uclass;                 /* unit class */
    uint16_t    clock_offset;           /* clock offset */
    int8_t      rssi;                   /* rssi, from -127 to +20dBm */
} __PACKED_POST__ hci_rssi_result;

#define HCI_RSSI_INVALID ((int8_t)128)

#define HCI_EVENT_READ_REMOTE_EXTENDED_FEATURES 0x23
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
    uint8_t     page;           /* page number */
    uint8_t     max;            /* max page number */
    uint8_t     features[HCI_FEATURES_SIZE]; /* LMP features bitmsk*/
} __PACKED_POST__ hci_read_remote_extended_features_ep;

#define HCI_EVENT_SCO_CON_COMPL                 0x2c
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
    btaddr_t    btaddr;         /* unit address */
    uint8_t     link_type;      /* link type */
    uint8_t     interval;       /* transmission interval */
    uint8_t     window;         /* retransmission window */
    uint16_t    rxlen;          /* rx packet length */
    uint16_t    txlen;          /* tx packet length */
    uint8_t     mode;           /* air mode */
} __PACKED_POST__ hci_sco_con_compl_ep;

#define HCI_EVENT_SCO_CON_CHANGED               0x2d
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
    uint8_t     interval;       /* transmission interval */
    uint8_t     window;         /* retransmission window */
    uint16_t    rxlen;          /* rx packet length */
    uint16_t    txlen;          /* tx packet length */
} __PACKED_POST__ hci_sco_con_changed_ep;

#define HCI_EVENT_SNIFF_SUBRATING               0x2e
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
    uint16_t    tx_latency;     /* max transmit latency */
    uint16_t    rx_latency;     /* max receive latency */
    uint16_t    remote_timeout; /* remote timeout */
    uint16_t    local_timeout;  /* local timeout */
} __PACKED_POST__ hci_sniff_subrating_ep;

#define HCI_EVENT_EXTENDED_RESULT               0x2f
typedef   struct {
    uint8_t     num_responses;  /* must be 0x01 */
    btaddr_t    btaddr;         /* remote device address */
    uint8_t     page_scan_rep_mode;
    uint8_t     reserved;
    btclass_t   uclass;
    uint16_t    clock_offset;
    int8_t      rssi;
    uint8_t     response[HCI_MAX_EXTENDED_INQ_RSP_LEN];
} __PACKED_POST__ hci_extended_result_ep;

#define HCI_EVENT_ENCRYPTION_KEY_REFRESH        0x30
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    uint16_t    con_handle;     /* connection handle */
} __PACKED_POST__ hci_encryption_key_refresh_ep;

#define HCI_EVENT_IO_CAPABILITY_REQ             0x31
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
} __PACKED_POST__ hci_io_capability_req_ep;

#define HCI_EVENT_IO_CAPABILITY_RSP             0x32
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
    uint8_t     io_capability;
    uint8_t     oob_data_present;
    uint8_t     auth_requirement;
} __PACKED_POST__ hci_io_capability_rsp_ep;

#define HCI_EVENT_USER_CONFIRM_REQ              0x33
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
    uint32_t    value;          /* 000000 - 999999 */
} __PACKED_POST__ hci_user_confirm_req_ep;

#define HCI_EVENT_USER_PASSKEY_REQ              0x34
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
} __PACKED_POST__ hci_user_passkey_req_ep;

#define HCI_EVENT_REMOTE_OOB_DATA_REQ           0x35
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
} __PACKED_POST__ hci_remote_oob_data_req_ep;

#define HCI_EVENT_SIMPLE_PAIRING_COMPL          0x36
typedef   struct {
    uint8_t     status;         /* 0x00 - success */
    btaddr_t    btaddr;         /* remote device address */
} __PACKED_POST__ hci_simple_pairing_compl_ep;

#define HCI_EVENT_LINK_SUPERVISION_TO_CHANGED   0x38
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
    uint16_t    timeout;        /* link supervision timeout */
} __PACKED_POST__ hci_link_supervision_to_changed_ep;

#define HCI_EVENT_ENHANCED_FLUSH_COMPL          0x39
typedef   struct {
    uint16_t    con_handle;     /* connection handle */
} __PACKED_POST__ hci_enhanced_flush_compl_ep;

#define HCI_EVENT_USER_PASSKEY_NOTIFICATION     0x3b
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
    uint32_t    value;          /* 000000 - 999999 */
} __PACKED_POST__ hci_user_passkey_notification_ep;

#define HCI_EVENT_KEYPRESS_NOTIFICATION         0x3c
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
    uint8_t     notification_type;
} __PACKED_POST__ hci_keypress_notification_ep;

#define HCI_EVENT_REMOTE_FEATURES_NOTIFICATION  0x3d
typedef   struct {
    btaddr_t    btaddr;         /* remote device address */
    uint8_t     features[HCI_FEATURES_SIZE]; /* LMP features bitmsk*/
} __PACKED_POST__ hci_remote_features_notification_ep;

#define HCI_EVENT_BT_LOGO                       0xfe

#define HCI_EVENT_VENDOR                        0xff

/*------------------------------------------------*/
/* TODO: btunit_info and hci_state are declared here because the ipc is
   tightly coupled with it - consider moving this declaration from here */

// -----> bt_ipc.h
typedef struct {
    char        name[HCI_DEVNAME_SIZE]; /* device name */
    btaddr_t    btaddr;                 /* device btaddr */
} bt_unit_info_t;

// -----> hci_internal.h
enum hci_state {
    BT_STATE_DISABLED = 0,
    BT_STATE_ENABLING,
    BT_STATE_ENABLED,
    BT_STATE_DISABLING,
    BT_STATE_DYING
};

typedef uint8_t hci_state_t;

/*
 * Authentication key per pair of local/remote devices
 */
/* TODO: this is a security struct, but the ipc is using it - consider moving
   it */
// --------> security, currently internal and ipc needs to include it
typedef struct {
    btaddr_t                    laddr;
    btaddr_t                    raddr;
    btkey_t                     key;
    uint8_t                     type;
} bt_key_pair_t;

/*------------------------------------------------*/

#endif
