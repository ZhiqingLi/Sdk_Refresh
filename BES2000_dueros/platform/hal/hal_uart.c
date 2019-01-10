#ifdef CHIP_HAS_UART

#include "plat_addr_map.h"
#include "hal_uart.h"
#include "reg_uart.h"
#include "cmsis_nvic.h"
#include "hal_bootmode.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_iomux.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "string.h"

#define UART_FLUSH_DELAY_DEFAULT        2

static struct UART_T * const uart[HAL_UART_ID_QTY] = {
    (struct UART_T *)UART0_BASE,
#if (CHIP_HAS_UART > 1)
    (struct UART_T *)UART1_BASE,
#endif
#if (CHIP_HAS_UART > 2)
    (struct UART_T *)UART2_BASE,
#endif
#ifdef BT_UART
    (struct UART_T *)BT_UART_BASE,
#endif
};

static const IRQn_Type uart_irq_num[HAL_UART_ID_QTY] = {
    UART0_IRQn,
#if (CHIP_HAS_UART > 1)
    UART1_IRQn,
#endif
#ifdef BT_UART
    INVALID_IRQn,
#endif
};

struct HAL_UART_MOD_NAME_T {
    enum HAL_CMU_MOD_ID_T mod;
    enum HAL_CMU_MOD_ID_T apb;
};

static const struct HAL_UART_MOD_NAME_T uart_mod[HAL_UART_ID_QTY] = {
    {
        .mod = HAL_CMU_MOD_O_UART0,
        .apb = HAL_CMU_MOD_P_UART0,
    },
#if (CHIP_HAS_UART > 1)
    {
        .mod = HAL_CMU_MOD_O_UART1,
        .apb = HAL_CMU_MOD_P_UART1,
    },
#endif
#if (CHIP_HAS_UART > 2)
    {
        .mod = HAL_CMU_MOD_QTY, //HAL_CMU_MOD_O_UART2,
        .apb = HAL_CMU_MOD_QTY, //HAL_CMU_MOD_P_UART2,
    },
#endif
#ifdef BT_UART
    {
        .mod = HAL_CMU_MOD_QTY,
        .apb = HAL_CMU_MOD_QTY,
    },
#endif
};

static bool init_done = false;

static HAL_UART_IRQ_HANDLER_T irq_handler[HAL_UART_ID_QTY] = { NULL };

static HAL_UART_IRQ_RXDMA_HANDLER_T rxdma_handler[HAL_UART_ID_QTY] = { NULL };
static HAL_UART_IRQ_TXDMA_HANDLER_T txdma_handler[HAL_UART_ID_QTY] = { NULL };
static HAL_UART_IRQ_BREAK_HANDLER_T break_handler[HAL_UART_ID_QTY] = { NULL };

static uint8_t recv_dma_chan[HAL_UART_ID_QTY];
static uint8_t send_dma_chan[HAL_UART_ID_QTY];

static uint32_t recv_dma_size[HAL_UART_ID_QTY];
static uint32_t send_dma_size[HAL_UART_ID_QTY];

static union HAL_UART_IRQ_T recv_mask[HAL_UART_ID_QTY];

static const char * const err_invalid_id = "Invalid UART ID: %d\n";

static const struct HAL_UART_CFG_T default_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_RTSCTS,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_4,
    .baud = 921600,
    .dma_rx = false,
    .dma_tx = false,
    .dma_rx_stop_on_err = false,
};

static void hal_uart_irq_handler(void);

int hal_uart_open(enum HAL_UART_ID_T id, const struct HAL_UART_CFG_T *cfg)
{
    uint32_t cr, lcr, ibrd, fbrd, dmacr;
    uint32_t div;
    int i;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);

    if (!init_done) {
        init_done = true;
        for (i = HAL_UART_ID_0; i < HAL_UART_ID_QTY; i++) {
            recv_dma_chan[i] = HAL_DMA_CHAN_NONE;
            send_dma_chan[i] = HAL_DMA_CHAN_NONE;
        }
    }

    if (hal_uart_opened(id)) {
        hal_uart_close(id);
    }

    hal_cmu_clock_enable(uart_mod[id].mod);
    hal_cmu_clock_enable(uart_mod[id].apb);
    hal_cmu_reset_clear(uart_mod[id].mod);
    hal_cmu_reset_clear(uart_mod[id].apb);

    cr = lcr = 0;

    if (cfg == NULL) {
        cfg = &default_cfg;
    }

    switch (cfg->parity) {
        case HAL_UART_PARITY_NONE:
            break;
        case HAL_UART_PARITY_ODD:
            lcr |= UARTLCR_H_PEN;
            break;
        case HAL_UART_PARITY_EVEN:
            lcr |= UARTLCR_H_PEN
                |  UARTLCR_H_EPS;
            break;
        case HAL_UART_PARITY_FORCE1:
            lcr |= UARTLCR_H_PEN
                |  UARTLCR_H_SPS;
            break;
        case HAL_UART_PARITY_FORCE0:
            lcr |= UARTLCR_H_PEN
                |  UARTLCR_H_EPS
                |  UARTLCR_H_SPS;
            break;
        default:
            ASSERT(false, "Invalid parity param: %d\n", cfg->parity);
            break;
    }

    if (cfg->stop == HAL_UART_STOP_BITS_2) {
        lcr |= UARTLCR_H_STP2;
    } else if (cfg->stop != HAL_UART_STOP_BITS_1) {
        ASSERT(false, "Invalid stop bits param: %d\n", cfg->stop);
    }

    switch (cfg->data) {
        case HAL_UART_DATA_BITS_5:
            lcr |= UARTLCR_H_WLEN_5;
            break;
        case HAL_UART_DATA_BITS_6:
            lcr |= UARTLCR_H_WLEN_5;
            break;
        case HAL_UART_DATA_BITS_7:
            lcr |= UARTLCR_H_WLEN_7;
            break;
        case HAL_UART_DATA_BITS_8:
            lcr |= UARTLCR_H_WLEN_8;
            break;
        default:
            ASSERT(false, "Invalid data bits param: %d\n", cfg->data);
            break;
    }

    switch (cfg->flow) {
        case HAL_UART_FLOW_CONTROL_NONE:
            break;
        case HAL_UART_FLOW_CONTROL_RTS:
            cr |= UARTCR_RTSEN;
            break;
        case HAL_UART_FLOW_CONTROL_CTS:
            cr |= UARTCR_CTSEN;
            break;
        case HAL_UART_FLOW_CONTROL_RTSCTS:
            cr |= UARTCR_RTSEN
               |  UARTCR_CTSEN;
            break;
        default:
            ASSERT(false, "Invalid flow control param: %d\n", cfg->flow);
            break;
    }

    lcr |= UARTLCR_H_FEN | UARTLCR_H_DMA_RT_CNT(9);
    cr |= UARTCR_UARTEN | UARTCR_TXE | UARTCR_RXE;

