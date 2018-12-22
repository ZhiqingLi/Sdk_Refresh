#ifndef __API_UART_H__
#define __API_UART_H__

enum {
    HUART_TR_PA7    = 0,
    HUART_TR_PB2,
    HUART_TR_PB3,
    HUART_TR_PE7,
    HUART_TR_PA1,
    HUART_TR_PA6,
    HUART_TR_PB1,
    HUART_TR_PB4,
    HUART_TR_PE6,
    HUART_TR_PA0,
};

void huart_init(void);
void huart_init_do(u8 tx_port, u8 rx_port, u32 baud_rate, u8 *buf, u16 buf_size);
void huart_setbaudrate(uint baudrate);
void huart_putchar(const char ch);
void huart_tx(const void *buf, uint len);
uint huart_get_rxcnt(void);
void huart_rxfifo_clear(void);
char huart_getchar(void);
void huart_exit(void);

#endif // __API_UART_H__
