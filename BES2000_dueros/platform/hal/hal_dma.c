#include "plat_addr_map.h"
#include "reg_dma.h"
#include "hal_dma.h"
#include "hal_trace.h"
#include "cmsis_nvic.h"
#include "hal_timer.h"
#include "hal_chipid.h"
#include "hal_cmu.h"

#if (CHIP_HAS_DMA == 1) || defined(ROM_BUILD) || defined(PROGRAMMER)
#define NO_DMA_REMAP
#endif

enum HAL_DMA_INST_T {
    HAL_DMA_INST_AUDMA = 0,
#if (CHIP_HAS_DMA == 1)
    HAL_DMA_INST_GPDMA = HAL_DMA_INST_AUDMA,
#else
    HAL_DMA_INST_GPDMA,
#endif

    HAL_DMA_INST_QTY
};

struct HAL_DMA_FIFO_ADDR_T {
    uint32_t count;
    const uint32_t *addr;
};

struct HAL_DMA_FIFO_PERIPH_T {
    uint32_t count;
    const enum HAL_DMA_PERIPH_T *periph;
#ifndef NO_DMA_REMAP
    const enum HAL_DMA_PERIPH_T *periph_remap;
#endif
};

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static struct DMA_T * const dma[HAL_DMA_INST_QTY] = {
    (struct DMA_T *)AUDMA_BASE,
#if (CHIP_HAS_DMA > 1)
    (struct DMA_T *)GPDMA_BASE
#endif
};

static const IRQn_Type irq_type[HAL_DMA_INST_QTY] = {
    AUDMA_IRQn,
#if (CHIP_HAS_DMA > 1)
    GPDMA_IRQn
#endif
};

static void hal_audma_irq_handler(void);
#if (CHIP_HAS_DMA > 1)
static void hal_gpdma_irq_handler(void);
#endif
static const uint32_t irq_entry[HAL_DMA_INST_QTY] = {
    (uint32_t)hal_audma_irq_handler,
#if (CHIP_HAS_DMA > 1)
    (uint32_t)hal_gpdma_irq_handler,
#endif
};

#if 0

#elif defined(CHIP_BEST3001)

#define AUDMA_PERIPH_NUM                        16
#define AUDMA_CHAN_NUM                          6
#define AUDMA_CHAN_START                        (0)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x1C,      // CODEC RX
    CODEC_BASE + 0x1C,      // CODEC TX
    CODEC_BASE + 0x28,      // DSD RX
    CODEC_BASE + 0x28,      // DSD TX
    UART0_BASE + 0x00,      // UART0 RX
    UART0_BASE + 0x00,      // UART0 TX
    UART1_BASE + 0x00,      // UART1 RX
    UART1_BASE + 0x00,      // UART1 TX
    I2C0_BASE + 0x010,      // I2C RX
    I2C0_BASE + 0x010,      // I2C TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    SPDIF0_BASE+ 0x1c0,     // SPDIF0 RX
    SPDIF0_BASE+ 0x1c8,     // SPDIF0 TX
    I2S_BASE + 0x1C0,       // I2S RX
    I2S_BASE + 0x1C8,       // I2S TX
};

static enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_DSD_TX,
    HAL_AUDMA_DSD_RX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
    HAL_AUDMA_I2S_RX,
    HAL_AUDMA_I2S_TX,
};

static const struct HAL_DMA_FIFO_ADDR_T fifo_addr[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_addr),
        .addr = audma_fifo_addr,
    },
};

static const struct HAL_DMA_FIFO_PERIPH_T fifo_periph[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_periph),
        .periph = audma_fifo_periph,
    },
};

#elif defined(CHIP_BEST2300)

#define AUDMA_PERIPH_NUM                        16
#define GPDMA_PERIPH_NUM                        16

#define AUDMA_CHAN_NUM                          8
#define GPDMA_CHAN_NUM                          8

#define AUDMA_CHAN_START                        (DMA_NUMBER_CHANNELS - AUDMA_CHAN_NUM)
#define GPDMA_CHAN_START                        (DMA_NUMBER_CHANNELS - GPDMA_CHAN_NUM)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x01C,     // CODEC RX
    CODEC_BASE + 0x01C,     // CODEC TX
    BTPCM_BASE + 0x1C0,     // BTPCM RX
    BTPCM_BASE + 0x1C8,     // BTPCM TX
    I2S_BASE + 0x200,       // I2S RX
    I2S_BASE + 0x240,       // I2S TX
    0,                      // FIR RX
    0,                      // FIR TX
    SPDIF0_BASE + 0x1C0,    // SPDIF0 RX
    SPDIF0_BASE + 0x1C8,    // SPDIF0 TX
    CODEC_BASE + 0x03C,     // IIR RX
    CODEC_BASE + 0x03C,     // IIR TX
    0,                      // BT Dump
    CODEC_BASE + 0x038,     // MC RX
    FFT_BASE + 0x000,       // FFT RX (TODO: Correct FIFO address)
    FFT_BASE + 0x000,       // FFT TX (TODO: Correct FIFO address)
};

static const enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_BTPCM_RX,
    HAL_AUDMA_BTPCM_TX,
    HAL_AUDMA_I2S_RX,
    HAL_AUDMA_I2S_TX,
    HAL_AUDMA_DPD_RX,
    HAL_AUDMA_DPD_TX,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
    HAL_AUDMA_IIR_RX,
    HAL_AUDMA_IIR_TX,
    HAL_AUDMA_BT_DUMP,
    HAL_AUDMA_MC_RX,
    HAL_AUDMA_FFT_RX,
    HAL_AUDMA_FFT_TX,
};

