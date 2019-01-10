#ifndef __REG_CODEC_BEST2000_H__
#define __REG_CODEC_BEST2000_H__

#include "plat_types.h"

struct CODEC_T {
    __IO uint32_t CONFIG;                   // 0x0000
    __IO uint32_t FIFO_FLUSH;               // 0x0004
    __IO uint32_t FIFO_ACCESS;              // 0x0008
    __IO uint32_t FIFO_THRESHOLD;           // 0x000C
    __IO uint32_t ISR;                      // 0x0010
    __IO uint32_t IMR;                      // 0x0014
    __IO uint32_t FIFO_RX_COUNT;            // 0x0018
    __IO uint32_t FIFO_TX_COUNT;            // 0x001C
    __IO uint32_t ADC_FIFO_ACCESS[8];       // 0x0020
    __IO uint32_t SYNC_PLAY;                // 0x0040
    __IO uint32_t CNT_KEEP;                 // 0x0044
    __IO uint32_t DSD_DATA;                 // 0x0048
    __IO uint32_t CMU_CTRL;                 // 0x004C
    __IO uint32_t SLEEP_CTRL;               // 0x0050
    __IO uint32_t MC_FIFO_ACCESS;           // 0x0054
    __IO uint32_t MC_CONFIG;                // 0x0058
    __IO uint32_t DRE_GAIN_STATUS;          // 0x005C
    __IO uint32_t CMU_CTRL2;                // 0x0060
         uint32_t RESERVED_0064[0x1C / 4];  // 0x0064
    __IO uint32_t REG32B_03;                // 0x0080
    __IO uint32_t REG32B_04;                // 0x0084
    __IO uint32_t REG32B_05;                // 0x0088
    __IO uint32_t REG32B_06;                // 0x008C
    __IO uint32_t REG32B_07;                // 0x0090
    __IO uint32_t REG32B_08;                // 0x0094
    __IO uint32_t REG32B_09;                // 0x0098
    __IO uint32_t REG32B_10;                // 0x009C
    __IO uint32_t REG32B_11;                // 0x00A0
    __IO uint32_t REG32B_12;                // 0x00A4
    __IO uint32_t REG32B_13;                // 0x00A8
    __IO uint32_t REG32B_14;                // 0x00AC
    __IO uint32_t REG32B_15;                // 0x00B0
    __IO uint32_t REG32B_16;                // 0x00B4
    __IO uint32_t REG32B_17;                // 0x00B8
    __IO uint32_t REG32B_18;                // 0x00BC
    __IO uint32_t REG32B_19;                // 0x00C0
    __IO uint32_t REG32B_20;                // 0x00C4
    __IO uint32_t REG32B_21;                // 0x00C8
    __IO uint32_t REG32B_22;                // 0x00CC
    __IO uint32_t REG32B_23;                // 0x00D0
    __IO uint32_t REG32B_24;                // 0x00D4
    __IO uint32_t REG32B_25;                // 0x00D8
    __IO uint32_t REG32B_26;                // 0x00DC
    __IO uint32_t REG32B_27;                // 0x00E0
    __IO uint32_t REG32B_28;                // 0x00E4
    __IO uint32_t REG32B_29;                // 0x00E8
    __IO uint32_t REG32B_30;                // 0x00EC
    __IO uint32_t REG32B_31;                // 0x00F0
    __IO uint32_t REG32B_32;                // 0x00F4
    __IO uint32_t REG32B_33;                // 0x00F8
    __IO uint32_t REG32B_34;                // 0x00FC
    __IO uint32_t FIR_CONFIG;               // 0x0100
    struct {
        __IO uint32_t CFG0;                 // 0x0104
        __IO uint32_t CFG1;                 // 0x0108
    } FIR_CH_CONFIG[8];
    __IO uint32_t FIR_CONFIG1;              // 0x0144
         uint32_t RESERVED_0148[0x20 / 4];  // 0x0148
    __IO uint32_t REG32B_35;                // 0x0168
    __IO uint32_t REG32B_36;                // 0x016C
    __IO uint32_t REG32B_37;                // 0x0170
    __IO uint32_t REG32B_38;                // 0x0174
    __IO uint32_t REG32B_39;                // 0x0178
    __IO uint32_t REG32B_40;                // 0x017C
    __IO uint32_t REG32B_41;                // 0x0180
    __IO uint32_t REG32B_42;                // 0x0184
    __IO uint32_t REG32B_43;                // 0x0188
    __IO uint32_t REG32B_44;                // 0x018C
    __IO uint32_t REG32B_45;                // 0x0190
    __IO uint32_t REG32B_46;                // 0x0194
    __IO uint32_t REG32B_47;                // 0x0198
    __IO uint32_t REG32B_48;                // 0x019C
    __IO uint32_t REG32B_49;                // 0x01A0
    __IO uint32_t REG32B_50;                // 0x01A4
    __IO uint32_t REG32B_51;                // 0x01A8
    __IO uint32_t REG32B_52;                // 0x01AC
    __IO uint32_t REG32B_53;                // 0x01B0
    __IO uint32_t REG32B_54;                // 0x01B4
    __IO uint32_t REG32B_55;                // 0x01B8
    __IO uint32_t REG32B_56;                // 0x01BC
    __IO uint32_t REG32B_57;                // 0x01C0
};

// CONFIG
#define CODEC_CFG_MC_ENABLE                 (1 << 30)
#define CODEC_CFG_MSB_FIRST                 (1 << 29)
#define CODEC_CFG_DSD_DUAL_CHANNEL          (1 << 28)
#define CODEC_CFG_DSD_IF_EN                 (1 << 27)
#define CODEC_CFG_DMA_CTRL_TX_DSD           (1 << 26)
#define CODEC_CFG_DMA_CTRL_RX_DSD           (1 << 25)
#define CODEC_CFG_DSD_ENABLE                (1 << 24)
#define CODEC_CFG_ADC_ENABLE                (1 << 23)
#define CODEC_CFG_MODE_16BIT_ADC_CH7        (1 << 22)
#define CODEC_CFG_MODE_16BIT_ADC_CH6        (1 << 21)
#define CODEC_CFG_MODE_16BIT_ADC_CH5        (1 << 20)
#define CODEC_CFG_MODE_16BIT_ADC_CH4        (1 << 19)
#define CODEC_CFG_MODE_16BIT_ADC_CH3        (1 << 18)
#define CODEC_CFG_MODE_16BIT_ADC_CH2        (1 << 17)
#define CODEC_CFG_MODE_16BIT_ADC_CH1        (1 << 16)
#define CODEC_CFG_MODE_16BIT_ADC_CH0        (1 << 15)
#define CODEC_CFG_MODE_16BIT_DAC            (1 << 14)
#define CODEC_CFG_DAC_EXCHANGE_L_R          (1 << 13)
#define CODEC_CFG_DUAL_CHANNEL_DAC          (1 << 12)
#define CODEC_CFG_DMACTRL_TX                (1 << 11)
#define CODEC_CFG_DMACTRL_RX                (1 << 10)
#define CODEC_CFG_DAC_ENABLE                (1 << 9)
#define CODEC_CFG_ADC_ENABLE_CH7            (1 << 8)
#define CODEC_CFG_ADC_ENABLE_CH6            (1 << 7)
#define CODEC_CFG_ADC_ENABLE_CH5            (1 << 6)
#define CODEC_CFG_ADC_ENABLE_CH4            (1 << 5)
#define CODEC_CFG_ADC_ENABLE_CH3            (1 << 4)
#define CODEC_CFG_ADC_ENABLE_CH2            (1 << 3)
#define CODEC_CFG_ADC_ENABLE_CH1            (1 << 2)
#define CODEC_CFG_ADC_ENABLE_CH0            (1 << 1)
#define CODEC_CFG_ADC_ENABLE_CH_ALL         (0xFF << 1)
#define CODEC_CFG_CODEC_IF_EN               (1 << 0)

