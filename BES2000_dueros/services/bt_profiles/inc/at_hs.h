/****************************************************************************
 *
 * File:
 *     $Id: at_hs.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains definitions for an AT Headset Command 
 *     Processor.  
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

#ifndef __AT_HS_H
#define __AT_HS_H

#if AT_HEADSET == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT Headset Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components 
 * that support Headset, Hands-free, and DUN profiles.  It also provides 
 * the ability to parse and encode Phonebook commands and responses, based 
 * on the GSM specifications.  Support for SMS messaging commands will 
 * be added in the future.  
 *
 * The Headset Command Processor module provides the commands necessary 
 * for the Headset Profile.  It is necessary to include this module 
 * when using the Hands-free Profile, because they share some AT commands.  
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtCommand type
 *
 *     Headset Command and Result types.
 */
#ifndef AT_COMMAND_DEFINED
#define AT_COMMAND_DEFINED
typedef U16 AtCommand;
#endif

/**  */ 
#define AT_MICROPHONE_GAIN      0x0100

/**  */ 
#define AT_SPEAKER_GAIN         0x0101

/**  */ 
#define AT_KEYPAD_CONTROL       0x0102

/* End of AtCommand */ 


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtHeadsetCmd
 * 
 *     This structure is used to specify the parameters associated with 
 *     Headset commands. Headset commands are sent from the Headset device 
 *     to the Audio Gateway.  As such, this structure is used by the Headset 
 *     device encoder, and the Audio Gateway decoder functions.  
 */
typedef union _AtHeadsetCmd {

    /* AT_MICROPHONE_GAIN */ 
    struct {
        U8          gain;
    } mic;

    /* AT_SPEAKER_GAIN */ 
    struct {
        U8          gain;
    } speaker;

    /* AT_KEYPAD_CONTROL */ 
    struct {
        U8          button;
    } keypad;

} AtHeadsetCmd;


/*---------------------------------------------------------------------------
 * AtHeadsetResult
 * 
 *     This structure is used to specify the parameters associated with 
 *     Headset results.  Headset results are sent from the Audio Gateway 
 *     to the Headset device.  As such, this structure is used by the Audio 
 *     Gateway encoder, and the Headset device decoder functions.  
 */
typedef union _AtHeadsetResult {

    /* AT_MICROPHONE_GAIN */ 
    struct {
        U8          gain;
    } mic;

    /* AT_SPEAKER_GAIN */ 
    struct {
        U8          gain;
    } speaker;

} AtHeadsetResult;

#else /* AT_HEADSET == XA_ENABLED */ 

/* Stub structures to keep #defines out 
 * of AtResults/AtCommands structures. 
 */ 
typedef U8 AtHeadsetCmd;
typedef U8 AtHeadsetResult;

#endif /* AT_HEADSET == XA_ENABLED */ 

#endif /* __AT_HS_H */ 

