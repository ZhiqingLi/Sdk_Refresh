/***************************************************************************
 *
 * File:
 *     $Id: sdpi.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains internal definitions for the Bluetooth
 *     Service Discovery Protocol.
 *
 * Created:
 *     October 18, 1999
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

#ifndef __SDPI_H
#define __SDPI_H

#include "sdp.h"
#include "utils.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SdpQueryResp type
 *
 *     Indicates the type of the response to a query during the BtCallBack.
 */
typedef U8 SdpQueryResp;

/* Indicates an error response to the query. Parameters include a two-byte
 * ErrorCode and an ErrorInfo field (currently undefined).
 *
 * Error codes are expressed using the SdpErrorCode type.
 */
#define BSQR_ERROR_RESP                 0x01

/* Indicates a successful response to a BSQR_SERVICE_SEARCH_REQ query.
 *
 * Parameters include the following:
 *
 * 1) The total number of matching service records (a 2-byte integer).
 *
 * 2) The total number of service record handles (a 2-byte integer)
 * expressed in this response. (Additional service records may require
 * a second query containing the ContinuationState parameter.)
 *
 * 3) An array of 32-bit service record handles.
 *
 * 4) The ContinuationState parameter.
 */
#define BSQR_SERVICE_SEARCH_RESP        0x03

/* Indicates a successful response to a BSQR_ATTRIB_REQ query.
 *
 * Parameters include the following:
 *
 * 1) The number of bytes in the attribute list parameter (next).
 * This is stored as a 2-byte integer and ranges between 2 and 0xFFFF.
 *
 * 2) A Data Element Sequence of attribute ID and value pairs, ascending
 * by ID. Attributes with non-null values are not included.
 *
 * 3) The ContinuationState parameter.
 */
#define BSQR_ATTRIB_RESP                0x05

/* Indicates a successful response to a BSQR_SERVICE_SEARCH_ATTRIB_REQ
 * query.
 *
 * Parameters include the following:
 *
 * 1) The number of bytes in the attribute list parameter (next).
 * This is stored as a 2-byte integer and ranges between 2 and 0xFFFF.
 *
 * 2) A Data Element Sequence, where each element corresponds to a
 * particular service record matching the original pattern.
 * Each element of the sequence is itself a Data Element Sequence of
 * attribute ID and value pairs, ascending by ID.
 *
 * 3) The ContinuationState parameter.
 */
#define BSQR_SERVICE_SEARCH_ATTRIB_RESP 0x07

/* End of SdpQueryResp */

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Number of attributes in the SDP server's SDP record.
 */
#define SDP_SERVER_ATTRIBS 3

/*---------------------------------------------------------------------------
 *
 * SDP_SERVER_TX_SIZE is the size of each transmit buffer used by the
 * SDP server. The SDP PDU header is 5 bytes.
 */
#define SDP_SERVER_TX_SIZE (SDP_SERVER_SEND_SIZE - 5)

/*---------------------------------------------------------------------------
 *
 * Service Record handle 0x00000000 is reserved for the SDP server's record.
 * Handles 0x00000001 - 0x0000ffff are reserved. Thus usable record handles 
 * start at 0x00010000
 */
#define SDP_SERV_OWN_HANDLE     0x00000000
#define SDP_SERV_FIRST_HANDLE   0x00010000
#define SDP_SERV_BOGUS_HANDLE   0xffffffff

/*---------------------------------------------------------------------------
 *
 * SDP Server's Service Database State size.  It contains a one byte header 
 * and a four byte value.
 */
#define SDP_ZERO_SERVICE_DATABASE_STATE_SIZE 5

#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *
 * Minimum size of an SDP query. Must have PDU id, transaction ID, parameter
 * len and some parameter data. The minimum parameter data is 8.
 */
#define SDP_QUERY_MIN_LEN                  13
#define SDP_ATTRIB_REQ_MIN_SIZE            12
#define SDP_SERVICE_SEARCH_MIN_SIZE         8
#define SDP_SERVICE_SEARCH_ATTRIB_MIN_SIZE 13

/*---------------------------------------------------------------------------
 *
 * Type of SDP entity - Servier or Client. 
 */
typedef U8 BtSdpType;

#define BST_SDP_SERVER 0
#define BST_SDP_CLIENT 1

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Values for sdpClientState. 
 */
#define SDP_CS_DISCONNECTED  0x00
#define SDP_CS_L2CAP_STARTED 0x01
#define SDP_CS_L2CAP_CONNECT 0x02
#define SDP_CS_L2CAP_DISCON  0x03

