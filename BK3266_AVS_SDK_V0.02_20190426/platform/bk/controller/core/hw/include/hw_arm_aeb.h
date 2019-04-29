#ifndef _PARTHUS_HW_ARM_AEB_
#define _PARTHUS_HW_ARM_AEB_

/******************************************************************************
 * MODULE NAME:    hw_arm_aeb.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Prototypes of functions to control ARM AEB.
 * MAINTAINER:     Ivan Griffin
 * DATE:           20 March 2000
 *
 * SOURCE CONTROL: $Id: hw_arm_aeb.h,v 1.4 2004/07/07 14:21:19 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    20 March 2000          IG           Initial Version
 *
 ******************************************************************************/

void HWarm_Aeb_Mem_Map_Device(u_int32 base_addr,
    u_int32 stop_addr, u_int8 wait_states, u_int8 segment,
    u_int16 ext_sel_low, u_int16 ext_sel_high);
void HWarm_Aeb_Init(void);


/***************************************************************************
 * 
 * Memory Address Positions
 *
 ***************************************************************************/

/* 
 * Cache 
 */
#define HWarm_LH77790_CCR        0xFFFFA400

/*
 * Local SRAM
 */
#define HWarm_LH77790_LSCR       0xFFFFA404

/*
 * Memory and Peripherals
 */
#define HWarm_LH77790_START0     0xFFFFA000
#define HWarm_LH77790_START1     0xFFFFA004
#define HWarm_LH77790_START2     0xFFFFA008
#define HWarm_LH77790_START3     0xFFFFA00C
#define HWarm_LH77790_START4     0xFFFFA010
#define HWarm_LH77790_START5     0xFFFFA014
#define HWarm_LH77790_START6     0xFFFFA018
#define HWarm_LH77790_START7     0xFFFFA01C

#define HWarm_LH77790_STOP0      0xFFFFA020
#define HWarm_LH77790_STOP1      0xFFFFA024
#define HWarm_LH77790_STOP2      0xFFFFA028
#define HWarm_LH77790_STOP3      0xFFFFA02C
#define HWarm_LH77790_STOP4      0xFFFFA030
#define HWarm_LH77790_STOP5      0xFFFFA034
#define HWarm_LH77790_STOP6      0xFFFFA038
#define HWarm_LH77790_STOP7      0xFFFFA03C

#define HWarm_LH77790_SDR0       0xFFFFA040
#define HWarm_LH77790_SDR1       0xFFFFA044
#define HWarm_LH77790_SDR2       0xFFFFA048
#define HWarm_LH77790_SDR3       0xFFFFA04C
#define HWarm_LH77790_SDR4       0xFFFFA050
#define HWarm_LH77790_SDR5       0xFFFFA054
#define HWarm_LH77790_SDR6       0xFFFFA058
#define HWarm_LH77790_SDR7       0xFFFFA05C
#define HWarm_LH77790_SDR8       0xFFFFA060

#define HWarm_LH77790_BCR0       0xFFFFA100
#define HWarm_LH77790_BCR1       0xFFFFA104
#define HWarm_LH77790_BCR2       0xFFFFA108
#define HWarm_LH77790_BCR3       0xFFFFA10C
#define HWarm_LH77790_BCR4       0xFFFFA110
#define HWarm_LH77790_BCR5       0xFFFFA114
#define HWarm_LH77790_BCR6a      0xFFFFA118
#define HWarm_LH77790_BCR7a      0xFFFFA11C
#define HWarm_LH77790_BCR6b      0xFFFFA120
#define HWarm_LH77790_BCR7b      0xFFFFA124

#define HWarm_LH77790_DRR        0xFFFFA128

/*
 * UART 0
 */
