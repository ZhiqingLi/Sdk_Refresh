#ifndef __HAL_DMA_COMMON_H__
#define __HAL_DMA_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define HAL_DMA_CHAN_NONE               0xFF

#define HAL_DMA_MAX_DESC_XFER_SIZE      0xFFF

enum HAL_DMA_RET_T {
    HAL_DMA_OK,
    HAL_DMA_ERR,
};

enum HAL_DMA_GET_CHAN_T {
    HAL_DMA_HIGH_PRIO,
    HAL_DMA_LOW_PRIO,
    HAL_DMA_LOW_PRIO_ONLY
};

// DMA Type of DMA controller
enum HAL_DMA_FLOW_CONTROL_T {
    HAL_DMA_FLOW_M2M_DMA            = 0,    /* Memory to memory - DMA control */
    HAL_DMA_FLOW_M2P_DMA            = 1,    /* Memory to peripheral - DMA control */
    HAL_DMA_FLOW_P2M_DMA            = 2,    /* Peripheral to memory - DMA control */
    HAL_DMA_FLOW_P2P_DMA            = 3,    /* Source peripheral to destination peripheral - DMA control */
    HAL_DMA_FLOW_P2P_DSTPERIPH      = 4,    /* Source peripheral to destination peripheral - destination peripheral control */
    HAL_DMA_FLOW_M2P_PERIPH         = 5,    /* Memory to peripheral - peripheral control */
    HAL_DMA_FLOW_P2M_PERIPH         = 6,    /* Peripheral to memory - peripheral control */
    HAL_DMA_FLOW_P2P_SRCPERIPH      = 7,    /* Source peripheral to destination peripheral - source peripheral control */

    HAL_DMA_FLOW_FLAG_SI            = 0x40,
    HAL_DMA_FLOW_FLAG_DI            = 0x80,
    HAL_DMA_FLOW_FLAG_MASK          = (HAL_DMA_FLOW_FLAG_SI | HAL_DMA_FLOW_FLAG_DI),

    HAL_DMA_FLOW_M2P_DI_DMA         = HAL_DMA_FLOW_M2P_DMA | HAL_DMA_FLOW_FLAG_DI,
    HAL_DMA_FLOW_P2M_SI_DMA         = HAL_DMA_FLOW_P2M_DMA | HAL_DMA_FLOW_FLAG_SI,
    HAL_DMA_FLOW_P2P_DI_DMA         = HAL_DMA_FLOW_P2P_DMA | HAL_DMA_FLOW_FLAG_DI,
    HAL_DMA_FLOW_P2P_SI_DMA         = HAL_DMA_FLOW_P2P_DMA | HAL_DMA_FLOW_FLAG_SI,
};

// DMA Burst size in Source and Destination definitions
enum HAL_DMA_BSIZE_T {
    HAL_DMA_BSIZE_1           = 0,    /* Burst size = 1 */
    HAL_DMA_BSIZE_4           = 1,    /* Burst size = 4 */
    HAL_DMA_BSIZE_8           = 2,    /* Burst size = 8 */
    HAL_DMA_BSIZE_16          = 3,    /* Burst size = 16 */
    HAL_DMA_BSIZE_32          = 4,    /* Burst size = 32 */
    HAL_DMA_BSIZE_64          = 5,    /* Burst size = 64 */
    HAL_DMA_BSIZE_128         = 6,    /* Burst size = 128 */
    HAL_DMA_BSIZE_256         = 7,    /* Burst size = 256 */
};

// Width in Source transfer width and Destination transfer width definitions
enum HAL_DMA_WDITH_T {
    HAL_DMA_WIDTH_BYTE        = 0,    /* Width = 1 byte */
    HAL_DMA_WIDTH_HALFWORD    = 1,    /* Width = 2 bytes */
    HAL_DMA_WIDTH_WORD        = 2,    /* Width = 4 bytes */
};

enum HAL_DMA_PERIPH_T {
    HAL_DMA_PERIPH_NULL         = 0,
    HAL_GPDMA_MEM               = 1,
    HAL_AUDMA_MEM               = 2,