// FIFO_FLUSH
#define CODEC_FIFO_FLUSH_DSD_TX_FIFO_FLUSH  (1 << 10)
#define CODEC_FIFO_FLUSH_DSD_RX_FIFO_FLUSH  (1 << 9)
#define CODEC_FIFO_FLUSH_TX_FIFO_FLUSH      (1 << 8)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH7  (1 << 7)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH6  (1 << 6)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH5  (1 << 5)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH4  (1 << 4)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH3  (1 << 3)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH2  (1 << 2)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH1  (1 << 1)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_CH0  (1 << 0)
#define CODEC_FIFO_FLUSH_RX_FIFO_FLUSH_ALL  (0xFF << 0)

// FIFO_THRESHOLD
#define CODEC_FIFO_THRESH_MC_SHIFT          15
#define CODEC_FIFO_THRESH_MC_MASK           (0xF << CODEC_FIFO_THRESH_MC_SHIFT)
#define CODEC_FIFO_THRESH_MC(n)             BITFIELD_VAL(CODEC_FIFO_THRESH_MC, n)
#define CODEC_FIFO_THRESH_TX_DSD_SHIFT      12
#define CODEC_FIFO_THRESH_TX_DSD_MASK       (0x7 << CODEC_FIFO_THRESH_TX_DSD_SHIFT)
#define CODEC_FIFO_THRESH_TX_DSD(n)         BITFIELD_VAL(CODEC_FIFO_THRESH_TX_DSD, n)
#define CODEC_FIFO_THRESH_RX_DSD_SHIFT      8
#define CODEC_FIFO_THRESH_RX_DSD_MASK       (0xF << CODEC_FIFO_THRESH_RX_DSD_SHIFT)
#define CODEC_FIFO_THRESH_RX_DSD(n)         BITFIELD_VAL(CODEC_FIFO_THRESH_RX_DSD, n)
#define CODEC_FIFO_THRESH_TX_SHIFT          4
#define CODEC_FIFO_THRESH_TX_MASK           (0xF << CODEC_FIFO_THRESH_TX_SHIFT)
#define CODEC_FIFO_THRESH_TX(n)             BITFIELD_VAL(CODEC_FIFO_THRESH_TX, n)
#define CODEC_FIFO_THRESH_RX_SHIFT          0
#define CODEC_FIFO_THRESH_RX_MASK           (0xF << CODEC_FIFO_THRESH_RX_SHIFT)
#define CODEC_FIFO_THRESH_RX(n)             BITFIELD_VAL(CODEC_FIFO_THRESH_RX, n)

// ISR
#define CODEC_ISR_REV_SHIFT                 29
#define CODEC_ISR_REV_MASK                  (0x7 << CODEC_ISR_REV_SHIFT)
#define CODEC_ISR_REV(n)                    BITFIELD_VAL(CODEC_ISR_REV, n)
#define CODEC_ISR_FEEDBACK_ERROR_TRIG_CH1   (1 << 28)
#define CODEC_ISR_FEEDBACK_ERROR_TRIG_CH0   (1 << 27)
#define CODEC_ISR_RESERVED_SHIFT            25
#define CODEC_ISR_RESERVED_MASK             (0x3 << CODEC_ISR_RESERVED_SHIFT)
#define CODEC_ISR_RESERVED(n)               BITFIELD_VAL(CODEC_ISR_RESERVED, n)
#define CODEC_ISR_MC_UNDERFLOW              (1 << 24)
#define CODEC_ISR_MC_OVERFLOW               (1 << 23)
#define CODEC_ISR_DSD_TX_UNDERFLOW          (1 << 22)
#define CODEC_ISR_DSD_TX_OVERFLOW           (1 << 21)
#define CODEC_ISR_DSD_RX_UNDERFLOW          (1 << 20)
#define CODEC_ISR_DSD_RX_OVERFLOW           (1 << 19)
#define CODEC_ISR_TSF_TRIGGER               (1 << 18)
#define CODEC_ISR_TX_UNDERFLOW              (1 << 17)
#define CODEC_ISR_TX_OVERFLOW               (1 << 16)
#define CODEC_ISR_RX_UNDERFLOW_SHIFT        8
#define CODEC_ISR_RX_UNDERFLOW_MASK         (0xFF << CODEC_ISR_RX_UNDERFLOW_SHIFT)
#define CODEC_ISR_RX_UNDERFLOW(n)           BITFIELD_VAL(CODEC_ISR_RX_UNDERFLOW, n)
#define CODEC_ISR_RX_OVERFLOW_SHIFT         0
#define CODEC_ISR_RX_OVERFLOW_MASK          (0xFF << CODEC_ISR_RX_OVERFLOW_SHIFT)
#define CODEC_ISR_RX_OVERFLOW(n)            BITFIELD_VAL(CODEC_ISR_RX_OVERFLOW, n)

// IMR
#define CODEC_IMR_REV_SHIFT                 27
#define CODEC_IMR_REV_MASK                  (0x1F << CODEC_IMR_REV_SHIFT)
#define CODEC_IMR_REV(n)                    BITFIELD_VAL(CODEC_IMR_REV, n)
#define CODEC_IMR_BT_INT_MASK               (1 << 26)
#define CODEC_IMR_MCU_INT_MASK              (1 << 25)
#define CODEC_IMR_MC_UNDERFLOW_MASK         (1 << 24)
#define CODEC_IMR_MC_OVERFLOW_MASK          (1 << 23)
#define CODEC_IMR_DSD_TX_UNDERFLOW_MASK     (1 << 22)
#define CODEC_IMR_DSD_TX_OVERFLOW_MASK      (1 << 21)
#define CODEC_IMR_DSD_RX_UNDERFLOW_MASK     (1 << 20)
#define CODEC_IMR_DSD_RX_OVERFLOW_MASK      (1 << 19)
#define CODEC_IMR_TSF_TRIGGER_MASK          (1 << 18)
#define CODEC_IMR_TX_UNDERFLOW_MASK         (1 << 17)
#define CODEC_IMR_TX_OVERFLOW_MASK          (1 << 16)
#define CODEC_IMR_RX_UNDERFLOW_MASK_SHIFT   8
#define CODEC_IMR_RX_UNDERFLOW_MASK_MASK    (0xFF << CODEC_IMR_RX_UNDERFLOW_MASK_SHIFT)
#define CODEC_IMR_RX_UNDERFLOW_MASK(n)      BITFIELD_VAL(CODEC_IMR_RX_UNDERFLOW_MASK, n)
#define CODEC_IMR_RX_OVERFLOW_MASK_SHIFT    0
#define CODEC_IMR_RX_OVERFLOW_MASK_MASK     (0xFF << CODEC_IMR_RX_OVERFLOW_MASK_SHIFT)
#define CODEC_IMR_RX_OVERFLOW_MASK(n)       BITFIELD_VAL(CODEC_IMR_RX_OVERFLOW_MASK, n)

// FIFO_RX_COUNT
#define CODEC_FIFO_RX_COUNT_SHIFT(i)        (4 * ((i) & 0x7))
#define CODEC_FIFO_RX_COUNT_MASK(i)         (0xF << CODEC_FIFO_RX_COUNT_SHIFT(i))
#define CODEC_FIFO_RX_COUNT(i, n)           (((n) & 0xF) << CODEC_FIFO_RX_COUNT_SHIFT(i))

