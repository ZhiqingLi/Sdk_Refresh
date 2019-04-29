#ifndef _PARTHUS_LMP_CONST_
#define _PARTHUS_LMP_CONST_

#include "sys_types.h"
#include "sys_const.h"
#include "config.h"

/**************************************************************************
 * MODULE NAME:    lmp_const.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Constants for LMP Objects, Message Opcodes, PDU lengths etc..
 * MAINTAINER:     Tom Kerwick
 * DATE:           23 July 99
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     xx xxx 1999 -   gf       - Initial Version 
 *
 * SOURCE CONTROL:  $Id: lmp_const.h,v 1.125 2014/07/02 15:39:50 garyf Exp $ 
 *    
 **************************************************************************/

/*********************************************************************
 * LMP MESSAGE OPCODES
 * -------------------
 *  LMP Message Opcodes as per Ver 1.1 of Bluetooth SIG Specification
 ********************************************************************/

#define NO_OPERATION                                     0
#define LMP_NAME_REQ                                     1
#define LMP_NAME_RES                                     2
#define LMP_ACCEPTED                                     3
#define LMP_NOT_ACCEPTED                                 4
#define LMP_CLKOFFSET_REQ                                5
#define LMP_CLKOFFSET_RES                                6
#define LMP_DETACH                                       7
#define LMP_IN_RAND                                      8
#define LMP_COMB_KEY                                     9
#define LMP_UNIT_KEY                                    10
#define LMP_AU_RAND                                     11
#define LMP_SRES                                        12
#define LMP_TEMP_RAND                                   13
#define LMP_TEMP_KEY                                    14
#define LMP_ENCRYPTION_MODE_REQ                         15
#define LMP_ENCRYPTION_KEY_SIZE_REQ                     16
#define LMP_START_ENCRYPTION_REQ                        17
#define LMP_STOP_ENCRYPTION_REQ                         18
#define LMP_SWITCH_REQ                                  19
#define LMP_HOLD                                        20
#define LMP_HOLD_REQ                                    21
#define LMP_SNIFF                                       22  /* Removed from Spec */
#define LMP_SNIFF_REQ                                   23
#define LMP_UNSNIFF_REQ                                 24
#define LMP_PARK_REQ                                    25
#define LMP_SET_BROADCAST_SCAN_WINDOW                   27
#define LMP_MODIFY_BEACON                               28
#define LMP_UNPARK_BD_ADDR_REQ                          29
#define LMP_UNPARK_PM_ADDR_REQ                          30
#define LMP_INCR_POWER_REQ                              31
#define LMP_DECR_POWER_REQ                              32
#define LMP_MAX_POWER                                   33
#define LMP_MIN_POWER                                   34
#define LMP_AUTO_RATE                                   35
#define LMP_PREFERRED_RATE                              36
#define LMP_VERSION_REQ                                 37
#define LMP_VERSION_RES                                 38
#define LMP_FEATURES_REQ                                39
#define LMP_FEATURES_RES                                40
#define LMP_QUALITY_OF_SERVICE                          41
#define LMP_QUALITY_OF_SERVICE_REQ                      42
#define LMP_SCO_LINK_REQ                                43
#define LMP_REMOVE_SCO_LINK_REQ                         44
#define LMP_MAX_SLOT                                    45
#define LMP_MAX_SLOT_REQ                                46
#define LMP_TIMING_ACCURACY_REQ                         47
#define LMP_TIMING_ACCURACY_RES                         48
#define LMP_SETUP_COMPLETE                              49
#define LMP_USE_SEMI_PERMANENT_KEY                      50
#define LMP_HOST_CONNECTION_REQ                         51
#define LMP_SLOT_OFFSET                                 52
#define LMP_PAGE_MODE_REQ                               53
#define LMP_PAGE_SCAN_MODE_REQ                          54
#define LMP_SUPERVISION_TIMEOUT                         55
#define LMP_TEST_ACTIVATE                               56
#define LMP_TEST_CONTROL                                57
#define LMP_ENCRYPTION_KEY_SIZE_MASK_REQ                58
#define LMP_ENCRYPTION_KEY_SIZE_MASK_RES                59
#define LMP_SET_AFH                                     60
#define LMP_ENCAPSULATED_HEADER                         61
#define LMP_ENCAPSULATED_PAYLOAD                        62
#define LMP_SIMPLE_PAIRING_CONFIRM                      63
#define LMP_SIMPLE_PAIRING_NUMBER                       64
#define LMP_DHKEY_CHECK                                 65
#define LMP_ESCAPE_1_OPCODE                             124
#define LMP_ESCAPE_2_OPCODE                             125
#define LMP_ESCAPE_3_OPCODE                             126
#define LMP_ESCAPE_4_OPCODE                             127

