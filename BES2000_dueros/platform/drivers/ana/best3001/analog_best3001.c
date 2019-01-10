#include <stdint.h>
#include "stdbool.h"
#include "cmsis.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "hal_codec.h"
#include "hal_chipid.h"
#include "analog.h"
#include "pmu.h"

#define ANALOG_TRACE(s,...)                 //TRACE(s, ##__VA_ARGS__)

#define TX_PA_GAIN_0DB_DIFF                 0x0
#define TX_PA_GAIN_0DB_SE                   0x2

#define DEFAULT_TX_PA_GAIN                  tx_pa_gain_0db

#define DEFAULT_ANC_FF_ADC_GAIN_DB          12
#define DEFAULT_ANC_FB_ADC_GAIN_DB          12
#define DEFAULT_VOICE_ADC_GAIN_DB           24

#ifndef ANALOG_ADC_A_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH0) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH0))
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH0) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH0))
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_A_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_B_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH1) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH1))
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH1) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH1))
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_B_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_C_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH2) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH2))
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH2) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH2))
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_C_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_D_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH3) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH3))
#define ANALOG_ADC_D_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH3) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH3))
#define ANALOG_ADC_D_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_D_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef ANALOG_ADC_E_GAIN_DB
#if defined(ANC_APP) && defined(ANC_FF_ENABLED) && ((ANC_FF_MIC_CH_L == AUD_CHANNEL_MAP_CH4) || (ANC_FF_MIC_CH_R == AUD_CHANNEL_MAP_CH4))
#define ANALOG_ADC_E_GAIN_DB                DEFAULT_ANC_FF_ADC_GAIN_DB
#elif defined(ANC_APP) && defined(ANC_FB_ENABLED) && ((ANC_FB_MIC_CH_L == AUD_CHANNEL_MAP_CH4) || (ANC_FB_MIC_CH_R == AUD_CHANNEL_MAP_CH4))
#define ANALOG_ADC_E_GAIN_DB                DEFAULT_ANC_FB_ADC_GAIN_DB
#else
#define ANALOG_ADC_E_GAIN_DB                DEFAULT_VOICE_ADC_GAIN_DB
#endif
#endif

#ifndef LINEIN_ADC_GAIN_DB
#define LINEIN_ADC_GAIN_DB                  0
#endif

#ifndef CFG_HW_AUD_INPUT_PATH_LINEIN_DEV
#define CFG_HW_AUD_INPUT_PATH_LINEIN_DEV    0
#endif

#ifndef CFG_HW_AUD_MICKEY_DEV
#define CFG_HW_AUD_MICKEY_DEV               (AUD_VMIC_MAP_VMIC1)
#endif

#ifndef CFG_HW_AUD_INPUT_PATH_ANC_DEV
#define CFG_HW_AUD_INPUT_PATH_ANC_DEV       (AUD_VMIC_MAP_VMIC1)
#endif

#define DAC_GAIN_TO_QDB(n)                  ((n) * 3 * 4)
#define QDB_TO_DAC_GAIN(n)                  ((n) / 4 / 3)

#define DEFAULT_XTAL_FCAP                   0x8000
#define DEFAULT_XTAL_FVAR                   0x8000

// Default 0.26 ppm/FcapBit or 260 ppb/FcapBit
#define DEFAULT_XTAL_TUNE_FACTOR            260

#define MAX_XTAL_VALUE                      0x7FF

#define XTAL_TUNE_FACTOR_UPDATE_THRESH      (16 * 8)
#define XTAL_TUNE_FACTOR_UPDATE_STEP        (128 * 8)

// REG61
#define REG_CODEC_RX_EN_ADCA                (1 << 0)
#define REG_CODEC_RX_EN_ADCB                (1 << 1)
#define REG_CODEC_RX_EN_ADCC                (1 << 2)
#define REG_CODEC_RX_EN_ADCD                (1 << 3)
#define REG_CODEC_RX_EN_ADCE                (1 << 4)
#define REG_CODEC_TX_EN_LDAC                (1 << 5)
#define REG_CODEC_TX_EN_RDAC                (1 << 6)
#define REG_CODEC_EN_TX                     (1 << 7)
#define CFG_TX_CH0_MUTE                     (1 << 8)
#define CFG_TX_CH1_MUTE                     (1 << 9)
#define REG_CODEC_TX_DAC_MUTEL              (1 << 10)
#define REG_CODEC_TX_DAC_MUTER              (1 << 11)
#define REG_CODEC_TX_EN_LPA                 (1 << 12)
#define REG_CODEC_TX_EN_RPA                 (1 << 13)
#define REG_CODEC_TX_EN_LEAR                (1 << 14)
#define REG_CODEC_TX_EN_REAR                (1 << 15)

// REG62
#define REG_CODEC_PGA1A_EN                  (1 << 0)
#define REG_CODEC_PGA2A_EN                  (1 << 1)
#define REG_CODEC_PGA1B_EN                  (1 << 2)
#define REG_CODEC_PGA2B_EN                  (1 << 3)
#define REG_CODEC_PGA1C_EN                  (1 << 4)
#define REG_CODEC_PGA2C_EN                  (1 << 5)
#define REG_CODEC_PGA1D_EN                  (1 << 6)
#define REG_CODEC_PGA2D_EN                  (1 << 7)
#define REG_CODEC_PGA1E_EN                  (1 << 8)
#define REG_CODEC_PGA2E_EN                  (1 << 9)
#define REG_CODEC_TX_PA_OFF_EN              (1 << 10)
#define REG_CODEC_TX_EAR_OFF_EN             (1 << 11)
#define REG_CODEC_PGA1E_CHANSEL_SHIFT       12
#define REG_CODEC_PGA1E_CHANSEL_MASK        (0x3 << REG_CODEC_PGA1E_CHANSEL_SHIFT)
#define REG_CODEC_PGA1E_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA1E_CHANSEL, n)
#define REG_CODEC_PGA2E_CHANSEL_SHIFT       14
#define REG_CODEC_PGA2E_CHANSEL_MASK        (0x3 << REG_CODEC_PGA2E_CHANSEL_SHIFT)
#define REG_CODEC_PGA2E_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA2E_CHANSEL, n)

// REG63
#define REG_CODEC_PGA1A_CHANSEL_SHIFT       0
#define REG_CODEC_PGA1A_CHANSEL_MASK        (0x3 << REG_CODEC_PGA1A_CHANSEL_SHIFT)
#define REG_CODEC_PGA1A_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA1A_CHANSEL, n)
#define REG_CODEC_PGA2A_CHANSEL_SHIFT       2
#define REG_CODEC_PGA2A_CHANSEL_MASK        (0x3 << REG_CODEC_PGA2A_CHANSEL_SHIFT)
#define REG_CODEC_PGA2A_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA2A_CHANSEL, n)
#define REG_CODEC_PGA1B_CHANSEL_SHIFT       4
#define REG_CODEC_PGA1B_CHANSEL_MASK        (0x3 << REG_CODEC_PGA1B_CHANSEL_SHIFT)
#define REG_CODEC_PGA1B_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA1B_CHANSEL, n)
#define REG_CODEC_PGA2B_CHANSEL_SHIFT       6
#define REG_CODEC_PGA2B_CHANSEL_MASK        (0x3 << REG_CODEC_PGA2B_CHANSEL_SHIFT)
#define REG_CODEC_PGA2B_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA2B_CHANSEL, n)
#define REG_CODEC_PGA1C_CHANSEL_SHIFT       8
#define REG_CODEC_PGA1C_CHANSEL_MASK        (0x3 << REG_CODEC_PGA1C_CHANSEL_SHIFT)
#define REG_CODEC_PGA1C_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA1C_CHANSEL, n)
#define REG_CODEC_PGA2C_CHANSEL_SHIFT       10
#define REG_CODEC_PGA2C_CHANSEL_MASK        (0x3 << REG_CODEC_PGA2C_CHANSEL_SHIFT)
#define REG_CODEC_PGA2C_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA2C_CHANSEL, n)
#define REG_CODEC_PGA1D_CHANSEL_SHIFT       12
#define REG_CODEC_PGA1D_CHANSEL_MASK        (0x3 << REG_CODEC_PGA1D_CHANSEL_SHIFT)
#define REG_CODEC_PGA1D_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA1D_CHANSEL, n)
#define REG_CODEC_PGA2D_CHANSEL_SHIFT       14
#define REG_CODEC_PGA2D_CHANSEL_MASK        (0x3 << REG_CODEC_PGA2D_CHANSEL_SHIFT)
#define REG_CODEC_PGA2D_CHANSEL(n)          BITFIELD_VAL(REG_CODEC_PGA2D_CHANSEL, n)

// REG64
#define REG_CODEC_TX_PA_GAIN_SHIFT          0
#define REG_CODEC_TX_PA_GAIN_MASK           (0x3 << REG_CODEC_TX_PA_GAIN_SHIFT)
#define REG_CODEC_TX_PA_GAIN(n)             BITFIELD_VAL(REG_CODEC_TX_PA_GAIN, n)
#define REG_CODEC_TX_EAR_RSTB               (1 << 2)
#define REG_CODEC_TX_PA_RSTB                (1 << 3)
#define REG_CODEC_TX_DAC_LPFVCM_SHIFT       4
#define REG_CODEC_TX_DAC_LPFVCM_MASK        (0xF << REG_CODEC_TX_DAC_LPFVCM_SHIFT)
#define REG_CODEC_TX_DAC_LPFVCM(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_LPFVCM, n)
#define CFG_EN_RX                           (1 << 8)
#define CFG_EN_RX_DR                        (1 << 9)
#define REG_AUDPLL_RSTN                     (1 << 10)
#define REG_AUDPLL_RSTN_DR                  (1 << 11)
#define REG_AUDPLL_PU                       (1 << 12)
#define REG_AUDPLL_PU_DR                    (1 << 13)
#define REG_AUDPLL_POSTDIV_RST              (1 << 14)
#define REG_AUDPLL_POSTDIV_RST_DR           (1 << 15)

// REG65
#define REG_AUDPLL_DIVN_320M_SHIFT          0
#define REG_AUDPLL_DIVN_320M_MASK           (0x7 << REG_AUDPLL_DIVN_320M_SHIFT)
#define REG_AUDPLL_DIVN_320M(n)             BITFIELD_VAL(REG_AUDPLL_DIVN_320M, n)
#define REG_AUDPLL_DIVN_480M_SHIFT          3
#define REG_AUDPLL_DIVN_480M_MASK           (0x7 << REG_AUDPLL_DIVN_480M_SHIFT)
#define REG_AUDPLL_DIVN_480M(n)             BITFIELD_VAL(REG_AUDPLL_DIVN_480M, n)
#define REG_AUDPLL_POST_DIVN_SHIFT          6
#define REG_AUDPLL_POST_DIVN_MASK           (0x3F << REG_AUDPLL_POST_DIVN_SHIFT)
#define REG_AUDPLL_POST_DIVN(n)             BITFIELD_VAL(REG_AUDPLL_POST_DIVN, n)
#define REG_AUDPLL_FDIV_SEL_SHIFT           12
#define REG_AUDPLL_FDIV_SEL_MASK            (0x3 << REG_AUDPLL_FDIV_SEL_SHIFT)
#define REG_AUDPLL_FDIV_SEL(n)              BITFIELD_VAL(REG_AUDPLL_FDIV_SEL, n)
#define REG_AUDPLL_FANA960M_DIV2_EN         (1 << 14)
#define REG_AUDPLL_FANA960M_SEL             (1 << 15)

// REG66
#define REG_AUDPLL_MDLL_DIV_SHIFT           0
#define REG_AUDPLL_MDLL_DIV_MASK            (0x3F << REG_AUDPLL_MDLL_DIV_SHIFT)
#define REG_AUDPLL_MDLL_DIV(n)              BITFIELD_VAL(REG_AUDPLL_MDLL_DIV, n)
#define REG_CODEC_TX_CLKSEL                 (1 << 6)
#define REG_AUDPLL_2PATH_EN                 (1 << 7)
#define REG_CRYSTAL_SEL_LV                  (1 << 8)
#define REG_PU_OSC                          (1 << 9)
#define CFG_TX_CLK_INV                      (1 << 10)
#define REG_AUDPLL_CODEC_CLKDIV2_EN         (1 << 11)
#define REG_AUDPLL_BB_AD_SEL                (1 << 12)
#define REG_AUDPLL_SDM_CLK_SEL              (1 << 13)
#define REG_AUDPLL_REFCLK_SEL               (1 << 14)
#define REG_AUDPLL_BT_CLK_EN                (1 << 15)

// REG67
#define REG_AUDPLL_FREQ_15_0_SHIFT          0
#define REG_AUDPLL_FREQ_15_0_MASK           (0xFFFF << REG_AUDPLL_FREQ_15_0_SHIFT)
#define REG_AUDPLL_FREQ_15_0(n)             BITFIELD_VAL(REG_AUDPLL_FREQ_15_0, n)

// REG68
#define REG_AUDPLL_FREQ_31_16_SHIFT         0
#define REG_AUDPLL_FREQ_31_16_MASK          (0xFFFF << REG_AUDPLL_FREQ_31_16_SHIFT)
#define REG_AUDPLL_FREQ_31_16(n)            BITFIELD_VAL(REG_AUDPLL_FREQ_31_16, n)

