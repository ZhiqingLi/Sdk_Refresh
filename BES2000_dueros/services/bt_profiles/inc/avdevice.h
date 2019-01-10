/****************************************************************************
 *
 * File:
 *     $Id: a2dp.h 1211 2009-03-26 22:59:24Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1211 $
 *
 * Description: This file contains public definitions for the Advanced Audio 
 *     Distribution Profile (A2DP).
 *
 * Created:     March 15, 2004
 *
 * Copyright 2004-2005 Extended Systems, Inc.
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

#ifndef _AVDEV_H_
#define _AVDEV_H_

#include "avdtp.h"
#include "conmgr.h"
#include "sec.h"

/*---------------------------------------------------------------------------
 * AV Device layer
 * 
 *     The AV Device layer was created to manage access to the AVDTP
 *     signalling channel by multiple AV profiles (i.e. A2DP and VDP)
 *     simultaneously. In addition, this API layer can be used by
 *     applications to create AVDTP signaling channel connections, discover
 *     stream endpoints, and get stream endpoint capabilities.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/* Device states - used internally */
#define AVDEV_STATE_DISCONNECTED  0x00
#define AVDEV_STATE_INCOMING      0x01
#define AVDEV_STATE_OUTGOING      0x02
#define AVDEV_STATE_CONNECTED     0x03

/* Device flags - used internally */
#define AV_DEV_FLAG_OUTGOING      0x01

/* Command Token Types - used internally */
#define AVDEV_CMD_CREATE_LINK           0x01
#define AVDEV_CMD_REMOVE_LINK           0x02
#define AVDEV_CMD_CONNECT               0x03
#define AVDEV_CMD_DISCONNECT            0x04
#define AVDEV_CMD_DISCOVER_STREAMS      0x05
#define AVDEV_CMD_GET_CAPABILITY        0x06
#define AVDEV_CMD_ACCEPT_LINK           0x07
                
/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AvdevEvent type
 *      
 *     All indications and confirmations are sent through a callback function
 *     as events. The type of event will determine which fields in the
 *     AvdevCallbackParms structure are valid.  The 'len', 'status', and
 *     'error' fields are always valid.  The 'event' field contains the event
 *     associated with the callback.
 */
typedef U8 AvdevEvent;

/** When an ACL connection request is received, this event will be
 *  received by the ACL listener (see AVDEV_ListenACLLink()).
 */
#define AVDEV_EVENT_AVDTP_CONNECT_IND   0x01

/** When the the ACL connection is established between the local and remote
 *  devices, this event will be generated.
 */
#define AVDEV_EVENT_LINK_CONNECT        0x02

/** This event will also be generated when an ACL connection attempt fails.
 *  
 *  The 'discReason' parameter contains the reason for the link disconnect
 *  (BtErrorCode).
 */
#define AVDEV_EVENT_LINK_DISCONNECT     0x03

/** When the the lower level AVDTP connection is established between the
 *  local and remote devices, this event will be generated.
 */
#define AVDEV_EVENT_AVDTP_CONNECT       0x04

/** When the the lower level AVDTP connection is disconnected, this event
 *  will be generated.  This event will also be generated when a connection
 *  attempt fails.
 *  
 *  The 'status' parameter contains the L2CAP disconnect reason
 *  (L2capDiscReason).  If 'status' equals L2DISC_LINK_DISCON, then the
 *  'discReason' parameter contains the reason for the link disconnect
 *  (BtErrorCode).
 */
#define AVDEV_EVENT_AVDTP_DISCONNECT    0x05

/** For all other AVDTP events, this event will be generated.  This event
 *  provides all the information associated with the AVDTP event.
 *  
 *  The 'avdtpParms' parameter contains the parameters of the AVDTP event.
 */
#define AVDEV_EVENT_AVDTP_EVENT         0x06

/* End of AvdevEvent */

/* Forward declaration */ 
typedef struct _AvDevice AvDevice;
typedef struct _AvCmdToken AvCmdToken;
typedef struct _AvdevCallbackParms AvdevCallbackParms;

/*---------------------------------------------------------------------------
 * AvdevCallback type
 *
 * A function of this type is called to indicate events to the application.
                                                                           */
