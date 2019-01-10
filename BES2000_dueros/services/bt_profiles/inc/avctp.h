/****************************************************************************
 *
 * File:
 *     $Id: avctp.h 2492 2009-07-08 23:09:09Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 2492 $
 *
 * Description: This file contains public definitions for the Audio/Video 
 *     Remote Control Transport Protocol (AVCTP).  
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

#ifndef __AVCTP_H_
#define __AVCTP_H_

#include "sys/avtp.h"
#include "sys/avctpcon.h"

/*---------------------------------------------------------------------------
 * AVCTP API layer
 *
 *     The Audio/Video Remote Control Transport Protocol (AVCTP) defines 
 *     procedures for exchanging 1394 Trade Association AV/C commands between 
 *     Bluetooth enabled Audio/Video devices.
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

/* Timers disabled in AVTP */
#define AVCTP_RTX_TIMEOUT 0xFFFFFFFF

/*---------------------------------------------------------------------------
 * AVCTP_MAX_MTU
 * 
 * Defines the L2CAP RX MTU size of the AVRCP channel.
 * 
 * The default value is L2CAP_MTU bytes.
 */
#ifndef AVCTP_MAX_MTU
#define AVCTP_MAX_MTU L2CAP_MTU
#endif

#if AVCTP_MAX_MTU > L2CAP_MTU
#error AVCTP_MAX_MTU must be less than or equal to L2CAP_MTU
#endif

 /*--------------------------------------------------------------------------
  * AVCTP_TX_MPS contant
  * 
  *     Defines the maximum transmit mps fragment size of the control
  *     channel. All control channel transmissions will be fragmented into
  *     this size.
  * 
  *     This default size is L2CAP_MINIMUM_MTU.
  */
#ifndef AVCTP_TX_MPS
#define AVCTP_TX_MPS L2CAP_MINIMUM_MTU
#endif

#if AVCTP_TX_MPS < L2CAP_MINIMUM_MTU
#error AVCTP_TX_MPS must be L2CAP_MINIMUM_MTU or greater.
#endif

#if AVCTP_TX_MPS > L2CAP_MTU
#error AVCTP_TX_MPS cannot exceed L2CAP_MTU
#endif

 /*--------------------------------------------------------------------------
  * AVCTP_RX_MPS contant
  * 
  *     Defines the mps fragment size of the control channel. All control
  *     channel receive data will be fragmented into this size.
  * 
  *     This default size is L2CAP_MPS.
  */
#ifndef AVCTP_RX_MPS
#define AVCTP_RX_MPS L2CAP_MPS
#endif

#if AVCTP_RX_MPS < L2CAP_MINIMUM_MTU
#error AVCTP_RX_MPS must be L2CAP_MINIMUM_MTU or greater.
#endif

#if AVCTP_RX_MPS > L2CAP_MTU
#error AVCTP_RX_MPS cannot exceed L2CAP_MTU
#endif

/*---------------------------------------------------------------------------
 * AVCTP_RETRANS_TIMEOUT
 * 
 *      Defines the value used for L2CAP Enhanced Retransmistion Mode's
 *      retransmit timeout for the control channel.
 * 
 *      The default value is 2000.
 */
#ifndef AVCTP_RETRANS_TIMEOUT
#define AVCTP_RETRANS_TIMEOUT 2000
#endif

/*---------------------------------------------------------------------------
 * AVCTP_MONITOR_TIMEOUT
 * 
 *      Defines the value used for L2CAP Enhanced Retransmistion Mode's
 *      monitor timeout for the control channel.
 * 
 *      The default value is 12000.
 */
#ifndef AVCTP_MONITOR_TIMEOUT
#define AVCTP_MONITOR_TIMEOUT 12000
#endif

#if AVCTP_MONITOR_TIMEOUT < 12000
#error AVCTP_MONITOR_TIMEOUT must be at least 12000.
#endif

 /*--------------------------------------------------------------------------
  * AVCTP_BROWSE_MAX_MTU contant
  * 
  *     Defines the receive buffer size of the browsing channel.  All
  *     browsing channel responses from the target will be limited by this
  *     size.  This value must be 335 or greater.
  * 
  *     This default size is L2CAP_MTU.
  */