// FIFO_TX_COUNT
#define CODEC_FIFO_MC_FIFO_COUNT_SHIFT      20
#define CODEC_FIFO_MC_FIFO_COUNT_MASK       (0xF << CODEC_FIFO_MC_FIFO_COUNT_SHIFT)
#define CODEC_FIFO_MC_FIFO_COUNT(n)         BITFIELD_VAL(CODEC_FIFO_MC_FIFO_COUNT, n)
#define CODEC_FIFO_DSD_TX_FIFO_COUNT_SHIFT  17
#define CODEC_FIFO_DSD_TX_FIFO_COUNT_MASK   (0x7 << CODEC_FIFO_DSD_TX_FIFO_COUNT_SHIFT)
#define CODEC_FIFO_DSD_TX_FIFO_COUNT(n)     BITFIELD_VAL(CODEC_FIFO_DSD_TX_FIFO_COUNT, n)
#define CODEC_FIFO_DSD_RX_FIFO_COUNT_SHIFT  13
#define CODEC_FIFO_DSD_RX_FIFO_COUNT_MASK   (0xF << CODEC_FIFO_DSD_RX_FIFO_COUNT_SHIFT)
#define CODEC_FIFO_DSD_RX_FIFO_COUNT(n)     BITFIELD_VAL(CODEC_FIFO_DSD_RX_FIFO_COUNT, n)
#define CODEC_FIFO_STATE_RX_CH_SHIFT        4
#define CODEC_FIFO_STATE_RX_CH_MASK         (0x1FF << CODEC_FIFO_STATE_RX_CH_SHIFT)
#define CODEC_FIFO_STATE_RX_CH(n)           BITFIELD_VAL(CODEC_FIFO_STATE_RX_CH, n)
#define CODEC_FIFO_TX_FIFO_COUNT_SHIFT      0
#define CODEC_FIFO_TX_FIFO_COUNT_MASK       (0xF << CODEC_FIFO_TX_FIFO_COUNT_SHIFT)
#define CODEC_FIFO_TX_FIFO_COUNT(n)         BITFIELD_VAL(CODEC_FIFO_TX_FIFO_COUNT, n)

// SYNC_PLAY
#define SYNC_CODEC_ADC_ENABLE_SEL_SHIFT     10
#define SYNC_CODEC_ADC_ENABLE_SEL_MASK      (0x3 << SYNC_CODEC_ADC_ENABLE_SEL_SHIFT)
#define SYNC_CODEC_ADC_ENABLE_SEL(n)        BITFIELD_VAL(SYNC_CODEC_ADC_ENABLE_SEL, n)
#define SYNC_CODEC_DAC_ENABLE_SEL_SHIFT     8
#define SYNC_CODEC_DAC_ENABLE_SEL_MASK      (0x3 << SYNC_CODEC_DAC_ENABLE_SEL_SHIFT)
#define SYNC_CODEC_DAC_ENABLE_SEL(n)        BITFIELD_VAL(SYNC_CODEC_DAC_ENABLE_SEL, n)
#define SYNC_GPIO_TRIGGER_DB_ENABLE         (1 << 7)
#define SYNC_ADC_ENABLE_SEL_SHIFT           5
#define SYNC_ADC_ENABLE_SEL_MASK            (0x3 << SYNC_ADC_ENABLE_SEL_SHIFT)
#define SYNC_ADC_ENABLE_SEL(n)              BITFIELD_VAL(SYNC_ADC_ENABLE_SEL, n)
#define SYNC_EVENT_FOR_CAPTURE              (1 << 4)
#define SYNC_EVENT_FOR_DAC_EN               (1 << 3)
#define SYNC_STAMP_CLR_USED                 (1 << 2)
#define SYNC_DAC_ENABLE_SEL_SHIFT           0
#define SYNC_DAC_ENABLE_SEL_MASK            (0x3 << SYNC_DAC_ENABLE_SEL_SHIFT)
#define SYNC_DAC_ENABLE_SEL(n)              BITFIELD_VAL(SYNC_DAC_ENABLE_SEL, n)

// CMU_CTRL
#define CODEC_CMU_TEST_PORT_SEL(i)          (1 << ((i) + 24)
#define CODEC_CMU_CFG_CLK_OUT(i)            (1 << ((i) + 21)
#define CODEC_CMU_POL_DAC_OUT               (1 << 20)
#define CODEC_CMU_POL_ADC_ANA(i)            (1 << ((i) + 15))
#define CODEC_CMU_EN_CLK_DAC                (1 << 14)
#define CODEC_CMU_EN_CLK_ADC(i)             (1 << ((i) + 5))
#define CODEC_CMU_EN_CLK_ADC_ALL            (0x1FF << 5)
#define CODEC_CMU_EN_CLK_ADC_ANA(i)         (1 << (i))
#define CODEC_CMU_EN_CLK_ADC_ANA_ALL        (0x1F << 0)

// SLEEP_CTRL
#define CODEC_SLEEP_IIR_SLEEP_MODE          (1 << 12)
#define CODEC_SLEEP_KEN                     (1 << 11)
#define CODEC_SLEEP_DMA_WITH_BT             (1 << 10)
#define CODEC_SLEEP_EMAW_SHIFT              8
#define CODEC_SLEEP_EMAW_MASK               (0x3 << CODEC_SLEEP_EMAW_SHIFT)
#define CODEC_SLEEP_EMAW(n)                 BITFIELD_VAL(CODEC_SLEEP_EMAW, n)
#define CODEC_SLEEP_EMA_SHIFT               5
#define CODEC_SLEEP_EMA_MASK                (0x7 << CODEC_SLEEP_EMA_SHIFT)
#define CODEC_SLEEP_EMA(n)                  BITFIELD_VAL(CODEC_SLEEP_EMA, n)
#define CODEC_SLEEP_RESAMPLE_SLEEP_MODE     (1 << 4)
#define CODEC_SLEEP_FIR3_SLEEP_MODE         (1 << 3)
#define CODEC_SLEEP_FIR2_SLEEP_MODE         (1 << 2)
#define CODEC_SLEEP_FIR1_SLEEP_MODE         (1 << 1)
#define CODEC_SLEEP_FIR0_SLEEP_MODE         (1 << 0)

// MC_FIFO_ACCESS
#define MC_FIFO_MC_FIFO_ACCESS_SHIFT        0
#define MC_FIFO_MC_FIFO_ACCESS_MASK         (0xFFFFFFFF << MC_FIFO_MC_FIFO_ACCESS_SHIFT)
#define MC_FIFO_MC_FIFO_ACCESS(n)           BITFIELD_VAL(MC_FIFO_MC_FIFO_ACCESS, n)

// MC_CONFIG
#define MC_CONFIG_MC_LR_SWAP                (1 << 17)
#define MC_CONFIG_MODE_32BIT_DSD            (1 << 16)
#define MC_CONFIG_MODE_24BIT_DSD            (1 << 15)
#define MC_CONFIG_MODE_32BIT_ADC            (1 << 14)
#define MC_CONFIG_MODE_24BIT_ADC            (1 << 13)
#define MC_CONFIG_MODE_32BIT_DAC            (1 << 12)
#define MC_CONFIG_MODE_24BIT_DAC            (1 << 11)
#define MC_CONFIG_MC_DELAY_SHIFT            3
#define MC_CONFIG_MC_DELAY_MASK             (0xFF << MC_CONFIG_MC_DELAY_SHIFT)
#define MC_CONFIG_MC_DELAY(n)               BITFIELD_VAL(MC_CONFIG_MC_DELAY, n)
#define MC_CONFIG_DMACTRL_MC                (1 << 2)
#define MC_CONFIG_MODE_16BIT_MC             (1 << 1)
#define MC_CONFIG_DUAL_CHANNEL_MC           (1 << 0)

