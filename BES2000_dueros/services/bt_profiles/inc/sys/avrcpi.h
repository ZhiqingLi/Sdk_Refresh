/****************************************************************************
 *
 * File:
 *     $Id: avrcpi.h 2752 2009-08-06 22:09:46Z joshs $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 2752 $
 *
 * Description: This file contains internal definitions for the Audio/Video 
 *     Remote Control Profile (AVRCP).  
 *
 * Created:     Mar 11, 2004
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

#ifndef __AVRCPI_H_
#define __AVRCPI_H_

#include "avrcp.h"

/* SDP query info */ 
extern const U8 AvrcpServiceSearchAttribReq[AVRCP_NUM_SDP_SEARCH_ATTRIBUTES];

/* The BtAvrcpContext structure contains the global context of AVDTP. */ 
typedef struct _BtAvrcpContext {
    AvrcpRole         role;
    U16               regChnlCount;
    SdpRecord         ctSdpRecord;
    SdpAttribute      ctSdpAttrib[AVRCP_NUM_SDP_SERVER_ATTRIBUTES];
    U16               ctChannelCount;
    SdpRecord         tgSdpRecord;
    SdpAttribute      tgSdpAttrib[AVRCP_NUM_SDP_SERVER_ATTRIBUTES];
    U16               tgChannelCount;
    BtSecurityRecord    secRec;
} BtAvrcpContext;

#if XA_CONTEXT_PTR == XA_ENABLED

extern BtAvrcpContext *avrcpContext;
#define AVRCP(s) (avrcpContext->s)

#else /* XA_CONTEXT_PTR == XA_ENABLED */ 

extern BtAvrcpContext avrcpContext;
#define AVRCP(s) (avrcpContext.s)

#endif /* XA_CONTEXT_PTR */ 

BOOL AvrcpAlloc(void);
BtStatus AvrcpRegisterSdp(AvrcpChannel *chnl);
BtStatus AvrcpDeregisterSdp(AvrcpChannel *chnl);
BtStatus AvrcpStartServiceQuery(AvrcpChannel *chnl, SdpQueryMode mode);
void AvrcpSdpCallback(const BtEvent *event);
void AvrcpReportFailedConnect(AvrcpChannel *chnl, BtRemoteDevice *remDev, BtStatus status);
U8 AvrcpGetNextTransId(AvrcpChannel *chnl);
void AvrcpDumpTransTable(AvrcpChannel *chnl);

/****************************************************************************
 *
 * AVRCP Panel Subunit Pass Through
 *
 ****************************************************************************/

#define AVRCP_PANEL_STATE_NONE       0

#define AVRCP_PANEL_STATE_C_IDLE     1
#define AVRCP_PANEL_STATE_C_PRESS    2
#define AVRCP_PANEL_STATE_C_PRESS_R  3
#define AVRCP_PANEL_STATE_C_RELEASE  4
#define AVRCP_PANEL_STATE_C_SKIP     5

#define AVRCP_PANEL_STATE_T_IDLE     101
#define AVRCP_PANEL_STATE_T_PRESS    102
#define AVRCP_PANEL_STATE_T_HOLD     103

/* Need an invalid operation code for logic to work properly */ 
#define AVRCP_POP_INVALID 0x00FF

/* An invalid transaction ID code to indicate failure in some cases */
#define AVRCP_INVALID_TRANS_ID 0xFF

/* Macros for acting on the Controller's operation queue */ 

/* Returns the number of elements in the op queue */ 
#define AVRCP_OpQueueLen(c) \
 ((U8)((AVRCP_SUBUNIT_OP_QUEUE_MAX + (c)->panel.Int.opWrite - (c)->panel.Int.opRead) \
  % AVRCP_SUBUNIT_OP_QUEUE_MAX))

/* Returns the number of elements free in the op queue */ 
#define AVRCP_OpQueueFree(c) \
 (AVRCP_SUBUNIT_OP_QUEUE_MAX - AVRCP_OpQueueLen(c) - 1)

/* Pushes an operation element onto the youngest end of the op queue */ 
#define AVRCP_OpQueuePush(c,o)  do { \
 (c)->panel.Int.opQueue[(c)->panel.Int.opWrite++] = o;   \
 (c)->panel.Int.opWrite %= AVRCP_SUBUNIT_OP_QUEUE_MAX; \
 } while(0)

/* Returns the p'th element from the oldest end of the operation queue */ 
#define AVRCP_OpQueueGet(c,p) \
 (c)->panel.Int.opQueue[((c)->panel.Int.opRead + (p)) % AVRCP_SUBUNIT_OP_QUEUE_MAX]

/* Advances the queue to the p'th next element from the top */ 
#define AVRCP_OpQueueAdvance(c,p) \
 (c)->panel.Int.opRead = (U8)(((c)->panel.Int.opRead + (p)) % AVRCP_SUBUNIT_OP_QUEUE_MAX)

#define AVRCP_POP_RELEASE 0x0080

void AvrcpInitPanel(AvrcpChannel *chnl);
void AvrcpResetPanelConnection(AvrcpChannel *chnl);
BOOL AvrcpHandlePanelCmd(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
BOOL AvrcpHandlePanelRsp(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
BOOL AvrcpHandlePanelOperands(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);
void AvrcpHandlePanelInput(AvrcpChannel *chnl);
BOOL AvrcpHandlePanelTxDone(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
BOOL AvrcpHandlePanelTimeout(AvrcpChannel *chnl, AvrcpCallbackParms *parms);

/****************************************************************************
 *
 * AVRCP Panel Subunit Bluetooth Specific
 *
 ****************************************************************************/

void AvrcpTxTimeout(EvmTimer *Timer);
void AvrcpInitAdvTarget(AvrcpChannel *chnl);
void AvrcpInitAdvController(AvrcpChannel *chnl);
void AvrcpResetAdvTgConnection(AvrcpChannel *chnl);
void AvrcpResetAdvCtConnection(AvrcpChannel *chnl);
BOOL AvrcpHandleAdvCmd(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
BOOL AvrcpHandleAdvRspTxDone(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
BOOL AvrcpHandleAdvCmdTxDone(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
BOOL AvrcpHandleAdvCmdOperands(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);
BOOL AvrcpHandleAdvRspOperands(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);
BOOL AvrcpHandleAdvTimeout(AvrcpChannel *chnl, AvrcpCallbackParms *parms);
BOOL AvrcpHandleAdvRsp(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);
BtStatus AvrcpFailNotification(AvrcpChannel  *chnl, AvrcpEventId   eventId, AvrcpErrorCode error);

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
BOOL AvrcpTgFindRegisteredPlayer(AvrcpChannel *chnl, AvrcpMediaPlayerItem *player, U16 *playerId);
AvrcpMediaPlayerItem * AvrcpEnumRegisteredPlayers(AvrcpChannel *chnl, U16 playerId);
#endif /* AVRCP_VERSION_1_3_ONLY == XA_DISABLED */

/****************************************************************************
 *
 * AVRCP Media Browsing 
 *
 ****************************************************************************/

BtStatus AvrcpInitBrowseChannel(AvrcpChannel *chnl);
BtStatus AvrcpDeinitBrowseChannel(AvrcpChannel *chnl);
void AvCtpBrwsCallback(AvctpChannel *Chnl, AvctpCallbackParms *Parms);

#endif /* __AVRCPI_H_ */ 