typedef void (*AvdevCallback)(AvDevice *Device, const AvdevCallbackParms *Parms);

/* End of AvdevCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/* AvdevCmdToken structure - used internally */
struct _AvCmdToken {

    /* Flag to determine if the token is in use */
    BOOL                   inUse;

    /* On input contains the callback function for discoveries */
    AvdevCallback          callback;
    
    /* On input contains the type of command to send */
    U8                     cmdType;

    /* On input contains the parameters of specific commands */
    union {
        /* Parms for the AVDTP Connect command  */
        struct {
            BtRemoteDevice     *remDev;
        } connect;

        /* Parms for the Get Capabillies command */
        struct {
            AvdtpStreamId id;
        } getCap;

        /* Parms for the Accept Link command */
        struct {
            BOOL accept;
        } acceptLink;
    } p;
};

/* AvDeviceHandler - used internally
 *
 * Identifies an upper layer entity that handles streams on a device.
 * Registering a stream handler, allows the reception of all AVDTP and Device
 * Layer events. Since taking action on many events could undermine the
 * actions taken by other handlers, great care must be taken. Typically, only
 * the A2DP or VDP profile code should register a stream handler.  If an
 * application does register a handler, then it should only do so for the
 * purpose of "listening" to events.
 * 
 * For applications, there are a set of commands defined that include a
 * callback function in the parameter list.  These commands are safe to be
 * used by applications, because the callbacks are directed specifically to
 * the application, which can then take actions on the received events.
 */
typedef struct _AvDeviceHandler {
    
    ListEntry      node;             /* Used internally by AvDevice */
    AvdevCallback  callback;         /* Used internally by AvDevice */
    
} AvDeviceHandler;

/*---------------------------------------------------------------------------
 * AvDevice structure
 *
 * Indentifies the remote AV device, or the signaling channel connected to
 * that device.  No fields in this structure are usable by applications.
 */ 
struct _AvDevice {

    ListEntry         node;           /* Used internally by AvDevice */

    AvdtpChannel      channel;        /* AVDTP channel */ 

    U8                refCount;       /* Reference count for signal channel */
    
    CmgrHandler       cmgrHandler;    /* Used for SDP and signal channel */ 

    U8                state;          /* Device state */

    U8                flags;          /* Device flags */
    
    AvCmdToken        cmdToken;       /* Command Token used for this device */

    BOOL              delayReporting; /* Indicates whether delay reporting is
                                       * supported or not. 
                                       */
    
    /* Used by upper layer */
    void             *currentStream;  /* Current stream on this device */
    U8                currentType;    /* Current type of stream */
    
    /* Updated by SDP queries */
    SdpQueryToken     sdpQueryToken;  /* SDP query token */ 
    U8                queryFlags;     /* SDP query flags */ 
    U8                features;
    U16               avdtpVersion;   /* Version of AVDTP on the remote device */
    U16               a2dpVersion;    /* Version of A2DP on the remote device */
    U16               vdpVersion;     /* Version of VDP on the remote device */
};

/*---------------------------------------------------------------------------
 * AvdevCallbackParms structure
 *
 * Contains information about the upper layer callback event.
 */
struct _AvdevCallbackParms {
    AvdevEvent                event;      /* The AVDEVICE Event */ 
    BtStatus                  status;     /* Transport status */ 
    AvdtpError                error;      /* AVDTDP error code */
    const AvdtpCallbackParms *avdtpParms; /* AVDTP callback parms.  Valid if
                                           * non-zero.
                                           */
    
    U16                       len;        /* Length of the AVDEVICE parameters */ 
    union {
        U8                    discReason; /* Reason for disconnect.  Valid
                                           * only when 
                                           * AVDEV_EVENT_AVDTP_DISCONNECT 
                                           * is received.
                                           */
    } p;
};

/* AvdevContext structure - used internally */
typedef struct _BtAvdevContext {

    /* Devices */
    ListEntry   freeDeviceList;
    ListEntry   inUseDeviceList;
    AvDevice    devices[NUM_BT_DEVICES];
    AvDevice   *currentDevice;
    U16         refCount;

    /* Upper layer streams */
    ListEntry   deviceHandlerList;

    /* Listener Callback */
    AvdevCallback listenCallback;

    /* Security record for A2DP and VDP. */
    BtSecurityRecord    secRec;
} BtAvdevContext;

