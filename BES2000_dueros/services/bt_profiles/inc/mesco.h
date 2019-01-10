/****************************************************************************
 *
 * File:
 *     $Id: mesco.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Management Entity.
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

#ifndef __MESCO_H
#define __MESCO_H

#include "osapi.h"
#include "bttypes.h"
#include "me.h"

#if NUM_SCO_CONNS > 0

/*---------------------------------------------------------------------------
 * SCO Manager (SCO) layer
 *
 *     This layer is part of the Management Entity. It allows applications
 *     to establish SCO connections and exchange audio data. SCO connections
 *     are derived from existing ACL (data) connections.
 *
 *     SCO-related events may be received by an application in two different
 *     ways: 1) All SCO-related events are sent to global handlers with
 *     appropriate masks (see ME_RegisterGlobalHandler). 2) An application
 *     must register an SCO handler (SCO_RegisterHandler) that
 *     receives only those SCO events that are associated with a certain
 *     ACL link.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtScoPacketType type
 *
 *     Bluetooth SCO Packet types are the different packet types allowed on
 *     an SCO link. They can be ORed together.
 */
typedef U16 BtScoPacketType;

#define BSPT_DM1   0x0008
#define BSPT_HV1   0x0020
#define BSPT_HV2   0x0040
#define BSPT_HV3   0x0080
#define BSPT_DV    0x0100

/* End of BtScoPacketType */

/*---------------------------------------------------------------------------
 * BtEscoPktType type
 *
 *     Bluetooth eSCO Packet types are the different packet types allowed on
 *     an eSCO link. They can be ORed together.
 */
typedef U16 BtEscoPktType;

#define BESPT_HV1      0x0001
#define BESPT_HV2      0x0002
#define BESPT_HV3      0x0004
#define BESPT_EV3      0x0008
#define BESPT_EV4      0x0010
#define BESPT_EV5      0x0020
#define BESPT_NO_2_EV3 0x0040
#define BESPT_NO_3_EV3 0x0080
#define BESPT_NO_2_EV5 0x0100
#define BESPT_NO_3_EV5 0x0200

/* End of BtEscoPktType */

/*---------------------------------------------------------------------------
 * BtScoAudioSettings type
 *
 *     Bluetooth SCO Audio Settings describe the input and air formats for
 *     the SCO link. To specify audio settings, OR together one item from
 *     each group.
 */
#ifndef __ME_H
/* Note that this definition must match the one in me.h */
typedef U16 BtScoAudioSettings;
#endif /* __ME_H */

/* The default settings (equivalent to (BSVS_IN_SAMPLE_16BIT |
 * BSVS_IN_DATA_TWOS) */
//#define BSAS_DEFAULT            0x0023
#ifdef _CVSD_BYPASS_
#define BSAS_DEFAULT            0x0040
#else
#define BSAS_DEFAULT            0x0060
#endif

/* Group: Input Coding. Select one of the following values. */
#define BSAS_IN_CODING_LINEAR   0x0000   /* Linear */
#define BSAS_IN_CODING_ULAW     0x0100   /* u-law */
#define BSAS_IN_CODING_ALAW     0x0200   /* a-law */

/* Group: Input Data Format. Select one of the following values. */
#define BSAS_IN_DATA_ONES       0x0000   /* 1's complement */
#define BSAS_IN_DATA_TWOS       0x0040   /* 2's complement */
#define BSAS_IN_DATA_SM         0x0080   /* Sign-Magnitude */

/* Group: Input Sample Size (for Linear PCM only). Select one of the
 * following values. */
#define BSAS_IN_SAMPLE_8BIT     0x0000   /* 8 bit */
#define BSAS_IN_SAMPLE_16BIT    0x0020   /* 16 bit */

/* Group: Air Coding Format. Select one of the following values. */
#define BSAS_CVSD               0x0000   /* CVSD */
#define BSAS_ULAW               0x0001   /* u-LAW */
#define BSAS_ALAW               0x0002   /* A-LAW */

#if HF_VERSION_1_6 == XA_ENABLED
#define BSAS_TRANSPNT 	     0x0003  /* msbc */

#define BSAS_MSBC    0x0060
#endif

/* End of BtScoAudioSettings */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtScoTxParms structure
 */
struct _BtScoTxParms
{
    U32             transmitBandwidth;
    U32             receiveBandwidth;
    U16             maxLatency;
    U16             voiceSetting;
    U8              retransmissionEffort;
    BtEscoPktType   eScoPktType;

};

/*---------------------------------------------------------------------------
 * BtScoRxParms structure
 */
struct _BtScoRxParms
{
    U8              scoTransmissionInterval;
    U8              scoRetransmissionWindow;
    U16             scoRxPacketLen;
    U16             scoTxPacketLen;

};

