#ifndef _PARTHUS_HC_CMD_DISP_
#define _PARTHUS_HC_CMD_DISP_

/**********************************************************************
 * MODULE NAME:    hc_cmd_disp.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Host Controller Command Dispatcher
 * MAINTAINER:     Gary Fleming
 * DATE:           27 / 05 / 99
 *
 * SOURCE CONTROL: $Id: hc_cmd_disp.h,v 1.12 2007/11/13 13:17:31 nicolal Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     xx xxx 1999 -   gf       - Initial Version 
 *
 **********************************************************************/

t_error HC_Command_Dispatcher(void);

/* 2846*/
#define map_hci_reason_to_lmp_reason(x) ( (x==UNSUPPORTED_FEATURE)?UNSUPPORTED_REMOTE_FEATURE:x)


#endif