#ifndef AVCTP_BROWSE_MAX_MTU
#define AVCTP_BROWSE_MAX_MTU L2CAP_MTU
#endif

#if AVCTP_BROWSE_MAX_MTU < 335
#error AVCTP_BROWSE_MAX_MTU must be 335 or greater.
#endif

#if AVCTP_BROWSE_MAX_MTU > L2CAP_MTU
#error AVCTP_BROWSE_MAX_MTU cannot exceed L2CAP_MTU
#endif

 /*--------------------------------------------------------------------------
  * AVCTP_BROWSE_TX_MPS contant
  * 
  *     Defines the maximum transmit mps fragment size of the browsing
  *     channel. All browsing channel transmissions will be fragmented into
  *     this size.
  * 
  *     This default size is L2CAP_MINIMUM_MTU.
  */
#ifndef AVCTP_BROWSE_TX_MPS
#define AVCTP_BROWSE_TX_MPS L2CAP_MINIMUM_MTU
#endif

#if AVCTP_BROWSE_TX_MPS < L2CAP_MINIMUM_MTU
#error AVCTP_BROWSE_TX_MPS must be L2CAP_MINIMUM_MTU or greater.
#endif

#if AVCTP_BROWSE_TX_MPS > L2CAP_MTU
#error AVCTP_BROWSE_TX_MPS cannot exceed L2CAP_MTU
#endif

 /*--------------------------------------------------------------------------
  * AVCTP_BROWSE_RX_MPS contant
  * 
  *     Defines the mps fragment size of the browsing channel. All browsing
  *     channel receive data will be fragmented into this sizee.
  * 
  *     This default size is L2CAP_MPS.
  */
#ifndef AVCTP_BROWSE_RX_MPS
#define AVCTP_BROWSE_RX_MPS L2CAP_MPS
#endif

#if AVCTP_BROWSE_RX_MPS < L2CAP_MINIMUM_MTU
#error AVCTP_BROWSE_RX_MPS must be L2CAP_MINIMUM_MTU or greater.
#endif

#if AVCTP_BROWSE_RX_MPS > L2CAP_MTU
#error AVCTP_BROWSE_RX_MPS cannot exceed L2CAP_MTU
#endif

/*---------------------------------------------------------------------------
 * AVCTP_BROWSE_RETRANS_TIMEOUT
 * 
 *      Defines the value used for L2CAP Enhanced Retransmistion Mode's
 *      retransmit timeout for the browsing channel.
 * 
 *      The default value is 2000.
 */
#ifndef AVCTP_BROWSE_RETRANS_TIMEOUT
#define AVCTP_BROWSE_RETRANS_TIMEOUT 2000
#endif

/*---------------------------------------------------------------------------
 * AVCTP_BROWSE_MONITOR_TIMEOUT
 * 
 *      Defines the value used for L2CAP Enhanced Retransmistion Mode's
 *      monitor timeout for the browsing channel.
 * 
 *      The default value is 12000.
 */
#ifndef AVCTP_BROWSE_MONITOR_TIMEOUT
#define AVCTP_BROWSE_MONITOR_TIMEOUT 12000
#endif

#if AVCTP_BROWSE_MONITOR_TIMEOUT < 12000
#error AVCTP_BROWSE_MONITOR_TIMEOUT must be at least 12000.
#endif

/* The minimum MTU on the browsing channel */
#define AVCTP_BROWSE_MIN_MTU 335

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AvctpEvent type
 *      
 */
typedef U8 AvctpEvent;

/** The transport layer is connected and commands/responses can now
 * be exchanged.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVCTP_EVENT_CONNECT             1

/** A remote device is attempting to connect the transport layer.
 *  Only the acceptor of the connection is notified.  The acceptor must
 *  call AVCTP_ConnectRsp() to either accept or reject the connection.
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVCTP_EVENT_CONNECT_IND         2

/** The transport layer been disconnected. 
 * 
 *  During this callback, the 'p.remDev' parameter is valid.
 */
