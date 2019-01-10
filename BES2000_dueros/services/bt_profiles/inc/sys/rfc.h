/***************************************************************************
 *
 * File:
 *     $Id: rfc.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains the internal header file for the rfcomm 
 *     protocol.
 *
 * Created:
 *     September 13, 1999
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

#ifndef __RFC_H
#define __RFC_H

#include "rfcomm.h"
#include "l2cap.h"
#include "eventmgr.h"

#if RFCOMM_PROTOCOL == XA_ENABLED

#define INVALID_RFCOMM_MUX 0xFF
#define INVALID_DLC_ID     0xFF
#define MAX_CMD_RSP_SIZE   11

/* Control Channel Commands */
#define PN     0x81
#define TEST   0x21
#define FCON   0xA1
#define FCOFF  0x61
#define MSC    0xE1
#define NSC    0x11
#define RPN    0x91
#define RLS    0x51

/* Credit based flow control tokens */
#define CREDIT_FLOW_REQ 0xF0
#define CREDIT_FLOW_RSP 0xE0

/* RFCOMM Control Fields and events */
#define SABM    0x3F
#define UA_F    0x73
#define DM_F    0x1F
#define DM      0x0F
#define DISC    0x53
#define UIH     0xEF
#define UIH_F   0xFF

/* Address bits */
#define EA_BIT 0x01
#define CR_BIT 0x02
#define D_BIT  0x04

/* Control Channel States */
#define CTL_STATE_IDLE           0
#define CTL_STATE_RSP_WAIT       1

typedef void (*RfState)(RfChannel *Channel, RfEvent Event);

/* RFCOMM DLC States */
#define DLC_STATE_DISCONNECTED   0
#define DLC_STATE_CONNECTED      1
#define DLC_STATE_CONN_PENDING   2
#define DLC_STATE_CONN_INCOMING  3
#define DLC_STATE_DISC_PENDING   4
#define DLC_STATE_DISC_INCOMING  5
#define DLC_STATE_NEG_PENDING    6
#define DLC_STATE_SEC_PENDING    7
#define DLC_STATE_SEC_FAILED     8

/* RFCOMM DLC Flags */
#define DLC_FLAG_ALLOCATED    0x01
#define DLC_FLAG_REQ_SENT     0x02
#define DLC_FLAG_REQ_INCOMING 0x04
#define DLC_FLAG_RPN_STATUS   0x08
#define DLC_FLAG_FCOFF        0x10
#define DLC_FLAG_CREDIT_FLOW  0x20
#define DLC_FLAG_MSC_PENDING  0x40
#define DLC_FLAG_MSC_RCVD     0x80

/* RFCOMM Mux States */
#define MUX_STATE_CLOSED         0
#define MUX_STATE_OPEN           1
#define MUX_STATE_CONN_PENDING   2
#define MUX_STATE_CONN_INCOMING  3
#define MUX_STATE_DISC_PENDING   4

/* RFCOMM Mux Flags */
#define MUX_FLAG_INITIATOR    0x01
#define MUX_FLAG_ALLOCATED    0x02
#define MUX_FLAG_FCOFF        0x04
#define MUX_FLAG_XMIT         0x08
#define MUX_FLAG_TEST_SENT    0x10
#define MUX_FLAG_TEST_RECV    0x20
#define MUX_FLAG_CMD_SENT     0x40
#define MUX_FLAG_CREDIT_FLOW  0x80

/* RFCOMM Channel Flags */
#define CHNL_FLAG_IN_USE      0x0001
#define CHNL_FLAG_USE_PENDING 0x0002
#define CHNL_FLAG_NEG_DONE    0x0008
#define CHNL_FLAG_RSP_DELAYED 0x0010
#define CHNL_FLAG_NO_RESOURCE 0x0020
#define CHNL_FLAG_CLIENT      0x0040
#define CHNL_FLAG_DISC_UA     0x0080

