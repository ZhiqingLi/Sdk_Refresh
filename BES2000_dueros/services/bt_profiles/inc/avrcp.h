/*****************************************************************************
 *
 * File:
 *     $Id: avrcp.h 2848 2009-08-12 21:10:00Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 2848 $
 *
 * Description: This file contains public definitions for the Audio/Video 
 *     Remote Control Profile (AVRCP).  
 *
 * Created:     Nov 19, 2003
 *
 * Copyright 2003-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved.  All unpublished rights reserved.
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

#ifndef __AVRCP_H_
#define __AVRCP_H_

#include "avctp.h"
#include "conmgr.h"
#include "sec.h"

/*---------------------------------------------------------------------------
 * AVRCP API layer
 *
 *     The Audio/Video Remote Control Profile (AVRCP) defines procedures for
 *     exchanging 1394 Trade Association AV/C commands between Bluetooth
 *     enabled Audio/Video devices. In addition, more advanced Bluetooth
 *     specific AV/C commands are available for exchanging media information
 *     and player settings.  Finally, if media browsing is supported (AVRCP
 *     1.4), AVRCP specific browsing commands can be exchanged. Internally,
 *     AVRCP uses the Audio/Video Control Transport Protocol (AVCTP) to
 *     exchange messages.
 *
 *     This API is designed to support AV remote control applications using
 *     the BES Blue SDK core protocol stack.  It provides an API
 *     for connection management and message handling.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AVRCP_RTX_CMD_TIMEOUT constant
 *    This response timeout value is used to terminate a command when the
 *    remote device becomes unresponsive.
 * 
 *    Default value is 200.
 */
#ifndef AVRCP_RTX_CMD_TIMEOUT
#define AVRCP_RTX_CMD_TIMEOUT 200
#endif

/*---------------------------------------------------------------------------
 * AVRCP_RTX_ADV_CMD_TIMEOUT constant
 *    This response timeout value is used to terminate a command when the
 *    remote device becomes unresponsive.
 * 
 *    Default value is 1000.
 */
#ifndef AVRCP_RTX_ADV_CMD_TIMEOUT
#define AVRCP_RTX_ADV_CMD_TIMEOUT 1000
#endif
                  
/*---------------------------------------------------------------------------
 * AVRCP_RTX_BROWSE_CMD_TIMEOUT constant
 *    This response timeout value is used to terminate a command when the
 *    remote device becomes unresponsive.
 * 
 *    Default value is 10000.
 */
#ifndef AVRCP_RTX_BROWSE_CMD_TIMEOUT
#define AVRCP_RTX_BROWSE_CMD_TIMEOUT 10000
#endif
                            
/*---------------------------------------------------------------------------
 * AVRCP_VERSION_1_3_ONLY
 * 
 *     Enables only the features associated with AVRCP profile version 1.3.
 *     AVRCP 1.3 will be the advertized version in the SDP entry of the local
 *     device.  All feature specific to AVRCP version 1.4 will be disabled.
 */
#ifndef AVRCP_VERSION_1_3_ONLY
#define AVRCP_VERSION_1_3_ONLY XA_DISABLED //XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * AVRCP_ADVANCED_TARGET constant
 *
 *     When set to XA_ENABLED, the AVRCP profile adds handling for the
 *     Advanced AV/C features of AVRCP for the target device.  This feature
 *     is used to exchanged media information and player settings with the
 *     remote device.
 *
 *     The application will receive AVRCP_EVENT_ADV_INFO and
 *     AVRCP_EVENT_ADV_REQ events for commands sent by the controller. Some
 *     commands will not generate events, but will be responded to
 *     automatically, based on the current media and player data that is set
 *     through API calls.  Several interfaces exist for setting the data that
 *     will be sent in the responses.
 */
#ifndef AVRCP_ADVANCED_TARGET
#define AVRCP_ADVANCED_TARGET XA_DISABLED
#endif /* AVRCP_ADVANCED_TARGET */

#ifndef AVRCP_ADVANCED_TARGET_SLIM
#define AVRCP_ADVANCED_TARGET_SLIM XA_DISABLED
#endif /* AVRCP_ADVANCED_TARGET_SLIM */

/*---------------------------------------------------------------------------
 * AVRCP_ADVANCED_CONTROLLER constant
 *
 *     When set to XA_ENABLED, the AVRCP profile adds handling for
 *     the advanced features of AVRCP for the controller device.  These
 *     feature is used to exchanged media information with the remote device.
 *
 *     For controllers, the application may invoke several functions for
 *     receiving the player settings and media information from the target
 *     device.  The application will also receive AVRCP_EVENT_ADV_RESPONSE
 *     and AVRCP_EVENT_ADV_NOTIFY events.
 */
#ifndef AVRCP_ADVANCED_CONTROLLER
#define AVRCP_ADVANCED_CONTROLLER XA_DISABLED
#endif /* AVRCP_ADVANCED_CONTROLLER */

/*---------------------------------------------------------------------------
 * AVRCP_PROVIDER_NAME constant
 *
 *     This constant defines a string that will be included in the SDP
 *     record which describes the provider name.
 */
#ifndef AVRCP_PROVIDER_NAME
#define AVRCP_PROVIDER_NAME 'i', 'A', 'n', 'y', 'w', 'h', 'e', 'r', 'e', ' ', \
                            'S', 'o', 'l', 'u', 't', 'i', 'o', 'n', 's', ',', \
                            ' ', 'I', 'n', 'c', '.', '\0'
#endif /* AVRCP_PROVIDER_NAME */

/*---------------------------------------------------------------------------
 * AVRCP_PROVIDER_NAME_LEN constant
 *
 *     This constant defines the length of the string that will be included 
 *     in the SDP record which describes the provider name.
 */
#ifndef AVRCP_PROVIDER_NAME_LEN
#define AVRCP_PROVIDER_NAME_LEN 26
#endif /* AVRCP_PROVIDER_NAME_LEN */

/*---------------------------------------------------------------------------
 * AVRCP_CT_SERVICE_NAME constant
 *
 *     This constant defines a string that will be included in the SDP
 *     record which describes the controller service name.
 */
#ifndef AVRCP_CT_SERVICE_NAME
#define AVRCP_CT_SERVICE_NAME 'A', 'V', 'R', 'C', 'P', ' ', 'R', 'e', 'm', 'o', \
                              't', 'e', ' ', 'C', 'o', 'n', 't', 'r', 'o', 'l', '\0'
#endif /* AVRCP_CT_SERVICE_NAME */

/*---------------------------------------------------------------------------
 * AVRCP_CT_SERVICE_NAME_LEN constant
 *
 *     This constant defines the length of the string that will be included 
 *     in the SDP record which describes the controller service name.
 */
#ifndef AVRCP_CT_SERVICE_NAME_LEN
#define AVRCP_CT_SERVICE_NAME_LEN 21
#endif /* AVRCP_CT_SERVICE_NAME_LEN */

/*---------------------------------------------------------------------------
 * AVRCP_TG_SERVICE_NAME constant
 *
 *     This constant defines a string that will be included in the SDP
 *     record which describes the target service name.
 */
#ifndef AVRCP_TG_SERVICE_NAME
#define AVRCP_TG_SERVICE_NAME 'A', 'V', 'R', 'C', 'P', ' ', 'R', 'e', 'm', 'o', \
                              't', 'e', ' ', 'C', 'o', 'n', 't', 'r', 'o', 'l', \
                              ' ', 'T', 'a', 'r', 'g','e', 't', '\0'
#endif /* AVRCP_TG_SERVICE_NAME */

/*---------------------------------------------------------------------------
 * AVRCP_TG_SERVICE_NAME_LEN constant
 *
 *     This constant defines the length of the string that will be included
 *     in the SDP record which describes the target service name.
 */
#ifndef AVRCP_TG_SERVICE_NAME_LEN
#define AVRCP_TG_SERVICE_NAME_LEN 28
#endif /* AVRCP_TG_SERVICE_NAME_LEN */

/*---------------------------------------------------------------------------
 * AVRCP_PANEL_COMPANY_ID constant
 *
 *     Defines the company ID used in UNIT INFO commands received by a
 *     target.  This value may be left at the default or replaced with a
 *     value assigned by the IEEE Registration Authority Committee as defined
 *     in the 1394 AV/C Digital Interface Command Set, version 1.1, section
 *     9.12.
 *
 *     This 24-bit value is expressed in a string as a three-byte value with
 *     most significant byte first.
 */
#define AVRCP_PANEL_COMPANY_ID "\xFF\xFF\xFF"

/*---------------------------------------------------------------------------
 * AVRCP_SUBUNIT_OP_QUEUE_MAX constant
 *
 *     Defines the length of the internal operation queue used on the
 *     controller side by AVRCP_SetPanelKey. To buffer n keystrokes,
 *     set this constant to (n*2)+1. Attempts to add keystrokes beyond
 *     the queue size with AVRCP_SetPanelKey() will fail.
 */
#ifndef AVRCP_SUBUNIT_OP_QUEUE_MAX
#define AVRCP_SUBUNIT_OP_QUEUE_MAX 15

#if AVRCP_SUBUNIT_OP_QUEUE_MAX > 255
#error AVRCP_SUBUNIT_OP_QUEUE_MAX must be no greater than 255.
#endif /* AVRCP_SUBUNIT_OP_QUEUE_MAX > 255 */ 

#endif /* AVRCP_SUBUNIT_OP_QUEUE_MAX */ 

/*---------------------------------------------------------------------------
 * AVRCP_BLUETOOTH_COMPANY_ID constant
 *
 *     Defines the company ID used to identify Bluetooth Vendor Dependent
 *     commands. This value was assigned by the IEEE Registration Authority
 *     Committee.
 *
 *     This 24-bit value is expressed in a string as a three-byte value with 
 *     most significant byte first.
 */
#define AVRCP_BLUETOOTH_COMPANY_ID "\x00\x19\x58"

/*---------------------------------------------------------------------------
 * AVRCP_MAX_CHAR_SETS constant
 *
 *     Defines the maximum number of supported character sets that will be 
 *     read by the TG and sent by the CT.  The character sets define all the 
 *     supported IANA character sets supported by the device.  UTF-8 is the 
 *     default and must be supported by all devices.  
 */
#ifndef AVRCP_MAX_CHAR_SETS
#define AVRCP_MAX_CHAR_SETS  10

#endif /* AVRCP_MAX_CHAR_SETS */ 

/*---------------------------------------------------------------------------
 * AVRCP_ADVANCED_RESPONSE_SIZE constant
 *
 *     Defines the maximum size of a response received by the controller or 
 *     sent by the target.  This size should be large enough to handle 
 *     the largest possible media attribute response.  The response includes 
 *     all the attribute strings + 8 bytes per attribute and one more byte 
 *     to hold the number of attributes in the response.  The supported media 
 *     attributes are defined by AvrcpMediaAttrId.  
 *
 *     If a response is too large for the allocated buffer on the target,
 *     only part of the response will be sent.  If a response received by the
 *     controller is too large, the response will be aborted.
 *
 *     The default size is 1024 bytes.  
 */
#ifndef AVRCP_ADVANCED_RESPONSE_SIZE
#define AVRCP_ADVANCED_RESPONSE_SIZE    1024

#endif /* AVRCP_ADVANCED_RESPONSE_SIZE */ 

/*---------------------------------------------------------------------------
 * AVRCP_NO_TRACK_CURRENTLY_SELECTED constant
 *
 *     The following value should be used for the hi and low playback
 *     positions and track indices when no track is currently selected.
 */ 
#define AVRCP_NO_TRACK_CURRENTLY_SELECTED 0xFFFFFFFF

                                                             
#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED

/*---------------------------------------------------------------------------
 * AVRCP_MAX_MEDIA_PLAYERS constant
 * 
 *     Defines the maximum number of media players that can be registered on
 *     a device.  Media players are entities to which advanced commands can
 *     be routed.  If browsing is supported, media libraries on specified
 *     players can be browsed.
 * 
 *     The default is 10.
 */
#ifndef AVRCP_MAX_MEDIA_PLAYERS
#define AVRCP_MAX_MEDIA_PLAYERS 10
#endif /* AVRCP_MAX_MEDIA_PLAYERS */

/*--------------------------------------------------------------------------
 * AVRCP_BROWSING_TARGET constant
 * 
 *     Defines whether the browsing channel (target role) is supported for
 *     browsing media players and their libraries.
 * 
 *     This feature is enabled by default.
 */
#if AVRCP_ADVANCED_TARGET == XA_ENABLED
/* Advanced control is supported */
#ifndef AVRCP_BROWSING_TARGET
#define AVRCP_BROWSING_TARGET XA_ENABLED
#endif
#else
/* Advanced control is not supported */
#if AVRCP_BROWSING_TARGET == XA_ENABLED

/* Browsing target is not supported if advanced target is not supported */
#error AVRCP_ADVANCED_TARGET must be enabled

#endif
#endif

/*--------------------------------------------------------------------------
 * AVRCP_BROWSING_CONTROLLER constant
 * 
 *     Defines whether the browsing channel (controller role) is supported
 *     for browsing media players and their libraries.
 * 
 *     This feature is enabled by default.
 */
#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED
/* Advanced control is supported */
#ifndef AVRCP_BROWSING_CONTROLLER
#define AVRCP_BROWSING_CONTROLLER XA_ENABLED
#endif
#else
/* Advanced control is not supported */
#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED

/* Browsing controller is not supported if advanced controller is not
 * supported
 */ 
#error AVRCP_ADVANCED_CONTROLLER must be enabled

#endif
#endif

/* For convenience */
#define AVRCP_BROWSING_ENABLED ((AVRCP_BROWSING_TARGET == XA_ENABLED)     || \
                                (AVRCP_BROWSING_CONTROLLER == XA_ENABLED))

/*--------------------------------------------------------------------------
 * AVRCP_LIST_PLAYERS_ENABLED constant
 * 
 *     If AVRCP_LIST_PLAYERS_ENABLED, then the browsing channel is enabled
 *     for the target (even if AVRCP_BROWSING_TARGET is disabled).  If a
 *     target supports Category 1 or Category 3 features, and it is an AVRCP
 *     1.4 device (AVRCP_VERSION_1_3_ONLY is disabed), then a browsing
 *     channel is required, as is support for listing media players.
 * 
 *     The default value is XA_ENABLED if AVRCP_ADVANCED_TARGET is enabled
 *     and AVRCP_VERSION_1_3 is disabled.
 * 
 *     This value can be overidden in overide.h, but this should only be done
 *     if an AVRCP 1.4 device supports only the Category 2 features as a
 *     target.  
 */
#ifndef AVRCP_LIST_PLAYERS_ENABLED
#define AVRCP_LIST_PLAYERS_ENABLED ((AVRCP_ADVANCED_TARGET == XA_ENABLED) && \
                                    (AVRCP_VERSION_1_3_ONLY == XA_DISABLED))
#endif                                    
                                 

/* Test for ERTM */
#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
#if L2CAP_NUM_ENHANCED_CHANNELS == 0
#error Browsing requires at least one enhanced channel
#endif
#endif

/*--------------------------------------------------------------------------
 * AVRCP_BROWSE_AUTO_ACCEPT constant
 * 
 *     If AVRCP_BROWSE_AUTO_ACCEPT is defined to be XA_ENABLED, then the
 *     acceptor of a browsing channel will automatically accept the
 *     connection.  If AVRCP_BROWSE_AUTO_ACCEPT is defined to be XA_DISABLED,
 *     then the application must call AVRCP_BrowseConnectRsp() to accept (or
 *     reject) the connection.
 * 
 *     The default value is XA_ENABLED.
 */
#ifndef AVRCP_BROWSE_AUTO_ACCEPT
#define AVRCP_BROWSE_AUTO_ACCEPT    XA_ENABLED
#endif
                      
/*--------------------------------------------------------------------------
 * AVRCP_MAX_FOLDER_DEPTH constant
 * 
 *     Defines the maximum folder depth of a media player.
 * 
 *     The default value is 10.
 */
#ifndef AVRCP_MAX_FOLDER_DEPTH
#define AVRCP_MAX_FOLDER_DEPTH 10
#endif

#else /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/* Browsing not supported in 1.3 */
#undef AVRCP_BROWSING_TARGET
#undef AVRCP_BROWSING_CONTROLLER
#define AVRCP_BROWSING_TARGET      XA_DISABLED
#define AVRCP_BROWSING_CONTROLLER  XA_DISABLED

#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/* Other internal constants */ 

#if AVRCP_VERSION_1_3_ONLY == XA_ENABLED

#define AVRCP_NUM_SDP_SERVER_ATTRIBUTES    7

#else /* AVRCP_VERSION_1_3_ONLY == XA_ENABLED */

#define AVRCP_NUM_SDP_SERVER_ATTRIBUTES    8

#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

#define AVRCP_NUM_SDP_SEARCH_ATTRIBUTES   27
#define AVRCP_NUM_EVENTS                  13
#define AVRCP_MAX_NOTIFICATIONS           15
#define AVRCP_ADVANCED_CMD_SIZE          128

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
#define AVRCP_MAX_VOLUME                0x7F
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */


/* The number of enumeration elements 
 * defined for the AvrcpMediaAttrId type.  
 */ 
#define AVRCP_NUM_MEDIA_ATTRIBUTES        7

/* Number of milliseconds to wait before deciding that a pressed key is being
 * held (target-side)
 */
#define AVRCP_PANEL_PRESSHOLD_TIME 300

/* How long to wait before assuming that the currently held key has been
 * released (target-side)
 */
#define AVRCP_PANEL_AUTORELEASE_TIME 2000

/* How long to wait between updates for a held-down key (controller-side)
 */
#define AVRCP_PANEL_HOLDUPDATE_TIME 1000

/* These are always enabled now */
#ifndef AVRCP_PANEL_SUBUNIT
#define AVRCP_PANEL_SUBUNIT XA_ENABLED
#endif

/* For testing only */
#ifndef AVRCP_SET_DEFAULT_BRWS_PLAYER 
#define AVRCP_SET_DEFAULT_BRWS_PLAYER XA_DISABLED
#endif

/*--------------------------------------------------------------------------
 * AVRCP_NUM_PLAYER_SETTINGS constant
 * 
 *     Defines the maximum number of player settings supported on the device.
 *     This value must be greater than or equal to 4, so that room is
 *     provided for the specification defined player settings.  If greater
 *     than 4, then application extensions to the player settings are
 *     possible.
 * 
 *     The default value is 4.
 */
#ifndef AVRCP_NUM_PLAYER_SETTINGS
#define AVRCP_NUM_PLAYER_SETTINGS 4
#endif

#if AVRCP_NUM_PLAYER_SETTINGS < 4
#error AVRCP_NUM_PLAYER_SETTINGS must be 4 or greater
#endif

/*--------------------------------------------------------------------------
 * AVRCP_MAX_PLAYER_STRINGS constant
 * 
 *     Defines the maximum number of setting strings supported by application
 *     extended player setting attributes.
 * 
 *     The default value is 5.
 */
#ifndef AVRCP_MAX_PLAYER_STRINGS
#define AVRCP_MAX_PLAYER_STRINGS  5
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AvrcpEvent type
 *      
 */
typedef AvctpEvent AvrcpEvent;

/* Group: Connection events for establishing and releasing the AVRCP control
 * channel.  The control channel is used for the exchange of basic AV/C
 * commands/responses and for the exchange of AV/C commands directed to the
 * panel subunit (which includes advanced Bluetooth specific commands).
 */

/** The transport layer is connected and commands/responses can now
 *  be exchanged.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_CONNECT             AVCTP_EVENT_CONNECT

/** The application will receive this event when a lower layer connection
 *  (L2CAP) has been disconnected.  Both the target and controller of the
 *  connection are notified.
 *  
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_DISCONNECT          AVCTP_EVENT_DISCONNECT

/** A remote device is attempting to connect the transport layer.
 *  Only the acceptor of the connection is notified.  The acceptor may
 *  call AVRCP_ConnectRsp() to either accept or reject the connection.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_CONNECT_IND         AVCTP_EVENT_CONNECT_IND

/* Group: Events for the exchange of basic AV/C commands that are not routed
 * to the panel subunit.
 */

/** A AV/C command was received from the remote device (controller). This
 *  event is received for commands not routed to the panel subunit.
 * 
 *  During this callback, the 'p.cmdFrame' parameter is valid. It contains the
 *  the AVRCP command header information, including operands. If the "more"
 *  value is TRUE then this event only signals the first part of the operands.
 *  Subsequent AVRCP_EVENT_OPERANDS events will follow this event with
 *  additional operand data.
 *
 *  Note that the AVRCP specification requires that target devices respond to
 *  commands within 100ms after receiving the command.
 */
#define AVRCP_EVENT_COMMAND             AVCTP_EVENT_COMMAND

/** A AV/C response was received from the remote device (target). This event
 *  is received for responses not routed to the panel subunit.
 * 
 * During this callback, the 'p.rspFrame' parameter is valid. It contains the
 * the AVRCP response header information, including operands. If the "more"
 * value is TRUE then this event only signals the first part of the operands.
 * Subsequent AVRCP_EVENT_OPERANDS events will follow this event with
 * additional operand data.
 */
#define AVRCP_EVENT_RESPONSE            AVCTP_EVENT_RESPONSE

/** The remote device (target) rejected the AV/C command.  This event is
 *  received for responses not routed to the panel subunit.
 * 
 * During this callback, the 'p.rspFrame' parameter is valid. It contains the
 * the AVRCP reject header information, including operands. If the "more"
 * value is TRUE then this event only signals the first part of the operands.
 * Subsequent AVRCP_EVENT_OPERANDS events will follow this event with 
 * additional operand data. 
 */
