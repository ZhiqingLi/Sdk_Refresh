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

#define DEFAULT_ANC_FF_ADC_GAIN_DB          6
#define DEFAULT_ANC_FB_ADC_GAIN_DB          6
#define DEFAULT_VOICE_ADC_GAIN_DB           12

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

// 61
#define REG_CODEC_EN_ADCA                   (1 << 0)
#define REG_CODEC_EN_ADCB                   (1 << 1)
#define REG_CODEC_EN_ADCC                   (1 << 2)
#define REG_CODEC_EN_ADCD                   (1 << 3)
#define REG_CODEC_ADCA_DITHER_EN            (1 << 4)
#define REG_CODEC_ADCB_DITHER_EN            (1 << 5)
#define REG_CODEC_ADCC_DITHER_EN            (1 << 6)
#define REG_CODEC_ADCD_DITHER_EN            (1 << 7)
#define REG_CODEC_ADCA_CH_SEL_SHIFT         8
#define REG_CODEC_ADCA_CH_SEL_MASK          (0x3 << REG_CODEC_ADCA_CH_SEL_SHIFT)
#define REG_CODEC_ADCA_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCA_CH_SEL, n)
#define REG_CODEC_ADCB_CH_SEL_SHIFT         10
#define REG_CODEC_ADCB_CH_SEL_MASK          (0x3 << REG_CODEC_ADCB_CH_SEL_SHIFT)
#define REG_CODEC_ADCB_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCB_CH_SEL, n)
#define REG_CODEC_ADCC_CH_SEL_SHIFT         12
#define REG_CODEC_ADCC_CH_SEL_MASK          (0x3 << REG_CODEC_ADCC_CH_SEL_SHIFT)
#define REG_CODEC_ADCC_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCC_CH_SEL, n)
#define REG_CODEC_ADCD_CH_SEL_SHIFT         14
#define REG_CODEC_ADCD_CH_SEL_MASK          (0x3 << REG_CODEC_ADCD_CH_SEL_SHIFT)
#define REG_CODEC_ADCD_CH_SEL(n)            BITFIELD_VAL(REG_CODEC_ADCD_CH_SEL, n)

// 63
#define REG_CODEC_ADCA_GAIN_BIT_SHIFT       0
#define REG_CODEC_ADCA_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCA_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCA_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCA_GAIN_BIT, n)
#define REG_CODEC_ADCB_GAIN_BIT_SHIFT       3
#define REG_CODEC_ADCB_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCB_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCB_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCB_GAIN_BIT, n)
#define REG_CODEC_ADCC_GAIN_BIT_SHIFT       6
#define REG_CODEC_ADCC_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCC_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCC_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCC_GAIN_BIT, n)
#define REG_CODEC_ADCD_GAIN_BIT_SHIFT       9
#define REG_CODEC_ADCD_GAIN_BIT_MASK        (0x7 << REG_CODEC_ADCD_GAIN_BIT_SHIFT)
#define REG_CODEC_ADCD_GAIN_BIT(n)          BITFIELD_VAL(REG_CODEC_ADCD_GAIN_BIT, n)
#define REG_CODEC_ADCA_GAIN_UPDATE          (1 << 12)
#define REG_CODEC_ADCB_GAIN_UPDATE          (1 << 13)
#define REG_CODEC_ADCC_GAIN_UPDATE          (1 << 14)
#define REG_CODEC_ADCD_GAIN_UPDATE          (1 << 15)

// 67
#define REG_CODEC_ADC_IBSEL_REG_SHIFT       0
#define REG_CODEC_ADC_IBSEL_REG_MASK        (0xF << REG_CODEC_ADC_IBSEL_REG_SHIFT)
#define REG_CODEC_ADC_IBSEL_REG(n)          BITFIELD_VAL(REG_CODEC_ADC_IBSEL_REG, n)
#define REG_CODEC_ADC_IBSEL_VCOMP_SHIFT     4
#define REG_CODEC_ADC_IBSEL_VCOMP_MASK      (0xF << REG_CODEC_ADC_IBSEL_VCOMP_SHIFT)
#define REG_CODEC_ADC_IBSEL_VCOMP(n)        BITFIELD_VAL(REG_CODEC_ADC_IBSEL_VCOMP, n)
#define REG_CODEC_ADC_IBSEL_VREF_SHIFT      8
#define REG_CODEC_ADC_IBSEL_VREF_MASK       (0xF << REG_CODEC_ADC_IBSEL_VREF_SHIFT)
#define REG_CODEC_ADC_IBSEL_VREF(n)         BITFIELD_VAL(REG_CODEC_ADC_IBSEL_VREF, n)
#define REG_CODEC_ADC_LPFVCM_SW_SHIFT       12
#define REG_CODEC_ADC_LPFVCM_SW_MASK        (0xF << REG_CODEC_ADC_LPFVCM_SW_SHIFT)
#define REG_CODEC_ADC_LPFVCM_SW(n)          BITFIELD_VAL(REG_CODEC_ADC_LPFVCM_SW, n)

// 68
#define REG_CODEC_ADC_OP1_IBIT_SHIFT        0
#define REG_CODEC_ADC_OP1_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP1_IBIT_SHIFT)
#define REG_CODEC_ADC_OP1_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP1_IBIT, n)
#define REG_CODEC_ADC_OP1_R_SEL             (1 << 2)
#define REG_CODEC_ADC_OP2_IBIT_SHIFT        3
#define REG_CODEC_ADC_OP2_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP2_IBIT_SHIFT)
#define REG_CODEC_ADC_OP2_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP2_IBIT, n)
#define REG_CODEC_ADC_OP2_R_SEL             (1 << 5)
#define REG_CODEC_ADC_OP3_IBIT_SHIFT        6
#define REG_CODEC_ADC_OP3_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP3_IBIT_SHIFT)
#define REG_CODEC_ADC_OP3_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP3_IBIT, n)
#define REG_CODEC_ADC_OP3_R_SEL             (1 << 8)
#define REG_CODEC_ADC_OP4_IBIT_SHIFT        9
#define REG_CODEC_ADC_OP4_IBIT_MASK         (0x3 << REG_CODEC_ADC_OP4_IBIT_SHIFT)
#define REG_CODEC_ADC_OP4_IBIT(n)           BITFIELD_VAL(REG_CODEC_ADC_OP4_IBIT, n)
#define REG_CODEC_ADC_OP4_R_SEL             (1 << 11)
#define REG_CODEC_ADC_VREF_SEL_SHIFT        12
#define REG_CODEC_ADC_VREF_SEL_MASK         (0xF << REG_CODEC_ADC_VREF_SEL_SHIFT)
#define REG_CODEC_ADC_VREF_SEL(n)           BITFIELD_VAL(REG_CODEC_ADC_VREF_SEL, n)

