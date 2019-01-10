/****************************************************************************
 *
 * File:
 *     $Id: atp.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains definitions for an AT Command Processor.  
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
 * Use of this work is governed by a license granted by BES 
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.  
 *
 ****************************************************************************/

#ifndef __ATP_H
#define __ATP_H

#include "atconfig.h"
#include "xatypes.h"
#include "xastatus.h"
#include "utils.h"

/*---------------------------------------------------------------------------
 * AT Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components 
 * that support Headset, Hands-free, and DUN profiles.  It also provides 
 * the ability to parse and encode Phonebook commands and responses, based 
 * on the GSM specifications.  Support for SMS messaging commands will 
 * be added in the future.  
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
/* Or'd with AT_??? command to create "Read" form of command/result ("?").  
 */
#define AT_READ             0x1000

/* Or'd with AT_??? command to create "Test" form of command/result ("=?").  
 */
#define AT_TEST             0x2000

/* Number of AT command groups supported by the processor. */ 
#define AT_NUM_GROUPS       6


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

/** This result acknowledges successful acceptance of a command.  There are 
 *  no parameters with this command.  
 */
#define AT_OK                   0x0000

/** This result indicates that the received command was not accepted.  There 
 *  are no parameters with this command.  
 */
#define AT_ERROR                0x0001

/** This unsolicited result indicates an incoming call signal from network.  
 *  There are no parameters with this command.  
 */
#define AT_RING                 0x0002

/** This result indicates that the received command was not accepted.  The 
 *  "error" parameter is used with this command.  
 */
#define AT_EXTENDED_ERROR       0x0003

/** This command is used to enable or disable extended error reporting.  The 
 * "error" parameter is used with this command.  
 */
#define AT_SET_ERROR_MODE       0x0004

#if (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) || (AT_SMS == XA_ENABLED)

/** This command informs the mobile equipment of the character set used by 
 *  the terminal equipment.  The "charSet" parameter is used with this 
 *  command.  
 */
#define AT_SELECT_CHARACTER_SET 0x0005

#endif /* (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) || (AT_SMS == XA_ENABLED) */ 

#if (AT_HANDSFREE == XA_ENABLED) || (AT_DUN == XA_ENABLED)

/** This result indicates that the call could not be established or the 
 *  connection was terminated.  There are no parameters with this command.  
 */
#define AT_NO_CARRIER           0x0006

/** This result indicates that the called party is busy.  There are no 
 *  parameters with this command.  
 */
#define AT_BUSY                 0x0007

/** This result indicates connection completion on timeout.  There are no 
 *  parameters with this command.  
 */
#define AT_NO_ANSWER            0x0008

/** This result indicates that calls to the number are temporarily 
 *  forbidden.  There are no parameters with this command.  
 */
#define AT_DELAYED              0x0009

/** This result indicates that calls to the number are forbidden until a 
 *  manual reset.  There are no parameters with this command.  
 */
#define AT_BLACKLISTED          0x000A

/** This indicates that the data will be sent as 'raw' AT data.  The 
 *  "data" parameter is used with this response.  
 */
#define AT_RAW                  0x000B

#endif /* (AT_HANDSFREE == XA_ENABLED) */ 

/* End of AtCommand */ 

/*---------------------------------------------------------------------------
 * AtStatus type
 *
 */
typedef U8 AtStatus;

/** The function completed successfully. */ 
#define AT_STATUS_OK                    XA_STATUS_SUCCESS

/** A function parameter was invalid. */ 
#define AT_STATUS_INVALID_PARM          XA_STATUS_INVALID_PARM

/** The buffer is insufficient to encode the command. */ 
#define AT_STATUS_NO_RESOURCES          XA_STATUS_NO_RESOURCES

/** The command is recognized but the format is not supported.  For example 
 *  decoding the test result for a command that we only support reading 
 *  would return this error code.  
 */
#define AT_STATUS_NOT_SUPPORTED         XA_STATUS_NOT_SUPPORTED

/** The command is not recognized. */ 
#define AT_STATUS_NOT_FOUND             XA_STATUS_NOT_FOUND

/* End of AtStatus */ 

/*--------------------------------------------------------------------------
 * AtError type
 *
 *     Error types used with AT_EXTENEDED_ERROR result.
 */
