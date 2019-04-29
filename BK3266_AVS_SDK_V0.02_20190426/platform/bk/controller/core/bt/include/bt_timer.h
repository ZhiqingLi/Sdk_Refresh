#ifndef _PARTHUS_BT_TIMER_
#define _PARTHUS_BT_TIMER_

/******************************************************************************
 * MODULE NAME:    bt_timer.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Bluetooth System Wide Baseband Slot Based Timer
 * MAINTAINER:     John Nelson
 * DATE:           27 May 1999
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    27 May 1999 -   jn       - Initial Version V0.9
 *    23 Jul 1999 -   jn       - New function added for Tick Timing
 *
 * SOURCE CONTROL: $Id: bt_timer.h,v 1.17 2013/11/19 02:15:56 garyf Exp $
 *
 * NOTES TO USERS:
 *    Beware that a slot is 625us!
 *    To convert to seconds, multiply by BT_CLOCK_TICKS_PER_SECOND [Avoids
 *    floating point version/calculation].
 *    This timer is currently based on the fact that there may be no guaranteed
 *    interrupt to the baseband every slot.  This requires that the end time
 *    is stored. 
 *    Assumes that t_timer is unsigned.
 *    Baseband times now over 20.48 seconds, which means an optimised 16 bits 
 *    version is no longer supported.
 ******************************************************************************/

#define BT_CLOCK_TICKS_PER_SECOND   3200

#define BT_USEC_PER_CLK_TICK         312
#define BT_USEC_PER_SLOT             625
#define BT_USEC_PER_FRAME           1250

/*
 *  Clock size is 28 bits, if ever becomes 32 revisit timer code!
 */
#define BT_CLOCK_NUMBER_OF_BITS 28
#define BT_CLOCK_MAX_TICKS       (( (t_clock) 1 << BT_CLOCK_NUMBER_OF_BITS) - 1)

/*
 * Restriction: timeout value must be less than 2^(#bits-1 of t_timer)
 */

#define BT_CLOCK_MAX_TIMEOUT      ( (t_clock) 1 << (BT_CLOCK_NUMBER_OF_BITS-1))
#define BTtimer_Piconet_Extend(timer,timeSlots) BTtimer_Piconet_Reset_Slots(timer, timeSlots)

/* use the fact that BT native clock is 28 bit - top 4 bits set to F would be OFF condition for the time*/
// GF 12 Nov 2013 -- dont use Signed !! - Compilers will SCREAM.. 
#define BT_TIMER_OFF 0xF0000000
//#define BT_TIMER_OFF	(-1)
#define BTtimer_Safe_Clockwrap(clk ) ((clk) & BT_CLOCK_MAX_TICKS)

#define BTtimer_Clock_Difference(clk_start, clk_finish) (((clk_finish)-(clk_start)) & BT_CLOCK_MAX_TICKS)
#define BTtimer_Clock_Add_Safe_Clockwrap(clk, slots ) (((clk) + (slots)) & BT_CLOCK_MAX_TICKS)
/*
 * Accessor to clock used for timers.
 */
t_clock BTtimer_Get_Native_Clock(void);

/*
 *  Set Timeout relative to clock
 */
DRAM_CODE t_timer BTtimer_Set_Slots(t_slots timeSlots);
t_timer BTtimer_Set_Ticks(t_clock timeTicks);

/*
 * Set Timeout relative to existing native timer value
 */
t_timer BTtimer_Reset_Slots(t_timer timer, t_slots timeSlots);
t_timer BTtimer_Reset_Ticks(t_timer timer, t_clock timeTicks);
DRAM_CODE boolean BTtimer_Is_Expired(t_timer ptimer);

/*
 * Set Timeout relative to existing piconet timer value
 */
t_timer BTtimer_Piconet_Set_Slots(t_slots timeSlots);
t_timer BTtimer_Piconet_Reset_Slots(t_timer timer, t_slots timeSlots);
boolean BTtimer_Piconet_if_Expired(t_timer ptimer);

/*
 * Procedure timing functions
 */
/************************************************************************
 * BTtimer_Piconet_Reset_Delta_Slots
 *
 * Set Timeout based on the Delta Slots relative to existing
 * timer value ensuring that the timer value is greater that
 * the reference target time.
 *
 * new_timer = timer + N * T_proc_slots st, new_timer > ref_time_ticks
 ************************************************************************/
