/****************************************************************************
 *
 * File:
 *     $Id: avsigmgr.h 1830 2009-05-06 23:14:04Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1830 $
 *
 * Description: This file contains the API for the Signal Manager.  
 *
 * Created:     Mar 23, 2004
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

#ifndef __AVSIGMGR_H_
#define __AVSIGMGR_H_

#include "avdtp.h"

#define MS_TO_FLUSH(ms) (ms*1000/625)
#define MS_TO_DELAY(ms) (ms*10)
#define DELAY_TO_MS(d)  (d>0?d/10:0)

/* Signal Commands */
#define AVDTP_SIG_DISCOVER             0x01
#define AVDTP_SIG_GET_CAPABILITIES     0x02
#define AVDTP_SIG_SET_CONFIG           0x03
#define AVDTP_SIG_GET_CONFIG           0x04
#define AVDTP_SIG_RECONFIG             0x05
#define AVDTP_SIG_OPEN                 0x06
#define AVDTP_SIG_START                0x07
#define AVDTP_SIG_CLOSE                0x08
#define AVDTP_SIG_SUSPEND              0x09
#define AVDTP_SIG_ABORT                0x0A
#define AVDTP_SIG_SECURITY_CTRL        0x0B
#define AVDTP_SIG_GET_ALL_CAPABILITIES 0x0C
#define AVDTP_SIG_DELAYREPORT          0x0D

/* Constants */
#define MAX_NUM_STREAMS             63

/* Sig Manager Transmit States */
#define AVTP_SIG_MGR_TXSTATE_IDLE            0
#define AVTP_SIG_MGR_TXSTATE_WAIT_RSP        1

/* Sig Manager Receive States */
#define AVTP_SIG_MGR_RXSTATE_BEGIN           0
#define AVTP_SIG_MGR_RXSTATE_CONTINUE        1
#define AVTP_SIG_MGR_RXSTATE_IGNORE          2

/* AVDTP Flags */
#define AVDTP_SIG_FLAG_INITIATOR       0x0001

void AvdtpSigInit(AvdtpChannel *Chnl);
                                                                                
BtStatus AvdtpSigStreamSendSimpleCmd(AvdtpChannel *Chnl,
                                    U8 SigCmd,
                                    AvdtpStream *Stream,
                                    AvdtpStreamId StrmId);

BtStatus AvdtpSigConnect(AvdtpChannel *Chnl, BtRemoteDevice *RemDev);
#define AvdtpSigConnect(s, r) AvdtpConnect(&(s)->conn, r, FALSE, 0);

BtStatus AvdtpSigConnectRsp(AvdtpChannel *Chnl, BOOL Accept);
#define AvdtpSigConnectRsp(s, a) AvdtpConnectRsp(&(s)->conn, a, FALSE,0);

BtStatus AvdtpSigDisconnect(AvdtpChannel *Chnl);
#define AvdtpSigDisconnect(s) AvdtpDisconnect(&(s)->conn)

BOOL AvdtpSigIsConnected(AvdtpChannel *Chnl);
#define AvdtpSigIsConnected(s) AvdtpIsConnected(&(s)->conn)

BtStatus AvdtpSigStreamDiscover(AvdtpChannel *Chnl);
#define AvdtpSigStreamDiscover(s) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_DISCOVER, \
                                                0, 0)

BtStatus AvdtpSigStreamGetCapabilities(AvdtpChannel *Chnl, AvdtpStreamId StrmId);
#define AvdtpSigStreamGetCapabilities(s, strmId) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_GET_CAPABILITIES, \
                                                0, \
                                                strmId)

BtStatus AvdtpSigStreamGetAllCapabilities(AvdtpChannel *Chnl, AvdtpStreamId StrmId);
#define AvdtpSigStreamGetAllCapabilities(s, strmId) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_GET_ALL_CAPABILITIES, \
                                                0, \
                                                strmId)

BtStatus AvdtpSigDelayReport(AvdtpChannel *Chnl, 
                             AvdtpStream  *Stream, 
                             U16           delayMs);

BtStatus AvdtpSigStreamSetConfig(AvdtpChannel *Chnl, AvdtpStream *Stream,
                                 AvdtpCodec *Codec, AvdtpContentProt *Cp,
                                 BOOL delayReporting);

BtStatus AvdtpSigStreamGetConfig(AvdtpChannel *Chnl, AvdtpStream *Stream);
#define AvdtpSigStreamGetConfig(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_GET_CONFIG, \
                                                strm, \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamReconfig(AvdtpChannel *Chnl, AvdtpStream *Stream,
                                AvdtpCodec *Codec, AvdtpContentProt *Cp);

BtStatus AvdtpSigStreamReconfigRsp(AvdtpChannel *Chnl, AvdtpStream *Stream, 
                                   AvdtpError Error, AvdtpCapabilityType Type);
                                                                                
BtStatus AvdtpSigStreamOpen(AvdtpChannel *Chnl, AvdtpStream *Stream);
#define AvdtpSigStreamOpen(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_OPEN, \
                                                strm, \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamStart(AvdtpChannel *Chnl, AvdtpStream *Stream);
#define AvdtpSigStreamStart(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_START, \
                                                strm, \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamStartRsp(AvdtpChannel *Chnl, AvdtpStream *Stream,
                             AvdtpError Error);

BtStatus AvdtpSigStreamClose(AvdtpChannel *Chnl, AvdtpStream *Stream);
#define AvdtpSigStreamClose(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_CLOSE, \
                                                strm, \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamSuspend(AvdtpChannel *Chnl, AvdtpStream *Stream);
#define AvdtpSigStreamSuspend(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_SUSPEND, \
                                                strm, \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamAbort(AvdtpChannel *Chnl, AvdtpStream *Stream);
#define AvdtpSigStreamAbort(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_ABORT, \
                                                strm, \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamSecurityCtrl(AvdtpChannel *Chnl, AvdtpStream *Stream, 
                                   U16 Len, U8 *Data, U8 msgType,
                                   AvdtpError Error);

void AvdtpStrmConnCallback(AvdtpConn *Conn, AvdtpConnCallbackParms *Parms);

#endif /* __AVSIGMGR_H_ */


