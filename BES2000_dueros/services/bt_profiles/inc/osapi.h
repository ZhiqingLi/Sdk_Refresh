/****************************************************************************
 *
 * File:
 *     $Id: osapi.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Defines the APIs used by the stack to access system services.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 * 
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __OSAPI_H
#define __OSAPI_H

#include "xatypes.h"


#ifdef WIN32


#else 
#ifdef __ICCARM__
#include <intrinsics.h>

#define OS_Report(s, ...)
#define Report(s)
#define OS_MemSet memset

#else

#include "cmsis.h"
#include "hal_uart.h"

#endif
#endif


/*---------------------------------------------------------------------------
 * Operating System API layer
 *
 *     The OS API is a set of functions used by the stack to access
 *     system services. If you are porting to a new operating system,
 *     you must provide the functions described here.
 *
 *     The functions in this API are intended for exclusive use by the
 *     stack. Applications should use the native operating system interface
 *     and avoid making OS_ calls themselves.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * TimeT type
 *
 *     Indicates an amount of time in system ticks or milliseconds. This must
 *     be a 32-bit value because some timeouts may exceed 5 minutes.
 */ 
typedef U32 TimeT;

/* End of TimeT type */


#if XA_MULTITASKING == XA_ENABLED

/*---------------------------------------------------------------------------
 * OsTimerNotify type
 *
 *     The type for a timer notification callback. The notification function
 *     is provided by the stack using OS_StartTimer, and is called
 *     by the OS API to indicate that the timer has fired.
 *
 *     This function may be called from interrupt mode or task mode as
 *     required by the operating system.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */ 
typedef void (*OsTimerNotify)(void);

#endif /* XA_MULTITASKING */

/* End of OsTimerNotify type */


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * OS_Init()
 *
 *     Initializes the stack and operating system layer. This function is
 *     not called by the stack, but must be called by your operating system
 *     or application to set up the stack and its operating system services.
 * 
 *     The OS_Init function must perform the following tasks in this order:
 * 
 *     1) Seed the random number generator with a random value such as
 *     the current system time.
 * 
 *     2) Any other operating-system specific functions, such as timer
 *     initialization or the creation of semaphores.
 * 
 *     3) Call EVM_Init to initialize the Event Manager. This also
 *     initializes all stack layers. Note that this function may
 *     return FALSE if stack initialization failed for some reason.
 * 
 *     4) Create and start the stack task. This task calls EVM_Process
 *     at least once for every call to OS_NotifyEvm. See OS_NotifyEvm
 *     for more details.
 * 
 * Returns:
 *     TRUE - Success.
 *
 *     FALSE - Initialization failed for some reason.
 */
BOOL OS_Init(void);


/*---------------------------------------------------------------------------
 * OS_Deinit()
 *
 *     Deinitializes the stack and operating system layer. This function
 *     is not called by the stack, but may be called by your operating
 *     system or application to shut down the stack and free its resources.
 */
void OS_Deinit(void);

/*---------------------------------------------------------------------------
 * OS_GetSystemTime()
 *
 *     Called by the stack to get the current system time in ticks.
 *
 *     The system time provided by this function can start at any value;
 *     it does not to start at 0. However, the time must "roll over" only
 *     when reaching the maximum value allowed by TimeT. For instance, a
 *     16-bit TimeT must roll over at 0xFFFF. A 32-bit TimeT must roll
 *     over at 0xFFFFFFFF.
 *
 *     System ticks may or may not be equivalent to milliseconds. See the
 *     BT_MS_TO_TICKS macro in config.h (General Configuration Constants)
 *     for more information.
 *
 * Returns:
 *     The current time in ticks.
 */
TimeT OS_GetSystemTime(void);


/*---------------------------------------------------------------------------
 * OS_Rand()
 *
 *     Called by the stack to generate a random number between
 *     0x0000 and 0xFFFF.
 *
 * Returns:
 *     A 16-bit random number.
 */
U16 OS_Rand(void);


