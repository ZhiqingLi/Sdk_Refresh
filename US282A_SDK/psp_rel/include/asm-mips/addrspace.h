/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：系统地址空间
 * 作者：liminxian
 ********************************************************************************/
 
#ifndef __ASM_ADDRSPACE_H__
#define __ASM_ADDRSPACE_H__

/*
 * Memory segments (32bit kernel mode addresses)
 * These are the traditional names used in the 32-bit universe.
 */
#define KUSEG           0x00000000
#define KSEG0           0x80000000
#define KSEG1           0xa0000000
#define KSEG2           0xc0000000
#define KSEG3           0xe0000000

#define TO_PHYS_MASK    (0x1fffffff)    /* 2^^8 - 1 */
#define TO_PHYS(x)      (((x) & TO_PHYS_MASK))

#define TO_CAC(x)       (KSEG0  | ((x) & TO_PHYS_MASK))
#define TO_UNCAC(x)     (KSEG1  | ((x) & TO_PHYS_MASK))
#define TO_KMAP(x)      (KSEG2  | ((x) & TO_PHYS_MASK))
#define TO_KMAP2(x)     (KSEG3  | ((x) & TO_PHYS_MASK))
#define TO_UMAP_WT(x)   (W_THR_START_ADDR   | ((x) & TO_PHYS_MASK))

#define TO_KSEGX(x)     ((x) & (~TO_PHYS_MASK))

#endif /* __ASM_ADDRSPACE_H__ */
