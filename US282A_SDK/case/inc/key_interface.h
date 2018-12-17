#ifndef _KEY_INTERFACE_H            //防止重定义
#define _KEY_INTERFACE_H

#include "psp_includes.h"
#include "case_independent.h"
#include "common_bat_charge.h"

#define PER_DETECT_PERIOD           200 //外围设备检查周期
#define CARD_DETECT_TIMER           2   //卡检测周期为400ms = 200ms * 2
#define USBUHOST_DETECT_TIMER       2   //USB/UHOST检测周期为400ms（检测） + 200ms（切换后等待200ms再使能）

#define PER_DETECT_CARD             0x00000001 //要检测卡，比较耗时，所以检测周期会长一点
#define PRE_DETECT_CARD_FOR_IN      0x00000002 //置位表示当前卡不在，检测卡插入
#define PER_DETECT_CARD_TEST        0x00000004 //测试卡是否插着
#define PER_DETECT_CARD_FORCE       0x00000080 //强制进行卡检测，一般在前台应用刚切换过来后强制检测一次
#define PER_DETECT_AUX              0x00000100 //要检测AUX
#define PER_DETECT_AUX_FOR_IN       0x00000200 //置位表示当前AUX不在，检测AUX插入
#define PER_DETECT_AUX_TEST         0x00000400 //测试AUX是否插着
#define PER_DETECT_HP               0x00010000 //要检测HPEAPHONE
#define PER_DETECT_HP_FOR_IN        0x00020000 //置位表示当前HP没插着，检测HP插入
#define PER_DETECT_HP_TEST          0x00040000 //测试HP是否插着
#define PER_DETECT_USBUHOST_SWITCH  0x01000000 //要切换USB/UHOST
#define PER_DETECT_UHOST_TEST       0x02000000 //测试UHOST是否插着
#define PER_DETECT_USB_FORCE        0x40000000 //强制切换到USB
#define PER_DETECT_UHOST_FORCE      0x80000000 //强制切换到UHOST

/*1.定义提供的具体接口命令，这里是按键驱动统一提供的接口*/

typedef enum
{
    TK_OP_ON = 0,
    TK_OP_OFF = 1,
    TK_OP_CHECK = 2,
} tk_op_e;

typedef enum
{
    KEY_BATTERY_CHARGE_OPEN = 0,
    KEY_BATTERY_CHARGE_STOP,
    KEY_BATTERY_GET_STATUS,
    KEY_ADJUST_FREQ_OP,
    KEY_PERIPHERAL_DETECT,
    KEY_REGISTER_SCAN_ISR_CBK,
    KEY_DISCRETE_LED_OP,
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
    KEY_EXTERN_TK_OP,
#endif
    SET_EFFECT_INFO,
    GET_FEATURE_INFO,
    SET_WAVES_EFFECT_INFO,
    
    KEY_ADDRESS_TRANSLATION,
    
    KEY_MAX,
} key_cmd_e;

extern void *key_op_entry(void *param1, void *param2, void *param3, key_cmd_e cmd)__FAR__;

#define key_battery_charge_open(current_ma, voltage_mv)   \
key_op_entry((void*)(uint32)(current_ma),(void*)(uint32)(voltage_mv),(void*)0, KEY_BATTERY_CHARGE_OPEN)
#define key_battery_charge_stop()    \
key_op_entry((void*)0,(void*)0,(void*)0, KEY_BATTERY_CHARGE_STOP)
#define key_battery_get_status(p_ad_val, p_vol_limit, app_id)    \
(battery_status_e)key_op_entry((void*)(p_ad_val),(void*)(p_vol_limit),(void*)app_id, KEY_BATTERY_GET_STATUS)

#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
#define key_extern_tk_op(tk_op)    \
(uint8)key_op_entry((void*)0,(void*)0,(void*)(uint32)(tk_op), KEY_EXTERN_TK_OP)
#endif

/*设置音效参数*/
/*int32 key_inner_set_effect_param(uint32 set_type, void *param_ptr, void *null3)*/
/*set_type:设置类型;param_ptr:音效参数指针*/
#define set_effect_param(a,b) (int32)key_op_entry((void*)(uint32)(a), (void*)(b), (void*)(0), SET_EFFECT_INFO)

/*获取音效处理信息*/
/*int32 key_inner_get_feature_info(uint32 set_type, void *info_ptr, void *null3)*/
/*set_type:音效类型;info_ptr:信息指针*/
#define get_feature_info(a,b) (int32)key_op_entry((void*)(uint32)(a), (void*)(b), (void*)(0), GET_FEATURE_INFO)

/*设置WAVES音效参数*/
/*int32 key_inner_set_waves_effect_param(uint32 set_type, void *param_ptr, void *null3)*/
/*set_type:设置类型;param_ptr:音效参数指针*/
#define waves_set_effect_param(a,b) (int32)key_op_entry((void*)(uint32)(a), (void*)(b), (void*)(0), SET_WAVES_EFFECT_INFO)

/*mips与dsp之间的地址转换*/
/*int32 key_inner_address_translation(uint32 type, void *param_ptr, void *null3)*/
/*type:address_translation_e;param_ptr:需要转换地址的地址*/