#if BT_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * OS_StopHardware()
 *
 *     Called by the Blue SDK stack to enter a critical section, during which
 *     the hardware driver must not execute.
 *
 *     The stack calls OS_StopHardware just before it is going to read
 *     or modify data that might be used by the hardware driver.
 *     Immediately following the data access, the stack calls
 *     OS_ResumeHardware. In most systems, the time spent in the critical
 *     section will be less than 50 microseconds.
 *
 *     In systems where the hardware driver is implemented with a system
 *     task, the hardware driver task must be at a higher priority and
 *     run to completion before the stack task can execute. Between
 *     OS_StopHardware and OS_ResumeHardware, the hardware driver
 *     task must not be scheduled to execute. For example, this could be
 *     accomplished by taking a semaphore required by the hardware driver.
 *
 *     In systems where the hardware driver is implemented entirely in an
 *     interrupt service routine, OS_StopHardware must prevent stack-
 *     related hardware interrupts from firing. Other interrupts can
 *     be left as they are.
 *
 *     The stack never "nests" calls to OS_StopHardware.
 *     OS_ResumeHardware is always called before OS_StopHardware
 *     is called again.
 *
 * Requires:
 *     BT_STACK enabled.
 */
void OS_StopHardware(void);

/*---------------------------------------------------------------------------
 * OS_ResumeHardware()
 *
 *     Called by the Blue SDK stack to leave the critical section entered by
 *     OS_StopHardware.
 *
 *     In systems where the hardware driver is implemented with a system
 *     task, OS_ResumeHardware allows the hardware driver task to be
 *     scheduled again. For example, a semaphore could be released
 *     that allows the hardware driver to run.
 *
 *     In systems where the hardware driver is implemented entirely by an
 *     interrupt service routine, OS_ResumeHardware must restore the
 *     interrupts disabled by OS_StopHardware.
 *
 * Requires:
 *     BT_STACK enabled.
 */
void OS_ResumeHardware(void);
#endif

#if IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * OS_IrStopHardware()
 *
 *     Called by the stack to enter a critical section, during which
 *     the hardware driver must not execute.
 *
 *     This call is exactly like OS_StopHardware() except it stops the IrDA
 *     hardware, not the Bluetooth hardware.
 */
void OS_IrStopHardware(void);

/*---------------------------------------------------------------------------
 * OS_IrResumeHardware()
 *
 *     Called by the stack to leave the critical section entered by
 *     OS_StopHardware.
 *
 *     This call is exactly like OS_ResumeHardware() except it resumes the
 *     IrDA hardware, not the Bluetooth hardware.
 */
void OS_IrResumeHardware(void);
#endif

/*---------------------------------------------------------------------------
 * OS_MemCopy()
 *
 *     Called by the stack to copy memory from one buffer to another.
 *     
 *     This function's implementation could use the ANSI memcpy function.
 *
 * Parameters:
 *     dest - Destination buffer for data.
 *
 *     source - Source buffer for data. "dest" and "source" must not
 *         overlap.
 *
 *     numBytes - Number of bytes to copy from "source" to "dest".
 */
void OS_MemCopy(U8 *dest, const U8 *source, U32 numBytes);


/*---------------------------------------------------------------------------
 * OS_MemCmp()
 *
 *     Called by the stack to compare the bytes in two different buffers.
 *     If the buffers lengths or contents differ, this function returns FALSE.
 *
 *     This function's implementation could use the ANSI memcmp
 *     routine as shown:
 *
 *     return (len1 != len2) ? FALSE : (0 == memcmp(buffer1, buffer2, len2));
 *     
 *
 * Parameters:
 *     buffer1 - First buffer to compare.
 *
 *     len1 - Length of first buffer to compare.
 *
 *     buffer2 - Second buffer to compare.
 *
 *     len2 - Length of second buffer to compare.
 *
 * Returns:
 *     TRUE - The lengths and contents of both buffers match exactly.
 *
 *     FALSE - Either the lengths or the contents of the buffers do not
 *         match.
 */
BOOL OS_MemCmp(const U8 *buffer1, U16 len1, const U8 *buffer2, U16 len2);


/*---------------------------------------------------------------------------
 * OS_MemSet()
 *
 *     Fills the destination buffer with the specified byte.
 *
 *     This function's implementation could use the ANSI memset
 *     function.
 *
 * Parameters:
 *     dest - Buffer to fill.
 *
 *     byte - Byte to fill with.
 *
 *     len - Length of the destination buffer.
 */
void OS_MemSet(U8 *dest, U8 byte, U32 len);


