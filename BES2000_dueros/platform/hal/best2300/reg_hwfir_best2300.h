#ifndef __REG_HWFIR_BEST2300_H__
#define __REG_HWFIR_BEST2300_H__

#include "plat_types.h"
#include "hal_hwfir.h"


struct HWFIR_CH_T {
    __IO uint32_t cfg0;
    __IO uint32_t cfg1;
};

struct HWFIR_T {
    __IO uint32_t config0;
    struct HWFIR_CH_T ch[8];
    __IO uint32_t config1;
};

extern struct HWFIR_T *HWFIR;

#define HWFIR_REG_READ(reg) \
    (*(volatile uint32_t *)(FFT_BASE+reg))

#define HWFIR_REG_WRITE(reg,val) \
    (*(volatile uint32_t *)(FFT_BASE+reg)) = val

// config 0
#define IIR_COEF_SWAP_SHIFT                 (27)
#define IIR_COEF_SWAP_MASK                  (0x1 << IIR_COEF_SWAP_SHIFT)
#define IIR_COEF_SWAP(n)                    (((n) & 0x1) << IIR_COEF_SWAP_SHIFT)

#define PDU_FS_SWAP_SHIFT                   (26)
#define PDU_FS_SWAP_MASK                    (0x1 << PDU_FS_SWAP_SHIFT)
#define PDU_FS_SWAP(n)                      (((n) & 0x1) << PDU_FS_SWAP_SHIFT)

#define ANC_COEF_SEL_PDU1_FS1_SHIFT         (25)
#define ANC_COEF_SEL_PDU1_FS1_MASK          (0x1 << ANC_COEF_SEL_PDU1_FS1_SHIFT)
#define ANC_COEF_SEL_PDU1_FS1(n)            (((n) & 0X1) << ANC_COEF_SEL_PDU1_FS1_SHIFT)

#define ANC_COEF_SEL_PDU0_FS0_SHIFT         (24)
#define ANC_COEF_SEL_PDU0_FS0_MASK          (0x1 << ANC_COEF_SEL_PDU0_FS0_SHIFT)
#define ANC_COEF_SEL_PDU0_FS0(n)            (((n) & 0X1) << ANC_COEF_SEL_PDU0_FS0_SHIFT)

#define ANC_COEF_SEL_FS0_FS1_SHIFT          (23)
#define ANC_COEF_SEL_FS0_FS1_MASK           (0x1 << ANC_COEF_SEL_FS0_FS1_SHIFT)
#define ANC_COEF_SEL_FS0_FS1(n)             (((n) & 0X1) << ANC_COEF_SEL_FS0_FS1_SHIFT)

#define ANC_COEF_SEL_PDU0_PDU1_SHIFT        (22)
#define ANC_COEF_SEL_PDU0_PDU1_MASK         (0x1 << ANC_COEF_SEL_PDU0_PDU1_SHIFT)
#define ANC_COEF_SEL_PDU0_PDU1(n)           (((n) & 0X1) << ANC_COEF_SEL_PDU0_PDU1_SHIFT)

#define DMA_FIR_TX_CTRL_SHIFT               (21)
#define DMA_FIR_TX_CTRL_MASK                (0x1 << DMA_FIR_TX_CTRL_SHIFT)
#define DMA_FIR_TX_CTRL(n)                  (((n) & 0X1) << DMA_FIR_TX_CTRL_SHIFT)

#define DMA_FIR_RX_CTRL_SHIFT               (20)
#define DMA_FIR_RX_CTRL_MASK                (0x1 << DMA_FIR_RX_CTRL_SHIFT)
#define DMA_FIR_RX_CTRL(n)                  (((n) & 0X1) << DMA_FIR_RX_CTRL_SHIFT)

#define FIR3_256P0_512P1_SHIFT              (19)
#define FIR3_256P0_512P1_MASK               (0x1 << FIR3_256P0_512P1_SHIFT)
#define FIR3_256P0_512P1(n)                 (((n) & 0X1) << FIR3_256P0_512P1_SHIFT)

#define FIR2_256P0_512P1_SHIFT              (18)
#define FIR2_256P0_512P1_MASK               (0x1 << FIR2_256P0_512P1_SHIFT)
#define FIR2_256P0_512P1(n)                 (((n) & 0X1) << FIR2_256P0_512P1_SHIFT)

#define FIR1_256P0_512P1_SHIFT              (17)
#define FIR1_256P0_512P1_MASK               (0x1 << FIR1_256P0_512P1_SHIFT)
#define FIR1_256P0_512P1(n)                 (((n) & 0X1) << FIR1_256P0_512P1_SHIFT)

