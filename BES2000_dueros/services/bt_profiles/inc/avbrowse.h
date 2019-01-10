/****************************************************************************
 *
 * File:
 *     $Id: avbrowse.h 1329 2009-04-14 15:12:06Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 1329 $
 *
 * Description: This file contains the header file for code that simulates
 *     browsing responses for AVRCP 1.4.
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
 * Use of this work is governed by a license granted by BES 
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __AVBROWSE_H_
#define __AVBROWSE_H_

#include "avrcp.h"

#if AVRCP_BROWSING_TARGET == XA_ENABLED

/*---------------------------------------------------------------------------
 * BRWS_HandleBrowseCommand()
 * 
 *     Handle a browse command.
 * 
 * Parameters:
 *
 *      chnl - A connected AVRCP channel.
 *
 *      Parms - AVRCP callback parameters received in a AVRCP
 *          event.
 */
void BRWS_HandleBrowseCommand(AvrcpChannel             *chnl, 
                              const AvrcpCallbackParms *Parms);

#endif /* AVRCP_BROWSING_TARGET == XA_ENABLED */

#endif /* __AVBROWSE_H_ */

