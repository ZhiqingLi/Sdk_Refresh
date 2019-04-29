#ifndef _bk3000_CHIP_H_
#define _bk3000_CHIP_H_

/*
 * MODULE NAME:    bk3000_reg.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Functions to access pcm on Parthus Chimera
 * MAINTAINER:     ccl
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
#include "config.h"

#define ENABLE   1
#define DISABLE  0

/* #define REG32(x)            (*((volatile u_int32 *)(x))) */

#define BIT_0               0x00000001
#define BIT_1               0x00000002
#define BIT_2               0x00000004
#define BIT_3               0x00000008
#define BIT_4               0x00000010
#define BIT_5               0x00000020
#define BIT_6               0x00000040
#define BIT_7               0x00000080
#define BIT_8               0x00000100
#define BIT_9               0x00000200
#define BIT_10              0x00000400
#define BIT_11              0x00000800
#define BIT_12              0x00001000
#define BIT_13              0x00002000
#define BIT_14              0x00004000
#define BIT_15              0x00008000
#define BIT_16              0x00010000
#define BIT_17              0x00020000
#define BIT_18              0x00040000
#define BIT_19              0x00080000
#define BIT_20              0x00100000
#define BIT_21              0x00200000
#define BIT_22              0x00400000
#define BIT_23              0x00800000
#define BIT_24              0x01000000
#define BIT_25              0x02000000
#define BIT_26              0x04000000
#define BIT_27              0x08000000
#define BIT_28              0x10000000
#define BIT_29              0x20000000
#define BIT_30              0x40000000 
#define BIT_31              0x80000000

#define Beken_Write_Register(addr, data) (*((volatile u_int32 *)(addr))) = (u_int32)(data);
#define Beken_Read_Register(addr) *(volatile u_int32 *)(addr);

// AMBA Address Mapping
//////////////////////////////////////
// AHB
#define BK3000_PMU_BASE_ADDR              0x00800000
#define BK3000_FLASH_BASE_ADDR            0x00801000
#define BK3000_DMA_BASE_ADDR              0x00900000
#define BK3000_FFT_BASE_ADDR              0x00A00000
#define BK3000_USB_BASE_ADDR              0x00B00000
#define BK3000_USBMEM_BASE                0x00D00000
// APB
#define BK3000_XVR_BASE_ADDR    	     0x00F00000
#define BK3000_UART_BASE_ADDR             0x00F08000
#define BK3000_I2C1_BASE_ADDR              0x00F10000
#define BK3000_PWM2_BASE_ADDR            0x00F18000
#define BK3000_IRDA_BASE_ADDR             0x00F20000
#define BK3000_I2C2_BASE_ADDR              0x00F28000
#define BK3000_SPI2_BASE_ADDR             0x00F30000
#define BK3000_SPI_BASE_ADDR                0x00F38000
#define BK3000_GPIO_BASE_ADDR             0x00F40000
#define BK3000_WDT_BASE_ADDR              0x00F48000
#define BK3000_PWM0_BASE_ADDR            0x00F50000
#define BK3000_PWM1_BASE_ADDR            0x00F58000
#define BK3000_AUD_BASE_ADDR               0x00F60000
#define BK3000_SADC_BASE_ADDR             0x00F68000
#define BK3000_CEVA_BASE_ADDR   	     0x00F70000
#define BK3000_SDIO_BASE_ADDR             0x00F80000

// PMU define
#define BK3000_PMU_CONFIG           (*((volatile unsigned int *)(BK3000_PMU_BASE_ADDR + 0*4)))
#define BK3000_PMU_PERI_PWDS        (*((volatile unsigned int *)(BK3000_PMU_BASE_ADDR + 1*4)))
#define BK3000_PMU_DQMEM_CFG        (*((volatile unsigned int *)(BK3000_PMU_BASE_ADDR + 2*4)))

#define BK3000_PMU_GATE_CFG         (*((volatile unsigned long *)(BK3000_PMU_BASE_ADDR + 3*4)))
#define BK3000_PMU_WAKEUP_INT_MASK  (*((volatile unsigned long *)(BK3000_PMU_BASE_ADDR + 4*4)))
#define BK3000_PMU_WAKEUP_WTCNT     (*((volatile unsigned long *)(BK3000_PMU_BASE_ADDR + 5*4)))

#define BK3000_PMU_SLEEP_MASK_INTR      (*((volatile unsigned long *)(BK3000_PMU_BASE_ADDR + 6*4)))

#define CPU_SLEEP    do{BK3000_PMU_CONFIG|=0x01; while(BK3000_PMU_CONFIG&0x01);}while(0)

///ICU register
#define BK3000_ICU_REG0_SYS_CLOCK_CONFIG        (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x00)))
#define BK3000_ICU_REG1_CPU_CLOCK_CONFIG        (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x01*4)))

#define sft_PMU_CPU_CLK_DIV               1
#define sft_PMU_CPU_CLK_SEL               8
#define PMU_AHB_CLK_ALWAYS_ON             (1<<10)

#define bit_PMU_DMA_PWD                   (1<<0)
#define bit_PMU_FFT_PWD                   (1<<1)
#define bit_PMU_UART_PWD                  (1<<2)
#define bit_PMU_IRDA_PWD                  (1<<3)
#define bit_PMU_I2C1_PWD                  (1<<4) //SPI2
#define bit_PMU_I2S_PWD                    (1<<5)
#define bit_PMU_I2C2_PWD                  (1<<6)
#define bit_PMU_PCM2_PWD                  (1<<7)
#define bit_PMU_SPI_PWD                   (1<<8)
#define bit_PMU_PWM0_PWD                  (1<<9)
#define bit_PMU_PWM1_PWD                  (1<<10)
#define bit_PMU_AUD_PWD                   (1<<11)
#define bit_PMU_ADC_PWD                   (1<<12)
#define bit_PMU_SDIO_PWD                  (1<<13)
#define bit_PMU_CLK13M_PWD                (1<<20)
#define bit_PMU_USB_PWD                   (1<<21)
#define bit_PMU_CEVA_PWD                  (1<<22)
#define bit_PMU_LPO_CLK_DISABLE           (1<<23)
#define bit_PMU_LPO_CLK_STEAL_ENABLE      (1<<24)
#define bit_PMU_LPO_CLK_SEL_XTAL          (1<<25)
#define bit_PWM2_CLK_SEL_XTAL          (1<<27)
#define bit_PWM2_CLK_DIV                    (1<<28)

#define bit_PMU_CLK_FREQ_SEL              (1<<31)

#define PMU_WDT_CLK_SEL_ROSC_DIV2         (3<<14)
#define PMU_WDT_CLK_SEL_ROSC_DIV4         (2<<14)
#define PMU_WDT_CLK_SEL_ROSC_DIV8         (1<<14)
#define PMU_WDT_CLK_SEL_ROSC_DIV16        (0<<14)

#define SDIO_CLK_SEL_BIT0                 (1 << 16)
#define SDIO_CLK_SEL_BIT1                 (1 << 17)
#define PMU_SDIO_CLK_SEL_13M              (0<<16)
#define PMU_SDIO_CLK_SEL_6_5M             (1<<16)
#define PMU_SDIO_CLK_SEL_3_25M            (2<<16)
#define PMU_SDIO_CLK_SEL_1_625M           (3<<16)

#define PMU_ADC_CLK_SEL_XTAL              (0<<18)
#define PMU_ADC_CLK_SEL_ROSC              (1<<18)

#define PMU_PWM1_CLK_SEL_XTAL             (0<<19)
#define PMU_PWM1_CLK_SEL_ROSC             (1<<19)

#define BK3000_ICU_REG2_CEVA_CLOCK_CONFIG       (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x02*4)))
#define BK3000_ICU_REG3_UART_CLOCK_CONFIG       (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x03*4)))
#define BK3000_ICU_REG4_I2C_CLOCK_CONFIG        (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x04*4)))
#define BK3000_ICU_REG5_PWM_CLOCK_CONFIG        (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x05*4)))
#define BK3000_ICU_REG6_WDT_CLOCK_CONFIG        (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x06*4)))
#define BK3000_ICU_REG7_LPO_CLOCK_CONFIG        (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x07*4)))

