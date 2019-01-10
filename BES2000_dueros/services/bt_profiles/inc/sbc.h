/****************************************************************************
 *
 * File:
 *     $Id: sbc.h 2259 2009-06-17 16:24:55Z dliechty $
 *     $Product: BES AV Profiles SDK v2.x $
 *     $Revision: 2259 $
 *
 * Description: This file contains header definitions for the SBC codec for 
 *     the A2DP profile.  
 *
 * Created:     August 6, 2004
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

#ifndef _SBC_H_
#define _SBC_H_

#include "utils.h"
#include "osapi.h"
#include "xastatus.h"
#include "xatypes.h"
#include "sbc_math.h"

/*---------------------------------------------------------------------------
 * SBC API layer
 *
 *     The Low Complexity Subband Coding (SBC) CODEC is a required component 
 *     of the Bluetooth A2DP profile.  This codec is defined by the Bluetooth 
 *     SIG and must be included with any A2DP source or sink component.  The 
 *     BES Blue SDK SBC CODEC consists of a library for encoding and 
 *     decoding SBC audio steams.  It has been written to the specification 
 *     requirements containted in the A2DP profile specification, and has 
 *     been tested using reference SBC data streams available from the 
 *     Bluetooth SIG.  
 *
 *     SBC requires operations on real numbers when performing the synthesis 
 *     or analysis phases.  The codec is designed to use either 
 *     floating-point or fixed-point integer based math.  Since many 
 *     processor do not provide floating point operations the default is to 
 *     use fixed-point math.  All math functions are implemented in 
 *     sbc_math.c and sbc_math.h.  Please see sbc_math.h for a description of 
 *     the math functions and how to best optimize them for your 
 *     environment.  
 *
 *     The codec generates 16-bit PCM data when decoding and 
 *     SBC stream, and requires 16-bit PCM data as input when encoding and 
 *     SBC stream.  The PCM data can have 1 or two channels with samples for 
 *     the left channel in the 1st 2 bytes, followed by the and the right 
 *     channel sample in the next 2 bytes.  Samples can be big-endian or 
 *     little-endian as defined by the SBC_PCM_BIG_ENDIAN constant.  
 *
 *     The codec has been optimized for speed and is largely 
 *     table driven to prevent any unnecessary multiplication or division 
 *     operations.  In addition, some looping functions have been expanded 
 *     out to inline code to prevent even more unnecessary math.  This 
 *     approach increases the ROM requirements some, however, it helps 
 *     lower-powered CPUs handle the real time requirements of the audio 
 *     encoding/decoding.
 * 
 *     The performance requirements of the codec, compiled with
 *     high-precision math has been calculated to be between 15 and 20 MIPS.
 *     Peformance is highly dependent on the compiler, the memory
 *     arhitecture, and the memory speed.  For the best performance, all
 *     tables and data structures should be placed in the fastest memory
 *     available on the device.  The performance requirements of the codec,
 *     if compiled with low-precision math has been calculated to be as low
 *     as 11 MIPS.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SBC_DECODER constant
 *     Determines whether code for decoding an SBC stream is included.
 */
#ifndef SBC_DECODER
#define SBC_DECODER XA_ENABLED
#endif
              
/*---------------------------------------------------------------------------
 * SBC_ENCODER constant
 *     Determines whether code for encoding an SBC stream is included.
 */
#ifndef SBC_ENCODER
#define SBC_ENCODER XA_ENABLED
#endif

#if SBC_ENCODER == XA_DISABLED && SBC_DECODER == XA_DISABLED
#error Either SBC_ENCODER or SBC_DECODER must be enabled
#endif

/*---------------------------------------------------------------------------
 * SBC_PCM_BIG_ENDIAN constant
 *     Determines whether code for encoding/decoding 16 bit PCM samples 
 *     assumes little-endian or big-endian values.  If defined as XA_ENABLED, 
 *     PCM samples are assumed to be little-endian (LSB first).  If defined 
 *     as XA_DISABLED, samples are assumed to be big-endian (MSB first).  
 */