static const uint32_t gpdma_fifo_addr[GPDMA_PERIPH_NUM] = {
    FLASH_CTRL_BASE + 0x008, // FLASH CTRL
    SDMMC_BASE + 0x200,     // SDMMC
    I2C0_BASE + 0x010,      // I2C RX
    I2C0_BASE + 0x010,      // I2C TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    SPILCD_BASE + 0x008,    // SPILCD RX
    SPILCD_BASE + 0x008,    // SPILCD TX
    UART0_BASE + 0x000,     // UART0 RX
    UART0_BASE + 0x000,     // UART0 TX
    UART1_BASE + 0x000,     // UART1 RX
    UART1_BASE + 0x000,     // UART1 TX
    ISPI_BASE + 0x008,      // ISPI TX
    ISPI_BASE + 0x008,      // ISPI RX
    UART2_BASE + 0x000,     // UART2 RX
    UART2_BASE + 0x000,     // UART2 TX
};

static const enum HAL_DMA_PERIPH_T gpdma_fifo_periph[GPDMA_PERIPH_NUM] = {
    HAL_GPDMA_NORFLASH,
    HAL_GPDMA_SDMMC,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_GPDMA_SPILCD_RX,
    HAL_GPDMA_SPILCD_TX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_I2C1_RX,
    HAL_GPDMA_I2C1_TX,
    HAL_GPDMA_UART2_RX,
    HAL_GPDMA_UART2_TX,
};

static const struct HAL_DMA_FIFO_ADDR_T fifo_addr[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_addr),
        .addr = audma_fifo_addr,
    },
    {
        .count = ARRAY_SIZE(gpdma_fifo_addr),
        .addr = gpdma_fifo_addr,
    },
};

static const struct HAL_DMA_FIFO_PERIPH_T fifo_periph[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_periph),
        .periph = audma_fifo_periph,
#ifndef NO_DMA_REMAP
        .periph_remap = gpdma_fifo_periph,
#endif
    },
    {
        .count = ARRAY_SIZE(gpdma_fifo_periph),
        .periph = gpdma_fifo_periph,
#ifndef NO_DMA_REMAP
        .periph_remap = audma_fifo_periph,
#endif
    },
};

#elif defined(CHIP_BEST2000)

#define AUDMA_PERIPH_NUM                        16
#define GPDMA_PERIPH_NUM                        16

#define AUDMA_CHAN_NUM                          8
#define GPDMA_CHAN_NUM                          8

#define AUDMA_CHAN_START                        (DMA_NUMBER_CHANNELS - AUDMA_CHAN_NUM)
#define GPDMA_CHAN_START                        (DMA_NUMBER_CHANNELS - GPDMA_CHAN_NUM)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x008,     // CODEC RX
    CODEC_BASE + 0x008,     // CODEC TX
    BTPCM_BASE + 0x1C0,     // BTPCM RX
    BTPCM_BASE + 0x1C8,     // BTPCM TX
    I2S_BASE + 0x200,       // I2S RX
    I2S_BASE + 0x240,       // I2S TX
    0,                      // FIR RX
    0,                      // FIR TX
    SPDIF0_BASE + 0x1C0,    // SPDIF0 RX
    SPDIF0_BASE + 0x1C8,    // SPDIF0 TX
    SPDIF1_BASE + 0x1C0,    // SPDIF1 RX
    SPDIF1_BASE + 0x1C8,    // SPDIF1 TX
    DSD_BASE + 0x000,       // DSD RX (TODO: Correct FIFO address)
    CODEC_BASE + 0x054,     // MC RX
    FFT_BASE + 0x000,       // FFT RX (TODO: Correct FIFO address)
    FFT_BASE + 0x000,       // FFT TX (TODO: Correct FIFO address)
};

static const enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_BTPCM_RX,
    HAL_AUDMA_BTPCM_TX,
    HAL_AUDMA_I2S_RX,
    HAL_AUDMA_I2S_TX,
    HAL_AUDMA_DPD_RX,
    HAL_AUDMA_DPD_TX,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
    HAL_AUDMA_SPDIF1_RX,
    HAL_AUDMA_SPDIF1_TX,
    HAL_AUDMA_DSD_RX,
    HAL_AUDMA_MC_RX,
    HAL_AUDMA_FFT_RX,
    HAL_AUDMA_FFT_TX,
};

static const uint32_t gpdma_fifo_addr[GPDMA_PERIPH_NUM] = {
    SDIO_BASE + 0x200,      // SDIO
    SDMMC_BASE + 0x200,     // SDMMC
    I2C0_BASE + 0x010,      // I2C RX
    I2C0_BASE + 0x010,      // I2C TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    SPILCD_BASE + 0x008,    // SPILCD RX
    SPILCD_BASE + 0x008,    // SPILCD TX
    UART0_BASE + 0x000,     // UART0 RX
    UART0_BASE + 0x000,     // UART0 TX
    UART1_BASE + 0x000,     // UART1 RX
    UART1_BASE + 0x000,     // UART1 TX
    ISPI_BASE + 0x008,      // ISPI TX
    ISPI_BASE + 0x008,      // ISPI RX
    UART2_BASE + 0x000,     // UART2 RX
    UART2_BASE + 0x000,     // UART2 TX
};

static const enum HAL_DMA_PERIPH_T gpdma_fifo_periph[GPDMA_PERIPH_NUM] = {
    HAL_GPDMA_SDIO,
    HAL_GPDMA_SDMMC,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_GPDMA_SPILCD_RX,
    HAL_GPDMA_SPILCD_TX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_ISPI_RX,
    HAL_GPDMA_ISPI_TX,
    HAL_GPDMA_UART2_RX,
    HAL_GPDMA_UART2_TX,
};