#define FIR0_256P0_512P1_SHIFT              (16)
#define FIR0_256P0_512P1_MASK               (0x1 << FIR0_256P0_512P1_SHIFT)
#define FIR0_256P0_512P1(n)                 (((n) & 0X1) << FIR0_256P0_512P1_SHIFT)

#define FIR_ENABLE_CH7_SHIFT                (15)
#define FIR_ENABLE_CH7_MASK                 (0x1 << FIR_ENABLE_CH7_SHIFT)
#define FIR_ENABLE_CH7(n)                   (((n) & 0X1) << FIR_ENABLE_CH7_SHIFT)

#define FIR_ENABLE_CH6_SHIFT                (14)
#define FIR_ENABLE_CH6_MASK                 (0x1 << FIR_ENABLE_CH6_SHIFT)
#define FIR_ENABLE_CH6(n)                   (((n) & 0X1) << FIR_ENABLE_CH6_SHIFT)

#define FIR_ENABLE_CH5_SHIFT                (13)
#define FIR_ENABLE_CH5_MASK                 (0x1 << FIR_ENABLE_CH5_SHIFT)
#define FIR_ENABLE_CH5(n)                   (((n) & 0X1) << FIR_ENABLE_CH5_SHIFT)

#define FIR_ENABLE_CH4_SHIFT                (12)
#define FIR_ENABLE_CH4_MASK                 (0x1 << FIR_ENABLE_CH4_SHIFT)
#define FIR_ENABLE_CH4(n)                   (((n) & 0X1) << FIR_ENABLE_CH4_SHIFT)

#define FIR_ENABLE_CH3_SHIFT                (11)
#define FIR_ENABLE_CH3_MASK                 (0x1 << FIR_ENABLE_CH3_SHIFT)
#define FIR_ENABLE_CH3(n)                   (((n) & 0X1) << FIR_ENABLE_CH3_SHIFT)

#define FIR_ENABLE_CH2_SHIFT                (10)
#define FIR_ENABLE_CH2_MASK                 (0x1 << FIR_ENABLE_CH2_SHIFT)
#define FIR_ENABLE_CH2(n)                   (((n) & 0X1) << FIR_ENABLE_CH2_SHIFT)

#define FIR_ENABLE_CH1_SHIFT                (9)
#define FIR_ENABLE_CH1_MASK                 (0x1 << FIR_ENABLE_CH1_SHIFT)
#define FIR_ENABLE_CH1(n)                   (((n) & 0X1) << FIR_ENABLE_CH1_SHIFT)

#define FIR_ENABLE_CH0_SHIFT                (8)
#define FIR_ENABLE_CH0_MASK                 (0x1 << FIR_ENABLE_CH0_SHIFT)
#define FIR_ENABLE_CH0(n)                   (((n) & 0X1) << FIR_ENABLE_CH0_SHIFT)

#define STREAM_ENABLE_CH7_SHIFT                (15)
#define STREAM_ENABLE_CH7_MASK                 (0x1 << STREAM_ENABLE_CH7_SHIFT)
#define STREAM_ENABLE_CH7(n)                   (((n) & 0X1) << STREAM_ENABLE_CH7_SHIFT)

#define STREAM_ENABLE_CH6_SHIFT                (14)
#define STREAM_ENABLE_CH6_MASK                 (0x1 << STREAM_ENABLE_CH6_SHIFT)
#define STREAM_ENABLE_CH6(n)                   (((n) & 0X1) << STREAM_ENABLE_CH6_SHIFT)

#define STREAM_ENABLE_CH5_SHIFT                (13)
#define STREAM_ENABLE_CH5_MASK                 (0x1 << STREAM_ENABLE_CH5_SHIFT)
#define STREAM_ENABLE_CH5(n)                   (((n) & 0X1) << STREAM_ENABLE_CH5_SHIFT)

#define STREAM_ENABLE_CH4_SHIFT                (12)
#define STREAM_ENABLE_CH4_MASK                 (0x1 << STREAM_ENABLE_CH4_SHIFT)
#define STREAM_ENABLE_CH4(n)                   (((n) & 0X1) << STREAM_ENABLE_CH4_SHIFT)

#define STREAM_ENABLE_CH3_SHIFT                (11)
#define STREAM_ENABLE_CH3_MASK                 (0x1 << STREAM_ENABLE_CH3_SHIFT)
#define STREAM_ENABLE_CH3(n)                   (((n) & 0X1) << STREAM_ENABLE_CH3_SHIFT)

