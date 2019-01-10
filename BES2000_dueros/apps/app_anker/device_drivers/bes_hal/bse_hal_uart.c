#include "cmsis_os.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "string.h"
#include "cmsis_nvic.h"
#include "hal_uart.h"
#include "hal_iomux.h"
#include "hal_timer_raw.h"
#include "bes_hal_uart.h"
#include <stdarg.h>
#include "bes_hal_i2c.h"

#define WAIT_TRACE_TIMEOUT                      MS_TO_TICKS(200)

#define OTA_PRINT_PREX_LEN                      (4)
#define OTA_PRINT_PREX_HEAD                     "@Up#"
#define OTA_PRINT_PAYLOAD_LEN_SIZE              (4) // (0)

static uint32_t send_timeout = MS_TO_TICKS(500);

static const struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE, // RTC/CTS pins might be unavailable for some chip packages
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_2,
#if UART_OTA_PORT_SEL == UART_OTA_PORT_SAME_SYS_DEBUG
    .baud = 921600,
#else
    .baud = 115200,
#endif
    .dma_rx = true,
    .dma_tx = true,
    .dma_rx_stop_on_err = false,
};

#if UART_OTA_PORT_SEL == UART_OTA_PORT_SAME_SYS_DEBUG
static const enum HAL_UART_ID_T comm_uart = HAL_UART_ID_0;
#else
static const enum HAL_UART_ID_T comm_uart = HAL_UART_ID_1;
#endif

static volatile UART_DMA_STATE uart_dma_rx_state = UART_DMA_IDLE;
static volatile UART_DMA_STATE uart_dma_tx_state = UART_DMA_IDLE;
static volatile uint32_t uart_dma_rx_size = 0;
static volatile uint32_t uart_dma_tx_size = 0;
static bool uart_opened = false;


static volatile bool cancel_xfer = false;

static uint32_t xfer_err_time = 0;
static uint32_t xfer_err_cnt = 0;
static unsigned char logLevel = 0;

void bes_hal_set_log_level(unsigned char lvl)
{
    logLevel = lvl;
}

unsigned char bes_hal_get_log_level(void)
{
    return logLevel;
}

static void bes_hal_uart_rx_dma_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    // The DMA transfer has been cancelled
    if (uart_dma_rx_state != UART_DMA_START) {
        return;
    }

    uart_dma_rx_size = xfer_size;
    if (dma_error || status.FE || status.OE || status.PE || status.BE) {
        BES_HAL_UART_ERROR("UART-RX Error: dma_error=%d, status=0x%08x", dma_error, status.reg);
        uart_dma_rx_state = UART_DMA_ERROR;
        bes_hal_uart_transport_event_notify(uart_dma_rx_state,uart_dma_rx_size);
    } else {
    //    BES_HAL_UART_DEBUG("UART-RX OK: rx_size=%d", uart_dma_rx_size);
        uart_dma_rx_state = UART_DMA_DONE;
        bes_hal_uart_transport_event_notify(uart_dma_rx_state,uart_dma_rx_size);
    }
}

static void bes_hal_uart_tx_dma_handler(uint32_t xfer_size, int dma_error)
{
    if(uart_dma_tx_state != UART_DMA_START)
    {
        return;
    }

    uart_dma_tx_size = xfer_size;
    if(dma_error)
    {
        uart_dma_tx_state = UART_DMA_ERROR;
    }
    else
    {
        uart_dma_tx_state = UART_DMA_DONE;
    }
}

static void bes_hal_uart_break_handler(void)
{
//    BES_HAL_UART_DEBUG("****** Handle break ******");

    cancel_xfer = true;
    hal_uart_stop_dma_recv(comm_uart);
    hal_uart_stop_dma_send(comm_uart);
    uart_dma_rx_state = UART_DMA_ERROR;
    bes_hal_uart_transport_event_notify(uart_dma_rx_state,uart_dma_rx_size);
}

