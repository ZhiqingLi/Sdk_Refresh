/****************************************************************************
 *
 * File:
 *     $Id: rfcomm.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     RFCOMM component. 
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

#ifndef __RFCOMM_H
#define __RFCOMM_H

#include "bttypes.h"
#include "me.h"
#include "sec.h"
#include "eventmgr.h"

#if RFCOMM_PROTOCOL == XA_ENABLED

/* RFCOMM is not designed to handle L2CAP fragments */
#if L2CAP_ERTM_FRAGMENTS == XA_ENABLED
#error L2CAP_ERTM_FRAGMENTS must be disabled for RFCOMM
#endif

/*---------------------------------------------------------------------------
 * RFCOMM layer
 *
 *     RFCOMM is a protocol that emulates a number of serial ports over
 *     the L2CAP protocol. RFCOMM is based on another standard (ETSI's
 *     TS 07.10 standard, version 6.3.0) and also contains some
 *     Bluetooth-specific adaptations.
 *
 *     RFCOMM supports up to 60 multiplexed ports (each of which is
 *     known as an RFCOMM channel) for each device connection. However,
 *     the total number of channels is limited by this implementation
 *     to conserve RAM. (See the NUM_RF_CHANNELS configuration.)
 *
 *     RFCOMM may also be used to emulate RS-232 control signals and
 *     baud rate changes.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RF_MIN_FRAME_SIZE constant
 *
 *     Defines the smallest maximum frame size allowed for an RFCOMM
 *     connection. The frame size is stored in the RfChannel structure
 *     before calling RF_OpenClientChannel.  After a connection is 
 *     established, the frame size for a channel can be retrieved with 
 *     RF_FrameSize.  The frame size in RFCOMM applies to both receive
 *     and transmit data.
 *
 *     Any RF_SendData call that specifies a packet larger than the
 *     negotiated frame size will fail.
 */
#define RF_MIN_FRAME_SIZE 23


/*---------------------------------------------------------------------------
 * RF_DEFAULT_FRAMESIZE constant
 *
 *     Defines the default frame size for RFCOMM channels.
 */
#define RF_DEFAULT_FRAMESIZE 127


/*---------------------------------------------------------------------------
 * RF_DEFAULT_PRIORITY constant
 *
 *     Identifies the default priority for an RFCOMM channel. The priority
 *     for a channel is stored in the RfChannel structure before calling
 *     RF_RegisterServerChannel or RF_OpenClientChannel.
 *
 *     Higher channel priorities correspond to lower values.
 */
#define RF_DEFAULT_PRIORITY  0


/*---------------------------------------------------------------------------
 * RF_LOWEST_PRIORITY constant
 *
 *     Identifies the lowest possible priority for an RFCOMM channel.
 *     Lower channel priorities correspond to higher priority values.
 */
#define RF_LOWEST_PRIORITY   63

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RfEvent type
 *
 *     All indications and confirmations are sent through a callback
 *     function as events. The type of event will determine which
 *     fields in the RfCallbackParms structure are valid.
 */
typedef U8 RfEvent;

/** A remote device has requested a connection to a local RFCOMM service.
 * Call RF_AcceptChannel to indicate whether the channel should be opened.  
 * This call can be made during or after the callback.
 * The remote device associated with the channel is provided in "ptrs.remDev."
 */
#define RFEVENT_OPEN_IND              1

/** A channel is now open and ready for data exchange. The remote device
 *  associated with the channel is provided in "ptrs.remDev."
 */
#define RFEVENT_OPEN                  2 

/** A request to close a channel was received. The remote device
 *  associated with the channel is provided in "ptrs.remDev."
 */
#define RFEVENT_CLOSE_IND             3

/** The channel is closed. The remote device
 *  associated with the channel is provided in "ptrs.remDev."
 */
#define RFEVENT_CLOSED                4

/** Data was received from the remote device. The "dataLen" and "ptrs.data"
 * fields describe the received data.
 */
#define RFEVENT_DATA_IND              5

/** RFCOMM is finished with the data packet provided in "ptrs.packet".
 * The RFCOMM user may reuse the packet structure to send more data.
 *
 * The "status" field indicates whether the packet was sent successfully
 * (BT_STATUS_SUCCESS) or if an error occurred (BT_STATUS_NO_CONNECTION).
 */
#define RFEVENT_PACKET_HANDLED        6

#if RF_SEND_TEST == XA_ENABLED
/** The remote device has responded to a test sent by RF_SendTest.
 * The "status" field contains BT_STATUS_SUCCESS if the test succeeded,
 * or BT_STATUS_FAILED if the response did not match.
 *
 * This event is only available when RF_SEND_TEST is enabled.
 */
#define RFEVENT_TEST_CNF              7
#endif /* RF_SEND_TEST enabled */

/** The remote device has requested negotiation of port settings. The
 * requested settings are in "ptrs.portSettings". The relevant parameters
 * for this indication are identified in "ptrs.portSettings->parmMask".
 *
 * After reviewing all relevant parameters, the RFCOMM user must
 * call RF_AcceptPortSettings to indicate which parameters were accepted
 * or rejected.  Either RF_AcceptPortSettings or RF_DelayPortRsp must be 
 * called during the callback.  If RF_DelayPortRsp is called, then the
 * processing of this event can be handled outside the context of the
 * callback.  If processing is delayed, it is important that the application
 * call RF_AcceptPortSettings within 10 seconds to prevent a link disconnect
 * by the remote device.
 *  
 * If neither RF_AcceptPortSettings nor RF_DelayPortRsp are called during
 * the callback, the requested parameters are considered to be accepted
 * and a response will be generated automatically.  Port settings are only
 * used for RS232 emulation and have no real effect on the data format
 * or flow control at the RFCOMM layer.
 *
 * It is possible for a registered service to receive a port negotiation
 * before the channel is actually open (RFEVENT_OPEN). If a port
 * negotiation is never received by the application, default values
 * should be assumed: 9600 baud, 8 bits, 1 stop bit, no parity, and no
 * port flow control.
 */
#define RFEVENT_PORT_NEG_IND          8


#if RF_SEND_CONTROL == XA_ENABLED
/** A port settings negotiation request (started with RF_RequestPortSettings)
 * is now complete. During this event, the "ptrs.portSettings->parmMask"
 * field contains bits that indicate which parameters were accepted by the
 * remote device.  
 *
 * This event is only available when RF_SEND_CONTROL is enabled.
 */
#define RFEVENT_PORT_NEG_CNF          9
#endif /* RF_SEND_CONTROL enabled */

