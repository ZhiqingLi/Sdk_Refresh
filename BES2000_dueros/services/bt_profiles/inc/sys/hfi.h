/****************************************************************************
 *
 * File:
 *     $Id: hfi.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains internal definitions for the Hands-free 
 *     SDK.  
 *
 * Created:     February 11, 2005
 *
 * Copyright 2001-2005 Extended Systems, Inc.  
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

#ifndef __HFI_H_
#define __HFI_H_

#include "hf.h"

/* Link Flags */ 
#define HF_LINK_ACL                  0x01
#define HF_LINK_HANDSFREE            0x08
#define HF_LINK_REMOVE_HF            0x10

/* Internal State Machine Events */ 
#define EVENT_OPEN_IND               0x80
#define EVENT_OPEN                   0x81
#define EVENT_AT_DATA                0x82
#define EVENT_CLOSED                 0x83
#define EVENT_COMMAND_TIMEOUT        0x84
#define EVENT_RF_PACKET_SENT         0x85
#define EVENT_COMMAND_COMPLETE       0x89
#define EVENT_POLL_COMPLETE          0x9A
#define EVENT_RING_TIMEOUT           0x9B

/* Channel Flags */

//wangjianjun
#if  1//HF_VERSION_1_6 == XA_ENABLED

#define HF_CHAN_FLAG_OUTGOING        		0x0001
#define HF_CHAN_FLAG_CODES_NEGTIATION 	0x0002
#define HF_CHAN_FLAG_TEST_INDICATORS 		0x0004
#define HF_CHAN_FLAG_READ_INDICATORS 	0x0008
#define HF_CHAN_FLAG_EVENT_REPORT    		0x0010
#define HF_CHAN_FLAG_TEST_HOLD       		0x0020
#define HF_CHAN_FLAG_NEG_DONE        		0x0040
#define HF_CHAN_FLAG_TX_IN_PROGRESS  		0x0080
#define HF_CHAN_FLAG_COPS_SET        		0x0200
#define HF_CHAN_FLAG_SNIFFING        		0x0400
#define HF_CHAN_FLAG_IGNORE_CLOSE    		0x0800
#define HF_CHAN_FLAG_BT_ACCESSORY            0x1000
#define HF_CHAN_FLAG_LIST_HF_INDICATORS   0x2000
#define HF_CHAN_FLAG_READ_AG_INDICATORS   0x4000
#define HF_CHAN_FLAG_READ_AG_INDICATORS_STATUS 0x8000

#else
#define HF_CHAN_FLAG_OUTGOING        0x0001
#define HF_CHAN_FLAG_TEST_INDICATORS 0x0002
#define HF_CHAN_FLAG_READ_INDICATORS 0x0004
#define HF_CHAN_FLAG_EVENT_REPORT    0x0008
#define HF_CHAN_FLAG_TEST_HOLD       0x0010
#define HF_CHAN_FLAG_NEG_DONE        0x0040
#define HF_CHAN_FLAG_TX_IN_PROGRESS  0x0080
#define HF_CHAN_FLAG_COPS_SET        0x0200
#define HF_CHAN_FLAG_SNIFFING        0x0400
#define HF_CHAN_FLAG_IGNORE_CLOSE    0x0800

#endif

/* SDP Query States */ 
#define HF_SDP_QUERY_FLAG_PROTOCOL   0x01
#define HF_SDP_QUERY_FLAG_PROFILE    0x02
#define HF_SDP_QUERY_FLAG_NETWORK    0x04
#define HF_SDP_QUERY_FLAG_FEATURES   0x08
#define HF_SDP_QUERY_FLAG_HF         0x10

/* The following twelve defines are event identifiers related to 
 * indicator values used with the CIND and CIEV messages.  The first 
 * nine are specific to CIEV messages, and must match the index values 
 * for the HfIndMsg array so that lookups in the indMap can be performed.  
 */ 
#define HF_AT_IND_CALL               0 /* Associated with CIEV messages */ 
#define HF_AT_IND_SIGNAL             1
#define HF_AT_IND_SERVICE            2
#define HF_AT_IND_ROAM               3
#define HF_AT_IND_SMS                4
#define HF_AT_IND_BATTCHG            5
#define HF_AT_IND_SETUP              6 /* Associated with CIEV messages */ 
#define HF_AT_IND_SETUP2             7 /* Associated with CIEV messages */ 
#define HF_AT_IND_CALLHELD           8 /* Associated with CIEV messages */ 