//#ifdef AUDIO_DEBUG
#if 1
    div = 2 * hal_cmu_get_crystal_freq() / 16;
#else
    div = hal_cmu_get_crystal_freq() / 16;
#endif
    div = (div * 64 + cfg->baud / 2) / cfg->baud;
    ibrd = div / 64;
    fbrd = div % 64;
    if (ibrd == 0 || ibrd >= 65535) {
        ASSERT(false, "Invalid baud param: %d\n", cfg->baud);
    }

    dmacr = 0;
    if (cfg->dma_rx) {
        dmacr |= UARTDMACR_RXDMAE;
    }
    if (cfg->dma_tx) {
        dmacr |= UARTDMACR_TXDMAE;
    }
    if (cfg->dma_rx_stop_on_err) {
        dmacr |= UARTDMACR_DMAONERR;
    }

    // Disable UART
    uart[id]->UARTCR &= ~UARTCR_UARTEN;
    // Empty FIFO
    uart[id]->UARTLCR_H &= ~UARTLCR_H_FEN;
    // Wait until UART becomes idle
    while (((uart[id]->UARTFR) & UARTFR_BUSY) != 0);
    // Clear previous errors
    uart[id]->UARTECR = 1;
    // Clear previous IRQs
    uart[id]->UARTIMSC = 0;
    uart[id]->UARTICR = ~0;
    // Configure UART
    uart[id]->UARTIBRD = ibrd;
    uart[id]->UARTFBRD = fbrd;
    uart[id]->UARTLCR_H = lcr;
    uart[id]->UARTDMACR = dmacr;
    uart[id]->UARTIFLS = UARTIFLS_TXFIFO_LEVEL(cfg->tx_level) |
                         UARTIFLS_RXFIFO_LEVEL(cfg->rx_level);
    uart[id]->UARTCR = cr;

    if (uart_irq_num[id] != INVALID_IRQn) {
        NVIC_SetVector(uart_irq_num[id], (uint32_t)hal_uart_irq_handler);
        // The priority should be the same as DMA's
        NVIC_SetPriority(uart_irq_num[id], IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(uart_irq_num[id]);
        NVIC_EnableIRQ(uart_irq_num[id]);
    }

    return 0;
}

int hal_uart_reopen(enum HAL_UART_ID_T id, const struct HAL_UART_CFG_T *cfg)
{
    uint32_t cr, dmacr;
    int i;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);

    if (!init_done) {
        init_done = true;
        for (i = HAL_UART_ID_0; i < HAL_UART_ID_QTY; i++) {
            recv_dma_chan[i] = HAL_DMA_CHAN_NONE;
            send_dma_chan[i] = HAL_DMA_CHAN_NONE;
        }
    }

    cr = 0;
    switch (cfg->flow) {
        case HAL_UART_FLOW_CONTROL_NONE:
            break;
        case HAL_UART_FLOW_CONTROL_RTS:
            cr |= UARTCR_RTSEN;
            break;
        case HAL_UART_FLOW_CONTROL_CTS:
            cr |= UARTCR_CTSEN;
            break;
        case HAL_UART_FLOW_CONTROL_RTSCTS:
            cr |= UARTCR_RTSEN
               |  UARTCR_CTSEN;
            break;
        default:
            ASSERT(false, "Invalid flow control param: %d\n", cfg->flow);
            break;
    }

    dmacr = 0;
    if (cfg->dma_rx) {
        dmacr |= UARTDMACR_RXDMAE;
    }
    if (cfg->dma_tx) {
        dmacr |= UARTDMACR_TXDMAE;
    }
    if (cfg->dma_rx_stop_on_err) {
        dmacr |= UARTDMACR_DMAONERR;
    }

    // Configure UART
    uart[id]->UARTDMACR = dmacr;
    uart[id]->UARTCR = (uart[id]->UARTCR & ~(UARTCR_RTSEN | UARTCR_CTSEN)) | cr;

    if (uart_irq_num[id] != INVALID_IRQn) {
        NVIC_SetVector(uart_irq_num[id], (uint32_t)hal_uart_irq_handler);
        // The priority should be the same as DMA's
        NVIC_SetPriority(uart_irq_num[id], IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(uart_irq_num[id]);
        NVIC_EnableIRQ(uart_irq_num[id]);
    }

    return 0;
}