#define BK3000_ICU_REG8_INTR_ENABLE             (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x08*4)))
#define BK3000_ICU_REG9_INTR_CLEAR              (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x09*4)))
#define BK3000_ICU_REGa_INTR_STATUS             (*((volatile unsigned long *)   (BK3000_PMU_BASE_ADDR+0x0A*4)))

#define CLK_ROSC        0   /*32Khz*/
#define CLK_XTAL        1   /*26Mhz*/
#define CLK_PLL0        2   /*52Mhz*/
#define CLK_PLL1        3   /*104Mhz*/


#define bit_CLK_DISABLE      BIT_0
#define bit_CLK_DIV(x)      (((x) << 1) & 0x000000fe)
#define bit_CLK_SEL(x)      (((x) << 8) & 0x00000300)

#define bit_EXTERNAL_INT0     BIT_0
#define bit_EXTERNAL_INT1     BIT_1
#define bit_PT0_INTR          BIT_2
#define bit_PT1_INTR          BIT_3
#define bit_PT2_INTR          BIT_4
#define bit_I2C_INTR          BIT_5
#define bit_UART_INTR         BIT_6
#define bit_CEVA_INTR         BIT_7

#define bit_IRQ_INTR_ENABLE         BIT_8
#define bit_FIQ_INTR_ENABLE         BIT_9

#define bit_ALL_IRQ_INTR         (bit_EXTERNAL_INT0|bit_EXTERNAL_INT1|bit_PT0_INTR|bit_PT1_INTR|bit_I2C_INTR|bit_UART_INTR)

#define bit_EXTERNAL_INT0_WAKEUP            BIT_16
#define bit_EXTERNAL_INT1_WAKEUP            BIT_17
#define bit_PT0_INTR_WAKEUP                 BIT_18
#define bit_PT1_INTR_WAKEUP                 BIT_19
#define bit_PCM_INTR_WAKEUP                 BIT_20
#define bit_I2C_INTR_WAKEUP                 BIT_21
#define bit_UART_INTR_WAKEUP                BIT_22
#define bit_CEVA_INTR_WAKEUP                BIT_23

#define bitpos_EXTERNAL_INT0_CONFIG     (10)
#define bitpos_EXTERNAL_INT1_CONFIG     (13)

//CEVA
#define BK3000_CEVA_BD_ADDR_LOW              (*((volatile unsigned long *)   (BK3000_CEVA_BASE_ADDR+0x1800*4)))
#define BK3000_CEVA_BD_ADDR_HIGH             (*((volatile unsigned long *)   (BK3000_CEVA_BASE_ADDR+0x1801*4)))

//////////////////////////////////////
// Pherial Register's Define  
//////////////////////////////////////
//FLASH
#define BIT_ADDRESS_SW                 0
#define BIT_OP_TYPE_SW                 24
#define BIT_OP_SW                      29
#define BIT_WP_VALUE                   30
#define BIT_BUSY_SW                    31

#define SET_ADDRESS_SW                 0xFFFFFF << BIT_ADDRESS_SW
#define SET_OP_TYPE_SW                 0x1F     << BIT_OP_TYPE_SW
#define SET_OP_SW                      0x1      << BIT_OP_SW
#define SET_WP_VALUE                   0x1      << BIT_WP_VALUE
#define SET_BUSY_SW                    0x1      << BIT_BUSY_SW

#define BIT_FLASH_CLK_CONF             0
#define BIT_MODE_SEL                   4
#define BIT_FWREN_FLASH_CPU            9
#define BIT_WRSR_DATA                  10
#define BIT_CRC_EN                     26

#define SET_FLASH_CLK_CONF             0xF      << BIT_FLASH_CLK_CONF
#define SET_MODE_SEL                   0x1F     << BIT_MODE_SEL
#define SET_FWREN_FLASH_CPU            0x1      << BIT_FWREN_FLASH_CPU
#define SET_WRSR_DATA                  0xFFFF   << BIT_WRSR_DATA
#define SET_CRC_EN                     0x1      << BIT_CRC_EN

#define BIT_SR_DATA_FLASH              0
#define BIT_CRC_ERR_COUNTER            8
#define BIT_DATA_FLASH_SW_SEL          16
#define BIT_DATA_SW_FLASH_SEL          19

#define SET_SR_DATA_FLASH              0xFF     << BIT_SR_DATA_FLASH
#define SET_CRC_ERR_COUNTER            0xFF     << BIT_CRC_ERR_COUNTER
#define SET_DATA_FLASH_SW_SEL          0x7      << BIT_DATA_FLASH_SW_SEL
#define SET_DATA_SW_FLASH_SEL          0x7      << BIT_DATA_SW_FLASH_SEL

#define reg_FLASH_OPERATE_SW           (*((volatile unsigned long *)    (BK3000_FLASH_BASE_ADDR+0*4)))
#define reg_FLASH_DATA_SW_FLASH        (*((volatile unsigned long *)    (BK3000_FLASH_BASE_ADDR+1*4)))
#define reg_FLASH_DATA_FLASH_SW        (*((volatile unsigned long *)    (BK3000_FLASH_BASE_ADDR+2*4)))
#define reg_FLASH_RDID_DATA_FLASH      (*((volatile unsigned long *)    (BK3000_FLASH_BASE_ADDR+4*4)))
#define reg_FLASH_SR_DATA_CRC_CNT      (*((volatile unsigned long *)    (BK3000_FLASH_BASE_ADDR+5*4)))
#define reg_FLASH_CONF                 (*((volatile unsigned long *)    (BK3000_FLASH_BASE_ADDR+7*4)))

// DMA
#define BK3000_DMA_DMA_CONTROL               (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+0*4)))
#define BK3000_DMA_DMA_CH0_CTL               (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+1*4)))
#define BK3000_DMA_DMA_CH0_SA                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+2*4)))
#define BK3000_DMA_DMA_CH0_DA                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+3*4)))
#define BK3000_DMA_DMA_CH0_SZ                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+4*4)))
#define BK3000_DMA_DMA_CH1_CTL               (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+5*4)))
#define BK3000_DMA_DMA_CH1_SA                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+6*4)))
#define BK3000_DMA_DMA_CH1_DA                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+7*4)))
#define BK3000_DMA_DMA_CH1_SZ                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+8*4)))
#define BK3000_DMA_DMA_CH2_CTL               (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+9*4)))
#define BK3000_DMA_DMA_CH2_SA                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+10*4)))
#define BK3000_DMA_DMA_CH2_DA                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+11*4)))
#define BK3000_DMA_DMA_CH2_SZ                (*((volatile unsigned int *)(BK3000_DMA_BASE_ADDR+12*4)))

#define sft_DMA_CONF_ROUND_ROBIN       15
#define sft_DMA_CONF_DMA2_SRC_SEL      6
#define sft_DMA_CONF_DMA1_SRC_SEL      3
#define sft_DMA_CONF_DMA0_SRC_SEL      0

#define sft_DMA_CTL_DT          14
#define sft_DMA_CTL_DSTINCR     12
#define sft_DMA_CTL_SRCINCR     10
#define sft_DMA_CTL_DSTBYTE     8
#define sft_DMA_CTL_SRCBYTE     6
#define sft_DMA_CTL_LEVEL       5
#define sft_DMA_CTL_IE          4
#define sft_DMA_CTL_EN          3
#define sft_DMA_CTL_REQ         2
#define sft_DMA_CTL_ABORT       1
#define sft_DMA_CTL_IFG         0

#define DMA_DATA_WIDTH_SEL_8            (0)
#define DMA_DATA_WIDTH_SEL_16           (1)
#define DMA_DATA_WIDTH_SEL_32           (2)

#define DMA_ADDR_MODE_UNCHANGED0        (0)
#define DMA_ADDR_MODE_UNCHANGED1        (1)
#define DMA_ADDR_MODE_DECREASE          (2)
#define DMA_ADDR_MODE_INCREASE          (3)