#define AVRCP_EVENT_REJECT              AVCTP_EVENT_REJECT

/** A command (see AVRCP_SendCommand) or response (see AVRCP_SendResponse)
 * has been sent. Memory allocated for the operation can be freed or reused
 * after receiving this event.  This event is received for commands or 
 * responses not routed to the panel subunit. 
 * 
 * During this callback, the 'p.cmdFrame' or 'p.rspFrame' parameter associated
 * with the sent command or response is valid. They contain a pointer to the 
 * AV/C comand or response that was sent.  In addition, "status" will be set 
 * to "BT_STATUS_SUCCESS" or "BT_STATUS_FAILED" to indicate whether 
 * the event was properly delivered. 
 */
#define AVRCP_EVENT_TX_DONE             AVCTP_EVENT_TX_DONE

/** Additional operand data has been received for the previous
 *  AVRCP_EVENT_COMMAND or AVRCP_EVENT_RESPONSE.  This event is received for
 *  commands or responses not routed to the panel subunit.
 * 
 * During this callback, the 'p.cmdFrame' or 'p.rspFrame' parameter associated
 * with the received command or response is valid. The "operands" and
 * "operandLen" fields indicate the chunk of operands being received for
 * the command or response. If the "more" field is set to TRUE, the full
 * operand buffer will be received in multiple _OPERANDS events and the
 * last operand buffer indicated with the "more" field set to FALSE.
 */
#define AVRCP_EVENT_OPERANDS            AVCTP_EVENT_OPERANDS

/** An AV/C command has timed out.  Memory allocated for the command can be
 * freed or reused after receiving this event.  This event is received for 
 * commands not routed to the panel subunit. 
 * 
 * During this callback, the 'p.cmdFrame' parameter is valid. It contains a 
 * pointer to the the AV/C command that was sent. 
 */
#define AVRCP_EVENT_CMD_TIMEOUT         (AVCTP_EVENT_LAST + 1)

/* Group: Events for the exchange of standard AV/C panel subunit commands.
 */ 

/** The key corresponding to a panel operation has been pressed on the
 * remote controller device. See p.panelInd for information about the
 * operation.
 *
 * AVRCP_RejectPanelOperation may be used to reject an unsupported or
 * reserved command. If the operation is not rejected during the callback
 * it is implicitly accepted. If accepted, the next panel-related event
 * for the operation will be AVRCP_EVENT_PANEL_HOLD (if the controller key
 * is held down for at least AVRCP_PANEL_PRESSHOLD_TIME) or
 * AVRCP_EVENT_PANEL_RELEASE (if the controller key is released more quickly).
 */
#define AVRCP_EVENT_PANEL_PRESS        (AVCTP_EVENT_LAST + 2)

/** The key corresponding to a panel operation has been held down on the
 * remote controller device for at least AVRCP_PANEL_PRESSHOLD_TIME.
 * See p.panelInd for information about the operation.
 *
 * A target receiving this event should act as if the key was held down until
 * the AVRCP_EVENT_PANEL_RELEASE event is received.
 */
#define AVRCP_EVENT_PANEL_HOLD         (AVCTP_EVENT_LAST + 3)

/** The key corresponding to a panel operation has been released on the
 * remote controller device.  See p.panelInd for information about the
 * operation.
 */
#define AVRCP_EVENT_PANEL_RELEASE      (AVCTP_EVENT_LAST + 4)

/** A panel response has been received from the remote target device.
 * All fields of "p.panelCnf" are valid.
 */
#define AVRCP_EVENT_PANEL_CNF          (AVCTP_EVENT_LAST + 5)

/* Group: Events for the exchange of Bluetooth specific advanced AV/C
 * commands routed through the panel subunit.  These event are used primarily
 * for the exchange of media information (metadata) and player settings.
 */ 

/** Advanced status information has been received from the controller. During
 *  this callback, "advOp" is set to the operation type that was received
 *  from the controller, and "p.adv.info" contains the relevant information
 *  based on the operation type.  A response was automatically sent to the
 *  controller.
 */
#define AVRCP_EVENT_ADV_INFO           (AVCTP_EVENT_LAST + 6)

/** An advanced request was received and requires a response by the
 *  application.  During this callback, "advOp" is set to the operation type
 *  that was received from the controller, and "p.adv.req" contains relevant
 *  information based on the operation type.  A response should be sent
 *  quickly to avoid a timeout on the controller (see
 *  AVRCP_TgSendAdvResponse()).
 */
#define AVRCP_EVENT_ADV_REQ            (AVCTP_EVENT_LAST + 7)

/** The notification of a registered event has been received.
 *  During this callback, "p.adv.notify" contains the relevant notification
 *  information.  If "errorCode" indicates that no error occured, the 
 *  "p.adv.notify.event" field contains the type of notification, 
 *  and "p.adv.notify.value" contains the value of the notification.
 */
#define AVRCP_EVENT_ADV_NOTIFY         (AVCTP_EVENT_LAST + 8)

/** An Advanced command was sent successfully to the target and a response
 *  was received.  During this callback, "advOp" is set to the operation type
 *  that was sent to the target, and "p.adv.rsp" contains the relevant
 *  response information based on the operation type.
 */
#define AVRCP_EVENT_ADV_RESPONSE       (AVCTP_EVENT_LAST + 9)

/** No response was received for an Advanced command and it has timed out.
 *  This event is received for advanced commands routed to the panel subunit.
 * 
 *  During this callback, the 'p.cmdFrame' parameter is valid. It contains
 *  the the Advanced command that was sent. 
 */
#define AVRCP_EVENT_ADV_CMD_TIMEOUT    (AVCTP_EVENT_LAST + 10)

/** An Advanced PDU was sent successfully.  The memory used by the PDU can be
 *  freed or reused. During this callback, "p.advTxPdu" points to the memory
 *  used by the PDU.  The operation associated with the PDU is contained in
 *  "p.advTxPdu->op", and the associated parameters are in
 *  "p.advTxPdu->parms".  The transmitted PDU may have been sent in either an
 *  advanced command or response.  The "status" field contains the results of
 *  the transmission.
 */
#define AVRCP_EVENT_ADV_TX_DONE        (AVCTP_EVENT_LAST + 11)

/* Group: Connection events for establishing and releasing the AVRCP browsing
 * channel.  The browsing channel is used for the exchange of AVCTP-based
 * commands/responses which contain Bluetooth specific media
 * operations.
 */

/** The transport layer is connected and commands/responses can now
 *  be exchanged on the browsing channel.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_BROWSE_CONNECT     (AVCTP_EVENT_LAST + 12)

/** The application will receive this event when a lower layer connection
 *  (L2CAP) has been disconnected.  Both the target and controller of the
 *  connection are notified.
 *  
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_BROWSE_DISCONNECT  (AVCTP_EVENT_LAST + 13)

/** A remote device is attempting to connect the browsing channel. Only the
 *  acceptor of the connection is notified.  The acceptor may call
 *  AVRCP_BrowseConnectRsp() to either accept or reject the connection.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVRCP_EVENT_BROWSE_CONNECT_IND (AVCTP_EVENT_LAST + 14)

/* Group: Events for the exchange of browsing channel commands.  The browsing
 * channel is used for the exchange of AVCTP-based commands/responses which
 * contain Bluetooth specific media operations.
 */

/** An browsing request was received and requires a response by the
 *  application. During this callback, "advOp" is set to the operation type
 *  that was received from the controller, and "p.adv.browseReq" contains
 *  relevant information based on the operation type.  A response should be
 *  sent quickly to avoid a timeout on the controller.
 */
#define AVRCP_EVENT_BROWSE_REQ         (AVCTP_EVENT_LAST + 15)

/** Advanced status information has been received from the controller. During
 *  this callback, "advOp" is set to the operation type that was received
 *  from the controller, and "p.adv.browseInfo" contains the relevant
 *  information based on the operation type.  A response was automatically
 *  sent to the controller.
 */
#define AVRCP_EVENT_BROWSE_INFO        (AVCTP_EVENT_LAST + 16)

/** An browsing command was sent successfully to the target and a response
 *  was received.  During this callback, "advOp" is set to the operation type
 *  that was sent to the target, and "p.adv.browseRsp" contains the relevant
 *  response information based on the operation type.
 */
#define AVRCP_EVENT_BROWSE_RESPONSE    (AVCTP_EVENT_LAST + 17)

/** No response was received for a Browsing command and it has timed out.
 *  This event is received for browsing commands only.
 * 
 *  During this callback, the 'p.cmdFrame' parameter is valid. It contains
 *  the the Browsing command that was sent. 
 */
#define AVRCP_EVENT_BROWSE_CMD_TIMEOUT (AVCTP_EVENT_LAST + 18)

/** A browsing PDU was sent successfully.  The memory used by the PDU can be
 *  freed or reused. During this callback, "p.advTxPdu" points to the memory
 *  used by the PDU.  The transmitted PDU may have been sent in either an
 *  browsing command or response.  The "status" field contains the results of
 *  the transmission.
 */
#define AVRCP_EVENT_BROWSE_TX_DONE     (AVCTP_EVENT_LAST + 19)

#define AVRCP_EVENT_LAST               (AVCTP_EVENT_LAST + 19)

/* End of AvrcpEvent */ 

/*---------------------------------------------------------------------------
 * AvrcpRole type
 *
 * Describes the roles supported by the device.  A device may have both
 * target and controller roles with various features sets.  These values may
 * be OR'd together to best describe the device.  The roles and features
 * defined here are included in the SDP database.  The actual functionality
 * of the API is not limited by the values chosen here.  A device is free to
 * play either role and send any command desired, regardless of the declared
 * roles and features.  It is up to the application to advertise its 
 * capabilities properly and provide the proper functionality.
 *
 * Typically, an audio sink will support AVRCP_CT_CATEGORY_1 (play control)
 * and AVRCP_TG_CATEGORY_2 (volume control), while an audio source will support
 * AVRCP_TG_CATEGORY_1 and AVRCP_CT_CATEGORY_2 (see AVRCP_CT and AVRCP_TG).  
 * It is important to note that a device that includes volume control is both
 * a controller and a target.  An audio sink will be a controller for most 
 * functions, but a target for volume control.  The reverse is true for an
 * audio source.  The API does not restrict the sending of any command or
 * response, regardless of the roles defined, so it is important that the
 * application only send the appropriate commands/responses.
 *
 * For AVRCP 1.3 or later devices, other advanced features that are implemented 
 * by the application can be declared here as well.  For devices supporting 
 * AVRCP 1.3 or earlier, the "browsing" feature is not supported.
 * Devices that support advanced features, such as Player Settings and Group
 * Navigation, must have the features enabled in the build (see 
 * AVRCP_ADVANCED_CONTROLLER and AVRCP_ADVANCED_TARGET).
 */
typedef U16 AvrcpRole;

/* Controller: Player/recorder features supported ("play" and "stop") */ 
#define AVRCP_CT_CATEGORY_1       0x0001

/* Controller: Monitor/amplifier features supported ("volume up" and "volume down") */ 
#define AVRCP_CT_CATEGORY_2       0x0002

/* Controller: Tuner features supported ("channel up" and "channel down") */ 
#define AVRCP_CT_CATEGORY_3       0x0004

/* Controller: Menu features supported ("root menu", "up", "down", "left", 
 * "right", and "select")
 */
#define AVRCP_CT_CATEGORY_4       0x0008

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
/* Controller: Media browsing is supported */
#define AVRCP_CT_BROWSING         0x0040
#endif /* AVRCP_BROWSING_CONTROLLER == XA_ENABLED */

/* Target: Player/recorder features supported ("play" and "stop") */ 
#define AVRCP_TG_CATEGORY_1       0x0100

/* Target: Monitor/amplifier features supported ("volume up" and "volume down") */ 
#define AVRCP_TG_CATEGORY_2       0x0200

/* Target: Tuner features supported ("channel up" and "channel down") */ 
#define AVRCP_TG_CATEGORY_3       0x0400

/* Target: Menu features supported ("root menu", "up", "down", "left", 
 * "right", and "select")
 */
#define AVRCP_TG_CATEGORY_4       0x0800

#if AVRCP_ADVANCED_TARGET == XA_ENABLED

/* Target: Player settings supported */
#define AVRCP_TG_PLAYER_SETTINGS  0x1000

/* Target: Group navigation supported */
#define AVRCP_TG_GROUP_NAV        0x2000

#if AVRCP_BROWSING_TARGET == XA_ENABLED
/* Target: Media browsing is supported */
#define AVRCP_TG_BROWSING         0x4000
#endif /* AVRCP_BROWSING_TARGET == XA_ENABLED */

/* Target: Multiple media players */
#define AVRCP_TG_MULTIPLE_PLAYERS 0x8000

#endif /* AVRCP_ADVANCED_TARGET == XA_ENABLED */

/* Primary Role as Controller.  This define can be used for a device 
 * whose primary role is as a Controller, providing Category 1 capabilities 
 * in the controller role, and Category 2 capabilities (volume control) 
 * in the target role.  Other features, such as Player Settings and 
 * Group Navigation can be added, if the application supports those features.
 */
#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
#define AVRCP_CT (AVRCP_CT_CATEGORY_1 | AVRCP_TG_CATEGORY_2 | AVRCP_CT_BROWSING)
#else
#define AVRCP_CT (AVRCP_CT_CATEGORY_1)
#endif

/* Primary Role as Target.  This define can be used for a device 
 * whose primary role is as a Target, providing Category 1 capabilities 
 * in the target role, and Category 2 capabilities (volume control) 
 * in the controller role.  Other features, such as Player Settings and 
 * Group Navigation can be added, if the application supports those features.
 */
#if AVRCP_BROWSING_TARGET == XA_ENABLED
#define AVRCP_TG (AVRCP_TG_CATEGORY_1 | AVRCP_CT_CATEGORY_2 | AVRCP_TG_BROWSING)
#else
#define AVRCP_TG (AVRCP_TG_CATEGORY_1 )
#endif
/* End of AvrcpRole */

/*---------------------------------------------------------------------------
 * AvrcpCtype type
 *
 * This type defines the AV/C ctype (command type) codes.
 */
typedef AvctpCtype AvrcpCtype;

#define AVRCP_CTYPE_CONTROL               AVCTP_CTYPE_CONTROL
#define AVRCP_CTYPE_STATUS                AVCTP_CTYPE_STATUS
#define AVRCP_CTYPE_SPECIFIC_INQUIRY      AVCTP_CTYPE_SPECIFIC_INQUIRY
#define AVRCP_CTYPE_NOTIFY                AVCTP_CTYPE_NOTIFY
#define AVRCP_CTYPE_GENERAL_INQUIRY       AVCTP_CTYPE_GENERAL_INQUIRY

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
#define AVRCP_CTYPE_BROWSING              AVCTP_CTYPE_BROWSING
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */
/* End of AvrcpCtype */ 

/*---------------------------------------------------------------------------
 * AvrcpResponse type
 *
 * This type defines the AV/C response codes.
 */
typedef AvctpResponse AvrcpResponse;

#define AVRCP_RESPONSE_NOT_IMPLEMENTED    AVCTP_RESPONSE_NOT_IMPLEMENTED   
#define AVRCP_RESPONSE_ACCEPTED           AVCTP_RESPONSE_ACCEPTED          
#define AVRCP_RESPONSE_REJECTED           AVCTP_RESPONSE_REJECTED          
#define AVRCP_RESPONSE_IN_TRANSITION      AVCTP_RESPONSE_IN_TRANSITION     
#define AVRCP_RESPONSE_IMPLEMENTED_STABLE AVCTP_RESPONSE_IMPLEMENTED_STABLE
#define AVRCP_RESPONSE_CHANGED            AVCTP_RESPONSE_CHANGED           
#define AVRCP_RESPONSE_INTERIM            AVCTP_RESPONSE_INTERIM

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
#define AVRCP_RESPONSE_BROWSING           AVCTP_RESPONSE_BROWSING
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

/* This code, when received in an AVRCP_EVENT_PANEL_CNF event, indicates
 * that a "release" command was not actually delivered to the target because
 * the original "press" command was rejected.
 *
 * This value is NOT legal for use in functions that accept AvrcpResponse
 * as a parameter.
 */
#define AVRCP_RESPONSE_SKIPPED            0xF0

/* This code, when received in an AVRCP_EVENT_PANEL_CNF event, indicates
 * that the expected response message from the target was not received
 * within the expected time frame. The application may proceed normally
 * as if the command was accepted, or take some other action.
 *
 * This value is NOT legal for use in functions that accept AvrcpResponse
 * as a parameter.
 */
#define AVRCP_RESPONSE_TIMEOUT            0xF1
/* End of AvrcpResponse */ 

/*---------------------------------------------------------------------------
 * AvrcpOpcode type
 *
 * This type defines the AV/C Opcodes.
 */
typedef AvctpOpcode AvrcpOpcode;

#define AVRCP_OPCODE_VENDOR_DEPENDENT     AVCTP_OPCODE_VENDOR_DEPENDENT
#define AVRCP_OPCODE_UNIT_INFO            AVCTP_OPCODE_UNIT_INFO       
#define AVRCP_OPCODE_SUBUNIT_INFO         AVCTP_OPCODE_SUBUNIT_INFO    
#define AVRCP_OPCODE_PASS_THROUGH         AVCTP_OPCODE_PASS_THROUGH    
/* End of AvrcpOpcode */ 

/*---------------------------------------------------------------------------
 * AvrcpCmdFrame type
 *
 * Defines the parameters required for an AV/C command.
 */
typedef AvctpCmdFrame AvrcpCmdFrame;

/* End of AvrcpCmdFrame */ 

/*---------------------------------------------------------------------------
 * AvrcpRspFrame type
 *
 * Defines the parameters required for an AV/C response.
 */
typedef AvctpRspFrame AvrcpRspFrame;

/* End of AvrcpRspFrame */ 

/*---------------------------------------------------------------------------
 * AvrcpSubunitType type
 *
 *     Defines recognized AVRCP subunit types 
 */
typedef U8 AvrcpSubunitType;

/* Panel subunit type, as specified by the 1394 AV/C Panel Subunit
 * Specification
 */
#define AVRCP_SUBUNIT_PANEL        0x09

/* Unit type, used when requesting Unit Info or Subunit Info */ 
#define AVRCP_SUBUNIT_UNIT         0x1F

/* End of AvrcpSubunitType */ 

/*---------------------------------------------------------------------------
 * AvrcpPanelOperation type
 *
 *     Panel subunit operations that may be sent (by a controller) or
 *     received (by a target). These codes are defined by the 1394
 *     AV/C Panel Subunit Specification (version 1.1).
 */
typedef U16 AvrcpPanelOperation;

#define AVRCP_POP_SELECT            0x0000
#define AVRCP_POP_UP                0x0001
#define AVRCP_POP_DOWN              0x0002
#define AVRCP_POP_LEFT              0x0003
#define AVRCP_POP_RIGHT             0x0004
#define AVRCP_POP_RIGHT_UP          0x0005
#define AVRCP_POP_RIGHT_DOWN        0x0006
#define AVRCP_POP_LEFT_UP           0x0007
#define AVRCP_POP_LEFT_DOWN         0x0008
#define AVRCP_POP_ROOT_MENU         0x0009
#define AVRCP_POP_SETUP_MENU        0x000A
#define AVRCP_POP_CONTENTS_MENU     0x000B
#define AVRCP_POP_FAVORITE_MENU     0x000C
#define AVRCP_POP_EXIT              0x000D

#define AVRCP_POP_0                 0x0020
#define AVRCP_POP_1                 0x0021
#define AVRCP_POP_2                 0x0022
#define AVRCP_POP_3                 0x0023
#define AVRCP_POP_4                 0x0024
#define AVRCP_POP_5                 0x0025
#define AVRCP_POP_6                 0x0026
#define AVRCP_POP_7                 0x0027
#define AVRCP_POP_8                 0x0028
#define AVRCP_POP_9                 0x0029
#define AVRCP_POP_DOT               0x002A
#define AVRCP_POP_ENTER             0x002B
#define AVRCP_POP_CLEAR             0x002C

#define AVRCP_POP_CHANNEL_UP        0x0030
#define AVRCP_POP_CHANNEL_DOWN      0x0031
#define AVRCP_POP_PREVIOUS_CHANNEL  0x0032
#define AVRCP_POP_SOUND_SELECT      0x0033
#define AVRCP_POP_INPUT_SELECT      0x0034
#define AVRCP_POP_DISPLAY_INFO      0x0035
#define AVRCP_POP_HELP              0x0036
#define AVRCP_POP_PAGE_UP           0x0037
#define AVRCP_POP_PAGE_DOWN         0x0038

#define AVRCP_POP_POWER             0x0040
#define AVRCP_POP_VOLUME_UP         0x0041
#define AVRCP_POP_VOLUME_DOWN       0x0042
#define AVRCP_POP_MUTE              0x0043
#define AVRCP_POP_PLAY              0x0044
#define AVRCP_POP_STOP              0x0045
#define AVRCP_POP_PAUSE             0x0046
#define AVRCP_POP_RECORD            0x0047
#define AVRCP_POP_REWIND            0x0048
#define AVRCP_POP_FAST_FORWARD      0x0049
#define AVRCP_POP_EJECT             0x004A
#define AVRCP_POP_FORWARD           0x004B
#define AVRCP_POP_BACKWARD          0x004C

#define AVRCP_POP_ANGLE             0x0050
#define AVRCP_POP_SUBPICTURE        0x0051

#define AVRCP_POP_F1                0x0071
#define AVRCP_POP_F2                0x0072
#define AVRCP_POP_F3                0x0073
#define AVRCP_POP_F4                0x0074
#define AVRCP_POP_F5                0x0075

