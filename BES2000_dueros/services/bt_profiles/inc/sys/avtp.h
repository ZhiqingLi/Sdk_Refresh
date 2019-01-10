/****************************************************************************
 *
 * File:
 *     $Id: avtp.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains the API for encoding AVDTP and AVCTP 
 *     packets.  
 *
 * Created:     Mar 22, 2004
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

#ifndef __AVTP_H_
#define __AVTP_H_

#include "bttypes.h"
#include "l2cap.h"

/* Maximum length of a packet (Based on the maximum AVDTP/AVCTP packet) */ 
#define AVTP_MAX_LEN 522

#define AVTP_HEADER_LEN 10

/* Transmission States */ 
#define AVTP_TRANS_STATE_FIRST      0
#define AVTP_TRANS_STATE_CONTINUE   1

/* Packet Types */ 
#define AVTP_PACKET_TYPE_SINGLE     0
#define AVTP_PACKET_TYPE_START      1
#define AVTP_PACKET_TYPE_CONTINUE   2
#define AVTP_PACKET_TYPE_END        3

/* Message Types */ 
#define AVTP_MSG_TYPE_COMMAND       0
#define AVTP_MSG_TYPE_ACCEPT        2
#define AVTP_MSG_TYPE_REJECT        3

/* Events */ 
#define AVTP_EVENT_TX_DONE            0
#define AVTP_EVENT_TX_TIMEOUT         1
#define AVTP_EVENT_TX_ERROR           2
#define AVTP_EVENT_RX_IND             3
#define AVTP_EVENT_RX_UNKNOWN_PACKET  4
#define AVTP_EVENT_RX_UNKNOWN_MESSAGE 5
#define AVTP_EVENT_RX_BUFFER_UNDERRUN 6
#define AVTP_EVENT_RX_BUFFER_OVERRUN  7

/* Status Codes */ 

/* Forward reference */ 
typedef struct _AvtpChannel AvtpChannel;
typedef struct _AvtpCallbackParms AvtpCallbackParms;

/* Channel Manager Callback */ 
typedef void (*AvtpCallback)(AvtpChannel *chnl, AvtpCallbackParms *Parms);

/* Channel Packet */ 
typedef struct _AvtpPacket {
    ListEntry   node;

    U8          transId;

    U8          msgType;

    U8          msgHdrLen;
    U8          msgHdr[AVTP_HEADER_LEN];

    U8          txIdSize;
    U16         txId;

    U16         txDataLen;
    U8         *txData;
    U32_PTR     context;
} AvtpPacket;

/* Channel */ 
struct _AvtpChannel {

    U32            context;

    /* Identifier */ 
    U16            rxId;

    /* Transmit Packet */ 
    ListEntry      avPacketList;
    AvtpPacket    *curAvPacket;

    /* Transmit State */ 
    U8             txState;
    U16            offset;
    U16            packetSize;
    U8             txPacketsLeft;
                   
    /* Channel Receive State */ 
    U8             rxState;
    U8             rxPacketsLeft;

    /* Channel Resources */ 
    BtPacket       packet;
    L2capChannelId l2ChannelId;

    /* Command Timeout */ 
    TimeT          txTimeout;
    EvmTimer       txTimer;

    /* Channel Callback */ 
    AvtpCallback  callback;

};

/* Channel Callback Parameters */ 
struct _AvtpCallbackParms {

    U8          event;       /* Callback event type               */ 
    BtStatus    status;      /* Transport status                  */ 
    U8          transId;     /* Transaction ID                    */ 
    U8          msgType;     /* Message type                      */ 
    U8          pktType;     /* Packet type                       */ 
    U16         rxId;        /* RX ID                             */ 

    U8          packetsLeft; /* Number of packet still to receive */ 
    U16         len;         /* Length of the current data        */ 
    AvtpPacket *packet;      /* Pointer to the transmitted packet */ 
    U8         *data;        /* Pointer to the received data      */ 
};

/* Initialize the channel structure */ 
void AVTP_InitChannel(AvtpChannel   *chnl, 
                      L2capChannelId L2ChannelId, 
                      AvtpCallback   Callback, 
                      TimeT          Timeout);

/* Deinitializes the channel structure */ 
void AVTP_DeinitChannel(AvtpChannel *chnl);

/* Start transmission of a packet */ 
BtStatus AVTP_SendStart(AvtpChannel *chnl, AvtpPacket *avtpPacket);

/* Fail transmission of a packet */
void AVTP_SendFailed(AvtpChannel *chnl, BtStatus status);

/* Continue and check completion of a packet */ 
void AVTP_SendContinue(AvtpChannel *chnl);

/* Parse received data */ 
void AVTP_Receive(AvtpChannel *chnl, U8 *Buffer, U16 Len, U8 rxIdSize);

#endif /* __AVTP_H_ */ 