// 69
#define REG_CODEC_BIAS_IBSEL_SHIFT          0
#define REG_CODEC_BIAS_IBSEL_MASK           (0xF << REG_CODEC_BIAS_IBSEL_SHIFT)
#define REG_CODEC_BIAS_IBSEL(n)             BITFIELD_VAL(REG_CODEC_BIAS_IBSEL, n)
#define REG_CODEC_BIAS_IBSEL_TX_SHIFT       4
#define REG_CODEC_BIAS_IBSEL_TX_MASK        (0xF << REG_CODEC_BIAS_IBSEL_TX_SHIFT)
#define REG_CODEC_BIAS_IBSEL_TX(n)          BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_TX, n)
#define REG_CODEC_BIAS_IBSEL_VOICE_SHIFT    8
#define REG_CODEC_BIAS_IBSEL_VOICE_MASK     (0xF << REG_CODEC_BIAS_IBSEL_VOICE_SHIFT)
#define REG_CODEC_BIAS_IBSEL_VOICE(n)       BITFIELD_VAL(REG_CODEC_BIAS_IBSEL_VOICE, n)
#define REG_CODEC_BIAS_LOWV                 (1 << 12)
#define REG_CODEC_BIAS_LOWV_LP              (1 << 13)
#define REG_CODEC_BUF_LOWPOWER              (1 << 14)
#define REG_CODEC_BUF_LOWPOWER2             (1 << 15)

// 6A
#define REG_CODEC_ADC_REG_VSEL_SHIFT        0
#define REG_CODEC_ADC_REG_VSEL_MASK         (0x7 << REG_CODEC_ADC_REG_VSEL_SHIFT)
#define REG_CODEC_ADC_REG_VSEL(n)           BITFIELD_VAL(REG_CODEC_ADC_REG_VSEL, n)
#define REG_CODEC_ADC_RES_SEL_SHIFT         3
#define REG_CODEC_ADC_RES_SEL_MASK          (0x7 << REG_CODEC_ADC_RES_SEL_SHIFT)
#define REG_CODEC_ADC_RES_SEL(n)            BITFIELD_VAL(REG_CODEC_ADC_RES_SEL, n)
#define REG_CODEC_BUF_LOWVCM_SHIFT          6
#define REG_CODEC_BUF_LOWVCM_MASK           (0x7 << REG_CODEC_BUF_LOWVCM_SHIFT)
#define REG_CODEC_BUF_LOWVCM(n)             BITFIELD_VAL(REG_CODEC_BUF_LOWVCM, n)
#define REG_CODEC_EN_BIAS                   (1 << 9)
#define REG_CODEC_EN_BIAS_LP                (1 << 10)
#define REG_CODEC_EN_RX_EXT                 (1 << 11)
#define REG_CODEC_EN_TX_EXT                 (1 << 12)
#define REG_CODEC_DAC_CLK_EDGE_SEL          (1 << 13)
#define CFG_TX_CH0_MUTE                     (1 << 14)
#define CFG_TX_CH1_MUTE                     (1 << 15)

// 6B
#define REG_CODEC_EN_VCM                    (1 << 0)
#define REG_CODEC_VCM_EN_LPF                (1 << 1)
#define REG_CODEC_LP_VCM                    (1 << 2)
#define REG_CODEC_VCM_LOW_VCM_SHIFT         3
#define REG_CODEC_VCM_LOW_VCM_MASK          (0xF << REG_CODEC_VCM_LOW_VCM_SHIFT)
#define REG_CODEC_VCM_LOW_VCM(n)            BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM, n)
#define REG_CODEC_VCM_LOW_VCM_LP_SHIFT      7
#define REG_CODEC_VCM_LOW_VCM_LP_MASK       (0xF << REG_CODEC_VCM_LOW_VCM_LP_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LP(n)         BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LP, n)
#define REG_CODEC_VCM_LOW_VCM_LPF_SHIFT     11
#define REG_CODEC_VCM_LOW_VCM_LPF_MASK      (0xF << REG_CODEC_VCM_LOW_VCM_LPF_SHIFT)
#define REG_CODEC_VCM_LOW_VCM_LPF(n)        BITFIELD_VAL(REG_CODEC_VCM_LOW_VCM_LPF, n)
#define REG_CODEC_EN_VCM_BUFFER             (1 << 15)

// 6C
#define REG_CODEC_RX_EN_VTOI                (1 << 0)
#define REG_CODEC_RX_VTOI_I_DAC2_SHIFT      1
#define REG_CODEC_RX_VTOI_I_DAC2_MASK       (0x7 << REG_CODEC_RX_VTOI_I_DAC2_SHIFT)
#define REG_CODEC_RX_VTOI_I_DAC2(n)         BITFIELD_VAL(REG_CODEC_RX_VTOI_I_DAC2, n)
#define REG_CODEC_RX_VTOI_IDAC_SEL_SHIFT    4
#define REG_CODEC_RX_VTOI_IDAC_SEL_MASK     (0xF << REG_CODEC_RX_VTOI_IDAC_SEL_SHIFT)
#define REG_CODEC_RX_VTOI_IDAC_SEL(n)       BITFIELD_VAL(REG_CODEC_RX_VTOI_IDAC_SEL, n)
#define REG_CODEC_RX_VTOI_VCS_SEL_SHIFT     8
#define REG_CODEC_RX_VTOI_VCS_SEL_MASK      (0x1F << REG_CODEC_RX_VTOI_VCS_SEL_SHIFT)
#define REG_CODEC_RX_VTOI_VCS_SEL(n)        BITFIELD_VAL(REG_CODEC_RX_VTOI_VCS_SEL, n)
#define REG_CODEC_TEST_SEL_SHIFT            13
#define REG_CODEC_TEST_SEL_MASK             (0x7 << REG_CODEC_TEST_SEL_SHIFT)
#define REG_CODEC_TEST_SEL(n)               BITFIELD_VAL(REG_CODEC_TEST_SEL, n)

// 6D
#define REG_CODEC_TX_DAC_MUTEL              (1 << 0)
#define REG_CODEC_TX_DAC_MUTER              (1 << 1)
#define REG_CODEC_TX_DAC_SWR_SHIFT          2
#define REG_CODEC_TX_DAC_SWR_MASK           (0x3 << REG_CODEC_TX_DAC_SWR_SHIFT)
#define REG_CODEC_TX_DAC_SWR(n)             BITFIELD_VAL(REG_CODEC_TX_DAC_SWR, n)
#define REG_CODEC_TX_DAC_VREF_L_SHIFT       4
#define REG_CODEC_TX_DAC_VREF_L_MASK        (0xF << REG_CODEC_TX_DAC_VREF_L_SHIFT)
#define REG_CODEC_TX_DAC_VREF_L(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_L, n)
#define REG_CODEC_TX_DAC_VREF_R_SHIFT       8
#define REG_CODEC_TX_DAC_VREF_R_MASK        (0xF << REG_CODEC_TX_DAC_VREF_R_SHIFT)
#define REG_CODEC_TX_DAC_VREF_R(n)          BITFIELD_VAL(REG_CODEC_TX_DAC_VREF_R, n)
#define REG_CODEC_TX_EAR_CAS_BIT_SHIFT      12
#define REG_CODEC_TX_EAR_CAS_BIT_MASK       (0x3 << REG_CODEC_TX_EAR_CAS_BIT_SHIFT)
#define REG_CODEC_TX_EAR_CAS_BIT(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_CAS_BIT, n)
#define REG_CODEC_TX_EAR_DIS_SHIFT          14
#define REG_CODEC_TX_EAR_DIS_MASK           (0x3 << REG_CODEC_TX_EAR_DIS_SHIFT)
#define REG_CODEC_TX_EAR_DIS(n)             BITFIELD_VAL(REG_CODEC_TX_EAR_DIS, n)

