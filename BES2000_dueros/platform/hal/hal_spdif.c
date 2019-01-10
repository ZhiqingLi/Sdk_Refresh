#ifdef CHIP_HAS_SPDIF

#include "plat_types.h"
#include "plat_addr_map.h"
#include "reg_spdifip.h"
#include "hal_spdifip.h"
#include "hal_spdif.h"
#include "hal_iomux.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "analog.h"

//#define SPDIF_CLOCK_SOURCE 240000000
//#define SPDIF_CLOCK_SOURCE 22579200
//#define SPDIF_CLOCK_SOURCE 48000000
#define SPDIF_CLOCK_SOURCE 3072000

//#define SPDIF_CLOCK_SOURCE 76800000
//#define SPDIF_CLOCK_SOURCE 84672000

#define HAL_SPDIF_TX_FIFO_TRIGGER_LEVEL     (SPDIFIP_FIFO_DEPTH/2)
#define HAL_SPDIF_RX_FIFO_TRIGGER_LEVEL     (SPDIFIP_FIFO_DEPTH/2)

#define HAL_SPDIF_YES 1
#define HAL_SPDIF_NO 0

#ifdef CHIP_BEST2000
#define SPDIF_CMU_DIV                       CODEC_CMU_DIV
#else
#define SPDIF_CMU_DIV                       CODEC_PLL_DIV
#endif

static const char * const invalid_id = "Invalid SPDIF ID: %d\n";
//static const char * const invalid_ch = "Invalid SPDIF CH: %d\n";

//---------------------------------------------------------------
/*
#define I2S_BASE  0x40020000
#define I2S_DMA_BASE 0x400201c0

#define I2STXSRCBASEADDR  0x20000040
#define I2SRXDESTBASEADDR 0x200000C0
#define I2S_TXSIZE 8

typedef struct i2sregisters {
	volatile uint32_t IER;
	volatile uint32_t IRER;
	volatile uint32_t ITER;
	volatile uint32_t CER;
	volatile uint32_t CCR;      // 0x10
	volatile uint32_t RXFFR;
	volatile uint32_t TXFFR;
    volatile uint32_t rev0;
	volatile uint32_t LRTBR0;   // 0x20
	volatile uint32_t RRTBR0;
    volatile uint32_t RER0;
	volatile uint32_t TER0;
	volatile uint32_t RCR0;     // 0x30
	volatile uint32_t TCR0;
	volatile uint32_t ISR0;
	volatile uint32_t IMR0;
	volatile uint32_t ROR0;     // 0x40
	volatile uint32_t TOR0;
    volatile uint32_t RFCR0;
	volatile uint32_t TFCR0;
	volatile uint32_t RFF0;     // 0x50
	volatile uint32_t TFF0;
    volatile uint32_t DMACTRL0;
    volatile uint32_t rev2;
    volatile uint32_t SPDIFRXCFG;  // 0x60
    volatile uint32_t SPDIFRXSTAT;
    volatile uint32_t SPDIFRXUSRCHSTCFG;
    volatile uint32_t SPDIFRXUSRCHSTDATA;
    volatile uint32_t rev3[2];
    volatile uint32_t SPDIFTXCFG;
    volatile uint32_t rev4;
    volatile uint32_t SPDIFUSRCHSTDATA[24];
    volatile uint32_t SPDIFINTMASK;  // 0xe0
    volatile uint32_t SPDIFINTSTAT;
} i2sreg;

typedef struct i2sdmaregisters {
  volatile uint32_t RXDMA;
  volatile uint32_t RRXDMA;
  volatile uint32_t TXDMA;
  volatile uint32_t RTXDMA;
} i2sdmareg;

  uint32_t    *i2stxsrcaddr   = (uint32_t *)    I2STXSRCBASEADDR;
  uint32_t    *i2srxdestaddr  = (uint32_t *)    I2SRXDESTBASEADDR;
  i2sreg    *i2sregs        = (i2sreg *)    I2S_BASE;
  i2sdmareg *i2sdmaregs     = (i2sdmareg *) I2S_DMA_BASE;

*/
//---------------------------------------------------------------

struct SPDIF_SAMPLE_RATE_T {
    enum AUD_SAMPRATE_T sample_rate;
    uint32_t codec_freq;
    uint8_t codec_div;
    uint8_t pcm_div;
    uint8_t tx_ratio;
};

// SAMPLE_RATE * 128 = PLL_nominal / PCM_DIV / TX_RATIO
static const struct SPDIF_SAMPLE_RATE_T spdif_sample_rate[]={
#if defined(CHIP_BEST1000) && defined(AUD_PLL_DOUBLE)
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES*2, CODEC_PLL_DIV, SPDIF_CMU_DIV,  4},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES*2, CODEC_PLL_DIV, SPDIF_CMU_DIV,  2},
    {AUD_SAMPRATE_384000, CODEC_FREQ_48K_SERIES*2, CODEC_PLL_DIV, SPDIF_CMU_DIV,  1},