    HAL_GPDMA_SDIO              = 10,
    HAL_GPDMA_SDMMC             = 11,
    HAL_GPDMA_I2C0_RX           = 12,
    HAL_GPDMA_I2C0_TX           = 13,
    HAL_GPDMA_SPI_RX            = 14,
    HAL_GPDMA_SPI_TX            = 15,
    HAL_GPDMA_SPILCD_RX         = 16,
    HAL_GPDMA_SPILCD_TX         = 17,
    HAL_GPDMA_UART0_RX          = 18,
    HAL_GPDMA_UART0_TX          = 19,
    HAL_GPDMA_UART1_RX          = 20,
    HAL_GPDMA_UART1_TX          = 21,
    HAL_GPDMA_ISPI_TX           = 22,
    HAL_GPDMA_ISPI_RX           = 23,
    HAL_GPDMA_UART2_RX          = 24,
    HAL_GPDMA_UART2_TX          = 25,
    HAL_GPDMA_NORFLASH          = 26,
    HAL_GPDMA_I2C1_RX           = 27,
    HAL_GPDMA_I2C1_TX           = 28,

    HAL_AUDMA_CODEC_RX          = 30,
    HAL_AUDMA_CODEC_TX          = 31,
    HAL_AUDMA_BTPCM_RX          = 32,
    HAL_AUDMA_BTPCM_TX          = 33,
    HAL_AUDMA_I2S_RX            = 34,
    HAL_AUDMA_I2S_TX            = 35,
    HAL_AUDMA_DPD_RX            = 36,
    HAL_AUDMA_DPD_TX            = 37,
    HAL_AUDMA_SPDIF0_RX         = 38,
    HAL_AUDMA_SPDIF0_TX         = 39,
    HAL_AUDMA_SPDIF1_RX         = 40,
    HAL_AUDMA_SPDIF1_TX         = 41,
    HAL_AUDMA_DSD_RX            = 42,
    HAL_AUDMA_DSD_TX            = 43,
    HAL_AUDMA_MC_RX             = 44,
    HAL_AUDMA_FFT_RX            = 45,
    HAL_AUDMA_FFT_TX            = 46,
    HAL_AUDMA_FIR_RX            = 47,
    HAL_AUDMA_FIR_TX            = 48,
    HAL_AUDMA_IIR_RX            = 49,
    HAL_AUDMA_IIR_TX            = 50,
    HAL_AUDMA_BT_DUMP           = 51,
};

struct HAL_DMA_DESC_T;

typedef void (*HAL_DMA_IRQ_HANDLER_T)(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli);

typedef void (*HAL_DMA_START_CALLBACK_T)(uint8_t chan);

typedef void (*HAL_DMA_DELAY_FUNC)(uint32_t ms);

// DMA structure using for DMA configuration
struct HAL_DMA_CH_CFG_T {
    uint8_t ch;                         /* DMA channel number */
    uint8_t try_burst;
    uint16_t src_tsize;                 /* Length/Size of transfer */
    enum HAL_DMA_WDITH_T src_width;
    enum HAL_DMA_WDITH_T dst_width;
    enum HAL_DMA_BSIZE_T src_bsize;
    enum HAL_DMA_BSIZE_T dst_bsize;
    enum HAL_DMA_FLOW_CONTROL_T type;   /* Transfer Type */
    enum HAL_DMA_PERIPH_T src_periph;
    enum HAL_DMA_PERIPH_T dst_periph;
    uint32_t src;                       /* Physical Source Address */
    uint32_t dst;                       /* Physical Destination Address */
    HAL_DMA_IRQ_HANDLER_T handler;
    HAL_DMA_START_CALLBACK_T start_cb;
};

// Transfer Descriptor structure typedef
struct HAL_DMA_DESC_T {
    uint32_t src;       /* Source address */
    uint32_t dst;       /* Destination address */
    uint32_t lli;       /* Pointer to next descriptor structure */
    uint32_t ctrl;      /* Control word that has transfer size, type etc. */
};

