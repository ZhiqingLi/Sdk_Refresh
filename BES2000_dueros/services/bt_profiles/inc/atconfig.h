/****************************************************************************
 *
 * File:
 *     $Id: atconfig.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains an AT Command Processor configuration 
 *     constants.  
 *
 * Copyright 2002-2005 Extended Systems, Inc.  
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

#ifndef __ATCONFIG_H
#define __ATCONFIG_H

#include "config.h"

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_HEADSET  
#define AT_HEADSET          XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_HANDSFREE
#define AT_HANDSFREE        XA_DISABLED
#endif

#if AT_HANDSFREE == XA_ENABLED

/* Headset commands must be included when using handsfree. */ 

#undef AT_HEADSET
#define AT_HEADSET          XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_PHONEBOOK
#define AT_PHONEBOOK        XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_SMS
#define AT_SMS              XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_DUN
#define AT_DUN              XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_ROLE_TERMINAL
#define AT_ROLE_TERMINAL    XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_ROLE_MOBILE
#define AT_ROLE_MOBILE      XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_MAX_PARMS
#define AT_MAX_PARMS        45
#endif

#if (AT_MAX_PARMS < 16) || (AT_MAX_PARMS > 255)
#error "AT_MAX_PARMS must be between 16 and 255."
#endif

/*----------------------------------------------------------------------------
 * This is maximum the number of indicators that are supported by the Hands 
 * Free code.  This value defaults to 20, but it can be changed to something 
 * less to save a few bytes by defining it in the overide.h file instead.  
 * This value must never be greater thatn 20, as indicated by the Hands Free 
 * specification Errata 2742.  Note that 20 is NOT the maximum number of 
 * indicators that can be reported by a cell network to an audio gateway.  
 * However, it is the maximum number that can be reported over a Bluetooth 
 * Handsfree connection.  If the cellular network has more than 20 
 * indicators, then the audio gateway will have to selectively limit what it 
 * reports to the Hands Free Unit.  
 */
#ifndef AT_MAX_INDICATORS
#define AT_MAX_INDICATORS   20
#endif

#if (20 < AT_MAX_INDICATORS)
#error "HF Spec Errata 2742 violation:  AT_MAX_INDICATORS must be less than or equal to 20."
#endif

#if (1 > AT_MAX_INDICATORS)
#error "AT_MAX_INDICATORS must be greater than 0."
#endif

#endif /* __ATCONFIG_H */ 