/* AV device context macro */
#if XA_CONTEXT_PTR == XA_ENABLED
extern BtAvdevContext *avdevContext;
#define AVDEV(s) (avdevContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtAvdevContext avdevContext;
#define AVDEV(s) (avdevContext.s)
#endif /* XA_CONTEXT_PTR */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * AVDEV_Init()
 *
 *     Initialize the AvDevice SDK.  This function should only be called
 *     once, normally at initialization time.  The calling of this function
 *     can be specified in overide.h using the XA_LOAD_LIST macro (i.e.
 *     #define XA_LOAD_LIST XA_MODULE(A2DP) ... ).
 *
 * Returns:
 *
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL AVDEV_Init(void);

/*---------------------------------------------------------------------------
 * AVDEV_CreateACLLink()
 *
 *     Create an ACL link to a device.
 * 
 * Parameters:
 * 
 *     Device - Upon returning from the call, contains a pointer to the
 *         device upon which the connection is created.
 * 
 *     Callback - A call back function to which events are sent.
 * 
 *     Addr - The bluetooth address of the device to which a aconnetion will
 *         be attempted.  If 0, the device selector will be called.
 * 
 *     Type - The type of endpoint to connect to (Source or Sink).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - A link exists to Device.
 * 
 *     BT_STATUS_PENDING - A link is being created to Device, once connected
 *         the AVDEV_EVENT_LINK_CONNECT will be received.
 * 
 *     BT_STATUS_NO_RESOURCES - No resources are available for creating a
 *         connection.  The number of resources are controlled by
 *         NUM_BT_DEVICES.
 * 
 *     BT_STATUS_BUSY - A connection is already in progress.
 */
BtStatus AVDEV_CreateACLLink(AvDevice **Device, 
                             AvdevCallback Callback, 
                             BT_BD_ADDR *Addr,
                             AvdtpStrmEndPointType Type);

/*---------------------------------------------------------------------------
 * AVDEV_CreateSignalLink()
 *
 *     Create a link to the signaling channel.
 * 
 * Parameters:
 * 
 *     Device - Contains a pointer to a connected device.
 * 
 *     Callback - A call back function to which events are sent.
 * 
 * Returns:
 * 
 *     BT_STATUS_BUSY - An operation is already in progress
 * 
 *     Other - See return codes for AVDTP_Connect() in the AVDTP API
 *         description.
 */
BtStatus AVDEV_CreateSignalLink(AvDevice *Device, AvdevCallback Callback);

/*---------------------------------------------------------------------------
 * AVDEV_ListenSignalLink()
 *
 *     Listen for a signal link on the specified callback.
 * 
 * Parameters:
 * 
 *     Callback - A call back function to which events are sent.
 * 
 * Returns:
 * 
 *     BT_STATUS_SUCCESS - Registration of the listener callback was
 *         successful.
 * 
 */
BtStatus AVDEV_ListenSignalLink(AvdevCallback Callback);

/*---------------------------------------------------------------------------
 * AVDEV_SignalConnectRsp()
 *
 *     Respond to an incoming signal link request.
 * 
 * Parameters:
 * 
 *     Device - Contains a pointer to a connected device.
 * 
 *     Accept - If TRUE, the connection is accepted.  If FALSE, the
 *         connection is rejected.
 * 
 *     Callback - A call back function to which events are sent.
 * 
 * Returns:
 * 
 *     BT_STATUS_BUSY - An operation is already in progress
 * 
 *     Other - See return codes for AVDTP_ConnectRsp() in the AVDTP API
 *         description.
 */
BtStatus AVDEV_SignalConnectRsp(AvDevice *Device, BOOL Accept, AvdevCallback Callback);

/*---------------------------------------------------------------------------
 * AVDEV_DiscSignalLink()
 *
 *     Disconnect the signaling channel.
 * 
 * Parameters:
 * 
 *     Device - Contains a pointer to a connected device.
 * 
 *     Callback - A call back function to which events are sent.
 * 
 * Returns:
 * 
 *     BT_STATUS_BUSY - An operation is already in progress
 * 
 *     Other - See return codes for AVDTP_Disconnect() in the AVDTP API
 *         description.
 */
BtStatus AVDEV_DiscSignalLink(AvDevice *Device, AvdevCallback Callback);

/*---------------------------------------------------------------------------
 * AVDEV_DiscoverStreamEndpoints()
 *
 *     Discover stream endpoints on the remote device.
 * 
 * Parameters:
 * 
 *     Device - Contains a pointer to a connected device.
 * 
 *     Callback - A call back function to which events are sent.
 * 
 * Returns:
 * 
 *     BT_STATUS_BUSY - An operation is already in progress
 * 
 *     Other - See return codes for AVDTP_GetCapabilities() in the AVDTP API
 *         description.
 * 
 */
BtStatus AVDEV_DiscoverStreamEndpoints(AvDevice *Device, 
                                       AvdevCallback Callback);

/*---------------------------------------------------------------------------
 * AVDEV_GetCapabilities()
 *
 *     Discover capabiltites of a stream endpoint.
 * 
 * Parameters:
 * 
 *     Device - Contains a pointer to a connected device.
 * 
 *     Callback - A call back function to which events are sent.
 * 
 *     Id - The ID of a stream endpoint
 *
 * Returns:
 * 
 *     BT_STATUS_BUSY - An operation is already in progress
 * 
 *     Other - See return codes for AVDTP_GetCapabilities() in the AVDTP API
 *         description.
 */
BtStatus AVDEV_GetCapabilities(AvDevice *Device, 
                               AvdevCallback Callback, 
                               AvdtpStreamId Id);

/*---------------------------------------------------------------------------
 * AVDEV_RegisterSec()
 *
 *     Registers security for VDP and A2DP.
 * 
 * Parameters:
 * 
 *     SecParms - Security parameters.  If 0, baseline security
 *                parameters are used (BSL_SECURITY_L1_IN | BSL_SECURITY_L1_OUT).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is registered.
 *
 *     BT_STATUS_INVALID_PARM - operation failed because the
 *         security level is not properly formed. (error check only).
 *
 *     BT_STATUS_IN_USE - operation failed because A/V security is alread
 *                        registered. 
 *
 *     BT_STATUS_FAILED - operation failed because record is
 *         already on the list (XA_ERROR_CHECK only).
 */
BtStatus AVDEV_RegisterSec(BtSecurityParms *SecParms);

/*---------------------------------------------------------------------------
 * AVDEV_UnregisterSec()
 *
 *     Unregisters security for VDP and A2DP.
 * 
 * Parameters:
 * 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - security record is unregistered.
 *
 *     BT_STATUS_FAILED - operation failed because record is not
 *         on the list.
 */
BtStatus AVDEV_UnregisterSec(void);

/* The following functions are only used by profile implementations (A2DP
 * and VDP)
 */ 

/* AVDEV_RegisterHandler() - Register a handler. */
BOOL AVDEV_RegisterHandler(AvDeviceHandler *deviceHandler, AvdevCallback Callback);

/* AVDEV_DeregisterHandler() - Deregister a handler. */
BOOL AVDEV_DeregisterHandler(AvDeviceHandler *deviceHandler);

/* AVDEV_RegisterChannel() - Register a channel. */
BtStatus AVDEV_RegisterChannel(void);

/* AVDEV_DeregisterChannel() - Deregister a channel. */
void AVDEV_DeregisterChannel(void);

/* AVDEV_IncrementUseCount() - Increment the stream use count on a device. */
void AVDEV_IncrementUseCount(AvDevice *device);

/* AVDEV_DecrementUseCount() - Decrement the stream use count on a device. */
void AVDEV_DecrementUseCount(AvDevice *device, BOOL initiator);

/* AVDEV_FindConnectedDevice() - searches for an existing connection. */
BOOL AVDEV_FindConnectedDevice(const BT_BD_ADDR *Addr,
                               AvDevice **deviceP, 
                               BtStatus *statusP);
BOOL AVDEV_CheckIfDeviceIsAvailableForTheBdAddr(const BT_BD_ADDR  *Addr);

#endif /* _AVDEV_H_ */