#ifndef SBC_PCM_BIG_ENDIAN
#define SBC_PCM_BIG_ENDIAN XA_DISABLED
#endif

#if SBC_PCM_BIG_ENDIAN == XA_DISABLED

#define PCMtoHost16 LEtoHost16
#define StorePCM16 StoreLE16

#else /* SBC_PCM_BIG_ENDIAN == XA_ENABLED */ 

#define PCMtoHost16 BEtoHost16
#define StorePCM16 StoreBE16

#endif /* else SBC_PCM_BIG_ENDIAN == XA_ENABLED */ 

/* Maximum size of PCM data returned in a single parsed frame */ 
#define SBC_MAX_PCM_DATA  512

/* Sync word used at the beginning of an SBC frame */ 
#define SBC_SYNC_WORD 0x9C

/* Sync word used at the beginning of an mSBC frame */
#define MSBC_SYNC_WORD	0xAD

#define MSBC_BLOCKS	15

/* Maximum SBC sizes */ 
#define SBC_MAX_NUM_BLK     16
#define SBC_MAX_NUM_SB       8
#define SBC_MAX_NUM_CHNL     2

/* Parser States */ 
#define SBC_PARSE_SYNC           0
#define SBC_PARSE_HEADER         1
#define SBC_PARSE_SCALE_FACTORS  2
#define SBC_PARSE_SAMPLES        3

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SbcSampleFreq type
 *
 *     Describes the sampling frequence of the SBC stream.  
 */
typedef U8 SbcSampleFreq;

#define SBC_CHNL_SAMPLE_FREQ_16    0
#define SBC_CHNL_SAMPLE_FREQ_32    1
#define SBC_CHNL_SAMPLE_FREQ_44_1  2
#define SBC_CHNL_SAMPLE_FREQ_48    3

/* End of SbcSampleFreq */

/*---------------------------------------------------------------------------
 * SbcChannelMode type
 *
 *     Describes the channel mode used to encode a stream.  
 */
typedef U8 SbcChannelMode;

#define SBC_CHNL_MODE_MONO          0
#define SBC_CHNL_MODE_DUAL_CHNL     1
#define SBC_CHNL_MODE_STEREO        2
#define SBC_CHNL_MODE_JOINT_STEREO  3

/* End of SbcChannelMode */

/*---------------------------------------------------------------------------
 * SbcAllocMethod type
 *
 *     Describes allocation method used to encode a stream.  
 */
typedef U8 SbcAllocMethod;

#define SBC_ALLOC_METHOD_LOUDNESS   0
#define SBC_ALLOC_METHOD_SNR        1

/* End of SbcAllocMethod */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SbcStreamInfo structure
 *
 * Contains information about the stream.  
 */
