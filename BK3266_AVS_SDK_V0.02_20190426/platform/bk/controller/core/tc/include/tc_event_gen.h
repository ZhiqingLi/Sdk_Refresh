#ifndef _PARTHUS_TC_HCI_EVENTS_
#define _PARTHUS_TC_HCI_EVENTS_

/**********************************************************************
 *
 * MODULE NAME:    tc_event_gen.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Generates TCI Events
 * MAINTAINER:     Daire McNamara <Daire.McNamara@sslinc.com>
 * CREATION DATE:  13 April 2000
 *
 * SOURCE CONTROL: $Id: tc_event_gen.h,v 1.27 2012/09/28 11:56:07 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 **********************************************************************/

#include "lmp_acl_connection.h"
#include "lmp_config.h"
#include "lmp_const.h"
#include "sys_types.h"
#include "tc_types.h"
#include "bt_pump.h"
#include "config.h"

#ifdef BT_DUALMODE

#include "le_connection.h"
#endif

void TCeg_Command_Status_Event(t_error status, u_int16 opcode);
void TCeg_Command_Complete_Event(u_int16 opcode, t_error status);
void TCeg_Read_Pump_Monitors_Event(t_BTpump_Log* log);

/*
 * TCI Event subcodes
 */
#define TCI_LMP_TX_EVENT                0x22
#define TCI_LMP_RX_EVENT                0x23
#define TCI_LC_TX_EVENT                 0x24
#define TCI_LC_RX_EVENT                 0x25
#define TCI_BB_TX_EVENT                 0x26
#define TCI_BB_RX_EVENT                 0x27
#define TCI_HW_ERROR_EVENT              0x28
#define TCI_RADIO_EVENT                 0x30
#define TCI_INTERRUPT_EVENT             0x40


void TCeg_Send_LE_GATT_Event(u_int8* le_gatt_data,u_int8 le_gatt_length, u_int8 tx0_rx1);
void TCeg_Send_LE_LLC_Event(const u_int8* le_llc_data, u_int8 le_llc_length, u_int8 tx0_rx1);

#if (PRH_BS_CFG_SYS_TRACE_LE_DEVICE_STATE_VIA_HCI_SUPPORTED == 1)
void TCeg_Send_LE_LEDeviceState_Event(u_int8 state, t_LE_Connection* p_connection );
#endif

#if (PRH_BS_CFG_SYS_TRACE_LLDATA_VIA_HCI_SUPPORTED==1)
void TCeg_Send_LE_LLData_Event(u_int16 header, const u_int8* le_ll_data, u_int8 le_lldata_length, u_int8 channel,u_int8 tx,u_int8 link_id);
#endif
/*
 * LMP Logger, used by lmp_cmd_disp.c/lmp_event_gen.c if activated
 */
#if (PRH_BS_CFG_SYS_TRACE_LMP_VIA_HCI_SUPPORTED==1)
void TCeg_Send_LMP_Event(const u_int8* lmp_data, u_int8 lmp_length, u_int8 tx0_rx1);
#else
#define TCeg_Send_LMP_Event(lmp_data, lmp_length, tx0_rx1)
#endif

/*
 * LC Logger, used by lc/uslc if activated to log LC events
 */
#if (PRH_BS_CFG_SYS_TRACE_LC_VIA_HCI_SUPPORTED==1)
void TCeg_Send_LC_Event(t_ulc_procedure lc_event, t_state state, 
                        u_int32 lc_arg, u_int8 tx0_rx1);
#else
#define TCeg_Send_LC_Event(lc_event, state, lc_arg, tx0_rx1)
#endif

/*
 * BB Logger, used by lslc_slot.c if activated
 */
#if (PRH_BS_CFG_SYS_TRACE_BB_VIA_HCI_SUPPORTED==1)
void TCeg_Send_BB_Event(u_int8 tx0_rx1);
#else
#define TCeg_Send_BB_Event(tx0_rx1)
#endif

