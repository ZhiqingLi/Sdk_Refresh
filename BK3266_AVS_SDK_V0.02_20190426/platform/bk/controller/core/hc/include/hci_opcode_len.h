#ifndef _PARTHUS_HCI_OPCODE_LEN_
#define _PARTHUS_HCI_OPCODE_LEN_

/**********************************************************************
 * MODULE NAME:    hci_opcode_len.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Arrays containing the HCI opcode parameter lengths
 * AUTHOR:         Gary Fleming
 * DATE:           
 *
 * SOURCE CONTROL: $Id: hci_opcode_len.h,v 1.40 2013/11/19 02:15:01 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    xx xxx 1999 -   gf       - Initial Version 
 *
 *********************************************************************/

/* Link Control Command Parameter Lengths  */

#include "hc_const.h"

CONST u_int8 link_control_command_len[MAX_HC_LINK_CONTROL_OPCODE+1] = {
        0x00,  /* Invalid Opcode                  0*/
        0x05,  /* HCI_INQUIRY                     1*/
        0x00,  /* HCI_INQUIRY_CANCLE              2*/
        0x09,  /* HCI_PERIODIC_INQUIRY_MODE       3*/  
        0x00,  /* HCI_EXIT_PERIODIC_INQUIRY_MODE  4*/
        0x0D,  /* HCI_CREATE_CONNECTION           5*/
        0x03,  /* HCI_DISCONNECT                  6*/
        0x04,  /* HCI_ADD_SCO_CONNECTION          7*/
        0x06,  /* HCI_CREATE_CONNECTION_CANCEL    8*/
        0x07,  /* HCI_ACCEPT_CONNECTION_REQUEST   9*/
        0x07,  /* HCI_REJECT_CONNECTION_REQUEST   A*/
        0x16,  /* HCI_LINK_KEY_REQUEST_REPLY      B*/        
        0x06,  /* HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY C*/   
        0x17,  /* HCI_PIN_CODE_REQUEST_REPLY          D*/ 
        0x06,  /* HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY E*/
        0x04,  /* HCI_CHANGE_CONNECTION_PACKET_TYPE   F*/
        0x00,  /* Invalid Opcode                  10*/
        0x02,  /* HCI_AUTHENTICATION_REQUESTED    11*/
        0x00,  /* Invalid Opcode                  12*/
        0x03,  /* HCI_SET_CONNECTION_ENCRYPTION_ENABLE 13*/
        0x00,  /* Invalid Opcode                  14*/
        0x02,  /* HCI_CHANGE_CONNECTION_LINK_KEY  15*/
        0x00,  /* Invalid Opcode                  16*/
        0x01,  /* HCI_MASTER_LINK_KEY             17*/
        0x00,  /* Invalid Opcode                  18*/
        0x0A,  /* HCI_REMOTE_NAME_REQUEST         19*/
        0x06,  /* HCI_REMOTE_NAME_REQUEST_CANCEL  1A*/
        0x02,  /* HCI_READ_REMOTE_FEATURES        1B*/
        0x03,  /* HCI_READ_REMOTE_EXTENDED_FEATURES 1C*/
        0x02,  /* HCI_READ_REMOTE_VER_INFO        1D*/
        0x00,  /* Invalid Opcodes                 1E*/
        0x02,  /* HCI_READ_CLOCK_OFFSET           1F*/
        0x02,  /* HCI_READ_LMP_HANDLE             20*/
        0x02,  /* HCI_EXCHANGE_FIXED_INFO         21*/
        0x08,  /* HCI_EXCHANGE_ALIAS_INFO         22*/
        0x02,  /* HCI_PRIVATE_PAIRING_REQUEST_REPLY 23*/
        0x02,  /* HCI_PRIVATE_PAIRING_REQUEST_NEGATIVE_REPLY 24*/
        0x08,  /* HCI_GENERATED_ALIAS             25*/
        0x08,  /* HCI_ALIAS_ADDRESS_REQUEST_REPLY 26*/
        0x06,  /* HCI_ALIAS_ADDRESS_REQUEST_NEGATIVE_REPLY 27*/
        0x11,  /* HCI_SETUP_SYNCHRONOUS_CONNECTION  28*/
        0x15,  /* HCI_ACCEPT_SYNCRONOUS_CONNECTION_REQUEST 29 */
        0x07,  /* HCI_REJECT_SYNCHRONOUS_CONNECTION_RQEUST 2A */
		0x09,  /* HCI_IO_CAPABILITY_REQUEST_REPLY                2B */
		0x06,  /* HCI_USER_CONFIRMATION_REQUEST_REPLY            2C */
		0x06,  /* HCI_USER_CONFIRMATION_NEGATIVE_REQUEST_REPLY   2D */
		0x0A,  /* HCI_USER_PASSKEY_REQUEST_REPLY                 2E */
		0x06,  /* HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY        2F */
		0x26,  /* HCI_REMOTE_OOB_DATA_REQUEST_REPLY              30 */
        0x00,  /* Invalid Opcodes                                31 */
        0x00,  /* Invalid Opcodes                                32 */
		0x06,  /* HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY     33 */
		0x07,  /* HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY       34 */
};

