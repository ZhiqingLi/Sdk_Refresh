/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_A2DP_TYPES_H_
#define _BT_A2DP_TYPES_H_

#include <uw_types.h>

/* *******************************************************************
 * Common Bluetooth Advanced Audio Distribution Profile (A2DP) Types
 * ******************************************************************* */

/* A2DP Codec Types */
#define A2DP_CODEC_SBC          0x00
#define A2DP_CODEC_MPEG12       0x01
#define A2DP_CODEC_MPEG_AAC     0x02
#define A2DP_CODEC_ATRAC        0x03
#define A2DP_CODEC_NON_A2DP     0xff

#define A2DP_CODEC_ANY          (-1)
typedef uint8_t bt_a2dp_codec_type_t;

/* Length of codec information element */
#define a2dp_codec_ie_length(codec_type) (\
    (codec_type) == A2DP_CODEC_SBC ? sizeof(bt_a2dp_sbc_t) :\
    (codec_type) == A2DP_CODEC_MPEG12 ? sizeof(bt_a2dp_mpeg_1_2_audio_t) :\
    (codec_type) == A2DP_CODEC_MPEG_AAC ? sizeof(bt_a2dp_mpeg_aac_t) :\
    (codec_type) == A2DP_CODEC_ATRAC ? sizeof(bt_a2dp_atrac_t) :\
    0)

/* ================================================== */
/* SBC Codec-Specific Information Elements */
typedef   struct {
    uint8_t     octet0; /* Sampling frequencies & channel modes bitmap */
    uint8_t     octet1; /* Block length, subbands & allocation methods bitmap */
    uint8_t     min_bitpool; /* Minimum bitpool value -- 2--250 */
    uint8_t     max_bitpool; /* Maximum bitpool value -- 2--250 */
} __PACKED_POST__ bt_a2dp_sbc_t;

/* SBC Sampling Frequencies (Octet0 Bits 7..4) */
#define SBC_FREQ_16K            (1 << 7) /* 16 KHz */
#define SBC_FREQ_32K            (1 << 6) /* 32 KHz */
#define SBC_FREQ_44K            (1 << 5) /* 44.1 KHz */
#define SBC_FREQ_48K            (1 << 4) /* 48 KHz */
/* Octet0 Sampling Frequency Bits Mask */
#define SBC_FREQ_MASK           0xf0
#define BEKEN_SBC_FREQ_MASK     SBC_FREQ_44K | SBC_FREQ_48K

typedef uint8_t bt_sbc_freq_t;

/* SBC Channel Modes (Octet0 Bits 3..0) */
#define SBC_CHANNEL_MONO                (1 << 3) /* Mono */
#define SBC_CHANNEL_DUAL                (1 << 2) /* Dual channel */
#define SBC_CHANNEL_STEREO              (1 << 1) /* Stereo */
#define SBC_CHANNEL_JOINT_STEREO        (1 << 0) /* Joint stereo */
/* Octet0 Channel Mode Bits Mask */
#define SBC_CHANNEL_MASK                0x0f

typedef uint8_t bt_sbc_chan_t;

/* SBC Block Lengths (Octet1 Bits 7..4)  */
#define SBC_BLOCK_LEN_4         (1 << 7) /* Block length 4 */
#define SBC_BLOCK_LEN_8         (1 << 6) /* Block length 8 */
#define SBC_BLOCK_LEN_12        (1 << 5) /* Block length 12 */
#define SBC_BLOCK_LEN_16        (1 << 4) /* Block length 16 */
/* Octet1 Block Length Bits Mask */
#define SBC_BLOCK_LEN_MASK      0xf0

/* SBC Subbands (Octet1 Bits 3..2) */
#define SBC_SUBBANDS_4          (1 << 3) /* 4 subbands */
#define SBC_SUBBANDS_8          (1 << 2) /* 8 subbands */
/* Octet1 Subband Bits Mask */
#define SBC_SUBBANDS_MASK       0x0c

/* SBC Allocation Modes (Octet1 Bits 1..0) */
#define SBC_ALLOCATION_SNR      (1 << 1) /* Signal-to-noise ratio (SNR) */
#define SBC_ALLOCATION_LOUDNESS (1 << 0) /* Loudness */
/* Octet1 Allocation Bits Mask */
#define SBC_ALLOCATION_MASK     0x03
/* ================================================== */

/* ================================================== */
/* SBC Media Payload Header Format (for internal use) */
typedef   struct {
    uint8_t     octet0; /* SBC header information bitmap */
} __PACKED_POST__ bt_a2dp_sbc_hdr_t;

