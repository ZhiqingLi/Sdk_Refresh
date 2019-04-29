#ifndef _PARTHUS_HW_HABANERO_
#define _PARTHUS_HW_HABANERO_

/******************************************************************************
 * MODULE NAME:    hw_habanero.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Habanero API
 * MAINTAINER:     Ivan Griffin
 * DATE:           20 March 2000
 *
 * SOURCE CONTROL: $Id: hw_habanero.h,v 1.16 2013/04/08 11:02:46 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    20 March 2000          IG           Initial Version
 *
 * NOTES TO USERS:
 ******************************************************************************/
#include "sys_config.h"
#include "sys_types.h"
#include "hw_hab_defs.h"                         /* All Habanero definitions */
#include "lslc_access.h" /* for t_frame_pos */

/***************************************************************************
 *
 * HWradio Interface Functions - these are declared in hw_radio.h
 * but defined in hw_habanero.c
 *
 * void HWradio_Initialise(void);
 * void HWradio_Reset(void);
 * void HWradio_Set_Syncword(t_syncword const);
 * void HWradio_Set_Rx_Mode(t_HWradio_Rx_Mode const rx_mode);
 *
 * void HWradio_Put_Slave_In_Full_Rx(void);
 * void HWradio_Put_Master_In_Double_Win(void);
 * void HWradio_Put_Slave_In_Double_Win(void);
 * void HWradio_Put_Device_In_TxRx(void);
 * void HWradio_Put_Device_In_Standby(void);
 * void HWradio_Handle_Multislot_Pkt(t_packet const pkt_type,
        t_frame_pos const frame_pos); 
 * void HWradio_Disable_Transmitter(void);
 * void HWradio_Enable_Transmitter(void);
 *
 ***************************************************************************/

/***************************************************************************
 *
 * HABANERO Specific Functions
 *
 ***************************************************************************/
#define HWhab_Set_Initial_Slicer_Level HWhab_Set_OCL_DC_Offset

/*
 * Receive Control Registers
 */
__INLINE__ void HWhab_Set_Syncword_Ref(const u_int32* syncword);
__INLINE__ void HWhab_Set_Sync_Error(const u_int8 sync_error);
__INLINE__ void HWhab_Set_Rx_Mode(const t_HWradio_Rx_Mode rx_mode);
__INLINE__ void HWhab_Set_DADC_Bypass(const boolean state);
__INLINE__ void HWhab_Set_RADC_Bypass(const boolean state);
__INLINE__ void HWhab_Set_RADC_Num(const boolean state);
__INLINE__ void HWhab_Set_FDB_DIS(const boolean state);
__INLINE__ void HWhab_Set_Sym_Gn(const u_int8 gain);
__INLINE__ void HWhab_Set_Sym_Enb(const boolean state);
__INLINE__ void HWhab_Set_Sym_Mask(const boolean state);
__INLINE__ void HWhab_Set_Cmb_C1(const u_int8 c1);
__INLINE__ void HWhab_Set_Cmb_C2(const u_int8 c2);
__INLINE__ void HWhab_Set_OCL_DC_Offset(const u_int8 offset);
__INLINE__ void HWhab_Set_OCL_Feedback_Mode(const boolean state);
__INLINE__ void HWhab_Set_OCL_Low_Gain(const u_int8 low_gain);
__INLINE__ void HWhab_Set_OCL_High_Gain(const u_int8 high_gain);

__INLINE__ void HWhab_Get_Syncword_Ex(u_int32 *);
__INLINE__ u_int8 HWhab_Get_Sync_Error(void);
__INLINE__ t_HWradio_Rx_Mode HWhab_Get_Rx_Mode(void);
__INLINE__ boolean HWhab_Get_DADC_Bypass(void);
__INLINE__ boolean HWhab_Get_RADC_Bypass(void);
__INLINE__ boolean HWhab_Get_RADC_Num(void);
__INLINE__ boolean HWhab_Get_FDB_DIS(void);
__INLINE__ u_int8 HWhab_Get_Sym_Gn(void);
__INLINE__ boolean HWhab_Get_Sym_Enb(void);
__INLINE__ boolean HWhab_Get_Sym_Mask(void);
__INLINE__ u_int8 HWhab_Get_Cmb_C1(void);
__INLINE__ u_int8 HWhab_Get_Cmb_C2(void);
__INLINE__ u_int8 HWhab_Get_OCL_DC_Offset(void);
__INLINE__ boolean HWhab_Get_OCL_Feedback_Mode(void);
__INLINE__ u_int8 HWhab_Get_OCL_Low_Gain(void);
__INLINE__ u_int8 HWhab_Get_OCL_High_Gain(void);

