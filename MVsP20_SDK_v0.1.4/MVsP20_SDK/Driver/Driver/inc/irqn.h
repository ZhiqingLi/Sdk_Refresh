/*
 * irqn.h
 *
 *  Created on: Dec 11, 2015
 *      Author: peter
 */
 
 /**
 * @addtogroup INTERRUPT
 * @{
 * @defgroup irqn irqn.h
 * @{
 */
 
#include "type.h"

#ifndef IRQN_H_
#define IRQN_H_
 
typedef enum IRQn
{
    /*
     * internal interrupt(Exception/Fault)
     */
    RST_IRQn		= -9,
    TLB_Fill_IRQn		= -8,
    PTE_IRQn	= -7,
    TLB_Misc_IRQn		= -6,//programmable as below
    TLB_VLPT_Miss_IRQn		= -5,
    Machine_ERR_IRQn		= -4,
    Debug_IRQn		= -3,
    General_Exception_IRQn		= -2,
    Syscall_IRQn		= -1,
    /*
     * SOC interrupt(External Interrupt)
     */

	TMR1_IRQn	    = 0,
	Wakeup_IRQn		= 1,
    GPIO_IRQn		= 2,
    RTC_IRQn		= 3,
	WDG_IRQn		= 4,
	SWI_IRQn		= 5,
	I2C_IRQn		= 6,
    UART0_IRQn		= 7,
    TMR2_IRQn		= 8,
	DMA0_IRQn		= 9,
	DMA1_IRQn		= 10,
	DMA2_IRQn		= 11,
	DMA3_IRQn		= 12,
	DMA4_IRQn		= 13,
	DMA5_IRQn		= 14,
	DMA6_IRQn		= 15,
	DMA7_IRQn		= 16,
    UART1_IRQn		= 17,
    UART2_IRQn		= 18,
	I2S_IRQn		= 19,
	TMR3_IRQn	    = 20,
    TMR4_IRQn		= 21,
	TMR5_IRQn	    = 22,
    TMR6_IRQn		= 23,
    TMR7_IRQn		= 24,
    TMR8_IRQn		= 25,
	SDIO_IRQn		= 26,
    OTG_IRQn		= 27,
	SPIM0_IRQn		= 28,
	SPIM1_IRQn		= 29,
	SPIS_IRQn		= 30,
	FDMA_IRQn		= 31
} IRQn_Type;

typedef enum IRQn_PRIO
{
    /*
     * SOC interrupt(External Interrupt)
     */
	TMR1_IRQn_PRIO	    = 3,
	Wakeup_IRQn_PRIO	= 2,
    GPIO_IRQn_PRIO		= 2,
    RTC_IRQn_PRIO		= 2,
	WDG_IRQn_PRIO		= 2,
	SWI_IRQn_PRIO		= 3,
	I2C_IRQn_PRIO		= 2,
    UART0_IRQn_PRIO		= 2,
    TMR2_IRQn_PRIO		= 2,
	DMA0_IRQn_PRIO		= 2,
	DMA1_IRQn_PRIO		= 2,
	DMA2_IRQn_PRIO		= 2,
	DMA3_IRQn_PRIO		= 2,
	DMA4_IRQn_PRIO		= 2,
	DMA5_IRQn_PRIO		= 2,
	DMA6_IRQn_PRIO		= 2,
	DMA7_IRQn_PRIO		= 2,
    UART1_IRQn_PRIO		= 2,
    UART2_IRQn_PRIO		= 2,
	I2S_IRQn_PRIO		= 2,
	TMR3_IRQn_PRIO	    = 2,
    TMR4_IRQn_PRIO		= 2,
	TMR5_IRQn_PRIO	    = 2,
    TMR6_IRQn_PRIO		= 2,
    TMR7_IRQn_PRIO		= 2,
    TMR8_IRQn_PRIO		= 2,
	SDIO_IRQn_PRIO		= 2,
    OTG_IRQn_PRIO		= 2,
	SPIM0_IRQn_PRIO		= 2,
	SPIM1_IRQn_PRIO		= 2,
	SPIS_IRQn_PRIO		= 2,
	FDMA_IRQn_PRIO		= 2
} IRQn_PRIO_TYPE;

#define NDS_THROTTLE_LEVEL_0  (0x00)
#define NDS_THROTTLE_LEVEL_1  (0x10)
#define NDS_THROTTLE_LEVEL_2  (0x20)
#define NDS_THROTTLE_LEVEL_3  (0x30)
#define NDS_THROTTLE_LEVEL_4  (0x40)
#define NDS_THROTTLE_LEVEL_5  (0x50)
#define NDS_THROTTLE_LEVEL_6  (0x60)
#define NDS_THROTTLE_LEVEL_7  (0x70)
#define NDS_THROTTLE_LEVEL_8  (0x80)
#define NDS_THROTTLE_LEVEL_9  (0x90)
#define NDS_THROTTLE_LEVEL_10 (0xA0)
#define NDS_THROTTLE_LEVEL_11 (0xB0)
#define NDS_THROTTLE_LEVEL_12 (0xC0)
#define NDS_THROTTLE_LEVEL_13 (0xD0)
#define NDS_THROTTLE_LEVEL_14 (0xE0)
#define NDS_THROTTLE_LEVEL_15 (0xF0)

/**
 * @brief  关闭中断
 * @param  NONE
 * @return NONE
 */

/**
 * @brief  使能中断
 * @param  NONE
 * @return NONE
 */
inline void GIE_DISABLE();

/**
 * @brief  使能PowerBrake功能
 * @param  NONE
 * @return NONE
 */
void Throttle_Enable(void);

/**
 * @brief  禁止PowerBrake功能
 * @param  NONE
 * @return NONE
 */
void Throttle_Disable(void);

/**
 * @brief  使能PowerBrake功能
 * @param  setting 设置CPU主频
 *  @arg  NDS_THROTTLE_LEVEL_0  CPU主频不变，具有最高的性能
 *  @arg	...
 *  @arg  NDS_THROTTLE_LEVEL_15 CPU主频降为原来的1/16,具有最低的性能
 * @return NONE
 */
void Throttle_Setup(uint32_t setting);

/**
 * @brief  使能外设的中断
 * @param  IRQn
 * @return NONE
 */
void NVIC_EnableIRQ(IRQn_Type IRQn);

/**
 * @brief  禁止外设的中断
 * @param  IRQn
 * @return NONE
 */
void NVIC_DisableIRQ(IRQn_Type IRQn);

/**
 * @brief  设置外设的中断优先级
 * @param  IRQn
 * @param  priority
 * 	@arg priority=0,1,2,3  0=highest 3=lowest
 * @return NONE
 */
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);

/**
 * @brief  获取外设的中断优先级
 * @param  IRQn
 * @return priority=0,1,2,3  0=highest 3=lowest
 */
uint32_t NVIC_GetPriority(IRQn_Type IRQn);

/**
 * @brief	系统软件复位
 * @param	无
 * @return 无
 * @note 执行该函数后，系统将被复位。
 */
void NVIC_SystemReset(void);

#endif /* IRQN_H_ */

/**
 * @}
 * @}
 */
