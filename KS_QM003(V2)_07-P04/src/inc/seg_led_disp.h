#ifndef __SEG_LED_DISP__
#define __SEG_LED_DISP__


#include "gpio.h"
#ifdef FUNC_SEG_LED_EN

//7和6脚LED屏的驱动程序，扫描算法如下：

//7脚LED屏，习惯上将7个管脚的序号称为1脚--7脚
//为了方便写代码，本模块中将7个脚序号改为0-6
//COM阶段分别为COM0--COM6
//SEG段码分别为SEG0--SEG6

//扫描算法

//COM高选中: 输出高电平
//COM非选中: 高阻态

//SEG亮: 使能下拉电流源
//SEG灭: 高阻态

//COM0: 1脚做COM输出高电平, 其他6个脚做SEG, 亮的笔画SEG使能下拉电流源，灭的笔画SEG设置为高阻态
//COM1: 2脚做COM输出高电平, 其他6个脚做SEG, 亮的笔画SEG使能下拉电流源，灭的笔画SEG设置为高阻态
//COM2: 3脚做COM输出高电平, 其他6个脚做SEG, 亮的笔画SEG使能下拉电流源，灭的笔画SEG设置为高阻态
//COM3: 4脚做COM输出高电平, 其他6个脚做SEG, 亮的笔画SEG使能下拉电流源，灭的笔画SEG设置为高阻态
//COM4: 5脚做COM输出高电平, 其他6个脚做SEG, 亮的笔画SEG使能下拉电流源，灭的笔画SEG设置为高阻态
//COM5: 6脚做COM输出高电平, 其他6个脚做SEG, 亮的笔画SEG使能下拉电流源，灭的笔画SEG设置为高阻态
//COM6: 7脚做COM输出高电平, 其他6个脚做SEG, 亮的笔画SEG使能下拉电流源，灭的笔画SEG设置为高阻态


#if (defined(FUNC_9PIN_SEG_LED_EN) || defined(FUNC_7PIN_SEG_LED_EN) || defined(FUNC_6PIN_SEG_LED_EN))

#ifdef FUNC_6PIN_SEG_LED_EN
#define MAX_LED_PIN_NUM		6	//LED 引脚数
#elif (defined(FUNC_7PIN_SEG_LED_EN))
#define MAX_LED_PIN_NUM		7	//LED 引脚数
#else
#define MAX_LED_PIN_NUM		9	//LED 引脚数
#endif


//LED 屏引脚与GPIO 连接定义,请根据硬件定义
//6只脚段码LED屏示例
#define LED_PIN1_PORT_PU 		GPIO_B_PU
#define LED_PIN1_PORT_PD 		GPIO_B_PD
#define LED_PIN1_PORT_OE 		GPIO_B_OE
#define LED_PIN1_PORT_IE 		GPIO_B_IE
#define LED_PIN1_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN1_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN1_PORT_OUT 		GPIO_B_OUT
#define LED_PIN1_BIT			GPIOB20

#define LED_PIN2_PORT_PU 		GPIO_B_PU
#define LED_PIN2_PORT_PD 		GPIO_B_PD
#define LED_PIN2_PORT_OE 		GPIO_B_OE
#define LED_PIN2_PORT_IE 		GPIO_B_IE
#define LED_PIN2_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN2_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN2_PORT_OUT 		GPIO_B_OUT
#define LED_PIN2_BIT			GPIOB21

#define LED_PIN3_PORT_PU 		GPIO_B_PU
#define LED_PIN3_PORT_PD 		GPIO_B_PD
#define LED_PIN3_PORT_OE 		GPIO_B_OE
#define LED_PIN3_PORT_IE 		GPIO_B_IE
#define LED_PIN3_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN3_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN3_PORT_OUT 		GPIO_B_OUT
#define LED_PIN3_BIT			GPIOB22

#define LED_PIN4_PORT_PU 		GPIO_B_PU
#define LED_PIN4_PORT_PD 		GPIO_B_PD
#define LED_PIN4_PORT_OE 		GPIO_B_OE
#define LED_PIN4_PORT_IE 		GPIO_B_IE
#define LED_PIN4_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN4_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN4_PORT_OUT 		GPIO_B_OUT
#define LED_PIN4_BIT			GPIOB23