#define STREAM_ENABLE_CH2_SHIFT                (10)
#define STREAM_ENABLE_CH2_MASK                 (0x1 << STREAM_ENABLE_CH2_SHIFT)
#define STREAM_ENABLE_CH2(n)                   (((n) & 0X1) << STREAM_ENABLE_CH2_SHIFT)

#define STREAM_ENABLE_CH1_SHIFT                (9)
#define STREAM_ENABLE_CH1_MASK                 (0x1 << STREAM_ENABLE_CH1_SHIFT)
#define STREAM_ENABLE_CH1(n)                   (((n) & 0X1) << STREAM_ENABLE_CH1_SHIFT)

#define STREAM_ENABLE_CH0_SHIFT                (8)
#define STREAM_ENABLE_CH0_MASK                 (0x1 << STREAM_ENABLE_CH0_SHIFT)
#define STREAM_ENABLE_CH0(n)                   (((n) & 0X1) << STREAM_ENABLE_CH0_SHIFT)

// channel config 0
#define FIR_SAMPLE_NUM_CH_SHIFT                (21)
#define FIR_SAMPLE_NUM_CH_MASK                 (0x1ff << FIR_SAMPLE_NUM_CH_SHIFT)
#define FIR_SAMPLE_NUM_CH(n)                   (((n) & 0x1ff) << FIR_SAMPLE_NUM_CH_SHIFT)

#define FIR_SAMPLE_START_CH_SHIFT              (12)
#define FIR_SAMPLE_START_CH_MASK               (0x1ff << FIR_SAMPLE_START_CH_SHIFT)
#define FIR_SAMPLE_START_CH(n)                 (((n) & 0x1FF) << FIR_SAMPLE_START_CH_SHIFT)

#define FIR_ORDER_CH_SHIFT                     (3)
#define FIR_ORDER_CH_MASK                      (0x1ff << FIR_ORDER_CH_SHIFT)
#define FIR_ORDER_CH(n)                        (((n) & 0x1FF) << FIR_ORDER_CH_SHIFT)

#define FIR_MODE_CH_SHIFT                      (1)
#define FIR_MODE_CH_MASK                       (0x3 << FIR_MODE_CH_SHIFT)
#define FIR_MODE_CH(n)                         (((n) & 0x3) << FIR_MODE_CH_SHIFT)
#define FIR_MODE_CH_GENERAL_FIR                (0)
#define FIR_MODE_CH_ANA2_SYN1                  (1)
#define FIR_MODE_CH_ANA1_SYN2                  (2)
#define FIR_MODE_CH_NOT_USE                    (3)

#define STREAM0_FIR1_CH_SHIFT                  (0)
#define STREAM0_FIR1_CH_MASK                   (0x1 << STREAM0_FIR1_CH_SHIFT)
#define STREAM0_FIR1_CH(n)                     (((n) & 0X1) << STREAM0_FIR1_CH_SHIFT)

// channel config 1
#define DO_REMAP_CH_SHIFT                      (31)
#define DO_REMAP_CH_MASK                       (0x1 << DO_REMAP_CH_SHIFT)
#define DO_REMAP_CH(n)                         (((n) & 0X1) << DO_REMAP_CH_SHIFT)

#define LOOP_NUM_CH_SHIFT                      (23)
#define LOOP_NUM_CH_MASK                       (0xff << LOOP_NUM_CH_SHIFT)
#define LOOP_NUM_CH(n)                         (((n) & 0Xff) << LOOP_NUM_CH_SHIFT)

#define GAIN_SEL_CH_SHIFT                      (19)
#define GAIN_SEL_CH_MASK                       (0xf << GAIN_SEL_CH_SHIFT)
#define GAIN_SEL_CH(n)                         (((n) & 0XF) << GAIN_SEL_CH_SHIFT)

#define BURST_LENGTH_CH_SHIFT                  (14)
#define BURST_LENGTH_CH_MASK                   (0x1f << BURST_LENGTH_CH_SHIFT)
#define BURST_LENGTH_CH(n)                     (((n) & 0X1F) << BURST_LENGTH_CH_SHIFT)

#define SLIDE_OFFSET_CH_SHIFT                  (9)
#define SLIDE_OFFSET_CH_MASK                   (0x1f << SLIDE_OFFSET_CH_SHIFT)
#define SLIDE_OFFSET_CH(n)                     (((n) & 0X1F) << SLIDE_OFFSET_CH_SHIFT)

#define RESULT_BASE_ADDR_CH_SHIFT              (0)
#define RESULT_BASE_ADDR_CH_MASK               (0x1ff << RESULT_BASE_ADDR_CH_SHIFT)
#define RESULT_BASE_ADDR_CH(n)                 (((n) & 0X1FF) << RESULT_BASE_ADDR_CH_SHIFT)

