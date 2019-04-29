#ifndef _PARTHUS_HC_CONST_
#define _PARTHUS_HC_CONST_

/**********************************************************************
 * MODULE NAME:    hc_const.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    HCI Operation and Error  Codes
 * MAINTAINER:     Gary Fleming
 * DATE:           23 JUN 99
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    23 JUN 1999 -   gf       - Initial Version 
 *
 * SOURCE CONTROL: $Id: hc_const.h,v 1.84 2014/10/08 02:13:59 garyf Exp $
 *    
 **********************************************************************/
#include "config.h"

/*
 *  Opcode Group Fields for the HCI Command Groups
 */
#define LINK_CONTROL_OGF                0x01
#define LINK_POLICY_OGF                 0x02
#define HOST_CONTROL_OGF                0x03
#define LOCAL_INFO_OGF                  0x04
#define LOCAL_STATUS_OGF                0x05
#define TEST_COMMANDS_OGF               0x06
#define LE_CONTROLLER_OGF               0x08
#define LE_SMP_COMMANDS_OGF             0x09
#define LE_GATT_COMMANDS_OGF            0x0A
#define LE_GAP_COMMANDS_OGF             0x0B
#define LE_L2CAP_COMMANDS_OGF           0x0C
#define LE_PROFILE_COMMANDS_OGF         0x0D
#define VENDOR_SPECIFIC_DEBUG_OGF       0x3f
#define PTS_TESTING_OGF                 0x10

/*
 *  HCI link policy command opcodes - Opcode group subfield = 0x02
 */
#define LC_G    (LINK_CONTROL_OGF << 10)

#define HCI_INQUIRY                                     (0x0001 + LC_G)
#define HCI_INQUIRY_CANCEL                              (0x0002 + LC_G)
#define HCI_PERIODIC_INQUIRY_MODE                       (0x0003 + LC_G)
#define HCI_EXIT_PERIODIC_INQUIRY_MODE                  (0x0004 + LC_G)
#define HCI_CREATE_CONNECTION                           (0x0005 + LC_G)
#define HCI_DISCONNECT                                  (0x0006 + LC_G)
#define HCI_ADD_SCO_CONNECTION                          (0x0007 + LC_G)
#define HCI_CREATE_CONNECTION_CANCEL                    (0x0008 + LC_G)
#define HCI_ACCEPT_CONNECTION_REQUEST                   (0x0009 + LC_G)
#define HCI_REJECT_CONNECTION_REQUEST                   (0x000A + LC_G)
#define HCI_LINK_KEY_REQUEST_REPLY                      (0x000B + LC_G)
#define HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY             (0x000C + LC_G)
#define HCI_PIN_CODE_REQUEST_REPLY                      (0x000D + LC_G)
#define HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY             (0x000E + LC_G)

#define HCI_CHANGE_CONNECTION_PACKET_TYPE               (0x000F + LC_G)
#define HCI_AUTHENTICATION_REQUESTED                    (0x0011 + LC_G)
#define HCI_SET_CONNECTION_ENCRYPTION_ENABLE            (0x0013 + LC_G)
#define HCI_CHANGE_CONNECTION_LINK_KEY                  (0x0015 + LC_G)
#define HCI_MASTER_LINK_KEY                             (0x0017 + LC_G)
#define HCI_REMOTE_NAME_REQUEST                         (0x0019 + LC_G)
#define HCI_REMOTE_NAME_REQUEST_CANCEL                  (0x001A + LC_G)
#define HCI_READ_REMOTE_FEATURES                        (0x001B + LC_G)
#define HCI_READ_REMOTE_EXTENDED_FEATURES               (0x001C + LC_G)
#define HCI_READ_REMOTE_VER_INFO                        (0x001D + LC_G)
#define HCI_READ_CLOCK_OFFSET                           (0x001F + LC_G)

#define HCI_READ_LMP_HANDLE                             (0x0020 + LC_G)
#define HCI_EXCHANGE_FIXED_INFO                         (0x0021 + LC_G)
#define HCI_EXCHANGE_ALIAS_INFO                         (0x0022 + LC_G)
#define HCI_PRIVATE_PAIRING_REQUEST_REPLY               (0x0023 + LC_G)
#define HCI_PRIVATE_PAIRING_REQUEST_NEGATIVE_REPLY      (0x0024 + LC_G)
#define HCI_GENERATED_ALIAS                             (0x0025 + LC_G)
#define HCI_ALIAS_ADDRESS_REQUEST_REPLY                 (0x0026 + LC_G)
#define HCI_ALIAS_ADDRESS_REQUEST_NEGATIVE_REPLY        (0x0027 + LC_G)
#define HCI_SETUP_SYNCHRONOUS_CONNECTION                (0x0028 + LC_G)
#define HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST       (0x0029 + LC_G)
#define HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST       (0x002A + LC_G)