#define AVRCP_POP_VENDOR_UNIQUE     0x007E

#define AVRCP_POP_NEXT_GROUP        0x017E
#define AVRCP_POP_PREV_GROUP        0x027E

#define AVRCP_POP_RESERVED          0x007F

/* End of AvrcpPanelOperation */

/*---------------------------------------------------------------------------
 * AvrcpOperation type
 *
 *     AVRCP Bluetooth specific operations that may be sent/received by a
 *     controller or target.  Some operations are Bluetooth specific AV/C
 *     commands sent over the control channel and routed through the panel
 *     subunit, and others are browsing commands sent over the browsing
 *     channel.
 * 
 *     Each operation is associated with an API function and/or event. The
 *     function name and the events and parameters used in the callback
 *     function are described below.
 */
typedef U8 AvrcpOperation;

/* Associated Function:  AVRCP_CtGetCapabilities()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_CAPABILITIES during the
 * AVRCP_EVENT_ADV_RESPONSE event, the 'p.adv.rsp.capability' field contains
 * the results of the operation.
 */ 
#define AVRCP_OP_GET_CAPABILITIES               0x10

/* Associated Function:  AVRCP_CtListPlayerSettingAttrs()
 * 
 * When 'advOp' is set to AVRCP_OP_LIST_PLAYER_SETTING_ATTRIBS during the
 * AVRCP_EVENT_ADV_RESPONSE event, the 'p.adv.rsp.attrMask' field contains
 * the results of the operation.
 */ 
#define AVRCP_OP_LIST_PLAYER_SETTING_ATTRIBS    0x11

/* Associated Function:  AVRCP_CtListPlayerSettingValues() 
 * 
 * When 'advOp' is set to AVRCP_OP_LIST_PLAYER_SETTING_VALUES during the 
 * AVRCP_EVENT_ADV_RESPONSE event, the 'p.adv.rsp.attrValues' field contains
 * the results of the operation.
 */ 
#define AVRCP_OP_LIST_PLAYER_SETTING_VALUES     0x12

/* Associated Function:  AVRCP_CtGetPlayerSettingValues()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_PLAYER_SETTING_VALUE during the
 * AVRCP_EVENT_ADV_RESPONSE event, the 'p.adv.rsp.setting' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_GET_PLAYER_SETTING_VALUE       0x13

/* Associated Function:  AVRCP_CtSetPlayerSettingValues()
 * 
 * When 'advOp' is set to AVRCP_OP_SET_PLAYER_SETTING_VALUE during the
 * AVRCP_EVENT_ADV_INFO event, the 'p.adv.info.setting' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_SET_PLAYER_SETTING_VALUE       0x14

/* Associated Function:  AVRCP_CtGetPlayerSettingAttrTxt()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_PLAYER_SETTING_ATTR_TEXT during the
 * AVRCP_EVENT_ADV_RESPONSE event, the 'p.adv.rsp.attrStrings' field contains
 * the results of the operation.
 */ 
#define AVRCP_OP_GET_PLAYER_SETTING_ATTR_TEXT   0x15

/* Associated Function:  AVRCP_CtGetPlayerSettingValueTxt()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_PLAYER_SETTING_VALUE_TEXT during the
 * AVRCP_EVENT_ADV_RESPONSE event, the 'p.adv.rsp.settingStrings' field
 * contains the results of the operation.
 */ 
#define AVRCP_OP_GET_PLAYER_SETTING_VALUE_TEXT  0x16

/* Associated Function:  AVRCP_CtInformCharset()
 * 
 * When 'advOp' is set to AVRCP_OP_INFORM_DISP_CHAR_SET during the 
 * AVRCP_EVENT_ADV_INFO event, the 'p.adv.info.charSet' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_INFORM_DISP_CHAR_SET           0x17

/* Associated Function:  AVRCP_CtInformBatteryStatus()
 * 
 * When 'advOp' is set to AVRCP_OP_INFORM_BATT_STATUS during the
 * AVRCP_EVENT_ADV_INFO event, the 'p.adv.info.battStatus' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_INFORM_BATT_STATUS             0x18

/* Associated Function:  AVRCP_CtGetMediaInfo()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_MEDIA_INFO during the
 * AVRCP_EVENT_ADV_RESPONSE event, the 'p.adv.rsp.element' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_GET_MEDIA_INFO                 0x20

/* Associated Function:  AVRCP_CtGetPlayStatus()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_PLAY_STATUS during the
 * AVRCP_EVENT_ADV_INFO event, the 'p.adv.info.charSet' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_GET_PLAY_STATUS                0x30

/* Associated Function:  AVRCP_CtRegisterNotification()
 * 
 * When 'advOp' is set to AVRCP_OP_REGISTER_NOTIFY during the
 * AVRCP_EVENT_ADV_RESPONSE event, or when the AVRCP_EVENT_ADV_NOTIFY event
 * is received, the 'p.adv.notify' field contains the response or parameters
 * for the operation.
 */ 
#define AVRCP_OP_REGISTER_NOTIFY                0x31

/* This operation is used internally and will not be seen at the API
 * layer.
 */ 
#define AVRCP_OP_REQUEST_CONT_RESP              0x40

/* This operation is used internally and will not be seen at the API
 * layer.
 */ 
#define AVRCP_OP_ABORT_CONT_RESP                0x41

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED

/* Associated Function:  AVRCP_CtSetAbsoluteVolume()
 * 
 * When 'advOp' is set to AVRCP_OP_SET_ABSOLUTE_VOLUME during the
 * AVRCP_EVENT_ADV_INFO event, the 'p.info.volume' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_SET_ABSOLUTE_VOLUME            0x50

/* Associated Function:  AVRCP_CtSetAddressedPlayer()
 * 
 * When 'advOp' is set to AVRCP_OP_SET_ADDRESSED_PLAYER during the
 * AVRCP_EVENT_ADV_INFO event, the 'p.info.addrPlayer' field contains the
 * results of the operation.
 */ 
#define AVRCP_OP_SET_ADDRESSED_PLAYER           0x60

#if AVRCP_BROWSING_ENABLED

/* Associated Function:  AVRCP_CtSetBrowsedPlayer()
 * 
 * When 'advOp' is set to AVRCP_OP_SET_BROWSED_PLAYER during the
 * AVRCP_EVENT_BROWSE_RESPONSE event, the 'p.adv.browseRsp.brwsPlayer' field
 * contains the results of the operation.
 * 
 * When 'advOp' is set to AVRCP_OP_SET_BROWSED_PLAYER during the
 * AVRCP_EVENT_BROWSE_INFO event, the 'p.adv.browseInfo.brwsPlayer' field
 * contains the browsing information (no response expected).
 * 
 * This operation is used only on the browsing channel.
 */ 
#define AVRCP_OP_SET_BROWSED_PLAYER             0x70

#endif /* AVRCP_BROWSING_ENABLED */

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED

/* Associated Function:  AVRCP_CtGetFolderItems()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_FOLDER_ITEMS during the
 * AVRCP_EVENT_BROWSE_RESPONSE event, the 'p.adv.browseRsp.fldrItems' field
 * contains the results of the operation.
 *
 * When 'advOp' is set to AVRCP_OP_GET_FOLDER_ITEMS during the
 * AVRCP_EVENT_BROWSE_REQ event, the 'p.adv.browseReq.folder' field contains
 * the browsing request (a response is expected).
 * 
 * This operation is used only on the browsing channel.
 */ 
#define AVRCP_OP_GET_FOLDER_ITEMS               0x71

#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

#if AVRCP_BROWSING_ENABLED

/* Associated Function:  AVRCP_CtChangePath()
 * 
 * When 'advOp' is set to AVRCP_OP_CHANGE_PATH during the
 * AVRCP_EVENT_BROWSE_RESPONSE event, the 'p.adv.browseRsp.chPath' field
 * contains the results of the operation.
 *
 * When 'advOp' is set to AVRCP_OP_CHANGE_PATH during the
 * AVRCP_EVENT_BROWSE_REQ event, the 'p.adv.browseReq.chPath' field contains
 * the browsing request (a response is expected).
 * 
 * This operation is used only on the browsing channel.
 */ 
#define AVRCP_OP_CHANGE_PATH                    0x72

/* Associated Function:  AVRCP_CtGetItemAttributes()
 * 
 * When 'advOp' is set to AVRCP_OP_GET_ITEM_ATTRIBUTES during the
 * AVRCP_EVENT_BROWSE_RESPONSE event, the 'p.adv.browseRsp.itemAttrs' field
 * contains the results of the operation.
 *
 * When 'advOp' is set to AVRCP_OP_GET_ITEM_ATTRIBUTES during the
 * AVRCP_EVENT_BROWSE_REQ event, the 'p.adv.browseReq.attr' field contains
 * the browsing request (a response is expected).
 * 
 * This operation is used only on the browsing channel.
 */ 
#define AVRCP_OP_GET_ITEM_ATTRIBUTES            0x73
#endif /* AVRCP_BROWSING_ENABLED */

/* Associated Function:  AVRCP_CtPlayItem()
 * 
 * When 'advOp' is set to AVRCP_OP_PLAY_ITEM during the AVRCP_EVENT_ADV_REQ
 * event, the 'p.adv.req.item' field contains the browsing request (a
 * response is expected).
 */ 
#define AVRCP_OP_PLAY_ITEM                      0x74

#if AVRCP_BROWSING_ENABLED
/* Associated Function:  AVRCP_CtSearch()
 * 
 * When 'advOp' is set to AVRCP_OP_SEARCH during the
 * AVRCP_EVENT_BROWSE_RESPONSE event, the 'p.adv.browseRsp.search' field
 * contains the results of the operation.
 *
 * When 'advOp' is set to AVRCP_OP_SEARCH during the
 * AVRCP_EVENT_BROWSE_REQ event, the 'p.adv.browseReq.search' field contains
 * the browsing request (a response is expected).
 * 
 * This operation is used only on the browsing channel.
 */ 
#define AVRCP_OP_SEARCH                         0x80
#endif /* AVRCP_BROWSING_ENABLED */

/* Associated Function:  AVRCP_CtAddToNowPlaying()
 * 
 * When 'advOp' is set to AVRCP_OP_ADD_TO_NOW_PLAYING during the
 * AVRCP_EVENT_ADV_REQ event, the 'p.adv.req.item' field contains the
 * browsing request (a response is expected).
 */ 
#define AVRCP_OP_ADD_TO_NOW_PLAYING             0x90

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
/* This operation is used internally and will not be seen at the API
 * layer.
 */ 
#define AVRCP_OP_GENERAL_REJECT                 0xA0
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/* End of AvrcpOperation */ 

/*---------------------------------------------------------------------------
 * AvrcpErrorCode
 *
 *     Error code for AVRCP specific operations.
 */
typedef U8 AvrcpErrorCode;

#define AVRCP_ERR_INVALID_CMD         0x00
#define AVRCP_ERR_INVALID_PARM        0x01
#define AVRCP_ERR_PARM_NOT_FOUND      0x02
#define AVRCP_ERR_INTERNAL_ERROR      0x03
#define AVRCP_ERR_NO_ERROR            0x04
#define AVRCP_ERR_UIDS_CHANGED        0x05
#define AVRCP_ERR_UNKNOWN_ERROR       0x06
#define AVRCP_ERR_INVALID_DIRECTION   0x07
#define AVRCP_ERR_NON_DIRECTORY       0x08
#define AVRCP_ERR_DOES_NOT_EXIST      0x09
#define AVRCP_ERR_INVALID_SCOPE       0x0A
#define AVRCP_ERR_OUT_OF_BOUNDS       0x0B
#define AVRCP_ERR_IS_DIRECTORY        0x0C
#define AVRCP_ERR_MEDIA_IN_USE        0x0D
#define AVRCP_ERR_NOW_PLAYING_FULL    0x0E
#define AVRCP_ERR_NO_SEARCH_SUPPORT   0x0F
#define AVRCP_ERR_SEARCH_IN_PROGRESS  0x10
#define AVRCP_ERR_INVALID_PLAYER_ID   0x11
#define AVRCP_ERR_NOT_BROWSABLE       0x12
#define AVRCP_ERR_NOT_ADDRESSED       0x13
#define AVRCP_ERR_NO_SEARCH_RESULTS   0x14
#define AVRCP_ERR_NO_AVAIL_PLAYERS    0x15
#define AVRCP_ERR_ADDR_PLAYER_CHANGED 0x16

/* End of AvrcpErrorCode */ 

/*---------------------------------------------------------------------------
 * AvrcpEventId
 *
 *     AVRCP events for the AVRCP_REGISTER_NOTIFY operation.
 */
typedef U8 AvrcpEventId;

#define AVRCP_EID_MEDIA_STATUS_CHANGED        0x01  /* Change in media status */ 
                                             
#define AVRCP_EID_TRACK_CHANGED               0x02  /* Current track changed */ 
                                             
#define AVRCP_EID_TRACK_END                   0x03  /* Reached end of track */ 
                                             
#define AVRCP_EID_TRACK_START                 0x04  /* Reached track start */ 
                                             
#define AVRCP_EID_PLAY_POS_CHANGED            0x05  /* Change in playback position.  
                                                     * Returned after the specified 
                                                     * playback notification change 
                                                     * notification interval. 
                                                     */ 
                                             
#define AVRCP_EID_BATT_STATUS_CHANGED         0x06  /* Change in battery status   */ 
#define AVRCP_EID_SYS_STATUS_CHANGED          0x07  /* Change in system status    */ 
#define AVRCP_EID_APP_SETTING_CHANGED         0x08  /* Change in player 
                                                     * application setting
                                                     */

#define AVRCP_EID_NOW_PLAYING_CONTENT_CHANGED 0x09 /* Contents of the now playing 
                                                    * list have changed 
                                                    */

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED

#define AVRCP_EID_AVAILABLE_PLAYERS_CHANGED   0x0A /* The available players have
                                                    * changed 
                                                    */

#define AVRCP_EID_ADDRESSED_PLAYER_CHANGED    0x0B /* The addressed player has 
                                                    * changed 
                                                    */

#define AVRCP_EID_UIDS_CHANGED                0x0C /* The UIDS have changed */

#define AVRCP_EID_VOLUME_CHANGED              0x0D /* The volume was changed */

#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

#define AVRCP_EID_FLAG_INTERIM                0x80 /* Used Internally */

/* End of AvrcpEventId */

/*---------------------------------------------------------------------------
 * AvrcpEventMask
 *
 * Bitmask of supported AVRCP events.  By default, only 
 * AVRCP_ENABLE_PLAY_STATUS_CHANGED and AVRCP_ENABLE_TRACK_CHANGED are 
 * enabled when a channel is registered.  The application must explicitly
 * enable any other supported events.

 */
typedef U16 AvrcpEventMask;

#define AVRCP_ENABLE_PLAY_STATUS_CHANGED      0x0001  /* Change in playback 
                                                       * status 
                                                       */ 
#define AVRCP_ENABLE_MEDIA_STATUS_CHANGED     0x0001  /* Alias */
#define AVRCP_ENABLE_TRACK_CHANGED            0x0002  /* Current track changed */ 
#define AVRCP_ENABLE_TRACK_END                0x0004  /* Reached end of track  */ 
#define AVRCP_ENABLE_TRACK_START              0x0008  /* Reached track start   */ 
#define AVRCP_ENABLE_PLAY_POS_CHANGED         0x0010  /* Change in playback 
                                                       * position 
                                                       */ 
#define AVRCP_ENABLE_BATT_STATUS_CHANGED      0x0020  /* Change in battery 
                                                       * status 
                                                       */ 
#define AVRCP_ENABLE_SYS_STATUS_CHANGED       0x0040  /* Change in system status */ 
#define AVRCP_ENABLE_APP_SETTING_CHANGED      0x0080  /* Change in player 
                                                       * application setting
                                                       */ 

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED

#define AVRCP_ENABLE_NOW_PLAYING_CHANGED      0x0100  /* Change in the now 
                                                       * playing list 
                                                       */
#define AVRCP_ENABLE_AVAIL_PLAYERS_CHANGED    0x0200  /* Available players
                                                       * changed 
                                                       */
#define AVRCP_ENABLE_ADDRESSED_PLAYER_CHANGED 0x0400  /* Addressed player changed */
#define AVRCP_ENABLE_UIDS_CHANGED             0x0800  /* UIDS changed */
#define AVRCP_ENABLE_VOLUME_CHANGED           0x1000  /* Volume Changed */

#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/* End of AvrcpEventMask */ 

/*---------------------------------------------------------------------------
 * AvrcpCapabilityId type
 *
 * Defines the capability ID for the AVRCP_OP_GET_CAPABILITIES operation.
 */
typedef U8 AvrcpCapabilityId;

#define AVRCP_CAPABILITY_COMPANY_ID        2
#define AVRCP_CAPABILITY_EVENTS_SUPPORTED  3

/* End of AvrcpCapabilityId */ 

/*---------------------------------------------------------------------------
 * AvrcpPlayerAttrId type
 *
 * Defines the attribute IDs used for specific player application settings,
 * as defined by the AVRCP spec.  Other values greater than 4 may be used for
 * application defined player settings extensions.
 */
typedef U8 AvrcpPlayerAttrId;

#define AVRCP_PLAYER_EQ_STATUS        1   /* Player equalizer status */
#define AVRCP_PLAYER_REPEAT_STATUS    2   /* Player repeat status */
#define AVRCP_PLAYER_SHUFFLE_STATUS   3   /* Player Shuffle status */
#define AVRCP_PLAYER_SCAN_STATUS      4   /* Player scan status */

/* End of AvrcpPlayerAttrId */ 

/*---------------------------------------------------------------------------
 * AvrcpPlayerAttrIdMask type
 *
 * Defines which attribute IDs are supported for player application settings.
 * By default, no attributes are enabled when a channel is registered.  The
 * application must explicitly enable supported attributes and set the
 * appropriate values.  The first 4 player attributes are defined by the
 * AVRCP specification.  If application defined extenstions to the player
 * settings are supported, the AVRCP_ENABLE_PLAYER_EXTENSION_ID_N bit must be
 * set, where N is the ID for the player attribute.
 */
typedef U16 AvrcpPlayerAttrIdMask;

#define AVRCP_ENABLE_PLAYER_EQ_STATUS       0x0001
#define AVRCP_ENABLE_PLAYER_REPEAT_STATUS   0x0002
#define AVRCP_ENABLE_PLAYER_SHUFFLE_STATUS  0x0004
#define AVRCP_ENABLE_PLAYER_SCAN_STATUS     0x0008
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_5  0x0010
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_6  0x0020
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_7  0x0040
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_8  0x0080
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_9  0x0100
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_10 0x0200
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_11 0x0400
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_12 0x0800
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_13 0x1000
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_14 0x2000
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_15 0x4000
#define AVRCP_ENABLE_PLAYER_EXTENSION_ID_16 0x8000

/* End of AvrcpPlayerAttrIdMask */ 

/*---------------------------------------------------------------------------
 * AvrcpEqValue type
 *
 * Defines values for the player equalizer status.
 */
typedef U8 AvrcpEqValue;

#define AVRCP_EQ_OFF  1
#define AVRCP_EQ_ON   2

/* End of AvrcpEqValue */ 

/*---------------------------------------------------------------------------
 * AvrcpRepeatValue type
 *
 * Defines values for the player repeat mode status.
 */
typedef U8 AvrcpRepeatValue;

#define AVRCP_REPEAT_OFF     1
#define AVRCP_REPEAT_SINGLE  2
#define AVRCP_REPEAT_ALL     3
#define AVRCP_REPEAT_GROUP   4

/* End of AvrcpRepeatValue */ 

/*---------------------------------------------------------------------------
 * AvrcpShuffleValue type
 *
 * Defines values for the player shuffle mode status.
 */
typedef U8 AvrcpShuffleValue;

#define AVRCP_SHUFFLE_OFF    1
#define AVRCP_SHUFFLE_ALL    2
#define AVRCP_SHUFFLE_GROUP  3

/* End of AvrcpShuffleValue */ 

/*---------------------------------------------------------------------------
 * AvrcpScanValue type
 *
 * Defines values for the player scan mode status.
 */
typedef U8 AvrcpScanValue;

#define AVRCP_SCAN_OFF    1
#define AVRCP_SCAN_ALL    2
#define AVRCP_SCAN_GROUP  3

/* End of AvrcpScanValue */ 

/*---------------------------------------------------------------------------
 * AvrcpExtValue type
 *
 * Defines values for the player setting extensions.
 */
typedef U8 AvrcpExtValue;

/* End of AvrcpExtValue */

/*---------------------------------------------------------------------------
 * AvrcpBatteryStatus type
 *
 * Defines values for battery status.
 */
typedef U8 AvrcpBatteryStatus;

#define AVRCP_BATT_STATUS_NORMAL      0
#define AVRCP_BATT_STATUS_WARNING     1
#define AVRCP_BATT_STATUS_CRITICAL    2
#define AVRCP_BATT_STATUS_EXTERNAL    3
#define AVRCP_BATT_STATUS_FULL_CHARGE 4

/* End of AvrcpBatteryStatus */ 

/*---------------------------------------------------------------------------
 * AvrcpSystemStatus type
 *
 * Defines values for system status.
 */
typedef U8 AvrcpSystemStatus;

#define AVRCP_SYS_POWER_ON   0
#define AVRCP_SYS_POWER_OFF  1
#define AVRCP_SYS_UNPLUGGED  2

/* End of AvrcpSystemStatus */ 

/*---------------------------------------------------------------------------
 * AvrcpMediaAttrId type
 *
 * Defines values for the media attribute ID.
 */
typedef U32 AvrcpMediaAttrId;