#define AVCTP_EVENT_DISCONNECT          3

/** A command was received from the remote AVCTP device (controller).
 * 
 *  During this callback, the 'p.cmdFrame' parameter is valid. It contains the
 *  the AVCTP command header information, including operands. If the "more"
 *  value is TRUE then this event only signals the first part of the operands.
 *  Subsequent AVCTP_EVENT_OPERANDS events will follow this event with
 *  additional operand data.
 */
#define AVCTP_EVENT_COMMAND             4

/** A response was received from the remote AVCTP device (target).
 * 
 * During this callback, the 'p.rspFrame' parameter is valid. It contains the
 * the AVCTP response header information, including operands. If the "more"
 * value is TRUE then this event only signals the first part of the operands.
 * Subsequent AVCTP_EVENT_OPERANDS events will follow this event with
 * additional operand data.
 */
#define AVCTP_EVENT_RESPONSE            5

/** The remote device (target) rejected the AVCTP command.
 * 
 * During this callback, the 'p.rspFrame' parameter is valid. It contains the
 * the AVCTP reject header information, including operands. If the "more"
 * value is TRUE then this event only signals the first part of the operands.
 * Subsequent AVCTP_EVENT_OPERANDS events will follow this event with
 * additional operand data.
 */
#define AVCTP_EVENT_REJECT              6

/** A command (see AVCTP_SendCommand) or response (see AVCTP_SendResponse)
 * has been sent. Memory allocated for the operation can be freed or reused
 * after receiving this event.
 * 
 * During this callback, the 'p.cmdFrame' or 'p.rspFrame' parameter associated
 * with the sent command or response is valid. In addition, "status" will be
 * set to indicate "BT_STATUS_SUCCESS" or "BT_STATUS_FAILED" to indicate
 * whether the event was properly delivered.
 */
#define AVCTP_EVENT_TX_DONE             8

/** Additional operand data has been received for the previous
 * AVCTP_EVENT_COMMAND or AVCTP_EVENT_RESPONSE.
 * 
 * During this callback, the 'p.cmdFrame' or 'p.rspFrame' parameter associated
 * with the received command or response is valid. The "operands" and
 * "operandLen" fields indicate the chunk of operands being received for
 * the command or response. If the "more" field is set to TRUE, the full
 * operand buffer will be received in multiple _OPERANDS events and the
 * last operand buffer indicated with the "more" field set to FALSE.
 */
#define AVCTP_EVENT_OPERANDS            9

/* End of AvctpEvent */

#define AVCTP_EVENT_LAST                9

/*---------------------------------------------------------------------------
 * AvctpCtype type
 *
 * This type defines the AV/C ctype (command type) codes.
 */
typedef U8 AvctpCtype;

#define AVCTP_CTYPE_CONTROL               0x00
#define AVCTP_CTYPE_STATUS                0x01
#define AVCTP_CTYPE_SPECIFIC_INQUIRY      0x02
#define AVCTP_CTYPE_NOTIFY                0x03
#define AVCTP_CTYPE_GENERAL_INQUIRY       0x04
#define AVCTP_CTYPE_BROWSING              0x80
/* End of AvctpCtype */

/*---------------------------------------------------------------------------
 * AvctpResponse type
 *
 * This type defines the AV/C response codes.
 */
typedef U8 AvctpResponse;

#define AVCTP_RESPONSE_NOT_IMPLEMENTED    0x08
#define AVCTP_RESPONSE_ACCEPTED           0x09
#define AVCTP_RESPONSE_REJECTED           0x0A
#define AVCTP_RESPONSE_IN_TRANSITION      0x0B
#define AVCTP_RESPONSE_IMPLEMENTED_STABLE 0x0C
#define AVCTP_RESPONSE_CHANGED            0x0D
#define AVCTP_RESPONSE_INTERIM            0x0F
#define AVCTP_RESPONSE_BROWSING           0x40
/* End of AvctpResponse */

