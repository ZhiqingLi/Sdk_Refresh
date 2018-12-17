/********************************************************************************
 *                              USDK(GL5116)
 *                            Module: KERNEL
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>      <time>           <version >             <desc>
 *      LMX         2014-7-22 10:50     1.0             build this file
 ********************************************************************************/
/*!
 * \file     sys_board_def.h
 * \brief    
 * \author   
 * \version  1.0
 * \date  2014/7/22
 *******************************************************************************/
#ifndef _SYS_BOARD_DEF_H__
#define _SYS_BOARD_DEF_H__


#define FOR_FPGA          0   //TEST  FPGA版本
#define FOR_TESTCHIP_EVB  1   //TEST  EVB版本
#define FOR_TESTCHIP_DEMO 2   //TEST  DEMO版本
#define FOR_IC_EVB        3   // IC EVB版本
#define FOR_IC_DEMO       4   // IC DEMO版本


#define SYS_BOARD_TYPE  FOR_IC_EVB
//#define SYS_BOARD_TYPE  FOR_IC_EVB

//#if(SYS_BOARD_TYPE < FOR_IC_EVB)//暂时定义，兼容之前的FPGA_DEBUG 定义的代码 
//#define FPGA_DEBUG
//#endif

//监控系统时间片利用率
//#define SYS_CHECK_THREAD_USEAGE

//监控系统栈使用
#define SYS_CHECK_STK_SIZE

//监控系统栈利用率
//#define SYS_CHECK_STK_USAGE


#endif  /*_SYS_BOARD_DEF_H__*/

