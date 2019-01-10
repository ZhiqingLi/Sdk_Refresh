#ifndef AUDIO_DEF_H
#define AUDIO_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define CODEC_FREQ_26M                      26000000
#define CODEC_FREQ_24P576M                  24576000
#define CODEC_FREQ_22P5792M                 22579200

#if 0
#elif defined(CHIP_BEST3001) || defined (CHIP_BEST2000)

#define CODEC_PLL_DIV                       32
#define CODEC_CMU_DIV                       8
#define CODEC_PLAYBACK_BIT_DEPTH            20

#elif defined(CHIP_BEST2300)

#define CODEC_FREQ_48K_SERIES               (CODEC_FREQ_24P576M / 2)
#define CODEC_FREQ_44_1K_SERIES             (CODEC_FREQ_22P5792M / 2)

#define CODEC_PLL_DIV                       16
#define CODEC_CMU_DIV                       16
#define CODEC_PLAYBACK_BIT_DEPTH            24

#elif defined(CHIP_BEST1000)

#if defined(__AUDIO_DIV_10___)
#define CODEC_PLL_DIV                       10
#elif defined(__AUDIO_DIV_9___)
#define CODEC_PLL_DIV                       9
#else
#define CODEC_PLL_DIV                       8
#endif
#define CODEC_PLAYBACK_BIT_DEPTH            18

#else

#error "Please update audio definitions"

#endif

#ifndef CODEC_FREQ_48K_SERIES
#define CODEC_FREQ_48K_SERIES               CODEC_FREQ_24P576M
#endif
#ifndef CODEC_FREQ_44_1K_SERIES
#define CODEC_FREQ_44_1K_SERIES             CODEC_FREQ_22P5792M
#endif

#define CODEC_TX_PA_GAIN_DEFAULT            -1

enum AUD_STREAM_USE_DEVICE_T{
    AUD_STREAM_USE_EXT_CODEC = 0,
    AUD_STREAM_USE_I2S_MASTER,
    AUD_STREAM_USE_I2S_SLAVE,
    AUD_STREAM_USE_INT_CODEC,
    AUD_STREAM_USE_INT_SPDIF,
    AUD_STREAM_USE_BT_PCM,
    AUD_STREAM_USE_DPD_RX,
    AUD_STREAM_USE_MC,
    AUD_STREAM_USE_DEVICE_NUM,

    AUD_STREAM_USE_DEVICE_NULL = 0xff,
};

enum AUD_SAMPRATE_T {
    AUD_SAMPRATE_7350 = 7350,
    AUD_SAMPRATE_8000 = 8000,
    AUD_SAMPRATE_8463 = 8463,       // 26M / 512 / 6
    AUD_SAMPRATE_14700 = 14700,
    AUD_SAMPRATE_16000 = 16000,
    AUD_SAMPRATE_16927 = 16927,     // 26M / 512 / 3
    AUD_SAMPRATE_22050 = 22050,
    AUD_SAMPRATE_24000 = 24000,
    AUD_SAMPRATE_32000 = 32000,
    AUD_SAMPRATE_44100 = 44100,
    AUD_SAMPRATE_48000 = 48000,
    AUD_SAMPRATE_50781 = 50781,     // 26M / 512
    AUD_SAMPRATE_88200 = 88200,
    AUD_SAMPRATE_96000 = 96000,
    AUD_SAMPRATE_128000 = 128000,
    AUD_SAMPRATE_176400 = 176400,
    AUD_SAMPRATE_192000 = 192000,
    AUD_SAMPRATE_352800 = 352800,
    AUD_SAMPRATE_384000 = 384000,
    AUD_SAMPRATE_768000 = 768000,

    AUD_SAMPRATE_NULL = 0,
};

enum AUD_CHANNEL_NUM_T {
    AUD_CHANNEL_NUM_1 = 1,
    AUD_CHANNEL_NUM_2 = 2,
    AUD_CHANNEL_NUM_3 = 3,
    AUD_CHANNEL_NUM_4 = 4,
    AUD_CHANNEL_NUM_5 = 5,
    AUD_CHANNEL_NUM_6 = 6,
    AUD_CHANNEL_NUM_7 = 7,
    AUD_CHANNEL_NUM_8 = 8,

    AUD_CHANNEL_NUM_NULL = 0xff,
};

// For preprocess check
#define AUD_CHANNEL_MAP_CH0                 (1 << 0)
#define AUD_CHANNEL_MAP_CH1                 (1 << 1)
#define AUD_CHANNEL_MAP_CH2                 (1 << 2)
#define AUD_CHANNEL_MAP_CH3                 (1 << 3)
#define AUD_CHANNEL_MAP_CH4                 (1 << 4)
#define AUD_CHANNEL_MAP_CH5                 (1 << 5)
#define AUD_CHANNEL_MAP_CH6                 (1 << 6)
#define AUD_CHANNEL_MAP_CH7                 (1 << 7)
#define AUD_CHANNEL_MAP_DIGMIC_CH0          (1 << 8)
#define AUD_CHANNEL_MAP_DIGMIC_CH1          (1 << 9)
#define AUD_CHANNEL_MAP_DIGMIC_CH2          (1 << 10)
#define AUD_CHANNEL_MAP_DIGMIC_CH3          (1 << 11)
#define AUD_CHANNEL_MAP_DIGMIC_CH4          (1 << 12)
#define AUD_CHANNEL_MAP_DIGMIC_CH5          (1 << 13)
#define AUD_CHANNEL_MAP_DIGMIC_CH6          (1 << 14)
#define AUD_CHANNEL_MAP_DIGMIC_CH7          (1 << 15)