#define HCI_IO_CAPABILITY_REQUEST_REPLY                 (0x002B + LC_G)
#define HCI_USER_CONFIRMATION_REQUEST_REPLY             (0x002C + LC_G)
#define HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY    (0x002D + LC_G)
#define HCI_USER_PASSKEY_REQUEST_REPLY                  (0x002E + LC_G)
#define HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY         (0x002F + LC_G)
#define HCI_REMOTE_OOB_DATA_REQUEST_REPLY               (0x0030 + LC_G)
#define HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY      (0x0033 + LC_G)
#define HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY        (0x0034 + LC_G)
#define MAX_HC_LINK_CONTROL_OPCODE                      0x35


/*
 *  HCI link policy command opcodes - Opcode group subfield = 0x02
 */

#define LP_G    (LINK_POLICY_OGF << 10)

#define HCI_HOLD_MODE                                   (0x0001 + LP_G)
#define HCI_SNIFF_MODE                                  (0x0003 + LP_G)
#define HCI_EXIT_SNIFF_MODE                             (0x0004 + LP_G)
#define HCI_PARK_MODE                                   (0x0005 + LP_G)
#define HCI_EXIT_PARK_MODE                              (0x0006 + LP_G)
#define HCI_QOS_SETUP                                   (0x0007 + LP_G)
#define HCI_ROLE_DISCOVERY                              (0x0009 + LP_G)
#define HCI_SWITCH_ROLE                                 (0x000B + LP_G)
#define HCI_READ_LINK_POLICY_SETTINGS                   (0x000C + LP_G)
#define HCI_WRITE_LINK_POLICY_SETTINGS                  (0x000D + LP_G)
#define HCI_READ_DEFAULT_LINK_POLICY_SETTINGS           (0x000E + LP_G)
#define HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS          (0x000F + LP_G)
#define HCI_FLOW_SPECIFICATION                          (0x0010 + LP_G)
#define HCI_SNIFF_SUBRATING                             (0x0011 + LP_G)

#define MAX_HC_LINK_POLICY_OPCODE                       0x11


/***********************************************
 * Host Controller and Baseband command opcode
 *  - Opcode group subfield = 0x03
 ***********************************************/

#define HC_G    (HOST_CONTROL_OGF << 10)

#define HCI_SET_EVENT_MASK                              (0x0001 + HC_G)
#define HCI_RESET                                       (0x0003 + HC_G)
#define HCI_SET_EVENT_FILTER                            (0x0005 + HC_G)
#define HCI_FLUSH                                       (0x0008 + HC_G)
#define HCI_READ_PIN_TYPE                               (0x0009 + HC_G)
#define HCI_WRITE_PIN_TYPE                              (0x000A + HC_G)
#define HCI_CREATE_NEW_UNIT_KEY                         (0x000B + HC_G)
#define HCI_READ_STORED_LINK_KEY                        (0x000D + HC_G)
#define HCI_WRITE_STORED_LINK_KEY                       (0x0011 + HC_G)
#define HCI_DELETE_STORED_LINK_KEY                      (0x0012 + HC_G)
#define HCI_CHANGE_LOCAL_NAME                           (0x0013 + HC_G)
#define HCI_READ_LOCAL_NAME                             (0x0014 + HC_G)
#define HCI_READ_CONNECT_ACCEPT_TO                      (0x0015 + HC_G)
#define HCI_WRITE_CONNECT_ACCEPT_TO                     (0x0016 + HC_G)
#define HCI_READ_PAGE_TIMEOUT                           (0x0017 + HC_G)
#define HCI_WRITE_PAGE_TIMEOUT                          (0x0018 + HC_G)
#define HCI_READ_SCAN_ENABLE                            (0x0019 + HC_G)
#define HCI_WRITE_SCAN_ENABLE                           (0x001A + HC_G)

