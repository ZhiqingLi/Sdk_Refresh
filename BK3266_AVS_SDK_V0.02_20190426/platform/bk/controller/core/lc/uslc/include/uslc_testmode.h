#ifndef _PARTHUS_USLC_TESTMODE_
#define _PARTHUS_USLC_TESTMODE_

/*************************************************************************
 * MODULE NAME:    uslc_testmode.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Bluetooth Test Mode: Baseband Test Procedures
 * DATE:           24 Sept 1999
 *
 * SOURCE CONTROL?: $Id: uslc_testmode.h,v 1.29 2004/07/07 14:22:00 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    24 Sept 1999 -   jn       - Initial Version V1.0
 *    22 Oct  1999 -   jn       - 1st Release
 *
 *************************************************************************/

#include "sys_config.h"
#include "sys_types.h"
#include "dl_dev.h"


#define PT_TEST_MASK	0x1F /* RGB assumes no more than 32 tests */
/* RGB can combine these to do both */
#define PT_TX_EVERY_POLL_PERIOD_OFFSET	0x40
#define PT_TEST_OFFSET  0x80 /*128*/
typedef enum
{
    TX_Off              = 0, 
    TX_0_Pattern        = 1, 
    TX_1_Pattern        = 2, 
    TX_10_Pattern       = 3,
    TX_Psuedo_Random    = 4, 
    LB_ACL_Packets_Whitening_On  = 5,
    LB_SCO_Packets_Whitening_On  = 6,
    LB_ACL_Packets_Whitening_Off = 7, 
    LB_SCO_Packets_Whitening_Off = 8, 

    TX_11110000_Pattern = 9,

    /*
     * Parthus extensions 
    */
	PT_TX_EVERY_POLL_PERIOD = PT_TX_EVERY_POLL_PERIOD_OFFSET,

	PT_TX_CYCLE_ALL_LENGTHS = PT_TEST_OFFSET /*+ TX_11110000_Pattern*/,

    Test_Mode_Exit      = 255

} t_testScenario; 

/*
 *  Types
 *
 *  Frames (1.25ms) defined in Test Mode Transmit Parameters for Poll
 */
typedef t_slots t_TDDframes;

/*
 * Test mode Structures
 */
/*
 * s_tm_control
 *
 * Structure to hold all testmode related variables
 */
typedef struct s_tm_control 
{
    u_int8         *p_data_buffer;
    t_testScenario testScenario;

    t_rfSelection  rfHoppingMode;
    t_freq         txDutFrequency;
    t_freq         rxDutFrequency;
    u_int8         powerControlMode;
    t_TDDframes    pollPeriod;
    t_packet       packetType;
    t_length       packetLength;

    t_state        packetContext;

    /*
     * Byte fields used over bit fields for performance
     */
    u_int8         transmitter_test;
    u_int8         loopback_test;

#ifdef USLC_TEST_MODE_EXTENSIONS
    /*
     * Testmode extensions
     */
    u_int8         cycle_length_test;
    u_int8         transmitter_test_all_packets;
    t_length       next_cycle_length;
    u_int8         send_packet_every_poll_period;
    t_slots        tx_Next_Packet_Timer;
#endif

    /* 
     * Device settings prior to test setup - restored when test mode exited 
     */
    u_int8         original_data_whitening_state;
    t_rfSelection  original_hopping_mode;
    t_freq         original_tx_freq;
    t_freq         original_rx_freq;

} t_tm_control;

#if (PRH_BS_CFG_SYS_TEST_MODE_SUPPORTED==1)
/*
 * Interface to Users
 */
t_error USLCtm_Testmode_Request(t_deviceIndex deviceIndex, 
                               t_p_pdu p_LMP_test_control_PDU);
t_error USLCtm_Store_Test_Parameters(t_p_pdu p_LMP_test_control_PDU);
void USLCtm_Testmode_Exit(void);
void USLCtm_Testmode_Change_Settings(t_deviceIndex device_index);

/*
 * Interface to Baseband Control
 */
void USLCtm_Initialise(void);
void USLCtm_Reinitialise(void);
void USLCtm_prepareTx(t_devicelink*);

#else
/*
 * Empty functions
 */
#define USLCtm_Initialise()     
#define USLCtm_Reinitialise()     
#define USLCtm_Testmode_Request(deviceIndex, p_LMP_test_control_PDU) UNSUPPORTED_FEATURE
#define USLCtm_Store_Test_Parameters(p_LMP_test_control_PDU) UNSUPPORTED_FEATURE
#define USLCtm_Testmode_Exit()

#endif
/*
 * Entry points for testing
 */
void USLCtm_Test_Transmitter(t_devicelink *pDL);
void USLCtm_Build_Test_Packet_Payload(t_testScenario testScenario, u_int8 *txBuf, t_length packetLength);

#endif