// DMA 2D configuration structure
struct HAL_DMA_2D_CFG_T {
    int16_t xmodify;
    uint16_t xcount;
    int16_t ymodify;
    uint16_t ycount;
};

//=============================================================

void hal_dma_open(void);

void hal_dma_close(void);

bool hal_dma_chan_busy(uint8_t ch);

uint8_t hal_dma_get_chan(enum HAL_DMA_PERIPH_T periph, enum HAL_DMA_GET_CHAN_T policy);

void hal_dma_free_chan(uint8_t ch);

uint32_t hal_dma_cancel(uint8_t ch);

uint32_t hal_dma_stop(uint8_t ch);

enum HAL_DMA_RET_T hal_dma_init_desc(struct HAL_DMA_DESC_T *desc,
                                     const struct HAL_DMA_CH_CFG_T *cfg,
                                     const struct HAL_DMA_DESC_T *next,
                                     int tc_irq);

enum HAL_DMA_RET_T hal_dma_sg_start(const struct HAL_DMA_DESC_T *desc,
                                    const struct HAL_DMA_CH_CFG_T *cfg);

enum HAL_DMA_RET_T hal_dma_sg_2d_start(const struct HAL_DMA_DESC_T *desc,
                                       const struct HAL_DMA_CH_CFG_T *cfg,
                                       const struct HAL_DMA_2D_CFG_T *src_2d,
                                       const struct HAL_DMA_2D_CFG_T *dst_2d);

enum HAL_DMA_RET_T hal_dma_start(const struct HAL_DMA_CH_CFG_T *cfg);

uint32_t hal_dma_get_cur_src_addr(uint8_t ch);

uint32_t hal_dma_get_cur_dst_addr(uint8_t ch);

uint32_t hal_dma_get_sg_remain_size(uint8_t ch);

void hal_dma_irq_run_chan(uint8_t ch);

bool hal_dma_busy(void);

HAL_DMA_DELAY_FUNC hal_dma_set_delay_func(HAL_DMA_DELAY_FUNC new_func);

void hal_dma_remap_periph(enum HAL_DMA_PERIPH_T periph, int enable);

//=============================================================

#define hal_audma_open                  hal_dma_open
#define hal_audma_close                 hal_dma_close
#define hal_audma_chan_busy             hal_dma_chan_busy
#define hal_audma_get_chan              hal_dma_get_chan
#define hal_audma_free_chan             hal_dma_free_chan
#define hal_audma_cancel                hal_dma_cancel
#define hal_audma_stop                  hal_dma_stop
#define hal_audma_init_desc             hal_dma_init_desc
#define hal_audma_sg_start              hal_dma_sg_start
#define hal_audma_start                 hal_dma_start
#define hal_audma_get_cur_src_addr      hal_dma_get_cur_src_addr
#define hal_audma_get_cur_dst_addr      hal_dma_get_cur_dst_addr
#define hal_audma_get_sg_remain_size    hal_dma_get_sg_remain_size
#define hal_audma_irq_run_chan          hal_dma_irq_run_chan

#define hal_gpdma_open                  hal_dma_open
#define hal_gpdma_close                 hal_dma_close
#define hal_gpdma_chan_busy             hal_dma_chan_busy
#define hal_gpdma_get_chan              hal_dma_get_chan
#define hal_gpdma_free_chan             hal_dma_free_chan
#define hal_gpdma_cancel                hal_dma_cancel
#define hal_gpdma_stop                  hal_dma_stop
#define hal_gpdma_init_desc             hal_dma_init_desc
#define hal_gpdma_sg_start              hal_dma_sg_start
#define hal_gpdma_start                 hal_dma_start
#define hal_gpdma_get_cur_src_addr      hal_dma_get_cur_src_addr
#define hal_gpdma_get_cur_dst_addr      hal_dma_get_cur_dst_addr
#define hal_gpdma_get_sg_remain_size    hal_dma_get_sg_remain_size
#define hal_gpdma_irq_run_chan          hal_dma_irq_run_chan

//=============================================================

#ifdef __cplusplus
}
#endif

#endif