// REG69
#define REG_CODEC_RX_IBSEL_SHIFT            0
#define REG_CODEC_RX_IBSEL_MASK             (0xF << REG_CODEC_RX_IBSEL_SHIFT)
#define REG_CODEC_RX_IBSEL(n)               BITFIELD_VAL(REG_CODEC_RX_IBSEL, n)
#define REG_CODEC_ADC_IBSEL_SHIFT           4
#define REG_CODEC_ADC_IBSEL_MASK            (0xF << REG_CODEC_ADC_IBSEL_SHIFT)
#define REG_CODEC_ADC_IBSEL(n)              BITFIELD_VAL(REG_CODEC_ADC_IBSEL, n)
#define REG_CODEC_ADC_VREF_SEL_SHIFT        8
#define REG_CODEC_ADC_VREF_SEL_MASK         (0xF << REG_CODEC_ADC_VREF_SEL_SHIFT)
#define REG_CODEC_ADC_VREF_SEL(n)           BITFIELD_VAL(REG_CODEC_ADC_VREF_SEL, n)
#define REG_AUDPLL_FREQ_EN                  (1 << 12)
#define REG_AUDPLL_FREQ_34_32_SHIFT         13
#define REG_AUDPLL_FREQ_34_32_MASK          (0x7 << REG_AUDPLL_FREQ_34_32_SHIFT)
#define REG_AUDPLL_FREQ_34_32(n)            BITFIELD_VAL(REG_AUDPLL_FREQ_34_32, n)

// REG6A
#define REG_AUDPLL_FREQ_OFFSET_SHIFT        0
#define REG_AUDPLL_FREQ_OFFSET_MASK         (0xFFFF << REG_AUDPLL_FREQ_OFFSET_SHIFT)
#define REG_AUDPLL_FREQ_OFFSET(n)           BITFIELD_VAL(REG_AUDPLL_FREQ_OFFSET, n)

// REG6B
#define REG_CODEC_TX_PA_SOFTSTART_SHIFT     0
#define REG_CODEC_TX_PA_SOFTSTART_MASK      (0x3F << REG_CODEC_TX_PA_SOFTSTART_SHIFT)
#define REG_CODEC_TX_PA_SOFTSTART(n)        BITFIELD_VAL(REG_CODEC_TX_PA_SOFTSTART, n)
#define REG_CODEC_TX_PA_IBSEL_SHIFT         6
#define REG_CODEC_TX_PA_IBSEL_MASK          (0x3 << REG_CODEC_TX_PA_IBSEL_SHIFT)
#define REG_CODEC_TX_PA_IBSEL(n)            BITFIELD_VAL(REG_CODEC_TX_PA_IBSEL, n)
#define REG_CODEC_TX_PA_OCEN                (1 << 8)
#define REG_CODEC_TX_PA_DOUBLEBIAS          (1 << 9)
#define REG_CODEC_TX_PA_DIS_SHIFT           10
#define REG_CODEC_TX_PA_DIS_MASK            (0x3 << REG_CODEC_TX_PA_DIS_SHIFT)
#define REG_CODEC_TX_PA_DIS(n)              BITFIELD_VAL(REG_CODEC_TX_PA_DIS, n)
#define REG_CODEC_TX_PA_OUTPUTSEL_SHIFT     12
#define REG_CODEC_TX_PA_OUTPUTSEL_MASK      (0xF << REG_CODEC_TX_PA_OUTPUTSEL_SHIFT)
#define REG_CODEC_TX_PA_OUTPUTSEL(n)        BITFIELD_VAL(REG_CODEC_TX_PA_OUTPUTSEL, n)

// REG6C
#define REG_CODEC_TX_DAC_IBSEL_SHIFT        0
#define REG_CODEC_TX_DAC_IBSEL_MASK         (0xF << REG_CODEC_TX_DAC_IBSEL_SHIFT)
#define REG_CODEC_TX_DAC_IBSEL(n)           BITFIELD_VAL(REG_CODEC_TX_DAC_IBSEL, n)
#define REG_CODEC_TX_DAC_IDAC_SEL_SHIFT     4
#define REG_CODEC_TX_DAC_IDAC_SEL_MASK      (0xF << REG_CODEC_TX_DAC_IDAC_SEL_SHIFT)
#define REG_CODEC_TX_DAC_IDAC_SEL(n)        BITFIELD_VAL(REG_CODEC_TX_DAC_IDAC_SEL, n)
#define REG_CODEC_TX_DAC_VCAS_SEL_SHIFT     8
#define REG_CODEC_TX_DAC_VCAS_SEL_MASK      (0x1F << REG_CODEC_TX_DAC_VCAS_SEL_SHIFT)
#define REG_CODEC_TX_DAC_VCAS_SEL(n)        BITFIELD_VAL(REG_CODEC_TX_DAC_VCAS_SEL, n)
#define REG_CODEC_TX_PA_LOWVCM_SHIFT        13
#define REG_CODEC_TX_PA_LOWVCM_MASK         (0x7 << REG_CODEC_TX_PA_LOWVCM_SHIFT)
#define REG_CODEC_TX_PA_LOWVCM(n)           BITFIELD_VAL(REG_CODEC_TX_PA_LOWVCM, n)

// REG6D
#define CFG_RESET_ADC_A                     (1 << 0)
#define CFG_RESET_ADC_A_DR                  (1 << 1)
#define CFG_RESET_ADC_B                     (1 << 2)
#define CFG_RESET_ADC_B_DR                  (1 << 3)
#define CFG_RESET_ADC_C                     (1 << 4)
#define CFG_RESET_ADC_C_DR                  (1 << 5)
#define CFG_RESET_ADC_D                     (1 << 6)
#define CFG_RESET_ADC_D_DR                  (1 << 7)
#define CFG_RESET_ADC_E                     (1 << 8)
#define CFG_RESET_ADC_E_DR                  (1 << 9)
#define CFG_PGA_RESETN                      (1 << 10)
#define CFG_PGA_RESETN_DR                   (1 << 11)
#define REG_AUDPLL_MDLL_RESETB              (1 << 12)
#define REG_AUDPLL_MDLL_RESETB_DR           (1 << 13)
#define REG_AUDPLL_PFD_RESETB               (1 << 14)
#define CFG_TX_TREE_EN                      (1 << 15)

// REG6E
#define CFG_TX_EN_LPPA                      (1 << 0)
#define CFG_TX_EN_LPPA_DR                   (1 << 1)
#define CFG_TX_EN_S1PA                      (1 << 2)
#define CFG_TX_EN_S1PA_DR                   (1 << 3)
#define CFG_TX_EN_S2PA                      (1 << 4)
#define CFG_TX_EN_S2PA_DR                   (1 << 5)
#define CFG_TX_EN_S3PA                      (1 << 6)
#define CFG_TX_EN_S3PA_DR                   (1 << 7)
#define CFG_TX_EN_DACLDO                    (1 << 8)
#define CFG_TX_EN_DACLDO_DR                 (1 << 9)
#define CFG_TX_EN_VTOI                      (1 << 10)
#define CFG_TX_EN_VTOI_DR                   (1 << 11)
#define REG_IDET_LEAR                       (1 << 12)
#define REG_IDET_REAR                       (1 << 13)
#define REG_IDET_LPA                        (1 << 14)
#define REG_IDET_RPA                        (1 << 15)

// REG6F
#define REG_CODEC_PGA1A_CAPMODE             (1 << 0)
#define REG_CODEC_PGA1A_FASTSETTLE          (1 << 1)
#define REG_CODEC_PGA1B_CAPMODE             (1 << 2)
#define REG_CODEC_PGA1B_FASTSETTLE          (1 << 3)
#define REG_CODEC_PGA1C_CAPMODE             (1 << 4)
#define REG_CODEC_PGA1C_FASTSETTLE          (1 << 5)
#define REG_CODEC_PGA1D_CAPMODE             (1 << 6)
#define REG_CODEC_PGA1D_FASTSETTLE          (1 << 7)
#define REG_CODEC_PGA1E_CAPMODE             (1 << 8)
#define REG_CODEC_PGA1E_FASTSETTLE          (1 << 9)
#define REG_PGA2A_DETEN                     (1 << 10)
#define REG_PGA2B_DETEN                     (1 << 11)
#define REG_PGA2C_DETEN                     (1 << 12)
#define REG_PGA2D_DETEN                     (1 << 13)
#define REG_PGA2E_DETEN                     (1 << 14)
#define REG_IDET_CMFB                       (1 << 15)

// REG70
#define REG_CODEC_PGA1A_GAIN_SHIFT          0
#define REG_CODEC_PGA1A_GAIN_MASK           (0x7 << REG_CODEC_PGA1A_GAIN_SHIFT)
#define REG_CODEC_PGA1A_GAIN(n)             BITFIELD_VAL(REG_CODEC_PGA1A_GAIN, n)
#define REG_CODEC_PGA1B_GAIN_SHIFT          3
#define REG_CODEC_PGA1B_GAIN_MASK           (0x7 << REG_CODEC_PGA1B_GAIN_SHIFT)
#define REG_CODEC_PGA1B_GAIN(n)             BITFIELD_VAL(REG_CODEC_PGA1B_GAIN, n)
#define REG_CODEC_PGA1C_GAIN_SHIFT          6
#define REG_CODEC_PGA1C_GAIN_MASK           (0x7 << REG_CODEC_PGA1C_GAIN_SHIFT)
#define REG_CODEC_PGA1C_GAIN(n)             BITFIELD_VAL(REG_CODEC_PGA1C_GAIN, n)
#define REG_CODEC_PGA1D_GAIN_SHIFT          9
#define REG_CODEC_PGA1D_GAIN_MASK           (0x7 << REG_CODEC_PGA1D_GAIN_SHIFT)
#define REG_CODEC_PGA1D_GAIN(n)             BITFIELD_VAL(REG_CODEC_PGA1D_GAIN, n)
#define REG_CODEC_PGA1E_GAIN_SHIFT          12
#define REG_CODEC_PGA1E_GAIN_MASK           (0x7 << REG_CODEC_PGA1E_GAIN_SHIFT)
#define REG_CODEC_PGA1E_GAIN(n)             BITFIELD_VAL(REG_CODEC_PGA1E_GAIN, n)
#define REG_ADC_SLOWMODE                    (1 << 15)

// REG71
#define REG_PGA2A_GAIN_SHIFT                0
#define REG_PGA2A_GAIN_MASK                 (0x7 << REG_PGA2A_GAIN_SHIFT)
#define REG_PGA2A_GAIN(n)                   BITFIELD_VAL(REG_PGA2A_GAIN, n)
#define REG_PGA2B_GAIN_SHIFT                3
#define REG_PGA2B_GAIN_MASK                 (0x7 << REG_PGA2B_GAIN_SHIFT)
#define REG_PGA2B_GAIN(n)                   BITFIELD_VAL(REG_PGA2B_GAIN, n)
#define REG_PGA2C_GAIN_SHIFT                6
#define REG_PGA2C_GAIN_MASK                 (0x7 << REG_PGA2C_GAIN_SHIFT)
#define REG_PGA2C_GAIN(n)                   BITFIELD_VAL(REG_PGA2C_GAIN, n)
#define REG_PGA2D_GAIN_SHIFT                9
#define REG_PGA2D_GAIN_MASK                 (0x7 << REG_PGA2D_GAIN_SHIFT)
#define REG_PGA2D_GAIN(n)                   BITFIELD_VAL(REG_PGA2D_GAIN, n)
#define REG_PGA2E_GAIN_SHIFT                12
#define REG_PGA2E_GAIN_MASK                 (0x7 << REG_PGA2E_GAIN_SHIFT)
#define REG_PGA2E_GAIN(n)                   BITFIELD_VAL(REG_PGA2E_GAIN, n)
#define REG_CODEC_ADC_VCM_CLOSE             (1 << 15)

// REG72
#define REG_PGA1A_DACGAIN_SHIFT             0
#define REG_PGA1A_DACGAIN_MASK              (0x3 << REG_PGA1A_DACGAIN_SHIFT)
#define REG_PGA1A_DACGAIN(n)                BITFIELD_VAL(REG_PGA1A_DACGAIN, n)
#define REG_PGA1B_DACGAIN_SHIFT             2
#define REG_PGA1B_DACGAIN_MASK              (0x3 << REG_PGA1B_DACGAIN_SHIFT)
#define REG_PGA1B_DACGAIN(n)                BITFIELD_VAL(REG_PGA1B_DACGAIN, n)
#define REG_PGA1C_DACGAIN_SHIFT             4
#define REG_PGA1C_DACGAIN_MASK              (0x3 << REG_PGA1C_DACGAIN_SHIFT)
#define REG_PGA1C_DACGAIN(n)                BITFIELD_VAL(REG_PGA1C_DACGAIN, n)
#define REG_PGA1D_DACGAIN_SHIFT             6
#define REG_PGA1D_DACGAIN_MASK              (0x3 << REG_PGA1D_DACGAIN_SHIFT)
#define REG_PGA1D_DACGAIN(n)                BITFIELD_VAL(REG_PGA1D_DACGAIN, n)
#define REG_PGA1E_DACGAIN_SHIFT             8
#define REG_PGA1E_DACGAIN_MASK              (0x3 << REG_PGA1E_DACGAIN_SHIFT)
#define REG_PGA1E_DACGAIN(n)                BITFIELD_VAL(REG_PGA1E_DACGAIN, n)
#define REG_CODEC_PGA1A_LARGEGAIN           (1 << 10)
#define REG_CODEC_PGA1B_LARGEGAIN           (1 << 11)
#define REG_CODEC_PGA1C_LARGEGAIN           (1 << 12)
#define REG_CODEC_PGA1D_LARGEGAIN           (1 << 13)
#define REG_CODEC_PGA1E_LARGEGAIN           (1 << 14)
#define REG_CODEC_RXBIAS_LOWV               (1 << 15)