static const struct HAL_DMA_FIFO_ADDR_T fifo_addr[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_addr),
        .addr = audma_fifo_addr,
    },
    {
        .count = ARRAY_SIZE(gpdma_fifo_addr),
        .addr = gpdma_fifo_addr,
    },
};

static const struct HAL_DMA_FIFO_PERIPH_T fifo_periph[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_periph),
        .periph = audma_fifo_periph,
#ifndef NO_DMA_REMAP
        .periph_remap = gpdma_fifo_periph,
#endif
    },
    {
        .count = ARRAY_SIZE(gpdma_fifo_periph),
        .periph = gpdma_fifo_periph,
#ifndef NO_DMA_REMAP
        .periph_remap = audma_fifo_periph,
#endif
    },
};

#elif defined(CHIP_BEST1200)

#define AUDMA_PERIPH_NUM                        10
#define AUDMA_CHAN_NUM                          4
#define AUDMA_CHAN_START                        (DMA_NUMBER_CHANNELS - AUDMA_CHAN_NUM)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x1C0,     // CODEC RX
    CODEC_BASE + 0x1C8,     // CODEC TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    I2S_BASE + 0x1C0,       // I2S RX
    I2S_BASE + 0x1C8,       // I2S TX
    I2C0_BASE + 0x010,      // I2C RX
    I2C0_BASE + 0x010,      // I2C TX
    UART0_BASE + 0x000,     // UART0 RX
    UART0_BASE + 0x000,     // UART0 TX
};

static enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_AUDMA_I2S_RX,
    HAL_AUDMA_I2S_TX,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
};

static const struct HAL_DMA_FIFO_ADDR_T fifo_addr[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_addr),
        .addr = audma_fifo_addr,
    },
};

static const struct HAL_DMA_FIFO_PERIPH_T fifo_periph[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_periph),
        .periph = audma_fifo_periph,
    },
};

#else // CHIP_BEST1000

#define AUDMA_PERIPH_NUM                        10
#define GPDMA_PERIPH_NUM                        14

#define AUDMA_CHAN_NUM                          4
#define GPDMA_CHAN_NUM                          8

#define AUDMA_CHAN_START                        (DMA_NUMBER_CHANNELS - AUDMA_CHAN_NUM)
#define GPDMA_CHAN_START                        (DMA_NUMBER_CHANNELS - GPDMA_CHAN_NUM)

static const uint32_t audma_fifo_addr[AUDMA_PERIPH_NUM] = {
    CODEC_BASE + 0x1C0,     // CODEC RX
    CODEC_BASE + 0x1C8,     // CODEC TX
    BTPCM_BASE + 0x1C0,     // BTPCM RX
    BTPCM_BASE + 0x1C8,     // BTPCM TX
    I2S_BASE + 0x1C0,       // I2S RX
    I2S_BASE + 0x1C8,       // I2S TX
    DPD_RX_BASE + 0x034,    // DPD RX
    DPD_TX_BASE + 0x034,    // DPD TX
    SPDIF0_BASE + 0x1C0,    // SPDIF0 RX
    SPDIF0_BASE + 0x1C8,    // SPDIF0 TX
};

static const enum HAL_DMA_PERIPH_T audma_fifo_periph[AUDMA_PERIPH_NUM] = {
    HAL_AUDMA_CODEC_RX,
    HAL_AUDMA_CODEC_TX,
    HAL_AUDMA_BTPCM_RX,
    HAL_AUDMA_BTPCM_TX,
    HAL_AUDMA_I2S_RX,
    HAL_AUDMA_I2S_TX,
    HAL_AUDMA_DPD_RX,
    HAL_AUDMA_DPD_TX,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
};

#ifndef NO_DMA_REMAP
static const enum HAL_DMA_PERIPH_T audma_fifo_periph_remap[AUDMA_PERIPH_NUM] = {
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_GPDMA_ISPI_TX,
    HAL_GPDMA_ISPI_RX,
};
#endif

static const uint32_t gpdma_fifo_addr[GPDMA_PERIPH_NUM] = {
    SDIO_BASE + 0x200,      // SDIO
    SDMMC_BASE + 0x200,     // SDMMC
    I2C0_BASE + 0x010,      // I2C RX
    I2C0_BASE + 0x010,      // I2C TX
    SPI_BASE + 0x008,       // SPI RX
    SPI_BASE + 0x008,       // SPI TX
    SPILCD_BASE + 0x008,    // SPILCD RX
    SPILCD_BASE + 0x008,    // SPILCD TX
    UART0_BASE + 0x000,     // UART0 RX
    UART0_BASE + 0x000,     // UART0 TX
    UART1_BASE + 0x000,     // UART1 RX
    UART1_BASE + 0x000,     // UART1 TX
    ISPI_BASE + 0x008,      // ISPI TX
    ISPI_BASE + 0x008,      // ISPI RX
};

static const enum HAL_DMA_PERIPH_T gpdma_fifo_periph[GPDMA_PERIPH_NUM] = {
    HAL_GPDMA_SDIO,
    HAL_GPDMA_SDMMC,
    HAL_GPDMA_I2C0_RX,
    HAL_GPDMA_I2C0_TX,
    HAL_GPDMA_SPI_RX,
    HAL_GPDMA_SPI_TX,
    HAL_GPDMA_SPILCD_RX,
    HAL_GPDMA_SPILCD_TX,
    HAL_GPDMA_UART0_RX,
    HAL_GPDMA_UART0_TX,
    HAL_GPDMA_UART1_RX,
    HAL_GPDMA_UART1_TX,
    HAL_GPDMA_ISPI_TX,
    HAL_GPDMA_ISPI_RX,
};