// 6E
#define REG_CODEC_TX_EAR_COMP_L_SHIFT       0
#define REG_CODEC_TX_EAR_COMP_L_MASK        (0x7 << REG_CODEC_TX_EAR_COMP_L_SHIFT)
#define REG_CODEC_TX_EAR_COMP_L(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_COMP_L, n)
#define REG_CODEC_TX_EAR_COMP_R_SHIFT       3
#define REG_CODEC_TX_EAR_COMP_R_MASK        (0x7 << REG_CODEC_TX_EAR_COMP_R_SHIFT)
#define REG_CODEC_TX_EAR_COMP_R(n)          BITFIELD_VAL(REG_CODEC_TX_EAR_COMP_R, n)
#define REG_CODEC_TX_EAR_DOUBLEBIAS         (1 << 6)
#define REG_CODEC_TX_EAR_DR_EN              (1 << 7)
#define REG_CODEC_TX_EAR_DR_ST_SHIFT        8
#define REG_CODEC_TX_EAR_DR_ST_MASK         (0x7 << REG_CODEC_TX_EAR_DR_ST_SHIFT)
#define REG_CODEC_TX_EAR_DR_ST(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_DR_ST, n)
#define REG_CODEC_TX_EAR_ENBIAS             (1 << 11)
#define REG_CODEC_TX_EAR_FBCAP_SHIFT        12
#define REG_CODEC_TX_EAR_FBCAP_MASK         (0x3 << REG_CODEC_TX_EAR_FBCAP_SHIFT)
#define REG_CODEC_TX_EAR_FBCAP(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_FBCAP, n)
#define REG_CODEC_TX_EAR_IBSEL_SHIFT        14
#define REG_CODEC_TX_EAR_IBSEL_MASK         (0x3 << REG_CODEC_TX_EAR_IBSEL_SHIFT)
#define REG_CODEC_TX_EAR_IBSEL(n)           BITFIELD_VAL(REG_CODEC_TX_EAR_IBSEL, n)

// 6F
#define REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT   0
#define REG_CODEC_TX_EAR_DRE_GAIN_L_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_L_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_L(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_L, n)
#define REG_CODEC_TX_EAR_DRE_GAIN_R_SHIFT   5
#define REG_CODEC_TX_EAR_DRE_GAIN_R_MASK    (0x1F << REG_CODEC_TX_EAR_DRE_GAIN_R_SHIFT)
#define REG_CODEC_TX_EAR_DRE_GAIN_R(n)      BITFIELD_VAL(REG_CODEC_TX_EAR_DRE_GAIN_R, n)
#define DRE_GAIN_SEL_L                      (1 << 10)
#define DRE_GAIN_SEL_R                      (1 << 11)
#define REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE  (1 << 12)
#define REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE  (1 << 13)
#define REG_CODEC_TX_EAR_GAIN_SHIFT         14
#define REG_CODEC_TX_EAR_GAIN_MASK          (0x3 << REG_CODEC_TX_EAR_GAIN_SHIFT)
#define REG_CODEC_TX_EAR_GAIN(n)            BITFIELD_VAL(REG_CODEC_TX_EAR_GAIN, n)

// 70
#define REG_CODEC_TX_EAR_LCAL               (1 << 0)
#define REG_CODEC_TX_EAR_RCAL               (1 << 1)
#define REG_CODEC_TX_EAR_LPBIAS             (1 << 2)
#define REG_CODEC_TX_EAR_OCEN               (1 << 3)
#define REG_CODEC_TX_EAR_OFFEN              (1 << 4)
#define REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT    5
#define REG_CODEC_TX_EAR_OUTPUTSEL_MASK     (0xF << REG_CODEC_TX_EAR_OUTPUTSEL_SHIFT)
#define REG_CODEC_TX_EAR_OUTPUTSEL(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_OUTPUTSEL, n)
#define REG_CODEC_TX_EAR_SOFTSTART_SHIFT    9
#define REG_CODEC_TX_EAR_SOFTSTART_MASK     (0x3F << REG_CODEC_TX_EAR_SOFTSTART_SHIFT)
#define REG_CODEC_TX_EAR_SOFTSTART(n)       BITFIELD_VAL(REG_CODEC_TX_EAR_SOFTSTART, n)
#define CFG_TX_TREE_EN                      (1 << 15)

// 71
#define REG_CODEC_TX_EAR_LOWGAINL_SHIFT     0
#define REG_CODEC_TX_EAR_LOWGAINL_MASK      (0x3 << REG_CODEC_TX_EAR_LOWGAINL_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINL, n)
#define REG_CODEC_TX_EAR_OFF_BITL_SHIFT     2
#define REG_CODEC_TX_EAR_OFF_BITL_MASK      (0x3FFF << REG_CODEC_TX_EAR_OFF_BITL_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITL(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITL, n)

// 72
#define REG_CODEC_TX_EAR_LOWGAINR_SHIFT     0
#define REG_CODEC_TX_EAR_LOWGAINR_MASK      (0x3 << REG_CODEC_TX_EAR_LOWGAINR_SHIFT)
#define REG_CODEC_TX_EAR_LOWGAINR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_LOWGAINR, n)
#define REG_CODEC_TX_EAR_OFF_BITR_SHIFT     2
#define REG_CODEC_TX_EAR_OFF_BITR_MASK      (0x3FFF << REG_CODEC_TX_EAR_OFF_BITR_SHIFT)
#define REG_CODEC_TX_EAR_OFF_BITR(n)        BITFIELD_VAL(REG_CODEC_TX_EAR_OFF_BITR, n)

// 73
#define REG_CODEC_TX_EN_DACLDO              (1 << 0)
#define REG_CODEC_TX_EN_EARPA_L             (1 << 1)
#define REG_CODEC_TX_EN_EARPA_R             (1 << 2)
#define REG_CODEC_TX_EN_LCLK                (1 << 3)
#define REG_CODEC_TX_EN_RCLK                (1 << 4)
#define REG_CODEC_TX_EN_LDAC                (1 << 5)
#define REG_CODEC_TX_EN_RDAC                (1 << 6)
#define REG_CODEC_TX_EN_LPPA                (1 << 7)
#define REG_CODEC_TX_EN_S1PA                (1 << 8)
#define REG_CODEC_TX_EN_S2PA                (1 << 9)
#define REG_CODEC_TX_EN_S3PA                (1 << 10)
#define REG_CODEC_TX_EN_S4PA                (1 << 11)
#define REG_CODEC_TX_EN_S5PA                (1 << 12)
#define REG_IDETLEAR_EN                     (1 << 13)
#define REG_IDETREAR_EN                     (1 << 14)
#define REG_CODEC_TX_SW_MODE                (1 << 15)

