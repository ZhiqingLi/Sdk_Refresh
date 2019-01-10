/****************************************************************************
 *
 * File:
 *     $Id: at_hf.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains definitions for an AT Handsfree Command
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

#ifndef __AT_HF_H
#define __AT_HF_H

#include "atconfig.h"
//#include "hf.h"
#if AT_HANDSFREE == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT Hands-Free Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and
 * encoding AT commands and responses.  It is modularized into components
 * that support Headset, Hands-free, and DUN profiles.  It also provides
 * the ability to parse and encode Phonebook commands and responses, based
 * on the GSM specifications.  Support for SMS messaging commands will
 * be added in the future.
 *
 * The Hands-free Command Processor module provides the commands necessary
 * for the Hands-free Profile.  It supports commands for all specification
 * versions up to 1.5.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtCommand type
 *
 *     Hands-Free Command and Result types.
 */
#ifndef AT_COMMAND_DEFINED
#define AT_COMMAND_DEFINED
typedef U16 AtCommand;
#endif

/** Command to answer incoming call. No parameters. */
#define AT_ANSWER               0x0200

/** Command to dial a number from memory. The AtHandsfreeCmd.dial parameter
 *  is valid.
 */
#define AT_DIAL_MEMORY          0x0201

/** Command to dial a phone number. The AtHandsfreeCmd.dial parameter is
 *  valid.
 */
#define AT_DIAL_NUMBER          0x0202

/** This command allows control of the Call Waiting supplementary service.
 *  An unsolicited result is used to indicate the presence of a call.  The
 *  "wait" parameter is used with this command.
 */
#define AT_CALL_WAIT_NOTIFY     0x0203

/** This command allows calls can be put on hold, recovered, released, added
 *  to conversation, and transferred.  The test command can be issued to
 *  request a list of supported operations.  The AtHandsfreeCmd.hold
 *  parameter is valid.
 */
#define AT_CALL_HOLD            0x0204

/** This command instructs the ME to hang-up the current GSM call.  There
 *  are no parameters.
 */
#define AT_HANG_UP              0x0205

/** The test command is used to retrieve the mapping between each indicator
 *  supported by the ME and its corresponding range and order index.  The TE
 *  can retrieve the current status of the indicators by issuing a read
 *  command.  There are no parameters to this command.
 */
#define AT_INDICATORS           0x0206

/** This command enables/disables presentation of the calling line identity
 *  (CLI) to the subscriber.  An unsolicited result is used to indicate the
 *  identity to the TE.  The "callId" parameter is used with this command.
 */
#define AT_CALL_ID              0x0207

/** The TE may test and read the status of the ME indicators.  Received
 *  unsolicited indicator result messages are indicated by
 *  AT_INDICATOR_EVENT.
 */
#define AT_EVENT_REPORTING      0x0208

/** This unsolicited result code is used to report an indicator status
 *  change.
 */
#define AT_INDICATOR_EVENT      0x0209

/** This command allows the transmission of DTMF and arbitrary tones.  These
 *  tones may be used (for example) when announcing the start of a recording
 *  period.  The "dtmf" parameter is used with this command.
 */
#define AT_GENERATE_DTMF_TONE   0x020A

/** Command used for requesting the phone number corresponding to the last
 *  voice tag recorded in the ME.  There are no parameters to this command.
 */
#define AT_VOICE_TAG            0x020B

/** This command is used to call the last phone number dialed. On reception,
 *  the ME shall set up a voice call to the last phone number dialed.  There
 *  are no parameters to this command.
 */
#define AT_REDIAL               0x020C

/** This command enables/disables the voice recognition function in the ME.
 *  The unsolicited result code is used to indicate when the voice
 *  recognition setting was changed autonomously on the ME.  The "vrec"
 *  parameter is used with this command.
 */
#define AT_VOICE_RECOGNITION    0x020D

/** This command and result are used for exchanging the supported features
 *  list between the TE and ME.  The "features" parameter is used with this
 *  command.
 */
#define AT_SUPPORTED_FEATURES   0x020E

/** This command is issued to disable any Echo Canceling and Noise Reduction
 *  functions embedded in the ME.  There are no parameters to this command.
 */
#define AT_ECHO_C_AND_NOISE_R   0x020F

/** An unsolicited result code issued by the ME to indicate to the TE that
 *  the in-band ring tone setting has been locally changed.  The TE may
 *  react accordingly by changing its own alert method.
 */
