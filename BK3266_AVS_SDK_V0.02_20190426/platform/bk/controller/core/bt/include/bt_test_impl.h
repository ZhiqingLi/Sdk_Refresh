#ifndef _PARTHUS_BT_TEST_IMPL
#define _PARTHUS_BT_TEST_IMPL

/******************************************************************************
 * MODULE NAME:    bt_test_impl.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Implementation header file for bt_test (inlines)
 * MAINTAINER:     John Sheehy
 * CREATION DATE:  1 March 2001
 *
 * SOURCE CONTROL: $Id: bt_test_impl.h,v 1.8 2004/07/07 14:21:00 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2001-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/

#include "sys_types.h"
#include "lmp_config.h"

#if (PRAGMA_INLINE==1)
#pragma inline(\
BTtst_Read_Loopback_Mode,\
BTtst_Initialise,\
BTtst_Get_DUT_Mode,\
BTtst_Set_DUT_Mode)
#endif

/* 
 * Module variables defined in bt_test.c (allow inline access)
 */
extern t_loopback_info BTtst_loopback_info;
extern t_dut_mode  BTtst_dut_mode;


/**************************************************************************
 *  FUNCTION : BTtst_Initialise
 *
 *  DESCRIPTION :
 *  Initialise dut_mode 
 *   
 **************************************************************************/
__INLINE__ void BTtst_Initialise(void)
{
    BTtst_dut_mode = DUT_DISABLED;
	BTtst_loopback_info.loopback_mode = NO_LOOPBACK;
}

/**************************************************************************
 *  FUNCTION : BTtst_Read_Loopback_Mode
 *
 *  RETURNS : t_loopback_mode - returns current loopback_mode from g_LM_config_info
 *
 *  CONTEXT: Any 
 *
 *  DESCRIPTION :
 *  Inline'd method to read the value of the device's loopback mode
 **************************************************************************/
__INLINE__ t_loopback_mode BTtst_Read_Loopback_Mode(void)
{
    return BTtst_loopback_info.loopback_mode;
}

/**************************************************************************
 *  FUNCTION : BTtst_Get_DUT_Mode
 *
 *  DESCRIPTION :
 *  Returns the current DUT Mode
 *   
 **************************************************************************/
__INLINE__ t_dut_mode BTtst_Get_DUT_Mode(void)
{
    return BTtst_dut_mode;
}

/**************************************************************************
 *  FUNCTION : BTtst_Set_DUT_Mode
 *
 *  DESCRIPTION :
 *  Set the current DUT Mode
 *   
 **************************************************************************/
__INLINE__ void BTtst_Set_DUT_Mode(t_dut_mode new_dut_mode)
{
    BTtst_dut_mode = new_dut_mode;
}

#endif /* _PARTHUS_BT_TEST_IMPL */