#define HCI_READ_PAGE_SCAN_ACT                          (0x001B + HC_G)
#define HCI_WRITE_PAGE_SCAN_ACT                         (0x001C + HC_G)
#define HCI_READ_INQUIRY_SCAN_ACT                       (0x001D + HC_G)
#define HCI_WRITE_INQUIRY_SCAN_ACT                      (0x001E + HC_G)
#define HCI_READ_AUTHENTICATION_ENABLE                  (0x001F + HC_G)
#define HCI_WRITE_AUTHENTICATION_ENABLE                 (0x0020 + HC_G)
#define HCI_READ_ENCRYPTION_MODE                        (0x0021 + HC_G)
#define HCI_WRITE_ENCRYPTION_MODE                       (0x0022 + HC_G)
#define HCI_READ_CLASS_OF_DEVICE                        (0x0023 + HC_G)
#define HCI_WRITE_CLASS_OF_DEVICE                       (0x0024 + HC_G)
#define HCI_READ_VOICE_SETTINGS                         (0x0025 + HC_G)
#define HCI_WRITE_VOICE_SETTINGS                        (0x0026 + HC_G)
#define HCI_READ_AUTOMATIC_FLUSH_TIMEOUT                (0x0027 + HC_G)
#define HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT               (0x0028 + HC_G)
#define HCI_READ_NUM_BROADCAST_RETRANSMISSIONS          (0x0029 + HC_G)
#define HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS         (0x002A + HC_G)
#define HCI_READ_HOLD_MODE_ACTIVITY                     (0x002B + HC_G)
#define HCI_WRITE_HOLD_MODE_ACTIVITY                    (0x002C + HC_G)
#define HCI_READ_TRANSMIT_POWER_LEVEL                   (0x002D + HC_G)
#define HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE        (0x002E + HC_G)
#define HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE       (0x002F + HC_G)
#define HCI_SET_HOST_CONTROLLER_TO_HOST_FLOW_CONTROL    (0x0031 + HC_G)
#define HCI_HOST_BUFFER_SIZE                            (0x0033 + HC_G)
#define HCI_HOST_NUMBER_OF_COMPLETED_PACKETS            (0x0035 + HC_G)
#define HCI_READ_SUPERVISION_TIMEOUT                    (0x0036 + HC_G)
#define HCI_WRITE_SUPERVISION_TIMEOUT                   (0x0037 + HC_G)
#define HCI_READ_NUMBER_OF_SUPPORTED_IAC                (0x0038 + HC_G)
#define HCI_READ_CURRENT_IAC_LAP                        (0x0039 + HC_G)
#define HCI_WRITE_CURRENT_IAC_LAP                       (0x003A + HC_G)
#define HCI_READ_PAGE_SCAN_PERIOD_MODE                  (0x003B + HC_G)
#define HCI_WRITE_PAGE_SCAN_PERIOD_MODE                 (0x003C + HC_G)
#define HCI_READ_PAGE_SCAN_MODE                         (0x003D + HC_G)
#define HCI_WRITE_PAGE_SCAN_MODE                        (0x003E + HC_G)

#define HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION         (0x003F + HC_G)

#define HCI_READ_INQUIRY_SCAN_TYPE                      (0x0042 + HC_G)
#define HCI_WRITE_INQUIRY_SCAN_TYPE                     (0x0043 + HC_G)
#define HCI_READ_INQUIRY_MODE                           (0x0044 + HC_G)
#define HCI_WRITE_INQUIRY_MODE                          (0x0045 + HC_G)
#define HCI_READ_PAGE_SCAN_TYPE                         (0x0046 + HC_G)
#define HCI_WRITE_PAGE_SCAN_TYPE                        (0x0047 + HC_G)
#define HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE            (0x0048 + HC_G)
#define HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE           (0x0049 + HC_G)
#define HCI_RESET_FIXED_ADDRESS_ATTEMPTS_COUNTER        (0x0050 + HC_G)
#define HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND      (0x0051 + HC_G)
#define HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_COMMAND     (0x0052 + HC_G)
#define HCI_REFRESH_ENCRYPTION_KEY						(0x0053 + HC_G)
#define HCI_READ_SIMPLE_PAIRING_MODE                    (0x0055 + HC_G)
#define HCI_WRITE_SIMPLE_PAIRING_MODE                   (0x0056 + HC_G)
#define HCI_READ_LOCAL_OOB_DATA                         (0x0057 + HC_G)
#define HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL  (0x0058 + HC_G)
#define HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL          (0x0059 + HC_G)
#define HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING       (0x005A + HC_G)
#define HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING      (0x005B + HC_G)
#define HCI_ENHANCED_FLUSH                              (0x005F + HC_G)
#define HCI_SEND_KEYPRESS_NOTIFICATION                  (0x0060 + HC_G)
#define HCI_SET_EVENT_MASK_PAGE2                        (0x0063 + HC_G)
#define HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL          (0X0068 + HC_G)

#define HCI_READ_LE_HOST_SUPPORT                        (0X006C + HC_G)
#define HCI_WRITE_LE_HOST_SUPPORT                       (0X006D + HC_G)

#define HCI_SET_RESERVED_LT_ADDR                        (0x0074 + HC_G)
#define HCI_DELETE_RESERVED_LT_ADDR                     (0x0075 + HC_G)
#define HCI_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA     (0x0076 + HC_G)
#define HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS       (0x0077 + HC_G)
#define HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS      (0x0078 + HC_G)


#define HCI_READ_SECURE_CONNECTIONS_ENABLE              (0X0079 + HC_G)
#define HCI_WRITE_SECURE_CONNECTIONS_ENABLE             (0X007A + HC_G)
#define HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT          (0X007B + HC_G)
#define HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT         (0X007C + HC_G)
#define HCI_READ_LOCAL_OOB_EXTENDED_DATA                (0X007D + HC_G)
#define HCI_READ_EXTENDED_PAGE_TIMEOUT                  (0X007E + HC_G)
#define HCI_WRITE_EXTENDED_PAGE_TIMEOUT                 (0X007F + HC_G)
#define HCI_READ_EXTENDED_INQUIRY_LENGTH                (0X0080 + HC_G)
#define HCI_WRITE_EXTENDED_INQUIRY_LENGTH               (0X0081 + HC_G)