/* Link Policy Command Parameter Lengths  */

CONST u_int8 link_policy_command_len[MAX_HC_LINK_POLICY_OPCODE+1] = {
        0x00,  /* Invalid Opcodes                 */
        0x06,  /* HCI_HOLD_MODE                   */
        0x00,  /* Invalid Opcodes                 */
        0x0A,  /* HCI_SNIFF_MODE                  */
        0x02,  /* HCI_EXIT_SNIFF_MODE             */
        0x06,  /* HCI_PARK_MODE                   */
        0x02,  /* HCI_EXIT_PARK_MODE              */
        0x14,  /* HCI_QOS_SETUP                   */

        0x00,  /* Invalid Opcodes                 */
        0x02,  /* HCI_ROLE_DISCOVERY              */
        0x00,  /* Invalid Opcodes                 */
        0x07,  /* HCI_SWITCH_ROLE                 */
        0x02,  /* HCI_READ_LINK_POLICY_SETTINGS   */        
        0x04,  /* HCI_WRITE_LINK_POLICY_SETTINGS  */
        0x00,  /* HCI_READ_DEFAULT_LINK_POLICY_SETTINGS 0x0E*/
        0x02,  /* HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS 0x0F*/
        0x15,  /* HCI_FLOW_SPECIFICATION          0x10*/
        0x08,  /* HCI_SNIFF_SUBRATING 0x11*/
};

/* Host Control and Baseband Commands     */

