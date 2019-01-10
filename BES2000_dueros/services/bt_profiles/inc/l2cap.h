/****************************************************************************
 *
 * File:
 *     $Id: l2cap.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Logical Link Control and Adaptation Protocol.
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

#ifndef __L2CAP_H
#define __L2CAP_H

#include "osapi.h"
#include "bttypes.h"
#include "me.h"

/*---------------------------------------------------------------------------
 * Logical Link Control and Adaptation Protocol (L2CAP) layer
 *
 *     This prerelease includes support for the new Enhanced L2CAP
 *     modes defined in the Bluetooth v2.1 Core Specification
 *     Addendum. This prerelease is provided for an early look at
 *     these new guaranteed delivery and reliability features as we
 *     enhance upper layer profiles to take advantage of them.
 *
 *     The L2CAP layer handles all Bluetooth data communications that
 *     take place over ACL links. It is primarily a protocol multiplexer,
 *     allowing protocols such as RFCOMM, TCS Binary, and SDP to share
 *     a single device-to-device connection. The L2CAP layer also allows
 *     a higher layer protocol to communicate with several other devices
 *     over separate "channels".
 *     
 *     L2CAP supports segmentation and reassembly, which is the process
 *     of breaking down transmit packets into an appropriate size for
 *     HCI, and reassembling received packets for higher layers. The
 *     Maximum Transmission Unit (MTU) represents the largest possible
 *     reassembled L2CAP packet.
 *
 *     L2CAP handles channel configuration. This includes settings
 *     for the default L2CAP payload size, timeouts for missing data,
 *     and "quality of service" requirements for data rate, peak bandwidth,
 *     latency, and delay.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * L2CAP_DEFAULT_MTU constant
 *
 *     Defines the MTU size assumed for devices that do not specify an MTU
 *     size explicitly. This value is defined by the Bluetooth
 *     specification as 672 bytes.
 */
#define L2CAP_DEFAULT_MTU           0x02A0


/*---------------------------------------------------------------------------
 * L2CAP_MINIMUM_MTU constant
 *
 *     Defines the minimum allowed MTU size for all L2CAP implementations.
 *     This value is defined by the Bluetooth specification as 48 bytes.
 */
#define L2CAP_MINIMUM_MTU           0x0030


/*---------------------------------------------------------------------------
 * L2CAP_MAXIMUM_MTU constant
 *
 *     Defines the maximum receive SDU size allowed for this L2CAP
 *     implementation. This is based on the L2CAP_MTU configuration value.
 *     Note that this value may be less than the L2CAP_DEFAULT_MTU value
 *     to save RAM.
 */
#define L2CAP_MAXIMUM_MTU           L2CAP_MTU


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * L2capEventType type
 *
 *     Indicates the type of an L2CAP callback's event during an
 *     L2capCallback function call. These events are delivered in the
 *     L2capCallbackParms structure's "event" field.
 *
 *     Depending on the event, a specific response may be required or
 *     certain fields in the L2capCallbackParms structure may be valid or
 *     invalid. The valid fields and required responses are described
 *     for each event below.
 */
typedef U8 L2capEventType;

/** A remote device has requested a new L2CAP connection (channel). The
 * L2CAP user must call L2CAP_ConnectRsp to confirm or deny the connection 
 * request. 
 */
#define L2EVENT_CONNECT_IND      1

/** The channel is connected and ready to exchange data. This event occurs
 *  regardless of which device initiated the connection.
 */
#define L2EVENT_CONNECTED        2

/** The remote device requested that its channel be disconnected. L2CAP
 * will respond with a disconnection response, and send the DISCONNECTED 
 * event when the disconnection process is complete. 
 */
#define L2EVENT_DISCON_IND       3

/** The channel is disconnected. The callback's "status" field contains
 * the reason for the disconnection. 
 */
#define L2EVENT_DISCONNECTED     4

/** Channel data has been received. The callback "ptrs.data" field
 * contains the received data, and "dataLen" indicates the data length. 
 */
#define L2EVENT_DATA_IND         5

/** A data packet has been sent. The application may re-use or deallocate
 * the packet in the "ptrs.packet" field. The status of the transmit is 
 * provided in the "status" field of the callback parameters structure.
 */
#define L2EVENT_PACKET_HANDLED   6

/** A request is complete. Different L2CAP operations may generate a
 * COMPLETE event, depending on configuration options. (See L2CAP_Ping, 
 * L2CAP_GetInfo, and L2CAP_ConnectRsp for more information.) 
 *
 * During this event, the "status" field contains the results of the
 * request.
 */
#define L2EVENT_COMPLETE         7

/** A Quality-of-Service violation was detected. It is up to the
 * application to take any action based on this event.
 */
#define L2EVENT_QOS_VIOLATION    8

/** The Channel configuration parameters are being negotiated. This event
 * is generated when the channel has auto-configuration disabled and
 * enters the configuration state. It is also generated when a connected
 * channel enters renegotiation as initiated by the remote device. No
 * data can be sent while a channel is negotiating is configuration.
 * Channels which have auto-configuration disabled should send a 
 * configuration request upon receiving this event.
 */
#define L2EVENT_CONFIGURING      9

/** If a channel undergoes configuration renegotiation (as indicated
 * by the L2EVENT_CONFIGURING event) this event will be indicated
 * once the configuration negotiation is complete. This event is roughly
 * equivalent to L2EVENT_CONNECTED, and indicates that data transmission
 * can now be resumed.
 */
#define L2EVENT_RECONFIGURED    10

/* Group: The following events do not occur unless L2CAP_FLEXIBLE_CONFIG
 * is enabled and channel auto-configuration is disabled.
 */
#if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED

/** A configuration request has been received from the remote device. The
 * options and configuration flags are available in the L2capCallbackParms 
 * "ConfigInd" structure. 
 *
 * This event is only received for CIDs with "auto config" disabled (via 
 * L2CAP_SetAutoConfigMode). After this event is received, 
 * L2CAP_SendConfigRsp must be called to indicate a configuration 
 * response. 
 *
 * If a configure request is received with the more (continue) flag set, 
 * the response must also specify this flag. The remote side will then 
 * send additional request(s) with additional options. 
 */
#define L2EVENT_CONFIG_IND      11

/** A configuration response has been received from the remote device.
 * The options and configuration flags are available in the L2capCallbackParms
 * "ConfigCnf" structure. If a configure response is received with the more
 * (continue) flag set, the local side must issue a Configure Request to
 * solicit another response from the remote device. This process continues
 * until a response without the more flag is received.
 */