// REG73
#define REG_CODEC_TX_EAR_DIS_SHIFT          0
#define REG_CODEC_TX_EAR_DIS_MASK           (0x3 << REG_CODEC_TX_EAR_DIS_SHIFT)
#define REG_CODEC_TX_EAR_DIS(n)             BITFIELD_VAL(REG_CODEC_TX_EAR_DIS, n)
#define REG_CODEC_TX_EAR_DOUBLEBIAS         (1 << 2)
#define REG_CODEC_TX_EAR_ENBIAS             (1 << 3)
#define REG_CODEC_TX_EAR_GAIN_SHIFT         4
#define REG_CODEC_TX_EAR_GAIN_MASK          (0x3 << REG_CODEC_TX_EAR_GAIN_SHIFT)
#define REG_CODEC_TX_EAR_GAIN(n)            BITFIELD_VAL(REG_CODEC_TX_EAR_GAIN, n)
#define REG_CODEC_TX_EAR_IBSEL_SHIFT        6
#define REG_CODEC_TX_EAR_IBSEL_MASK         (0x3 << REG_CODEC_TX_EAR_IBSEL_SHIFT)
#define REG_CODEC_TX_EAR_IBSEL(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_IBSEL, n)
#define REG_CODEC_TX_EAR_LPBIAS             (1 << 8)
#define REG_CODEC_TX_EAR_OCEN               (1 << 9)
#define REG_CODEC_TX_EAR_SOFTSTART_SHIFT    10
#define REG_CODEC_TX_EAR_SOFTSTART_MASK     (0x3F << REG_CODEC_TX_EAR_SOFTSTART_SHIFT)
#define REG_CODEC_TX_EAR_SOFTSTART(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_SOFTSTART, n)

// REG74
#define REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT    0
#define REG_CODEC_TX_EAR_OUTPUTSEL_MASK     (0xF << REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT)
#define REG_CODEC_TX_EAR_OUTPUTSEL(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_OUTPUTSEL, n)
#define REG_CODEC_TX_LDOVSEL_SHIFT          4
#define REG_CODEC_TX_LDOVSEL_MASK           (0x1F << REG_CODEC_TX_LDOVSEL_SHIFT)
#define REG_CODEC_TX_LDOVSEL(n)             BITFIELD_VAL(REG_CODEC_TX_LDOVSEL, n)
#define REG_CODEC_TX_PA_ENBIAS              (1 << 9)
#define REG_CODEC_TX_PA_VCMLOWPOWER_SHIFT   10
#define REG_CODEC_TX_PA_VCMLOWPOWER_MASK    (0x3 << REG_CODEC_TX_PA_VCMLOWPOWER_SHIFT)
#define REG_CODEC_TX_PA_VCMLOWPOWER(n)      BITFIELD_VAL(REG_CODEC_TX_PA_VCMLOWPOWER, n)
#define REG_CODEC_TX_PA_LPBIAS              (1 << 12)
#define REG_CODEC_VCMBUF_VSEL_SHIFT         13
#define REG_CODEC_VCMBUF_VSEL_MASK          (0x7 << REG_CODEC_VCMBUF_VSEL_SHIFT)
#define REG_CODEC_VCMBUF_VSEL(n)            BITFIELD_VAL(REG_CODEC_VCMBUF_VSEL, n)

// REG75
#define REG_CODEC_VCMBUF_LOWP_SHIFT         0
#define REG_CODEC_VCMBUF_LOWP_MASK          (0x3 << REG_CODEC_VCMBUF_LOWP_SHIFT)
#define REG_CODEC_VCMBUF_LOWP(n)            BITFIELD_VAL(REG_CODEC_VCMBUF_LOWP, n)
#define REG_CODEC_EN_VCMLPF                 (1 << 2)
#define REG_CODEC_ADC_VREFP_CLOSE           (1 << 3)
#define REG_CODEC_ADC_VCM_RES_SHIFT         4
#define REG_CODEC_ADC_VCM_RES_MASK          (0x3 << REG_CODEC_ADC_VCM_RES_SHIFT)
#define REG_CODEC_ADC_VCM_RES(n)            BITFIELD_VAL(REG_CODEC_ADC_VCM_RES, n)
#define REG_CODEC_ADC_VREFP_RES_SHIFT       6
#define REG_CODEC_ADC_VREFP_RES_MASK        (0x3 << REG_CODEC_ADC_VREFP_RES_SHIFT)
#define REG_CODEC_ADC_VREFP_RES(n)          BITFIELD_VAL(REG_CODEC_ADC_VREFP_RES, n)
#define REG_CODEC_VCM_VSEL_SHIFT            8
#define REG_CODEC_VCM_VSEL_MASK             (0xF << REG_CODEC_VCM_VSEL_SHIFT)
#define REG_CODEC_VCM_VSEL(n)               BITFIELD_VAL(REG_CODEC_VCM_VSEL, n)
#define REG_AUDPLL_MDLL_CALEN               (1 << 12)
#define REG_AUDPLL_LDO_MDLL_SHIFT           13
#define REG_AUDPLL_LDO_MDLL_MASK            (0x7 << REG_AUDPLL_LDO_MDLL_SHIFT)
#define REG_AUDPLL_LDO_MDLL(n)              BITFIELD_VAL(REG_AUDPLL_LDO_MDLL, n)

// REG76
#define REG_AUDPLL_DITHER_BYPASS            (1 << 0)
#define REG_AUDPLL_INT_DEC_SEL_SHIFT        1
#define REG_AUDPLL_INT_DEC_SEL_MASK         (0x7 << REG_AUDPLL_INT_DEC_SEL_SHIFT)
#define REG_AUDPLL_INT_DEC_SEL(n)           BITFIELD_VAL(REG_AUDPLL_INT_DEC_SEL, n)
#define REG_AUDPLL_DIV_DR                   (1 << 4)
#define REG_AUDPLL_DIV_FRAC_SHIFT           5
#define REG_AUDPLL_DIV_FRAC_MASK            (0x3 << REG_AUDPLL_DIV_FRAC_SHIFT)
#define REG_AUDPLL_DIV_FRAC(n)              BITFIELD_VAL(REG_AUDPLL_DIV_FRAC, n)
#define REG_AUDPLL_DIV_INT_SHIFT            7
#define REG_AUDPLL_DIV_INT_MASK             (0x7F << REG_AUDPLL_DIV_INT_SHIFT)
#define REG_AUDPLL_DIV_INT(n)               BITFIELD_VAL(REG_AUDPLL_DIV_INT, n)
#define REG_AUDPLL_CPMODE                   (1 << 14)
#define REG_AUDPLL_LDO_BYPASS               (1 << 15)

// REG77
#define REG_AUDPLL_BW_SEL_SHIFT             0
#define REG_AUDPLL_BW_SEL_MASK              (0xF << REG_AUDPLL_BW_SEL_SHIFT)
#define REG_AUDPLL_BW_SEL(n)                BITFIELD_VAL(REG_AUDPLL_BW_SEL, n)
#define REG_AUDPLL_ICP_SHIFT                4
#define REG_AUDPLL_ICP_MASK                 (0xF << REG_AUDPLL_ICP_SHIFT)
#define REG_AUDPLL_ICP(n)                   BITFIELD_VAL(REG_AUDPLL_ICP, n)
#define REG_AUDPLL_DIV_DLY_CAP_SHIFT        8
#define REG_AUDPLL_DIV_DLY_CAP_MASK         (0xF << REG_AUDPLL_DIV_DLY_CAP_SHIFT)
#define REG_AUDPLL_DIV_DLY_CAP(n)           BITFIELD_VAL(REG_AUDPLL_DIV_DLY_CAP, n)
#define REG_AUDPLL_FBC_SEL_SHIFT            12
#define REG_AUDPLL_FBC_SEL_MASK             (0x7 << REG_AUDPLL_FBC_SEL_SHIFT)
#define REG_AUDPLL_FBC_SEL(n)               BITFIELD_VAL(REG_AUDPLL_FBC_SEL, n)
#define REG_AUDPLL_CLK_FBC_EDGE             (1 << 15)

// REG78
#define REG_AUDPLL_IOFST_SHIFT              0
#define REG_AUDPLL_IOFST_MASK               (0xF << REG_AUDPLL_IOFST_SHIFT)
#define REG_AUDPLL_IOFST(n)                 BITFIELD_VAL(REG_AUDPLL_IOFST, n)
#define REG_AUDPLL_SPEED_SHIFT              4
#define REG_AUDPLL_SPEED_MASK               (0x7 << REG_AUDPLL_SPEED_SHIFT)
#define REG_AUDPLL_SPEED(n)                 BITFIELD_VAL(REG_AUDPLL_SPEED, n)
#define REG_AUDPLL_VCAP_SHIFT               7
#define REG_AUDPLL_VCAP_MASK                (0x7 << REG_AUDPLL_VCAP_SHIFT)
#define REG_AUDPLL_VCAP(n)                  BITFIELD_VAL(REG_AUDPLL_VCAP, n)
#define REG_AUDPLL_LDO_DIG_SHIFT            10
#define REG_AUDPLL_LDO_DIG_MASK             (0x7 << REG_AUDPLL_LDO_DIG_SHIFT)
#define REG_AUDPLL_LDO_DIG(n)               BITFIELD_VAL(REG_AUDPLL_LDO_DIG, n)
#define REG_AUDPLL_LDO_FBRES_SHIFT          13
#define REG_AUDPLL_LDO_FBRES_MASK           (0x7 << REG_AUDPLL_LDO_FBRES_SHIFT)
#define REG_AUDPLL_LDO_FBRES(n)             BITFIELD_VAL(REG_AUDPLL_LDO_FBRES, n)

// REG79
#define REG_AUDPLL_PRESCALER_DEL_SEL_SHIFT  0
#define REG_AUDPLL_PRESCALER_DEL_SEL_MASK   (0xF << REG_AUDPLL_PRESCALER_DEL_SEL_SHIFT)
#define REG_AUDPLL_PRESCALER_DEL_SEL(n)     BITFIELD_VAL(REG_AUDPLL_PRESCALER_DEL_SEL, n)
#define REG_AUDPLL_LDO_PFD_SHIFT            4
#define REG_AUDPLL_LDO_PFD_MASK             (0x7 << REG_AUDPLL_LDO_PFD_SHIFT)
#define REG_AUDPLL_LDO_PFD(n)               BITFIELD_VAL(REG_AUDPLL_LDO_PFD, n)
#define REG_AUDPLL_LDO_VCO_SHIFT            7
#define REG_AUDPLL_LDO_VCO_MASK             (0x7 << REG_AUDPLL_LDO_VCO_SHIFT)
#define REG_AUDPLL_LDO_VCO(n)               BITFIELD_VAL(REG_AUDPLL_LDO_VCO, n)
#define REG_AUDPLL_LDO_CP_SHIFT             10
#define REG_AUDPLL_LDO_CP_MASK              (0x7 << REG_AUDPLL_LDO_CP_SHIFT)
#define REG_AUDPLL_LDO_CP(n)                BITFIELD_VAL(REG_AUDPLL_LDO_CP, n)
#define REG_AUDPLL_LCKDET_WIN_SHIFT         13
#define REG_AUDPLL_LCKDET_WIN_MASK          (0x7 << REG_AUDPLL_LCKDET_WIN_SHIFT)
#define REG_AUDPLL_LCKDET_WIN(n)            BITFIELD_VAL(REG_AUDPLL_LCKDET_WIN, n)

// REG7A
#define REG_AUDPLL_MDLL_LATCH_SHIFT         0
#define REG_AUDPLL_MDLL_LATCH_MASK          (0x7 << REG_AUDPLL_MDLL_LATCH_SHIFT)
#define REG_AUDPLL_MDLL_LATCH(n)            BITFIELD_VAL(REG_AUDPLL_MDLL_LATCH, n)
#define REG_AUDPLL_MDLL_ICP_SHIFT           3
#define REG_AUDPLL_MDLL_ICP_MASK            (0x3 << REG_AUDPLL_MDLL_ICP_SHIFT)
#define REG_AUDPLL_MDLL_ICP(n)              BITFIELD_VAL(REG_AUDPLL_MDLL_ICP, n)
#define REG_MDLL_CAL_MODE_SHIFT             5
#define REG_MDLL_CAL_MODE_MASK              (0x3 << REG_MDLL_CAL_MODE_SHIFT)
#define REG_MDLL_CAL_MODE(n)                BITFIELD_VAL(REG_MDLL_CAL_MODE, n)
#define REG_MDLL_BIT_SHIFT                  7
#define REG_MDLL_BIT_MASK                   (0x7 << REG_MDLL_BIT_SHIFT)
#define REG_MDLL_BIT(n)                     BITFIELD_VAL(REG_MDLL_BIT, n)
#define REG_MDLL_BIT_DR                     (1 << 10)
#define REG_MDLL_CAL_COUNT_ADJ_SHIFT        11
#define REG_MDLL_CAL_COUNT_ADJ_MASK         (0xF << REG_MDLL_CAL_COUNT_ADJ_SHIFT)
#define REG_MDLL_CAL_COUNT_ADJ(n)           BITFIELD_VAL(REG_MDLL_CAL_COUNT_ADJ, n)
#define REG_MDLL_CAL_START                  (1 << 15)