#define MAX_HC_BB_COMMAND_OPCODE                        0x81


/***********************************************
 *  HCI Informaional parameters command opcode 
 *  - Opcode group subfield = 0x04
 ***********************************************/

#define HI_G    (LOCAL_INFO_OGF << 10)

#define HCI_READ_LOCAL_VER_INFO                         (0x0001 + HI_G)
#define HCI_READ_LOCAL_COMMANDS							(0x0002 + HI_G)
#define HCI_READ_LOCAL_FEATURES                         (0x0003 + HI_G)
#define HCI_READ_LOCAL_EXTENDED_FEATURES                (0x0004 + HI_G)
#define HCI_READ_BUFFER_SIZE                            (0x0005 + HI_G)
#define HCI_READ_COUNTRY_CODE                           (0x0007 + HI_G)
#define HCI_READ_BD_ADDR                                (0x0009 + HI_G)

#define MAX_HC_INFO_COMMAND_OPCODE                      0x09

/***********************************************
 * HCI Status parameters command opcode
 *  - Opcode group subfield = 0x05
 ***********************************************/

#define HS_G    (LOCAL_STATUS_OGF << 10)

#define HCI_READ_FAILED_CONTACT_COUNTER                 (0x0001 + HS_G)
#define HCI_RESET_FAILED_CONTACT_COUNTER                (0x0002 + HS_G)
#define HCI_GET_LINK_QUALITY                            (0x0003 + HS_G)
#define HCI_READ_RSSI                                   (0x0005 + HS_G)
#define HCI_READ_AFH_CHANNEL_MAP                        (0x0006 + HS_G)
#define HCI_READ_CLOCK                                  (0x0007 + HS_G)
#define HCI_READ_ENCRYPTION_KEY_SIZE                    (0x0008 + HS_G)

#define MAX_HC_STATUS_COMMAND_OPCODE                    0x08

/**********************************************
 *  HCI Testing Command opcodes
 *    - Opcode group subfield = 0x06
 **********************************************/

#define HT_G    (TEST_COMMANDS_OGF << 10) 

#define HCI_READ_LOOPBACK_MODE                          (0x0001 + HT_G)
#define HCI_WRITE_LOOPBACK_MODE                         (0x0002 + HT_G)
#define HCI_ENABLE_DEVICE_UNDER_TEST_MODE               (0x0003 + HT_G)
#define HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE             (0x0004 + HT_G)
#define MAX_HC_TEST_COMMAND_OPCODE                      0x04


#define LE_G    (LE_CONTROLLER_OGF << 10)

#define HCI_LE_SET_EVENT_MASK                          (0x0001 + LE_G)	
#define HCI_LE_READ_BUFFER_SIZE                        (0x0002 + LE_G)	
#define HCI_LE_READ_LOCAL_SUPPORTED_FEATURES           (0x0003 + LE_G)	
#define HCI_LE_SET_RANDOM_ADDRESS                      (0x0005 + LE_G)
#define HCI_LE_SET_ADVERTISING_PARAMETERS              (0x0006 + LE_G)	
#define HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER       (0x0007 + LE_G)	
#define HCI_LE_SET_ADVERTISING_DATA                    (0x0008 + LE_G)	
#define HCI_LE_SET_SCAN_RESPONSE_DATA                  (0x0009 + LE_G)	
#define HCI_LE_SET_ADVERTISE_ENABLE                    (0x000A + LE_G)	
#define HCI_LE_SET_SCAN_PARAMETERS                     (0x000B + LE_G)	
#define HCI_LE_SET_SCAN_ENABLE                         (0x000C + LE_G)	
#define HCI_LE_CREATE_CONNECTION                       (0x000D + LE_G)	
#define HCI_LE_CREATE_CONNECTION_CANCEL                (0x000E + LE_G)	
#define HCI_LE_READ_WHITE_LIST_SIZE                    (0x000F + LE_G)	
#define HCI_LE_CLEAR_WHITE_LIST                        (0x0010 + LE_G)	
#define HCI_LE_ADD_DEVICE_TO_WHITE_LIST                (0x0011 + LE_G)	
#define HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST           (0x0012 + LE_G)
#define HCI_LE_CONNECTION_UPDATE                       (0x0013 + LE_G)
#define HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION         (0x0014 + LE_G)
#define HCI_LE_READ_CHANNEL_MAP                        (0x0015 + LE_G)	
#define HCI_LE_READ_REMOTE_USED_FEATURES               (0x0016 + LE_G)	
#define HCI_LE_ENCRYPT                                 (0x0017 + LE_G)	
#define HCI_LE_RAND                                    (0x0018 + LE_G)	
#define HCI_LE_START_ENCRYPTION                        (0x0019 + LE_G)
#define HCI_LE_LONG_TERM_KEY_REQUEST_REPLY             (0x001A + LE_G)	
#define HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY    (0x001B + LE_G)	
#define HCI_LE_READ_SUPPORTED_STATES                   (0x001C + LE_G)	
#define HCI_LE_RECEIVER_TEST                           (0x001D + LE_G)	
#define HCI_LE_TRANSMITTER_TEST                        (0x001E + LE_G)	
#define HCI_LE_TEST_END                                (0x001F + LE_G)
#define HCI_LE_REMOTE_CONNECTION_PARAM_REQUEST_REPLY   (0x0020 + LE_G)
#define HCI_LE_REMOTE_CONNECTION_PARAM_REQUEST_NEGATIVE_REPLY  (0x0021 + LE_G)
#define MAX_HC_LE_COMMAND_OPCODE                       0x21


