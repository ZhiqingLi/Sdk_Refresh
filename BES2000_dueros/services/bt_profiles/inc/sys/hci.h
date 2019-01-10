/****************************************************************************
 *
 * File:
 *     $Id: hci.h 666 2009-01-26 23:57:40Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 666 $
 *
 * Description:
 *     This file contains the upper layer API for the Host Controller
 *     Interface driver.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __HCI_H
#define __HCI_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Host Controller Interface (HCI) layer
 *
 *     In a typical Bluetooth implementation, the "Host" system communicates
 *     with its Bluetooth "Host Controller" using the Host Controller
 *     Interface (HCI). This interface provides a standardized way to
 *     control Bluetooth operations like inquiry, connection establishment,
 *     piconet management, and data exchange.
 *
 *     The "Host Controller" receives HCI commands from the host. Internally,
 *     it uses the Link Manager and Baseband protocols to carry out the
 *     commands, and generates events in response to the host. The HCI
 *     also allows the exchange of ACL and SCO data.
 *
 *     The HCI layer uses an HCI Transport Driver to exchange commands,
 *     events, and data with the Host Controller. The Transport Driver
 *     is specific to the transport used, such as UART, RS232, or USB.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_NUM_HANDLES constant
 *
 *     Total number of HCI handles that HCI will be required to manage.
 *     This is equal to the number of ACL and SCO links allowed, plus
 *     two more for L2CAP broadcast handles.
 *
 */
#define HCI_NUM_HANDLES (NUM_BT_DEVICES + NUM_SCO_CONNS + 2)

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HciCallbackEvent type
 *
 *     These events are passed up to the Management Entity in the
 *     ME_Callback() function. The callback parameters (in HciCallbackParms)
 *     contain a union of different pointer types that are used depending
 *     on the event received.
 */
typedef U8 HciCallbackEvent;

/* An event was received from the host controller. Events occur in response
 * to commands sent by the host, although several events can be received
 * for a single command.
 *
 * The "hciEvent" callback parameter contains the data associated with the
 * command. The "status" field will always indicate success, and the
 * "hciHandle" field is not used.
 */
#define HCI_CONTROLLER_EVENT 1

/* Either SCO or ACL data was received from another device. This data can
 * be found in the "rxBuff" field. The type of data (ACL or SCO)
 * is identified in the rxBuff->flags field. The "hciHandle" field will also
 * be set with the ACL or SCO connection handle. The "status" field will
 * always indicate success.
 */
#define HCI_DATA_IND         2

/* A transmitted ACL or SCO packet is free to be re-used by
 * higher layers. The returned packet is identified in the "packet" field,
 * and the "hciHandle" field indicates the connection on which the packet
 * was sent.
 *
 * If the "status" field indicates success, the packet was successfully sent
 * to the host controller, but the remote device might not have received
 * the data yet.
 *
 * If the "status" field indicates failure, the packet was not sent
 * successfully due to link disconnection or a transport error.
 */
#define HCI_PACKET_HANDLED   3

/* A transmitted command is free to be re-used by higher layers.
 * The returned command is identified in the "cmd" field. The
 * "hciHandle" field is not used.
 *
 * If the "status" field indicates success, the command was successfully
 * transmitted to the host controller. One or more events will follow
 * in response.
 *
 * If the "status" field indicates failure, the command could not be
 * sent due to a transport error.
 */
#define HCI_COMMAND_HANDLED  4

/* The HCI layer is ready for SCO or ACL transmit data. This event is
 * generated sometime after HCI_RequestToSend() is called by a higher layer.
 * The "status" field always indicates success.
 *
 * The "hciHandle" field contains the handle of the connection. During
 * the callback, data for this connection should be stored in the
 * "packet" field. The HCI layer will continue to generate this event
 * until the "packet" field is set to NULL.
 */
#define HCI_SEND_IND         5

/* HCI initialization state has changed. The "status" field indicates whether
 * initialization succeeded or failed.   If "status" is BT_STATUS_FAILED,
 * synchronization with the radio will be retried BT_HCI_NUM_INIT_RETRIES times.
 * If "status" is BT_STATUS_HCI_INIT_ERR, no more retries will be made.
 * All other callback fields are not used.
 */
#define HCI_INIT_STATUS      6

/* HCI deinitialization is complete. The "status" field indicates whether
 * deinitialization completed gracefully  or not (succeeded or failed). All other
 * callback fields are not used.
 */
#define HCI_DEINIT_STATUS    7

/* An HCI transport error occured.  This can be the result of an error in the
 * the transport, or an error generated by the Bluetooth radio.
 * An error code may be found in the "error" field.
 */
#define HCI_TRANSPORT_ERROR  8

/* Event for bluetooth logo testing */
#define HCI_BLUETOOTH_LOGO   9

/* Vendor specific event (debugging) */
#define HCI_VENDOR_SPECIFIC  10

/* End of HciCallbackEvent */


/*---------------------------------------------------------------------------
 * HciCommandType type
 *
 *     Commands are indicated to the HCI layer when calling
 *     HCI_SendCommand. A command type from the list below is specified
 *     in the "opCode" parameter to this function. Additional
 *     command-specific information is provided in the "cmd" parameter.
 */
typedef U16 HciCommandType;