#define LED_PIN5_PORT_PU 		GPIO_B_PU
#define LED_PIN5_PORT_PD 		GPIO_B_PD
#define LED_PIN5_PORT_OE 		GPIO_B_OE
#define LED_PIN5_PORT_IE 		GPIO_B_IE
#define LED_PIN5_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN5_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN5_PORT_OUT 		GPIO_B_OUT
#define LED_PIN5_BIT			GPIOB25

#define LED_PIN6_PORT_PU 		GPIO_B_PU
#define LED_PIN6_PORT_PD 		GPIO_B_PD
#define LED_PIN6_PORT_OE 		GPIO_B_OE
#define LED_PIN6_PORT_IE 		GPIO_B_IE
#define LED_PIN6_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN6_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN6_PORT_OUT 		GPIO_B_OUT
#define LED_PIN6_BIT			GPIOB26

#if defined(FUNC_7PIN_SEG_LED_EN)
//7只脚段码LED屏示例
#define LED_PIN7_PORT_PU 		GPIO_B_PU
#define LED_PIN7_PORT_PD 		GPIO_B_PD
#define LED_PIN7_PORT_OE 		GPIO_B_OE
#define LED_PIN7_PORT_IE 		GPIO_B_IE
#define LED_PIN7_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN7_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN7_PORT_OUT 		GPIO_B_OUT
#define LED_PIN7_BIT			GPIOB27

#elif defined(FUNC_9PIN_SEG_LED_EN)
//9只脚段码LED屏示例
#define LED_PIN7_PORT_PU 		GPIO_B_PU
#define LED_PIN7_PORT_PD 		GPIO_B_PD
#define LED_PIN7_PORT_OE 		GPIO_B_OE
#define LED_PIN7_PORT_IE 		GPIO_B_IE
#define LED_PIN7_PORT_ICS_1MA7 	GPIO_B_PULLDOWN1
#define LED_PIN7_PORT_ICS_2MA4 	GPIO_B_PULLDOWN2
#define LED_PIN7_PORT_OUT 		GPIO_B_OUT
#define LED_PIN7_BIT			GPIOB27

#define LED_PIN8_PORT_PU 		GPIO_C_PU
#define LED_PIN8_PORT_PD 		GPIO_C_PD
#define LED_PIN8_PORT_OE 		GPIO_C_OE
#define LED_PIN8_PORT_IE 		GPIO_C_IE
#define LED_PIN8_PORT_ICS_1MA7 	GPIO_C_PULLDOWN1
#define LED_PIN8_PORT_ICS_2MA4 	GPIO_C_PULLDOWN2
#define LED_PIN8_PORT_OUT 		GPIO_C_OUT
#define LED_PIN8_BIT			GPIOC11

#define LED_PIN9_PORT_PU 		GPIO_C_PU
#define LED_PIN9_PORT_PD 		GPIO_C_PD
#define LED_PIN9_PORT_OE 		GPIO_C_OE
#define LED_PIN9_PORT_IE 		GPIO_C_IE
#define LED_PIN9_PORT_ICS_1MA7 	GPIO_C_PULLDOWN1
#define LED_PIN9_PORT_ICS_2MA4 	GPIO_C_PULLDOWN2
#define LED_PIN9_PORT_OUT 		GPIO_C_OUT
#define LED_PIN9_BIT			GPIOC12

#endif


//设置LED 各个引脚输出高电平
#define LED_PIN1_OUT_HIGH	GpioSetRegOneBit(LED_PIN1_PORT_OE,LED_PIN1_BIT),\
                                                 GpioSetRegOneBit(LED_PIN1_PORT_OUT,LED_PIN1_BIT)

#define LED_PIN2_OUT_HIGH	GpioSetRegOneBit(LED_PIN2_PORT_OE,LED_PIN2_BIT),\
                                                 GpioSetRegOneBit(LED_PIN2_PORT_OUT,LED_PIN2_BIT)

#define LED_PIN3_OUT_HIGH	GpioSetRegOneBit(LED_PIN3_PORT_OE,LED_PIN3_BIT),\
                                                 GpioSetRegOneBit(LED_PIN3_PORT_OUT,LED_PIN3_BIT)

