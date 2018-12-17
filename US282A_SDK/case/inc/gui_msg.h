/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：逻辑 GUI（按键，红外遥控，TK）消息相关的宏定义，枚举类型，结构体定义等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _gui_msg_H_
#define _gui_msg_H_

#include "psp_includes.h"
#include "key_phy.h"

/*!
 * \brief
 *  input_msg_type_e 输入型消息类型，也叫 gui 消息类型
 */
typedef enum
{
    /*! 普通按键 */
    INPUT_MSG_KEY = 0x00,
} input_msg_type_e;

/*!
 * \brief
 *  key_value_e 逻辑按键枚举类型
 */
typedef enum
{
    /*! 空按键 */
    KEY_NULL = 0x00,
    /*! 播放/暂停按键 */
    KEY_PLAY = 0x01,
    /*! 上一项（左）按键 */
    KEY_PREV = 0x02,
    /*! 下一项（右）按键 */
    KEY_NEXT = 0x03,
    /*! 音量调节快捷按键 */
    KEY_VOL = 0x04,
    /*! 音量加快捷按键 */
    KEY_VADD = 0x05,
    /*! 音量减快捷按键 */
    KEY_VSUB = 0x06,
    /*! 菜单/选项按键 */
    KEY_MODE = 0x07,
    /*! eq按键 */
    KEY_EQ = 0x08,
    /*! 循环模式按键 */
    KEY_REPEAT = 0x09,
    /*! mute按键 */
    KEY_MUTE = 0x0a,
    /*! clock按键 */
    KEY_CLK = 0x0b,
    /*! folder+按键 */
    KEY_FOLDER_ADD = 0x0c,
    /*! folder-按键 */
    KEY_FOLDER_SUB = 0x0d,
    /*!next_vol_add复用按键*/
    KEY_NEXT_VOLADD = 0x0e,
    /*!prev_vol_sub复用按键*/
    KEY_PREV_VOLSUB = 0x0f,
    /*! 数字0按键 */
    KEY_NUM0 = 0x10,
    /*! 数字1按键 */
    KEY_NUM1 = 0x11,
    /*! 数字2按键 */
    KEY_NUM2 = 0x12,
    /*! 数字3按键 */
    KEY_NUM3 = 0x13,
    /*! 数字4按键 */
    KEY_NUM4 = 0x14,
    /*! 数字5按键 */
    KEY_NUM5 = 0x15,
    /*! 数字6按键 */
    KEY_NUM6 = 0x16,
    /*! 数字7按键 */
    KEY_NUM7 = 0x17,
    /*! 数字8按键 */
    KEY_NUM8 = 0x18,
    /*! 数字9按键 */
    KEY_NUM9 = 0x19,
    /*! 泛指数字按键 */
    KEY_NUMBER = 0x1a,
    /*! FM搜索按键 */
    MSG_SCAN = 0x1b,
    /*!开关机按键*/
    KEY_POWER = 0x1c,
    /*!for ab/record key*/
    KEY_ABREC = 0x1d,
    /*!Phone 键*/
    KEY_PHONE = 0x1e,
    /*!Phone/Play 键*/
    KEY_PHONEPLAY = 0x1f,
} key_value_e;

/*!
 * \brief
 *  key_type_e 按键类型
 */
typedef enum
{
    /*! 没有按键*/
    KEY_TYPE_NULL = 0x00,
    /*! 双击按键*/
    KEY_TYPE_DBL_CLICK = 0x20,
    /*! 按键按下*/
    KEY_TYPE_DOWN = 0x10,
    /*! 按键长按*/
    KEY_TYPE_LONG = 0x08,
    /*! 按键保持*/
    KEY_TYPE_HOLD = 0x04,
    /*! 按键短按弹起*/
    KEY_TYPE_SHORT_UP = 0x02,
    /*! 按键长按弹起*/
    KEY_TYPE_LONG_UP = 0x01,
    /*! 任意按键类型 */
    KEY_TYPE_ALL = 0x3f,
    /*!超长按*/    
    KEY_TYPE_LONG_10S = 0x40,
} key_type_e;

typedef enum
{
    KEY_DEAL_NULL, //不做额外处理
    KEY_DEAL_FILTER, //过滤后续按键
    KEY_DEAL_FILTER_UP, //仅过滤抬起按键
    KEY_DEAL_ITSELF,//过滤自身按键，防止超长按多次重复响应
} key_deal_e;

/*!
 * \brief  
 *  key_event_t 按键事件结构体
 */
typedef struct
{
    /*! 按键（逻辑）值*/
    key_value_e val;
    /*! 按键选项：bit0 表示是否需要按键音，用于临时屏蔽按键音 */
    uint8 option;
    /*! 按键类型*/
    key_type_e type;
    /*! 后续按键处理选项*/
    key_deal_e deal;
} key_event_t;

/*!
 * \brief  
 *  input_gui_msg_t 输入型消息，又称 gui 消息结构体
 */
typedef struct
{
    /*! 消息类型*/
    input_msg_type_e type;
    /*! 消息数据*/
    union
    {
        /*! 按键事件*/
        key_event_t kmsg;
    } data;
} input_gui_msg_t;

/*!
 * \brief  
 *  app_msg_hook 按键消息特殊处理勾函数
 */
typedef bool (*gui_msg_hook)(input_gui_msg_t *input_msg);

/*! 
 *  \brief
 *  g_this_gui_msg_hook 应用私有消息勾函数
 */
extern gui_msg_hook g_this_gui_msg_hook;

/*!
 *  \brief
 *  g_buffer_gui_msg 缓冲gui消息
 */
extern input_gui_msg_t g_buffer_gui_msg;

//按键逻辑分析模块
typedef enum
{
    KEY_STATUS_NULL,
    KEY_STATUS_DOWN,
    KEY_STATUS_HOLD,
    KEY_STATUS_DOWN_1ST,
    KEY_STATUS_UP_1ST,
    KEY_STATUS_DOWN_2ND,
    KEY_STATUS_DCK_MATCH,
} key_status_e;

#define KEY_MAX_COUNT     8
#define TK_KEY_MAX_COUNT  8

typedef struct
{
    uint32 dck_map; //逻辑按键是否有双击按键事件，每个逻辑按键对应1个bit，按照逻辑按键ID，该值要求在get_gui_msg前计算
    uint16 dck_timestamp;
    uint16 last_detect_timestamp;
    uint8 dck_val;
    bool dck_fail;
    bool dck_flag;
    uint8 count_fail;
    key_status_e status;
    uint8 count;
    /*! 过滤掉按键长按后续动作，即 KEY_TYPE_HOLD & KEY_TYPE_SHORT_UP */
    uint8 filter_key;
    /*! 过滤掉按键长按后续动作，即 KEY_TYPE_SHORT_UP */
    uint8 filter_key_up;
    /*! 过滤超长按键超长按后续动作*/
    uint8 filter_key_itself;
    //逻辑按键映射表
    uint8 key_val_map[KEY_MAX_COUNT];
    //触摸按键映射表
    uint8 tk_key_val_map[TK_KEY_MAX_COUNT];
} key_infor_t;

extern key_infor_t g_key_infor;

//接收消息接口
extern bool get_gui_msg(input_gui_msg_t *input_msg) __FAR__;
extern bool com_phy_key_fsm(input_gui_msg_t *input_msg) __FAR__;
extern bool com_need_cont_dck_deal(uint8 key_count) __FAR__;
extern void com_filter_keymsg_in_queue(key_value_e k_val, key_type_e k_type) __FAR__;

#endif