#define AVRCP_MEDIA_ATTR_TITLE       0x00000001
#define AVRCP_MEDIA_ATTR_ARTIST      0x00000002
#define AVRCP_MEDIA_ATTR_ALBUM       0x00000003
#define AVRCP_MEDIA_ATTR_TRACK       0x00000004
#define AVRCP_MEDIA_ATTR_NUM_TRACKS  0x00000005
#define AVRCP_MEDIA_ATTR_GENRE       0x00000006
#define AVRCP_MEDIA_ATTR_DURATION    0x00000007

/* End of AvrcpMediaAttrId */ 

/*---------------------------------------------------------------------------
 * AvrcpMediaAttrIdMask type
 *
 * Defines supported values for the media attribute ID.  By default, only
 * AVRCP_ENABLE_MEDIA_ATTR_TITLE is enabled when a channel is registered.
 * The application must explicitly enable any supported attributes and
 * set the appropriate values.
 */
typedef U8 AvrcpMediaAttrIdMask;

#define AVRCP_ENABLE_MEDIA_ATTR_TITLE       0x01
#define AVRCP_ENABLE_MEDIA_ATTR_ARTIST      0x02
#define AVRCP_ENABLE_MEDIA_ATTR_ALBUM       0x04
#define AVRCP_ENABLE_MEDIA_ATTR_TRACK       0x08
#define AVRCP_ENABLE_MEDIA_ATTR_NUM_TRACKS  0x10
#define AVRCP_ENABLE_MEDIA_ATTR_GENRE       0x20
#define AVRCP_ENABLE_MEDIA_ATTR_DURATION    0x40

/* End of AvrcpMediaAttrIdMask */ 

/*---------------------------------------------------------------------------
 * AvrcpMediaStatus type
 *
 * Defines play status of the currently playing media.
 */
typedef U8 AvrcpMediaStatus;

#define AVRCP_MEDIA_STOPPED       0x00
#define AVRCP_MEDIA_PLAYING       0x01
#define AVRCP_MEDIA_PAUSED        0x02
#define AVRCP_MEDIA_FWD_SEEK      0x03
#define AVRCP_MEDIA_REV_SEEK      0x04
#define AVRCP_MEDIA_ERROR         0xFF

/* End of AvrcpMediaStatus */ 

/*--------------------------------------------------------------------------
 * AvrcpVersion type
 *
 *     AVRCP profile version discovered during the SDP query.  
 */
typedef U16 AvrcpVersion;

/* Unable to determine the Hands Free Profile version that is supported */
#define AVRCP_VERSION_UNKNOWN 0x0000

/* Supports Version 1.0 of the AVRCP Profile */
#define AVRCP_VERSION_1_0     0x0100

/* Supports Version 1.3 of the AVRCP Profile */
#define AVRCP_VERSION_1_3     0x0103

/* Supports Version 1.4 of the AVRCP Profile */
#define AVRCP_VERSION_1_4     0x0104

/* End of AvrcpVersion */

/*---------------------------------------------------------------------------
 * AvrcpFeatures type
 * 
 * Device features used in SDP queries.
 */
typedef U16 AvrcpFeatures;

/* Player/recorder features supported ("play" and "stop") */ 
#define AVRCP_FEATURES_CATEGORY_1       0x0001

/* Monitor/amplifier features supported ("volume up" and "volume down") */ 
#define AVRCP_FEATURES_CATEGORY_2       0x0002

/* Tuner features supported ("channel up" and "channel down") */ 
#define AVRCP_FEATURES_CATEGORY_3       0x0004

/* Menu features supported ("root menu", "up", "down", "left", "right", and
 * "select")
 */
#define AVRCP_FEATURES_CATEGORY_4       0x0008

/* Player settings supported */
#define AVRCP_FEATURES_PLAYER_SETTINGS  0x0010

/* Group navigation supported */
#define AVRCP_FEATURES_GROUP_NAV        0x0020

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
/* Media browsing is supported */
#define AVRCP_FEATURES_BROWSING         0x0040
#endif /* AVRCP_BROWSING_ENABLED */

/* Multiple media players */
#define AVRCP_FEATURES_MULTIPLE_PLAYERS 0x0080

#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/* End of AvrcpFeatures */

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED

/*---------------------------------------------------------------------------
 * AvrcpMajPlayerType type
 * 
 */ 
typedef U8 AvrcpMajPlayerType;

#define AVRCP_MAJ_PLAYER_AUDIO      0x01
#define AVRCP_MAJ_PLAYER_VIDEO      0x02
#define AVRCP_MAJ_PLAYER_AUD_BCAST  0x04
#define AVRCP_MAJ_PLAYER_VID_BCAST  0x08

/* End of AvrcpMajPlayerType */

/*---------------------------------------------------------------------------
 * AvrcpPlayerSubType type
 * 
 */
typedef U32 AvrcpPlayerSubType;

#define AVRCP_PLAYER_SUB_AUDIO_BOOK  0x00000001
#define AVRCP_PLAYER_SUB_PODCAST     0x00000002

/* End of AvrcpPlayerSubType */

/*---------------------------------------------------------------------------
 * AvrcpScope type
 * 
 */
typedef U8 AvrcpScope;

#define AVRCP_SCOPE_MEDIA_PLAYER_LIST  0x00
#define AVRCP_SCOPE_VIRTUAL_FILESYS    0x01
#define AVRCP_SCOPE_SEARCH             0x02
#define AVRCP_SCOPE_NOW_PLAYING        0x03

/* End of AvrcpScope */

/*---------------------------------------------------------------------------
 * AvrcpChgPathDirection type
 * 
 */
typedef U8 AvrcpChgPathDirection;

#define AVRCP_DIR_UP                   0x00
#define AVRCP_DIR_DOWN                 0x01

/* End of AvrcpChgPathDirection */

/*---------------------------------------------------------------------------
 * AvrcpPlyrFeatrByte type
 * 
 */
typedef U8 AvrcpPlyrFeatrByte;

/* Byte 0 */

#define AVRCP_BRWS_0_FEAT_SELECT              0x01
#define AVRCP_BRWS_0_FEAT_UP                  0x02
#define AVRCP_BRWS_0_FEAT_DOWN                0x04
#define AVRCP_BRWS_0_FEAT_LEFT                0x08
#define AVRCP_BRWS_0_FEAT_RIGHT               0x10
#define AVRCP_BRWS_0_FEAT_RIGHT_UP            0x20
#define AVRCP_BRWS_0_FEAT_RIGHT_DOWN          0x40
#define AVRCP_BRWS_0_FEAT_LEFT_UP             0x80

/* Byte 1 */

#define AVRCP_BRWS_1_FEAT_LEFT_DOWN           0x01
#define AVRCP_BRWS_1_FEAT_ROOT_MENU           0x02
#define AVRCP_BRWS_1_FEAT_SETUP_MENU          0x04
#define AVRCP_BRWS_1_FEAT_CONTENTS_MENU       0x08
#define AVRCP_BRWS_1_FEAT_FAVORITE_MENU       0x10
#define AVRCP_BRWS_1_FEAT_EXIT                0x20
#define AVRCP_BRWS_1_FEAT_0                   0x40
#define AVRCP_BRWS_1_FEAT_1                   0x80

/* Byte 2 */

#define AVRCP_BRWS_2_FEAT_2                   0x01
#define AVRCP_BRWS_2_FEAT_3                   0x02
#define AVRCP_BRWS_2_FEAT_4                   0x04
#define AVRCP_BRWS_2_FEAT_5                   0x08
#define AVRCP_BRWS_2_FEAT_6                   0x10
#define AVRCP_BRWS_2_FEAT_7                   0x20
#define AVRCP_BRWS_2_FEAT_8                   0x40
#define AVRCP_BRWS_2_FEAT_9                   0x80

/* Byte 3 */

#define AVRCP_BRWS_3_FEAT_DOT                 0x01
#define AVRCP_BRWS_3_FEAT_ENTER               0x02
#define AVRCP_BRWS_3_FEAT_CLEAR               0x04
#define AVRCP_BRWS_3_FEAT_CHNL_UP             0x08
#define AVRCP_BRWS_3_FEAT_CHNL_DOWN           0x10
#define AVRCP_BRWS_3_FEAT_PREV_CHNL           0x20
#define AVRCP_BRWS_3_FEAT_SOUND_SEL           0x40
#define AVRCP_BRWS_3_FEAT_INPUT_SEL           0x80

/* Byte 4 */

#define AVRCP_BRWS_3_FEAT_DISPLAY_INFO        0x01
#define AVRCP_BRWS_4_FEAT_HELP                0x02
#define AVRCP_BRWS_4_FEAT_PG_UP               0x04
#define AVRCP_BRWS_4_FEAT_PG_DOWN             0x08
#define AVRCP_BRWS_4_FEAT_POWER               0x10
#define AVRCP_BRWS_4_FEAT_VOL_UP              0x20
#define AVRCP_BRWS_4_FEAT_VOL_DOWN            0x40
#define AVRCP_BRWS_4_FEAT_MUTE                0x80

/* Byte 5 */

#define AVRCP_BRWS_5_FEAT_PLAY                0x01
#define AVRCP_BRWS_5_FEAT_STOP                0x02
#define AVRCP_BRWS_5_FEAT_PAUSE               0x04
#define AVRCP_BRWS_5_FEAT_RECORD              0x08
#define AVRCP_BRWS_5_FEAT_REWIND              0x10
#define AVRCP_BRWS_5_FEAT_FF                  0x20
#define AVRCP_BRWS_5_FEAT_EJECT               0x40
#define AVRCP_BRWS_5_FEAT_FORWARD             0x80

/* Byte 6 */

#define AVRCP_BRWS_6_FEAT_BACKWARD            0x01
#define AVRCP_BRWS_6_FEAT_ANGLE               0x02
#define AVRCP_BRWS_6_FEAT_SUBPIC              0x04
#define AVRCP_BRWS_6_FEAT_F1                  0x08
#define AVRCP_BRWS_6_FEAT_F2                  0x10
#define AVRCP_BRWS_6_FEAT_F3                  0x20
#define AVRCP_BRWS_6_FEAT_F4                  0x40
#define AVRCP_BRWS_6_FEAT_F5                  0x80

/* Byte 7 */

#define AVRCP_BRWS_7_FEAT_VENDOR_UNIQUE       0x01
#define AVRCP_BRWS_7_FEAT_GROUP_NAV           0x02
#define AVRCP_BRWS_7_FEAT_ADVANCED_CTRL       0x04
#define AVRCP_BRWS_7_FEAT_BROWSING            0x08
#define AVRCP_BRWS_7_FEAT_SEARCHING           0x10
#define AVRCP_BRWS_7_FEAT_ADD_TO_PLAYING      0x20
#define AVRCP_BRWS_7_FEAT_UIDS_UNIQUE         0x40
#define AVRCP_BRWS_7_FEAT_BRWS_WHEN_ADDRESSED 0x80

/* Byte 8 */

#define AVRCP_BRWS_8_SEARCH_WHEN_ADDRESSED    0x01
#define AVRCP_BRWS_8_NOW_PLAYING              0x02
#define AVRCP_BRWS_8_UID_PERSISTENT           0x04

/* End of AvrcpPlyrFeatrByte */

/*---------------------------------------------------------------------------
 * AvrcpItemType type
 * 
 * The type of an media item.
 */
typedef U8 AvrcpItemType;

#define AVRCP_ITEM_MEDIA_PLAYER  0x01
#define AVRCP_ITEM_FOLDER        0x02
#define AVRCP_ITEM_MEDIA_ELEMENT 0x03

/* End of AvrcpItemType */

/*---------------------------------------------------------------------------
 * AvrcpFolderType type
 * 
 * The type of a media folder.
 */
typedef U8 AvrcpFolderType;

#define AVRCP_FOLDER_TYPE_MIXED     0x00
#define AVRCP_FOLDER_TYPE_TITLES    0x01
#define AVRCP_FOLDER_TYPE_ALBUMS    0x02
#define AVRCP_FOLDER_TYPE_ARTISTS   0x03
#define AVRCP_FOLDER_TYPE_GENRES    0x04
#define AVRCP_FOLDER_TYPE_PLAYLISTS 0x05
#define AVRCP_FOLDER_TYPE_YEARS     0x06

/* End of AvrcpFolderType */

/*---------------------------------------------------------------------------
 * AvrcpIsPlayable type
 * 
 * Flag that describes whether a folder is playable or not.
 */
typedef U8 AvrcpIsPlayable;

#define AVRCP_IS_NOT_PLAYABLE  0x00
#define AVRCP_IS_PLAYABLE      0x01

/* End of AvrcpIsPlayable */

/*---------------------------------------------------------------------------
 * AvrcpMediaType type
 * 
 * The type of media element.
 */
typedef U8 AvrcpMediaType;

#define AVRCP_MEDIA_TYPE_AUDIO 0x00
#define AVRCP_MEIDA_TYPE_VIDEO 0x01

/* End of AvrcpMediaType */

#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/* Forward reference */ 
typedef struct _AvrcpChannel AvrcpChannel;
typedef struct _AvrcpCallbackParms AvrcpCallbackParms;
typedef struct _AvrcpAdvancedPdu AvrcpAdvancedPdu;
typedef struct _AvrcpPlayerSetting AvrcpPlayerSetting;
typedef struct _AvrcpPlayerStrings AvrcpPlayerStrings;
typedef struct _AvrcpEqString AvrcpEqString;
typedef struct _AvrcpRepeatString AvrcpRepeatString;
typedef struct _AvrcpShuffleString AvrcpShuffleString;
typedef struct _AvrcpScanString AvrcpScanString;
typedef struct _AvrcpExtString AvrcpExtString;
typedef struct _AvrcpCharSets AvrcpCharSets;
typedef struct _AvrcpMediaInfo AvrcpMediaInfo;
typedef struct _AvrcpMediaPlayStatus AvrcpMediaPlayStatus;
typedef struct _AvrcpTrackStruct AvrcpTrackStruct;

/*---------------------------------------------------------------------------
 * AvrcpCallback type
 *
 * A function of this type is called to indicate events to the application.
 */
typedef void (*AvrcpCallback)(AvrcpChannel *chnl, 
                              const AvrcpCallbackParms *Parms);

/* End of AvrcpCallback */ 

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AvrcpEqString Structure
 *
 * Contains EQ Status strings.
 */
struct _AvrcpEqString {

    /* Index 0 == Off, Index 1 == On */ 
    const char    *string[2];

    U8             len[2];
};

/*---------------------------------------------------------------------------
 * AvrcpRepeatString Structure
 *
 * Contains Repeat status strings.
 */
struct _AvrcpRepeatString {

    /* Index 0 == Off, Index 1 == Single, Index 2 == All, Index 3 == Group */ 
    const char       *string[4];

    U8                len[4];
};

/*---------------------------------------------------------------------------
 * AvrcpShuffleString Structure
 *
 * Contains Shuffle status strings.
 */
struct _AvrcpShuffleString {

    /* Index 0 == Off, Index 1 == All, Index 2 == Group */ 
    const char        *string[3];

    U8                 len[3];
};

/*---------------------------------------------------------------------------
 * AvrcpScanString Structure
 *
 * Contains Scan status strings.
 */
struct _AvrcpScanString {

    /* Index 0 == Off, Index 1 == All, Index 2 == Group */ 
    const char     *string[3];

    U8              len[3];
};

/*---------------------------------------------------------------------------
 * AvrcpExtString Structure
 *
 * Contains strings for extended player settings defined by the application.
 */
struct _AvrcpExtString {

    /* Number of extended player settings */
    U8 numStrings;

    const char *string[AVRCP_MAX_PLAYER_STRINGS];  /* The player setting string */
    U8          len[AVRCP_MAX_PLAYER_STRINGS];     /* The player setting string length */
};

/*---------------------------------------------------------------------------
 * AvrcpPlayerSetting Structure
 *
 * Contains the Media Player device settings.
 */
struct _AvrcpPlayerSetting {
    AvrcpPlayerAttrId      attrId;     /* Media Player Attribute ID */ 

    union {
        AvrcpEqValue       eq;
        AvrcpRepeatValue   repeat;
        AvrcpShuffleValue  shuffle;
        AvrcpScanValue     scan;
        AvrcpExtValue      extValue;

        U8                 value;     /* Used Internally */ 
    } setting;
};

/*---------------------------------------------------------------------------
 * AvrcpPlayerStrings Structure
 *
 * Contains the Media Player attribute and value strings.
 */
struct _AvrcpPlayerStrings {

    const char         *attrString;  /* Media Player Attribute Text */ 
    U8                  attrLen;     /* Length of the Attribute Text */ 

    union {
        AvrcpEqString       eq;      /* Equalizer status */ 
        AvrcpRepeatString   repeat;  /* Repeat mode status */ 
        AvrcpShuffleString  shuffle; /* Shuffle mode status */ 
        AvrcpScanString     scan;    /* Scan mode status */ 
        AvrcpExtString      ext;     /* Extended settings status */
    } setting;
};

/*---------------------------------------------------------------------------
 * AvrcpMediaInfo Structure
 *
 * Contains the Media element information.
 */
struct _AvrcpMediaInfo {
    AvrcpMediaAttrId      attrId;      /* Media element attribute ID */ 
    const char           *string;      /* The media element */ 
    U16                   length;      /* Length of the media element */ 
};

/*---------------------------------------------------------------------------
 * AvrcpCharSets Structure
 *
 * Contains the supported character sets.
 */
struct _AvrcpCharSets {
    U8   numCharSets;
    U16  charSets[AVRCP_MAX_CHAR_SETS];
};

/*---------------------------------------------------------------------------
 * AvrcpMediaPlayStatus structure
 *
 * Contains status of the currently playing media.
 */
struct _AvrcpMediaPlayStatus {
    U32               length;

    /* If a track is currently selected, then this contains the current 
     * playback position within the current track, measured in milliseconds.  
     * Otherwise, it will contain AVRCP_NO_TRACK_CURRENTLY_SELECTED.
     */ 
    U32               position;

    AvrcpMediaStatus  mediaStatus;
};

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
/*---------------------------------------------------------------------------
 * AvrcpFolder
 * 
 * Contains information about a media player folder.
 */
typedef struct _AvrcpFolder {

    U16         nameLen;
    const char *name;

} AvrcpFolder;

/*---------------------------------------------------------------------------
 * AvrcpMediaPlayerItem
 * 
 * Contains information about a media player item.
 */
typedef struct _AvrcpMediaPlayerItem {

    /* Used Internally */
    ListEntry          node;

    /* Used Internally */
    U16                playerId;

    /* Major Player Type */
    AvrcpMajPlayerType majorType;

    /* Player Sub Type */
    AvrcpPlayerSubType subType;

    /* Current media play status */
    AvrcpMediaStatus   mediaStatus;

    /* Player feature bitmask */
    AvrcpPlyrFeatrByte features[16];

    /* Displayable character set */
    U16                charSet;

    /* Player name length */
    U16                nameLen;

    /* Player name */
    const char        *name;

    /* Folder depth */
    U16                folderDepth;

    /* Current folder */
    AvrcpFolder        folder[AVRCP_MAX_FOLDER_DEPTH];

    /* Number of items in the current folder */
    U32                numItems;

} AvrcpMediaPlayerItem;

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
/*---------------------------------------------------------------------------
 * AvrcpFolderItem
 * 
 * Contains information about a folder item.
 */
typedef struct _AvrcpFolderItem {

    /* The UID of this media element */
    U8               uid[8];

    /* Folder Type */
    AvrcpFolderType  folderType;

    /* Is Playable Flag */
    AvrcpIsPlayable  isPlayable;

    /* Character Set */
    U16              charSet;

    /* The name length of this media element */
    U16              nameLen;

    /* The name of this media element */
    U8              *name;

} AvrcpFolderItem;

/*---------------------------------------------------------------------------
 * AvrcpMediaAttributes
 * 
 * Media Attributes
 */ 
typedef struct _AvrcpMediaAttribute {

    /* Attribute ID */
    AvrcpMediaAttrId  attrId;

    /* Character Set */
    U16               charSet;

    /* Attribute Length */
    U16               valueLen;

    /* Attribute Name */
    U8               *value;

} AvrcpMediaAttribute;

/*---------------------------------------------------------------------------
 * AvrcpMediaElementItem
 * 
 * Media Elements 
 */ 
typedef struct _AvrcpMediaElementItem {

    /* The UID of this media element */
    U8                  uid[8];

    /* Media type (audio or video) */
    AvrcpMediaType      mediaType;

    /* Character Set */
    U16                 charSet;

    /* The name length of this media element */
    U16                 nameLen;

    /* The name of this media element */
    U8                 *name;

    /* Number of media attributes */
    U8                  numAttrs;

    /* Media Attributes */
    AvrcpMediaAttribute attrs[AVRCP_NUM_MEDIA_ATTRIBUTES];

} AvrcpMediaElementItem;

/*---------------------------------------------------------------------------
 * AvrcpMediaItem
 * 
 * A media item. 
 */ 
typedef struct _AvrcpMediaItem {

    /* Media Item Type */
    AvrcpItemType itemType;

    /* The Media Item */
    union {
        AvrcpMediaPlayerItem   player;
        AvrcpFolderItem        folder;
        AvrcpMediaElementItem  element;
    } item;

} AvrcpMediaItem;
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/*--------------------------------------------------------------------------
 * AvrcpAdvancedPdu structure
 *
 * Contains the data for a advanced PDU.
 */
struct _AvrcpAdvancedPdu {

    /* Used Internally by AVRCP */
    ListEntry node;

    /* Operation (PDU ID) */ 
    AvrcpOperation  op;

    /* Operation parameter length */ 
    U16 parmLen;