/*
 * Extension 1 (124) opcodes
 */
/* None */

/*
 * Extension 2 (125) opcodes
 */
/* None */

/*
 * Extension 3 (126) opcodes
 */
/* None */

/*
 * Extension 4 (127) opcodes
 */
/*
 * The ENCODE_ESC_OPCODE allows for the opcodes to be extended to
 * include the escape opcode if necessary
 */
#define ENCODE_ESC_OPCODE(escape_opcode, extended_op_code)  \
    ((escape_opcode<<8)+extended_op_code)
#define LMP_ACCEPTED_EXT                ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 1)
#define LMP_NOT_ACCEPTED_EXT            ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 2)
#define LMP_FEATURES_REQ_EXT            ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 3)
#define LMP_FEATURES_RES_EXT            ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 4)
#define LMP_SCATTER_REQ                 ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 5)
#define LMP_UNSCATTER_REQ               ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 6)
#define LMP_SET_SUBRATE                 ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 7)
#define LMP_SCATTER_ALGORITHMS_REQ      ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 8)
#define LMP_SCATTER_ALGORITHMS_RES      ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE, 9)
#define LMP_PP_EXTENSION_REQ            ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,10)
#define LMP_PACKET_TYPE_TABLE_REQ       ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,11)
#define LMP_ESCO_LINK_REQ               ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,12)
#define LMP_REMOVE_ESCO_LINK_REQ        ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,13)
#define LMP_CHANNEL_CLASSIFICATION_REQ  ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,16)
#define LMP_CHANNEL_CLASSIFICATION      ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,17)
#define LMP_ALIAS_ADDRESS               ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,18)
#define LMP_ACTIVE_ADDRESS              ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,19)
#define LMP_FIXED_ADDRESS               ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,20)
#define LMP_SNIFF_SUBRATING_REQ         ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,21)
#define LMP_SNIFF_SUBRATING_RES         ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,22)
#define LMP_PAUSE_ENCRYPTION_REQ        ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,23)
#define LMP_RESUME_ENCRYPTION_REQ       ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,24)
#define LMP_IO_CAPABILITY_REQUEST       ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,25)
#define LMP_IO_CAPABILITY_RESPONSE      ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,26)
#define LMP_NUMERIC_COMPARISON_FAILED   ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,27)
#define LMP_PASSKEY_ENTRY_FAILED        ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,28)
#define LMP_OOB_FAILED                  ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,29)
#define LMP_KEYPRESS_NOTIFICATION       ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,30) /* tbd */
#define LMP_POWER_CONTROL_REQ           ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,31) 
#define LMP_POWER_CONTROL_RESP          ENCODE_ESC_OPCODE(LMP_ESCAPE_4_OPCODE,32) 
#if 1
/********************************************************
 * Default Timer Values, as for specification
 ********************************************************/

#define PRH_DEFAULT_ACCEPT_TIMEOUT          0x1FA0   /* 5.06 Seconds */
#define PRH_DEFAULT_PAGE_TIMEOUT            0x2000   /* 5.12 Seconds */
#if(PRH_BS_CFG_SYS_HCI_V12_FUNCTIONALITY_SUPPORTED==1)
#define DEFAULT_INQUIRY_SCAN_INTERVAL       0x0800   /* 2.56 Seconds  0x1000 */
#else
#define DEFAULT_INQUIRY_SCAN_INTERVAL       0x0800   /* 1.28 Seconds  0x0800 */
#endif
#define DEFAULT_INQUIRY_SCAN_WINDOW         0x0012   /* 11.25 ms == 18 slots */
#define DEFAULT_PAGE_SCAN_INTERVAL          0x0800   /* 1.28 Seconds */
#define DEFAULT_PAGE_SCAN_WINDOW            0x0012   /*      */
#else
#define PRH_DEFAULT_ACCEPT_TIMEOUT          0x1FA0   /* 5.06 Seconds */
#define PRH_DEFAULT_PAGE_TIMEOUT            0xFA0   //0x960   // 0x960:1.5s    0x2000: 5.12 Seconds
#if(PRH_BS_CFG_SYS_HCI_V12_FUNCTIONALITY_SUPPORTED==1)

