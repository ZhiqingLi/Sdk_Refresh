/****************************************************************************
 *
 * File:
 *     $Id: avctpcon.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains the header file for the L2CAP/ACL 
 *     connection state machine for AVCTP connections.  
 *
 * Created:     Mar 31, 2004
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

#ifndef __AVCTPCON_H_
#define __AVCTPCON_H_

#include "l2cap.h"

/* Connection Flags */ 
#define AVCTP_FLAG_LINK_ACL       0x01

/* Conn Events */ 
#define AVCTP_CONN_EVENT_CONNECT     1
#define AVCTP_CONN_EVENT_CONNECT_IND 2
#define AVCTP_CONN_EVENT_DISCONNECT  3
#define AVCTP_CONN_EVENT_DATA_SENT   4
#define AVCTP_CONN_EVENT_DATA_IND    5

/* Connection States */ 
#define AVCTP_CONN_NUM_STATES     6

#define AVCTP_STATE_DISCONNECTED   0
#define AVCTP_STATE_CONN_PENDING   1
#define AVCTP_STATE_CONN_INCOMING  2
#define AVCTP_STATE_DISC_PENDING   3
#define AVCTP_STATE_DISC_INCOMING  4
#define AVCTP_STATE_CONNECTED      5

/* Forward references */ 
typedef struct _AvctpConnCallbackParms AvctpConnCallbackParms;
typedef struct _AvctpConn AvctpConn;

/* State machine function prototype */ 
typedef void (*AvctpConnState)(AvctpConn *Conn, const L2capCallbackParms *Parms);

/* Conn callback function */ 
typedef void (*AvctpConnCallback)(AvctpConn *Conn, const AvctpConnCallbackParms *Parms);

/* Conn calback parameters */ 
struct _AvctpConnCallbackParms {
    U8         event;
    U16        dataLen;
    BtStatus   status;
    union {
        BtRemoteDevice *remDev;
        U8             *data;
    } ptrs;
};

/* Connection State */ 
struct _AvctpConn {
    L2capChannelId      l2ChannelId;
    L2capPsmValue       psm;
    U8                  state;
    AvctpConnCallback   callback;
    BtRemoteDevice     *remDev;
};

/* State machine functions */ 
void AvctpConnInitStateMachine(void);
void AvctpL2Callback(L2capChannelId l2ChannelId, L2capCallbackParms *parms);
void AvctpL2BrowseCallback(L2capChannelId l2ChannelId, L2capCallbackParms *parms);

/* Function Prototypes */ 
void AvctpInitConn(AvctpConn *conn, L2capPsmValue psm, const AvctpConnCallback callback);
BtStatus AvctpConnect(AvctpConn *Conn, BtRemoteDevice *RemDev);
BtStatus AvctpConnectRsp(AvctpConn *Conn, BOOL Accept);
BtStatus AvctpDisconnect(AvctpConn *conn);
BOOL AvctpIsConnected(AvctpConn *conn);
BOOL AvctpIsPendingAccept(AvctpConn *conn);

#endif /* __AVCTPCON_H_ */ 

