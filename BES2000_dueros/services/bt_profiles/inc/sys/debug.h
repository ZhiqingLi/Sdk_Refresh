/***************************************************************************
 *
 * File:
 *     $Id: debug.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains prototypes for debug message functions.
 *
 * Created:
 *     August 13, 1999
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

#ifndef __DEBUG_H
#define __DEBUG_H

/*****************************************************************************
 *
 * Debug functions for converting protocol and status types to strings.
 *
 * NOTE: Do not include this file in a header file. It should only be included
 *       in source code files and should be the last file in the include list.
 *
 *****************************************************************************/

/*---------------------------------------------------------------------------
 * SniffProtocolId type
 *
 *     This type is used in ChannelInfo to identify the valid protocol id's.
 */
typedef U16 SniffProtocolId;

#define SPI_LM          0x0001  /* LMP Connection Handle */
#define SPI_L2CAP       0x0002  /* L2CAP Protocol */
#define SPI_SDP         0x0004  /* SDP Protocol */
#define SPI_RFCOMM      0x0008  /* RFCOMM Channel protocol */
#define SPI_OBEX        0x0010  /* OBEX Protocol */
#define SPI_TCS         0x0020  /* TCS Binary protocol */
#define SPI_DATA        0x0040  /* Application data */
#define SPI_L2CAP_E     0x0080  /* L2CAP Protocol in Enhanced mode */
#define SPI_SIM         0x0100  /* SIM Access protocol */
#define SPI_AV          0x0200  /* AV protocols */
#define SPI_BNEP        0x0400  /* BNEP protocol */
/* End of SniffProtocolId */

#if (XA_SNIFFER == XA_ENABLED) && (HCI_ALLOW_PRESCAN == XA_ENABLED)

/*---------------------------------------------------------------------------
 * SnifferRegisterEndpoint()
 *
 *     This function is called by protocol implementations to register a
 *     new connection in the sniffer connection table. The registering
 *     protocol provides information about the local endpoint, remote
 *     device and the protocol that is running over that endpoint.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     EndpointType - Protocol type of 'Endpoint'. Must be one of SPI_L2CAP
 *         or SPI_RFCOMM.
 *
 *     Endpoint - Identifier for the endpoint. Must be a pointer to an
 *         L2CAP L2capChannelId, or RFCOMM RfChannel->dlci.
 *
 *     RemDev - BtRemoteDevice pointer.
 *
 *     Protocol - Protocol layered above 'Endpoint'.
 */
void SnifferRegisterEndpoint(SniffProtocolId EndpointType, void *Endpoint, 
                             BtRemoteDevice *RemDev, SniffProtocolId Protocol);
#else
#define SnifferRegisterEndpoint(A,B,C,D)   (void)0
#endif


#if XA_DEBUG_PRINT == XA_ENABLED

/*
 *---------------------------------------------------------------------------
 *            TCS Debug functions
 *---------------------------------------------------------------------------
 */
#if defined(__TCS_H)
const char *pTCS_Event(TcsEvent tcs_event);
const char *pTCS_CallState(TcsCallState State);
const char *pTCS_Message(TcsMsgType msg);
#endif

#endif /* XA_DEBUG_PRINT */

#endif /* __DEBUG_H */