CONST u_int8 hc_bb_command_len[MAX_HC_BB_COMMAND_OPCODE+1] = {
        0x00,  /* Invalid Opcodes                         00 */
        0x08,  /* HCI_SET_EVENT_MASK                      01 */
        0x00,  /* Invalid Opcodes                         02 */
        0x00,  /* HCI_RESET                               03 */
        0x00,  /* Invalid Opcodes                         04 */
        0xFF,  /* HCI_SET_EVENT_FILTER - Variable         05 */
        0x00,  /* Invalid Opcode                          06 */
        0x00,  /* Invalid Opcode                          07 */
        0x02,  /* HCI_FLUSH                               08 */
        0x00,  /* HCI_READ_PIN_TYPE                       09 */
        0x01,  /* HCI_WRITE_PIN_TYPE                      0A */
        0x00,  /* HCI_CREATE_NEW_UNIT_KEY                 0B */
        0x00,  /* Invalid Opcode                          0C */
        0x07,  /* HCI_READ_STORED_LINK_KEY                0D */
        0x00,  /* Invalid Opcode                          0E */
        0x00,  /* Invalid Opcode                          0F */
        0x00,  /* Invalid Opcode                          10 */
        0xFF,  /* HCI_WRITE_STORED_LINK_KEY - Variable    11 */
        0x07,  /* HCI_DELETE_STORED_LINK_KEY              12 */
        0xFF,  /* HCI_CHANGE_LOCAL_NAME  - Variable       13 */
        0x00,  /* Invalid Opcode                          14 */
        0x00,  /* HCI_READ_CONNECT_ACCEPT_TO              15 */
        0x02,  /* HCI_WRITE_CONNECT_ACCEPT_TO             16 */
        0x00,  /* HCI_READ_PAGE_TIMEOUT                   17 */
        0x02,  /* HCI_WRITE_PAGE_TIMEOUT                  18 */
        0x00,  /* HCI_READ_SCAN_ENABLE                    19 */
        0x01,  /* HCI_WRITE_SCAN_ENABLE                   1A */
        0x00,  /* HCI_READ_PAGE_SCAN_ACT                  1B */ 
        0x04,  /* HCI_WRITE_PAGE_SCAN_ACT                 1C */
        0x00,  /* HCI_READ_INQUIRY_SCAN_ACT               1D */ 
        0x04,  /* HCI_WRITE_INQUIRY_SCAN_ACT              1E */
        0x00,  /* HCI_READ_AUTHENTICATION_ENABLE          1F */
        0x01,  /* HCI_WRITE_AUTHENTICATION_ENABLE         20 */
        0x00,  /* HCI_READ_ENCRYPTION_MODE                21 */ 
        0x01,  /* HCI_WRITE_ENCRYPTION_MODE               22 */
        0x00,  /* HCI_READ_CLASS_OF_DEVICE                23 */ 
        0x03,  /* HCI_WRITE_CLASS_OF_DEVICE               24 */
        0x00,  /* HCI_READ_VOICE_SETTINGS                 25 */
        0x02,  /* HCI_WRITE_VOICE_SETTINGS                26 */
        0x02,  /* HCI_READ_AUTOMATIC_FLUSH_TIMEOUT        27 */ 
        0x04,  /* HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT       28 */
        0x00,  /* HCI_READ_NUM_BROADCAST_RETRANSMISSIONS  29 */
        0x01,  /* HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS 2A */
        0x00,  /* HCI_READ_HOLD_MODE_ACTIVITY             2B */
        0x01,  /* HCI_WRITE_HOLD_MODE_ACTIVITY            2C */
        0x03,  /* HCI_READ_TRANSMIT_POWER_LEVEL           2D */
        0x00,  /* HCI_READ_SCO_FLOW_CONTROL_ENABLE        2E */
        0x01,  /* HCI_WRITE_SCO_FLOW_CONTROL_ENABLE       2F */
        0x00,  /* Invalid Opcode                          30 */
        0x01,  /* HCI_SET_HOST_CONTROLLER_TO_HOST_FLOW_CONTROL 31 */
        0x00,  /* Invalid Opcode                          32 */
        0x07,  /* HCI_HOST_BUFFER_SIZE                    33 */
        0x00,  /* Invalid Opcode                          34 */
        0xFF,  /* HCI_HOST_NUMBER_OF_COMPLETED_PACKETS    35  - Variable */
        0x02,  /* HCI_READ_LINK_SUPERVISION_TIMEOUT       36  */
        0x04,  /* HCI_WRITE_LINK_SUPERVISION_TIMEOUT      37  */
        0x00,  /* HCI_READ_NUMBER_OF_SUPPORTED_IAC        38  */
        0x00,  /* HCI_READ_CURRENT_IAC                    39  */ 
        0xFF,  /* HCI_WRITE_CURRENT_IAC                   3A  */
        0x00,  /* HCI_READ_PAGE_SCAN_PERIOD_MODE          3B  */
        0x01,  /* HCI_WRITE_PAGE_SCAN_PERIOD_MODE         3C  */
        0x00,  /* HCI_READ_PAGE_SCAN_MODE                 3D  */
        0x01,  /* HCI_WRITE_PAGE_SCAN_MODE                3E  */ 

        0x0A,  /* HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION 3F  */ 
        0x00,  /* Invalid Opcode                          40  */ 
        0x00,  /* Invalid Opcode                          41  */ 
        0x00,  /* HCI_Read_Inquiry_Scan_Type              42  */ 
        0x01,  /* HCI_Write_Inquiry_Scan_Type             43  */ 
        0x00,  /* HCI_Read_Inquiry_Mode                   44  */ 
        0x01,  /* HCI_Write_Inquiry_Mode                  45  */ 
        0x00,  /* HCI_Read_Page_Scan_Type                 46  */ 
        0x01,  /* HCI_Write_Page_Scan_Type                47  */ 
        0x00,  /* HCI_Read_AFH_Channel_Classification_Mode 48  */ 
        0x01,  /* HCI_Write_AFH_Channel_Classification_Mode 49  */ 
        0x00,  /* HCI_Read_Anonymity_Mode                 4A  */ 
        0x01,  /* HCI_Write_Anonymity_Mode                4B  */ 
        0x00,  /* HCI_Read_Alias_Authentication_Enable    4C  */ 
        0x01,  /* HCI_Write_Alias_Authentication_Enable   4D  */ 
        0x00,  /* HCI_Read_Anonymous_Address_Change_Parameters 4E  */ 
        0x06,  /* HCI_Write_Anonymous_Address_Change_Parameters 4F  */ 
        0x01,  /* HCI_Reset_Fixed_Address_Attempts_Counter   50  */ 
        0x00,  /* HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND 51 */
        0xFF,   /* HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_COMMAND 52 */
        0x02,  /* HCI_Refresh_Encryption_Key 53 */
        0x00,  /* Not Used                                    54 */
        0x00,  /* HCI_READ_SIMPLE_PAIRING_MODE                55 */
        0x01,  /* HCI_WRITE_SIMPLE_PAIRING_MODE               56 */
        0x00,  /* HCI_READ_LOCAL_OOB_DATA                     57 */
        0x00,  /* HCI_READ_INQ_RESP_TX_POWER                  58 */
        0x01,  /* HCI_WRITE_INQ_TX_POWER                      59 */
        0x00,  /* Not Used                                    5A */
        0x01,  /* Not Used                                    5B */
        0x00,  /* Not Used                                    5C */
        0x00,  /* Not Used                                    5D */
        0x00,  /* Not Used                                    5E */
        0x03,  /* HCI_ENHANCED_FLUSH                          5F */
        0x07,  /* HCI_SEND_KEYPRESS_NOTIFICATION_COMMAND      60 */
        0x00,  /* Not Used                                    61 */
		0x00,  /* Not Used                                    62 */
		0x00,  /* Not Used                                    63 */
		0x00,  /* Not Used                                    64 */
		0x00,  /* Not Used                                    65 */
		0x00,  /* Not Used                                    66 */
		0x00,  /* Not Used                                    67 */
		0x03,  /* HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL      68 */
        0x00,  /* Not Used                                    69 */
		0x00,  /* Not Used                                    6A */
		0x00,  /* Not Used                                    6B */
		0x00,  /* HCI_READ_LE_HOST_SUPPORT                    6C */
		0x02,  /* HCI_WRITEc_LE_HOST_SUPPORT                   6D */
};