/* Group: Link control commands */
#define HCC_INQUIRY                      0x0401
#define HCC_INQUIRY_CANCEL               0x0402
#define HCC_START_PERIODIC_INQ_MODE      0x0403
#define HCC_EXIT_PERIODIC_INQ_MODE       0x0404
#define HCC_CREATE_CONNECTION            0x0405
#define HCC_DISCONNECT                   0x0406
#define HCC_ADD_SCO_CONNECTION           0x0407 /* Not in 1.2 */
#define HCC_CREATE_CONNECTION_CANCEL     0x0408 /* 1.2 */
#define HCC_ACCEPT_CON_REQ               0x0409
#define HCC_REJECT_CON_REQ               0x040A
#define HCC_LINK_KEY_REQ_REPL            0x040B
#define HCC_LINK_KEY_REQ_NEG_REPL        0x040C
#define HCC_PIN_CODE_REQ_REPL            0x040D
#define HCC_PIN_CODE_REQ_NEG_REPL        0x040E
#define HCC_CHNG_CONN_PACKET_TYPE        0x040F
#define HCC_AUTH_REQ                     0x0411
#define HCC_SET_CONN_ENCRYPT             0x0413
#define HCC_CHNG_CONN_LINK_KEY           0x0415
#define HCC_MASTER_LINK_KEY              0x0417
#define HCC_REM_NAME_REQ                 0x0419
#define HCC_REM_NAME_REQ_CANCEL          0x041A /* 1.2 */
#define HCC_READ_REMOTE_FEATURES         0x041B
#define HCC_READ_REMOTE_EXT_FEATURES     0x041C /* 1.2 */
#define HCC_READ_REMOTE_VERSION          0x041D
#define HCC_READ_CLOCK_OFFSET            0x041F
#define HCC_READ_LMP_HANDLE              0x0420 /* 1.2 */
#define HCC_EXCHANGE_FIXED_INFO          0x0421 /* 1.2, not in 2.1 */
#define HCC_EXCHANGE_ALIAS_INFO          0x0422 /* 1.2, not in 2.1 */
#define HCC_PRIVATE_PAIRING_REQ_REPL     0x0423 /* 1.2, not in 2.1 */
#define HCC_PRIVATE_PAIRING_REQ_NEG_REPL 0x0424 /* 1.2, not in 2.1 */
#define HCC_GENERATED_ALIAS              0x0425 /* 1.2, not in 2.1 */
#define HCC_ALIAS_ADDRESS_REQ_REPL       0x0426 /* 1.2, not in 2.1 */
#define HCC_ALIAS_ADDRESS_REQ_NEG_REPL   0x0427 /* 1.2, not in 2.1 */
#define HCC_SETUP_SYNC_CONNECTION        0x0428 /* 1.2 */
#define HCC_ACCEPT_SYNC_CON_REQ          0x0429 /* 1.2 */
#define HCC_REJECT_SYNC_CON_REQ          0x042A /* 1.2 */
#define HCC_IO_CAPABILITY_RESPONSE       0x042B /* 2.1 */
#define HCC_USER_CONFIRM_REQ_REPL        0x042C /* 2.1 */
#define HCC_USER_CONFIRM_REQ_NEG_REPL    0x042D /* 2.1 */
#define HCC_USER_PASSKEY_REQ_REPL        0x042E /* 2.1 */
#define HCC_USER_PASSKEY_REQ_NEG_REPL    0x042F /* 2.1 */
#define HCC_REM_OOB_DATA_REQ_REPL        0x0430 /* 2.1 */
#define HCC_REM_OOB_DATA_REQ_NEG_REPL    0x0433 /* 2.1 */
#define HCC_IO_CAPABILITY_NEG_REPL       0x0434 /* 2.1 */

/* Group: Link policy commands */
#define HCC_HOLD_MODE                    0x0801
#define HCC_SNIFF_MODE                   0x0803
#define HCC_EXIT_SNIFF_MODE              0x0804
#define HCC_PARK_MODE                    0x0805
#define HCC_EXIT_PARK_MODE               0x0806
#define HCC_QOS_SETUP                    0x0807
#define HCC_ROLE_DISCOVERY               0x0809
#define HCC_SWITCH_ROLE                  0x080B
#define HCC_READ_LINK_POLICY             0x080C
#define HCC_WRITE_LINK_POLICY            0x080D
#define HCC_READ_DEFAULT_LINK_POLICY     0x080E /* 1.2 */
#define HCC_WRITE_DEFAULT_LINK_POLICY    0x080F /* 1.2 */
#define HCC_FLOW_SPECIFICATION           0x0810 /* 1.2 */
#define HCC_SNIFF_SUBRATING              0x0811 /* 2.1 */

