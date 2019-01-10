/***************************************************************************
 *
 * File:
 *     $Id: mexp.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains functions exported to the Event Manager by the
 *     Management Entity.
 *
 * Created:
 *     July 20, 1999
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

#ifndef __MEXP_H
#define __MEXP_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus ME_Init(void)
 *
 * Descriptions:  Initialize the Management Entity. 
 *
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - ME initialized successfully.
 *
 *                BT_STATUS_FAILED - ME failed to initialize.
 */
BOOL ME_Init(void);

/*---------------------------------------------------------------------------
 * Prototype:     void ME_Deinit(void)
 *---------------------------------------------------------------------------
 *
 * Description:   Deinitialize the Management Entity. 
 *
 * Parameters:    void
 *
 * Returns:       void
 */
void ME_Deinit(void);

#endif