// 74
#define REG_CODEC_TX_RVREF_CAP_BIT          (1 << 0)
#define REG_DAC_LDO0P9_VSEL_SHIFT           1
#define REG_DAC_LDO0P9_VSEL_MASK            (0x1F << REG_DAC_LDO0P9_VSEL_SHIFT)
#define REG_DAC_LDO0P9_VSEL(n)              BITFIELD_VAL(REG_DAC_LDO0P9_VSEL, n)
#define REG_BYPASS_TX_REGULATOR             (1 << 6)
#define REG_PU_TX_REGULATOR                 (1 << 7)
#define REG_TX_REGULATOR_BIT_SHIFT          8
#define REG_TX_REGULATOR_BIT_MASK           (0xF << REG_TX_REGULATOR_BIT_SHIFT)
#define REG_TX_REGULATOR_BIT(n)             BITFIELD_VAL(REG_TX_REGULATOR_BIT, n)
#define REG_TX_REG_CAP_BIT                  (1 << 12)
#define REG_CODEC_TX_EAR_VCM_BIT_SHIFT      13
#define REG_CODEC_TX_EAR_VCM_BIT_MASK       (0x3 << REG_CODEC_TX_EAR_VCM_BIT_SHIFT)
#define REG_CODEC_TX_EAR_VCM_BIT(n)         BITFIELD_VAL(REG_CODEC_TX_EAR_VCM_BIT, n)
#define REG_CODEC_TX_EAR_VCM_SEL            (1 << 15)

// 75
#define REG_CRYSTAL_SEL_LV                  (1 << 0)
#define REG_EXTPLL_SEL                      (1 << 1)
#define REG_AUDPLL_CAL_EN                   (1 << 2)
#define REG_AUDPLL_CP_IEN_SHIFT             3
#define REG_AUDPLL_CP_IEN_MASK              (0xF << REG_AUDPLL_CP_IEN_SHIFT)
#define REG_AUDPLL_CP_IEN(n)                BITFIELD_VAL(REG_AUDPLL_CP_IEN, n)
#define REG_AUDPLL_CP_SWRC_SHIFT            7
#define REG_AUDPLL_CP_SWRC_MASK             (0x3 << REG_AUDPLL_CP_SWRC_SHIFT)
#define REG_AUDPLL_CP_SWRC(n)               BITFIELD_VAL(REG_AUDPLL_CP_SWRC, n)
#define REG_AUDPLL_DIG_SWRC_SHIFT           9
#define REG_AUDPLL_DIG_SWRC_MASK            (0x3 << REG_AUDPLL_DIG_SWRC_SHIFT)
#define REG_AUDPLL_DIG_SWRC(n)              BITFIELD_VAL(REG_AUDPLL_DIG_SWRC, n)
#define REG_AUDPLL_VCO_SPD_SHIFT            11
#define REG_AUDPLL_VCO_SPD_MASK             (0x7 << REG_AUDPLL_VCO_SPD_SHIFT)
#define REG_AUDPLL_VCO_SPD(n)               BITFIELD_VAL(REG_AUDPLL_VCO_SPD, n)
#define REG_AUDPLL_VCO_SWRC_SHIFT           14
#define REG_AUDPLL_VCO_SWRC_MASK            (0x3 << REG_AUDPLL_VCO_SWRC_SHIFT)
#define REG_AUDPLL_VCO_SWRC(n)              BITFIELD_VAL(REG_AUDPLL_VCO_SWRC, n)

// 77
#define REG_AUDPLL_DIVN_SHIFT               0
#define REG_AUDPLL_DIVN_MASK                (0x1F << REG_AUDPLL_DIVN_SHIFT)
#define REG_AUDPLL_DIVN(n)                  BITFIELD_VAL(REG_AUDPLL_DIVN, n)
#define REG_AUDPLL_DIVN_RST                 (1 << 5)
#define REG_AUDPLL_DIV_DR                   (1 << 6)
#define REG_AUDPLL_EN_CLKA                  (1 << 7)
#define REG_AUDPLL_EN_CLKD                  (1 << 8)
#define REG_AUDPLL_FORCE_LK                 (1 << 9)
#define REG_AUDPLL_LK_ERR23                 (1 << 10)
#define REG_AUDPLL_LK_LONG                  (1 << 11)
#define REG_AUDPLL_LK_RSTB                  (1 << 12)
#define REG_AUDPLL_LK_WIN_SHIFT             13
#define REG_AUDPLL_LK_WIN_MASK              (0x7 << REG_AUDPLL_LK_WIN_SHIFT)
#define REG_AUDPLL_LK_WIN(n)                BITFIELD_VAL(REG_AUDPLL_LK_WIN, n)

// 7A
#define REG_PU_OSC                          (1 << 0)
#define REG_BBPLL_TST_EN                    (1 << 1)
#define REG_AUDPLL_TRIGGER_EN               (1 << 2)
#define REG_AUDPLL_FREQ_EN                  (1 << 3)
#define REG_AUDPLL_CLK_FBC_EDGE             (1 << 4)
#define REG_AUDPLL_INT_DEC_SEL_SHIFT        5
#define REG_AUDPLL_INT_DEC_SEL_MASK         (0x7 << REG_AUDPLL_INT_DEC_SEL_SHIFT)
#define REG_AUDPLL_INT_DEC_SEL(n)           BITFIELD_VAL(REG_AUDPLL_INT_DEC_SEL, n)
#define REG_AUDPLL_DITHER_BYPASS            (1 << 8)
#define REG_AUDPLL_PRESCALER_DEL_SEL_SHIFT  9
#define REG_AUDPLL_PRESCALER_DEL_SEL_MASK   (0xF << REG_AUDPLL_PRESCALER_DEL_SEL_SHIFT)
#define REG_AUDPLL_PRESCALER_DEL_SEL(n)     BITFIELD_VAL(REG_AUDPLL_PRESCALER_DEL_SEL, n)
#define REG_AUDPLL_FREQ_34_32_SHIFT         13
#define REG_AUDPLL_FREQ_34_32_MASK          (0x7 << REG_AUDPLL_FREQ_34_32_SHIFT)
#define REG_AUDPLL_FREQ_34_32(n)            BITFIELD_VAL(REG_AUDPLL_FREQ_34_32, n)

