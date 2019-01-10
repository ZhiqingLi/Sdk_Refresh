#include "stdbool.h"
#include "hal_intersys.h"
#include "hal_uart.h"
#include "hal_trace.h"
#include "string.h"

#define BT_UART_BRIDGE_BUF_LEN (1024)

extern int app_audio_mempool_init(void);
extern uint32_t app_audio_mempool_free_buff_size(void);
extern int app_audio_mempool_get_buff(uint8_t **buff, uint32_t size);

static const struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_4,
    .baud = 115200,
    .dma_rx = true,
    .dma_tx = true,
    .dma_rx_stop_on_err = false,
};

#if (DEBUG_PORT == 1)
enum HAL_UART_ID_T br_uart = HAL_UART_ID_0;
#else
enum HAL_UART_ID_T br_uart = HAL_UART_ID_0;
#endif
enum HAL_INTERSYS_ID_T br_intersys = HAL_INTERSYS_ID_0;

static volatile bool uart_rx_done = false;
static volatile bool uart_tx_done = false;
static volatile bool bt_rx_done = false;
static volatile bool bt_tx_done = false;

static unsigned char *uart_rx_data_p;
static volatile unsigned int uart_rx_len = 0;

static unsigned char *bt_rx_data_p;
static volatile unsigned int bt_rx_len = 0;

static void uart_rx(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    if (dma_error) {
        TRACE("uart_rx dma error: xfer_size=%d", xfer_size);
        uart_rx_len = 0;
    } else if (status.BE || status.FE || status.OE || status.PE) {
        TRACE("uart_rx uart error: xfer_size=%d, status=0x%08x", xfer_size, status.reg);
        uart_rx_len = 0;
    } else {
        uart_rx_len = xfer_size;
    }
    uart_rx_done = true;
}

static void uart_tx(uint32_t xfer_size, int dma_error)
{
    if (dma_error) {
        TRACE("uart_tx dma error: xfer_size=%d", xfer_size);
    }
    uart_tx_done = true;
}

static unsigned int bt_rx(const unsigned char *data, unsigned int len)
{
    unsigned int processed = len;


    hal_intersys_stop_recv(br_intersys);
    if (bt_rx_len + processed > BT_UART_BRIDGE_BUF_LEN) {
        TRACE("bt_rx data too long: bt_rx_len=%d, len=%d, limit=%d", bt_rx_len, processed, sizeof(bt_rx_data_p));
        processed = BT_UART_BRIDGE_BUF_LEN-bt_rx_len;
    }
    if (processed > 0) {
        memcpy(bt_rx_data_p + bt_rx_len, data, processed);
        bt_rx_len += processed;
        bt_rx_done = true;
    }
    return len;
}

static void bt_tx(const unsigned char *data, unsigned int len)
{
    bt_tx_done = true;
}

void btdrv_uart_bridge_loop(void)
{
    int ret;
    union HAL_UART_IRQ_T mask;

    app_audio_mempool_init();
    
    app_audio_mempool_get_buff(&uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN);
    app_audio_mempool_get_buff(&bt_rx_data_p, BT_UART_BRIDGE_BUF_LEN);

    ret = hal_uart_open(br_uart, &uart_cfg);
    if (ret) {
        TRACE("Failed to open uart");
        return;
    }

    hal_uart_irq_set_dma_handler(br_uart, uart_rx, uart_tx, NULL);
    hal_uart_dma_recv(br_uart, uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN, NULL, NULL);
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    mask.RT = 1;
    hal_uart_irq_set_mask(br_uart, mask);

    ret = hal_intersys_open(br_intersys, HAL_INTERSYS_MSG_HCI, bt_rx, bt_tx, false);
    if (ret) {
        TRACE("Failed to open intersys");
        return;
    }
    hal_intersys_start_recv(br_intersys);

    while (1) {
        if (uart_rx_done) {
            uart_rx_done = false;
            if (uart_rx_len > 0) {
                hal_intersys_send(br_intersys, HAL_INTERSYS_MSG_HCI, uart_rx_data_p, uart_rx_len);
            } else {
                bt_tx_done = true;
            }
        }
        if (uart_tx_done) {
            uart_tx_done = false;
            bt_rx_len = 0;
            hal_intersys_start_recv(br_intersys);
        }
        if (bt_rx_done) {
            bt_rx_done = false;
            if (bt_rx_len > 0) {
                hal_uart_dma_send(br_uart, bt_rx_data_p, bt_rx_len, NULL, NULL);
            } else {
                uart_tx_done = true;
            }
        }
        if (bt_tx_done) {
            bt_tx_done = false;
            hal_uart_dma_recv(br_uart, uart_rx_data_p, BT_UART_BRIDGE_BUF_LEN, NULL, NULL);
        }
    }
}

