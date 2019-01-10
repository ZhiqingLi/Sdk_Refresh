/****************************************************************************
 *
 * File:
 *     $Id: at_dun.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description:  This file contains an Dial Up Networking AT Command 
 *     Processor.  
 *
 * Copyright 2002-2005 Extended Systems, Inc.
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

#ifndef __AT_DUN_H
#define __AT_DUN_H

#include "atconfig.h"

#if AT_DUN == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT Dial Up Networking Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components
 * that support Headset, Hands-free, and DUN profiles.  It also provides
 * the ability to parse and encode Phonebook commands and responses, based
 * on the GSM specifications.  Support for SMS messaging commands will
 * be added in the future.
 *
 * The Dial Up Networking Command Processor module provides the commands 
 * necessary for the Dial-Up Networking Profile.  This API is not complete
 * yet, but will be released at a future date.
 */


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtCommand type
 *
 */
#ifndef AT_COMMAND_DEFINED
#define AT_COMMAND_DEFINED
typedef U16 AtCommand;
#endif 

/** Determines how ITU-T V.24 circuit 109 (or equivalent) relates to the
 *  detection of received line signal from remote end (recommended default 1
 *  i.e. 109 operation relates to detection of received signal).  
 */
#define AT_SIGNAL_DCD           0x0500

/* End of AtCommand */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * 
 *
 */
typedef union _AtDunCmd {
    U8          junk;
} AtDunCmd;

/*---------------------------------------------------------------------------
 * 
 *
 */
typedef union _AtDunResult {
    U8          junk;
} AtDunResult;

#else /* AT_DUN == XA_ENABLED */ 

/* Stub structures to keep #defines out of AtResults/AtCommands structures. */
typedef U8 AtDunCmd;
typedef U8 AtDunResult;

#endif /* AT_DUN == XA_ENABLED */ 

#endif /* __AT_DUN_H */ 