#define DMA_TRANSFER_MODE_SINGLE        (0)
#define DMA_TRANSFER_MODE_BLOCK         (1)
#define DMA_TRANSFER_MODE_RE_SINGLE     (2)
#define DMA_TRANSFER_MODE_RE_BLOCK      (3)

// FFT
#define BK3000_FFT_FFT_CONF                  (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+0*4)))
#define BK3000_FFT_FIR_CONF                  (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+1*4)))
#define BK3000_FFT_DATA_PORT                 (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+2*4)))
#define BK3000_FFT_COEF_PORT                 (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+3*4)))
#define BK3000_FFT_MAC_LOW                   (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+4*4)))
#define BK3000_FFT_MAC_HIGH                  (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+5*4)))
#define BK3000_FFT_FFT_STATUS                (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+6*4)))
#define BK3000_FFT_FFT_START                 (*((volatile unsigned long *)(BK3000_FFT_BASE_ADDR+7*4)))

#define FFT_MEMORY_BASE_ADDR              (BK3000_FFT_BASE_ADDR+0x800)

#define sft_FFT_CONF_FFT_MODE             0
#define stf_FFT_CONF_IFFT                 1
#define sft_FFT_CONF_FFT_INT_EN           2
#define sft_FFT_CONF_FFT_ENABLE           3

#define sft_FIR_CONF_FIR_LEN              0
#define sft_FIR_CONF_FIR_CWIDTH           8
#define sft_FIR_CONF_FIR_DWIDTH           11
#define sft_FIR_CONF_FIR_MODE             14
#define sft_FIR_CONF_FIR_INT_EN           15
#define sft_FIR_CONF_FIR_ENABLE           16

#define bit_FFT_STATUS_FFT_DONE           ( 1 << 0 )
#define bit_FFT_STATUS_FIR_DONE           ( 1 << 1 )

#define Fft_Set_Start()                   (BK3000_FFT_FFT_START=0x1)
#define Fft_Write_Data(v)                 (BK3000_FFT_DATA_PORT=v)
#define Fft_Read_Data()                   (BK3000_FFT_DATA_PORT) 
#define Fft_Wrtie_Coef(v)                 (BK3000_FFT_COEF_PORT=v) 




//XVR
/*#define BK3000_XVR_POWER_UP         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x24)))*//* Zhigang, 2011-8-24*/
#define BK3000_XVR_REG_0x00		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x00*4)))
#define BK3000_XVR_REG_0x01		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x01*4)))
#define BK3000_XVR_REG_0x02		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x02*4)))
#define BK3000_XVR_REG_0x03		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x03*4)))
#define BK3000_XVR_REG_0x04		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x04*4)))
#define BK3000_XVR_REG_0x05		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x05*4)))
#define BK3000_XVR_REG_0x06		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x06*4)))
#define BK3000_XVR_REG_0x07		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x07*4)))
#define BK3000_XVR_REG_0x08		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x08*4)))
#define BK3000_XVR_REG_0x09		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x09*4)))
#define BK3000_XVR_REG_0x0A		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x0A*4)))
#define BK3000_XVR_REG_0x0B		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x0B*4)))
#define BK3000_XVR_REG_0x0C		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x0C*4)))
#define BK3000_XVR_REG_0x0D		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x0D*4)))
#define BK3000_XVR_REG_0x0E		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x0E*4)))
#define BK3000_XVR_REG_0x0F		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x0F*4)))
#define BK3000_XVR_REG_0x10		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x10*4)))
#define BK3000_XVR_REG_0x11		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x11*4)))
#define BK3000_XVR_REG_0x12		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x12*4)))
#define BK3000_XVR_REG_0x13		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x13*4)))
#define BK3000_XVR_REG_0x14		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x14*4)))
#define BK3000_XVR_REG_0x15		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x15*4)))
#define BK3000_XVR_REG_0x16		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x16*4)))
#define BK3000_XVR_REG_0x17		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x17*4)))
#define BK3000_XVR_REG_0x18		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x18*4)))
#define BK3000_XVR_REG_0x19		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x19*4)))
#define BK3000_XVR_REG_0x1A		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x1A*4)))
#define BK3000_XVR_REG_0x1B		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x1B*4)))
#define BK3000_XVR_REG_0x1C		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x1C*4)))
#define BK3000_XVR_REG_0x1D		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x1D*4)))
#define BK3000_XVR_REG_0x1E		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x1E*4)))
#define BK3000_XVR_REG_0x1F		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x1F*4)))
#define BK3000_XVR_REG_0x20		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x20*4)))
#define BK3000_XVR_REG_0x21		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x21*4)))
#define BK3000_XVR_REG_0x22		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x22*4)))
#define BK3000_XVR_REG_0x23		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x23*4)))
#define BK3000_XVR_REG_0x24		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x24*4)))
#define BK3000_XVR_REG_0x25		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x25*4)))
#define BK3000_XVR_REG_0x26		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x26*4)))
#define BK3000_XVR_REG_0x27		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x27*4)))
#define BK3000_XVR_REG_0x28		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x28*4)))
#define BK3000_XVR_REG_0x29		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x29*4)))
#define BK3000_XVR_REG_0x2A		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x2A*4)))
#define BK3000_XVR_REG_0x2B		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x2B*4)))
#define BK3000_XVR_REG_0x2C		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x2C*4)))
#define BK3000_XVR_REG_0x2D		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x2D*4)))
#define BK3000_XVR_REG_0x2E		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x2E*4)))
#define BK3000_XVR_REG_0x2F		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x2F*4)))
#define BK3000_XVR_REG_0x30		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x30*4)))
#define BK3000_XVR_REG_0x31		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x31*4)))
#define BK3000_XVR_REG_0x32		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x32*4)))
#define BK3000_XVR_REG_0x33		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x33*4)))
#define BK3000_XVR_REG_0x34		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x34*4)))
#define BK3000_XVR_REG_0x35		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x35*4)))
#define BK3000_XVR_REG_0x36		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x36*4)))
#define BK3000_XVR_REG_0x37		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x37*4)))
#define BK3000_XVR_REG_0x38		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x38*4)))
#define BK3000_XVR_REG_0x39		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x39*4)))
#define BK3000_XVR_REG_0x3A		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x3A*4)))
#define BK3000_XVR_REG_0x3B		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x3B*4)))
#define BK3000_XVR_REG_0x3C		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x3C*4)))
#define BK3000_XVR_REG_0x3D		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x3D*4)))
#define BK3000_XVR_REG_0x3E		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x3E*4)))
#define BK3000_XVR_REG_0x3F		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x3F*4)))
#define BK3000_XVR_REG_0x40		         (*((volatile unsigned long *)   (BK3000_XVR_BASE_ADDR+0x40*4)))

//watchdog
#define REG_APB7_WDT_CFG                (*((volatile unsigned long *)   (BK3000_WDT_BASE_ADDR+0x00*4)))


//I2C
#define BK3000_I2C_REG0_CN              (*((volatile unsigned long *)   (BK3000_I2C_BASE_ADDR+0x00*4)))
#define BK3000_I2C_REG1_STAT            (*((volatile unsigned long *)   (BK3000_I2C_BASE_ADDR+0x01*4)))
#define BK3000_I2C_REG2_DAT             (*((volatile unsigned long *)   (BK3000_I2C_BASE_ADDR+0x02*4)))

#define bitpos_I2C_CN_IDLE_CR_DEDECT_THREASHOLD     (0)
#define bitpos_I2C_CN_SCL_CR_LOW_DETECT_THREASHOLD  (3)
#define bitpos_I2C_CN_FREQ_DIV                      (6)
#define bitpos_I2C_CN_SLAVE_ADDR                    (16)
#define bitpos_I2C_CN_CLOCK_SOURCE                  (26)

#define bitpos_I2C_CN_SMBTOE                        (28)
#define bitpos_I2C_CN_SMBFTE                        (29)
#define bitpos_I2C_CN_INH                           (30)
#define bitpos_I2C_CN_ENSMB                         (31)


