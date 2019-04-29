#ifndef _PARTHUS_TC_TYPES_
#define _PARTHUS_TC_TYPES_

/*****************************************************************************
 *
 * MODULE NAME:     tc_types.h   
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:     System test related implementation types
 * MAINTAINER:      Daire McNamara <Daire.McNamara@sslinc.com>
 * CREATION DATE:   24th February 2000     
 *
 * SOURCE CONTROL: $Id: tc_types.h,v 1.20 2007/11/27 15:36:16 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *     27.Jan.2000    DMN   Initial Draft - support TCI_Read_Packet_Log
 *     01.Feb.2000    IG    Active IRQ, Build Params
 *     24.Feb.2000    DMN   Split off TC Interface from implementation types
 *
 ****************************************************************************/

#include "sys_config.h"
#include "sys_types.h"
#include "uslc_testmode.h"

typedef enum
{
    SYSTEM_TEST_GROUP_RESERVED          = 0x00000000,
    TESTMODE_DUT_TESTER                 = 0x00010000,
    TESTMODE_DATA_PUMP                  = 0x00010001,
    TESTMODE_TX_RX_TEST_STANDALONE      = 0x00010002
} t_system_test_group;

#endif