#else
    {AUD_SAMPRATE_8000,   CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, SPDIF_CMU_DIV, 24},
    {AUD_SAMPRATE_16000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, SPDIF_CMU_DIV, 12},
    {AUD_SAMPRATE_22050,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, SPDIF_CMU_DIV,  8},
    {AUD_SAMPRATE_24000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, SPDIF_CMU_DIV,  8},
    {AUD_SAMPRATE_44100,  CODEC_FREQ_44_1K_SERIES, CODEC_PLL_DIV, SPDIF_CMU_DIV,  4},
    {AUD_SAMPRATE_48000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, SPDIF_CMU_DIV,  4},
    {AUD_SAMPRATE_96000,  CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, SPDIF_CMU_DIV,  2},
    {AUD_SAMPRATE_192000, CODEC_FREQ_48K_SERIES,   CODEC_PLL_DIV, SPDIF_CMU_DIV,  1},
#endif
};

#if (CHIP_HAS_SPDIF > 1)
static uint8_t spdif_map;
STATIC_ASSERT(HAL_SPDIF_ID_QTY <= sizeof(spdif_map) * 8, "Too many SPDIF IDs");
#endif

static inline unsigned char reverse(unsigned char in)
{
    uint8_t out = 0;
    uint32_t i = 0;
    for (i = 0; i < 8; ++i) {
        if((1<<i & in))
            out = out | (1<<(7-i));
        else
            out = out & (~(1<<(7-i)));
    }

    return out;
}

static inline uint32_t _spdif_get_reg_base(enum HAL_SPDIF_ID_T id)
{
    ASSERT(id < HAL_SPDIF_ID_QTY, invalid_id, id);

    switch(id) {
        case HAL_SPDIF_ID_0:
        default:
            return SPDIF0_BASE;
            break;
#if (CHIP_HAS_SPDIF > 1)
        case HAL_SPDIF_ID_1:
            return SPDIF1_BASE;
            break;
#endif
    }
	return 0;
}

int hal_spdif_open(enum HAL_SPDIF_ID_T id)
{
    uint32_t reg_base;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

#ifndef SIMU
    bool cfg_pll = true;
#if (CHIP_HAS_SPDIF > 1)
    if (spdif_map) {
        cfg_pll = false;
    }
    spdif_map |= (1 << id);
#endif
    if (cfg_pll) {
        analog_aud_pll_open(ANA_AUD_PLL_USER_SPDIF);
    }
#endif

#if (CHIP_HAS_SPDIF > 1)
    if (id == HAL_SPDIF_ID_1) {
        hal_iomux_set_spdif1();
    } else
#endif
    {
        hal_iomux_set_spdif0();
    }
    hal_cmu_spdif_clock_enable(id);
    hal_cmu_clock_enable(HAL_CMU_MOD_O_SPDIF0);
    hal_cmu_clock_enable(HAL_CMU_MOD_P_SPDIF0);
	hal_cmu_reset_clear(HAL_CMU_MOD_O_SPDIF0);
	hal_cmu_reset_clear(HAL_CMU_MOD_P_SPDIF0);

    reg_base = _spdif_get_reg_base(id);

//    i2sdmaregs->RRXDMA = 0x1;   // reset dma rx register.
//    i2sdmaregs->RTXDMA = 0x1;   // reset dma tx register.

    spdifip_w_enable_spdifip(reg_base, HAL_SPDIF_YES);

//    i2sregs->SPDIFRXUSRCHSTCFG = 0x20;

    return 0;
}

int hal_spdif_close(enum HAL_SPDIF_ID_T id)
{
    uint32_t reg_base;

    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    reg_base = _spdif_get_reg_base(id);

    spdifip_w_enable_spdifip(reg_base, HAL_SPDIF_NO);

    hal_cmu_spdif_set_div(id, 0x1FFF + 2);
    hal_cmu_reset_set(HAL_CMU_MOD_P_SPDIF0);
    hal_cmu_reset_set(HAL_CMU_MOD_O_SPDIF0);
    hal_cmu_clock_disable(HAL_CMU_MOD_P_SPDIF0);
    hal_cmu_clock_disable(HAL_CMU_MOD_O_SPDIF0);
    hal_cmu_spdif_clock_disable(id);

#ifndef SIMU
    bool cfg_pll = true;
#if (CHIP_HAS_SPDIF > 1)
    spdif_map &= ~(1 << id);
    if (spdif_map) {
        cfg_pll = false;
    }
#endif
    if (cfg_pll) {
        analog_aud_pll_close(ANA_AUD_PLL_USER_SPDIF);
    }
#endif

    return 0;
}

int hal_spdif_start_stream(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t reg_base;

    reg_base = _spdif_get_reg_base(id);

    if (stream == AUD_STREAM_PLAYBACK) {
        spdifip_w_enable_tx_channel0(reg_base, HAL_SPDIF_YES);
        spdifip_w_enable_tx(reg_base, HAL_SPDIF_YES);
        spdifip_w_tx_valid(reg_base, HAL_SPDIF_YES);        //send data is valid

    }
    else {
        spdifip_w_enable_rx_channel0(reg_base, HAL_SPDIF_YES);
        spdifip_w_enable_rx(reg_base, HAL_SPDIF_YES);
        spdifip_w_sample_en(reg_base, HAL_SPDIF_YES);
    }

    return 0;
}