/************************************************
*  CEVA Specific Command for LE SMP 
*    - Opcode group subfield = 9
*************************************************/

#define H_LE_SMP_G (LE_SMP_COMMANDS_OGF << 10)

#define LE_SMP_CONFIGURE_SECURITY                    (0x0001 + H_LE_SMP_G)
#define LE_SMP_INITIATE_SECURITY                     (0x0002 + H_LE_SMP_G)
#define LE_SMP_PASSKEY_ENTRY                         (0x0003 + H_LE_SMP_G)
#define LE_SMP_OOB_DATA_ENTRY                        (0x0004 + H_LE_SMP_G)

#define MAX_LE_SMP_COMMAND_OPCODE   0x04

#define H_LE_GATT_G (LE_GATT_COMMANDS_OGF << 10)

#define LE_GATT_CLIENT_PRIMARY_SERVICE_DISCOVERY                 ( 01 + H_LE_GATT_G)
#define LE_GATT_CLIENT_RELATIONSHIP_DISCOVERY   	             ( 02 + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_DISCOVERY                  ( 03 + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY       ( 04 + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_VALUE_READ                 ( 05 + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_VALUE_WRITE                ( 06 + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ      ( 0x0B + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE     ( 0x0C + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_VALUE_READ_MULTIPLE        ( 0x0D + H_LE_GATT_G)
#define LE_GATT_CLIENT_CHARACTERISTIC_EXECUTE_WRITE              ( 0x0E + H_LE_GATT_G)
#define LE_GATT_CLIENT_EXCHANGE_MTU                              ( 0x11 + H_LE_GATT_G)

//#define LE_GATT_SERVER_ADD_INCLUDED_SERVICE 					 ( 07 + H_LE_GATT_G)
//#define LE_GATT_SERVER_ADD_CHARACTERISTIC                        ( 08 + H_LE_GATT_G)
//#define LE_GATT_SERVER_ADD_SERVICE                               ( 09 + H_LE_GATT_G)
//#define LE_GATT_SERVER_CHARACTERISTIC_VALUE_NOTIFICATION         ( 0x0A + H_LE_GATT_G)

#define LE_GATT_SERVER_CHARACTERISTIC_VALUE_WRITE                ( 0x0F + H_LE_GATT_G)
#define LE_GATT_SERVER_WRITE_PERMISSIONS                         ( 0x10 + H_LE_GATT_G)
#define LE_GATT_SERVER_CHARACTERISTIC_VALUE_READ                 ( 0x14 + H_LE_GATT_G) 

#define MAX_LE_GATT_COMMAND_OPCODE  0x14

#define H_LE_GAP_G    (LE_GAP_COMMANDS_OGF << 10)

#define GAP_SETDISCOVERABLE                                      ( 0x01 + H_LE_GAP_G)
#define GAP_SETDISCOVERY                                         ( 0x02 + H_LE_GAP_G)
#define GAP_SETCONNECTABLE                                       ( 0x03 + H_LE_GAP_G)
#define GAP_FINDCONNECTABLEDEVICES                               ( 0x04 + H_LE_GAP_G)
#define GAP_CONNECTIONESTABLISH                                  ( 0x05 + H_LE_GAP_G)
#define GAP_BROADCASTMODE								         ( 0x06 + H_LE_GAP_G)
#define GAP_BROADCASTDATA		                                 ( 0x07 + H_LE_GAP_G)
#define GAP_SET_AD_DATA											 ( 0x08 + H_LE_GAP_G)	
#define GAP_SET_SCAN_RESP_DATA									 ( 0x09 + H_LE_GAP_G)
#define GAP_DISCONNECT                                           ( 0x0A + H_LE_GAP_G)
#define GAP_SET_PRIVACY                                          ( 0x0B + H_LE_GAP_G)
#define MAX_LE_GAP_COMMAND_OPCODE  0x0C

#define H_LE_L2CAP_G    (LE_L2CAP_COMMANDS_OGF << 10)

