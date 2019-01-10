#ifndef __USB_CDC_H__
#define __USB_CDC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

enum USB_SERIAL_API_MODE {
    USB_SERIAL_API_NONBLOCKING,
    USB_SERIAL_API_BLOCKING,
};

enum USB_SERIAL_STATE_EVENT_T {
    USB_SERIAL_STATE_RESET, // RESET event should be processed as quickly as possible
    USB_SERIAL_STATE_DISCONNECT,
    USB_SERIAL_STATE_SLEEP,
    USB_SERIAL_STATE_WAKEUP,
    USB_SERIAL_STATE_CONFIG,
};

typedef void (*USB_SERIAL_STATE_CALLBACK)(enum USB_SERIAL_STATE_EVENT_T event);
typedef void (*USB_SERIAL_BREAK_CALLBACK)(void);
typedef void (*USB_SERIAL_XFER_CALLBACK)(const uint8_t *data, uint32_t size, int error);

struct USB_SERIAL_CFG_T {
    USB_SERIAL_STATE_CALLBACK state_callback;
    USB_SERIAL_BREAK_CALLBACK break_callback;
    enum USB_SERIAL_API_MODE mode;
};

int usb_serial_open(const struct USB_SERIAL_CFG_T *cfg);

int usb_serial_reopen(void (*break_handler)(void));

void usb_serial_close(void);

int usb_serial_ready(void);

int usb_serial_connected(void);

void usb_serial_init_xfer(void);

void usb_serial_cancel_recv(void);

int usb_serial_flush_recv_buffer(void);

int usb_serial_recv(uint8_t *buf, uint32_t size);

int usb_serial_direct_recv(uint8_t *dma_buf, uint32_t size, uint32_t expect, uint32_t *recv,
                           int zlp, USB_SERIAL_XFER_CALLBACK callback);

void usb_serial_cancel_send(void);

int usb_serial_send(const uint8_t *buf, uint32_t size);

int usb_serial_direct_send(const uint8_t *dma_buf, uint32_t size, int zlp, USB_SERIAL_XFER_CALLBACK callback);

int usb_serial_send_break(void);

#ifdef __cplusplus
}
#endif

#endif