int hal_spdif_stop_stream(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream)
{
    uint32_t reg_base;

    reg_base = _spdif_get_reg_base(id);

    if (stream == AUD_STREAM_PLAYBACK) {
        spdifip_w_enable_tx(reg_base, HAL_SPDIF_NO);
        spdifip_w_enable_tx_channel0(reg_base, HAL_SPDIF_NO);
    }
    else {
        spdifip_w_enable_rx(reg_base, HAL_SPDIF_NO);
        spdifip_w_enable_rx_channel0(reg_base, HAL_SPDIF_NO);
    }

    return 0;
}

int hal_spdif_setup_stream(enum HAL_SPDIF_ID_T id, enum AUD_STREAM_T stream, struct HAL_SPDIF_CONFIG_T *cfg)
{
    uint8_t i;
    uint32_t reg_base;

    reg_base = _spdif_get_reg_base(id);

//        i2sregs->SPDIFTXCFG |= 0x0000a3;

    for (i = 0; i < ARRAY_SIZE(spdif_sample_rate); i++)
    {
        if (spdif_sample_rate[i].sample_rate == cfg->sample_rate)
        {
            break;
        }
    }
    ASSERT(i < ARRAY_SIZE(spdif_sample_rate), "%s: Invalid spdif sample rate: %d", __func__, cfg->sample_rate);

    TRACE("[%s] stream=%d sample_rate=%d", __func__, stream, cfg->sample_rate);

#ifdef FPGA
    hal_cmu_spdif_set_div(id, 2);
#else
#ifndef SIMU
    analog_aud_freq_pll_config(spdif_sample_rate[i].codec_freq, spdif_sample_rate[i].codec_div);
#ifdef CHIP_BEST2000
    analog_aud_pll_set_dig_div(spdif_sample_rate[i].codec_div / spdif_sample_rate[i].pcm_div);
#endif
#endif
    hal_cmu_spdif_set_div(id, spdif_sample_rate[i].pcm_div);
#endif

    spdifip_w_tx_ratio(reg_base, spdif_sample_rate[i].tx_ratio - 1);

    hal_cmu_reset_pulse(HAL_CMU_MOD_O_SPDIF0);

    switch (cfg->bits) {
        case AUD_BITS_16:
            if (stream == AUD_STREAM_PLAYBACK)
                spdifip_w_tx_format_cfg_reg(reg_base, 0);
            else
                spdifip_w_rx_format_cfg_reg(reg_base, 0);
            break;
        // here, 32-bit is treated as 24-bit
        case AUD_BITS_32:
        case AUD_BITS_24:
            if (stream == AUD_STREAM_PLAYBACK)
                spdifip_w_tx_format_cfg_reg(reg_base, 8);
            else
                spdifip_w_rx_format_cfg_reg(reg_base, 8);
            break;

        default:
            ASSERT(0, "%s: invalid bits[%d]", __func__, cfg->bits);
    }

    if (cfg->use_dma)
    {
        if (stream == AUD_STREAM_PLAYBACK)
        {
            spdifip_w_tx_fifo_threshold(reg_base, HAL_SPDIF_TX_FIFO_TRIGGER_LEVEL);
            spdifip_w_enable_tx_dma(reg_base, HAL_SPDIF_YES);
        }
        else
        {
            spdifip_w_rx_fifo_threshold(reg_base, HAL_SPDIF_RX_FIFO_TRIGGER_LEVEL);
            spdifip_w_enable_rx_dma(reg_base, HAL_SPDIF_YES);
        }
    }

    return 0;
}

int hal_spdif_send(enum HAL_SPDIF_ID_T id, uint8_t *value, uint32_t value_len)
{
    uint32_t i = 0;
    uint32_t reg_base;

    reg_base = _spdif_get_reg_base(id);

    for (i = 0; i < value_len; i += 4) {
        while (!(spdifip_r_int_status(reg_base) & SPDIFIP_INT_STATUS_TX_FIFO_EMPTY_MASK));

        spdifip_w_tx_left_fifo(reg_base, value[i+1]<<8 | value[i]);
        spdifip_w_tx_right_fifo(reg_base, value[i+3]<<8 | value[i+2]);
    }

    return 0;
}

uint8_t hal_spdif_recv(enum HAL_SPDIF_ID_T id, uint8_t *value, uint32_t value_len)
{
    //uint32_t reg_base;

    //reg_base = _spdif_get_reg_base(id);
    return 0;
}

int hal_spdif_clock_out_enable(enum HAL_SPDIF_ID_T id, uint32_t div)
{
    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    hal_cmu_spdif_clock_enable(id);
    hal_cmu_spdif_set_div(id, div);

    return 0;
}

int hal_spdif_clock_out_disable(enum HAL_SPDIF_ID_T id)
{
    if (id >= HAL_SPDIF_ID_QTY) {
        return 1;
    }

    hal_cmu_spdif_set_div(id, 0x1FFF + 2);
    hal_cmu_spdif_clock_disable(id);

    return 0;
}

#endif // CHIP_HAS_SPDIF
