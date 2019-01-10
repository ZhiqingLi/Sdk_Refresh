/****************************************************************************
 *
 * File:
 *     $Id: avpdus.h 1338 2009-04-14 17:26:26Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1338 $
 *
 * Description: This file contains the header file for code that
 *     allocates AVRCP PDUs.
 *
 * Created:     Feb 9, 2009
 *
 * Copyright 2004-2005 Extended Systems, Inc.
 * Portions copyright BES. All rights
 * reserved. All unpublished rights reserved.
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

#ifndef __AVPDUS_H_
#define __AVPDUS_H_

#include "avrcp.h"

/*---------------------------------------------------------------------------
 * PDU_Free()
 *
 *     Free a PDU
 * 
 * Parameters:
 * 
 *     pdu - a pointer to an allocated PDU.
 */
void PDU_Free(AvrcpAdvancedPdu *pdu);

/*---------------------------------------------------------------------------
 * PDU_AllocCmd()
 *
 *     Allocate a command PDU
 * 
 * Returns:
 * 
 *     A pointer to an allocated PDU.
 */
AvrcpAdvancedPdu * PDU_AllocCmd(void);

/*---------------------------------------------------------------------------
 * PDU_AllocRsp()
 *
 *     Allocate a response PDU
 * 
 * Returns:
 * 
 *     A pointer to an allocated PDU.
 */
AvrcpAdvancedPdu * PDU_AllocRsp(void);

#if AVRCP_VERSION_1_3_ONLY == XA_DISABLED
/*---------------------------------------------------------------------------
 * AppAllocBrwsCmdPdu()
 * 
 *     Allocated a browsing channel command PDU
 * 
 * Returns:
 * 
 *     A pointer to an allocated Browse PDU.
 */
AvrcpAdvancedPdu *PDU_AllocBrwsCmd(AvrcpChannel *chnl);

/*---------------------------------------------------------------------------
 * PDU_AllocBrwsRsp()
 *
 *     Allocate a browsing response PDU
 * 
 * Returns:
 * 
 *     A pointer to an allocated Browse PDU.
 */
AvrcpAdvancedPdu * PDU_AllocBrwsRsp(AvrcpChannel *chnl);
#endif

#endif /* __AVPDUS_H_ */