/* Group: Host controller and baseband commands */
#define HCC_SET_EVENT_MASK               0x0C01
#define HCC_RESET                        0x0C03
#define HCC_EVENT_FILTER                 0x0C05
#define HCC_FLUSH                        0x0C08
#define HCC_READ_PIN_TYPE                0x0C09
#define HCC_WRITE_PIN_TYPE               0x0C0A
#define HCC_CREATE_NEW_UNIT_KEY          0x0C0B
#define HCC_READ_STORED_LINK_KEY         0x0C0D
#define HCC_WRITE_STORED_LINK_KEY        0x0C11
#define HCC_DEL_STORED_LINK_KEY          0x0C12
#if BT_BEST_SYNC_CONFIG == XA_ENABLED
#define HCC_WRITE_SYNC_CONFIG            0xFC51
#endif
#define HCC_CHNG_LOCAL_NAME              0x0C13
#define HCC_READ_LOCAL_NAME              0x0C14
#define HCC_READ_CONN_ACCEPT_TIMEOUT     0x0C15
#define HCC_WRITE_CONN_ACCEPT_TIMEOUT    0x0C16
#define HCC_READ_PAGE_TIMEOUT            0x0C17
#define HCC_WRITE_PAGE_TIMEOUT           0x0C18
#define HCC_READ_SCAN_ENABLE             0x0C19
#define HCC_WRITE_SCAN_ENABLE            0x0C1A
#define HCC_READ_PAGE_SCAN_ACTIVITY      0x0C1B
#define HCC_WRITE_PAGE_SCAN_ACTIVITY     0x0C1C
#define HCC_READ_INQ_SCAN_ACTIVITY       0x0C1D
#define HCC_WRITE_INQ_SCAN_ACTIVITY      0x0C1E
#define HCC_READ_AUTH_ENABLE             0x0C1F
#define HCC_WRITE_AUTH_ENABLE            0x0C20
#define HCC_READ_ENCRYPT_MODE            0x0C21 /* Not in 2.1 */
#define HCC_WRITE_ENCRYPT_MODE           0x0C22 /* Not in 2.1 */
#define HCC_READ_CLASS_OF_DEVICE         0x0C23
#define HCC_WRITE_CLASS_OF_DEVICE        0x0C24
#define HCC_READ_VOICE_SETTING           0x0C25
#define HCC_WRITE_VOICE_SETTING          0x0C26
#define HCC_READ_AUTO_FLUSH_TIMEOUT      0x0C27
#define HCC_WRITE_AUTO_FLUSH_TIMEOUT     0x0C28
#define HCC_READ_NUM_BC_RETRANSMIT       0x0C29
#define HCC_WRITE_NUM_BC_RETRANSMIT      0x0C2A
#define HCC_READ_HOLD_MODE_ACTIVITY      0x0C2B
#define HCC_WRITE_HOLD_MODE_ACTIVITY     0x0C2C
#define HCC_READ_XMIT_POWER_LEVEL        0x0C2D
#define HCC_READ_SCO_FC_ENABLE           0x0C2E
#define HCC_WRITE_SCO_FC_ENABLE          0x0C2F
#define HCC_SET_CTRLR_TO_HOST_FLOW_CTRL  0x0C31
#define HCC_HOST_BUFFER_SIZE             0x0C33
#define HCC_HOST_NUM_COMPLETED_PACKETS   0x0C35
#define HCC_READ_LINK_SUPERV_TIMEOUT     0x0C36
#define HCC_WRITE_LINK_SUPERV_TIMEOUT    0x0C37
#define HCC_READ_NUM_IAC                 0x0C38
#define HCC_READ_CURRENT_IAC_LAP         0x0C39
#define HCC_WRITE_CURRENT_IAC_LAP        0x0C3A
#define HCC_READ_PAGE_SCAN_PERIOD_MODE   0x0C3B /* Not in 2.1 */
#define HCC_WRITE_PAGE_SCAN_PERIOD_MODE  0x0C3C /* Not in 2.1 */
#define HCC_READ_PAGE_SCAN_MODE          0x0C3D /* Not in 1.2 */
#define HCC_WRITE_PAGE_SCAN_MODE         0x0C3E /* Not in 1.2 */
#define HCC_SET_AFH_HOST_CHAN_CLASS      0x0C3F /* 1.2 */
#define HCC_READ_INQ_SCAN_TYPE           0x0C42 /* 1.2 */
#define HCC_WRITE_INQ_SCAN_TYPE          0x0C43 /* 1.2 */
#define HCC_READ_INQ_MODE                0x0C44 /* 1.2 */
#define HCC_WRITE_INQ_MODE               0x0C45 /* 1.2 */
#define HCC_READ_PAGE_SCAN_TYPE          0x0C46 /* 1.2 */
#define HCC_WRITE_PAGE_SCAN_TYPE         0x0C47 /* 1.2 */
#define HCC_READ_AFH_CHAN_ASSESS_MODE    0x0C48 /* 1.2 */
#define HCC_WRITE_AFH_CHAN_ASSESS_MODE   0x0C49 /* 1.2 */
#define HCC_READ_ANONYMITY_MODE          0x0C4A /* 1.2, not in 2.1 */
#define HCC_WRITE_ANONYMITY_MODE         0x0C4B /* 1.2, not in 2.1 */
#define HCC_READ_ALIAS_AUTH_ENABLE       0x0C4C /* 1.2, not in 2.1 */
#define HCC_WRITE_ALIAS_AUTH_ENABLE      0x0C4D /* 1.2, not in 2.1 */
#define HCC_READ_ANON_ADDR_CHNG_PARM     0x0C4E /* 1.2, not in 2.1 */
#define HCC_WRITE_ANON_ADDR_CHNG_PARM    0x0C4F /* 1.2, not in 2.1 */
#define HCC_RESET_FIXED_ADDR_COUNTER     0x0C50 /* 1.2, not in 2.1 */
#define HCC_READ_EXTENDED_INQ_RESP       0x0C51 /* 2.1 */
#define HCC_WRITE_EXTENDED_INQ_RESP      0x0C52 /* 2.1 */
#define HCC_REFRESH_ENCRYPT_KEY          0x0C53 /* 2.1 */
#define HCC_READ_SIMPLE_PAIRING_MODE     0x0C55 /* 2.1 */
#define HCC_WRITE_SIMPLE_PAIRING_MODE    0x0C56 /* 2.1 */
#define HCC_READ_LOCAL_OOB_DATA          0x0C57 /* 2.1 */
#define HCC_READ_INQ_RESP_TRANS_PWR_LVL  0x0C58 /* 2.1 */
#define HCC_WRITE_INQ_TRANS_PWR_LVL      0x0C59 /* 2.1 */
#define HCC_READ_DEF_ERR_DATA_REPORTING  0x0C5A /* 2.1 */
#define HCC_WRITE_DEF_ERR_DATA_REPORTING 0x0C5B /* 2.1 */
#define HCC_ENHANCED_FLUSH               0x0C5F /* 2.1 */
#define HCC_SEND_KEYPRESS_NOTIFY         0x0C60 /* 2.1 */

/* Group: Informational parameters */
#define HCC_READ_LOCAL_VERSION           0x1001
#define HCC_READ_LOCAL_COMMANDS          0x1002 /* 1.2 */
#define HCC_READ_LOCAL_FEATURES          0x1003
#define HCC_READ_LOCAL_EXT_FEATURES      0x1004 /* 1.2 */
#define HCC_READ_BUFFER_SIZE             0x1005
#define HCC_READ_COUNTRY_CODE            0x1007 /* Not in 1.2 */
#define HCC_READ_BD_ADDR                 0x1009

/* Group: Status parameters */
#define HCC_READ_FAILED_CONTACT_COUNT    0x1401
#define HCC_RESET_FAILED_CONTACT_COUNT   0x1402
#define HCC_GET_LINK_QUALITY             0x1403
#define HCC_READ_RSSI                    0x1405
#define HCC_READ_AFH_CHANNEL_MAP         0x1406 /* 1.2 */
#define HCC_READ_CLOCK                   0x1407 /* 1.2 */

/* Group: Testing commands */
#define HCC_READ_LOOPBACK_MODE           0x1801
#define HCC_WRITE_LOOPBACK_MODE          0x1802
#define HCC_ENABLE_DUT                   0x1803
#define HCC_WRITE_SIMPLE_PAIR_DBG_MODE   0x1804 /* 2.1 */


/*group ble commands                     */
#define HCC_BLE_SET_EVENT_MASK             	0x2001
#define HCC_BLE_REM_CON_PARAM_REQ_REPLY     0x2020
#define HCC_WRITE_RANDOM_ADDR             	0x2005
#define HCC_WRITE_ADV_PARAMETER             0x2006
#define HCC_WRITE_ADV_DATA                 	0x2008
#define HCC_WRITE_ADV_ENABLE             	0x200A
#define HCC_WRITE_BLE_SCAN_PARAMETER        0x200B
#define HCC_WRITE_BLE_SCAN_ENABLE           0x200C
#define HCC_BLE_CREATE_LL_CONN           	0x200D

#define HCC_CLEAR_WHITE_LIST             	0x2010
#define HCC_ADD_DEVICE_TO_WHITELIST         0x2011
#define HCC_BLE_READ_REMOTE_FEAT        	0x2016
#define HCC_BLE_UPD_LL_CONN_PARAMS      	0x2013
#define HCC_BLE_START_ENC               	0x2019


#define HCC_WRITE_BD_ADDRESS   0xfc32
#define HCC_DBG_SET_SLEEP_EXWAKEUP_EN  0xfc47
//#define HCC_DBG_SET_SCO_SWITCH  0xfc62
#define HCC_DBG_WR_MEM_CMD_OPCODE 0xfC02
#define HCC_DBG_SET_SNIFFER_ENV_CMD_OPCODE 0xfC67