// REG7B
#define REG_CODEC_TX_PA_OFF_BITL_SHIFT      0
#define REG_CODEC_TX_PA_OFF_BITL_MASK       (0x3FF << REG_CODEC_TX_PA_OFF_BITL_SHIFT)
#define REG_CODEC_TX_PA_OFF_BITL(n)         BITFIELD_VAL(REG_CODEC_TX_PA_OFF_BITL, n)
#define REG_CODEC_TX_PA_LCAL                (1 << 10)
#define REG_CODEC_TX_PA_LSTB                (1 << 11)
#define REG_CODEC_TX_PA_RCAL                (1 << 12)
#define REG_CODEC_TEST_SEL_SHIFT            13
#define REG_CODEC_TEST_SEL_MASK             (0x7 << REG_CODEC_TEST_SEL_SHIFT)
#define REG_CODEC_TEST_SEL(n)               BITFIELD_VAL(REG_CODEC_TEST_SEL, n)

// REG7C
#define REG_CODEC_TX_PA_OFF_BITR_SHIFT      0
#define REG_CODEC_TX_PA_OFF_BITR_MASK       (0x3FF << REG_CODEC_TX_PA_OFF_BITR_SHIFT)
#define REG_CODEC_TX_PA_OFF_BITR(n)         BITFIELD_VAL(REG_CODEC_TX_PA_OFF_BITR, n)
#define RX_TIMER_RSTN_DLY_SHIFT             10
#define RX_TIMER_RSTN_DLY_MASK              (0x3F << RX_TIMER_RSTN_DLY_SHIFT)
#define RX_TIMER_RSTN_DLY(n)                BITFIELD_VAL(RX_TIMER_RSTN_DLY, n)

// REG7D
#define REG_CODEC_TX_EAR_OFF_BITR_SHIFT     0
#define REG_CODEC_TX_EAR_OFF_BITR_MASK      (0x3FF << REG_CODEC_TX_EAR_OFF_BITR_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITR, n)
#define REG_AUDPLL_TST_SEL_SHIFT            10
#define REG_AUDPLL_TST_SEL_MASK             (0x3 << REG_AUDPLL_TST_SEL_SHIFT)
#define REG_AUDPLL_TST_SEL(n)               BITFIELD_VAL(REG_AUDPLL_TST_SEL, n)
#define REG_AUDPLL_TST_EN                   (1 << 12)
#define REG_AUDPLL_TST_VREF_SHIFT           13
#define REG_AUDPLL_TST_VREF_MASK            (0x7 << REG_AUDPLL_TST_VREF_SHIFT)
#define REG_AUDPLL_TST_VREF(n)              BITFIELD_VAL(REG_AUDPLL_TST_VREF, n)

// REG7E
#define REG_CODEC_TX_EAR_OFF_BITL_SHIFT     0
#define REG_CODEC_TX_EAR_OFF_BITL_MASK      (0x3FF << REG_CODEC_TX_EAR_OFF_BITL_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL, n)
#define REG_CODEC_TX_EAR_LCAL               (1 << 10)
#define REG_CODEC_TX_EAR_LSTB               (1 << 11)
#define REG_CODEC_TX_EAR_RCAL               (1 << 12)
#define REG_AUDPLL_TSTBUF_PLUS              (1 << 13)
#define RESERVED_ANA_SHIFT                  14
#define RESERVED_ANA_MASK                   (0x3 << RESERVED_ANA_SHIFT)
#define RESERVED_ANA(n)                     BITFIELD_VAL(RESERVED_ANA, n)

// REG7F
#define ANA_AUDPLL_VCTRL_COMP               (1 << 0)
#define DIG_AUDPLL_DIV_FRAC_SHIFT           1
#define DIG_AUDPLL_DIV_FRAC_MASK            (0x3 << DIG_AUDPLL_DIV_FRAC_SHIFT)
#define DIG_AUDPLL_DIV_FRAC(n)              BITFIELD_VAL(DIG_AUDPLL_DIV_FRAC, n)
#define DIG_AUDPLL_DIV_INT_SHIFT            3
#define DIG_AUDPLL_DIV_INT_MASK             (0x7F << DIG_AUDPLL_DIV_INT_SHIFT)
#define DIG_AUDPLL_DIV_INT(n)               BITFIELD_VAL(DIG_AUDPLL_DIV_INT, n)
#define DIG_MDLL_CAL_DONE                   (1 << 10)
#define AUDPLL_LOCK                         (1 << 11)
#define CALOUT_LEAR                         (1 << 12)
#define CALOUT_LPA                          (1 << 13)
#define CALOUT_REAR                         (1 << 14)
#define CALOUT_RPA                          (1 << 15)

// REG165
#define REG_EXTPLL_SEL                      (1 << 5)

// REG_RF_C2
#define REG_XTAL_FCAP_NORMAL_SHIFT          8
#define REG_XTAL_FCAP_NORMAL_MASK           (0xFF << REG_XTAL_FCAP_NORMAL_SHIFT)
#define REG_XTAL_FCAP_NORMAL(n)             BITFIELD_VAL(REG_XTAL_FCAP_NORMAL, n)

// REG_RF_D5
#define REG_BBPLL_FREQ_34_32_SHIFT          13
#define REG_BBPLL_FREQ_34_32_MASK           (0x7 << REG_BBPLL_FREQ_34_32_SHIFT)
#define REG_BBPLL_FREQ_34_32(n)             BITFIELD_VAL(REG_BBPLL_FREQ_34_32, n)

// REG_RF_D9
#define REG_BBPLL_FREQ_EN                   (1 << 0)

// REG_RF_EE
#define REG_XTAL_FVAR_SHIFT                 8
#define REG_XTAL_FVAR_MASK                  (0xFF << REG_XTAL_FVAR_SHIFT)
#define REG_XTAL_FVAR(n)                    BITFIELD_VAL(REG_XTAL_FVAR, n)

// REG_RF_1EF
#define REG_BBPLL_DIVN_480M_SHIFT           4
#define REG_BBPLL_DIVN_480M_MASK            (0x7 << REG_BBPLL_DIVN_480M_SHIFT)
#define REG_BBPLL_DIVN_480M(n)              BITFIELD_VAL(REG_BBPLL_DIVN_480M, n)

// REG_RF_1FA
#define REG_BBPLL_CODEC_DIVN_SHIFT          0
#define REG_BBPLL_CODEC_DIVN_MASK           (0x3F << REG_BBPLL_CODEC_DIVN_SHIFT)
#define REG_BBPLL_CODEC_DIVN(n)             BITFIELD_VAL(REG_BBPLL_CODEC_DIVN, n)
#define REG_BBPLL_CODEC_CLK25M_EN           (1 << 6)
#define REG_BBPLL_CODECDIV_RST              (1 << 9)

enum ANA_REG_T {
    ANA_REG_61 = 0x61,
    ANA_REG_62,
    ANA_REG_63,
    ANA_REG_64,
    ANA_REG_65,
    ANA_REG_66,
    ANA_REG_67,
    ANA_REG_68,
    ANA_REG_69,
    ANA_REG_6A,
    ANA_REG_6B,
    ANA_REG_6C,
    ANA_REG_6D,
    ANA_REG_6E,
    ANA_REG_6F,
    ANA_REG_70,
    ANA_REG_71,
    ANA_REG_72,
    ANA_REG_73,
    ANA_REG_74,
    ANA_REG_75,
    ANA_REG_76,
    ANA_REG_77,
    ANA_REG_78,
    ANA_REG_79,
    ANA_REG_7A,
    ANA_REG_7B,
    ANA_REG_7C,
    ANA_REG_7D,
    ANA_REG_7E,
    ANA_REG_7F,

    ANA_REG_165 = 0x165,

    ANA_REG_168 = 0x168,
    ANA_REG_169 = 0x169,
    ANA_REG_16A = 0x16A,
    ANA_REG_16B = 0x16B,
    ANA_REG_16C = 0x16C,
    ANA_REG_16D = 0x16D,
    ANA_REG_16E = 0x16E,
    ANA_REG_16F = 0x16F,
    ANA_REG_170 = 0x170,
    ANA_REG_171 = 0x171,
    ANA_REG_172 = 0x172,
    ANA_REG_173 = 0x173,
    ANA_REG_174 = 0x174,
    ANA_REG_175 = 0x175,

    ANA_REG_RF_C2 = 0xC2,
    ANA_REG_RF_D5 = 0xD5,
    ANA_REG_RF_D6 = 0xD6,
    ANA_REG_RF_D7 = 0xD7,
    ANA_REG_RF_D9 = 0xD9,
    ANA_REG_RF_EE = 0xEE,
    ANA_REG_RF_1EF = 0x1EF,
    ANA_REG_RF_1FA = 0x1FA,
};

enum ANA_CODEC_USER_T {
    ANA_CODEC_USER_DAC,
    ANA_CODEC_USER_ADC,

    ANA_CODEC_USER_CODEC,
    ANA_CODEC_USER_MICKEY,

    ANA_CODEC_USER_ANC_FF,
    ANA_CODEC_USER_ANC_FB,

    ANA_CODEC_USER_ANC_INIT,

    ANA_CODEC_USER_QTY
};

typedef uint8_t CODEC_USER_MAP;
STATIC_ASSERT(sizeof(CODEC_USER_MAP) * 8 >= ANA_CODEC_USER_QTY, "Invalid codec user map type size");

struct ANALOG_PLL_CFG_T {
    uint32_t freq;
    uint8_t div;
    uint16_t val[3];
    int32_t total_delta;
};

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

#ifdef AUDIO_OUTPUT_DIFF
static OPT_TYPE bool audio_output_diff = true;
static OPT_TYPE uint8_t tx_pa_gain_0db = TX_PA_GAIN_0DB_DIFF;
#else
static OPT_TYPE bool audio_output_diff = false;
static OPT_TYPE uint8_t tx_pa_gain_0db = TX_PA_GAIN_0DB_SE;
#endif
static OPT_TYPE uint16_t vcodec_mv =
#ifdef VCODEC_1P5V
    1500;
#elif defined(VCODEC_1P6V)
    1600;
#elif defined(VCODEC_2P5V)
    2500;
#elif defined(VCODEC_3P5V)
    3500;
#else // VCODEC_1P8V
    1800;
#endif
static OPT_TYPE bool low_power_adc =
#ifdef LOW_POWER_ADC
    true;
#else
    false;
#endif

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static CODEC_USER_MAP adc_map[MAX_ANA_MIC_CH_NUM];
static CODEC_USER_MAP vmic_1_map;
static CODEC_USER_MAP vmic_2_map;
static CODEC_USER_MAP rx_cfg_map;
static CODEC_USER_MAP vcodec_map;

static uint8_t ana_aud_pll_map;
STATIC_ASSERT(ANA_AUD_PLL_USER_QTY <= sizeof(ana_aud_pll_map) * 8, "Too many ANA AUD PLL users");

static uint8_t dac_gain;

#ifdef ANC_APP
static int8_t anc_dac_gain_offset;

#ifndef DYN_ADC_GAIN
#define DYN_ADC_GAIN
#endif
#endif

#ifdef DYN_ADC_GAIN
static int8_t adc_gain_offset[MAX_ANA_MIC_CH_NUM];
#endif

static const uint8_t pga2_qdb[] = { 0, 1.5*4, 3*4, 4.5*4, 6*4, 12*4, 18*4, 24*4, };
//static const uint8_t pga1_qdb[] = { -1, 0, 6*4, 12*4, 18*4, 24*4, 30*4, 36*4, };

static const uint8_t tgt_adc_qdb[MAX_ANA_MIC_CH_NUM] = {
    ANALOG_ADC_A_GAIN_DB * 4, ANALOG_ADC_B_GAIN_DB * 4,
    ANALOG_ADC_C_GAIN_DB * 4, ANALOG_ADC_D_GAIN_DB * 4,
    ANALOG_ADC_E_GAIN_DB * 4,
};

// Max allowed total delta is +/-10M offset (11600ppm for 860M, or 10400ppm for 960M)
#define MAX_ALLOWED_TOTAL_DELTA             ((uint64_t)(1 << 28) * 10) / 26

static struct ANALOG_PLL_CFG_T ana_pll_cfg[2];
static int pll_cfg_idx;

#ifdef __AUDIO_RESAMPLE__
static ANA_AUD_PLL_TUNE_HANDLER pll_tune_hdlr;

static uint16_t xtal_fcap;
static uint16_t xtal_fvar;
static uint16_t xtal_tune_factor;
static int16_t prev_diff;
static int prev_ppb;

static void analog_init_xtal_fcap(void)
{
    xtal_fcap = DEFAULT_XTAL_FCAP;
    xtal_fvar = DEFAULT_XTAL_FVAR;
    xtal_tune_factor = DEFAULT_XTAL_TUNE_FACTOR;

    analog_write(ANA_REG_RF_C2, xtal_fcap);
    analog_write(ANA_REG_RF_EE, xtal_fvar);
}

static void analog_set_xtal_fcap(uint32_t fcap, uint32_t fvar)
{
    TRACE("%s: Set xtal fcap: fcap=%u fvar=%u", __func__, fcap, fvar);

    xtal_fcap = SET_BITFIELD(xtal_fcap, REG_XTAL_FCAP_NORMAL, fcap);
    xtal_fvar = SET_BITFIELD(xtal_fvar, REG_XTAL_FVAR, fvar);

    analog_write(ANA_REG_RF_C2, xtal_fcap);
    analog_write(ANA_REG_RF_EE, xtal_fvar);
}

