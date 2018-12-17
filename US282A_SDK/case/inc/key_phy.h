/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：物理 GUI（按键，红外遥控，TK）消息相关的宏定义，枚举类型，结构体定义等。
 *       物理 GUI 消息从按键驱动发出，在COMMON层进行分析转换为逻辑 GUI 消息。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _KEY_PHY_H_
#define _KEY_PHY_H_

#include "psp_includes.h"

/*!
 * \brief
 *  key_type_e 物理按键类型
 */
typedef enum
{
    /*! 普通按键 */
    KEY_TYPE_KEY = 0x00,
    /*! IR红外遥控 */
    KEY_TYPE_IR = 0x01,
    /*! 触摸按键 */
    KEY_TYPE_TK = 0x02,
} key_phy_type_e;

typedef enum
{
    /*! 起始按键 */
    KEY_PHY_DOWN = 0x00,
    /*! 后续按键 */
    KEY_PHY_CONT = 0x01,
    /*! 抬起按键 */
    KEY_PHY_UP = 0x02,
} key_phy_status_e;

//特殊KEY ID
#define NO_KEY          0xff
#define KEY_Hold        0x30
#define KEY_UnHold      0x31
#define KEY_PHY_PLAY    0x32
#define KEY_UP          0x80

//IR遥控器编码类型
#define IR_CODE_FF00    0
#define IR_CODE_BF00    1
#define IR_CODE_7F80    2
#define IR_CODE_C33C    3
#define IR_CODE_MAX     4

#define IR_KEY_MAX      21

/*!
 * \brief  
 *  key_phy_msg_t 物理按键消息
 */
typedef struct
{
    uint8 key_type :3;
    uint8 key_status :2;
    uint8 key_ir_code :2;
    uint8 key_reserve :1;
    uint8 key_id;/*0开始，0x80表示抬起，0xff表示无按键*/
    uint16 key_timestamp;
} key_phy_msg_t;

#endif