#ifdef CHIP_BEST2300
#define HCC_DBG_SET_SCO_SWITCH  0xfc89
#else
#define HCC_DBG_SET_SCO_SWITCH  0xfc62
#endif

/* End of HciCommandType */


/*---------------------------------------------------------------------------
 * HciEventType type
 *
 *     These events are indicated by HCI to the Management Entity during
 *     a HCI_CONTROLLER_EVENT callback. The event type is found in the
 *     "hciEvent->event" field of the callback parameters structure
 *     (HciCallbackParms).
 */
typedef U8 HciEventType;

#define HCE_INQUIRY_COMPLETE               0x01
#define HCE_INQUIRY_RESULT                 0x02
#define HCE_CONNECT_COMPLETE               0x03
#define HCE_CONNECT_REQUEST                0x04
#define HCE_DISCONNECT_COMPLETE            0x05
#define HCE_AUTH_COMPLETE                  0x06
#define HCE_REMOTE_NAME_REQ_COMPLETE       0x07
#define HCE_ENCRYPT_CHNG                   0x08
#define HCE_CHNG_CONN_LINK_KEY_COMPLETE    0x09
#define HCE_MASTER_LINK_KEY_COMPLETE       0x0A
#define HCE_READ_REMOTE_FEATURES_COMPLETE  0x0B
#define HCE_READ_REMOTE_VERSION_COMPLETE   0x0C
#define HCE_QOS_SETUP_COMPLETE             0x0D
#define HCE_COMMAND_COMPLETE               0x0E
#define HCE_COMMAND_STATUS                 0x0F
#define HCE_HARDWARE_ERROR                 0x10
#define HCE_FLUSH_OCCURRED                 0x11
#define HCE_ROLE_CHANGE                    0x12
#define HCE_NUM_COMPLETED_PACKETS          0x13
#define HCE_MODE_CHNG                      0x14
#define HCE_RETURN_LINK_KEYS               0x15
#define HCE_PIN_CODE_REQ                   0x16
#define HCE_LINK_KEY_REQ                   0x17
#define HCE_LINK_KEY_NOTIFY                0x18
#define HCE_LOOPBACK_COMMAND               0x19
#define HCE_DATA_BUFFER_OVERFLOW           0x1A
#define HCE_MAX_SLOTS_CHNG                 0x1B
#define HCE_READ_CLOCK_OFFSET_COMPLETE     0x1C
#define HCE_CONN_PACKET_TYPE_CHNG          0x1D
#define HCE_QOS_VIOLATION                  0x1E
#define HCE_PAGE_SCAN_MODE_CHANGE          0x1F /* Not in 1.2 */
#define HCE_PAGE_SCAN_REPETITION_MODE      0x20
#define HCE_FLOW_SPECIFICATION_COMPLETE    0x21 /* 1.2 */
#define HCE_INQUIRY_RESULT_WITH_RSSI       0x22 /* 1.2 */
#define HCE_READ_REMOTE_EXT_FEAT_COMPLETE  0x23 /* 1.2 */
#define HCE_FIXED_ADDRESS                  0x24 /* 1.2 */
#define HCE_ALIAS_ADDRESS                  0x25 /* 1.2 */
#define HCE_GENERATE_ALIAS_REQ             0x26 /* 1.2 */
#define HCE_ACTIVE_ADDRESS                 0x27 /* 1.2 */
#define HCE_ALLOW_PRIVATE_PAIRING          0x28 /* 1.2 */
#define HCE_ALIAS_ADDRESS_REQ              0x29 /* 1.2 */
#define HCE_ALIAS_NOT_RECOGNIZED           0x2A /* 1.2 */
#define HCE_FIXED_ADDRESS_ATTEMPT          0x2B /* 1.2 */
#define HCE_SYNC_CONNECT_COMPLETE          0x2C /* 1.2 */
#define HCE_SYNC_CONN_CHANGED              0x2D /* 1.2 */
#define HCE_HCI_SNIFF_SUBRATING            0x2E /* 2.1 */
#define HCE_EXTENDED_INQUIRY_RESULT        0x2F /* 2.1 */
#define HCE_ENCRYPT_KEY_REFRESH_COMPLETE   0x30 /* 2.1 */
#define HCE_IO_CAPABILITY_REQUEST          0x31 /* 2.1 */
#define HCE_IO_CAPABILITY_RESPONSE         0x32 /* 2.1 */
#define HCE_USER_CONFIRMATION_REQUEST      0x33 /* 2.1 */
#define HCE_USER_PASSKEY_REQUEST           0x34 /* 2.1 */
#define HCE_REMOTE_OOB_DATA_REQUEST        0x35 /* 2.1 */
#define HCE_SIMPLE_PAIRING_COMPLETE        0x36 /* 2.1 */
#define HCE_LINK_SUPERV_TIMEOUT_CHANGED    0x38 /* 2.1 */
#define HCE_ENHANCED_FLUSH_COMPLETE        0x39 /* 2.1 */
#define HCE_USER_PASSKEY_NOTIFICATION      0x3B /* 2.1 */
#define HCE_KEYPRESS_NOTIFICATION          0x3C /* 2.1 */
#define HCE_REMOTE_HOST_SUPPORTED_FEATURES 0x3D /* 2.1 */
#if IAG_BLE_INCLUDE == XA_ENABLED
#define HCE_BLE_EVENT                      0x3E
#endif
#define HCE_BLUETOOTH_LOGO                 0xFE
#define HCE_VENDOR_SPECIFIC                0xFF

/* End of HciEventType */


/*---------------------------------------------------------------------------
 * HcStatus type
 *
 *     The first parameter in an HCI event often contains a "status" value.
 *     0x00 means pending or success, according to the event type, but
 *     other values provide a specific reason for the failure. These
 *     values are listed below.
 */
typedef U8  HcStatus;