void bes_hal_uart_recv_state_get(UART_DMA_STATE *state)
{
    *state = uart_dma_rx_state;
}

void bes_hal_uart_recv_reset(void)
{    
    union HAL_UART_IRQ_T mask;
    mask.reg = 0;
    mask.BE = 1;
    hal_uart_irq_set_mask(comm_uart, mask);
    uart_dma_rx_state = UART_DMA_IDLE;
    bes_hal_uart_transport_event_notify(uart_dma_rx_state,uart_dma_rx_size);
}

void bes_hal_uart_init(void)
{
    union HAL_UART_IRQ_T mask;
    
    if (!uart_opened) {
    #if UART_OTA_PORT_SEL == UART_OTA_PORT_SAME_SYS_DEBUG
        hal_iomux_set_uart0();
    #else
        bes_hal_i2c_close();
        hal_iomux_set_uart1();
    #endif
        hal_uart_open(comm_uart, &uart_cfg);
        mask.reg = 0;
        mask.BE = 1;
        mask.FE = 1;
        mask.OE = 1;
        mask.PE = 1;        
        mask.RT = 1;
        hal_uart_irq_set_dma_handler(comm_uart, bes_hal_uart_rx_dma_handler, bes_hal_uart_tx_dma_handler, bes_hal_uart_break_handler);
        hal_uart_irq_set_mask(comm_uart, mask);
        uart_opened = true;
    }

    cancel_xfer = false;

    hal_uart_flush(comm_uart,0);
//    BES_HAL_UART_DEBUG("%s: uart init",__func__);
}

void bes_hal_uart_reinit(void)
{
    union HAL_UART_IRQ_T mask;
    struct HAL_UART_CFG_T comm_uart_cfg;
    #if UART_OTA_PORT_SEL == UART_OTA_PORT_SAME_SYS_DEBUG
        hal_iomux_set_uart0();
    #else
        bes_hal_i2c_close();
        hal_iomux_set_uart1();
    #endif
    uart_opened = false;
    memcpy(&comm_uart_cfg, &uart_cfg, sizeof(comm_uart_cfg));
    hal_uart_open(comm_uart, &comm_uart_cfg);
    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;       
    mask.RT = 1; 
    hal_uart_irq_set_dma_handler(comm_uart, bes_hal_uart_rx_dma_handler, bes_hal_uart_tx_dma_handler, bes_hal_uart_break_handler);
    hal_uart_irq_set_mask(comm_uart, mask);
    uart_opened = true;

    cancel_xfer = false;

    hal_uart_flush(comm_uart,0);
}

void bes_hal_uart_deinit(void)
{
    if(uart_opened)
        hal_uart_close(comm_uart);
    uart_opened = 0;
}

void bes_hal_uart_set_send_timeout(uint32_t timeout)
{
    send_timeout = MS_TO_TICKS(timeout);
}

/* // using tx dma
static int bes_hal_uart_send_data(const unsigned char *buf, unsigned int len)
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

    hal_uart_flush(comm_uart,0);
    if (sent != len) {
        return 1;
    }

    return 0;
}
*/

int bes_hal_uart_send(const unsigned char *buf, unsigned int len)
{
    if (cancel_xfer) {
        return -1;
    }

//    return bes_hal_uart_send_data(buf, len);
    return bes_hal_uart_send_dma((unsigned char *)buf,len);
}

int bes_hal_uart_send_dma(const unsigned char *buf, unsigned int len)
{
    int ret = 0;
    uint32_t start;
    start = hal_sys_timer_get();

    while(uart_dma_tx_state == UART_DMA_START)
    {
        osDelay(1);
        if(hal_sys_timer_get() - start >= send_timeout)
            return -1;
    }
    uart_dma_tx_state = UART_DMA_START;
    uart_dma_tx_size = 0;

    ret = hal_uart_dma_send(comm_uart,buf,len,NULL,NULL);

    while(uart_dma_tx_state != UART_DMA_ERROR && uart_dma_tx_state != UART_DMA_DONE)
    {
        osDelay(1);
        if(hal_sys_timer_get() - start >= send_timeout)
        {
            uart_dma_tx_state = UART_DMA_ERROR;
            return -1;
        }
    }
    
#if UART_OTA_PORT_SEL == UART_OTA_PORT_SAME_SYS_DEBUG
// if uart ota proess has log printf to pc, we need add this delay, if close the log, we can del this delay
//    osDelay(10);
#endif
    return ret;
}

