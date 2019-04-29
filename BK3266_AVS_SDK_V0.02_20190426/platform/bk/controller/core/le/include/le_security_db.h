#ifndef _CEVA_LE_SECURITY_DB__
#define _CEVA_LE_SECURITY_DB__

/*********************************************************************
 * MODULE NAME:     le_security_db.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:
 * MAINTAINER:      Gary Fleming
 * DATE:            Jan-2012
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2012 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/
#include "config.h"
#ifdef BT_DUALMODE
u_int8 LEsecdb_Get_IRK_BdAddr_Pairs(u_int8 role,u_int8 max_pairs,u_int8** p_bd_addr_list,u_int8**  p_IRK_list);
u_int8 LEsecdb_Add_DB_Entry(u_int8 address_type,u_int8* p_address, u_int8 role,u_int8 entity,u_int16* p_ediv, u_int8* key);
u_int8 LEsecdb_Search_DB(u_int8 address_type, u_int8* p_address, u_int8 role,u_int8 entity,u_int16* p_ediv, u_int8* key );
t_error LEsecdb_Add_Device_To_SecDB(u_int8 address_type, t_p_pdu p_pdu);
t_error LEsecdb_Clear_Security_DB(void);

#endif
#endif