#define L2CAP_CONNECTIONUPDATE                                   ( 0x01 + H_LE_L2CAP_G)
#define MAX_LE_L2CAP_COMMAND_OPCODE  0x01


#define H_LE_PROFILE_G    (LE_PROFILE_COMMANDS_OGF << 10)

#define CTS_PROFILE_COMMAND										 ( 0x01 + H_LE_PROFILE_G)
#define NDSC_PROFILE_COMMAND								     ( 0x02 + H_LE_PROFILE_G)
#define SIMPLE_PROFILE_COMMAND						     	     ( 0x03 + H_LE_PROFILE_G)
#define HID_PROFILE_COMMAND							     	     ( 0x04 + H_LE_PROFILE_G)
#define HRS_PROFILE_COMMAND							     	     ( 0x05 + H_LE_PROFILE_G)
#define HT_PROFILE_COMMAND							     	     ( 0x06 + H_LE_PROFILE_G)
#define CSC_PROFILE_COMMAND							     	     ( 0x07 + H_LE_PROFILE_G)
#define RSC_PROFILE_COMMAND							     	     ( 0x08 + H_LE_PROFILE_G)
#define BP_PROFILE_COMMAND                                       ( 0x09 + H_LE_PROFILE_G)
#define BG_PROFILE_COMMAND                                       ( 0x0A + H_LE_PROFILE_G)
/*****************************
 * VENDOR SPECIFIC SPECIFIC COMMANDS
 ******************************/

#define ERIC_WRITE_PCM_SETTINGS                         0xFC07
#define ERIC_EWHWR                                      0xFC04
#define ERIC_EETM                                       0xFC11
#define ERIC_ETC                                        0xFC12

/*****************************
 * HCI Events - Event codes
 ******************************/

#define HCI_INQUIRY_COMPLETE_EVENT                         0x01
#define HCI_INQUIRY_RESULT_EVENT                           0x02
#define HCI_CONNECTION_COMPLETE_EVENT                      0x03
#define HCI_CONNECTION_REQUEST_EVENT                       0x04
#define HCI_DISCONNECTION_COMPLETE_EVENT                   0x05
#define HCI_AUTHENTICATION_COMPLETE_EVENT                  0x06
#define HCI_READ_REMOTE_NAME_REQUEST_COMPLETE_EVENT        0x07
#define HCI_ENCRYPTION_CHANGE_EVENT                        0x08
#define HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT      0x09
#define HCI_MASTER_LINK_KEY_COMPLETE_EVENT                 0x0A
#define HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT  0x0B
#define HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT 0x0C
#define HCI_QoS_SETUP_COMPLETE_EVENT                       0x0D
#define HCI_COMMAND_COMPLETE_EVENT                         0x0E
#define HCI_COMMAND_STATUS_EVENT                           0x0F
#define HCI_HARDWARE_ERROR_EVENT                           0x10
#define HCI_FLUSH_OCCURED_EVENT                            0x11
#define HCI_ROLE_CHANGE_EVENT                              0x12
#define HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT              0x13
#define HCI_MODE_CHANGE_EVENT                              0x14
#define HCI_RETURN_LINK_KEYS_EVENT                         0x15
#define HCI_PIN_CODE_REQUEST_EVENT                         0x16
#define HCI_LINK_KEY_REQUEST_EVENT                         0x17
#define HCI_LINK_KEY_NOTIFICATION_EVENT                    0x18
#define HCI_LOOPBACK_COMMAND_EVENT                         0x19
#define HCI_DATA_BUFFER_OVERFLOW_EVENT                     0x1A
#define HCI_MAX_SLOTS_CHANGE_EVENT                         0x1B
#define HCI_READ_CLOCK_OFFSET_EVENT                        0x1C
#define HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT           0x1D
#define HCI_QOS_VIOLATION_EVENT                            0x1E
#define HCI_PAGE_SCAN_MODE_CHANGE_EVENT                    0x1F
#define HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT         0x20

/*
 * V1.2 
 */
#define HCI_FLOW_SPECIFICATION_COMPLETE_EVENT              0x21
#define HCI_INQUIRY_RESULT_EVENT_WITH_RSSI                 0x22
#define HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT   0x23

#define HCI_DEBUG_EVENT                                    0x24

#define HCI_SYNCHRONOUS_CONNECTION_COMPLETE_EVENT          0x2C
#define HCI_SYNCHRONOUS_CONNECTION_CHANGED_EVENT           0x2D
#define HCI_SNIFF_SUBRATING_EVENT                          0x2E
#define HCI_EXTENDED_INQUIRY_RESULT_EVENT                  0x2F
#define HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT          0x30