static void analog_get_xtal_fcap(uint32_t *fcap, uint32_t *fvar)
{
    *fcap = GET_BITFIELD(xtal_fcap, REG_XTAL_FCAP_NORMAL);
    *fvar = GET_BITFIELD(xtal_fvar, REG_XTAL_FVAR);
}

static void analog_aud_xtal_set_val(uint32_t val)
{
    uint32_t fcap, fvar;

    fcap = val >> 3;
    fvar = (val & 0x7) << 5;
    analog_set_xtal_fcap(fcap, fvar);
}

static uint32_t analog_aud_xtal_get_val(void)
{
    uint32_t fcap, fvar;

    analog_get_xtal_fcap(&fcap, &fvar);
    return (fcap << 3) + ((fvar >> 5) & 0x7);
}

void analog_aud_xtal_tune(uint32_t rate, int32_t sample, uint32_t ms)
{
    int ppb;
    int offset;
    int64_t div;
    int64_t round;
    uint32_t old_val, new_val;
    int diff = 0;
    uint16_t step;

    div = rate * ms;
    round = div / 2;
    if (sample < 0) {
        round = -round;
    }

    ppb = ((int64_t)sample * 1000 * 1000 * 1000 * 1000 + round) / div;

    TRACE("\n%s: ppb=%d\n", __FUNCTION__, (int)ppb);

    if (ppb == 0) {
        goto _exit;
    }

    step = ABS(prev_diff);
    if (step > XTAL_TUNE_FACTOR_UPDATE_THRESH) {
        int new_factor;
        uint16_t old_factor;

        new_factor = ((ppb - prev_ppb) * 8  - (prev_diff / 2))/ -prev_diff;
        TRACE("Prepare xtal factor: ppb=%d prev_ppb=%d prev_diff=%d", (int)ppb, (int)prev_ppb, prev_diff);
        if (new_factor <= 0) {
            TRACE("WARNING: Bad xtal factor: %d", new_factor);
            goto _exit;
        }

        old_factor = xtal_tune_factor;

        if (step > XTAL_TUNE_FACTOR_UPDATE_STEP) {
            step = XTAL_TUNE_FACTOR_UPDATE_STEP;
        }
        xtal_tune_factor = (old_factor * (XTAL_TUNE_FACTOR_UPDATE_STEP - step) +
            new_factor * step + XTAL_TUNE_FACTOR_UPDATE_STEP / 2) / XTAL_TUNE_FACTOR_UPDATE_STEP;

        TRACE("Update xtal factor: old=%u new=%u final=%u", old_factor, new_factor, xtal_tune_factor);
    }

    round = xtal_tune_factor / 2;
    if (ppb < 0) {
        round = -round;
    }

    offset = (ppb * 8 + round) / xtal_tune_factor;
    if (offset == 0) {
        goto _exit;
    }

    old_val = analog_aud_xtal_get_val();
    if (offset < 0) {
        if (old_val > -offset) {
            new_val = old_val + offset;
        } else {
            new_val = 0;
        }
    } else {
        if (old_val + offset < MAX_XTAL_VALUE) {
            new_val = old_val + offset;
        } else {
            new_val = MAX_XTAL_VALUE;
        }
    }

    if (new_val != old_val) {
        analog_aud_xtal_set_val(new_val);
        diff = new_val - old_val;
    }

_exit:
    prev_diff = diff;
    prev_ppb = ppb;
}
#endif

#ifdef USB_HIGH_SPEED
static void analog_aud_freq_reset_codec_div(void)
{
    uint16_t val;

    // 1) Codec div must be reset if BBPLL is shutdown before
    // 2) Codec div can be reset only when BBPLL is active (there is a clock)

    analog_read(ANA_REG_RF_1FA, &val);
    val |= REG_BBPLL_CODECDIV_RST;
    analog_write(ANA_REG_RF_1FA, val);
    val &= ~REG_BBPLL_CODECDIV_RST;
    analog_write(ANA_REG_RF_1FA, val);
}
#endif

void analog_aud_freq_pll_config(uint32_t freq, uint32_t div)
{
    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)

    uint64_t PLL_cfg_val;
    uint16_t val;
    int i, j;

    if (pll_cfg_idx < ARRAY_SIZE(ana_pll_cfg) &&
            ana_pll_cfg[pll_cfg_idx].freq == freq &&
            ana_pll_cfg[pll_cfg_idx].div == div) {
        return;
    }

    j = ARRAY_SIZE(ana_pll_cfg);
    for (i = 0; i < ARRAY_SIZE(ana_pll_cfg); i++) {
        if (ana_pll_cfg[i].freq == freq && ana_pll_cfg[i].div == div) {
            break;
        }
        if (j == ARRAY_SIZE(ana_pll_cfg) && ana_pll_cfg[i].freq == 0) {
            j = i;
        }
    }

    if (i < ARRAY_SIZE(ana_pll_cfg)) {
        pll_cfg_idx = i;
    } else {
        if (j < ARRAY_SIZE(ana_pll_cfg)) {
            pll_cfg_idx = j;
        } else {
            j = 0;
        }

        ana_pll_cfg[pll_cfg_idx].freq = freq;
        ana_pll_cfg[pll_cfg_idx].div = div;
        ana_pll_cfg[pll_cfg_idx].total_delta = 0;

        PLL_cfg_val = ((uint64_t)(1 << 28) * freq * div + 26000000 / 2) / 26000000;
        ana_pll_cfg[pll_cfg_idx].val[0] = PLL_cfg_val & 0xFFFF;
        ana_pll_cfg[pll_cfg_idx].val[1] = (PLL_cfg_val >> 16) & 0xFFFF;
        ana_pll_cfg[pll_cfg_idx].val[2] = (PLL_cfg_val >> 32) & 0xFFFF;
    }

#ifdef USB_HIGH_SPEED
    analog_read(ANA_REG_RF_1FA, &val);
    val = SET_BITFIELD(val, REG_BBPLL_CODEC_DIVN, div) | REG_BBPLL_CODEC_CLK25M_EN;
    analog_write(ANA_REG_RF_1FA, val);
    analog_aud_freq_reset_codec_div();

    analog_write(ANA_REG_RF_D7, ana_pll_cfg[pll_cfg_idx].val[0]);
    analog_write(ANA_REG_RF_D6, ana_pll_cfg[pll_cfg_idx].val[1]);
    analog_read(ANA_REG_RF_D5, &val);
    val = SET_BITFIELD(val, REG_BBPLL_FREQ_34_32, ana_pll_cfg[pll_cfg_idx].val[2]);
    analog_write(ANA_REG_RF_D5, val);

    analog_read(ANA_REG_RF_D9, &val);
    val &= ~REG_BBPLL_FREQ_EN;
    analog_write(ANA_REG_RF_D9, val);
    val |= REG_BBPLL_FREQ_EN;
    analog_write(ANA_REG_RF_D9, val);
#else
    analog_read(ANA_REG_65, &val);
    val = SET_BITFIELD(val, REG_AUDPLL_POST_DIVN, div);
    analog_write(ANA_REG_65, val);

    analog_write(ANA_REG_67, ana_pll_cfg[pll_cfg_idx].val[0]);
    analog_write(ANA_REG_68, ana_pll_cfg[pll_cfg_idx].val[1]);
    analog_read(ANA_REG_69, &val);
    val = SET_BITFIELD(val, REG_AUDPLL_FREQ_34_32, ana_pll_cfg[pll_cfg_idx].val[2]) | REG_AUDPLL_FREQ_EN;
    analog_write(ANA_REG_69, val);
#endif
}

void analog_aud_pll_reset_cur_config(void)
{
    uint32_t freq, div;

    if (pll_cfg_idx < ARRAY_SIZE(ana_pll_cfg) && ana_pll_cfg[pll_cfg_idx].freq != 0) {
        freq = ana_pll_cfg[pll_cfg_idx].freq;
        div = ana_pll_cfg[pll_cfg_idx].div;
        ana_pll_cfg[pll_cfg_idx].freq = 0;
        ana_pll_cfg[pll_cfg_idx].div = 0;
        analog_aud_freq_pll_config(freq, div);
    }
}

void analog_aud_set_pll_tune_handler(ANA_AUD_PLL_TUNE_HANDLER hdlr)
{
#ifdef __AUDIO_RESAMPLE__
    pll_tune_hdlr = hdlr;
#endif
}

void analog_aud_pll_tune(uint32_t rate, int32_t sample, uint32_t ms)
{
#ifdef __AUDIO_RESAMPLE__
    if (hal_cmu_get_audio_resample_status()) {
        if (pll_tune_hdlr) {
            pll_tune_hdlr(rate, sample, ms);
        }
        return;
    }
#endif

    // CODEC_FREQ is likely 24.576M (48K series) or 22.5792M (44.1K series)
    // PLL_nominal = CODEC_FREQ * CODEC_DIV
    // PLL_cfg_val = ((CODEC_FREQ * CODEC_DIV) / 26M) * (1 << 28)
    // Delta = ((SampleDiff / Fs) / TimeDiff) * PLL_cfg_val

    int64_t delta, total_delta, new_pll;
    uint16_t high, low;
    uint16_t new_high, new_low;
    uint16_t val, bit34_32, new_bit34_32;

    if (pll_cfg_idx >= ARRAY_SIZE(ana_pll_cfg) ||
            ana_pll_cfg[pll_cfg_idx].freq == 0) {
        TRACE("%s: WARNING: aud pll config cache invalid. Skip tuning", __FUNCTION__);
        return;
    }

    // Avoid uint16_t->int (integer promotions in C)
    delta = (int64_t)(((uint64_t)ana_pll_cfg[pll_cfg_idx].val[2] << 32) |
        ((uint32_t)ana_pll_cfg[pll_cfg_idx].val[1] << 16) |
        (uint32_t)ana_pll_cfg[pll_cfg_idx].val[0]) * sample * 1000 / ms / rate;

    total_delta = delta + ana_pll_cfg[pll_cfg_idx].total_delta;

    if (ABS(total_delta) > MAX_ALLOWED_TOTAL_DELTA) {
        TRACE("\n------\nWARNING: TUNE: total_delta=%d too large, delta=%d is cut\n------\n", (int)total_delta, (int)delta);
        if (total_delta > 0) {
            total_delta = MAX_ALLOWED_TOTAL_DELTA;
        } else {
            total_delta = -MAX_ALLOWED_TOTAL_DELTA;
        }
        delta = total_delta - ana_pll_cfg[pll_cfg_idx].total_delta;
    }
    ana_pll_cfg[pll_cfg_idx].total_delta = total_delta;

    TRACE("%s: rate=%u sample=%d ms=%u delta=%d", __FUNCTION__, rate, sample, ms, (int)delta);

    if (delta) {
        low = ana_pll_cfg[pll_cfg_idx].val[0];
        high = ana_pll_cfg[pll_cfg_idx].val[1];
        bit34_32 = ana_pll_cfg[pll_cfg_idx].val[2];
        // Avoid uint16_t->int (integer promotions in C)
        new_pll = (int64_t)(((uint64_t)bit34_32 << 32) | ((uint32_t)high << 16) | (uint32_t)low) - delta;

        new_low = new_pll & 0xFFFF;
        new_high = (new_pll >> 16) & 0xFFFF;
        new_bit34_32 = (new_pll >> 32) & 0xFFFF;

#ifdef USB_HIGH_SPEED
        if (new_low != low) {
            ana_pll_cfg[pll_cfg_idx].val[0] = new_low;
            analog_write(ANA_REG_RF_D7, new_low);
        }

        if (new_high != high) {
            ana_pll_cfg[pll_cfg_idx].val[1] = new_high;
            analog_write(ANA_REG_RF_D6, new_high);
        }

        if (bit34_32 != new_bit34_32) {
            ana_pll_cfg[pll_cfg_idx].val[2] = new_bit34_32;
            analog_read(ANA_REG_RF_D5, &val);
            val = SET_BITFIELD(val, REG_BBPLL_FREQ_34_32, new_bit34_32);
            analog_write(ANA_REG_RF_D5, val);
        }

        analog_read(ANA_REG_RF_D9, &val);
        val &= ~REG_BBPLL_FREQ_EN;
        analog_write(ANA_REG_RF_D9, val);
        val |= REG_BBPLL_FREQ_EN;
        analog_write(ANA_REG_RF_D9, val);
#else
        if (new_low != low) {
            ana_pll_cfg[pll_cfg_idx].val[0] = new_low;
            analog_write(ANA_REG_67, new_low);
        }

        if (new_high != high) {
            ana_pll_cfg[pll_cfg_idx].val[1] = new_high;
            analog_write(ANA_REG_68, new_high);
        }

        if (bit34_32 != new_bit34_32) {
            ana_pll_cfg[pll_cfg_idx].val[2] = new_bit34_32;
        }

        analog_read(ANA_REG_69, &val);
        val = SET_BITFIELD(val, REG_AUDPLL_FREQ_34_32, new_bit34_32) | REG_AUDPLL_FREQ_EN;
        analog_write(ANA_REG_69, val);
#endif
    }
}

void analog_aud_pll_set_dig_div(uint32_t div)
{
    uint16_t val;

#ifdef USB_HIGH_SPEED
    analog_read(ANA_REG_RF_1EF, &val);
    val = SET_BITFIELD(val, REG_BBPLL_DIVN_480M, div);
    analog_write(ANA_REG_RF_1EF, val);
#else
    analog_read(ANA_REG_65, &val);
    val = SET_BITFIELD(val, REG_AUDPLL_DIVN_480M, div);
    analog_write(ANA_REG_65, val);
#endif
}

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    enum HAL_CMU_PLL_T pll;

    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {
        uint16_t val;

        analog_read(ANA_REG_66, &val);
        val |= REG_CRYSTAL_SEL_LV | REG_PU_OSC;
        // Invert tx clock to avoid noise when vcodec=1.8V and vcore=1.1V
        val |= CFG_TX_CLK_INV;
        analog_write(ANA_REG_66, val);
        return;
    }