// 7E
#define REG_CLKMUX_DVDD_SEL                 (1 << 0)
#define REG_CLKMUX_LDO0P9_VSEL_SHIFT        1
#define REG_CLKMUX_LDO0P9_VSEL_MASK         (0x1F << REG_CLKMUX_LDO0P9_VSEL_SHIFT)
#define REG_CLKMUX_LDO0P9_VSEL(n)           BITFIELD_VAL(REG_CLKMUX_LDO0P9_VSEL, n)
#define REG_PU_CLKMUX_LDO0P9                (1 << 6)
#define REG_AUDPLL_LDO_VREF_SHIFT           7
#define REG_AUDPLL_LDO_VREF_MASK            (0x7 << REG_AUDPLL_LDO_VREF_SHIFT)
#define REG_AUDPLL_LDO_VREF(n)              BITFIELD_VAL(REG_AUDPLL_LDO_VREF, n)
#define REG_AUDPLL_LPF_BW_SEL               (1 << 10)
#define REG_CODEC_ADC_DITHER0P5_EN          (1 << 11)
#define REG_CODEC_ADC_DITHER0P25_EN         (1 << 12)
#define REG_CODEC_ADC_DITHER1_EN            (1 << 13)
#define REG_CODEC_ADC_DITHER2_EN            (1 << 14)
#define REG_CODEC_ADC_DITHER_PHASE_SEL      (1 << 15)

// 174
#define CFG_TX_PEAK_OFF_ADC_EN              (1 << 0)
#define CFG_TX_PEAK_OFF_DAC_EN              (1 << 1)
#define CFG_PEAK_DET_DR                     (1 << 2)
#define CFG_TX_PEAK_OFF_ADC                 (1 << 3)
#define CFG_TX_PEAK_OFF_DAC                 (1 << 4)
#define CFG_PEAK_DET_DB_DELAY_SHIFT         5
#define CFG_PEAK_DET_DB_DELAY_MASK          (0x7 << CFG_PEAK_DET_DB_DELAY_SHIFT)
#define CFG_PEAK_DET_DB_DELAY(n)            BITFIELD_VAL(CFG_PEAK_DET_DB_DELAY, n)
#define REG_CODEC_TX_PEAK_DET_BIT           (1 << 8)
#define REG_CODEC_TX_PEAK_NL_EN             (1 << 9)
#define REG_CODEC_TX_PEAK_NR_EN             (1 << 10)
#define REG_CODEC_TX_PEAK_PL_EN             (1 << 11)
#define REG_CODEC_TX_PEAK_PR_EN             (1 << 12)
#define CFG_TX_CLK_INV                      (1 << 13)
#define CFG_CODEC_DIN_L_RST                 (1 << 14)
#define CFG_CODEC_DIN_R_RST                 (1 << 15)

// C4
#define CODEC_RESAMPLE_CLK_BUF_PU           (1 << 2)

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

    ANA_REG_161 = 0x161,
    ANA_REG_162,
    ANA_REG_163,
    ANA_REG_164,
    ANA_REG_165,
    ANA_REG_166,
    ANA_REG_167,
    ANA_REG_168,
    ANA_REG_169,
    ANA_REG_16A,
    ANA_REG_16B,
    ANA_REG_16C,
    ANA_REG_16D,
    ANA_REG_16E,
    ANA_REG_16F,
    ANA_REG_170,
    ANA_REG_171,
    ANA_REG_172,
    ANA_REG_173,
    ANA_REG_174,
    ANA_REG_175,
    ANA_REG_176,
    ANA_REG_177,
    ANA_REG_178,
    ANA_REG_179,
    ANA_REG_17A,
    ANA_REG_17B,
    ANA_REG_17C,
    ANA_REG_17D,
    ANA_REG_17E,
    ANA_REG_17F,

    ANA_REG_RF_C4 = 0xC4,
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

static OPT_TYPE uint16_t vcodec_mv =
#ifdef VCODEC_1P6V
    1600;
#elif defined(VCODEC_1P8V)
    1800;
#elif defined(VCODEC_1P9V)
    1900;
#elif defined(VCODEC_1P95V)
    1950;
#else // VCODEC_1P7V
    1700;
#endif

static bool ana_spk_req;
static bool ana_spk_muted;
static bool ana_spk_enabled;

static CODEC_USER_MAP adc_map[MAX_ANA_MIC_CH_NUM];
static CODEC_USER_MAP vmic_map[MAX_ANA_MIC_CH_NUM];
static CODEC_USER_MAP codec_common_map;
static CODEC_USER_MAP adda_common_map;
static CODEC_USER_MAP vcodec_map;

static uint8_t ana_aud_pll_map;
STATIC_ASSERT(ANA_AUD_PLL_USER_QTY <= sizeof(ana_aud_pll_map) * 8, "Too many ANA AUD PLL users");

#ifdef ANC_APP
#ifndef DYN_ADC_GAIN
#define DYN_ADC_GAIN
#endif
#endif

#ifdef DYN_ADC_GAIN
static int8_t adc_gain_offset[MAX_ANA_MIC_CH_NUM];
#endif

static const int8_t adc_db[] = { -9, -6, -3, 0, 3, 6, 9, 12, };

static const int8_t tgt_adc_db[MAX_ANA_MIC_CH_NUM] = {
    ANALOG_ADC_A_GAIN_DB, ANALOG_ADC_B_GAIN_DB,
    ANALOG_ADC_C_GAIN_DB, ANALOG_ADC_D_GAIN_DB,
    ANALOG_ADC_E_GAIN_DB,
};

// Max allowed total delta is +/-5M offset (11961ppm for 418M)
#define MAX_ALLOWED_TOTAL_DELTA             ((uint64_t)(1 << 28) * 5) / 26

static struct ANALOG_PLL_CFG_T ana_pll_cfg[2];
static int pll_cfg_idx;

#ifdef __AUDIO_RESAMPLE__
static ANA_AUD_PLL_TUNE_HANDLER pll_tune_hdlr;
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

        PLL_cfg_val = ((uint64_t)(1 << 28) * freq * div * 2 + 26000000 / 2) / 26000000;
        ana_pll_cfg[pll_cfg_idx].val[0] = PLL_cfg_val & 0xFFFF;
        ana_pll_cfg[pll_cfg_idx].val[1] = (PLL_cfg_val >> 16) & 0xFFFF;
        ana_pll_cfg[pll_cfg_idx].val[2] = (PLL_cfg_val >> 32) & 0xFFFF;
    }

    analog_read(ANA_REG_77, &val);
    val = SET_BITFIELD(val, REG_AUDPLL_DIVN, div);
    analog_write(ANA_REG_77, val);

    analog_write(ANA_REG_7C, ana_pll_cfg[pll_cfg_idx].val[0]);
    analog_write(ANA_REG_7B, ana_pll_cfg[pll_cfg_idx].val[1]);
    analog_read(ANA_REG_7A, &val);
    val = SET_BITFIELD(val, REG_AUDPLL_FREQ_34_32, ana_pll_cfg[pll_cfg_idx].val[2]) | REG_AUDPLL_FREQ_EN;
    analog_write(ANA_REG_7A, val);
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

        if (new_low != low) {
            ana_pll_cfg[pll_cfg_idx].val[0] = new_low;
            analog_write(ANA_REG_7C, new_low);
        }

        if (new_high != high) {
            ana_pll_cfg[pll_cfg_idx].val[1] = new_high;
            analog_write(ANA_REG_7B, new_high);
        }

        if (bit34_32 != new_bit34_32) {
            ana_pll_cfg[pll_cfg_idx].val[2] = new_bit34_32;
        }

        analog_read(ANA_REG_7A, &val);
        val = SET_BITFIELD(val, REG_AUDPLL_FREQ_34_32, new_bit34_32) | REG_AUDPLL_FREQ_EN;
        analog_write(ANA_REG_7A, val);
    }
}

