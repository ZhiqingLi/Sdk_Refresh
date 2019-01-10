/***************************************************************************
 *
 * File:
 *     $Id: btrom.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains external references to global constant data
 *     for the stack.
 *
 * Created:
 *     March 6, 2000
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

#include "bttypes.h"
#include "sdp.h"

/* CRC table for calculating FCS for RFCOMM frames */
extern const U8 crctable[256];

#define SDP_ZERO_SERV_CLASS_ID_SIZE 5
extern const U8 ZeroServClassIdVal[SDP_ZERO_SERV_CLASS_ID_SIZE];

#define SDP_ZERO_VER_NUMBER_SIZE    8
extern const U8 ZeroVersionNumberList[SDP_ZERO_VER_NUMBER_SIZE];

/* Bluetooth base UUID for SDP */
extern const U8 BT_BASE_UUID[16];

/* Table for new version of CRC-CCITT generator used by L2CAP.*/
extern const U16 L2CrcTab[16];

