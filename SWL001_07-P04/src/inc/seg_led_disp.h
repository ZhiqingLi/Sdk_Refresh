#ifndef __SEG_LED_DISP__
#define __SEG_LED_DISP__


#include "gpio.h"
#ifdef FUNC_SEG_LED_EN

//LED 屏引脚与GPIO 连接定义,请根据硬件定义
//6只脚段码LED屏示例
#define LED_PIN1_PORT_PU 		GPIO_A_PU
#define LED_PIN1_PORT_PD 		GPIO_A_PD
#define LED_PIN1_PORT_OE 		GPIO_A_OE
#define LED_PIN1_PORT_IE 		GPIO_A_IE
#define LED_PIN1_PORT_ICS_1MA7 	GPIO_A_PULLDOWN1
#define LED_PIN1_PORT_ICS_2MA4 	GPIO_A_PULLDOWN2
#define LED_PIN1_PORT_OUT 		GPIO_A_OUT
#define LED_PIN1_PORT_BIT		GPIOA21

#define LED_PIN2_PORT_PU 		GPIO_A_PU
#define LED_PIN2_PORT_PD 		GPIO_A_PD
#define LED_PIN2_PORT_OE 		GPIO_A_OE
#define LED_PIN2_PORT_IE 		GPIO_A_IE
#define LED_PIN2_PORT_ICS_1MA7 	GPIO_A_PULLDOWN1
#define LED_PIN2_PORT_ICS_2MA4 	GPIO_A_PULLDOWN2
#define LED_PIN2_PORT_OUT 		GPIO_A_OUT
#define LED_PIN2_PORT_BIT		GPIOA20

#define LED_PIN3_PORT_PU 		GPIO_A_PU
#define LED_PIN3_PORT_PD 		GPIO_A_PD
#define LED_PIN3_PORT_OE 		GPIO_A_OE
#define LED_PIN3_PORT_IE 		GPIO_A_IE
#define LED_PIN3_PORT_ICS_1MA7 	GPIO_A_PULLDOWN1
#define LED_PIN3_PORT_ICS_2MA4 	GPIO_A_PULLDOWN2
#define LED_PIN3_PORT_OUT 		GPIO_A_OUT
#define LED_PIN3_PORT_BIT		GPIOA19

#define LED_PIN4_PORT_PU 		GPIO_C_PU
#define LED_PIN4_PORT_PD 		GPIO_C_PD
#define LED_PIN4_PORT_OE 		GPIO_C_OE
#define LED_PIN4_PORT_IE 		GPIO_C_IE
#define LED_PIN4_PORT_ICS_1MA7 	GPIO_C_PULLDOWN1
#define LED_PIN4_PORT_ICS_2MA4 	GPIO_C_PULLDOWN2
#define LED_PIN4_PORT_OUT 		GPIO_C_OUT
#define LED_PIN4_PORT_BIT		GPIOC2

#define LED_PIN5_PORT_PU 		GPIO_C_PU
#define LED_PIN5_PORT_PD 		GPIO_C_PD
#define LED_PIN5_PORT_OE 		GPIO_C_OE
#define LED_PIN5_PORT_IE 		GPIO_C_IE
#define LED_PIN5_PORT_ICS_1MA7 	GPIO_C_PULLDOWN1
#define LED_PIN5_PORT_ICS_2MA4 	GPIO_C_PULLDOWN2
#define LED_PIN5_PORT_OUT 		GPIO_C_OUT
#define LED_PIN5_PORT_BIT		GPIOC11

#define LED_PIN6_PORT_PU 		GPIO_C_PU
#define LED_PIN6_PORT_PD 		GPIO_C_PD
#define LED_PIN6_PORT_OE 		GPIO_C_OE
#define LED_PIN6_PORT_IE 		GPIO_C_IE
#define LED_PIN6_PORT_ICS_1MA7 	GPIO_C_PULLDOWN1
#define LED_PIN6_PORT_ICS_2MA4 	GPIO_C_PULLDOWN2
#define LED_PIN6_PORT_OUT 		GPIO_C_OUT
#define LED_PIN6_PORT_BIT		GPIOC12


//设置LED 各个引脚输出高电平

#define LED_PIN1_IN_ON()	 				do{\
										GpioSetRegOneBit(LED_PIN1_PORT_PU, LED_PIN1_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN1_PORT_PD, LED_PIN1_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN1_PORT_IE, LED_PIN1_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN1_PORT_OE, LED_PIN1_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN1_PORT_OUT, LED_PIN1_PORT_BIT);\
									}while(0)

#define LED_PIN2_IN_ON()					do{\
										GpioSetRegOneBit(LED_PIN2_PORT_PU, LED_PIN2_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN2_PORT_PD, LED_PIN2_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN2_PORT_IE, LED_PIN2_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN2_PORT_OE, LED_PIN2_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN2_PORT_OUT, LED_PIN2_PORT_BIT);\
									}while(0)