/*---------------------------------------------------------------------------
 * AvctpOpcode type
 *
 * This type defines the AV/C Opcodes.
 */
typedef U8 AvctpOpcode;

#define AVCTP_OPCODE_VENDOR_DEPENDENT     0x00
#define AVCTP_OPCODE_UNIT_INFO            0x30
#define AVCTP_OPCODE_SUBUNIT_INFO         0x31
#define AVCTP_OPCODE_PASS_THROUGH         0x7C
/* End of AvctpOpcode */


/* Forward references */
typedef struct _AvctpCallbackParms AvctpCallbackParms;
typedef struct _AvctpChannel AvctpChannel;
typedef struct _AvctpCmdFrame AvctpCmdFrame;
typedef struct _AvctpRspFrame AvctpRspFrame;

/*---------------------------------------------------------------------------
 * AvctpCallback type
 *
 * A function of this type is called to indicate events to the application.
 */
typedef void (*AvctpCallback)(AvctpChannel *chnl, AvctpCallbackParms *Parms);

/* End of AvctpCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AvctpCmdFrame structure
 *
 * Defines the parameters required for an AVCTP command.
 */
struct _AvctpCmdFrame {
    ListEntry   node;           /* Used internally by AVCTP. */
    U8          transId;        /* Transaction ID */
    AvctpCtype  ctype;          /* 4 bits */
    U8          subunitType;    /* 5 bits */
    U8          subunitId;      /* 3 bits */
    AvctpOpcode opcode;         /* 8 bits */
    U8          headerLen;      /* Header length */
    U8          header[6];      /* Header information */
    U16         operandLen;     /* Length of buffer in "operands" */
    U8         *operands;       /* Buffer containing the command data */
    BOOL        more;           /* Indicates whether to expect additional
                                 * frames containing more operand data. */
    AvtpPacket  avtpPacket;     /* For sending over AVTP */
    EvmTimer    timer;          /* Timer for the command */
};

/*---------------------------------------------------------------------------
 * AvctpRspFrame structure
 *
 * Defines the parameters required for an AVCTP response.
 */
struct _AvctpRspFrame {
    ListEntry     node;           /* Used internally by AVCTP. */
    U8            transId;        /* Transaction ID */
    AvctpResponse response;       /* 4 bits */         
    U8            subunitType;    /* 5 bits */         
    U8            subunitId;      /* 3 bits */         
    AvctpOpcode   opcode;         /* 8 bits */         
    U8            headerLen;      /* Header length */
    U8            header[6];      /* Header information */
    U16           operandLen;     /* Length of buffer in "operands" */
    U8           *operands;       /* Buffer containing the response data */
    BOOL          more;           /* Indicates whether to expect additional
                                   * frames containing more operand data. */
    AvtpPacket    avtpPacket;     /* For sending over AVTP */
    EvmTimer      padding;        /* Timer for the command */
};

/*---------------------------------------------------------------------------
 * AvctpChannel structure
 *
 * Defines the AVCTP channel.
 */
struct _AvctpChannel {

    /* === Internal use only === */
    ListEntry       node;

    /* Transmit queue */
    ListEntry       txQueue;

    /* Connection Handle */
    AvctpConn       conn;

    /* Channel Handle */
    AvtpChannel     avtpChnl;

    /* Current Transmitting Command */
    void           *curCmd;

    /* Current Waiting Command ID */
    U8              curId;

    /* Current Transmitting Response */
    void           *curRsp;

    /* Rx Frame */
    union {
        AvctpCmdFrame  cmdFrame;
        AvctpRspFrame  rspFrame;
    } rx;

    /* System response packet */
    AvtpPacket         rspPacket;

    /* Transaction ID */
    U8                 txTransId;

    /* Receive State */
    U8                 rxState;

    /* Transmit State */
    U8                 txState;

    /* Application callback function */
    AvctpCallback      callback;
};

