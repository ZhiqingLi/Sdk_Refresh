/***************************************************************************
 *
 * File:
 *     $Id: btalloc.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Contains the definitions for all the memory allocations
 *     (both ROM and RAM) for the Bluetooth stack.
 *
 * Created:
 *     October 19, 1999
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

#ifndef __BTALLOCH
#define __BTALLOCH

#include "sys/btstats.h"

#include "sys/mei.h"
#include "sys/host.h"
#include "sys/rxbuff.h"
#include "sys/radiomgi.h"
#include "sys/l2cap_i.h"
#if RFCOMM_PROTOCOL == XA_ENABLED
#include "sys/rfc.h"
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#include "sys/sdpi.h"

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

typedef struct {
    BtMeContext        me;
    BtHciContext       hci;   
    BtRxbuffContext    rxbf;
    BtRmgContext       rmg;
    BtL2CapContext     l2cap;
#if SDP_SERVER_SUPPORT == XA_ENABLED
    BtSdpServContext   sdps;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
    BtSdpClientContext sdpc;
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
    BtRfcContext       rfc;
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
    ListEntry          timerList;
#if XA_STATISTICS == XA_ENABLED
    BtStatistics       stats;
#endif /* XA_STATISTICS == XA_ENABLED */
} BtContext;


/****************************************************************************
 *
 * Functions and Macros
 *
 ****************************************************************************/

/* The stack context can be either a global structure or a pointer
 * to a global structure. If XA_CONTEXT_PTR is set to XA_ENABLED then the
 * the context is a pointer otherwise it is a global structure.
 */
#if XA_CONTEXT_PTR == XA_ENABLED
extern BtContext* bt;
#define BTC(s)  (bt->s)
#define MEC(s)  (bt->me.s)
#define HCC(s)  (bt->hci.s)
#define RMG(s)  (bt->rmg.s)
#define L2C(s)  (bt->l2cap.s)
#if SDP_SERVER_SUPPORT == XA_ENABLED
#define SDPS(s) (bt->sdps.s)
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
#define SDPC(s) (bt->sdpc.s)
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
#define RFC(s)  (bt->rfc.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#define RXBUF(s) (bt->rxbf.s)

#else /* XA_CONTEXT_PTR == XA_ENABLED */

extern BtContext bt;
#define BTC(s)  (bt.s)
#define MEC(s)  (bt.me.s)
#define HCC(s)  (bt.hci.s)
#define RMG(s)  (bt.rmg.s)
#define L2C(s)  (bt.l2cap.s)
#if SDP_SERVER_SUPPORT == XA_ENABLED
#define SDPS(s) (bt.sdps.s)
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
#define SDPC(s) (bt.sdpc.s)
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
#define RFC(s)  (bt.rfc.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#define RXBUF(s) (bt.rxbf.s)

#endif /* XA_CONTEXT_PTR */

BOOL BtInitMemory(void);

#endif /* __BTALLOCH */