int hal_uart_close(enum HAL_UART_ID_T id)
{
    uint32_t lock;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);

    if (uart_irq_num[id] != INVALID_IRQn) {
        NVIC_DisableIRQ(uart_irq_num[id]);
    }

    lock = int_lock();
    if (recv_dma_chan[id] != HAL_DMA_CHAN_NONE) {
        hal_gpdma_cancel(recv_dma_chan[id]);
        hal_gpdma_free_chan(recv_dma_chan[id]);
        recv_dma_chan[id] = HAL_DMA_CHAN_NONE;
    }
    if (send_dma_chan[id] != HAL_DMA_CHAN_NONE) {
        hal_gpdma_cancel(send_dma_chan[id]);
        hal_gpdma_free_chan(send_dma_chan[id]);
        send_dma_chan[id] = HAL_DMA_CHAN_NONE;
    }
    int_unlock(lock);

    // Disable UART
    uart[id]->UARTCR &= ~UARTCR_UARTEN;
    // Empty FIFO
    uart[id]->UARTLCR_H &= ~UARTLCR_H_FEN;

    hal_cmu_reset_set(uart_mod[id].apb);
    hal_cmu_reset_set(uart_mod[id].mod);
    hal_cmu_clock_disable(uart_mod[id].apb);
    hal_cmu_clock_disable(uart_mod[id].mod);

    return 0;
}

int hal_uart_opened(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    if (uart_mod[id].apb < HAL_CMU_MOD_QTY &&
            hal_cmu_clock_get_status(uart_mod[id].apb) != HAL_CMU_CLK_ENABLED) {
        return 0;
    }
    if (uart[id]->UARTCR & UARTCR_UARTEN) {
        return 1;
    }
    return 0;
}

int hal_uart_pause(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    if (hal_uart_opened(id)) {
        uart[id]->UARTCR &= ~(UARTCR_TXE | UARTCR_RXE);
        return 0;
    }
    return 1;
}

int hal_uart_continue(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    if (hal_uart_opened(id)) {
        uart[id]->UARTCR |= (UARTCR_TXE | UARTCR_RXE);
        return 0;
    }
    return 1;
}

int hal_uart_readable(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    return (uart[id]->UARTFR & UARTFR_RXFE) == 0;
}

int hal_uart_writable(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    return (uart[id]->UARTFR & UARTFR_TXFF) == 0;
}

uint8_t hal_uart_getc(enum HAL_UART_ID_T id)
{
    uint32_t c;
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    ASSERT((uart[id]->UARTDMACR & UARTDMACR_RXDMAE) == 0, "RX-DMA configured on UART %d", id);
    c = uart[id]->UARTDR;
    return (c & 0xFF);
}

int hal_uart_putc(enum HAL_UART_ID_T id, uint8_t c)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    ASSERT((uart[id]->UARTDMACR & UARTDMACR_TXDMAE) == 0, "TX-DMA configured on UART %d", id);
    uart[id]->UARTDR = c;
    return 0;
}

uint8_t hal_uart_blocked_getc(enum HAL_UART_ID_T id)
{
    while (hal_uart_readable(id) == 0);
    return hal_uart_getc(id);
}

int hal_uart_blocked_putc(enum HAL_UART_ID_T id, uint8_t c)
{
    while (hal_uart_writable(id) == 0);
    return hal_uart_putc(id, c);
}

union HAL_UART_FLAG_T hal_uart_get_flag(enum HAL_UART_ID_T id)
{
    union HAL_UART_FLAG_T flag;
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    flag.reg = uart[id]->UARTFR;
    return flag;
}

union HAL_UART_STATUS_T hal_uart_get_status(enum HAL_UART_ID_T id)
{
    union HAL_UART_STATUS_T status;
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    status.reg = uart[id]->UARTRSR;
    return status;
}

void hal_uart_clear_status(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    uart[id]->UARTECR = 0;
}

void hal_uart_break_set(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    uart[id]->UARTLCR_H |= UARTLCR_H_BRK;
}

void hal_uart_break_clear(enum HAL_UART_ID_T id)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    uart[id]->UARTLCR_H &= ~UARTLCR_H_BRK;
}

void hal_uart_flush(enum HAL_UART_ID_T id, uint32_t ticks)
{
    uint32_t cr;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);

    if ((uart[id]->UARTLCR_H & UARTLCR_H_FEN) == 0) {
        return;
    }

    cr = uart[id]->UARTCR;
    if (cr & UARTCR_UARTEN) {
        // Disable the UART
        uart[id]->UARTCR &= ~UARTCR_UARTEN;
        // Wait for the end of transmission or reception of the current character
        hal_sys_timer_delay((ticks > 0) ? ticks : UART_FLUSH_DELAY_DEFAULT);
    }
    // Flush FIFO
    uart[id]->UARTLCR_H &= ~UARTLCR_H_FEN;
    uart[id]->UARTLCR_H |= UARTLCR_H_FEN;
    if (cr & UARTCR_UARTEN) {
        // Enable the UART
        uart[id]->UARTCR |= UARTCR_UARTEN;
    }
}

union HAL_UART_IRQ_T hal_uart_get_raw_irq(enum HAL_UART_ID_T id)
{
    union HAL_UART_IRQ_T irq;

    irq.reg = uart[id]->UARTRIS;

    return irq;
}

void hal_uart_clear_irq(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T irq)
{
    uart[id]->UARTICR = irq.reg;
}

union HAL_UART_IRQ_T hal_uart_irq_get_mask(enum HAL_UART_ID_T id)
{
    union HAL_UART_IRQ_T mask;

    mask.reg = uart[id]->UARTIMSC;

    return mask;
}

union HAL_UART_IRQ_T hal_uart_irq_set_mask(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T mask)
{
    union HAL_UART_IRQ_T old_mask;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    old_mask.reg = uart[id]->UARTIMSC;
    uart[id]->UARTIMSC = mask.reg;

    return old_mask;
}

HAL_UART_IRQ_HANDLER_T hal_uart_irq_set_handler(enum HAL_UART_ID_T id, HAL_UART_IRQ_HANDLER_T handler)
{
    HAL_UART_IRQ_HANDLER_T old_handler;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    old_handler = irq_handler[id];
    irq_handler[id] = handler;

    return old_handler;
}

