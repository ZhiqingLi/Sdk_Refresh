#ifndef _PARTHUS_USLC_SLEEP_CTRL_STATES_
#define _PARTHUS_USLC_SLEEP_CTRL_STATES_

/***********************************************************************
 *
 * MODULE NAME:    uslc_sleep_ctrl_states.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  14 November 2001
 *
 * SOURCE CONTROL: $Id: uslc_sleep_ctrl_states.h,v 1.7 2004/07/20 15:11:38 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/

/*
 * Note:
 * Any modifications to enumerators below must be reflected in
 * char *TESTsleep_Ctrl_State_Desc[] in unit test module test_sleep.c
 */
typedef enum 
{
    OS_INACTIVE =                           0x0000,
    OS_START =                              0x0001,

    OS_W4_RADIO_TO_SLEEP =                  0x0002,
    OS_W4_BASEBAND_TO_SLEEP =               0x0004,

    OS_SLEEPING =                           0x0008,

    OS_WAKEN_RADIO =                        0x0010,
    OS_WAKEN_BASEBAND =                     0x0020,

    OS_REALIGN_FRAME_INTERRUPTS =           0x0040,
    OS_REALIGN_FRAME_INTERRUPTS_2 =         0x0080,

    OS_END =                                0x0100,

    OS_SENTINEL /* NOT A VALID STATE */ =   0xFE00
} t_USLCsleep_Ctrl_State;

t_USLCsleep_Ctrl_State USLCsleep_Debug_Get_Ctrl_State(void);

/* DEBUG */

#endif
