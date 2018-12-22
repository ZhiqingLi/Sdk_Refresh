#include "include.h"

#if I2S_EN

#define I2S_DMA_BUF_LEN         512
#define I2S_DMA_BUF_INDEX       5
typedef struct {
    volatile u8 wptr;
    volatile u8 rptr;
    u16 dma_cnt;
    u32 dma_in0;
    u32 dma_in1;
    u32 dma_out0;
    u32 dma_out1;
    u8 (*buf)[I2S_DMA_BUF_LEN];
}i2s_dma_t;

typedef struct {
    u8 mapping      : 1;
    u8 mode         : 1;
    u8 bit_mode     : 1;
    u8 data_mode    : 1;
    u8 dma_en       : 1;
    u8 mclk_en      : 1;
    u8 mclk_sel     : 2;
    u8 pcm_mode_en  : 1;
    i2s_dma_t i2s_dma;
}i2s_cfg_t;

i2s_cfg_t i2s_cfg;
void i2s_init(void *cfg);

void bsp_i2s_init(void)
{
    i2s_cfg.mapping     = I2S_MAPPING_SEL;
    i2s_cfg.mode        = I2S_MODE_SEL;
    i2s_cfg.bit_mode    = I2S_BIT_MODE;
    i2s_cfg.data_mode   = I2S_DATA_MODE;
    i2s_cfg.dma_en      = I2S_DMA_EN;
    i2s_cfg.mclk_en     = I2S_MCLK_EN;
    i2s_cfg.mclk_sel    = I2S_MCLK_SEL;
    i2s_cfg.pcm_mode_en = I2S_PCM_MODE;
    i2s_init(&i2s_cfg);
}
#endif