RAM_CODE t_timer BTtimer_Piconet_Reset_Delta_Slots(
    t_timer timer,                      /* -i   TImer to Adjust             */
    t_clock ref_time_ticks,             /* -i   Time to check equations at  */
    t_slots T_proc_slots                /* -i   Procedure Interval in Slots */
    );

/************************************************************************
 * BTtimer_Set_Timer_On_Timing_Equation
 *
 * Set Timeout based on the timing equation to next time for which 
 * equation will be satisfied.
 *
 *  CLK 27-1 mod T_proc = D_proc           for initialization 1
 *  ______
 * (CLK 27 ,CLK 26-1 ) mod T_proc = D_proc for initialization 2
 *
 * The idea of initialisation is to ensure the clock is not going
 * to wrap in the vicinity of initialisation.
 ************************************************************************/
RAM_CODE t_timer BTtimer_Set_Timer_On_Timing_Equation( 
    t_clock ref_time_ticks,             /* -i   Time to check equations at  */
    boolean init_proc_2,                /* -i   Initialisation 1/2 if 0/1   */
    t_slots T_proc_slots,               /* -i   Procedure Interval in Slots */
    t_slots D_proc_slots                /* -i   Procedure Offset in Slots   */
    );

/************************************************************************
 * BTtimer_Is_Timing_Equation_Satisfied
 *
 * Is the timing equation for this procedure satisfied:
 *  CLK 27-1 mod T_proc = D_proc           for initialization 1
 *  ______
 * (CLK 27 ,CLK 26-1 ) mod T_proc = D_proc for initialization 2
 *
 * Given the modulus operation this procedure is relatively slow
 ************************************************************************/
RAM_CODE boolean BTtimer_Is_Timing_Equation_Satisfied(                        
    t_clock ref_time_ticks,             /* -i   Time to check equations at  */
    boolean init_proc,                  /* -i   Initialisation 1/2 if 0/1   */
    t_slots T_proc_slots,               /* -i   Procedure Interval in Slots */
    t_slots D_proc_slots                /* -i   Procedure Offset in Slots   */
    );

/************************************************************************
 *  BTtimer_Is_Expired_For_Time
 *
 *  The conditional expression will evaluate to false until the
 *  reference time becomes greater than the set time out value.
 *  Since the check is for greater than, the setting of the timer must
 *  ensure that the timer is actually set to the preceding tick
 *  after which the timeout occurs.
 ************************************************************************/
DRAM_CODE boolean BTtimer_Is_Expired_For_Time(
    t_timer timer,                      /* -i   Timer to Adjust             */
    t_clock ref_time_ticks              /* -i   Time to check equations at  */
    );    

/************************************************************************
 *  BTtimer_Is_Expired_For_Time_Instant
 *
 *  The conditional expression will evaluate to false unless the
 *  reference time is one greater than the set time out value.
 *  Hence evaluates to true only on first instant timer has expired.
 *  Useful alternative where we want to avoid events occuring late.
 *  
 ************************************************************************/
boolean BTtimer_Is_Expired_For_Time_Instant(
    t_timer timer,                      /* -i   Timer to Adjust             */
    t_clock ref_time_ticks              /* -i   Time to check equations at  */
    );    

/************************************************************************
 * BTtimer_Extract_Timer_D_proc_slots
 *
 * Extract the current D procedure in slots for the presented timer.  
 *
 * D_proc continuously change as clock wraps through each bit27 
 * transition i.e. every 11h 40m.  Therefore before setting up  
 * associated devices the current D proc must be determined.
 *
 ************************************************************************/
t_clock BTtimer_Extract_Timer_D_proc_slots( 
    t_timer timer,                      /* -i   Timer to extract D_proc     */
    boolean init_proc_2,                /* -i   Initialisation 1/2 if 0/1   */
    t_slots T_proc_slots                /* -i   Procedure Interval in Slots */
    );


/*
 * Support normal convention of_ Is_ not _if_
 */
#define BTtimer_Piconet_Is_Expired BTtimer_Piconet_if_Expired
#endif