static void default_uart_handler(enum HAL_UART_ID_T id, union HAL_UART_IRQ_T status)
{
    uint32_t xfer = 0;
    uint32_t lock;

    if (status.BE) {
        if (break_handler[id]) {
            break_handler[id]();
            return;
        }
    }

    if (status.RT || status.FE || status.OE || status.PE || status.BE) {
		TRACE("RT=%d",status.RT);
		TRACE("FE=%d",status.FE);
		TRACE("OE=%d",status.OE);
		TRACE("PE=%d",status.PE);
		TRACE("BE=%d",status.BE);
        // Restore the traditional RT behaviour
        lock = int_lock();
        uart[id]->UARTLCR_H &= ~UARTLCR_H_DMA_RT_EN;
        int_unlock(lock);

        if (rxdma_handler[id]) {
            if (recv_dma_chan[id] != HAL_DMA_CHAN_NONE) {
                xfer = hal_uart_stop_dma_recv(id);
                if (recv_dma_size[id] > xfer) {
                    xfer = recv_dma_size[id] - xfer;
                } else {
                    xfer = 0;
                }
            }
            rxdma_handler[id](xfer, 0, status);
        }
    }
}

void hal_uart_irq_set_dma_handler(enum HAL_UART_ID_T id, HAL_UART_IRQ_RXDMA_HANDLER_T rxdma,
                                  HAL_UART_IRQ_TXDMA_HANDLER_T txdma, HAL_UART_IRQ_BREAK_HANDLER_T brk)
{
    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    rxdma_handler[id] = rxdma;
    txdma_handler[id] = txdma;
    break_handler[id] = brk;
    irq_handler[id] = default_uart_handler;
}

static void recv_dma_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    enum HAL_UART_ID_T id;
    uint32_t xfer;
    uint32_t lock;
    union HAL_UART_IRQ_T status;

    lock = int_lock();
    for (id = HAL_UART_ID_0; id < HAL_UART_ID_QTY; id++) {
        if (recv_dma_chan[id] == chan) {
            recv_dma_chan[id] = HAL_DMA_CHAN_NONE;
            break;
        }
    }
    int_unlock(lock);

    if (id == HAL_UART_ID_QTY) {
        return;
    }

    // Get remain xfer size
    xfer = hal_gpdma_get_sg_remain_size(chan);

    hal_gpdma_free_chan(chan);

    // Restore the traditional RT behaviour
    lock = int_lock();
    uart[id]->UARTLCR_H &= ~UARTLCR_H_DMA_RT_EN;
    int_unlock(lock);

    // Keep the same logic as default_uart_handler()
    status.reg = uart[id]->UARTMIS;
    if (status.reg) {
        uart[id]->UARTICR = status.reg;
        if (status.BE) {
            if (break_handler[id]) {
                break_handler[id]();
                return;
            }
        }
    }

    if (rxdma_handler[id]) {
        // Get already xfer size
        if (recv_dma_size[id] > xfer) {
            xfer = recv_dma_size[id] - xfer;
        } else {
            xfer = 0;
        }
        rxdma_handler[id](xfer, error, status);
    }
}


static void recv_dma_irq_handler_pingpang(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    enum HAL_UART_ID_T id;
    uint32_t xfer;
    uint32_t lock;
    union HAL_UART_IRQ_T status;
	//TRACE("recv_dma_irq_handler_pingpang");
    lock = int_lock();
#if 1    
    for (id = HAL_UART_ID_0; id < HAL_UART_ID_QTY; id++) {
        if (recv_dma_chan[id] == chan) {
          //  recv_dma_chan[id] = HAL_DMA_CHAN_NONE;
            break;
        }
    }
#endif    
    int_unlock(lock);

    if (id == HAL_UART_ID_QTY) {
        return;
    }

    // Get remain xfer size
    xfer = hal_gpdma_get_sg_remain_size(chan);

    //hal_gpdma_free_chan(chan);

    // Restore the traditional RT behaviour
    lock = int_lock();
    uart[id]->UARTLCR_H &= ~UARTLCR_H_DMA_RT_EN;

    int_unlock(lock);

    // Keep the same logic as default_uart_handler()
    status.reg = uart[id]->UARTMIS;
    if (status.reg) {
        uart[id]->UARTICR = status.reg;
        if (status.BE) {
            if (break_handler[id]) {
                break_handler[id]();
                return;
            }
        }
    }

    if (rxdma_handler[id]) {
        // Get already xfer size
#if 1        
        if (recv_dma_size[id] > xfer) {
            xfer = recv_dma_size[id] - xfer;
        } else {
            xfer = 0;
        }
#endif
            xfer = recv_dma_size[id]/2;

//        xfer = recv_dma_size[id] - xfer;
        rxdma_handler[id](xfer, error, status);
    }
}

static void start_dma_callback(uint8_t chan)
{
    enum HAL_UART_ID_T id;

    for (id = HAL_UART_ID_0; id < HAL_UART_ID_QTY; id++) {
        if (recv_dma_chan[id] == chan) {
            break;
        }
    }

    if (id == HAL_UART_ID_QTY) {
        return;
    }

    uart[id]->UARTIMSC = recv_mask[id].reg;
}

// Recv on 2 buffers
int hal_uart_dma_recv_mask2(enum HAL_UART_ID_T id, unsigned char *buf0, unsigned int len0,
                            unsigned char *buf1, unsigned int len1,
                            struct HAL_DMA_DESC_T desc[2], const union HAL_UART_IRQ_T *mask)
{
    struct HAL_DMA_CH_CFG_T dma_cfg;
    enum HAL_DMA_RET_T ret;
    uint32_t lock;
    enum HAL_DMA_PERIPH_T periph;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    ASSERT(uart_irq_num[id] != INVALID_IRQn, "DMA not supported on UART %d", id);
    ASSERT((uart[id]->UARTDMACR & UARTDMACR_RXDMAE), "DMA not configured on UART %d", id);

    if (len0 == 0 && len1 == 0) {
        return 0;
    }

    periph = (id == HAL_UART_ID_0) ? HAL_GPDMA_UART0_RX : HAL_GPDMA_UART1_RX;

