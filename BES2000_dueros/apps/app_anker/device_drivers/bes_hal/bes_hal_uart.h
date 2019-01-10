#ifndef __BES_HAL_UART_H__
#define __BES_HAL_UART_H__

#include "plat_types.h"
#include "hal_trace.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UART_OTA_PORT_SAME_SYS_DEBUG     0
#define UART_OTA_PORT_DIFF_SYS_DEBUG     1
#define UART_OTA_PORT_SEL                UART_OTA_PORT_SAME_SYS_DEBUG//UART_OTA_PORT_DIFF_SYS_DEBUG

typedef enum UART_DMA_STATE_T_{
    UART_DMA_IDLE,
    UART_DMA_START,
    UART_DMA_DONE,
    UART_DMA_ERROR,
}UART_DMA_STATE;

/* because ble uart transport module should receive rx dma data in app_thread,
 so bes_hal_uart_transport_init should be called after app_os_init is called. */
int bes_hal_uart_transport_init(void);

void bes_hal_uart_transport_event_notify(UART_DMA_STATE rxDmaState, uint32_t rxDmaSize);

void bes_hal_uart_recv_state_get(UART_DMA_STATE *state);
void bes_hal_uart_recv_reset(void);
void bes_hal_uart_init(void);
void bes_hal_uart_reinit(void);
void bes_hal_uart_deinit(void);
void bes_hal_uart_set_send_timeout(uint32_t timeout);
int bes_hal_uart_send(const unsigned char *buf, unsigned int len);
int bes_hal_uart_send_dma(const unsigned char *buf, unsigned int len);
int bes_hal_uart_recv(unsigned char *buf, unsigned int len);
int bes_hal_uart_recv_dma(unsigned char *buf, unsigned int len, unsigned int expect);
int bes_hal_uart_error_process(void);
int bes_hal_uart_cancel_input(void);
int bes_hal_uart_transport_init(void);
void bes_hal_set_log_level(unsigned char lvl);
unsigned char bes_hal_get_log_level(void);
int bes_hal_trace_print(unsigned char lvl, const char *fmt, ...);
int32_t bes_hal_dump8(const void *buffer, uint32_t count);
#define BES_LOG_LVL_DUEBU       5
#define BES_LOG_LVL_INFO        4
#define BES_LOG_LVL_WARING      3
#define BES_LOG_LVL_ERORR       2
#define BES_LOG_LVL_FATEL       1
#define BES_LOG_LVL_OFF         0

#if defined(DEBUG)
#define BES_HAL_UART_DEBUG(str, ...) \
if(hal_trace_get_log_flag() == 1) \
hal_trace_printf(str, ##__VA_ARGS__); \
else \
bes_hal_trace_print(BES_LOG_LVL_DUEBU, str, ##__VA_ARGS__);

#define BES_HAL_UART_INFO(str, ...) \
if(hal_trace_get_log_flag() == 1) \
hal_trace_printf(str, ##__VA_ARGS__); \
else \
bes_hal_trace_print(BES_LOG_LVL_INFO, str, ##__VA_ARGS__);

#define BES_HAL_UART_WARNING(str, ...) \
if(hal_trace_get_log_flag() == 1) \
hal_trace_printf(str, ##__VA_ARGS__); \
else \
bes_hal_trace_print(BES_LOG_LVL_WARING, str, ##__VA_ARGS__);

#define BES_HAL_UART_ERROR(str, ...) \
if(hal_trace_get_log_flag() == 1) \
hal_trace_printf(str, ##__VA_ARGS__); \
else \
bes_hal_trace_print(BES_LOG_LVL_ERORR, str, ##__VA_ARGS__);

#define BES_HAL_UART_FATEL(str, ...) \
if(hal_trace_get_log_flag() == 1) \
hal_trace_printf(str, ##__VA_ARGS__); \
else \
bes_hal_trace_print(BES_LOG_LVL_FATEL, str, ##__VA_ARGS__);

#define BES_HAL_DUMP8(buf, cnt)                 DUMP8("%02x", buf, cnt)

#else
#define BES_HAL_UART_DEBUG(str, ...)            hal_trace_dummy(str, ##__VA_ARGS__)
#define BES_HAL_UART_INFO(str, ...)             hal_trace_dummy(str, ##__VA_ARGS__)
#define BES_HAL_UART_WARNING(str, ...)          hal_trace_dummy(str, ##__VA_ARGS__)
#define BES_HAL_UART_ERROR(str, ...)            hal_trace_dummy(str, ##__VA_ARGS__)
#define BES_HAL_UART_FATEL(str, ...)            hal_trace_dummy(str, ##__VA_ARGS__)
#define BES_HAL_DUMP8(buf, cnt)                 bes_hal_dump8(buf, cnt)
#endif

#ifdef __cplusplus
}
#endif
#endif