/* SBC Header Format Information (Octet0 Bits 7..0) */
#define SBC_HDR_FRAGMENTED              (1 << 7) /* Fragmented frame (F bit) */
#define SBC_HDR_START_PACKET            (1 << 6) /* Starting packet (S bit) */
#define SBC_HDR_LAST_PACKET             (1 << 5) /* Last packet (L bit) */
#define SBC_HDR_NUM_OF_FRAMES           0xf /* Number of frames, or number of
                                               remaining fragments -- if
                                               SBC_HDR_FRAGMENTED is set */
/* ================================================== */

/* ================================================== */
/* MPEG-1,2 Audio Codec-Specific Information Elements */
typedef struct {
    uint8_t     octet0; /* Layers, CRC protection & channel modes bitmap */
    uint8_t     octet1; /* Media payload format & sampling frequencies bitmap */
    uint8_t     octet2; /* VBR support and bit-rate index bits 14..8 bitmap */
    uint8_t     octet3; /* Bit-rate index bits 7..0 */
} bt_a2dp_mpeg_1_2_audio_t;

/* MPEG-1,2 Audio Layers (Octet0 Bits 7..5) */
#define MPEG12_LAYER_I          (1 << 7)  /* Layer I (mp1) */
#define MPEG12_LAYER_II         (1 << 6)  /* Layer II (mp2) */
#define MPEG12_LAYER_III        (1 << 5)  /* Layer III (mp3) */

/* MPEG-1,2 Audio Cyclic Redundancy Check (CRC) Protection (Octet0 Bit 4) */
#define MPEG12_CRC              (1 << 4) /* CRC protection supported */

/* MPEG-1,2 Audio Channel Modes (Octet0 Bits 3..0) */
#define MPEG12_CHANNEL_MONO             (1 << 3) /* Mono */
#define MPEG12_CHANNEL_DUAL             (1 << 2) /* Dual channel */
#define MPEG12_CHANNEL_STEREO           (1 << 1) /* Stereo */
#define MPEG12_CHANNEL_JOINT_STEREO     (1 << 0) /* Joint stereo */

/* MPEG-1,2 Audio Media Payload Format (MPF) (Octet1 Bit 6) */
#define MPEG12_MPF (1 << 6) /* MPF-2 is supported or is configured as a
                               transferred media payload format*/

/* MPEG-1,2 Audio Sampling Frequencies (Octet1 Bits 5..0) */
#define MPEG12_FREQ_16K         (1 << 5) /* 16 kHz */
#define MPEG12_FREQ_22K         (1 << 4) /* 22.05 kHz */
#define MPEG12_FREQ_24K         (1 << 3) /* 24 kHz */
#define MPEG12_FREQ_32K         (1 << 2) /* 32 kHz */
#define MPEG12_FREQ_44K         (1 << 1) /* 44.1 kHz */
#define MPEG12_FREQ_48K         (1 << 0) /* 48 kHz */

/* MPEG-1,2 Audio Variable Bit Rate (VBR) Support (Octet2 Bit 7) */
#define MPEG12_VBR              (1 << 7) /* VBR supported */

/* MPEG-1,2 Audio Bit-Rate Index Bits 14..8 Octet2 Mask (Octet2 Bits 6..0) */
#define MPEG12_OCTET2_BIT_RATE_MASK 0x7f
/* ================================================== */

/* ================================================== */
/* MPEG-2,4 AAC Codec-Specific Information Elements */
typedef struct {
    uint8_t     object_type; /* Object types bitmap */
    uint8_t     octet1;      /* Low sampling frequencies bitmap */
    uint8_t     octet2;      /* High sampling frequencies & channels bitmap */
    uint8_t     octet3;      /* VBR support & bit-rate bits 22..16 bitmap */
    uint8_t     octet4;      /* Bit rate bits 15..8 */
    uint8_t     octet5;      /* Bit rate bits 7..0 */
} bt_a2dp_mpeg_aac_t;

/* MPEG-2,4 AAC Object Types (Octet0) */
#define MPEG2_AAC_LC                    (1 << 7) /* MPEG-2 AAC LC */
#define MPEG4_AAC_LC                    (1 << 6) /* MPEG42 AAC LC */
#define MPEG4_AAC_LTP                   (1 << 5) /* MPEG-2 AAC LTP */
#define MPEG4_AAC_SCALABLE              (1 << 4) /* MPEG-4 AAC scalable */

#define MPEG_AAC_ALL_TYPES              0xf0 /* All MPEG-2,4 AAC object types */

/* MPEG-2,4 AAC Low Sampling Frequencies (Octet1) */
#define MPEG_AAC_FREQ_8K                (1 << 7) /* 8 kHz */
#define MPEG_AAC_FREQ_11K               (1 << 6) /* 11.025 kHz */
#define MPEG_AAC_FREQ_12K               (1 << 5) /* 12 kHz */
#define MPEG_AAC_FREQ_16K               (1 << 4) /* 16 kHz */
#define MPEG_AAC_FREQ_22K               (1 << 3) /* 22.05 kHz */
#define MPEG_AAC_FREQ_24K               (1 << 2) /* 24 kHz */
#define MPEG_AAC_FREQ_32K               (1 << 1) /* 32 kHz */
#define MPEG_AAC_FREQ_44K               (1 << 0) /* 44.1 kHz */

