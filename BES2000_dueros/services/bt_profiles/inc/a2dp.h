/****************************************************************************
 *
 * File:
 *     $Id: a2dp.h 2748 2009-08-06 19:14:20Z joshs $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 2748 $
 *
 * Description: This file contains public definitions for the Advanced Audio 
 *     Distribution Profile (A2DP).
 *
 * Created:     June 2, 2004
 *
 * Copyright 2004-2005 Extended Systems, Inc.
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

#ifndef __A2DP_H_
#define __A2DP_H_

#include "avdtp.h"
#include "conmgr.h"
#include "sbc.h"
#include "avdevice.h"

/*---------------------------------------------------------------------------
 * A2DP API layer
 *
 *     Advanced Audio Distribution Profile (A2DP) is the Profile 
 *     defined by the Bluetooth Audio Video Working Group for 
 *     streaming audio content over a Bluetooth connection.  
 *
 *     The A2DP API is designed for audio applications using the 
 *     BES Blue SDK core protocol stack.  It provides functions 
 *     for connection management, audio stream discovery, audio stream
 *     management, and security.
 *
 *     Applications that use the A2DP API will also require knowledge of 
 *     certain AVDTP (Audio/Video Distribution Transport Protocol) types as 
 *     referenced by this API.  Although some AVDTP data structures are 
 *     used by A2DP, applications should use either the AVDTP API or 
 *     the A2DP API and not mix the two.  
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * A2DP_SOURCE constant
 *    This value determines whether code to support an A2DP source is 
 *    included or not.  Set this value to XA_ENABLED to include the A2DP 
 *    source.  
 */
#ifndef A2DP_SOURCE
#define A2DP_SOURCE XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * A2DP_SINK constant
 *    This value determines whether code to support an A2DP sink is included 
 *    or not.  Set this value to XA_ENABLED to include the A2DP sink.  
 */
#ifndef A2DP_SINK
#define A2DP_SINK XA_ENABLED
#endif

#if A2DP_SOURCE != XA_ENABLED && A2DP_SINK != XA_ENABLED
#error Either A2DP_SOURCE or A2DP_SINK should be enabled
#endif

/*---------------------------------------------------------------------------
 * A2DP_MINOR_DEVICE_CLASS constant
 *    This value defines the minor device class.  An appropriate value can 
 *    be found in me.h under "Group: Minor Device Class - Audio Major 
 *    class."  
 */
#ifndef A2DP_MINOR_DEVICE_CLASS
#define A2DP_MINOR_DEVICE_CLASS COD_MINOR_AUDIO_HIFIAUDIO
#endif

/*---------------------------------------------------------------------------
 * A2DP_SRC_FEATURES constant
 *    This value defines the features supported by the A2DP source.  
 */
#ifndef A2DP_SRC_FEATURES
#define A2DP_SRC_FEATURES A2DP_SRC_FEATURE_PLAYER
#endif
 
/*---------------------------------------------------------------------------
 * A2DP_SNK_FEATURES constant
 *    This value defines the features supported by the A2DP sink.  
 */
#ifndef A2DP_SNK_FEATURES
#define A2DP_SNK_FEATURES (A2DP_SNK_FEATURE_SPEAKER | A2DP_SNK_FEATURE_HEADPHONE)
#endif
 
#if (AVDTP_RTX_SIG_TIMEOUT > 3000) || (AVDTP_RTX_SIG_TIMEOUT < 500)
#error AVDTP_RTX_SIG_TIMEOUT must be between 500 and 3000
#endif

#ifndef AV_WORKER
#if L2CAP_MTU < 335
#error L2CAP_MTU must be 335 or greater
#endif
#endif

#if BT_PACKET_HEADER_LEN < 25
#error BT_PACKET_HEADER_LEN must be at least 25 (l2cap + SBC media header)
#endif

/*---------------------------------------------------------------------------
 * A2DP_MAX_STREAMINFOS constant
 *    This constant defines the maximum number of remote streams which 
 *    will be cached during discovery of remote streams (during the 
 *    A2DP_OpenStream process). At least one must be present. If this 
 *    number is too small, additional streams will be ignored.  
 */
#ifndef A2DP_MAX_STREAMINFOS
#define A2DP_MAX_STREAMINFOS 8
#endif

#if A2DP_MAX_STREAMINFOS < 1
#error A2DP_MAX_STREAMINFOS must be at least 1
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * A2dpEvent type
 *      
 *     All indications and confirmations are sent through a callback 
 *     function as events. The type of event will determine which 
 *     fields in the A2dpCallbackParms structure are valid.  The 'len', and 
 *     'error' fields are always valid.  The 'event' field contains the event 
 *     associated with the callback.  
 */
typedef U8 A2dpEvent;
                                        
/** This event is received by the application when the remote device is 
 *  requesting that stream be opened.  The application can accept or reject 
 *  this request by calling A2DP_OpenStreamRsp().  
 * 
 *  During this callback, the 'p.configReq' parameter is valid.  
 */
#define A2DP_EVENT_STREAM_OPEN_IND           1

/** This event is received by the application when a stream is open.  This 
 *  can happen in response to a call from A2DP_OpenStream() or 
 *  A2D_OpenStreamRsp().  
 * 
 *  During this callback, the 'p.configReq' parameter is valid.  
 */
#define A2DP_EVENT_STREAM_OPEN               2

/** This event is received by the application during the establishment of an 
 *  outgoing stream (A2DP_OpenStream()) to indicate the capabilities of the 
 *  remote device.  This event may also be received after calling 
 *  A2DP_GetStreamCapabilities().  
 * 
 *  During this callback, the 'p.codec' parameter is valid.  
 */
#define A2DP_EVENT_CODEC_INFO                3

/** This event is received by the application during the establishment of an 
 *  outgoing stream (A2DP_OpenStream()) to indicated the capabilities of the 
 *  remote device.  This event may also be received after calling 
 *  A2DP_GetStreamCapabilities().  
 * 
 *  During this callback, the 'p.cp' parameter is valid.  
 */
#define A2DP_EVENT_CP_INFO                   4

/** This event is received by the application during the establishment of an 
 *  outgoing stream (A2DP_OpenStream()) to indicated the capabilities of the 
 *  remote device.  If this event is received, then the remote device
 *  supports the Delay Reporting feature.
 */
#define A2DP_EVENT_DELAY_REPORTING_INFO      5

/** This event is received by the application when a stream is opening and 
 *  all matching capabilities have been found.  The application must call 
 *  A2DP_SetStreamConfig() to configure the stream.  If successful, the 
 *  stream will be opened.  A2DP_CloseStream() can also be called to close 
 *  the stream.  This event is only received on outgoing connections.  
 */
#define A2DP_EVENT_GET_CONFIG_IND            6

/** This event is received after calling A2DP_GetStreamCapabilities().  This 
 *  event is received after all the capabilities of the remote device have 
 *  been indicated (see A2DP_EVENT_CODEC_INFO and A2DP_EVENT_CP_INFO).  
 */
#define A2DP_EVENT_GET_CAP_CNF               7

/** This event is received by the application when an open stream has been 
 *  closed.  This can happen as a result of a call to A2DP_CloseStream(), if 
 *  the stream has been closed by the remote device, if a link loss has been 
 *  detected, or if the remote device rejects a request to open the stream.  
 *
 *  During this callback, the 'p.capability' parameter will be valid if the 
 *  stream is closed because the configuration was rejected.  The 
 *  'p.capability.type' field will be set to the capability type that was 
 *  rejected.  The 'len' field will be non-zero if the 'p.capability' 
 *  parameter is valid.  
 *
 *  The 'error' field will contain the error code. If the close was 
 *  successful, 'error' will contain AVDTP_ERR_NO_ERROR.  
 *  
 *  If the stream was closed because of an L2CAP disconnect, the 'status'
 *  field will contain the L2CAP disconnect reason (L2capDiscReason),
 *  otherwise it will contain L2DISC_REASON_UNKNOWN.
 */
#define A2DP_EVENT_STREAM_CLOSED             8

