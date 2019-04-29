#ifndef _PARTHUS_SYS_CONST_
#define _PARTHUS_SYS_CONST_

/******************************************************************************
 * MODULE NAME:    sys_const.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    All System Wide Constants
 * MAINTAINER:     John Nelson
 * DATE:           15 May 1999
 *
 * SOURCE CONTROL: $Id: sys_const.h,v 1.49 2013/05/30 16:07:52 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    15 May 1999 -   jn       - Initial Version V0.9
 *    21 July 1999 -  cm       - Update
 *    25 July 1999 -  gf       - Update
 *
 * NOTES:
 *    Little endian format used, i.e. least significant byte in lowest address    
 ******************************************************************************/

/*
 *      BT Baseband Constants
 */
#define DEFAULT_CHECK_INIT			0              /* Used for HEC/CRC initial.  */
#define GIAC_LAP                0x9E8B33       /* B.13.2.3                   */
#define TWS_LAP                 0X9E8B11

/*
 *      User PDU sizes in User Bytes carried
 *  (excluding payload header, CRC bytes not included)
 */
#define FHS_PACKET_LENGTH        18
#define MAX_EIR_PACKET_LENGTH    240

#define MAX_DM1_USER_PDU         17   
#define MAX_DH1_USER_PDU         27        

#define MAX_DM3_USER_PDU         121     
#define MAX_DH3_USER_PDU         183   

#define MAX_DM5_USER_PDU         224     
#define MAX_DH5_USER_PDU         339

#define MAX_2DH1_USER_PDU        54
#define MAX_3DH1_USER_PDU        83

#define MAX_EDR_AUX1_PDU         29

#define MAX_2DH3_USER_PDU        367
#define MAX_3DH3_USER_PDU        552

#define MAX_2DH5_USER_PDU        679
#define MAX_3DH5_USER_PDU        1021

#define MAX_AUX1_USER_PDU        29      

#define MAX_DVACL_USER_PDU       9 
#define MAX_DVSCO_USER_PDU       10

#define MAX_HV1_USER_PDU         10
#define MAX_HV2_USER_PDU         20
#define MAX_HV3_USER_PDU         30

#define MAX_EV3_USER_PDU         30
#define MAX_EV4_USER_PDU         120
#define MAX_EV5_USER_PDU         180

#define MAX_2EV3_USER_PDU        60
#define MAX_3EV3_USER_PDU        90

#define MAX_2EV5_USER_PDU        360
#define MAX_3EV5_USER_PDU        540

#define MIN_L2CAP_MTU            (48+4)
#define DEFAULT_L2CAP_MTU        (672+4)

#define L2CAP_PICO_BCAST_Q MAX_ACTIVE_DEVICE_LINKS
#define L2CAP_ACTIVE_BCAST_Q     0

/*
 * Bluetooth Baseband Timer Defaults  
 * Appendix VI V1.1 Specification Values (Do Not Change)
 */
#define PAGE_RESP_TIMEOUT_DEFAULT       8   /* pagerespTOvalue              */
#define INQ_RESP_TIMEOUT_DEFAULT        128 /* inqrespTOvalue               */  
#define NEW_CONNECTION_TIMEOUT_DEFAULT  32  /* newconnectionTO              */

/* 
 *V1.1 P21 Transmitter Characteristics - Power Control
 *
 * Power control is required for power class 1 equipment. The power control is
 * used for limiting the transmitted power over 0 dBm. Power control capability
 * under 0 dBm is optional and could be used for optimizing the power consump-tion
 * and overall interference level. The power steps shall form a monotonic
 * sequence, with a maximum step size of 8 dB and a minimum step size of 2 dB.
 * A class 1 equipment with a maximum transmit power of +20 dBm must be able
 * to control its transmit power down to 4 dBm or less.
 * 
 *                                             with min step size 2dB 
 * Class 1 Pmax 20dBm   Pmin  0dBm..~-30dBm    ==> 25 levels
 * Class 2 Pmax  4dBm   Pmin -6dBm..~-30dBm    ==> 17 levels
 * Class 3 Pmax  0dBm   Pmin        ~-30dBm    ==> 15 levels
 */

/*
 * Power level definitions for Shoga and
 * Chimera, Win32, Linux moved to respective hals.
 */
//!!VALUES FOR THESE SHOULD ALWAYS BE DEFINED IN RELEVANT HAL - NOT CORE!!
//#define MAX_POWER_LEVEL_UNITS 15
//#define TX_POWER_LEVEL_Pmax 0
//#define TX_POWER_LEVEL_Pmin -30
//
//#define GOLDEN_RECEIVER_RSSI_MIN -56
//#define GOLDEN_RECEIVER_RSSI_MAX -30

