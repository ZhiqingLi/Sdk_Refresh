#ifndef __PARTHUS_LC_LOG
#define __PARTHUS_LC_LOG

/*************************************************************************
 * MODULE NAME:    lc_log.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Link Controller Interface Module
 * MAINTAINER:     Conor Morris
 *
 * SOURCE CONTROL: $Id: lc_log.h,v 1.5 2004/07/07 14:21:36 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 *************************************************************************/

#if (PRH_BS_DBG_LC_LOG==1 || PRH_BS_CFG_SYS_TRACE_LC_VIA_HCI_SUPPORTED)
/*************************************************************************
 * Set of LC logging functions for debug purposes only.
 *
 *************************************************************************/
typedef struct t_lc_log_procedure_info
{
    u_int16 procedure_request;
    u_int16 procedure_accept;
    u_int16 procedure_start;
    u_int16 procedure_cancel;
    u_int16 pkts_recv_state1;
    u_int16 pkts_recv_state2;
} t_lc_log_procedure_info;
void LC_Log_Initialise(void);
void LC_Log_Procedure_Request(t_ulc_procedure procedure);
void LC_Log_Procedure_Accept(t_ulc_procedure procedure);
void LC_Log_Procedure_Start(t_ulc_procedure procedure);
void LC_Log_Procedure_Cancel(t_ulc_procedure procedure);
void LC_Log_Procedure_Recv_Pkts_State1(t_ulc_procedure procedure);
void LC_Log_Procedure_Recv_Pkts_State2(t_ulc_procedure procedure);

#else 
/*
 * No LC Log, hence ignore
 */
#define LC_Log_Initialise()
#define LC_Log_Procedure_Request(foo)
#define LC_Log_Procedure_Accept(foo)
#define LC_Log_Procedure_Start(foo)
#define LC_Log_Procedure_Cancel(foo)
#define LC_Log_Procedure_Recv_Pkts_State1(foo)
#define LC_Log_Procedure_Recv_Pkts_State2(foo)

#endif

#endif
