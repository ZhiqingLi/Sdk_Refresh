/****************************************************************************
 *
 * File:
 *     $Id: hf_cmgr.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains the call state manager for the Hands-free 
 *     SDK.  
 *
 * Created:     February 11, 2005
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

#ifndef __HF_CALLMGR_H_
#define __HF_CALLMGR_H_

#if HF_USE_CALL_MANAGER == XA_ENABLED

#include "sys/hfi.h"

/* Call state indicator values */ 
#define CALL_STATUS_NONE    0
#define CALL_STATUS_ACTIVE  1

/* Call setup indicator values */ 
#define CALL_SETUP_NONE     0
#define CALL_SETUP_INCOMING 1
#define CALL_SETUP_OUTGOING 2
#define CALL_SETUP_OUTALERT 3

/* Call manager flags */ 
#define CALLMGR_FLAG_HANGUP          0x0001
#define CALLMGR_FLAG_ACCEPT          0x0002
#define CALLMGR_FLAG_CALL_OUTGOING   0x0004
#define CALLMGR_FLAG_RELEASE         0x0008
#define CALLMGR_FLAG_HOLD            0x0010
#define CALLMGR_FLAG_REJECT          0x0020
#define CALLMGR_FLAG_REPLACE         0x0040
#define CALLMGR_FLAG_ACTIVATE        0x0080
#define CALLMGR_FLAG_CONFERENCE      0x0100

#if HF_USE_RESP_HOLD == XA_ENABLED

#define CALLMGR_FLAG_RH_QUERY        0x0200
#define CALLMGR_FLAG_RH_HOLD         0x0400
#define CALLMGR_FLAG_RH_ACCEPT       0x0800
#define CALLMGR_FLAG_RH_REJECT       0x1000

#endif /* HF_USE_RESP_HOLD == XA_ENABLED */ 

#if HF_USE_IIA == XA_ENABLED

#define CALLMGR_FLAG_IIA             0x2000

#endif /* HF_USE_IIA == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 * Event Group type
 *
 *     Distinguishes groups of events handled by the call manager.  
 */
typedef U16 EventGroupType;

/** Used for CIEV indications.  
 */
#define EG_AT_INTERNAL      0x0000

/** Used for CIND polling indications.  
 */
#define EG_AT_INTERNAL_POLL 0x1000

/** Used for service connection events.  
 */
#define EG_HFEVENT          0x2000

/** Used for AT Command Processor events.  
 */
#define EG_AT_PROCESSOR     0x3000

/** Used for Internal State Machine Processor events.  
 */
#define EG_STATE_MACHINE    0x4000

/** Used for AT Command Hands Free events.  
 */
#define EG_AT_HANDSFREE     0x5000

/* End of EventGroupType */ 

/* Exported call manager functions */ 
BOOL     CallManager(          HfChannel     *Chan, 
                               EventGroupType EventGroup, 
                               U16            Event, 
                               U32            Parms);
BtStatus CallMgrAnswerCall(    HfChannel *Chan, HfCommand *Command);
BtStatus CallMgrCallHold(      HfChannel   *Chan, 
                               HfHoldAction Action, 
                               U8           Line, 
                               HfCommand   *Command);
BtStatus CallMgrHangupCall(    HfChannel *Chan, HfCommand *Command);
BtStatus CallMgrInitiateCall(  HfChannel *Chan, HfCommand *Command);

#if HF_USE_RESP_HOLD == XA_ENABLED

BtStatus CallMgrRHQuery(       HfChannel *Chan, HfCommand *Command);
BtStatus CallMgrRHCmd(         HfChannel       *Chan, 
                               HfRespHoldAction RespHoldAction, 
                               HfCommand       *Command);

#endif /* HF_USE_RESP_HOLD == XA_ENABLED */ 

#endif /* HF_USE_CALL_MANAGER == XA_ENABLED */ 

#endif /* __HF_CALLMGR_H_ */ 

