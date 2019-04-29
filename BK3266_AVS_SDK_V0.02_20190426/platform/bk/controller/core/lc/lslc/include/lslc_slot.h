#ifndef _PARTHUS_LSLC_SLOT_
#define _PARTHUS_LSLC_SLOT_

/*************************************************************************
 *
 * HEADER NAME:    lslc_slot.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Slot Handling Code to simplify USLC state machines for
 *                 page and inquiry 
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  16 March 2000
 *
 * SOURCE CONTROL: $Id: lslc_slot.h,v 1.31 2007/10/05 13:41:56 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 *    16 March 2000        IG          Initial Version
 *    20 March 2000        CM          State Machine Diagrams from USLCsched
 *
 ***************************************************************************/

#include "dl_dev.h"

void LSLCslot_Initialise(void);
DRAM_CODE void LSLCslot_Handle_PKA(void);
DRAM_CODE void LSLCslot_Handle_PKD(void);
RAM_CODE void LSLCslot_Handle_NO_PKT_RCVD(void);
DRAM_CODE void LSLCslot_Handle_PKD_RX_HDR(void);
DRAM_CODE void LSLCslot_Handle_TIM0(void);
DRAM_CODE void LSLCslot_Handle_TIM1(void);
DRAM_CODE void LSLCslot_Handle_TIM2(void);
DRAM_CODE void LSLCslot_Handle_TIM3(void);
DRAM_CODE void LSLCslot_Handle_SYNC_DET(void);
RAM_CODE void LSLCslot_Handle_AUX_TIM(void);

RAM_CODE boolean LSLCslot_Local_Piconet_Request(void);

DRAM_CODE boolean LSLCslot_Write_BT_Clk_Avoid_Race(t_clock const * p_bt_clk);

/***********************************************************************
 * Externally available functions to control interrupt logging if active
 **********************************************************************/
#if (PRH_BS_DBG_LC_LSLC_SLOT==1)
void LSLCslot_Enable_Interrupt_Log_If_Active(void);
void LSLCslot_Disable_Interrupt_Log_If_Active(void);
#else
#define LSLCslot_Enable_Interrupt_Log_If_Active()
#define LSLCslot_Disable_Interrupt_Log_If_Active()
#endif

#endif

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1)
void LSLCslot_eSCO_tx_3slot_packet_expected(boolean expected);
void LSLCslot_eSCO_rx_3slot_packet_expected(boolean expected);
#endif

#if (BUILD_TYPE==UNIT_TEST_BUILD)
boolean LSLCslot_Get_eSCO_tx_3slot_packet_expected(void);
boolean LSLCslot_Get_eSCO_rx_3slot_packet_expected(void);
#endif
