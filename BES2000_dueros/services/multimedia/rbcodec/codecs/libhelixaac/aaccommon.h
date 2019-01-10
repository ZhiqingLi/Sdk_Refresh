/* ***** BEGIN LICENSE BLOCK *****  
 * Source last modified: $Id: aaccommon.h,v 1.1 2005/02/26 01:47:34 jrecker Exp $ 
 *   
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc. All Rights Reserved.  
 *       
 * The contents of this file, and the files included with this file, 
 * are subject to the current version of the RealNetworks Public 
 * Source License (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the current version of the RealNetworks Community 
 * Source License (the "RCSL") available at 
 * http://www.helixcommunity.org/content/rcsl, in which case the RCSL 
 * will apply. You may also obtain the license terms directly from 
 * RealNetworks.  You may not use this file except in compliance with 
 * the RPSL or, if you have a valid RCSL with RealNetworks applicable 
 * to this file, the RCSL.  Please see the applicable RPSL or RCSL for 
 * the rights, obligations and limitations governing use of the 
 * contents of the file. 
 *   
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the 
 * portions it created. 
 *   
 * This file, and the files included with this file, is distributed 
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
 * ENJOYMENT OR NON-INFRINGEMENT. 
 *  
 * Technology Compatibility Kit Test Suite(s) Location:  
 *    http://www.helixcommunity.org/content/tck  
 *  
 * Contributor(s):  
 *   
 * ***** END LICENSE BLOCK ***** */  

/**************************************************************************************
 * Fixed-point HE-AAC decoder
 * Jon Recker (jrecker@real.com)
 * February 2005
 *
 * aaccommon.h - implementation-independent API's, datatypes, and definitions
 **************************************************************************************/

#ifndef _AACCOMMON_H
#define _AACCOMMON_H

#include "aacdec.h"
// #include "statname.h"

/* 12-bit syncword */
#define	SYNCWORDH			0xff
#define	SYNCWORDL			0xf0

#define MAX_NCHANS_ELEM		2	/* max number of channels in any single bitstream element (SCE,CPE,CCE,LFE) */

#define ADTS_HEADER_BYTES	7
#define NUM_SAMPLE_RATES	12
#define NUM_DEF_CHAN_MAPS	8
#define NUM_ELEMENTS		8
#define MAX_NUM_PCE_ADIF	16

#define MAX_WIN_GROUPS		8
#define MAX_SFB_SHORT		15
#define MAX_SF_BANDS		(MAX_SFB_SHORT*MAX_WIN_GROUPS)	/* worst case = 15 sfb's * 8 windows for short block */
#define MAX_MS_MASK_BYTES	((MAX_SF_BANDS + 7) >> 3)
#define MAX_PRED_SFB		41
#define MAX_TNS_FILTERS		8
#define MAX_TNS_COEFS		60
#define MAX_TNS_ORDER		20
#define MAX_PULSES			4
#define MAX_GAIN_BANDS		3
#define MAX_GAIN_WIN		8
#define MAX_GAIN_ADJUST		7

#define NSAMPS_LONG			1024
#define NSAMPS_SHORT		128

#define NUM_SYN_ID_BITS		3
#define NUM_INST_TAG_BITS	4

#define EXT_SBR_DATA		0x0d
#define EXT_SBR_DATA_CRC	0x0e

#define IS_ADIF(p)		((p)[0] == 'A' && (p)[1] == 'D' && (p)[2] == 'I' && (p)[3] == 'F')
#define GET_ELE_ID(p)	((AACElementID)(*(p) >> (8-NUM_SYN_ID_BITS)))

/* AAC file format */

enum {
	AAC_FF_Unknown = 0,		/* should be 0 on init */

	AAC_FF_ADTS = 1,
	AAC_FF_ADIF = 2,
	AAC_FF_RAW =  3,
	AAC_FF_LOAS = 4

};

/* syntactic element type */
enum {
	AAC_ID_INVALID = -1,

	AAC_ID_SCE =  0,
	AAC_ID_CPE =  1,
	AAC_ID_CCE =  2,
	AAC_ID_LFE =  3,
	AAC_ID_DSE =  4,
	AAC_ID_PCE =  5,
	AAC_ID_FIL =  6,
	AAC_ID_END =  7
};


