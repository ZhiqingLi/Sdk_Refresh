#ifndef __HAL_CODECIP_H__
#define __HAL_CODECIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "reg_codecip_best1000.h"

#define codecip_read32(b,a) \
     (*(volatile uint32_t *)(b+a))

#define codecip_write32(v,b,a) \
     ((*(volatile uint32_t *)(b+a)) = v)

static inline void codecipbus_w_adc_ch3_dma_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_ADC_ANC_SEL_CH1_MASK;
    else
        val &= ~CODECIP_CTRL4_ADC_ANC_SEL_CH1_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_adc_ch2_dma_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_ADC_ANC_SEL_CH0_MASK;
    else
        val &= ~CODECIP_CTRL4_ADC_ANC_SEL_CH0_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_anc_ch1_dma_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_HSR_SEL_CH1_MASK;
    else
        val &= ~CODECIP_CTRL4_HSR_SEL_CH1_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_anc_ch0_dma_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_HSR_SEL_CH0_MASK;
    else
        val &= ~CODECIP_CTRL4_HSR_SEL_CH0_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_adc_ch1_sel_pdm_path(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_PDM_ADC_SEL_CH1_MASK;
    else
        val &= ~CODECIP_CTRL4_PDM_ADC_SEL_CH1_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_adc_ch0_sel_pdm_path(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_PDM_ADC_SEL_CH0_MASK;
    else
        val &= ~CODECIP_CTRL4_PDM_ADC_SEL_CH0_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_pdm_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_PDM_ENABLE_MASK;
    else
        val &= ~CODECIP_CTRL4_PDM_ENABLE_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_dual_dac_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CR_REG_OFFSET);
    if (v)
        val |= CODECIP_CR_DUAL_CHANNEL_DAC_MASK;
    else
        val &= ~CODECIP_CR_DUAL_CHANNEL_DAC_MASK;
    codecip_write32(val, reg_base, CODECIP_CR_REG_OFFSET);
}
static inline void codecip_w_dual_dac_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_DUAL_CH_DAC_MASK;
    else
        val &= ~CODECIP_CTRL4_DUAL_CH_DAC_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}
static inline void codecipbus_w_dual_mic_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CR_REG_OFFSET);
    if (v)
        val |= CODECIP_CR_DUAL_MIC_ENABLE_MASK;
    else
        val &= ~CODECIP_CR_DUAL_MIC_ENABLE_MASK;
    codecip_write32(val, reg_base, CODECIP_CR_REG_OFFSET);
}
static inline void codecip_w_dual_mic_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_DUAL_CH_ADC_MASK;
    else
        val &= ~CODECIP_CTRL4_DUAL_CH_ADC_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}

static inline void codecip_w_dac_gain_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL4_DAC_GAIN_SEL_MASK;
    else
        val &= ~CODECIP_CTRL4_DAC_GAIN_SEL_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}