    /* Operation parameters */ 
    U8 *parms;

    /* Transaction ID */
    U8  transId;

    /* === Internal use only === */ 

    BOOL more;
    U16  curLen;
    U16  bytesToSend;
    U8   contOp;
    BOOL abort;

    /* For responses */
    BOOL           internal;
    AvrcpResponse  response;
    AvrcpErrorCode error;

    BOOL isCmd;
    AvctpCtype  ctype;          /* 4 bits */
};

/*---------------------------------------------------------------------------
 * AvrcpTrackStruct structure
 *
 * Defines a 64 bit structure used to store track index information.  As of
 * AVRCP spec v1.4, the only valid values are 0 (track selected) and 0
 * AVRCP_NO_TRACK_CURRENTLY_SELECTED.  Any other value should be treated the
 * same as 0.
 */
struct _AvrcpTrackStruct {

    /* The most significant 32 bits of the track index information.  */ 
    U32 msU32;

    /* The least significant 32 bits of the track index information.  */ 
    U32 lsU32;

};

#if AVRCP_ADVANCED_TARGET == XA_ENABLED
#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
/*---------------------------------------------------------------------------
 * AvrcpAdvReqParms req;
 * 
 * Defines the call back parameters received during an AVRCP_EVENT_ADV_REQ
 * event.
 * 
 */
typedef struct _AvrcpAdvReqParms {

    /* Transaction ID of the request */
    U8 transId;

    union {
        /* Contains an item to be played on the addressed media player.  This
         * is valid when "advOp" is set to AVRCP_OP_PLAY_ITEM or
         * AVRCP_OP_ADD_TO_NOW_PLAYING.
         */ 
        struct {
            AvrcpScope  scope;
            U8          uid[8];
            U16         uidCounter;
        } item;
    } p;

} AvrcpAdvReqParms;
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/*---------------------------------------------------------------------------
 * AvrcpAdvInfParms structure
 * 
 * Defines the callback parameters received during an AVRCP_EVENT_ADV_INFO
 * event.
 */
typedef union _AvrcpAdvInfParms {

    /* The value of each player setting.  This is valid when "advOp" is set
     * to AVRCP_OP_SET_PLAYER_SETTING_VALUE.
     */ 
    AvrcpPlayerSetting playerSetting;
    struct {

        /* Bitmask that describes which 
         * attributes are being reported 
         */
        AvrcpPlayerAttrIdMask  attrMask;

        /* The equalizer setting. */ 
        AvrcpEqValue           eq;

        /* The repeat setting. */ 
        AvrcpRepeatValue       repeat;

        /* The shuffle setting. */ 
        AvrcpShuffleValue      shuffle;

        /* The scan setting. */ 
        AvrcpScanValue         scan;

#if AVRCP_NUM_PLAYER_SETTINGS > 4
        /* Extended player setting. */
        AvrcpExtValue          extValue[AVRCP_NUM_PLAYER_SETTINGS-4];
#endif

    } setting;

    /* The list of character sets displayable on the controller.  
     * This is valid when "advOp" is set to AVRCP_OP_INFORM_DISP_CHAR_SET. The
     * default is 106 (UTF-8).
     */ 
    AvrcpCharSets       charSet;

    /* The battery status of the controller.  
     * This is valid when "advOp" is set to AVRCP_OP_INFORM_BATT_STATUS.
     */ 
    AvrcpBatteryStatus  battStatus;

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
    /* The Absolute Volume 
     * This is valid when "advOp" is set to AVRCP_OP_SET_ABSOLUTE_VOLUME.
     */
    U8                  volume;

    /* The addressed player
     * This is valid when "advOp" is set to AVRCP_OP_SET_ADDRESSED_PLAYER.
     */ 
    AvrcpMediaPlayerItem  *addrPlayer;
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

} AvrcpAdvInfParms;
#endif /* AVRCP_ADVANCED_TARGET == XA_ENABLED */

#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED
/*---------------------------------------------------------------------------
 * AvrcpPlayerAttrib structure
 * 
 * Used to describe player setting attributes.
 */
typedef struct _AvrcpPlayerAttrib {
    AvrcpPlayerAttrId  attrId;
    U16                charSet;
    U8                 length;
    const char        *string;
} AvrcpPlayerAttrib;


/*---------------------------------------------------------------------------
 * AvrcpPlayerSettings structure
 * 
 * Used to describe player setting values.
 */
typedef struct _AvrcpPlayerSettings {
    U8          valueId;
    U16         charSet;
    U8          length;
    const char *string;
} AvrcpPlayerSettings;

/*---------------------------------------------------------------------------
 * AvrcpMediaAttr structure
 * 
 * Used to describe media info attributes.
 */
typedef struct _AvrcpMediaAttr {
    U32         attrId;
    U16         charSet;
    U16         length;
    const char *string;
} AvrcpMediaAttr;


/*---------------------------------------------------------------------------
 * AvrcpRspParms structure
 * 
 * Defines the callback parameters received during the
 * AVRCP_EVENT_ADV_RESPONSE event, not including AVRCP_OP_REGISTER_NOTIFY
 * operation (see AvrcpNotifyParms).
 */
typedef union _AvrcpAdvRspParms {
    /* The capabilities of the target.  
     * This is valid when "advOp" is set to AVRCP_OP_GET_CAPABILITIES.
     */ 
    struct {

        /* The type of capability. */ 
        AvrcpCapabilityId type;

        /* The capability info. */ 
        union {

            /* The list of company IDs.  
             * (type == AVRCP_CAPABILITY_COMPANY_ID) 
             */ 
            struct {

                /* The number of supported company IDs. */ 
                U8  numIds;

                /* An array of company IDs (3 bytes each). */ 
                U8 *ids;

            } companyId;

            /* A bitmask of the supported events. 
             * (type == AVRCP_CAPABILITY_EVENTS_SUPPORTED)
             */ 
            AvrcpEventMask  eventMask;

        } info;

    } capability;

    /* A bitmask that indicates the player settings supported by 
     * the target device.
     * This is valid when "advOp" is set to
     * AVRCP_OP_LIST_PLAYER_SETTING_ATTRIBS.
     */ 
    AvrcpPlayerAttrIdMask  attrMask;

    /* The list of values for the requested attribute on the target 
     * device.  
     * This is valid when "advOp" is set to AVRCP_OP_LIST_PLAYER_SETTING_VALUES.
     */ 
    struct {

        /* The number of supported value IDs. */ 
        U8  numIds;

        /* An array of value ID (1 byte each). */ 
        U8 *ids;

    } attrValues;

    /* The text of each player setting attribute.  
     * This is valid when "advOp" is set to
     * AVRCP_OP_GET_PLAYER_SETTING_ATTR_TEXT
     */ 
    struct {

        /* The number of attribute IDs returned. */ 
        U8 numIds;

        /* An array of attribute text information */ 
        AvrcpPlayerAttrib txt[AVRCP_NUM_PLAYER_SETTINGS];

    } attrStrings;

    /* The value of each player setting.  
     * This is valid when "advOp" is set to AVRCP_OP_GET_PLAYER_SETTING_VALUE.
     */ 
    struct {

        /* Bitmask that describes which 
         * attributes are being reported 
         */
        AvrcpPlayerAttrIdMask  attrMask;

        /* The equalizer setting. */ 
        AvrcpEqValue           eq;

        /* The repeat setting. */ 
        AvrcpRepeatValue       repeat;

        /* The shuffle setting. */ 
        AvrcpShuffleValue      shuffle;

        /* The scan setting. */ 
        AvrcpScanValue         scan;

#if AVRCP_NUM_PLAYER_SETTINGS > 4
        /* Extended player setting. */
        AvrcpExtValue          extValue[AVRCP_NUM_PLAYER_SETTINGS-4];
#endif

    } setting;

    /* The list of setting values for the requested attribute on the target.
     * This is valid when "advOp" is set to
     * AVRCP_OP_GET_PLAYER_SETTING_VALUE_TEXT.
     */ 
    struct {

        /* The number of settings returned */ 
        U8  numSettings;

        /* An array of value text information */ 
        AvrcpPlayerSettings txt[AVRCP_NUM_PLAYER_SETTINGS];

    } settingStrings;

    /* The list of element values for the current track on the 
     * target.  This is valid when "advOp" is set to AVRCP_OP_GET_MEDIA_INFO.
     */ 
    struct {

        /* The number of elements returned */ 
        U8 numIds;

        /* An array of element value text information */ 
        AvrcpMediaAttr txt[AVRCP_NUM_MEDIA_ATTRIBUTES];

    } element;

    /* The playback status of the current track.
     * This is valid when "advOp" is set to AVRCP_OP_GET_PLAY_STATUS.
     */ 
    struct {
        U32               length;
        U32               position;
        AvrcpMediaStatus  mediaStatus;
    } playStatus;

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
    /* The Absolute Volume 
     * This is valid when "advOp" is set to AVRCP_OP_SET_ABSOLUTE_VOLUME.
     */
    U8 volume;
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

} AvrcpAdvRspParms; 

/*---------------------------------------------------------------------------
 * AvrcpNotifyParms structure
 *
 * Defines the callback parameters for AVRCP_EVENT_ADV_RESPONSE when "advOp" is
 * set to AVRCP_OP_REGISTER_NOTIFY, and for the AVRCP_EVENT_ADV_NOTIFY event.
 */
typedef struct _AvrcpAdvNotifyParms {

    /* Defines the event ID that was received */
    AvrcpEventId   event;

    union {

        /* Play status of the media.  Valid when the event ID is
         * AVRCP_EID_MEDIA_STATUS_CHANGED.
         */ 
        AvrcpMediaStatus    mediaStatus;

        /* The current track.  Valid when the event ID is
         * AVRCP_EID_TRACK_CHANGED.
         */ 
        AvrcpTrackStruct    track;

        /* The position (ms) of the current track.  Valid when the event
         * ID is AVRCP_EID_PLAY_POS_CHANGED.
         */ 
        U32                 position;

        /* The battery status of the target.  Valid when the event ID is
         * AVRCP_EID_BATT_STATUS_CHANGED.
         */ 
        AvrcpBatteryStatus  battStatus;

        /* The system status of the target.  Valid when the event ID is
         * AVRCP_EID_SYS_STATUS_CHANGED.
         */ 
        AvrcpSystemStatus   sysStatus;

        /* The player settings.  Valid when the event ID is
         * AVRCP_EID_APP_SETTING_CHANGED
         */ 
        struct {

            /* Bitmask that describes which 
             * attributes are being reported 
             */ 
            AvrcpPlayerAttrIdMask  attrMask;

            /* The equalizer setting. */ 
            AvrcpEqValue           eq;

            /* The repeat setting. */ 
            AvrcpRepeatValue       repeat;

            /* The shuffle setting. */ 
            AvrcpShuffleValue      shuffle;

            /* The scan setting. */ 
            AvrcpScanValue         scan;

#if AVRCP_NUM_PLAYER_SETTINGS > 4
            /* Extended player setting. */
            AvrcpExtValue          extValue[AVRCP_NUM_PLAYER_SETTINGS-4];
#endif

        } setting;

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
        /* Absolute volume.  Valid when the event ID is
         * AVRCP_EID_VOLUME_CHANGED.
         */
        U8 volume;

        /* The addressed player.  Valid when the event ID is
         * AVRCP_EID_ADDRESSED_PLAYER_CHANGED.
         */ 
        struct {
            U16  playerId;
            U16  uidCounter;
        } addrPlayer;

        /* The UID counter.  Valid when the event ID is
         * AVRCP_EID_UIDS_CHANGED.
         */ 
        U16 uidCounter;
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

    } p;

} AvrcpAdvNotifyParms;
#endif

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
/*---------------------------------------------------------------------------
 * AvrcpBrowseReqParms structure
 *
 * Defines the callback parameters for the AVRCP_EVENT_BROWSE_REQ event.
 */
typedef struct _AvrcpBrowseReqParms {

    U8 transId;

    union {
        /* Folder Item.  Valid when "advOp" is AVRCP_OP_GET_FOLDER_ITEMS */
        struct {
            AvrcpScope scope;        /* The requested scope */
            U32        start;        /* The starting item */
            U32        end;          /* The ending item */
            U8         numAttrs;     /* Number of attributes requested */
            U8        *attrList;     /* List of attributes requested */
        } folder;

        /* Path.  Valid when "advOp" is AVRCP_OP_CHANGE_PATH */
        struct {
            U16 uidCounter;          /* Current UID counter */
            U8  direction;           /* Direction to change in the path (up or down) */
            U8 *uid;                 /* UID of folder to change to (down) */
        } chPath;

        /* Item Attributes.  Valid when "advOp" is AVRCP_OP_GET_ITEM_ATTRIBUTES */
        struct {
            AvrcpScope  scope;       /* The requested scope */
            U8         *uid;         /* UID of the requested item */
            U16         uidCounter;  /* Current UID counter */
            U8          numAttrs;    /* Number of attributes requested */
            U8         *attrList;    /* List of attributes requested */
        } attr;

        /* Search. Valid when "advOp" is AVRCP_OP_SEARCH */
        struct {
            U16  charSet;            /* Character set of the search string */
            U16  len;                /* Length of the search string */
            U8  *str;                /* Search string */
        } search;
    } p;

} AvrcpBrowseReqParms;
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

#if AVRCP_BROWSING_TARGET == XA_ENABLED
/*---------------------------------------------------------------------------
 * AvrcpBrowseInfParms structure
 *
 * Defines the callback parameters for the AVRCP_EVENT_BROWSE_INFO event.
 */
typedef union _AvrcpBrowseInfParms {

    /* The browsed player.  Valid when "advOp" is AVRCP_OP_SET_BROWSED_PLAYER
     */
    AvrcpMediaPlayerItem  *brwsPlayer;

} AvrcpBrowseInfParms;
#endif /* AVRCP_BROWSING_TARGET == XA_ENABLED */

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
/*---------------------------------------------------------------------------
 * AvrcpBrowseRspParms structure
 *
 * Defines the callback parameters for the AVRCP_EVENT_BROWSE_RESPONSE.
 */
typedef union _AvrcpBrowseRspParms {

    /* Browsed Player. Valid when "advOp" is AVRCP_OP_SET_BROWSED_PLAYER */
    struct {
        U16  uidCounter;  /* Current UID counter */
        U32  numItems;    /* Number of items in the current path */
        U16  charSet;     /* Character set used by the player */
        U8   fDepth;      /* Number of folder length/name pairs to follow */
        U8  *list;        /* List of folder names */
    } brwsPlayer;

    /* Folder Items. Valid when "advOp" is AVRCP_OP_GET_FOLDER_ITEMS */
    struct {
        U16  uidCounter;  /* Current UID counter */
        U32  numItems;    /* Number of items in the current path */
        U8  *list;        /* List of items returned */
    } fldrItems;

    /* Change Path. Valid when "advOp" is AVRCP_OP_CHANGE_PATH */
    struct {
        U32  numItems;    /* Number of items in the current path */
    } chPath;

    /* Item Attributes. Valid when "advOp" is AVRCP_OP_GET_ITEM_ATTRIBUTES */
    struct {
        U8   numAttrs;    /* Number of attributes returned */
        U8  *list;        /* List of attributes returned */
    } itemAttrs;

    /* Search. Valid when "advOp" is AVRCP_OP_SEARCH */
    struct {
        U16  uidCounter;  /* Current UID counter */
        U32  numItems;    /* Number of items found in the search */
    } search;

} AvrcpBrowseRspParms;
#endif /* AVRCP_BROWSING_CONTROLLER == XA_ENABLED */


#if (AVRCP_ADVANCED_TARGET == XA_ENABLED) || (AVRCP_ADVANCED_CONTROLLER == XA_ENABLED)
/*---------------------------------------------------------------------------
 * AvrcpAdvCallbackParms structure
 *
 * Defines the callback parameters for advanced AVRCP commands/responses.
 */
typedef union _AvrcpAdvCallbackParms {

    /* Advanced PDU associated with the event. Valid during the
       AVRCP_EVENT_ADV_TX_DONE event */
    AvrcpAdvancedPdu *txPdu;

#if AVRCP_ADVANCED_TARGET == XA_ENABLED

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
    /* Contains the parameters of a request that was received from the
     * controller.  A response is required.  This is valid during the
     * AVRCP_EVENT_ADV_REQ event.
     */ 
    AvrcpAdvReqParms req;
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

    /* Contains information about a command that altered the local settings.
     * This is valid during the AVRCP_EVENT_ADV_INFO event.  No response is
     * required
     */
    AvrcpAdvInfParms info;

#endif /* AVRCP_ADVANCED_TARGET == XA_ENABLED */

#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED

    /* Contains the parameters received in response to an advanced control
     * command. This is valid during the AVRCP_EVENT_ADV_RESPONSE event.
     */ 
    AvrcpAdvRspParms rsp;

    /* Contains the parameters of a notification or status of a registered 
     * event. This is valid during the AVRCP_EVENT_ADV_RESPONSE
     * when "advOp" is set to AVRCP_OP_REGISTER_NOTIFY.  Also valid
     * during the AVRCP_EVENT_ADV_NOTIFY event.
     */
    AvrcpAdvNotifyParms notify;

#endif /* AVRCP_ADVANCED_CONTROLLER == XA_ENABLED */

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
    /* Contains the parameters of a request that was received from the
     * controller on the browsing channel.  A response is required.  This is
     * valid during the AVRCP_EVENT_BROWSE_REQ event.
     */ 
    AvrcpBrowseReqParms  browseReq;
    
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

#if AVRCP_BROWSING_TARGET == XA_ENABLED
    /* Contains information about a command that altered the local settings.
     * This is valid during the AVRCP_EVENT_BROWSE_INFO event.  No response
     * is required
     */
    AvrcpBrowseInfParms  browseInfo;
#endif /* AVRCP_BROWSING_TARGET == XA_ENABLED */

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
    /* Contains the parameters received in response to an browsing command.
     * This is valid during the AVRCP_EVENT_BROWSE_RESPONSE event.
     */ 
    AvrcpBrowseRspParms  browseRsp;
#endif /* AVRCP_BROWSING_CONTROLLER == XA_ENABLED */

} AvrcpAdvCallbackParms;

#endif /* (AVRCP_ADVANCED_TARGET == XA_ENABLED) || (AVRCP_ADVANCED_CONTROLLER == XA_ENABLED) */

/*---------------------------------------------------------------------------
 * AvrcpCallbackParms structure
 *
 * Contains information for the application callback event.
 */
struct _AvrcpCallbackParms {

    /* AVRCP event */ 
    AvrcpEvent      event;

    /* AVRCP channel associated with the event */ 
    AvrcpChannel   *channel;

    /* Status of the communications channel */
    BtStatus        status;

#if (AVRCP_ADVANCED_TARGET == XA_ENABLED) || (AVRCP_ADVANCED_CONTROLLER == XA_ENABLED)
    /* AVRCP Advanced operation */
    AvrcpOperation  advOp;

    /* AVRCP error code.  Contains the AVRCP error code when an advanced
     * operation fails.  Valid if 'status' is not equal to BT_STATUS_SUCCESS,
     * otherwise it is set to AVRCP_ERR_NO_ERROR.  If 'status' is not equal
     * to BT_STATUS_SUCCESS and 'errorCode' is set to AVRCP_ERR_NO_ERROR,
     * then there was a transport error.
     */
    AvrcpErrorCode errorCode;
#endif

    /* Callback parameter object */ 
    union {

        /* Remote device associated with the event */ 
        BtRemoteDevice   *remDev;

        /* Command frame associated with the event */ 
        AvrcpCmdFrame    *cmdFrame;

        /* Response frame associated with the event */ 
        AvrcpRspFrame    *rspFrame;

        /* Panel indication received during AVRCP_EVENT_PANEL_CNF */ 
        struct {

            /* Operation to which the remote target responded */ 
            AvrcpPanelOperation operation;

            /* The press state of the key in the command to which 
             * the target responded.  
             */ 
            BOOL                press;

            /* Response from the target. May indicate 
             * an "extended" response code such as 
             * AVRCP_RESPONSE_SKIPPED or AVRCP_RESPONSE_TIMEOUT.  
             */ 
            AvrcpResponse       response;
        } panelCnf;

        /* Panel indication received during AVRCP_EVENT_PANEL_PRESS, 
         * AVRCP_EVENT_PANEL_HOLD, or AVRCP_EVENT_PANEL_RELEASE.  
         */ 
        struct {

            /* Operation corresponding to the key pressed, held, or 
             * released. AVRCP will only indicate a new operation 
             * when the previous one has been _RELEASE'd.  
             */ 
            AvrcpPanelOperation operation;

        } panelInd;

#if (AVRCP_ADVANCED_TARGET == XA_ENABLED) || (AVRCP_ADVANCED_CONTROLLER == XA_ENABLED)

        /* Advanced AVRCP command/response parameters */
        AvrcpAdvCallbackParms adv;
#endif

    } p;
};

/*---------------------------------------------------------------------------
 * AvrcpChannel structure
 *
 * Defines the RCP channel.
 */
struct _AvrcpChannel {

    /* === Internal use only === */ 

    /* AVRCP Role */ 
    AvrcpRole       role;

    /* Application callback function */ 
    AvrcpCallback   callback;

    /* AVCTP Channel */ 
    AvctpChannel    chnl;
    BOOL            initiator;

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
    /* AVCTP Browing Channel */
    AvctpChannel    brwsChnl;
    BOOL            disconnecting;
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
    U8              brwsCmdTransId;
    U8              brwsCmdTransactions[AVRCP_MAX_NOTIFICATIONS+1];
#endif /* AVRCP_BROWSING_CONTROLLER == XA_ENABLED */