#ifndef NO_DMA_REMAP
static const enum HAL_DMA_PERIPH_T gpdma_fifo_periph_remap[GPDMA_PERIPH_NUM] = {
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_AUDMA_I2S_RX,
    HAL_AUDMA_I2S_TX,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_DMA_PERIPH_NULL,
    HAL_AUDMA_SPDIF0_RX,
    HAL_AUDMA_SPDIF0_TX,
};
#endif

static const struct HAL_DMA_FIFO_ADDR_T fifo_addr[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_addr),
        .addr = audma_fifo_addr,
    },
    {
        .count = ARRAY_SIZE(gpdma_fifo_addr),
        .addr = gpdma_fifo_addr,
    },
};

static const struct HAL_DMA_FIFO_PERIPH_T fifo_periph[HAL_DMA_INST_QTY] = {
    {
        .count = ARRAY_SIZE(audma_fifo_periph),
        .periph = audma_fifo_periph,
#ifndef NO_DMA_REMAP
        .periph_remap = audma_fifo_periph_remap,
#endif
    },
    {
        .count = ARRAY_SIZE(gpdma_fifo_periph),
        .periph = gpdma_fifo_periph,
#ifndef NO_DMA_REMAP
        .periph_remap = gpdma_fifo_periph_remap,
#endif
    },
};

#endif // CHIP_BEST1000

static const uint8_t chan_start[HAL_DMA_INST_QTY] = {
    AUDMA_CHAN_START,
#if (CHIP_HAS_DMA > 1)
    GPDMA_CHAN_START,
#endif
};

static const uint8_t chan_num[HAL_DMA_INST_QTY] = {
    AUDMA_CHAN_NUM,
#if (CHIP_HAS_DMA > 1)
    GPDMA_CHAN_NUM,
#endif
};

/* Channel array to monitor free channel */
static bool chan_enabled[HAL_DMA_INST_QTY][DMA_NUMBER_CHANNELS];

static HAL_DMA_IRQ_HANDLER_T handler[HAL_DMA_INST_QTY][DMA_NUMBER_CHANNELS];

#ifndef NO_DMA_REMAP
static uint32_t periph_remap_bitmap[HAL_DMA_INST_QTY];
#endif

static const char * const err_invalid_inst = "Invalid DMA inst: %d";

static const char * const err_invalid_chan[HAL_DMA_INST_QTY] = {
    "Invalid AUDMA channel: %d",
#if (CHIP_HAS_DMA > 1)
    "Invalid GPDMA channel: %d",
#endif
};

static bool dma_opened = false;

static HAL_DMA_DELAY_FUNC dma_delay = NULL;


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void hal_dma_delay(uint32_t ms)
{
    if (dma_delay) {
        dma_delay(ms);
    } else {
        hal_sys_timer_delay(MS_TO_TICKS(ms));
    }
}

static inline uint8_t generate_chan(enum HAL_DMA_INST_T inst, uint8_t hwch)
{
    return ((inst << 4) | (hwch & 0xF));
}

static inline enum HAL_DMA_INST_T get_inst_from_chan(uint8_t ch)
{
    return (enum HAL_DMA_INST_T)(ch >> 4);
}

static inline uint8_t get_hwch_from_chan(uint8_t ch)
{
    return (ch & 0xF);
}

static inline int get_index_from_periph(enum HAL_DMA_PERIPH_T periph, enum HAL_DMA_INST_T *pinst, uint8_t *pidx)
{
    enum HAL_DMA_INST_T inst;
    int i;

    for (inst = 0; inst < HAL_DMA_INST_QTY; inst++) {
        for (i = 0; i < fifo_periph[inst].count; i++) {
            if (fifo_periph[inst].periph[i] == periph) {
                *pinst = inst;
                *pidx = i;
                return 0;
            }
        }
    }

    return 1;
}

#ifndef NO_DMA_REMAP
static inline int get_remap_index_from_periph(enum HAL_DMA_INST_T inst, enum HAL_DMA_PERIPH_T periph, uint8_t *pidx)
{
    int i;

    if (fifo_periph[inst].periph_remap == NULL) {
        return 1;
    }

    for (i = 0; i < fifo_periph[inst].count; i++) {
        if (fifo_periph[inst].periph_remap[i] == periph) {
            *pidx = i;
            return 0;
        }
    }

    return 2;
}
#endif

static inline uint32_t hal_dma_get_periph_addr(enum HAL_DMA_PERIPH_T periph)
{
    int ret;
    enum HAL_DMA_INST_T inst;
    uint8_t index;

    ret = get_index_from_periph(periph, &inst, &index);
    if (ret) {
        return 0;
    }

    return fifo_addr[inst].addr[index];
}

/* Initialize the DMA */
static void hal_dma_open_inst(enum HAL_DMA_INST_T inst)
{
    uint8_t i;

    /* Reset all channel configuration register */
    for (i = 0; i < DMA_NUMBER_CHANNELS; i++) {
        dma[inst]->CH[i].CONFIG = 0;
    }

    /* Clear all DMA interrupt and error flag */
    dma[inst]->INTTCCLR = ~0;
    dma[inst]->INTERRCLR = ~0;

    /* Reset all channels are free */
    for (i = 0; i < DMA_NUMBER_CHANNELS; i++) {
        chan_enabled[inst][i] = false;
    }

    NVIC_SetVector(irq_type[inst], irq_entry[inst]);
    if (inst == HAL_DMA_INST_AUDMA) {
        NVIC_SetPriority(irq_type[inst], IRQ_PRIORITY_ABOVENORMAL);
    } else {
        NVIC_SetPriority(irq_type[inst], IRQ_PRIORITY_NORMAL);
    }
    NVIC_ClearPendingIRQ(irq_type[inst]);
    NVIC_EnableIRQ(irq_type[inst]);
}

