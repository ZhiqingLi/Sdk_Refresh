#ifndef _PARTHUS_LSLC_IRQ_
#define _PARTHUS_LSLC_IRQ_

/******************************************************************************
 * MODULE NAME:    lslc_irq.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Hardware Interrupt Functions
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  09 August 1999
 *
 * SOURCE CONTROL: $Id: lslc_irq.h,v 1.16 2013/11/27 13:20:30 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    09 Aug 1999 - IG       - Initial Version V0.9
 *    02 Feb 2001 - IG       - Split from SYSirq
 *
 ******************************************************************************/

#include "sys_types.h"

typedef void (*t_dsr_pointer)(void);

void LSLCirq_Initialise(void);

#if (PRH_BS_DEV_USE_DELAYED_SERVICE_ROUTINES == 1)

//t_dsr_pointer LSLCirq_Tabasco_ISR(void);
void (*LSLCirq_Tabasco_ISR(void))(void);

void LSLCirq_Unmask_Tim0(void);
void LSLCirq_Unmask_Tim1(void);
void LSLCirq_Unmask_Tim2(void);
void LSLCirq_Unmask_Tim3(void);
void LSLCirq_Unmask_PKA(void);
void LSLCirq_Unmask_SYNC_DET(void);
void LSLCirq_Unmask_AUX_TIM(void);
void LSLCirq_Unmask_NO_PKT_RCVD(void);
void LSLCirq_Unmask_PKD_RX_HDR(void);
void LSLCirq_Unmask_PKD(void);

//void (*LSLCirq_Tabasco_ISR(void))(void);
#endif   /* (PRH_BS_DEV_USE_DELAYED_SERVICE_ROUTINES == 1) */

DRAM_CODE void LSLCirq_IRQ_Handler(void);

void LSLCirq_Disable_All_Intr_Except(u_int32 except_tim_intr_mask);

RAM_CODE void LSLCirq_R2P_Turn_On_Intr(void);

void LSLCirq_Disable_And_Clear_Tabasco_Intr(void);

void LSLCirq_Enable_Aux_Tim_Intr(void);
void LSLCirq_Disable_Aux_Tim_Intr(void);
void LSLCirq_Disable_All_Tim_Intr_Except(u_int32 except_tim_intr_mask);

void LSLCirq_Enable_Sync_Det_Intr(void);
void LSLCirq_Disable_Sync_Det_Intr(void);

typedef struct
{
    u_int tim0_count, tim1_count, tim2_count, tim3_count,
        pka_count,
        pkd_count, no_pkt_rcvd_count, pkd_rx_hdr_count, pkd_rx_pay_hdr_count,
        total_irq_count,
        sync_det_count,
        aux_tim_count;
} t_LSLCirq_Counters; 


#endif
