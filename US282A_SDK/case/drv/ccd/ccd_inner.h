/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：外部pa驱动接口
 * 作者：Gary Wang
 ********************************************************************************/

#ifndef _CCD_INNER_H            //防止重定义
#define _CCD_INNER_H

#include "ccd_interface.h"
#include "PA_TAS5709.h"
#include "STA339BWS.h"
#include "TWI.h"

extern uint8 g_i2s_pa_status;
extern uint8 g_i2s_pa_delay_twi;
extern uint8 g_i2s_pa_drc_enable;

typedef void* (*ccd_op_i)(void*, void*, void*);

void pa_op_set_volume(uint16 vol) __FAR__;

uint16 pa_op_get_volume(void) __FAR__;

void pa_op_poweron(void) __FAR__;

void pa_op_powerdown(void) __FAR__;

void pa_op_set_clock(uint8 rate) __FAR__;

//for twi interface
uint8 TWI_Trans_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

uint8 TWI_Recev_Bytes(uint8 *buf, uint8 address, uint8 length, gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

void TWI_Init(gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

void TWI_Exit(gpio_init_cfg_t* gpio) __FAR__;

void TWI_Start(gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

void TWI_Stop(gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

void TWI_WriteByte(uint8 dat, gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

uint8 TWI_ReadByte(gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

void TWI_SendAck(uint8 ack, gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

uint8 TWI_GetAck(gpio_init_cfg_t* gpio, uint8* delay) __FAR__;

#endif/*_CCD_INNER_H*/