#define HC_STATUS_SUCCESS                           0x00
#define HC_STATUS_UNKNOWN_HCI_CMD                   0x01
#define HC_STATUS_NO_CONNECTION                     0x02
#define HC_STATUS_HARDWARE_FAILURE                  0x03
#define HC_STATUS_PAGE_TIMEOUT                      0x04
#define HC_STATUS_AUTH_FAILURE                      0x05
#define HC_STATUS_KEY_MISSING                       0x06
#define HC_STATUS_MEMORY_FULL                       0x07
#define HC_STATUS_CONN_TIMEOUT                      0x08
#define HC_STATUS_MAX_NUM_CONNS                     0x09
#define HC_STATUS_MAX_SCO_CONNS                     0x0A
#define HC_STATUS_ACL_ALREADY_EXISTS                0x0B
#define HC_STATUS_CMD_DISALLOWED                    0x0C
#define HC_STATUS_HOST_REJ_NO_RESOURCES             0x0D
#define HC_STATUS_HOST_REJ_SECURITY                 0x0E
#define HC_STATUS_HOST_REJ_PERSONAL_DEV             0x0F
#define HC_STATUS_HOST_TIMEOUT                      0x10
#define HC_STATUS_UNSUPP_FEATUR_PARM_VAL            0x11
#define HC_STATUS_INVAL_HCI_PARM_VAL                0x12
#define HC_STATUS_CONN_TERM_USER_REQ                0x13
#define HC_STATUS_CONN_TERM_LOW_RESOURCES           0x14
#define HC_STATUS_CONN_TERM_POWER_OFF               0x15
#define HC_STATUS_CONN_TERM_LOCAL_HOST              0x16
#define HC_STATUS_REPEATED_ATTEMPTS                 0x17
#define HC_STATUS_PAIRING_DISALLOWED                0x18
#define HC_STATUS_UNKNOWN_LMP_PDU                   0x19
#define HC_STATUS_UNSUPP_REMOTE_FEATURE             0x1A
#define HC_STATUS_SCO_OFFSET_REJECTED               0x1B
#define HC_STATUS_SCO_INTERVAL_REJECTED             0x1C
#define HC_STATUS_SCO_AIR_MODE_REJECTED             0x1D
#define HC_STATUS_INVALID_LMP_PARM                  0x1E
#define HC_STATUS_UNSPECIFIED_ERROR                 0x1F
#define HC_STATUS_UNSUPP_LMP_PARM                   0x20
#define HC_STATUS_ROLE_CHANGE_DISALLOWED            0x21
#define HC_STATUS_LMP_RESPONSE_TIMEDOUT             0x22
#define HC_STATUS_LMP_ERR_TRANSACT_COLL             0x23
#define HC_STATUS_LMP_PDU_DISALLOWED                0x24
#define HC_STATUS_ENCRYPTN_MODE_UNACCEPT            0x25
#define HC_STATUS_UNIT_KEY_USED                     0x26
#define HC_STATUS_QOS_NOT_SUPPORTED                 0x27
#define HC_STATUS_INSTANT_PASSED                    0x28
#define HC_STATUS_PAIRING_W_UNIT_KEY_UNSUPP         0x29
#define HC_STATUS_DIFFERENT_TRANSACTION_COLLISION   0x2A
#define HC_STATUS_INSUFF_RESOURCES_FOR_SCATTER_MODE 0x2B
#define HC_STATUS_QOS_UNACCEPTABLE_PARAMETER        0x2C
#define HC_STATUS_QOS_REJECTED                      0x2D
#define HC_STATUS_CHANNEL_CLASSIF_NOT_SUPPORTED     0x2E
#define HC_STATUS_INSUFFICIENT_SECURITY             0x2F
#define HC_STATUS_PARAMETER_OUT_OF_MANDATORY_RANGE  0x30
#define HC_STATUS_SCATTER_MODE_NO_LONGER_REQUIRED   0x31
#define HC_STATUS_ROLE_SWITCH_PENDING               0x32
#define HC_STATUS_SCATTER_MODE_PARM_CHNG_PENDING    0x33
#define HC_STATUS_RESERVED_SLOT_VIOLATION           0x34
#define HC_STATUS_SWITCH_FAILED                     0x35
#define HC_STATUS_EXTENDED_INQ_RESP_TOO_LARGE       0x36
#define HC_STATUS_SECURE_SIMPLE_PAIR_NOT_SUPPORTED  0x37

/* End of HcStatus */


/*---------------------------------------------------------------------------
 * HciHandle type
 *
 * The HciHandle contains an 12-bit connection identifier as well as
 * two bits for packet boundary flags and two bits for broadcast flags.
 * These fields are combined together to exchange information about the
 * packet data exchanged between the Host Controller and L2CAP.
 *
 */
typedef U16 HciHandle;

/* Group: Broadcast Flags. When receiving a packet from the Host Controller,
 * one of these flags may be set. If no flags are set, the packet is
 * a normal, point-to-point transmission.
 */

/* The Host Controller sets this flag in a received packet's handle
 * to indicate it was received as an Active or Piconet Broadcast packet.
 *
 * L2CAP should not use this flag for transmitted packets, but use
 * ACTIVE_BC_HANDLE instead.
 */
#define ACTIVE_BROADCAST       0x4000

/* The Host Controller sets this flag in a received packet's handle
 * to indicate it was received as a Piconet Broadcast packet while
 * in "park" mode.
 *
 * L2CAP should not use this flag for transmitted packets, but use
 * PICONET_BC_HANDLE instead.
 */
#define PICONET_BROADCAST      0x8000

/* Represents an invalid Host Controller handle. */
#define INVALID_HCI_HANDLE      0xFFFF

/* Broadcast handle used by L2CAP to transmit to all active slaves
 * (not including parked slaves).
 */
#define ACTIVE_BC_HANDLE       (0x0F01 | ACTIVE_BROADCAST)

/* Broadcast handle used by L2CAP to transmit to all slaves, including
 * both parked and active slaves.
 */
#define PICONET_BC_HANDLE       (0x0F00 | PICONET_BROADCAST)

/* Group: Packet Boundary Flags. When receiving a packet from the Host
 * Controller, either of these two flags will be set. When sending a
 * packet to the Host Controller, the HCI layer fills in these
 * fields automatically.
 */
#define PBF_MASK               0x3000 /* Mask for operations on the
                                       * Packet Boundry Flags (PBF).
                                       */
#define FIRST_NON_FLUSH_PKT    0x0000 /* This packet fragment is the
                                       * non-flushable first fragment
                                       * of an L2CAP packet.
                                       */
#define CONTINUING_PACKET      0x1000 /* This packet fragment continues
                                       * a previously started L2CAP packet.
                                       */
#define FIRST_FLUSHABLE_PKT    0x2000 /* This packet fragment is the
                                       * flushable first fragment of
                                       * an L2CAP TX packet.
                                       */
#define FIRST_PACKET           0x2000 /* This packet fragment is the
                                       * flushable first fragment of
                                       * an L2CAP RX packet.
                                       */

/* Group: When Erroneous Data Reporting enabled, a buffer received from the
 * Host Controller will set these bits as described below.
 */
#define SCO_DATA_CORRECT       0x0000 /* This SCO packet is valid */

#define SCO_DATA_INVALID       0x1000 /* This SCO packet is invalid */

