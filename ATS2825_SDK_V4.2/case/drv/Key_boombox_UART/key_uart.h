#ifndef _KEY_UART_H_
#define _KEY_UART_H_

//#include "key_inner.h"
//#include "psp_includes.h"
#include "key_interface.h"

#define WHOLECHIP_SIM
//#define K_PLL_FREQ        36
#define K_MCUPLL_VALUE    (0x40|(K_PLL_FREQ/6))
//#define I2C_SPEED         0x00   // 00: 100k;  32=0x20:400K

#define K_VARBUF_SIZE     0x21 //物理传输最大数据包长度
//读写buffer 大小
#define PYH_WRBUF_SIZE 8

///////////////////////////////////////////////////////////////////////////////////
#define PASSED     0
#define FAILED     1

//logical communication finite state machine
#define COMUNICATE_IDLE               0x00 /*0表示空闲状态*/
#define COMUNICATE_BUSY               0x01 /*1表示忙状态*/
#define COMUNICATE_REPLY              0x02 /*2表示I2C从机开始应答，长应答会带第一个数据包*/
#define COMUNICATE_COMND              0x03 /*3表示接收命令成功*/
#define COMUNICATE_FAIL               0x04 /*4表示I2C从机接收失败*//*暂时不用*/
//#define COMUNICATE_GET_RET_DATA       0x05 /*5表示I2C主机正在读数据，包括命令和数据*/
#define COMUNICATE_COM_CONT           0x06 /*6表示I2C从机接收写命令的不定长参数*/
#define COMUNICATE_WAIT_ACK           0x07 /*7表示I2C从机应答数据后等待主机ACK*/

//uart bus finite state machine
#define UART_IDLE                    0x00 /*0表示空闲状态*/

#define UART_RECEIVE                 0x01 /*5表示正在读数据，包括命令和数据*/

#define UART_SEND                    0x02 /*2表示发送状态*/

#define UART_BUSY                    0x03 /*表示处理接收到的数据*/

///////////////////////////////////////////////////////////////////////////////////
//typedef volatile unsigned int* REG32;
//typedef volatile unsigned short* REG16;
//typedef volatile unsigned char* REG8;

typedef enum {
    SEND_CMD_DATA,
    SEND_CMD_START,
    SEND_CMD_END,
    SEND_VALID_FLAG,
    SEND_INVALID_FLAG
} send_packet_e;

typedef enum {
    SEND_STATUS_START,
    SEND_STATUS_CONT,
    SEND_STATUS_FLAG,
    SEND_STATUS_END,
    SEND_STATUS_MAX
} send_status_e;

//cmd status
typedef enum {
    RECEIVE_STATUS_IDLE,
    RECEIVE_STATUS_START
    //RECEIVE_STATUS_CONT,
    //RECEIVE_STATUS_END
} receive_status_e;

void uart1_int_process(void) __FAR__;
void uart1_init(void) __FAR__;

extern uint8 g_uart_rx_buf[PYH_WRBUF_SIZE]; //uart接收物理通信buffer
extern uint8 g_uart_tx_buf[PYH_WRBUF_SIZE]; //uart发送物理通信buffer
extern uint8 g_receive_buf[K_VARBUF_SIZE];
extern uint8 g_transmit_buf[MAX_SEGMENT_LEN]; //应答数据buffer
extern uint8 *g_read_ptr;//读指针
extern uint8 *g_write_ptr;//写指针

extern uint16 long_answer_len; //应答不定长参数长度
extern bool long_answer_reply; //正在应答不定长参数
extern uint8 *pmasterdata; //接收命令不定长参数指针
extern uint8 *pmasterdata_head; //接收命令指针头，用于最终发送RCP对象传参
extern uint16 long_command_len; //接收命令不定长参数长度
extern bool long_command_recv; //正在接收长命令不定长参数
//extern spp_command_t cur_spp_cmd; //当前接收到的命令体
//extern spp_command_t reply_spp_cmd; //应答命令体

extern uint16 g_rw_index; //读写数据序号
extern uint8 g_bus_status; //总线类型，分为写命令，读数据，读处理状态，和空闲

extern send_status_e g_cmd_status; //命令处理状态，空闲状态，忙状态，有应答成功状态，无应答成功状态，

extern uint8 g_retry_times; //重传次数
extern uint8 g_send_error;//出错标志

extern uint16 g_send_data_len;

extern uint8 g_send_cont;

extern void uart1_transmit_deal(send_packet_e mode, uint8 send_len) __FAR__;

extern void store_command_data(void) __FAR__;

extern void load_answer_data(void) __FAR__;

extern uint8 get_communicate_status(void) __FAR__;

extern void set_communicate_status(uint8 status) __FAR__;

extern uint8 get_send_status(void) __FAR__;

extern void set_send_status(uint8 status) __FAR__;

#endif //_KEY_UART_H_