/*
 * Radio Event Logger, used by hal variants of hw_radio.c
 */ 
#if (PRH_BS_CFG_SYS_TRACE_RADIO_VIA_HCI_SUPPORTED==1)
/*
 * TCI Radio Event ids
 */
typedef enum {
    TCeg_RADIO_INITIALISE,
    TCeg_RADIO_RESET,
    TCeg_RADIO_SET_SYNCWORD,
    TCeg_RADIO_PUT_SLAVE_IN_FULL_RX,
    TCeg_RADIO_PUT_DEVICE_IN_STANDBY,
    TCeg_RADIO_PUT_DEVICE_IN_TX_RX,
    TCeg_RADIO_PUT_MASTER_IN_DOUBLE_WIN,
    TCeg_RADIO_PUT_SLAVE_IN_DOUBLE_WIN,
    TCeg_RADIO_PUT_SLAVE_IN_SINGLE_TX,
    TCeg_RADIO_FHS_RX_OVERRIDE,
    TCeg_RADIO_CANCEL_FHS_RX_OVERRIDE,
    TCeg_RADIO_DISABLE_TRANSMITTER,
    TCeg_RADIO_ENABLE_TRANSMITTER,
    TCeg_RADIO_HANDLE_MULTISLOT_PKT,
    TCeg_RADIO_SET_RADIO_FREQUENCY,
    TCeg_RADIO_WRITE_SERIAL_WORD,
    TCeg_RADIO_WRITE_SERIAL_WORD_NOW
} t_tci_eg_radio_state;

void TCeg_Send_Radio_Event(t_tci_eg_radio_state radio_state, u_int32 radio_arg);

#elif (PRH_BS_CFG_SYS_TRACE_RADIO_VIA_HCI_SUPPORTED==0)
/*
 * No radio logging (impactless)
 */
#define TCeg_Send_Radio_Event(radio_state, radio_arg)
#endif

/*
 * Interrupt Logger 
 */
#if (PRH_BS_CFG_SYS_TRACE_INTR_VIA_HCI_SUPPORTED==1)
/*
 * TCI Radio Event ids
 */
typedef enum {
    TCeg_INTR_TIM0,
    TCeg_INTR_TIM1,
    TCeg_INTR_TIM2,
    TCeg_INTR_TIM3,
    TCeg_INTR_PKA,
    TCeg_INTR_PKD,
    TCeg_INTR_NO_PKT_RCVD,
    TCeg_INTR_PKD_RX_HDR,
    TCeg_INTR_AUX_TMR,
    TCeg_INTR_SYNC_DET,
    TCeg_EVENT_RETURN_TO_OWN_PICONET
} t_tci_eg_interrupt;
void TCeg_Send_Interrupt_Event(t_tci_eg_interrupt interrupt_event, u_int8 interrupt_state);
#else
#define TCeg_Send_Interrupt_Event(interrupt_event, interrupt_state);
#endif

void TCeg_Command_Complete_Event(u_int16 opcode, t_error status);

#if (PRH_BS_CFG_SYS_TRACE_ADV_VIA_HCI_SUPPORTED==1)
void TCeg_Send_LE_Adv_Event(u_int16 header, const u_int8* le_adv_data, u_int8 le_adv_length, u_int8 channel,u_int8 tx);
#endif
#if (PRH_BS_CFG_SYS_TRACE_ATT_VIA_HCI_SUPPORTED==1)
void TCeg_Send_LE_GATT_Event(u_int8* le_gatt_data,u_int8 le_gatt_length, u_int8 tx0_rx1);
#endif

#if (PRH_BS_CFG_SYS_TRACE_SMP_VIA_HCI_SUPPORTED==1)
void TCeg_Send_LE_SMP_Event(u_int8* le_smp_data,u_int8 le_smp_length, u_int8 tx0_rx1);
#endif
#endif