/*---------------------------------------------------------------------------
 * AvctpCallbackParms structure
 *
 * Contains information for the application callback event.
 * 
 */
struct _AvctpCallbackParms {
    /* AVCTP event */
    AvctpEvent      event;

    /* AVCTP channel associated with the event */
    AvctpChannel   *channel;

    /* Status of event (valid only for certain events)  */
    BtStatus        status;

    /* Callback parameter object, depending on "event" */
    union {
        /* Remote device associated with the event */
        BtRemoteDevice *remDev;

        /* Command frame associated with the event */
        AvctpCmdFrame  *cmdFrame;

        /* Response frame associated with the event */
        AvctpRspFrame  *rspFrame;
    } p;
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AVCTP_Init()
 *
 *     Initialize the AVCTP SDK.  This function should only be called
 *     once, normally at initialization time.  The calling of this function
 *     can be specified in overide.h using the XA_LOAD_LIST macro
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(AVCTP) ... ).
 *
 * Returns:
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL AVCTP_Init(void);


#if L2CAP_DEREGISTER_FUNC == XA_ENABLED

/*---------------------------------------------------------------------------
 * AVCTP_Deinit()
 *
 *     Deinitializes the AVCTP module. AVCTP_Init() must later be called
 *     again to use any AVCTP function call. This function does no
 *     error checking so do NOT call when AVCTP is in use.
 *
 * Requires:
 *     L2CAP_DEREGISTER_FUNC set to XA_ENABLED
 */
void AVCTP_Deinit(void);

#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 * AVCTP_Register()
 *
 *      Registers an application callback to receive AVCTP events. This 
 *      function must be called before any other AVCTP functions.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 *      psm - The type of PSM to register (BT_PSM_AVCTP)
 *
 *      callback - Identifies the application function that will be called
 *          with AVCTP events.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The AVCTP application callback Function was
 *          successfully registered.
 *
 *      BT_STATUS_IN_USE - The specified channel is already in use.
 *
 *      BT_STATUS_INVALID_PARM - The chnl or Callback parameter does not 
 *         contain a valid pointer (XA_ERROR_CHECK only), or psm is not a
 *         valid PSM value.
 */
BtStatus AVCTP_Register(AvctpChannel *chnl,
                        L2capPsmValue psm,
                        AvctpCallback callback);

/*---------------------------------------------------------------------------
 * AVCTP_Deregister()
 *
 *      De-registers the AVCTP callback. After making this call
 *      successfully, the callback specified in AVCTP_Register will
 *      receive no further events.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The AVCTP callback was successfully deregistered.
 * 
 *      BT_STATUS_IN_USE - The specified channel is still in use.
 *
 *      BT_STATUS_NOT_FOUND - An AVCTP callback was not previously registered.
 *     
 *      BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 */