#if (XA_MULTITASKING == XA_ENABLED)

/*---------------------------------------------------------------------------
 * OS_StartTimer()
 *
 *     Called by the stack to start the event timer. When the time
 *     expires, it is the system's responsibility to call the notify
 *     function provided. The notify function may be called in either
 *     interrupt or task modes as required by the operating system.
 *
 *     If the timer is already active, OS_StartTimer automatically
 *     cancels the previous timer as if OS_CancelTimer was called.
 *
 *     In non-multitasking systems (where XA_MULTITASKING is disabled), this
 *     function is not used. EVM_Process will call OS_GetSystemTime
 *     frequently to determine if its internal timers have elapsed.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 *
 * Parameters:
 *     time - number of ticks until the timer fires
 * 
 *     func - The function to call when the timer expires.
 *
 */
void OS_StartTimer(TimeT time, OsTimerNotify func);


/*---------------------------------------------------------------------------
 * OS_CancelTimer()
 *
 *     Called by the stack to stops the event timer. This must prevent
 *     the operating system from calling the timer notification
 *     function provided by OS_StartTimer.
 *
 *     If function is called when the event timer is not running, do
 *     nothing.
 *
 *     In non-multitasking systems (where XA_MULTITASKING is disabled), this
 *     function is not used.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_CancelTimer(void);

/*---------------------------------------------------------------------------
 * OS_NotifyEvm()
 *
 *     Called by the stack to indicate that EVM_Process should be called.
 *     Depending on your operating system, this call may be made in
 *     interrupt mode or task mode.
 *
 *     In multitasking systems, the stack task alternately calls EVM_Process
 *     and blocks to save CPU time. The block may be implemented by an
 *     event semaphore or similar object. OS_NotifyEvm should post the
 *     semaphore, or otherwise cause the stack task to unblock and
 *     call EVM_Process.
 *
 *     In non-multitasking systems (where XA_MULTITASKING is disabled),
 *     tasks are usually simulated using a round-robin execution scheme. In
 *     this case, EVM_Process is called continuously, and no task ever
 *     blocks. Therefore, this function is not used.
 *
 *     If OS_NotifyEvm is called during EVM_Process execution,
 *     EVM_Process must be executed again before the stack task blocks.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_NotifyEvm(void);


/*---------------------------------------------------------------------------
 * OS_LockStack()
 *
 *     Called by the stack and its APIs to prevent other tasks from
 *     simultaneous access to the stack. When the stack is "locked" by
 *     a task, calls to OS_LockStack from different task will block
 *     until the stack is unlocked. This allows multiple application tasks
 *     and the stack task to coexist without corrupting data.
 *
 *     OS_LockStack calls may be "nested", meaning that a single task
 *     may call OS_LockStack several times without blocking, then call
 *     OS_UnlockStack an equal number of times.
 *
 *     In multitasking systems, OS_LockStack may be implemented with a
 *     mutual-exclusion semaphore or a similar object. It may be necessary
 *     to check the current task's ID and manage a use count if the operating
 *     system does not handle this for you.
 *
 *     In non-multitasking systems, no task can interrupt another task.
 *     Therefore, this call is not used and need not be present.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_LockStack(void);

/*---------------------------------------------------------------------------
 * OS_UnlockStack()
 *
 *     Called by the stack and its APIs to re-enable access to the stack.
 *     If a task calls OS_UnlockStack the same number of times as
 *     OS_LockStack, the stack is unlocked and any task may lock the
 *     stack without blocking.
 *
 *     In non-multitasking systems, no task can interrupt another task.
 *     Therefore, this call is not used and need not be present.
 *
 * Requires:
 *     XA_MULTITASKING enabled.
 */
void OS_UnlockStack(void);

#else /* XA_MULTITASKING disabled */
#define OS_LockStack()   (void)0
#define OS_UnlockStack() (void)0
#define OS_NotifyEvm()   (void)0

#endif /* XA_MUTLTITASKING */

/*---------------------------------------------------------------------------
 * OS_StrCmp()
 *
 *     Compares two strings for equality.
 *
 * Parameters:
 *     Str1 - String to compare.
 *     Str2 - String to compare.
 *
 * Returns:
 *     Zero - If strings match.
 *     Non-Zero - If strings do not match.
 */
U8 OS_StrCmp(const char *Str1, const char *Str2);


