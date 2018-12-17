/*******************************************************************************
 *                              us280A
 *                            Module: LED驱动
 *                 Copyright(c) 2003-2008 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       johnsen    2012-12-31 14:16     1.0             build this file
 *******************************************************************************/
/*!
 * \file     led_driver.h
 * \brief    led物理驱动模块相关数据结构，宏定义，接口声明等
 * \author   johnsen
 * \version 1.0
 * \date  2012-12-31 14:17
 *******************************************************************************/

#ifndef _LED_DRIVER_H
#define _LED_DRIVER_H

/*! \cond LED_DRIVER_API */
#include "psp_includes.h"
#include "case_independent.h"

/*!
 * \brief
 *  lcd_cmd_e：led驱动命令类型枚举类型
 */
typedef enum
{
    /*!显示LED*/
    LED_DISPLAY = 0,
    /*!清除屏幕*/
    LED_CLEAR_SCREEN,
    /*!闪烁屏幕*/
    LED_FLASH_SCREEN,
    /*唤醒LED显示*/
    LED_WAKE_UP,
    /*关LED显示*/
    LED_SLEEP,
} led_cmd_e;

//LED显示地址分配表（逻辑表，在显示驱动中进行逻辑到物理的映射）
#define S1 0        //地址为0的ICON的地址，内容为0xff表示“ICON”
#define S2 1        //地址为1的ICON的地址，内容为0xff表示“ICON”
#define S3 2        //地址为2的ICON的地址，内容为0xff表示“ICON”
#define S4 3        //地址为3的ICON的地址，内容为0xff表示“ICON”
#define S5 4        //地址为4的ICON的地址，内容为0xff表示“ICON”
#define S6 5        //地址为5的ICON的地址，内容为0xff表示“ICON”
#define S7 6        //地址为6的ICON的地址，内容为0xff表示“ICON”
#define S8 7        //地址为7的ICON的地址，内容为0xff表示“ICON”
#define S9 8        //地址为8的ICON的地址，内容为0xff表示“ICON”
#define S10 9       //地址为9的ICON的地址，内容为0xff表示“ICON”
#define S11 10      //地址为10的ICON的地址，内容为0xff表示“ICON”
#define S12 11      //地址为11的ICON的地址，内容为0xff表示“ICON”
#define S13 12      //地址为12的ICON的地址，内容为0xff表示“ICON”
#define S14 13      //地址为13的ICON的地址，内容为0xff表示“ICON”
#define S15 14      //地址为14的ICON的地址，内容为0xff表示“ICON”
#define S16 15      //地址为15的ICON的地址，内容为0xff表示“ICON”
#define S17 16      //地址为16的ICON的地址，内容为0xff表示“ICON”
#define S18 17      //地址为17的ICON的地址，内容为0xff表示“ICON”
#define S19 18      //地址为18的ICON的地址，内容为0xff表示“ICON”
#define S20 19      //地址为19的ICON的地址，内容为0xff表示“ICON”
#define S21 20      //地址为20的ICON的地址，内容为0xff表示“ICON”
#define S22 21      //地址为21的ICON的地址，内容为0xff表示“ICON”
#define S23 22      //地址为22的ICON的地址，内容为0xff表示“ICON”
#define S24 23      //地址为23的ICON的地址，内容为0xff表示“ICON”
#define S25 24      //地址为24的ICON的地址，内容为0xff表示“ICON”
#define S_NULL 25

#define NUMBER1 S1      //第一个数字的地址，与ICON地址不统一编排
#define NUMBER2 S2      //第二个数字的地址，与ICON地址不统一编排
#define NUMBER3 S3      //第三个数字的地址，与ICON地址不统一编排
#define NUMBER4 S4      //第四个数字的地址，与ICON地址不统一编排

//for 04311
// #define LED_SD  S1
// #define LED_USB S2
// #define LED_AUX S6
// #define LCD_COL S7
// #define LED_FM  S8  ;FM与.为同一个ICON，只有在"FM"ICON和"."ICON同时点亮
// #define LED_PLAY S17
// #define LED_PAUSE S18

//下表的ICON 地址分配集合了多种数码管的场合，为的是能尽可能多的支持不同
//数码管而程序不修改或少修改。

#if (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)

#define LCD_SD    S18   //ICON "SD"
#define LCD_USB   S2   //ICON "USB"
#define LCD_P1    S_NULL   //ICON "."
#define LCD_TIMER S_NULL   //ICON "CLOCK"
#define LCD_TEMER S_NULL   //ICON "TEMPLETURE"
#define LCD_AUX   S13   //ICON "AUX"
#define LCD_COL   S7   //ICON ":"
#define LCD_FM    S17   //ICON "FM"
#define LCD_SOUND S_NULL   //ICON "sound"
#define LCD_BAT1  S_NULL  //ICON "bat1"
#define LCD_BAT2  S_NULL  //ICON "bat2"
#define LCD_BAT3  S_NULL  //ICON "bat3"
#define LCD_PMHZ  S_NULL  //ICON "PM"
#define LCD_NO1   S_NULL  //ICON "NO."
#define LCD_VOL   S_NULL  //ICON "VOL"
#define LCD_MUSIC S_NULL  //ICON "MUSIC"
#define LCD_PLAY  S1      //ICON "PLAY"
#define LCD_PAUSE S8      //ICON "PAUSE"
#define ICON_TOTAL 18

#else

