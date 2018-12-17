/**
 *******************************************************************************
 * @file    cache_tcm.h
 * @author  Robert, Aissen Li
 * @version V2.0.0
 * @date    2014/09/22
 *
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2014 MVSilicon </center></h2>
 */

/**
 * @addtogroup CACHE_TCM
 * @{
 * @defgroup cache_tcm cache_tcm.h
 * @{
 */

#ifndef __CACHE_TCM_H__
#define __CACHE_TCM_H__

#include <string.h>

/**
 * @brief
 * 缓存/紧耦合内存主要用于加速指令取指
 * 芯片内部包含224KB内存，主要分为以下7个物理内存块，这些内存块满足以下特性：
 * Note: 1. 每块内存块类型必须为：缓存/紧耦合内存/普通内存 之一
 *       2. 内存块顺序必须为: 缓存，紧耦合内存，普通内存
 *       3. PM8只能为紧耦合内存. PM7只能为普通内存
 *
 *       224K   208KB  192KB   128K      64K       0
 *       |  PM0 |  PM1 |  PM2  |   PM3   |   PM7   | PM8 |
 *       | 16KB | 16KB | 64KB  |  64KB   |  64KB   | 4KB |
 *                                                                                
 * Cache |------------>|                                                  
 * TCM   |------------------------------>|         |*****|
 * XMEM  |<------------------------------|*********|
 * e.g.  |    Cache    |  TCM  |       XMEM        | TCM |
 *
 * PM0: 缓存 / 紧耦合内存 / 普通内存
 * PM1: 缓存 / 紧耦合内存 / 普通内存
 * PM2: 紧耦合内存 / 普通内存
 * PM3: 紧耦合内存 / 普通内存
 * PM7: 普通内存
 * PM8: 紧耦合内存
 */

/**
 * @brief 物理内存块定义
 */
typedef enum
{
	PHYMEM_NONE 	=   (0),
	PHYMEM_16KBPM0	=	(1 << 0),
	PHYMEM_16KBPM1	=	(1 << 1),
	PHYMEM_64KBPM2	=	(1 << 2),
	PHYMEM_64KBPM3	=	(1 << 3),
	PHYMEM_64KBPM7	=	(1 << 7),
	PHYMEM_4KBPM8	=	(1 << 8),
	PHYMEM_MASKALL	=	(0x1FF),
} PHYMEM_TYPE_T;

/**
 * @brief 缓存 / 紧耦合内存配置参数结构体
 */
typedef struct PHYMEM_SELECT
{
	PHYMEM_TYPE_T	CacheMemSel;
	PHYMEM_TYPE_T	TcmMemSel;
	PHYMEM_TYPE_T	XmemSel;
	unsigned int	Tcm0CodeAddr;
	unsigned int 	Tcm0CodeSz;
	unsigned int	Tcm1CodeAddr;
	unsigned int 	Tcm1CodeSz;
} PHYMEM_CONTROL_T;

/**
 * @brief 缓存 / 紧耦合内存初始化
 * @param[in] PhyMemCtl 缓存 / 紧耦合内存配置参数结构体指针 
 * @return 0: 表示初始化成功，其他值表示初始化失败
 */
int CacheTcmInit(PHYMEM_CONTROL_T* PhyMemCtl);

/**
 * @brief 缓存 / 紧耦合内存快速初始化
 * @param[in] CacheMemSel  缓存内存块选择 
 * @param[in] Tcm0MemSel   紧耦合内存块选择
 * @param[in] Tcm0CodeAddr 常驻代码区域0起始地址
 * @param[in] Tcm0CodeSz   常驻代码区域0大小
 * @param[in] Tcm1CodeAddr 常驻代码区域1起始地址
 * @param[in] Tcm1CodeSz   常驻代码区域1大小
 * @return 0: 表示初始化成功，其他值表示初始化失败
 */
static inline int CacheTcmInitFast(PHYMEM_TYPE_T CacheMemSel, PHYMEM_TYPE_T Tcm0MemSel, long Tcm0CodeAddr, int Tcm0CodeSz, PHYMEM_TYPE_T Tcm1MemSel, long Tcm1CodeAddr, int Tcm1CodeSz)
{
	PHYMEM_CONTROL_T PhyMemCtl;

	/*
	 * set to default value
	 */
	memset(&PhyMemCtl, 0, sizeof(PHYMEM_CONTROL_T));

	PhyMemCtl.XmemSel       = PHYMEM_16KBPM0 | PHYMEM_16KBPM1 | PHYMEM_64KBPM2 | PHYMEM_64KBPM3;
	PhyMemCtl.CacheMemSel   = CacheMemSel;
	PhyMemCtl.TcmMemSel     = Tcm0MemSel | Tcm1MemSel;
	PhyMemCtl.XmemSel      &= ~(Tcm0MemSel | Tcm1MemSel | CacheMemSel);
	PhyMemCtl.Tcm0CodeAddr  = Tcm0CodeAddr;
	PhyMemCtl.Tcm0CodeSz    = Tcm0CodeSz;
	PhyMemCtl.Tcm1CodeAddr  = Tcm1CodeAddr;
	PhyMemCtl.Tcm1CodeSz    = Tcm1CodeSz;

	return CacheTcmInit(&PhyMemCtl);
}

/**
 * @brief  缓存 / 紧耦合内存去初始化
 * @param	 无
 * @return 无
 */
void CacheTcmDeInit(void);

/**
 * @brief  缓存 / 紧耦合内存控制命令
 */
typedef enum {

    CACHETCM_IOCTL_GET_XMEM_USGBLK          = 0x10,
    CACHETCM_IOCTL_GET_XMEM_FREEBLK,

    CACHETCM_IOCTL_DIS_CACHE                = 0x20,
    CACHETCM_IOCTL_DIS_TCM,

    CACHETCM_IOCTL_SELMAP_SOURCE            = 0x30,
    CACHETCM_IOCTL_SELMAP_MEMORY,

    CACHETCM_IOCTL_DBGSTAT_ACCESS_CNT_CM3   = 0xE0,
    CACHETCM_IOCTL_DBGSTAT_ACCESS_CNT_SLV,
    CACHETCM_IOCTL_DBGSTAT_MISS_CNT_BASIC,
    CACHETCM_IOCTL_DBGSTAT_MISS_CNT_ADV,
    CACHETCM_IOCTL_DBGSTAT_PENALTY_CYCLE_CNT_CM3,
    CACHETCM_IOCTL_DBGSTAT_PENALTY_CYCLE_CNT_SLV,
    CACHETCM_IOCTL_DBGSTAT_MISS_ADDR,
    CACHETCM_IOCTL_DBGSTAT_BIST_ERR_CNT,
    CACHETCM_IOCTL_DBGSTAT_DECRYPT_BIST_ERR_CNT,
    CACHETCM_IOCTL_RESET_STATISTIC_INFO,
} CACHETCM_IOCTL_T;

/**
 * @brief  缓存 / 紧耦合内存控制
 * @param  Cmd	 缓存 / 紧耦合内存控制命令
 * @param  Arg	 缓存 / 紧耦合内存控制参数
 * @return 控制结果
 */
int CacheTcmIOctl(CACHETCM_IOCTL_T Cmd, ...);

#endif //__CACHE_TCM_H__

/**
 * @}
 * @}
 */
