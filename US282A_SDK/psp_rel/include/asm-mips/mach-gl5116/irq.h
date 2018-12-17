/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：中断处理相关宏定义、数据结构和函数声明
 * 作者：liminxian
 ********************************************************************************/

#ifndef __ASM_US282XX_IRQ_H__
#define __ASM_US282XX_IRQ_H__

#ifdef PC

#include <asm-mips/mach-gl5116/GL5116_Design_Specification.h>

#define NR_IRQS			32

#ifndef _ASSEMBLER_

#ifdef PC
    #define inline
#endif
/*
static inline void act_writeb(unsigned char val, unsigned int reg)
{
    *(volatile unsigned char *)(reg) = val;
}

static inline void act_writew(unsigned short val, unsigned int reg)
{
    *(volatile unsigned short *)(reg) = val;
}
static inline void act_writel(unsigned int val, unsigned int reg)
{
    *(volatile unsigned int *)(reg) = val;
}

static inline unsigned char act_readb(unsigned long port)
{
    return (*(volatile unsigned char *)port);
}

static inline unsigned short act_readw(unsigned long port)
{
    return (*(volatile unsigned short *)port);
}

static inline unsigned int act_readl(unsigned long port)
{
    return (*(volatile unsigned int *)port);
}
*/

#define act_writeb(val, reg) ((*((volatile unsigned char *)(reg))) = (val))

#define act_writew(val, reg) ((*((volatile unsigned short *)(reg))) = (val))

#define act_writel(val, reg) ((*((volatile unsigned int *)(reg))) = (val))

#define act_readb(port) (*((volatile unsigned char *)(port)))

#define act_readw(port) (*((volatile unsigned short *)(port)))

#define act_readl(port) (*((volatile unsigned int *)(port)))



/*!
 *  中断源 \n
 *  \li 0~31 : 共32个中断源 
 * 
 */ 
enum {  
    IRQ_BT = 0,
    IRQ_NFC,
    IRQ_2HZ,/*WD & 2HZ*/
    IRQ_TIMER1,
    IRQ_TIMER0,    
    IRQ_RTC,
    IRQ_UART0,
    IRQ_SIRQ0,
    IRQ_TOUCHKEY,
    IRQ_SPI,            
    IRQ_USB,
    IRQ_I2C,
    IRQ_UART1,
    IRQ_SIRQ1,
    IRQ_DAC_I2S_TX,
    IRQ_ADC_I2S_RX,  
    IRQ_MPU,  
    IRQ_SD_MMC,
    IRQ_DMA0,
    IRQ_DMA1,
    IRQ_DMA2,
    IRQ_DMA3,
    IRQ_DMA4,
    IRQ_DMA5,
    IRQ_PCM_RX,
    IRQ_PCM_TX,
    IRQ_SPI1,
    IRQ_OUT_USER0,
    IRQ_OUT_USER1,
    IRQ_OUT_USER2,
    IRQ_OUT_USER3,
    IRQ_OUT_USER4,    
};

/*! \cond KMODULE_API*/
/******************************************************************************/
/*!                    
* \par  Description:
*	  使能中断线
* \param[in]    unsigned int irq_nr 中断号
* \return       void  
* \ingroup      irq
* \par
* \code 
* \endcode
*******************************************************************************/
static inline void  local_enable_irq(unsigned int irq_nr)
{
	act_writel(act_readl((unsigned int)INTC_MSK)| (1<<irq_nr), (unsigned int)INTC_MSK);
}

/******************************************************************************/
/*!                    
* \par  Description:
*	  关闭中断线
* \param[in]    unsigned int irq_nr 中断号
* \return       void
* \ingroup      irq
* \par
* \code 
* \endcode
*******************************************************************************/
static inline void  local_disable_irq(unsigned int irq_nr)
{
 	act_writel(act_readl((unsigned int)INTC_MSK)&~(1<<irq_nr), (unsigned int)INTC_MSK);
}
/*! \endcond */

#endif /*_ASSEMBLER_*/
#else
#include <asm-mips/mach-gl5116/GL5116_Design_Specification.h>

#define NR_IRQS			32

#ifndef _ASSEMBLER_

#ifdef PC
    #define inline
#endif
/*
static inline void act_writeb(unsigned char val, unsigned int reg)
{
    *(volatile unsigned char *)(reg) = val;
}

static inline void act_writew(unsigned short val, unsigned int reg)
{
    *(volatile unsigned short *)(reg) = val;
}
static inline void act_writel(unsigned int val, unsigned int reg)
{
    *(volatile unsigned int *)(reg) = val;
}

static inline unsigned char act_readb(unsigned long port)
{
    return (*(volatile unsigned char *)port);
}

static inline unsigned short act_readw(unsigned long port)
{
    return (*(volatile unsigned short *)port);
}

static inline unsigned int act_readl(unsigned long port)
{
    return (*(volatile unsigned int *)port);
}
*/

#define act_writeb(val, reg) ((*((volatile unsigned char *)(reg))) = (val))

#define act_writew(val, reg) ((*((volatile unsigned short *)(reg))) = (val))

#define act_writel(val, reg) ((*((volatile unsigned int *)(reg))) = (val))

#define act_readb(port) (*((volatile unsigned char *)(port)))

#define act_readw(port) (*((volatile unsigned short *)(port)))

#define act_readl(port) (*((volatile unsigned int *)(port)))



/*!
 *  中断源 \n
 *  \li 0~31 : 共32个中断源 
 * 
 */ 
enum {  
    IRQ_BT = 0,
    IRQ_NFC,
    IRQ_2HZ,/*WD & 2HZ*/
    IRQ_TIMER1,
    IRQ_TIMER0,    
    IRQ_RTC,
    IRQ_UART0,
    IRQ_SIRQ0,
    IRQ_TOUCHKEY,
    IRQ_SPI,            
    IRQ_USB,
    IRQ_I2C,
    IRQ_UART1,
    IRQ_SIRQ1,
    IRQ_DAC_I2S_TX,
    IRQ_ADC_I2S_RX,  
    IRQ_MPU,  
    IRQ_SD_MMC,
    IRQ_DMA0,
    IRQ_DMA1,
    IRQ_DMA2,
    IRQ_DMA3,
    IRQ_DMA4,
    IRQ_DMA5,
    IRQ_PCM_RX,
    IRQ_PCM_TX,
    IRQ_SPI1,
    IRQ_OUT_USER0,
    IRQ_OUT_USER1,
    IRQ_OUT_USER2,
    IRQ_OUT_USER3,
    IRQ_OUT_USER4,    
};


/*! \cond KMODULE_API*/
/******************************************************************************/
/*!                    
* \par  Description:
*	  使能中断线
* \param[in]    unsigned int irq_nr 中断号
* \return       void  
* \ingroup      irq
* \par
* \code 
* \endcode
*******************************************************************************/
static inline void  local_enable_irq(unsigned int irq_nr)
{
	act_writel(act_readl(INTC_MSK)| (1<<irq_nr), INTC_MSK);
}

/******************************************************************************/
/*!                    
* \par  Description:
*	  关闭中断线
* \param[in]    unsigned int irq_nr 中断号
* \return       void
* \ingroup      irq
* \par
* \code 
* \endcode
*******************************************************************************/
static inline void  local_disable_irq(unsigned int irq_nr)
{
 	act_writel(act_readl(INTC_MSK)&~(1<<irq_nr), INTC_MSK);
}
/*! \endcond */

#endif /*_ASSEMBLER_*/
#endif
#endif /*__ASM_US282XX_IRQ_H__*/