#define L2EVENT_CONFIG_CNF      12

#endif /* L2CAP_FLEXIBLE_CONFIG */

/* Group: The following events do not occur unless L2CAP_NUM_ENHANCED_CHANNELS
 * are configured and L2CAP_ERTM_FRAGMENTS is enabled.
 */

/** Fragmented channel data has been received. The callback
 * "ptrs.data" field contains the received data, "dataLen" field
 * indicates the data length of this fragment, and "sduLen" 
 * field indicates total length of all fragments. 
 */
#define L2EVENT_FRAGMENT_IND    13

/** The last fragment of fragmented channel data has been
 * received. The callback "ptrs.data" field contains the 
 * received data, "dataLen" field indicates the data length of 
 * this fragment, and "sduLen" field indicates total length of 
 * all fragments. 
 */
#define L2EVENT_LAST_FRAG_IND   14

/** The Flush Timeout command was sent to HCI. The "flushTimeout" field
 * indicates the flush timeout value used. The BTEVENT_HCI_COMMAND_SENT 
 * event will confirm success or failure of this command. 
 */
#define L2EVENT_FLUSH_TIMEOUT   15

/* End of L2capEventType */


/*---------------------------------------------------------------------------
 * L2capDiscReason type
 *
 *     Indicates the reason for a disconnection. The reason is delivered
 *     during an L2EVENT_DISCONNECTED callback, and can be found in the
 *     L2capCallbackParms "status" field.
 */
typedef U16 L2capDiscReason;

/* Disconnection occurred for an unknown reason. */
#define L2DISC_REASON_UNKNOWN      0x0000 

/* The local or remote device requested the disconnection. */
#define L2DISC_USER_REQUEST        0x0001

/* An L2CAP request timed out. */
#define L2DISC_REQUEST_TIMEDOUT    0x0002

/* The underlying ACL Link was disconnected. */
#define L2DISC_LINK_DISCON         0x0003

/* The connection was terminated due to a QOS violation. */
#define L2DISC_QOS_VIOLATION       0x0004

/* The local security manager refused the connection attempt. */
#define L2DISC_SECURITY_BLOCK      0x0005

/* The remote device does not support the requested protocol service (PSM). */
#define L2DISC_CONN_PSM_UNSUPP     0x0042

/* The remote device's security architecture denied the connection. */
#define L2DISC_CONN_SECURITY_BLOCK 0x0043

/* The remote device is out of resources. */
#define L2DISC_CONN_NO_RESOURCES   0x0044

/* Configuration failed due to unacceptable parameters. */
#define L2DISC_CONFIG_UNACCEPT     0x0081

/* Configuration was rejected (unknown reason). */
#define L2DISC_CONFIG_REJECT       0x0082

/* Configuration failed due to an unrecognized configuration option. */
#define L2DISC_CONFIG_OPTIONS      0x0083

/* The maximum number of retransmissions has been exceeded (BT1.2 mode only). */
#define L2DISC_MAX_RETRANS         0x0084

/* The connection was terminated due to a protocol violation. */
#define L2DISC_PROTOCOL_ERROR      0x0085

/* End of L2capDiscReason */


/*---------------------------------------------------------------------------
 * L2capConnStatus type
 *
 *     Indicates the status of a requested connection. When the L2CAP
 *     user receives the L2EVENT_CONNECT_IND event, it must call
 *     L2CAP_ConnectRsp to provide one of the status codes below.
 */
typedef U16 L2capConnStatus;

/* The connection request is accepted. */
#define L2CONN_ACCEPTED                     0x0000

/* The connection request is pending. This might be necessary if some
 * user action is required to approve the connection.
 *
 * After the pending indication is delivered to the remote device, the local
 * protocol will receive an L2EVENT_COMPLETE event. After this event occurs
 * the protocol may re-issue the L2CAP_ConnectRsp call with one of the
 * other connection status values.
 */
#define L2CONN_PENDING                      0x0001

/* Rejects the connection request because no such protocol service exists.
 * Generally, this rejection status should only be originated by L2CAP,
 * not by protocol services.
 *
 * The L2EVENT_DISCONNECTED event will be generated after the rejection
 * has been sent.
 */
#define L2CONN_REJECT_PSM_NOT_SUPPORTED     0x0002

/* Rejects the connection request due to a security failure. The
 * L2EVENT_DISCONNECTED event will be generated after the rejection has
 * been sent.
 */
#define L2CONN_REJECT_SECURITY_BLOCK        0x0003

/* Rejects the connection request due to a resource shortage. The
 * L2EVENT_DISCONNECTED event will be generated after the rejection has
 * been sent.
 */
#define L2CONN_REJECT_NO_RESOURCES          0x0004

/* End of L2capConnStatus */


/*---------------------------------------------------------------------------
 * L2capPsmValue type
 *
 *     Defines well-known protocol service values. These values identify
 *     protocol services that use L2CAP. When creating an L2capPsm structure
 *     for registration with L2CAP_RegisterPsm, the "psm" field must be
 *     set to one of these values.
 *
 *     Dynamically assigned PSM values are also permitted; however, they
 *     must be odd, within the range of 0x1001 to 0xFFFF, and have
 *     the 9th bit (0x0100) set to zero.
 */
typedef U16 L2capPsmValue;

/* PSMs of this type cannot receive a connection. Only clients establishing
 * outbound L2CAP connections can use it.
 */
#define BT_CLIENT_ONLY_PSM      0x0000

/* Value for a Service Discovery Protocol server */
#define BT_PSM_SDP              0x0001

/* Value for an RFCOMM server */
#define BT_PSM_RFCOMM           0x0003

/* Value for a TCS Binary server */
#define BT_PSM_TCS              0x0005

/* Value for a TCS Binary group */
#define BT_PSM_TCS_CORDLESS     0x0007

/* Value for the BNEP service */
#define BT_PSM_BNEP             0x000F

/* Value for the HID Control Channel */
#define BT_PSM_HID_CTRL         0x0011

/* Value for the HID Interrupt Channel */
#define BT_PSM_HID_INTR         0x0013

/* Value for the UPnP/ESDP service */
#define BT_PSM_UPNP             0x0015

/* Value for the A/V Control Transport Protocol signal channel */
#define BT_PSM_AVCTP            0x0017

/* Value for the A/V Distribution Transport protocol */
#define BT_PSM_AVDTP            0x0019

/* Value for the A/V Control Transport Protocol browsing channel*/
#define BT_PSM_AVCTP_BROWSING   0x001B

