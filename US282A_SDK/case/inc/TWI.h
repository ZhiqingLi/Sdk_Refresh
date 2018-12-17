/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：TWI GPIO模拟从模组控制模块宏定义，结构体定义等。
 *       NOTE!!! 接口声明请各调用模块自己声明。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _TWI_H
#define _TWI_H

#include "psp_includes.h"
#include "case_independent.h"

#if ((CASE_BOARD_TYPE == CASE_BOARD_EVB) || (CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))

/* 模拟TWI 总线GPIO 配置宏*/
#define  TWI_SCL_BIT        31
#define  TWI_SDA_BIT        15
#define  GPIO_SCL_BIT    (0x00000001<<TWI_SCL_BIT)
#define  GPIO_SDA_BIT    (0x00000001<<TWI_SDA_BIT)

#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)

/* 模拟TWI 总线GPIO 配置宏*/
#define  TWI_SCL_BIT        0
#define  TWI_SDA_BIT        1
#define  GPIO_SCL_BIT    (0x00000001<<TWI_SCL_BIT)
#define  GPIO_SDA_BIT    (0x00000001<<TWI_SDA_BIT)

#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)

/* 模拟TWI 总线GPIO 配置宏*/
#define  TWI_SCL_BIT        15
#define  TWI_SDA_BIT        31
#define  GPIO_SCL_BIT    (0x00000001<<TWI_SCL_BIT)
#define  GPIO_SDA_BIT    (0x00000001<<TWI_SDA_BIT)

#endif

/*TWI GPIO 定义结构 */
typedef struct
{
    uint32 reg_in; //输入使能寄存器
    uint32 reg_out; //输出使能寄存器
    uint32 reg_data; //数据寄存器
    uint32 reg_bit; //bit 位
} gpio_init_cfg_t;

#endif /*_TWI_H */

