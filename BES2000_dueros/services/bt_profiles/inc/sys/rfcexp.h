/***************************************************************************
 *
 * File:
 *     $Id: rfcexp.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Function exports to lower layer (ME).
 *
 * Created:
 *     September 13, 1999
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

#ifndef __RFCEXP_H
#define __RFCEXP_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus RF_Init(void)
 *
 * Description:   Initialize RFCOMM.  Register with L2CAP, initialize state and
 *                other variables.
 *            
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - initialization successful.
 *
 *                BT_STATUS_FAILED - initialization failed.   
 */
BtStatus RF_Init(void);

/*---------------------------------------------------------------------------
 * Prototype:     RF_Deinit()
 *
 * Description:   Deinitialize RFCOMM.  Deregister with L2CAP.
 *            
 * Parameters:    void
 *
 * Returns:       void   
 */
void RF_Deinit(void);

#endif /* __RFCEXP_H */

