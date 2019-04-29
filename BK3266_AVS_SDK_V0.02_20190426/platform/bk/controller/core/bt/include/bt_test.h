#ifndef _PARTHUS_BT_TEST
#define _PARTHUS_BT_TEST

/******************************************************************************
 * MODULE NAME:    bt_test.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Routines to manage DUT mode.
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:  06 August 1999
 *
 * SOURCE CONTROL: $Id: bt_test.h,v 1.27 2010/02/11 02:02:31 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/
#include "sys_types.h"

/*
 * Types specific to HCI Testing Commands
 */

/*
 * t_dut_mode is the valid test states a device under test can be in
 *
 * DUT_DISABLED -> DUT_ENABLED -> DUT_ACTIVE -> DUT_ACTIVE_LOOPBACK
 * DUT_DISABLED -> DUT_ENABLED -> DUT_ACTIVE -> DUT_ACTIVE_TXTEST
 * DUT_DISABLED -> DUT_STANDALONE_TXTEST
 *
 * Enumerated values for the states are order important.
 */
typedef enum
{
       DUT_DISABLED            = 0,     /* Testmode Disabled [Default]      */
       DUT_ENABLED             = 1,     /* HCI_ENABLE_DEVICE_UNDER_TEST_MODE*/
       DUT_ACTIVE              = 2,     /* Testmode Activated               */
       DUT_ACTIVE_LOOPBACK     = 3,     /* Testmode Loopback Test           */
       DUT_ACTIVE_TXTEST       = 4,     /* Testmode Transmitter Test        */
       DUT_STANDALONE_TXTEST   = 5,     /* Parthus Tx Radio Test Mode       */
       DUT_STANDALONE_RXTEST   = 6,     /* Parthus Rx Radio Test Mode       */
       DUT_TESTMODE_TESTER     = 7      /* Parthus Testmode Tester          */
} t_dut_mode;

/*
 * t_loopback_mode is the loopback state a device can be in
 */
typedef enum
{
       NO_LOOPBACK             = 0,
       LOCAL_LOOPBACK          = 1,
       REMOTE_LOOPBACK         = 2
} t_loopback_mode;

typedef struct
{
    t_loopback_mode  loopback_mode;
    /*
     * ACL handle + 3 SCO handles
     */
    t_connectionHandle connection_handle[1+PRH_BS_CFG_SYS_MAX_SYN_LINKS];
} t_loopback_info;


__INLINE__ void BTtst_Initialise(void);

t_error BTtst_Write_Loopback_Mode(t_loopback_mode loop_back_mode);
__INLINE__ t_loopback_mode BTtst_Read_Loopback_Mode(void);

__INLINE__ t_dut_mode BTtst_Get_DUT_Mode(void);
__INLINE__ void BTtst_Set_DUT_Mode(t_dut_mode new_dut_mode);

void BTtst_Handle_Loopback_HCI_Data(void);
u_int32 BTtst_Get_Allowed_Packets(void);
boolean BTtst_Loopback_SCO_Enabled(void); 
t_packet BTtst_Get_Loopback_SCO_Packet(void);
u_int16  BTtst_Get_Loopback_Packet_Length(void);
t_packet BTtst_Get_Loopback_Packet(void);
#ifdef __USE_INLINES__
#include "bt_test_impl.h"
#endif

#endif /*_PARTHUS_BT_TEST*/