typedef struct _SbcStreamInfo {

    /* Bitpool affects the bitrate of the stream according to the following 
     * formula: bit_rate = 8 * frameLength * sampleFreq / numSubBands / 
     * numBlocks.  The frameLength value can be determined by setting the 
     * bitPool value and calling the SBC_FrameLen() function.  
     * Bitpool can be changed dynamically from frame to frame during 
     * encode/decode without suspending the stream.  
     */
    U8   bitPool;

    /* Sampling frequency of the stream */ 
    SbcSampleFreq sampleFreq;

    /* The channel (Mono, Stereo, etc.) */ 
    SbcChannelMode channelMode;

    /* The allocation method for the stream */ 
    SbcAllocMethod allocMethod;

    /* Number of blocks used to encode the stream (4, 8, 12, or 16) */ 
    U8   numBlocks;

    /* The number of subbands in the stream (4 or 8) */ 
    U8   numSubBands;

    /* The number of channels in the stream (calculated from channelMode) */ 
    U8   numChannels;

	/* The flag of msbc */
	U8   mSbcFlag;

    /* === Internal use only === */ 
    U16  bitOffset;
    U8   crc;
    U8   fcs;
    U8   join[SBC_MAX_NUM_SB];
    U8   scale_factors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    S32  scaleFactors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    U16  levels[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    S8   bitNeed0[SBC_MAX_NUM_SB];
    S8   bitNeed1[SBC_MAX_NUM_SB];
    U8   bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    REAL sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    U8   fcs_bak;
} SbcStreamInfo;


/*---------------------------------------------------------------------------
 * SbcEncoder structure
 *
 * Contains information about the SBC Encoder.  
 */

#if SBC_ENCODER == XA_ENABLED

typedef struct _SbcEncoder {

    /* Encoded stream info */ 
    SbcStreamInfo streamInfo;

    /* === Internal use only === */ 
    U8   sFactorsJoint[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
    REAL sbJoint[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL];
    S16  X0[80 * 2];
    S16  X1[80 * 2];
    U16  X0pos, X1pos;
    REAL Y[16];
} SbcEncoder;

#endif /* SBC_ENCODER == XA_ENABLED */ 

#if SBC_DECODER == XA_ENABLED

/*---------------------------------------------------------------------------
 * SbcDecoder structure
 *
 * Contains information about the stream for the SBC Decoder.
 */
typedef struct _SbcDecoder {

    /* Decoded stream info */ 
    SbcStreamInfo streamInfo;

    /* === Internal use only === */ 
    U16  maxPcmLen;
    REAL V0[160];
    REAL V1[160];
    
    /* Parser state information */ 
    struct {
        U8    stageBuff[SBC_MAX_PCM_DATA]; /* Staging buffer            */ 
        U16   stageLen;                    /* Length of staged data     */ 
        U16   curStageOff;                 /* Offset into staged data   */ 
        U8    *rxBuff;                     /* The Received buffer       */ 
        U16   rxSize;                      /* Remaining rx buff size    */ 
        U8    rxState;                     /* Parser state              */ 
    } parser;

} SbcDecoder;

#endif /* SBC_DECODER == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 * SbcPcmData structure
 *
 * Contains PCM data to be decoded from or to encode into the SBC stream, 
 * including information about the format of the data.  
 */
typedef struct _SbcPcmData {

    /* Sampling frequency of the stream */ 
    SbcSampleFreq  sampleFreq;

    /* The number of channels in the stream  */ 
    U8             numChannels;

    /* The length of decode PCM audio data (in bytes) */ 
    U16            dataLen;

    /* Decoded 16 bit PCM data, memory 
     * must be allocated by the application 
     */ 
    U8            *data;

} SbcPcmData;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SBC_FrameLen()
 *
 *     Returns the maximum size of an SBC frame, derived from the fields in 
 *     an initialized StreamInfo structure.  The StreamInfo structure is 
 *     initialized manually before calling SBC_EncodeFrames(), and is 
 *     initialized automatically after calling SBC_DecodeFrames().  
 *
 * Parameters:
 *
 *     Frame - An initialized SBC frame structure.  See SBC_EncodeFrame for 
 *         a description on initializing the frame structure.  
 *
 * Returns:
 *
 *     The maximum SBC frame size.
 */
U16 SBC_FrameLen(SbcStreamInfo *StreamInfo);

#if SBC_DECODER == XA_ENABLED

/*---------------------------------------------------------------------------
 * SBC_InitDecoder()
 *
 *     Initializes a decoder structure for processing a stream.  Must be 
 *     called before each new stream is decoded.  The same initialized 
 *     decoder structure must be used to decode the entire stream (passed to 
 *     SBC_DecodeFrame).  If the stream parameters change (sampling 
 *     frequency, number of subbands, etc.), the decoder structure must be 
 *     reinitialized.  
 *
 * Parameters:
 *
 *     Decoder - An SBC decoder structure.  
 *
 * Returns:
 *
 *     void
 */
void SBC_InitDecoder(SbcDecoder *Decoder);

/*---------------------------------------------------------------------------
 * SBC_DecodeFrames()
 *
 *    Decodes SBC frames from the data stream.  Data of any arbitrary 
 *    length up to 65535 bytes is accepted.  The PcmData parameter contains 
 *    the decoded 16-bit PCM data, as well as other information about the 
 *    format of the PCM data.  When PcmData->data has been filled as much as 
 *    possible with PCM data, this function will return XA_STATUS_SUCCESS.  
 *    Below are the fields of the PcmData structure:  
 *
 *        data - The decoded PCM data.  
 *
 *        len - The length (in bytes) of the PCM data.  
 *
 *        sampleFreq - The sampling frequence of the PCM data.  
 *
 *        numChannels - The number of channels (1 or 2) in the PCM data.  
 *
 *    If only a partial frame can be decoded or if the PcmData structure has 
 *    not been filled, XA_STATUS_CONTINUE is returned, and SBC_DecodeFrames 
 *    must be called again with new data from the stream.  BytesParsed 
 *    returns the amount of data parsed from the stream.  If there is no more 
 *    stream data to parse, the partially filled PcmData structure can be 
 *    used to complete playing the stream.  
 *
 *    If an error occurs during the decoding process, XA_STATUS_FAILED is 
 *    returned.  BytesParsed returns the amount of data parsed before the 
 *    error occured.  Whenever an error occurs, the PCM data in PcmData->data 
 *    is muted and the value of PcmData->len is set to the value of the 
 *    previously decoded PCM data block.  
 *
 * Parameters:
 *
 *     Decoder - An initialized SBC decoder structure.  No checking is done 
 *         to determine if the decoder has been initialized, therefore it is 
 *         up to the user to ensure that SBC_InitDecider has been called 
 *         prior to using this structure.  This same structure must be used 
 *         for each call while processing the same stream.  
 *
 *     Buff - A pointer to a buffer from the SBC audio stream.  Each call to 
 *         this function must provide new stream data.  
 *
 *     Len - The length of the data pointed to by Buff.  
 *
 *     BytesDecoded - Returns the number of bytes decoded from Buff.  
 *
 *     PcmData - A structure which receives the decoded PCM data.  If this 
 *         value is set to 0, then only the SBC header is parsed, no SBC data 
 *         is decoded.  The value of PcmData->len must be initialized to 0 
 *         before each new PCM data block is used (i.e. - after 
 *         SBC_DecodeFrame returns XA_STATUS_SUCCESS).  PCM data is in 16-bit 
 *         format with left channel sample first, followed by the right 
 *         channel sample (if more than 1 channel).  Byte order is determined 
 *         by the SBC_PCM_BIG_ENDIAN constant.  
 *
 *     MaxPcmData - The maximum amount of PCM data (in bytes) to place into 
 *         the PcmData structure.  The actual amount of PCM data decoded from 
 *         the stream will be placed in PcmData->len.  
 *
 * Returns:
 *
 *     XA_STATUS_SUCCESS - The PCM data buffer is filled as much as possible 
 *         with decoded PCM samples.  The SbcPcmData structure contains the 
 *         decoded samples in 16 bit PCM format.  The SbcDecoder structure 
 *         contains other important information about the stream.  
 *
 *     XA_STATUS_FAILED - A parsing error occured while decoding the stream.  
 *
 *     XA_STATUS_NO_RESOURCES - The length of the PCM data block is not 
 *         sufficient to receive even a single decoded SBC frame.  
 *
 *     XA_STATUS_CONTINUE - Only part of the SBC data was decoded, or the
 *         PcmData strcuture has not been filled.  SBC_DecodeFrame must be
 *         called with new data from the stream to continue decoding.
 */
XaStatus SBC_DecodeFrames(SbcDecoder *Decoder, 
                          U8         *Buff, 
                          U16         Len, 
                          U16        *BytesParsed, 
                          SbcPcmData *PcmData, 
                          U16         MaxPcmData,
                          float*       gains);

#endif /* SBC_DECODER == XA_ENABLED */ 

#if SBC_ENCODER == XA_ENABLED

/*---------------------------------------------------------------------------
 * SBC_InitEncoder()
 *
 *     Initializes a encoder structure for processing a stream.  Must be 
 *     called before each new stream is encoded.  The same initialized 
 *     encoder structure must be used to encode the entire stream (passed to 
 *     SBC_EncodeFrame).  If the stream parameters change (sampling 
 *     frequency, number of subbands, etc.), the encoder structure must be 
 *     reinitialized.  
 *
 * Parameters:
 *
 *     Encoder - An SBC encoder structure.  
 *
 * Returns:
 *
 *     void
 */
void SBC_InitEncoder(SbcEncoder *Encoder);

/*---------------------------------------------------------------------------
 * SBC_EncodeFrames()
 *
 *     Encodes PCM data into the SBC data stream.  PCM data of any arbitrary 
 *     size (up to 65535 bytes) is accepted.  When the SBC data buffer has 
 *     been filled as much as possible, this function returns 
 *     XA_STATUS_SUCCESS.  If XA_ERROR_CHECK is enabled, and the fields of 
 *     the SbcFrame structure are incorrect, then XA_STATUS_INVALID_PARM is 
 *     returned.  
 *
 *     PcmData->len must be greater than or equal to 
 *     Encoder->streamInfo->numBlocks * Encoder->streamInfo->numSubBands * 
 *     Encoder->streamInfo->numChannels * 2.  If the available PCM data is 
 *     smaller than this, it must be padded to this size with zeros (muted).  
 *     If PcmData->len is too small, this function will return 
 *     XA_STATUS_FAILED.  
 *
 *     The memory pointed to by the Buff parameter must be at least large 
 *     enough to receive an entire encoded SBC frame.  The frame length can 
 *     be checked by calling SBC_FrameLen.  The frame length is dependent on 
 *     several fields of the SbcEncoder->streamInfo structure, including 
 *     the bitPool value.  Since the bitPool value can be changed from frame 
 *     to frame, it would be prudent to allocate a buffer to receive SBC 
 *     frames that is large enough to receive data based on the largest 
 *     possible bitPool.  
 *
 *     In order to increase performance, no error checking is done when 
 *     XA_ERROR_CHECK or XA_DEBUG are disabled.  It is important to test with 
 *     these enabled before building a release product.  An improperly formed 
 *     SbcEncoder structure will yield unexpected results.  
 *
 * Parameters:
 *
 *     Encoder - An initialized SBC Encoder structure.  No checking is done 
 *         to determine if the Encoder has been initialized, therefore it is 
 *         up to the user to ensure that SBC_InitEncoder has been called 
 *         prior to using this structure.  This same structure must be used 
 *         for each call while processing the same stream.  The Encoder is 
 *         initialized by calling SBC_InitEncoder and then by initializing 
 *         each of the following parameters with information about the 
 *         stream.  
 *
 *             streamInfo.bitPool - set to the desired bitpool value for the 
 *                 stream.  This value is dynamic and can change depending on 
 *                 the bandwidth of the data stream.  A2DP negotiation will 
 *                 provide a minimum and maximum bitpool value.  
 * 
 *             streamInfo.sampleFreq - set to the sampling frequence of the 
 *                 PCM data.
 *
 *             streamInfo.channelMode - set to the channel mode of the stream 
 *                 (mono, dual channel, stereo, and joint stereo).  
 *
 *             streamInfo.allocMethod - allocation method for encoded samples 
 *                 (SNR or loudness).  
 *
 *             streamInfo.numBlocks - number of blocks of encoded samples per 
 *                 frame.  
 *
 *             streamInfo.numSubBands - number of subbands in the encoded 
 *                 stream.  
 *
 *         Values for each of these fields are derived from A2DP negotation.  
 *         See SbcStreamInfo for more information.  
 *
 *
 *     PcmData - A structure which contains the PCM data to encode.  
 *         The value of PcmData->len must be initialized to the length of the 
 *         data, and PcmData->data must contain valid 16-bit PCM data.  
 *         PCM data with 2 channels are encoded with the sample from the left 
 *         channel followed by the sample from the right channel.   Byte 
 *         order of each sample is determined by the SBC_PCM_BIG_ENDIAN 
 *         constant.  
 *
 *     BytesEncoded - Returns the number of bytes from the PCM data block 
 *         that were encoded into the SBC frames.  
 *
 *     Buff - Returns the encoded SBC frames.  
 *
 *     Len - Returns the length of the encoded SBC frames.  
 *
 *     MaxSbcData - This is the length of the buffer to receive encoded 
 *         SBC frames.  SBC_EncodeFrame will encode as many full SBC frames 
 *         as possible into this buffer.  
 *
 * Returns:
 *
 *     XA_STATUS_SUCCESS - The PCM data buffer was decoded and Buff has been 
 *         filled with SBC frames.  
 *
 *     XA_STATUS_FAILED - MaxSbcData is not large enough to receive even one 
 *         SBC frame.  
 *
 *     XA_STATUS_CONTINUE - Not enough PCM data to create an SBC frame.  Get 
 *         more data and try again.  
 *
 *     XA_STATUS_INVALID_PARM - When XA_ERROR_CHECK is enabled, parameters 
 *         are checked for validity, otherwise no error checking is done in 
 *         order to increase performance.  
 */
XaStatus SBC_EncodeFrames(SbcEncoder *Encoder, 
                          SbcPcmData *PcmData, 
                          U16        *BytesEncoded, 
                          U8         *Buff, 
                          U16        *Len, 
                          U16         MaxSbcData);

#endif  /* SBC_ENCODER == XA_ENABLED */ 

#ifdef __SBC_FUNC_IN_ROM__

typedef I8 XaStatus;

typedef U16 (*__SBC_FrameLen)(SbcStreamInfo *StreamInfo);
typedef void (*__SBC_InitDecoder)(SbcDecoder *Decoder);
typedef void  (*__SbcSynthesisFilter4)(SbcDecoder *Decoder, SbcPcmData *PcmData, float* gain);
typedef void (*__SbcSynthesisFilter8)(SbcDecoder *Decoder, SbcPcmData *PcmData, float* gain);
typedef XaStatus (*__SBC_DecodeFrames)(SbcDecoder *Decoder, 
						  U8		 *Buff, 
						  U16		  Len, 
						  U16		 *BytesDecoded, 
						  SbcPcmData *PcmData, 
						  U16		  MaxPcmData,
						  float* gains);

typedef XaStatus (*__SBC_DecodeFrames_Out_SBSamples)(SbcDecoder *Decoder, 
        U8		 *Buff, 
        U16		  Len, 
        U16		 *BytesDecoded, 
        SbcPcmData *PcmData, 
        U16		  MaxPcmData,
        float* gains,
        U8     ChooseDecChannel,
        REAL       *SBSamplesBuf,
        U32        SBSamplesBufLen,
        U32        *SBSamplesBufUsed,
        U8     ChooseSplitChannel);


typedef struct{
    __SBC_FrameLen SBC_FrameLen;
    __SBC_InitDecoder SBC_InitDecoder;
    __SbcSynthesisFilter4 SbcSynthesisFilter4;
    __SbcSynthesisFilter8 SbcSynthesisFilter8;
    __SBC_DecodeFrames SBC_DecodeFrames;
    __SBC_DecodeFrames_Out_SBSamples SBC_DecodeFrames_Out_SBSamples;
}SBC_ROM_STRUCT;

extern SBC_ROM_STRUCT   SBC_ROM_FUNC;
#endif


#endif /* _SBC_H_ */ 

