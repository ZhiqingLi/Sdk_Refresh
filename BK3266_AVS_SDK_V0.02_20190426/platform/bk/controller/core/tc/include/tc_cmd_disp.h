#ifndef _PARTHUS_TC_CMD_DISP_
#define _PARTHUS_TC_CMD_DISP_

/**********************************************************************
 *
 * MODULE NAME:    tc_cmd_disp.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Test Control Command Dispatcher
 * MAINTAINER:     Daire McNamara <Daire.McNamara@sslinc.com>
 * CREATION DATE:  13th April 2000
 *
 * SOURCE CONTROL: $Id: tc_cmd_disp.h,v 1.7 2004/07/07 14:22:37 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 **********************************************************************/

#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)
t_error TCI_Dispatch_Prop_Command(u_int8* p_payload, u_int16 opcode);
#else
#define TCI_Dispatch_Prop_Command(p_payload, opcode)   ILLEGAL_COMMAND
#endif
#endif