/* Value for Unrestricted Digital Information Control Plane protocol */
#define BT_PSM_UDI_C            0x001D

/* Servers that use dynamically allocated PSMs should register using
 * this value. The L2CAP_RegisterPsm function will assign a dynamic
 * PSM value and return it in the "psm" field.
 */
#define BT_DYNAMIC_PSM          0x1101

/* End of L2capPsmValue */


/*---------------------------------------------------------------------------
 * L2capInfoType type
 *
 *     Defines the types of information that can be queried from a remote
 *     L2CAP entity with L2CAP_GetInfo.
 */
typedef U16 L2capInfoType;

#define L2INFO_CONNECTIONLESS_MTU   0x0001

#define L2INFO_EXTENDED_FEATURES    0x0002

/* End of L2capInfoType */


/*---------------------------------------------------------------------------
 * L2capConfigFlags type
 * 
 */
typedef U16 L2capConfigFlags;

/* No flags. */
#define L2CFG_FLAG_NONE     0x0000

/* More/Continuation bit in configuration packets. */
#define L2CFG_FLAG_MORE     0x0001

/* End of L2capConfigFlags */

/*---------------------------------------------------------------------------
 * L2capConfigResult type
 * 
 *     Describes the result of configuration negotiation. This type is
 *     indicated during L2EVENT_CONFIG_CNF events. It is also used when
 *     sending a L2CAP Configure Response.
 */
typedef U16 L2capConfigResult;

/* Requested parameters are acceptable */
#define L2CFG_RESULT_ACCEPTED      0x0000

/* Requested parameters are unacceptable */
#define L2CFG_RESULT_UNACCEPTABLE  0x0001

/* Request rejected - unspecified reason */
#define L2CFG_RESULT_REJECTED      0x0002

/* Unknown option(s) were present */
#define L2CFG_RESULT_UNKN_OPTIONS  0x0003   

/* End of L2capConfigResult */


/*---------------------------------------------------------------------------
 * L2capConfigOption type
 *
 *     Describes the type of a configuration parameter option. Configuration
 *     parameters are accessible during a L2EVENT_READ_CONFIG_OPTION callback
 *     by calling the L2CAP_ReadOption function.
 *
 *     The buffer returned by L2CAP_ReadOption contains a set of
 *     configuration parameters. Each parameter begins with an
 *     L2capConfigOption type, a one-byte length, and option-specific data.
 *
 */
typedef U8 L2capConfigOption;

/* Option type for the remote device's maximum payload size (maximum
 * transmission unit). The length field will indicate 2 bytes, and be
 * followed by the 16-bit MTU size.
 */
#define L2CFG_OPTION_MTU            0x01

/* Option type for the remote device's flush timeout option. This is the
 * amount of time the remote device will attempt to retransmit before
 * giving up and flushing the data. The length field will indicate 2 bytes,
 * and be followed by the flush timeout value.
 */
#define L2CFG_OPTION_FLUSH_TIMEOUT  0x02

/* Option type for the remote device's quality of service parameters.
 * This option corresponds with the guarantees that the remote device
 * will provide. See the Bluetooth L2CAP specification for more information.
 */
#define L2CFG_OPTION_QOS            0x03

/* Option type for the remote device's Enhanced L2CAP modes.
 * This option specifies the link modes supported by L2CAP such 
 * as Enhanced Retransmission mode or Streaming mode plus related 
 * configuration parameters. 
 */
#define L2CFG_OPTION_FLOW_CONTROL   0x04

/* Option type for the remote device's Frame Check Sequence (FCS) type
 * when in Enhanced Retransmission or Streaming modes.
 */
#define L2CFG_OPTION_FCS            0x05

/* End of L2capConfigOption */

/*---------------------------------------------------------------------------
 * L2capQosServiceType type
 *
 *      Indicates the quality of service (QOS) guaranteed by a device.
 *      This type is used when reading a L2CFG_OPTION_QOS configuration
 *      option parameter (see L2capConfigOption).
 */
typedef U8 L2capQosServiceType;

/* No outgoing data will be sent so QOS parameters are unnecessary */
#define L2QOS_OPTION_NO_TRAFFIC       0x00

/* The device cannot guarantee the QOS parameters, but will make its
 * "best effort" to honor them.
 */
#define L2QOS_OPTION_BEST_EFFORT      0x01

/* The device guarantees that it will operate within the QOS parameters
 * provided.
 */
#define L2QOS_OPTION_GUARANTEED       0x02

/* End of L2capQosServiceType */


/*---------------------------------------------------------------------------
 * L2capLinkMode type
 *
 *     Indicates the L2CAP Link modes supported by the channel. This type
 *     is used when reading a L2CFG_OPTION_FLOW_CONTROL configuration option
 *     flags parameter.
 */
typedef U8 L2capLinkMode;

/* Bluetooth v1.1 mode, no Enhanced L2CAP modes supported. */
#define L2MODE_BASIC                0x01

/* This mode is not supported. It has been replaced by L2MODE_ERTM. */
#define L2MODE_RETRANSMISSION       0x02

/* This mode is not supported. It has been replaced by L2MODE_ERTM. */
#define L2MODE_FLOW_CONTROL         0x04

/* Enhanced Retransmission of L2CAP frames is enabled. In
 * enhanced retransmission mode, flow control is utilized on the 
 * connection and frames may be selectively retransmitted as 
 * part of an error recovery procedure. 
 */
#define L2MODE_ERTM                 0x08

/* Streaming of L2CAP frames is enabled. In streaming mode,
 * there is no retransmission and a finite flush timeout is 
 * utilized to keep the transmission stream current. 
 */
#define L2MODE_STREAMING            0x10

/* End of L2capLinkMode */


/*---------------------------------------------------------------------------
 * L2capChannelId type
 *
 *     Identifies a particular channel. Channel ID values are assigned
 *     by L2CAP to uniquely identify a connection to a remote device,
 *     using a specific higher-layer protocol. Channel IDs may also
 *     identify a connectionless group of devices.
 */
typedef U16 L2capChannelId;

/* Used to specify an invalid L2CAP channel ID. */
#define L2CID_INVALID        0x0000

/* End of L2capChannelId */


/*---------------------------------------------------------------------------
 * L2ExtendedFeatures type
 *
 *     Defines the L2CAP Extended Feature Mask values found in an L2CAP 
 *     Information Response. The response value is sent in Little Endian and
 *     may need conversion before being compared with the mask values. If a
 *     bit is set in the response, the corresponding feature is present.
 */
typedef U32 L2ExtendedFeatures;