typedef U8 AtError;

#define ATCME_NO_CONNECTION                 1
#define ATCME_OP_NOT_ALLOWED                3
#define ATCME_OP_NOT_SUPPORTED              4
#define ATCME_PH_SIM_PIN_REQUIRED           5
#define ATCME_SIM_NOT_INSERTED             10
#define ATCME_SIM_PIN_REQUIRED             11
#define ATCME_SIM_PUK_REQUIRED             12
#define ATCME_SIM_FAILURE                  13
#define ATCME_SIM_BUSY                     14
#define ATCME_INCORRECT_PASSWORD           16
#define ATCME_SIM_PIN2_REQUIRED            17
#define ATCME_SIM_PUK2_REQUIRED            18
#define ATCME_MEMORY_FULL                  20
#define ATCME_INVALID_INDEX                21
#define ATCME_MEMORY_FAILURE               23
#define ATCME_TEXT_STRING_TOO_LONG         24
#define ATCME_INVALID_CHARS_IN_TEXT_STRING 25
#define ATCME_DIAL_STRING_TOO_LONG         26
#define ATCME_INVALID_CHARS_IN_DIAL_STRING 27
#define ATCME_NO_NETWORK_SERVICE           30
#define ATCME_NETWORK_NOT_ALLOWED          32
#define ATCME_UNKNOWN                     100

/* End of AtError */ 


/*---------------------------------------------------------------------------
 * AtNumberFormat type
 *
 *     This type is used to convey the format of a phone number.  The format 
 *     is created by or'ing together an ATNUM_TYPE value with an 
 *     ATNUM_PLAN.  For more information refer to GSM 04.08 section 
 *     10.5.4.7.  
 */
typedef U8 AtNumberFormat;

#define ATNUM_TYPE_UNKNOWN          0x80
#define ATNUM_TYPE_INTERNATIONAL    0x90
#define ATNUM_TYPE_NATIONAL         0xA0
#define ATNUM_TYPE_NETWORK_SPEC     0xB0
#define ATNUM_TYPE_DEDICATED_ACC    0xC0

#define ATNUM_PLAN_UNKNOWN          0x00
#define ATNUM_PLAN_ISDN_TELEPHONY   0x01
#define ATNUM_PLAN_DATA_NUMBERING   0x03
#define ATNUM_PLAN_TELEX_NUMBERING  0x04
#define ATNUM_PLAN_NATIONAL         0x08
#define ATNUM_PLAN_PRIVATE          0x09

/* Commonly used formats */ 
#define ATNUM_INTERNATIONAL_ISDN    (ATNUM_TYPE_INTERNATIONAL|ATNUM_PLAN_ISDN_TELEPHONY)
#define ATNUM_ISDN_TELEPHONY        (ATNUM_TYPE_UNKNOWN|ATNUM_PLAN_ISDN_TELEPHONY)
#define ATNUM_NATIONAL_ISDN         (ATNUM_TYPE_NATIONAL|ATNUM_PLAN_ISDN_TELEPHONY)

/* End of AtNumberFormat */ 


/* Pull in definitions for command group specific structures. */ 
#include "at_hs.h"
#include "at_hf.h"
#include "at_pb.h"
#include "at_sms.h"
#include "at_dun.h"

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtRawContext 
 *
 *     Private information necessary for the encoding and decoding of raw AT 
 *     commands and results.  
 */
typedef struct _AtRawContext {
    BOOL  match;
    char  buff[0x10];
    char *str;
    I8    commandNdx;
    I8    tableNdx;
} AtRawContext;


/*---------------------------------------------------------------------------
 * AtContext 
 *
 *     Private information necessary for the encoding and decoding of AT 
 *     commands and results.  
 */
typedef struct _AtContext {
    U8           lastCmd[AT_NUM_GROUPS];
    AtRawContext raw;
} AtContext;


/*---------------------------------------------------------------------------
 * AtRaw structure
 *
 *     Defines the structure containing raw AT data.  
 */
typedef struct _AtRaw {
    U8 *str;
    U8  len;
} AtRaw;

