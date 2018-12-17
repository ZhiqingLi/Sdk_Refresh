/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：分立LED灯相关的宏定义，枚举类型，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef __COMMON_DISCRETE_LED_H__
#define __COMMON_DISCRETE_LED_H__

#include "psp_includes.h"
#include "case_independent.h"

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)

#define LED_BL_LINK_PORT     LED_PORT_GPIOA
#define LED_BL_LINK_PIN      (12)

#define LED_MUSIC_PLAY_PORT  LED_PORT_GPIOA
#define LED_MUSIC_PLAY_PIN   (14)

#define LED_AUX_PLAY_PORT    LED_PORT_GPIOA
#define LED_AUX_PLAY_PIN     (13)

#define LED_FM_PLAY_PORT     LED_PORT_GPIOA
#define LED_FM_PLAY_PIN      (22)

#define LED_POWER_PORT       LED_PORT_GPIOA
#define LED_POWER_PIN        (23)

#else

#define LED_BL_LINK_PORT     LED_PORT_GPIOA
#define LED_BL_LINK_PIN      (4)

#endif

//定义各种用途的LED灯
typedef enum
{
    LED_ID_BT_LINK = 0, //蓝牙连接指示灯
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    LED_ID_MUSIC_PLAY = 1, //音乐播放指示灯
    LED_ID_AUX_PLAY = 2, //LINEIN播放指示灯
    LED_ID_FM_PLAY = 3, //收音机播放指示灯
    LED_ID_POWER = 4, //电源指示灯
#endif
    LED_ID_MAX
} led_id_e;

typedef enum
{
    LED_ALWAYS_OFF = 0,
    LED_ALWAYS_ON = 1,
    LED_TWINKLE = 2,
} led_mode_e;

typedef struct
{
    /*! 循环周期数目，0表示循环停止，-1表示一直循环直到重新设置 */
    uint8 cycle_count;
    /*! LED亮时间，闪烁时有效，单位为1ms */
    uint16 duty_on_time;
    /*! LED灭时间，闪烁时有效，单位为1ms */
    uint16 duty_off_time;
} led_duty_t;

//周期闪烁：管理多个led灯
typedef struct
{
    /*端口地址，详见 led_port_e */
    uint16 led_port : 3;
    /*! GPIO PIN 号，0表示GPIO0 */
    uint16 led_pin : 5;
    /*! LED灯亮度，8级可调 */
    uint16 lightness : 3;
    /*! 工作模式，LED_MODE类型：0表示常灭，1表示常亮，2表示闪烁，详见 led_mode_e */
    uint16 work_mode : 2;
    /*! LED亮灭状态，0表示灭，1表示亮 */
    uint16 on_off : 1;
    //定时器ID
    int8  timer_id;
    uint8 reserve;
} discrete_led_info_t;

typedef struct
{
    led_port_e port;
    uint8 pin;
} discrete_led_config_t;

extern discrete_led_info_t g_discrete_led_info[LED_ID_MAX];
extern bool g_discrete_led_a23_available;

extern void discrete_led_set(led_id_e led_id, led_mode_e mode, led_duty_t *duty_param) __FAR__;
extern void discrete_led_init(void) __FAR__;
extern void discrete_led_low_power(void) __FAR__;
extern void discrete_led_restore(void) __FAR__;

#endif

