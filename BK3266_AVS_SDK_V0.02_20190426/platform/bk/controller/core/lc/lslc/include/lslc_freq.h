#ifndef _PARTHUS_LSLC_FREQ_
#define _PARTHUS_LSLC_FREQ_
/*************************************************************************
 * MODULE NAME:    lslc_freq.c
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LSLC_Frequency     Determine and Set Radio Frequency
 * MAINTAINER:     John Nelson
 * CREATION DATE:  19 Jun 2000
 *
 * SOURCE CONTROL: $Id: lslc_freq.h,v 1.21 2004/10/06 15:57:43 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * Notes:
 *    1. Extracted all frequency aspects from lslc_access
 *
 * ISSUES:
 *
 ************************************************************************/

#include "dl_dev.h"
#include "sys_types.h"

/*
 * Initialise any frequency related variables
 */
void LSLCfreq_Initialise(void);

/*
 * Prepare the frequency settings and setup for sequence to radio
 */
DRAM_CODE void LSLCfreq_Prepare_Frequency_Context(const t_devicelink *p_device_link,
     t_frame_pos freq_pos_required, t_state context);

/*
 * Prepare the frequency settings optimised for Connection
 */
RAM_CODE void LSLCfreq_Prepare_Frequency_Connection(
    const t_devicelink *p_device_link, t_frame_pos freq_pos_required);

/*
 * Support a R2P frequency where expected master tx clock is presented
 */
void LSLCfreq_Prepare_Rx_Frequency_R2P(
    const t_devicelink *p_device_link, t_clock rx_freq_clk);

/*
 * Get the last frequency calculated for the presented position
 */
RAM_CODE t_freq LSLCfreq_Get_Frequency(t_frame_pos freq_pos_required);

/*
 * Get the clock used for the last frequency calculation
 */
RAM_CODE t_clock LSLCfreq_Get_Last_Frequency_Clock(void);

/*
 * Setup the outgoing frequency for the response to a Rx FHS
 */
void LSLCfreq_Early_Prepare_Frequency_FHS_Response_ID(void);

/*
 * Setup the next TX RX frequencies in response to an Rx ID
 */
RAM_CODE void LSLCfreq_Early_Prepare_TxRxFreqs_SlavePageResponse(void);

/*
 * Prepare the outgoing frequency for the first FHS in master page response
 */
RAM_CODE void LSLCfreq_Prepare_Frequency_FirstFHS_MasterPageResponse(t_clock base_clk);

/*****************************************************************************
 * mLSLCfreq_Prepare_Frequency_Connection
 *
 * Maps to either normal hop or the afh engine functions
 *
 * AFH:         LSLCafh_Get_Frequency_Channel
 * Normal hop:  LSLChop_Get_Frequency_Channel_Connection
 *
 ****************************************************************************/
#if (PRH_BS_CFG_SYS_LMP_AFH_CAPABLE_SUPPORTED==1)
#include "lslc_afh.h"
#define mLSLCfreq_Get_Frequency_Channel_Connection(device_index, clk) \
    LSLCafh_Get_Frequency_Channel(device_index, clk)
#elif (PRH_BS_CFG_SYS_LMP_AFH_CAPABLE_SUPPORTED==0)
#define mLSLCfreq_Get_Frequency_Channel_Connection(device_index, clk) \
    LSLChop_Get_Frequency_Channel_Connection(NULL, clk)
#endif

/*****************************************************************************
 * LSLCfreq_Assign_Frequency_Kernel
 *
 * Assign and setup the either normal hop or the afh kernel functions
 *
 * AFH:         LSLCafh_Assign_AFH_Kernel
 * Normal hop:  LSLChop_Setup_Hop_Kernel
 *
 ****************************************************************************/
void LSLCfreq_Assign_Frequency_Kernel(const t_devicelink *p_device_link);

#endif

