/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：RCP 命令结构体定义。
 * 作者：cailizhen
 ********************************************************************************/
 
#ifndef _RCP_CONTROL_H
#define _RCP_CONTROL_H

#include "psp_includes.h"

#define RCP_MAGIC_SOH            (0x01)
#define RCP_MAGIC_SOH_R          (0xfe)
#define RCP_CHAN_MASK            (0x03 << 0)
#define RCP_CHAN_SPP             (0x00 << 0)
#define RCP_CHAN_I2C_SPI_UART    (0x01 << 0)
#define RCP_CHAN_GPIO            (0x02 << 0)
#define RCP_SEGMENT_MASK         (0x03 << 2)
#define RCP_SEGMENT_NONE         (0x00 << 2)
#define RCP_SEGMENT_FIRST        (0x01 << 2)
#define RCP_SEGMENT_CONT         (0x02 << 2)
#define RCP_SEGMENT_LAST         (0x03 << 2)
#define RCP_ACKFLAG_MASK         (0x01 << 4)
#define RCP_ACKFLAG_ACK          (0x01 << 4)
#define RCP_SEQ_MASK             (0x0f << 0)

/*! RCP命令结构体 */
typedef struct
{
    /*! 0x01, 0xfe ,ASCII码的SOH start of head 及其反码 */
    uint8 magic[2];

    /*! 第一个字节的一些特殊标志：
     *    bit0-1 表示通道模式，00表示SPP，01表示I2C或SPI或UART通道，10表示GPIO通道，11未定义。
     *    bit2-3 表示数据包位置，00表示不用分包，01表示某个命令的第一个数据包，10表示后续但非最后一个包，
     *           11表示最后一个包。
     *    bit4   ACK标志，1表示需要应答或ACK才能继续发送命令包。
     *    bit5-7 未定义，应该初始化为0。
     *  第二个字节暂时不用。
     */
    uint8 flags[2];

    /*! 命令类型
     * 【1】 'P'：应用切换类 (0x50)
     * 【2】 'C'：控制类     (0x43)
     * 【3】 'K'：按键模拟类 (0x4B)
     * 【4】 'Q'：查询类     (0x51)
     * 【5】 'A'：查询应答类 (0x41)
     * 【6】 'S'：参数设置类 (0x53)
     * 【7】 'R': ACK/NACK类 (0x52)
     */
    uint8 cmd_type;

    /*! 命令ID号 */
    uint8 cmd_id;
    /*! 命令长度，命令头+参数长度，包括magic和flags，但不包括checksum */
    uint16 cmd_len;
    /*! 命令参数1 */
    uint32 param1;
    /*! 命令参数2 */
    uint32 param2;
} rcp_command_t;

#endif