/** The remote device has requested the status of the port settings.  The
 * application must respond with its current port settings by calling
 * RF_SendPortStatus.  Either RF_SendPortStatus or RF_DelayPortRsp must be 
 * called during the callback.  If RF_DelayPortRsp is called, then the
 * processing of this event can be handled outside the context of the
 * callback.  If processing is delayed, it is important that the application
 * call RF_SendPortStatus within 10 seconds to prevent a link disconnect
 * by the remote device.  
 *
 * If the application does not respond to this event during the callback, 
 * the response will be generated  automatically with the default Serial Port 
 * Profile settings of: 9600 baud, 8 data bits, 1 stop bit, and no parity.
 */
#define RFEVENT_PORT_STATUS_IND       10

#if RF_SEND_CONTROL == XA_ENABLED
/** The remote device has responded to a request for its current port
 * status.  This happens in response to RF_RequestPortStatus.  The relevant 
 * parameters for this indication are identified in "ptrs.portSettings".
 *
 * This event is only available when RF_SEND_CONTROL is enabled.
 */
#define RFEVENT_PORT_STATUS_CNF       11
#endif /* RF_SEND_CONTROL enabled */

/** The remote device has provided modem status. The new status settings
 * are in "ptrs.modemStatus".
 */
#define RFEVENT_MODEM_STATUS_IND      12

/** The remote device has acknowledged new modem status settings. The RFCOMM
 * user sent these settings with the RF_SetModemStatus function.
 */
#define RFEVENT_MODEM_STATUS_CNF      13

#if RF_SEND_CONTROL == XA_ENABLED
/** The remote device has provided line status information. The new
 * line status settings are in "ptrs.lineStatus".
 *
 * This event is only available when RF_SEND_CONTROL is enabled.
 */
#define RFEVENT_LINE_STATUS_IND       14

/** RFCOMM has finished sending line status to the remote device. The RFCOMM
 * user sent these settings with the RF_SetLineStatus function.
 *
 * This event is only available when RF_SEND_CONTROL is enabled.
 */
#define RFEVENT_LINE_STATUS_CNF       15  

#endif /* RF_SEND_CONTROL enabled */

/** The remote device has indicated that no RFCOMM data can be processed.
 * This indication affects all RFCOMM channels.  This event will only
 * be received if credit-based flow control has not been negotiated.
 */
#define RFEVENT_FLOW_OFF_IND          16

/** The remote device has indicated that RFCOMM data can be processed again.
 * This indication affects all RFCOMM channels.  This event will only
 * be received if credit-based flow control has not been negotiated.
 */
#define RFEVENT_FLOW_ON_IND           17

/** A resource is available for use by RF_AcceptPortSettings or 
 *  RF_SendPortStatus.  If either of these functions returns 
 *  BT_STATUS_NO_RESOURCES, this event will be sent to indicate that a
 *  resource is now available.
 */
#define RFEVENT_RESOURCE_FREE         18

/** The framesize for the specified channel has changed.  The new value can
 *  be found by calling RF_FrameSize().
 */
#define RFEVENT_FRAME_SIZE_CHANGED    19

/* End of RfEvent */


/*---------------------------------------------------------------------------
 * RfSignals type
 *
 *     Specifies signal types in an RfModemStatus structure.
 */
typedef U8 RfSignals;

/* Group: Any of the following signals may be set or clear. They represent
 * standard TS 7.10 Terminology for expressing each possible signal.
 */
#define RF_FLOW  0x02   /* Set when sender is unable to receive frames. 
                         *  This is only used when credit-based flow control 
                         *  has not been negotiated. 
                         */
#define RF_RTC   0x04   /* Set when sender is ready to communicate.     */
#define RF_RTR   0x08   /* Set when sender is ready to receive data.    */
#define RF_IC    0x40   /* Set when a call is incoming.        */
#define RF_DV    0x80   /* Set when valid data is being sent.  */

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DCE would send to a DTE device.
 */
#define RF_DSR   0x04
#define RF_CTS   0x08
#define RF_RI    0x40
#define RF_CD    0x80

/* Group: The following signal names represent a subset of the TS 7.10 signals
 * above. They represent the signals that a DTE would send to a DCE device.
 */
#define RF_DTR   0x04
#define RF_RTS   0x08

/* End of RfSignals */


/*---------------------------------------------------------------------------
 * RfBaudRate type
 *
 *     Specifies the baud rate in an RfPortSettings structure. Note that
 *     the baud rate setting does not actually affect RFCOMM throughput.
 */
typedef U8 RfBaudRate;

#define RF_BAUD_2400         0x00
#define RF_BAUD_4800         0x01
#define RF_BAUD_7200         0x02
#define RF_BAUD_9600         0x03
#define RF_BAUD_19200        0x04
#define RF_BAUD_38400        0x05
#define RF_BAUD_57600        0x06
#define RF_BAUD_115200       0x07
#define RF_BAUD_230400       0x08

/* End of RfBaudRate */


/*---------------------------------------------------------------------------
 * RfDataFormat type
 *
 *     Specifies the data bits, stop bits, and parity in an RfPortSettings
 *     structure. The data bits, stop bits, and parity settings are OR'ed
 *     together in the "dataFormat" field.
 */
typedef U8 RfDataFormat;

/* Group: These values identify the number of data bits. */
#define RF_DATA_BITS_5       0x00
#define RF_DATA_BITS_6       0x02
#define RF_DATA_BITS_7       0x01
#define RF_DATA_BITS_8       0x03

/* Group: These values identify the number of stop bits. */
#define RF_STOP_BITS_1       0x00
#define RF_STOP_BITS_1_5     0x04

/* Group: These values identify whether of parity is used. */
#define RF_PARITY_NONE       0x00
#define RF_PARITY_ON         0x08

/* Group: These values identify the type of parity in use. Note that
 * these values have no effect unless parity is enabled (RF_PARITY_ON). */
#define RF_PARITY_TYPE_ODD        0x00
#define RF_PARITY_TYPE_EVEN       0x20
#define RF_PARITY_TYPE_MARK       0x10
#define RF_PARITY_TYPE_SPACE      0x30

/* Group: These masks allow you to select only the relevant bits from
 * the "dataFormat" field.
 */

/* AND with "dataFormat" for the data bits setting. */
#define RF_DATA_BITS_MASK    0x03

/* AND with "dataFormat" for the stop bits setting. */
#define RF_STOP_BITS_MASK    0x04

/* AND with "dataFormat" to determine whether parity is on.  */
#define RF_PARITY_MASK       0x08

/* AND with "dataFormat" to determine the type of parity selected. */
#define RF_PARITY_TYPE_MASK  0x30

/* End of RfDataFormat */


/*---------------------------------------------------------------------------
 * RfFlowControl type
 *
 *     Specifies the port flow control type in an RfPortSettings structure.
 *     This refers to flow control for RS232 emulation and not to the actual
 *     flow control of RFCOMM data which is controlled by either credit-based
 *     flow control (if negotiated), aggregate flow control (FCON/FCOFF), or
 *     the FC bit in the Modem Status Command.
 */