// DRE_GAIN_STATUS
#define DRE_GAIN_ANA_GAIN_DAC_CH1_SHIFT     24
#define DRE_GAIN_ANA_GAIN_DAC_CH1_MASK      (0xF << DRE_GAIN_ANA_GAIN_DAC_CH1_SHIFT)
#define DRE_GAIN_ANA_GAIN_DAC_CH1(n)        BITFIELD_VAL(DRE_GAIN_ANA_GAIN_DAC_CH1, n)
#define DRE_GAIN_ANA_GAIN_DAC_CH0_SHIFT     20
#define DRE_GAIN_ANA_GAIN_DAC_CH0_MASK      (0xF << DRE_GAIN_ANA_GAIN_DAC_CH0_SHIFT)
#define DRE_GAIN_ANA_GAIN_DAC_CH0(n)        BITFIELD_VAL(DRE_GAIN_ANA_GAIN_DAC_CH0, n)
#define DRE_GAIN_ANA_GAIN_ADC_CH4_SHIFT     16
#define DRE_GAIN_ANA_GAIN_ADC_CH4_MASK      (0xF << DRE_GAIN_ANA_GAIN_ADC_CH4_SHIFT)
#define DRE_GAIN_ANA_GAIN_ADC_CH4(n)        BITFIELD_VAL(DRE_GAIN_ANA_GAIN_ADC_CH4, n)
#define DRE_GAIN_ANA_GAIN_ADC_CH3_SHIFT     12
#define DRE_GAIN_ANA_GAIN_ADC_CH3_MASK      (0xF << DRE_GAIN_ANA_GAIN_ADC_CH3_SHIFT)
#define DRE_GAIN_ANA_GAIN_ADC_CH3(n)        BITFIELD_VAL(DRE_GAIN_ANA_GAIN_ADC_CH3, n)
#define DRE_GAIN_ANA_GAIN_ADC_CH2_SHIFT     8
#define DRE_GAIN_ANA_GAIN_ADC_CH2_MASK      (0xF << DRE_GAIN_ANA_GAIN_ADC_CH2_SHIFT)
#define DRE_GAIN_ANA_GAIN_ADC_CH2(n)        BITFIELD_VAL(DRE_GAIN_ANA_GAIN_ADC_CH2, n)
#define DRE_GAIN_ANA_GAIN_ADC_CH1_SHIFT     4
#define DRE_GAIN_ANA_GAIN_ADC_CH1_MASK      (0xF << DRE_GAIN_ANA_GAIN_ADC_CH1_SHIFT)
#define DRE_GAIN_ANA_GAIN_ADC_CH1(n)        BITFIELD_VAL(DRE_GAIN_ANA_GAIN_ADC_CH1, n)
#define DRE_GAIN_ANA_GAIN_ADC_CH0_SHIFT     0
#define DRE_GAIN_ANA_GAIN_ADC_CH0_MASK      (0xF << DRE_GAIN_ANA_GAIN_ADC_CH0_SHIFT)
#define DRE_GAIN_ANA_GAIN_ADC_CH0(n)        BITFIELD_VAL(DRE_GAIN_ANA_GAIN_ADC_CH0, n)

// CMU_CTRL2
#define CODEC_CMU2_SOFT_RSTN_IIR            (1 << 11)
#define CODEC_CMU2_SOFT_RSTN_DAC            (1 << 10)
#define CODEC_CMU2_SOFT_RSTN_ADC_FREE       (1 << 9)
#define CODEC_CMU2_SOFT_RSTN_ADC(i)         (1 << (((i) & 0x7) + 1))
#define CODEC_CMU2_SOFT_RSTN_ADC_ANA        (1 << 0)
#define CODEC_CMU2_SOFT_RSTN_ADC_ALL        (0x3FF << 0)

#define CODEC_03_RAMP_IND                   (1 << 31)
#define CODEC_03_RAMP_BYPASS                (1 << 30)
#define CODEC_03_RAMPUP_STEP_SHIFT          23
#define CODEC_03_RAMPUP_STEP_MASK           (0x7F << CODEC_03_RAMPUP_STEP_SHIFT)
#define CODEC_03_RAMPUP_STEP(n)             BITFIELD_VAL(CODEC_03_RAMPUP_STEP, n)
#define CODEC_03_RAMPDN_STEP_SHIFT          16
#define CODEC_03_RAMPDN_STEP_MASK           (0x7F << CODEC_03_RAMPDN_STEP_SHIFT)
#define CODEC_03_RAMPDN_STEP(n)             BITFIELD_VAL(CODEC_03_RAMPDN_STEP, n)
#define CODEC_03_ADC_DOWN_SEL_CH_SHIFT(i)   (2 * ((i) & 0x7))
#define CODEC_03_ADC_DOWN_SEL_CH_MASK(i)    (0x3 << CODEC_03_ADC_DOWN_SEL_CH_SHIFT(i))
#define CODEC_03_ADC_DOWN_SEL_CH(i, n)      (((n) & 0x3) << CODEC_03_ADC_DOWN_SEL_CH_SHIFT(i))

#define CODEC_04_CODEC_EN                   (1 << 31)
#define CODEC_04_DAC_EN                     (1 << 30)
#define CODEC_04_ADC_EN                     (1 << 29)
#define CODEC_04_RESERVED                   (1 << 28)
#define CODEC_04_S_DITHER_BYPASS            (1 << 26)
#define CODEC_04_S_ADC_IN_INV_CH1           (1 << 25)
#define CODEC_04_S_ADC_IN_INV_CH0           (1 << 24)
#define CODEC_04_DAC_HBF1_BYPASS            (1 << 16)
#define CODEC_04_DAC_HBF2_BYPASS            (1 << 15)
#define CODEC_04_DAC_HBF3_BYPASS            (1 << 14)
#define CODEC_04_DAC_EN_CH1                 (1 << 13)
#define CODEC_04_DAC_EN_CH0                 (1 << 12)
#define CODEC_04_ADC_EN_CH7                 (1 << 11)
#define CODEC_04_ADC_EN_CH6                 (1 << 10)
#define CODEC_04_ADC_EN_CH5                 (1 << 9)
#define CODEC_04_ADC_EN_CH4                 (1 << 8)
#define CODEC_04_ADC_EN_CH3                 (1 << 7)
#define CODEC_04_ADC_EN_CH2                 (1 << 6)
#define CODEC_04_ADC_EN_CH1                 (1 << 5)
#define CODEC_04_ADC_EN_CH0                 (1 << 4)
#define CODEC_04_ADC_EN_CH_ALL              (0xFF << 4)
#define CODEC_04_DAC_UP_SEL_SHIFT           0
#define CODEC_04_DAC_UP_SEL_MASK            (0x7 << CODEC_04_DAC_UP_SEL_SHIFT)
#define CODEC_04_DAC_UP_SEL(n)              BITFIELD_VAL(CODEC_04_DAC_UP_SEL, n)

