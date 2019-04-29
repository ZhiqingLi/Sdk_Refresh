#ifndef _DRIVER_SERIO_H_
#define _DRIVER_SERIO_H_

#define UART_TX_POLL_MODE
#define UART_DEBUG_TX_POLL_MODE

#define DEF_TX_EN                   0x1
#define DEF_RX_EN                   0x1
#define DEF_IRDA_MODE               0x0    // 0:uart mode  1:IRDA MODE
#define DEF_DATA_LEN                0x3  // 0=5bit, 1=6bit, 2=7bit, 3=8bit
#define DEF_PARITY_EN               0x0  // 0=no parity  1: enable parity
#define DEF_PARITY_MODE             0x0  // 0:odd  1: even
#define DEF_STOP_BIT                0x0  // 1bit

#define UART_SYS_CLK                26000000 //For FPGA

#define DEF_BAUD_RATE	            115200
#define DEF_DEBUG_BAUD_RATE	        115200

#define DEF_TX_FIFO_THRE            64
#define DEF_RX_FIFO_THRE            64
#define DEF_RX_TIMEOUT              0
#define RX_BUF_LEN                  1536
#define TX_BUF_LEN                  512

#define TX_BUF_DEBUG_LEN            384

#define TX_FIFO_MAX_COUNT           128
#define DEBUG_TX_FIFO_MAX_COUNT     16

void controller_read_virtual_uart(uint8 ch);
#define host_write_virtual_uart(ch)  (controller_read_virtual_uart(ch))
uint16 uart_send_poll( uint8 * buff, int len );

#endif // _SERIO_H_