/*---------------------------------------------------------------------------
 * OS_StrLen()
 *
 *     Calculate the length of the string.
 *
 * Parameters:
 *     Str - String to count length.
 *
 * Returns:
 *     Returns length of string.
 */
U16 OS_StrLen(const char *Str);

/*---------------------------------------------------------------------------
 * OS_StrCopy()
 *
 *     Copies the source string into the destination string. This function
 *     copies at most size - 1 bytes. 
 *
 * Parameters:
 *     dest - Destination string that is being copied to.
 *     source - Source string that is being copied.
 *     size   - Copies up to size bytes of the string.
 *
 */
void OS_StrCopy(char *dest, const char *source, U32 size);

/*---------------------------------------------------------------------------
 * OS_StrCat()
 *
 *     Concatenates the source string onto the destination string. This
 *     will copy at most size - OS_StrLen(dest) - 1 bytes. This function is
 *     should be used with strlcat.
 *
 * Parameters:
 *     dest - Destination string that is being added to.
 *     source - Source string that is being copied.
 *     size   - Size of the dest buffer. 
 *
 */
void OS_StrCat(char *dest, const char *source, U32 size);

/*---------------------------------------------------------------------------
 * OS_StrStr()
 *
 *     Locates the substring str2 in the string str1.
 *
 * Parameters:
 *     str1 - The string to search. 
 *     str2 - The substring to search for. 
 *
 * Returns:
 *      Returns a pointer to the beginning of the substring if found, or null.
 */
char *OS_StrStr(char *str1, const char *str2);
#if XA_DEBUG == XA_ENABLED

/*---------------------------------------------------------------------------
 * OS_Assert()
 *
 *     Called by the stack to indicate that an assertion failed. OS_Assert
 *     should display the failed expression and the file and line number
 *     where the expression occurred.
 *
 *     The stack uses Assert calls, which are redefined to call OS_Assert
 *     if their expression fails. These calls verify the correctness of
 *     function parameters, internal states, and data to detect problems
 *     during debugging.
 *
 * Requires:
 *     XA_DEBUG enabled.
 *
 * Parameters:
 *     expression - A string containing the failed expression.
 *
 *     file - A string containing the file in which the expression
 *         occurred.
 *
 *     line - The line number that tested the expression.
 */

#ifdef __ICCARM__
#define OS_Assert(a,b,c) (void)0
#else
void OS_Assert(const char *expression, const char *file, U16 line);
#endif

/* A define to convert asserts to OS_Assert calls */
#define Assert(exp)  (((exp) != 0) ? (void)0 : OS_Assert(#exp,__func__,(U16)__LINE__))

/* A similar define that always evaluates the expression regardless of
 * the DEBUG mode
 */
#define AssertEval(exp) Assert(exp)

#else /* XA_DEBUG */

/* When XA_DEBUG is disabled, null versions of these functions are
 * substituted for the calls.
 */
#define Assert(ex) (void)0
#define AssertEval(exp) (void)(exp)

#endif /* XA_DEBUG */


#ifdef WIN32


#else 
#ifdef __ICCARM__

#else
#include <stdio.h>
#include "hal_trace.h"
#if (XA_DEBUG_PRINT == XA_ENABLED) && !defined(AUDIO_DEBUG)

/*---------------------------------------------------------------------------
 * OS_Report()
 *
 *     Called by the stack to report debugging information.
 *
 * Requires:
 *     XA_DEBUG_PRINT enabled.
 *
 * Parameters:
 *     format - A string containing the failed expression.
 *
 *     ... - printf style arguments.
 */
void OS_Report(const char *format, ...);
#define OS_Report(s, ...) hal_trace_printf_without_crlf(s, ##__VA_ARGS__)

/* DebugPrint and Report are used in place of OS_Report() */
#define DebugPrint(s) OS_Report s 
#define Report(s) OS_Report s
#define OS_DUMP8(x,y,z)    DUMP8(x,y,z)
#else /* XA_DEBUG_PRINT */

#define DebugPrint(s) (void)0
#define Report(s) (void)0
#define OS_DUMP8(x,y,z)   
#endif /* XA_DEBUG_PRINT */

#endif
#endif


#define __printf(s, ...) (void)0



#endif /* __OSAPI_H */