#endif

#ifdef USB_HIGH_SPEED
    if (user == ANA_AUD_PLL_USER_CODEC) {
        uint16_t val;

        analog_read(ANA_REG_165, &val);
        val |= REG_EXTPLL_SEL;
        analog_write(ANA_REG_165, val);
    }

    pll = HAL_CMU_PLL_USB;
#else
    pll = HAL_CMU_PLL_AUD;
#endif

    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_enable(pll, HAL_CMU_PLL_USER_AUD);
    }
    ana_aud_pll_map |= (1 << user);

#ifdef USB_HIGH_SPEED
    if (user == ANA_AUD_PLL_USER_CODEC) {
        analog_aud_freq_reset_codec_div();
    }
#endif
}

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    enum HAL_CMU_PLL_T pll;

    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {
        uint16_t val;

        analog_read(ANA_REG_66, &val);
        val &= ~(REG_CRYSTAL_SEL_LV | REG_PU_OSC | CFG_TX_CLK_INV);
        analog_write(ANA_REG_66, val);
        return;
    }
#endif

#ifdef USB_HIGH_SPEED
    if (user == ANA_AUD_PLL_USER_CODEC) {
        uint16_t val;

        analog_read(ANA_REG_165, &val);
        val &= ~REG_EXTPLL_SEL;
        analog_write(ANA_REG_165, val);
    }

    pll = HAL_CMU_PLL_USB;
#else
    pll = HAL_CMU_PLL_AUD;
#endif

    ana_aud_pll_map &= ~(1 << user);
    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_disable(pll, HAL_CMU_PLL_USER_AUD);
    }
}

static void analog_aud_enable_dac(uint32_t dac)
{
    uint16_t val;

    analog_read(ANA_REG_61, &val);
    val &= ~(REG_CODEC_TX_EN_LDAC | REG_CODEC_TX_EN_RDAC);
    if (dac & AUD_CHANNEL_MAP_CH0) {
        val |= REG_CODEC_TX_EN_LDAC;
    }
    if (dac & AUD_CHANNEL_MAP_CH1) {
        val |= REG_CODEC_TX_EN_RDAC;
    }
    analog_write(ANA_REG_61, val);
}

static void analog_aud_enable_dac_pa(uint32_t dac)
{
    uint16_t val;

    if (!audio_output_diff) {
        if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
            //hal_cmu_classg_clock_enable();
        }
    }

    analog_read(ANA_REG_61, &val);
    if (audio_output_diff) {
        val &= ~(REG_CODEC_TX_EN_LEAR | REG_CODEC_TX_EN_REAR);
        if (dac & AUD_CHANNEL_MAP_CH0) {
            val |= REG_CODEC_TX_EN_LEAR;
        }
        if (dac & AUD_CHANNEL_MAP_CH1) {
            val |= REG_CODEC_TX_EN_REAR;
        }
    } else {
        val &= ~(REG_CODEC_TX_EN_LPA | REG_CODEC_TX_EN_RPA);
        if (dac & AUD_CHANNEL_MAP_CH0) {
            val |= REG_CODEC_TX_EN_LPA;
        }
        if (dac & AUD_CHANNEL_MAP_CH1) {
            val |= REG_CODEC_TX_EN_RPA;
        }
    }
    analog_write(ANA_REG_61, val);

    if (!audio_output_diff) {
        if (dac == 0) {
            //hal_cmu_classg_clock_disable();
        }
    }
}

static void analog_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    int i;
    uint16_t val_61, val_62, val_63;
    CODEC_USER_MAP old_map;
    bool set;
    bool pga1_en;
    uint8_t chansel1, chansel2;

    pga1_en = false;
    if (vcodec_mv < 2500) {
        if (hal_get_chip_metal_id() < HAL_CHIP_METAL_ID_1) {
            pga1_en = true;
        }
    }

    if (pga1_en) {
        chansel1 = 1;
        chansel2 = 1;
    } else {
        chansel1 = 0;
        chansel2 = 2;
    }

    ANALOG_TRACE("[%s] user=%d ch_map=0x%x", __func__, user, ch_map);

    analog_read(ANA_REG_61, &val_61);
    analog_read(ANA_REG_62, &val_62);
    analog_read(ANA_REG_63, &val_63);

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            set = false;
            if (en) {
                if (adc_map[i] == 0) {
                    set = true;
                }
                adc_map[i] |= (1 << user);
            } else {
                old_map = adc_map[i];
                adc_map[i] &= ~(1 << user);
                if (old_map != 0 && adc_map[i] == 0) {
                    set = true;
                }
            }
            if (set) {
                if (adc_map[i]) {
                    val_61 |= (REG_CODEC_RX_EN_ADCA << i);
                    if (pga1_en) {
                        val_62 |= (REG_CODEC_PGA1A_EN << 2 * i);
                    }
                    val_62 |= (REG_CODEC_PGA2A_EN << 2 * i);
                    if (i < 4) {
                        val_63 = (val_63 & ~((REG_CODEC_PGA1A_CHANSEL_MASK | REG_CODEC_PGA2A_CHANSEL_MASK) << 4 * i)) |
                            ((REG_CODEC_PGA1A_CHANSEL(chansel1) | REG_CODEC_PGA2A_CHANSEL(chansel2)) << 4 * i);
                    } else {
                        val_62 = (val_62 & ~(REG_CODEC_PGA1E_CHANSEL_MASK | REG_CODEC_PGA2E_CHANSEL_MASK)) |
                            (REG_CODEC_PGA1E_CHANSEL(chansel1) | REG_CODEC_PGA2E_CHANSEL(chansel2));
                    }
                } else {
                    val_61 &= ~(REG_CODEC_RX_EN_ADCA << i);
                    val_62 &= ~((REG_CODEC_PGA1A_EN | REG_CODEC_PGA2A_EN) << 2 * i);
                    if (i < 4) {
                        val_63 = (val_63 & ~((REG_CODEC_PGA1A_CHANSEL_MASK | REG_CODEC_PGA2A_CHANSEL_MASK) << 4 * i)) |
                            ((REG_CODEC_PGA1A_CHANSEL(0) | REG_CODEC_PGA2A_CHANSEL(0)) << 4 * i);
                    } else {
                        val_62 = (val_62 & ~(REG_CODEC_PGA1E_CHANSEL_MASK | REG_CODEC_PGA2E_CHANSEL_MASK)) |
                            (REG_CODEC_PGA1E_CHANSEL(0) | REG_CODEC_PGA2E_CHANSEL(0));
                    }
                }
            }
        }
    }

    analog_write(ANA_REG_61, val_61);
    analog_write(ANA_REG_62, val_62);
    analog_write(ANA_REG_63, val_63);
}

static uint32_t qdb_to_adc_gain(uint32_t qdb)
{
    int i;
    uint8_t cnt;
    const uint8_t *list;

    list = pga2_qdb;
    cnt = ARRAY_SIZE(pga2_qdb);

    for (i = 0; i < cnt - 1; i++) {
        if (qdb < list[i + 1]) {
            break;
        }
    }

    if (i == cnt - 1) {
        return i;
    }
    else if (qdb * 2 < list[i] + list[i + 1]) {
        return i;
    } else {
        return i + 1;
    }
}

static uint16_t get_chan_adc_gain(uint32_t i)
{
    uint16_t gain;

    gain = tgt_adc_qdb[i];

#ifdef DYN_ADC_GAIN
    if (adc_gain_offset[i] < 0 && -adc_gain_offset[i] > gain) {
        gain = 0;
    } else {
        gain += adc_gain_offset[i];
    }
#endif

    return gain;
}

static void analog_aud_set_adc_gain(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map)
{
    int i;
    uint16_t gain;
    uint16_t val;

    analog_read(ANA_REG_71, &val);

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (ch_map & (AUD_CHANNEL_MAP_CH0 << i)) {
            if (0) {
#ifdef ANC_APP
#ifdef ANC_FF_ENABLED
            } else if ((ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R) & (AUD_CHANNEL_MAP_CH0 << i)) {
                gain = get_chan_adc_gain(i);
#endif
#ifdef ANC_FB_ENABLED
            } else if ((ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R) & (AUD_CHANNEL_MAP_CH0 << i)) {
                gain = get_chan_adc_gain(i);
#endif
#endif
            } else if (input_path == AUD_INPUT_PATH_LINEIN) {
                gain = LINEIN_ADC_GAIN_DB * 4;
            } else {
                gain = get_chan_adc_gain(i);
            }
            gain = qdb_to_adc_gain(gain);
            val = (val & ~(REG_PGA2A_GAIN_MASK << 3 * i)) | (REG_PGA2A_GAIN(gain) << 3 * i);
        }
    }

    analog_write(ANA_REG_71, val);
}

static uint32_t POSSIBLY_UNUSED get_msb_pos(uint32_t val)
{
    uint32_t lead_zero;

    lead_zero = __CLZ(val);
    return (lead_zero >= 32) ? 32 : 31 - lead_zero;
}

#ifdef ANC_APP
void analog_aud_apply_anc_adc_gain_offset(enum ANC_TYPE_T type, int16_t offset_l, int16_t offset_r)
{
    enum AUD_CHANNEL_MAP_T ch_map;
    uint32_t l, r;

    ch_map = 0;
    l = r = 32;
    if (type == ANC_FEEDFORWARD) {
#ifdef ANC_FF_ENABLED
        ch_map = ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R;
        l = get_msb_pos(ANC_FF_MIC_CH_L);
        r = get_msb_pos(ANC_FF_MIC_CH_R);
#endif
    } else {
#ifdef ANC_FB_ENABLED
        ch_map = ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R;
        l = get_msb_pos(ANC_FB_MIC_CH_L);
        r = get_msb_pos(ANC_FB_MIC_CH_R);
#endif
    }

    if ((l >= MAX_ANA_MIC_CH_NUM || adc_gain_offset[l] == offset_l) &&
            (r >= MAX_ANA_MIC_CH_NUM || adc_gain_offset[r] == offset_r)) {
        return;
    }

    TRACE("ana: apply anc adc gain offset: type=%d offset=%d/%d", type, offset_l, offset_r);

    if (l < MAX_ANA_MIC_CH_NUM) {
        adc_gain_offset[l] = offset_l;
    }
    if (r < MAX_ANA_MIC_CH_NUM) {
        adc_gain_offset[r] = offset_r;
    }

    analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
}
#endif

#ifdef DYN_ADC_GAIN
void analog_aud_apply_adc_gain_offset(enum AUD_CHANNEL_MAP_T ch_map, int16_t offset)
{
    enum AUD_CHANNEL_MAP_T map;
    int i;

#ifdef ANC_APP
#ifdef ANC_FF_ENABLED
    ch_map &= ~(ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R);
#endif
#ifdef ANC_FB_ENABLED
    ch_map &= ~(ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R);
#endif
#endif

    if (ch_map) {
        map = ch_map;

        while (map) {
            i = get_msb_pos(map);
            map &= ~(1 << i);
            if (i < MAX_ANA_MIC_CH_NUM) {
                adc_gain_offset[i] = offset;
            }
        }

        TRACE("ana: apply adc gain offset: ch_map=0x%X offset=%d", ch_map, offset);

        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
    }
}
#endif

static void analog_codec_tx_pa_gain_sel(uint32_t v)
{
    uint16_t val;

    if (audio_output_diff) {
        analog_read(ANA_REG_73, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_GAIN, v);
        analog_write(ANA_REG_73, val);
    } else {
        analog_read(ANA_REG_64, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_PA_GAIN, v);
        analog_write(ANA_REG_64, val);
    }
}

void analog_aud_set_dac_gain(int32_t v)
{
    if (v < 0) {
        v = DEFAULT_TX_PA_GAIN;
    }

    dac_gain = v;

#ifdef ANC_APP
    if (v) {
        if (anc_dac_gain_offset < 0 && -anc_dac_gain_offset > v) {
            v = 0;
        } else {
            v += anc_dac_gain_offset;
            if (v > (REG_CODEC_TX_EAR_GAIN_MASK >> REG_CODEC_TX_EAR_GAIN_SHIFT)) {
                v = (REG_CODEC_TX_EAR_GAIN_MASK >> REG_CODEC_TX_EAR_GAIN_SHIFT);
            }
        }
    }

    //TRACE("ana: set dac gain: %u / %u (offset=%d)", dac_gain, v, anc_dac_gain_offset);
#endif

    analog_codec_tx_pa_gain_sel(v);
}

uint32_t analog_codec_get_dac_gain(void)
{
    uint16_t val;

    if (audio_output_diff) {
        analog_read(ANA_REG_73, &val);
        val = GET_BITFIELD(val, REG_CODEC_TX_EAR_GAIN);
    } else {
        analog_read(ANA_REG_64, &val);
        val = GET_BITFIELD(val, REG_CODEC_TX_PA_GAIN);
    }

    return val;
}

uint32_t analog_codec_dac_gain_to_db(int32_t gain)
{
    if (gain < 0) {
        gain = DEFAULT_TX_PA_GAIN;
    }
    return (DAC_GAIN_TO_QDB(gain) + 2) / 4;
}

