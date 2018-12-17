/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：dma定义，无用
 * 作者：liminxian
 ********************************************************************************/

#ifndef __ASM_US282XX_DMA_H__
#define __ASM_US282XX_DMA_H__

#ifdef PC

#include <typeext.h>
#include <asm-mips/mach-gl5116/GL5116_Design_Specification.h>
#include <asm-mips/mach-gl5116/irq.h>


#define DMA0_AL 0
#define DMA0_COPY_STORRAGE 0
#define DMA3_LCD 3
#define DMA1_STORAGE 1
#define DMA2_NORMAL 2


#define NUM_DMA_CHANNELS_BUS	        (6)

#define DMA_CHANNEL_BUS_START           (0)

/* DMA Channel Base Addresses */
#define DMA_CHANNEL_BASE                (DMA0CTL)
#define DMA_CHANNEL_LEN			         0x20

/* DMA Channel Register Offsets */
#define     DMA_CTL_OFF          	        0x00	
#define     DMA_SRC0_OFF                    0x04
#define     DMA_SRC1_OFF                    0x08
#define     DMA_SRC2_OFF                    0x0C
#define	    DMA_DST0_OFF                    0x10
#define	    DMA_DST1_OFF                    0x14
#define	    DMA_FRAMELEN_OFF                0x18
#define	    DMA_LCDCFG_OFF                  0x1C


#ifndef _ASSEMBLER_


static inline void act_writeb_inline(unsigned char val, unsigned int reg)
{
    *(volatile unsigned char *)(reg) = val;
}

static inline void act_writew_inline(unsigned short val, unsigned int reg)
{
    *(volatile unsigned short *)(reg) = val;
}
static inline void act_writel_inline(unsigned int val, unsigned int reg)
{
    *(volatile unsigned int *)(reg) = val;
}

static inline unsigned char act_readb_inline(unsigned long port)
{
    return (*(volatile unsigned char *)(unsigned int)port);
}

static inline unsigned short act_readw_inline(unsigned long port)
{
    return (*(volatile unsigned short *)(unsigned int)port);
}

static inline unsigned int act_readl_inline(unsigned long port)
{
    return (*(volatile unsigned int *)(unsigned int)port);
}



static inline void set_dma_cpu_priority(unsigned int prio)
{
	act_writel_inline(prio&0x0000000f, (unsigned int)DMAPriority);
}


static inline int get_dma_cpu_priority(void)
{
	return (act_readl_inline((unsigned int)DMAPriority) & 0x0000000f);
}

#define dma_channel_reg(dmanr,reg)	(unsigned int)(DMA_CHANNEL_BASE + (dmanr) * DMA_CHANNEL_LEN + (reg))

static inline void reset_dma(int dmanr)
{
    act_writel_inline(act_readl_inline(dma_channel_reg(dmanr,DMA_CTL_OFF)) & (~0x01), dma_channel_reg(dmanr,DMA_CTL_OFF));  	
}

static inline void start_dma(int dmanr)
{
    act_writel_inline(act_readl_inline(dma_channel_reg(dmanr,DMA_CTL_OFF))|(0x01<<DMA0CTL_DMA0START), dma_channel_reg(dmanr,DMA_CTL_OFF));      
}

static inline bool is_dma_finished(int dmanr)
{
    return (bool)((act_readl_inline(dma_channel_reg(dmanr,DMA_CTL_OFF)) & 0x1) == 0 ? 1 : 0);                 	    
}

static inline void stop_dma(int dmanr)
{
    act_writel_inline(act_readl_inline(dma_channel_reg(dmanr,DMA_CTL_OFF))&(~(0x01<<DMA0CTL_DMA0START)), dma_channel_reg(dmanr,DMA_CTL_OFF));   	
}


#define DMA_HALT_POLL 0x5000

//RESET之后所有寄存器都恢复原来状态了，不必要clear操作，只需要set操作
//设置时也是直接读出来再或上就行，不需要mask掉。

static inline void set_dma_ctl(int dmanr, unsigned int val)
{
     act_writel_inline(act_readl_inline(dma_channel_reg(dmanr,DMA_CTL_OFF))|(val), dma_channel_reg(dmanr,DMA_CTL_OFF)); 	
}