enum AudioObjectType {
  AOT_NONE             = -1,
  AOT_NULL_OBJECT      = 0,
  AOT_AAC_MAIN         = 1, /**< Main profile                              */
  AOT_AAC_LC           = 2, /**< Low Complexity object                     */
  AOT_AAC_SSR          = 3,
  AOT_AAC_LTP          = 4,
  AOT_SBR              = 5,
  AOT_AAC_SCAL         = 6,
  AOT_TWIN_VQ          = 7,
  AOT_CELP             = 8,
  AOT_HVXC             = 9,
  AOT_RSVD_10          = 10, /**< (reserved)                                */
  AOT_RSVD_11          = 11, /**< (reserved)                                */
  AOT_TTSI             = 12, /**< TTSI Object                               */
  AOT_MAIN_SYNTH       = 13, /**< Main Synthetic object                     */
  AOT_WAV_TAB_SYNTH    = 14, /**< Wavetable Synthesis object                */
  AOT_GEN_MIDI         = 15, /**< General MIDI object                       */
  AOT_ALG_SYNTH_AUD_FX = 16, /**< Algorithmic Synthesis and Audio FX object */
  AOT_ER_AAC_LC        = 17, /**< Error Resilient(ER) AAC Low Complexity    */
  AOT_RSVD_18          = 18, /**< (reserved)                                */
  AOT_ER_AAC_LTP       = 19, /**< Error Resilient(ER) AAC LTP object        */
  AOT_ER_AAC_SCAL      = 20, /**< Error Resilient(ER) AAC Scalable object   */
  AOT_ER_TWIN_VQ       = 21, /**< Error Resilient(ER) TwinVQ object         */
  AOT_ER_BSAC          = 22, /**< Error Resilient(ER) BSAC object           */
  AOT_ER_AAC_LD        = 23, /**< Error Resilient(ER) AAC LowDelay object   */
  AOT_ER_CELP          = 24, /**< Error Resilient(ER) CELP object           */
  AOT_ER_HVXC          = 25, /**< Error Resilient(ER) HVXC object           */
  AOT_ER_HILN          = 26, /**< Error Resilient(ER) HILN object           */
  AOT_ER_PARA          = 27, /**< Error Resilient(ER) Parametric object     */
  AOT_RSVD_28          = 28, /**< might become SSC                          */
  AOT_PS               = 29, /**< PS, Parametric Stereo (includes SBR)      */
  AOT_MPEGS            = 30, /**< MPEG Surround                             */

  AOT_ESCAPE           = 31, /**< Signal AOT uses more than 5 bits          */

  AOT_MP3ONMP4_L1      = 32, /**< MPEG-Layer1 in mp4                        */
  AOT_MP3ONMP4_L2      = 33, /**< MPEG-Layer2 in mp4                        */
  AOT_MP3ONMP4_L3      = 34, /**< MPEG-Layer3 in mp4                        */
  AOT_DTS          = 35, /**< might become DST                          */
  AOT_ALS          = 36, /**< might become ALS                          */
  AOT_AAC_SLS          = 37, /**< AAC + SLS                                 */
  AOT_SLS              = 38, /**< SLS                                       */
  AOT_ER_AAC_ELD       = 39, /**< AAC Enhanced Low Delay                    */

  AOT_USAC             = 42, /**< USAC                                      */
  AOT_SAOC             = 43, /**< SAOC                                      */
  AOT_LD_MPEGS         = 44, /**< Low Delay MPEG Surround                   */

  AOT_RSVD50           = 50,  /**< Interim AOT for Rsvd50                   */

  /* Pseudo AOTs */
  AOT_MP2_AAC_MAIN     = 128, /**< Virtual AOT MP2 Main profile                           */
  AOT_MP2_AAC_LC       = 129, /**< Virtual AOT MP2 Low Complexity profile                 */
  AOT_MP2_AAC_SSR      = 130, /**< Virtual AOT MP2 Scalable Sampling Rate profile         */

  AOT_MP2_SBR          = 132, /**< Virtual AOT MP2 Low Complexity Profile with SBR        */

  AOT_DAB              = 134, /**< Virtual AOT for DAB (Layer2 with scalefactor CRC)      */
  AOT_DABPLUS_AAC_LC   = 135, /**< Virtual AOT for DAB plus AAC-LC                        */
  AOT_DABPLUS_SBR      = 136, /**< Virtual AOT for DAB plus HE-AAC                        */
  AOT_DABPLUS_PS       = 137, /**< Virtual AOT for DAB plus HE-AAC v2                     */

  AOT_PLAIN_MP1        = 140, /**< Virtual AOT for plain mp1                              */
  AOT_PLAIN_MP2        = 141, /**< Virtual AOT for plain mp2                              */
  AOT_PLAIN_MP3        = 142, /**< Virtual AOT for plain mp3                              */