/* L2CAP Flow Control is supported */
#define L2EXTENDED_FEATURE_FLOW_CONTROL         0x00000001

/* L2CAP Retransmission is supported */
#define L2EXTENDED_FEATURE_RETRANSMISSIONS      0x00000002

/* L2CAP bi-directional QoS is supported */
#define L2EXTENDED_FEATURE_BIDIRECTIONAL_QOS    0x00000004

/* L2CAP Enhanced Retransmission is supported */
#define L2EXTENDED_FEATURE_ERTM                 0x00000008

/* L2CAP Streaming is supported */
#define L2EXTENDED_FEATURE_STREAMING            0x00000010

/* L2CAP FCS Options are supported */
#define L2EXTENDED_FEATURE_FCS                  0x00000020

/* End of L2ExtendedFeatures */

#if L2CAP_NUM_ENHANCED_CHANNELS > 0

/*---------------------------------------------------------------------------
 * L2capFcsOption type
 * 
 */
typedef U16 L2capFcsOption;

/* No FCS */
#define L2FCS_NONE          0x01

/* 16-bit FCS (default) */
#define L2FCS_16BIT         0x02

/* Require 16-bit FCS */
#define L2FCS_16BIT_ONLY    0x04

/* End of L2capFcsOption */

#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */


/* Forward reference for the L2capCallback type */
typedef struct _L2capCallbackParms L2capCallbackParms;

/*---------------------------------------------------------------------------
 * L2capCallback type
 *
 *     A function of this type is called to indicate L2CAP events to
 *     a protocol service. When L2CAP calls this function, it provides
 *     the channel ID that received the event and additional information
 *     in the "info" parameter.
 *
 *     Protocol services provide the callback function's address to
 *     L2CAP by calling the L2CAP_RegisterPsm function.
 */
typedef void (*L2capCallback)(L2capChannelId cid, L2capCallbackParms *Info);

/* End of L2capCallback */


/*---------------------------------------------------------------------------
 * L2CAP Security Id
 *
 * Security ID for L2CAP access.
 */
 #if BT_SECURITY == XA_ENABLED
#define SEC_L2CAP_ID    (BtProtocolId)L2Cap_SecAccessRsp
#else
#define SEC_L2CAP_ID    (BtProtocolId)(0)
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * L2capCallbackParms structure
 *
 *     Provides information to the L2CAP user about an L2capCallback
 *     event. Different fields are valid depending on the type of
 *     the event, indicated by the "event" field. See L2capEventType for
 *     more information about specific callback events.
 *     
 *     The L2CAP user should not modify elements in this structure.
 *     Unless otherwise noted, the callback structure and any data pointers
 *     it provides should be considered invalid after the callback returns.
 */  
struct _L2capCallbackParms
{
    /* L2CAP upper layer event. Always valid. */
    L2capEventType      event;

    /* Completion status, based on the event type. Possible types are
     * L2capDiscReason for L2EVENT_DISCONNECTED events or BtStatus for
     * most others. */
    U16                 status;

    /* Device that originated the event or data. Always valid. */
    BtRemoteDevice     *aclLink;

    union {
        /* Pointer to the PSM structure passed in to L2CAP_RegisterPsm */
        const struct _L2capPsm     *psm;

        /* Pointer to the group structure passed in to L2CAP_RegisterGroup */
        const struct _L2capGroup   *group;
    } owner;

    /* Length of data in "ptrs.data", during L2EVENT_DATA_IND events. */
    U16                 dataLen;

    /* Total length of fragments in fragmented data indications.
     * Valid only during L2EVENT_FRAGMENT_IND and 
     * L2EVENT_LAST_FRAG_IND events. 
     */ 
    U16                 sduLen;

    union {
        /* Packet data, valid only during L2EVENT_PACKET_HANDLED callback
         * events. */
        BtPacket       *packet;

        /* During a L2EVENT_DATA_IND callback event this points to the data received
         * by the channel or group. 
         * During a L2EVENT_COMPLETE callback event for an L2CAP_GetInfo() call,
         * this points to the requested information.
         * During L2EVENT_COMPLETE events for an L2CAP_Ping() call, this points
         * to any data provided in the echo response.
         */
        U8             *data;

        /* The Flush Timeout value sent to HCI indicated in an
         * L2EVENT_FLUSH_TIMEOUT event.
         */
        U16             flushTimeout;

#if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
        struct _ConfigInd {
            U8                  ident;
            L2capConfigFlags    flags;
            U8                 *options;
            U16                 optionsLen;
        } configInd;

        struct _ConfigCnf {
            U8                  ident;
            L2capConfigFlags    flags;
            U16                 result;
            U8                 *options;
            U16                 optionsLen;
        } configCnf;
#endif
    } ptrs;
};


#if L2CAP_NUM_ENHANCED_CHANNELS > 0
/*---------------------------------------------------------------------------
 * L2capFlowParms structure
 *
 *     Specifies the Enhanced L2CAP mode parameters for the PSM or Channel.
 */  
