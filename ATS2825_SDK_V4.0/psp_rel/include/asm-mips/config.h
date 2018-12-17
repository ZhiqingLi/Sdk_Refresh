/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：mips体系架构的细节配置
 * 作者：liminxian
 ********************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CONFIG_CPU_MIPSR2
//#define CONFIG_IRQ_CPU
#define CONFIG_CPU_HAS_SYNC

#define cpu_has_llsc 0
#define R10000_LLSC_WAR 0

#define _MIPS_SZLONG 32

#define CONFIG_BUG


#endif /* __CONFIG_H__ */