/*---------------------------------------------------------------------------
 * BtScoConnect structure
 *
 *     Represents an SCO connection. An SCO connection must be added to an
 *     exiting ACL connection. 
 */
struct _BtScoConnect
{
    /* Group: The following fields are used internally by ME. */
    BtOperation     scoop; /* This must be the first item in the structure */
    ListEntry       node;
    BtRemDevState   scostate;       /* State of the SCO link */
    U16             scoHciHandle;   /* SCO Connection handle for HCI */
#if BT_SCO_HCI_DATA == XA_ENABLED
    ListEntry       scoTxQueue;
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */ 
    BtRemoteDevice *scord;          /* Link to ACL Remote Device structure */

    /* Group: The following are parameters to me operations */
        /* SCO Link type. See BtScoLinkType defines. */
    BtLinkType      scoLinkType;
        /* Create SCO connection operation. See BtScoPacketType defines. */ 
    BtScoPacketType scopt;          
        /* SCO Connection reject. */
    U8              scoRejectReason;    

    /* Group: The following fields are BT v1.2 SCO parameters. */
    BtScoTxParms    scoTxParms;
    BtScoRxParms    scoRxParms;

    /* Indicates whether we are waiting for the radio to reject an
     * outgoing connection attempt.
     */
    BOOL raceAutoFail;
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SCO_CreateLink()
 *
 *     Creates an SCO (audio) connection to a remote device. An ACL
 *     link with the device must already be established.
 *
 * Parameters:
 *     scocon - Receives a pointer to an SCO connection structure.  
 *
 *     remDev - pointer to remote device.  
 *
 *     types - For legacy SCO connection methods (before Bluetooth version 
 *         1.2), this parameter indicates the packet types allowed for the 
 *         connection.  Otherwise, this parameter is not used.  To set up 
 *         custom parameters for a SCO or an eSCO link, use SCO_SetupLink() 
 *         instead.  If your radio supports the Bluetooth 1.2 specification 
 *         or later, but you still want to use the older SCO connection 
 *         methods, be sure to define BT_SCO_USE_LEGACY_CONNECT as 
 *         XA_ENABLED.  
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has been started successfully.  A 
 *         BTEVENT_SCO_CONNECT_CNF event will be sent to the SCO handler to 
 *         indicate the status of the SCO connection.  If the operation is 
 *         successful, then the error code will be BEC_NO_ERROR.  Other 
 *         codes indicate errors.  If the remote device attempts a SCO 
 *         connection generating an incoming connection at the same time, 
 *         the outgoing connection will fail with the error 
 *         BEC_LMP_TRANS_COLLISION or BEC_REQUEST_CANCELLED.  In this case, 
 *         the application should ignore the outgoing connection error and 
 *         accept the incoming connection to resolve the conflict.  
 *
 *     BT_STATUS_FAILED - A SCO handler must be registered first.  See the 
 *         SCO_RegisterHandler function.
 *
 *     BT_STATUS_NO_RESOURCES - Too many SCO connections already in place
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - A parameter was invalid (XA_ERROR_CHECK
 *         only).
 */
BtStatus SCO_CreateLink(BtScoConnect  **scocon, 
                        BtRemoteDevice *remDev, 
                        BtScoPacketType types);

/*---------------------------------------------------------------------------
 * SCO_SetupLink()
 *
 *     Creates or modifies a SCO or eSCO connection to a remote device.  An 
 *     ACL link with the device must already be established.  
 *
 * Parameters:
 *     scocon - In the case of a new connection, receives a pointer to a 
 *         SCO connection structure.  This value must be initialized to 0
 *         when establishing a new connection.  In the case of an existing
 *         eSCO connection to be modified, is the address of a pointer to an
 *         initialized SCO connection structure.
 *
 *     remDev - pointer to remote device.
 *
 *     scoTxParms - pointer to an initialized BtScoTxParms structure.  
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has been started successfully.  A 
 *         BTEVENT_SCO_CONNECT_CNF event will be sent to the SCO handler to 
 *         indicate the status of a new eSCO connection.  A 
 *         BTEVENT_SCO_CONN_CHNG event will be sent to the SCO handler to 
 *         indicate the status of a modified eSCO connection.  If the 
 *         operation is successful then the error code will be 
 *         BEC_NO_ERROR.  Other codes indicate errors.  If the remote device 
 *         attempts a SCO connection generating an incoming connection at 
 *         the same time, the outgoing connection will fail with the error 
 *         BEC_LMP_TRANS_COLLISION or BEC_REQUEST_CANCELLED.  In this case, 
 *         the application should ignore the outgoing connection error and 
 *         accept the incoming connection to resolve the conflict.  
 *
 *     BT_STATUS_FAILED - A SCO handler must be registered first.  See the 
 *         SCO_RegisterHandler function.  
 *
 *     BT_STATUS_NO_RESOURCES - Too many SCO connections already in place
 *         (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_INVALID_PARM - A parameter was invalid (XA_ERROR_CHECK
 *         only).  
 */
BtStatus SCO_SetupLink(BtScoConnect  **scocon,
                       BtRemoteDevice *remDevice,
                       BtScoTxParms   *scoTxParms);

/*---------------------------------------------------------------------------
 * SCO_DisconnectLink()
 *
 *     Disconnects the SCO link.
 *
 * Parameters:
 *     scocon - SCO connection to disconnect.
 *
 * Returns:
 *     BT_STATUS_PENDING - the operation has started successfully.
 *         When the link is disconnected, BTEVENT_SCO_DISCONNECT will be
 *         sent to the handler(s).
 *
 *     BT_STATUS_IN_PROGRESS - A disconnect operation is already in progress.
 *
 *     BT_STATUS_BUSY - A connect operation is in progress; wait for
 *         completion of the connection process before calling this function.
 *
 *     BT_STATUS_SUCCESS - The given BtScoConnect structure is not
 *         currently connected.
 *
 *     BT_STATUS_INVALID_PARM - A parameter was invalid (XA_ERROR_CHECK
 *         only).
 */
BtStatus SCO_DisconnectLink(BtScoConnect *scocon);


#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * SCO_SendData()
 *
 *     Sends the specified audio data on the SCO link.
 *
 * Requires:
 *     BT_SCO_HCI_DATA enabled.
 *
 * Parameters:
 *     scocon - The connection over which to send data.
 *
 *     packet - The packet of data to send. After this call, the SCO
 *         manager owns the packet. When the packet has been transmitted
 *         to the host controller, BTEVENT_SCO_DATA_CNF is sent to the
 *         handler(s).
 *
 * Returns:
 *     BT_STATUS_PENDING - The packet was queued successfully.
 *
 *     BT_STATUS_NO_CONNECTION - No SCO connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus SCO_SendData(BtScoConnect *scocon, BtPacket *packet);
#endif /* BT_SCO_HCI_DATA == XA_ENABLED */ 


/*---------------------------------------------------------------------------
 * SCO_RegisterHandler()
 *
 *     Register the given SCO handler for the given remote device. Only one
 *     handler can be registered for any one ACL connection. Because multiple
 *     SCO connections may exist over a single ACL link, this handler
 *     may receive messages for up to three distinct SCO connections
 *     (BtScoConnect structures).
 *
 * Parameters:
 *     remDev - Register a handler for this ACL device connection.
 *
 *     handler - Handler to notify of all SCO events pertaining to this
 *         device. Note that this same handler may be registered as
 *         a global ME event handler. If the handler's mask allows SCO
 *         events, two copies of each SCO event will be received.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler was registered.
 *
 *     BT_STATUS_BUSY - A handler is already in place (it must be
 *         unregistered with SCO_UnregisterHandler).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus SCO_RegisterHandler(BtRemoteDevice *remDev, BtHandler *handler);


/*---------------------------------------------------------------------------
 * SCO_UnregisterHandler()
 *
 *     Unregister the given SCO handler for the given remote device. The SCO 
 *     handler is automatically unregistered after a BTEVENT_LINK_DISCONNECT 
 *     event is received by the ACL link associated with the given 
 *     BtRemoteDevice structure.
 *
 * Parameters:
 *     remDev - Remote device.
 *
 *     handler - Handler to unregister for "remDev".
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The handler was successfully unregistered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter or the specified handler
 *         was not previously registered for "remDev" (XA_ERROR_CHECK only).
 */
BtStatus SCO_UnregisterHandler(BtRemoteDevice *remDev, BtHandler *handler);


/*---------------------------------------------------------------------------
 * SCO_SetSettings()
 *
 *     Configures SCO audio settings for all voice connections.
 *     This causes a Write_Voice_Setting command to be sent to the
 *     host controller.
 *
 * Parameters:
 *     token - pointer to token containing input parameters and
 *         storage for output parameters. The token must be initialized 
 *         before calling this function as follows:
 *
 *         "token.callback" must be set to the callback function.
 *
 *         "token.vsettings" must contain the combination of bits that
 *         specify the new audio settings.
 *
 * Returns:
 *     BT_STATUS_PENDING - operation has been started successfully.
 *         When the command is complete, the handler specified in
 *         "token.callback" will receive the BTEVENT_SCO_VSET_COMPLETE
 *         event.
 *
 *     BT_STATUS_FAILED - The connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only)
 *
 *     BT_STATUS_FAILED - Command could not be completed because a
 *         SCO connection is currently open (XA_ERROR_CHECK only), or the
 *         remote device is not in a state to initiate this command.
 */
BtStatus SCO_SetSettings(MeCommandToken *token);

/*---------------------------------------------------------------------------
 * SCO_AcceptIncomingConnection()
 *
 *     Accepts the given SCO connection request from the remote device. For
 *     Bluetooth v1.2 compliant radios, the "scoTxParms" structure fields
 *     in "scoCon" must be set correctly per Bluetooth v1.2 eSCO connection
 *     procedures.
 *
 * Parameters:
 *     scoCon - Connection structure indicated during BTEVENT_SCO_CONNECT_REQ
 *         event. The BTEVENT_SCO_CONNECT_REQ event will also indicate the
 *         "scoTxParms" and "scoRxParms" structures when the link type is
 *         BLT_ESCO. The "scoTxParms" structure fields must be set to
 *         correct values when the link type is BLT_ESCO.
 *
 *     handler - The handler for this connection. This handler must have
 *         been previously registered with SCO_RegisterHandler.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection will be accepted. When the
 *         connection is up, the handler will receive the
 *         BTEVENT_SCO_CONNECT_IND event with "errCode" set to zero. If
 *         an error occurred during the connection process, "errCode"
 *         will indicate the type of error.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter or handlers did not
 *         match (XA_ERROR_CHECK only).
 *
 */
BtStatus SCO_AcceptIncomingConnection(BtScoConnect *scoCon,
                                      BtHandler *handler);

/*---------------------------------------------------------------------------
 * SCO_RejectIncomingConnection()
 *
 *     Rejects the incoming SCO connection.
 *
 * Parameters: 
 *     scoCon - Connection structure indicated during
 *         BTEVENT_SCO_CONNECT_REQ event.
 *
 *     handler - The handler for this connection. This handler must have
 *         been previously registered with SCO_RegisterHandler.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection will be rejected. The handler
 *         will receive a BTEVENT_SCO_CONNECT_IND with a non-zero "errCode"
 *         field.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter or handler did not
 *         match (XA_ERROR_CHECK only).
 */
BtStatus SCO_RejectIncomingConnection(BtScoConnect *scoCon,
                                      BtHandler *handler,
                                      BtErrorCode reason);

/*---------------------------------------------------------------------------
 * SCO_SetConnectionPacketType()
 *
 *     Changes the allowed SCO packet types for the specified SCO 
 *     connection.  
 *
 * Parameters:
 *     scocon - pointer to the SCO connection
 *
 *     packetTypes - allowed connection packet types.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.  The stack 
 *         will attempt to change the packet types for the SCO connection.  
 *
 *     BT_STATUS_BUSY - the operation failed because another operation is
 *         already in progress.
 *
 *     BT_STATUS_NO_CONNECTION - the operation failed because the SCO 
 *         connection has not been established.  
 */
BtStatus SCO_SetConnectionPacketType(BtScoConnect   *scocon,
                                     BtScoPacketType packetTypes);

/*---------------------------------------------------------------------------
 * SCO_SetErroneousDataReporting()
 *
 *     Enable/Disable Erroneous Data reporting.  When Erroneous Data
 *     Reporting is enabled, the SCO or eSCO data received during a
 *     BTEVENT_SCO_DATA_IND event will be flagged in the
 *     "scoDataInd.errFlags" field of BtEvent as follows:
 * 
 *     BSE_NO_ERROR - Valid data.
 * 
 *     BSE_PARTIAL_ERRORS - Possibly invalid data. Some of the data may have
 *         errors.
 * 
 *     BSE_NO_DATA - No data received. The data payload will be set to 0.
 * 
 *     BSE_PARTIAL_DATA - Partially lost data. Some of the data was lost and
 *          that part of the payload will be set to 0.
 * 
 * Parameters:
 *     errDataRep - Defines the desired state of Erroneous Data Reporting.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation was started successfully.  The stack
 *         will attempt to change the Erroneous Data Reporting state.  The
 *         results will be reported in the BTEVENT_SET_ERR_DATA_REPORTING_CNF
 *         event.
 * 
 *     BT_STATUS_IN_PROGRESS - The operation failed because another
 *         attempt to change the Erroneous Data Reporting state is already in
 *         progress.
 *
 *     BT_STATUS_HCI_INIT_ERR - The operation cannot be processed, because an
 *         initialization error has occurred.
 */
BtStatus SCO_SetErroneousDataReporting(BtErrDataReporting errDataRep);

/*---------------------------------------------------------------------------
 * SCO_GetErroneousDataReporting()
 *
 *     Get the current state of erroneous data reporting.
 * 
 * Returns:
 *     The current state of erroneous data reporting.
 */
BtErrDataReporting SCO_GetErroneousDataReporting(void);

#endif /* NUM_SCO_CONNS > 0 */ 
#endif /* if __MESCO_H not defined */