#define bit_I2C_STAT_SI                                BIT_0
#define bit_I2C_STAT_SCL_TMOT                          (1<<1)
#define bit_I2C_STAT_ARBLOST                           (1<<3)
#define bit_I2C_STAT_RXFIFO_EMPTY                      (1<<4)
#define bit_I2C_STAT_TXFIFO_FULL                       (1<<5)

#define bit_I2C_STAT_RXINT_MODE                        (1<<6)
#define bit_I2C_STAT_TXINT_MODE                        (1<<7)
#define bit_I2C_STAT_ACK                               (1<<8)

#define bit_I2C_STAT_STO                               (1<<9)
#define bit_I2C_STAT_STA                               (1<<10)
#define bit_I2C_STAT_ADDR_MATCH                        (1<<11)

#define bit_I2C_STAT_ACKRQ                             (1<<12)
#define bit_I2C_STAT_TXMODE                            (1<<13)

#define bit_I2C_STAT_MASTER                            (1<<14)
#define bit_I2C_STAT_BUSY                              (1<<15)

//GPIO
#define BK3000_GPIO_0_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+0*4)))
#define BK3000_GPIO_1_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+1*4)))
#define BK3000_GPIO_2_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+2*4)))
#define BK3000_GPIO_3_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+3*4)))
#define BK3000_GPIO_4_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+4*4)))
#define BK3000_GPIO_5_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+5*4)))
#define BK3000_GPIO_6_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+6*4)))
#define BK3000_GPIO_7_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+7*4)))
#define BK3000_GPIO_8_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+8*4)))
#define BK3000_GPIO_9_CONFIG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+9*4)))
#define BK3000_GPIO_10_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+10*4)))
#define BK3000_GPIO_11_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+11*4)))
#define BK3000_GPIO_12_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+12*4)))
#define BK3000_GPIO_13_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+13*4)))
#define BK3000_GPIO_14_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+14*4)))
#define BK3000_GPIO_15_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+15*4)))
#define BK3000_GPIO_16_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+16*4)))
#define BK3000_GPIO_17_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+17*4)))
#define BK3000_GPIO_18_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+18*4)))
#define BK3000_GPIO_19_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+19*4)))
#define BK3000_GPIO_20_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+20*4)))
#define BK3000_GPIO_21_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+21*4)))
#define BK3000_GPIO_22_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+22*4)))
#define BK3000_GPIO_23_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+23*4)))
#define BK3000_GPIO_24_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+24*4)))
#define BK3000_GPIO_25_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+25*4)))
#define BK3000_GPIO_26_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+26*4)))

#define BK3000_GPIO_39_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+39*4)))
#define BK3000_GPIO_45_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+45*4)))
#define BK3000_GPIO_47_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+47*4)))

#define BK3000_GPIO_60_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+60*4)))



#define BK3000_GPIO_56_CONFIG                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+56*4)))
#define READ_CHIPID                          (BK3000_GPIO_56_CONFIG & 0x00ffffff)

#define BK3000_GPIO_ANABUCKFERQ              (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+31*4)))
#define BK3000_GPIO_GPIOCON                  (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+32*4)))
#define BK3000_GPIO_INTEN                    (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+33*4)))
#define BK3000_GPIO_INTLV                    (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+34*4)))
#define BK3000_GPIO_INTSTA                   (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+35*4)))
#define BK3000_GPIO_DPSLP                    (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+36*4)))
#define BK3000_GPIO_WKUPEN                   (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+37*4)))
#define BK3000_GPIO_DPLL_UNLOCK              (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+38*4)))
#define BK3000_GPIO_DRB                      (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+39*4)))
#define BK3000_GPIO_ENC_WORD                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+40*4)))
#define BK3000_GPIO_DEBUGMSG                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+41*4)))
#define BK3000_GPIO_LATCHEN                  (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+42*4)))
#define BK3000_GPIO_GPIODCON                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+43*4)))
#define BK3000_GPIO_CLK26MOEN                (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+44*4)))

#define BK3000_GPIO_PAD_CTRL                 (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+45*4)))

#define BK3000_GPIO_DEEP_SLEEP_LATCH         (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+47*4)))


#define BK3000_A0_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+48*4)))
#define BK3000_A1_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+49*4)))
#define BK3000_A2_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+50*4)))
#define BK3000_A3_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+51*4)))
#define BK3000_A4_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+52*4)))
#define BK3000_A5_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+53*4)))
#define BK3000_A6_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+54*4)))
#define BK3000_A7_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+55*4)))

#define BK3000_A8_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+56*4)))
#define BK3000_A9_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+57*4)))
#define BK3000_AA_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+58*4)))
#define BK3000_AB_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+59*4)))
#define BK3000_AC_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+60*4)))
#define BK3000_AD_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+61*4)))
#define BK3000_AE_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+62*4)))
#define BK3000_AF_CONFIG                     (*((volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+63*4)))


#define A3_SPI_DIGREGHEN_BIT            (1 << 29)
#define A3_SPI_PWDDIGREGL_BIT           (1 << 28)
#define A3_SPI_BUCK_D_EN_BIT            (1 << 22)

#define A6_BUCKD_VSEL_2_BIT               (1 << 25)
#define A6_VSELSARLDO_1_BIT               (1 << 23)
#define A6_VSELSARLDO_0_BIT               (1 << 22)
#define A6_BUCKD_VSEL_1_BIT               (1 << 18)
#define A6_BUCKD_VSEL_0_BIT               (1 << 17)

#define A7_BUCKA_VSEL_2_BIT               (1 << 28)
#define A7_BUCKA_VSEL_1_BIT               (1 << 27)
#define A7_SS_EN_BIT                             (1 << 24)
#define A7_BUCKA_VSEL_0_BIT               (1 << 23)
#define A7_FSEL_1_BIT                           (1 << 22)
#define A7_FSEL_0_BIT                           (1 << 21)
#define A7_NONTIME_SEL_1_BIT              (1 << 20)
#define A7_NONTIME_SEL_0_BIT              (1 << 19)
#define A7_FSEL_1_1_BIT                        (1 << 18)
#define A7_FSEL_1_0_BIT                        (1 << 17)

#define sft_GPIO_INPUT                             0
#define sft_GPIO_OUTPUT                          1
#define sft_GPIO_INPUT_ENABLE               2
#define sft_GPIO_OUTPUT_ENABLE              3
#define sft_GPIO_PULL_MODE                     4
#define sft_GPIO_PULL_ENABLE                  5
#define sft_GPIO_FUNCTION_ENABLE            6
#define sft_GPIO_INPUT_MONITOR              7
#define sft_BT_LDO_en                         (1<<21)
#define sft_GPIO_LDO_en                     (1<<26)

// WDT
#define BK3000_WDT_CONFIG                    (*((volatile unsigned long *)(BK3000_WDT_BASE_ADDR+0*4)))
#define CLEAR_WDT      do{ BK3000_WDT_CONFIG = (0x5AFFFF); BK3000_WDT_CONFIG = (0xA5FFFF); }while(0)

//PCM
//#define BK3000_PCM_REG0_CONFIG                   (*((volatile unsigned long *)   (BK3000_PCM_BASE_ADDR+0x00*4)))
//#define BK3000_PCM_REG1_FIFO_INTR_ENABLE         (*((volatile unsigned long *)   (BK3000_PCM_BASE_ADDR+0x01*4)))
//#define BK3000_PCM_REG2_PCM_ENABLE               (*((volatile unsigned long *)   (BK3000_PCM_BASE_ADDR+0x02*4)))
//#define BK3000_PCM_REG3_FIFO_INTR_STATUS         (*((volatile unsigned long *)   (BK3000_PCM_BASE_ADDR+0x03*4)))
//#define BK3000_PCM_REG4_RD_DATA                  (*((volatile unsigned long *)   (BK3000_PCM_BASE_ADDR+0x04*4)))
//#define BK3000_PCM_REG5_WR_DATA                  (*((volatile unsigned long *)   (BK3000_PCM_BASE_ADDR+0x05*4)))

