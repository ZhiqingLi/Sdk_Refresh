
/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*     brief    led驱动总接口定义;供驱动安装时使用;
*      <author>       <time>           
*       Wekan       2015-3-26pm            
*******************************************************************************/


#include "led_driver.h"

typedef void*(*led_op_func)(void*, void*, void*);

typedef struct
{
    led_op_func sdrv_led_display;//display
    led_op_func sdrv_clear_screen;//clear
    led_op_func sdrv_flash_screen;
    led_op_func sdrv_wake_up_LED_display;  //wake
    led_op_func sdrv_sleep_LED_display;    //sleep
} led_driver_operations;



led_driver_operations led_driver_op =
{
    (led_op_func) sdrv_led_display, //display
    (led_op_func) sdrv_clear_screen, //clear
    (led_op_func) sdrv_flash_screen ,//flush
    (led_op_func) sdrv_wake_up_LED_display,  //wake
    (led_op_func) sdrv_sleep_LED_display,    //sleep
};

