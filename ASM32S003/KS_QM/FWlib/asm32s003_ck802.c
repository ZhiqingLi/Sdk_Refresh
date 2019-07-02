/*
  ******************************************************************************
  * @file    asm32s003_ck802.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"

/**
  * @addtogroup ASM32S003_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/**
  * @addtogroup CK802 Functions
  * @{
  */

/**
  * @brief  
  * @param  
  * @retval 
  */
void CK802_Init(void)
{
        /* Initial the Interrupt source priority level registers */
        CK802->IPR[0] = 0xC0804000;
        CK802->IPR[1] = 0xC0804000;
        CK802->IPR[2] = 0xC0804000;
        CK802->IPR[3] = 0xC0804000;
        CK802->IPR[4] = 0xC0804000;
        CK802->IPR[5] = 0xC0804000;
        CK802->IPR[6] = 0xC0804000;
        CK802->IPR[7] = 0xC0804000;

        CK802->IPTR 	 = 0x00000000;//disable threshold
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void force_interrupt(IRQn_Type IRQn)
{
	CK802->ISPR = (1 << (uint32_t)(IRQn));
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void CK_CPU_EnAllNormalIrq(void)
{
  asm  ("psrset ee,ie");
}

/**
  * @brief  
  * @param  
  * @retval 
  */
void CK_CPU_DisAllNormalIrq(void)
{
 asm  ("psrclr ie");
}

/* ##########################   NVIC functions  #################################### */
/**
 * @brief  Enable Interrupt in NVIC Interrupt Controller
 *
 * @param  IRQn   The positive number of the external interrupt to enable
 *
 * Enable a device specific interupt in the NVIC interrupt controller.
 * The interrupt number cannot be a negative value.
 */
__INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)
{

        CK802->ISER = 1 << (uint32_t)(IRQn);
}

/**
 * @brief  Disable the interrupt line for external interrupt specified
 * 
 * @param  IRQn   The positive number of the external interrupt to disable
 * 
 * Disable a device specific interupt in the NVIC interrupt controller.
 * The interrupt number cannot be a negative value.
 */
__INLINE void NVIC_DisableIRQ(IRQn_Type IRQn)
{
        CK802->ICER = 1 << (uint32_t)(IRQn);
}

/**
 * @brief  Read the interrupt pending bit for a device specific interrupt source
 * 
 * @param  IRQn    The number of the device specifc interrupt
 * @return         always 0
 */
__INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
        return (uint32_t)(CK802->ISPR);
}

/**
 * @brief  Set the pending bit for an external interrupt
 * 
 * @param  IRQn    The number of the interrupt for set pending
 *
 * No effect.
 */
__INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
	CK802->ISPR = (1 << (uint32_t)(IRQn));
}
/**
 * @brief  Clear the pending bit for an external interrupt
 *
 * @param  IRQn    The number of the interrupt for clear pending
 *
 * No effect.
 */
__INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
	CK802->ICPR = (1 << (uint32_t)(IRQn));
}

/**
 * @brief  Read the active bit for an external interrupt
 *
 * @return         always 0
 *
 */
__INLINE uint32_t NVIC_GetActive(IRQn_Type IRQn)
{
    return (CK802->IABR & (1 << IRQn));
}

__INLINE uint32_t NVIC_GetActiveVector(void)
{
	unsigned int vectactive = 0;
	//isr low 8bits gives the active vector
	vectactive = (CK802 ->ISR & 0xff);
    return vectactive;
}

/**
 * @brief  Set the priority for an interrupt
 *
 * @param  IRQn      The number of the interrupt for set priority
 * @param  priority  The priority to set ,the number rang: [0-3]
 *
 * Set the priority for the specified interrupt. The interrupt 
 * number must be positive to specify an external (device specific) 
 * interrupt.
 */
__INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{

	uint32_t tmp = ((IRQn & 0x03) << 3);
	uint8_t index = IRQn>>2;
    if(IRQn >= 0) {
    	CK802->IPR[index] &= ~(0xff << tmp);
    	CK802->IPR[index] |= priority << (tmp+6);
    }

}
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
__INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn)
{
	uint32_t tmp = ((IRQn & 0x03) << 3);
	uint8_t index = IRQn>>2;
	return (uint32_t)(CK802->IPR[index])>>(tmp + 6);
}


/*###################################################################*/
/*#############    Threshold Enable & Set Threshold   ###############*/
/*###################################################################*/

/************************************************************
 * @brief	enable NVIC threshold
 * @name: NVIC_EnableThreshold
 * @no param
 *
 */

__INLINE void NVIC_EnableThreshold(void)
{
	CK802 ->IPTR |= 0x80000000;
}

/************************************************************
 * @brief	disnable NVIC threshold
 * @name: NVIC_DisableThreshold
 * @no param
 *
 */

__INLINE void NVIC_DisableThreshold(void)
{
	CK802 ->IPTR &= ~0x80000000;
}


/************************************************************
 * @brief	set NVIC Priothreshold
 * @name: NVIC_SetPrioThreshold
 * @param prioshreshold		the priority of threshold[0,3]
 *
 */

__INLINE void NVIC_SetPrioThreshold(uint8_t prioshreshold)
{
	CK802 -> IPTR &= 0xffffff00;
	CK802 -> IPTR |= (prioshreshold << 6);
}

/************************************************************
 * @brief	set NVIC Vectthreshold
 * @name: NVIC_SetVectThreshold
 * @param vectthreshold		the vector of threshold[0,31]
 *
 */

__INLINE void NVIC_SetVectThreshold(uint8_t vectthreshold)
{
	CK802 -> IPTR &= 0xffff00ff;
	CK802 -> IPTR |= ((vectthreshold + 32) << 8);
}


/*###################################################################*/
/*################    Low Power Wakeup Enable     ###################*/
/*###################################################################*/

/*************************************************************
 * @name:	NVIC_PowerWakeUp_Enable
 * @brief:	enable the bit for Power wake up
 * @param:	irqn	the irqnumber,eg:CK802_CORETIM_IRQn
 */
__INLINE void NVIC_PowerWakeUp_Enable(IRQn_Type irqn)
{
	CK802->IWER |= (1 << irqn);
}

/*************************************************************
 * @name:	NVIC_PowerWakeUp_Disable
 * @func:	disable the bit for Power wake up
 * @param:	irqn	the irqnumber,eg:CK802_CORETIM_IRQn
 */
__INLINE void NVIC_PowerWakeUp_Disable(IRQn_Type irqn)
{
	CK802->IWDR |= (1 << irqn);
}

/*************************************************************
 * @name:	NVIC_PowerWakeUp_EnableAll
 * @func:	enable all bits for Power wake up
 * @param:	none
 */
__INLINE void NVIC_PowerWakeUp_EnableAll(void)
{
	CK802->IWER = 0xffffffff;
}

/*************************************************************
 * @name:	NVIC_PowerWakeUp_EnableAll
 * @func:	disable all bits for Power wake up
 * @param:	none
 */
__INLINE void NVIC_PowerWakeUp_DisableAll(void)
{
	CK802->IWDR = 0xffffffff;
}

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/