/** This event is received by the application when an open stream has been 
 *  set to the idle state.  This happens as a result of a call to 
 *  A2DP_IdleStream().  
 */
#define A2DP_EVENT_STREAM_IDLE               9

/** When the remote device requests streaming to begin, this event will be 
 *  received by the application.  The application can accept or reject 
 *  this request by calling A2DP_StartStreamRsp(). (Note: this event 
 *  will be received only after A2DP_EVENT_STREAM_OPEN_IND but it may arrive 
 *  before A2DP_EVENT_STREAM_OPEN.)  
 */
#define A2DP_EVENT_STREAM_START_IND         10

/** When streaming has been started, this event will be received by the 
 *  application.  This can happen as the result to a call to 
 *  A2DP_StartStream() or A2DP_StartStreamRsp().  
 */
#define A2DP_EVENT_STREAM_STARTED           11

/** When streaming has been suspended, this event will be received by the 
 *  application.  This happens as the result to a call to 
 *  A2DP_SuspendStream(), or when the remote device suspends the stream.  
 */
#define A2DP_EVENT_STREAM_SUSPENDED         12

/** When the remote device wishes to reconfigure an open stream, this event 
 *  is received by the application.  The application can accept or reject 
 *  the request by calling A2DP_ReconfigStreamRsp().  
 * 
 *  During this callback, the 'p.configReq' parameter will be valid.  
 */
#define A2DP_EVENT_STREAM_RECONFIG_IND      13

/** When an open stream is reconfigured, this event is received by the 
 *  application.  
 * 
 *  During this callback, the 'p.configReq' parameter is valid when the 
 *  'error' field is set to AVDTP_ERR_NO_ERROR.  Otherwise, 
 *  'p.capability.type' contains the capability that caused the failure.  
 */
#define A2DP_EVENT_STREAM_RECONFIG_CNF      14

/** This event is received when the remote device requests the security 
 *  process to begin.  The application responds to this request by calling 
 *  A2DP_SecurityControlRsp().  
 * 
 *  If 'error' contains no error, the 'p.data' parameter is valid.  
 */
#define A2DP_EVENT_STREAM_SECURITY_IND      15

/** This event is received by the application when the remote device responds 
 *  to the security process request.  
 * 
 *  If 'error' contains no error, the 'p.data' parameter is valid.  
 */
#define A2DP_EVENT_STREAM_SECURITY_CNF      16

/** When the stream is aborted, this event is received by the application.  
 *  This can happen in response to a request from the remote device to abort 
 *  the stream, or as the result of a call to A2DP_AbortStream().  When a 
 *  stream is aborted, the stream is closed.  
 */
#define A2DP_EVENT_STREAM_ABORTED           17

/** This event is received when stream data has been received from the remote 
 *  device.  The data is raw and is not parsed by A2DP.  It should contain 
 *  a single media packet.  
 * 
 *  If 'error' contains no error, the 'p.data' parameter is valid.  
 */
#define A2DP_EVENT_STREAM_DATA_IND          18

/** This event is received when raw data has been sent to the remote device.  
 *  This happens as the result of a call to A2DP_StreamSendRawPacket().  
 * 
 *  During this callback, the 'p.packet' parameter is valid.  
 */
#define A2DP_EVENT_STREAM_PACKET_SENT       19

/** This event is received when SBC data has been sent to the remote device.  
 *  This happens as the result of a call to A2DP_StreamSendSbcPacket().  
 */
#define A2DP_EVENT_STREAM_SBC_PACKET_SENT   20

/** This event is received by a Source when the Sink reports the value of its
 *  buffer/processing delay.  This may happen when the stream is configured
 *  (or reconfigured), and when the stream is in the streaming state.
 *  
 *  During this callback, the 'p.delayMs' parameter is valid.
 */
#define A2DP_EVENT_DELAY_REPORT_IND         21

/** This event is received by a Sink when the Source acknowldeges the
 *  transmitted buffer/processing delay.
 */
#define A2DP_EVENT_DELAY_REPORT_CNF         22

/** When the the lower level AVDTP connection is established between the
 *  local and remote devices, this event will be generated.
 *  
 *  During this callback, the 'p.device' parameter contains a pointer to the
 *  device that was connected.
 */
#define A2DP_EVENT_AVDTP_CONNECT            23

/** When the the lower level AVDTP connection is disconnected, this event
 *  will be generated.
 *  
 *  During this callback, the 'p.device' parameter contains a pointer to the
 *  device that was disconnected.  The 'status' parameter contains the L2CAP
 *  disconnect reason (L2capDiscReason).  If 'status' equals
 *  L2DISC_LINK_DISCON, then the 'discReason' parameter contains the reason
 *  for the link disconnect (BtErrorCode).
 */
#define A2DP_EVENT_AVDTP_DISCONNECT         24

#ifdef AV_WORKER
/* Confirms AVDTP has received configuration information after an 
 * AVDTP_GetConfig() request.  
 */
#define A2DP_AVDTP_EVENT_GET_CFG_CNF        27

/* Confirms AVDTP has sent configuration information after an 
 * AVDTP_OpenStream() request.  
 */
#define A2DP_AVDTP_EVENT_SET_CFG_CNF        28

#endif

/* End of A2dpEvent */ 

/*---------------------------------------------------------------------------
 * A2dpError type
 * 
 *     Error codes for the A2DP profile.  If errors are detected in the
 *     Codec or Content protection requests, these error codes are used.
 *     Each error code description indicates the codec to which the error
 *     is applicable.
 *
 *     This variable type can also contain AVDTP errors.  See AvdtpError
 *     for more error codes.
 */
typedef AvdtpError A2dpError;

/** No error
 */
#define A2DP_ERR_NO_ERROR                         0x00

/** Bad Service
 * 
 *  Generic
 */
#define A2DP_ERR_BAD_SERVICE                      0x80

/** Insufficient Resources 
 * 
 *  Generic
 */
#define A2DP_ERR_INSUFFICIENT_RESOURCE            0x81

/** Codec media type not valid
 *
 *  Generic
 */
#define A2DP_ERR_INVALID_CODEC_TYPE               0xC1

/** Codec media type not supported
 * 
 *  Generic
 */
#define A2DP_ERR_NOT_SUPPORTED_CODEC_TYPE   AVDTP_ERR_NOT_SUPPORTED_CODEC_TYPE

/** Sampling frequency not valid
 * 
 *  Generic
 */
#define A2DP_ERR_INVALID_SAMPLING_FREQUENCY       0xC3

/** Sampling frequency not supported
 * 
 *  Generic
 */
#define A2DP_ERR_NOT_SUPPORTED_SAMP_FREQ          0xC4

/** Channel mode not valid
 * 
 *  SBC
 *  MPEG-1,2 Audio
 *  ATRAC family
 */
#define A2DP_ERR_INVALID_CHANNEL_MODE             0xC5

/** Channel mode not supported
 * 
 *  SBC
 *  MPEG-1,2 Audio
 *  ATRAC family
 */
#define A2DP_ERR_NOT_SUPPORTED_CHANNEL_MODE       0xC6

/** Number of subbands not valid
 * 
 *  SBC
 */
#define A2DP_ERR_INVALID_SUBBANDS                 0xC7

/** Number of subbands not supported
 * 
 *  SBC
 */
#define A2DP_ERR_NOT_SUPPORTED_SUBBANDS           0xC8

/** Allocation method not valid
 * 
 *  SBC
 */
#define A2DP_ERR_INVALID_ALLOCATION_METHOD        0xC9

/** Allocation method not supported
 * 
 *  SBC
 */
#define A2DP_ERR_NOT_SUPPORTED_ALLOC_METHOD       0xCA

/** Minimum bitpool value not valid
 * 
 *  SBC
 */
#define A2DP_ERR_INVALID_MIN_BITPOOL_VALUE        0xCB

/** Minimum bitpool value not supported
 * 
 *  SBC
 */
#define A2DP_ERR_NOT_SUPPORTED_MIN_BITPOOL_VALUE  0xCC

/** Maximum bitpool value not valid
 * 
 *  SBC
 */