#define CODEC_05_DITHER_GAIN_2_0_SHIFT      29
#define CODEC_05_DITHER_GAIN_2_0_MASK       (0x7 << CODEC_05_DITHER_GAIN_2_0_SHIFT)
#define CODEC_05_DITHER_GAIN_2_0(n)         BITFIELD_VAL(CODEC_05_DITHER_2_0_GAIN, n)
#define CODEC_05_S_ADC_VOLUME_CH0_SHIFT     25
#define CODEC_05_S_ADC_VOLUME_CH0_MASK      (0xF << CODEC_05_S_ADC_VOLUME_CH0_SHIFT)
#define CODEC_05_S_ADC_VOLUME_CH0(n)        BITFIELD_VAL(CODEC_05_S_ADC_VOLUME_CH0, n)
#define CODEC_05_S_DAC1K                    (1 << 24)
#define CODEC_05_S_LOOP                     (1 << 23)
#define CODEC_05_TPORTS_CODEC_TEST_EN       (1 << 22)
#define CODEC_05_TPORTS_SEL_SHIFT           17
#define CODEC_05_TPORTS_SEL_MASK            (0x1F << CODEC_05_TPORTS_SEL_SHIFT)
#define CODEC_05_TPORTS_SEL(n)              BITFIELD_VAL(CODEC_05_TPORTS_SEL, n)
#define CODEC_05_S_ADC_DVST_CTL_SHIFT       12
#define CODEC_05_S_ADC_DVST_CTL_MASK        (0x1F << CODEC_05_S_ADC_DVST_CTL_SHIFT)
#define CODEC_05_S_ADC_DVST_CTL(n)          BITFIELD_VAL(CODEC_05_S_ADC_DVST_CTL, n)
#define CODEC_05_SDM_GAIN_SHIFT             10
#define CODEC_05_SDM_GAIN_MASK              (0x3 << CODEC_05_SDM_GAIN_SHIFT)
#define CODEC_05_SDM_GAIN(n)                BITFIELD_VAL(CODEC_05_SDM_GAIN, n)
#define CODEC_05_DITHER_GAIN_4_3_SHIFT      8
#define CODEC_05_DITHER_GAIN_4_3_MASK       (0x3 << CODEC_05_DITHER_GAIN_4_3_SHIFT)
#define CODEC_05_DITHER_GAIN_4_3(n)         BITFIELD_VAL(CODEC_05_DITHER_GAIN_4_3, n)
#define CODEC_05_RESERVED_SHIFT             2
#define CODEC_05_RESERVED_MASK              (0x3F << CODEC_05_RESERVED_SHIFT)
#define CODEC_05_RESERVED(n)                BITFIELD_VAL(CODEC_05_RESERVED, n)
#define CODEC_05_ADC_CH1_IIR_ENABLE         (1 << 1)
#define CODEC_05_ADC_CH0_IIR_ENABLE         (1 << 0)

#define CODEC_06_ADC_HBF1_BYPASS_CH(i)      (1 << (10 + (3 * ((i) & 0x7))))
#define CODEC_06_ADC_HBF2_BYPASS_CH(i)      (1 << (9 + (3 * ((i) & 0x7))))
#define CODEC_06_ADC_HBF3_BYPASS_CH(i)      (1 << (8 + (3 * ((i) & 0x7))))
#define CODEC_06_ADC_HBF_BYPASS_CH_ALL      (0xFFFFFF << 8)
#define CODEC_06_RESERVED                   (1 << 7)
#define CODEC_06_DAC_R_IIR_ENABLE           (1 << 6)
#define CODEC_06_DAC_L_IIR_ENABLE           (1 << 5)
#define CODEC_06_S_DAC_OSR_SEL_SHIFT        3
#define CODEC_06_S_DAC_OSR_SEL_MASK         (0x3 << CODEC_06_S_DAC_OSR_SEL_SHIFT)
#define CODEC_06_S_DAC_OSR_SEL(n)           BITFIELD_VAL(CODEC_06_S_DAC_OSR_SEL, n)
#define CODEC_06_S_FS_SEL_SHIFT             0
#define CODEC_06_S_FS_SEL_MASK              (0x7 << CODEC_06_S_FS_SEL_SHIFT)
#define CODEC_06_S_FS_SEL(n)                BITFIELD_VAL(CODEC_06_S_FS_SEL, n)

#define CODEC_07_MC_RATE_SEL                (1 << 7)
#define CODEC_07_ANC_RATE_SEL               (1 << 6)
#define CODEC_07_MUTE_GAIN_PASS0_CH1        (1 << 5)
#define CODEC_07_MUTE_GAIN_PASS0_CH0        (1 << 4)
#define CODEC_07_DUAL_ANC_CH1               (1 << 3)
#define CODEC_07_DUAL_ANC_CH0               (1 << 2)
#define CODEC_07_ANC_ENABLE_CH1             (1 << 1)
#define CODEC_07_ANC_ENABLE_CH0             (1 << 0)

#define CODEC_08_PDM_MUX_CH_SHIFT(i)        (8 + 3 * ((i) & 0x7))
#define CODEC_08_PDM_MUX_CH_MASK(i)         (0x7 << CODEC_08_PDM_MUX_CH_SHIFT(i))
#define CODEC_08_PDM_MUX_CH(i, n)           (((n) & 0x7) << CODEC_08_PDM_MUX_CH_SHIFT(i))
#define CODEC_08_FEEDBACK_MC_HD_CH1         (1 << 6)
#define CODEC_08_FEEDBACK_MC_HD_CH0         (1 << 5)
#define CODEC_08_FEEDBACK_MC_EN_CH1         (1 << 4)
#define CODEC_08_FEEDBACK_MC_EN_CH0         (1 << 3)
#define CODEC_08_FEEDBACK_CH1               (1 << 1)
#define CODEC_08_FEEDBACK_CH0               (1 << 0)

#define CODEC_09_ANC_CH1_MUTE               (1 << 31)
#define CODEC_09_ANC_CH0_MUTE               (1 << 30)
#define CODEC_09_ANC_FS1_TO_FIR_BYPASS      (1 << 29)
#define CODEC_09_ANC_FS0_TO_FIR_BYPASS      (1 << 28)
#define CODEC_09_ANC_PDU1_TO_FIR_BYPASS     (1 << 27)
#define CODEC_09_ANC_PDU0_TO_FIR_BYPASS     (1 << 26)
#define CODEC_09_DSD_SAMPLE_RATE            (1 << 25)
#define CODEC_09_DSD_DATA_INV               (1 << 24)
#define CODEC_09_DAC_SDM_SWAP               (1 << 23)
#define CODEC_09_PDM_ADC_SEL_CH4            (1 << 22)
#define CODEC_09_PDM_ADC_SEL_CH3            (1 << 21)
#define CODEC_09_PDM_ADC_SEL_CH2            (1 << 20)
#define CODEC_09_SIDETONE_MIC_SEL           (1 << 19)
#define CODEC_09_SIN1K_STEP_SHIFT           15
#define CODEC_09_SIN1K_STEP_MASK            (0xF << CODEC_09_SIN1K_STEP_SHIFT)
#define CODEC_09_SIN1K_STEP(n)              BITFIELD_VAL(CODEC_09_SIN1K_STEP, n)
#define CODEC_09_DAC_GAIN_SEL               (1 << 14)
#define CODEC_09_ADC_GAIN_SEL               (1 << 13)
#define CODEC_09_ADC_LR_SWAP                (1 << 12)
#define CODEC_09_DAC_LR_SWAP                (1 << 11)
#define CODEC_09_S_ADC_VOLUME_CH1_SHIFT     7
#define CODEC_09_S_ADC_VOLUME_CH1_MASK      (0xF << CODEC_09_S_ADC_VOLUME_CH1_SHIFT)
#define CODEC_09_S_ADC_VOLUME_CH1(n)        BITFIELD_VAL(CODEC_09_S_ADC_VOLUME_CH1, n)
#define CODEC_09_DSD_ENABLE_R               (1 << 6)
#define CODEC_09_DSD_ENABLE_L               (1 << 5)
#define CODEC_09_PDM_ADC_SEL_CH1            (1 << 4)
#define CODEC_09_PDM_ADC_SEL_CH0            (1 << 3)
#define CODEC_09_DSD_CH_SEL                 (1 << 2)
#define CODEC_09_PDM_DATA_INV               (1 << 1)
#define CODEC_09_PDM_ENABLE                 (1 << 0)