#define HCI_IO_CAPABILITY_REQUEST_EVENT                    0x31
#define HCI_IO_CAPABILITY_RESPONSE_EVENT                   0x32
#define HCI_USER_CONFIRMATION_REQUEST_EVENT                0x33
#define HCI_USER_PASSKEY_REQUEST_EVENT                     0x34
#define HCI_REMOTE_OOB_DATA_REQUEST_EVENT                  0x35
#define HCI_SIMPLE_PAIRING_COMPLETE_EVENT                  0x36

#define HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT         0x38
#define HCI_ENHANCED_FLUSH_COMPLETE_EVENT                  0x39
#define HCI_USER_PASSKEY_NOTIFICATION_EVENT                0x3B
#define HCI_KEYPRESS_NOTIFICATION_EVENT                    0x3C
#define HCI_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT 0x3D
#define HCI_AUTHENTICATION_PAYLOAD_TIMEOUT_EXCEEDED        0x57
#define HCI_LE_EVENT                                       0x3E

#define LE_SUBEVENT_CONNECTION_COMPLETE                    0x01
#define LE_SUBEVENT_ADVERTISING_REPORT	                   0x02
#define LE_SUBEVENT_CONNECTION_UPDATE_COMPLETE             0x03
#define LE_SUBEVENT_READ_REMOTE_USED_FEATURES_COMPLETE	   0x04
#define LE_SUBEVENT_LONG_TERM_KEY_REQUEST 	               0x05
#define LE_SUBEVENT_REMOTE_CONNECTION_PARAMETER_REQUEST    0x06

#define SMP_USER_PASSKEY_NOTIFICATION_EVENT                0x21
#define SMP_USER_PASSKEY_REQUEST_EVENT                     0x22
#define SMP_USER_OOB_DATA_REQUEST_EVENT                    0x23
#define SMP_PAIRING_COMPLETE_EVENT                         0x24
#define SMP_PAIRING_REQUEST_EVENT                          0x25
#define SMP_SECURITY_REQUEST_EVENT                         0x26

#define GATT_COMMAND_COMPLETE_EVENT                        0x41
#define GATT_DISCOVER_SERVICES_EVENT               		   0x42
#define GATT_DISCOVER_SERVICES_BY_UUID_EVENT               0x43
#define GATT_DISCOVER_CHARACTERISTICS_EVENT                0x44
#define GATT_DISCOVER_DESCRIPTORS_EVENT                    0x45
#define GATT_READ_RESPONSE_EVENT                           0x46
#define GATT_READ_BY_TYPE_RESPONSE_EVENT                   0x47
#define GATT_DISCOVER_INCLUDED_SERVICES_EVENT              0x48
#define GATT_MAX_MTU_EVENT                                 0x49
//#define GATT_RELIABLE_WRITE_COMPLETE_EVENT                 0x4A  - Deprecated
#define GATT_WRITE_COMPLETE_EVENT                          0x4B
//#define GATT_READ_LONG_RESPONSE_EVENT                      0x4C
//#define GATT_WRITE_RESPONSE_EVENT                          0x4D  - Deprecated
#define GATT_VALUE_NOTIFICATION_EVENT                      0x4E
#define GATT_VALUE_INDICATION_EVENT                        0x4F
#define GATT_VALUE_CONFIRMATION_EVENT                      0x50
#define GATT_WRITE_INDICATION_EVENT                        0x51
#define GATT_INFORMATION_READ_COMPLETE_EVENT               0x52

#define L2CAP_CONNECTION_UPDATE_RESPONSE_EVENT             0x61

#define HCI_LE_APP_EVENT                                   0x71

#define GAP_CONNECTION_COMPLETE_EVENT                      0x81

#define SIZE_OF_BD_ADDR                                     6
#define SIZE_OF_DEVICE_CLASS                                3
#define SIZE_OF_EVENT_CODE                                  1
#define SIZE_OF_SR_MODE                                     1
#define SIZE_OF_SP_MODE                                     1
#define SIZE_OF_PAGE_SCAN_MODE                              1
#define SIZE_OF_CLK_OFFSET                                  2
#define SIZE_OF_NUM_RESPONSES_FIELD                         1
#define Num_HCI_Command_Packets                             1
#define SIZE_OF_EVENT_HEADER                                2
#define SIZE_OF_COMMAND_COMPLETE_HEADER                     3
#define SIZE_OF_RSSI                                        1


/* NOTE ALL THE FOLLOWING NEED TO BE CHECKED AS THEY ARE ONLY STUBBED */

#define MAX_Tpagescan_FOR_R1        0x0800
#define MAX_Tpagescan_FOR_R2        0x1000

#define AUTO_ACCEPT_OFF             0x01
#define AUTO_ACCEPT_ON              0x02

#define MAX_HC_TIMERS               10

#define MAX_CONNECTION_HANDLE       0xEFF
#define MAX_INQUIRY_LENGTH          0x30
#define NO_RESPONSES                0x00

#define MAX_NUM_LINK_KEYS_TO_WRITE  0X0B

#ifndef MAX_INQUIRY_RESPONSES
#define MAX_INQUIRY_RESPONSES       10
#endif

