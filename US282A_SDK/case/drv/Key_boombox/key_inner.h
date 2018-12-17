/*
 ********************************************************************************
 *                       ACTOS
 *                  key board message define
 *
 *                (c) Copyright 2002-2013, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : key.msa
 * By     : johnsen
 * Version: 1> v1.00     first version     2013-5-18 13:53
 ********************************************************************************
 ********************************************************************************
 */
#ifndef _KEY_INNER_H			//防止重定义
#define _KEY_INNER_H

//#include "psp_includes.h"
#include "key_interface.h"
#include "gui_msg.h"
#include "app_msg.h"
#include "TWI.h"

//#define SUPPORT_UART_SIM_KEY

//#define lock_key

#define DOWN_KEY_TIMER  3//按键按下消息时间为60ms
#define UP_KEY_TIMER    3//按键抬起消息时间为60ms
#define HOLD_KEY_TIMER  8//按键连续hold消息间隔为160ms

//Hold min and max adc
#define MAX_HOLD_ADC 0x39
#define MIN_HOLD_ADC 0x33

//for gpio define
#define KSIN0_REG_BIT    1<<5
#define KSIN0_EN_REG     GPIOAINEN
#define KSIN0_DATA_REG   GPIOADAT

#define KSIN1_REG_BIT    1<<6
#define KSIN1_EN_REG     GPIOAINEN
#define KSIN1_DATA_REG   GPIOADAT

#define KSIN2_REG_BIT    1<<15
#define KSIN2_EN_REG     GPIOAINEN
#define KSIN2_DATA_REG   GPIOADAT

//for gpio define
#ifdef GPIOB0_CTL /*2014-7新增寄存器*/
#define KSOUT0_EN_REG_BIT   (1<<4)
#define KSOUT0_EN_REG    GPIOB0_CTL
#define KSOUT0_DATA_REG_BIT   (1<<2)
#define KSOUT0_DATA_REG  GPIOB0_CTL

#else

#define KSOUT0_EN_REG_BIT   (1<<0)
#define KSOUT0_EN_REG    GPIOBOUTEN
#define KSOUT0_DATA_REG_BIT   (1<<0)
#define KSOUT0_DATA_REG  GPIOBDAT
#endif

#define KSOUT1_REG_BIT   1<<11
#define KSOUT1_EN_REG    GPIOBOUTEN
#define KSOUT1_DATA_REG  GPIOBDAT

#define KSOUT2_REG_BIT   1<<12
#define KSOUT2_EN_REG    GPIOBOUTEN
#define KSOUT2_DATA_REG  GPIOBDAT

//标识当前按键为红外遥控按键
extern uint8 g_ir_key;
//当前遥控器用户码数据
extern uint8 g_current_usercode;
extern uint8 g_ir_oldkey_val;
extern uint8 g_ir_tmp_count;

//当前为触摸按键
extern uint8 g_tk_key;
extern uint8 g_tk_oldkey_val;
extern uint8 g_tk_tmp_count;

extern uint8 g_oldkey_val;
extern uint8 g_tmpkey_val;
extern uint8 g_tmp_count;
extern uint8 g_hold_count;

extern int g_time_irq;

//for fpga gpio value
extern uint32 in_reg_arrary[][3];

extern uint32 out_reg_arrary[][4];

//for key use

//scan timer interrupt process
extern void key_scan_int_process(void);

//for ir interrupt
extern void key_ir_int_process(void);

//for tk interrupt
extern void key_tk_int_process(void);

//for tk check
extern bool key_tk_up_status_check(void);

extern void key_ir_int_process(void);

extern void key_ir_status_check(void);

extern bool post_key_msg(key_phy_type_e key_type, key_phy_status_e key_status, uint8 key_id);

extern int exp_fast(int db);

//电池及其充电接口
extern void ker_battery_charge_deal(void);
extern void key_inner_battery_charge_open(chg_current_e CurrentMA, chg_voltage_e VoltageMV, void* null2);
extern void key_inner_battery_charge_stop(void* null0, void* null1, void* null2);
battery_status_e key_inner_battery_get_status(uint32 *ad_val, int8* vol_limit, uint8* app_id);
extern void key_bat_charge_init(chg_current_e CurrentMA, chg_voltage_e VoltageMV) __FAR__;
extern void key_sampling_battery_voltage_init(void) __FAR__;
extern void adjust_freq_do_init(void) __FAR__;

extern int adjust_freq_op_entry(void* ,void* ,int ) __FAR__;

extern void key_inner_peripheral_detect_handle(uint32 detect_mode) __FAR__;

#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
extern void key_i2c_tk_init(void) __FAR__;
extern uint8 key_i2c_tk_check(void);
extern uint8 key_inner_extern_tk_op(void* null0, void *null1, tk_op_e op);

extern void TWI_Init(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_Start(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_Stop(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_WriteByte(uint8 dat, gpio_init_cfg_t* gpio, uint8* delay);
extern uint8 TWI_ReadByte(gpio_init_cfg_t* gpio, uint8* delay);
extern void TWI_SendAck(uint8 ack, gpio_init_cfg_t* gpio, uint8* delay);
extern uint8 TWI_GetAck(gpio_init_cfg_t* gpio, uint8* delay);

#endif

extern uint8 g_key_bat_charge_mode;

extern scan_isr_cbk g_key_scan_isr_cbk;

extern uint8 g_i2s_tk_enable;

extern int key_inner_register_scan_isr_cbk(scan_isr_cbk cbk);

extern void key_inner_discrete_led_op(led_port_e port, uint8 pin, discrete_led_op_e op);

extern int32 key_inner_set_effect_param(uint32 set_type, void *param_ptr, void *null3);
extern int32 key_inner_get_feature_info(uint32 set_type, void *info_ptr, void *null3);

extern int32 key_inner_set_waves_effect_param(uint32 set_type, void *param_ptr, void *null3);
extern void dae_param_print(uint8 audiopp_type);
extern void dae_param_print1(uint8 audiopp_type);

extern int32 key_inner_address_translation(uint32 type, void *param_ptr, void *null3);


#endif/*_KEY_H*/