static int bes_hal_uart_recv_data_from_dma(unsigned char *buf, unsigned int len, unsigned int expect)
{
    int ret;
    union HAL_UART_IRQ_T mask;
    struct HAL_DMA_DESC_T dma_desc[17];
    unsigned int desc_cnt = ARRAY_SIZE(dma_desc);

    if (uart_dma_rx_state != UART_DMA_IDLE) {
        ret = -3;
        goto _no_state_exit;
    }

    uart_dma_rx_state = UART_DMA_START;
    uart_dma_rx_size = 0;

    ret = hal_uart_dma_recv(comm_uart, buf, expect, &dma_desc[0], &desc_cnt);
    if (ret) {
        uart_dma_rx_state = UART_DMA_ERROR;
        goto err_exit;
    }

    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    mask.RT = 1;
    hal_uart_irq_set_mask(comm_uart, mask);
    bes_hal_uart_transport_event_notify(uart_dma_rx_state,uart_dma_rx_size);
_no_state_exit:
    return ret;
err_exit:    
    
    return -1;
}

int bes_hal_uart_recv(unsigned char *buf, unsigned int len)
{
    if (cancel_xfer) {
        return -1;
    }
    return bes_hal_uart_recv_data_from_dma(buf, len, len);
}

int bes_hal_uart_recv_dma(unsigned char *buf, unsigned int len, unsigned int expect)
{
    if (cancel_xfer) {
        return -1;
    }
    
    return bes_hal_uart_recv_data_from_dma(buf, len, expect);
}

static int bes_hal_uart_handle_error(void)
{
//    BES_HAL_UART_DEBUG("****** Send break ******");

    // Send break signal, to tell the peer to reset the connection
    hal_uart_break_set(comm_uart);
    osDelay(100);
    hal_uart_break_clear(comm_uart);

    return 0;
}