#define HWarm_LH77790_RBR0       0xFFFF0000
#define HWarm_LH77790_THR0       0xFFFF0000
#define HWarm_LH77790_DLL0       0xFFFF0000
#define HWarm_LH77790_IER0       0xFFFF0004
#define HWarm_LH77790_DLM0       0xFFFF0004
#define HWarm_LH77790_IIR0       0xFFFF0008
#define HWarm_LH77790_LCR0       0xFFFF000C
#define HWarm_LH77790_MCR0       0xFFFF0010
#define HWarm_LH77790_LSR0       0xFFFF0014
#define HWarm_LH77790_MSR0       0xFFFF0018
#define HWarm_LH77790_SCR0       0xFFFF001C

/*
 * UART 1
 */
#define HWarm_LH77790_RBR1       0xFFFF0400
#define HWarm_LH77790_THR1       0xFFFF0400
#define HWarm_LH77790_DLL1       0xFFFF0400
#define HWarm_LH77790_IER1       0xFFFF0404
#define HWarm_LH77790_DLM1       0xFFFF0404
#define HWarm_LH77790_IIR1       0xFFFF0408
#define HWarm_LH77790_LCR1       0xFFFF040C
#define HWarm_LH77790_MCR1       0xFFFF0410
#define HWarm_LH77790_LSR1       0xFFFF0414
#define HWarm_LH77790_MSR1       0xFFFF0418
#define HWarm_LH77790_SCR1       0xFFFF041C

/*
 * UART 2
 */
#define HWarm_LH77790_RBR2       0xFFFF0800
#define HWarm_LH77790_THR2       0xFFFF0800
#define HWarm_LH77790_DLL2       0xFFFF0800
#define HWarm_LH77790_IER2       0xFFFF0804
#define HWarm_LH77790_DLM2       0xFFFF0804
#define HWarm_LH77790_IIR2       0xFFFF0808
#define HWarm_LH77790_LCR2       0xFFFF080C
#define HWarm_LH77790_MCR2       0xFFFF0810
#define HWarm_LH77790_LSR2       0xFFFF0814
#define HWarm_LH77790_MSR2       0xFFFF0818
#define HWarm_LH77790_SCR2       0xFFFF081C

/*
 * SIR (IrDA/DASK)
 */
#define HWarm_LH77790_SIRCTLR    0xFFFF0C00

/*
 * PWM 
 */

/*
 * LCD Controller
 */

/*
 * Counters/Timers
 */

/*
 * PPI
 */
#define HWarm_LH77790_PPI_PA    0xFFFF1C00
#define HWarm_LH77790_PPI_PB    0xFFFF1C04
#define HWarm_LH77790_PPI_PC    0xFFFF1C08
#define HWarm_LH77790_PPI_CTLR  0xFFFF1C0C

/*
 * Interrupt Controller
 */
#define HWarm_LH77790_ICR0      0xFFFFA800
#define HWarm_LH77790_ICR1      0xFFFFA804
#define HWarm_LH77790_ICLR      0xFFFFA808
#define HWarm_LH77790_IRQER     0xFFFFA80C
#define HWarm_LH77790_FIQER     0xFFFFA810
#define HWarm_LH77790_IRQSR     0xFFFFA814
#define HWarm_LH77790_FIQSR     0xFFFFA818
#define HWarm_LH77790_IPR       0xFFFFA81C

/*
 * Clock and Power Management
 */
#define HWarm_LH77790_PCSR      0xFFFFAC04
#define HWarm_LH77790_U0CCR     0xFFFFAC08
#define HWarm_LH77790_U1CCR     0xFFFFAC0C
#define HWarm_LH77790_U2CCR     0xFFFFAC10
#define HWarm_LH77790_CT0CCR    0xFFFFAC18
#define HWarm_LH77790_CT1CCR    0xFFFFAC1C
#define HWarm_LH77790_CT2CCR    0xFFFFAC20
#define HWarm_LH77790_CCCR      0xFFFFAC28

/*
 * Watchdog Timer
 */

#define HWarm_LH77790_WDCTLR    0xFFFFAC30
#define HWarm_LH77790_WDCR      0xFFFFAC34

/*
 * I/O Configuration
 */
#define HWarm_LH77790_IOCR      0xFFFFA410

/*
 * Software Reset 
 */