typedef struct _L2capFlowParms
{
    /* Requested operating modes for channels opened for this PSM. For
     * maximum compatiblity, this should include L2MODE_BASIC. If basic is
     * not included, connections to Bluetooth v1.1 devices will fail. An
     * L2CAP Information request may be issued to retrieve the extended
     * features mask to ascertain the remote devices capabilities. If the
     * PSM does not permit Enhanced L2CAP modes added in the Bluetooth v2.1
     * addendum, set this parameter to L2MODE_BASIC.
     *
     * The inbound and outbound link modes must be symmetric. For example:
     * if inbound packets can use Enhanced Retranmission mode, outbound data 
     * must use Enhanced Retransmission mode also. If the link is configured 
     * to use basic mode in one direction, it must use basic mode in the other 
     * direction as well. 
     */

    /* Specify the acceptable modes for the link. Both incoming and outgoing
     * links have the same mode since supported link modes are symmetric.
     */
    L2capLinkMode   linkMode;

    /* Specify the preferred link mode for the link. L2CAP will begin
     * negotiating the channel configuration with this mode if more than
     * one mode is specified in "linkMode".
     */
    L2capLinkMode   prefLinkMode;

    /* The size of the send window for retransmission modes. */
    U8              txWindow;

    /* The size of the receive window for retransmission modes. */
    U8              rxWindow;

    /* The maximum number of times the remote L2CAP can transmit a packet
     * before considering the channel lost. The minimum value of 1, indicates
     * no retransmissions shall be sent. Note that if the retransmit timer
     * expires and the maxTransmit value has been reached, the channel will
     * be disconnected. This value is not used by basic mode connections.
     */
    U8              maxTransmit;

    /* The retransmission timeout to be used by the remote device when 
     * retransmission mode is enabled. This timeout should be set fairly
     * high in order to avoid unnecessary retransmissions due to delayed
     * acknowledgements. Suitable values, specified in milliseconds, could
     * be 100’s of milliseconds or higher. This value is not used by basic
     * mode connections.
     */
    U16             retransTimeout;

    /* The monitor timeout to be used by the remote device when no activity
     * is present on the channel. It serves to ensure that lost acknowledgements
     * are retransmitted. Suitable values, specified in milliseconds, could
     * be 100’s of milliseconds or higher. This value is not used by basic
     * mode connections.
     */
    U16             monitorTimeout;

    /* The FCS Option allows configuration of a channel with a preference
     * for no Frame Check Sequence (FCS) in the L2CAP packet. If this option
     * is L2FCS_NONE, the configuration will specify no FCS. Note that if either
     * side of the link requests FCS in a configuration request, then
     * FCS will be used for the link.
     */
    L2capFcsOption  fcsOption;

    /* The txMps defines the maximum PDU size the local channel can send.
     * Each channel specifies this value separately. Note that L2CAP will
     * adjust this to be less than or equal to the remote device's rxMps.
     */
    U16             txMps;

    /* The rxMps defines the maximum PDU size the local channel can accept.
     * Each channel specifies this value separately.
     */
    U16             rxMps;

} L2capFlowParms;
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */


/*---------------------------------------------------------------------------
 * L2capPsm structure
 *
 *     Represents a protocol service that uses L2CAP. Any higher-layer
 *     protocol or application that will use L2CAP's connection-oriented
 *     services must allocate and completely fill out this structure.
 *     The structure is then provided to L2CAP by calling L2CAP_RegisterPsm.
 *
 *     For an L2CAP user who will only call L2CAP_Ping and L2CAP_GetInfo,
 *     only the "callback" field is required to be filled out. Connection
 *     oriented services may use L2CAP_Ping and L2CAP_GetInfo as well.
 */
typedef struct _L2capPsm
{
    /* Callback to invoke whenever an event occurs. */
    L2capCallback   callback;
   
    /* Identifier for the protocol service. */
    L2capPsmValue   psm;
                                    
    /* Group: The following parameters specify the connection settings used
     * to configure channels opened on this PSM. L2CAP will automatically 
     * negotiate the connection configuration with the remote peer. Channels 
     * that require control over their configuration process should use the 
     * L2CAP_ConfigReq and L2CAP_ConfigRsp functions to execute their own 
     * configuration process. 
     */

    /* Maximum receive MTU for this service. Should be equal to or less 
     * than L2CAP_MAXIMUM_MTU. This is the MTU; which will be equivalent
     * to the higher layer SDU size only in Basic L2CAP mode.
     */
    U16             localMtu;

    /* Minimum acceptable value for remote device's receive MTU size. Aka, 
     * local transmit MTU. Should be no less than L2CAP_MINIMUM_MTU. 
     */
    U16             minRemoteMtu;

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    /* Default Enhanced L2CAP mode parameters for PSM. These can be
     * overriden when a channel is established (see L2capConnSettings.)
     */
    L2capFlowParms  flowParms;
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */

} L2capPsm;


/*---------------------------------------------------------------------------
 * L2capConnSettings structure
 *
 *     Provides connection settings to L2CAP during connection establishment.
 */
typedef struct _L2capConnSettings {

   /* Toggle the auto configuration mode either on or off for this
    * connection. By default auto configuration is on. When auto 
    * configuration is on, L2CAP will manage the connection configuration 
    * process. When it's off, the L2CAP user is responsible for sending and 
    * processing configuration request and response packets. This setting is 
    * valid only when L2CAP_FLEXIBLE_CONFIG is XA_ENABLED. 
    */
    BOOL        autoConfigOff;

   /* The channel-specific flush timeout sent to the remote device in a
    * configuration response. A non-zero value specifies the flush timeout to
    * be sent. Otherwise, the default infinite flush timeout is assumed and 
    * no flush timeout will be sent in the response. A non-zero flush timeout
    * should be calculated from the processing time and buffer capacity of 
    * the local receiver. It is sent in a configuration response so the 
    * remote device will know what flush timeout to use based on local 
    * receiving capacity. 
    */
    U16         flushTimeout;

#if L2CAP_PRIORITY == XA_ENABLED
    /* The default channel-specific priority.
     */
    BtPriority  priority;
#endif

#if L2CAP_NUM_ENHANCED_CHANNELS > 0
    /* Optional Enhanced L2CAP mode parameters for Channel. The channel will
     * use the parameters set in the PSM if not specified here. 
     */
    L2capFlowParms  *flowParms;
#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */
} L2capConnSettings;


#if L2CAP_NUM_GROUPS > 0
/*---------------------------------------------------------------------------
 * L2capGroup structure
 *
 *     Represents a set of devices in a group. Before calling
 *     L2CAP_RegisterGroup, the L2CAP user must allocate an L2capGroup
 *     structure and fill the "callback", "psm", and "maxMembers" fields.
 *
 *     This structure is only available when the L2CAP_NUM_GROUPS
 *     configuration is greater than 0.
 */
typedef struct _L2capGroup
{
    /* Callback to use for group events. */
    L2capCallback callback;
    
    /* The PSM value for this group. */
    L2capPsmValue  psm;

    /* Maximum number of group members in the "members" field. */
    U8             maxMembers;

    /* Group Channel ID. This is filled in by L2AP during the 
     * L2CAP_RegisterGroup call. */
    L2capChannelId cid;

    /* === Internal use only === */
    U16            inUse;
    BT_BD_ADDR        members[1];

} L2capGroup;