#define CODEC_10_CLASSG_THD2_SHIFT          24
#define CODEC_10_CLASSG_THD2_MASK           (0xFF << CODEC_10_CLASSG_THD2_SHIFT)
#define CODEC_10_CLASSG_THD2(n)             BITFIELD_VAL(CODEC_10_CLASSG_THD2, n)
#define CODEC_10_CLASSG_THD1_SHIFT          16
#define CODEC_10_CLASSG_THD1_MASK           (0xFF << CODEC_10_CLASSG_THD1_SHIFT)
#define CODEC_10_CLASSG_THD1(n)             BITFIELD_VAL(CODEC_10_CLASSG_THD1, n)
#define CODEC_10_CLASSG_THD0_SHIFT          8
#define CODEC_10_CLASSG_THD0_MASK           (0xFF << CODEC_10_CLASSG_THD0_SHIFT)
#define CODEC_10_CLASSG_THD0(n)             BITFIELD_VAL(CODEC_10_CLASSG_THD0, n)
#define CODEC_10_RESERVED                   (1 << 7)
#define CODEC_10_RESAMPLE_441K              (1 << 6)
#define CODEC_10_RESAMPLE_DUAL_CH           (1 << 5)
#define CODEC_10_RESAMPLE_ENABLE            (1 << 4)
#define CODEC_10_CLASSG_LR                  (1 << 3)
#define CODEC_10_CLASSG_STEP_4_3N           (1 << 2)
#define CODEC_10_CLASSG_QUICK_DOWN          (1 << 1)
#define CODEC_10_CLASSG_EN                  (1 << 0)

#define CODEC_11_ADC1_MUSIC_CANCEL_EN       (1 << 20)
#define CODEC_11_ADC0_MUSIC_CANCEL_EN       (1 << 19)
#define CODEC_11_CLASSG_WINDOW_SHIFT        0
#define CODEC_11_CLASSG_WINDOW_MASK         (0xFFF << CODEC_11_CLASSG_WINDOW_SHIFT)
#define CODEC_11_CLASSG_WINDOW(n)           BITFIELD_VAL(CODEC_11_CLASSG_WINDOW, n)

#define CODEC_12_S_ADC_VOLUME_CH7_SHIFT     20
#define CODEC_12_S_ADC_VOLUME_CH7_MASK      (0xF << CODEC_12_S_ADC_VOLUME_CH7_SHIFT)
#define CODEC_12_S_ADC_VOLUME_CH7(n)        BITFIELD_VAL(CODEC_12_S_ADC_VOLUME_CH7, n)
#define CODEC_12_S_ADC_VOLUME_CH6_SHIFT     16
#define CODEC_12_S_ADC_VOLUME_CH6_MASK      (0xF << CODEC_12_S_ADC_VOLUME_CH6_SHIFT)
#define CODEC_12_S_ADC_VOLUME_CH6(n)        BITFIELD_VAL(CODEC_12_S_ADC_VOLUME_CH6, n)
#define CODEC_12_S_ADC_VOLUME_CH5_SHIFT     12
#define CODEC_12_S_ADC_VOLUME_CH5_MASK      (0xF << CODEC_12_S_ADC_VOLUME_CH5_SHIFT)
#define CODEC_12_S_ADC_VOLUME_CH5(n)        BITFIELD_VAL(CODEC_12_S_ADC_VOLUME_CH5, n)
#define CODEC_12_S_ADC_VOLUME_CH4_SHIFT     8
#define CODEC_12_S_ADC_VOLUME_CH4_MASK      (0xF << CODEC_12_S_ADC_VOLUME_CH4_SHIFT)
#define CODEC_12_S_ADC_VOLUME_CH4(n)        BITFIELD_VAL(CODEC_12_S_ADC_VOLUME_CH4, n)
#define CODEC_12_S_ADC_VOLUME_CH3_SHIFT     4
#define CODEC_12_S_ADC_VOLUME_CH3_MASK      (0xF << CODEC_12_S_ADC_VOLUME_CH3_SHIFT)
#define CODEC_12_S_ADC_VOLUME_CH3(n)        BITFIELD_VAL(CODEC_12_S_ADC_VOLUME_CH3, n)
#define CODEC_12_S_ADC_VOLUME_CH2_SHIFT     0
#define CODEC_12_S_ADC_VOLUME_CH2_MASK      (0xF << CODEC_12_S_ADC_VOLUME_CH2_SHIFT)
#define CODEC_12_S_ADC_VOLUME_CH2(n)        BITFIELD_VAL(CODEC_12_S_ADC_VOLUME_CH2, n)
#define CODEC_12_S_ADC_VOLUME_CH_SHIFT(i)   (4 * (i))
#define CODEC_12_S_ADC_VOLUME_CH_MASK(i)    (0xF << CODEC_12_S_ADC_VOLUME_CH_SHIFT(i))
#define CODEC_12_S_ADC_VOLUME_CH(i, n)      (((n) & 0xF) << CODEC_12_S_ADC_VOLUME_CH_SHIFT(i))

#define CODEC_13_IIR_COUNT_CH3_SHIFT        14
#define CODEC_13_IIR_COUNT_CH3_MASK         (0x7 << CODEC_13_IIR_COUNT_CH3_SHIFT)
#define CODEC_13_IIR_COUNT_CH3(n)           BITFIELD_VAL(CODEC_13_IIR_COUNT_CH3, n)
#define CODEC_13_IIR_COUNT_CH2_SHIFT        11
#define CODEC_13_IIR_COUNT_CH2_MASK         (0x7 << CODEC_13_IIR_COUNT_CH2_SHIFT)
#define CODEC_13_IIR_COUNT_CH2(n)           BITFIELD_VAL(CODEC_13_IIR_COUNT_CH2, n)
#define CODEC_13_IIR_COUNT_CH1_SHIFT        8
#define CODEC_13_IIR_COUNT_CH1_MASK         (0x7 << CODEC_13_IIR_COUNT_CH1_SHIFT)
#define CODEC_13_IIR_COUNT_CH1(n)           BITFIELD_VAL(CODEC_13_IIR_COUNT_CH1, n)
#define CODEC_13_IIR_COUNT_CH0_SHIFT        5
#define CODEC_13_IIR_COUNT_CH0_MASK         (0x7 << CODEC_13_IIR_COUNT_CH0_SHIFT)
#define CODEC_13_IIR_COUNT_CH0(n)           BITFIELD_VAL(CODEC_13_IIR_COUNT_CH0, n)
#define CODEC_13_IIR_CH3_BYPASS             (1 << 4)
#define CODEC_13_IIR_CH2_BYPASS             (1 << 3)
#define CODEC_13_IIR_CH1_BYPASS             (1 << 2)
#define CODEC_13_IIR_CH0_BYPASS             (1 << 1)
#define CODEC_13_IIR_ENABLE                 (1 << 0)

#define CODEC_14_MUTE_GAIN_COEF_PDU_CH1_SHIFT   12
#define CODEC_14_MUTE_GAIN_COEF_PDU_CH1_MASK    (0xFFF << CODEC_14_MUTE_GAIN_COEF_PDU_CH1_SHIFT)
#define CODEC_14_MUTE_GAIN_COEF_PDU_CH1(n)      BITFIELD_VAL(CODEC_14_MUTE_GAIN_COEF_PDU_CH1, n)
#define CODEC_14_MUTE_GAIN_COEF_PDU_CH0_SHIFT   0
#define CODEC_14_MUTE_GAIN_COEF_PDU_CH0_MASK    (0xFFF << CODEC_14_MUTE_GAIN_COEF_PDU_CH0_SHIFT)
#define CODEC_14_MUTE_GAIN_COEF_PDU_CH0(n)      BITFIELD_VAL(CODEC_14_MUTE_GAIN_COEF_PDU_CH0, n)