#define HWarm_LH77790_SWRST     0xFFFFAC38

/***************************************************************************
 * 
 * Field Values for BCR and SCR registers - values can be ORed together
 *
 ***************************************************************************/

typedef enum 
{
    /* System Privilege */
    HWarm_LH77790_SDR_SPR_NONE  = 0x00,
    HWarm_LH77790_SDR_SPR_RO    = 0x01 << 13,
    HWarm_LH77790_SDR_SPR_WO    = 0x02 << 13,
    HWarm_LH77790_SDR_SPR_RW    = 0x03 << 13,

    /* User Privilege */
    HWarm_LH77790_SDR_UPR_NONE  = 0x00,
    HWarm_LH77790_SDR_UPR_RO    = 0x01 << 11,
    HWarm_LH77790_SDR_UPR_WO    = 0x02 << 11,
    HWarm_LH77790_SDR_UPR_RW    = 0x03 << 11,

    /* Cacheability */
    HWarm_LH77790_SDR_C_OFF     = 0x00,
    HWarm_LH77790_SDR_C_ON      = 0x01 << 10,

    /* Half Word Mode */
    HWarm_LH77790_SDR_HW_32BIT  = 0x00,
    HWarm_LH77790_SDR_HW_16BIT  = 0x01 << 8,

    /* Bank Select */
    HWarm_LH77790_SDR_HW_BSEL_0 = 0x01,
    HWarm_LH77790_SDR_HW_BSEL_1 = 0x02,
    HWarm_LH77790_SDR_HW_BSEL_2 = 0x04,
    HWarm_LH77790_SDR_HW_BSEL_3 = 0x08,
    HWarm_LH77790_SDR_HW_BSEL_4 = 0x10,
    HWarm_LH77790_SDR_HW_BSEL_5 = 0x20,
    HWarm_LH77790_SDR_HW_BSEL_6 = 0x40,
    HWarm_LH77790_SDR_HW_BSEL_7 = 0x80
} t_HWarm_LH77790_SDR_Fields;

typedef enum
{
    /* External Bus Size */
    HWarm_LH77790_BCR_MS_8BIT   = 0x00,
    HWarm_LH77790_BCR_MS_16BIT  = 0x01 << 15,

    /* Wait Cycles for External Device */
    HWarm_LH77790_BCR_WAIT_0    = 0x00,
    HWarm_LH77790_BCR_WAIT_1    = 0x01 << 12,
    HWarm_LH77790_BCR_WAIT_2    = 0x02 << 12,
    HWarm_LH77790_BCR_WAIT_3    = 0x03 << 12,
    HWarm_LH77790_BCR_WAIT_4    = 0x04 << 12,
    HWarm_LH77790_BCR_WAIT_5    = 0x05 << 12,
    HWarm_LH77790_BCR_WAIT_6    = 0x06 << 12,
    HWarm_LH77790_BCR_WAIT_7    = 0x07 << 12,

    /* External Chip Enable */
    HWarm_LH77790_BCR_CE0_LOW   = 0x00,
    HWarm_LH77790_BCR_CE0_HIGH  = 0x01,
    HWarm_LH77790_BCR_CE1_LOW   = 0x01 << 1,
    HWarm_LH77790_BCR_CE1_HIGH  = 0x01 << 2,
    HWarm_LH77790_BCR_CE2_LOW   = 0x01 << 3,
    HWarm_LH77790_BCR_CE2_HIGH  = 0x01 << 4,
    HWarm_LH77790_BCR_CE3_LOW   = 0x01 << 5,
    HWarm_LH77790_BCR_CE3_HIGH  = 0x01 << 6,
    HWarm_LH77790_BCR_CE4_LOW   = 0x01 << 7,
    HWarm_LH77790_BCR_CE4_HIGH  = 0x01 << 8,
    HWarm_LH77790_BCR_CE5_LOW   = 0x01 << 9,
    HWarm_LH77790_BCR_CE5_HIGH  = 0x01 << 10
} t_HWarm_LH77790_BCR_Fields;
 
#endif