/*---------------------------------------------------------------------------
 * AtCommands structure
 *
 *     This structure is used to specify the parameters associated with 
 *     all supported AT commands.  Commands are sent by the Terminal Entity 
 *     (e.g. Hands-Free unit) and received on the Mobile Entity (e.g. 
 *     Gateway).  
 */
typedef struct _AtCommands {

    /* This field is intended for use by the structure owner. */ 
    ListEntry           node;

    /* AT_??? type from any specified command group. The type may include
     * the AT_READ or AT_TEST flags. No parameters are specified when
     * reading or testing a command.
     */ 
    AtCommand           type;

    union {

        /* Headset AT command group parameters. */ 
        AtHeadsetCmd        hs;

        /* Hands-Free AT command group parameters. */ 
        AtHandsfreeCmd      hf;

        /* Phonebook AT command group parameters. */ 
        AtPhonebookCmd      pb;

        /* Short Message Service AT command group parameters. */ 
        AtSmsCmd            sms;

        /* Dial Up Networking AT command group parameters. */ 
        AtDunCmd            dun;

        /* AT_SET_ERROR_MODE */ 
        struct {

            /* Error mode: 0=disable, 1=enable, 2=verbose */ 
            U8              mode;

        } error;

#if (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED)

        /* AT_SELECT_CHARACTER_SET */ 
        struct {

            /* Character set type (e.g. "GSM", "HEX", "UCS2"). */ 
            const char     *type;

        } charSet;

#endif /* (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) */ 

        AtRaw               raw;
    } p;

} AtCommands;


/*---------------------------------------------------------------------------
 * AtResults structure
 *
 *     This structure is used to specify the parameters associated with 
 *     all supported AT results.  Results, solicited or unsolicited are sent 
 *     by the Mobile Entity (e.g. Gateway) and received on the Terminal 
 *     Entity (e.g. Hands-Free unit).  
 */
typedef struct _AtResults {

    /* This field is intended for use by the structure owner. */ 
    ListEntry           node;

    /* AT_??? type from any specified command group.  The type may include 
     * the AT_READ or AT_TEST flags.  
     */
    AtCommand           type;

    union {
        /* Headset AT result group parameters. */ 
        AtHeadsetResult     hs;

        /* Hands-Free AT result group parameters. */ 
        AtHandsfreeResult   hf;

        /* Phonebook AT command group parameters. */ 
        AtPhonebookResult   pb;

        /* Short Message Service AT command group parameters. */ 
        AtSmsResult         sms;

        /* Dial Up Networking AT command group parameters. */ 
        AtDunResult         dun;

        /* Raw data response */ 
        U8                 *data;

        /* 1 byte generic parameter. */ 
        U8                  parm8;

        /* 2 byte generic parameter. */ 
        U16                 parm16;

        /* 4 byte generic parameter. */ 
        U32                 parm32;

        /* BOOL generic parameter. */ 
        BOOL                parmB;

        /* AT_EXTENDED_ERROR */ 
        struct {
            AtError         type;
        } error;

    } p;

} AtResults;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AT_Init()
 *
 *     This function initializes the AT processor.  It must be called before 
 *     any other AT command is used.  
 *
 * Parameters:
 *     Atc - Uninitialized AtContext structure pointer.  
 *
 * Returns:
 *     AT_STATUS_OK - The command was encoded successfully.  
 *
 *     AT_STATUS_INVALID_PARM - A function parameter is invalid.  
 */
AtStatus AT_Init(AtContext *Atc);

#if AT_ROLE_TERMINAL == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT_TEncode()
 *
 *     This function encodes an AT command for transmission to the gateway 
 *     (ME).  
 *
 * Requires:
 *     AT_ROLE_TERMINAL be enabled.  
 *
 * Parameters:
 *     Atc - AtContext structure pointer.  
 *
 *     Command - The command to encode with its parameters.  
 *
 *     Output - Data buffer that receives the encoded command.  
 *
 * Returns:
 *     AT_STATUS_OK - The command was encoded successfully.  
 *
 *     AT_STATUS_NOT_FOUND - The command was not recognized.  
 *
 *     AT_STATUS_NO_RESOURCES - The "Output" buffer was too small to hold 
 *         the encoded command.
 */