#define key_address_translation(a,b) (int32)key_op_entry((void*)(uint32)(a), (void*)(b), (void*)(0), KEY_ADDRESS_TRANSLATION)


/***************************************/

typedef enum
{
    FREQ_NULL = 0,  //不耗用MIPS或DSP频率，即频率值为0
    FREQ_LEVEL1,    //如果是MIPS->1MM, 如果是DSP->35M
    FREQ_LEVEL2,    //如果是MIPS->2M,  如果是DSP->40M
    FREQ_LEVEL3,    //如果是MIPS->4M,  如果是DSP->55M
    FREQ_LEVEL4,    //如果是MIPS->6M,  如果是DSP->60M
    FREQ_LEVEL5,    //如果是MIPS->8M,  如果是DSP->70M
    FREQ_LEVEL6,    //如果是MIPS->10M, 如果是DSP->80M
    FREQ_LEVEL7,    //如果是MIPS->20M, 如果是DSP->100M
    FREQ_LEVEL8,    //如果是MIPS->30M, 如果是DSP->120M
    FREQ_LEVEL9,    //如果是MIPS->40M, 如果是DSP->140M
    FREQ_LEVEL10,   //如果是MIPS->60M, 如果是DSP->160M
    FREQ_LEVEL11,   //如果是MIPS->85M, 如果是DSP->180M
    FREQ_LEVEL_MAX
} adjust_freq_level_e;

//调频接口
typedef enum
{
    ADJUST_FQ_SET_LEVEL,
    ADJUST_FQ_ADD_VALUE,
    ADJUST_FQ_CMD_MAX,
} adjust_freq_cmd_e;


/*
 int adjust_freq_set_level(uint8 prio,uint8 level_a,uint8 level_b)
 该接口用于调整misp 和 dsp 频率档位
 prio：线程对应的优先级
 level_a：misp调频档位，adjust_freq_level_e,若为 FREQ_NULL 表示不耗频率
 level_b：dsp调频档位，adjust_freq_level_e，若为 FREQ_NULL 表示不耗频率
 返回值int：0-成功，非0失败
 !!!NOTE:bank代码，禁止在中断调用
 */
#define adjust_freq_set_level(prio,level_a,level_b)  \
(int)key_op_entry((void*)(uint32)(prio),(void*)(((uint32)(level_b) << 8) | ((uint32)(level_a))),(void*)ADJUST_FQ_SET_LEVEL, KEY_ADJUST_FREQ_OP)


/*
 int adjust_freq_add_value(int16 freq_a,int16 freq_b)
 该接口用于微调 misp 和 dsp 频率值,可调高或调低
 freq_a：叠加misp的频率，值可为负数
 freq_b：叠加dsp的频率，值可为负数
 返回值int：0-成功，非0失败
 !!!NOTE:bank代码，禁止在中断调用
 */
#define adjust_freq_add_value(freq_a,freq_b)  \
(int)key_op_entry((void*)(int)(int16)(freq_a),(void*)(int)(int16)(freq_b),(void*)ADJUST_FQ_ADD_VALUE, KEY_ADJUST_FREQ_OP)


/*
 int key_peripheral_detect_handle(uint32 detect_mode)
 该接口用于周期进行外部设备检测，检测周期为200ms，各外设自己决定检测周期为几个200ms
 detect_mode：表示要检测哪些外设，详见 PER_DETECT_XXX 宏定义
 测试模式调用时，返回1表示外设插着，返回0表示外设没插着
 */
#define key_peripheral_detect_handle(detect_mode)  \
(int)key_op_entry((void*)detect_mode,(void*)0,(void*)0, KEY_PERIPHERAL_DETECT)

#define key_register_scan_isr_cbk(cbk)  \
(int)key_op_entry((void*)(cbk),(void*)0,(void*)0, KEY_REGISTER_SCAN_ISR_CBK)


typedef enum
{
    LED_PORT_GPIOA = 0,
    LED_PORT_GPIOB = 1,
    LED_PORT_PWM0 = 2,
    LED_PORT_PWM1 = 3,
    LED_PORT_PWM2 = 4,
    LED_PORT_PWM3 = 5,
} led_port_e;

#define LED_ON_GPIO_HIGH  0
#define LED_ON_GPIO_LOW   1
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#define LED_ON_GPIO_SEL   LED_ON_GPIO_LOW
#else
#define LED_ON_GPIO_SEL   LED_ON_GPIO_HIGH
#endif

//分立LED灯接口
typedef enum
{
    DISCRETE_LED_INIT,
    DISCRETE_LED_ON,
    DISCRETE_LED_OFF,
} discrete_led_op_e;


#define key_discrete_led_init(port,pin)  \
(int)key_op_entry((void*)(uint32)(port),(void*)(uint32)(pin),(void*)DISCRETE_LED_INIT, KEY_DISCRETE_LED_OP)

#define key_discrete_led_on(port,pin)  \
(int)key_op_entry((void*)(uint32)(port),(void*)(uint32)(pin),(void*)DISCRETE_LED_ON, KEY_DISCRETE_LED_OP)

#define key_discrete_led_off(port,pin)  \
(int)key_op_entry((void*)(uint32)(port),(void*)(uint32)(pin),(void*)DISCRETE_LED_OFF, KEY_DISCRETE_LED_OP)

#endif

