#ifndef _PARTHUS_HW_MCU_IMPL_
#define _PARTHUS_HW_MCU_IMPL_

/*
 * MODULE NAME:    bk300_mcu.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Functions to access pcm on Parthus Chimera
 * MAINTAINER:     Ivan Grifffin
 * DATE:           
 *
 * SOURCE CONTROL: $Id: hw_leds_impl.h,v 1.9 2004/07/07 14:30:48 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 BEKEN Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    1 June 2011 -   ccl       - Initial Version 
 *
 */
#include "sys_config.h"
#include "sys_types.h"

#include "bk3000_reg.h"

#define CPU_26M_CLK  1
#define CPU_52M_CLK  2
#define CPU_104M_CLK 3

typedef struct _struct_beken_config {
	u_int8 g_enable_32k_sleep;
	u_int8 g_CPU_Halt_mode;    //0:normal mode; 1:only stop CPU when acl number = 0; 2:not stop CPU
	u_int8 g_enable_gpio_eint_wakeup;
	u_int8 g_Disable_ACL_active_check_when_Sleep;
	u_int8 g_Enable_TX_Power_Control;
	u_int8 cur_cpu_clk;
    u_int8 Enable_Role_Switch; 
    u_int8 Enable_Qos;
} struct_beken_config;

RAM_CODE void BK3000_cpu_halt(void);
DRAM_CODE void BK3000_cpu_pre_halt(void);
#if (BK3000_RF_DPLL_OPTIMIZATION == 1)
DRAM_CODE void BK3000_Open_DPLL(u_int8 enable);
#endif
DRAM_CODE void BK3000_set_clock (int clock_sel, int div);

extern u_int32 XVR_analog_reg_save[16];

extern struct_beken_config g_beken_initial_config;

#define Open_Buck() {  XVR_analog_reg_save[9] &= ~BIT_29; BK3000_XVR_REG_0x09=XVR_analog_reg_save[9]; }

#define Close_Buck() {XVR_analog_reg_save[9] |= BIT_29; BK3000_XVR_REG_0x09=XVR_analog_reg_save[9]; }


#define Open_26M_XTAL() {  XVR_analog_reg_save[9] &= ~BIT_28; BK3000_XVR_REG_0x09=XVR_analog_reg_save[9];\
XVR_analog_reg_save[0xa] &= ~BIT_12; BK3000_XVR_REG_0x0A=XVR_analog_reg_save[0xa];\
 }

#define Close_26M_XTAL() {XVR_analog_reg_save[9] |= BIT_28; BK3000_XVR_REG_0x09=XVR_analog_reg_save[9]; \
XVR_analog_reg_save[0xa] |= BIT_12; BK3000_XVR_REG_0x0A=XVR_analog_reg_save[0xa];\
 }

#endif