typedef U8 RfFlowControl;

#define RF_FLOW_CTRL_NONE    0x00
#define RF_XON_ON_INPUT      0x01
#define RF_XON_ON_OUTPUT     0x02
#define RF_RTR_ON_INPUT      0x04
#define RF_RTR_ON_OUTPUT     0x08
#define RF_RTC_ON_INPUT      0x10
#define RF_RTC_ON_OUTPUT     0x20

/* Group: These symbols define common combinations of port flow control
 * settings used in RS232 emulation
 */
#define RF_FLOW_RTS_CTS      (RF_RTR_ON_INPUT | RF_RTR_ON_OUTPUT)
#define RF_FLOW_DTR_DSR      (RF_RTC_ON_INPUT | RF_RTC_ON_OUTPUT)
#define RF_FLOW_XON_XOFF     (RF_XON_ON_INPUT | RF_XON_ON_OUTPUT)

/* End of RfFlowControl */



/*---------------------------------------------------------------------------
 * RfPortSettingsMask type
 *
 *     Used to specify which settings in an RfPortSettings structure are
 *     valid.  Port settings are used for RS232 emulation.
 */
typedef U16 RfPortSettingsMask;

/* The "baudRate" field is valid. */
#define RF_PARM_BAUDRATE     0x0001

/* The RF_DATA_BITS_* part of the "dataFormat" field is valid. */
#define RF_PARM_DATABITS     0x0002

/* The RF_STOP_BITS_* part of the "dataFormat" field is valid. */
#define RF_PARM_STOPBITS     0x0004

/* The RF_PARITY_NONE or RF_PARITY_ON part of the "dataFormat" field is
 * valid.
 */
#define RF_PARM_PARITY       0x0008

/* The RF_PARITY_* part of the "dataFormat" field is valid. */
#define RF_PARM_PARITY_TYPE  0x0010

/* All elements in the "dataFormat" field are valid. */
#define RF_PARM_DATA_FORMAT  (RF_PARM_DATABITS | RF_PARM_STOPBITS | \
                              RF_PARM_PARITY | RF_PARM_PARITY_TYPE)

/* The "xonChar" field is valid. */
#define RF_PARM_XON_CHAR     0x0020

/* The "xoffChar" field is valid. */
#define RF_PARM_XOFF_CHAR    0x0040

/* Group: The following fields identify whether specific bits in the 
 * "flowControl" field are valid. They are equivalent to the values
 * described in the RfFlowControl type.
 */
#define RF_PARM_XON_INPUT    0x0100
#define RF_PARM_XON_OUTPUT   0x0200
#define RF_PARM_RTR_INPUT    0x0400
#define RF_PARM_RTR_OUTPUT   0x0800
#define RF_PARM_RTC_INPUT    0x1000
#define RF_PARM_RTC_OUTPUT   0x2000

#define RF_PARM_FLOW_RTS_CTS  (RF_PARM_RTR_INPUT | RF_PARM_RTR_OUTPUT)
#define RF_PARM_FLOW_DTR_DSR  (RF_PARM_RTC_INPUT | RF_PARM_RTC_OUTPUT)
#define RF_PARM_FLOW_XON_XOFF (RF_PARM_XON_INPUT | RF_PARM_XON_OUTPUT)

/* End of RfPortSettingsMask */


/*---------------------------------------------------------------------------
 * RfLineStatus type
 *
 *     Represents the line status. Line status is set using the
 *     RF_SetLineStatus function. If the remote device changes the line
 *     status, an RfLineStatus value will be received during an
 *     RFEVENT_LINE_STATUS_IND event.
 */
typedef U8 RfLineStatus;

/* When set, one or more errors have occurred. Any of the following bits
 * may be set to indicate an error. If RF_LINE_ERROR is clear, no error
 * has occurred, regardless of the state of the other bits.
 */
#define RF_LINE_ERROR  0x01
#define RF_OVERRUN     0x02 /* Set to indicate an overrun error. */
#define RF_PARITY      0x04 /* Set to indicate a parity error. */
#define RF_FRAMING     0x08 /* Set to indicate a framing error. */

/* End of RfLineStatus */

/*---------------------------------------------------------------------------
 * RfAggregateFlow type
 *
 *     Represents a state for a particular device's aggregate RFCOMM flow
 *     control.
 */
typedef U8 RfAggregateFlow;

/* Flow control is off; the remote device has requested that no data be
 * sent to its RFCOMM channels.  This is only used when credit-based
 * flow control has not been negotiated.

 */
#define RF_FLOW_OFF  FALSE

/* Flow control is on; the remote device is allowing data on all of its
 * RFCOMM channels.  This is only used when credit-based flow control has 
 * not been negotiated.
 */
#define RF_FLOW_ON   TRUE

/* End of RfAggregateFlow */



/* Forward references for callback function */
typedef struct _RfCallbackParms RfCallbackParms;
typedef struct _RfChannel RfChannel;

/*---------------------------------------------------------------------------
 * RfCallback type
 *
 *     Represents a callback function called by RFCOMM to indicate events
 *     and data to the RFCOMM user. The event is targeted to the
 *     RFCOMM channel identified by the "channel" parameter. Information
 *     about the event is contained in the "parms" parameter.
 *
 *     A callback function is provided to RFCOMM by storing it in an
 *     RfChannel structure and calling RF_RegisterServerChannel.
 */
typedef void (*RfCallback)(RfChannel *channel, RfCallbackParms *parms);

/* End of RfCallback */

#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
/*---------------------------------------------------------------------------
 * SEC_RFCOMM_ID
 *
 * Security ID for RFCOMM access.  This value is used to initialize a security
 * record for Security Mode 4 security.  
 *
 * When registering a security record for RFCOMM, the 'id' field is set to 
 * SEC_RFCOMM_ID.
 *
 * The 'channel' field is set to the server ID on incoming connections 
 * (see RfService).  When registering a security record for incoming connections,
 * the 'level' field of the security record should use one the following flags:
 *
 *   BSL_SECURITY_L1_IN
 *   BSL_SECURITY_L2_IN
 *   BSL_SECURITY_L3_IN
 *
 * If Authorization is desired, then the following value can be OR'd together
 * with one of the values above
 *
 *   BSL_AUTHORIZATION_IN
 *
 * For outgoing connections, the 'channel' field is set to the address of the 
 * RfChannel structure that will be used to create the client connection.
 * This value must be cast to a U32.  When registering a security record for
 * an outgoing connection, the 'level' field of the security record should only
 * use the following flags:
 *
 *   BSL_SECURITY_L1_OUT
 *   BSL_SECURITY_L2_OUT
 *   BSL_SECURITY_L3_OUT
 *
 * If Authorization is desired, then the following value can be OR'd together
 * with one of the values above
 *
 *   BSL_AUTHORIZATION_OUT
 *
 * See BtSecurityToken and SEC_Register for more information.
 */