#define MPEG_AAC_FREQ_LOW_ALL           0xff /* All low sampling frequencies */

/* MPEG-2,4 AAC High Sampling Frequencies (Octet2 Bits 7..4) */
#define MPEG_AAC_FREQ_48K               (1 << 7) /* 48 kHz */
#define MPEG_AAC_FREQ_64K               (1 << 6) /* 64 kHz */
#define MPEG_AAC_FREQ_88K               (1 << 5) /* 88.2 kHz */
#define MPEG_AAC_FREQ_96K               (1 << 4) /* 96 kHz */

#define MPEG_AAC_FREQ_HIGH_ALL          0xf0 /* All high sampling frequencies */

/* Minimum Frequency Supported by the AAC Codec
 * The frequencies are according to the A2DP v1.2 specification, Table 4.15. */
#define MPEG_AAC_FREQ_MIN(aac) (\
    ((aac)->octet1 & MPEG_AAC_FREQ_8K) ? 8000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_11K) ? 11025 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_12K) ? 12000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_16K) ? 16000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_22K) ? 22050 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_24K) ? 24000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_32K) ? 32000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_44K) ? 44100 :\
    ((aac)->octet2 & MPEG_AAC_FREQ_48K) ? 48000 :\
    ((aac)->octet2 & MPEG_AAC_FREQ_64K) ? 64000 :\
    ((aac)->octet2 & MPEG_AAC_FREQ_88K) ? 88200 :\
    ((aac)->octet2 & MPEG_AAC_FREQ_96K) ? 96000 :\
    44100 /* Fallback */)

/* Maximum Frequency Supported by the AAC Codec
 * The frequencies are according to the A2DP v1.2 specification, Table 4.15. */
#define MPEG_AAC_FREQ_MAX(aac) (\
    ((aac)->octet2 & MPEG_AAC_FREQ_96K) ? 96000 :\
    ((aac)->octet2 & MPEG_AAC_FREQ_88K) ? 88200 :\
    ((aac)->octet2 & MPEG_AAC_FREQ_64K) ? 64000 :\
    ((aac)->octet2 & MPEG_AAC_FREQ_48K) ? 48000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_44K) ? 44100 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_32K) ? 32000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_24K) ? 24000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_22K) ? 22050 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_16K) ? 16000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_12K) ? 12000 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_11K) ? 11025 :\
    ((aac)->octet1 & MPEG_AAC_FREQ_8K) ? 8000 :\
    44100 /* Fallback */)

/* MPEG-2,4 AAC Channels (Octet2 Bits 3..2) */
#define MPEG_AAC_CHANNELS_1             (1 << 3)
#define MPEG_AAC_CHANNELS_2             (1 << 2)

/* MPEG-2,4 AAC Variable Bit Rate (VBR) Support (Octet2 Bit 7) */
#define MPEG_AAC_VBR                    (1 << 7)

/* MPEG-2,4 AAC Bit-Rate Bits 22..16 Octet3 Mask (Octet3 Bits 6..0) */
#define MPEG_AAC_OCTET3_BIT_RATE_MASK   0x7f

/* Get MPEG-2,4 AAC Bit Rate (a 23-bit UiMsbf Value) From bt_a2dp_mpeg_aac_t */
#define MPEG_AAC_BIT_RATE(aac) ((uint32_t)(\
    (((aac)->octet3 & MPEG_AAC_OCTET3_BIT_RATE_MASK) << 16) |\
    ((aac)->octet4 << 8) |\
    ((aac)->octet5)))
/* ================================================== */

/* ATRAC Codec-Specific Information Elements */
typedef struct {
    uint8_t     octet0;
    uint8_t     octet1;
    uint8_t     octet2;
    uint8_t     octet3;
    uint8_t     octet4;
    uint8_t     octet5;
    uint8_t     octet6;
} bt_a2dp_atrac_t;

/* Codec Descriptor */
typedef struct {
    bt_a2dp_codec_type_t          type;
    union a2dp_codec {
        bt_a2dp_sbc_t             sbc;
        bt_a2dp_mpeg_1_2_audio_t  mpeg_1_2_audio;
        bt_a2dp_mpeg_aac_t        mpeg_aac;
        bt_a2dp_atrac_t           atrac;
    } u;
} bt_a2dp_codec_t;

/* Audio Stream Endpoint (SEP) Descriptor */
typedef struct {
    uint32_t                      codecs_count;
    bt_a2dp_codec_t               codecs[1]; /* One of the codecs must be SBC */
} bt_a2dp_endpoint_desc_t;

#endif