/* The next three defines are used in conjunction with with CIND 
 * messages.  Note that since we only poll with CLCC messages, these 
 * events only occur at initialization time.  Previous versions of the 
 * code polled with CIND messages if CLCC messages were not supported.  
 * However, since many of the AGWs did not consistently report CIEV and 
 * CIND information, it was decided to drop the CIND polling mechanism 
 * and solely rely on what was reported by the CIEV messages.  
 */ 
#define HF_AT_IND_POLL_CALL          100 /* Associated with CIND messages */ 
#define HF_AT_IND_POLL_SETUP         101 /* Associated with CIND messages */ 
#define HF_AT_IND_POLL_CALLHELD      102 /* Associated with CIND messages */ 

/* Polling flags */ 
#define CALLMGR_POLL_LIST_CALLS      0x01 /* This flag is set when issuing a 
                                           * CLCC request and awaiting a 
                                           * reply.  
                                           */ 
#define CALLMGR_POLL_ATTEMPTED       0x02
#define CALLMGR_POLL_RESP_RECVD_1    0x08 /* This flag is set when a CLCC 
                                           * reply for line 1 is received.  
                                           */ 
#define CALLMGR_POLL_RESP_RECVD_2    0x10 /* This flag is set when a CLCC 
                                           * reply for line 2 is received.  
                                           */ 
#define CALLMGR_POLL_STARTED         0x40 /* This flag is set after the 
                                           * initialization sequence has 
                                           * finished and normal periodic 
                                           * polling has commenced.  
                                           */ 
#define CALLMGR_POLL_RELIABLE        0x80

#define AT_TIMEOUT                   0xFFFE

/* Unknown AT data received */ 
#define HF_AT_UNKNOWN                0xFFFF

/*---------------------------------------------------------------------------
 * HfCmdCompType type
 *
 *     This type indicates the condition that caused a command to be 
 *     completed.  
 */
typedef U8 HfCmdCompType;

#define HF_CMD_COMP_OK       0
#define HF_CMD_COMP_ERROR    1
#define HF_CMD_COMP_EXTENDED 2
#define HF_CMD_COMP_TIMEOUT  3

/* End of HfCmdCompType */ 


/* State machine function prototype */ 
void HfSMHandleEvent(HfChannel *chanP, U8 smEvent, AtResults *parmsP);

/* Indicator variables */ 
extern const char *HfIndMsg[];

/* General utility functions */ 
void HfCloseChannel(HfChannel *Chan);
BtStatus HfParseRfcommData(HfChannel       *chan, 
                           RfCallbackParms *parms, 
                           U16             *offset, 
                           AtResults       *atParms);
void HfAppCallback(HfChannel *Chan, 
                   HfEvent    Event, 
                   BtStatus   Status, 
                   void      *data);

#if HF_USE_PHONEBOOK_COMMANDS == XA_ENABLED

void HfPhonebookCallback(HfChannel *Chan, AtResults *Parms);

#endif /* HF_USE_PHONEBOOK_COMMANDS == XA_ENABLED */ 

void HfIndicatorCallback(HfChannel *Chan, AtResults * Parms);
void HfEventCallback(HfChannel *Chan, AtResults *Parms);
HfChannel *HfFindChannel(BtRemoteDevice *remDev);
HfChannel *HfGetClosedChannel(void);

/* SDP utility functions */ 
BtStatus HfRegisterSdpServices(HfChannel *Chan);
BtStatus HfDeregisterSdpServices(void);
BtStatus HfStartServiceQuery(HfChannel *Chan, SdpQueryMode mode);
BtStatus HfVerifySdpQueryRsp(HfChannel *Chan, SdpQueryToken *token);

/* Handsfree functions */ 
void HfRfCallback(RfChannel *Chan, RfCallbackParms *Parms);

/* At send functions */ 
BtStatus AtSendCommand(HfChannel *Chan, AtCommands *AtCommand);
BtStatus HfAtSendRfPacket(HfChannel *Chan, BtPacket *Packet);
void AtParseResultCode(HfChannel *Chan, 
                       U8        *input, 
                       U16        inputLen, 
                       AtResults *results);
BOOL HfStartNextQueuedCmd(HfChannel *hfChanP, BOOL preSend);

#endif /* __HFI_H_ */ 