/* Informational Commands     */

CONST u_int8 information_command_len[MAX_HC_INFO_COMMAND_OPCODE+1] = {
        0x00,  /* Invalid Opcode                            */
        0x00,  /* READ_LOCAL_VER_INFO                  01   */
        0x00,  /* READ_LOCAL_HCI_COMMAND_SET		   02   */
        0x00,  /* READ_LOCAL_SUPPORTED_FEATURES        03   */
        0x01,  /* Read_Local_Extended_Features         04   */
        0x00,  /* READ_BUFFER_SIZE                     05   */
        0x00,  /* Invalid Opcode                            */
        0x00,  /* HCI_READ_COUNTRY_CODE                07   */
        0x00,  /* Invalid Opcode                            */
        0x00   /* HCI_READ_BD_ADDR                     09   */
};


CONST u_int8 le_command_len[MAX_HC_LE_COMMAND_OPCODE+1] = {
        0x00,  /* Invalid Opcode                            */
        0x08,  /* HCI_LE_SET_EVENT_MASK                      01   */
        0x00,  /* HCI_LE_READ_BUFFER_SIZE 		             02   */
        0x00,  /* HCI_LE_READ_LOCAL_SUPPORTED_FEATURES       03   */
        0x00,  /* Invalid Opcode                             04   */
        0x06,  /* HCI_LE_SET_RANDOW_ADDRESS                  05   */
        0x0F,  /* HCI_LE_SET_ADVERTISING_PARAMETERS          06   */
        0x00,  /* HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER   07   */                
        0x20,  /* HCI_LE_SET_ADVERTISING_DATA                08   */
        0x20,  /* HCI_LE_SET_SCAN_RESPONSE_DATA              09   */
        0x01,  /* HCI_LE_SET_ADVERTISE_ENABLE                0A   */
        0x07,  /* HCI_LE_SET_SCAN_PARAMETERS                 0B   */
        0x02,  /* HCI_LE_SET_SCAN_ENABLE                     0C   */
        0x19,  /* HCI_LE_CREATE_CONNECTION                   0D   */
        0x00,  /* HCI_LE_CREATE_CONNECTION_CANCEL            0E   */
        0x00,  /* HCI_LE_READ_WHITE_LIST_SIZE                0F   */
        0x00,  /* HCI_LE_CLEAR_WHITE_LIST                    10   */

        0x07,  /* HCI_LE_ADD_DEVICE_TO_WHITE_LIST            11   */
        0x07,  /* HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST       12   */
        0x0E,  /* HCI_LE_CONNECTION_UPDATE                   13   */ 
        0x05,  /* HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION     14   */
        0x02,  /* HCI_LE_READ_CHANNEL_MAP                    15   */
        0x02,  /* HCI_LE_READ_REMOTE_USED_FEATURES           16   */

        0x20,  /* HCI_LE_ENCRYPT                             17   */
        0x00,  /* HCI_LE_RAND                                18   */
        0x1C,  /* HCI_LE_START_ENCRYPTION                    19   */ 
        0x12,  /* HCI_LE_LONG_TERM_KEY_REQUEST_REPLY         1A   */
        0x02,  /* HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY 1B   */
        0x00,  /* HCI_LE_READ_SUPPORTED_STATES               1C   */
        0x01,  /* HCI_LE_RECEIVER_TEST                       1D   */
        0x03,  /* HCI_LE_TRANSMITTER_TEST                    1E   */ 
        0x00,  /* HCI_LE_TEST_END                            1F   */
};