/* Shutdown the DMA */
static void hal_dma_close_inst(enum HAL_DMA_INST_T inst)
{
    NVIC_DisableIRQ(irq_type[inst]);
    dma[inst]->DMACONFIG = 0;
}

static bool hal_dma_chan_busy_inst(enum HAL_DMA_INST_T inst, uint8_t hwch)
{
    return !!(dma[inst]->ENBLDCHNS & DMA_STAT_CHAN(hwch));
}

static void hal_dma_irq_handler(enum HAL_DMA_INST_T inst)
{
    int hwch;
    uint32_t remains;
    struct HAL_DMA_DESC_T *lli;
    bool tcint, errint;

    for (hwch = 0; hwch < DMA_NUMBER_CHANNELS; hwch++) {
        if ((dma[inst]->INTSTAT & DMA_STAT_CHAN(hwch)) == 0) {
            continue;
        }

        /* Check counter terminal status */
        tcint = !!(dma[inst]->INTTCSTAT & DMA_STAT_CHAN(hwch));
        /* Check error terminal status */
        errint = !!(dma[inst]->INTERRSTAT & DMA_STAT_CHAN(hwch));

        if (tcint || errint) {
            if (tcint) {
                /* Clear terminate counter Interrupt pending */
                dma[inst]->INTTCCLR = DMA_STAT_CHAN(hwch);
            }
            if (errint) {
                /* Clear error counter Interrupt pending */
                dma[inst]->INTERRCLR = DMA_STAT_CHAN(hwch);
            }

            if (handler[inst][hwch]) {
                remains = GET_BITFIELD(dma[inst]->CH[hwch].CONTROL, DMA_CONTROL_TRANSFERSIZE);
                lli = (struct HAL_DMA_DESC_T *)dma[inst]->CH[hwch].LLI;
                handler[inst][hwch](generate_chan(inst, hwch), remains, errint, lli);
            }
        }
    }
}

static void hal_audma_irq_handler(void)
{
    hal_dma_irq_handler(HAL_DMA_INST_AUDMA);
}

#if (CHIP_HAS_DMA > 1)
static void hal_gpdma_irq_handler(void)
{
    hal_dma_irq_handler(HAL_DMA_INST_GPDMA);
}
#endif

static enum HAL_DMA_RET_T hal_dma_init_control(uint32_t *ctrl,
                                               const struct HAL_DMA_CH_CFG_T *cfg,
                                               int tc_irq)
{
    uint32_t addr_inc;
    enum HAL_DMA_FLOW_CONTROL_T type;

    if (cfg->src_tsize > HAL_DMA_MAX_DESC_XFER_SIZE) {
        return HAL_DMA_ERR;
    }

#ifdef CHIP_BEST1000
    type = cfg->type;
#else
    type = cfg->type & ~HAL_DMA_FLOW_FLAG_MASK;
#endif

    switch (type) {
        case HAL_DMA_FLOW_M2M_DMA:
            addr_inc = DMA_CONTROL_SI | DMA_CONTROL_DI;
            break;
        case HAL_DMA_FLOW_M2P_DMA:
        case HAL_DMA_FLOW_M2P_PERIPH:
            addr_inc = DMA_CONTROL_SI;
            break;
        case HAL_DMA_FLOW_P2M_DMA:
        case HAL_DMA_FLOW_P2M_PERIPH:
            addr_inc = DMA_CONTROL_DI;
            break;
        case HAL_DMA_FLOW_P2P_DMA:
        case HAL_DMA_FLOW_P2P_DSTPERIPH:
        case HAL_DMA_FLOW_P2P_SRCPERIPH:
            addr_inc = 0;
        default:
            return HAL_DMA_ERR;
    }

#ifndef CHIP_BEST1000
    if (cfg->type & HAL_DMA_FLOW_FLAG_SI) {
        addr_inc |= DMA_CONTROL_SI;
    }
    if (cfg->type & HAL_DMA_FLOW_FLAG_DI) {
        addr_inc |= DMA_CONTROL_DI;
    }
#endif

    *ctrl = DMA_CONTROL_TRANSFERSIZE(cfg->src_tsize) |
            DMA_CONTROL_SBSIZE(cfg->src_bsize) |
            DMA_CONTROL_DBSIZE(cfg->dst_bsize) |
            DMA_CONTROL_SWIDTH(cfg->src_width) |
            DMA_CONTROL_DWIDTH(cfg->dst_width) |
            (tc_irq ? DMA_CONTROL_TC_IRQ : 0) |
            addr_inc;

    return HAL_DMA_OK;
}

/*****************************************************************************
 * Generic Public functions
 ****************************************************************************/

