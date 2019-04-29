#ifndef _PARTHUS_USLC_SCHED_
#define _PARTHUS_USLC_SCHED_

#include "lc_types.h"
#include "config.h"

/******************************************************************************
 * MODULE NAME:    uslc_scheduler.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Controller Module
 * MAINTAINER:     John Nelson, Conor Morris
 *
 * SOURCE CONTROL: $Id: uslc_scheduler.h,v 1.38 2014/09/16 06:51:27 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/

RAM_CODE void USLCsched_Tx_Start(void);
RAM_CODE void USLCsched_Rx_Start(void) ;
RAM_CODE void USLCsched_Tx_Complete(void);
RAM_CODE void USLCsched_Rx_Complete(void);
RAM_CODE void USLCsched_Prepare_Tx(void);
RAM_CODE void USLCsched_Process_Rx(void);
void USLCsched_Initialise(void);

t_error USLCsched_Local_Piconet_Request(void);
void USLCsched_Update_Chac_Procedure_If_Req(void);
RAM_CODE void USLCsched_Set_Next_Frame_Activity(t_USLC_Frame_Activity next_USLC_Frame_Activity);
DRAM_CODE t_USLC_Frame_Activity USLCsched_Get_Next_Frame_Activity(void);
DRAM_CODE t_USLC_Frame_Activity USLCsched_Get_Current_Frame_Activity(void);

t_devicelink* USLCsched_Get_Active_Device_Link(void);
void USLCsched_Update_Chac_Procedure_If_Req(void);
void USLCsched_Set_Next_DL_Activity(t_devicelink *pDL);
#ifdef BT_DUALMODE

RAM_CODE u_int8 Check_need_to_switch_BLE(void);
void Immediate_Config_Switch_To_LE(void);

void USLCsched_Force_switch_piconet(void);
void Set_SCO_tx_complete_time(void);
u_int32 Get_SCO_tx_complete_time(void);
u_int8 Immediate_switch_BLE_Active(void);
#endif



#endif