#define A2DP_ERR_INVALID_MAX_BITPOOL_VALUE        0xCD

/** Maximum bitpool value not supported
 * 
 *  SBC
 */
#define A2DP_ERR_NOT_SUPPORTED_MAX_BITPOOL_VALUE  0xCE

/** None or multiple values have been selected for Layer
 * 
 *  MPEG-1,2 Audio
 */
#define A2DP_ERR_INVALID_LAYER                    0xCF

/** Layer is not supported
 * 
 *  MPEG-1,2 Audio
 */
#define A2DP_ERR_NOT_SUPPORTED_LAYER              0xD0

/** CRC is not supported
 * 
 *  MPEG-1,2 Audio
 */
#define A2DP_ERR_NOT_SUPPORTED_CRC                0xD1

/** MPF-2 is not supported
 * 
 *  MPEG-1,2 Audio
 */
#define A2DP_ERR_NOT_SUPPORTED_MPF                0xD2

/** VBR is not supported
 * 
 *  MPEG-1,2 Audio
 *  MPEG-2,4 AAC
 *  ATRAC family
 */
#define A2DP_ERR_NOT_SUPPORTED_VBR                0xD3

/** None or multiple values have been selected for Bit Rate
 * 
 *  MPEG-1,2 Audio
 *  ATRAC family
 */
#define A2DP_ERR_INVALID_BIT_RATE                 0xD4

/** Bit Rate is not supported
 * 
 *  MPEG-1,2 Audio
 *  MPEG-2,4 AAC
 *  ATRAC family
 */
#define A2DP_ERR_NOT_SUPPORTED_BIT_RATE           0xD5

/** Either 1) Object type is not valid (b3-b0) or 2) None or multiple values 
 *  have been selected for Object Type
 * 
 *  MPEG-2,4 AAC
 */
#define A2DP_ERR_INVALID_OBJECT_TYPE              0xD6

/** Object Type is not supported
 * 
 *  MPEG-2,4 AAC
 */
#define A2DP_ERR_NOT_SUPPORTED_OBJECT_TYPE        0xD7

/** None or multiple values have been selected for Channels
 * 
 *  MPEG-2,4 AAC
 */
#define A2DP_ERR_INVALID_CHANNELS                 0xD8

/** Channels is not supported
 * 
 *  MPEG-2,4 AAC
 */
#define A2DP_ERR_NOT_SUPPORTED_CHANNELS           0xD9

#define A2DP_AAC_OCTET0_MPEG2_AAC_LC              0x80
#define A2DP_AAC_OCTET0_MPEG4_AAC_LC              0x40
#define A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100  0x01
#define A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000  0x80
#define A2DP_AAC_OCTET2_CHANNELS_1                0x08
#define A2DP_AAC_OCTET2_CHANNELS_2                0x04
#define A2DP_AAC_OCTET3_VBR_SUPPORTED             0x80

#define A2DP_AAC_OCTET_NUMBER                     (6)
#if defined(A2DP_SCALABLE_ON)
/** 
 *
 * scalable codec
 *
 */
#define A2DP_SCALABLE_OCTET_NUMBER                (7)
#endif
/** Version is not valid
 * 
 *  ATRAC family
 */
#define A2DP_ERR_INVALID_VERSION                  0xDA

/** Version is not supported
 * 
 *  ATRAC family
 */
#define A2DP_ERR_NOT_SUPPORTED_VERSION            0xDB

/** Maximum SUL is not acceptable for the Decoder in the SNK
 * 
 *  ATRAC family
 */
#define A2DP_ERR_NOT_SUPPORTED_MAXIMUM_SUL        0xDC

/** None or multiple values have been selected for Block Length
 * 
 *  SBC
 */
#define A2DP_ERR_INVALID_BLOCK_LENGTH             0xDD

/** The requested CP Type is not supported
 * 
 *  Generic
 */
#define A2DP_ERR_INVALID_CP_TYPE                  0xE0

/** The format of Content Protection Service Capability/Content Protection 
 *  Scheme Dependent Data is not correct 
 * 
 *  Generic
 */
#define A2DP_ERR_INVALID_CP_FORMAT                0xE1

/** Unknown error
 */
#define A2DP_ERR_UNKNOWN_ERROR                    AVDTP_ERR_UNKNOWN_ERROR

/* End of A2dpError */ 


/*---------------------------------------------------------------------------
 * A2dpVersion type
 *
 *     Describes the version of the A2DP profile (0x0100 means version 1.0).  
 */
typedef U16 A2dpVersion;

/* End of A2dpVersion */

/*---------------------------------------------------------------------------
 * A2dpFeatures type
 *
 *     Describes the features of a source or sink.  
 */
typedef U16 A2dpFeatures;

/* Audio Player */ 
#define A2DP_SRC_FEATURE_PLAYER    0x01

/* Microphone */ 
#define A2DP_SRC_FEATURE_MIC       0x02

/* Tuner */ 
#define A2DP_SRC_FEATURE_TUNER     0x04

/* Mixer */ 
#define A2DP_SRC_FEATURE_MIXER     0x08

/* Headphones */ 
#define A2DP_SNK_FEATURE_HEADPHONE 0x01

/* Loudspeaker */ 
#define A2DP_SNK_FEATURE_SPEAKER   0x02

/* Audio Recorder */ 
#define A2DP_SNK_FEATURE_RECORDER  0x04

/* Amplifier */ 
#define A2DP_SNK_FEATURE_AMP       0x08

/* End of A2dpFeatures */ 

/*---------------------------------------------------------------------------
 * A2dpEndpointType type
 *
 *     Describes the type of stream (Source or Sink).  
 */
typedef U8 A2dpEndpointType;

/* The stream is a source */ 
#define A2DP_STREAM_TYPE_SOURCE  0

/* The stream is a sink */ 
#define A2DP_STREAM_TYPE_SINK    1

/* End of A2dpEndpointType */ 

/*---------------------------------------------------------------------------
 * A2dpStreamState type
 *
 *     Describes the state of a stream.  
 */
typedef U8 A2dpStreamState;

/* The stream is closed */ 
#define A2DP_STREAM_STATE_CLOSED     0

/* The stream is idle */ 
#define A2DP_STREAM_STATE_IDLE       1

/* The stream is open */ 
#define A2DP_STREAM_STATE_OPEN       2

/* The stream is streaming */ 
#define A2DP_STREAM_STATE_STREAMING  3

/* Unknown state */ 
#define A2DP_STREAM_STATE_UNKNOWN    0xFF

/* End of A2dpStreamState */ 

/*---------------------------------------------------------------------------
 * A2dpSbcCodecInfElement0 type
 *     
 *     The first byte of an SBC codec information element contains the 
 *     sampling frequency and the channel mode of the stream.  These values 
 *     can all be OR'd together to describe the capabilities of the coded.  
 */
typedef U8 A2dpSbcCodecInfElement0;

#define A2DP_SBC_CODEC_FREQ_16000       0x80
#define A2DP_SBC_CODEC_FREQ_32000       0x40
#define A2DP_SBC_CODEC_FREQ_44100       0x20
#define A2DP_SBC_CODEC_FREQ_48000       0x10
#define A2DP_SBC_CODEC_CHNL_MODE_MONO   0x08
#define A2DP_SBC_CODEC_CHNL_MODE_DUAL   0x04
#define A2DP_SBC_CODEC_CHNL_MODE_STEREO 0x02
#define A2DP_SBC_CODEC_CHNL_MODE_JOINT  0x01

/* End of A2dpSbcCodecInfElement0 */ 

/*---------------------------------------------------------------------------
 * A2dpSbcCodecInfElement1 type
 *
 *     The second byte of an SBC codec information element contains the 
 *     number of blocks, subbands, and the allocation method to use when 
 *     encoding the stream.  These values can all be OR'd together to 
 *     describe the capabilities of the codec.  
 */
typedef U8 A2dpSbcCodecInfElement1;

