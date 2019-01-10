#include "cmsis_os.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "cmsis_nvic.h"
#include "hal_timer_raw.h"
#include "hal_uart.h"
#include "hal_cmu.h"
#include "hal_bootmode.h"
#include "hal_iomux.h"
#include "pmu.h"
#include "app_thread.h"
#include "Knowles_mic_communication.h"


//#define USE_UART0

#define WAIT_TRACE_TIMEOUT                      MS_TO_TICKS(200)

static uint32_t send_timeout = MS_TO_TICKS(100);
static uint32_t recv_timeout = MS_TO_TICKS(100);

#define TIMEOUT_INFINITE                ((uint32_t)-1)

static const struct HAL_UART_CFG_T uart_cfg = {
    HAL_UART_PARITY_NONE,
    HAL_UART_STOP_BITS_2,
    HAL_UART_DATA_BITS_8,
    HAL_UART_FLOW_CONTROL_NONE, // RTC/CTS pins might be unavailable for some chip packages
    HAL_UART_FIFO_LEVEL_1_2,
    HAL_UART_FIFO_LEVEL_1_2,
    115200,
    true,
    false,
    false,
};
#ifdef USE_UART0
static const enum HAL_UART_ID_T comm_uart = HAL_UART_ID_0;
#else
static const enum HAL_UART_ID_T comm_uart = HAL_UART_ID_1;
#endif
static volatile enum UART_DMA_STATE uart_dma_rx_state = UART_DMA_IDLE;
static volatile uint32_t uart_dma_rx_size = 0;
static bool uart_opened = false;


static volatile bool cancel_xfer = false;

static uint32_t xfer_err_time = 0;
static uint32_t xfer_err_cnt = 0;


static void uart_rx_dma_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    // The DMA transfer has been cancelled
    if (uart_dma_rx_state != UART_DMA_START) {
        return;
    }

    uart_dma_rx_size = xfer_size;
    if (dma_error || status.FE || status.OE || status.PE || status.BE) {
        TRACE("UART-RX Error: dma_error=%d, status=0x%08x", dma_error, status.reg);
        uart_dma_rx_state = UART_DMA_ERROR;
    } else {
        TRACE("UART-RX OK: rx_size=%d", uart_dma_rx_size);
        uart_dma_rx_state = UART_DMA_DONE;
    }
}

static void uart_break_handler(void)
{
    TRACE("****** Handle break ******");

    cancel_xfer = true;
    hal_uart_stop_dma_recv(comm_uart);
    hal_uart_stop_dma_send(comm_uart);
    uart_dma_rx_state = UART_DMA_ERROR;
}

void smart_mic_recv_data_state_get(enum UART_DMA_STATE *state)
{
    *state = uart_dma_rx_state;
}

void smart_mic_recv_data_reset(void)
{    
    union HAL_UART_IRQ_T mask;
    mask.reg = 0;
    mask.BE = 1;
    hal_uart_irq_set_mask(comm_uart, mask);
    uart_dma_rx_state = UART_DMA_IDLE;
}

void smart_mic_init_transport()
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP POSSIBLY_UNUSED pinmux_uart1[] = {
        {HAL_IOMUX_PIN_P2_6, HAL_IOMUX_FUNC_UART1_RX, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P2_7, HAL_IOMUX_FUNC_UART1_TX, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };
    union HAL_UART_IRQ_T mask;
    struct HAL_UART_CFG_T comm_uart_cfg;
#ifdef USE_UART0	
	hal_iomux_set_uart0();
#else
	hal_iomux_set_uart1();
	//hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)pinmux_uart1, sizeof(pinmux_uart1)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
#endif
#if 0
    if (!uart_opened) {
		
		//hal_iomux_set_uart0();
        //hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)pinmux_uart1, sizeof(pinmux_uart1)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
        memcpy(&comm_uart_cfg, &uart_cfg, sizeof(comm_uart_cfg));	
        hal_uart_open(comm_uart, &comm_uart_cfg);
        mask.reg = 0;
        mask.BE = 1;
        mask.FE = 1;
        mask.OE = 1;
        mask.PE = 1;        
      
        hal_uart_irq_set_dma_handler(comm_uart, uart_rx_dma_handler, NULL, uart_break_handler);
        hal_uart_irq_set_mask(comm_uart, mask);
        uart_opened = true;
    }
#endif
    cancel_xfer = false;

    //hal_uart_flush(comm_uart,0);
}