#define SCO_DATA_NONE          0x2000 /* No data was received for this
                                       * SCO packet
                                       */

#define SCO_DATA_PARTIAL       0x3000 /* Partial data was received for this
                                       * SCO packet
                                       */

/* End of HciHandle */


/*---------------------------------------------------------------------------
 * HciPrescanFlags type
 *
 *     Describes the order in which prescan structures will be registered.
 */
typedef U8 HciPrescanFlags;

#define PRESCAN_FLAG_LAST      0x00
#define PRESCAN_FLAG_FIRST     0x01

/* End of HciPrescanFlags */

/* Forward reference for HciPacket */
typedef struct _HciPacket HciPacket;

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * HciCommand structure
 *
 *     Represents an HCI command to be sent to the host controller when
 *     calling HCI_SendCommand.
 *
 *     The parameters buffer contains a set of command-specific parameters.
 *     The format and description for these parameters can be found in
 *     the Bluetooth HCI specification.
 */
typedef struct _HciCommand {
    ListEntry node;      /* For internal use only */

    U8        header[3]; /* For internal use only */

    /* Command-specific parameters provided by the caller of
     * HCI_SendCommand. Note that the length of this buffer is
     * limited to HCI_CMD_PARM_LEN in btconfig.h. If a larger paramters
     * buffer is required, override this configuration setting
     * in overide.h.
     */
    U8        parms[HCI_CMD_PARM_LEN];

} HciCommand;

/*---------------------------------------------------------------------------
 * HciBuffer structure
 *
 *     Represents a buffer of data received from the hardware by the
 *     transport. Except for "node", all of the fields in this structure
 *     must be initialized before providing it to the HCI layer with
 *     HCI_BufferInd.
 */
typedef struct _HciBuffer
{
    ListEntry     node;     /* Node element for linked list management. */
    U16           len;      /* Length of data in "buffer". */
    U8           *buffer;   /* Pointer to received data buffer. */
    U8            flags;    /* Flags to identify the type of packet.
                             * Actually uses the HciBufFlags type.
                             */
#if XA_STATISTICS == XA_ENABLED
    U32           hci_timer;
#endif /* XA_STATISTICS == XA_ENABLED */
} HciBuffer;

/*---------------------------------------------------------------------------
 * HciEvent structure
 *
 *     When the host controller generates events, these events are passed
 *     to the ME_Callback function with a callback event type of
 *     HCI_CONTROLLER_EVENT. The contents of the HCI event are found in
 *     the callback parameters "hciEvent" field.
 *
 *     The parameters buffer contains a set of event-specific parameters.
 *     The format and description for these parameters can be found in
 *     the Bluetooth HCI specification.
 */
typedef struct _HciEvent {
   HciEventType  event;   /* The type of HCI event received  */
   U8            len;     /* Length of the event parameters */
   U8           *parms;   /* Event-specific parameters */
#if IAG_BLE_INCLUDE == XA_ENABLED
   HciBuffer    *rx_buff;     /* Received packet  */
#endif
} HciEvent;

/*---------------------------------------------------------------------------
 * HciCallbackParms structure
 *
 *     To indicate different kinds of events to the Management Entity,
 *     the HCI layer calls the ME_Callback function with an HciCallbackEvent
 *     and a callback parameters structure (this one).
 *
 *     Different fields in the "ptr" union are valid depending on the
 *     event type. See the definition for HciCallbackEvent for more
 *     information.
 */
typedef struct _HciCallbackParms {
    BtStatus  status;       /* May indicate the status of an operation */
    HciHandle hciHandle;    /* May indicate the HCI handle concerned with the
                             * event.
                             */

    /* Group: Only one of the following fields may be valid. The correct
     * field to use will depend on the HciCallbackEvent received by
     * the ME_Callback.
     */
    union {
        HciEvent   *hciEvent;   /* Host controller event      */
        HciBuffer  *rxBuff;     /* Received packet  */
        BtPacket   *packet;     /* Returned transmit packet   */
        HciCommand *cmd;        /* Host controller command    */
        HciTransError error;    /* Error that caused transport failure */
    } ptr;
} HciCallbackParms;

/*---------------------------------------------------------------------------
 * HciFowParms structure
 *
 *     A structure used to hold the current HCI flow parameters.  This
 *     structure contains the maximum size of ACL and SCO transmit packets,
 *     as well as the current number of available buffers on the Host
 *     Controller.
 */
typedef struct _HciFlowParms {
    U16  aclTxPacketLen;
    U16  aclTxPacketsLeft;
    U8   scoTxPacketLen;
    U16  scoTxPacketsLeft;
} HciFlowParms;

/* ME callback function */
void ME_Callback(U8 event, HciCallbackParms *parms);

/*---------------------------------------------------------------------------
 * RadioHandlerCallback type
 *
 *     Called by the HCI driver to allow radio initialization.
 */
typedef void (*RadioHandlerCallback)(U8 event, HciCallbackParms *parms);

/* End of RadioHandlerCallback */


#if HCI_ALLOW_PRESCAN == XA_ENABLED

typedef BOOL (*HciTxPrescanHandler)(HciPacket *Packet);
typedef BOOL (*HciTxDoneHandler)(HciPacket *Packet);
typedef BOOL (*HciRxPrescanHandler)(HciBuffer *Buffer);
typedef BOOL (*HciRxDoneHandler)(HciBuffer *Buffer);

/*---------------------------------------------------------------------------
 * HciPrescanHandler structure
 *
 *     Handler used to register functions that examine HCI data before it is
 *     transmitted to the radio and when it is received from the radio.  This
 *     can be used for debugging (as in sniffer.c) or to actually modify
 *     HCI data in cases where the formatting of the data needs to be (or is)
 *     different than the current Bluetooth spec.  These are very powerful
 *     functions, which can trouble if not used properly.  If the data is
 *     not modified in the function, and the function always returns FALSE,
 *     then they should cause no problems.
 */
typedef struct _HciPrescanHandler {
    ListEntry node;                 /* Used internally by HCI */


    /* Called before data is transmittted. Return TRUE if the event
     * was handled. If handled, the prescan is responsible for delivering
     * appropriate notifications back to HCI concerning the progress
     * of the command.
     */
    HciTxPrescanHandler hciTxPrescan;

    /* Called when the transmit is complete. Return TRUE if the event
     * was handled.  If TRUE is returned, the HCI_COMMAND_HANDLED event
     * will not be passed up to ME.
     */
    HciTxDoneHandler hciTxDone;

    /* Called before receive data is sent up to the Management
     * Entity.  Return TRUE if the event was handled. If TRUE is
     * returned, the data (or event) will not be passed up to ME.
     */
    HciRxPrescanHandler hciRxPrescan;

    /* Called when the receive data has been handled. Return TRUE if the
     * event was handled. If TRUE is returned, the prescan must be
     * responsible for returning the receive buffer to RXBUFF.
     */
    HciRxDoneHandler hciRxDone;

} HciPrescanHandler;