int32_t analog_codec_dac_max_attn_db(void)
{
    return -(DAC_GAIN_TO_QDB(tx_pa_gain_0db) + 2) / 4;
}

#ifdef ANC_APP
void analog_aud_apply_anc_dac_gain_offset(int16_t offset)
{
    int16_t new_offset;

    new_offset = QDB_TO_DAC_GAIN(offset);

    if (anc_dac_gain_offset == new_offset) {
        return;
    }

    TRACE("ana: apply anc dac gain offset: %d", new_offset);

    anc_dac_gain_offset = new_offset;
    analog_aud_set_dac_gain(dac_gain);

    // Compensate the offset in digital DAC
    hal_codec_apply_dac_gain_offset(HAL_CODEC_ID_0, -offset);
}
#endif

static int POSSIBLY_UNUSED dc_calib_checksum_valid(uint16_t efuse)
{
    int i;
    uint32_t cnt = 0;

    for (i = 0; i < 12; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & 0xF) == ((efuse >> 12) & 0xF));
}

void analog_aud_get_dc_calib_value(int16_t *dc_l, int16_t *dc_r)
{
    static const uint8_t EFUSE_PAGE_DIFF[2] = { PMU_EFUSE_PAGE_DCCALIB2_L, PMU_EFUSE_PAGE_DCCALIB2_R, };
    static const uint8_t EFUSE_PAGE_SE[2] = { PMU_EFUSE_PAGE_DCCALIB_L, PMU_EFUSE_PAGE_DCCALIB_R, };
    const uint8_t *page;

    if (audio_output_diff) {
        page = EFUSE_PAGE_DIFF;
    } else {
        page = EFUSE_PAGE_SE;
    }

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    static const uint16_t DC_VALUE_MASK = 0x03FF;
#else
    //static const uint16_t DC_VALUE_MASK = 0x07FF;
    //static const uint16_t DC_VALUE_SIGN = 0x0800;
    union DC_EFUSE_T {
        struct DC_VALUE_T {
            int16_t dc          : 11;
            uint16_t reserved   : 1;
            uint16_t checksum   : 4;
        } val;
        uint16_t reg;
    };
    union DC_EFUSE_T dc;
#endif
    uint16_t efuse;

    pmu_get_efuse(page[0], &efuse);
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    *dc_l = efuse & DC_VALUE_MASK;
#else
    dc.reg = efuse;
    *dc_l = dc.val.dc;
#endif
    if (dc_calib_checksum_valid(efuse)) {
        TRACE("Dc calib L OK: 0x%04x", efuse);
    } else {
        TRACE("Warning: Bad dc calib efuse L: 0x%04x", efuse);
        *dc_l = 0;
    }

    pmu_get_efuse(page[1], &efuse);
#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    *dc_r = efuse & DC_VALUE_MASK;
#else
    dc.reg = efuse;
    *dc_r = dc.val.dc;
#endif
    if (dc_calib_checksum_valid(efuse)) {
        TRACE("Dc calib R OK: 0x%04x", efuse);
    } else {
        TRACE("Warning: Bad dc calib efuse R: 0x%04x", efuse);
        *dc_r = 0;
    }

    TRACE("ANA: DC CALIB L=%d R=%d", *dc_l, *dc_r);

#if defined(ANA_DC_CALIB_L) || defined(ANA_DC_CALIB_R)
#ifdef ANA_DC_CALIB_L
    *dc_l = ANA_DC_CALIB_L;
#endif
#ifdef ANA_DC_CALIB_R
    *dc_r = ANA_DC_CALIB_R;
#endif
    TRACE("ANA: OVERRIDE DC CALIB L=%d R=%d", *dc_l, *dc_r);
#endif

    return;
}

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
static void analog_aud_dc_calib_init(void)
{
    uint16_t val;
    int16_t dc_l, dc_r;

    analog_aud_get_dc_calib_value(&dc_l, &dc_r);

    if (audio_output_diff) {
        analog_read(ANA_REG_7E, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL, dc_l);
        analog_write(ANA_REG_7E, val);

        analog_read(ANA_REG_7D, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITR, dc_r);
        analog_write(ANA_REG_7D, val);
    } else {
        analog_read(ANA_REG_7B, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_PA_OFF_BITL, dc_l);
        analog_write(ANA_REG_7B, val);

        analog_read(ANA_REG_7C, &val);
        val = SET_BITFIELD(val, REG_CODEC_TX_PA_OFF_BITR, dc_r);
        analog_write(ANA_REG_7C, val);
    }
}

static void analog_aud_dc_calib_enable(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_62, &val);
    if (audio_output_diff) {
        if (en) {
            val |= REG_CODEC_TX_EAR_OFF_EN;
        } else {
            val &= ~REG_CODEC_TX_EAR_OFF_EN;
        }
    } else {
        if (en) {
            val |= REG_CODEC_TX_PA_OFF_EN;
        } else {
            val &= ~REG_CODEC_TX_PA_OFF_EN;
        }
    }
    analog_write(ANA_REG_62, val);
}
#endif

static void analog_aud_vcodec_enable(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (vcodec_map == 0) {
            set = true;
        }
        vcodec_map |= (1 << user);
    } else {
        vcodec_map &= ~(1 << user);
        if (vcodec_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        pmu_codec_config(!!vcodec_map);
    }
}

static void analog_aud_enable_rx_cfg(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (rx_cfg_map == 0) {
            set = true;
        }
        rx_cfg_map |= (1 << user);
    } else {
        rx_cfg_map &= ~(1 << user);
        if (rx_cfg_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_64, &val);
        if (rx_cfg_map) {
            val |= CFG_EN_RX | CFG_EN_RX_DR;
        } else {
            val &= ~(CFG_EN_RX | CFG_EN_RX_DR);
        }
        analog_write(ANA_REG_64, val);
    }
}

static void analog_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev)
{
    uint32_t lock;
    CODEC_USER_MAP old_map;
    bool set = false;

    lock = int_lock();
    if (dev & AUD_VMIC_MAP_VMIC1) {
        if (vmic_1_map == 0) {
            set = true;
        }
        vmic_1_map |= (1 << user);
    } else {
        old_map = vmic_1_map;
        vmic_1_map &= ~(1 << user);
        if (old_map != 0 && vmic_1_map == 0) {
            set = true;
        }
    }

    if (dev & AUD_VMIC_MAP_VMIC2) {
        if (vmic_2_map == 0) {
            set = true;
        }
        vmic_2_map |= (1 << user);
    } else {
        old_map = vmic_2_map;
        vmic_2_map &= ~(1 << user);
        if (old_map != 0 && vmic_2_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        uint32_t pmu_map = 0;

        if (vmic_1_map) {
            pmu_map |= AUD_VMIC_MAP_VMIC1;
        }
        if (vmic_2_map) {
            pmu_map |= AUD_VMIC_MAP_VMIC2;
        }
        pmu_codec_mic_bias_enable(pmu_map);
        if (pmu_map) {
            osDelay(1);
        }
    }
}

void analog_open(void)
{
    uint16_t val;

    return;

#ifdef __AUDIO_RESAMPLE__
    if (hal_cmu_get_audio_resample_status()) {
        analog_init_xtal_fcap();
    }
#endif

    // Enable vcm_cap (tx bias)
    analog_write(ANA_REG_61, REG_CODEC_EN_TX);

    analog_write(ANA_REG_62, 0);

    analog_write(ANA_REG_63, 0);

    if (audio_output_diff) {
        val = REG_CODEC_TX_DAC_LPFVCM(4);
    } else {
        val = REG_CODEC_TX_DAC_LPFVCM(6);
    }
    analog_write(ANA_REG_64, val);

    analog_read(ANA_REG_69, &val);
    if (low_power_adc) {
        val = SET_BITFIELD(val, REG_CODEC_RX_IBSEL, 3);
    } else {
        val = SET_BITFIELD(val, REG_CODEC_RX_IBSEL, 8);
    }
    val = SET_BITFIELD(val, REG_CODEC_ADC_IBSEL, 0xA);
    val = SET_BITFIELD(val, REG_CODEC_ADC_VREF_SEL, 8);
    analog_write(ANA_REG_69, val);

    if (audio_output_diff) {
        val = REG_CODEC_TX_PA_SOFTSTART(0x20) | REG_CODEC_TX_PA_IBSEL(1);
        analog_write(ANA_REG_6B, val);
        val = REG_CODEC_TX_DAC_IBSEL(8) | REG_CODEC_TX_DAC_IDAC_SEL(0xB) | REG_CODEC_TX_DAC_VCAS_SEL(0xA);
        analog_write(ANA_REG_6C, val);
    } else {
        val = REG_CODEC_TX_PA_SOFTSTART(0x20) | REG_CODEC_TX_PA_IBSEL(1) | REG_CODEC_TX_PA_OUTPUTSEL(2);
        analog_write(ANA_REG_6B, val);
        // If 30mW output is required, increase IDAC_SEL to 0xB
        val = REG_CODEC_TX_DAC_IBSEL(8) | REG_CODEC_TX_DAC_IDAC_SEL(0xB) | REG_CODEC_TX_DAC_VCAS_SEL(8) |
            REG_CODEC_TX_PA_LOWVCM(4);
        analog_write(ANA_REG_6C, val);
    }

    analog_read(ANA_REG_6D, &val);
    val = (val & ~CFG_PGA_RESETN) | CFG_PGA_RESETN_DR;
    analog_write(ANA_REG_6D, val);

    analog_write(ANA_REG_6F, 0);

    // Init pga1 gain to 0dB
    val = REG_CODEC_PGA1A_GAIN(1) | REG_CODEC_PGA1B_GAIN(1) | REG_CODEC_PGA1C_GAIN(1) |
        REG_CODEC_PGA1D_GAIN(1) | REG_CODEC_PGA1E_GAIN(1);
    if (low_power_adc) {
        val |= REG_ADC_SLOWMODE;
    }
    analog_write(ANA_REG_70, val);

    val = REG_CODEC_ADC_VCM_CLOSE;
    analog_write(ANA_REG_71, val);

    if (audio_output_diff) {
        analog_read(ANA_REG_73, &val);
#if defined(AUDIO_OUTPUT_DC_CALIB_ANA)
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_SOFTSTART, 0x20);
#endif
        val = SET_BITFIELD(val, REG_CODEC_TX_EAR_IBSEL, 1);
        analog_write(ANA_REG_73, val);
    }

    val = REG_CODEC_TX_EAR_OUTPUTSEL(2) | REG_CODEC_VCMBUF_VSEL(4);
    if (vcodec_mv == 1500 || vcodec_mv == 1600) {
        val = SET_BITFIELD(val, REG_CODEC_TX_LDOVSEL, 0x18);
    } else {
        val = SET_BITFIELD(val, REG_CODEC_TX_LDOVSEL, 8);
    }
    analog_write(ANA_REG_74, val);

    // Fast charge vcm_cap to the closest voltage
    val = REG_AUDPLL_LDO_MDLL(3);
    if (vcodec_mv >= 2500) {
        val |= REG_CODEC_VCM_VSEL(8);
    } else {
        val |= REG_CODEC_VCM_VSEL(0);
    }
    analog_write(ANA_REG_75, val);
    if (vcodec_mv < 2500) {
        // It takes 50ms for vcm_cap to ramp up to 0.65V.
        // The 50ms interval cannot be changed for vcodec=1.6V and reg75=0x6000.
        // If the interval is longer, the voltage will be higher, which means extra stable time is needed.
        osDelay(50);
        val |= REG_CODEC_EN_VCMLPF;
        val = SET_BITFIELD(val, REG_CODEC_VCMBUF_LOWP, 1);
        val = SET_BITFIELD(val, REG_CODEC_VCM_VSEL, 0xF);
        if (vcodec_mv == 1500 || vcodec_mv == 1600) {
            val |= REG_CODEC_ADC_VREFP_CLOSE;
        }
        analog_write(ANA_REG_75, val);
    }

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    analog_aud_dc_calib_init();
    analog_aud_dc_calib_enable(true);
#endif

#ifdef ANC_APP
#ifdef ANC_INIT_SPEEDUP_EXTRA
    // Save 30% ANC setup time (200 ms --> 140 ms)
    analog_aud_enable_rx_cfg(ANA_CODEC_USER_ANC_INIT, true);
    analog_aud_enable_adc(ANA_CODEC_USER_ANC_INIT,
        AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 |
        AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4,
        true);
    hal_codec_anc_init_speedup(true);
    // Generate a PGA reset pulse
    analog_read(ANA_REG_6D, &val);
    val |= CFG_PGA_RESETN | CFG_PGA_RESETN_DR;
    analog_write(ANA_REG_6D, val);
    osDelay(20);
    val &= ~CFG_PGA_RESETN;
    analog_write(ANA_REG_6D, val);
#endif
#endif
}

void analog_sleep(void)
{
    return;

    // For vcodec lower than 2.5V, vcodec is powered off and shorted with vana.
    // Since vana is always powered on, vcm_cap can be always enabled to reduce the DAC PA stable time.
    // Note that deep-sleep vana is 1.1V, whereas active vana is 1.6V or 1.8V. So even if vcm_cap
    // is always enabled, it still needs some stable time.

    // Vcm_cap off --> save deep sleep current
    // Vcm_cap on  --> reduce DAC PA stable time

    if (vcodec_mv >= 2500) {
        uint16_t val;

        // Disable vcm_cap (tx bias)
        analog_read(ANA_REG_61, &val);
        val &= ~REG_CODEC_EN_TX;
        analog_write(ANA_REG_61, val);
    }
}

