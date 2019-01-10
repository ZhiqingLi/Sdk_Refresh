/****************************************************************************
 *
 * File:
 *     $Id: at_sms.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains definitions for a Short Message Service 
 *     (SMS) AT Command Processor.  
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

#ifndef __AT_SMS_H
#define __AT_SMS_H

#include "atconfig.h"

#if AT_SMS == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT SMS Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components 
 * that support Headset, Hands-free, and DUN profiles.  It also provides 
 * the ability to parse and encode Phonebook commands and responses, based 
 * on the GSM specifications.  Support for SMS messaging commands will 
 * be added in the future.  
 *
 * The SMS Command Processor module provides the commands necessary 
 * for handilng SMS messages on the Audio Gateway.  This module is currently 
 * not complete, but will be added in a future release.  
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

/** This command selects the type messaging service.  It returns the types 
 *  of messages supported by the ME for mobile terminated messages, mobile 
 *  originated messages, and broadcast type messages.  
 */
#define AT_SELECT_SMS_SERVICE       0x0400

/** This command selects memory storages to be used for reading, writing, 
 *  etc.  The read command returns information about each memory storage.  
 */
#define AT_PREFERRED_SMS_STORAGE    0x0401

/** */ 
#define AT_SMS_MESSAGE_FORMAT       0x0402

/** */ 
#define AT_SMS_SERVICE_CENTER       0x0403

/** */ 
#define AT_SET_TEXT_MODE_PARMS      0x0404

/** */ 
#define AT_SET_CBM_TYPES            0x0405

/** */ 
#define AT_NEW_MESSAGE_INDICATION   0x0406

/** */ 
#define AT_SMS_DELIVER_INDICATION   0x0407

/** */ 
#define AT_CELL_BROADCAST_MSG_ARRIVE 0x0408

/** */ 
#define AT_SMS_STATUS_REPORT        0x0409

/** */ 
#define AT_LIST_MESSAGES            0x040A

/** */ 
#define AT_READ_MESSAGE             0x040B

/** */ 
#define AT_SEND_MESSAGE             0x040C

/** */ 
#define AT_SEND_STORED_MESSAGE      0x040D

/** */ 
#define AT_STORE_MESSAGE            0x040E

/** */ 
#define AT_DELETE_MESSAGE           0x040F

/* End of AtCommand */ 

/*--------------------------------------------------------------------------
 * AtError type
 *
 *     Error types used with AT_SMS_ERROR result.
 */
typedef U8 AtError;

#define ATCMS_ME_FAILURE                    300
#define ATCMS_SMS_SERVICE_OF_ME_RESERVED    301
#define ATCMS_OPERATION_NOT_ALLOWED         302
#define ATCMS_OPERATION_NOT_SUPPORTED       303
#define ATCMS_INVALID_PDU_MODE_PARAMETER    304
#define ATCMS_INVALID_TEXT_MODE_PARAMETER   305
#define ATCMS_SIM_NOT_INSERTED              310
#define ATCMS_SIM_PIN_REQUIRED              311
#define ATCMS_PH_SIM_PIN_REQUIRED           312
#define ATCMS_SIM_FAILURE                   313
#define ATCMS_SIM_BUSY                      314
#define ATCMS_SIM_WRONG                     315
#define ATCMS_SIM_PUK_REQUIRED              316
#define ATCMS_SIM_PIN2_REQUIRED             317
#define ATCMS_SIM_PUK2_REQUIRED             318
#define ATCMS_MEMORY_FAILURE                320
#define ATCMS_INVALID_MEMORY_INDEX          321
#define ATCMS_MEMORY_FULL                   322
#define ATCMS_SMSC_ADDRESS_UNKNOWN          330
#define ATCMS_NO_NETWORK_SERVICE            331
#define ATCMS_NETWORK_TIMEOUT               332
#define ATCMS_NO_CNMA_ACK_EXPECTED          340
#define ATCMS_ERROR                         500

/* End of AtError */

/*---------------------------------------------------------------------------
 * AtSmsStorage
 *
 *     Memory storage location definitions.  
 */
typedef U8 AtSmsStorage;

/** Broadcast Message storage. */ 
#define AT_SMS_STORE_CBM            0x01

/** ME message storage. */ 
#define AT_SMS_STORE_ME             0x02

/** Any of the storages associated with ME. */ 
#define AT_SMS_STORE_ANY            0x04

/** SIM message storage. */ 
#define AT_SMS_STORE_SIM            0x08

/** Status report storage. */ 
#define AT_SMS_STORE_STATUS         0x10

/* End of AtSmsStorage */ 

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * 
 *
 */
typedef union _AtSmsCmd {
    struct {

        /* Service type: 0=GSM 7.05 Phase 2, 1=GSM 7.05 Phase 2+ */ 
        U8          type;

    } service;

    struct {

        /* Memory from which messages are read and deleted. */ 
        AtSmsStorage    read;

        /* Memory to which writing and sending operations are made. */ 
        AtSmsStorage    write;
        
        /* Memory to which received SMs are preferred to be stored. */ 
        AtSmsStorage    recv;

    } preferred;

    struct {

        /* Message format 0=PDU mode, 1=text mode. */ 
        U8              mode;

    } format;

} AtSmsCmd;

/*---------------------------------------------------------------------------
 * AtSmsResult
 *
 */
typedef union _AtSmsResult {
    struct {
        U8              type;

        /* Support for receiving (mobile terminated) messages. */ 
        BOOL            recv;

        /* Support for sending (mobile originated) messages. */ 
        BOOL            send;
        
        /* Support for cell broadcast messages.*/ 
        BOOL            bcast;

    } service;

    struct {

        /* Memory from which messages are read and deleted. */ 
        U16             readUsed;
        U16             readTotal;

        /* Memory to which writing and sending operations are made. */ 
        U16             writeUsed;
        U16             writeTotal;
        
        /* Memory to which received SMs are preferred to be stored. */ 
        U16             recvUsed;
        U16             recvTotal;

    } preferred;

    struct {

        /* Memory from which messages are read and deleted. */ 
        AtStorageType   read;
        U16             readUsed;
        U16             readTotal;

        /* Memory to which writing and sending operations are made. */ 
        AtStorageType   write;
        U16             writeUsed;
        U16             writeTotal;

        /* Memory to which received SMs are preferred to be stored. */ 
        AtStorageType   recv;
        U16             recvUsed;
        U16             recvTotal;

    } preferred_read;

} AtSmsResult;

#else /* AT_SMS != XA_ENABLED */ 

/* Stub structures to keep #defines out 
 * of AtResults/AtCommands structures.  
 */ 
typedef U8 AtSmsCmd;
typedef U8 AtSmsResult;

#endif /* else AT_SMS != XA_ENABLED */ 

#endif /* __AT_SMS_H */ 

