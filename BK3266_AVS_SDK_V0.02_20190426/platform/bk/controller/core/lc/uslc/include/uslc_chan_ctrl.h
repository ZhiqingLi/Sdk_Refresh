#ifndef _PARTHUS_USLC_CHAN_
#define _PARTHUS_USLC_CHAN_

/******************************************************************************
 * MODULE NAME:    uslc_chac.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Channel Control Processor Module
 * MAINTAINER:     John Nelson, Conor Morris
 * DATE:           1 Feb 2000 Jun 1999
 *
 * SOURCE CONTROL: $Id: uslc_chan_ctrl.h,v 1.51 2014/07/03 17:27:19 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 * 
 ******************************************************************************/
#include "sys_types.h"
#include "bt_fhs.h"
#include "lc_interface.h"
#include "dl_dev.h"

/*
 * Prototype Declarations
 */
void USLCchac_Initialise(void);

#if (PRH_BS_CFG_SYS_LMP_MSS_SUPPORTED==1)
void USLCchac_Set_Active_Procedure(t_ulc_procedure new_procedure);
#endif

RAM_CODE t_USLC_Frame_Activity USLCchac_prepare_next_slave_frame_activity(t_devicelink 
                            **p_next_active_device_link);

t_USLC_Frame_Activity USLCchac_prepare_next_master_frame_activity(t_devicelink 
         **p_next_active_device_link,t_USLC_Frame_Activity current_USLC_Frame_Activity);

boolean USLCchac_check_slave_activity_start_next_frame(void);
boolean USLCchac_is_next_activity_scan_activation(void);

boolean USLCchac_Is_Inquiry_Scan_Rand_Wait(void);
void USLCchac_Set_Saved_Device_State(t_state state);
void USLCchac_Set_Device_State(t_state state);

RAM_CODE t_ulc_procedure USLCchac_Get_Last_Procedure(void);

RAM_CODE t_ulc_procedure USLCchac_Get_Active_Procedure(void);
RAM_CODE void USLCchac_Set_Sleep_Request(boolean sleep_request);
RAM_CODE boolean USLCchac_Get_Sleep_Request(void);
RAM_CODE t_state USLCchac_get_device_state(void);

t_error USLCchac_Procedure_Request(t_ulc_procedure procedure, boolean status);

void USLCchac_Procedure_Start(t_ulc_procedure procedure, t_devicelink *p_device_link);

DRAM_CODE void USLCchac_Procedure_Finish(boolean set_previous_super_state);

t_ulc_procedure USLCchac_Process_Rx(t_USLC_Frame_Activity next_USLC_Frame_Activity);

DRAM_CODE void USLCchac_prepareRx(t_devicelink* p_device_link);

t_error USLCchac_InqScan_Rand_Wait(t_LC_Event_Info* eventInfo);

void USLCchac_Wakeup(void);

boolean USLCchac_Is_Sleep_Possible(t_clock *next_instant);

boolean USLCchac_Is_Sleep_Request_Pending(void);

boolean USLCchac_Is_R2P_Request_Pending(void);
RAM_CODE void USLCchac_Force_To_Tx_State_If_Req(void);
#if (PRH_BS_CFG_SYS_ESCO_IN_SCATTERNET==1)
void USLCchac_Check_Switch_Piconet_IRQ_Context(u_int8 irq);
#endif
#endif