#define A2DP_SBC_CODEC_BLOCKS_4            0x80
#define A2DP_SBC_CODEC_BLOCKS_8            0x40
#define A2DP_SBC_CODEC_BLOCKS_12           0x20
#define A2DP_SBC_CODEC_BLOCKS_16           0x10
#define A2DP_SBC_CODEC_SUBBANDS_4          0x08
#define A2DP_SBC_CODEC_SUBBANDS_8          0x04
#define A2DP_SBC_CODEC_ALLOCATION_SNR      0x02
#define A2DP_SBC_CODEC_ALLOCATION_LOUDNESS 0x01

/* End of A2dpSbcCodecInfElement1 */ 

/* Forward declaration */ 
typedef struct _A2dpCallbackParms A2dpCallbackParms;
typedef struct _A2dpStream A2dpStream;

/*---------------------------------------------------------------------------
 * A2dpCallback type
 *
 * A function of this type is called to indicate events to the application.  
 */
typedef void (*A2dpCallback)(A2dpStream *Stream, const A2dpCallbackParms *Info);

/* End of A2dpCallback */ 

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * A2dpSbcPacket structure
 *
 * Used for transmitting SBC data.  
 */
typedef struct _A2dpSbcPacket {
    ListEntry node;             /* Used internally by A2DP  */ 

    U8       *data;             /* Pointer to transmit data */ 
    U16       dataLen;          /* Length of transmit data  */ 
    U16       frameSize;        /* Size of an SBC frame     */ 

    /* === Internal use only === */ 
    BtPacket  packet;           /* For sending over L2CAP   */ 
    U16       dataSent;
    U16       frmDataSent;
} A2dpSbcPacket;

/*---------------------------------------------------------------------------
 * A2dpCallbackParms structure
 *
 * Contains information about the application callback event.
 */
struct _A2dpCallbackParms {
    A2dpEvent   event;                  /* The A2DP Event */ 
    U16         len;                    /* Length of the parameters */ 
    BtStatus    status;                 /* Transport status */ 
    A2dpError   error;                  /* The A2DP or AVDTP error code 
                                         * associated with the event (if 
                                         * any).  
                                         */
    BtErrorCode discReason;             /* Link disconnect reason (if any). */
    union {
        AvdtpContentProt   *cp;         /* Content Protection Capability */ 
        AvdtpCodec         *codec;      /* Codec Capability              */ 
        AvdtpCapability    *capability; /* AVDTP Capability              */ 
        AvdtpConfigRequest *configReq;  /* AVDTP Config request          */ 
        BtPacket           *btPacket;   /* Raw Transmit packet           */ 
        A2dpSbcPacket      *sbcPacket;  /* SBC Transmit packet           */ 
        U8                 *data;       /* Receive data                  */ 
        U16                 delayMs;    /* Buffering/processing delay of
                                         * the Sink.
                                         */
        AvdtpStreamInfo    *streamInfo; /* Stream information */ 
        AvDevice           *device;     /* The connecting or disconnecting
                                         * device.
                                         */
    } p;
};

/* Stream information returned by stream discovery */ 
typedef struct _A2dpStreamInfo {
    ListEntry node;                     /* Used internally by A2DP. */ 

    AvdtpStreamInfo info;               /* Stream information */ 
} A2dpStreamInfo;

/*---------------------------------------------------------------------------
 * A2dpStream structure
 *
 * Contains the A2DP stream object.  This structure identifies the stream
 * in callbacks and API calls.
 */
struct _A2dpStream {
    ListEntry           node;           /* Used internally by A2DP. */ 

    A2dpEndpointType    type;           /* Stream endpoint type */ 

    /* === Internal use only === */ 
    A2dpCallback        callback;       /* Application callback */ 

    AvDevice           *device;         /* The device associated with the 
                                         * stream 
                                         */ 

    AvdtpStream         stream;         /* Stream structure used by this 
                                         * source 
                                         */

    ListEntry           sbcQueue;       /* Transmit queue for SBC data */ 

    AvdtpMediaHeader    mediaHeader;    /* Media header for SBC packets */ 

    U16                 timeInc;        /* Time stamp increment */ 

    AvdtpCapability     lastCap;
    AvdtpCallbackParms  lastParms;      /* Last AV error that occurred */ 

    U8                  flags;          /* State flags */ 

    ListEntry           streamInfoList; /* List of stream information used 
                                         * during the discovery process.  
                                         */ 

    A2dpStreamInfo     *currentInfo;    /* Stream discovery info used with 
                                         * with the current operation 
                                         */ 

    EvmTimer            closeTimer;     /* Closes stream when open never 
                                         * completes 
                                         */
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * A2DP_Init()
 *
 *     Initialize the A2DP SDK.  This function should only be called 
 *     once, normally at initialization time.  The calling of this function 
 *     can be specified in overide.h using the XA_LOAD_LIST macro 
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(A2DP) ... ).  
 *
 * Returns:
 *
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL A2DP_Init(void);

/*---------------------------------------------------------------------------
 * A2DP_Register()
 *
 *      Registers an audio source or sink for both originating and receiving 
 *      connections.  An application callback function is provided for 
 *      receiving A2DP events.  This function must be called before any 
 *      other A2DP source functions.  
 *
 * Parameters:
 *     Stream - An initialized A2DP stream structure, the 'type' field must 
 *         be initialized with the appropriate endpoint type (see 
 *         A2dpEndpointType).  
 *
 *     Codec - An initialized AVDTP codec structure (see AvdtpCodec).  All 
 *         fields must be initialized.  
 *
 *     Callback - Identifies the application function that will be called 
 *         with A2DP events.  
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The A2DP stream was successfully registered.  
 *
 *      BT_STATUS_IN_USE - The specified source is already in use.  
 *
 *      BT_STATUS_INVALID_PARM - The 'Stream' or 'Callback' parameter 
 *      does not contain a valid pointer. (XA_ERROR_CHECK only).  
 */
BtStatus A2DP_Register(A2dpStream  *Stream, 
                       AvdtpCodec  *Codec, AvdtpContentProt *Cp,
                       A2dpCallback Callback);

/*---------------------------------------------------------------------------
 * A2DP_Deregister()
 *
 *     Deregisters the audio source or sink.  
 *
 * Parameters:
 *
 *     Stream - A registered A2DP stream structure.  
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - The A2DP stream was successfully deregistered.  
 *
 *     BT_STATUS_IN_USE - The specified stream is already in use.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not contain 
 *         a valid pointer. (XA_ERROR_CHECK only).  
 */
BtStatus A2DP_Deregister(A2dpStream *Stream);

/*---------------------------------------------------------------------------
 * A2DP_AddContentProtection()
 *
 *     Adds content protection capabilities to a registered stream.  Multiple 
 *     capabilities can be added to a single stream.  Once registered, 
 *     the content protection structure's memory is "owned" by A2DP.  This 
 *     memory must not be changed or freed.  Once a stream is deregistered 
 *     the content protection structure memory can be changed or freed.  
 *
 * Parameters:
 *
 *     Stream - A registered A2DP stream.  
 *
 *     Cp - Structure that defines the type of content protection supported 
 *         by the stream.  
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - The content protection was successfully 
 *         registered.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' or 'Cp' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified stream has not been registered.  
 *
 *     BT_STATUS_IN_USE - The specified stream or content protection is in 
 *         use.  
 */
BtStatus A2DP_AddContentProtection(A2dpStream *Stream, AvdtpContentProt *Cp);
#define A2DP_AddContentProtection(s, c) AVDTP_AddContentProtection(&(s)->stream, c)

