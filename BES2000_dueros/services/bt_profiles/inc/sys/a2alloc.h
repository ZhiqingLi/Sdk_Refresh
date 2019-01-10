/****************************************************************************
 *
 * File:
 *     $Id: a2alloc.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains memory allocation code for the Advanced 
 *     Audio Distribution Profile (A2DP).  
 *
 * Created:     June 8, 2004
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

#ifndef __A2ALLOC_H_
#define __A2ALLOC_H_

#include "a2dp.h"
#include "avdevice.h"

/* The BtA2dpContext structure contains 
 * the global context of the A2DP SDK. 
 */ 
typedef struct _BtA2dpContext {

    AvDeviceHandler     streamHandler;
    AvdtpConfigRequest  currentCfgReq;
    
    /* Stream Registration state */ 
    U8                  srcRefCount;
    U8                  snkRefCount;
    ListEntry           streamList;

    /* Pool of StreamInfo structures used during stream discovery */ 
    A2dpStreamInfo      streamInfos[A2DP_MAX_STREAMINFOS];
    ListEntry           freeStreamInfoList;

#if A2DP_SOURCE == XA_ENABLED

    /* A2DP Source SDP Variables */ 
    SdpAttribute        a2dpSrcSdpAttribute[7];
    SdpRecord           a2dpSrcSdpRecord;

#endif /* A2DP_SOURCE == XA_ENABLED */ 

#if A2DP_SINK == XA_ENABLED

    /* A2DP Sink SDP Variables */ 
    U8                  a2dpSnkSdpSupportedFeatures[3];
    SdpAttribute        a2dpSnkSdpAttribute[7];
    SdpRecord           a2dpSnkSdpRecord;

#endif /* A2DP_SINK == XA_ENABLED */ 

} BtA2dpContext;

#if XA_CONTEXT_PTR == XA_ENABLED

extern BtA2dpContext *a2dpContext;
#define A2DP(s) (a2dpContext->s)

#else /* XA_CONTEXT_PTR == XA_ENABLED */ 

extern BtA2dpContext a2dpContext;
#define A2DP(s) (a2dpContext.s)

#endif /* XA_CONTEXT_PTR */ 

/* Function prototypes */ 
BOOL A2dpAlloc(void);

#endif /* __A2ALLOC_H_ */ 


