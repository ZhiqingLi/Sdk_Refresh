#ifndef _PARTHUS_BT_ADDR_SYNC_
#define _PARTHUS_BT_ADDR_SYNC_


/******************************************************************************
 * MODULE NAME:    bt_addr_sync.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    ADDR_Syncword Generation
 * MAINTAINER:     John Nelson
 * DATE:           1 May 1999
 *
 * SOURCE CONTROL: $Id: bt_addr_sync.h,v 1.8 2004/07/07 14:21:00 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     1 June 1999 -   jn       - Initial Version V0.9
 *
 * SOURCE: Based on V1_4 Design
 * ISSUES:
 *    Reimplement as macros.
 * NOTES TO USERS:
 *    
 ******************************************************************************/

/* 
 * Sync word: BCH(64,30) Generator, degree 34, g(x) = Octal 260534236651   
 */
#define SYNC_BCHgenHi 026053423665L     /* Right align in 64 bit register  */
#define SYNC_BCHgenLo (1L << 29)        /* Octal as specified              */

/* 
 * Sync word: Pseudo-random noise generator sequence generation            
 */
#define SYNC_PNseqHi  0x83848D96L
#define SYNC_PNseqLo  0xBBCC54FCL

void BTaddr_Build_Sync_Word(const t_lap a_lap, u_int32* syncHi, u_int32* syncLo);

#endif
