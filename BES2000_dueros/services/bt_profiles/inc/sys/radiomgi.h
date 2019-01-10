/***************************************************************************
 *
 * File:
 *     $Id: radiomgi.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains internal definitions for the Radio Manager.
 *
 * Created:
 *     April 11, 2003
 *
 * Copyright 2003-2005 Extended Systems, Inc.
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

#ifndef __RADIOMGI_H
#define __RADIOMGI_H

#include "bttypes.h"

/*--------------------------------------------------------------------------*
 * Typedefs and Data structures used internally by Radio Manager            *
 *--------------------------------------------------------------------------*/
 
/* Radio Manager States */
#define RMGR_STATE_SHUTDOWN             0
#define RMGR_STATE_SHUTTING_DOWN        1
#define RMGR_STATE_RESETTING            2
#define RMGR_STATE_INITIALIZING         3
#define RMGR_STATE_READY                4

/* Radio Manager context */
typedef struct _BtRmgContext {
    U8              retries;          /* Number of retries during init */
    U8              rmState;          /* Radio manager state           */
} BtRmgContext;

#endif /* __RADIOMGI_H */

