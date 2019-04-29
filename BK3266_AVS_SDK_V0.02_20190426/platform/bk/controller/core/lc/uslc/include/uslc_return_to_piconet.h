#ifndef _PARTHUS_USLC_RETURN_TO_PICONET_
#define _PARTHUS_USLC_RETURN_TO_PICONET_

/***********************************************************************
 *
 * HEADER NAME:    uslc_return_from_hold.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LC Portion of Return To Piconet Procedure
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  04 September 2000
 *
 * SOURCE CONTROL: $Id: uslc_return_to_piconet.h,v 1.10 2004/07/07 14:21:59 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    04.Sep.2000 - IG  - Initial LC State Machine for Return From Hold
 *    08.Oct.2000 - IG  - Renamed Return to Piconet
 *
 ***********************************************************************/

void USLCr2p_Initialise(void);
/*
 * USLCr2p_Request():
 *     timeout specifies a timeout value - should be zero for
 *     Return from Hold, and a number of slots for Sniff etc.
 */
t_error USLCr2p_Request(t_deviceIndex device_index, t_slots timeout, t_clock instance);

t_error USLCr2p_Cancel(void);

void USLCr2p_State_Dispatcher(boolean /*dummy*/);

/***********************************************************************
 *
 * FUNCTION:  USLCr2p_Get_Search_Window_Frames()
 * PURPOSE:   To determine how many frames to search for next frequency
 *
 ***********************************************************************/
u_int8 USLCr2p_Get_Search_Window_Frames(
	const t_devicelink *p_device_link,    /* i - ref to device link    */
	t_slots r2p_offset_ticks              /* i - offset to r2p instant */
);


#endif