#define DEFAULT_INQUIRY_SCAN_INTERVAL       0x400   // 0x400: 640ms  0x1000 :2.56 Seconds 
#else
#define DEFAULT_INQUIRY_SCAN_INTERVAL       0x0800   /* 1.28 Seconds */
#endif
#define DEFAULT_INQUIRY_SCAN_WINDOW         0x0100   // 0x0100 :160ms     0x0012:11.25ms
#define DEFAULT_PAGE_SCAN_INTERVAL          0x0400   // 0x0400 : 640ms  0x0800: 1.28 Seconds 
#define DEFAULT_PAGE_SCAN_WINDOW            0x0060  // 0x0060:60ms    0x0012:11.25ms
#endif
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
	#define DEFAULT_T_POLL                          (g_LM_config_info.default_t_poll)
	#define DEFAULT_T_POLL_ESCO                     40
#else
#if (BT_MODE == BT_MODE_1V2)
    #define DEFAULT_T_POLL                          10
    #define DEFAULT_T_POLL_ESCO                     (4*DEFAULT_T_POLL)
#else
    #define DEFAULT_T_POLL                          40
    #define DEFAULT_T_POLL_ESCO                     (DEFAULT_T_POLL)
#endif
#endif
#define DEFAULT_TRANS_INTERVAL                   0

#define DEFAULT_SUPERVISION_TIMEOUT         0x7D00   /*20.00 Seconds */
#define DEFAULT_FLUSH_TIMEOUT               0x0000
#define PRH_DEFAULT_LMP_TRANSACTION_TIMEOUT 0xbb80   /*30.00 Seconds */
#define DEFAULT_NBC                         0x0002   /* 2 Broadcast  */
#ifdef LMP_LINK_L2CAL_TIMEOUT_ENABLE 
	#define DEFAULT_L2CAP_TIMEOUT               0x4000   /*10.00 Seconds,consider LMP_SET_AFH retransission time with HUAWEI_P9 */
#endif

#define PRH_ENCRYPTION_KEY_REFRESH_TIMEOUT  (30*60*(BT_CLOCK_TICKS_PER_SECOND/2))

/********************************************************
 * CONFIGURATION CONSTANTS
 ********************************************************/

/* #1701: current implementation contraint DACC >= TBRP+6 */
#define _LM_MIN_TBRP_DACC_GUARD             6

/* #2292: next beacon instant - end access window > 2 frames */
#define _LM_MIN_BEACON_INSTANT_GUARD        4

/********************************************************
 * Indexs for LMP Events to the higher layers
 ********************************************************/
#define _LM_REMOTE_SUPORTED_FEATURES_CFM                  0x00
#define _LM_REMOTE_VERSION_CFM                            0x01
#define _LM_CONNECT_IND                                   0x02
#define _LM_CONNECT_CFM                                   0x03
#define _LM_CONNECT_COMPLETE                              0x04
#define _LM_DISCONNECT_IND                                0x05
#define _LM_DISCONNECT_COMPLETE                           0x06
#define _LM_REMOTE_NAME_CFM                               0x07
#define _LM_ROLE_CHANGE_EVENT                             0x08
#define _LM_INQUIRY_RESULT_EVENT                          0x09
#define _LM_INQUIRY_COMPLETE_EVENT                        0x0a
#define _LM_READ_CLOCK_OFFSET_EVENT                       0x0b
#define _LM_RETURN_LINK_KEY_DB_EVENT                      0x0c
#define _LM_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT 0x0d
#define _LM_CONNECTION_PACKET_TYPE_CHANGED_EVENT          0x0f
#define _LM_INQUIRY_RESULT_WITH_RSSI_EVENT                0x10
#define _LM_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT  0x11
#define _LM_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT 0x12
#define _LM_MAX_NUM_EVENTS                0x13 /* Always ensure last event */


#define FEATURES_SIZE           8
#define MAX_KEY_SIZE            16
#define MAX_RANDOM_NUMBER       16
#define ROLE_TID                0
#define PEER_ROLE_TID           1

#define ACTIVE_MODE             0x00
#define HOLD_MODE               0x01
#define SNIFF_MODE              0x02
#define PARK_MODE               0x03

#define MAX_LMP_OPCODE          57

#define LIAC_LAP                0x9E8B00

