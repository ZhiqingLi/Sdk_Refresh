#ifndef __KNOWLES_MIC_COMMUNICATION_H__
#define __KNOWLES_MIC_COMMUNICATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include "stdio.h"

#define TRACE(str, ...)                 do { printf("%s/" str "\n", __FUNCTION__, __VA_ARGS__); } while (0)
#define ASSERT(cond, str, ...)          \
    do { if (!(cond)) { printf("[ASSERT]%s/" str, __FUNCTION__, __VA_ARGS__); while (1); } } while (0)
#define TRACE_TIME(str, ...)            TRACE(str, __VA_ARGS__)

#else
#include "hal_trace.h"
#include "hal_timer.h"

#define TRACE_TIME(str, ...)            TRACE("[%05u] " str, TICKS_TO_MS(hal_sys_timer_get()), ##__VA_ARGS__)
#endif

enum DOWNLOAD_TRANSPORT {
    TRANSPORT_USB,
    TRANSPORT_UART,
};

enum XFER_TIMEOUT {
    XFER_TIMEOUT_SHORT,
    XFER_TIMEOUT_MEDIUM,
    XFER_TIMEOUT_LONG,
    XFER_TIMEOUT_IDLE,

    XFER_TIMEOUT_QTY
};

enum UART_DMA_STATE {
    UART_DMA_IDLE,
    UART_DMA_START,
    UART_DMA_DONE,
    UART_DMA_ERROR,
};

void smart_mic_init_transport();

void smart_mic_reinit_transport(unsigned int bps,unsigned char stop_bits);

void smart_mic_set_send_timeout(unsigned int timeout);
void smart_mic_set_recv_timeout(unsigned int timeout);


int smart_mic_send_data(unsigned char *buf, unsigned int len);
int smart_mic_recv_data(unsigned char *buf, unsigned int len,unsigned int *rlen);


void smart_mic_recv_data_state_get(enum UART_DMA_STATE *state);
void smart_mic_recv_data_reset(void);
int smart_mic_handle_error(void);
int smart_mic_cancel_input(void);




#ifdef __cplusplus
}
#endif

#endif