    /* Transaction IDs */ 
    U8              cmdTransId;
    U8              rspTransId;
    U8              rspTransType;
    U8              cmdTransactions[AVRCP_MAX_NOTIFICATIONS+1];

    /* Connection Manager Handler */ 
    CmgrHandler     cmgrHandler;

    /* SDP Variables */ 
    U8              sdpStage;
    SdpQueryToken   sdpQueryToken;
    U8              sdpSrchAttrib[AVRCP_NUM_SDP_SEARCH_ATTRIBUTES];

    /* Profile version parsed from SDP */ 
    AvrcpVersion    avrcpVersion;

    /* Features of currently connected remote device */ 
    AvrcpFeatures   remoteCtFeatures;
    AvrcpFeatures   remoteTgFeatures;

    /* Panel Subunit */
    struct {
        struct {
            U8                   state;
            EvmTimer             timer;
            AvrcpPanelOperation  opQueue[AVRCP_SUBUNIT_OP_QUEUE_MAX];
            U8                   opRead, opWrite;
            AvrcpCmdFrame        cmd;
            U8                   data[7];
        } Int;

        struct {
            U8                   state;
            EvmTimer             timer;
            AvrcpPanelOperation  curOp;
            AvrcpResponse        curRsp;
            AvrcpRspFrame        rsp;
            BOOL                 rspInUse;
            U8                   data[7];
        } Rsp;
        
        /* Received Commands */
        ListEntry       rxCmdPool;
        ListEntry       rxCmdQueue;
        AvrcpCmdFrame   rxCmdFrames[AVRCP_MAX_NOTIFICATIONS+1];
        U8              rxCmdParms[AVRCP_MAX_NOTIFICATIONS+1][7];

    } panel;

#if (AVRCP_ADVANCED_TARGET == XA_ENABLED) || (AVRCP_ADVANCED_CONTROLLER == XA_ENABLED)

    /* Advanced Control */
    struct {
#if (AVRCP_ADVANCED_TARGET == XA_ENABLED) || (AVRCP_BROWSING_TARGET == XA_ENABLED) || (AVRCP_LIST_PLAYERS_ENABLED)
        /* Advanced PDU resource pool */
        ListEntry               advPduPool;
        AvrcpAdvancedPdu        advPdus[AVRCP_MAX_NOTIFICATIONS+1];
        U8                      advPduParms[AVRCP_MAX_NOTIFICATIONS+1][AVRCP_ADVANCED_CMD_SIZE];
#endif

#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED
        AvrcpEventMask          rem_eventMask;

#endif

#if AVRCP_ADVANCED_TARGET_SLIM == XA_ENABLED
        /* Supported Events */ 
        AvrcpEventMask          eventMask;
#endif

#if AVRCP_ADVANCED_TARGET == XA_ENABLED

        /* Supported Events */ 
        AvrcpEventMask          eventMask;

        /* Supported player settings */ 
        AvrcpPlayerAttrIdMask   playerSettingsMask;

        /* Supported media attributes */ 
        AvrcpMediaAttrIdMask    mediaAttrMask;

        /* Current player settings */ 
        AvrcpPlayerSetting      playerSettings[AVRCP_NUM_PLAYER_SETTINGS];

        /* Current player strings */ 
        AvrcpPlayerStrings      playerStrings[AVRCP_NUM_PLAYER_SETTINGS];

        /* Current media info */ 
        AvrcpMediaInfo          mediaInfo[AVRCP_NUM_MEDIA_ATTRIBUTES];

        /* Current status of the playing media */ 
        AvrcpMediaPlayStatus    playStatus;

        /* Battery Status */ 
        AvrcpBatteryStatus      battStatus;

        /* System Status */ 
        AvrcpSystemStatus       sysStatus;

        /* Current track index */ 
        AvrcpTrackStruct        currentTrack;

        /* Current character set */ 
        AvrcpCharSets           charSets;
        U16                     defaultCharSet;

        /* This is part of the Current Registered Notifications Parameters.  
         * It contains the number of notification that have been sent.  
         */ 
        U8                      numNotifications;

        /* This is part of the Current Registered Notifications Parameters.  
         * It contains the time interval that should elapse between position 
         * reports.  
         */ 
        U32                     playbackInterval;

        /* This is part of the Current Registered Notifications Parameters.  
         * If the track was selected when the playback position was last 
         * reported, then this contains the last reported playback position 
         * within the current track, measured in milliseconds.  Otherwise, 
         * it will contain AVRCP_NO_TRACK_CURRENTLY_SELECTED.
         */ 
        U32                     playbackReference;

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
        /* The absolute volume on the device.  With volume commands, the
         * Sink is the target and the Source is the controller
         */ 
        U8                      volume;

        /* Media players selected on the target */
        AvrcpMediaPlayerItem   *addrPlayer;
        U16                     uidCounter;
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
        AvrcpMediaPlayerItem   *brwsPlayer;

        /* System browsing structure */
        BOOL                    brwsRspInUse;
        AvrcpRspFrame           brwsRsp;

        ListEntry               txBrwsRspQueue;
        AvrcpAdvancedPdu       *txBrwsRspPdu;
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

        /* System response structure */
        BOOL                    rspInUse;
        AvrcpRspFrame           rsp;

        /* Current Registered Notifications */ 
        U8                      rspNotifications[AVRCP_MAX_NOTIFICATIONS+1];

        /* Received Command PDUs */
        ListEntry               rxCmdQueue;
        AvrcpAdvancedPdu       *rxCmdPdu;
        BOOL                    cmdFragment;

        /* API Response PDU queue */
        ListEntry               txRspQueue;
        AvrcpAdvancedPdu       *txRspPdu;

        /* System Response PDU */
        AvrcpAdvancedPdu        sysRspPdu;
        U8                      sysRspParms[AVRCP_ADVANCED_RESPONSE_SIZE];
        U16                     sysRspParmLen;

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
        /* Registered Media Players */
        U16                     regPlyrCount;
        AvrcpMediaPlayerItem   *regPlayers[AVRCP_MAX_MEDIA_PLAYERS];
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

#endif /* AVRCP_ADVANCED_TARGET == XA_ENABLED */

#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED

        /* System command structure */
        BOOL                    cmdInUse;
        AvrcpCmdFrame           cmd;

        /* 0 = unknown, 1 = command, 2 = response */ 
        U8                      type;

        /* API Command PDU queue   */
        ListEntry               txCmdQueue;
        AvrcpAdvancedPdu       *txCmdPdu;

        /* System PDU */
        AvrcpAdvancedPdu        sysCmdPdu;
        U8                      sysCmdParms[AVRCP_ADVANCED_CMD_SIZE];

        /* Received Response PDU */
        AvrcpAdvancedPdu        rspPdu;
        U8                      rspParms[AVRCP_ADVANCED_RESPONSE_SIZE];
        U16                     rspParmLen;
        BOOL                    rspFragment;

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
        BOOL                    brwsCmdInUse;
        AvrcpCmdFrame           brwsCmd;

        ListEntry               txBrwsCmdQueue;
        AvrcpAdvancedPdu       *txBrwsCmdPdu;
#endif /* AVRCP_BROWSING_CONTROLLER == XA_ENABLED */

#endif /* AVRCP_ADVANCED_CONTROLLER == XA_ENABLED */

    } adv;

#endif /* (AVRCP_ADVANCED_TARGET == XA_ENABLED) ||( AVRCP_ADVANCED_CONTROLLER == XA_ENABLED) */

};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

#if XA_DECODER == XA_ENABLED
/* AvrcpInitTypes() */
void AvrcpInitTypes(void);
#endif

/*---------------------------------------------------------------------------
 * AVRCP_Init()
 *
 *     Initialize the AVRCP SDK.  This function should only be called
 *     once, normally at initialization time.  The calling of this function
 *     can be specified in overide.h using the XA_LOAD_LIST macro
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(AVRCP) ... ).
 *
 * Returns:
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL AVRCP_Init(void);

/*---------------------------------------------------------------------------
 * AVRCP_Register()
 *
 *      Registers an application callback to receive AVRCP events. This 
 *      function must be called before any other AVRCP functions.  Multiple
 *      channels may be registered for connections to multiple devices,
 *      however, the features registered with a role must be the same for
 *      each registration.  The features and security parameters specified 
 *      with the first registration of a role will be registered with the 
 *      SDP entry, and the features of subsequent registrations for that 
 *      role will be ignored.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 *      callback - Identifies the application function that will be called
 *          with AVRCP events.
 *
 *      role - Role and features supported by the device.  Each registration
 *          of a channel must specify the same role and features.
 *
 *      SecParms - Requested security parameters.  If 0, baseline security
 *                 parameters are used (BSL_SECURITY_L1_IN | BSL_SECURITY_L1_OUT).
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The AVRCP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_IN_USE - The specified channel is already in use.
 * 
 *      BT_STATUS_FAILED - The specified role differs from a previously
 *      registered role.  Only one role can be defined per device.
 *
 *      BT_STATUS_INVALID_PARM - The chnl or Callback parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).
 */
BtStatus AVRCP_RegisterSec(AvrcpChannel *chnl, 
                           AvrcpCallback callback, 
                           AvrcpRole     role,
                           BtSecurityParms *SecParms);

/*---------------------------------------------------------------------------
 * AVRCP_Register()
 *
 *      Registers an application callback to receive AVRCP events. This 
 *      function must be called before any other AVRCP functions.  Multiple
 *      channels may be registered for connections to multiple devices,
 *      however, the features registered with a role must be the same for
 *      each registration.  The features specified with the first
 *      registration of a role will be registered with the SDP entry, and the
 *      features of subsequent registrations for that role will be ignored.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 *      callback - Identifies the application function that will be called
 *          with AVRCP events.
 *
 *      role - Role and features supported by the device.  Each registration
 *          of a channel must specify the same role and features.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The AVRCP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_IN_USE - The specified channel is already in use.
 * 
 *      BT_STATUS_FAILED - The specified role differs from a previously
 *      registered role.  Only one role can be defined per device.
 *
 *      BT_STATUS_INVALID_PARM - The chnl or Callback parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).
 */
BtStatus AVRCP_Register(AvrcpChannel *chnl, 
                        AvrcpCallback callback, 
                        AvrcpRole     role);
#define AVRCP_Register(X,Y,Z) AVRCP_RegisterSec(X, Y, Z, 0)

/*---------------------------------------------------------------------------
 * AVRCP_Deregister()
 *
 *      De-registers the AVRCP callback. After making this call
 *      successfully, the callback specified in AVRCP_Register will
 *      receive no further events.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The AVRCP callback was successfully 
 *          deregistered.  
 * 
 *      BT_STATUS_IN_USE - The specified channel is still in use.
 *
 *      BT_STATUS_NOT_FOUND - An AVRCP callback was not previously 
 *          registered.  
 *
 *      BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 */
