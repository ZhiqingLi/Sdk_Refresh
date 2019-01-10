/****************************************************************************
 *
 * File:
 *     $Id: a2dpi.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains internal definitions for the Advanced 
 *     Audio Distribution Profile (A2DP).
 *
 * Created:     June 3, 2004
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

#ifndef __A2DPI_H_
#define __A2DPI_H_

#include "sys/a2alloc.h"

/* SDP Query Flags */
#define A2DP_SDP_QUERY_FLAG_SERVICE   0x01
#define A2DP_SDP_QUERY_FLAG_PROTOCOL  0x02
#define A2DP_SDP_QUERY_FLAG_PROFILE   0x03
#define A2DP_SDP_QUERY_FLAG_FEATURES  0x08

/* Supported AVDTP command types */
#define A2DP_AVDTP_CMD_DISCOVER            0x01
#define A2DP_AVDTP_CMD_GET_CAPABILITY      0x02
                                      
/* Stream Flags */
#define A2DP_STRM_FLAG_DISCOVERY    0x02
#define A2DP_STRM_FLAG_CLOSE_INT    0x04
#define A2DP_STRM_FLAG_QUERY_CODEC  0x08
#define A2DP_STRM_FLAG_QUERY_CP     0x10
#define A2DP_STRM_FLAG_GET_CAP      0x20
#define A2DP_STRM_FLAG_IDLE         0x40

/* Function Prototypes */
BtStatus A2dpRegisterSdpService(A2dpEndpointType streamType);
BtStatus A2dpDeregisterSdpService(A2dpEndpointType streamType);
BtStatus A2dpStartServiceQuery(AvDevice *Device, A2dpEndpointType streamType, 
                               SdpQueryMode mode);
void A2dpAvdevCallback(AvDevice *Device, const AvdevCallbackParms *Parms);

void A2dpAvdtpCallback(AvdtpChannel *Chnl, const AvdtpCallbackParms *Parms);
void A2dpFreeStreamInfo(A2dpStream *Stream);
void A2dpStreamClose(A2dpStream *Stream, const AvdtpCallbackParms *Parms);
void A2dpClose(AvDevice *Device, const AvdtpCallbackParms *Parms);
BtStatus A2dpStreamSendSbcPacket(A2dpStream *Stream, A2dpSbcPacket *Packet);


#endif /* __A2DPI_H_ */