#if SDP_PARSING_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Values for the SDP parsing state machine. 
 */
#define SDP_PARSE_ATTRIB_LIST_HEADER    0x01
#define SDP_PARSE_ATTRIB_LIST_LEN       0x02
#define SDP_PARSE_ATTRIB_ID             0x03
#define SDP_PARSE_ATTRIB_ID_HEADER      0x04
#define SDP_PARSE_ATTRIB_ID_LIST_LEN    0x05
#define SDP_PARSE_ATTRIB_VALUE          0x06
#define SDP_SKIP_ATTRIB_VALUE           0x07
#define SDP_PARSE_RESULT_HEADER         0x08
#define SDP_PARSE_RESULT_LIST_LEN       0x09
#define SDP_PARSE_RESULT_VALUE          0x0A

/*---------------------------------------------------------------------------
 *
 * Values for the SDP parsing flags. 
 */
#define READY_PARSE                     0x00
#define READY_RESUME                    0x01
#define READY_CONT_STATE                0x02
#endif /* SDP_PARSING_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 *
 * Default SDP Query Timeout Value (10 seconds). 
 */
#define SDP_QUERY_TIMEOUT_VALUE         10000

#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * SdpServerInfo contains all the data used internally by the SDP Server and
 * server on a per device basis.
 */

typedef struct _SdpServerInfo {

    BtPacket         txPacket;

    SdpAttribute     *sdpAttrib;       /* Current SDP attribute */
    SdpRecord        *sdpRecord;       /* Current SDP record */

    BOOL             busy;             /* Indicates if info record is busy */

    /* Total amount of attribute data left to send */
    U32              totalAttribSize; 

    /* Total amount of service search attrib data to send */
    U32              totalServAttribSize; 

    U16              channel;           /* L2CAP channel for SDP server */
    U16              recMask;           /* Record mask for this device */
    U16              totalMatches;
    U16              matchesSent;
    U16              curAttribTotal;
    U16              curAttribOff;
    U8               scratchLeft;
    U8               scratchOff;
    U8               attribState;
    U8               contState;
    U8               txPacketBusy;
    U8               attribScratch[8];
    U8               txBuff[SDP_SERVER_TX_SIZE];
} SdpServerInfo;

/*---------------------------------------------------------------------------
 *
 * BtSdpServContext contains all the data used internally by the SDP Server.
 */

typedef struct {
    ListEntry       records;    /* List of SDP records */
    U32             nextHandle;
    U8              numClients; /* Number of clients connected to server */
    U8              contState;

    L2capPsm        SdpServPsm;

    /* Sdp Record Zero stuff */
    SdpAttribute    record0Attribs[SDP_SERVER_ATTRIBS];
    SdpRecord       record0;

    /* Server Database State is a U32 */
    U8              ZeroServiceDatabaseState[SDP_ZERO_SERVICE_DATABASE_STATE_SIZE];

    /* Pool of SdpServerInfo structures */
    SdpServerInfo   serverInfos[SDP_ACTIVE_CLIENTS];

    /* Is SDP locked out from SDP queries due to a record update? */
    BOOL            sdpLocked;
} BtSdpServContext;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * BtSdpClientContext contains all the data used internally by the SDP Client.
 */

typedef struct {
    L2capPsm        SdpClientPsm;

    U16             transID;    /* Transaction ID */
    U8              contState;  /* Continuation State value for BSQM_FIRST queries */
    BtEvent         btEvent;    /* Event for calling the client */
} BtSdpClientContext;
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

BtStatus SDP_Init(void);
void SDP_Deinit(void);
BOOL SdpValidateQueryParms(SdpQueryType type, const U8* parms, U16 len, 
                           U8* contState, U16 contStateLen, SdpQueryMode mode);
BtRemoteDevice* SdpFindRemDevWithSdpChannel(U16 channel, BtSdpType type);

#if SDP_CLIENT_SUPPORT == XA_ENABLED
BtStatus SdpInitClient(void);
void SdpDeinitClient(void);
#if SDP_PARSING_FUNCS == XA_ENABLED
U8 SdpParseElementHeader(U8 dataElem);
#endif /* SDP_PARSING_FUNCS == XA_ENABLED */
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

#if SDP_SERVER_SUPPORT == XA_ENABLED
void SdpDeinitServer(void);
BtStatus SdpInitServer(void);
SdpAttribute* SdpServFindMarkedAttribute(SdpRecord* record, U16 mask);
void SdpServUpdateClassOfDevice(void);
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

#endif /* __SDPI_H */