#define SEC_RFCOMM_ID (BtProtocolId)RfSecCallback
void RfSecCallback(const BtEvent *Event);
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */

/* End of RfSecCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RfPortSettings structure
 *
 *     Represents port settings for an RFCOMM channel. Port settings
 *     are used for RS232 emulation.  They are informational and have 
 *     no real effect on the data stream at the RFCOMM level.  To change 
 *     port settings for a channel, this structure is filled and provided
 *     to RFCOMM using the RF_RequestPortSettings function.
 *
 *     When the remote device changes port settings, RFCOMM provides
 *     this structure along with the RFEVENT_PORT_NEG_IND event. The
 *     RFCOMM user can then review the settings, then accept or reject
 *     them with RF_AcceptPortSettings.
 *
 *     Fields in this structure may be valid or invalid, depending on
 *     the contents of the "parmMask" field. 
 */
typedef struct _RfPortSettings
{
    RfBaudRate    baudRate;    /* Indicates the baud rate */
    RfDataFormat  dataFormat;  /* Contains data bits, stop bits, and
                                * parity settings.
                                */
    RfFlowControl flowControl; /* Indicates port flow control options */
    U8            xonChar;     /* Indicates the XON character */
    U8            xoffChar;    /* Indicates the XOFF character */

    /* Contains a bitmask of settings.
     *
     * When changing port settings with RF_RequestPortSettings, "parmMask"
     * indicates which port settings are being changed.
     *
     * During an RFEVENT_PORT_NEG_IND event, "parmMask" describes which
     * parameters the remote device sent.
     */
    RfPortSettingsMask    parmMask;
    
} RfPortSettings;


/*---------------------------------------------------------------------------
 * RfModemStatus structure
 *
 * Represents the status of V.24 signals. To send signals on a connection,
 * create and fill in this structure, then call RF_SetModemStatus.
 *
 * When the remote device sends these signals, RFCOMM generates an
 * RFEVENT_MODEM_STATUS_IND event. During this event, you can read the
 * status in the RfCallbackParms "ptrs.modemStatus" field.
 */
typedef struct _RfModemStatus
{
    RfSignals signals;  /* Contains all signals that apply to this status
                         * message.
                         */
    U8        breakLen; /* Indicates the length of the break signal in 200 ms
                         * units. If 0, no break signal was sent. Must be
                         * between 0 and 15 (inclusive).
                         */
} RfModemStatus;


/*---------------------------------------------------------------------------
 * RfCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Various fields in the structure may be valid or not, depending
 *     on the "event" field.
 */
struct _RfCallbackParms
{
    RfEvent   event;     /* Type of the RFCOMM event */
    BtStatus  status;    /* Status or error information */
    U16       dataLen;   /* Length of data (provided during RFEVENT_DATA_IND
                          * events).
                          */
    
    /* Group: The event type determines whether a field in this
     * union is valid.
     */
    union {

        /* During an RFEVENT_PACKET_HANDLED event, contains the previously
         * transmitted packet.
         */
        BtPacket       *packet;

        /* During an RFEVENT_DATA_IND event, contains the data received from the
         * remote device.
         */
        U8             *data;

        /* During an RFEVENT_MODEM_STATUS_IND event, contains modem status settings.
         */
        RfModemStatus  *modemStatus;

        /* During an RFEVENT_LINE_STATUS_IND event, contains line status settings.
         */
        RfLineStatus   *lineStatus;

        /* During an RFEVENT_PORT_NEG_IND, RFEVENT_PORT_NEG_CNF, or 
         * RFEVENT_PORT_STATUS_CNF event, contains port settings.
         */
        RfPortSettings *portSettings;

        /* During an RFEVENT_OPEN event, contains the remote device structure */
        BtRemoteDevice *remDev;
        
    } ptrs;
};


/*---------------------------------------------------------------------------
 * RfService structure
 *
 *     Represents an RFCOMM service entity. An RFCOMM service may offer
 *     one or more channels (one for each remote device it allows).
 *     These channels are added to the service with the
 *     RF_RegisterServerChannel function.
 */
typedef struct _RfService
{
    /* Identifies the ID that corresponds to this service. This ID value
     * can be set to 0 before this structure is used if the desired RFCOMM 
     * Server ID not known.  If it is set to 0, then, after the service has been 
     * registered with RFCOMM, it will contain a valid  RFCOMM Server ID.  If
     * the desired RFCOMM Server ID is known, then this ID value can be set to
     * that value before registering with RFCOMM.  In either case, make sure to
     * register this ID with SDP so that remote devices can locate this RFCOMM
     * service. This ID can also be used as the channel in * BtSecurityRecords
     * (for incoming connections only).
     */
    U8 serviceId;
} RfService;


/*---------------------------------------------------------------------------
 * RfChannel structure
 *
 *     Represents a single RFCOMM channel. The RFCOMM user must create
 *     this structure and fill in the "callback", "maxFrameSize", and
 *     "priority" fields prior to calling RF_OpenClientChannel.  Only the
 *     "callback" filed must be set when calling RF_RegisterServerChannel.
 */
struct _RfChannel
{
    ListEntry node;            /* Used internally by RFCOMM. */

    /* Callback function for channel events */
    RfCallback  callback;     
    
    /* Frame size for the channel. This value must be between
     * RF_MIN_FRAME_SIZE and RF_MAX_FRAME_SIZE.
     */
    U16         maxFrameSize;
    
    /* Priority of the channel. Channels with higher priority have smaller
     * "priority" values. Generally, data on higher priority channels is
     * sent before lower priority channels. The priority must be between
     * RF_DEFAULT_PRIORITY (0) and RF_LOWEST_PRIORITY (63).
     */
    U8          priority;

    /* User context.  In some environments the context of the application
     * during a callback is not known.  In these cases, a pointer to the
     * application's context can be stored in userContext and then
     * retrieved during the callback.
     */
    void       *userContext;

