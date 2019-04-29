#ifndef _PARTHUS_LMP_LINK_KEY_DB_
#define _PARTHUS_LMP_LINK_KEY_DB_

/******************************************************************************
 * MODULE NAME:   lmp_link_key_db.h 
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:    Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_link_key_db.h,v 1.12 2004/07/07 14:22:18 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_types.h"
#include "hc_event_gen.h"
#include "lmp_acl_container.h"

void LMkeydb_LM_Read_Stored_Link_Key(t_bd_addr* p_bd_addr, u_int8 read_all_flag,
                                     t_cmd_complete_event* p_cmd_complete_info);
void LMkeydb_LM_Write_Stored_Link_Key(t_bd_addr* p_bd_addr, u_int8* link_key,
                                      t_cmd_complete_event* p_cmd_complete_info);
void LMkeydb_LM_Delete_Stored_Link_Key(t_bd_addr* p_bd_addr, u_int8 delete_all_flag,
                                       t_cmd_complete_event* p_cmd_complete_info);
u_int8 LMkeydb_Read_Max_Keys(void);
u_int8 LMkeydb_Read_Num_Keys(void);
u_int8* LMkeydb_Read_Link_Key(t_bd_addr* p_bd_addr);
t_error LMkeydb_Write_Link_Key(t_bd_addr* p_bd_addr,t_link_key link_key);
t_error LMkeydb_Delete_Link_Key(t_bd_addr* p_bd_addr);
void LMkeydb_Initialise(void);


#endif