BtStatus AVRCP_Deregister(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_Connect()
 * 
 *     Initiates a connection to a remote AVRCP device.  This function is 
 *     used to establish the lower layer connection (L2CAP), which allows
 *     sending messages.
 *
 *     If the connection attempt is successful, the AVRCP_EVENT_CONNECT event 
 *     will be received.  If the connection attempt is unsuccessful, the 
 *     AVRCP_EVENT_DISCONNECT event will be received.  
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.  
 *
 *      addr - The Bluetooth address of the device to which the connection 
 *             should be made. If 0, the connection manager is used 
 *             to select an appropriate device.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection process has been successfully 
 *         started. When the connection process is complete, the 
 *         application callback will receive either the AVRCP_EVENT_CONNECT 
 *         or AVRCP_EVENT_DISCONNECT event.
 *
 *     BT_STATUS_IN_USE - This channel is already connected or is in the 
 *         process of connecting.  
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device 
 *         selector database.  The device must be discovered, paired, or 
 *         added manually using DS_AddDevice().  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus AVRCP_Connect(AvrcpChannel *chnl, BT_BD_ADDR *addr);

/*---------------------------------------------------------------------------
 * AVRCP_ConnectRsp()
 * 
 *     Responds to a connection request from the remote AVRCP device.  This 
 *     function is used to establish the lower layer connection (L2CAP), 
 *     which allows the sending of commands.  
 *
 * Parameters:
 *
 *     Chnl - A registered AVRCP channel.  
 *
 *     Accept - TRUE accepts the connect or FALSE rejects the connection.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection responses has been successfully 
 *         sent. When the connection process is complete, the application 
 *         callback will receive the AVRCP_EVENT_CONNECT event.  
 *
 *     BT_STATUS_BUSY - The connection is already connected.  
 *
 *     BT_STATUS_INVALID_PARM - The Chnl parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device 
 *         selector database.  The device must be discovered, paired, or 
 *         added manually using DS_AddDevice().  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus AVRCP_ConnectRsp(AvrcpChannel *Chnl, BOOL Accept);
#define AVRCP_ConnectRsp(c, a) AVCTP_ConnectRsp(&(c)->chnl, a)

/*---------------------------------------------------------------------------
 * AVRCP_Disconnect()
 *
 *     Terminates a connection with a remote AVRCP device.  The lower layer 
 *     connection (L2CAP) is disconnected.  
 *
 * Parameters:
 *
 *     chnl - A registered and open AVRCP channel.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The disconnect process has been successfully 
 *         started. When the disconnect process is complete, the 
 *         application callback will receive the AVRCP_EVENT_DISCONNECT 
 *         event.  
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists on the specified 
 *         channel.  
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device 
 *         selector database.  The device must be discovered, paired, or 
 *         added manually using DS_AddDevice().  
 *
 *     It is possible to receive other error codes, depending on the lower 
 *     layer service in use (L2CAP or Management Entity).  
 */
BtStatus AVRCP_Disconnect(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_SendCommand()
 *
 *     Sends an RCP command on the specified channel. The channel must be 
 *     connected, in the open state, and without another command 
 *     outstanding.  The "cmdFrame" parameter must be set with valid RCP 
 *     command parameters.  
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmdFrame - An AvrcpCmdFrame structure initialized with valid 
 *         RCP command parameters.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The send command operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the AVRCP_EVENT_TX_DONE 
 *         event.  
 *
 *     BT_STATUS_BUSY - A command is already outstanding.  
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer, or the channel is not registered as a controller 
 *         (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_SendCommand(AvrcpChannel *chnl, AvrcpCmdFrame *cmdFrame);

/*---------------------------------------------------------------------------
 * AVRCP_SendResponse()
 *
 *     Sends an RCP command on the specified channel. The channel must be 
 *     connected and in the open state. The "cmdFrame" parameter must be 
 *     set with valid RCP command parameters.  
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     rspFrame - An AvrcpRspFrame structure initialized with valid 
 *         RCP response parameters.  The 'transId' parameter should be set to
 *         match the transaction ID of a received command.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The send response operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the AVRCP_EVENT_TX_DONE 
 *         event.  
 *
 *     BT_STATUS_FAILED - There is no outstanding command to respond to.  
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_SendResponse(AvrcpChannel *chnl, AvrcpRspFrame *rspFrame);

/*---------------------------------------------------------------------------
 * AVRCP_GetRemoteDevice()
 *
 *     Returns a pointer to the current remote device.  
 *
 * Parameters:
 *
 *     chnl - An AVRCP channel.  
 *
 * Returns:
 *
 *     A pointer to a remote device.  
 */
BtRemoteDevice *AVRCP_RemoteDevice(AvrcpChannel *chnl);
#define AVRCP_RemoteDevice(c) AVCTP_RemoteDevice(&(c)->chnl)

/*---------------------------------------------------------------------------
 * AVRCP_SetPanelKey()
 *
 *     Indicates the state of the key corresponding to the specified 
 *     panel subunit operation. Successive calls to this function will 
 *     queue up key events to be delivered to the target.  
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     op - Panel operation code to send. If a previous call indicated a 
 *         different "advOp" as pressed, calling this function with a new 
 *         "advOp" will automatically release it.  
 *
 *     press - TRUE indicates the key corresponding to operation was pressed, 
 *         FALSE indicates the key was released. For FALSE, if the "advOp" 
 *         specified was not already pressed, this call signals a 
 *         single press-and-release of the key.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully. 
 *         AVRCP_EVENT_PANEL_CNF message(s) will be sent corresponding 
 *         to the target's responses to the press, release, or hold messages 
 *         brought about by this command.  
 *
 *     BT_STATUS_NO_RESOURCES - The internal keystroke buffer is full.  
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).  
 */
BtStatus AVRCP_SetPanelKey(AvrcpChannel       *chnl, 
                           AvrcpPanelOperation op,
                           BOOL                press);

/*---------------------------------------------------------------------------
 * AVRCP_RejectPanelOperation()
 *
 *     Rejects a panel operation received from the connected 
 *     controller. This function must be called during 
 *     notification of the AVRCP_EVENT_PANEL_IND event. Further notifications 
 *     corresponding to the operation (AVRCP_PS_HOLD, AVRCP_PS_RELEASE) 
 *     will not be received.  
 *
 *     If this function is not called during the AVRCP_EVENT_PANEL_IND 
 *     the operation is automatically "ACCEPTED".  
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     rsp - Response code. Must be one of AVRCP_RESPONSE_NOT_IMPLEMENTED 
 *         or AVRCP_RESPONSE_REJECTED. Note that REJECTED is only used 
 *         when the operation is already reserved by another controller.  
 *         This response code does NOT indicate the result of the 
 *         operation itself (for example, a PLAY command when no media 
 *         is available should be accepted, but simply have no effect).  
 *
 * Returns:
 *     None
 */
void AVRCP_RejectPanelOperation(AvrcpChannel *chnl, AvrcpResponse rsp);
#define AVRCP_RejectPanelOperation(c,r) (c)->panel.Rsp.curRsp = (r)

#if AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED
/*---------------------------------------------------------------------------
 * AVRCP_GetBrowsingTxMtuSize()
 *
 *     Returns the maximum amount of data that the remote device can receive
 *     on the browsing channel.
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 * 
 * Returns:
 *     Browsing channel MTU size.
 */
U16 AVRCP_GetBrowsingTxMtuSize(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_BrowseConnect()
 * 
 *     Initiates a connection to a remote AVRCP device for browsing.  This
 *     function is used to establish the lower layer connection (L2CAP),
 *     which allows sending browsing commands.
 *
 *     If the connection attempt is successful, the
 *     AVRCP_EVENT_BROWSE_CONNECT event will be received.  If the connection
 *     attempt is unsuccessful, the AVRCP_EVENT_BROWSE_DISCONNECT event will
 *     be received.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection process has been successfully 
 *         started. When the connection process is complete, the 
 *         application callback will receive either the AVRCP_EVENT_CONNECT 
 *         or AVRCP_EVENT_DISCONNECT event.
 *
 *     BT_STATUS_IN_USE - This channel is already connected or is in the 
 *         process of connecting.  
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NO_CONNECTION - The AVRCP control channel does not exist.  A
 *         browsing channel cannot be established if the control channel is
 *         not already connected.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus AVRCP_BrowseConnect(AvrcpChannel *chnl);

#if AVRCP_BROWSE_AUTO_ACCEPT == XA_DISABLED
/*---------------------------------------------------------------------------
 * AVRCP_BrowseConnectRsp()
 * 
 *     Responds to a connection request from the remote AVRCP device for the
 *     browsing channel. This function is used to establish the lower layer
 *     connection (L2CAP), which allows the sending of browsing commands.
 *
 * Parameters:
 *
 *     Chnl - A registered AVRCP channel.  
 *
 *     Accept - TRUE accepts the connect or FALSE rejects the connection.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection responses has been successfully 
 *         sent. When the connection process is complete, the application 
 *         callback will receive the AVRCP_EVENT_BROWSE_CONNECT event.  
 *
 *     BT_STATUS_BUSY - The connection is already connected.  
 *
 *     BT_STATUS_INVALID_PARM - The Chnl parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NO_CONNECTION - The AVRCP control channel does not exist.  A
 *         browsing channel cannot be established if the control channel is
 *         not already connected.
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus AVRCP_BrowseConnectRsp(AvrcpChannel *Chnl, BOOL Accept);
#endif /* AVRCP_BROWSE_AUTO_ACCEPT == XA_DISABLED */

/*---------------------------------------------------------------------------
 * AVRCP_BrowseDisconnect()
 *
 *     Terminates the browsing channel connection.  The lower layer
 *     connection (L2CAP) is disconnected.
 *
 * Parameters:
 *
 *     chnl - A registered and open AVRCP channel.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The disconnect process has been successfully 
 *         started. When the disconnect process is complete, the 
 *         application callback will receive the
 *         AVRCP_EVENT_BROWSE_DISCONNECT event.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists on the specified 
 *         channel.  
 *
 *     It is possible to receive other error codes, depending on the lower 
 *     layer service in use (L2CAP or Management Entity).  
 */
BtStatus AVRCP_BrowseDisconnect(AvrcpChannel *chnl);
#endif /* AVRCP_BROWSING_ENABLED || AVRCP_LIST_PLAYERS_ENABLED */

#if AVRCP_ADVANCED_TARGET == XA_ENABLED
/*---------------------------------------------------------------------------
 * AVRCP_TgSetEventMask()
 *
 *     Allows the application to specify which events will be supported 
 *     by the current media player.  When a flag is set in the event 
 *     mask, then associated feature is supported.  
 *     AVRCP_ENABLE_PLAY_STATUS_CHANGED and AVRCP_ENABLE_TRACK_CHANGED 
 *     must both be set, and if not specified, will be added to the 
 *     mask.  
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     mask - A bitmask with bits set to enable individual events.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.  
 */
BtStatus AVRCP_TgSetEventMask(AvrcpChannel  *chnl, 
                              AvrcpEventMask mask);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetPlayerSettingsMask()
 *
 *     Allows the application to specify which player settings are 
 *     supported by the current media player.  When a flag is set in 
 *     the event mask, then associated setting is supported.  This mask
 *     applies only to player settings defined by the AVRCP specification.
 *     Extensions to these player settings are possible, and are defined by
 *     the application.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     mask - A bitmask with bits set to enable individual player settings.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.  
 */
BtStatus AVRCP_TgSetPlayerSettingsMask(AvrcpChannel         *chnl, 
                                       AvrcpPlayerAttrIdMask mask);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMediaAttributesMask()
 *
 *     Allows the application to specify which media attributes are
 *     supported by the current media player.  When a flag is set in 
 *     the event mask, then associated setting is supported.  
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     mask - A bitmask with bits set to enable individual media attributes.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.  
 */
BtStatus AVRCP_TgSetMediaAttributesMask(AvrcpChannel        *chnl, 
                                        AvrcpMediaAttrIdMask mask);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetPlayerSetting()
 *
 *     Allows the application to specify the local settings for the addressed
 *     player. The settings are updated internally and, when requested, will
 *     be sent to the controller.
 * 
 *     If the controller has registered for notification of a change in the
 *     player settings (AVRCP_EID_APP_SETTING_CHANGED), then calling this
 *     function will result in completion of that notification.  The memory
 *     pointed to by the 'rsp' parameter will be used to send the completion
 *     event, and this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 *
 *     setting - The new player setting.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl', 'rsp', or 'setting' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetPlayerSetting(AvrcpChannel       *chnl, 
                                  AvrcpAdvancedPdu   *rsp,
                                  AvrcpPlayerSetting *setting);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetPlayerStrings()
 *
 *     Allows the application to specify the local strings for the addressed
 *     player. The strings are updated internally and, when requested, will
 *     be sent to the controller.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     attrId - The player setting attribute to which the strings apply.  
 *
 *     strings - A set of strings in the IANA format specified in the call to 
 *         AVRCP_TgSetCharSet().  If AVRCP_TgSetCharSet() has never been
 *         called, then the default format is UTF-8.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or strings parameter is invalid.  
 */
BtStatus AVRCP_TgSetPlayerStrings(AvrcpChannel       *chnl, 
                                  AvrcpPlayerAttrId   attrId,
                                  AvrcpPlayerStrings *strings);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetMediaInfo()
 *
 *     Allows the application to specify the local media information for the 
 *     current track.  The media information is updated internally and, when
 *     requested, will be sent to the controller.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     numElements - The number of elements in the info parameter.  
 *
 *     info - An array of structures containing the media information for the 
 *         current track (see AvrcpMediaInfo).  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or info parameter is invalid.  
 */
BtStatus AVRCP_TgSetMediaInfo(AvrcpChannel   *chnl, 
                              U8              numElements, 
                              AvrcpMediaInfo *info);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetPlayStatus()
 *
 *     Allows the application to specify the media status of the local
 *     addressed player.  The status is updated internally and, when
 *     requested, will be sent to the controller.
 * 
 *     If the controller has registered for notification of a change in the
 *     media play status (AVRCP_EID_MEDIA_STATUS_CHANGED), then calling this
 *     function will result in completion of that notification.  The memory
 *     pointed to by the 'rsp' parameter will be used to send the completion
 *     event, and this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     mediaStatus - The current play status of the media.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetPlayStatus(AvrcpChannel     *chnl, 
                               AvrcpAdvancedPdu *rsp,
                               AvrcpMediaStatus  mediaStatus);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetPlayPosition()
 *
 *     Allows the application to specify the local addressed player's media
 *     play position and length.  The status is updated internally and, when
 *     requested, will be sent to the controller.
 * 
 *     If the controller has registered for notification of a change in the
 *     play position (AVRCP_EID_PLAY_POS_CHANGED), then calling this function
 *     will result in completion of that notification.  The memory pointed to
 *     by the 'rsp' parameter will be used to send the completion event, and
 *     this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     rsp - A PDU for sending responses.
 * 
 *     position - The current position of the media in milliseconds.  
 *
 *     length - The total length of the media in milliseconds.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetPlayPosition(AvrcpChannel     *chnl, 
                                 AvrcpAdvancedPdu *rsp,
                                 U32               position, 
                                 U32               length);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetBattStatus()
 *
 *     Allows the application to specify the current Battery status of the
 *     local device.  The status is updated internally and, when requested,
 *     will be sent to the controller.
 * 
 *     If the controller has registered for notification of a change in the
 *     battery status (AVRCP_EID_BATT_STATUS_CHANGED), then calling this
 *     function will result in completion of that notification.  The memory
 *     pointed to by the 'rsp' parameter will be used to send the completion
 *     event, and this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     rsp - A PDU for sending responses.
 * 
 *     battStatus - The current battery status.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetBattStatus(AvrcpChannel      *chnl, 
                               AvrcpAdvancedPdu  *rsp,
                               AvrcpBatteryStatus battStatus);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetSystemStatus()
 *
 *     Allows the application to specify the current System status of the
 *     local device.  The status is updated internally and, when requested,
 *     will be sent to the controller.
 * 
 *     If the controller has registered for notification of a change in the
 *     system status (AVRCP_EID_SYS_STATUS_CHANGED), then calling this
 *     function will result in completion of that notification.  The memory
 *     pointed to by the 'rsp' parameter will be used to send the completion
 *     event, and this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     sysStatus - The current system status.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetSystemStatus(AvrcpChannel     *chnl, 
                                 AvrcpAdvancedPdu *rsp,
                                 AvrcpSystemStatus sysStatus);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetTrack()
 *
 *     Allows the application to specify the current track of the addressed
 *     player.  The status is updated internally and, when requested, will be
 *     sent to the controller.
 * 
 *     If the controller has registered for notification of a change in the
 *     current track (AVRCP_EID_TRACK_CHANGED), then calling this function
 *     will result in completion of that notification.  The memory pointed to
 *     by the 'rsp' parameter will be used to send the completion event, and
 *     this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     trackP - A pointer to a 64-bit track index structure.  As of AVRCP
 *         specification 1.4, the only valid values for the track index are 0
 *         (track selected) and AVRCP_NO_TRACK_CURRENTLY_SELECTED, unless
 *         browsing is supported.  If browsing is supported, the UID of a
 *         media element in the Now Playing folder should be used.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetTrack(AvrcpChannel     *chnl, 
                          AvrcpAdvancedPdu *rsp,
                          AvrcpTrackStruct *trackP);

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
/*---------------------------------------------------------------------------
 * AVRCP_TgSetAbsoluteVolume()
 *
 *     Allows the application to specify the current volume of the local
 *     device.  This function should only be called by the rendering device
 *     (Sink), which means that the local device may need to be registered as
 *     both a Category 2 Target and a Category 1 Controller.  The status is
 *     updated internally and, when requested, will be sent to the controller
 *     (Source).
 * 
 *     If the controller has registered for notification of a change in the
 *     volume (AVRCP_EID_VOLUME_CHANGED), then calling this function will
 *     result in completion of that notification.  The memory pointed to by
 *     the 'rsp' parameter will be used to send the completion event, and
 *     this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     volume - The absolute volume of the Sink (0 - 0x7F).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_PENDING - The send response operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the AVCTP_EVENT_TX_DONE 
 *         event.  
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetAbsoluteVolume(AvrcpChannel *chnl,  AvrcpAdvancedPdu *rsp, U8 volume);
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/*---------------------------------------------------------------------------
 * AVRCP_TgSignalTrackStart()
 *
 *     Allows the application to signal the start of a new track on the local
 *     addressed player.  The status is updated internally and, when
 *     requested, will be sent to the controller.
 * 
 *     If the controller has registered for notification of a track start
 *     (AVRCP_EID_TRACK_START), then calling this function will result in
 *     completion of that notification. The memory pointed to by the 'rsp'
 *     parameter will be used to send the completion event, and this function
 *     will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSignalTrackStart(AvrcpChannel *chnl, AvrcpAdvancedPdu *rsp);

/*---------------------------------------------------------------------------
 * AVRCP_TgSignalTrackEnd()
 *
 *     Allows the application to signal the end of a track on the local
 *     addressed player.  The status is updated internally and, when
 *     requested, will be sent to the controller.
 * 
 *     If the controller has registered for notification of a track end
 *     (AVRCP_EID_TRACK_END), then calling this function will result in
 *     completion of that notification. The memory pointed to by the 'rsp'
 *     parameter will be used to send the completion event, and this function
 *     will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSignalTrackEnd(AvrcpChannel *chnl, AvrcpAdvancedPdu *rsp);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetCurrentCharSet()
 *
 *     Allows the application to specify the default IANA character set for
 *     the local device. This number must match a character set received
 *     during the AVRCP_EVENT_ADV_INFO event when 'p.advOp' is set to
 *     AVRCP_OP_INFORM_DISP_CHAR_SET, or it must be set to UTF-8 (default).
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 *     charSet - The IANA character set that will be used.  
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' parameter is invalid.  
 */
BtStatus AVRCP_TgSetCurrentCharSet(AvrcpChannel *chnl, U16 charSet);

/*---------------------------------------------------------------------------
 * AVRCP_TgGetCurrentCharSet()
 *
 *     Retrieves the character set currently selected for display by the 
 *     controller.  
 *
 * Parameters:
 *     chnl - A registered AVRCP channel.  
 *
 * Returns:
 *     A 16 bit value containing the current IANA character set.  
 */
U16 AVRCP_TgGetCurrentCharSet(AvrcpChannel *chnl);

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED

/*---------------------------------------------------------------------------
 * AVRCP_TgInitMediaPlayerItem()
 * 
 *     Initialized an AvrcpMediaPlayerItem structure.  Before setting fields
 *     an the structure, this function should be called to initialize all
 *     fields to 0.
 * 
 * Parameters:
 *     player - An initialized media player structure.
 *
 * Returns:
 *     void
 */
void AVRCP_TgInitMediaPlayerItem(AvrcpMediaPlayerItem *player);
#define AVRCP_TgInitMediaPlayerItem(p) (OS_MemSet((U8*)p, 0, sizeof(AvrcpMediaPlayerItem)))

/*---------------------------------------------------------------------------
 * AVRCP_TgRegisterPlayer()
 *
 *     Allows the application to register a player on the Target.  Before
 *     registering a player, AVRCP_TgInitMediaPlayerItem() should be called,
 *     and then any information about the player should be initialized in the
 *     structure.
 * 
 *     If the controller has registered for notification of a change in the
 *     available players (AVRCP_EID_AVAILABLE_PLAYERS_CHANGED), then calling
 *     will result in completion of that notification. The memory pointed to
 *     by the 'rsp' parameter will be used to send the completion event, and
 *     this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     player - An initialized media player structure.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_FAILED - Not enough room to register the new player (see
 *         AVRCP_MAX_MEDIA_PLAYERS).
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl', 'rsp' or 'player' parameter is invalid.
 * 
 *     BT_STATUS_IN_USE - The player is already registered.
 */
BtStatus AVRCP_TgRegisterPlayer(AvrcpChannel *chnl,
                                AvrcpAdvancedPdu *rsp,
                                AvrcpMediaPlayerItem *player);

/*---------------------------------------------------------------------------
 * AVRCP_TgDeregisterPlayer()
 *
 *     Allows the application to deregister a player on the Target.
 * 
 *     If the controller has registered for notification of a change in the
 *     available players (AVRCP_EID_AVAILABLE_PLAYERS_CHANGED), then calling
 *     this function will result in completion of that notification. The
 *     memory pointed to by the 'rsp' parameter will be used to send the
 *     completion event, and this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     player - A data structure describing the media player that is being
 *         deregistered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl', 'rsp', or 'player' parameter is invalid.
 * 
 *     BT_STATUS_NOT_FOUND - The player is not registered.
 */
BtStatus AVRCP_TgDeregisterPlayer(AvrcpChannel *chnl,
                                  AvrcpAdvancedPdu *rsp,
                                  AvrcpMediaPlayerItem *player);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetAddressedPlayer()
 *
 *     Allows the application to specify the locally addressed media player.
 * 
 *     If the controller has registered for notification when the addressed
 *     player changes (AVRCP_EID_ADDRESSED_PLAYER_CHANGED), then calling
 *     this function will result in completion of that notification. The
 *     memory pointed to by the 'rsp' parameter will be used to send the
 *     completion event, and this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     player - A data structure describing the media player that will
 *         receive AVRCP commands.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl', 'rsp', or 'player' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetAddressedPlayer(AvrcpChannel         *chnl,  
                                    AvrcpAdvancedPdu     *rsp,
                                    AvrcpMediaPlayerItem *player);

/*---------------------------------------------------------------------------
 * AVRCP_TgNowPlayingChanged()
 *
 *     Allows the application to indicate a change in the "Now Playing"
 *     folder.
 * 
 *     If the controller has registered for notification of a change in the
 *     "Now Playing" folder (AVRCP_EID_NOW_PLAYING_CONTENT_CHANGED), then
 *     calling this function will result in completion of that notification.
 *     The memory pointed to by the 'rsp' parameter will be used to send the
 *     completion event, and this function will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     player - A data structure describing the media player whose now player
 *         list has changed.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl', 'rsp', or 'player' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgNowPlayingChanged(AvrcpChannel          *chnl,  
                                   AvrcpAdvancedPdu      *rsp,
                                   AvrcpMediaPlayerItem  *player);

/*---------------------------------------------------------------------------
 * AVRCP_TgUidsChanged()
 *
 *     Allows the application to indicate a change in the UIDs of the local
 *     virtual file system (media library).
 * 
 *     If the controller has registered for notification of a change in the
 *     UIDs (AVRCP_EID_UIDS_CHANGED), then calling this function will result
 *     in completion of that notification. The memory pointed to by the 'rsp'
 *     parameter will be used to send the completion event, and this function
 *     will return BT_STATUS_PENDING.
 * 
 *     If no request for notification has been registered by the controller,
 *     this function will return BT_STATUS_SUCCESS.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully without
 *         sending any data to the controller.  Only an internal status was
 *         set.  The memory pointed to by the "rsp" parameter can be re-used
 *         or freed.
 *
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         'chnl' or 'rsp' parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgUidsChanged(AvrcpChannel          *chnl,  
                             AvrcpAdvancedPdu      *rsp);

/*---------------------------------------------------------------------------
 * AVRCP_TgCreatePlayItemRsp()
 *
 *     Allows the application to create a response to a AVRCP_EVENT_ADV_REQ
 *     event where "advOp" is set to AVRCP_OP_PLAY_ITEM.   Once a response is
 *     created, it can be sent by calling AVRCP_TgSendAdvResponse().
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for creating the responses.
 * 
 *     error - Error code of the response.  If successful, set to
 *        AVRCP_ERR_NO_ERROR.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The response was created successfully.
 * 
 *     BT_STATUS_INVALID_PARM - The 'chnl' or 'rsp' parameter is invalid.
 */
BtStatus AVRCP_TgCreatePlayItemRsp(AvrcpChannel     *chnl,
                                   AvrcpAdvancedPdu *rsp,
                                   AvrcpErrorCode    error);

/*---------------------------------------------------------------------------
 * AVRCP_TgCreateAddToNowPlayingRsp()
 *
 *     Allows the application to create a response to a AVRCP_EVENT_ADV_REQ
 *     event where "advOp" is set to AVRCP_OP_ADD_TO_NOW_PLAYING.  Once a
 *     response is created, it can be sent by calling
 *     AVRCP_TgSendAdvResponse().
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for creating the responses.
 * 
 *     error - Error code of the response.  If successful, set to
 *        AVRCP_ERR_NO_ERROR.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The response was created successfully.
 * 
 *     BT_STATUS_INVALID_PARM - The 'chnl' or 'rsp' parameter is invalid.
 */
BtStatus AVRCP_TgCreateAddToNowPlayingRsp(AvrcpChannel     *chnl,
                                          AvrcpAdvancedPdu *rsp,
                                          AvrcpErrorCode    error);
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/*--------------------------------------------------------------------------
 * AVRCP_TgSendAdvResponse()
 *
 *     Allows the application to send a response to the AVRCP_EVENT_ADV_REQ
 *     event.  The 'rsp' parameter must be properly formatted according to
 *     the AVRCP specification.  Helper functions are available for creating
 *     a response to most advanced commands.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     transId - The transaction ID received in the callback parameters of
 *         the AVRCP_EVENT_ADV_REQ event (p.adv.req.transId).
 *
 * Returns:
 * 
 *     BT_STATUS_PENDING - The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_ADV_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 *
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSendAdvResponse(AvrcpChannel     *chnl,
                                 AvrcpAdvancedPdu *rsp,
                                 U8                transId);

#if AVRCP_BROWSING_TARGET == XA_ENABLED
/*---------------------------------------------------------------------------
 *  AVRCP_TgSendBrowsingResponse()
 *
 *     Sends a browsing response on the specified channel. The channel must
 *     be connected and in the open state.  Called in response to a
 *     AVRCP_EVENT_BROWSE_REQ event.  The 'rsp' parameter must be properly  
 *     formatted according to the AVRCP specification.  Helper functions are
 *     available for creating a response to most browsing commands.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     rsp - A PDU for sending responses.
 * 
 *     transId - The transaction ID of the request.
 * 
 * Returns:
 *     BT_STATUS_PENDING -  The operation was started and data was queued
 *         to be sent to the controller. When the associated data has been
 *         sent, the application callback will receive the
 *         AVRCP_EVENT_BROWSE_TX_DONE event.  The callback parameter
 *         'p.advTxPdu' will point to the memory passed into the 'rsp'
 *         parameter.
 * 
 *     BT_STATUS_IN_USE - The 'rsp' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSendBrowsingResponse(AvrcpChannel     *chnl, 
                                      AvrcpAdvancedPdu *rsp, 
                                      U8                transId);

#if AVRCP_SET_DEFAULT_BRWS_PLAYER == XA_ENABLED
/*---------------------------------------------------------------------------
 * AVRCP_TgSetBrowsedPlayer()
 *
 *     Sets the browsed player locally on the target.  All browsing commands
 *     on the specified control channel will be routed to that player.  This
 *     function is intended for testing only.  
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     player - A data structure describing the media player that will
 *         receive AVRCP commands.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.  
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_TgSetBrowsedPlayer(AvrcpChannel *chnl,  
                                  AvrcpMediaPlayerItem *player);
#endif

/*---------------------------------------------------------------------------
 * AVRCP_TgGetBrowsedPlayer()
 *
 *     Returns the current browsed player, or 0 if not browsed player has
 *     been set.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 * Returns:
 *     The current browsed player or 0.
 */
AvrcpMediaPlayerItem * AVRCP_TgGetBrowsedPlayer(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_TgSetUidCounter()
 *
 *     Set the current value of the UID counter.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     uidCounter - The new UID counter.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The response was created successfully.
 * 
 *     BT_STATUS_INVALID_PARM - The 'chnl' or 'rsp' parameter is invalid.
 */
BtStatus AVRCP_TgSetUidCounter(AvrcpChannel *chnl, U16 uidCounter);

/*---------------------------------------------------------------------------
 * AVRCP_TgGetUidCounter()
 *
 *     Get the current value of the UID counter.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 * Returns:
 *     The current UID counter.
 */
U16 AVRCP_TgGetUidCounter(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVRCP_TgCreateItemAttrRsp()
 *
 *    Create a response to an item attributes request.  This function will
 *    create a properly formatted response PDU for use with
 *    AVRCP_TgSendBrowsingResponse.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     pdu - A PDU for creating the responses.
 * 
 *     pduLen - The maximum size of the PDU.
 * 
 *     error - Error code of the response.  If successful, set to
 *        AVRCP_ERR_NO_ERROR.
 * 
 *     numAttrs - The number of attributes in the response.
 * 
 *     attributes - The list of attributes.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The response was created successfully.
 * 
 *     BT_STATUS_INVALID_PARM - The 'chnl' or 'rsp' parameter is invalid.
 */
BtStatus AVRCP_TgCreateItemAttrRsp(AvrcpChannel         *chnl,
                                   AvrcpAdvancedPdu     *pdu,
                                   U16                   pduLen,
                                   AvrcpErrorCode        errCode,
                                   U8                    numAttrs,
                                   AvrcpMediaAttribute  *attributes);

/*---------------------------------------------------------------------------
 * AVRCP_TgCreateChangePathRsp()
 *
 *     Create a response to a change directory request.  This function will
 *     create a properly formatted response PDU for use with
 *     AVRCP_TgSendBrowsingResponse.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     pdu - A PDU for creating the responses.
 * 
 *     pduLen - The maximum size of the PDU.
 * 
 *     error - Error code of the response.  If successful, set to
 *        AVRCP_ERR_NO_ERROR.
 * 
 *     numItems - The number of items in the current directory.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The response was created successfully.
 * 
 *     BT_STATUS_INVALID_PARM - The 'chnl' or 'rsp' parameter is invalid.
 */
BtStatus AVRCP_TgCreateChangePathRsp(AvrcpChannel     *chnl,
                                     AvrcpAdvancedPdu *pdu,
                                     U16               pduLen,
                                     AvrcpErrorCode    errCode,
                                     U32               numItems);

/*---------------------------------------------------------------------------
 * AVRCP_TgCreateSearchRsp()
 *
 *     Create a response to a search request.  This function will create a
 *     properly formatted response PDU for use with
 *     AVRCP_TgSendBrowsingResponse.
 * 
 * Parameters:
 *     chnl - A registered AVRCP channel.
 * 
 *     pdu - A PDU for creating the responses.
 * 
 *     pduLen - The maximum size of the PDU.
 * 
 *     error - Error code of the response.  If successful, set to
 *        AVRCP_ERR_NO_ERROR.
 * 
 *     numItems - The number of items in the search response.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The response was created successfully.
 * 
 *     BT_STATUS_INVALID_PARM - The 'chnl' or 'rsp' parameter is invalid.
 */
BtStatus AVRCP_TgCreateSearchRsp(AvrcpChannel     *chnl,
                                 AvrcpAdvancedPdu *pdu,
                                 U16               pduLen,
                                 AvrcpErrorCode    errCode,
                                 U32               numItems);

/*---------------------------------------------------------------------------
 * AVRCP_TgCreateFolderItemsRsp()
 *
 *     Create a response to a folder item list request.  This function will
 *     create a properly formatted response PDU for use with
 *     AVRCP_TgSendBrowsingResponse.
 * 
 *     Parameters: chnl - A registered AVRCP channel.
 * 
 *     pdu - A PDU for creating the responses.
 * 
 *     pduLen - The maximum size of the PDU.
 * 
 *     error - Error code of the response.  If successful, set to
 *        AVRCP_ERR_NO_ERROR.
 * 
 *     numItems - The number of items in the response.
 * 
 *     items - The list of items in the response.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The response was created successfully.
 * 
 *     BT_STATUS_INVALID_PARM - The 'chnl' or 'rsp' parameter is invalid.
 */
U16 AVRCP_TgCreateFolderItemsRsp(AvrcpChannel      *chnl, 
                                 AvrcpAdvancedPdu  *pdu, 
                                 U16                pduLen,
                                 AvrcpErrorCode     errCode,
                                 U16                numItems,
                                 AvrcpMediaItem   *items);
#endif /* AVRCP_BROWSING_TARGET == XA_ENABLED */
#endif /* AVRCP_ADVANCED_TARGET == XA_ENABLED */



BtStatus AVRCP_CtGetCapabilities_Rsp(AvrcpChannel *chnl,
                                 AvrcpAdvancedPdu *cmd,
                                 AvrcpCapabilityId  capabilityId,
                                 u16 eventmsk);

BtStatus AVRCP_CtGetAbsoluteVolume_Rsp(AvrcpChannel *chnl, 
                                   AvrcpAdvancedPdu *cmd,
                                   U8 volume);


BtStatus AVRCP_CtAcceptAbsoluteVolume_Rsp(AvrcpChannel *chnl, 
                                   AvrcpAdvancedPdu *cmd,
                                   U8 volume);

#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED
/*---------------------------------------------------------------------------
 * AVRCP_CtGetCapabilities()
 *
 *     Get the capabilities of the target device.  The capabilities 
 *     supported by the target device may change.  If the target device 
 *     application changes to support less functionality, then errors 
 *     will begin to be received for capabilities that were previously 
 *     supported.  If this occurs, AVRCP_CtGetCapabilities() can be called
 *     again to get the most current capabilities. AVRCP_CtGetCapabilities()
 *     may be occasionally called to poll for capability changes.
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_GET_CAPABILITIES.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 * 
 *     cmd - A PDU for sending commands.
 *
 *     capabilityId - The specific capability that is being requested.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetCapabilities(AvrcpChannel         *chnl, 
                                 AvrcpAdvancedPdu     *cmd,
                                 AvrcpCapabilityId     capabilityId);

/*---------------------------------------------------------------------------
 * AVRCP_CtListPlayerSettingAttrs()
 *
 *     List the attributes for the settings of the current media 
 *     player on the target device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_LIST_PLAYER_SETTING_ATTRIBS.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtListPlayerSettingAttrs(AvrcpChannel     *chnl, 
                                        AvrcpAdvancedPdu *cmd);

/*---------------------------------------------------------------------------
 * AVRCP_CtListPlayerSettingValues()
 *
 *     List the set of possible values for the settings of the current media 
 *     player on the target device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_LIST_PLAYER_SETTING_VALUES.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     attrId - The player application setting attribute identification for 
 *         which the set of possible is to be listed.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtListPlayerSettingValues(AvrcpChannel      *chnl, 
                                         AvrcpAdvancedPdu  *cmd,
                                         AvrcpPlayerAttrId  attrId);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetPlayerSettingValues()
 *
 *     Get the indicated setting values for the current media player on the 
 *     target device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_GET_PLAYER_SETTING_VALUE.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     attrMask - Defines which attibutes are to be queried.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetPlayerSettingValues(AvrcpChannel          *chnl, 
                                        AvrcpAdvancedPdu      *cmd,
                                        AvrcpPlayerAttrIdMask  attrMask);

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
/*---------------------------------------------------------------------------
 * AVRCP_CtSetAbsoluteVolume()
 *
 *     Set the absolute volume on the target.  This function should be called
 *     only by the source (capturing) device, which is the opposite of
 *     almost all other controller commands.  This means that the local
 *     device may need to be registered as both a Category 1 Target and a
 *     Category 2 Controller.
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_SET_ABSOLUTE_VOLUME.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.
 * 
 *     cmd - A PDU for sending commands.
 * 
 *     volume - The absolute volume of the Sink (0 - 0x7F).
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or setting parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtSetAbsoluteVolume(AvrcpChannel *chnl, AvrcpAdvancedPdu *cmd,U8 volume);
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/*---------------------------------------------------------------------------
 * AVRCP_CtSetPlayerSettingValues()
 *
 *     Set the settings values of the current media player on 
 *     the target device.  
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_SET_PLAYER_SETTING_VALUE.
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     numSettings - The number of attribute ID and setting pair elements in 
 *         the array pointed at by the setting parameter.
 *
 *     setting - This value must point to an array of attribute ID and 
 *         setting pairs sized to contain numSettings elements.  The setting 
 *         pairs must be set to the desired setting values.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or setting parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtSetPlayerSettingValues(AvrcpChannel       *chnl, 
                                        AvrcpAdvancedPdu   *cmd,
                                        U8                  numSettings, 
                                        AvrcpPlayerSetting *setting);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetPlayerSettingAttrTxt()
 *
 *     Get the text for the setting attributes of the current media 
 *     player on the target device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_GET_PLAYER_SETTING_ATTR_TEXT.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     attrMask - Defines which attibutes are to be queried.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetPlayerSettingAttrTxt(AvrcpChannel          *chnl, 
                                         AvrcpAdvancedPdu      *cmd,
                                         AvrcpPlayerAttrIdMask  attrMask);

/* Forward reference */
BtStatus AvrcpCtGetPlayerSettingValueTxt(AvrcpChannel      *chnl, 
                                         AvrcpAdvancedPdu  *cmd,
                                         AvrcpPlayerAttrId  attrId,
                                         U16                NumValues);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetPlayerSettingValueTxt()
 *
 *     Get the text for the player setting values of the current media 
 *     player on the target device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_GET_PLAYER_SETTING_VALUE_TEXT.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     attrId -  The player application setting attribute identification for 
 *         which the set of possible is to be listed.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetPlayerSettingValueTxt(AvrcpChannel      *chnl, 
                                          AvrcpAdvancedPdu  *cmd,
                                          AvrcpPlayerAttrId  attrId);
#define AVRCP_CtGetPlayerSettingValueTxt(ch,cmd,att) AvrcpCtGetPlayerSettingValueTxt(ch,cmd,att,0)

/*---------------------------------------------------------------------------
 * AVRCP_CtGetExtPlayerSettingValueTxt()
 *
 *     Get the text for the player setting values of the current media 
 *     player on the target device.  This function may be used used to get
 *     any values, including those not defined by the AVRCP specification.
 *     For application extensions to the player settings, attribute IDs
 *     greater than 4 are used.
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_GET_PLAYER_SETTING_VALUE_TEXT.
 *
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     attrId -  The player application setting attribute identification for 
 *         which the set of possible is to be listed.
 * 
 *     NumValues - The number of player settings values supported by the
 *         player setting attribute.  This value is ignored for player
 *         settings attributes defined by the AVRCP specification (1 - 4).
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetExtPlayerSettingValueTxt(AvrcpChannel      *chnl, 
                                             AvrcpAdvancedPdu  *cmd,
                                             AvrcpPlayerAttrId  attrId,
                                             U16                NumValues);
#define AVRCP_CtGetExtPlayerSettingValueTxt(ch,cmd,att,num) AvrcpCtGetPlayerSettingValueTxt(ch,cmd,att,num)

/*---------------------------------------------------------------------------
 * AVRCP_CtInformCharset()
 *
 *     Send the controller's displayable character sets to the target 
 *     device.  When the target device receives this command, the target 
 *     device can begin sending strings in one of the character sets that was 
 *     specified by AVRCP_CtInformCharset().  By default target device sends
 *     strings in the UTF-8 format until AVRCP_CtInformCharset() has been
 *     invoked.
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_INFORM_DISP_CHAR_SET.
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     numCharsets - The number of displayable character set elements in the 
 *         array pointed at by the charsets parameter.  
 *
 *     charsets - This value must point to an array of character set 
 *         identifiers sized to contain numCharsets elements.  The character 
 *         set identifiers must be set to values supported by this device.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or charsets parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtInformCharset(AvrcpChannel     *chnl, 
                               AvrcpAdvancedPdu *cmd,
                               U8                numCharsets,
                               U16              *charsets);

/*---------------------------------------------------------------------------
 * AVRCP_CtInformBatteryStatus()
 *
 *     Send the controller's battery status to the target device.  
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_INFORM_BATT_STATUS.
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     battStatus - Must be set to an enumeration value corresponding to the 
 *         controller's current battery status.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtInformBatteryStatus(AvrcpChannel       *chnl, 
                                     AvrcpAdvancedPdu   *cmd,
                                     AvrcpBatteryStatus  battStatus);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetMediaInfo()
 *
 *     Get the media attributes for the current track on the current 
 *     media player on target device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_GET_MEDIA_INFO.
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     mediaMask - Defines which media attibutes are to be queried.  If set
 *                 to 0, then all attributes will be requested.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetMediaInfo(AvrcpChannel         *chnl, 
                              AvrcpAdvancedPdu     *cmd,
                              AvrcpMediaAttrIdMask  mediaMask);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetPlayStatus()
 *
 *     Get the play status of the current media player on the target 
 *     device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_GET_PLAY_STATUS.
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetPlayStatus(AvrcpChannel     *chnl,
                               AvrcpAdvancedPdu *cmd);

/*---------------------------------------------------------------------------
 * AVRCP_CtRegisterNotification()
 *
 *     Register for notification of events on the target device.  
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_REGISTER_NOTIFY.
 * 
 * Parameters:
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 *
 *     eventId - The event for which the CT requires notifications.  
 *
 *     interval - For the AVRCP_EID_PLAY_POS_CHANGED event, the interval 
 *         parameter specifies the time interval (in seconds) at which the 
 *         change in the playback position will be notified.  If the song is 
 *         being forwarded or rewind, a notification will be received 
 *         whenever the playback position will change by this value even if 
 *         the interval has not yet expired.  For other events, the value of 
 *         this parameter is ignored.  
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtRegisterNotification(AvrcpChannel         *chnl, 
                                      AvrcpAdvancedPdu     *cmd,
                                      AvrcpEventId          eventId,
                                      U32                   interval);

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
/*---------------------------------------------------------------------------
 * AVRCP_CtSetAddressedPlayer()
 *
 *     Set the addressed player on the target.
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_SET_ADDRESSED_PLAYER.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 * 
 *     playerId - Identifier of the player on the remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtSetAddressedPlayer(AvrcpChannel *chnl, 
                                    AvrcpAdvancedPdu *cmd,
                                    U16 playerId);

/*---------------------------------------------------------------------------
 * AVRCP_CtPlayItem()
 *
 *     Play an item on the target.
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_PLAY_ITEM.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 * 
 *     scope - The scope on the virtual file system.
 * 
 *     uid - The UID of the item to be played.
 * 
 *     uidCount - The UID count reported from the target.
 * 
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtPlayItem(AvrcpChannel     *chnl, 
                          AvrcpAdvancedPdu *cmd,
                          AvrcpScope        scope,
                          U8               *uid,
                          U16               uidCount);

/*---------------------------------------------------------------------------
 * AVRCP_CtAddToNowPlaying()
 *
 *     Add the specified item to the now playing list.
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_ADD_TO_NOW_PLAYING.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 * 
 *     scope - The scope on the virtual file system.
 * 
 *     uid - The UID of the item to be played.
 * 
 *     uidCount - The UID count reported from the target.
 * 
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started successfully. When
 *         the associated packet has been sent, the application callback will
 *         receive the AVRCP_EVENT_ADV_TX_DONE event.  When a response has
 *         been received from the remote device, a AVRCP_EVENT_ADV_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtAddToNowPlaying(AvrcpChannel     *chnl, 
                                 AvrcpAdvancedPdu *cmd,
                                 AvrcpScope        scope,
                                 U8               *uid,
                                 U16               uidCount);
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
/*---------------------------------------------------------------------------
 * AVRCP_CtSearch()
 *
 *     Search on the browsed target.
 *
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_SEARCH.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     cmd - A PDU for sending commands.
 * 
 *     charSet - The character set used by the search string.
 * 
 *     length - The length of the search string.
 * 
 *     searchSTr - A pointer to the search string.
 * 
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the
 *         AVRCP_EVENT_BROWSE_TX_DONE event.  When a response has been
 *         received from the remote device, a AVRCP_EVENT_BROWSE_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtSearch(AvrcpChannel     *chnl, 
                        AvrcpAdvancedPdu *cmd,
                        U16               charSet,
                        U16               length,
                        U8               *searchStr);

/*---------------------------------------------------------------------------
 * AVRCP_CtSetBrowsedPlayer()
 *
 *     Set the browsed player on the target.
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to
 *     AVRCP_OP_SET_BROWSED_PLAYER.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     pdu - A PDU for sending the browse command.
 * 
 *     playerId - Identifier for the player on the target that will be
 *         browsed by subsequent requests.
 * 
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the
 *         AVRCP_EVENT_BROWSE_TX_DONE event.  When a response has been
 *         received from the remote device, a AVRCP_EVENT_BROWSE_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtSetBrowsedPlayer(AvrcpChannel *chnl, 
                                  AvrcpAdvancedPdu *pdu, 
                                  U16 playerId);

/*---------------------------------------------------------------------------
 * AVRCP_CtChangePath()
 *
 *     Change the current path on the browsed target.
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to 
 *     AVRCP_OP_CHANGE_PATH.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     pdu - A PDU for sending the browse command.
 * 
 *     uidCounter - The UID count reported from the target.
 * 
 *     direction - Direction for the change path command (up or down)
 * 
 *     uid - The UID of the directory when direction is down.  Otherwise,
 *         this is ignored and may bet set to 0.
 * 
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the
 *         AVRCP_EVENT_BROWSE_TX_DONE event.  When a response has been
 *         received from the remote device, a AVRCP_EVENT_BROWSE_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtChangePath(AvrcpChannel *chnl, 
                            AvrcpAdvancedPdu *pdu, 
                            U16 uidCounter, 
                            AvrcpChgPathDirection direction, 
                            U8 *uid);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetItemAttributes()
 *
 *     Get the attributes of the specified item on the browsed target.
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to 
 *     AVRCP_OP_GET_ITEM_ATTRIBUTES.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     pdu - A PDU for sending a browse command.
 * 
 *     uidCounter - The UID count reported from the target.
 * 
 *     uid - The UID of the item.
 * 
 *     scope - The scope on the virtual file system.
 * 
 *     numAttrs - The number of attributes being requested.  If 0 is
 *                specified, then all attributes will be returned.
 * 
 *     attrList - The list of attributes being requested.  If numAttrs is 0,
 *                then this value can be NULL.
 * 
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the
 *         AVRCP_EVENT_BROWSE_TX_DONE event.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetItemAttributes(AvrcpChannel *chnl,
                                   AvrcpAdvancedPdu *pdu,
                                   U16 uidCounter, 
                                   U8 *uid, 
                                   AvrcpScope scope,
                                   U8 numAttrs, 
                                   AvrcpMediaAttrId *attrList);

/*---------------------------------------------------------------------------
 * AVRCP_CtGetFolderItems()
 *
 *     Get items from the current folder on the browsed target.  Only the
 *     number of items that will fit in a single L2CAP MTU will be returned.
 *     The response will contain the number of items returned.  To get more
 *     items, subsequent requests must be made.
 * 
 *     When the final response event is received (AVRCP_EVENT_ADV_RESPONSE),
 *     the 'advOp' field in the callback parameters will set to 
 *     AVRCP_OP_GET_FOLDER_ITEMS.
 * 
 * Parameters:
 * 
 *     chnl - A registered and open AVRCP channel.  
 *
 *     pdu - A PDU for sending a browse command.
 * 
 *     scope - The scope on the virtual file system.
 * 
 *     start - The starting item in the current directory to be returned.
 * 
 *     end - The ending item in the current directory to be returned.
 * 
 *     numAttrs - The number of attributes being requested.
 * 
 *     attrList - The list of attributes being requested.
 * 
 * Returns:
 *     BT_STATUS_PENDING - The operation has been started 
 *         successfully. When the associated packet has been sent, 
 *         the application callback will receive the
 *         AVRCP_EVENT_BROWSE_TX_DONE event.  When a response has been
 *         received from the remote device, a AVRCP_EVENT_BROWSE_RESPONSE
 *         event will be received.
 *
 *     BT_STATUS_IN_USE - The 'cmd' packet was in use and cannot be sent.
 *
 *     BT_STATUS_INVALID_PARM - The operation failed, because the 
 *         chnl or cmd parameter is invalid.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.  
 */
BtStatus AVRCP_CtGetFolderItems(AvrcpChannel *chnl, 
                                AvrcpAdvancedPdu *pdu,
                                AvrcpScope scope,
                                U32 start, 
                                U32 end, 
                                U8 numAttrs, 
                                AvrcpMediaAttrId *attrList);

/*---------------------------------------------------------------------------
 * AVRCP_CtParseNextFolderItem()
 *
 *     Parse folder item response.  This function can be used to parse a list
 *     of folder items in a GetFolderItem response (see the
 *     'p.adv.browseRsp.fldrItems.list' parameter of the
 *     AVRCP_OP_GET_FOLDER_ITEMS operation). The 'list' parameter should
 *     point to the beginning of the list on the first call. Each call will
 *     return a folder item into the 'mediaItem' structure. The function will
 *     return a pointer to the next folder item, which should be passed into
 *     the 'list' parameter on the next call. This function should only be
 *     called once for each item in the list. The number of items in the list
 *     is returned with the AVRCP_OP_GET_FOLDER_ITEMS operation in the
 *     'p.adv.browseRsp.fldrItems.numItems' parameter.
 * 
 * Parameters:
 * 
 *     list - A pointer to a list of folder items that were returned from the
 *         browsed target.
 * 
 *     mediaItem - A pointer to memory where the parsed results will be
 *         placed.
 * 
 * Returns:
 *     A pointer to the next item in the list.
 */
U8 * AVRCP_CtParseNextFolderItem(U8  *list, AvrcpMediaItem *mediaItem);

/*---------------------------------------------------------------------------
 * AVRCP_CtParseNextAttribute()
 *
 *     Parse an attribute list.  This function can be used to parse a list of
 *     attributes in a GetItemAttributes response (see the
 *     'p.adv.browseRsp.itemAttrs.list' parameter of the
 *     AVRCP_OP_GET_ITEM_ATTRIBUTES operation). The 'list' parameter should
 *     point to the beginning of the list on the first call.  Each call will
 *     return a folder item into the 'mediaAttr' structure. The function will
 *     return a pointer to the next attribute, which should be passed into
 *     the 'list' parameter on the next call. This function should only be
 *     called once for each attribute in the list. The number of attributes
 *     in the list is returned with the AVRCP_OP_GET_ITEM_ATTRIBUTES
 *     operation in the 'p.adv.browseRsp.itemAttrs.numAttrs' parameter.
 * 
 * Parameters:
 * 
 *     list - A pointer to a list of folder items that were returned from the
 *         browsed target.
 * 
 *     mediaAttr - A pointer to memory where the parsed results will be
 *         placed.
 * 
 * Returns:
 *     A pointer to the next item in the list.
 */
U8 * AVRCP_CtParseNextAttribute(U8 *list, AvrcpMediaAttribute *mediaAttr);

/*---------------------------------------------------------------------------
 * AVRCP_CtParseNextFolderName()
 *
 *     Parse a list of folder names. This function can be used to parse a
 *     list of attributes in a SetBrowsedPlayer response (see the
 *     'p.adv.browseRsp.brwsPlayer.list' parameter of the
 *     AVRCP_OP_SET_BROWSED_PLAYER operation). The 'list' parameter should
 *     point to the beginning of the list on the first call. Each call will
 *     copy a folder name into the 'folderName' structure. The function will
 *     return a pointer to the next folderName, which should be passed into
 *     the 'list' parameter on the next call. This function should only be
 *     called once for each attribute in the list. The number of folder names
 *     in the list is returned with the AVRCP_OP_SET_BROWSED_PLAYER
 *     operation in the 'p.adv.browseRsp.brwsPlayer.fDepth'parameter.
 * 
 * Parameters:
 * 
 *     list - A pointer to a list of folder names were returned
 *         from the AVRCP_OP_SET_BROWSED_PLAYER operation.
 * 
 *     folderName - A pointer to memory where the parsed results
 *         will be copied.
 * 
 * Returns:
 *     A pointer to the next item in the list.
 */
U8 * AVRCP_CtParseNextFolderName(U8 *list, U16 length, U8 *folderName);


#endif /* AVRCP_BROWSING_CONTROLLER == XA_ENABLED */
#endif /* AVRCP_ADVANCED_CONTROLLER == XA_ENABLED */

#endif /* __AVRCP_H_ */ 