//pwm
#define BK3000_PWM0_PT0_CONF                 (*((volatile unsigned int *)(BK3000_PWM0_BASE_ADDR+0*4)))
#define BK3000_PWM0_PT1_CONF                 (*((volatile unsigned int *)(BK3000_PWM0_BASE_ADDR+1*4)))
#define BK3000_PWM0_PWM_CTRL                 (*((volatile unsigned int *)(BK3000_PWM0_BASE_ADDR+2*4)))
#define BK3000_PWM0_PT2_CONF                 (*((volatile unsigned int *)(BK3000_PWM0_BASE_ADDR+3*4)))
#define BK3000_PWM1_PT0_CONF                 (*((volatile unsigned int *)(BK3000_PWM1_BASE_ADDR+0*4)))
#define BK3000_PWM1_PT1_CONF                 (*((volatile unsigned int *)(BK3000_PWM1_BASE_ADDR+1*4)))
#define BK3000_PWM1_PWM_CTRL                 (*((volatile unsigned int *)(BK3000_PWM1_BASE_ADDR+2*4)))
#define BK3000_PWM1_PT2_CONF                 (*((volatile unsigned int *)(BK3000_PWM1_BASE_ADDR+3*4)))
#define PWM1_PT2_INTR_BIT                    (1 << 21)

#define sft_PWM_PT01_CNT_MID                        0
#define sft_PWM_PT01_CNT_TO                         16
#define sft_PWM_PT0_MODE                            0
#define sft_PWM_PT1_MODE                            1
#define sft_PWM_PT0_ENABLE                          4
#define sft_PWM_PT1_ENABLE                          5
#define sft_PWM_PT_DIVID                            8
#define bit_PWM_PT0_INT_FLAG                       (1<<12)
#define bit_PWM_PT1_INT_FLAG                       (1<<13)

#define PT_DIVID_MASK                              (0x0F00)

#define sft_PWM_PT2_CNT                              0
#define sft_PWM_PT2_ENABLE                           20
#define bit_PWM_PT2_INT_FLAG                        (1 << 21)
#define BK3000_PWM2_PWM0_CONF                 (*((volatile unsigned int *)(BK3000_PWM2_BASE_ADDR+0*4)))
#define BK3000_PWM2_PWM1_CONF                 (*((volatile unsigned int *)(BK3000_PWM2_BASE_ADDR+1*4)))
#define BK3000_PWM2_PWM2_CONF                 (*((volatile unsigned int *)(BK3000_PWM2_BASE_ADDR+2*4)))
#define BK3000_PWM2_PWM3_CONF                 (*((volatile unsigned int *)(BK3000_PWM2_BASE_ADDR+3*4)))
#define BK3000_PWM2_PWM4_CONF                 (*((volatile unsigned int *)(BK3000_PWM2_BASE_ADDR+4*4)))
#define BK3000_PWM2_PWM5_CONF                 (*((volatile unsigned int *)(BK3000_PWM2_BASE_ADDR+5*4)))
#define BK3000_PWM2_PWM_CTRL                    (*((volatile unsigned int *)(BK3000_PWM2_BASE_ADDR+6*4)))

// AUDIO (ADC & 2*DAC & DTMF)
#define BK3000_AUD_AUDIO_CONF                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 0*4)))
#define BK3000_AUD_DTMF_CONF0                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 1*4)))
#define BK3000_AUD_DTMF_CONF1                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 2*4)))
#define BK3000_AUD_DTMF_CONF2                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 3*4)))
#define BK3000_AUD_ADC_CONF0                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 4*4)))
#define BK3000_AUD_ADC_CONF1                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 5*4)))
#define BK3000_AUD_ADC_CONF2                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 6*4)))
#define BK3000_AUD_DAC_CONF0                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 7*4)))
#define BK3000_AUD_DAC_CONF1                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 8*4)))
#define BK3000_AUD_DAC_CONF2                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+ 9*4)))
#define BK3000_AUD_FIFO_CONF                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+10*4)))
#define BK3000_AUD_AGC_CONF0                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+11*4)))
#define BK3000_AUD_AGC_CONF1                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+12*4)))
#define BK3000_AUD_AGC_CONF2                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+13*4)))
#define BK3000_AUD_FIFO_STATUS               (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+14*4)))
#define BK3000_AUD_AGC_STATUS                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+15*4)))
#define BK3000_AUD_DTMF_PORT                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+16*4)))
#define BK3000_AUD_ADC_PORT                  (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+17*4)))
#define BK3000_AUD_DAC_PORT                  (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+18*4)))
#define BK3000_AUD_AUDIO_CONF2               (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+24*4)))
#define BK3000_AUD_DAC_FRACCOEF              (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+25*4)))
#define BK3000_AUD_ADC_FRACCOEF              (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+26*4)))
#define BK3000_AUD_HPF2_LBIT                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+31*4)))
#define BK3000_AUD_FLT0_A1_A2                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+32*4)))
#define BK3000_AUD_FLT0_B0_B1                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+33*4)))
#define BK3000_AUD_FLT0_B2                   (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+34*4)))
#define BK3000_AUD_FLT1_A1_A2                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+35*4)))
#define BK3000_AUD_FLT1_B0_B1                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+36*4)))
#define BK3000_AUD_FLT1_B2                   (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+37*4)))
#define BK3000_AUD_FLT2_A1_A2                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+38*4)))
#define BK3000_AUD_FLT2_B0_B1                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+39*4)))
#define BK3000_AUD_FLT2_B2                   (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+40*4)))
#define BK3000_AUD_FLT3_A1_A2                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+41*4)))
#define BK3000_AUD_FLT3_B0_B1                (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+42*4)))
#define BK3000_AUD_FLT3_B2                   (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+43*4)))
#define BK3000_AUD_FLT0_LBIT                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+44*4)))
#define BK3000_AUD_FLT1_LBIT                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+45*4)))
#define BK3000_AUD_FLT2_LBIT                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+46*4)))
#define BK3000_AUD_FLT3_LBIT                 (*((volatile unsigned int *)(BK3000_AUD_BASE_ADDR+47*4)))

#define sft_AUD_SAMP_DAC_RATE                   6
#define sft_AUD_SAMP_ADC_RATE                   0

#define sft_AUD_DAC_ENABLE                 2


#define sft_AUD_ADC_ENABLE                 3
#define sft_AUD_DTMF_ENABLE               4
#define sft_AUD_LINE_ENABLE               5

#define sft_AUD_DTMF_TONE_PATTERN         0
#define sft_AUD_DTMF_TONE_MODE             1
#define sft_AUD_DTMF_TONE_PAUSE            2
#define sft_AUD_DTMF_TONE_ACT                6

#define sft_AUD_DTMF_TONE_STEP               0
#define sft_AUD_DTMF_TONE_ATTU              16
#define sft_AUD_DTMF_TONE_ENABLE           20

#define sft_AUD_ADC_HPF2_COEF_B2            0
#define sft_AUD_ADC_HPF2_BYPASS             16
#define sft_AUD_ADC_HPF1_BYPASS             17
#define sft_AUD_ADC_SET_GAIN                    18
#define sft_AUD_ADC_SAMPLE_EDGE             24

#define sft_AUD_ADC_HPF2_COEF_B0            0
#define sft_AUD_ADC_HPF2_COEF_B1            16

#define sft_AUD_ADC_HPF2_COEF_A0            0
#define sft_AUD_ADC_HPF2_COEF_A1            16

#define sft_AUD_DAC_HPF2_COEF_B2            0
#define sft_AUD_DAC_HPF2_BYPASS             16
#define sft_AUD_DAC_HPF1_BYPASS             17
#define sft_AUD_DAC_SET_GAIN                    18
#define sft_AUD_DAC_CLK_INVERT                 24

#define sft_AUD_DAC_HPF2_COEF_B0             0
#define sft_AUD_DAC_HPF2_COEF_B1            16

#define sft_AUD_DAC_HPF2_COEF_A0              0
#define sft_AUD_DAC_HPF2_COEF_A1             16

