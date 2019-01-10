/****************************************************************************
 *
 * File:
 *     $Id: atpi.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains internal definitions used by the AT 
 *     Command Processor.  
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

#ifndef __ATPI_H
#define __ATPI_H

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
#define AT_GROUP_COMMON         0x0000
#define AT_GROUP_HEADSET        0x0100
#define AT_GROUP_HANDSFREE      0x0200
#define AT_GROUP_PHONEBOOK      0x0300
#define AT_GROUP_SMS            0x0400
#define AT_GROUP_DUN            0x0500

/* Maximum result "type" string size.  Based on "NO DIALTONE\0". */ 
#define RESULT_STAGE_LEN        12

/* Maximum command "type" string size.  Based on "+CIND\0". */ 
#define COMMAND_STAGE_LEN       6

/* Maximum supported indicator string size.  Based on "call_setup\0" */ 
#define INDICATOR_STAGE_LEN     11

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/* Internal parsing structures */ 
typedef struct _AtParameter {
    U16     offset;
    U8      len;
} AtParameter;

/****************************************************************************
 *
 * Function Prototypes
 *
 ****************************************************************************/

#if AT_HEADSET == XA_ENABLED

/*
 * Headset Group definitions
 */
extern const char *AT_HeadsetCmds[];

AtStatus AT_TEncode_Headset(U16                 Type, 
                            const AtHeadsetCmd *Command, 
                            XaBufferDesc       *Output);
AtStatus AT_TDecode_Headset(U16             *Type, 
                            AtHeadsetResult *Result, 
                            XaBufferDesc    *Input);
AtStatus AT_MEncode_Headset(U16                    Type, 
                            const AtHeadsetResult *Result, 
                            XaBufferDesc          *Output);
AtStatus AT_MDecode_Headset(U16          *Type, 
                            AtHeadsetCmd *Command, 
                            XaBufferDesc *Input);

#else /* AT_HEADSET == XA_ENABLED */ 

#define AT_HeadsetCmds          0

#endif /* AT_HEADSET == XA_ENABLED */ 

#if AT_HANDSFREE == XA_ENABLED

/*
 * Handsfree Group definitions
 */
extern const char *AT_HandsfreeCmds[];

AtStatus AT_TEncode_Handsfree(U16                   Type, 
                              const AtHandsfreeCmd *Command, 
                              XaBufferDesc         *Output);
AtStatus AT_TDecode_Handsfree(U16               *Type, 
                              AtHandsfreeResult *Result, 
                              XaBufferDesc      *Input);
AtStatus AT_MEncode_Handsfree(U16                      Type, 
                              const AtHandsfreeResult *Result, 
                              XaBufferDesc            *Output);
AtStatus AT_MDecode_Handsfree(U16            *Type, 
                              AtHandsfreeCmd *Command, 
                              XaBufferDesc   *Input);

#else /* AT_HANDSFREE == XA_ENABLED */

#define AT_HandsfreeCmds        0

#endif /* AT_HANDSFREE == XA_ENABLED */

#if AT_PHONEBOOK == XA_ENABLED

/*
 * Phonebook Group definitions
 */
extern const char *AT_PhonebookCmds[];

AtStatus AT_TEncode_Phonebook(U16                   Type, 
                              const AtPhonebookCmd *Command, 
                              XaBufferDesc         *Output);
AtStatus AT_TDecode_Phonebook(U16               *Type, 
                              AtPhonebookResult *Result, 
                              XaBufferDesc      *Input);
AtStatus AT_MEncode_Phonebook(U16                      Type, 
                              const AtPhonebookResult *Result, 
                              XaBufferDesc            *Output);
AtStatus AT_MDecode_Phonebook(U16            *Type, 
                              AtPhonebookCmd *Command, 
                              XaBufferDesc   *Input);

#else /* AT_PHONEBOOK == XA_ENABLED */

#define AT_PhonebookCmds        0

#endif /* AT_PHONEBOOK == XA_ENABLED */