static inline void codecip_w_adc_down_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    val &= ~CODECIP_CTRL4_ADC_DOWN_SEL_MASK;
    if (v)
        val |= (CODECIP_CTRL4_ADC_DOWN_SEL_MASK & (v << CODECIP_CTRL4_ADC_DOWN_SEL_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}

static inline uint32_t codecip_r_adc_down_sel(uint32_t reg_base)
{
    uint32_t val;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    return (val & CODECIP_CTRL4_ADC_DOWN_SEL_MASK) >> CODECIP_CTRL4_ADC_DOWN_SEL_SHIFT;
}

static inline void codecip_w_dac_order_swap(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    val &= ~CODECIP_CTRL4_DAC_LR_SWAP_MASK;
    if (v)
        val |= (CODECIP_CTRL4_DAC_LR_SWAP_MASK & (v << CODECIP_CTRL4_DAC_LR_SWAP_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}

static inline void codecip_w_adc_order_swap(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    val &= ~CODECIP_CTRL4_ADC_LR_SWAP_MASK;
    if (v)
        val |= (CODECIP_CTRL4_ADC_LR_SWAP_MASK & (v << CODECIP_CTRL4_ADC_LR_SWAP_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}

static inline void codecipbus_w_dac_16bit_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CR_REG_OFFSET);
    if (v)
        val |= CODECIP_CR_16BIT_MODE_MASK;
    else
        val &= ~CODECIP_CR_16BIT_MODE_MASK;
    codecip_write32(val, reg_base, CODECIP_CR_REG_OFFSET);
}
static inline void codecip_w_adc_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL1_ADC_EN_MASK;
    else
        val &= ~CODECIP_CTRL1_ADC_EN_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL1_REG_OFFSET);
}
static inline void codecip_w_dac_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL1_DAC_EN_MASK;
    else
        val &= ~CODECIP_CTRL1_DAC_EN_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL1_REG_OFFSET);
}
static inline void codecipbus_w_dac_if_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CR_REG_OFFSET);
    if (v)
        val |= CODECIP_CR_DAC_ENABLE_MASK;
    else
        val &= ~CODECIP_CR_DAC_ENABLE_MASK;
    codecip_write32(val, reg_base, CODECIP_CR_REG_OFFSET);
}
static inline void codecipbus_w_adc_if_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CR_REG_OFFSET);
    if (v)
        val |= CODECIP_CR_ADC_ENABLE_MASK;
    else
        val &= ~CODECIP_CR_ADC_ENABLE_MASK;
    codecip_write32(val, reg_base, CODECIP_CR_REG_OFFSET);
}
static inline void codecipbus_w_codec_if_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_DMA_CTRL_REG_OFFSET);
    if (v)
        val |= CODECIP_DMA_CTRL_CODEC_IF_ENABLE_MASK;
    else
        val &= ~CODECIP_DMA_CTRL_CODEC_IF_ENABLE_MASK;
    codecip_write32(val, reg_base, CODECIP_DMA_CTRL_REG_OFFSET);
}
static inline void codecipbus_w_en(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_ENABLE_REG_REG_OFFSET);
    if (v)
        val |= CODECIP_ENABLE_REG_CODEC_ENABLE_MASK;
    else
        val &= ~CODECIP_ENABLE_REG_CODEC_ENABLE_MASK;
    codecip_write32(val, reg_base, CODECIP_ENABLE_REG_REG_OFFSET);
}
static inline void codecipbus_w_tx_fifo_reset(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_TX_BLOCK_FIFO_RESET_REG_OFFSET);
    if (v)
        val |= CODECIP_TX_BLOCK_FIFO_RESET_RESET_MASK;
    else
        val &= ~CODECIP_TX_BLOCK_FIFO_RESET_RESET_MASK;
    codecip_write32(val, reg_base, CODECIP_TX_BLOCK_FIFO_RESET_REG_OFFSET);
}
static inline void codecipbus_w_tx_fifo_threshold(uint32_t reg_base, uint32_t v)
{
    codecip_write32(v<<CODECIP_TX_FIFO_CFG_LEVEL_SHIFT, reg_base, CODECIP_TX_FIFO_CFG_REG_OFFSET);
}
static inline void codecipbus_w_rx_fifo_reset(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_RX_BLOCK_FIFO_RESET_REG_OFFSET);
    if (v)
        val |= CODECIP_RX_BLOCK_FIFO_RESET_RESET_MASK;
    else
        val &= ~CODECIP_RX_BLOCK_FIFO_RESET_RESET_MASK;
    codecip_write32(val, reg_base, CODECIP_RX_BLOCK_FIFO_RESET_REG_OFFSET);
}
static inline void codecipbus_w_rx_fifo_threshold(uint32_t reg_base, uint32_t v)
{
    codecip_write32(v<<CODECIP_RX_FIFO_CFG_LEVEL_SHIFT, reg_base, CODECIP_RX_FIFO_CFG_REG_OFFSET);
}
static inline void codecipbus_w_enable_tx_dma(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_DMA_CTRL_REG_OFFSET);
    if (v)
        val |= CODECIP_DMA_CTRL_TX_ENABLE_MASK;
    else
        val &= ~CODECIP_DMA_CTRL_TX_ENABLE_MASK;

    codecip_write32(val, reg_base, CODECIP_DMA_CTRL_REG_OFFSET);
}
static inline void codecipbus_w_enable_rx_dma(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_DMA_CTRL_REG_OFFSET);
    if (v)
        val |= CODECIP_DMA_CTRL_RX_ENABLE_MASK;
    else
        val &= ~CODECIP_DMA_CTRL_RX_ENABLE_MASK;

    codecip_write32(val, reg_base, CODECIP_DMA_CTRL_REG_OFFSET);
}

static inline void codecipbus_w_enable_sadc_clk_x2_mode(uint32_t reg_base, uint32_t v)
{
    uint32_t val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL1_SADC_CLK_X2_MODE_MASK;
    else
        val &= ~CODECIP_CTRL1_SADC_CLK_X2_MODE_MASK;

    codecip_write32(val, reg_base, CODECIP_CTRL1_REG_OFFSET);
}