#define sft_AUD_DAC_DACR_RD_THRE            0
#define sft_AUD_DAC_DACL_RD_THRE            5
#define sft_AUD_DTMF_WR_THRE                   10
#define sft_AUD_ADC_WR_THRE                     15
#define sft_AUD_DACR_INT_EN                       20
#define sft_AUD_DACL_INT_EN                       21
#define sft_AUD_DTMF_INT_EN                      22
#define sft_AUD_ADC_INT_EN                        23
#define sft_AUD_LOOP_TON2DAC                    24
#define sft_AUD_LOOP_ADC2DAC                    25

#define sft_AUD_AGC_NOISE_THRE                  0
#define sft_AUD_AGC_NOISE_HIGH                  10
#define sft_AUD_AGC_NOISE_LOW                   20
#define sft_AUD_AGC_STEP                               30

#define sft_AUD_AGC_NOISE_MIN                   0
#define sft_AUD_AGC_NOISE_TOUT                 7
#define sft_AUD_AGC_HIGH_DUR                    10
#define sft_AUD_AGC_LOW_DUR                     13
#define sft_AUD_AGC_MIN                             16
#define sft_AUD_AGC_MAX                             23
#define sft_AUD_AGC_NG_METHOD                  30
#define sft_AUD_AGC_NG_ENABLE                    31

#define sft_AUD_AGC_DECAY_TIME                  0
#define sft_AUD_AGC_ATTACK_TIME                 3
#define sft_AUD_AGC_HIGH_THRE                   6
#define sft_AUD_AGC_LOW_THRE                   11
#define sft_AUD_AGC_IIR_COEF                       16
#define sft_AUD_AGC_ENABLE                          19
#define sft_AUD_MAN_PGA_VALUE                   20
#define sft_AUD_MAN_PGA                              27

#define bit_DACR_NEAR_FULL                (0x01<<0)
#define bit_DACL_NEAR_FULL                (0x01<<1)
#define bit_ADCL_NEAR_FULL                (0x01<<2)
#define bit_DTMF_NEAR_FULL                (0x01<<3)
#define bit_DACR_NEAR_EMPTY               (0x01<<4)
#define bit_DACL_NEAR_EMPTY               (0x01<<5)
#define bit_ADCL_NEAR_EMPTY               (0x01<<6)
#define bit_DTMF_NEAR_EMPTY               (0x01<<7)
#define bit_DACR_FIFO_FULL                (0x01<<8)
#define bit_DACL_FIFO_FULL                (0x01<<9)
#define bit_ADCL_FIFO_FULL                (0x01<<10)
#define bit_DTMF_FIFO_FULL                (0x01<<11)
#define bit_DACR_FIFO_EMPTY               (0x01<<12)
#define bit_DACL_FIFO_EMPTY               (0x01<<13)
#define bit_ADCL_FIFO_EMPTY               (0x01<<14)
#define bit_DTMF_FIFO_EMPTY               (0x01<<15)
#define bit_DACR_INTR_FLAG                (0x01<<16)
#define bit_DACL_INTR_FLAG                (0x01<<17)
#define bit_ADCL_INTR_FLAG                (0x01<<18)
#define bit_DTMF_INTR_FLAG                (0x01<<19)

#define AUD_READ_RSSI_DB            (BK3000_AUD_AGC_STATUS & 0xFF )
#define AUD_READ_MIC_PGA          ((BK3000_AUD_AGC_STATUS >> 8 ) & 0xFF )
#define AUD_READ_MIC_RSSI        ((BK3000_AUD_AGC_STATUS >> 16 )&0xFFFF )

#define AUD_WRITE_DACL(L,R)        (BK3000_AUD_DAC_PORT = ((R << 16)|L))

// DMA 
#define reg_DMA_GLOBAL_CONFIG             (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+ 0*4)))
#define reg_DMA_CH0_CONTROL               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+ 8*4)))
#define reg_DMA_CH0_XFR_SIZE              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+ 9*4)))
#define reg_DMA_CH0_SRC_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+10*4)))
#define reg_DMA_CH0_DST_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+11*4)))
#define reg_DMA_CH0_SRC_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+12*4)))
#define reg_DMA_CH0_SRC_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+13*4)))
#define reg_DMA_CH0_DST_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+14*4)))
#define reg_DMA_CH0_DST_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+15*4)))
#define reg_DMA_CHO_SRC_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x40*4)))  
#define reg_DMA_CHO_DST_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x41*4)))  

#define reg_DMA_CH1_CONTROL               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+16*4)))
#define reg_DMA_CH1_XFR_SIZE              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+17*4)))
#define reg_DMA_CH1_SRC_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+18*4)))
#define reg_DMA_CH1_DST_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+19*4)))
#define reg_DMA_CH1_SRC_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+20*4)))
#define reg_DMA_CH1_SRC_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+21*4)))
#define reg_DMA_CH1_DST_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+22*4)))
#define reg_DMA_CH1_DST_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+23*4)))
#define reg_DMA_CH1_SRC_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x42*4)))  
#define reg_DMA_CH1_DST_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x43*4)))  

#define reg_DMA_CH2_CONTROL               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+24*4)))
#define reg_DMA_CH2_XFR_SIZE              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+25*4)))
#define reg_DMA_CH2_SRC_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+26*4)))
#define reg_DMA_CH2_DST_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+27*4)))
#define reg_DMA_CH2_SRC_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+28*4)))
#define reg_DMA_CH2_SRC_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+29*4)))
#define reg_DMA_CH2_DST_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+30*4)))
#define reg_DMA_CH2_DST_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+31*4)))
#define reg_DMA_CH2_SRC_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x44*4)))  
#define reg_DMA_CH2_DST_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x45*4)))  

#define reg_DMA_CH3_CONTROL               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+32*4)))
#define reg_DMA_CH3_XFR_SIZE              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+33*4)))
#define reg_DMA_CH3_SRC_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+34*4)))
#define reg_DMA_CH3_DST_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+35*4)))
#define reg_DMA_CH3_SRC_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+36*4)))
#define reg_DMA_CH3_SRC_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+37*4)))
#define reg_DMA_CH3_DST_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+38*4)))
#define reg_DMA_CH3_DST_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+39*4)))
#define reg_DMA_CH3_SRC_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x46*4)))  
#define reg_DMA_CH3_DST_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x47*4)))  

#define reg_DMA_CH4_CONTROL               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+40*4)))
#define reg_DMA_CH4_XFR_SIZE              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+41*4)))
#define reg_DMA_CH4_SRC_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+42*4)))
#define reg_DMA_CH4_DST_ADDR              (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+43*4)))
#define reg_DMA_CH4_SRC_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+44*4)))
#define reg_DMA_CH4_SRC_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+45*4)))
#define reg_DMA_CH4_DST_TOP               (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+46*4)))
#define reg_DMA_CH4_DST_BOTTOM            (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+47*4)))
#define reg_DMA_CH4_SRC_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x48*4)))  
#define reg_DMA_CH4_DST_CUR_PTR           (*((volatile unsigned long *)   (BK3000_DMA_BASE_ADDR+0x49*4)))  


// SAR-ADC
#define BK3000_SARADC_ADC_CONF                 (*((volatile unsigned int *)(BK3000_SADC_BASE_ADDR+0*4)))
#define BK3000_SARADC_ADC_DATA                 (*((volatile unsigned int *)(BK3000_SADC_BASE_ADDR+1*4)))

#define sft_SARADC_CONF_MODE                0
#define sft_SARADC_CONF_CHNL                2
#define bit_SARADC_CONF_FIFO_EMPTY      (1<< 6)
#define bit_SARADC_CONF_BUSY                 (1<<7)
#define sft_SARADC_CONF_SAMP_RATE        8
#define sft_SARADC_CONF_SETTING             10
#define sft_SARADC_CONF_CLK_RATE          16