#define LED_PIN4_OUT_HIGH	GpioSetRegOneBit(LED_PIN4_PORT_OE,LED_PIN4_BIT),\
                                                 GpioSetRegOneBit(LED_PIN4_PORT_OUT,LED_PIN4_BIT)

#define LED_PIN5_OUT_HIGH	GpioSetRegOneBit(LED_PIN5_PORT_OE,LED_PIN5_BIT),\
                                                 GpioSetRegOneBit(LED_PIN5_PORT_OUT,LED_PIN5_BIT)

#define LED_PIN6_OUT_HIGH	GpioSetRegOneBit(LED_PIN6_PORT_OE,LED_PIN6_BIT),\
                                                 GpioSetRegOneBit(LED_PIN6_PORT_OUT,LED_PIN6_BIT)

#if defined(FUNC_7PIN_SEG_LED_EN)
#define LED_PIN7_OUT_HIGH	GpioSetRegOneBit(LED_PIN7_PORT_OE,LED_PIN7_BIT),\
                                                 GpioSetRegOneBit(LED_PIN7_PORT_OUT,LED_PIN7_BIT)
#elif defined(FUNC_9PIN_SEG_LED_EN)
#define LED_PIN7_OUT_HIGH	GpioSetRegOneBit(LED_PIN7_PORT_OE,LED_PIN7_BIT),\
												GpioSetRegOneBit(LED_PIN7_PORT_OUT,LED_PIN7_BIT)
												
#define LED_PIN8_OUT_HIGH	GpioSetRegOneBit(LED_PIN8_PORT_OE,LED_PIN8_BIT),\
												GpioSetRegOneBit(LED_PIN8_PORT_OUT,LED_PIN8_BIT)
												
#define LED_PIN9_OUT_HIGH	GpioSetRegOneBit(LED_PIN9_PORT_OE,LED_PIN9_BIT),\
												GpioSetRegOneBit(LED_PIN9_PORT_OUT,LED_PIN9_BIT)
#endif


#define LED_PIN1_IN_ON	 GpioSetRegOneBit(LED_PIN1_PORT_ICS_2MA4,LED_PIN1_BIT),\
                                           GpioSetRegOneBit(LED_PIN1_PORT_ICS_1MA7,LED_PIN1_BIT)

#define LED_PIN2_IN_ON	 GpioSetRegOneBit(LED_PIN2_PORT_ICS_2MA4,LED_PIN2_BIT),\
                                           GpioSetRegOneBit(LED_PIN2_PORT_ICS_1MA7,LED_PIN2_BIT)

#define LED_PIN3_IN_ON	 GpioSetRegOneBit(LED_PIN3_PORT_ICS_2MA4,LED_PIN3_BIT),\
                                           GpioSetRegOneBit(LED_PIN3_PORT_ICS_1MA7,LED_PIN3_BIT)

#define LED_PIN4_IN_ON	 GpioSetRegOneBit(LED_PIN4_PORT_ICS_2MA4,LED_PIN4_BIT),\
                                           GpioSetRegOneBit(LED_PIN4_PORT_ICS_1MA7,LED_PIN4_BIT)

#define LED_PIN5_IN_ON	 GpioSetRegOneBit(LED_PIN5_PORT_ICS_2MA4,LED_PIN5_BIT),\
                                           GpioSetRegOneBit(LED_PIN5_PORT_ICS_1MA7,LED_PIN5_BIT)

#define LED_PIN6_IN_ON	GpioSetRegOneBit(LED_PIN6_PORT_ICS_2MA4,LED_PIN6_BIT),\
                                           GpioSetRegOneBit(LED_PIN6_PORT_ICS_1MA7,LED_PIN6_BIT)

#if defined(FUNC_7PIN_SEG_LED_EN)
#define LED_PIN7_IN_ON	GpioSetRegOneBit(LED_PIN7_PORT_ICS_2MA4,LED_PIN7_BIT),\
                                           GpioSetRegOneBit(LED_PIN7_PORT_ICS_1MA7,LED_PIN7_BIT)

