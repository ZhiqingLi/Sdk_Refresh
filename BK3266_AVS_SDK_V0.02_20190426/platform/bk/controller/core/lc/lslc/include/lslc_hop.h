#ifndef _PARTHUS_LSLC_HOP_
#define _PARTHUS_LSLC_HOP_

/******************************************************************************
 * MODULE NAME:    lslc_hop.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    LSLC Hop frequency generator
 * MAINTAINER:     John Nelson
 * DATE:           5 April 1999
 *
 * SOURCE CONTROL: $Id: lslc_hop.h,v 1.24 2004/07/07 14:21:47 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V0.1     20 Apr 1999 -   jn       - Risk resolution V0.8
 *    V1.0     27 May 1999 -   jn       - Initial Version V0.9
 *
 * NOTES TO USERS:
 *       Information is shared over states and hence a single Hop object
 *       must be used across the states.
 *                   Page/MasterResponse,
 *                   PageScan/SlaveResponse,
 *                   InquiryScan/Inquiry Response
 *       The PageMasterResponse must use the previous train of Page.
 *       Carefully switch states!
 ******************************************************************************/

#define LSLChop_INVALID_FREQUENCY   93

/*
 * t_hse_type     to select Hardware/Software HSE
 *                      u_int8 used for optimum packing
 */
enum
{
    LSLChop_HSE_HARDWARE=0,
    LSLChop_HSE_SOFTWARE=1
}; 
typedef u_int8 t_hse_type;
/*
 * Notes on Engines and Kernels
 *
 * The Kernel data structure is used to represent the A-F based on uap_lap
 * The Engine data structure represents all internal parameters for 
 * the hop selection calculation.
 */

/*
 * t_hs_kernel
 * Data structure to represent the hop kernel configuration
 * Allows faster connection software hop calculations
 */
typedef struct s_hop_selection_kernel
{
   t_uap_lap     uap_lap;               /* Initialised Kernel's uap_lap  */ 
   u_int8        Ai;                    /* A extracted from uap_lap only */
   u_int8        Bi;                    /* B extracted from uap_lap only */
   u_int8        Ci;                    /* C extracted from uap_lap only */
   u_int8        EFi;                   /* E extracted from uap_lap only */
   u_int16       Di;                    /* D extracted from uap_lap only */

   u_int32       sum_before_mod;        /* Current sum before mod       */
} t_hs_kernel;

/*
 * t_hs_engine
 *
 * Data structure to store the current state of the hop selection engine
 * for all states.  A single instance will exist in the lslc_hop.c module.
 */
typedef struct s_hop_selection_engine
{
   t_clock       frozen_clk_page_inq;   /* Frozen clock: Page/Inq Resp  */

   u_int8        frozen_X_inquiry_scan; /* Implicitly set here          */
   u_int8        phase_N_inquiry_scan;  /* Implicitly set here          */
   u_int8        frozen_X_page_scan;    /* Implicitly set here          */
   u_int8        frozen_X_page;         /* Explicitly set by USLC_Page  */
   u_int8        X;                     /* Current X:                   */

   t_train       train;
   t_rfMode      rfMode;
   t_rfSelection rfSelection;
   t_hse_type    hs_engine_type; 
   t_freq        txMasterHopFixedFreq, rxMasterHopFixedFreq;

   t_freq        (*hs_connection_fn)(t_hs_kernel *p_hs_kernel,  t_clock clk);

} t_hs_engine;


/*
 * LSLChop external methods
 */
void    LSLChop_Initialise(t_rfSelection selection,
		       t_freq txMasterFixedFreq, t_freq rxMasterFixedFreq );

void    LSLChop_Setup_Hop_Kernel(t_uap_lap uap_lap);
DRAM_CODE t_freq  LSLChop_Get_Next_Frequency(t_state state, t_clock clk);
DRAM_CODE t_freq  LSLChop_Get_Frequency_Channel_Connection(
    t_hs_kernel *p_hs_kernel, t_clock clk);

void    LSLChop_Build_Frequency_Table(t_state state, t_uap_lap uap_lap);
t_freq  LSLChop_Get_Table_Frequency_Page_FHS_Response_ID(void);
void    LSLChop_Get_TxRxFreqs_SlavePageResponse( t_freq *p_tx_chan, t_freq *p_rx_chan);
t_freq  LSLChop_Get_Frequency_Interlaced_Second_Scan(t_state context);
t_freq  LSLChop_Get_Frequency_FirstFHS_MasterPageResponse(t_clock base_clk);

void    LSLChop_Set_Train(t_train AorB);
t_train LSLChop_Get_Train(void);
void    LSLChop_Toggle_Train(void);
DRAM_CODE u_int   LSLChop_Get_X(void);
void    LSLChop_Freeze_X_for_Page(t_clock clk);
DRAM_CODE void    LSLChop_Set_X_for_Inquiry(t_clock clk);
void    LSLChop_Increment_X_Page_Scan(void);
void    LSLChop_Increment_N_Inquiry_Scan(void);

DRAM_CODE t_rfSelection LSLChop_Get_rfSelection(void);
void LSLChop_Set_HSE_Engine(t_hse_type hse_type);
DRAM_CODE t_hse_type LSLChop_Get_HSE_Engine(void);

/*
 * Define default as hardware HSE if supported, else software HSE
 */
#if (PRH_BS_CFG_SYS_HW_HOP_SELECTION_ENGINE_SUPPORTED==1)
#define LSLChop_HSE_DEFAULT    LSLChop_HSE_HARDWARE
#else
#define LSLChop_HSE_DEFAULT    LSLChop_HSE_SOFTWARE
#endif

/*
 * AFH related functions
 */
void LSLChop_Build_Hop_Kernel(t_hs_kernel *p_hs_kernel, t_uap_lap uap_lap);

DRAM_CODE u_int32 LSLChop_Execute_79_Hop_Kernel_Connection_Summation(
    t_clock clk, t_hs_kernel *p_hs_kernel);
u_int8 LSLChop_Get_AFH_k_prime(t_clock clk, t_hs_kernel *pKernel);

#endif
