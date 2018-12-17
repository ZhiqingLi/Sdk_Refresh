#ifndef _STUB_INTERFACE_H
#define _STUB_INTERFACE_H

#include "typeext.h"
#include "kernel_interface.h"

#define OT_10MS_132MHZ  1200
#define OT_500MS_72MHZ  300000
#define OT_500MS_132MHZ 600000

#define STUB_DELAY_100MS    (100)
#define STUB_DELAY_200MS    (100)
#define STUB_DELAY_500MS    (500)
#define STUB_DELAY_1000MS   (1000)

#define STUB_TRANS_EXT_CMD_HEADER_LEN  (8)
//trans command
typedef struct
{
    uint8 type;
    uint8 opcode;
    uint8 sub_opcode;
    uint8 reserv;
    uint32 data_len;
} stub_trans_cmd_t;

typedef struct
{
    uint8 type;
    uint8 opcode;
    uint8 sub_opcode;
    uint8 reserved;
    uint16 payload_len;
}stub_ext_cmd_t;

typedef struct
{
    uint8 *rw_buffer;
    uint16 read_len;
} stub_rw_param_t;

/*! \cond MMM_ID_API*/
/*!
 *  \brief
 *     提供给应用层的stub命令
 */
typedef enum
{
    /*! 打开stub */
    STUB_OP_OPEN = 0,
    /*! 关闭stub */
    STUB_OP_CLOSE = 1,
    /*! set param*/
    STUB_OP_IOCTL = 2,
    /*! read */
    STUB_OP_READ = 3,
    /*! write */
    STUB_OP_WRITE = 4,
    /*! read write ext */
    STUB_OP_EXT_RW = 5,  
    /*! read write raw */
    STUB_OP_RAW_RW      
} stub_cmd_t;

typedef enum
{
    STUB_USE_URAM0 = 0,
    STUB_USE_URAM1	
} stub_uram_type_e;

typedef enum
{
    STUB_PHY_USB,
    STUB_PHY_UART
} stub_phy_interface_e;


typedef enum
{
    SET_TIMEOUT = 0, 
    RESET_FIFO = 1,
    SWITCH_URAM = 2,
    SET_STATUS = 3,
    GET_CONNECT_STATE = 4,
    SET_LED_STATE = 5,
    SET_READ_MODE = 6,
    SET_DEBUG_MODE = 7,
    GET_PHY_INTERFACE = 8,
    CLR_RECV_DATA = 9,
    SET_IN_TIMEOUT = 10,
} stub_ioctl_t;

typedef struct
{
    uint16 opcode;
    uint16 payload_len;
    uint8 *rw_buffer;
} stub_ext_param_t;


#define STUB_COMMAND_HEAD_SIZE  8

extern int stub_mod_cmd(void *, void *, void *, int) __FAR__;

/*启动stub,设备与pc连接*/
/*int stub_init(bool need_uninstall_led)*/
#define stub_open(a)  (int)stub_mod_cmd((void*)(a), (void*)(0), (void*)(0), STUB_OP_OPEN)

/*关闭stub,设备与pc断开连接*/
/*int stub_exit(void)*/
#define stub_close() (int)stub_mod_cmd((void*)(0), (void*)(0), (void*)(0), STUB_OP_CLOSE)

/*读取数据pc发送的数据*/
/*int stub_read(uint8 opcode, uint8 *data_buffer, uint32 data_len)*/
/*opcode：协议命令字;data_buffer：存放数据的buffer指针;data_len：数据长度*/
#define stub_get_data(a, b, c)  (int)stub_mod_cmd((void*)(a), (void*)(b), (void*)(c), STUB_OP_READ)

/*向pc发送数据*/
/*int stub_write(uint8 opcode, uint8 *data_buffer, uint32 data_len)*/
/*opcode：协议命令字;data_buffer：存放数据的buffer指针;data_len：数据长度*/
#define stub_set_data(a, b, c)  (int)stub_mod_cmd((void*)(a), (void*)(b), (void*)(c), STUB_OP_WRITE)

/*预留扩展*/
/*int stub_ioctl(uint8 op_code, void *param1, void *param2)*/
#define stub_ioctrl_set(a, b, c) (int)stub_mod_cmd((void*)(a), (void*)(b), (void*)(c), STUB_OP_IOCTL)

/*读取数据pc发送的数据*/
/*int stub_read(uint8 opcode, uint8 *data_buffer, uint32 data_len)*/
/*opcode：协议命令字;data_buffer：存放数据的buffer指针;data_len：数据长度*/
#define stub_ext_read(a)  (int)stub_mod_cmd((void*)(a), (void*)(0), (void*)(0), STUB_OP_EXT_RW)

/*读取数据pc发送的数据*/
/*int stub_read(uint8 opcode, uint8 *data_buffer, uint32 data_len)*/
/*opcode：协议命令字;data_buffer：存放数据的buffer指针;data_len：数据长度*/
#define stub_ext_write(a)  (int)stub_mod_cmd((void*)(a), (void*)(1), (void*)(0), STUB_OP_EXT_RW)

/*读取数据pc发送的数据*/
/*int stub_read(uint8 opcode, uint8 *data_buffer, uint32 data_len)*/
/*opcode：协议命令字;data_buffer：存放数据的buffer指针;data_len：数据长度*/
#define stub_raw_read(a,b)  (int)stub_mod_cmd((void*)(a), (void*)(b), (void*)(0), STUB_OP_RAW_RW)

/*读取数据pc发送的数据*/
/*int stub_read(uint8 opcode, uint8 *data_buffer, uint32 data_len)*/
/*opcode：协议命令字;data_buffer：存放数据的buffer指针;data_len：数据长度*/
#define stub_raw_write(a,b)  (int)stub_mod_cmd((void*)(a), (void*)(b), (void*)(1), STUB_OP_RAW_RW)

/* 获取连接状态; a: 检测连接标志, 0: 返回上次连接状态, 1: 重新检测连接状态;
   返回值 <= 0: 未连接, > 0: 已连接 (返回 UART_RX1: A21 或 A16). */
#define stub_get_connect_state(a)  (int)stub_mod_cmd((void*)GET_CONNECT_STATE, (void*)(a), (void*)(0), STUB_OP_IOCTL)

/* 设置 LED 指示灯状态 (每一位表示一个指示灯); a: LED 指示灯编号掩码, b: 状态 . */
#define stub_set_led_state(a,b)  (int)stub_mod_cmd((void*)SET_LED_STATE, (void*)(a), (void*)(b), STUB_OP_IOCTL)

/* 设置调试模式; 1: 允许调试打印, 0: 禁止调试打印. */
#define stub_set_debug_mode(a)  (int)stub_mod_cmd((void*)SET_DEBUG_MODE, (void*)(a), (void*)(0), STUB_OP_IOCTL)

#endif