/* End of HciPrescanHandler */

#endif /* HCI_ALLOW_PRESCAN == XA_ENABLED */

/****************************************************************************
 *
 * Function Reference - Upper layer functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_Init()
 *
 *     Initialize the HCI queues, receive buffers, and broadcast handles.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - HCI is initialized.
 *
 *     BT_STATUS_FAILED - hardware failed to initialize.
 */
BtStatus HCI_Init(void);

/*---------------------------------------------------------------------------
 * HCI_Deinit()
 *
 *     Deinitialize the HCI.  Return all resources to the upper layers.
 */
void HCI_Deinit(void);

/*---------------------------------------------------------------------------
 * HCI_Reset()
 *
 *     Sends the HCC_RESET command only. Called by Radio_Init in the module
 *     initialization code to give flexibility on how and when it is called
 *     during the radio initialization.
 *
 * Returns:
 *     BT_STATUS_PENDING - HCI is initializing the radio to a known state.
 *       HCI is initializing the radio to a known state. The status will
 *       indicate successful or unsucessfull attempt.
 *
 */
BtStatus HCI_Reset(void);

/*---------------------------------------------------------------------------
 * HCI_Config()
 *
 *    Initiates the sequence of commands for negotiating HCI Flow Control
 *    including the following:
 *      HCC_READ_BUFFER_SIZE,
 *      HCC_SET_CTRLR_TO_HOST_FLOW_CTRL,
 *      HCC_HOST_BUFFER_SIZE,
 *      HCC_WRITE_SCO_FC_ENABLE
 *    When the process is completed, callbacks of one of these commands will
 *    indicate to the ME that the HCI is initialized vi HCI_RadioInitialized.
 *
 * Returns:
 *     BT_STATUS_PENDING - HCI is now initializing the Host Controller.
 *         When initialization is complete, the Management Entity will be
 *         called with an HCI_INIT_STATUS event. The "status" field will
 *         indicate whether the intialization was successful.
 */
BtStatus HCI_Config(void);

/*---------------------------------------------------------------------------
 * HCI_RegisterRadioHandler()
 *
 *     Called by the Radio Manager to register a radio handler.  Radio
 *     handlers are modules that send vendor specific commands and handle
 *     events required to properly initialize a radio.  Only one handler is
 *     allowed to be registered at a time (since only one radio is supported
 *     at a time).  A handler must be registered before the HCI is initialized.
 *
 *     Once the HCI is properly initialized, the handler will be called with
 *     an HCI_INIT_STATUS event.  At this time the handler can begin to send
 *     any vendor specific events that it requires.
 *
 * Parameters:
 *     radioHandlerCallback - Radio handler callback entry point.  This parameter
 *         should contain a pointer to the radio handler's callback function.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The radio handler callback was successfully
 *         registered with HCI.
 *
 *     BT_STATUS_FAILED - The radio handler was not registered, because the HCI
 *         has already been initialized.
 */
void HCI_RegisterRadioHandler(RadioHandlerCallback radioHandlerCallback);

/*---------------------------------------------------------------------------
 * HCI_DeregisterRadioHandler()
 *
 *     Called by the transport driver to deregister a radio handler.  Radio
 *     handlers are modules that send vendor specific commands and handle
 *     events required to properly initialize a radio.  Once this function is
 *     called, the HCI will no longer call the radio handler, but direct
 *     events to the ME.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
void HCI_DeregisterRadioHandler(void);
#define HCI_DeregisterRadioHandler() HCI_RegisterRadioHandler(0)

/*---------------------------------------------------------------------------
 * HCI_RadioInitialized()
 *
 *     Called by the radio manager to indicates to the HCI that radio
 *     initialization is complete.
 *
 * Parameters:
 *     Status - Tells the status of initialization.  If successful, then
 *         the HCI is operational.  If not successful, then initialization
 *         failed.
 *
 * Returns:
 *     void
 */
void HCI_RadioInitialized(BtStatus Status);


/*---------------------------------------------------------------------------
 * HCI_RadioDeinitializing()
 *
 *     Called by the radio manager to indicate the current status of radio
 *     deinitialization.
 *
 * Parameters:
 *     status - BT_STATUS_PENDING to indicate that deinit has
 *         begun, or BT_STATUS_SUCCESS to indicate that deinit is complete.
 *
 */
void HCI_RadioDeinitializing(BtStatus status);

/*---------------------------------------------------------------------------
 * HCI_RadioDeinitialized()
 *
 *     Synonym for HCI_RadioDeinitializing(BT_STATUS_SUCCESS).
 *
 */
void HCI_RadioDeinitialized(void);
#define HCI_RadioDeinitialized() HCI_RadioDeinitializing(BT_STATUS_SUCCESS)

/*---------------------------------------------------------------------------
 * HCI_SendCommand()
 *
 *     Starts sending a command to the host controller. After the command
 *     is completely sent to the host controller, HCI calls ME_Callback
 *     with the HCI_COMMAND_HANDLED callback event type. The callback
 *     parameters contain the command that was sent (in "ptr.cmd") and
 *     the success of the operation (in "status").
 *
 *     In response to the command, the host controller will send one
 *     or more events. These events are also passed to the ME_Callback
 *     function as described by the HciCallbackType HCI_CONTROLLER_EVENT
 *     type.
 *
 *     Before calling this function, the command's "parms" field must
 *     be filled with the appropriate command-specific parameters as
 *     defined in the Bluetooth specification. Filling the "header"
 *     field is not required.
 *
 * Parameters:
 *    opCode - The type of HCI command to send.
 *
 *    parmLen - The length of the command parameters.  The command
 *        parameter block is located in the HciCommand structure.
 *
 *    cmd - the command structure. The "cmd->parm" field must be filled
 *        with command-specific parameters.
 *
 * Returns:
 *    BT_STATUS_PENDING - the command was accepted by the HCI. The
 *        upper layer will be notified at a later time of the command
 *        completion and/or status.
 *
 *    BT_STATUS_INVALID_PARM - the "cmd" parameter is not valid
 *        (XA_ERROR_CHECK only).
 */
BtStatus HCI_SendCommand(HciCommandType opCode, U8 parmLen, HciCommand *cmd);
#define HCI_SendCommand(o, p, c) HciSendCommand(o, p, c, FALSE)