    lock = int_lock();
    if (recv_dma_chan[id] != HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 1;
    }
    recv_dma_chan[id] = hal_gpdma_get_chan(periph, HAL_DMA_HIGH_PRIO);
    if (recv_dma_chan[id] == HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 2;
    }
    int_unlock(lock);

    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.ch = recv_dma_chan[id];
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_32;
    dma_cfg.dst_periph = 0; // useless
    dma_cfg.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.handler = recv_dma_irq_handler;
    dma_cfg.src = 0; // useless
    dma_cfg.src_bsize = HAL_DMA_BSIZE_8;
    dma_cfg.src_periph = periph;
    dma_cfg.src_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.type = HAL_DMA_FLOW_P2M_DMA;
    dma_cfg.try_burst = 0;

    if (mask) {
        recv_mask[id] = *mask;
        dma_cfg.start_cb = start_dma_callback;
    }

    // Activate DMA RT behaviour
    lock = int_lock();
    uart[id]->UARTLCR_H |= UARTLCR_H_DMA_RT_EN;
    int_unlock(lock);

    if (len0 == 0) {
        buf0 = buf1;
        len0 = len1;
        buf1 = NULL;
        len1 = 0;
    }
    if (len0 > HAL_UART_DMA_TRANSFER_STEP) {
        len1 = len0 - HAL_UART_DMA_TRANSFER_STEP;
        buf1 = buf0 + HAL_UART_DMA_TRANSFER_STEP;
    }

    dma_cfg.dst = (uint32_t)buf0;
    dma_cfg.src_tsize = len0;
    if (len1) {
        ret = hal_gpdma_init_desc(&desc[0], &dma_cfg, &desc[1], 0);
        if (ret != HAL_DMA_OK) {
            goto _err_exit;
        }
        if (len1 > HAL_UART_DMA_TRANSFER_STEP) {
            len1 = HAL_UART_DMA_TRANSFER_STEP;
        }
        dma_cfg.dst = (uint32_t)buf1;
        dma_cfg.src_tsize = len1;
        ret = hal_gpdma_init_desc(&desc[1], &dma_cfg, NULL, 1);
    } else {
        ret = hal_gpdma_init_desc(&desc[0], &dma_cfg, NULL, 1);
    }
    if (ret != HAL_DMA_OK) {
        goto _err_exit;
    }

    recv_dma_size[id] = len0 + len1;

    ret = hal_gpdma_sg_start(desc, &dma_cfg);

    if (ret != HAL_DMA_OK) {
_err_exit:
        // Restore the traditional RT behaviour
        lock = int_lock();
        uart[id]->UARTLCR_H &= ~UARTLCR_H_DMA_RT_EN;
        int_unlock(lock);
        hal_gpdma_free_chan(recv_dma_chan[id]);
        recv_dma_chan[id] = HAL_DMA_CHAN_NONE;
        return 3;
    }

    return 0;
}

// Safe API to trigger receive timeout IRQ and DMA IRQ
int hal_uart_dma_recv_mask(enum HAL_UART_ID_T id, unsigned char *buf, unsigned int len,
                           struct HAL_DMA_DESC_T *desc, unsigned int *desc_cnt,
                           const union HAL_UART_IRQ_T *mask)
{
    struct HAL_DMA_CH_CFG_T dma_cfg;
    enum HAL_DMA_RET_T ret;
    uint32_t lock;
    uint32_t cnt;
    uint32_t i;
    enum HAL_DMA_PERIPH_T periph;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    ASSERT(uart_irq_num[id] != INVALID_IRQn, "DMA not supported on UART %d", id);
    ASSERT((uart[id]->UARTDMACR & UARTDMACR_RXDMAE), "DMA not configured on UART %d", id);

    cnt = (len + HAL_UART_DMA_TRANSFER_STEP - 1) / HAL_UART_DMA_TRANSFER_STEP;

    // Return the required DMA descriptor count
    if (desc == NULL && desc_cnt) {
        *desc_cnt = (cnt == 1) ? 0 : cnt;
        return 0;
    }

    if (cnt == 0) {
        return 0;
    }
    if (cnt > 1) {
        if (desc == NULL || desc_cnt == NULL) {
            return -1;
        }
        if (*desc_cnt < cnt) {
            return -2;
        }
    }
    if (desc_cnt) {
        *desc_cnt = (cnt == 1) ? 0 : cnt;
    }

    periph = (id == HAL_UART_ID_0) ? HAL_GPDMA_UART0_RX : HAL_GPDMA_UART1_RX;

    lock = int_lock();
    if (recv_dma_chan[id] != HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 1;
    }
    recv_dma_chan[id] = hal_gpdma_get_chan(periph, HAL_DMA_HIGH_PRIO);
    if (recv_dma_chan[id] == HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 2;
    }
    int_unlock(lock);

    recv_dma_size[id] = len;

    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.ch = recv_dma_chan[id];
    dma_cfg.dst = (uint32_t)buf;
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_32;
    dma_cfg.dst_periph = 0; // useless
    dma_cfg.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.handler = recv_dma_irq_handler;
    dma_cfg.src = 0; // useless
    dma_cfg.src_bsize = HAL_DMA_BSIZE_8;
    dma_cfg.src_periph = periph;
    dma_cfg.src_tsize = len;
    dma_cfg.src_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.type = HAL_DMA_FLOW_P2M_DMA;
    dma_cfg.try_burst = 0;

    if (mask) {
        recv_mask[id] = *mask;
        dma_cfg.start_cb = start_dma_callback;
    }

    // Activate DMA RT behaviour
    lock = int_lock();
    uart[id]->UARTLCR_H |= UARTLCR_H_DMA_RT_EN;
    int_unlock(lock);

    if (cnt == 1) {
        ret = hal_gpdma_start(&dma_cfg);
    } else {
        for (i = 0; i < cnt - 1; i++) {
            dma_cfg.src_tsize = HAL_UART_DMA_TRANSFER_STEP;
            ret = hal_gpdma_init_desc(&desc[i], &dma_cfg, &desc[i + 1], 0);
            if (ret != HAL_DMA_OK) {
                goto _err_exit;
            }
            dma_cfg.dst += HAL_UART_DMA_TRANSFER_STEP;
        }
        dma_cfg.src_tsize = len - (HAL_UART_DMA_TRANSFER_STEP * i);
        ret = hal_gpdma_init_desc(&desc[i], &dma_cfg, NULL, 1);
        if (ret != HAL_DMA_OK) {
            goto _err_exit;
        }
        ret = hal_gpdma_sg_start(desc, &dma_cfg);
    }

    if (ret != HAL_DMA_OK) {
_err_exit:
        // Restore the traditional RT behaviour
        lock = int_lock();
        uart[id]->UARTLCR_H &= ~UARTLCR_H_DMA_RT_EN;
        int_unlock(lock);
        hal_gpdma_free_chan(recv_dma_chan[id]);
        recv_dma_chan[id] = HAL_DMA_CHAN_NONE;
        return 3;
    }

    return 0;
}



