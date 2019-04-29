#ifndef _PARTHUS_BT_TESTER
#define _PARTHUS_BT_TESTER

/******************************************************************************
 * MODULE NAME:		bt_tester.h 
 * PROJECT CODE:	BlueStream
 * DESCRIPTION:		Handles LMP tasks required to emulate a tester
 * MAINTAINER:		Tom Kerwick
 * CREATION DATE:       01 Aug 2000  
 *
 * SOURCE CONTROL: $Id: bt_tester.h,v 1.5 2004/07/07 14:21:01 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *	01-August-2000	RGB		Initial Version
 *
 ******************************************************************************/

#include "lmp_acl_connection.h"
#include "dl_dev.h"

/*
 * Constants
 */
#define BTtmt_NUM_POLL_INTERVALS_BEFORE_CHANGING_TO_DUT_TESTMODE 8
#define BTtmt_LOOPBACK_NUM_PACKETS 500
#define BTtmt_LOOPBACK_TX_PATTERN  TX_10_Pattern

/*
 * States
 */
#define BTtmt_TESTER_IDLE                0
#define BTtmt_TESTER_W2_CHANGE_SETTINGS  1
#define BTtmt_TESTER_ACTIVE              2
/*
 * Local structure to hold all testmode tester parameters
 */
typedef struct s_BTtester
{
    /*
     * Counters
     */
    u_int32 num_packets_sent;
    u_int32 num_valid_payloads_received;
    u_int32 num_invalid_payloads_received;
    u_int32 num_payloads_received;
    u_int32 num_payloads_not_received;
    u_int32 num_packets_to_loopback;
    u_int32 num_bit_errors;
    u_int32 num_bytes_received;
    /*
     * Internal
     */
    u_int32 polls_intervals_before_change;
    u_int32 num_polls_sent;
    t_deviceIndex device_index;         /* Active tester device index       */
    u_int8  state;

} t_BTtmt_Tester;

/*
 * Interface operations
 */
t_error BTtmt_LMP_Send_Test_Activate(u_int16 acl_handle);
t_error BTtmt_LMP_Send_Test_Control(t_p_pdu test_control);
t_error BTtmt_LMP_Accepted(t_lmp_link* p_link, t_p_pdu p_pdu);
t_error BTtmt_LMP_Not_Accepted(t_lmp_link* p_link, u_int16 opcode, t_error reason);

#if (PRH_BS_CFG_SYS_TEST_MODE_TESTER_SUPPORTED==1)
void BTtmt_Initialise(void);
void BTtmt_Verify_Tester_Before_Tx(t_deviceIndex device,
         const u_int8 *p_payload, t_length tx_length, t_LCHmessage msg_type);
RAM_CODE void    BTtmt_Verify_Tester_After_Rx(t_deviceIndex device,
         const u_int8 *p_payload, t_length rx_length, t_LCHmessage msg_type);
boolean BTtmt_Get_Next_Master_Tx_Activity(t_devicelink *p_device_link);
void BTtmt_Record_Rx_Packet_Error(void);
u_int32 BTtmt_Get_PER(void);
u_int32 BTtmt_Get_BER(void);
boolean BTtmt_Is_PER_BER_Available(void);
#else
#define BTtmt_Initialise()
#define BTtmt_Verify_Tester_Before_Tx(device, p_payload, rx_length, msg_type)
#define BTtmt_Verify_Tester_After_Rx(device, p_payload, rx_length, msg_type)
#define BTtmt_Get_Next_Master_Tx_Activity(p_device_link) FALSE
#define BTtmt_Record_Rx_Packet_Error()
#define BTtmt_Get_PER() (0)
#define BTtmt_Get_BER() (0)
#define BTtmt_Is_PER_BER_Available() FALSE
#endif

#endif /*_PARTHUS_BT_TESTER*/
