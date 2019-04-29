/******************************************************************************
 * MODULE NAME:    sys_irq.h
 * PROJECT CODE:   Bluetooth
 * DESCRIPTION:    Hardware Interrupt Functions
 * MAINTAINER:     Tom Kerwick
 * DATE:           10 Jan 2011
 *
 * SOURCE CONTROL: $Id: sys_irq.h,v 1.1 2011/05/17 16:30:39 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/
#ifndef _PARTHUS_HW_IRQ_
#define _PARTHUS_HW_IRQ_

#include "sys_types.h"

/*vector interrupt controller mast register bit macro*/
#define VIC_SDIO_ISR_INDEX            (0)
#define VIC_SADC_ISR_INDEX            (1)
#define VIC_AUD_ISR_INDEX             (2)
#define VIC_PWM0_ISR_INDEX            (3)
#define VIC_PWM1_ISR_INDEX            (4)
#define VIC_PWM2_ISR_INDEX            (5)
#define VIC_PWM3_ISR_INDEX            (6)
#define VIC_PWM4_ISR_INDEX            (7)
#define VIC_PWM5_ISR_INDEX            (8)
#define VIC_GPIO_ISR_INDEX            (9)
#define VIC_SPI_ISR_INDEX             (10)
#define VIC_I2C1_ISR_INDEX             (11)
#define VIC_I2C2_ISR_INDEX            (12)
#define VIC_UART_ISR_INDEX            (13)
#define VIC_SPI2_ISR_INDEX             (14)
#define VIC_IRDA_ISR_INDEX            (15)
#define VIC_RTC_ISR_INDEX              (16)
#define VIC_FFT_ISR_INDEX              (17)
#define VIC_DMA_ISR_INDEX             (18)
#define VIC_CEVA_ISR_INDEX            (19)
#define VIC_USB_ISR_INDEX              (20)
#define VIC_USB_PLUG_INDEX           (21)

DRAM_CODE void SYSirq_Disable_Interrupts_Save_Flags(u_int32* flags, u_int32* mask);
DRAM_CODE void SYSirq_Interrupts_Restore_Flags(u_int32 flags, u_int32 mask);
DRAM_CODE void SYSirq_Interrupts_Clear_Trig_Flags(void);

void SYSirq_Initialise(void);
void SYSirq_clear(void);
void SYSirq_set(void);
void SYSirq_Disable_Interrupts_Except(u_int32 *oldflags, u_int32 flags);
void SYSirq_Enable_All_Interrupts(u_int32 flags);
void SYSirq_Unmask_Interrupt(u_int32 *oldflags,u_int32 flags);

#endif