enum HAL_DMA_RET_T hal_dma_init_desc(struct HAL_DMA_DESC_T *desc,
                                     const struct HAL_DMA_CH_CFG_T *cfg,
                                     const struct HAL_DMA_DESC_T *next,
                                     int tc_irq)
{
    uint32_t ctrl;
    enum HAL_DMA_RET_T ret;
    enum HAL_DMA_FLOW_CONTROL_T type;

    ret = hal_dma_init_control(&ctrl, cfg, tc_irq);
    if (ret != HAL_DMA_OK) {
        return ret;
    }

#ifdef CHIP_BEST1000
    type = cfg->type;
#else
    type = cfg->type & ~HAL_DMA_FLOW_FLAG_MASK;
#endif

    if (type == HAL_DMA_FLOW_M2M_DMA || type == HAL_DMA_FLOW_M2P_DMA ||
            type == HAL_DMA_FLOW_M2P_PERIPH) {
        desc->src = cfg->src;
    } else {
        desc->src = hal_dma_get_periph_addr(cfg->src_periph);
    }
    if (type == HAL_DMA_FLOW_M2M_DMA || type == HAL_DMA_FLOW_P2M_DMA ||
            type == HAL_DMA_FLOW_P2M_PERIPH) {
        desc->dst = cfg->dst;
    } else {
        desc->dst = hal_dma_get_periph_addr(cfg->dst_periph);
    }
    desc->lli = (uint32_t)next;
    desc->ctrl = ctrl;

    return HAL_DMA_OK;
}

enum HAL_DMA_RET_T hal_dma_sg_2d_start(const struct HAL_DMA_DESC_T *desc,
                                       const struct HAL_DMA_CH_CFG_T *cfg,
                                       const struct HAL_DMA_2D_CFG_T *src_2d,
                                       const struct HAL_DMA_2D_CFG_T *dst_2d)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;
    enum HAL_DMA_FLOW_CONTROL_T type;
    uint8_t src_periph, dst_periph;
    enum HAL_DMA_INST_T periph_inst;
    int ret;
    uint32_t irq_mask, try_burst;
    uint32_t lock;

    inst = get_inst_from_chan(cfg->ch);
    hwch = get_hwch_from_chan(cfg->ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

#ifdef CHIP_BEST1000
    type = cfg->type;
#else
    type = cfg->type & ~HAL_DMA_FLOW_FLAG_MASK;
#endif

    if (type == HAL_DMA_FLOW_M2M_DMA || type == HAL_DMA_FLOW_M2P_DMA ||
            type == HAL_DMA_FLOW_M2P_PERIPH) {
        src_periph = 0;
    } else {
        ret = get_index_from_periph(cfg->src_periph, &periph_inst, &src_periph);
        ASSERT(ret == 0, "Failed to get src periph: %d", cfg->src_periph);
#ifndef NO_DMA_REMAP
        if (periph_remap_bitmap[periph_inst] & (1 << src_periph)) {
            periph_inst ^= 1;
            ret = get_remap_index_from_periph(periph_inst, cfg->src_periph, &src_periph);
            ASSERT(ret == 0, "Failed to get remap src periph: %d", cfg->src_periph);
        }
#endif
        ASSERT(inst == periph_inst, "Mismatch inst in chan=0x%02x and src periph %d", cfg->ch, cfg->src_periph);
    }
    if (type == HAL_DMA_FLOW_M2M_DMA || type == HAL_DMA_FLOW_P2M_DMA ||
            type == HAL_DMA_FLOW_P2M_PERIPH) {
        dst_periph = 0;
    } else {
        ret = get_index_from_periph(cfg->dst_periph, &periph_inst, &dst_periph);
        ASSERT(ret == 0, "Failed to get dst periph: %d", cfg->dst_periph);
#ifndef NO_DMA_REMAP
        if (periph_remap_bitmap[periph_inst] & (1 << dst_periph)) {
            periph_inst ^= 1;
            ret = get_remap_index_from_periph(periph_inst, cfg->dst_periph, &dst_periph);
            ASSERT(ret == 0, "Failed to get remap dst periph: %d", cfg->dst_periph);
        }
#endif
        ASSERT(inst == periph_inst, "Mismatch inst in chan=0x%02x and dst periph %d", cfg->ch, cfg->dst_periph);
    }

    if (!chan_enabled[inst][hwch]) {
        // Not acquired
        return HAL_DMA_ERR;
    }
    if (hal_dma_chan_busy_inst(inst, hwch)) {
        // Busy
        return HAL_DMA_ERR;
    }

    if (cfg->handler == NULL) {
        irq_mask = 0;
    } else {
        irq_mask = DMA_CONFIG_ERR_IRQMASK | DMA_CONFIG_TC_IRQMASK;
        handler[inst][hwch] = cfg->handler;
    }

    try_burst = cfg->try_burst ? DMA_CONFIG_TRY_BURST : 0;

    /* Reset the Interrupt status */
    dma[inst]->INTTCCLR = DMA_STAT_CHAN(hwch);
    dma[inst]->INTERRCLR = DMA_STAT_CHAN(hwch);

    dma[inst]->CH[hwch].SRCADDR = desc->src;
    dma[inst]->CH[hwch].DSTADDR = desc->dst;
    dma[inst]->CH[hwch].LLI = desc->lli;
    dma[inst]->CH[hwch].CONTROL = desc->ctrl;
    dma[inst]->CH[hwch].CONFIG = DMA_CONFIG_SRCPERIPH(src_periph) |
                                 DMA_CONFIG_DSTPERIPH(dst_periph) |
                                 DMA_CONFIG_TRANSFERTYPE(type) |
                                 irq_mask |
                                 try_burst;
    dma[inst]->DMACONFIG = (dma[inst]->DMACONFIG & ~(DMA_DMACONFIG_AHB1_BIGENDIAN |
        DMA_DMACONFIG_AHB2_BIGENDIAN)) | DMA_DMACONFIG_TC_IRQ_EN_MASK | DMA_DMACONFIG_EN;

#ifndef CHIP_BEST1000
    if (src_2d) {
        dma[inst]->_2D[hwch].SRCX = DMA_2D_MODIFY(src_2d->xmodify) | DMA_2D_COUNT(src_2d->xcount);
        dma[inst]->_2D[hwch].SRCY = DMA_2D_MODIFY(src_2d->ymodify) | DMA_2D_COUNT(src_2d->ycount);
        dma[inst]->_2D[hwch].CTRL |= DMA_2D_CTRL_SRC_EN;
    } else {
        dma[inst]->_2D[hwch].CTRL &= ~DMA_2D_CTRL_SRC_EN;
    }
    if (dst_2d) {
        dma[inst]->_2D[hwch].DSTX = DMA_2D_MODIFY(dst_2d->xmodify) | DMA_2D_COUNT(dst_2d->xcount);
        dma[inst]->_2D[hwch].DSTY = DMA_2D_MODIFY(dst_2d->ymodify) | DMA_2D_COUNT(dst_2d->ycount);
        dma[inst]->_2D[hwch].CTRL |= DMA_2D_CTRL_DST_EN;
    } else {
        dma[inst]->_2D[hwch].CTRL &= ~DMA_2D_CTRL_DST_EN;
    }
#endif

    lock = int_lock();
    if (cfg->start_cb) {
        cfg->start_cb(cfg->ch);
    }
    dma[inst]->CH[hwch].CONFIG |= DMA_CONFIG_EN;
    int_unlock(lock);

    return HAL_DMA_OK;
}

