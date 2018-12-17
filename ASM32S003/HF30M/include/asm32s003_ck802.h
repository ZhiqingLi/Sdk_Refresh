/*
  ******************************************************************************
  * @file    asm32s003_ck802.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_ck802_H
#define _asm32s003_ck802_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

//---------------------------------------------------------------------------- 
//                      Interrupt Controller
//---------------------------------------------------------------------------- 
#define INTC_ISER_WRITE(val)     *(volatile UINT32 *) (INTC_ISER  ) = val
#define INTC_IWER_WRITE(val)     *(volatile UINT32 *) (INTC_IWER  ) = val
#define INTC_ICER_WRITE(val)     *(volatile UINT32 *) (INTC_ICER  ) = val
#define INTC_IWDR_WRITE(val)     *(volatile UINT32 *) (INTC_IWDR  ) = val
#define INTC_ISPR_WRITE(val)     *(volatile UINT32 *) (INTC_ISPR  ) = val
#define INTC_ICPR_WRITE(val)     *(volatile UINT32 *) (INTC_ICPR  ) = val
#define INTC_IABR_WRITE(val)     *(volatile UINT32 *) (INTC_IABR  ) = val
#define INTC_IPR0_WRITE(val)   	 *(volatile UINT32 *) (INTC_IPR0  ) = val
#define INTC_IPR1_WRITE(val)     *(volatile UINT32 *) (INTC_IPR1  ) = val
#define INTC_IPR2_WRITE(val)     *(volatile UINT32 *) (INTC_IPR2  ) = val
#define INTC_IPR3_WRITE(val)     *(volatile UINT32 *) (INTC_IPR3  ) = val
#define INTC_IPR4_WRITE(val)     *(volatile UINT32 *) (INTC_IPR4  ) = val
#define INTC_IPR5_WRITE(val)     *(volatile UINT32 *) (INTC_IPR5  ) = val
#define INTC_IPR6_WRITE(val)     *(volatile UINT32 *) (INTC_IPR6  ) = val
#define INTC_IPR7_WRITE(val)     *(volatile UINT32 *) (INTC_IPR7  ) = val
#define INTC_ISR_WRITE(val)      *(volatile UINT32 *) (INTC_ISR   ) = val
#define INTC_IPTR_WRITE(val)     *(volatile UINT32 *) (INTC_IPTR  ) = val


#define INTC_ISER_READ(intc)   (intc->ISER  )
#define INTC_IWER_READ(intc)   (intc->IWER  )
#define INTC_ICER_READ(intc)   (intc->ICER  )
#define INTC_IWDR_READ(intc)   (intc->IWDR  )
#define INTC_ISPR_READ(intc)   (intc->ISPR  )
#define INTC_ICPR_READ(intc)   (intc->ICPR  )
#define INTC_IABR_READ(intc)   (intc->IABR  )
#define INTC_IPR0_READ(intc)   (intc->IPR0  )
#define INTC_IPR1_READ(intc)   (intc->IPR1  )
#define INTC_IPR2_READ(intc)   (intc->IPR2  )
#define INTC_IPR3_READ(intc)   (intc->IPR3  )
#define INTC_IPR4_READ(intc)   (intc->IPR4  )
#define INTC_IPR5_READ(intc)   (intc->IPR5  )
#define INTC_IPR6_READ(intc)   (intc->IPR6  )
#define INTC_IPR7_READ(intc)   (intc->IPR7  )
#define INTC_ISR_READ(intc)    (intc->ISR   )
#define INTC_IPTR_READ(intc)   (intc->IPTR  )

typedef enum IRQn
{
/******  CK802 Processor Exceptions Numbers ***************************************************/
        ISR_Restart                     = -32,
        ISR_Misaligned_Access           = -31,
        ISR_Access_Error                = -30,
        ISR_Divided_By_Zero             = -29,
        ISR_Illegal                     = -28,
        ISR_Privlege_Violation          = -27,
        ISR_Trace_Exection              = -26,
        ISR_Breakpoint_Exception        = -25,
        ISR_Unrecoverable_Error         = -24,
        ISR_Idly4_Error                 = -23,
        ISR_Auto_INT                 	= -22,
        ISR_Auto_FINT                   = -21,
        ISR_Reserved_HAI                = -20,
        ISR_Reserved_FP                 = -19,
        ISR_TLB_Ins_Empty               = -18,
        ISR_TLB_Data_Empty              = -17,
  
/******  CK802 specific Interrupt Numbers *********************************************************/
        INTC_CORETIM_IRQn               = 0,
        INTC_SYSCON_IRQn                = 1,
        INTC_SYSIFC_IRQn                = 2,
        INTC_ADC_IRQn                	= 3,
        INTC_GTC0_IRQn                	= 4, 
        INTC_GTC1_IRQn                	= 5,
        INTC_GTC2_IRQn                	= 6, 
        INTC_EXI0_IRQn                	= 7, 
        INTC_EXI1_IRQn                	= 8, 
        INTC_GTC3_IRQn                	= 9, 
        INTC_RSVD0_IRQn                	= 10, 
        INTC_RSVD1_IRQn                	= 11, 
        INTC_RSVD2_IRQn                	= 12, 
        INTC_UART0_IRQn                 = 13,
        INTC_UART1_IRQn                 = 14,
        INTC_RSVD3_IRQn                	= 15, 
        INTC_RSVD4_IRQn                	= 16, 
        INTC_I2C_IRQn                	= 17,
        INTC_RSVD5_IRQn                	= 18, 
        INTC_RSVD6_IRQn                	= 19, 
        INTC_RSVD7_IRQn                	= 20,
        INTC_EXI2_IRQn                	= 21, 
        INTC_EXI3_IRQn                	= 22, 
        INTC_EXI4_IRQn                	= 23,
        INTC_CNTA_IRQn                	= 24, 
        INTC_TKEY0_IRQn                	= 25, 
        INTC_TKEY1_IRQn                	= 26,
        INTC_LED_IRQn                	= 27,
        INTC_RSVD8_IRQn                	= 28, 
        INTC_RSVD9_IRQn                	= 29, 
        INTC_RSVD10_IRQn                = 30,
        INTC_RSVD11_IRQn                = 31, 
} IRQn_Type;