/*
 * Transmit Control Registers
 */
__INLINE__ void HWhab_Set_Gaussian_Coeff(const u_int8 index, const u_int8 value);
__INLINE__ void HWhab_Set_DAC_Bypass(const boolean state);
__INLINE__ void HWhab_Set_DAC_Ref(const u_int8 voltage_ref);
__INLINE__ void HWhab_Set_Gaussian_Offset(const u_int16 value); /* 9 bit value */

__INLINE__ u_int8 HWhab_Get_Gaussian_Coeff(const u_int8 index);
__INLINE__ boolean HWhab_Get_DAC_Bypass(void);
__INLINE__ u_int8 HWhab_Get_DAC_Ref(void);
__INLINE__ u_int16 HWhab_Get_Gaussian_Offset(void);

/*
 * Interrupt Control Registers
 */
__INLINE__ void HWhab_Set_RIF_Intr_Mask0(const boolean state);
__INLINE__ void HWhab_Set_RIF_Intr_Mask1(const boolean state);
__INLINE__ void HWhab_Set_RIF_Intr_Mask2(const boolean state);
__INLINE__ void HWhab_Set_RIF_Intr_Mask3(const boolean state);
__INLINE__ void HWhab_Set_LTR_Intr_Mask(const boolean state);

__INLINE__ void HWhab_Set_RIF_Intr_Clr(const u_int8 clear_value);
__INLINE__ void HWhab_Set_LTR_Intr_Clr(const u_int8 value);

__INLINE__ boolean HWhab_Get_RIF_Intr_Mask0(void);
__INLINE__ boolean HWhab_Get_RIF_Intr_Mask1(void);
__INLINE__ boolean HWhab_Get_RIF_Intr_Mask2(void);
__INLINE__ boolean HWhab_Get_RIF_Intr_Mask3(void);
__INLINE__ boolean HWhab_Get_LTR_Intr_Mask(void);

__INLINE__ u_int8 HWhab_Get_RIF_Intr_Clr(void);
__INLINE__ u_int8 HWhab_Get_LTR_Intr_Clr(void);

/*
 * Interface Configuration Registers
 */
__INLINE__ void HWhab_Set_PHY_Cfg(const u_int32 magic_phy_value);

__INLINE__ u_int32 HWhab_Get_PHY_Cfg(void);

/*
 * Status Registers
 */
__INLINE__ u_int8 HWhab_Get_RADC_RSSI(void);       /* RSSI indication for power control */
__INLINE__ u_int8 HWhab_Get_LTR_Long_Period(void); /* Trimming Comparison Indicator */
__INLINE__ u_int8 HWhab_Get_LTR_Intr(void);        /* LTR generated LPO Period Interrupt */
__INLINE__ u_int8 HWhab_Get_RIF_Intr(void);        /* RF Status Line Interrupt */
__INLINE__ u_int8 HWhab_Get_GIO_Status(void);
__INLINE__ u_int8 HWhab_Get_Core_Minor_Rev(void);  /* Minor Revision Number of Habanero */
__INLINE__ u_int8 HWhab_Get_Core_Major_Rev(void);  /* Major Revision Number of Habanero */

/*
 * GIO Control Registers
 */
__INLINE__ u_int8 HWhab_Set_GIO_Ctrl(const u_int8 index, const u_int8 value);

/*
 * Reset Control Registers
 */
__INLINE__ void HWhab_Set_Rst_Code(const u_int8 code);

__INLINE__ u_int8 HWhab_Get_Rst_Code(void);

#if (PRH_BS_CFG_SYS_HW_WINDOW_WIDENING_SUPPORTED==1)
/*
 * WIN_EXT GIO Early/Late Control Registers
 */
__INLINE__ void HWhab_Set_GIOs_Hi_Early(const u_int16 mask);
__INLINE__ void HWhab_Set_GIOs_Hi_Late(const u_int16 mask);
__INLINE__ void HWhab_Set_GIOs_Lo_Early(const u_int16 mask);
__INLINE__ void HWhab_Set_GIOs_Lo_Late(const u_int16 mask);
#endif

#ifdef __USE_INLINES__
#include "hw_habanero_impl.h"
#endif

#endif
