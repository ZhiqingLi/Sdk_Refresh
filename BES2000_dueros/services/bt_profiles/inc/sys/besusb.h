/***************************************************************************
 *
 * File:
 *     $Id: besusb.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains definitions and structures specific 
 *     to the IA HCI USB hardware driver.  
 *
 * Copyright BES.
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

#ifndef __BES_USB_H
#define __BES_USB_H

#include "hcitrans.h"

/* Function prototypes */
BtStatus BESUSB_Init(TranCallback cb);
BtStatus BESUSB_Shutdown(void);

#endif /* __BES_USB_H */