#define MAX_PEER_POWER_LEVEL_UNITS 3      /* 15 */


/*
 * The testmode LMP_test_control parameters are whitened post V1.0
 */
#define TESTMODE_WHITEN 0x55

/*
 * Map of packet types to lengths (currently defined in sys_config.c),
 *     while consider requirement for  new  module  sys_const.c
 */
extern const  u_int16 SYSconst_Packet_Length[32];
extern const  u_int16 SYSconst_HCI_Packet_Type[32];

/*
 * Use accessor macros to allow array to be extended as required.
 */
#define SYSconst_Get_Packet_Length(pkt) ( SYSconst_Packet_Length[(pkt)] )
#define SYSconst_ACL_SEGMENT_LENGTH_ALIGNMENT_MASK \
    (~(PRH_BS_CFG_SYS_ACL_SEGMENT_LENGTH_ALIGNMENT-1))
#define SYSconst_Get_Packet_Segmented_Tx_Length(pkt) \
 ( SYSconst_Packet_Length[(pkt)] & SYSconst_ACL_SEGMENT_LENGTH_ALIGNMENT_MASK)

#define SYSconst_HCI_Packet_Type(pkt) ( SYSconst_HCI_Packet_Type[(pkt)] )

/*
 * Adding HW Error Events
 */
#define PRH_BS_HW_ERROR_NONE                        0x00
#define PRH_BS_HW_ERROR_HCIT_UART_OVERRUN           0x01
#define PRH_BS_HW_ERROR_HCIT_RX_PDU_MALFORMED       0x02
#define PRH_BS_HW_ERROR_INVALID_DEVICE_LINK         0x03
#define PRH_BS_HW_ERROR_MSS_FAILURE                 0x04
#define PRH_BS_HW_ERROR_DEBUG_CLKN_READ             0x05
#define PRH_BS_HW_ERROR_STACK_CORRUPTION            0x06
#define PRH_BS_HW_ERROR_HCIT_UART_FRAMING           0x07
#define PRH_BS_HW_ERROR_INVALID_LMP_LINK            0x08
#define PRH_BS_HW_ERROR_INVALID_PICONET_INDEX       0x09
#define PRH_BS_HW_ERROR_INCORRECT_RADIO_VERSION     0x0A
#define PRH_BS_HW_ERROR_SCHEDULER_CONFIGURATION     0x10
#define PRH_BS_HW_ERROR_INCORRECT_TABASCO_VERSION   0x11

#define PRH_BS_HW_ERROR_INCORRECT_CTRLSTATE         0x20
#define PRH_BS_HW_ERROR_OUT_OF_LMP_TIMERS           0x21
#define PRH_BS_HW_ERROR_INCORRECT_SUPER_STATE       0x22
#define PRH_BS_HW_ERROR_CORRUPTION_OF_LMP_TIMERS    0x23
#define PRH_BS_HW_ERROR_CORRUPTION_OF_QUEUES        0x24
#define PRH_BS_HW_ERROR_OUT_OF_LMP_QUEUE_BUFFERS    0x25
#define PRH_BS_HW_ERROR_LMP_QUEUE_CORRUPTED         0x26
#define PRH_BS_HW_ERROR_OUT_OF_LE_LLC_QUEUE_BUFFERS 0x27
#define PRH_BS_HW_ERROR_LE_LLC_QUEUE_CORRUPTED      0x28

/*
 *  Tabasco hardware revision numbers
 */
#define PRH_BS_HW_TABASCO_VERSION_T1_0_0        4
#define PRH_BS_HW_TABASCO_VERSION_T1_0_1        5
#define PRH_BS_HW_TABASCO_VERSION_T2_0_0        6
#define PRH_BS_HW_TABASCO_VERSION_T3_0_0        7

#define PRH_BS_HW_TABASCO_VERISON_DM_T1_1_1     20 /* initial DM verison */
#define PRH_BS_HW_TABASCO_VERSION_DM_T1_1_3     21 /* added window widening */
#define PRH_BS_HW_TABASCO_VERSION_DM_T2_0_0     22 /* ????? BT4.1_HW */

/*-------------DualMode use only---------------*/
#define GAP_PROTOCOL  0x10//merge CEVA 0517
#define SMP_PROTOCOL  0x11//merge CEVA 0517
#define HCI_PROTOCOL  0x00//merge CEVA 0517 
/*-------------DualMode use only---------------*/
#endif   