enum HAL_DMA_RET_T hal_dma_sg_start(const struct HAL_DMA_DESC_T *desc,
                                    const struct HAL_DMA_CH_CFG_T *cfg)
{
    return hal_dma_sg_2d_start(desc, cfg, NULL, NULL);
}

enum HAL_DMA_RET_T hal_dma_start(const struct HAL_DMA_CH_CFG_T *cfg)
{
    struct HAL_DMA_DESC_T desc;
    enum HAL_DMA_RET_T ret;

    ret = hal_dma_init_desc(&desc, cfg, NULL, 1);
    if (ret != HAL_DMA_OK) {
        return ret;
    }

    ret = hal_dma_sg_start(&desc, cfg);
    if (ret != HAL_DMA_OK) {
        return ret;
    }

    return HAL_DMA_OK;
}

uint32_t hal_dma_cancel(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;
    uint32_t remains;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    dma[inst]->CH[hwch].CONFIG &= ~DMA_CONFIG_EN;
    dma[inst]->INTTCCLR = DMA_STAT_CHAN(hwch);
    dma[inst]->INTERRCLR = DMA_STAT_CHAN(hwch);

    remains = GET_BITFIELD(dma[inst]->CH[hwch].CONTROL, DMA_CONTROL_TRANSFERSIZE);

    return remains;
}

uint32_t hal_dma_stop(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;
    uint8_t retry = 10;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    dma[inst]->CH[hwch].CONFIG |= DMA_CONFIG_HALT;
#if 1
    while ((dma[inst]->CH[hwch].CONFIG & DMA_CONFIG_ACTIVE) && ((--retry) > 0)) {
        hal_dma_delay(1);
    }
#else
    while (dma[inst]->CH[hwch].CONFIG & DMA_CONFIG_ACTIVE);
#endif
    return hal_dma_cancel(ch);
}

uint8_t hal_dma_get_chan(enum HAL_DMA_PERIPH_T periph, enum HAL_DMA_GET_CHAN_T policy)
{
    enum HAL_DMA_INST_T inst;
    int ret;
    uint8_t i, hwch;
    uint8_t got = HAL_DMA_CHAN_NONE;
    uint32_t lock;

    ASSERT(policy == HAL_DMA_HIGH_PRIO || policy == HAL_DMA_LOW_PRIO || policy == HAL_DMA_LOW_PRIO_ONLY,
        "Invalid DMA policy: %d", policy);

    if (periph == HAL_GPDMA_MEM) {
        inst = HAL_DMA_INST_GPDMA;
    } else if (periph == HAL_AUDMA_MEM) {
        inst = HAL_DMA_INST_AUDMA;
    } else {
        ret = get_index_from_periph(periph, &inst, &i);
        ASSERT(ret == 0, "Invalid DMA periph: %d", periph);
#ifndef NO_DMA_REMAP
        if (periph_remap_bitmap[inst] & (1 << i)) {
            inst ^= 1;
        }
#endif
    }

    lock = int_lock();
    for (i = 0; i < DMA_NUMBER_CHANNELS; i++) {
        if (policy == HAL_DMA_HIGH_PRIO) {
            hwch = i;
        } else if (policy == HAL_DMA_LOW_PRIO) {
            hwch = DMA_NUMBER_CHANNELS - 1 - i;
        } else {
            hwch = DMA_NUMBER_CHANNELS - 1 - i;
            if (hwch < 6) {
                break;
            }
        }

        if (!chan_enabled[inst][hwch] && !hal_dma_chan_busy_inst(inst, hwch)) {
            chan_enabled[inst][hwch] = true;
            got = generate_chan(inst, hwch);
            break;
        }
    }
    int_unlock(lock);

    return got;
}

void hal_dma_free_chan(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;
    uint32_t lock;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    hal_dma_cancel(ch);

    lock = int_lock();
    chan_enabled[inst][hwch] = false;
    for (hwch = 0; hwch < DMA_NUMBER_CHANNELS; hwch++) {
        if (chan_enabled[inst][hwch]) {
            break;
        }
    }
    if (hwch == DMA_NUMBER_CHANNELS) {
        dma[inst]->DMACONFIG = 0;
    }
    int_unlock(lock);
}

uint32_t hal_dma_get_cur_src_addr(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    return dma[inst]->CH[hwch].SRCADDR;
}