/*---------------------------------------------------------------------------
 * A2DP_OpenStream()
 *
 *    Searches for and begins to open a stream on the remote device.  This 
 *    function searches for a stream with capabilities matching those that 
 *    were registered using A2DP_Register() and 
 *    A2DP_AddContentProtection().  As streams are discovered, the 
 *    application is notified of matching capability types through the 
 *    A2DP_EVENT_CODEC_INFO and A2DP_EVENT_CP_INFO events.  The application 
 *    can cache or verify the specific elements of these capabilities.  When 
 *    all matching capabilities have been discovered, the application is 
 *    notified through the A2DP_EVENT_GET_CONFIG_IND event.  When this event 
 *    is received, the application must call A2DP_SetStreamConfig().  The 
 *    connection progress will not continue until the stream has been 
 *    configured. Once configured, the application will receive the 
 *    A2DP_STREAM_OPEN event.  When this event has been received with no 
 *    errors, the stream is open.  If no streams with matching capabilities 
 *    can be found, the stream is closed.  
 *
 *    A2DP_AbortStream() can be called to close the stream to abort this 
 *    process if necessary.  
 *
 * Parameters:
 *
 *     Stream - A registered and configured A2DP stream.  
 *
 *     Addr - The Bluetooth address of the device to which the connection 
 *            should be made.  If Addr is 0, then a device inquiry will be 
 *            initiated.  A Device Monitor handler must be registered to 
 *            receive events and to select the desired device.  It is only 
 *            possible to use and value of 0 for Addr if connecting to a 
 *            device that is not already connected.  If the device was 
 *            already connected, an attempt to connect again will fail.  To 
 *            establish a new stream to an already connected device, the 
 *            BD_ADDR of the connected device must be used in the Addr 
 *            parameter.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Open Stream operation was started 
 *         successfully.  Codec and Content Protection events will be 
 *         received if a stream is discovered.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream', or 'Cp' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_IN_USE - The specified stream is already in use.
 * 
 *     BT_STATUS_BUSY - A search is already in progress.
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_OpenStream(A2dpStream *Stream, BT_BD_ADDR *Addr);

/* Used by the following macros */
BtStatus A2dpSetStreamConfig(A2dpStream       *Stream, 
                             AvdtpCodec       *Codec,
                             AvdtpContentProt *Cp,
                             U16               delayMs);

/*---------------------------------------------------------------------------
 * A2DP_SetStreamConfig()
 *
 *     Configures the specified stream.  This function is called in response 
 *     to the A2DP_EVENT_GET_CONFIG_IND event or when the stream has been 
 *     placed into the idle state (see A2DP_IdleStream()).  The desired Codec 
 *     and Content Protection information is selected based on the 
 *     capabilities received in the A2DP_EVENT_CODEC_INFO and 
 *     A2DP_EVENT_CP_INFO events.  If these event have not been received, 
 *     the stream must be opened by caling A2DP_OpenStream().  
 *
 * Parameters:
 *
 *     Stream - A registered A2DP stream.  
 *
 *     Codec - Structure that defines the specific codec type to use for this 
 *         stream, and the specific codec elements to use.  Selected codec 
 *         elements must not be ambiguous.  For example, only one sampling 
 *         frequency should be selected.  
 *
 *     Cp - Structure that defines the type of content protection to 
 *         configure on the remote device.  This structure must have been 
 *         registered on the stream previously by calling 
 *         A2DP_AddContentProtection().  If no content protection is 
 *         desired, this parameter may be set to NULL.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Stream Config operation was started 
 *         successfully.  The stream open process will continue.  Once the 
 *         stream is open, the A2DP_EVENT_STREAM_OPEN event will be 
 *         received.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream', 'Codec', or 'Cp' parameter 
 *         does not contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_FAILED - The specified stream is not in the correct state.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_SetStreamConfig(A2dpStream       *Stream, 
                              AvdtpCodec       *Codec, 
                              AvdtpContentProt *Cp);
#define A2DP_SetStreamConfig(s, c, p) A2dpSetStreamConfig(s, c, p, 0)

/*---------------------------------------------------------------------------
 * A2DP_SetStreamConfigWithSinkDelay()
 *
 *     Configures the specified stream.  This function is called in response 
 *     to the A2DP_EVENT_GET_CONFIG_IND event or when the stream has been 
 *     placed into the idle state (see A2DP_IdleStream()).  The desired Codec 
 *     and Content Protection information is selected based on the 
 *     capabilities received in the A2DP_EVENT_CODEC_INFO and 
 *     A2DP_EVENT_CP_INFO events.  If these event have not been received, 
 *     the stream must be opened by caling A2DP_OpenStream().  A value
 *     representing the delay in the Sink buffering/processing may be
 *     specified.
 *
 * Parameters:
 *
 *     Stream - A registered A2DP stream.  
 *
 *     Codec - Structure that defines the specific codec type to use for this 
 *         stream, and the specific codec elements to use.  Selected codec 
 *         elements must not be ambiguous.  For example, only one sampling 
 *         frequency should be selected.  
 *
 *     Cp - Structure that defines the type of content protection to 
 *         configure on the remote device.  This structure must have been 
 *         registered on the stream previously by calling 
 *         A2DP_AddContentProtection().  If no content protection is 
 *         desired, this parameter may be set to NULL.  
 * 
 *     delayMs - Delay in milliseconds for the Sinks buffering/processing of
 *         audio data.  If the Stream is a Source, this value is ignored.
 *
 * 
 * Returns:
 *
 *     BT_STATUS_PENDING - The Stream Config operation was started 
 *         successfully.  The stream open process will continue.  Once the 
 *         stream is open, the A2DP_EVENT_STREAM_OPEN event will be 
 *         received.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream', 'Codec', or 'Cp' parameter 
 *         does not contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_FAILED - The specified stream is not in the correct state.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_SetStreamConfigWithSinkDelay(A2dpStream       *Stream, 
                                           AvdtpCodec       *Codec, 
                                           AvdtpContentProt *Cp,
                                           U16               delayMs);
#define A2DP_SetStreamConfigWithSinkDelay(s, c, p, f) A2dpSetStreamConfig(s, c, p, f)

/* Used by the following macros */
BtStatus A2dpOpenStreamRsp(A2dpStream         *Stream, 
                           A2dpError           Error, 
                           AvdtpCapabilityType CapType,
                           U16                 delayMs);