BtStatus AVCTP_Deregister(AvctpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVCTP_Connect()
 * 
 *     Initiates a signal channel connection to a remote AVCTP device.  This 
 *     function is used to establish the lower layer connection (L2CAP), which 
 *     allows sending signal messages.  Only one connection can exist between
 *     two devices.
 *
 *     If the connection attempt is successful, the AVCTP_EVENT_CONNECT event
 *     will be received.  If the connection attempt is unsuccessful, the
 *     AVCTP_EVENT_DISCONNECT event will be received.
 *
 * Parameters:
 *
 *      chnl - Channel structure that receives or initiates connections.
 *
 *      remDev - A connected remote device.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         started. When the connection process is complete, the
 *         application callback will receive either the AVCTP_EVENT_CONNECT or 
 *         AVCTP_EVENT_DISCONNECT event.
 *
 *     BT_STATUS_IN_USE - The connection already exists.
 *
 *     BT_STATUS_RESTRICTED - A connection of this type already exists with the
 *         remote device.
 *
 *     BT_STATUS_NOT_FOUND - An AVCTP channel was not previously registered.
 *
 *     BT_STATUS_INVALID_PARM - The chnl or addr parameter does not contain a 
 *         valid pointer (XA_ERROR_CHECK only).
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus AVCTP_Connect(AvctpChannel *chnl, BtRemoteDevice *RemDev);

/*---------------------------------------------------------------------------
 * AVCTP_ConnectRsp()
 * 
 *     Responds to a connection request from the remote AVCTP device.  This 
 *     function is used to establish the lower layer connection (L2CAP), 
 *     which allows sending signaling messages, such as discover, 
 *     configuration, and stream management.
 *
 * Parameters:
 *
 *     Chnl - A registered and open AVCTP channel.
 *
 *     Accept - TRUE accepts the connect or FALSE rejects the connection.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The connection responses has been successfully
 *         sent. When the connection process is complete, the application 
 *         callback will receive the AVCTP_EVENT_CONNECT event.
 *
 *     BT_STATUS_BUSY - The connection is already connected.
 *
 *     BT_STATUS_INVALID_PARM - The Chnl parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device
 *         selector database.  The device must be discovered, paired, or added
 *         manually using DS_AddDevice().  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).
 */
BtStatus AVCTP_ConnectRsp(AvctpChannel *Chnl, BOOL Accept);

/*---------------------------------------------------------------------------
 * AVCTP_Disconnect()
 *
 *     Terminates a connection with a remote AVCTP device.  The lower layer
 *     connection (L2CAP) is disconnected.
 *
 * Parameters:
 *
 *     chnl - A registered and open AVCTP channel.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The disconnect process has been successfully
 *         started. When the disconnect process is complete, the
 *         application callback will receive the AVCTP_EVENT_DISCONNECT event.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists on the specified 
 *         channel.
 *
 *     BT_STATUS_NOT_FOUND - The specified device was not found in the device
 *         selector database.  The device must be discovered, paired, or added
 *         manually using DS_AddDevice().  
 *
 *     It is possible to receive other error codes, depending on the lower 
 *     layer service in use (L2CAP or Management Entity).
 */
BtStatus AVCTP_Disconnect(AvctpChannel *chnl);

/*---------------------------------------------------------------------------
 * AVCTP_SendCommand()
 *
 *     Sends an AVCTP command on the specified channel. The channel must be
 *     connected and in the open state. The "cmdFrame" parameter must be
 *     set with valid AVCTP command parameters.
 *
 * Parameters:
 *
 *     chnl - A registered and open AVCTP channel.
 *
 *     cmdFrame - An AvctpCmdFrame structure initialized with valid
 *         AVCTP command parameters.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The send command operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_IN_USE - The command frame is alread in use on this channel.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVCTP_SendCommand(AvctpChannel *chnl, AvctpCmdFrame *cmdFrame);

/*---------------------------------------------------------------------------
 * AVCTP_SendResponse()
 *
 *     Sends an AVCTP response on the specified channel. The channel must be
 *     connected and in the open state. The "rspFrame" parameter must be
 *     set with valid AVCTP response parameters.
 *
 * Parameters:
 *
 *     chnl - A registered and open AVCTP channel.
 *
 *     rspFrame - An AvctpRspFrame structure initialized with valid
 *         AVCTP response parameters.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The send response operation has been started
 *         successfully. When the associated packet has been sent,
 *         the application callback will receive the AVCTP_EVENT_TX_DONE
 *         event.
 *
 *     BT_STATUS_IN_USE - The response frame is alread in use on this channel.
 *
 *     BT_STATUS_INVALID_PARM - The chnl parameter does not contain a valid 
 *         pointer. (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified channel is not registered.
 */
BtStatus AVCTP_SendResponse(AvctpChannel *chnl, AvctpRspFrame *rspFrame);

/*---------------------------------------------------------------------------
 * AVCTP_GetRemoteDevice()
 *
 *     Returns a pointer to the current remote device.
 *
 * Parameters:
 *
 *     chnl - An AVCTP channel.
 *
 * Returns:
 *
 *     A pointer to a remote device.
 */
BtRemoteDevice * AVCTP_RemoteDevice(AvctpChannel *chnl);

#endif /* __AVCTP_H_ */
