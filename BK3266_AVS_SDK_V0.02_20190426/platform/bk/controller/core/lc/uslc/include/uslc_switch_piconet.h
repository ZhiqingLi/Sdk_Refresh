#ifndef _PARTHUS_USLC_SWITCH_PICONET_
#define _PARTHUS_USLC_SWITCH_PICONET_
#include "config.h"

/***********************************************************************
 *
 * HEADER NAME:    uslc_switch_piconet.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LC Portion of Scatternet Interpiconet Switch Procedure
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  8 April 2002
 *
 * SOURCE CONTROL: $Id: uslc_switch_piconet.h,v 1.8 2014/07/03 17:27:19 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2002-2004 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/

void USLCsp_Initialise(void);
t_error USLCsp_Request(t_deviceIndex device_index, boolean sp_override);
t_error USLCsp_Cancel(void);
u_int8 USLCsp_State_Dispatcher(boolean);

t_deviceIndex USLCsp_Get_Activated_Device_Index(void);
t_error USLCsp_Set_Activated_Device_Index(t_deviceIndex device_index);
#ifdef BT_DUALMODE

void USLCsp_BLE_Recover_Piconet_Clk(void);
#endif
#if (PRH_BS_CFG_SYS_SCO_REPEATER_SUPPORTED==1)
void USLCsp_SCO_Rep_Switch(t_devicelink* p_dev_link);
#endif

#if (PRH_BS_CFG_SYS_ESCO_IN_SCATTERNET==1)
void USLCsp_Set_eSCO_Scatternet_Device_Link(t_devicelink* p_esco_devlink);
void USLCsp_Set_eSCO_Scatternet_Other_Link(t_devicelink* p_other_devlink);
void USLCsp_Set_eSCO_Next_Time(u_int32 next_esco_clk_time );
void USLCsp_Set_eSCO_Scatternet_pSCO_Link(t_sco_info* p_esco_link);

t_devicelink* USLCsp_Get_eSCO_Scatternet_Device_Link(void);
t_devicelink* USLCsp_Get_eSCO_Scatternet_Other_Link(void);
t_deviceIndex USLCsp_Get_eSCO_Scatternet_Device_Index(void);
t_deviceIndex USLCsp_Get_eSCO_Scatternet_Other_Device_Index(void);

u_int32 USLCsp_Get_eSCO_Next_Time(void);

t_sco_info* USLCsp_Get_eSCO_Scatternet_pSCO_Link(void);
#endif
#define USLCsp_RESUME_PICONET FALSE
#define USLCsp_CHANGE_PICONET TRUE

#endif