void analog_aud_pll_open(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {
        uint16_t val;

        analog_read(ANA_REG_RF_C4, &val);
        val |= CODEC_RESAMPLE_CLK_BUF_PU;
        analog_write(ANA_REG_RF_C4, val);
        analog_read(ANA_REG_7A, &val);
        val |= REG_PU_OSC;
        analog_write(ANA_REG_7A, val);
        analog_read(ANA_REG_75, &val);
        val |= REG_CRYSTAL_SEL_LV;
        analog_write(ANA_REG_75, val);
#if 0
        analog_read(ANA_REG_174, &val);
        val |= CFG_TX_CLK_INV;
        analog_write(ANA_REG_174, val);
#endif
        return;
    }
#endif

    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
    ana_aud_pll_map |= (1 << user);
}

void analog_aud_pll_close(enum ANA_AUD_PLL_USER_T user)
{
    if (user >= ANA_AUD_PLL_USER_QTY) {
        return;
    }

#ifdef __AUDIO_RESAMPLE__
    if (user == ANA_AUD_PLL_USER_CODEC &&
            hal_cmu_get_audio_resample_status()) {
        uint16_t val;

#if 0
        analog_read(ANA_REG_174, &val);
        val &= ~CFG_TX_CLK_INV;
        analog_write(ANA_REG_174, val);
#endif
        analog_read(ANA_REG_75, &val);
        val &= ~REG_CRYSTAL_SEL_LV;
        analog_write(ANA_REG_75, val);
        analog_read(ANA_REG_7A, &val);
        val &= ~REG_PU_OSC;
        analog_write(ANA_REG_7A, val);
        analog_read(ANA_REG_RF_C4, &val);
        val &= ~CODEC_RESAMPLE_CLK_BUF_PU;
        analog_write(ANA_REG_RF_C4, val);
        return;
    }
#endif

    ana_aud_pll_map &= ~(1 << user);
    if (ana_aud_pll_map == 0) {
        hal_cmu_pll_disable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_AUD);
    }
}

static void analog_aud_enable_dac(uint32_t dac)
{
}

static void analog_aud_enable_dac_pa(uint32_t dac)
{
    uint16_t val_6e;
    uint16_t val_70;
    uint16_t val_73;

    val_73 = 0;

    analog_read(ANA_REG_6E, &val_6e);
    analog_read(ANA_REG_70, &val_70);

    if (dac & (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)) {
        val_6e |= REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS;
        val_70 |= CFG_TX_TREE_EN;
        if (dac & AUD_CHANNEL_MAP_CH0) {
            val_73 |= REG_CODEC_TX_EN_EARPA_L | REG_CODEC_TX_EN_LCLK | REG_CODEC_TX_EN_LDAC;
        }
        if (dac & AUD_CHANNEL_MAP_CH1) {
            val_73 |= REG_CODEC_TX_EN_EARPA_R | REG_CODEC_TX_EN_RCLK | REG_CODEC_TX_EN_RDAC;
        }
        val_73 |= REG_CODEC_TX_EN_DACLDO | REG_CODEC_TX_EN_LPPA | REG_CODEC_TX_EN_S1PA;
    } else {
        val_6e &= ~(REG_CODEC_TX_EAR_DR_EN | REG_CODEC_TX_EAR_ENBIAS);
        val_70 &= ~CFG_TX_TREE_EN;
    }

    analog_write(ANA_REG_6E, val_6e);
    analog_write(ANA_REG_70, val_70);
    analog_write(ANA_REG_73, val_73);

    if (val_73) {
        osDelay(1);
        val_73 |= REG_CODEC_TX_EN_S4PA;
        analog_write(ANA_REG_73, val_73);
    }
}

static void analog_aud_enable_adc(enum ANA_CODEC_USER_T user, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    int i;
    uint16_t val_61, val_6c;
    CODEC_USER_MAP old_map;
    bool set;
    bool global_update;

    ANALOG_TRACE("[%s] user=%d ch_map=0x%x", __func__, user, ch_map);

    global_update = false;

    analog_read(ANA_REG_61, &val_61);

    for (i = 0; i < MAX_ANA_MIC_CH_NUM - 1; i++) {
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
                global_update = true;
                if (adc_map[i]) {
                    val_61 |= (REG_CODEC_EN_ADCA << i);
                } else {
                    val_61 &= ~(REG_CODEC_EN_ADCA << i);
                }
            }
        }
    }

    analog_write(ANA_REG_61, val_61);

    if (ch_map & (AUD_CHANNEL_MAP_CH4)) {
        // TODO: Enable/disable GMC
        // global_update = ?
    }

    if (global_update) {
        for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
            if (adc_map[i]) {
                break;
            }
        }
        analog_read(ANA_REG_6C, &val_6c);
        if (i < MAX_ANA_MIC_CH_NUM) {
            val_6c |= REG_CODEC_RX_EN_VTOI;
        } else {
            val_6c &= ~REG_CODEC_RX_EN_VTOI;
        }
        analog_write(ANA_REG_6C, val_6c);
    }
}

static uint32_t db_to_adc_gain(int db)
{
    int i;
    uint8_t cnt;
    const int8_t *list;

    list = adc_db;
    cnt = ARRAY_SIZE(adc_db);

    for (i = 0; i < cnt - 1; i++) {
        if (db < list[i + 1]) {
            break;
        }
    }

    if (i == cnt - 1) {
        return i;
    }
    else if (db * 2 < list[i] + list[i + 1]) {
        return i;
    } else {
        return i + 1;
    }
}

static uint16_t get_chan_adc_gain(uint32_t i)
{
    uint16_t gain;

    gain = tgt_adc_db[i];

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
    int gain;
    uint16_t gain_val;
    uint16_t val;

    analog_read(ANA_REG_63, &val);

    for (i = 0; i < MAX_ANA_MIC_CH_NUM - 1; i++) {
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
                gain = LINEIN_ADC_GAIN_DB;
            } else {
                gain = get_chan_adc_gain(i);
            }
            gain_val = db_to_adc_gain(gain);
            val = (val & ~(REG_CODEC_ADCA_GAIN_BIT_MASK << 3 * i)) | (REG_CODEC_ADCA_GAIN_BIT(gain_val) << 3 * i);
        }
    }

    analog_write(ANA_REG_63, val);

    if (ch_map & (AUD_CHANNEL_MAP_CH4)) {
        // TODO: Set GMC gain
    }
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

    // qdb to db
    offset_l /= 4;
    offset_r /= 4;

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

void analog_aud_set_dac_gain(int32_t v)
{
}

uint32_t analog_codec_get_dac_gain(void)
{
    return 0;
}