#endif /* L2CAP_NUM_GROUPS */


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * L2CAP_RegisterPsm()
 *
 *     Registers a PSM identifier with L2CAP. Once registered, the protocol
 *     can initiate connections and data transfer as well as receive
 *     connection requests and data.
 *
 * Parameters:
 *     protocol - Pointer to the L2capPsm structure to be registered.
 *         This structure is owned by L2CAP until it is successfully
 *         deregistered. All fields in the structure must be initialized
 *         before this function is called.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - PSM was successfully registered with L2CAP.
 *
 *     BT_STATUS_FAILED - Invalid parameter value (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Maximum number of registered PSMs reached.
 *
 *     BT_STATUS_IN_USE - The protocol is already registered (XA_ERROR_CHECK only).
 */
BtStatus L2CAP_RegisterPsm(L2capPsm *protocol);


#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_DeregisterPsm()
 *
 *     Deregisters a registered protocol from L2CAP.
 *
 * Requires:
 *     L2CAP_DEREGISTER_FUNC enabled.
 *
 * Parameters:
 *     protocol - Pointer to L2capPsm structure previously registered with
 *         the L2CAP_RegisterPsm function call.
 *                
 * Returns:
 *     BT_STATUS_SUCCESS - The protocol has been deregistered.
 *
 *     BT_STATUS_FAILED - The protocol parameter was invalid (XA_ERROR_CHECK
 *         only).
 * 
 *     BT_STATUS_NOT_FOUND - The protocol was not previously registered.
 * 
 *     BT_STATUS_BUSY - The protocol has one or more active channels and
 *         cannot be deregistered.
 */
BtStatus L2CAP_DeregisterPsm(const L2capPsm *protocol);

#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2CAP_ConnectReq()
 *
 *     Requests the establishment of an L2CAP channel to the specified
 *     remote protocol service and device. If this function returns
 *     BT_STATUS_PENDING, the request was successfully initiated.
 *     Connection status events will be sent to the initiating
 *     L2capPsm's callback function.
 *                
 * Parameters:
 *     protocol - Pointer to a registered L2capPsm structure.
 *     
 *     psm - PSM value of protocol to connect to on the remote device.
 *
 *     remDevice - The remote device to connect with.
 *
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if no settings are relevant.
 *
 *     localCid - Pointer to a channel ID. If this function returns
 *         BT_STATUS_PENDING, this parameter is filled with a channel
 *         ID assigned to this connection.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         initialized. When the connection process is complete, the
 *         protocol service's callback will receive either the
 *         L2EVENT_CONNECTED or L2EVENT_DISCONNECTED L2capEventType.
 *
 *     BT_STATUS_INVALID_PARM - Protocol specified is not registered
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - Protocol linkModes mismatch.
 * 
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a channel
 *         structure to initiate the connection.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 *
 *     BT_STATUS_RESTRICTED - The PSM is not authorized to establish
 *         outbound connections.
 */
BtStatus L2CAP_ConnectReq(const L2capPsm *protocol, L2capPsmValue psm, 
                          BtRemoteDevice *remDevice, L2capConnSettings *settings,
                          L2capChannelId *localCid);


/*---------------------------------------------------------------------------
 * L2CAP_ConnectRsp()
 *
 *     Indicates the response to a connection request (L2EVENT_CONNECT_IND
 *     L2capEventType) from a remote device.
 *
 * Parameters:
 *     cid - Channel ID assigned by L2CAP to this connection. This
 *         ID must match the one provided by the L2EVENT_CONNECT_IND event
 *         callback.
 *
 *     status - Response code used in the response to the remote device.
 *         This value may indicate success, pending, or rejection. 
 * 
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if no settings are relevant.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection response packet has been initiated
 *         successfully. One of the following events will arrive to
 *         indicate completion:
 *
 *         L2EVENT_CONNECTED, if the connection was established successfully.
 *
 *         L2EVENT_DISCONNECTED, if the connection failed. Check the callback
 *         info's "status" parameter for the exact reason.
 *
 *         L2EVENT_COMPLETE, when the remote device has been informed of the
 *         L2CONN_PENDING connection status. L2CAP_ConnectRsp must
 *         be called a second time.
 *
 *     BT_STATUS_FAILED - Invalid channel ID (XA_ERROR_CHECK only), or
 *         L2CAP_ConnectRsp was called at the wrong time.
 *
 *     BT_STATUS_IN_PROGRESS - A response is already in progress. Wait for
 *         L2EVENT_COMPLETE before calling L2CAP_ConnectRsp.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_ConnectRsp(L2capChannelId cid, L2capConnStatus status, 
                          L2capConnSettings *settings);


/*---------------------------------------------------------------------------
 * L2CAP_DisconnectReq()
 *
 *     Requests the disconnection of the specified L2CAP channel.
 *
 * Parameters:
 *     cid - The channel to disconnect.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The disconnect request has been initiated
 *         successfully. When the disconnect is complete, the
 *         L2EVENT_DISCONNECTED event is provided to the L2CAP user.
 * 
 *     BT_STATUS_IN_PROGRESS - The channel is already disconnecting
 *         (XA_ERROR_CHECK Only).    
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         disconnected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_DisconnectReq(L2capChannelId cid);


/*---------------------------------------------------------------------------
 * L2CAP_SendData()
 *
 *      Sends data on the specified channel.
 *
 * Parameters:
 *      cid - Channel to send the data over. The channel must be
 *          connected or refer to a registered group.
 * 
 *      packet - Data packet describing data to transmit. The caller
 *          must initialize all necessary BtPacket fields, including
 *          "data", "dataLen", "flags", "ulpContext", "tail", "tailLen",
 *          "header" and "headerLen" as described below.
 *
 *          The "data" pointer and "dataLen" length indicate the payload
 *          data to be transmitted by L2CAP.
 *
 *          The "header" array is filled with higher-layer protocol
 *          header data. This array is filled back-to-front so that
 *          protocols may fill in data as the packet is handed down
 *          the protocol stack. "headerLen" indicates the number of
 *          valid header bytes at the end of the "header" array.
 *
 *          The "ulpContext" field may be filled with a protocol-specific
 *          value. L2CAP and lower layers ignore this field.
 *
 *          The "flags" field may be set as described by the BtPacketFlags
 *          type.
 *
 *          The "tail" pointer may point to a data buffer that is appended
 *          to the end of the packet (after "data"). The field "tailLen"
 *          is set to the number of bytes pointed to by "tail". If this field
 *          is used, the packet flag BTP_FLAG_TAIL must be set.        
 * 
 *          The packet's total size ("headerLen" plus "dataLen" plus
 *          "tailLen", if present) must be less than or equal to
 *          the remote device's L2CAP MTU. For group channels,
 *          the total size should typically be kept below 670 bytes. 
 *          
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission on
 *         the channel. The completion of the send request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_SendData(L2capChannelId cid, BtPacket *packet);

/*---------------------------------------------------------------------------
 * L2CAP_SetFlushTimeout()
 *
 *      Sets or resets the flush timeout on the specified channel.
 *
 * Parameters:
 *      ChannelId - Channel for which to set the flush timeout. The channel
 *          must be connected or refer to a registered group.
 * 
 *      FlushTimeout - Value of new flush timeout.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Flush timeout command was sent.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 */
BtStatus L2CAP_SetFlushTimeout(L2capChannelId ChannelId, U16 FlushTimeout);


#if L2CAP_NUM_ENHANCED_CHANNELS > 0

/*---------------------------------------------------------------------------
 * L2CAP_GetConnectionMode()
 *
 *     Retrieves the link mode of the connected channel.
 *
 * Parameters:
 *     Cid - Channel ID to query.
 *
 * Returns:
 *     Link Mode - The L2capLinkMode selected during channel configuration.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
L2capLinkMode L2CAP_GetConnectionMode(L2capChannelId Cid);

/*---------------------------------------------------------------------------
 * L2CAP_SetLocalBusy()
 *
 *     Sets the busy mode of the connected ERTM channel.
 *
 * Parameters:
 *     Cid - Channel ID to set.
 *
 *     Set - When true, set busy. When false, clear busy.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Retransmissions have been disabled.
 *
 *     BT_STATUS_FAILED - The channel is not configured properly
 *                        or is in wrong state for this operation.
 */
BtStatus L2CAP_SetLocalBusy(L2capChannelId Cid, BOOL Set);

#endif /* L2CAP_NUM_ENHANCED_CHANNELS > 0 */


/*---------------------------------------------------------------------------
 * L2CAP_GetTxMtu()
 *
 *     Retrieves the transmit MTU of the connected channel.
 *
 * Parameters:
 *     cid - Channel ID to query.
 *
 * Returns:
 *     Size - The maximum transmission unit size in bytes.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
U16 L2CAP_GetTxMtu(L2capChannelId cid);


#if L2CAP_PING_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_Ping()
 *
 *     Sends an Echo Request to the L2CAP protocol on the specified remote
 *     device. The data length should not exceed the default L2CAP
 *     signaling MTU (44 bytes).
 *
 * Requires:
 *     L2CAP_PING_SUPPORT enabled.
 *
 * Parameters:
 *     pingClient - Client registration structure used by L2CAP to track
 *         the Ping request. The caller must set the callback field.
 *         
 *     remAddr - Bluetooth address of device to ping.
 *
 *     data - Data to send to remote device.
 *
 *     dataLen - Length of "data" buffer in bytes. Must be less than 44.
 *
 * Returns:
 *     BT_STATUS_PENDING - Ping request was successfully queued for
 *         transmit. The result is indicated by an L2EVENT_COMPLETE event,
 *         sent to the "pingClient" callback function, when a response is
 *         received or the request times out. Check the L2capCallbackParms
 *         "status" parameter for the completion status. Any data echoed
 *         back can be found in the callback info's "data" and "dataLen"
 *         fields.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *         channel or packet.
 * 
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 */
BtStatus L2CAP_Ping(const L2capPsm *pingClient, BtRemoteDevice *remDevice,
                    const U8 *data, U16 dataLen);

#endif /* L2CAP_PING_SUPPORT */


#if L2CAP_GET_INFO_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_GetInfo()
 *
 *     Sends an Info Request to the L2CAP protocol on the specified remote
 *     device. Info requests are used to exchange implementation-specific
 *     information regarding L2CAP's capabilities.
 *
 * Requires:
 *     L2CAP_GET_INFO_SUPPORT enabled.
 *
 * Parameters:
 *     infoClient - Client registration structure used by L2CAP to track
 *         the Info request. The caller must set the callback field.
 *
 *     remAddr - Bluetooth address of the device to query.
 *
 *     type - Type of information to request.
 *
 * Returns:
 *     BT_STATUS_PENDING - Sending of an Information Request has been
 *         successfully initiated. The result is indicated by an
 *         L2EVENT_COMPLETE event when a response is received or the request
 *         times out. The result is reported in the L2capCallbackParms
 *         "status" parameter. The result value & length is conveyed in the
 *         callback "data" and "dataLen" fields.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *         channel or packet.
 * 
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 */
BtStatus L2CAP_GetInfo(const L2capPsm *infoClient,
                       BtRemoteDevice *remDevice, L2capInfoType type);

#endif /* L2CAP_GET_INFO_SUPPORT */


#if L2CAP_NUM_GROUPS > 0
/*---------------------------------------------------------------------------
 * L2CAP_RegisterGroup()
 *
 *     Registers an L2CAP group structure and allocates a channel for
 *     the group.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     group - Group structure used by L2CAP to track group membership.
 *         L2CAP retains ownership of this structure for the duration of
 *         the group. The caller must initialize the "maxMembers", "psm",
 *         and "callback" fields prior to calling this function.
 *
 *     groupId - Pointer to a Channel ID that receives the Group ID
 *         assigned by L2CAP. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Group was successfully registered.
 *
 *     BT_STATUS_INVALID_PARM - Group could not be registered because a parameter
 *         was invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - There are no more group entries available
 *         in L2CAP. Increase the L2CAP_NUM_GROUPS configuration parameter
 *         to avoid this error.
 */
BtStatus L2CAP_RegisterGroup(L2capGroup *group, L2capChannelId *groupId);


/*---------------------------------------------------------------------------
 * L2CAP_GroupDestroy()
 *
 *     Removes a group from L2CAP.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     group - Group structure used to register the group. After successful
 *         completion of this call L2CAP no longer maintains ownership of
 *         this group.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The group was removed.
 *
 *     BT_STATUS_FAILED  - The group was not registered (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - An operation concerning this group is in progress.
 *         Wait for completion before destroying the group.
 */
BtStatus L2CAP_GroupDestroy(L2capGroup *Group);


/*---------------------------------------------------------------------------
 * L2CAP_GroupAddMember()
 *
 *     Adds a Bluetooth device address to the membership list.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     groupId - Channel ID assigned to the group by L2CAP_RegisterGroup.
 *
 *     newMember - Bluetooth address of device to add.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device was added to group membership.
 *
 *     BT_STATUS_FAILED - A parameter was invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Group member list is full.
 */
BtStatus L2CAP_GroupAddMember(L2capChannelId GroupId,
                              const BT_BD_ADDR *newMember);


/*---------------------------------------------------------------------------
 * L2CAP_GroupRemoveMember()
 *
 *     Removes a device from the group.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     groupId - Channel ID assigned to the group by L2CAP_RegisterGroup.
 * 
 *     member - Bluetooth address of device to remove.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Device was removed from group membership.
 *
 *     BT_STATUS_FAILED - A parameter was invalid (XA_ERROR_CHECK only).
 *                                    
 *     BT_STATUS_NOT_FOUND - The device was not a member of the group.
 */
BtStatus L2CAP_GroupRemoveMember(L2capChannelId GroupId,
                                 const BT_BD_ADDR *Member);


/*---------------------------------------------------------------------------
 * L2CAP_GroupGetMembers()
 *
 *     Retrieves a list of members for the specified group.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     groupId - Channel ID assigned to the group by L2CAP_RegisterGroup.
 *
 *     list - Pointer that receives the address of the member list. Do
 *         not modify this list.
 *
 *     count - Pointer that receives the number of BD_ADDR entries in
 *         "list".
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The group member list has been returned.
 *
 *     BT_STATUS_FAILED - A parameter was invalid (XA_ERROR_CHECK only).
 */
BtStatus L2CAP_GroupGetMembers(L2capChannelId GroupId, BT_BD_ADDR **List,
                               U8 *Count);


/*---------------------------------------------------------------------------
 * L2CAP_DisableCLT()
 *
 *     Disables incoming connectionless traffic. By default, incoming
 *     connectionless traffic is enabled.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     psm - Not used at this time.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Call was successful.
 */
BtStatus L2CAP_DisableCLT(U16 Psm);


/*---------------------------------------------------------------------------
 * L2CAP_EnableCLT()
 *
 *     Enables incoming connectionless traffic. By default, incoming
 *     connectionless traffic is enabled.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     psm - Not used at this time.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Call was successful.
 */
BtStatus L2CAP_EnableCLT(U16 Psm);

#endif /* L2CAP_NUM_GROUPS */


#if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_SetAutoConfigMode()
 *
 *     This function is used to toggle the auto configuration mode in 
 *     L2CAP either on or off. By default auto configuration is on. When
 *     auto configuration is on L2CAP will manage the connection configuration
 *     process. When off, the application is responsible for managing the
 *     configuration.
 *
 *     This function can only be called when the channel is in the unconnected
 *     state. Or when a Connect Request is being indicated.
 *
 * Requires:
 *     L2CAP_FLEXIBLE_CONFIG enabled.
 *
 * Parameters:
 *      ChannelId - Channel to set the auto configuration mode of.
 *
 *      On - TRUE to enable auto configuration, FALSE to disable it.
 */
void L2CAP_SetAutoConfigMode(L2capChannelId ChannelId, BOOL On);


/*---------------------------------------------------------------------------
 * L2CAP_SendConfigReq()
 *
 *     Sends an L2CAP Configuration Request. If any configuration options
 *     are provided they must be in the data area of the BtPacket. The
 *     L2CAP protocol layer will not add any options to the request. This
 *     PDU should be sent when the L2EVENT_CONFIGURING is received. It is
 *     not permitted to wait for an L2EVENT_CONFIG_IND before initiating
 *     local configuration as this can lead to a deadlock. It is also not
 *     permitted to send a configuration request immediately after sending
 *     a connect response, you must wait for the L2EVENT_CONFIGURING event.
 *
 *     If a configuration setting present in the L2capPsm structure is
 *     negotiated using this function, the registered L2capPsm structure
 *     must be updated to reflect the negotiated value.
 *
 * Requires:
 *     L2CAP_FLEXIBLE_CONFIG enabled and channel auto-configure disabled.
 *
 * Parameters:
 *     ChannelId - Channel to send the data over. The channel must be
 *         in the connected or configuration state.
 * 
 *     Request - Data packet describing data to transmit. The caller
 *         must initialize all necessary BtPacket fields, including
 *         "data", "dataLen", "flags", "ulpContext", and "headerLen"
 *         as described below.
 *
 *         The "data" pointer and "dataLen" length indicate the 
 *         configuration options to be transmitted by L2CAP. L2CAP will affix
 *         the configuration option header. Only the options themselves can
 *         be present. 
 *
 *         The "header" array reserved by L2CAP when using this function.
 *         Therefore "headerLen" should be zero.
 *
 *         The "ulpContext" field may be filled with a protocol-specific
 *         value. L2CAP and lower layers ignore this field.
 *
 *         The "flags" field may be set as described by the BtPacketFlags
 *         type.
 *
 *     Flags - Flags for this configuration request.
 *
 *
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission
 *         on the channel. The completion of the send request is indicated
 *         by an L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_SendConfigReq(L2capChannelId ChannelId, BtPacket *Request,
                             L2capConfigFlags Flags);
                             
/*---------------------------------------------------------------------------
 * L2CAP_SendConfigRsp()
 *
 *     Sends an L2CAP Configuration Response. If any configuration options
 *     are provided they must be in the data area of the BtPacket. The
 *     L2CAP protocol layer will not add any options to the response. This
 *     PDU must be sent upon reception of an L2EVENT_CONFIG_IND event.
 *
 *     If a configuration setting present in the L2capPsm structure is
 *     negotiated using this function, the registered L2capPsm structure
 *     must be updated to reflect the negotiated value.
 *
 * Requires:
 *     L2CAP_FLEXIBLE_CONFIG enabled and channel auto-configure disabled.
 *
 * Parameters:
 *     ChannelId - Channel to send the data over. The channel must be
 *         in the configuration state.
 * 
 *     Request - Data packet describing data to transmit. The caller
 *         must initialize all necessary BtPacket fields, including
 *         "data", "dataLen", "flags", "ulpContext", and "headerLen"
 *         as described below.
 *
 *         The "data" pointer and "dataLen" length indicate the 
 *         configuration options to be transmitted by L2CAP. L2CAP will affix
 *         the configuration option header. Only the options themselves can
 *         be present.
 *         
 *         The "header" array reserved by L2CAP when using this function.
 *         Therefore "headerLen" should be zero.
 *         
 *         The "ulpContext" field may be filled with a protocol-specific
 *         value. L2CAP and lower layers ignore this field.
 *
 *         The "flags" field may be set as described by the BtPacketFlags
 *         type.
 *
 *     Result - The result of the configuration request
 *
 *     Flags - Flags for this configuration response.
 *
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission
 *         on the channel. The completion of the send request is indicated
 *         by an L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_SendConfigRsp(L2capChannelId ChannelId, BtPacket *Response, 
                             L2capConfigResult Result, L2capConfigFlags Flags);

#endif /* L2CAP_FLEXIBLE_CONFIG == XA_ENABLED */

#define BT_INVALID_PSM      BT_CLIENT_ONLY_PSM

void L2Cap_SecAccessRsp(const BtEvent *Event);

#endif /* __L2CAP_H */