static inline uint32_t codecipbus_w_sdac_get_lvolume(uint32_t reg_base)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL2_REG_OFFSET);
    val &= CODECIP_CTRL2_SDAC_L_VOL_MASK;
    val >>= CODECIP_CTRL2_SDAC_L_VOL_SHIFT;

    return val;
}

static inline uint32_t codecipbus_w_sdac_get_rvolume(uint32_t reg_base)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    val &= CODECIP_CTRL1_SDAC_R_VOL_MASK;
    val >>= CODECIP_CTRL1_SDAC_R_VOL_SHIFT;

    return val;
}

static inline void codecipbus_w_sdac_lvolume_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL2_REG_OFFSET);
    val &= ~CODECIP_CTRL2_SDAC_L_VOL_MASK;
    if (v)
        val |= (CODECIP_CTRL2_SDAC_L_VOL_MASK & (v << CODECIP_CTRL2_SDAC_L_VOL_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL2_REG_OFFSET);
}

static inline void codecipbus_w_sdac_rvolume_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    val &= ~CODECIP_CTRL1_SDAC_R_VOL_MASK;
    if (v)
        val |= (CODECIP_CTRL1_SDAC_R_VOL_MASK & (v << CODECIP_CTRL1_SDAC_R_VOL_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL1_REG_OFFSET);
}

static inline void codecipbus_w_sdm_gain_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL2_REG_OFFSET);
    val &= ~CODECIP_CTRL2_SDM_GAIN_MASK;
    if (v)
        val |= (CODECIP_CTRL2_SDM_GAIN_MASK & (v << CODECIP_CTRL2_SDM_GAIN_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL2_REG_OFFSET);
}

static inline void codecipbus_w_sdac_osr_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL3_REG_OFFSET);
    val &= ~CODECIP_CTRL3_SDAC_OSR_SEL_MASK;
    if (v)
        val |= (CODECIP_CTRL3_SDAC_OSR_SEL_MASK & (v << CODECIP_CTRL3_SDAC_OSR_SEL_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL3_REG_OFFSET);
}

static inline uint32_t codecipbus_r_sdac_osr_sel(uint32_t reg_base)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL3_REG_OFFSET);
    return (val & CODECIP_CTRL3_SDAC_OSR_SEL_MASK) >> CODECIP_CTRL3_SDAC_OSR_SEL_SHIFT;
}

static inline void codecipbus_w_dac_up_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    val &= ~CODECIP_CTRL1_DAC_UP_SEL_MASK;
    if (v)
        val |= (CODECIP_CTRL1_DAC_UP_SEL_MASK & (v << CODECIP_CTRL1_DAC_UP_SEL_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL1_REG_OFFSET);
}

static inline uint32_t codecipbus_r_dac_up_sel(uint32_t reg_base)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    return (val & CODECIP_CTRL1_DAC_UP_SEL_MASK) >> CODECIP_CTRL1_DAC_UP_SEL_SHIFT;
}

static inline void codecipbus_w_sadc_vol_ch0_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL2_REG_OFFSET);
    val &= ~CODECIP_CTRL2_SADC_VOL_CH0_MASK;
    if (v)
        val |= (CODECIP_CTRL2_SADC_VOL_CH0_MASK & (v << CODECIP_CTRL2_SADC_VOL_CH0_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL2_REG_OFFSET);
}

static inline void codecipbus_w_sadc_vol_ch1_sel(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL4_REG_OFFSET);
    val &= ~CODECIP_CTRL4_SADC_VOL_CH1_MASK;
    if (v)
        val |= (CODECIP_CTRL4_SADC_VOL_CH1_MASK & (v << CODECIP_CTRL4_SADC_VOL_CH1_SHIFT));
    codecip_write32(val, reg_base, CODECIP_CTRL4_REG_OFFSET);
}

static inline void codecipbus_w_ff_fb_disable(uint32_t reg_base, uint32_t v)
{
    uint32_t val = 0;
    val = codecip_read32(reg_base, CODECIP_CTRL1_REG_OFFSET);
    if (v)
        val |= CODECIP_CTRL1_FF_FB_DIS_MASK;
    else
        val &= ~CODECIP_CTRL1_FF_FB_DIS_MASK;
    codecip_write32(val, reg_base, CODECIP_CTRL1_REG_OFFSET);
}

#ifdef __cplusplus
}
#endif

#endif /* __HAL_CODECIP_H__ */