/*---------------------------------------------------------------------------
 * A2DP_OpenStreamRsp()
 *
 *    Responds to an request to open a connection with the remote device 
 *    (See A2DP_EVENT_STREAM_OPEN_IND).  The open request is accepted by the 
 *    application if this function is called with A2DP_ERR_NO_ERROR.  Any 
 *    other error code rejects the request.  
 *
 * Parameters:
 *
 *     Stream - A registered and configured A2DP stream.  
 *
 *     Error - If the request to open the connection is granted then 
 *             A2DP_ERR_NO_ERROR is passed in this parameter, otherwise 
 *             another appropriate error code should be used.  
 *
 *     CapType - Capability type associated with the reason for rejecting 
 *               the connection.  If 'Error' is set to A2DP_ERR_NO_ERROR, 
 *               this parameter is ignored.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Open Stream Response operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_OPEN event will be received 
 *         when the stream is open.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not contain a 
 *         valid pointer.  (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NO_CONNECTION - The specified stream did not request a 
 *         connection.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_OpenStreamRsp(A2dpStream         *Stream, 
                            A2dpError           Error, 
                            AvdtpCapabilityType CapType);
#define A2DP_OpenStreamRsp(s, e, c) A2dpOpenStreamRsp(s, e, c, 0)

/*---------------------------------------------------------------------------
 * A2DP_OpenStreamRspWithSinkDelay()
 *
 *    Responds to an request to open a connection with the remote device 
 *    (See A2DP_EVENT_STREAM_OPEN_IND).  The open request is accepted by the 
 *    application if this function is called with A2DP_ERR_NO_ERROR.  Any 
 *    other error code rejects the request.  A value representing the delay
 *    in the Sink buffering/processing may be specified.
 *
 * Parameters:
 *
 *     Stream - A registered and configured A2DP stream.  
 *
 *     Error - If the request to open the connection is granted then 
 *         A2DP_ERR_NO_ERROR is passed in this parameter, otherwise 
 *         another appropriate error code should be used.  
 *
 *     CapType - Capability type associated with the reason for rejecting 
 *         the connection.  If 'Error' is set to A2DP_ERR_NO_ERROR, 
 *         this parameter is ignored.
 * 
 *     delayMs - A value representing the delay in the Sink  
 *         buffering/processing.  If the Stream is a Source, this value is
 *         ignored.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Open Stream Response operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_OPEN event will be received 
 *         when the stream is open.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not contain a 
 *         valid pointer.  (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NO_CONNECTION - The specified stream did not request a 
 *         connection.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_OpenStreamRspWithSinkDelay(A2dpStream         *Stream, 
                                         A2dpError           Error, 
                                         AvdtpCapabilityType CapType,
                                         U16                 delayMs);
#define A2DP_OpenStreamRspWithSinkDelay(s, e, c, f) A2dpOpenStreamRsp(s, e, c, f)


/*---------------------------------------------------------------------------
 * A2DP_SetSinkDelay()
 * 
 *     Set a value representing the delay in the Sink buffering/processing.
 *     This call can be made by a Sink when the delay changes, or by a Source
 *     if the Sink reports a change in the delay.  If a Source calls this
 *     function, it will set the flush timeout and, upon success, return
 *     BT_STATUS_SUCCESS.  If a Sink calls this function, and Delay Reporting
 *     feature is enabled, it will send the delay value to the Source and,
 *     upon success, return BT_STATUS_PENDING.
 * 
 * Parameters:
 * 
 *     Stream - A registered and configured A2DP stream.  
 *
 *     delayMs - A value representing the delay in the Sink
 *         buffering/processing.  If the Stream is a Source, this value is
 *         ignored.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - Flush Timeout was set.  No events will be
 *         received.
 * 
 *     BT_STATUS_PENDING - The delay value was sent to the Source.  When the
 *         Source acnkowldeged the value, the A2DP_EVENT_DELAY_REPORTING_CNF
 *         event will be received (only if Delay Reporting feature is
 *         enabled).
 *
 *     BT_STATUS_FAILED - The stream is not a Sink or is not in the correct
 *         state (STREAMING) for sending this command.
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not contain a 
 *         valid pointer.  (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_IN_USE - The specified streams is already in use.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     BT_STATUS_NO_CONNECTION - The stream is not allocated on a channel.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_SetSinkDelay(A2dpStream *Stream, U16 delayMs);


/*---------------------------------------------------------------------------
 * A2DP_GetStreamCapabilities()
 *
 *     Query the remote stream for its capabilities.  
 *
 * Parameters:
 *
 *     Stream - A registered and open A2DP stream.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Get Capabilities operation was started 
 *         successfully.  An A2DP_EVENT_CODEC_INFO event will be received.  
 *         If the remote device has any content protection registered, then 
 *         a A2DP_EVENT_CP_INFO event will be received for each registered 
 *         capability.  When all capabilities have been notified, an 
 *         A2DP_EVENT_GET_CAP_CNF event will be received.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter is not valid. 
 *         (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified stream is not registered.  
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists on this stream.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_GetStreamCapabilities(A2dpStream *Stream);

/* Used by macros below */ 
BtStatus A2dpCloseStream(A2dpStream *Stream, BOOL discFlag);