static inline void set_dma_src0(int dmanr, unsigned int val)
{
     act_writel_inline(val, dma_channel_reg(dmanr,DMA_SRC0_OFF)); 	
}

static inline void set_dma_src1(int dmanr, unsigned int val)
{
     act_writel_inline(val, dma_channel_reg(dmanr,DMA_SRC1_OFF)); 	
}

static inline void set_dma_src2(int dmanr, unsigned int val)
{
     act_writel_inline(val, dma_channel_reg(dmanr,DMA_SRC2_OFF)); 	
}

static inline void set_dma_dst0(int dmanr, unsigned int val)
{
     act_writel_inline(val, dma_channel_reg(dmanr,DMA_DST0_OFF)); 	
}

static inline void set_dma_dst1(int dmanr, unsigned int val)
{
     act_writel_inline(val, dma_channel_reg(dmanr,DMA_DST1_OFF)); 	
}

static inline void set_dma_framelen(int dmanr, unsigned int val)
{
     act_writel_inline(val, dma_channel_reg(dmanr,DMA_FRAMELEN_OFF)); 	
}

static inline void set_dma_lcdwidth(int dmanr, unsigned int val)
{
     act_writel_inline(val, dma_channel_reg(dmanr,DMA_LCDCFG_OFF)); 	
}


static inline int get_dma_tcirq_pend(int dmanr)
{
    return (act_readl_inline((unsigned int)DMAIP) & (1 << dmanr)) ? 1 : 0; 
}

static inline int get_dma_htcirq_pend(int dmanr)
{
    return (act_readl_inline((unsigned int)DMAIP) & (1 << (4 + dmanr))) ? 1 : 0;         	    
}

static inline void clear_dma_tcirq_pend(int dmanr)
{
    act_writel_inline(1<<dmanr, (unsigned int)DMAIP);     	
}

static inline void clear_dma_htcirq_pend(int dmanr)
{
    act_writel_inline(1<<(4 + dmanr), (unsigned int)DMAIP);	    	
}

static inline void enable_dma_tcirq(int dmanr)
{
    act_writel_inline(1<<dmanr, (unsigned int)DMAIE);  	      	
}

static inline void enable_dma_htcirq(int dmanr)
{
    act_writel_inline(1<<(4 + dmanr), (unsigned int)DMAIE);    	     	
}

static inline void disable_dma_tcirq(int dmanr)
{
    act_writel_inline(act_readl_inline((unsigned int)DMAIE)&(~(1<<dmanr)), (unsigned int)DMAIE); 	       
}

static inline void disable_dma_htcirq(int dmanr)
{
    act_writel_inline(act_readl_inline((unsigned int)DMAIE)&(~(1<<(4 + dmanr))), (unsigned int)DMAIE); 
}
#endif /*_ASSEMBLER_*/
#else
#include <typeext.h>
#include <asm-mips/mach-gl5116/GL5116_Design_Specification.h>
#include <asm-mips/mach-gl5116/irq.h>


#define DMA0_AL 0
#define DMA0_COPY_STORRAGE 0
#define DMA3_LCD 3
#define DMA1_STORAGE 1
#define DMA2_NORMAL 2


#define NUM_DMA_CHANNELS_BUS	        (6)

#define DMA_CHANNEL_BUS_START           (0)

/* DMA Channel Base Addresses */
#define DMA_CHANNEL_BASE                (DMA0CTL)
#define DMA_CHANNEL_LEN			         0x20

/* DMA Channel Register Offsets */
#define     DMA_CTL_OFF          	        0x00	
#define     DMA_SRC0_OFF                    0x04
#define     DMA_SRC1_OFF                    0x08
#define     DMA_SRC2_OFF                    0x0C
#define	    DMA_DST0_OFF                    0x10
#define	    DMA_DST1_OFF                    0x14
#define	    DMA_FRAMELEN_OFF                0x18
#define	    DMA_LCDCFG_OFF                  0x1C


