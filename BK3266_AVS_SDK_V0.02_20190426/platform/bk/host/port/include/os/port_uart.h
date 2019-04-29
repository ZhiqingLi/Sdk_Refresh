/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _PORT_TTY_H_
#define _PORT_TTY_H_

typedef struct {
    /* Write bytes into port uart TTY, not all bytes are guaranteed to be
     * written.
     * 'write' is not allowed to lose safe context */
    result_t (*write)(j_device_h, const uint8_t *, uint32_t, uint32_t *);
} port_uart_creator_ops_t;

typedef struct {
    /* 'input' - will be called when a new buffer is a available for the owner
     * to receive */
    void (*input)(j_device_h, const uint8_t *, uint32_t);
    /* 'tx_wakeup' - will be called when the creator can accept more data from
     * the owner.
     * The creator may chose not to call this method, and in such case it must
     * guarantee that 'write' ('port_uart_creator_ops_t') either successfully
     * transfers all data or fails */
    void (*tx_wakeup)(j_device_h);
} port_uart_owner_ops_t;

#endif