CONST u_int8 le_smp_command_len[MAX_LE_SMP_COMMAND_OPCODE+1] = {
        0x00,  /* Invalid Opcode                                  */
        0x07,  /* LE_SMP_CONFIGURE_SECURITY                  01   */
        0x03,  /* LE_SMP_INITIATE_SECURITY 		             02   */
        0x06,  /* LE_SMP_PASSKEY_ENTRY                       03   */
        0x12,  /* LE_SMP_OOB_DATA_ENTRY                      04   */
};

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
		
/*******************/
/* Status Commands */
/*******************/

CONST u_int8 status_command_len[MAX_HC_STATUS_COMMAND_OPCODE+1] = { 
        0x00,  /* Invalid Opcode                            */
        0x02,  /* HCI_READ_FAILED_CONTACT_COUNTER       01  */
        0x02,  /* HCI_RESET_FAILED_CONTACT_COUNTER      02  */
        0x02,  /* HCI_GET_LINK_QUALITY                  03  */
        0x00,  /* Invalid Opcode                            */
        0x02,  /* HCI_READ_RSSI                         05  */
        0x02,  /* HCI_AFH_CHANNEL_MAP                   06  */
        0x03,   /* HCI_READ_CLOCK                        07  */
		0x02   /* HCI_READ_ENCRYPTION_KEY_SIZE           08  */
};


/*******************/
/* Test Commands   */
/*******************/

CONST u_int8 test_command_len[MAX_HC_TEST_COMMAND_OPCODE+1] = {
        0x00,  /*  Invalid Opcode                           */
        0x00,  /*  HCI_READ_LOOPBACK_MODE               01  */
        0x01,  /*  HCI_WRITE_LOOPBCK_MODE               02  */
        0x00,  /*  HCI_ENABLE_DUT                       03  */
		0x01,  /*  HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE  04  */
};  

#endif