/* RFCOMM Server Flags */
#define SRV_FLAG_ALLOCATED 0x01

/* Timeout for a service channel that has been requested, 
 * but never allocated.
 */
#define SRV_CHNL_PENDING_TIMEOUT RF_CONNECT_TIMEOUT

/* Test pattern */
#define TEST_PATTERN  0xFE

/* RFCOMM DLC */
typedef struct _RF_DLC {
    /* Priority link */
    ListEntry       node;
    U8              flags;                 /* DLC flags                     */

    /* Control Channel Parms and Status */
    RfChannel      *channel;               /* Channel structure             */

} RF_DLC;

/* RFCOMM Multiplexer */
typedef struct _RF_MUX {
    /* Priority List */
    ListEntry       priorityList;

    /* Data Link Channels */    
    RF_DLC          dlc[NUM_RF_CHANNELS];      /* Channels for this mux         */
    U8              dlcCount;                  /* Count of open channels        */

    /* Current DLC for transmission */
    RF_DLC         *cDlc;

    /* Multiplexer Context */
    U8              cDlcId;                    /* DLC ID for the current oper.  */
    U8              sabmDlcId;                 /* DLC ID for outstanding SABM.  */
    L2capChannelId  l2ChannelId;               /* L2CAP channel ID              */
    BtRemoteDevice *remDev;                    /* Remote device                 */
    BtPacket       *txPacket;                  /* Returned application packet   */
    BtStatus        l2Status;                  /* L2CAP status for current op   */
    U8             *rxBuff;                    /* Receive buffer                */
    U16             rxLen;
    U8              flags;                     /* Mux flags                     */
    U8              state;                     /* Mux state                     */
    EvmTimer        timer;                     /* Ack timer                     */

    /* Packet pool for Control Channel requests/responses and for
     * responses to multiplexer commands.
     */
    ListEntry       freeList;                     /* List of free req/rsp packets */
    BtPacket        packets[NUM_RF_CHANNELS+3];   /* Packets for sending req/rsp  */
    U8              reqBuff[MAX_CMD_RSP_SIZE *
                            (NUM_RF_CHANNELS+3)]; /* Buffer to hold packet data   */

    ListEntry       cmdQueue;

    /* Buffer to hold received and transmitted test data */
#if RF_SEND_TEST == XA_ENABLED
    BtPacket        testCmdPacket;
    U8              testCmdData[MAX_CMD_RSP_SIZE];
    U8              testDlcId;
#endif /* RF_SEND_TEST == XA_ENABLED */

    BtPacket        testRspPacket;
    U8              testRspData[RF_DEFAULT_FRAMESIZE+2];
} RF_MUX;

/* Server Registration */
typedef struct _RF_SERVER {
    ListEntry       channelList;
    RfService      *service;
    U8              inUseCnt;
    U8              flags;
} RF_SERVER;

/* RFCOMM Context */
typedef struct _BtRfcContext {
    L2capPsm   psm;                        /* L2CAP PSM                     */
    RF_MUX     mux[NUM_BT_DEVICES];        /* Alloced RFCOMM Multiplexers   */

#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
    RfState    state[8];
#else
    RfState    state[7];
#endif

#if NUM_RF_SERVERS != 0
    RF_SERVER   servers[NUM_RF_SERVERS];   /* Table of available servers    */
#endif /* NUM_RF_SERVERS != 0 */

} BtRfcContext;

/* Some useful macros */
#define DLC(m,d)  (RFC(mux)[m].dlc[d])
#define pDLC(m,d) (&(RFC(mux)[m].dlc[d]))
#define MUX(m)    (RFC(mux)[m])

/* Function prototypes */
void RfL2Callback(U16 ChannelId, L2capCallbackParms *Info);