uint32_t analog_codec_dac_gain_to_db(int32_t gain)
{
    return 0;
}

int32_t analog_codec_dac_max_attn_db(void)
{
    return 0;
}

#ifdef ANC_APP
void analog_aud_apply_anc_dac_gain_offset(int16_t offset)
{
}
#endif

static int POSSIBLY_UNUSED dc_calib_checksum_valid(uint32_t efuse)
{
    int i;
    uint32_t cnt = 0;

    for (i = 0; i < 14; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & 0xF) == ((efuse >> 14) & 0xF));
}

void analog_aud_get_dc_calib_value(int16_t *dc_l, int16_t *dc_r)
{
    static const uint8_t EFUSE_PAGE_DIFF[2] = { PMU_EFUSE_PAGE_DCCALIB2_L, PMU_EFUSE_PAGE_DCCALIB2_R, };
    static const uint8_t EFUSE_PAGE_CHK = PMU_EFUSE_PAGE_DCCALIB_L;
    const uint8_t *page;
    uint16_t efuse;
    uint16_t check_sum;

    union DC_EFUSE_T {
        struct DC_VALUE_T {
            int16_t dc          : 14;
            uint16_t checksum   : 2;
        } val;
        uint16_t reg;
    };
    union DC_EFUSE_T dc;

    pmu_get_efuse(EFUSE_PAGE_CHK, &check_sum);

    page = EFUSE_PAGE_DIFF;

    pmu_get_efuse(page[0], &efuse);
    if (dc_calib_checksum_valid(efuse | ((check_sum & 0x3) << 16))) {
        TRACE("Dc calib L OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_l = dc.val.dc;
    } else {
        TRACE("Warning: Bad dc calib efuse L: 0x%04x", efuse);
        *dc_l = 0;
    }

    pmu_get_efuse(page[1], &efuse);
    if (dc_calib_checksum_valid(efuse | (((check_sum >> 2) & 0x3) << 16))) {
        TRACE("Dc calib R OK: 0x%04x", efuse);
        dc.reg = efuse;
        *dc_r = dc.val.dc;
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

    analog_read(ANA_REG_71, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITL, dc_l);
    analog_write(ANA_REG_71, val);

    analog_read(ANA_REG_72, &val);
    val = SET_BITFIELD(val, REG_CODEC_TX_EAR_OFF_BITR, dc_r);
    analog_write(ANA_REG_72, val);
}

static void analog_aud_dc_calib_enable(bool en)
{
    uint16_t val;

    analog_read(ANA_REG_70, &val);
    val |= REG_CODEC_TX_EAR_OFFEN;
    analog_write(ANA_REG_70, val);
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

static void analog_aud_enable_codec_common(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val_6b, val_74;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (codec_common_map == 0) {
            set = true;
        }
        codec_common_map |= (1 << user);
    } else {
        codec_common_map &= ~(1 << user);
        if (codec_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_6B, &val_6b);
        analog_read(ANA_REG_74, &val_74);
        val_6b &= ~(REG_CODEC_EN_VCM | REG_CODEC_VCM_EN_LPF);
        val_74 &= ~(REG_PU_TX_REGULATOR | REG_BYPASS_TX_REGULATOR);
        if (codec_common_map) {
            val_6b |= REG_CODEC_EN_VCM;
            if (vcodec_mv >= 1800) {
                val_74 |= REG_PU_TX_REGULATOR;
            } else {
                val_74 |= REG_BYPASS_TX_REGULATOR;
            }
        }
        analog_write(ANA_REG_74, val_74);
        analog_write(ANA_REG_6B, val_6b);
        if (val_6b & REG_CODEC_EN_VCM) {
            // Requires 60ms stable time
            // TODO: Set to 60 when using 32K derived from 26M
            osDelay(80);
            val_6b |= REG_CODEC_VCM_EN_LPF;
            analog_write(ANA_REG_6B, val_6b);
        }
    }
}

static void analog_aud_enable_adda_common(enum ANA_CODEC_USER_T user, bool en)
{
    uint32_t lock;
    uint16_t val_6a, val_6b, val_7e;
    bool set = false;

    lock = int_lock();
    if (en) {
        if (adda_common_map == 0) {
            set = true;
        }
        adda_common_map |= (1 << user);
    } else {
        adda_common_map &= ~(1 << user);
        if (adda_common_map == 0) {
            set = true;
        }
    }
    int_unlock(lock);

    if (set) {
        analog_read(ANA_REG_6A, &val_6a);
        analog_read(ANA_REG_6B, &val_6b);
        analog_read(ANA_REG_7E, &val_7e);
        if (adda_common_map) {
            val_6a |= REG_CODEC_EN_BIAS;
            val_6b |= REG_CODEC_EN_VCM_BUFFER;
            val_7e |= REG_PU_CLKMUX_LDO0P9;
        } else {
            val_6a &= ~REG_CODEC_EN_BIAS;
            val_6b &= ~REG_CODEC_EN_VCM_BUFFER;
            val_7e &= ~REG_PU_CLKMUX_LDO0P9;
        }
        analog_write(ANA_REG_6A, val_6a);
        analog_write(ANA_REG_6B, val_6b);
        analog_write(ANA_REG_7E, val_7e);
    }
}

static void analog_aud_enable_vmic(enum ANA_CODEC_USER_T user, uint32_t dev)
{
    uint32_t lock;
    CODEC_USER_MAP old_map;
    bool set = false;
    int i;
    uint32_t pmu_map = 0;

    lock = int_lock();

    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (dev & (AUD_VMIC_MAP_VMIC1 << i)) {
            if (vmic_map[i] == 0) {
                set = true;
            }
            vmic_map[i] |= (1 << user);
        } else {
            old_map = vmic_map[i];
            vmic_map[i] &= ~(1 << user);
            if (old_map != 0 && vmic_map[i] == 0) {
                set = true;
            }
        }

        if (vmic_map[i]) {
            pmu_map |= (AUD_VMIC_MAP_VMIC1 << i);
        }
    }

    int_unlock(lock);

    if (set) {
        pmu_codec_mic_bias_enable(pmu_map);
        if (pmu_map) {
            osDelay(1);
        }
    }
}

uint32_t analog_aud_get_max_dre_gain(void)
{
    if (vcodec_mv >= 1900) {
        return 0x11;
    } else {
        return 0xF;
    }
}

void analog_open(void)
{
    uint16_t val;
    uint16_t field;

    val = REG_CODEC_ADCA_CH_SEL(1) | REG_CODEC_ADCB_CH_SEL(1) | REG_CODEC_ADCC_CH_SEL(1) | REG_CODEC_ADCD_CH_SEL(1);
    analog_write(ANA_REG_61, val);

    val = 0; //REG_CODEC_ADCA_GAIN_UPDATE | REG_CODEC_ADCB_GAIN_UPDATE | REG_CODEC_ADCC_GAIN_UPDATE | REG_CODEC_ADCD_GAIN_UPDATE;
    analog_write(ANA_REG_63, val);

    val = REG_CODEC_ADC_IBSEL_REG(8) | REG_CODEC_ADC_IBSEL_VCOMP(8) | REG_CODEC_ADC_IBSEL_VREF(8) | REG_CODEC_ADC_LPFVCM_SW(2);
    analog_write(ANA_REG_67, val);

    val = REG_CODEC_ADC_OP1_IBIT(2) | REG_CODEC_ADC_VREF_SEL(4);
    analog_write(ANA_REG_68, val);

    val = REG_CODEC_BIAS_IBSEL(8) | REG_CODEC_BIAS_IBSEL_VOICE(8);
    if (vcodec_mv >= 1900) {
        val |= REG_CODEC_BIAS_IBSEL_TX(5);
    } else {
        val |= REG_CODEC_BIAS_IBSEL_TX(8);
    }
    analog_write(ANA_REG_69, val);

    val = REG_CODEC_ADC_REG_VSEL(3) | REG_CODEC_BUF_LOWVCM(4) | REG_CODEC_EN_RX_EXT | REG_CODEC_EN_TX_EXT | REG_CODEC_DAC_CLK_EDGE_SEL;
    analog_write(ANA_REG_6A, val);

    if (vcodec_mv >= 1900) {
        field = 0xB;
    } else {
        field = 0xC;
    }
    val = REG_CODEC_VCM_LOW_VCM(field) | REG_CODEC_VCM_LOW_VCM_LP(field) | REG_CODEC_VCM_LOW_VCM_LPF(field);
    analog_write(ANA_REG_6B, val);

    val = REG_CODEC_RX_VTOI_I_DAC2(4) | REG_CODEC_RX_VTOI_IDAC_SEL(8) | REG_CODEC_RX_VTOI_VCS_SEL(0x10);
    analog_write(ANA_REG_6C, val);

    val = REG_CODEC_TX_EAR_CAS_BIT(1);
    if (vcodec_mv == 1900) {
        val |= REG_CODEC_TX_DAC_VREF_L(0xA) | REG_CODEC_TX_DAC_VREF_R(0xA);
    } else if (vcodec_mv > 1900) {
        val |= REG_CODEC_TX_DAC_VREF_L(9) | REG_CODEC_TX_DAC_VREF_R(9);
    } else {
        val |= REG_CODEC_TX_DAC_VREF_L(2) | REG_CODEC_TX_DAC_VREF_R(2);
    }
    analog_write(ANA_REG_6D, val);

    val = REG_CODEC_TX_EAR_COMP_L(7) | REG_CODEC_TX_EAR_COMP_R(7) | REG_CODEC_TX_EAR_DR_ST(1) | REG_CODEC_TX_EAR_FBCAP(3) | REG_CODEC_TX_EAR_IBSEL(1);
    analog_write(ANA_REG_6E, val);

    val = REG_CODEC_TX_EAR_DRE_GAIN_L(0xF) | REG_CODEC_TX_EAR_DRE_GAIN_R(0xF) | DRE_GAIN_SEL_L | DRE_GAIN_SEL_R |
        REG_CODEC_TX_EAR_DRE_GAIN_L_UPDATE | REG_CODEC_TX_EAR_DRE_GAIN_R_UPDATE | REG_CODEC_TX_EAR_GAIN(1);
    analog_write(ANA_REG_6F, val);

    val = REG_CODEC_TX_EAR_OUTPUTSEL(1) | REG_CODEC_TX_EAR_SOFTSTART(8);
    analog_write(ANA_REG_70, val);

    val = REG_DAC_LDO0P9_VSEL(2) | REG_CODEC_TX_EAR_VCM_BIT(3) | REG_CODEC_TX_EAR_VCM_SEL;
    if (vcodec_mv >= 1900) {
        val |= REG_CODEC_TX_RVREF_CAP_BIT | REG_TX_REGULATOR_BIT(0xD);
    } else {
        val |= REG_TX_REGULATOR_BIT(4);
    }
    analog_write(ANA_REG_74, val);

    val = REG_CLKMUX_DVDD_SEL | REG_CLKMUX_LDO0P9_VSEL(2) | REG_AUDPLL_LDO_VREF(4) | REG_AUDPLL_LPF_BW_SEL;
    analog_write(ANA_REG_7E, val);

#ifdef AUDIO_OUTPUT_DC_CALIB_ANA
    analog_aud_dc_calib_init();
    analog_aud_dc_calib_enable(true);
#endif
}

void analog_sleep(void)
{
}

void analog_wakeup(void)
{
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
        analog_aud_enable_codec_common(user, true);
        analog_aud_enable_adda_common(user, true);
        analog_aud_enable_vmic(user, CFG_HW_AUD_INPUT_PATH_ANC_DEV);
        analog_aud_set_adc_gain(AUD_INPUT_PATH_MAINMIC, ch_map);
        analog_aud_enable_adc(user, ch_map, true);
    } else {
        analog_aud_enable_adc(user, ch_map, false);
        analog_aud_enable_vmic(user, 0);
        analog_aud_enable_adda_common(user, false);
        analog_aud_enable_codec_common(user, false);
    }
}