int hal_uart_dma_recv_mask_pingpang(enum HAL_UART_ID_T id, unsigned char *buf, unsigned int len,
                           struct HAL_DMA_DESC_T *desc, unsigned int *desc_cnt,
                           const union HAL_UART_IRQ_T *mask)
{
    struct HAL_DMA_CH_CFG_T dma_cfg;
    enum HAL_DMA_RET_T ret;
    uint32_t lock;
    uint32_t cnt;
    uint32_t i;
    enum HAL_DMA_PERIPH_T periph;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    ASSERT(uart_irq_num[id] != INVALID_IRQn, "DMA not supported on UART %d", id);
    ASSERT((uart[id]->UARTDMACR & UARTDMACR_RXDMAE), "DMA not configured on UART %d", id);

    cnt = (len + HAL_UART_DMA_TRANSFER_STEP_PINGPANG - 1) / HAL_UART_DMA_TRANSFER_STEP_PINGPANG;
	TRACE("cnt=%d",cnt);
    // Return the required DMA descriptor count
    if (desc == NULL && desc_cnt) {
        *desc_cnt = (cnt == 1) ? 0 : cnt;
        return 0;
    }
    if (cnt == 0) {
        return 0;
    }
    if (cnt > 1) {
        if (desc == NULL || desc_cnt == NULL) {
            return -1;
        }
        if (*desc_cnt < cnt) {
            return -2;
        }
    }
    if (desc_cnt) {
        *desc_cnt = (cnt == 1) ? 0 : cnt;
    }

    periph = (id == HAL_UART_ID_0) ? HAL_GPDMA_UART0_RX : HAL_GPDMA_UART1_RX;

    lock = int_lock();
    if (recv_dma_chan[id] != HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 1;
    }
    recv_dma_chan[id] = hal_gpdma_get_chan(periph, HAL_DMA_HIGH_PRIO);
    if (recv_dma_chan[id] == HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 2;
    }
    int_unlock(lock);

    recv_dma_size[id] = len;

    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.ch = recv_dma_chan[id];
    dma_cfg.dst = (uint32_t)buf;
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_32;
    dma_cfg.dst_periph = 0; // useless
    dma_cfg.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.handler = recv_dma_irq_handler_pingpang;
    dma_cfg.src = 0; // useless
    dma_cfg.src_bsize = HAL_DMA_BSIZE_8;
    dma_cfg.src_periph = periph;
    dma_cfg.src_tsize = len/2;
    dma_cfg.src_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.type = HAL_DMA_FLOW_P2M_DMA;
    dma_cfg.try_burst = 0;

    if (mask) {
        recv_mask[id] = *mask;
        dma_cfg.start_cb = start_dma_callback;
    }

    // Activate DMA RT behaviour
    lock = int_lock();
    uart[id]->UARTLCR_H |= UARTLCR_H_DMA_RT_EN;
    int_unlock(lock);
    if (cnt == 1) {
        ret = hal_gpdma_start(&dma_cfg);
    } else {
        for (i = 0; i < cnt - 1; i++) {
            dma_cfg.src_tsize = HAL_UART_DMA_TRANSFER_STEP_PINGPANG;
            ret = hal_gpdma_init_desc(&desc[i], &dma_cfg, &desc[i + 1], 1);
            if (ret != HAL_DMA_OK) {
                goto _err_exit;
            }
            dma_cfg.dst += HAL_UART_DMA_TRANSFER_STEP_PINGPANG;
        }
        dma_cfg.src_tsize = len - (HAL_UART_DMA_TRANSFER_STEP_PINGPANG * i);
        ret = hal_gpdma_init_desc(&desc[i], &dma_cfg, &desc[0], 1);
        if (ret != HAL_DMA_OK) {
            goto _err_exit;
        }
        ret = hal_gpdma_sg_start(desc, &dma_cfg);
    }

    if (ret != HAL_DMA_OK) {
_err_exit:
        // Restore the traditional RT behaviour
        lock = int_lock();
        uart[id]->UARTLCR_H &= ~UARTLCR_H_DMA_RT_EN;
        int_unlock(lock);
        hal_gpdma_free_chan(recv_dma_chan[id]);
        recv_dma_chan[id] = HAL_DMA_CHAN_NONE;
        return 3;
    }

    return 0;
}

int hal_uart_dma_recv(enum HAL_UART_ID_T id, unsigned char *buf, unsigned int len,
                      struct HAL_DMA_DESC_T *desc, unsigned int *desc_cnt)
{
    return hal_uart_dma_recv_mask(id, buf, len, desc, desc_cnt, NULL);
}


int hal_uart_dma_recv_pingpang(enum HAL_UART_ID_T id, unsigned char *buf, unsigned int len,
                      struct HAL_DMA_DESC_T *desc, unsigned int *desc_cnt)
{
    return hal_uart_dma_recv_mask_pingpang(id, buf, len, desc, desc_cnt, NULL);
}


