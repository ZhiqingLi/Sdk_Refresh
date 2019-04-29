#ifndef _PARTHUS_CONNECTION_FILTER_
#define _PARTHUS_CONNECTION_FILTER_

/******************************************************************************
 * MODULE NAME:   lm_con_filter.h 
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:   
 * MAINTAINER:    Gary Fleming
 * CREATION DATE:        
 *
 * SOURCE CONTROL: $Id: lmp_con_filter.h,v 1.11 2010/05/10 23:48:54 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/
#include "sys_types.h"
#include "lmp_types.h"

typedef struct 
{
    t_classDevice class_of_device;
    t_classDevice class_of_device_mask;
    t_bd_addr bd_addr;
    u_int8 filter_condition_type;
    u_int8 auto_accept;

} t_filter;

#if (PRH_BS_CFG_SYS_FILTERS_SUPPORTED==1)

void LMfltr_Initialise(void);
t_error LMfltr_LM_Set_Filter(u_int8 filter_type, t_filter* p_filter);
u_int8  LMfltr_Connection_Filter_Check(t_bd_addr* p_bd_addr, t_classDevice device_class,
									   t_linkType link_type);

u_int8  LMfltr_Inquiry_Filter_Check(t_bd_addr* p_bd_addr, t_classDevice device_class);

#elif (PRH_BS_CFG_SYS_FILTERS_SUPPORTED==0)

#define LMfltr_Initialise()
#define LMfltr_LM_Set_Filter(filter_type, p_filter)                 COMMAND_DISALLOWED

#define LMfltr_Connection_Filter_Check(p_bd_addr, device_class)     DONT_AUTO_ACCEPT
#define LMfltr_Inquiry_Filter_Check(p_bd_addr, device_class)        TRUE

#endif /*(PRH_BS_CFG_SYS_FILTERS_SUPPORTED==1)*/


#endif
