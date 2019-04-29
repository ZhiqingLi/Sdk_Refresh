#ifndef _PARTHUS_LMP_TIMING_INFO_
#define _PARTHUS_LMP_TIMING_INFO_

/******************************************************************************
 * MODULE NAME:    lmp_timing_info.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_timing_info.h,v 1.4 2004/07/07 14:22:20 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/

#include "sys_types.h"

t_error LMPti_LMP_Clock_Offset_Req(t_deviceIndex device_index, t_p_pdu p_payload);
t_error LMPti_LMP_Clock_Offset_Res(t_deviceIndex device_index, t_p_pdu p_payload);

#endif
