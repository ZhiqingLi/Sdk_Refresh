/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：TWI GPIO模拟从模组控制模块外部接口声明，这些接口声明只能让内部使用，外部
 *       模块要调用这些函数，必须自己声明，如果是BANK函数调用这些函数，必须将其声
 *       明为 __FAR__ 等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _TWI_INNER_H
#define _TWI_INNER_H

#include "psp_includes.h"

extern uint8 TWI_Trans_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay);
extern uint8 TWI_Recev_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_Start(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_Stop(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_WriteByte(uint8 dat, gpio_init_cfg_t* gpio, uint8* delay);
extern uint8 TWI_ReadByte(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_Init(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_Exit(gpio_init_cfg_t* gpio);
extern void TWI_SendAck(uint8 ack, gpio_init_cfg_t* gpio, uint8* delay);
extern uint8 TWI_GetAck(gpio_init_cfg_t* gpio, uint8* delay);

#endif /*_TWI_INNER_H */