// SD-Card
#define BK3000_SDIO_CMD_SEND_CTRL            (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+0*4)))
#define BK3000_SDIO_CMD_SEND_AGUMENT         (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+1*4)))
#define BK3000_SDIO_CMD_RSP_TIMER            (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+2*4)))
#define BK3000_SDIO_DATA_REC_CTRL            (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+3*4)))
#define BK3000_SDIO_DATA_REC_TIMER           (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+4*4)))
#define BK3000_SDIO_CMD_RSP_AGUMENT0         (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+5*4)))
#define BK3000_SDIO_CMD_RSP_AGUMENT1         (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+6*4)))
#define BK3000_SDIO_CMD_RSP_AGUMENT2         (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+7*4)))
#define BK3000_SDIO_CMD_RSP_AGUMENT3         (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+8*4)))
#define BK3000_SDIO_CMD_RSP_INT_SEL          (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+9*4)))
#define BK3000_SDIO_CMD_RSP_INT_MASK         (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+10*4)))
#define BK3000_SDIO_WR_DATA_ADDR             (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+11*4)))
#define BK3000_SDIO_RD_DATA_ADDR             (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+12*4)))
#define BK3000_SDIO_FIFO_THRESHOLD           (*((volatile unsigned int *)(BK3000_SDIO_BASE_ADDR+13*4)))
#define SDIO_RX_FIFO_RST_BIT           (1 << 16)
#define SDIO_TX_FIFO_RST_BIT           (1 << 17)

// IRDA_NEC
#define BK3000_IRDA_CTRL                     (*((volatile unsigned int *)   (BK3000_IRDA_BASE_ADDR+0*4)))
#define BK3000_IRDA_INT_MASK                 (*((volatile unsigned int *)   (BK3000_IRDA_BASE_ADDR+1*4)))
#define BK3000_IRDA_INT                      (*((volatile unsigned int *)   (BK3000_IRDA_BASE_ADDR+2*4)))
#define BK3000_IRDA_DATA                     (*((volatile unsigned int *)   (BK3000_IRDA_BASE_ADDR+3*4)))

#define    sft_IRDA_NEC_EN                    0
#define    sft_IRDA_POLARIT                1
#define sft_CLK_DIV                        8
#define    IRDA_END_INT_EN                    (1<<0)
#define IRDA_RIGHT_INT_EN                (1<<1)
#define    IRDA_REPEAT_INT_EN                (1<<2)
#define    IRDA_END_INT_FLAG                (1<<0)
#define IRDA_RIGHT_INT_FLAG                (1<<1)
#define    IRDA_REPEAT_INT_FLAG            (1<<2)
#define bit_PT0_MODE        BIT_0
#define bit_PT1_MODE        BIT_1
#define bit_PT0_ENABLE      BIT_4
#define bit_PT1_ENABLE      BIT_5
#define bit_PT2_ENABLE      BIT_20
#define bit_PT0_INT_FLAG    BIT_12
#define bit_PT1_INT_FLAG    BIT_13
#define bit_PT2_INT_FLAG    BIT_21

#define PT_DIV(x)           (x) << 8



#define Enable_UART_Low_Level_Wakeup(v) {   REG_APB3_UART_WAKEUP_ENABLE=v;}


/*
 * defination of registers
*/
#define REG_APB3_UART_CFG                   (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x00*4)))
#define REG_APB3_UART_FIFO_THRESHOLD        (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x01*4)))
#define REG_APB3_UART_FIFO_STATUS           (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x02*4)))
#define REG_APB3_UART_DATA                  (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x03*4)))
#define REG_APB3_UART_INT_ENABLE            (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x04*4)))
#define REG_APB3_UART_INT_FLAG              (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x05*4)))
#define REG_APB3_UART_FC_CFG                (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x06*4)))
#define REG_APB3_UART_WAKEUP_ENABLE         (*((volatile unsigned long *)(BK3000_UART_BASE_ADDR+0x07*4)))

#define UART_WRITE_BYTE(v) (REG_APB3_UART_DATA=v)
#define UART_READ_BYTE()  ((REG_APB3_UART_DATA>>8)&0xff)

#define bit_UART_TX_FIFO_FULL                (1<<16)
#define bit_UART_TX_FIFO_EMPTY               (1<<17)
#define bit_UART_RX_FIFO_FULL                (1<<18)
#define bit_UART_RX_FIFO_EMPTY               (1<<19)
#define bit_UART_TX_WRITE_READY              (1<<20)
#define bit_UART_RX_READ_READY               (1<<21)

#define sft_UART_CONF_TX_ENABLE          0
#define sft_UART_CONF_RX_ENABLE          1
#define sft_UART_CONF_IRDA               2
#define sft_UART_CONF_UART_LEN           3
#define sft_UART_CONF_PAR_EN             5
#define sft_UART_CONF_PAR_MODE           6
#define sft_UART_CONF_STOP_LEN           7
#define sft_UART_CONF_CLK_DIVID          8

#define sft_UART_FIFO_CONF_TX_FIFO       0
#define sft_UART_FIFO_CONF_RX_FIFO       8
#define sft_UART_FIFO_CONF_DETECT_TIME   16

#define bit_UART_INT_TX_NEED_WRITE       ( 1 << 0 )
#define bit_UART_INT_RX_NEED_READ        ( 1 << 1 )
#define bit_UART_INT_RX_OVER_FLOW        ( 1 << 2 )
#define bit_UART_INT_RX_PAR_ERR          ( 1 << 3 )
#define bit_UART_INT_RX_STOP_ERR         ( 1 << 4 )
#define bit_UART_INT_TX_STOP_END         ( 1 << 5 )
#define bit_UART_INT_RX_STOP_END         ( 1 << 6 )
#define bit_UART_INT_RXD_WAKEUP          ( 1 << 7 )

#define sft_UART_FLOW_CONF_LOW_CNT       0
#define sft_UART_FLOW_CONF_HIGH_CNT      8
#define sft_UART_FLOW_CONF_ENABLE        16

#define sft_UART_WAKE_CONF_WAKE_COUNT       0
#define sft_UART_WAKE_CONF_TXD_WAIT_CNT     10
#define sft_UART_WAKE_CONF_RXD_WAKE_EN      20
#define sft_UART_WAKE_CONF_TXD_WAKE_EN      21
#define sft_UART_WAKE_CONF_RXD_EDGE_EN      22

#define UART_INTR_TX_FIFO_NEED_WRITE_MASK       BIT_0
#define UART_INTR_RX_FIFO_NEED_READ_MASK        BIT_1
#define UART_INTR_RX_OV_MASK                    BIT_2
#define UART_INTR_RX_PAR_ERR_MASK               BIT_3
#define UART_INTR_RX_STOP_ERR_MASK              BIT_4
#define UART_INTR_TX_STOP_END_MASK              BIT_5
#define UART_INTR_RX_STOP_END_MASK              BIT_6
#define UART_INTR_RX_WAKE_PULSE_MASK            BIT_7

#define UART_INTR_TX_FIFO_NEED_WRITE_WAKEUP     BIT_0
#define UART_INTR_RX_FIFO_NEED_READ_WAKEUP      BIT_1
#define UART_INTR_RX_OV                         BIT_2
#define UART_INTR_RX_PAR_ERR                    BIT_3
#define UART_INTR_RX_STOP_ERR                   BIT_4
#define UART_INTR_TX_STOP_END                   BIT_5
#define UART_INTR_RX_STOP_END                   BIT_6
#define UART_INTR_RX_WAKE_PULSE                 BIT_7

#define UART_RX_WAKEUP_COUNT(x)                 ((x) & 0x3FF)
#define UART_WAKEUP_SIG_WAIT_COUNT(x)           (((x) << 10) & 0x000FFC00)
#define UART_RX_WAKEUP_ENABLE                   BIT_20
#define UART_TX_WAKEUP_ENABLE                   BIT_21
#define UART_RX_LOW_PULSE_WAKEUP                BIT_22

#define UART_FC_CTSRTX_LOW_COUNT(x)             ((x)&0xFF)
#define UART_FC_CTSRTX_HIGH_COUNT(x)            (((x)<<8) & 0x0000FF00)
#define UART_FC_CTSRTX_ENABLE                   BIT_16
#define UART_RTS_LOW_ENABLE                     BIT_17
#define UART_CTS_LOW_ENABLE                     BIT_18