#define LCD_SD    S18   //ICON "SD"
#define LCD_USB   S13   //ICON "USB"
#define LCD_P1    S_NULL   //ICON "."
#define LCD_TIMER S_NULL   //ICON "CLOCK"
#define LCD_TEMER S_NULL   //ICON "TEMPLETURE"
#define LCD_AUX   S1   //ICON "AUX"
#define LCD_COL   S7   //ICON ":"
#define LCD_FM    S17   //ICON "FM"
#define LCD_SOUND S_NULL   //ICON "sound"
#define LCD_BAT1  S_NULL  //ICON "bat1"
#define LCD_BAT2  S_NULL  //ICON "bat2"
#define LCD_BAT3  S_NULL  //ICON "bat3"
#define LCD_PMHZ  S_NULL  //ICON "PM"
#define LCD_NO1   S_NULL  //ICON "NO."
#define LCD_VOL   S_NULL  //ICON "VOL"
#define LCD_MUSIC S_NULL  //ICON "MUSIC"
#define LCD_PLAY  S8  //ICON "PLAY"
#define LCD_PAUSE S2  //ICON "PAUSE"
#define ICON_TOTAL 18

#endif




//数据内容取值分配表
#define NUM_0  0
#define NUM_1  1
#define NUM_2  2
#define NUM_3  3
#define NUM_4  4
#define NUM_5  5
#define NUM_6  6
#define NUM_7  7
#define NUM_8  8
#define NUM_9  9
#define NUM_A  10
#define NUM_B  11
#define NUM_C  12
#define NUM_D  13
#define NUM_E  14
#define NUM_F  15
#define NUM_G  16
#define NUM_H  17
#define NUM_I  18
#define NUM_J  19
#define NUM_K  20
#define NUM_L  21
#define NUM_M  22   //can't display
#define NUM_N  23
#define NUM_O  24
#define NUM_P  25
#define NUM_Q  26
#define NUM_R  27
#define NUM_S  28
#define NUM_t  29
#define NUM_T  29   //the same as 't'
#define NUM_U  30
#define NUM_V  31   //the same as U
#define NUM_W  32   //can't display
#define NUM_X  33   //can't display
#define NUM_y  34
#define NUM_Z  35
#define NUM_b  36
#define NUM_ERROR  37   //'-',该序列最大值，超出此值按此处理
#define NUMBER_TOTAL 38

#define ICON_NUM 7
#define ADDR_NUM 5
#define NUMBERADDR 4

#define reg_writeb(val, reg)  *(volatile unsigned char *)(reg) = (val)
#define reg_writew(val, reg)  *(volatile unsigned short *)(reg) = (val)
#define reg_writel(val, reg)  *(volatile unsigned int *)(reg) = (val)
#define reg_readb(port)      (*(volatile unsigned char *)(port))
#define reg_readw(port)      (*(volatile unsigned short *)(port))
#define reg_readl(port)      (*(volatile unsigned int *)(port))

///供上层调用统一接口声明
extern void *lcd_op_entry(void *param1,
        void *param2, void *param3, led_cmd_e cmd)__FAR__;

//for welcome module
extern void sdrv_led_display(uint8 addr, uint8 content, uint8 type)__FAR__; //显示LED
extern void sdrv_clear_screen(void *null1, void *null2, void *null3)__FAR__; //清除屏幕
extern void sdrv_flash_screen(uint8 onoff, void *null2, void *null3)__FAR__; //闪烁屏幕
extern uint8 sdrv_wake_up_LED_display(uint8, uint8, uint8)__FAR__; //唤醒LED显示
extern void sdrv_sleep_LED_display(uint8, uint8, uint8)__FAR__; //0.5秒执行一次计时休眠

#if (SUPPORT_LED_DRIVER == 1)

/*!
 * \brief显示LED
 * input: addr--地址，取值：ICON（从S1到S25）；
 *              数字、字母（从NUMBER1-NUMBER4）
 *        content--数据内容，0xff表示ICON；
 *              其余表示数字或字母,取值从NUM_0-NUM_Z
 *        type--显示或不显示；0--不显示，非0--显示
 */

#define led_display(uint8_addr,uint8_content,uint8_type) \
lcd_op_entry((void *)(uint32)(uint8_addr), (void *)(uint32)(uint8_content), (void *)(uint32)(uint8_type), LED_DISPLAY)

/*!
 * \brief
 *   清除屏幕
 */
#define led_clear_screen() \
lcd_op_entry((void *)(0), (void *)(0), (void *)(0), LED_CLEAR_SCREEN)

/*!
 * \brief
 *   闪烁屏幕
 *input: onoff--闪烁开关，取值：1表示开，显示原数据，0表示关，清除数据；
 */
#define led_flash_screen(onoff) \
lcd_op_entry((void *)(uint32)(onoff), (void *)(0), (void *)(0), LED_FLASH_SCREEN)

#define led_wake_up_display()  \
((uint8)(uint32)lcd_op_entry((void *)(0), (void *)(0), (void *)(0), LED_WAKE_UP))

#define led_sleep_display() \
lcd_op_entry((void *)(0), (void *)(0), (void *)(0),LED_SLEEP)

#else

#define led_display(addr,content,type)
#define led_clear_screen()
#define led_flash_screen(onoff)
#define led_wake_up_display()
#define led_sleep_display()

#endif

/*! \endcond */

#endif //_LED_DRIVER_H