#define AT_IN_BAND_RING_TONE    0x0210

/** This command is used by the TE to read the current "Response and Hold"
 *  state of the ME.  This command may also be used to set the response
 *  and hold state on the ME of an incoming call.  The unsolicited result
 *  code is used to indicate when an incoming call is either put on hold,
 *  accepted or rejected.  The "btrh" parameters are used with this
 *  command.
 */
#define AT_RESPONSE_AND_HOLD    0x0211

/** This command is used to request the Subscriber Number information from
 *  the ME.  The result provides "subscriberNum" parameters.
 */
#define AT_SUBSCRIBER_NUM       0x0212

/** This command is used to request a list of the current calls.  The result
 *  indicates each current call and its information.  The "currentCall"
 *  parameter is used with the result of this command.
 */
#define AT_LIST_CURRENT_CALLS   0x0213

/** This command is used for reading, selecting and registering the network
 *  operator.  The response is used to return the current mode and the
 *  currently selected operator.  The "networkOper" and "networkOper_read"
 *  parameters are used with this command.
 */
#define AT_NETWORK_OPERATOR     0x0214

/** This command is used to activate and deactivate reporting for individual
 *  indicators.  Within the AtHandsfreeCmd structure, the iia.activationStr
 *  parameter is valid.
 */
#define AT_IIA                  0x0215



//wangjianjun
#if  1//HF_VERSION_1_6 == XA_ENABLED
#define AT_CODEC_NEGOTIATION   0x0216  /* AT+BAC=1,2  */

#define AT_CODEC_CONNECTION 0x0217  /* AT+BCC */

#define AT_CODEC_SELECTION      0x0218   /* AT+BCS */

#endif
/*This command is used to Enables custom AT commands from an accessory*/
#define AT_BT_ACCESSORY          0x219   /*AT+XAPL*/


/*This command is used to Reports a headset state change.*/
#define AT_HANDSET_ACCESSORY   0x021a   /*AT+IPHONEACCEV*/

//Siri add.
//group:02;
//Id in AT_HandsfreeCmds is 0x1b
#define AT_ACCESSORY_APLSIRI   0x021b   /*AT+APLSIRI*/

//zjq add for hfp1.7 hf_indicators
#define AT_HF_INDICATORS           0x021c  /* AT+BIND= *///List HF supported indicators
                                                                  /* AT+BIND=? *///Read AG supported indicators
                                                                  /* AT+BIND? *///Read AG enabled/disabled status of indicators
#define AT_UPDATE_HF_INDICATORS_VALUE  0x021d  /* AT+BIEV */

#define AT_BES_TEST                    0x021e  /* AT+BESTEST */

/* End of AtCommand */


/*--------------------------------------------------------------------------
 * AtHoldAction type
 *
 *
 */
typedef U8  AtHoldAction;

/** Releases all held calls or sets User Determined User Busy
 *  (UDUB) for a waiting call.
 */
#define AT_HOLD_RELEASE_HELD_CALLS      0

/** Releases all active calls (if any exist) and accepts the other
 *  (held or waiting) call.  If "call" is specified, will release the
 *  specific call.
 */
#define AT_HOLD_RELEASE_ACTIVE_CALLS    1

/** Places all active calls (if any exist) on hold and accepts the other
 *  (held or waiting) call. If "call" is specified, places all active calls
 *  on hold except the specified call.
 */
#define AT_HOLD_HOLD_ACTIVE_CALLS       2

/** Adds a held call to the conversation.
 */
#define AT_HOLD_ADD_HELD_CALL           3

/** Connects the two calls and disconnects the AG from both calls
 *  (Explicit Call Transfer).
 */
#define AT_HOLD_EXPLICIT_TRANSFER       4

/* End of AtHoldAction */

//wangjianjun
/*---------------------------------------------------------------------------
 * AtHfFeatures
 *
 */
typedef U32 AtHfFeatures;

/** */
#define AT_HFF_ECHO_AND_NOISE           0x00000001

/** */
#define AT_HFF_CALL_WAITING             0x00000002

/** */
#define AT_HFF_CLI_PRESENTATION         0x00000004

/** */
#define AT_HFF_VOICE_RECOGNITION        0x00000008

/** */
#define AT_HFF_VOLUME_CONTROL           0x00000010

#if HF_VERSION_1_6 == XA_ENABLED
#define AT_HFF_WIDE_BAND_SPEECH        0x00000020
#endif

