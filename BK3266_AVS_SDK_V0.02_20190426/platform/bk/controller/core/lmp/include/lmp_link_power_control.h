#ifndef __PARTHUS_LMP_LINK_POWER_CONTROL__
#define __PARTHUS_LMP_LINK_POWER_CONTROL__
/***********************************************************************
 *
 * HEADER NAME:    lmp_link_power_control.c
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Scans links requesting peer to update Tx power if required
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  30 August 2001
 *
 * SOURCE CONTROL: $Id: lmp_link_power_control.h,v 1.6 2005/01/25 16:28:36 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2001-2004 Ceva Inc.
 *     All rights reserved.
 *
 *    
 ***********************************************************************/

#include "dl_dev.h"

#if (PRH_BS_CFG_SYS_LMP_POWER_CONTROL_SUPPORTED==1) 
/*
 * Function prototypes
 */
void LMlpctrl_Scan_All_Links_Request_Peer_Power_If_Reqd(t_lmp_link* p_link);
void LMlpctrl_Incr_Power_Req_Ack(t_deviceIndex);
void LMlpctrl_Decr_Power_Req_Ack(t_deviceIndex);
void LMlpctrl_Power_Req_Ack(t_lmp_link*);

#else
/*
 * No lmp power control, hence empty functions
 */
#define LMlpctrl_Scan_All_Links_Request_Peer_Power_If_Reqd(p_link);
#endif

#endif