/*---------------------------------------------------------------------------
 * A2DP_CloseStream()
 *
 *     Initiate the closing an open stream.  
 *
 * Parameters:
 *
 *     Stream - A registered and open A2DP stream.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Close Stream operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_CLOSED event will be received 
 *         when the stream is closed.  
 *
 *     BT_STATUS_NO_CONNECTION - The stream is not in the open or active 
 *         state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_CloseStream(A2dpStream *Stream);
#define A2DP_CloseStream(s) A2dpCloseStream((s), TRUE)

/*---------------------------------------------------------------------------
 * A2DP_IdleStream()
 *
 *     Put the stream into the Idle state.  This differs from closing the 
 *     stream, because the lower layers (L2CAP and ACL) stay connected.  When 
 *     in this state, it is necessary to configure the stream again before 
 *     starting the Stream (see A2DP_SetStreamConfig()).  
 *
 * Parameters:
 *
 *     Stream - A registered and open A2DP stream.
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Idle Stream operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_IDLE event will be received 
 *         when the stream is closed.  
 *
 *     BT_STATUS_FAILED - The stream is not in the open or active state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_IdleStream(A2dpStream *Stream);
#define A2DP_IdleStream(s) A2dpCloseStream((s), FALSE)

/*---------------------------------------------------------------------------
 * A2DP_StartStream()
 *
 *     Initiates streaming on the open stream.  Calling this function puts 
 *     the stream into an active state, which allows media packets to be sent 
 *     on the stream.  
 *
 * Parameters:
 *
 *     Stream - A registered and open A2DP stream.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The operation was initiated successfully.  
 *         An A2DP_EVENT_STREAM_STARTED event will be received when the 
 *         stream is ready for streaming.
 *
 *     BT_STATUS_FAILED - The stream is not in the open state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_IN_USE - One of the specified streams is already in use.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_StartStream(A2dpStream *Stream);
#define A2DP_StartStream(s) AVDTP_StartStream(&(s)->stream)

/*---------------------------------------------------------------------------
 * A2DP_StartStreamRsp()
 *
 *     Called in response to an A2DP_EVENT_STREAM_START_IND event.  Calling 
 *     this function will either accept the streaming request or reject it.  
 *
 * Parameters:
 *
 *     Stream - A registered and open A2DP stream.  
 *
 *     Error - If the streaming request is accepted, this parameter must be 
 *         set to A2DP_ERR_NO_ERROR.  If the streaming request is rejected, 
 *         this parameter must be set to the appropriate error defined by 
 *         A2dpError.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Start Streams Response operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_STARTED event will be received 
 *         when the stream has been started.  If the start stream request was 
 *         rejected, the A2DP_EVENT_STREAM_SUSPENDED event will be received.  
 *
 *     BT_STATUS_FAILED - The stream is not in the open state.  
 *
 *     BT_STATUS_INVALID_PARM - The Stream parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_IN_USE - One of the specified streams is already in use.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_StartStreamRsp(A2dpStream *Stream, A2dpError Error);
#define A2DP_StartStreamRsp(s, e) AVDTP_StartStreamRsp(&(s)->stream, e)

/*---------------------------------------------------------------------------
 * A2DP_SuspendStream()
 *
 *     Suspends a stream that is currently active.  
 *
 * Parameters:
 *
 *     Stream - A registered and active A2DP stream.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Suspend Streams Response operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_SUSPENDED event will be 
 *         received when the stream has been suspended.  
 *
 *     BT_STATUS_FAILED - The stream is not in an active state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_SuspendStream(A2dpStream *Stream);
#define A2DP_SuspendStream(s) AVDTP_SuspendStream(&(s)->stream)

/*---------------------------------------------------------------------------
 * A2DP_ReconfigStream()
 *
 *     Reconfigure an open stream.  This call can be made only if the stream 
 *     is in the open state.  In other words, a stream that is currently 
 *     streaming audio/video must be suspended before trying to reconfigure 
 *     it.  
 *
 * Parameters:
 *
 *     Stream - A registered and open A2DP stream.  
 *
 *     Codec - Structure that defines the specific codec type to use for this 
 *         stream, and the specific codec elements to use.  Selected codec 
 *         elements must not be ambiguous.  For example, only one sampling 
 *         frequency should be selected.  
 *
 *     Cp - Structure that defines the type of content protection to 
 *         configure on the remote device.  This structure must have been 
 *         registered on the stream previously by calling 
 *         A2DP_AddContentProtection().  If no content protection is 
 *         desired, this parameter may be set to NULL.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Reconfig operation was started successfully.  
 *         An A2DP_EVENT_STREAM_RECONFIG event will be received when the 
 *         stream is reconfigured.  
 *
 *     BT_STATUS_FAILED - The stream is not in the open state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' or 'Cp' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_ReconfigStream(A2dpStream       *Stream, 
                             AvdtpCodec       *Codec,
                             AvdtpContentProt *Cp);
#define A2DP_ReconfigStream(s, c, cp) AVDTP_Reconfig(&(s)->stream, c, cp)

/*---------------------------------------------------------------------------
 * A2DP_ReconfigStreamRsp()
 *
 *     Respond to a request to reconfigure an open stream (See 
 *     A2DP_EVENT_STREAM_RECONFIG_IND).  This call can be made only if the 
 *     stream is in the open state.  
 *
 * Parameters:
 *
 *     Stream - A registered and open A2DP stream.  
 *
 *     Error - If the request to reconfigure the stream is granted then 
 *             A2DP_ERR_NO_ERROR is passed in this parameter, otherwise 
 *             another appropriate error code should be used.  
 *
 *     Type - Capability type that caused an error (if any).  This value 
 *            is ignored if 'Error' is set to A2DP_ERR_NO_ERROR.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The operation was started successfully.  
 *         An A2DP_EVENT_STREAM_RECONFIG_CNF event will be received when the 
 *         stream is reconfigured.  
 *
 *     BT_STATUS_FAILED - The stream is not in the open state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_ReconfigStreamRsp(A2dpStream         *Stream, 
                                A2dpError           Error, 
                                AvdtpCapabilityType Type);
#define A2DP_ReconfigStreamRsp(s, e, t) AVDTP_ReconfigRsp(&(s)->stream, e, t)

/*---------------------------------------------------------------------------
 * A2DP_SecurityControlReq()
 *
 *     Initiates a security procedure (content protection).  The security 
 *     procedure data is application specific and is not interpreted in any 
 *     way by A2DP.  
 *
 * Parameters:
 *
 *     Stream - A registered A2DP stream.  
 *
 *     Data - Application specific data.  
 *
 *     Len - Length of the security data.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Security Request operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_SECURITY_CNF event will be 
 *         received when the security data has been sent.  
 *
 *     BT_STATUS_FAILED - The stream is not in the correct state or content 
 *         protection is not configured on the stream.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' or 'Data' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_SUPPORTED - The current stream is not configured for 
 *         content protection.  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_SecurityControlReq(A2dpStream *Stream, U8 *Data, U16 Len);
#define A2DP_SecurityControlReq(s, d, l) AVDTP_SecurityControlReq(&(s)->stream, d, l)

/*---------------------------------------------------------------------------
 * A2DP_SecurityControlRsp()
 *
 *     Called in response to an A2DP_EVENT_STREAM_SECURITY_IND event.  The 
 *     security procedure data is application specific and is not interpreted 
 *     in any way by A2DP.  
 *
 * Parameters:
 *
 *     Stream - A registered A2DP stream.  
 *
 *     Data - Application specific data.  
 *
 *     Len - Length of the security data.  
 *
 *     Error - If the security request is accepted, this parameter must be 
 *         set to AVDTP_ERR_NO_ERROR.  If the security request is rejected, 
 *         this parameter must be set to the appropriate error defined by 
 *         AvdtpError.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Security Request operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_SECURITY_CNF event will be 
 *         received when the security data has been sent.  This event will 
 *         be received even if the security request was rejected, however, 
 *         the 'error' field of the callback parameters will be set to the 
 *         value sent in the 'Error' parameter.  
 *
 *     BT_STATUS_FAILED - The stream is not in the correct state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' or 'Data' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_SecurityControlRsp(A2dpStream *Stream, 
                                 U8         *Data, 
                                 U16         Len, 
                                 AvdtpError  Error);
#define A2DP_SecurityControlRsp(s, d, l, e) AVDTP_SecurityControlRsp(&(s)->stream, d, l, e)

/*---------------------------------------------------------------------------
 * A2DP_AbortStream()
 *
 *     Aborts any open or active stream.  Once aborted, an open stream will 
 *     be in a closed state.  
 *
 * Parameters:
 *
 *     Stream - An open, or active A2DP stream.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Abort Stream operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_ABORTED event will be received 
 *         when the stream has been aborted.  
 *
 *     BT_STATUS_FAILED - The stream is not in the correct state.  
 *
 *     BT_STATUS_INVALID_PARM - The Stream parameter does not contain a 
 *         valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_AbortStream(A2dpStream *Stream);
#define A2DP_AbortStream(s) AVDTP_AbortStream(&(s)->stream)

/*---------------------------------------------------------------------------
 * A2DP_StreamSendRawPacket()
 *
 *     Sends data on the specified stream.  The stream must be open and in 
 *     an streaming state.  No checking is done on the validity of 
 *     the data format.  Data is simply delivered to the stream raw.  It is 
 *     assumed that the data is properly formatted using the RTP media 
 *     packet format described in RFC1889 (see A2DP_CreateMediaHeader).  It 
 *     is further assumed that the payload of the RTP media packet is in the 
 *     proper format for the configured stream.  
 *
 * Parameters:
 *
 *     Stream - A registered and active A2DP stream.  
 *
 *     Packet - An initialized BtPacket structure.  The Packet->data, 
 *         Packet->dataLen, Packet->header, Packet->headerLen, and 
 *         Packet->flags fields must be properly initialized (see 
 *         BtPacket).  The only valid flag that can be used is 
 *         BTP_FLAG_INUSE.  The data must be formatted as an RTP media 
 *         packet defined by RFC1889.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Write operation was started 
 *         successfully.  An A2DP_EVENT_STREAM_PACKET_SENT event will be 
 *         received when the data has been sent.  
 *
 *     BT_STATUS_FAILED - The stream is not in the active state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream' or 'Packet' parameter does not 
 *         contain a valid pointer. (XA_ERROR_CHECK only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_StreamSendRawPacket(A2dpStream *Stream, BtPacket *Packet);

/*---------------------------------------------------------------------------
 * A2DP_StreamSendSbcPacket()
 *
 *     Sends SBC data on the specified stream.  The stream must be open and 
 *     in an streaming state.  No checking is done on the validity of 
 *     the data format.  Data is delivered to the stream with the media 
 *     packet header and SBC header.  
 *
 * Parameters:
 *
 *     Stream - A registered and active A2DP stream.  
 *
 *     Packet - An initialized SbcPacket structure with valid SBC data.  The 
 *         data, datalen, and FrameSize fields must be initialized.  All 
 *         frames in the buffer to be sent must be the same size, which 
 *         means that the Bitpool value for each frame in the SBC data to be 
 *         transmitted must be the same.  The bitpool value can changed in 
 *         subsequent calls to this function.  
 *
 *     StreamInfo - Information about the SBC stream.  
 *
 * Returns:
 *
 *     BT_STATUS_PENDING - The Write operation was started successfully.  
 *         An A2DP_EVENT_STREAM_SBC_PACKET_SENT event will be received when 
 *         the data has been sent.  
 *
 *     BT_STATUS_FAILED - The stream is not in the active state.  
 *
 *     BT_STATUS_INVALID_PARM - The 'Stream'' 'Packet,' or 'StreamInfo' 
 *         parameter does not contain a valid pointer.  (XA_ERROR_CHECK 
 *         only).  
 *
 *     BT_STATUS_NOT_FOUND - The specified channel or stream is not 
 *         registered.  
 *
 *     Other - It is possible to receive other error codes, depending on the 
 *         lower layer service in use (L2CAP or Management Entity).  
 */
BtStatus A2DP_StreamSendSbcPacket(A2dpStream    *Stream, 
                                  A2dpSbcPacket *Packet,
                                  SbcStreamInfo *StreamInfo);

/*---------------------------------------------------------------------------
 * A2DP_StreamIncrementTimeStamp()
 *
 *     Increments the timestamp for the SBC stream.  This function should be 
 *     called whenever sampled data cannot be sent and will be discarded 
 *     (due to a lack of resources, for example).  Each SBC packet has a 
 *     timestamp associated with it when it is sent, so when PCM data cannot 
 *     be sent, the timestamp needs to be adjusted to account for the lost 
 *     audio data.  The amount of the adjustment is obtained from the number 
 *     of samples in the PCM data block.  
 *
 * Parameters:
 *
 *     Stream - A registered and active A2DP stream.  
 *
 *     Packet - An initialized SbcPacket structure with valid SBC data.  The 
 *         data, datalen, and FrameSize fields must be initialized.  
 *
 *     StreamInfo - Information about the SBC stream.  
 *
 * Returns: void
 *
 */
