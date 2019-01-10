/****************************************************************************
 *
 * File:
 *     $Id: hfalloc.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains internal memory allocation definitions 
 *     for the Hands-free unit.  
 *
 * Created:     April 28, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.  
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

#ifndef __HFALLOC_H_
#define __HFALLOC_H_

#include "sys/hfi.h"
#include "atconfig.h"

/*---------------------------------------------------------------------------
 * BtHfContext structure
 *
 * The BtHfContext structure contains the global context of the Hands-free 
 * unit SDK that is shared by all its connection channels.  
 *
 */
typedef struct _BtHfContext {

    /* List of registered channels */ 
    ListEntry         channelList;

    /* RFCOMM variables */ 
    RfService         hfService;
    U8                regCount;

#if BT_SECURITY == XA_ENABLED

    BtSecurityRecord  hfSecRecIn;
    BtSecurityRecord  hfSecRecOut;

#endif /* BT_SECURITY == XA_ENABLED */ 

    /* Device Manager Variables */ 
    CmgrHandler       cmgrHandler;

    /* Hands-Free SDP Variables */ 
    U8                hfSdpProtoDescList[14];
    U8                hfSdpSupportedFeatures[3];
    SdpAttribute      hfSdpAttribute[7];
    SdpRecord         hfSdpRecord;

    /* AT Parser info */ 
    AtContext         atContext;

    /* Command Prescan */ 
    HfCmdOverride     cmdOverride;

} BtHfContext;

#if HF_MEMORY_EXTERNAL == XA_DISABLED

#if XA_CONTEXT_PTR == XA_ENABLED

extern BtHfContext *hfContext;
#define HF(s) (hfContext->s)

#else /* XA_CONTEXT_PTR == XA_ENABLED */ 

extern BtHfContext hfContext;
#define HF(s) (hfContext.s)

#endif /* XA_CONTEXT_PTR */ 

#endif /* HF_MEMORY_EXTERNAL == XA_DISABLED */ 

BOOL HfAlloc(void);

#endif /* __HFALLOC_H_ */ 