#define AUD_CHANNEL_MAP_ANA_ALL             (0x00FF)
#define AUD_CHANNEL_MAP_DIGMIC_ALL          (0xFF00)
#define AUD_CHANNEL_MAP_ALL                 (0xFFFF)

enum AUD_CHANNEL_MAP_T {
    AUD_CHANNEL_MAP_END = AUD_CHANNEL_MAP_DIGMIC_CH7,
};

enum AUD_VMIC_MAP_T {
    AUD_VMIC_MAP_VMIC1 = (AUD_CHANNEL_MAP_END << 1),
    AUD_VMIC_MAP_VMIC2 = (AUD_CHANNEL_MAP_END << 2),
    AUD_VMIC_MAP_VMIC3 = (AUD_CHANNEL_MAP_END << 3),
    AUD_VMIC_MAP_VMIC4 = (AUD_CHANNEL_MAP_END << 4),
    AUD_VMIC_MAP_VMIC5 = (AUD_CHANNEL_MAP_END << 5),
};

enum AUD_BITS_T {
    AUD_BITS_8 = 8,
    AUD_BITS_12 = 12,
    AUD_BITS_16 = 16,
    AUD_BITS_20 = 20,
    AUD_BITS_24 = 24,
    AUD_BITS_32 = 32,

    AUD_BITS_NULL = 0xff,
};

enum AUD_STREAM_ID_T {
    AUD_STREAM_ID_0 = 0,
    AUD_STREAM_ID_1,
    AUD_STREAM_ID_2,
    AUD_STREAM_ID_NUM,
};

enum AUD_STREAM_T {
    AUD_STREAM_PLAYBACK = 0,
    AUD_STREAM_CAPTURE,
    AUD_STREAM_NUM,
};

// TODO: Need rename
enum AUD_IO_PATH_T {
    AUD_IO_PATH_NULL = 0,

    AUD_INPUT_PATH_MAINMIC,
    AUD_INPUT_PATH_LINEIN,

    AUD_OUTPUT_PATH_SPEAKER,
};

enum ANC_TYPE_T {
    ANC_NOTYPE = 0,
    ANC_FEEDFORWARD,
    ANC_FEEDBACK,
};

struct CODEC_DAC_VOL_T {
    signed char tx_pa_gain      :6;
    unsigned char sdm_gain      :2;
    signed char sdac_volume;
};

enum TGT_VOLUME_LEVEL_T {
    TGT_VOLUME_LEVEL_WARNINGTONE = 0,

    TGT_VOLUME_LEVEL_MUTE,
    TGT_VOLUME_LEVEL_0,
    TGT_VOLUME_LEVEL_1,
    TGT_VOLUME_LEVEL_2,
    TGT_VOLUME_LEVEL_3,
    TGT_VOLUME_LEVEL_4,
    TGT_VOLUME_LEVEL_5,
    TGT_VOLUME_LEVEL_6,
    TGT_VOLUME_LEVEL_7,
    TGT_VOLUME_LEVEL_8,
    TGT_VOLUME_LEVEL_9,
    TGT_VOLUME_LEVEL_10,
    TGT_VOLUME_LEVEL_11,
    TGT_VOLUME_LEVEL_12,
    TGT_VOLUME_LEVEL_13,
    TGT_VOLUME_LEVEL_14,
    TGT_VOLUME_LEVEL_15,

    TGT_VOLUME_LEVEL_QTY
};

enum TGT_ADC_VOL_LEVEL_T {
    TGT_ADC_VOL_LEVEL_0,
    TGT_ADC_VOL_LEVEL_1,
    TGT_ADC_VOL_LEVEL_2,
    TGT_ADC_VOL_LEVEL_3,
    TGT_ADC_VOL_LEVEL_4,
    TGT_ADC_VOL_LEVEL_5,
    TGT_ADC_VOL_LEVEL_6,
    TGT_ADC_VOL_LEVEL_7,
    TGT_ADC_VOL_LEVEL_8,
    TGT_ADC_VOL_LEVEL_9,
    TGT_ADC_VOL_LEVEL_10,
    TGT_ADC_VOL_LEVEL_11,
    TGT_ADC_VOL_LEVEL_12,
    TGT_ADC_VOL_LEVEL_13,
    TGT_ADC_VOL_LEVEL_14,
    TGT_ADC_VOL_LEVEL_15,

    TGT_ADC_VOL_LEVEL_QTY
};

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_DEF_H */