// config 1
#define ACCESS_OFFSET_CH7_SHIFT                (21)
#define ACCESS_OFFSET_CH7_MASK                 (0x7 << ACCESS_OFFSET_CH7_SHIFT)
#define ACCESS_OFFSET_CH7(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH7_SHIFT)

#define ACCESS_OFFSET_CH6_SHIFT                (18)
#define ACCESS_OFFSET_CH6_MASK                 (0x7 << ACCESS_OFFSET_CH6_SHIFT)
#define ACCESS_OFFSET_CH6(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH6_SHIFT)

#define ACCESS_OFFSET_CH5_SHIFT                (15)
#define ACCESS_OFFSET_CH5_MASK                 (0x7 << ACCESS_OFFSET_CH5_SHIFT)
#define ACCESS_OFFSET_CH5(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH5_SHIFT)

#define ACCESS_OFFSET_CH4_SHIFT                (12)
#define ACCESS_OFFSET_CH4_MASK                 (0x7 << ACCESS_OFFSET_CH4_SHIFT)
#define ACCESS_OFFSET_CH4(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH4_SHIFT)

#define ACCESS_OFFSET_CH3_SHIFT                (9)
#define ACCESS_OFFSET_CH3_MASK                 (0x7 << ACCESS_OFFSET_CH3_SHIFT)
#define ACCESS_OFFSET_CH3(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH3_SHIFT)

#define ACCESS_OFFSET_CH2_SHIFT                (6)
#define ACCESS_OFFSET_CH2_MASK                 (0x7 << ACCESS_OFFSET_CH2_SHIFT)
#define ACCESS_OFFSET_CH2(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH2_SHIFT)

#define ACCESS_OFFSET_CH1_SHIFT                (3)
#define ACCESS_OFFSET_CH1_MASK                 (0x7 << ACCESS_OFFSET_CH1_SHIFT)
#define ACCESS_OFFSET_CH1(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH1_SHIFT)

#define ACCESS_OFFSET_CH0_SHIFT                (0)
#define ACCESS_OFFSET_CH0_MASK                 (0x7 << ACCESS_OFFSET_CH0_SHIFT)
#define ACCESS_OFFSET_CH0(n)                   (((n) & 0X7) << ACCESS_OFFSET_CH0_SHIFT)

// address
#define HWFIR_CH0_SAMP_MEMORY_BASE (0x8000)
#define HWFIR_CH0_COEF_MEMORY_BASE (0x9000)

// ip operation
inline void hwfir_set_data_is_from_fir_to_dma(int en)
{
    if (en) {
        HWFIR->config0 |= DMA_FIR_TX_CTRL_MASK;
    }
    else {
        HWFIR->config0 &= ~DMA_FIR_TX_CTRL_MASK;
    }
}
inline void hwfir_set_data_is_from_dma_to_fir(int en)
{
    if (en) {
        HWFIR->config0 |= DMA_FIR_RX_CTRL_MASK;
    }
    else {
        HWFIR->config0 &= ~DMA_FIR_RX_CTRL_MASK;
    }
}
inline void hwfir_ch_set_256samp_to_512samp(HAL_HWFIR_CHANNEL_ID id, int en)
{
    uint32_t mask = 0;

    switch(id) {
        case HWFIR_CHANNEL_ID_0:mask = FIR0_256P0_512P1_SHIFT;break;
        case HWFIR_CHANNEL_ID_2:mask = FIR1_256P0_512P1_SHIFT;break;
        case HWFIR_CHANNEL_ID_4:mask = FIR2_256P0_512P1_SHIFT;break;
        case HWFIR_CHANNEL_ID_6:mask = FIR3_256P0_512P1_SHIFT;break;
        default:break;
    }

    if (en)
        HWFIR->config0 |= mask;
    else
        HWFIR->config0 &= ~mask;
}

inline void hwfir_ch_set_fir_enable(HAL_HWFIR_CHANNEL_ID id, int en)
{
    uint32_t mask = 0;

    switch(id) {
        case HWFIR_CHANNEL_ID_0:mask = FIR_ENABLE_CH0_MASK;break;
        case HWFIR_CHANNEL_ID_1:mask = FIR_ENABLE_CH1_MASK;break;
        case HWFIR_CHANNEL_ID_2:mask = FIR_ENABLE_CH2_MASK;break;
        case HWFIR_CHANNEL_ID_3:mask = FIR_ENABLE_CH3_MASK;break;
        case HWFIR_CHANNEL_ID_4:mask = FIR_ENABLE_CH4_MASK;break;
        case HWFIR_CHANNEL_ID_5:mask = FIR_ENABLE_CH5_MASK;break;
        case HWFIR_CHANNEL_ID_6:mask = FIR_ENABLE_CH6_MASK;break;
        case HWFIR_CHANNEL_ID_7:mask = FIR_ENABLE_CH7_MASK;break;
        default:break;
    }

    if (en)
        HWFIR->config0 |= mask;
    else
        HWFIR->config0 &= ~mask;
}

