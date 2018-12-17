
#ifndef _UART_RECORDKEY_H			//防止重定义
#define _UART_RECORDKEY_H

#include "key_inner.h"
#define HOSC_CLOCK    26000000  // 26 MHz
#define UART_RECORDKEY  1            //串口录制按键开关

typedef struct
{ 
    uint8 record_key_type;
    uint8 record_key_id;
    uint32 record_key_time;
}key_record_t;

extern uint32 atoitok(const char *src, int addr, int lenth);
extern uint8 record_key_play(uint8 key_val);
extern void key_data_check(uint8 rx_buf[17]);
extern uint8 uart1_receive_deal(void);
extern void uart1_process(void);
extern void reocrdkey_print(void);
extern void get_keytime(void);
extern void get_keyval(uint8 keyval);


#endif/*_UART_RECORDKEY_H*/