#define LED_PIN3_IN_ON()	 				do{\
										GpioSetRegOneBit(LED_PIN3_PORT_PU, LED_PIN3_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN3_PORT_PD, LED_PIN3_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN3_PORT_IE, LED_PIN3_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN3_PORT_OE, LED_PIN3_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN3_PORT_OUT, LED_PIN3_PORT_BIT);\
									}while(0)

#define LED_PIN4_IN_ON()	 				do{\
										GpioSetRegOneBit(LED_PIN4_PORT_PU, LED_PIN4_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN4_PORT_PD, LED_PIN4_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN4_PORT_IE, LED_PIN4_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN4_PORT_OE, LED_PIN4_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN4_PORT_OUT, LED_PIN4_PORT_BIT);\
									}while(0)

#define LED_PIN5_IN_ON()	 				do{\
										GpioSetRegOneBit(LED_PIN5_PORT_PU, LED_PIN5_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN5_PORT_PD, LED_PIN5_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN5_PORT_IE, LED_PIN5_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN5_PORT_OE, LED_PIN5_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN5_PORT_OUT, LED_PIN5_PORT_BIT);\
									}while(0)

#define LED_PIN6_IN_ON()					do{\
										GpioSetRegOneBit(LED_PIN6_PORT_PU, LED_PIN6_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN6_PORT_PD, LED_PIN6_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN6_PORT_IE, LED_PIN6_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN6_PORT_OE, LED_PIN6_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN6_PORT_OUT, LED_PIN6_PORT_BIT);\
									}while(0)


//禁止LED 各个引脚电流输入
#define LED_PIN1_IN_OFF()	 				do{\
										GpioSetRegOneBit(LED_PIN1_PORT_PU, LED_PIN1_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN1_PORT_PD, LED_PIN1_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN1_PORT_IE, LED_PIN1_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN1_PORT_OE, LED_PIN1_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN1_PORT_OUT, LED_PIN1_PORT_BIT);\
									}while(0)
										   
#define LED_PIN2_IN_OFF()					do{\
										GpioSetRegOneBit(LED_PIN2_PORT_PU, LED_PIN2_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN2_PORT_PD, LED_PIN2_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN2_PORT_IE, LED_PIN2_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN2_PORT_OE, LED_PIN2_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN2_PORT_OUT, LED_PIN2_PORT_BIT);\
									}while(0)
										   
#define LED_PIN3_IN_OFF()	 				do{\
										GpioSetRegOneBit(LED_PIN3_PORT_PU, LED_PIN3_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN3_PORT_PD, LED_PIN3_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN3_PORT_IE, LED_PIN3_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN3_PORT_OE, LED_PIN3_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN3_PORT_OUT, LED_PIN3_PORT_BIT);\
									}while(0)
										   
#define LED_PIN4_IN_OFF()	 				do{\
										GpioSetRegOneBit(LED_PIN4_PORT_PU, LED_PIN4_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN4_PORT_PD, LED_PIN4_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN4_PORT_IE, LED_PIN4_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN4_PORT_OE, LED_PIN4_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN4_PORT_OUT, LED_PIN4_PORT_BIT);\
									}while(0)
										   
#define LED_PIN5_IN_OFF()	 				do{\
										GpioSetRegOneBit(LED_PIN5_PORT_PU, LED_PIN5_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN5_PORT_PD, LED_PIN5_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN5_PORT_IE, LED_PIN5_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN5_PORT_OE, LED_PIN5_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN5_PORT_OUT, LED_PIN5_PORT_BIT);\
									}while(0)
										   
#define LED_PIN6_IN_OFF()					do{\
										GpioSetRegOneBit(LED_PIN6_PORT_PU, LED_PIN6_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN6_PORT_PD, LED_PIN6_PORT_BIT);\
										GpioClrRegOneBit(LED_PIN6_PORT_IE, LED_PIN6_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN6_PORT_OE, LED_PIN6_PORT_BIT);\
										GpioSetRegOneBit(LED_PIN6_PORT_OUT, LED_PIN6_PORT_BIT);\
									}while(0)

//LED整体控制

#define LED_ALL_POWER_ON()				do{\
										LED_PIN1_IN_ON();\
										LED_PIN2_IN_ON();\
										LED_PIN3_IN_ON();\
										LED_PIN4_IN_ON();\
										LED_PIN5_IN_ON();\
										LED_PIN6_IN_ON();\
									}while(0)

#define LED_ALL_POWER_OFF()				do{\
										LED_PIN1_IN_OFF();\
										LED_PIN2_IN_OFF();\
										LED_PIN3_IN_OFF();\
										LED_PIN4_IN_OFF();\
										LED_PIN5_IN_OFF();\
										LED_PIN6_IN_OFF();\
									}while(0)


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