/*---------------------------------------------------------------------------
 * HCI_IsCommandBusy()
 *
 *     Checks if HciCommand parameter is still in use.
 *
 * Parameters:
 *    cmd - the command structure to check.
 *
 * Returns:
 *    TRUE - the "cmd" parameter is still in use.
 *
 *    FALSE - the "cmd" parameter is free.
 */
BOOL HCI_IsCommandBusy(HciCommand *cmd);

#if HCI_ALLOW_PRESCAN == XA_ENABLED
/*---------------------------------------------------------------------------
 * HCI_RegisterPrescan()
 *
 *     Used to register functions that examine HCI data before it is
 *     transmitted to the radio and when it is received from the radio.  This
 *     can be used for debugging (as in sniffer.c) or to actually modify
 *     HCI data in cases where the formatting of the data needs to be (or is)
 *     different than the current Bluetooth spec.  This call may be called
 *     only after the stack initialization completes (EVM_Init returns).  It
 *     should NOT be called at interrupt time or during the execution of the
 *     high-priority driver thread (see OS_StopHardware).
 *
 * Parameters:
 *     Handler - Structure that contains entry points to the prescan handlers.
 *               The hciTxPrescan, hciTxDone, hciRxPrescan, and hciRxDone
 *               fields must be initialized before calling this function.
 *               Multiple handlers can be registered and will be called in
 *               the order that they are registered.  A NULL value can be
 *               assigned to an entry point to indicate that no prescan
 *               is required for that particular function.
 *
 *     Flags - The Flags describe how the prescan structure will be
 *             registered.  One of the following values should be used:
 *
 *             PRESCAN_FLAG_LAST (default) - If this flag is used, the prescan
 *                 will be registered at the end of the current list.
 *
 *             PRESCAN_FLAG_FIRST - If this flag is set, the prescan will
 *                 be registered at the front of the current list.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler was registered successfully.
 *
 *     BT_STATUS_INVALID_PARM - The Handler structure was not properly
 *         initialized.
 *
 *     BT_STATUS_IN_USE - The handler was not registered, because it is
 *         already in use.
 */
BtStatus HCI_RegisterPrescan(HciPrescanHandler *Handler, HciPrescanFlags Flags);

/*---------------------------------------------------------------------------
 * HCI_UnegisterPrescan()
 *
 *     Used to unregister the prescan handler (see HCI_RegisterPrescan()).
 *
 * Parameters:
 *     Handler - Structure that contains entry points to the prescan handlers.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler was unregistered successfully.
 *
 *     BT_STATUS_INVALID_PARM - The Handler structure was not valid.
 *
 *     BT_STATUS_FAILED - The handler was not registered, and could not be
 *         unregistered.
 */
BtStatus HCI_UnregisterPrescan(HciPrescanHandler *Handler);
#endif /* HCI_ALLOW_PRESCAN == XA_ENABLED */

/****************************************************************************
 *
 * Function Reference - Internal functions.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_RequestToSend()
 *
 *     Called to notify HCI that the Management Entity has SCO or ACL
 *     data ready to send.  Must only be called when the stack is already locked.
 *
 *     Later, when HCI is running on the stack thread and is able to
 *     send, it will call ME_Callback with the HCI_SEND_IND event. The
 *     callback parameters will contain the requested HciHandle to
 *     indicate which connection is ready for data.
 *
 *     During the callback, the "packet" field of the callback
 *     parameters structure is filled with a BtPacket to be sent,
 *     or filled with NULL to indicate that no more packets to send
 *     on that connection. The HCI layer will continue to generate
 *     HCI_SEND_IND events until the "packet" field is returned
 *     as NULL.
 *
 *     When a packet has been completely sent to the Host Controller,
 *     the HCI calls ME again with an HCI_PACKET_HANDLED event.
 *     The "packet" callback parameters field contains a pointer to
 *     the packet that was transmitted.  The "status" field indicates
 *     whether the packet was sent successfully.
 *
 *     Requests to send on multiple simultaneous connections are
 *     allowed.
 *
 * Parameters:
 *     handle - Data is ready to be transmitted for this HCI connection
 *         handle.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the connection
 *         associated with handle could not be found.
 */
BtStatus HCI_RequestToSend(HciHandle handle);

/*---------------------------------------------------------------------------
 * HCI_GetFlowParms()
 *
 *     Called to retrieve the negotiated flow control parameters of the
 *     HCI transport.  This information may be useful for upper layers that
 *     wish to optimize the transmistion of data in BtPackets.  It is especially
 *     useful for the transmission of SCO/eSCO data, which should be sent in
 *     packets equal to the size advertized by the radio.
 *
 * Parameters:
 *     Parms - A pointer to a memory structure that will receive the flow
 *         control information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the parameters have
 *         not yet been negotiated.
 */
BtStatus HCI_GetFlowParms(HciFlowParms *Parms);

/*---------------------------------------------------------------------------
 * HCI_SetScoPacketSize()
 *
 *     Called to change the negotiated SCO packet size.  The ability
 *     to change the SCO packet size is important when BT_SCO_HCI_DATA
 *     is enabled and the transport is USB.  USB has very strict
 *     requirements for the packet size, and often radios do not
 *     advertise the appropriate size.  In addition, the size should
 *     be changed if multiple SCO channels are used.  This function should
 *     only be called if the HCI has completed initialization.
 *
 *     Example USB packet sizes:
 *
 *     16 bit audio:  Each SCO channel requires 48 byte packets.
 *     8  bit audio:  Each SCO channel requires 24 byte packets.
 *
 * Parameters:
 *     scoPacketLen - The new SCO packet size.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The request was successful
 *
 *     BT_STATUS_FAILED - The request failed because the parameters have
 *                        not yet been negotiated.
 */
BtStatus HCI_SetScoPacketSize(U8 scoPacketLen);

/****************************************************************************
 *
 * Function Reference - Lower-layer functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HCI_Process()
 *
 *     Called by EVM_Process() to give a thread of execution to the
 *     Bluetooth stack. During this call, all received data and events
 *     are processed. Requests to send data or commands are handled as well.
 *     Must only be called when the stack is already locked.
 */
void HCI_Process(void);


BtStatus  HCI_InitConn_packetsending(HciHandle aclhandle);
BtStatus  HCI_DeInitConn_packetsending(HciHandle aclhandle);
BtStatus  HCI_Inc_Conn_packetsending(HciHandle aclhandle);
BtStatus  HCI_Dec_Conn_packetsending(HciHandle aclhandle);
U8  HCI_Get_Conn_packetsending(HciHandle aclhandle);




#endif /* __HCI_H */