/*
 * In V1.1 this has a valid range of 10 bits but 16 bit value
 * To support SCO VIA HCI with HC software Codec the following is 
 * required.
 */ 
#define PRH_BS_HCI_VOICE_SETTING_FOR_SCO_VIA_HCI      0x8000

/***************************************
 ****        EVENT MASKs     ***********
 ***************************************/

#define NO_EVENT_MASK                                        0x00000000
#define INQUIRY_COMPLETE_EVENT_MASK                          0x00000001
#define INQUIRY_RESULT_EVENT_MASK                            0x00000002
#define CONNECTION_COMPLETE_EVENT_MASK                       0x00000004
#define CONNECTION_REQUEST_EVENT_MASK                        0x00000008
#define DISCONNECTION_COMPLETE_EVENT_MASK                    0x00000010
#define AUTHENTICATION_COMPLETE_EVENT_MASK                   0x00000020
#define REMOTE_NAME_REQUEST_COMPLETE_EVENT_MASK              0x00000040
#define ENCRYPTION_CHANGE_EVENT_MASK                         0x00000080
#define CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT_MASK       0x00000100
#define MASTER_LINK_KEY_COMPLETE_EVENT_MASK                  0x00000200
#define READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT_MASK   0x00000400
#define READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT_MASK  0x00000800
#define QOS_SETUP_COMPLETE_EVENT_MASK                        0x00001000
#define COMMAND_COMPLETE_EVENT_MASK                          0x00002000
#define COMMAND_STATUS_EVENT_MASK                            0x00004000
#define HARDWARE_ERROR_EVENT_MASK                            0x00008000
#define FLUSH_OCCURED_EVENT_MASK                             0x00010000
#define ROLE_CHANGE_EVENT_MASK                               0x00020000
#define NUMBER_OF_COMPLETED_PACKETS_EVENT_MASK               0x00040000
#define MODE_CHANGE_EVENT_MASK                               0x00080000
#define RETURN_LINK_KEYS_EVENT_MASK                          0x00100000
#define PIN_CODE_REQUEST_EVENT_MASK                          0x00200000
#define LINK_KEY_REQUEST_EVENT_MASK                          0x00400000
#define LINK_KEY_NOTIFICATION_EVENT_MASK                     0x00800000
#define LOOPBACK_COMMAND_EVENT_MASK                          0x01000000
#define DATA_BUFFER_OVERFLOW_EVENT_MASK                      0x02000000
#define MAX_SLOTS_CHANGE_EVENT_MASK                          0x04000000
#define READ_CLOCK_OFFSET_EVENT_MASK                         0x08000000
#define CONNECTION_PACKET_TYPE_CHANGED_EVENT_MASK            0x10000000
#define QOS_VIOLATION_EVENT_MASK                             0x20000000
#define PAGE_SCAN_MODE_CHANGE_EVENT_MASK                     0x40000000
#define PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT_MASK          0x80000000
#define INQUIRY_RESULT_WITH_RSSI_EVENT_MASK                  0x00000002

#define LE_NO_EVENT_MASK                                     0x00000000
#define LE_CONNECTION_COMPLETE_EVENT_MASK					 0x00000001
#define LE_ADVERTISING_REPORT_EVENT_MASK					 0x00000002
#define LE_CONNECTION_UPDATE_COMPLETE_EVENT_MASK			 0x00000004
#define LE_READ_REMOTE_USED_FEATURES_COMPLETE_EVENT_MASK	 0x00000008
#define LE_LONG_TERM_REQUEST_EVENT_MASK						 0x00000010
#define LE_SUBEVENT_REMOTE_CONNECTION_PARAMETER_REQUEST_MASK 0x00000020

#define LE_DEFAULT											 0x0000001F


/*
 * HCI SCO Voice Settings for Air-Mode 
 * [Note do not map directly to LM Air Modes]
 */
#define HCI_AIR_MODE_CVSD           0x00
#define HCI_AIR_MODE_U_LAW          0x01
#define HCI_AIR_MODE_A_LAW          0x02
#define HCI_AIR_MODE_TRANSPARENT    0x03

/*--------------DualMode use only----------------*/
#ifdef BT_DUALMODE
	#define SEAN_MODIFY_REF_BQB   1
#else
	#define SEAN_MODIFY_REF_BQB   0
#endif
#ifdef BT_DUALMODE
/* merge from BK3431, charles modify,HCI LEadv_Set_Advertising_Params define */
//#if (SEAN_MODIFY_REF_BQB == 1)
typedef enum
{
	HCI_ADV_IND                 = 0,
	HCI_ADV_DIRECT_IND          = 1,
	HCI_ADV_SCAN_IND            = 2,
	HCI_NONCONN_IND             = 3,
} t_HCI_LE_ADV_TYPE;
//#endif
/*--------------DualMode use only----------------*/
#endif
#endif