AtStatus AT_TEncode(AtContext        *Atc, 
                    const AtCommands *Command, 
                    XaBufferDesc     *Output);

/*---------------------------------------------------------------------------
 * AT_TDecode()
 *
 *     This function decodes an AT result received by the device (TE).  
 *
 * Requires:
 *     AT_ROLE_TERMINAL be enabled.  
 *
 * Parameters:
 *     Atc - AtContext structure pointer.  
 *
 *     Result - A pointer to a result structure that receives the result 
 *         type and parameters.  
 *
 *     Input - The data buffer that provides the result to decode.  
 *
 * Returns:
 *     AT_STATUS_OK - The result was decoded successfully.  
 *
 *     AT_STATUS_NOT_FOUND - The result type was not recognized.  
 */
AtStatus AT_TDecode(AtContext *Atc, AtResults *Result, XaBufferDesc *Input);

#endif /* AT_ROLE_TERMINAL == XA_ENABLED */ 

#if AT_ROLE_MOBILE == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT_MEncode()
 *
 *     This function encodes an AT result for transmission to the device 
 *     (TE).  
 *
 * Requires:
 *     AT_ROLE_MOBILE be enabled.  
 *
 * Parameters:
 *     Atc - AtContext structure pointer.  
 *
 *     Result - The result to encode with its parameters.  
 *
 *     Output - Data buffer that receives the encoded result.  
 *
 * Returns:
 *     AT_STATUS_OK - The result was encoded successfully.  
 *
 *     AT_STATUS_NOT_FOUND - The result was not recognized.  
 *
 *     AT_STATUS_NO_RESOURCES - The "Output" buffer was too small to hold the 
 *         encoded result.  
 */
AtStatus AT_MEncode(AtContext       *Atc, 
                    const AtResults *Result, 
                    XaBufferDesc    *Output);

/*---------------------------------------------------------------------------
 * AT_MDecode()
 *
 *     This function decodes AT commands received by a gateway (ME).  
 *
 * Requires:
 *     AT_ROLE_MOBILE be enabled.  
 *
 * Parameters:
 *     Atc - AtContext structure pointer.  
 *
 *     Result - A pointer to a command structure that receives the command 
 *         type and parameters.  
 *
 *     Input - The data buffer that provides the command to decode.  
 *
 * Returns:
 *     AT_STATUS_OK - The command was decoded successfully.  
 *
 *     AT_STATUS_NOT_FOUND - The command type was not recognized.  
 */
AtStatus AT_MDecode(AtContext    *Atc, 
                    AtCommands   *Command, 
                    XaBufferDesc *Input);

#endif /* AT_ROLE_MOBILE == XA_ENABLED */ 


/*---------------------------------------------------------------------------
 * AT_Range2MinMax()
 *
 *     Returns the first and last values found in range strings that contain 
 *     a list of integers, such as "(0,1)", "(0-5)" or "(0,1,2)".  
 *
 * Parameters:
 *     Range - A null-terminated string, devoid of whitespace.  
 *     
 *     Min - A pointer to the variable which receives the first value in 
 *         the range. On error *Min and *Max == 0xff.  
 *     
 *     Max - A pointer to the variable which receives the last value in 
 *         the range. If only one value is present Min == Max.  
 */
void AT_Range2MinMax(const char *Range, U16 *Min, U16 *Max);

#if (AT_ROLE_TERMINAL == XA_ENABLED) && (AT_ROLE_MOBILE != XA_ENABLED)

#define AT_Encode(a, b, c)     AT_TEncode(a, b, c)
#define AT_Decode(a, b, c)     AT_TDecode(a, b, c)

#endif /* (AT_ROLE_TERMINAL == XA_ENABLED) && (AT_ROLE_MOBILE != XA_ENABLED) */ 

#if (AT_ROLE_TERMINAL != XA_ENABLED) && (AT_ROLE_MOBILE == XA_ENABLED)

#define AT_Encode(a, b, c)     AT_MEncode(a, b, c)
#define AT_Decode(a, b, c)     AT_MDecode(a, b, c)

#endif /* (AT_ROLE_TERMINAL != XA_ENABLED) && (AT_ROLE_MOBILE == XA_ENABLED) */ 

#endif /* __ATP_H */ 

