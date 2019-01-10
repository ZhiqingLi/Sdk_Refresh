/****************************************************************************
 *
 * File:
 *     $Id: avctpi.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains internal definitions for the Audio/Video 
 *     Remote Control Transport Protocol (AVCTP).  
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

#ifndef __AVCTPI_H_
#define __AVCTPI_H_

#include "avctp.h"

/* Message handler Transmit States */
#define AVCTP_MSG_TXSTATE_IDLE            0
#define AVCTP_MSG_TXSTATE_SEND            1

/* Message handler Receive States */
#define AVCTP_MSG_RXSTATE_BEGIN           0
#define AVCTP_MSG_RXSTATE_CONTINUE        1

/* The BtAvctpContext structure contains the global context of AVDTP. */
typedef struct _BtAvctpContext {
    ListEntry       chnlList;
    AvctpConnState  connState[AVCTP_CONN_NUM_STATES];
    L2capPsm        psm;
    L2capPsm        brwsPsm;
} BtAvctpContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern BtAvctpContext *avctpContext;
#define AVCTP(s) (avctpContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtAvctpContext avctpContext;
#define AVCTP(s) (avctpContext.s)
#endif /* XA_CONTEXT_PTR */

BOOL AvctpAlloc(void);
BtStatus AvctpMsgSendCommand(AvctpChannel *chnl);
void AvtpMsgInit(AvctpChannel *Chnl, L2capPsmValue psm);
BtStatus AvctpSendNextFrame(AvctpChannel *chnl);

#endif /* __AVCTPI_H_ */