void A2DP_StreamIncrementTimeStamp(A2dpStream *Stream, SbcPcmData *Data);
#define A2DP_StreamIncrementTimeStamp(s,d) ((s)->mediaHeader.timestamp += \
                                            (d)->numChannels ? \
                                            (d)->dataLen / 2 / (d)->numChannels : \
                                            0)


/*---------------------------------------------------------------------------
 * A2DP_MediaPacketSize()
 *
 *     Returns the negotiated L2CAP packet size for sending media packets.  
 *
 * Parameters:
 *
 *     Stream - A registered and active A2DP stream.  
 *
 * Returns:
 *
 *     The maximum size for media packets.  
 */
U16 A2DP_MediaPacketSize(A2dpStream *Stream);
#define A2DP_MediaPacketSize(s) (AVDTP_MediaPacketSize(&(s)->stream))

/*---------------------------------------------------------------------------
 * A2DP_CreateMediaHeader()
 *
 *     Used to create a media packet header (RFC1889) before transmitting 
 *     media data using the A2DP_StreamSendRawPacket().  A media packet 
 *     header is typically 12 bytes, but can be up to 72 bytes.  Passing in 
 *     a NULL buffer will simply calculate the media header size.  
 *
 * Parameters:
 *
 *     Header - A media packet header structure initialized with the 
 *         appropriate values.  The media header values are specified by 
 *         the profile.  
 *
 *     Buffer - A buffer in which the media packet header is assembled.  If 
 *         NULL, the header is not created, but the size is calculated.  
 *
 * Returns:
 *
 *     The actual size of the media header.  
 */
U16 A2DP_CreateMediaHeader(AvdtpMediaHeader *Header, U8 *Buffer);
#define A2DP_CreateMediaHeader(h, b) AVDTP_CreateMediaHeader(h, b)

/*---------------------------------------------------------------------------
 * A2DP_ParseMediaHeader()
 *
 *     Used to parse a media packet header (RFC1889) when a media packet has 
 *     been received from the remote device.  
 *
 * Parameters:
 *
 *     Buffer - A buffer received during the A2DP_EVENT_STREAM_DATA_IND 
 *         event.  
 *
 *     Header - A media packet header structure to receive the header 
 *         values.  
 *
 * Returns:
 *
 *     The actual size of the media header.  
 */
U16 A2DP_ParseMediaHeader(AvdtpMediaHeader *Header, U8 *Buffer);
#define A2DP_ParseMediaHeader(h, b) AVDTP_ParseMediaHeader(h, b)

/*---------------------------------------------------------------------------
 * A2DP_GetStreamState()
 *
 *     Returns the current stream state.  
 *
 * Parameters:
 *
 *     Stream - An open, or active A2DP stream.  
 *
 * Returns:
 *
 *     The current stream state (see A2dpStreamState).  
 */
A2dpStreamState A2DP_GetStreamState(A2dpStream *Stream);

/*---------------------------------------------------------------------------
 * A2DP_GetConfiguredCodec()
 *
 *     Returns the current configured codec.  
 *
 * Parameters:
 *
 *     Stream - An open, or active A2DP stream.  
 *
 * Returns:
 *
 *     The current stream codec (see AvtdpCodec).  
 */
AvdtpCodec *A2DP_GetConfiguredCodec(A2dpStream *Stream);

/*---------------------------------------------------------------------------
 * A2DP_GetRegisteredCodec()
 *
 *     Returns the registered codec.  
 *
 * Parameters:
 *
 *     Stream - An registered stream.  
 *
 * Returns:
 *
 *     The registered stream codec (see AvtdpCodec).  
 */
AvdtpCodec *A2DP_GetRegisteredCodec(A2dpStream *Stream);

/*---------------------------------------------------------------------------
 * A2DP_GetConfiguredCP()
 *
 *     Returns the configured content protection structure of an open 
 *     stream.  If no content protection is configured, this function 
 *     returns 0.  
 *
 * Parameters:
 *
 *     Stream - An open, or active A2DP stream.  
 *
 * Returns:
 *
 *     The configured content protection of the current stream, or 0 if 
 *         no content protection is configured.  
 */
AvdtpContentProt *A2DP_GetConfiguredCP(A2dpStream *Stream);

/*---------------------------------------------------------------------------
 * A2DP_GetRemoteDevice()
 *
 *     Returns the remote device of that is associated with the specified 
 *     stream.  
 *
 * Parameters:
 *
 *     Stream - An open, or active A2DP stream.  
 *
 * Returns:
 *
 *     A pointer to the BtRemoteDevice structure.  
 */
BtRemoteDevice *A2DP_GetRemoteDevice(A2dpStream *Stream);

/*---------------------------------------------------------------------------
 * A2DP_GetRegisteredCP()
 *
 *     Returns the registered content protection structures.  The first call 
 *     should be made with the value of 'Cp' set to 0.  This will retrieve 
 *     the first registered structure.  Subsequent calls should be made with 
 *     a pointer to the last structure returned by this function in the 'Cp' 
 *     parameter.  In this case, the next registered structure will be 
 *     returned.  If there are no more structures, then 0 will be returned.  
 *
 * Parameters:
 *
 *     Stream - An open, or active A2DP stream.  
 *
 *     Cp - Should be set to 0, or to a pointer to the last structure 
 *         received.  
 *
 * Returns:
 *
 *     The next registered content protection structure.  
 */
AvdtpContentProt *A2DP_GetRegisteredCP(A2dpStream       *Stream,
                                       AvdtpContentProt *Cp);


/*---------------------------------------------------------------------------
 * A2DP_SetMasterRole()
 *
 *     Sets the stream's preference for being the piconet master.
 *
 * Parameters:
 *
 *     Stream - An open, or active A2DP stream.  
 *
 *     Flag - TRUE indicates that the current stream's connection wishes to 
 *         remain as master. FALSE indicates that the stream doesn't care.
 *
 * Returns:
 *
 *     The next registered content protection structure.  
 */
BtStatus A2DP_SetMasterRole(A2dpStream *Stream, BOOL Flag);
#define A2DP_SetMasterRole(s, f) \
    CMGR_SetMasterRole(&((s)->device->cmgrHandler), f)

/****************************************************************************
 *
 * Section: A2DP Macros 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * A2DP_SBC_CODEC_INF_ELEMENTS()
 *
 *     Macro that forms the SBC information elements.  The information 
 *     elements define the parameters that the SBC codec can use to 
 *     encode or decode the stream.  This macro can be used to create the 
 *     SBC elements for the AvdtpCodec structure.  This macro should be used 
 *     in the initialization of a statically declared byte array.  For a 
 *     definition of acceptable values, see the description of 
 *     A2dpSbcCodecInfElement0 and A2dpSbcCodecInfElement1.  Certain 
 *     features are required for SRC and SNK roles.  These are defined 
 *     below.  
 *
 *     SRC Role requirements:
 *         Sampling frequencies - Must support 48000 or 44100.  
 *         Channel mode - Must support MONO and one other mode.  
 *         Block length - Must support all block lengths.  
 *         Subbands - Must support 8 subbands.  
 *         Allocation Method - must support LOUDNESS.  
 *
 *     SNK Role requirements:
 *         Sampling frequencies - Must support 48000 and 44100.  
 *         Channel mode - Must support all modes.  
 *         Block length - Must support all block lengths.  
 *         Subbands - Must support all subbands.  
 *         Allocation Method - Must support all methods.  
 *
 * Parameters:
 *     f - Sampling frequency of the encoded stream.  
 *     c - Channel mode of the encoded stream.  
 *     b - Block length of the encoded stream.  
 *     s - Number of subbands in the encoded stream.  
 *     m - Allocation method used to encode the stream.  
 *     a - Maximum bitpool value for the stream.  
 *     i - Minimum bitpool value for the stream.  
 */
#define A2DP_SBC_CODEC_INF_ELEMENT(f, c, b, s, m, a, i)  \
                                  (U8)((f) | (c)),       \
                                  (U8)((b) | (s) | (m)), \
                                  (U8)(a),               \
                                  (U8)(i)

#endif /* __A2DP_H_ */ 