#if AT_SMS == XA_ENABLED

/*
 * Short Message Service Group definitions
 */
extern const char *AT_SmsCmds[];

AtStatus AT_TEncode_Sms(AtContext      *Atc, 
                        U16             Type, 
                        const AtSmsCmd *Command, 
                        XaBufferDesc   *Output);
AtStatus AT_TDecode_Sms(AtContext    *Atc, 
                        U16          *Type, 
                        AtSmsResult  *Result, 
                        XaBufferDesc *Input);
AtStatus AT_MEncode_Sms(AtContext         *Atc, 
                        U16                Type, 
                        const AtSmsResult *Result, 
                        XaBufferDesc      *Output);
AtStatus AT_MDecode_Sms(AtContext    *Atc, 
                        U16          *Type, 
                        AtSmsCmd     *Command, 
                        XaBufferDesc *Input);

#else /* AT_SMS == XA_ENABLED */

#define AT_SmsCmds              0

#endif /* AT_SMS == XA_ENABLED */

#if AT_DUN == XA_ENABLED

/*
 * Dialup Networking Group definitions
 */
extern const char *AT_DunCmds[];

AtStatus AT_TEncode_Dun(AtContext      *Atc, 
                        U16             Type, 
                        const AtDunCmd *Command, 
                        XaBufferDesc   *Output);
AtStatus AT_TDecode_Dun(AtContext    *Atc, 
                        U16          *Type, 
                        AtDunResult  *Result, 
                        XaBufferDesc *Input);
AtStatus AT_MEncode_Dun(AtContext         *Atc, 
                        U16                Type, 
                        const AtDunResult *Result, 
                        XaBufferDesc      *Output);
AtStatus AT_MDecode_Dun(AtContext    *Atc, 
                        U16          *Type, 
                        AtDunCmd     *Command, 
                        XaBufferDesc *Input);

#else /* AT_DUN == XA_ENABLED */ 

#define AT_DunCmds              0

#endif /* AT_DUN == XA_ENABLED */ 

/*
 * Common Group definitions
 */
extern const char *AT_CommonCmds[];

AtStatus TE_Encode_Common(AtContext        *Atc, 
                          U16               Type, 
                          const AtCommands *Command, 
                          XaBufferDesc     *Output);
AtStatus TE_Decode_Common(AtContext    *Atc, 
                          U16          *Type, 
                          AtResults    *Result, 
                          XaBufferDesc *Input);

/* Utility macros to prevent warnings with casting */ 
#define AscToU8(a) ((U8)((a) - '0'))
#define U8ToAsc(a) ((char)((a) + '0'))

U8   AtItoA(U8 *buff, U32 val);
U32  AtAtoI(const U8 *a, U8 len);
U8 Athex16toA(U8 *buff, U16 val);

U8   AtParseParameters(U8         *input, 
                       U16         inputLen, 
                       AtParameter parms[AT_MAX_PARMS]);
void AtRemoveWhiteSpace(XaBufferDesc *Input);
void AtDebugOut(XaBufferDesc *Input);
char *AtMakeString(U8 *String, U16 Length);

#if XA_DEBUG_PRINT != XA_ENABLED

#define AtDebugOut(A)       (void)0

#endif /* XA_DEBUG_PRINT != XA_ENABLED */ 

/* Writes a string into the specified buffer. Assumes "len" is defined as 
 * U16 and overwrites it.  
 */

/* Writes the U16 provided as a string into the XaBuffer */ 
#define XaBufferWriteNum(b, num) \
    (b)->writeOffset = (U16)\
        ((b)->writeOffset + AtItoA((b)->buff + (b)->writeOffset, (num)));

#ifndef XaBufferWriteChar

/* Writes a single char into the buffer */
#define XaBufferWriteChar(b, c) \
    (b)->buff[(b)->writeOffset++] = c;

#endif /* XaBufferWriteChar */ 

#endif /* __ATPI_H */ 