/* End of AtHfFeatures */

/*---------------------------------------------------------------------------
 * AtAgFeatures
 *
 */
typedef U32 AtAgFeatures;

/** */
#define AT_AGF_THREE_WAY_CALLING        0x00000001

/** */
#define AT_AGF_ECHO_AND_NOISE           0x00000002

/** */
#define AT_AGF_VOICE_RECOGNITION        0x00000004

/** */
#define AT_AGF_IN_BAND_RING_TONE        0x00000008

/** */
#define AT_AGF_VOICE_TAG                0x00000010

/** */
#define AT_AGF_REJECT_CALL              0x00000020

#define AT_AGF_WIDE_BAND_SPEECH 0x00000040
/* End of AtAgFeatures */

/*--------------------------------------------------------------------------
 * AtHoldFlag type
 *
 *
 */
typedef U8 AtHoldFlag;

/** Releases all held calls or sets User Determined User Busy
 *  (UDUB) for a waiting call.
 */
#define AT_FLAG_HOLD_RELEASE_HELD_CALLS      0x01

/** Releases all active calls (if any exist) and accepts the other
 *  (held or waiting) call.
 */
#define AT_FLAG_HOLD_RELEASE_ACTIVE_CALLS    0x02

/** Releases a specific call. */
#define AT_FLAG_HOLD_RELEASE_SPECIFIC_CALL   0x04

/** Places all active calls (if any exist) on hold and accepts the
 *  other (held or waiting) call.
 */
#define AT_FLAG_HOLD_HOLD_ACTIVE_CALLS       0x08

/** Places a specific call on hold. */
#define AT_FLAG_HOLD_HOLD_SPECIFIC_CALL      0x10

/** Adds a held call to the conversation. */
#define AT_FLAG_HOLD_ADD_HELD_CALL           0x20

/** Connects the two calls and disconnects the AG from
 * both calls (Explicit Call Transfer).
 */
#define AT_FLAG_HOLD_EXPLICIT_TRANSFER       0x40

/* End of AtHoldFlag */

/*---------------------------------------------------------------------------
 * AtResponseHold
 *
 */
typedef U8 AtResponseHold;

/** Put the incoming call on hold (or the incoming call was put on hold).
 */
#define ATRH_ON_HOLD            0

/** Accept the incoming held call (or the call on hold was accepted).
 */
#define ATRH_ACCEPT_HELD        1

/** Reject the incoming held call (or the call on hold was rejected).
 */
#define ATRH_REJECT_HELD        2

/* End of AtResponseHold */

/*---------------------------------------------------------------------------
 * AtCallState
 *
 */
typedef U8 AtCallState;

/** Active */
#define ATCS_ACTIVE             0

/** Held */
#define ATCS_HELD               1

/**  Dialing (Mobile Originated only) */
#define ATCS_DIALING            2

/** Alerting (Mobile Originated only) */
#define ATCS_ALERTING           3

/** Incoming (Mobile Terminated only) */
#define ATCS_INCOMING           4

/** Waiting (Mobile Terminated only) */
#define ATCS_WAITING            5

/** Response an Hold.  See the Hands Free Specification Errata 2716, which
 *  adds this new call state to the CLCC messages.
 */
#define ATCS_RESPHOLD           6

/* End of AtCallState */

/*---------------------------------------------------------------------------
 * AtOperFormat
 *
 */
typedef U8 AtOperFormat;

/** */
#define ATOF_LONG_ALPHANUM     0

/** */
#define ATOF_SHORT_ALPHANUM    1

/** */
#define ATOF_NUMERIC           2

/* End of AtOperFormat */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtHandsfreeCmd
 *
 *     This structure is used to specify the parameters associated with
 *     Hands-Free commands.  Hands-Free commands are sent from the
 *     Hands-Free unit to the Audio Gateway.  As such, this structure is
 *     used by the Hands-Free unit encoder, and the Audio Gateway decoder
 *     functions.
 */