uint32_t hal_dma_get_cur_dst_addr(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    return dma[inst]->CH[hwch].DSTADDR;
}

void hal_dma_irq_run_chan(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;
    uint32_t remains;
    struct HAL_DMA_DESC_T *lli;
    bool tcint, errint;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    if ((dma[inst]->INTSTAT & DMA_STAT_CHAN(hwch)) == 0) {
        return;
    }

    /* Check counter terminal status */
    tcint = !!(dma[inst]->INTTCSTAT & DMA_STAT_CHAN(hwch));
    /* Check error terminal status */
    errint = !!(dma[inst]->INTERRSTAT & DMA_STAT_CHAN(hwch));

    if (tcint || errint) {
        if (tcint) {
            /* Clear terminate counter Interrupt pending */
            dma[inst]->INTTCCLR = DMA_STAT_CHAN(hwch);
        }
        if (errint) {
            /* Clear error counter Interrupt pending */
            dma[inst]->INTERRCLR = DMA_STAT_CHAN(hwch);
        }

        if (handler[inst][hwch]) {
            remains = GET_BITFIELD(dma[inst]->CH[hwch].CONTROL, DMA_CONTROL_TRANSFERSIZE);
            lli = (struct HAL_DMA_DESC_T *)dma[inst]->CH[hwch].LLI;
            handler[inst][hwch](ch, remains, errint, lli);
        }
    }
}

bool hal_dma_chan_busy(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    return hal_dma_chan_busy_inst(inst, hwch);
}

bool hal_dma_busy(void)
{
    enum HAL_DMA_INST_T inst;
    int hwch;

    for (inst = HAL_DMA_INST_AUDMA; inst < HAL_DMA_INST_QTY; inst++) {
        for (hwch = chan_start[inst]; hwch < chan_num[inst]; hwch++) {
            if (hal_dma_chan_busy_inst(inst, hwch)) {
                return true;
            }
        }
    }

    return false;
}

uint32_t hal_dma_get_sg_remain_size(uint8_t ch)
{
    enum HAL_DMA_INST_T inst;
    uint8_t hwch;
    uint32_t remains;
    const struct HAL_DMA_DESC_T *desc, *first;

    inst = get_inst_from_chan(ch);
    hwch = get_hwch_from_chan(ch);

    ASSERT(inst < HAL_DMA_INST_QTY, err_invalid_inst, inst);
    ASSERT(hwch < DMA_NUMBER_CHANNELS, err_invalid_chan[inst], hwch);

    remains = GET_BITFIELD(dma[inst]->CH[hwch].CONTROL, DMA_CONTROL_TRANSFERSIZE);
    first = (const struct HAL_DMA_DESC_T *)dma[inst]->CH[hwch].LLI;
    desc = first;
    while (desc) {
        remains += GET_BITFIELD(desc->ctrl, DMA_CONTROL_TRANSFERSIZE);
        desc = (const struct HAL_DMA_DESC_T *)desc->lli;
        if (desc == first) {
            break;
        }
    }

    return remains;
}

#ifndef NO_DMA_REMAP
void hal_dma_remap_periph(enum HAL_DMA_PERIPH_T periph, int enable)
{
    enum HAL_DMA_INST_T inst1, inst2;
    uint8_t index1, index2;
    int ret;
    uint32_t lock;

    // Tag the periph
    ret = get_index_from_periph(periph, &inst1, &index1);
    ASSERT(ret == 0, "Invalid DMA periph for remap1: %d", periph);

    // Tag the peer periph
    inst2 = inst1 ^ 1;
    ret = get_remap_index_from_periph(inst2, periph, &index2);
    ASSERT(ret == 0, "Invalid DMA periph for remap2: %d", periph);

    lock = int_lock();
    if (enable) {
        periph_remap_bitmap[inst1] |= (1 << index1);
        periph_remap_bitmap[inst2] |= (1 << index2);
#ifndef CHIP_BEST1000
        // For best2000, index1 == index2
        hal_cmu_dma_swap_enable(index1);
#endif
    } else {
        periph_remap_bitmap[inst1] &= ~(1 << index1);
        periph_remap_bitmap[inst2] &= ~(1 << index2);
#ifndef CHIP_BEST1000
        hal_cmu_dma_swap_disable(index1);
#endif
    }
    int_unlock(lock);
}
#endif

void hal_dma_open(void)
{
    enum HAL_DMA_INST_T inst;

    if (dma_opened) {
        return;
    }

    for (inst = 0; inst < HAL_DMA_INST_QTY; inst++) {
        hal_dma_open_inst(inst);
    }

#ifndef NO_DMA_REMAP
#ifdef CHIP_BEST1000
    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4) {
        hal_dma_remap_periph(HAL_AUDMA_I2S_RX, 1);
        hal_dma_remap_periph(HAL_AUDMA_I2S_TX, 1);
        hal_dma_remap_periph(HAL_AUDMA_SPDIF0_RX, 1);
        hal_dma_remap_periph(HAL_AUDMA_SPDIF0_TX, 1);
    }
#endif
#endif

    dma_opened = true;
}

void hal_dma_close(void)
{
    enum HAL_DMA_INST_T inst;

    if (!dma_opened) {
        return;
    }

    for (inst = 0; inst < HAL_DMA_INST_QTY; inst++) {
        hal_dma_close_inst(inst);
    }

    dma_opened = false;
}

HAL_DMA_DELAY_FUNC hal_dma_set_delay_func(HAL_DMA_DELAY_FUNC new_func)
{
    HAL_DMA_DELAY_FUNC old_func = dma_delay;
    dma_delay = new_func;
    return old_func;
}