  AOT_DRM_AAC          = 143, /**< Virtual AOT for DRM (ER-AAC-SCAL without SBR)          */
  AOT_DRM_SBR          = 144, /**< Virtual AOT for DRM (ER-AAC-SCAL with SBR)             */
  AOT_DRM_MPEG_PS      = 145, /**< Virtual AOT for DRM (ER-AAC-SCAL with SBR and MPEG-PS) */
  AOT_DRM_SURROUND     = 146, /**< Virtual AOT for DRM Surround (ER-AAC-SCAL (+SBR) +MPS) */

  AOT_MP2_PS           = 156, /**< Virtual AOT MP2 Low Complexity Profile with SBR and PS */

  AOT_MPEGS_RESIDUALS  = 256  /**< Virtual AOT for MPEG Surround residuals                */
};


typedef struct _AACDecInfo {
	/* pointers to platform-specific state information */
	void *psInfoBase;	/* baseline MPEG-4 LC decoding */
	void *psInfoSBR;	/* MPEG-4 SBR decoding */
	
	/* raw decoded data, before rounding to 16-bit PCM (for postprocessing such as SBR) */
	void *rawSampleBuf[AAC_MAX_NCHANS];
	int rawSampleBytes;
	int rawSampleFBits;

	/* fill data (can be used for processing SBR or other extensions) */
	unsigned char *fillBuf;
	int fillCount;
	int fillExtType;

	/* block information */
	int prevBlockID;
	int currBlockID;
	int currInstTag;
	int sbDeinterleaveReqd[MAX_NCHANS_ELEM];
	int adtsBlocksLeft;

	/* user-accessible info */
	int bitRate;
	int nChans;
	int sampRate;
	int profile;
	int format;
	int sbrEnabled;
	int tnsUsed;
	int pnsUsed;
	int frameCount;
      int *buff_start;
      int buff_len;

} AACDecInfo;

/* decoder functions which must be implemented for each platform */
AACDecInfo *AllocateBuffers(void);
void FreeBuffers(AACDecInfo *aacDecInfo);
void ClearBuffer(void *buf, int nBytes);

int UnpackADTSHeader(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail);
int GetADTSChannelMapping(AACDecInfo *aacDecInfo, unsigned char *buf, int bitOffset, int bitsAvail);

int UnpackLOASHeader(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail);

int UnpackADIFHeader(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail);
int SetRawBlockParams(AACDecInfo *aacDecInfo, int copyLast, int nChans, int sampRate, int profile);
int PrepareRawBlock(AACDecInfo *aacDecInfo);
int FlushCodec(AACDecInfo *aacDecInfo);

int DecodeNextElement(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail);
int DecodeNoiselessData(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail, int ch);

int Dequantize(AACDecInfo *aacDecInfo, int ch);
int StereoProcess(AACDecInfo *aacDecInfo);
int DeinterleaveShortBlocks(AACDecInfo *aacDecInfo, int ch);
int PNS(AACDecInfo *aacDecInfo, int ch);
int TNSFilter(AACDecInfo *aacDecInfo, int ch);
int IMDCT(AACDecInfo *aacDecInfo, int ch, int chBase, short *outbuf);

/* SBR specific functions */
int InitSBR(AACDecInfo *aacDecInfo);
void FreeSBR(AACDecInfo *aacDecInfo);
int DecodeSBRBitstream(AACDecInfo *aacDecInfo, int chBase);
int DecodeSBRData(AACDecInfo *aacDecInfo, int chBase, short *outbuf);
int FlushCodecSBR(AACDecInfo *aacDecInfo);

/* aactabs.c - global ROM tables */
extern const int sampRateTab[NUM_SAMPLE_RATES];
extern const int predSFBMax[NUM_SAMPLE_RATES];
extern const int channelMapTab[NUM_DEF_CHAN_MAPS];
extern const int elementNumChans[NUM_ELEMENTS];
extern const unsigned char sfBandTotalShort[NUM_SAMPLE_RATES];
extern const unsigned char sfBandTotalLong[NUM_SAMPLE_RATES];
extern const int sfBandTabShortOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabShort[76];
extern const int sfBandTabLongOffset[NUM_SAMPLE_RATES];
extern const short sfBandTabLong[325];
extern const int tnsMaxBandsShortOffset[AAC_NUM_PROFILES];
extern const unsigned char tnsMaxBandsShort[2*NUM_SAMPLE_RATES];
extern const unsigned char tnsMaxOrderShort[AAC_NUM_PROFILES];
extern const int tnsMaxBandsLongOffset[AAC_NUM_PROFILES];
extern const unsigned char tnsMaxBandsLong[2*NUM_SAMPLE_RATES];
extern const unsigned char tnsMaxOrderLong[AAC_NUM_PROFILES];

#endif	/* _AACCOMMON_H */