typedef union _AtHandsfreeCmd {

    /* AT_DIAL_NUMBER, AT_DIAL_MEMORY */
    struct {
        const char     *number;
    } dial;

    /* AT_CALL_WAIT_NOTIFY */
    struct {
        /* Enable/Disable the presentation of the AT_CALL_WAIT_NOTIFY
         * unsolicited result code.
         */
        U8              notify;
    } wait;

    /* AT_CALL_HOLD */
    struct {

        /* Call hold procedure to perform. */
        AtHoldAction    action;

        /* If "action" is AT_HOLD_RELEASE_ACTIVE_CALLS or
         * AT_HOLD_HOLD_ACTIVE_CALLS, this value can be used to specify
         * the index (1 - 9) of a specific call to address.
         */
        U8              call;

    } hold;

    /* AT_CALL_ID */
    struct {
        BOOL            enabled;
    } callId;

    /* AT_EVENT_REPORTING */
    struct {
        U8              mode;
        U8              ind;
    } report;

    /* AT_GENERATE_DTMF_TONE */
    struct {
        U8              tone;
    } dtmf;

    /* AT_VOICE_RECOGNITION */
    struct {
        BOOL            enabled;
    } vrec;

    struct {
        AtHfFeatures    bitmap;
    } features;

    /* AT_RESPONSE_AND_HOLD */
    struct {
        AtResponseHold  setting;
    } btrh;

    /* AT_NETWORK_OPERATOR */
    struct {

        /* 0 = automatic, 1 = manual, 2 = deregister, 3 = set format only,
         * 4 = manual/automatic.
         */
        U8              mode;

        /* Format of "oper" parameter (0-3) */
        AtOperFormat    format;

    } networkOper;

    /* AT_IIA */
    struct {

        /* A string of up to 20 zeroes and ones separated by commas.  One
         * means activate, and zero means deactivate.  The zeroes and ones
         * can be left out for individual indicators whose reporting state
         * should not change.
         */
        const char     *activationStr;

    } iia;

#if HF_VERSION_1_6 == XA_ENABLED
    struct {
        BOOL    at_msbc_enabled;
        U16   at_negotiated_codec;
    } msbc;
#endif

#if HF_CUSTOM_FEATURE_SUPPORT
    struct{
        U16 vendor_id;
        U16 product_id;
        U16 version;
        U16 feature;
    } bt_accessory;
#endif

#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_BATTERY_REPORT
    struct{
        U8  key_num;
        U8  battery_report;
        U8  battery_level;
    } bt_battery_report;
#endif

#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
    struct{
        U8  request;
    } bt_siri_report;
#endif

    struct{
        U8 content;
    }bes_test;

    struct{
        U16 enhanced_safety_num;
        U16  battery_level_num;
    }hf_indicators;//AT_LIST_HF_INDICATORS

    struct{
        U16 assigned_num;
        U8 value;
    }hf_indicators_value;//AT_UPDATE_HF_INDICATORS_VALUE

} AtHandsfreeCmd;


/*---------------------------------------------------------------------------
 * AtHandsfreeResult
 *
 *     This structure is used to specify the parameters associated with
 *     Hands-Free results.  Hands-Free results are sent from the Audio
 *     Gateway to the Hands-Free unit.  As such, this structure is used by
 *     the Audio Gateway encoder, and the Hands-Free unit decoder
 *     functions.
 */