#define HCI_2DH1 0x0002
#define HCI_3DH1 0x0004
#define HCI_DM1  0x0008
#define HCI_DH1  0x0010
#define HCI_2DH3 0x0100
#define HCI_3DH3 0x0200
#define HCI_DM3  0x0400
#define HCI_DH3  0x0800
#define HCI_2DH5 0x1000
#define HCI_3DH5 0x2000
#define HCI_DM5  0x4000
#define HCI_DH5  0x8000

typedef u_int16 t_HCIpacketTypes;   /* packet-types based up | of above */

#define HCI_1_SLOT_EDR  (HCI_2DH1 | HCI_3DH1)
#define HCI_3_SLOT_EDR  (HCI_2DH3 | HCI_3DH3)
#define HCI_5_SLOT_EDR  (HCI_2DH5 | HCI_3DH5)

#define HCI_ACL_2MBS_EDR (HCI_2DH1 | HCI_2DH3 | HCI_2DH5)
#define HCI_ACL_3MBS_EDR (HCI_3DH1 | HCI_3DH3 | HCI_3DH5)

#define HCI_ACL_EDR_ALL  (HCI_ACL_2MBS_EDR | HCI_ACL_3MBS_EDR)

#define HCI_ACL_EDR  (HCI_ACL_2MBS_EDR)

#define HCI_1_SLOT_BR  (HCI_DM1 | HCI_DH1)
#define HCI_3_SLOT_BR  (HCI_DM3 | HCI_DH3)
#define HCI_5_SLOT_BR  (HCI_DM5 | HCI_DH5)

#define HCI_HIGH_RATE    (HCI_DH1 | HCI_DH3 | HCI_DH5)
#define HCI_MEDIUM_RATE  (HCI_DM1 | HCI_DM3 | HCI_DM5)

#define HCI_ACL_BR   (HCI_HIGH_RATE | HCI_MEDIUM_RATE)

#define HCI_1_SLOT  (HCI_1_SLOT_BR | HCI_1_SLOT_EDR)
#define HCI_3_SLOT  (HCI_3_SLOT_BR | HCI_3_SLOT_EDR)
#define HCI_5_SLOT  (HCI_5_SLOT_BR | HCI_5_SLOT_EDR)

#if (PRH_BS_CFG_SYS_HCI_V20_FUNCTIONALITY_SUPPORTED==1)
#define HCI_ACL      (HCI_ACL_BR | HCI_ACL_EDR)
#else
#define HCI_ACL      (HCI_ACL_BR)
#endif

#define HCI_SYN_HV1   0x0001
#define HCI_SYN_HV2   0x0002
#define HCI_SYN_HV3   0x0004
#define HCI_SYN_EV3   0x0008
#define HCI_SYN_EV4   0x0010
#define HCI_SYN_EV5   0x0020
#define HCI_SYN_2EV3  0x0040
#define HCI_SYN_3EV3  0x0080
#define HCI_SYN_2EV5  0x0100
#define HCI_SYN_3EV5  0x0200

#define HCI_SYN_EDR   (HCI_SYN_2EV3 | HCI_SYN_3EV3 | HCI_SYN_2EV5 | HCI_SYN_3EV5)

#define HCI_SCO_BR    (HCI_SYN_HV1 | HCI_SYN_HV2 | HCI_SYN_HV3)
#define HCI_eSCO_BR   (HCI_SYN_EV3 | HCI_SYN_EV4 | HCI_SYN_EV5)     

#define HCI_SYN_BR    (HCI_SCO_BR | HCI_eSCO_BR)

#if (PRH_BS_CFG_SYS_HCI_V20_FUNCTIONALITY_SUPPORTED==1)
#define HCI_SYN       (HCI_SYN_BR | HCI_SYN_EDR)
#else
#define HCI_SYN       (HCI_SYN_BR)
#endif

/*
 * The HCI Accept Synchronous Connection (the 
 * HCI Setup Synchronous Connection has nothing
 * to say on the matter. Normally they
 * are tied low if unused.. go figure :-)
 * command requires that the reserved bits are set
 * high...
 */
#define SYN_RESERVED_PACKET_TYPES 0xfc00

/*
 * Bitfields for indicating allocated  DSCOs    
 */
#define LM_SCO_PACKET_HV1      0x00
#define LM_SCO_PACKET_HV2      0x01
#define LM_SCO_PACKET_HV3      0x02
#define LM_SCO_PACKET_HV4      0x03