#define CODEC_15_MUTE_GAIN_COEF_FS_CH1_SHIFT    12
#define CODEC_15_MUTE_GAIN_COEF_FS_CH1_MASK     (0xFFF << CODEC_15_MUTE_GAIN_COEF_FS_CH1_SHIFT)
#define CODEC_15_MUTE_GAIN_COEF_FS_CH1(n)       BITFIELD_VAL(CODEC_15_MUTE_GAIN_COEF_FS_CH1, n)
#define CODEC_15_MUTE_GAIN_COEF_FS_CH0_SHIFT    0
#define CODEC_15_MUTE_GAIN_COEF_FS_CH0_MASK     (0xFFF << CODEC_15_MUTE_GAIN_COEF_FS_CH0_SHIFT)
#define CODEC_15_MUTE_GAIN_COEF_FS_CH0(n)       BITFIELD_VAL(CODEC_15_MUTE_GAIN_COEF_FS_CH0, n)

#define CODEC_16_ANC_FF_OUT_PHASE_SEL_SHIFT 22
#define CODEC_16_ANC_FF_OUT_PHASE_SEL_MASK  (0x7 << CODEC_16_ANC_FF_OUT_PHASE_SEL_SHIFT)
#define CODEC_16_ANC_FF_OUT_PHASE_SEL(n)    BITFIELD_VAL(CODEC_16_ANC_FF_OUT_PHASE_SEL, n)
#define CODEC_16_ANC_FF_IN_PHASE_SEL_SHIFT  19
#define CODEC_16_ANC_FF_IN_PHASE_SEL_MASK   (0x7 << CODEC_16_ANC_FF_IN_PHASE_SEL_SHIFT)
#define CODEC_16_ANC_FF_IN_PHASE_SEL(n)     BITFIELD_VAL(CODEC_16_ANC_FF_IN_PHASE_SEL, n)
#define CODEC_16_ANC_FF_SR_SEL_SHIFT        16
#define CODEC_16_ANC_FF_SR_SEL_MASK         (0x3 << CODEC_16_ANC_FF_SR_SEL_SHIFT)
#define CODEC_16_ANC_FF_SR_SEL(n)           BITFIELD_VAL(CODEC_16_ANC_FF_SR_SEL, n)
#define CODEC_16_ANC_FB_OUT_PHASE_SEL_SHIFT 5
#define CODEC_16_ANC_FB_OUT_PHASE_SEL_MASK  (0x7 << CODEC_16_ANC_FB_OUT_PHASE_SEL_SHIFT)
#define CODEC_16_ANC_FB_OUT_PHASE_SEL(n)    BITFIELD_VAL(CODEC_16_ANC_FB_OUT_PHASE_SEL, n)
#define CODEC_16_ANC_FB_IN_PHASE_SEL_SHIFT  2
#define CODEC_16_ANC_FB_IN_PHASE_SEL_MASK   (0x7 << CODEC_16_ANC_FB_IN_PHASE_SEL_SHIFT)
#define CODEC_16_ANC_FB_IN_PHASE_SEL(n)     BITFIELD_VAL(CODEC_16_ANC_FB_IN_PHASE_SEL, n)
#define CODEC_16_ANC_FB_SR_SEL_SHIFT        0
#define CODEC_16_ANC_FB_SR_SEL_MASK         (0x3 << CODEC_16_ANC_FB_SR_SEL_SHIFT)
#define CODEC_16_ANC_FB_SR_SEL(n)           BITFIELD_VAL(CODEC_16_ANC_FB_SR_SEL, n)

#define CODEC_17_LOOP_SEL_R_SHIFT           18
#define CODEC_17_LOOP_SEL_R_MASK            (0x7 << CODEC_17_LOOP_SEL_R_SHIFT)
#define CODEC_17_LOOP_SEL_R(n)              BITFIELD_VAL(CODEC_17_LOOP_SEL_R, n)
#define CODEC_17_LOOP_SEL_L_SHIFT           15
#define CODEC_17_LOOP_SEL_L_MASK            (0x7 << CODEC_17_LOOP_SEL_L_SHIFT)
#define CODEC_17_LOOP_SEL_L(n)              BITFIELD_VAL(CODEC_17_LOOP_SEL_L, n)
#define CODEC_17_ADC_IN_SEL_CH4_SHIFT       12
#define CODEC_17_ADC_IN_SEL_CH4_MASK        (0x7 << CODEC_17_ADC_IN_SEL_CH4_SHIFT)
#define CODEC_17_ADC_IN_SEL_CH4(n)          BITFIELD_VAL(CODEC_17_ADC_IN_SEL_CH4, n)
#define CODEC_17_ADC_IN_SEL_CH3_SHIFT       9
#define CODEC_17_ADC_IN_SEL_CH3_MASK        (0x7 << CODEC_17_ADC_IN_SEL_CH3_SHIFT)
#define CODEC_17_ADC_IN_SEL_CH3(n)          BITFIELD_VAL(CODEC_17_ADC_IN_SEL_CH3, n)
#define CODEC_17_ADC_IN_SEL_CH2_SHIFT       6
#define CODEC_17_ADC_IN_SEL_CH2_MASK        (0x7 << CODEC_17_ADC_IN_SEL_CH2_SHIFT)
#define CODEC_17_ADC_IN_SEL_CH2(n)          BITFIELD_VAL(CODEC_17_ADC_IN_SEL_CH2, n)
#define CODEC_17_ADC_IN_SEL_CH1_SHIFT       3
#define CODEC_17_ADC_IN_SEL_CH1_MASK        (0x7 << CODEC_17_ADC_IN_SEL_CH1_SHIFT)
#define CODEC_17_ADC_IN_SEL_CH1(n)          BITFIELD_VAL(CODEC_17_ADC_IN_SEL_CH1, n)
#define CODEC_17_ADC_IN_SEL_CH0_SHIFT       0
#define CODEC_17_ADC_IN_SEL_CH0_MASK        (0x7 << CODEC_17_ADC_IN_SEL_CH0_SHIFT)
#define CODEC_17_ADC_IN_SEL_CH0(n)          BITFIELD_VAL(CODEC_17_ADC_IN_SEL_CH0, n)

#define CODEC_18_RESAMPLE_SLOW_EN           (1 << 20)
#define CODEC_18_RESAMPLE_SLOW_VAL_SHIFT    21
#define CODEC_18_RESAMPLE_SLOW_VAL_MASK     (0xF << CODEC_18_RESAMPLE_SLOW_VAL_SHIFT)
#define CODEC_18_RESAMPLE_SLOW_VAL(n)       BITFIELD_VAL(CODEC_18_RESAMPLE_SLOW_VAL, n)

// CODEC_18 / 20 / 22 / 29 / 31
#define DRE_ADC_THD_DB_OFFSET_SIGN          (1 << 19)
#define DRE_ADC_DRE_DELAY_ANA_SHIFT         14
#define DRE_ADC_DRE_DELAY_ANA_MASK          (0x1F << DRE_ADC_DRE_DELAY_ANA_SHIFT)
#define DRE_ADC_DRE_DELAY_ANA(n)            BITFIELD_VAL(DRE_ADC_DRE_DELAY_ANA, n)
#define DRE_ADC_DRE_DELAY_DIG_SHIFT         11
#define DRE_ADC_DRE_DELAY_DIG_MASK          (0x7 << DRE_ADC_DRE_DELAY_DIG_SHIFT)
#define DRE_ADC_DRE_DELAY_DIG(n)            BITFIELD_VAL(DRE_ADC_DRE_DELAY_DIG, n)
#define DRE_ADC_INI_ANA_GAIN_SHIFT          7
#define DRE_ADC_INI_ANA_GAIN_MASK           (0xF << DRE_ADC_INI_ANA_GAIN_SHIFT)
#define DRE_ADC_INI_ANA_GAIN(n)             BITFIELD_VAL(DRE_ADC_INI_ANA_GAIN, n)
#define DRE_ADC_THD_DB_OFFSET_SHIFT         3
#define DRE_ADC_THD_DB_OFFSET_MASK          (0xF << DRE_ADC_THD_DB_OFFSET_SHIFT)
#define DRE_ADC_THD_DB_OFFSET(n)            BITFIELD_VAL(DRE_ADC_THD_DB_OFFSET, n)
#define DRE_ADC_STEP_MODE_SHIFT             1
#define DRE_ADC_STEP_MODE_MASK              (0x3 << DRE_ADC_STEP_MODE_SHIFT)
#define DRE_ADC_STEP_MODE(n)                BITFIELD_VAL(DRE_ADC_STEP_MODE, n)
#define DRE_ADC_DRE_ENABLE                  (1 << 0)