#elif defined(FUNC_9PIN_SEG_LED_EN)
#define LED_PIN7_IN_ON	GpioSetRegOneBit(LED_PIN7_PORT_ICS_2MA4,LED_PIN7_BIT),\
                                           GpioSetRegOneBit(LED_PIN7_PORT_ICS_1MA7,LED_PIN7_BIT)

#define LED_PIN8_IN_ON	GpioSetRegOneBit(LED_PIN8_PORT_ICS_2MA4,LED_PIN8_BIT),\
                                           GpioSetRegOneBit(LED_PIN8_PORT_ICS_1MA7,LED_PIN8_BIT)

#define LED_PIN9_IN_ON	GpioSetRegOneBit(LED_PIN9_PORT_ICS_2MA4,LED_PIN9_BIT),\
                                           GpioSetRegOneBit(LED_PIN9_PORT_ICS_1MA7,LED_PIN9_BIT)

#endif

//禁止LED 各个引脚电流输入
#define LED_PIN1_IN_OFF	 GpioClrRegOneBit(LED_PIN1_PORT_ICS_2MA4,LED_PIN1_BIT),\
                                           GpioClrRegOneBit(LED_PIN1_PORT_ICS_1MA7,LED_PIN1_BIT)

#define LED_PIN2_IN_OFF	 GpioClrRegOneBit(LED_PIN2_PORT_ICS_2MA4,LED_PIN2_BIT),\
                                           GpioClrRegOneBit(LED_PIN2_PORT_ICS_1MA7,LED_PIN2_BIT)

#define LED_PIN3_IN_OFF	 GpioClrRegOneBit(LED_PIN3_PORT_ICS_2MA4,LED_PIN3_BIT),\
                                           GpioClrRegOneBit(LED_PIN3_PORT_ICS_1MA7,LED_PIN3_BIT)

#define LED_PIN4_IN_OFF	 GpioClrRegOneBit(LED_PIN4_PORT_ICS_2MA4,LED_PIN4_BIT),\
                                           GpioClrRegOneBit(LED_PIN4_PORT_ICS_1MA7,LED_PIN4_BIT)

#define LED_PIN5_IN_OFF	 GpioClrRegOneBit(LED_PIN5_PORT_ICS_2MA4,LED_PIN5_BIT),\
                                           GpioClrRegOneBit(LED_PIN5_PORT_ICS_1MA7,LED_PIN5_BIT)

#define LED_PIN6_IN_OFF	 GpioClrRegOneBit(LED_PIN6_PORT_ICS_2MA4,LED_PIN6_BIT),\
                                           GpioClrRegOneBit(LED_PIN6_PORT_ICS_1MA7,LED_PIN6_BIT)

#if defined(FUNC_7PIN_SEG_LED_EN)
#define LED_PIN7_IN_OFF	 GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4,LED_PIN7_BIT),\
                                           GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7,LED_PIN7_BIT)

#elif defined(FUNC_9PIN_SEG_LED_EN)
#define LED_PIN7_IN_OFF	 GpioClrRegOneBit(LED_PIN7_PORT_ICS_2MA4,LED_PIN7_BIT),\
                                           GpioClrRegOneBit(LED_PIN7_PORT_ICS_1MA7,LED_PIN7_BIT)

#define LED_PIN8_IN_OFF	 GpioClrRegOneBit(LED_PIN8_PORT_ICS_2MA4,LED_PIN8_BIT),\
                                           GpioClrRegOneBit(LED_PIN8_PORT_ICS_1MA7,LED_PIN8_BIT)

#define LED_PIN9_IN_OFF	 GpioClrRegOneBit(LED_PIN9_PORT_ICS_2MA4,LED_PIN9_BIT),\
                                           GpioClrRegOneBit(LED_PIN9_PORT_ICS_1MA7,LED_PIN9_BIT)
#endif

#endif


void DispLedIcon(ICON_IDX Icon, ICON_STATU Light);
void LedDispDevSymbol(void);
void LedDispRepeat(void);
void LedDispRepeatAB(void);
void LedDispPlayState(void);

void LedAllPinGpioInput(void);
void LedPinGpioDeInit(void);
void LedPinGpioInit(void);
#endif
#endif