#define LM_SCO_PACKET_POLLNULL 0x00
#define LM_SCO_PACKET_EV3      0x07
#define LM_SCO_PACKET_EV4      0x0C
#define LM_SCO_PACKET_EV5      0x0D
#define LM_SCO_PACKET_2EV3     0x26
#define LM_SCO_PACKET_2EV5     0x2C
#define LM_SCO_PACKET_3EV3     0x37
#define LM_SCO_PACKET_3EV5     0x3D



#define LM_SCO_PACKET_INVALID 0xFF



#define LMP_SNIFF_ACTIVATION_PENDING    0x21
#define LMP_SNIFF_ACTIVE                0x22
#define LMP_SNIFF_INACTIVE              0x23

#define LMP_SSR_INACTIVE                0x00
#define LMP_SSR_ACTIVE                  0x01
//#define LMP_SSR_INITIATE                0x02



#define LMP_BEACON_ACTIVE   0x31
#define LMP_BEACON_INACTIVE 0x32
#define LMP_ACCESS_WINDOW_ACTIVE 0x33
#define LMP_N_POLL_AFTER_ACCESS_WINDOW 0x34
#define LMP_UNPARK_AFTER_ACCESS_WINDOW 0x35
#define LMP_BROADCAST_SCAN_WINDOW_EXTENSION_ACTIVE 0x36

#define MAX_NAME_FRAGMENT_SIZE 14

#define PRH_NO_BROADCAST         0x01
#define PRH_ACTIVE_BROADCAST     0x02
#define PRH_PICONET_BROADCAST    0x03

#define PRH_NO_TRAFFIC           0x00
#define PRH_BEST_EFFORT          0x01
#define PRH_GUARANTEED_SERVICE   0x02

#define PRH_NO_FLUSH_PENDING       0x00
#define PRH_INIT_FLUSH_PENDING     0x01
#define PRH_START_PKT_FLUSHED      0x02
#define PRH_QUEUE_FLUSH_PENDING    0x04

#define PRH_NO_UNPARK_PENDING       0x00
#define PRH_MASTER_INITIATED_UNPARK 0x01
#define PRH_SLAVE_INITIATED_UNPARK  0x02

#define PRH_ONE_SHOT_INQUIRY  1
#define PRH_PERIODIC_INQUIRY  2

#define PRH_NO_FLUSH                   0
#define PRH_LINK_BUFFER_FLUSH          1
#define PRH_PACKET_FLUSH               2
#define PRH_LINK_BUFFER_FLUSH_AUTO     3
#define PRH_INTERNAL_PACKET_FLUSH      4

#define PRH_FLUSH_IDLE                0
#define PRH_W4_SCAN_FROM_SCHEDULAR    2
#define PRH_W4_FLUSH_TO_EXECUTE       4
#define PRH_W4_ACK                    8

#define LINK_POLICY_NONE_ENABLED   0x00
#define LINK_POLICY_SWITCH_ENABLED 0x01
#define LINK_POLICY_HOLD_ENABLED   0x02
#define LINK_POLICY_SNIFF_ENABLED  0x04
#define LINK_POLICY_PARK_ENABLED   0x08

#define LINK_POLICY_AUTOMATIC_UNPARK_ENABLED 0x10
#define LINK_POLICY_AUTOMATIC_REHOLD_ENABLED 0x20


#define BT_SPEC_MAX_NAME_LENGTH 248

/* PRTH SPECIFIC DEBUG CONSTANTS */
#define PRTH_HW_ERROR_LMP_CH       0x01

#define MAX_NUM_LMP_PDUS  5
#define MAX_LMP_PDU_SIZE  20

/* Max time to service piconet */
//#define PRH_BS_PICONET_SERVICE_TIME ((1* DEFAULT_T_POLL) + LMP_SLOT_GAURD_FOR_CHANGE_PICONET)
/* Because of DEFAULT_T_POLL = 10 */
#define PRH_BS_PICONET_SERVICE_TIME ((4* DEFAULT_T_POLL) + LMP_SLOT_GAURD_FOR_CHANGE_PICONET)

/*
 * Maximum number of frames the d_esco
 * calculator will lookahead for clashes
 * before assuming the new connection
 * can be supported. 
 */
#define MAX_D_ESCO_LOOKAHEAD_FRAMES      36
#define MAX_NUM_D_ESCOS_IN_LIST (MAX_D_ESCO_LOOKAHEAD_FRAMES>>1)
#define MAX_NUM_RS_T_ESCOS_IN_LIST (MAX_D_ESCO_LOOKAHEAD_FRAMES>>1)

#endif