uint32_t hal_uart_stop_dma_recv(enum HAL_UART_ID_T id)
{
    uint32_t remains;
    uint32_t lock;
    uint8_t chan;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);

    lock = int_lock();
    chan = recv_dma_chan[id];
    recv_dma_chan[id] = HAL_DMA_CHAN_NONE;
    // Restore the traditional RT behaviour
    uart[id]->UARTLCR_H &= ~UARTLCR_H_DMA_RT_EN;
    int_unlock(lock);

    if (chan == HAL_DMA_CHAN_NONE) {
        return 0;
    }

    // Save the data in DMA FIFO
    hal_gpdma_stop(chan);
    remains = hal_gpdma_get_sg_remain_size(chan);
    hal_gpdma_free_chan(chan);

    return remains;
}

static void send_dma_irq_handler(uint8_t chan, uint32_t remain_tsize, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
    enum HAL_UART_ID_T id;
    uint32_t xfer;
    uint32_t lock;

    lock = int_lock();
    for (id = HAL_UART_ID_0; id < HAL_UART_ID_QTY; id++) {
        if (send_dma_chan[id] == chan) {
            send_dma_chan[id] = HAL_DMA_CHAN_NONE;
            break;
        }
    }
    int_unlock(lock);

    if (id == HAL_UART_ID_QTY) {
        return;
    }

    // Get remain xfer size
    xfer = hal_gpdma_get_sg_remain_size(chan);

    hal_gpdma_free_chan(chan);

    if (txdma_handler[id]) {
        // Get already xfer size
        if (send_dma_size[id] > xfer) {
            xfer = send_dma_size[id] - xfer;
        } else {
            xfer = 0;
        }
        txdma_handler[id](xfer, error);
    }
}

int hal_uart_dma_send2(enum HAL_UART_ID_T id, const unsigned char *buf0, unsigned int len0,
                       const unsigned char *buf1, unsigned int len1,
                       struct HAL_DMA_DESC_T desc[2])
{
    struct HAL_DMA_CH_CFG_T dma_cfg;
    enum HAL_DMA_RET_T ret;
    uint32_t lock;
    enum HAL_DMA_PERIPH_T periph;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    ASSERT(uart_irq_num[id] != INVALID_IRQn, "DMA not supported on UART %d", id);
    ASSERT((uart[id]->UARTDMACR & UARTDMACR_TXDMAE), "DMA not configured on UART %d", id);

    if (len0 == 0 && len1 == 0) {
        return 0;
    }

    periph = (id == HAL_UART_ID_0) ? HAL_GPDMA_UART0_TX : HAL_GPDMA_UART1_TX;

    lock = int_lock();
    if (send_dma_chan[id] != HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 1;
    }
    send_dma_chan[id] = hal_gpdma_get_chan(periph, HAL_DMA_HIGH_PRIO);
    if (send_dma_chan[id] == HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 2;
    }
    int_unlock(lock);


    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.ch = send_dma_chan[id];
    dma_cfg.dst = 0; // useless
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_8;
    dma_cfg.dst_periph = periph;
    dma_cfg.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.handler = send_dma_irq_handler;
    dma_cfg.src_bsize = HAL_DMA_BSIZE_32;
    dma_cfg.src_periph = 0; // useless
    dma_cfg.src_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.type = HAL_DMA_FLOW_M2P_DMA;
    dma_cfg.try_burst = 0;

    if (len0 == 0) {
        buf0 = buf1;
        len0 = len1;
        buf1 = NULL;
        len1 = 0;
    }
    if (len0 > HAL_UART_DMA_TRANSFER_STEP) {
        len1 = len0 - HAL_UART_DMA_TRANSFER_STEP;
        buf1 = buf0 + HAL_UART_DMA_TRANSFER_STEP;
    }

    dma_cfg.src = (uint32_t)buf0;
    dma_cfg.src_tsize = len0;
    if (len1) {
        ret = hal_gpdma_init_desc(&desc[0], &dma_cfg, &desc[1], 0);
        if (ret != HAL_DMA_OK) {
            goto _err_exit;
        }
        if (len1 > HAL_UART_DMA_TRANSFER_STEP) {
            len1 = HAL_UART_DMA_TRANSFER_STEP;
        }
        dma_cfg.src = (uint32_t)buf1;
        dma_cfg.src_tsize = len1;
        ret = hal_gpdma_init_desc(&desc[1], &dma_cfg, NULL, 1);
    } else {
        ret = hal_gpdma_init_desc(&desc[0], &dma_cfg, NULL, 1);
    }
    if (ret != HAL_DMA_OK) {
        goto _err_exit;
    }

    send_dma_size[id] = len0 + len1;

    ret = hal_gpdma_sg_start(desc, &dma_cfg);

    if (ret != HAL_DMA_OK) {
_err_exit:
        hal_gpdma_free_chan(send_dma_chan[id]);
        send_dma_chan[id] = HAL_DMA_CHAN_NONE;
        return 3;
    }

    return 0;
}