int bes_hal_uart_error_process(void)
{
    int ret = 0;
    uint32_t err_time;

    osDelay(200);

    if (!cancel_xfer) {
        ret = bes_hal_uart_handle_error();
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

int bes_hal_uart_cancel_input(void)
{
    hal_uart_flush(comm_uart,0);
    return 0;
}


#if OTA_PRINT_PAYLOAD_LEN_SIZE == 0
static inline int bes_hal_trace_format_va(char *buf, unsigned int size, int crlf, const char *fmt, va_list ap)
{
    int len;

    len = vsnprintf(&buf[0], size, fmt, ap);

    if (crlf) 
    {
        if(len < size)
        {
            if (len + 2 < size) {
                buf[len++] = '\r';
            }
            if (len + 1 < size) {
                buf[len++] = '\n';
            }

            buf[len] = 0;
        }
        else
        {
            buf[size] = 0;
            len = size;
        }
    }
    else
    {
        if (len < size) 
        {
            buf[len] = 0;
        }
        else
        {
            buf[size] = 0;
            len = size;
        }
    }

    
    return len;
}
#else
static inline int bes_hal_trace_format_va(char *buf, unsigned int size, int crlf, const char *fmt, va_list ap)
{
    int len, tempLen = 0;
    char tmpLenStr[5] = {0};

    len = vsnprintf(&buf[OTA_PRINT_PAYLOAD_LEN_SIZE], size, fmt, ap);
    tempLen = strlen(&buf[OTA_PRINT_PAYLOAD_LEN_SIZE]);
    len = tempLen + OTA_PRINT_PAYLOAD_LEN_SIZE;
    sprintf((char*)tmpLenStr,"%04x",(unsigned int)tempLen);
    memcpy(&buf[0],tmpLenStr,OTA_PRINT_PAYLOAD_LEN_SIZE);
    return len;
}
#endif

int bes_hal_trace_print(unsigned char lvl, const char *fmt, ...)
{
    char buf[200] = {0};
    unsigned int len;
    va_list ap;
    return 0;
    if(bes_hal_get_log_level() < lvl)
        return 0;

    memcpy(buf,OTA_PRINT_PREX_HEAD,OTA_PRINT_PREX_LEN);
    va_start(ap, fmt);
    len = bes_hal_trace_format_va(buf+OTA_PRINT_PREX_LEN, sizeof(buf)-OTA_PRINT_PREX_LEN - OTA_PRINT_PAYLOAD_LEN_SIZE, true, fmt, ap);
    va_end(ap);

    len += OTA_PRINT_PREX_LEN;
    return bes_hal_uart_send((unsigned char *)buf,len);
}


int32_t bes_hal_dump8(const void *buffer, uint32_t count)
{
    #define DUMP_BUF_LEN  (16)
    const uint8_t* pBuf = (uint8_t*)buffer;
    uint8_t buf[2*DUMP_BUF_LEN + OTA_PRINT_PREX_LEN + OTA_PRINT_PAYLOAD_LEN_SIZE];
    uint8_t tmpbuf[2];
    uint32_t i, len = 0, curPrint;
    int32_t ret = 0;
    tmpbuf[0] = '\r';
    tmpbuf[1] = '\n';
    return 0;
    while(len < count)
    {
        uint8_t tempLenStr[5] = {0};
        memset(buf,0,2*DUMP_BUF_LEN + OTA_PRINT_PREX_LEN + OTA_PRINT_PAYLOAD_LEN_SIZE);
        memcpy(buf,OTA_PRINT_PREX_HEAD,OTA_PRINT_PREX_LEN);
        if(len + DUMP_BUF_LEN <= count)
        {
            
            curPrint = DUMP_BUF_LEN;
            sprintf((char*)tempLenStr,"%04x",(unsigned int)curPrint);
            memcpy(&buf[OTA_PRINT_PREX_LEN],tempLenStr,OTA_PRINT_PAYLOAD_LEN_SIZE);
            
            for(i = 0; i < DUMP_BUF_LEN; i++)
            {
                sprintf((char*)&buf[2*i + OTA_PRINT_PREX_LEN + OTA_PRINT_PAYLOAD_LEN_SIZE],"%02x ",(unsigned int)*(pBuf+len+i));
            }
        }
        else
        {
            curPrint = count - len;
            sprintf((char*)tempLenStr,"%04x",(unsigned int)curPrint);
            memcpy(&buf[OTA_PRINT_PREX_LEN],tempLenStr,OTA_PRINT_PAYLOAD_LEN_SIZE);
            for(i = 0; i < curPrint; i++)
            {
                sprintf((char*)&buf[2*i + OTA_PRINT_PREX_LEN + OTA_PRINT_PAYLOAD_LEN_SIZE],"%02x ",(unsigned int)*(pBuf+len+i));
            }
        }
    //    TRACE("%s",buf);
        
        if(hal_trace_get_log_flag() == 1)
        {
            ret = hal_trace_output((unsigned char *)buf, curPrint*2 + OTA_PRINT_PREX_LEN + OTA_PRINT_PAYLOAD_LEN_SIZE);
            hal_trace_output((unsigned char *)tmpbuf, 2);
        }
        else
        {
            ret = bes_hal_uart_send((unsigned char *)buf,curPrint*2 + OTA_PRINT_PREX_LEN + OTA_PRINT_PAYLOAD_LEN_SIZE);
            bes_hal_uart_send((unsigned char *)tmpbuf, 2);
        }
        
        len += curPrint;
    }
    return ret;
}