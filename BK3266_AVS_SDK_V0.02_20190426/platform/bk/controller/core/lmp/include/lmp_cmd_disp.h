#ifndef _PARTHUS_LMP_CMD_DISP_
#define _PARTHUS_LMP_CMD_DISP_

/*
 * MODULE NAME:    lmp_cmd_disp.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    
 * AUTHOR:         Gary Fleming
 * DATE:           
 *
 * SOURCE CONTROL: $Id: lmp_cmd_disp.h,v 1.15 2005/05/18 09:41:06 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 */
#include "lmp_config.h"

t_error LMdisp_LMP_Command_Dispatcher(void);
u_int8  LMdisp_Get_Dispatched_LMP_PDU_Length(void);
t_error LMdisp_LMP_Accepted(t_lmp_link *p_link , t_p_pdu payload);
t_error LMdisp_LMP_Accepted_Ext(t_lmp_link *p_link , t_p_pdu payload);
t_error LMdisp_LMP_Not_Accepted(t_lmp_link *p_link, t_p_pdu payload);
t_error LMdisp_LMP_Not_Accepted_Ext(t_lmp_link *p_link, t_p_pdu payload);
t_error LMdisp_LMP_Ignore_Command(t_lmp_link* p_link, t_p_pdu p_payload);
t_error LMdisp_LMP_Unsupported_Command(t_lmp_link  *p_link, t_p_pdu p_payload);

t_error LMdisp_LMP_Accepted_Common(t_lmp_link *p_link, t_p_pdu payload,
								   u_int8 extended_flag);
t_error LMdisp_LMP_Not_Accepted_Common(t_lmp_link *p_link, t_p_pdu payload,
								   u_int8 extended_flag);

typedef t_error (*t_lmp_funct)(t_lmp_link*, t_p_pdu);

#endif