int hal_uart_dma_send(enum HAL_UART_ID_T id, const unsigned char *buf, unsigned int len,
                      struct HAL_DMA_DESC_T *desc, unsigned int *desc_cnt)
{
    struct HAL_DMA_CH_CFG_T dma_cfg;
    enum HAL_DMA_RET_T ret;
    uint32_t lock;
    uint32_t cnt;
    uint32_t i;
    enum HAL_DMA_PERIPH_T periph;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);
    ASSERT(uart_irq_num[id] != INVALID_IRQn, "DMA not supported on UART %d", id);
    ASSERT((uart[id]->UARTDMACR & UARTDMACR_TXDMAE), "DMA not configured on UART %d", id);

    cnt = (len + HAL_UART_DMA_TRANSFER_STEP - 1) / HAL_UART_DMA_TRANSFER_STEP;

    // Return the required DMA descriptor count
    if (desc == NULL && desc_cnt) {
        *desc_cnt = (cnt == 1) ? 0 : cnt;
        return 0;
    }

    if (cnt == 0) {
        return 0;
    }
    if (cnt > 1) {
        if (desc == NULL || desc_cnt == NULL) {
            return -1;
        }
        if (*desc_cnt < cnt) {
            return -2;
        }
    }
    if (desc_cnt) {
        *desc_cnt = (cnt == 1) ? 0 : cnt;
    }

    periph = (id == HAL_UART_ID_0) ? HAL_GPDMA_UART0_TX : HAL_GPDMA_UART1_TX;

    lock = int_lock();
    if (send_dma_chan[id] != HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 1;
    }
    send_dma_chan[id] = hal_gpdma_get_chan(periph, HAL_DMA_HIGH_PRIO);
    if (send_dma_chan[id] == HAL_DMA_CHAN_NONE) {
        int_unlock(lock);
        return 2;
    }
    int_unlock(lock);

    send_dma_size[id] = len;

    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.ch = send_dma_chan[id];
    dma_cfg.dst = 0; // useless
    dma_cfg.dst_bsize = HAL_DMA_BSIZE_8;
    dma_cfg.dst_periph = periph;
    dma_cfg.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.handler = send_dma_irq_handler;
    dma_cfg.src = (uint32_t)buf;
    dma_cfg.src_bsize = HAL_DMA_BSIZE_32;
    dma_cfg.src_periph = 0; // useless
    dma_cfg.src_tsize = len;
    dma_cfg.src_width = HAL_DMA_WIDTH_BYTE;
    dma_cfg.type = HAL_DMA_FLOW_M2P_DMA;
    dma_cfg.try_burst = 0;

    if (cnt == 1) {
        ret = hal_gpdma_start(&dma_cfg);
    } else {
        for (i = 0; i < cnt - 1; i++) {
            dma_cfg.src_tsize = HAL_UART_DMA_TRANSFER_STEP;
            ret = hal_gpdma_init_desc(&desc[i], &dma_cfg, &desc[i + 1], 0);
            if (ret != HAL_DMA_OK) {
                goto _err_exit;
            }
            dma_cfg.src += HAL_UART_DMA_TRANSFER_STEP;
        }
        dma_cfg.src_tsize = len - (HAL_UART_DMA_TRANSFER_STEP * i);
        ret = hal_gpdma_init_desc(&desc[i], &dma_cfg, NULL, 1);
        if (ret != HAL_DMA_OK) {
            goto _err_exit;
        }
        ret = hal_gpdma_sg_start(desc, &dma_cfg);
    }

    if (ret != HAL_DMA_OK) {
_err_exit:
        hal_gpdma_free_chan(send_dma_chan[id]);
        send_dma_chan[id] = HAL_DMA_CHAN_NONE;
        return 3;
    }

    return 0;
}

uint32_t hal_uart_stop_dma_send(enum HAL_UART_ID_T id)
{
    uint32_t remains;
    uint32_t lock;
    uint8_t chan;

    ASSERT(id < HAL_UART_ID_QTY, err_invalid_id, id);

    lock = int_lock();
    chan = send_dma_chan[id];
    send_dma_chan[id] = HAL_DMA_CHAN_NONE;
    int_unlock(lock);

    if (chan == HAL_DMA_CHAN_NONE) {
        return 0;
    }

    // Not to keep the data in DMA FIFO
    hal_gpdma_cancel(chan);
    remains = hal_gpdma_get_sg_remain_size(chan);
    hal_gpdma_free_chan(chan);

    return remains;
}

static void hal_uart_irq_handler(void)
{
    enum HAL_UART_ID_T id;
    union HAL_UART_IRQ_T state;

    for (id = HAL_UART_ID_0; id < HAL_UART_ID_QTY; id++) {
        state.reg = uart[id]->UARTMIS;

        if (state.reg) {
            uart[id]->UARTICR = state.reg;

            if (irq_handler[id] != NULL) {
                irq_handler[id](id, state);
            }
        }
    }
}

// ========================================================================
// Test function

#include "stdarg.h"
#include "stdio.h"

#if !defined(DEBUG_PORT) || (DEBUG_PORT == 1)
#define UART_PRINTF_ID                  HAL_UART_ID_0
#else
#define UART_PRINTF_ID                  HAL_UART_ID_1
#endif

int hal_uart_printf_init(void)
{
    static const struct HAL_UART_CFG_T uart_cfg = {
        .parity = HAL_UART_PARITY_NONE,
        .stop = HAL_UART_STOP_BITS_1,
        .data = HAL_UART_DATA_BITS_8,
        .flow = HAL_UART_FLOW_CONTROL_NONE,//HAL_UART_FLOW_CONTROL_RTSCTS,
        .tx_level = HAL_UART_FIFO_LEVEL_1_2,
        .rx_level = HAL_UART_FIFO_LEVEL_1_4,
        .baud = 921600,
        .dma_rx = false,
        .dma_tx = false,
        .dma_rx_stop_on_err = false,
    };

    if (UART_PRINTF_ID == HAL_UART_ID_0) {
        hal_iomux_set_uart0();
    } else {
        hal_iomux_set_uart1();
    }

    return hal_uart_open(UART_PRINTF_ID, &uart_cfg);
}

void hal_uart_printf(const char *fmt, ...)
{
    char buf[200];
    int ret;
    int i;
    va_list ap;

    va_start(ap, fmt);
    ret = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (ret + 2 < sizeof(buf)) {
        buf[ret++] = '\r';
    }
    if (ret + 1 < sizeof(buf)) {
        buf[ret++] = '\n';
    }
    //buf[ret] = 0;
    va_end(ap);

    if (ret > 0) {
        for (i = 0; i < ret; i++) {
            hal_uart_blocked_putc(UART_PRINTF_ID, buf[i]);
        }
    }
}

#endif // CHIP_HAS_UART