void analog_wakeup(void)
{
    return;

    if (vcodec_mv >= 2500) {
        uint16_t val;

        // Enable vcm_cap (tx bias)
        analog_read(ANA_REG_61, &val);
        val |= REG_CODEC_EN_TX;
        analog_write(ANA_REG_61, val);
    }
}

void analog_aud_codec_anc_enable(enum ANC_TYPE_T type, bool en)
{
    enum ANA_CODEC_USER_T user;
    enum AUD_CHANNEL_MAP_T ch_map;

    ch_map = 0;
    if (type == ANC_FEEDFORWARD) {
        user = ANA_CODEC_USER_ANC_FF;
#ifdef ANC_FF_ENABLED
        ch_map = ANC_FF_MIC_CH_L | ANC_FF_MIC_CH_R;
#endif
    } else {
        user = ANA_CODEC_USER_ANC_FB;
#ifdef ANC_FB_ENABLED
        ch_map = ANC_FB_MIC_CH_L | ANC_FB_MIC_CH_R;
#endif
    }

    ANALOG_TRACE("%s: type=%d en=%d ch_map=0x%x", __func__, type, en, ch_map);

    if (en) {
        analog_aud_enable_rx_cfg(user, true);
        analog_aud_enable_vmic(user, CFG_HW_AUD_INPUT_PATH_ANC_DEV);
        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
        analog_aud_enable_adc(user, ch_map, true);
    } else {
        analog_aud_enable_adc(user, ch_map, false);
        analog_aud_enable_vmic(user, 0);
        analog_aud_enable_rx_cfg(user, false);
    }
}

void analog_aud_codec_anc_init_close(void)
{
#ifdef ANC_APP
#ifdef ANC_INIT_SPEEDUP_EXTRA
    analog_aud_enable_rx_cfg(ANA_CODEC_USER_ANC_INIT, false);
    analog_aud_enable_adc(ANA_CODEC_USER_ANC_INIT,
        AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 | AUD_CHANNEL_MAP_CH2 |
        AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4,
        false);
    hal_codec_anc_init_speedup(false);
#endif
#endif
}

void analog_aud_mickey_enable(bool en)
{
    if (en) {
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV);
    } else {
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, 0);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_MICKEY, false);
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, false);
    }
}

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    if (en) {
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_ADC, true);
        if (input_path == AUD_INPUT_PATH_MAINMIC) {
            dev = CFG_HW_AUD_INPUT_PATH_MAINMIC_DEV;
        } else if (input_path == AUD_INPUT_PATH_LINEIN) {
            dev = CFG_HW_AUD_INPUT_PATH_LINEIN_DEV;
        } else {
            ASSERT(false, "%s: Bad input path: %d", __func__, input_path);
        }
        analog_aud_enable_vmic(ANA_CODEC_USER_ADC, dev);
        analog_aud_set_adc_gain(input_path, ch_map);
        analog_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, true);
    } else {
        analog_aud_enable_adc(ANA_CODEC_USER_ADC, ch_map, false);
        analog_aud_enable_vmic(ANA_CODEC_USER_ADC, 0);
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_ADC, false);
    }
}

static void analog_aud_codec_config_speaker(void)
{
    bool en;

    if (ana_spk_req && !ana_spk_muted) {
        en = true;
    } else {
        en = false;
    }

    if (ana_spk_enabled != en) {
        ana_spk_enabled = en;
        if (en) {
            analog_aud_enable_dac_pa(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);
#if defined(AUDIO_OUTPUT_DC_CALIB_ANA)
            if (audio_output_diff) {
                uint16_t val;
                int ret;
                ret = hal_codec_dac_reset_set();
                analog_read(ANA_REG_6E, &val);
                val |= CFG_TX_EN_S2PA;
                analog_write(ANA_REG_6E, val);
                if (ret) {
                    hal_codec_dac_reset_clear();
                }
            }
#endif
#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
            hal_sys_timer_delay_us(5);
            hal_gpio_pin_set(CFG_HW_AUD_OUTPUT_POP_SWITCH);
#endif
        } else {
#if defined(AUDIO_OUTPUT_DC_CALIB_ANA)
            if (audio_output_diff) {
                uint16_t val;
                analog_read(ANA_REG_6E, &val);
                val &= ~CFG_TX_EN_S2PA;
                analog_write(ANA_REG_6E, val);
            }
#endif
            analog_aud_enable_dac_pa(0);
#ifdef CFG_HW_AUD_OUTPUT_POP_SWITCH
            hal_gpio_pin_clr(CFG_HW_AUD_OUTPUT_POP_SWITCH);
#endif
        }
    }
}

void analog_aud_codec_speaker_enable(bool en)
{
    ana_spk_req = en;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_dac_enable(bool en)
{
    uint16_t val;

    if (en) {
        analog_aud_enable_rx_cfg(ANA_CODEC_USER_DAC, true);

        analog_read(ANA_REG_6D, &val);
        val |= CFG_TX_TREE_EN;
        analog_write(ANA_REG_6D, val);

        val = CFG_TX_EN_LPPA_DR | CFG_TX_EN_S1PA | CFG_TX_EN_S1PA_DR |
            CFG_TX_EN_S2PA_DR | CFG_TX_EN_S3PA | CFG_TX_EN_S3PA_DR |
            CFG_TX_EN_DACLDO | CFG_TX_EN_DACLDO_DR | CFG_TX_EN_VTOI | CFG_TX_EN_VTOI_DR;
#if defined(AUDIO_OUTPUT_DC_CALIB_ANA)
        if (audio_output_diff) {
            val |= CFG_TX_EN_LPPA;
        }
#else
        val |= CFG_TX_EN_S2PA;
#endif
        analog_write(ANA_REG_6E, val);

        if (audio_output_diff) {
            analog_read(ANA_REG_73, &val);
            val |= REG_CODEC_TX_EAR_ENBIAS;
            analog_write(ANA_REG_73, val);
        } else {
            analog_read(ANA_REG_74, &val);
            val |= REG_CODEC_TX_PA_ENBIAS;
            analog_write(ANA_REG_74, val);
        }

        analog_aud_enable_dac(CFG_HW_AUD_OUTPUT_PATH_SPEAKER_DEV);

        pmu_codec_hppa_enable(1);

#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        osDelay(1);
        analog_aud_codec_speaker_enable(true);
#endif
    } else {
#if !defined(AUDIO_OUTPUT_DC_CALIB_ANA) && !defined(AUDIO_OUTPUT_DC_CALIB)
        analog_aud_codec_speaker_enable(false);
        osDelay(1);
#endif

        pmu_codec_hppa_enable(0);

        analog_aud_enable_dac(0);

        analog_read(ANA_REG_6D, &val);
        val &= ~CFG_TX_TREE_EN;
        analog_write(ANA_REG_6D, val);

        analog_write(ANA_REG_6E, 0);

        if (audio_output_diff) {
            analog_read(ANA_REG_73, &val);
            val &= ~REG_CODEC_TX_EAR_ENBIAS;
            analog_write(ANA_REG_73, val);
        } else {
            analog_read(ANA_REG_74, &val);
            val &= ~REG_CODEC_TX_PA_ENBIAS;
            analog_write(ANA_REG_74, val);
        }

        analog_aud_enable_rx_cfg(ANA_CODEC_USER_DAC, false);
    }
}

void analog_aud_codec_open(void)
{
    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, true);

#ifdef _AUTO_SWITCH_POWER_MODE__
    //pmu_mode_change(PMU_POWER_MODE_DIG_DCDC);
#endif

    // vcodec >  1.8V: vcm_cap is charged by vcodec and DAC PA needs some stable time each time vcodec is powered on
    // vcodec <= 1.8V: vcm_cap is charged by vana (vcodec is powered off and shorted with vana) and DAC PA needs the
    //                 stable time only at system startup and wakeup from deep sleep (vana is never powered off)

    if (vcodec_mv >= 2500) {
        uint16_t val;

        // Wait for vcm_cap fast charge finished
        osDelay(80);

        analog_read(ANA_REG_75, &val);
        val |= REG_CODEC_EN_VCMLPF;
        val = SET_BITFIELD(val, REG_CODEC_VCMBUF_LOWP, 1);
        val = SET_BITFIELD(val, REG_CODEC_VCM_VSEL, 8);
        analog_write(ANA_REG_75, val);
    }
}

void analog_aud_codec_close(void)
{
    static const enum AUD_CHANNEL_MAP_T all_ch = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 |
        AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4;

    analog_aud_codec_speaker_enable(false);
    osDelay(1);
    analog_aud_codec_dac_enable(false);

    analog_aud_codec_adc_enable(AUD_IO_PATH_NULL, all_ch, false);

    if (vcodec_mv >= 2500) {
        uint16_t val;

        // Set vcm_cap to fast charge mode
        analog_read(ANA_REG_75, &val);
        val &= ~(REG_CODEC_EN_VCMLPF | REG_CODEC_VCMBUF_LOWP_MASK | REG_CODEC_VCM_VSEL_MASK | REG_CODEC_ADC_VREFP_CLOSE);
        val |= REG_CODEC_VCM_VSEL(8);
        analog_write(ANA_REG_75, val);
    }

#ifdef _AUTO_SWITCH_POWER_MODE__
    //pmu_mode_change(PMU_POWER_MODE_ANA_DCDC);
#endif

    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, false);
}

void analog_aud_codec_mute(void)
{
#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_codec_tx_pa_gain_sel(0);
#endif

    ana_spk_muted = true;
    analog_aud_codec_config_speaker();
}

void analog_aud_codec_nomute(void)
{
    ana_spk_muted = false;
    analog_aud_codec_config_speaker();

#ifndef AUDIO_OUTPUT_DC_CALIB
    //analog_aud_set_dac_gain(dac_gain);
#endif
}

int analog_debug_config_audio_output(bool diff)
{
#ifdef ANC_PROD_TEST
    int ret;

    ret = pmu_debug_config_audio_output(diff);
    if (ret) {
        return ret;
    }

    audio_output_diff = diff;
    if (diff) {
        tx_pa_gain_0db = TX_PA_GAIN_0DB_DIFF;
    } else {
        tx_pa_gain_0db = TX_PA_GAIN_0DB_SE;
    }
#endif
    return 0;
}

int analog_debug_config_codec(uint16_t mv)
{
#ifdef ANC_PROD_TEST
    int ret;

    ret = pmu_debug_config_codec(mv);
    if (ret) {
        return ret;
    }
 
    vcodec_mv = mv;
#endif
    return 0;
}

int analog_debug_config_low_power_adc(bool enable)
{
#ifdef ANC_PROD_TEST
    low_power_adc = enable;
#endif
    return 0;
}

//--------------------------------------------------------------
// DCXO
//--------------------------------------------------------------

// ANA_REG_7C
#define DCO_FINE_CLK_SEL                    (1 << 0)
#define REG_COARSE_EN                       (1 << 1)
#define REG_FINE_EN                         (1 << 2)
#define DCO_PU_XO_BUFF_DR                   (1 << 3)
#define DCO_PU_LDO_DR                       (1 << 4)
#define DCO_PU_CORE_DR                      (1 << 5)
#define DCO_PU_BUFFER_PRES_DR               (1 << 6)
#define DCO_PU_DIVN_DR                      (1 << 7)
#define DCO_XO_BUFFER_SEL_MODE_DR           (1 << 8)
#define DCO_XO_PWUP_DR                      (1 << 9)
#define DCO_TST_BUFFER_RCTRL_DR             (1 << 10)
#define DCO_XO_BUFFER_PWUP_DR               (1 << 11)
#define DCO_FINE_TUN_RST_DR                 (1 << 12)
#define REG_COARSE_EN_DR                    (1 << 13)
#define REG_FINE_EN_DR                      (1 << 14)
#define RESERVED_ANA_50                     (1 << 15)

// ANA_REG_174
#define DBG_FINE_TUN_CODE_SHIFT             0
#define DBG_FINE_TUN_CODE_MASK              (0x7FF << DBG_FINE_TUN_CODE_SHIFT)
#define DBG_FINE_TUN_CODE(n)                BITFIELD_VAL(DBG_FINE_TUN_CODE, n)
#define DIG_XTAL_DCXO_EN                    (1 << 11)
#define DBG_FINE_TUN_STABLE                 (1 << 12)

int analog_dcxo_enabled(void)
{
    uint16_t val;

    analog_read(ANA_REG_174, &val);
    return !!(val & DIG_XTAL_DCXO_EN);
}

int analog_dcxo_fine_tune_stable(void)
{
    uint16_t val;

    analog_read(ANA_REG_174, &val);
    return !!(val & DBG_FINE_TUN_STABLE);
}

void analog_dcxo_enable_hs_mode(void)
{
    uint16_t val;

    analog_read(ANA_REG_7C, &val);
    val &= ~REG_FINE_EN;
    analog_write(ANA_REG_7C, val);
    val |= REG_FINE_EN_DR;
    analog_write(ANA_REG_7C, val);

    analog_write(ANA_REG_16C, 0x80FF);
    analog_write(ANA_REG_168, 0xA600);
    analog_write(ANA_REG_169, 0x0064);
    analog_write(ANA_REG_16A, 0x49EF);
    analog_write(ANA_REG_16B, 0x0002);
    analog_write(ANA_REG_16C, 0x00FF);

    analog_read(ANA_REG_7C, &val);
    val = (val & ~REG_FINE_EN_DR) | REG_FINE_EN;
    analog_write(ANA_REG_7C, val);
}

