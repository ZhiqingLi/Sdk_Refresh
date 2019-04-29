#ifndef _CEVA_LE_CONST_
#define _CEVA_LE_CONST_

/**********************************************************************
 * MODULE NAME:    le_const.h
 * PROJECT CODE:    BlueStream Low Energy
 * DESCRIPTION:    LE HC Consts
 * MAINTAINER:     Gary Fleming
 * DATE:           23 JUN 99
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 * SOURCE CONTROL: $Id: le_const.h,v 1.3 2014/03/11 03:13:55 garyf Exp $
 *    
 **********************************************************************/



#define PUBLIC_ADDRESS 0x00
#define RANDOM_ADDRESS 0x01

#define ADVERTISING_ACCESS_ADDRESS  0x8E89BED6

#define LE_DEFAULT_MIN_ADVERTISING_INTERVAL 0x0800
#define LE_DEFAULT_MAX_ADVERTISING_INTERVAL 0x0800
#define LE_MAX_ADVERTISING_EVENT_WIDTH_IN_CLK_TICKS 20
#define PUBLIC_DEVICE_ADDRESS_TYPE 0x00
#define RANDOM_DEVICE_ADDRESS_TYPE 0x01

#define LE_PASSIVE_SCAN 0x00
#define LE_ACTIVE_SCAN  0x01


#define LE_MAX_ADV_CHANNEL_PDU_LEN  0x25  // 37
#define LE_MIN_ADV_CHANNEL_PDU_LEN  0x06  //  6


// Interrupts
#define LE_TIM_0             0x00
#define LE_TIM_2             0x01
#define LE_PKA				 0x02
#define LE_PKD               0x03
#define LE_NO_PKD            0x04
#define LE_SYNC_DET          0x05
#define LE_AES_COMPLETE      0x06
#define LE_RX_HDR            0x07
#define LE_TIM_1             0x08
#define LE_TIM_3             0x09


#define LE_META_EVENT 0x3E

#define _LE_LL_CHECK_ADV_PKT_LENGTH(length)  ((length<=LE_MAX_ADV_CHANNEL_PDU_LEN) && (length >= LE_MIN_ADV_CHANNEL_PDU_LEN))

#define DIRECT_ADVERTISING_TIME_IN_TICKS 4096 // (0x10 * 0x0800)

#define STATIC_RANDOM_ADDRESS          0x01
#define NON_RESOLVABLE_RANDOM_ADDRESS  0x02
#define RESOLVABLE_RANDOM_ADDRESS      0x03


#define LL_VERSION_IND_SENT     0x01
#define LL_VERSION_IND_RECIEVED 0x02
#define MIN_CE_INTERVAL_IN_SCATTERNET 0x12

// Below Value is only for Test with IPAD - as default Inverval in iPad test apps is 30ms
//#define MIN_CE_INTERVAL_IN_SCATTERNET 0x48

// BQB Security Tests
#define TP_SEC_SLA_BV_03 0x01010003  // WORKS - BUT IUT has to have enabled as tester.
#define TP_SEC_SLA_BI_01 0x01110000  // WORKS
#define TP_SEC_SLA_BI_03 0x01110003  // WORKS
#define TP_SEC_SLA_BI_04 0x01110004  // WORKS
#define TP_SEC_SLA_BI_05 0x01110005  // WORKS
#define TP_SEC_MAS_BV_03 0x01000003  // WORKS
#define TP_SEC_MAS_BV_04 0x01000004  // WORKS  - OK
#define TP_SEC_MAS_BV_05 0x01000005  // WORKS  - OK
#define TP_SEC_MAS_BV_12 0x0100000C  // WORKS  - OK
#define TP_SEC_MAS_BI_01 0x01100001  // WORKS  - OK
#define TP_SEC_MAS_BI_03 0x01100003  // WORKS  - OK
#define TP_SEC_MAS_BI_04 0x01100004  // WORKS  - OK
#define TP_SEC_MAS_BI_05 0x01100005  // WORKS  - OK
#define TP_SEC_MAS_BI_06 0x01100006  // WORKS  - OK
#define TP_SEC_MAS_BI_07 0x01100007  // WORKS  - OK
#define TP_SEC_MAS_BI_08 0x01100008  // WORKS  - OK
#define TP_SEC_MAS_BI_09 0x01100009  // WORKS  - OK

//
#endif
