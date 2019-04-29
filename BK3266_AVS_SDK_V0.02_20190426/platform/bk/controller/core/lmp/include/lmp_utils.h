#ifndef LMP_UTILS_H
#define LMP_UTILS_H
/**************************************************************************
 * MODULE NAME:    lmp_utils.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Link Manager Utility function declarations
 * AUTHOR:         Gary Fleming
 * DATE:           04-12-1999
 *
 * SOURCE CONTROL: $Id: lmp_utils.h,v 1.12 2004/07/07 14:22:20 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * DESCRIPTION
 *     This module contains some general utility functions used by all link
 *     manager module. 
 *        i.e.     Setting a Bd_Addr
 *                 Checking for Bd_Addr Match
 *
 **************************************************************************/

#include "sys_types.h"
#include "lmp_acl_connection.h"
#include "lmp_types.h"
/* LMP Utils */

void LMutils_Set_Bd_Addr(t_bd_addr *p_bda_dest, const t_bd_addr *p_bda_src);
int LMutils_Bd_Addr_Match(
    const t_bd_addr *p_bd_addr_1, const t_bd_addr *p_bd_addr_2);
void LMutils_Array_Copy(u_int8 ARRAY_SIZE,u_int8 *arr_1, const u_int8 *arr_2);

u_int16 LMutils_Get_Uint16(t_p_pdu p_pdu);
u_int32 LMutils_Get_Uint24(t_p_pdu p_u_int8);
u_int32 LMutils_Get_Uint32(t_p_pdu p_u_int8);
void LMutils_Set_Uint32(t_p_pdu p_buffer, u_int32 value_32_bit);
void LMutils_Set_Uint16(t_p_pdu p_buffer, u_int16 value_16_bit);
void LMutils_Set_U128bits(t_p_pdu p_buffer, u_int8* pvalue_8_bit);



/****************************************************************
 * FUNCTION : mLMutils_Set_Uint8
 *
 * DESCRIPTION
 *  Write a uint8 field to a location in memory
 ***************************************************************/
#define mLMutils_Set_Uint8(p_buffer, value_8_bit) \
    ((*(p_buffer)) = value_8_bit)

u_int8 LMutils_Alloc_u_int8_ID(u_int32* Pool);
void LMutils_Free_u_int8_ID(u_int32* pool,u_int8 id);

#endif