typedef union _AtHandsfreeResult {

    /* AT_CALL_WAIT_NOTIFY */
    struct {
        const char     *number;
        U8              classmap;
        AtNumberFormat  type;
    } wait;

    /* AT_CALL_HOLD | AT_TEST */
    struct {
        AtHoldFlag      flags;
    } hold_test;

    /* AT_INDICATORS | AT_TEST */
    struct {

        /* Number of indicators reported. */
        U8              num;

        struct {

            /* Description of indicator */
            const char *description;

            /* Range of values supported by indicator (e.g. "0,1").
             * Quotes must not be included in this string.
             */
            const char *range;

        } ind[AT_MAX_INDICATORS];

    } ind_test;

    /* AT_INDICATORS | AT_READ */
    struct {

        /* Number of indicators reported. */
        U8              num;

        struct {

            /* Value of indicator. */
            U8          value;

        } ind[AT_MAX_INDICATORS];
    } indicators_read;

    /* AT_CALL_ID */
    struct {
        const char     *number;
        AtNumberFormat  type;
    } callId;

    /* AT_INDICATOR_EVENT */
    struct {

        /* One based indicator index. */
        U8              indicator;

        /* Current value of this indicator. */
        U8              value;

    } indEvent;

    /* AT_VOICE_TAG */
    struct {
        const char     *number;
    } voiceTag;

    /* AT_VOICE_RECOGNITION */
    struct {
        BOOL            enabled;
    } vrec;

    /* AT_SUPPORTED_FEATURES */
    struct {
        AtAgFeatures    bitmap;
    } features;

    /* AT_IN_BAND_RING_TONE */
    struct {
        BOOL            enabled;
    } inBandRing;

    /* AT_RESPONSE_AND_HOLD */
    struct {
        AtResponseHold  setting;
    } btrh;

    /* AT_SUBSCRIBER_NUM */
    struct {

        /* String phone number of format specified by "type". */
        const char     *number;

        /* Phone number format */
        AtNumberFormat  type;

        /* Service related to the phone number. */
        U8              service;

    } subscribeNum;

    /* AT_LIST_CURRENT_CALLS */
    struct {

        /* The numbering (starting with 1) of the call given by the sequence
         * of setting up or receiving the calls (active, held or waiting) as
         * seen by the served subscriber.  Calls hold their number until
         * they are released. New calls take the lowest available number.
         */
        U8              index;

        /* Direction: 0 (Mobile Originated), 1 (Mobile Terminated),
         * 0xFF (Value not present)
         */
        U8              dir;

        /* Call State */
        AtCallState     state;

        /* Mode: 0=Voice, 1=Data, 2=FAX */
        U8              mode;

        /* Multiparty 0=Not Multiparty, 1=Multiparty. */
        U8              multiParty;

        /* Phone number */
        const char     *number;

        /* Phone number type */
        AtNumberFormat  type;

    } currentCalls;

    /* AT_NETWORK_OPERATOR */
    struct {

        /* 0 = automatic, 1 = manual, 2 = deregister, 3 = set format only,
         * 4 = manual/automatic.
         */
        U8              mode;

        /* Format of "oper" parameter (0-3) */
        AtOperFormat    format;

        /* Numeric or long or short alphanumeric name of operator */
        const char     *oper;

    } networkOper_read;


#if HF_VERSION_1_6 == XA_ENABLED
   /* AT_BCS */
       struct {
        U16   codec_id;
    } negotiated_codec ;
#endif

#if HF_CUSTOM_FEATURE_SUPPORT
    struct{
        const char *device;
        U8  accessory_feature;
    }bt_accessory;
#endif

#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
    struct{
        U8  status;
    }apl_siri;
#endif

    struct{
        U8 content;
    }bes_test;

    //AT_READ_AG_INDICATORS
    struct{
        U16 assigned_num[AT_MAX_INDICATORS];
        U8  indicators_count;
    }ag_indicators;

    //AT_READ_AG_INDICATORS_STATUS
    struct{
        U16 assigned_num;
        U8  status;
    }ag_indicators_status;

} AtHandsfreeResult;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

#if AT_ROLE_TERMINAL == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT_MapHfIndicators()
 *
 *     This function is used to create a mapping table that matches the
 *     remote device indicators to the locally supported indicators.
 *
 * Requires:
 *     AT_ROLE_TERMINAL be enabled.
 *
 * Parameters:
 *     Known - Table of indicators supported locally.  The table is
 *         terminated with a null entry.  All indicator table entries must
 *         be in upper case, matching will be performed case insensitively.
 *
 *     Map - Table that receives mapping information. Upon completion the
 *         expression Map[<remote indicator> - 1] will return the matching
 *         local indicator index into "Known".  One must be subtracted from
 *         the remote indicator value because indicators are one based.
 *         This table must have the same number of entries as the "Known"
 *         table.  If a Map table entry contains 0xff, the indicator is not
 *         supported by the remote device.
 *
 *     maxNumInd - This value must contain the maximum allowable number of
 *         indicators that can be handled by the code.
 *
 *     Remote - A pointer to an AtHandsfreeResult structure that represents
 *         the remote devices indicators.
 *
 * Returns:
 *     AT_STATUS_OK - Indicators have been successfully mapped.
 *
 *     AT_STATUS_INVALID_PARM - A parameter is invalid.
 */
void AT_MapHfIndicators(const char *Known[],
                        U8 Map[],
                        const U8 maxNumInd,
                        AtHandsfreeResult *Remote);

#endif /* AT_ROLE_TERMINAL == XA_ENABLED */

#else /* AT_HANDSFREE == XA_ENABLED */

/* Stub structures to keep #defines out
 * of AtResults/AtCommands structures.
 */
typedef U8 AtHandsfreeCmd;
typedef U8 AtHandsfreeResult;

#endif /* AT_HANDSFREE == XA_ENABLED */

#endif /* __AT_HF_H */