void analog_aud_codec_anc_init_close(void)
{
}

void analog_aud_mickey_enable(bool en)
{
    if (en) {
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, true);
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, CFG_HW_AUD_MICKEY_DEV);
    } else {
        analog_aud_enable_vmic(ANA_CODEC_USER_MICKEY, 0);
        analog_aud_enable_codec_common(ANA_CODEC_USER_MICKEY, false);
        analog_aud_vcodec_enable(ANA_CODEC_USER_MICKEY, false);
    }
}

void analog_aud_codec_adc_enable(enum AUD_IO_PATH_T input_path, enum AUD_CHANNEL_MAP_T ch_map, bool en)
{
    uint32_t dev;

    if (en) {
        analog_aud_enable_codec_common(ANA_CODEC_USER_ADC, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_ADC, true);
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
        analog_aud_enable_adda_common(ANA_CODEC_USER_ADC, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_ADC, false);
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
        } else {
            analog_aud_enable_dac_pa(0);
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
    if (en) {
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, true);
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, true);
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
        analog_aud_enable_adda_common(ANA_CODEC_USER_DAC, false);
        analog_aud_enable_codec_common(ANA_CODEC_USER_DAC, false);
    }
}

void analog_aud_codec_open(void)
{
    analog_aud_vcodec_enable(ANA_CODEC_USER_CODEC, true);

#ifdef _AUTO_SWITCH_POWER_MODE__
    //pmu_mode_change(PMU_POWER_MODE_DIG_DCDC);
#endif
}

void analog_aud_codec_close(void)
{
    static const enum AUD_CHANNEL_MAP_T all_ch = AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1 |
        AUD_CHANNEL_MAP_CH2 | AUD_CHANNEL_MAP_CH3 | AUD_CHANNEL_MAP_CH4;

    analog_aud_codec_speaker_enable(false);
    osDelay(1);
    analog_aud_codec_dac_enable(false);

    analog_aud_codec_adc_enable(AUD_IO_PATH_NULL, all_ch, false);

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
    return 0;
}

