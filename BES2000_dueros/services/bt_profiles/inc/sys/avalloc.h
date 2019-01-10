/****************************************************************************
 *
 * File:
 *     $Id: avalloc.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains internal definitions for the Audio/Video 
 *     Distribution Transport Protocol (AVTP).  
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

#ifndef __AVALLOC_H_
#define __AVALLOC_H_

#include "avdtp.h"

/* The BtAvdtpContext structure contains the global context of AVDTP. */
typedef struct _BtAvdtpContext {
    ListEntry         streamList;
    ListEntry         chnlList;
    AvdtpConnState    connState[AVDTP_CONN_NUM_STATES];
    L2capPsm          psm;
    U16               numStreams;

    /* Packet resources */
    AvtpPacket        txPackets[AVDTP_NUM_TX_PACKETS];
    U8                txData[AVDTP_NUM_TX_PACKETS][AVTP_MAX_LEN];
    ListEntry         txPacketList;
} BtAvdtpContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern BtAvdtpContext *avdtpContext;
#define AVDTP(s) (avdtpContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtAvdtpContext avdtpContext;
#define AVDTP(s) (avdtpContext.s)
#endif /* XA_CONTEXT_PTR */

BOOL AvdtpAlloc(void);

#endif /* __AVALLOC_H_ */