inline void hwfir_ch_set_fir_sample_num(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg0 &= ~FIR_SAMPLE_NUM_CH_MASK;
    HWFIR->ch[id].cfg0 |= FIR_SAMPLE_NUM_CH(val);
}

inline void hwfir_ch_set_fir_sample_start(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg0 &= ~FIR_SAMPLE_START_CH_MASK;
    HWFIR->ch[id].cfg0 |= FIR_SAMPLE_START_CH(val);
}

inline void hwfir_ch_set_fir_order(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg0 &= ~FIR_ORDER_CH_MASK;
    HWFIR->ch[id].cfg0 |= FIR_ORDER_CH(val);
}

inline void hwfir_ch_set_fir_mode(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg0 &= ~FIR_MODE_CH_MASK;
    HWFIR->ch[id].cfg0 |= FIR_MODE_CH(val);
}

inline void hwfir_ch_set_stream0_fir1(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg0 &= ~STREAM0_FIR1_CH_MASK;
    HWFIR->ch[id].cfg0 |= STREAM0_FIR1_CH(val);
}

inline void hwfir_ch_set_do_remap(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg1 &= ~DO_REMAP_CH_MASK;
    HWFIR->ch[id].cfg1 |= DO_REMAP_CH(val);
}

inline void hwfir_ch_set_loop_num(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg1 &= ~LOOP_NUM_CH_MASK;
    HWFIR->ch[id].cfg1 |= LOOP_NUM_CH(val);
}

inline void hwfir_ch_set_gain_sel(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg1 &= ~GAIN_SEL_CH_MASK;
    HWFIR->ch[id].cfg1 |= GAIN_SEL_CH(val);
}

inline void hwfir_ch_set_burst_length(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg1 &= ~BURST_LENGTH_CH_MASK;
    HWFIR->ch[id].cfg1 |= BURST_LENGTH_CH(val);
}

inline void hwfir_ch_set_slide_offset(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg1 &= ~SLIDE_OFFSET_CH_MASK;
    HWFIR->ch[id].cfg1 |= SLIDE_OFFSET_CH(val);
}

inline void hwfir_ch_set_result_base_addr(HAL_HWFIR_CHANNEL_ID id, int val)
{
    HWFIR->ch[id].cfg1 &= ~RESULT_BASE_ADDR_CH_MASK;
    HWFIR->ch[id].cfg1 |= RESULT_BASE_ADDR_CH(val);
}

inline void hwfir_ch_set_access_offset(HAL_HWFIR_CHANNEL_ID id, int val)
{
    uint32_t mask = 0;
    uint32_t v = 0;

    switch(id) {
        case HWFIR_CHANNEL_ID_0:mask = ACCESS_OFFSET_CH0_MASK;v = ACCESS_OFFSET_CH0(val);break;
        case HWFIR_CHANNEL_ID_1:mask = ACCESS_OFFSET_CH1_MASK;v = ACCESS_OFFSET_CH1(val);break;
        case HWFIR_CHANNEL_ID_2:mask = ACCESS_OFFSET_CH2_MASK;v = ACCESS_OFFSET_CH2(val);break;
        case HWFIR_CHANNEL_ID_3:mask = ACCESS_OFFSET_CH3_MASK;v = ACCESS_OFFSET_CH3(val);break;
        case HWFIR_CHANNEL_ID_4:mask = ACCESS_OFFSET_CH4_MASK;v = ACCESS_OFFSET_CH4(val);break;
        case HWFIR_CHANNEL_ID_5:mask = ACCESS_OFFSET_CH5_MASK;v = ACCESS_OFFSET_CH5(val);break;
        case HWFIR_CHANNEL_ID_6:mask = ACCESS_OFFSET_CH6_MASK;v = ACCESS_OFFSET_CH6(val);break;
        case HWFIR_CHANNEL_ID_7:mask = ACCESS_OFFSET_CH7_MASK;v = ACCESS_OFFSET_CH7(val);break;
        default:break;
    }

    HWFIR->config1 &= ~mask;
    HWFIR->config1 |= v;
}

#endif /* __REG_HWFIR_BEST2300_H__ */
