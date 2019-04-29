#ifndef _PARTHUS_LMP_SCO_
#define _PARTHUS_LMP_SCO_

/**************************************************************************
 * MODULE NAME:    lmp_sco.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Header LMP SCO routines 
 * AUTHOR:         John Sheehy
 * DATE:           02-03-2000
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    02-03-2000 -   JS
 *
 * SOURCE CONTROL: $Id: lmp_sco.h,v 1.10 2004/07/07 14:22:19 namarad Exp $ 
 *
 ***************************************************************************/

#include "sys_types.h"

t_error LMPsco_LM_Add_SCO_Connection(t_connectionHandle,t_packetTypes,t_deviceIndex);
t_error LMPsco_LM_Remove_SCO_Connection(t_connectionHandle,t_p_pdu);
t_error LMPsco_LMP_SCO_Link_Req(t_deviceIndex,t_p_pdu);
t_error LMPsco_LMP_SCO_Remove_Link_Req(t_deviceIndex,t_p_pdu);

void LMPsco_LMP_SCO_Link_Req_Accepted(t_deviceIndex,t_p_pdu);
void LMPsco_LMP_SCO_Link_Req_Not_Accepted(t_deviceIndex,t_p_pdu);

enum t_sco_pdu_pkt_types { HV1_SUPPORTED=0,HV2_SUPPORTED,HV3_SUPPORTED };

#endif 