void INTC_Init(void);
void force_interrupt(IRQn_Type IRQn);

void CK_CPU_EnAllNormalIrq(void);
void CK_CPU_DisAllNormalIrq(void);

#ifndef __INLINE
#define __INLINE inline
#endif
#ifndef uint32_t
#define uint32_t unsigned int
#endif

#ifndef uint8_t
#define uint8_t unsigned char
#endif

#ifndef uint16_t
#define uint16_t unsigned short
#endif
/**
 * @brief  Enable Interrupt in NVIC Interrupt Controller
 *
 * @param  IRQn   The positive number of the external interrupt to enable
 *
 * Enable a device specific interupt in the NVIC interrupt controller.
 * The interrupt number cannot be a negative value.
 */
__INLINE void NVIC_EnableIRQ(IRQn_Type IRQn);

/**
 * @brief  Disable the interrupt line for external interrupt specified
 * 
 * @param  IRQn   The positive number of the external interrupt to disable
 * 
 * Disable a device specific interupt in the NVIC interrupt controller.
 * The interrupt number cannot be a negative value.
 */
__INLINE void NVIC_DisableIRQ(IRQn_Type IRQn);

/**
 * @brief  Read the interrupt pending bit for a device specific interrupt source
 * 
 * @param  IRQn    The number of the device specifc interrupt
 * @return         always 0
 */
__INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn);

/**
 * @brief  Set the pending bit for an external interrupt
 * 
 * @param  IRQn    The number of the interrupt for set pending
 *
 * No effect.
 */
__INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn);

/**
 * @brief  Clear the pending bit for an external interrupt
 *
 * @param  IRQn    The number of the interrupt for clear pending
 *
 * No effect.
 */
__INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn);

/**
 * @brief  Read the active bit for an external interrupt
 *
 * @param  IRQn
 * @return         always 0
 *
 */
__INLINE uint32_t NVIC_GetActive(IRQn_Type IRQn);



__INLINE uint32_t NVIC_GetActiveVector(void);

/**
 * @brief  Set the priority for an interrupt
 *
 * @param  n
 * @param  priority  The priority to set
 *
 * Set the priority for the specified interrupt. The interrupt 
 * number must be positive to specify an external (device specific) 
 * interrupt.
 */
__INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);

/**
 * @brief  Read the priority for an interrupt
 *
 * @param  IRQn      The number of the interrupt for get priority
 * @return           The priority for the interrupt
 *
 * Read the priority for the specified interrupt. The interrupt 
 * number must be positive to specify an external (device specific) 
 * interrupt.
 */
__INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn);

/**
 * @brief  Encode the priority for an interrupt
 *
 * @param  PriorityGroup    The used priority group, no effect
 * @param  PreemptPriority  The preemptive priority value (starting from 0)
 * @param  SubPriority      The sub priority value (starting from 0), no effect
 * @return                  equals PreemptPriority
 *
 * Encode the priority for an interrupt with the given priority group,
 * preemptive priority value and sub priority value.
 * In case of a conflict between priority grouping and available
 * priority bits (__NVIC_PRIO_BITS) the samllest possible priority group is set.
 *
 * this demo does not support subpriority, so only the preemptive priority will affect.
 *
 * The returned priority value can be used for NVIC_SetPriority(...) function
 */
__INLINE uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority);

/**
 * @brief  Decode the priority of an interrupt
 *
 * @param  Priority           The priority for the interrupt
 * @param  PriorityGroup      The used priority group
 * @param  pPreemptPriority   The preemptive priority value (starting from 0)
 * @param  pSubPriority       The sub priority value (starting from 0)
 *
 * Decode an interrupt priority value with the given priority group to 
 * preemptive priority value and sub priority value.
 * In case of a conflict between priority grouping and available
 * priority bits (__NVIC_PRIO_BITS) the samllest possible priority group is set.
 *
 * this demo does not support subpriority, so *pSubPriority always be 0 
 * and *pPreemptPriority equals Priority.
 *
 * The priority value can be retrieved with NVIC_GetPriority(...) function
 */
__INLINE void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority);



__INLINE void NVIC_EnableThreshold(void);
__INLINE void NVIC_DisableThreshold(void);
__INLINE void NVIC_SetPrioThreshold(uint8_t prioshreshold);
__INLINE void NVIC_SetVectThreshold(uint8_t vectthreshold);
__INLINE void NVIC_PowerWakeUp_EnableAll(void);
__INLINE void NVIC_PowerWakeUp_DisableAll(void);
__INLINE void NVIC_PowerWakeUp_Enable(IRQn_Type irqn);
__INLINE void NVIC_PowerWakeUp_Disable(IRQn_Type irqn);

#endif   /**< asm32s003_ck802_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/