#ifndef _ASSEMBLER_


static inline void act_writeb_inline(unsigned char val, unsigned int reg)
{
    *(volatile unsigned char *)(reg) = val;
}

static inline void act_writew_inline(unsigned short val, unsigned int reg)
{
    *(volatile unsigned short *)(reg) = val;
}
static inline void act_writel_inline(unsigned int val, unsigned int reg)
{
    *(volatile unsigned int *)(reg) = val;
}

static inline unsigned char act_readb_inline(unsigned long port)
{
    return (*(volatile unsigned char *)port);
}

static inline unsigned short act_readw_inline(unsigned long port)
{
    return (*(volatile unsigned short *)port);
}

static inline unsigned int act_readl_inline(unsigned long port)
{
    return (*(volatile unsigned int *)port);
}



static inline void set_dma_cpu_priority(unsigned int prio)
{
	act_writel_inline(prio&0x0000000f, DMAPriority);
}


static inline int get_dma_cpu_priority(void)
{
	return (act_readl_inline(DMAPriority) & 0x0000000f);
}

static inline void reset_dma(int dmanr)
{
    act_writel_inline(act_readl_inline(DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF) & (~0x01), DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF);  	
}

static inline void start_dma(int dmanr)
{
    act_writel_inline(act_readl_inline(DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF)|(0x01<<DMA0CTL_DMA0START), DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF);      
}

static inline bool is_dma_finished(int dmanr)
{
    return (bool)((act_readl_inline(DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF) & 0x1) == 0 ? 1 : 0);                 	    
}

static inline void stop_dma(int dmanr)
{
    act_writel_inline(act_readl_inline(DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF)&(~(0x01<<DMA0CTL_DMA0START)), DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF);   	
}


#define DMA_HALT_POLL 0x5000

//RESET之后所有寄存器都恢复原来状态了，不必要clear操作，只需要set操作
//设置时也是直接读出来再或上就行，不需要mask掉。

static inline void set_dma_ctl(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_CTL_OFF); 	
}

static inline void set_dma_src0(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_SRC0_OFF); 	
}

static inline void set_dma_src1(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_SRC1_OFF); 	
}

static inline void set_dma_src2(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_SRC2_OFF); 	
}

static inline void set_dma_dst0(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_DST0_OFF); 	
}

static inline void set_dma_dst1(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_DST1_OFF); 	
}

static inline void set_dma_framelen(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_FRAMELEN_OFF); 	
}

static inline void set_dma_lcdwidth(int dmanr, unsigned int val)
{
     act_writel_inline(val, DMA_CHANNEL_BASE + dmanr * DMA_CHANNEL_LEN + DMA_LCDCFG_OFF); 	
}


static inline int get_dma_tcirq_pend(int dmanr)
{
    return (act_readl_inline(DMAIP) & (1 << dmanr)) ? 1 : 0; 
}

static inline int get_dma_htcirq_pend(int dmanr)
{
    return (act_readl_inline(DMAIP) & (1 << (4 + dmanr))) ? 1 : 0;         	    
}

static inline void clear_dma_tcirq_pend(int dmanr)
{
    act_writel_inline(1<<dmanr, DMAIP);     	
}

static inline void clear_dma_htcirq_pend(int dmanr)
{
    act_writel_inline(1<<(4 + dmanr), DMAIP);	    	
}

static inline void enable_dma_tcirq(int dmanr)
{
    act_writel_inline(1<<dmanr, DMAIE);  	      	
}

static inline void enable_dma_htcirq(int dmanr)
{
    act_writel_inline(1<<(4 + dmanr), DMAIE);    	     	
}

static inline void disable_dma_tcirq(int dmanr)
{
    act_writel_inline(act_readl_inline(DMAIE)&(~(1<<dmanr)), DMAIE); 	       
}

static inline void disable_dma_htcirq(int dmanr)
{
    act_writel_inline(act_readl_inline(DMAIE)&(~(1<<(4 + dmanr))), DMAIE); 
}
#endif /*_ASSEMBLER_*/
#endif
#endif /* __ASM_ATJ213X_DMA_H__ */
