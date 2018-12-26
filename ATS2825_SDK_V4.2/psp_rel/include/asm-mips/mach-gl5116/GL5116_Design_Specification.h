/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：定义 方案 SPEC 类型
 * 作者：liminxian
 ********************************************************************************/


/*
* Automatically include design specification's head file
* us282a sofeware depend on  FPGA/EVB/DEMO  for IC 5116 OR 6091
* 2014-7-22 14:15
*/
#ifndef __DESIGN_SPECIFICATION_DEFINE_H__
#define __DESIGN_SPECIFICATION_DEFINE_H__

#include "../../sys_board_def.h"

#if(SYS_BOARD_TYPE == FOR_FPGA)
#include "DEF_GL5116_Design_Specification.h"
//#error "FPGA spc"

#elif(SYS_BOARD_TYPE < FOR_IC_EVB)
//#include "DEF_GL6091_Design_Specification.h"
//#error "6091 spc"
#else

#include "DEF_GL5116_Design_Specification.h"
//#error "5116 IC spc"

#endif

#endif