void smart_mic_reinit_transport(unsigned int bps,unsigned char stop_bits)
{
    union HAL_UART_IRQ_T mask;
    struct HAL_UART_CFG_T comm_uart_cfg;

    uart_opened = false;
    memcpy(&comm_uart_cfg, &uart_cfg, sizeof(comm_uart_cfg));
	comm_uart_cfg.baud=bps;
	comm_uart_cfg.stop=(enum HAL_UART_STOP_BITS_T)(stop_bits-1);
    hal_uart_open(comm_uart, &comm_uart_cfg);
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;        
    hal_uart_irq_set_dma_handler(comm_uart, uart_rx_dma_handler, NULL, uart_break_handler);
    hal_uart_irq_set_mask(comm_uart, mask);
    uart_opened = true;

    cancel_xfer = false;

    hal_uart_flush(comm_uart,0);
}



void smart_mic_set_send_timeout(unsigned int timeout)
{
    send_timeout = MS_TO_TICKS(timeout);
}

void smart_mic_set_recv_timeout(unsigned int timeout)
{
    recv_timeout = MS_TO_TICKS(timeout);
}


static int uart_send_data(unsigned char *buf, unsigned int len)
{
    uint32_t start;
    uint32_t sent = 0;

    start = hal_sys_timer_get();
    while (sent < len) {
        while (!cancel_xfer && !hal_uart_writable(comm_uart) &&
                hal_sys_timer_get() - start < send_timeout);
        if (cancel_xfer) {
            break;
        }
        if (hal_uart_writable(comm_uart)) {
            hal_uart_putc(comm_uart, buf[sent++]);
        } else {
            break;
        }
    }

    if (sent != len) {
        return 1;
    }

    return 0;
}

int smart_mic_send_data(unsigned char *buf, unsigned int len)
{
    if (cancel_xfer) {
        return -1;
    }
	if(0){
		TRACE("TX:");
		DUMP8("%02x ",buf,len);
	}

    return uart_send_data(buf, len);
}


static int uart_recv_data_dma(unsigned char *buf,unsigned int expect_len,unsigned int *rlen)
{
    int ret;
    union HAL_UART_IRQ_T mask;
	uint32_t start;
    struct HAL_DMA_DESC_T dma_desc[17];
    unsigned int desc_cnt = ARRAY_SIZE(dma_desc);

    if (uart_dma_rx_state != UART_DMA_IDLE) {
        ret = -3;
        goto _exit;
    }

    uart_dma_rx_state = UART_DMA_START;
    uart_dma_rx_size = 0;

    ret = hal_uart_dma_recv(comm_uart, buf, expect_len, &dma_desc[0], &desc_cnt);
    if (ret) {
        uart_dma_rx_state = UART_DMA_ERROR;
        goto _exit;
    }

    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    //mask.RT = 1;
    hal_uart_irq_set_mask(comm_uart, mask);
	
	start = hal_sys_timer_get();
    while (uart_dma_rx_state == UART_DMA_START && !cancel_xfer &&
            (recv_timeout == TIMEOUT_INFINITE || hal_sys_timer_get() - start < recv_timeout));

    if (cancel_xfer) {
        ret = -14;
        goto _exit;
    }
    if (uart_dma_rx_state == UART_DMA_START) {
        hal_uart_stop_dma_recv(comm_uart);
        ret = -15;
        goto _exit;
    }
    if (uart_dma_rx_state != UART_DMA_DONE) {
        ret = -2;
        goto _exit;
    }
	
    ret = 0;
    *rlen = uart_dma_rx_size;

	
_exit:
	mask.reg = 0;
	mask.BE = 1;
	hal_uart_irq_set_mask(comm_uart, mask);
	//TRACE("RX");
	//DUMP8("%02x ",buf,uart_dma_rx_size);
	uart_dma_rx_state = UART_DMA_IDLE;

_no_state_exit:
    return ret;

}


int smart_mic_recv_data(unsigned char *buf, unsigned int len,unsigned int *rlen)
{
    if (cancel_xfer) {
        return -1;
    }
	TRACE("######smart_mic_recv_data#####\n");
    return uart_recv_data_dma(buf, len, rlen);
}



static int uart_handle_error(void)
{
    TRACE("****** Send break ******");

    // Send break signal, to tell the peer to reset the connection
    hal_uart_break_set(comm_uart);
    osDelay(100);
    hal_uart_break_clear(comm_uart);

    return 0;
}

int smart_mic_handle_error(void)
{
    int ret = 0;
    uint32_t err_time;

    osDelay(200);

    if (!cancel_xfer) {
        ret = uart_handle_error();
    }

    xfer_err_cnt++;
    err_time = hal_sys_timer_get();
    if (err_time - xfer_err_time > MS_TO_TICKS(5000)) {
        xfer_err_cnt = 0;
    }
    xfer_err_time = err_time;
    if (xfer_err_cnt < 2) {
        osDelay(500);
    } else if (xfer_err_cnt < 5) {
        osDelay(1000);
    } else {
        osDelay(2000);
    }

    return ret;
}

static int uart_cancel_input(void)
{
    hal_uart_flush(comm_uart,0);
    return 0;
}

int smart_mic_cancel_input(void)
{
    return uart_cancel_input();
}