#define UART_TX_FIFO_SIZE       128
#define UART_RX_FIFO_SIZE       128
#define UART_TX_FIFO_COUNT      (REG_APB3_UART_FIFO_STATUS&0xff)
#define UART_RX_FIFO_COUNT      ((REG_APB3_UART_FIFO_STATUS>>8)&0xff)
#define UART_TX_FIFO_FULL        (REG_APB3_UART_FIFO_STATUS&0x00010000)
#define UART_TX_FIFO_EMPTY       (REG_APB3_UART_FIFO_STATUS&0x00020000)
#define UART_RX_FIFO_FULL        (REG_APB3_UART_FIFO_STATUS&0x00040000)
#define UART_RX_FIFO_EMPTY       (REG_APB3_UART_FIFO_STATUS&0x00080000)
#define UART_TX_WRITE_READY      (REG_APB3_UART_FIFO_STATUS&0x00100000)
#define UART_RX_READ_READY       (REG_APB3_UART_FIFO_STATUS&0x00200000)

#define UART_BAUDRATE_3250000   3250000
#define UART_BAUDRATE_2000000   2000000
#define UART_BAUDRATE_921600    921600  //
#define UART_BAUDRATE_460800    460800
#define UART_BAUDRATE_230400    230400  //
#define UART_BAUDRATE_115200    115200  //default
#define UART_BAUDRATE_3000      3250  //

#define UART_CLOCK_FREQ_26M  26000000
#define UART_CLOCK_FREQ_48M  48000000
#define UART_CLOCK_FREQ_24M  24000000
#define UART_CLOCK_FREQ_52M  52000000

#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)
#define AGC_RSSI_MASK               0xff
#define AGC_RSSI_SHIFT              0

#define AGC_LNA_HIGH_SHIFT          29
#define AGC_LNA_HIGH_MASK           (0x01<<AGC_LNA_HIGH_SHIFT)

#define AGC_PGA_G1_HIGH_SHIFT       25
#define AGC_PGA_G1_HIGH_MASK        (0x07<<AGC_PGA_G1_HIGH_SHIFT)

#define AGC_PGA_G2_HIGH_SHIFT       21
#define AGC_PGA_G2_HIGH_MASK        (0x01<<AGC_PGA_G2_HIGH_SHIFT)

#define AGC_PGA_G3_HIGH_SHIFT       19
#define AGC_PGA_G3_HIGH_MASK        (0x01<<AGC_PGA_G3_HIGH_SHIFT)

#define AGC_PGA_G4_HIGH_SHIFT       17
#define AGC_PGA_G4_HIGH_MASK        (0x01<<AGC_PGA_G4_HIGH_SHIFT)


#define AGC_LNA_LOW_SHIFT          28
#define AGC_LNA_LOW_MASK           (0x01<<AGC_LNA_LOW_SHIFT)

#define AGC_PGA_G1_LOW_SHIFT       22
#define AGC_PGA_G1_LOW_MASK        (0x07<<AGC_PGA_G1_LOW_SHIFT)

#define AGC_PGA_G2_LOW_SHIFT       20
#define AGC_PGA_G2_LOW_MASK        (0x01<<AGC_PGA_G2_LOW_SHIFT)

#define AGC_PGA_G3_LOW_SHIFT       18
#define AGC_PGA_G3_LOW_MASK        (0x01<<AGC_PGA_G3_LOW_SHIFT)

#define AGC_PGA_G4_LOW_SHIFT       16
#define AGC_PGA_G4_LOW_MASK        (0x01<<AGC_PGA_G4_LOW_SHIFT)


#define mXVRGetRegValue(RegNum,BitField,BitShift)\
((BK3000_XVR_##RegNum&BitField) >> BitShift)

#define mXVRSetRegValue(RegNum,BitField,BitShift,Value)\
{  (BK3000_XVR_##RegNum = (BK3000_XVR_##RegNum & (~BitField)) | (Value<<BitShift));}
   
#endif


// I2C1
#define BK3000_I2C1_CN                       (*((volatile unsigned int *)    (BK3000_I2C1_BASE_ADDR+0*4)))
#define BK3000_I2C1_DATA                     (*((volatile unsigned int *)    (BK3000_I2C1_BASE_ADDR+1*4)))

#define bit_I2C1_BUSY             (1<<19)
#define bit_I2C1_STA              (1<<1)
#define bit_I2C1_STO              (1<<2)
#define bit_I2C1_TXACK            (1<<3)
#define bit_I2C1_TXMODE           (1<<4)
#define bit_I2C1_SI               (1<<16)
#define bit_I2C1_RXACK            (1<<17)
#define bit_I2C1_ACKRQ            (1<<18)

#define sft_I2C1_FREQ_DIV          6
#define sft_I2C1_ENSMB             0

//USB register
#define REG_USB_BASE_ADDR               (0x00D02000UL)

#define REG_USB_FADDR                     (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x00)))//800
#define REG_USB_POWER                     (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x01)))//801
#define REG_USB_INTRTX1                   (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x02)))//802
#define REG_USB_INTRTX2                   (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x03)))//803
#define REG_USB_INTRRX1                   (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x04)))//804
#define REG_USB_INTRRX2                   (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x05)))//805
#define REG_USB_INTRUSB                   (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x06)))//806
#define REG_USB_INTRTX1E                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x07)))//807
#define REG_USB_INTRTX2E                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x08)))//808
#define REG_USB_INTRRX1E                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x09)))//809
#define REG_USB_INTRRX2E                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x0A)))//810
#define REG_USB_INTRUSBE                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x0B)))//811
#define REG_USB_FRAME1                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x0C)))//812
#define REG_USB_FRAME2                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x0D)))//813
#define REG_USB_INDEX                     (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x0E)))//814
#define REG_USB_DEVCTL                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x0F)))//815
#define REG_USB_TXMAXP                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x10)))//816
#define REG_USB_CSR0                      (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x11)))//817
#define REG_USB_CSR02                     (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x12)))//818
#define REG_USB_RXMAXP                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x13)))//819
#define REG_USB_RXCSR1                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x14)))//820
#define REG_USB_RXCSR2                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x15)))//821
#define REG_USB_COUNT0                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x16)))//822
#define REG_USB_RXCOUNT2                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x17)))//823
#define REG_USB_TXTYPE                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x18)))//824
#define REG_USB_NAKLIMIT0                 (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x19)))//825
#define REG_USB_RXTYPE                    (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x1A)))//826
#define REG_USB_RXINTERVAL                (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x1B)))//827

#define REG_USB_FIFOSIZE                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x1F)))//828

#define REG_USB_FIFO0                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x20)))//829
#define REG_USB_FIFO1                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x24)))//829
#define REG_USB_FIFO2                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x28)))//829
#define REG_USB_FIFO3                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x2C)))//829
#define REG_USB_FIFO4                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x30)))//829
#define REG_USB_FIFO5                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x34)))//829
#define REG_USB_FIFO6                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x38)))//829
#define REG_USB_FIFO7                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x3C)))//829
#define REG_USB_FIFO8                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x40)))//829
#define REG_USB_FIFO9                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x44)))//829
#define REG_USB_FIFOA                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x48)))//829
#define REG_USB_FIFOB                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x4C)))//829
#define REG_USB_FIFOC                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x50)))//829
#define REG_USB_FIFOD                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x54)))//829
#define REG_USB_FIFOE                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x58)))//829
#define REG_USB_FIFOF                     (*((volatile unsigned long *) (REG_USB_BASE_ADDR + 0x5C)))//829

#define REG_AHB2_USB_OTG_CFG              (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x80)))
#define REG_AHB2_USB_DMA_ENDP             (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x84)))
#define REG_AHB2_USB_VTH                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x88)))
#define REG_AHB2_USB_GEN                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x8C)))
#define REG_AHB2_USB_STAT                 (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x90)))
#define REG_AHB2_USB_INT                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x94)))
#define REG_AHB2_USB_RESET                (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x98)))
#define REG_AHB2_USB_DEV_CFG              (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x9C)))

extern void delay_us(u_int32 num);

#endif