    /* === Internal use only === */
#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
    BtSecurityToken secToken;
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */
    U8              state;
    U8              muxId;            
    U8              dlcId;            
    U8              dlci;             
    U16             flags;            
    ListEntry       txQueue;          
    EvmTimer        timer;            
    U8              cmdSent;
    RfSignals       rfSignals;
    S16             rxCredit;
    U16             txCredit;
    U8              grantedCredit;
    U8              initialRxCredit;
    U16             initialFrameSize;
    EvmTimer        openTimer;
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * RF_RegisterServerChannel()
 *
 *     Registers a service with RFCOMM. After this call, a remote device
 *     may attempt to connect to the service using the channel provided.
 *
 *     RFCOMM allows connections from multiple remote devices to the same
 *     service. To achieve this, the RFCOMM user must call this function
 *     repeatedly to associate a set of RfChannel structures with the
 *     same RfService object. As each incoming connection arrives, an
 *     RfChannel structure is assigned and provided to the callback
 *     function with the RFEVENT_OPEN_IND event.
 * 
 * Parameters:
 *     channel - Channel to register as a server with the service indicated.
 *         The "callback" and "maxFrameSize" fields must be initialized
 *         before calling this function.  It is not necessary to set the
 *         "priority" field of the channel structure, since only the
 *         client can specify that value when connecting.  This structure
 *         is owned by RFCOMM until a successful call to RF_DeregisterService
 *         or RF_DeregisterServerChannel is made.
 *
 *     service - Service to register the channel with. To have RFCOMM find the
 *         first available service ID, set the "serviceId" field to 0.  If the
 *         desired service ID is known, it can be specified in the "serviceId"
 *         field. 
 *
 *         When adding new channels to an existing service, do not
 *         modify the "serviceId" field before making subsequent calls to this
 *         function. Refer to RfService for information on serviceId and how it
 *         relates to SDP and security records (BtSecurityRecord).
 *
 *         This structure is owned by RFCOMM until a successful call to
 *         RF_DeregisterService is made.
 *
 *     credit - This parameter is used to specify an initial amount of credit 
 *         that will be granted to a client upon connection.  The amount of 
 *         credit that is advanced tells the client how many RFCOMM packets it 
 *         can send before flow control is shut off from client to server.  
 *         In order to allow the client to send more data, RF_AdvanceCredit must
 *         be called.  If the remote device does not support credit-based flow 
 *         control, then the flow control is controlled automatically by the FC 
 *         bit in the modem status command.
 *
 *         See RF_AdvanceCredit for a discussion of flow control.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Service was successfully registered.
 *
 *     BT_STATUS_IN_USE - The channel is already in use by another service.
 *
 *     BT_STATUS_FAILED  - Service failed to register because the number
 *         of servers or channels would exceed the maximum (NUM_RF_SERVERS or
 *         NUM_RF_CHANNELS).
 *         
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
BtStatus RF_RegisterServerChannel(RfChannel *Channel, RfService *Service, U8 Credit);


/*---------------------------------------------------------------------------
 * RF_DeregisterServerChannel()
 *
 *     Deregisters a channel from an RFCOMM service. After this call, 
 *     the RfChannel structure may be reused or deallocated by the RFCOMM user.
 *
 * Parameters:
 *     channel - Channel to deregister from the service.
 *
 *     service - Service from which the channel will be deregistered.  The
 *         channel must be closed before making this call.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The channel was successfully deregistered.
 *
 *     BT_STATUS_FAILED - The service or channel could not be found.
 *
 *     BT_STATUS_BUSY - The channel is currently open.  The channel must
 *         be successfully closed with RF_CloseChannel before calling
 *         this function.
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_DeregisterServerChannel(RfChannel *Channel, RfService *Service);


/*---------------------------------------------------------------------------
 * RF_DeregisterService()
 *
 *     Deregisters an RFCOMM service. After this call, the RfService
 *     structure and all associated RfChannel structures may be reused or
 *     deallocated by the RFCOMM user.
 *
 *     When deregistering the service, it's a good idea to remove the
 *     corresponding SDP entry.
 *
 * Parameters:
 *     service - Service to deregister. All channels associated with the
 *         service must be disconnected (RF_CloseChannel) before the
 *         service is deregistered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The service was successfully deregistered.
 *
 *     BT_STATUS_FAILED - The service was invalid.
 *
 *     BT_STATUS_BUSY - The service has an open channel. All channels
 *         must be successfully closed with RF_CloseChannel before calling
 *         this function.
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_DeregisterService(RfService *service);


/*---------------------------------------------------------------------------
 * RF_OpenClientChannel()
 *
 *     Attempts to establish a channel with a remote service. The RFCOMM
 *     user must identify the remote service by its device and remote
 *     server ID.
 * 
 * Parameters:
 *     remDev - Identifies the remote device. 
 *
 *     serviceId - Identifies the RFCOMM server ID on the remote device.
 *         The server ID can be determined by querying the remote device's
 *         SDP server.
 *
 *     channel - Provides information about the client channel. The
 *         "callback", "maxFrameSize", and "priority" fields must
 *         be initialized prior to the call. This structure is owned by 
 *         RFCOMM until the channel is closed.
 *
 *     credit - This parameter is used to specify an initial amount of credit 
 *         that will be granted to a client upon connection.  The amount of 
 *         credit that is advanced tells the client how many RFCOMM packets it 
 *         can send before flow control is shut off from client to server.  
 *         In order to allow the client to send more data, RF_AdvanceCredit must
 *         be called.  If the remote device does not support credit-based flow 
 *         control, then the flow control is controlled automatically by the FC 
 *         bit in the modem status command.
 *
 *         See RF_AdvanceCredit for a discussion of flow control.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to open the channel was sent.
 *         If the remote device accepts the request, RFCOMM will
 *         generate an RFEVENT_OPEN event. If the channel is rejected, RFCOMM
 *         will generate an RFEVENT_CLOSED event.
 *
 *     BT_STATUS_FAILED - The L2CAP connection is not ready for RFCOMM
 *         data
 *
 *     BT_STATUS_NO_RESOURCES - The system has run out of resources
 *         and cannot open the channel.  A channel must be closed before a
 *         new channel can be opened.
 *
 *     BT_STATUS_RESTRICTED - The channel failed a security check.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_OpenClientChannel(BtRemoteDevice *RemDev, 
                              U8              ServerId,
                              RfChannel      *Channel,
                              U8              Credit);


/* Function used by the two macros below */
BtStatus RF_RespondChannel(RfChannel *channel, BOOL flag);

/*---------------------------------------------------------------------------
 * RF_AcceptChannel()
 *
 *     Accepts an incoming request in response to an RFEVENT_OPEN_IND event.
 *     This event occurs when a remote client attempts to connect to a
 *     local RFCOMM server channel. Either this function or RF_RejectChannel
 *     must be used to respond to the connection request.
 *
 * Parameters:
 *     channel - Identifies the channel to be accepted. This channel is
 *         provided to the callback function as a parameter during the
 *         RFEVENT_OPEN_IND event.
 *
 * Returns:
 *     BT_STATUS_PENDING - The accept message will be sent. The application
 *         will receive a RFEVENT_OPEN when the accept message has been sent
 *         and the channel is open.
 *
 *     BT_STATUS_FAILED - The specified channel did not have a pending
 *         connection request or the stack has run out of resources.
 *
 *     BT_STATUS_NO_CONNECTION - No L2CAP connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_AcceptChannel(RfChannel *channel);
#define RF_AcceptChannel(c) RF_RespondChannel(c, TRUE)


/*---------------------------------------------------------------------------
 * RF_RejectChannel()
 *
 *     Rejects an incoming request in response to an RFEVENT_OPEN_IND event.
 *     This event occurs when a remote client attempts to connect to a
 *     local RFCOMM server channel. Either this function or RF_AcceptChannel
 *     must be used to respond to the connection request.
 *
 * Parameters:
 *     channel - Identifies the channel to be rejected. This channel is
 *         provided to the callback function as a parameter during the
 *         RFEVENT_OPEN_IND event.
 *
 * Returns:
 *     BT_STATUS_PENDING - The rejection message has been sent. RFCOMM
 *         will provide an RFEVENT_CLOSED event when the rejection is complete.
 *
 *     BT_STATUS_FAILED - The specified channel did not have a pending
 *         connection request or the stack has run out of resources.
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_RejectChannel(RfChannel *channel);
#define RF_RejectChannel(c) RF_RespondChannel(c, FALSE)


/*---------------------------------------------------------------------------
 * RF_CloseChannel()
 *
 *     Closes a connected channel between two devices.  When the channel
 *     is closed, the RFCOMM user will receive an RFEVENT_CLOSED event.
 *
 *     If outstanding transmit packets exist when a channel is closed,
 *     they will be returned to the RFCOMM user with RFEVENT_PACKET_HANDLED
 *     events. The "status" field for these events will be set to
 *     BT_STATUS_NO_CONNECTION.  All packets are returned to the application
 *     before the RFEVENT_CLOSED event is received.
 *
 * Parameters:
 *     channel - Identifies the channel to be closed. If it is a server
 *         channel, the RFEVENT_CLOSED event indicates that the connection
 *         is closed and a new client may attempt to connect to the channel.
 *
 *         If "channel" is a client channel, RFEVENT_CLOSED indicates that
 *         the connection is closed and channel structure can be re-used
 *         or deallocated.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to close the channel will be sent.
 *         The application will receive an RFEVENT_CLOSED event when the
 *         channel is closed.
 *
 *     BT_STATUS_IN_PROGRESS - RFCOMM is already in the process of closing.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only). 
 */
BtStatus RF_CloseChannel(RfChannel *Channel);


/*---------------------------------------------------------------------------
 * RF_SendData()
 *
 *     Sends data on the specified channel. When data transmission is
 *     complete, RFCOMM generates an RFEVENT_PACKET_HANDLED event for the
 *     channel. The "status" field of the RfCallbackParms structure
 *     will indicate whether the transmission was successful.
 *
 * Parameters:
 *     channel - Identifies the channel upon which the data is sent.
 *
 *     packet - The data to be sent. This structure is owned by RFCOMM
 *         until it is returned with the RFEVENT_PACKET_HANDLED event.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The data has been queued. If the data is sent
 *         successfully, an RFEVENT_PACKET_HANDLED event will arrive with
 *         a "status" of BT_STATUS_SUCCESS.  If the transmission fails, the
 *         same event will be received with a status specifying the reason.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK).
 */
BtStatus RF_SendData(RfChannel *channel, BtPacket *packet);

#if RF_SEND_CONTROL == XA_ENABLED
/*---------------------------------------------------------------------------
 * RF_SetModemStatus()
 *
 *     Sends a modem status command to the remote device.  This function
 *     allows the application to send flow control and V.24 signals to the
 *     remote device.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the connected channel for this action.
 *
 *     modemStatus - The modem status structure.  The "signals" field should
 *         be initialized with the V.24 bits and/or flow control bit.
 *         The "breakLen" field should contain a value between 0 and 15,
 *         signifying the length of a break signal in 200 ms units.
 *         If "breakLen" is zero, no break signal is sent.
 *
 *         The "modemStatus" structure is owned by RFCOMM until either an
 *         RFEVENT_MODEM_STATUS_CNF or RFEVENT_CLOSED event is received.
 *
 *         The FC bit of the modem status is used for flow control of the
 *         channel.  Applications do not need to set this bit, however,
 *         because flow control is automatically managed using credits.
 *
 *         See RF_AdvanceCredit for a discussion of flow control.
 *
 * Returns:
 *     BT_STATUS_PENDING - The modem status will be sent to the remote
 *         device.  If the remote device receives the status, the
 *         channel's callback function will receive an RFEVENT_MODEM_STATUS_CNF
 *         event. If the operation times out, the callback function will
 *         receive an RFEVENT_CLOSED event.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SetModemStatus(RfChannel *channel, RfModemStatus *modemStatus);


/*---------------------------------------------------------------------------
 * RF_SetLineStatus()
 *
 *     Sends the line status command to the remote device.  This function
 *     allows the RFCOMM user to communicate overrun, framing, and parity
 *     errors to the remote device.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 *     lineStatus - The line status bit mask.  This value should be
 *         initialized with the appropriate line status bits set.
 *
 * Returns:
 *     BT_STATUS_PENDING - The line status will be sent to the remote
 *         device.  When the remote device receives the status, RFCOMM
 *         will generate an RFEVENT_LINE_STATUS_CNF event. If the operation
 *         times out, RFCOMM will generate RFEVENT_CLOSED.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SetLineStatus(RfChannel *channel, RfLineStatus lineStatus);


/* Function used by two macros below */
BtStatus RF_SendPortSettings(RfChannel *Channel, RfPortSettings *PortSettings);

/*---------------------------------------------------------------------------
 * RF_RequestPortSettings()
 *
 *     Requests a change to the current port settings. The caller may set
 *     only a subset of the fields in the "portSettings" parameter. The
 *     "parmMask" field of the structure identifies which fields are
 *     important.
 *
 *     After a successful call to this function, RFCOMM exchanges
 *     the parameters with the remote device on the channel. After the
 *     remote device responds, RFCOMM generates an RFEVENT_PORT_NEG_CNF event
 *     to indicate which settings were accepted or rejected. Bits in
 *     the "parmMask" field indicate whether the parameter was accepted
 *     (bit set) or rejected (bit clear).
 *
 *     If the remote device rejects some of the parameters, subsequent
 *     requests can be sent with modified parameters until a final
 *     agreement is reached.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 * 
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 *     portSettings - A pointer to an RfPortSettings structure, initialized
 *         with the desired port settings. This structure is owned by
 *         RFCOMM until either the RFEVENT_PORT_NEG_CNF or RFEVENT_CLOSED event
 *         is received.
 *
 * Returns:
 *     BT_STATUS_PENDING - The port negotiation will be sent to the
 *         remote device.  If the remote device responds to the request,
 *         RFCOMM sends an RFEVENT_PORT_NEG_CNF event. If the operation times
 *         out, RFCOMM sends an RFEVENT_CLOSED event.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_RequestPortSettings(RfChannel *channel,
                                RfPortSettings *portSettings);
#define RF_RequestPortSettings(c, p) RF_SendPortSettings(c, p)


/*---------------------------------------------------------------------------
 * RF_RequestPortStatus()
 *
 *     Requests the status of the port settings for the remote device.
 *
 *     After a successful call to this function, the remote device responds
 *     with its current port settings. 
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request will be sent to the remote device.  
 *         If the remote device responds to the request, RFCOMM sends an 
 *         RFEVENT_PORT_STATUS_CNF event. If the operation times out, RFCOMM 
 *         sends an RFEVENT_CLOSED event.  
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_RequestPortStatus(RfChannel *channel);
#define RF_RequestPortStatus(c) RF_SendPortSettings(c, (RfPortSettings *)0)


/* Function used by two macros below */
BtStatus RF_RespondPortSettings(RfChannel *Channel, 
                                RfPortSettings *PortSettings,
                                BOOL delayedResponse);

/*---------------------------------------------------------------------------
 * RF_AcceptPortSettings()
 *
 *     Accepts or rejects the port settings received during an
 *     RFEVENT_PORT_NEG_IND event.
 *
 *     This function must be called during the RFEVENT_PORT_NEG_IND callback,
 *     unless processing of the Port Negotiation has been delayed by calling
 *     RF_DelayPortRsp.  If called during the callback, the 
 *     RfPortSettings structure provided in the "ptrs.portSettings" field can 
 *     be modified and used for the "PortSettings" parameter.  Only the 
 *     "parmMask" field of the RfPortSettings structure should be modified to 
 *     indicate whether a parameter is accepted or rejected.
 *
 *     Processing of the Port Negotiation request can be delayed by calling 
 *     RF_DelayPortRsp.  This allows the application to accept the
 *     port settings outside the callback context.  If processing is
 *     delayed, it is important for the application to respond within 10 seconds,
 *     otherwise, a link disconnect may result.  The application will also
 *     be required to allocate an RfPortSettings structure, copy the port
 *     settings from the "ptrs.portSettings" field receive during the
 *     RFEVENT_PORT_NEG_IND event into that data structure, set the response
 *     mask to the appropriate value, and provide a pointer to this structure 
 *     in the "PortSettings" parameter.
 *
 *     If neither this function nor RF_DelayPortRsp are called during the
 *     callback, the stack will automatically respond to the 
 *     RFEVENT_PORT_NEG_IND by accepting the requested parameters.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 * 
 *     portSettings - Contains port settings received in the RFEVENT_PORT_NEG_IND
 *         event.  The "parmMask" field should be set with bits that indicate 
 *         which parameters are valid.  To accept a parameter, set its bit
 *         to 1. To reject a parameter, set its bit to 0.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The port settings have been accepted or rejected
 *         as indicated, and an indication will be sent to the remote
 *         device.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state or a request
 *         is already outstanding.
 *
 *     BT_STATUS_NO_RESOURCES - There are no available resources for sending
 *         this response.  When resources are free, an RFEVENT_RESOURCE_FREE
 *         event will be sent to the application callback function.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_AcceptPortSettings(RfChannel *channel,
                               RfPortSettings *portSettings);
#define RF_AcceptPortSettings(c, p) RF_RespondPortSettings(c, p, FALSE)


/*---------------------------------------------------------------------------
 * RF_SendPortStatus()
 *
 *     Sends the current port settings (specified by "portSettings") when they
 *     have been requested by the RFEVENT_PORT_STATUS_IND event. 
 *
 *     This function must be called during the RFEVENT_PORT_NEG_IND callback,
 *     unless processing of the Port Status request has been delayed by calling
 *     RF_DelayPortRsp. If called during the callback, the 
 *     RfPortSettings structure provided in the "ptrs.portSettings" field can 
 *     be modified and used for the "PortSettings" parameter.  Each field should
 *     be set to the current state of each port parameter.
 *
 *     Processing of the Port Status request can be delayed by calling 
 *     RF_DelayPortRsp.  This allows the application to report the port status 
 *     outside the callback context.  If processing is delayed, it is 
 *     important for the application to respond within 10 seconds, otherwise, a 
 *     link disconnect may result.  Also, the application is required to 
 *     allocate an RfPortSettings structure, copy the port status into that 
 *     structure, and provide a pointer to this structure in the "PortSettings" 
 *     parameter.
 *
 *     If neither this function nor RF_DelayPortRsp are called during the
 *     callback, the stack will automatically respond to the 
 *     RFEVENT_PORT_STATUS_IND by sending the default Serial Port Profile
 *     port settings of:  9600 baud, 8 data bits, 1 stop bit, no parity.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 * 
 *     portSettings - Contains the current port settings maintained by the 
 *         application or port entity.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The port settings have been accepted or rejected
 *         as indicated.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state or a request
 *         is already outstanding.
 *
 *     BT_STATUS_NO_RESOURCES - There are no available resources for sending
 *         this response.  When resources are free, an RFEVENT_RESOURCE_FREE
 *         event will be sent to the application callback function.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SendPortStatus(RfChannel *channel, RfPortSettings *portSettings);
#define RF_SendPortStatus(c, p) RF_RespondPortSettings(c, p, FALSE)


/*---------------------------------------------------------------------------
 * RF_DelayPortRsp()
 *
 *     Delays the processing of an RFEVENT_PORT_NEG_IND or an
 *     RFEVENT_PORT_STATUS_IND event so that a response may be sent outside
 *     the context of the callback.
 * 
 *     This function must be called during the RFEVENT_PORT_NEG_IND or 
 *     RFEVENT_PORT_STATUS_IND callbacks.  When this function is called during 
 *     the callback, no response will be sent to the remote device until
 *     the application calls RF_AcceptPortSettings or RF_SendPortStatus.
 *     It is important for the application to respond within 10 seconds,
 *     otherwise, a link disconnect may result.  
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The port response will be delayed.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_DelayPortRsp(RfChannel *channel);
#define RF_DelayPortRsp(c) RF_RespondPortSettings(c, 0L, TRUE)


#endif /* RF_SEND_CONTROL enabled */

#if RF_SEND_TEST == XA_ENABLED
/*---------------------------------------------------------------------------
 * RF_SendTest()
 *
 *     Sends a TEST frame. When the test result is received, RFCOMM generates
 *     an RFEVENT_TEST_CNF event. During the event, the "status" field of the
 *     RfCallbackParms structure contains the result of the test.
 *
 *     If the status is set to BT_STATUS_SUCCESS, the test was successful
 *     (a valid test response was received). If the status is
 *     BT_STATUS_FAILED, the data transmitted did not match the received
 *     data.
 *
 * Requires:
 *     RF_SEND_TEST enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 * Returns:
 *     BT_STATUS_PENDING - The test operation was started successfully. The
 *         result will be returned in the RFEVENT_TEST_CNF event. If the
 *         connection times out before the response is detected, RFEVENT_CLOSED
 *         is indicated instead.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SendTest(RfChannel *Channel);
#endif /* RF_SEND_TEST enabled */

/*---------------------------------------------------------------------------
 * RF_FrameSize()
 *
 *     Returns the maximum size allowed for a transmit or receive data.  The 
 *     value returned is only valid for an active channel. The maximum size 
 *     will vary for each channel and is based on negotiated channel parameters.
 *
 * Parameters:
 *     Channel - Identifies the channel for this action.
 *
 * Returns:
 *     U16 - The maximum number of bytes allowed for a transmit packet.
 */
U16 RF_FrameSize(RfChannel *Channel);


/*---------------------------------------------------------------------------
 * RF_AdvanceCredit()
 *
 *     Advances transmit credit to the remote device.
 *
 * Parameters:
 *     Channel - Identifies the channel for this action. The channel must
 *         be open for credit to be advanced.
 *
 *     Credit - Each credit value represents one RFCOMM packet.  Advancing n
 *         credits allows the remote device to send n packets.  Once those
 *         packets have been sent, the remote device can no longer send (flow
 *         is off).  Subsequent calls to RF_AdvanceCredit will allow the
 *         remote device to send again (flow is on).  Credits are additive, 
 *         so calling this function once with 3 credits and then with 2 credits 
 *         will grant a total of 5 credits to the remote device, allowing the
 *         remote device to send 5 RFCOMM packets.
 *
 *         It is important to know that there are two types of flow control 
 *         at the RFCOMM protocol layer.  Newer devices use "credit" based 
 *         flow control, and older devices use the FC bit in the modem status 
 *         command to control flow.  RFCOMM will always try to negotiate
 *         credit-based flow control with a remote device during the connection
 *         process. If that negotiation is successful, then RFCOMM provides
 *         a completely reliable link.  If the negotiation is not successful,
 *         the link is automatically managed by RFCOMM using MSC commands, 
 *         based on the credits granted by the application.  It is never 
 *         necessary for the application to send an MSC command to control
 *         the flow of data.
 *
 *         Since RFCOMM is running over packet-based protocols, it may not 
 *         flow the link off with great precision when forced to use MSC
 *         commands.  In this case, RFCOMM's flow control is unreliable.  
 *         It is possible for an application to receive more data packets
 *         than the credits advanced.
 * 
 *         Because of this unreliability, some buffer overflow will need to 
 *         be allocated for data that is received after the FC bit is sent. 
 *         The amount of buffer space to allocate is difficult to calculate, 
 *         so some experimentation may be required.  An application, if possible, 
 *         should use some higher layer flow control mechanism if greater 
 *         precision is required.
 *
 *         Only Bluetooth 1.0B devices (and earlier) will not negotiate
 *         credit flow control.  These devices are fairly scarce.  All 
 *         Bluetooth 1.1 compliant devices must support credit flow control.
 *
 *         Call RF_CreditFlowEnabled to determine what type of flow control
 *         has been negotiated.
 *  
 * Returns:    
 *      BT_STATUS_SUCCESS - Credit was advanced successfully.
 *
 *      BT_STATUS_FAILED - Credit could not be advanced.  Either the maximum
 *          amount of credit was exceeded, or credit flow control is not
 *          enabled.
 *
 *      BT_STATUS_NO_CONNECTION - no RFCOMM connection has been established 
 *          yet.  A connection must exist to advance credit.
 *
 *      BT_STATUS_INVALID_PARM - A parameter was invalid or the channel is
 *          not open (XA_ERROR_CHECK only).
 */
BtStatus RF_AdvanceCredit(RfChannel *Channel, U8 Credit);

/*---------------------------------------------------------------------------
 * RF_CreditFlowEnabled()
 *
 *     Identifies whether credit based flow control has been negotiated 
 *     for the current RFCOMM session or not.  Credit flow control is
 *     negotiated in RFCOMM.  If credit flow control is enabled locally and
 *     the remote device is capable of credit flow control, then this function 
 *     will return TRUE.  It must be called only after the channel is open
 *     (RFEVENT_OPEN is received).  
 *
 *     See RF_AdvanceCredit for a discussion of flow control.
 *
 * Parameters:
 *     Channel - Identifies the channel for this action.
 *
 * Returns:
 *     TRUE - credit flow is enabled.
 *
 *     FALSE - credit flow is not enabled.
 */
BOOL RF_CreditFlowEnabled(RfChannel *Channel);

/*---------------------------------------------------------------------------
 * RF_RemoteDevice()
 *
 *     Returns the remote device associated with the specified channel.
 *
 * Parameters:
 *     Channel - Identifies the channel for this action.
 *
 * Returns:
 *     BtRemoteDevice - a pointer to the remote device structure used for 
 *        the specified channel (zero if no remote device can be found).
 */
BtRemoteDevice *RF_RemoteDevice(RfChannel *Channel);


/* Added for unplugfest support */
#if UPF_TWEAKS == XA_ENABLED

#define RFEVENT_FLOW_OFF_CNF      0x44
#define RFEVENT_FLOW_ON_CNF       0x45
#define INVALID_CMD               0xDC
#define RFEVENT_INVALID_CMD_CNF   0x46
#define CHNL_FLAG_NO_CFC          0x8000

BtStatus RF_SendAggregateFlow(RfChannel *Channel, U8 state);
BtStatus RF_SendAggregateFlowOff(RfChannel *Channel);
#define RF_SendAggregateFlowOff(c) RF_SendAggregateFlow(c, RF_FLOW_OFF)
BtStatus RF_SendAggregateFlowOn(RfChannel *Channel);
#define RF_SendAggregateFlowOn(c) RF_SendAggregateFlow(c, RF_FLOW_ON)
BtStatus RF_SendBadCommand(RfChannel *Channel);
BtStatus RF_SendBadMSC(RfChannel *Channel, RfModemStatus *ModemStatus);
BtStatus RF_SendPN(RfChannel *Channel);

void RF_DisableCFC(RfChannel *Channel);
#define RF_DisableCFC(c) ((c)->flags |= CHNL_FLAG_NO_CFC)
void RF_EnableCFC(RfChannel *Channel);
#define RF_EnableCFC(c)  ((c)->flags &= ~CHNL_FLAG_NO_CFC)

#endif /* UPF_TWEAKS == XA_ENABLED */

#endif /* RFCOMM_PROTOCOL */

#endif /* __RFCOMM_H */