#define CODEC_19_PDM_CAP_PHASE_CH_SHIFT(i)  (0 + 3 * ((i) & 0x7))
#define CODEC_19_PDM_CAP_PHASE_CH_MASK(i)   (0x7 << CODEC_19_PDM_CAP_PHASE_CH_SHIFT(i))
#define CODEC_19_PDM_CAP_PHASE_CH(i, n)     (((n) & 0x7) << CODEC_19_PDM_CAP_PHASE_CH_SHIFT(i))

// CODEC_21 / 23 / 25 / 27 / 28 / 30 / 32
#define DRE_WIN_DRE_WINDOW_SHIFT            11
#define DRE_WIN_DRE_WINDOW_MASK             (0xFFFFF << DRE_WIN_DRE_WINDOW_SHIFT)
#define DRE_WIN_DRE_WINDOW(n)               BITFIELD_VAL(DRE_WIN_DRE_WINDOW, n)
#define DRE_WIN_AMP_HIGH_SHIFT              0
#define DRE_WIN_AMP_HIGH_MASK               (0x7FF << DRE_WIN_AMP_HIGH_SHIFT)
#define DRE_WIN_AMP_HIGH(n)                 BITFIELD_VAL(DRE_WIN_AMP_HIGH, n)

// CODEC_24 / 26
#define DRE_DAC_THD_DB_OFFSET_SIGN          (1 << 17)
#define DRE_DAC_DRE_DELAY_SHIFT             11
#define DRE_DAC_DRE_DELAY_MASK              (0x3F << DRE_DAC_DRE_DELAY_SHIFT)
#define DRE_DAC_DRE_DELAY(n)                BITFIELD_VAL(DRE_DAC_DRE_DELAY, n)
#define DRE_DAC_INI_ANA_GAIN_SHIFT          7
#define DRE_DAC_INI_ANA_GAIN_MASK           (0xF << DRE_DAC_INI_ANA_GAIN_SHIFT)
#define DRE_DAC_INI_ANA_GAIN(n)             BITFIELD_VAL(DRE_DAC_INI_ANA_GAIN, n)
#define DRE_DAC_THD_DB_OFFSET_SHIFT         3
#define DRE_DAC_THD_DB_OFFSET_MASK          (0xF << DRE_DAC_THD_DB_OFFSET_SHIFT)
#define DRE_DAC_THD_DB_OFFSET(n)            BITFIELD_VAL(DRE_DAC_THD_DB_OFFSET, n)
#define DRE_DAC_STEP_MODE_SHIFT             1
#define DRE_DAC_STEP_MODE_MASK              (0x3 << DRE_DAC_STEP_MODE_SHIFT)
#define DRE_DAC_STEP_MODE(n)                BITFIELD_VAL(DRE_DAC_STEP_MODE, n)
#define DRE_DAC_DRE_ENABLE                  (1 << 0)

#define CODEC_33_DAC_CH0_GAIN_UPDATE        (1 << 20)
#define CODEC_33_DAC_CH0_GAIN_SHIFT         0
#define CODEC_33_DAC_CH0_GAIN_MASK          (0xFFFFF << CODEC_33_DAC_CH0_GAIN_SHIFT)
#define CODEC_33_DAC_CH0_GAIN(n)            BITFIELD_VAL(CODEC_33_DAC_CH0_GAIN, n)

#define CODEC_34_DAC_CH1_GAIN_UPDATE        (1 << 20)
#define CODEC_34_DAC_CH1_GAIN_SHIFT         0
#define CODEC_34_DAC_CH1_GAIN_MASK          (0xFFFFF << CODEC_34_DAC_CH1_GAIN_SHIFT)
#define CODEC_34_DAC_CH1_GAIN(n)            BITFIELD_VAL(CODEC_34_DAC_CH1_GAIN, n)

// CODEC_35
#define CODEC_DAC_COMPENSATE_BYPASS_CH0     (1 << 20)

// CODEC_43
#define CODEC_DAC_COMPENSATE_BYPASS_CH1     (1 << 20)

// CODEC_35 ~ 50
#define CODEC_FIR_COMP_COEF_SHIFT           0
#define CODEC_FIR_COMP_COEF_MASK            (0xFFFFF << CODEC_FIR_COMP_COEF_SHIFT)
#define CODEC_FIR_COMP_COEF(n)              BITFIELD_VAL(CODEC_FIR_COMP_COEF, n)

// CODEC_51 / 52
#define CODEC_FB_CH_CHECK_TRIG_WIN_SHIFT    17
#define CODEC_FB_CH_CHECK_TRIG_WIN_MASK     (0x3FF << CODEC_FB_CH_CHECK_TRIG_WIN_SHIFT)
#define CODEC_FB_CH_CHECK_TRIG_WIN(n)       BITFIELD_VAL(CODEC_FB_CH_CHECK_TRIG_WIN, n)
#define CODEC_FB_CH_CHECK_ACC_WIN_SHIFT     5
#define CODEC_FB_CH_CHECK_ACC_WIN_MASK      (0xFFF << CODEC_FB_CH_CHECK_ACC_WIN_SHIFT)
#define CODEC_FB_CH_CHECK_ACC_WIN(n)        BITFIELD_VAL(CODEC_FB_CH_CHECK_ACC_WIN, n)
#define CODEC_FB_CH_CHECK_KEEP_SEL          (1 << 4)
#define CODEC_FB_CH_CHECK_SRC_SEL           (1 << 3)
#define CODEC_FB_CH_CHECK_ACC_SAMP_RATE_SHIFT 1
#define CODEC_FB_CH_CHECK_ACC_SAMP_RATE_MASK (0x3 << CODEC_FB_CH_CHECK_ACC_SAMP_RATE_SHIFT)
#define CODEC_FB_CH_CHECK_ACC_SAMP_RATE(n)  BITFIELD_VAL(CODEC_FB_CH_CHECK_ACC_SAMP_RATE, n)
#define CODEC_FB_CH_CHECK_ENABLE            (1 << 0)

#define CODEC_57_DAC_CH1_GAIN_UPDATE        (1 << 31)
#define CODEC_57_DAC_CH1_DC_SHIFT           16
#define CODEC_57_DAC_CH1_DC_MASK            (0x7FFF << CODEC_57_DAC_CH1_DC_SHIFT)
#define CODEC_57_DAC_CH1_DC(n)              BITFIELD_VAL(CODEC_57_DAC_CH1_DC, n)
#define CODEC_57_DAC_CH0_GAIN_UPDATE        (1 << 15)
#define CODEC_57_DAC_CH0_DC_SHIFT           0
#define CODEC_57_DAC_CH0_DC_MASK            (0x7FFF << CODEC_57_DAC_CH0_DC_SHIFT)
#define CODEC_57_DAC_CH0_DC(n)              BITFIELD_VAL(CODEC_57_DAC_CH0_DC, n)

#endif