#if NUM_RF_SERVERS != 0
BtStatus RfAllocService(RfChannel *Channel, RfService *Service);
BtStatus RfFreeServerChannel(RfChannel *Channel, RfService *Service);
BtStatus RfFreeService(RfService *Service);
BtStatus RfGetFreeServiceChannel(U8 MuxId, U8 ServiceId, RfChannel **Channel);
#endif /* NUM_RF_SERVERS != 0 */

BtStatus RfAllocMux(bool isHandleConnInd, BtRemoteDevice *RemDev, L2capChannelId L2ChannelId, U8 *MuxId);
void RfFreeMux(U8 MuxId);
BtStatus RfFindMuxIdByRemDev(BtRemoteDevice *RemDev, U8 *MuxId);
BtStatus RfFindMuxIdByRemDevToFree(BtRemoteDevice *RemDev, L2capChannelId channelId, U8 *MuxId);
BtStatus RfAllocDLC(U8 MuxId, U8 Dlci, RfChannel *Channel, U8 *DlcId);
void RfFreeDLC(U8 MuxId, U8 DlcId);
BtStatus RfFindDlcIdByDlci(U8 MuxId, U8 Dlci, U8 *DlcId);
BtStatus RfSendCommand(U8 Command, U8 MuxId, U8 DlcId);
BtStatus RfSendAck(U8 Response, U8 MuxId, U8 Dlci);
void RfAppCallback(RfChannel *channel, BtStatus status, U8 Event, void *data);
U8 RfCalcFCS(U8 *Buffer, U8 len);
BOOL RfIsValidFCS(U8 *Buffer, U8 len, U8 RxFCS);
void RfDisconnectL2CAP(U8 MuxId);
void RfTimeoutHandler(EvmTimer *timer);
void RfAdvanceNegPending(RfChannel *Channel, BOOL negOK);
void RfControlChannel(U8 MuxId, U8 Event);
BtStatus RfDisconnectDLC(U8 MuxId, U8 DlcId);
BtStatus RfSendDataInternal(U8 MuxId, U8 Dlci, BtPacket *Packet);
BtStatus RfSendParmNegotiation(RfChannel *channel, U8 CR);
BtStatus RfSendModemStatus(RfChannel *Channel, U8 Status, U8 Break, U8 CR);
BtStatus RfSendLineStatus(RfChannel *Channel, RfLineStatus Status, U8 CR);
BtStatus RfSendRemotePortNeg(RfChannel *Channel, RfPortSettings *Rpn, U8 CR);
BtStatus RfSendAggregateFlow(U8 MuxId, BOOL Flow, U8 CR);
BtStatus RfSendTest(U8 MuxId, U8 length, U8 CR);
void RfMUXCheckTxQueues(U8 MuxId);
void RfMUXCheckNextTxQueue(U8 MuxId);
BOOL RfIsValidChannel(RfChannel *Channel);
BOOL RfIsChannelAllocated(RfChannel *Channel);
void RfFreeUsePendingChannel(RfChannel *channel);
void RfCheckForCreditToSend(RfChannel *Channel);
BtStatus RfDisconnectMux(U8 MuxId);
void RfFreeChannel(RfChannel *Channel);
void RfAdvanceIncomingToOpenState(RfChannel *Channel, BOOL accepted);
void RfSendCommandPacket(U8 MuxId, BtPacket *Packet);

/* DLC States */
void RfDlcStateDisconnected(RfChannel *Channel, U8 Event);
void RfDlcStateConnected(RfChannel *Channel, U8 Event);
void RfDlcStateConnPending(RfChannel *Channel, U8 Event);
void RfDlcStateConnIncoming(RfChannel *Channel, U8 Event);
void RfDlcStateDiscPending(RfChannel *Channel, U8 Event);
void RfDlcStateDiscIncoming(RfChannel *Channel, U8 Event);
void RfDlcStateNegPending(RfChannel *Channel, U8 Event);
#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
void RfDlcStateSecPending(RfChannel *Channel, U8 Event);
#endif

#endif /* RFCOMM_PROTOCOL */
#endif /* __RFC_H */

