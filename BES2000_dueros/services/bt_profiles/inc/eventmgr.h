/***************************************************************************
 *
 * File:
 *     $Id: eventmgr.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Types, defines, and prototypes for OS access to the Event Manager.
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

#ifndef __EVENTMGR_H
#define __EVENTMGR_H

#include "osapi.h"
#include "utils.h"

#if (IRDA_STACK == XA_ENABLED) || (BT_STACK == XA_ENABLED) || (XA_EVENTMGR == XA_ENABLED)

/*---------------------------------------------------------------------------
 * Event Manager API layer
 *
 *     The Event Manager provides a thread of execution to all stack
 *     components and manages stack initialization and deinitialization. Refer
 *     to the documentation in each function for specific instructions
 *     on how it interacts with the operating system integration layer
 *     (OS API).
 */

/* Forward reference of EvmTimer. 
 */
typedef struct _EvmTimer EvmTimer;

/*---------------------------------------------------------------------------
 * Callback used to notify a stack component that the timer expired.
 */
typedef void (*EvmTimerNotify)(EvmTimer*);


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * EvmTimer structure
 *
 *     Represents a stack timer. This structure is for use by
 *     internal stack components only.
 */
struct _EvmTimer
{
    ListEntry        node;     /* Used internally by the Event Manager */
    void            *context;  /* Context area for use by callers */
    EvmTimerNotify   func;     /* Function to call when timer fires */

    /* === Internal use only === */
    TimeT            time;      /* Amount of time to wait */
    TimeT            startTime; /* System time when the timer started */
};


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * EVM_Init()
 *
 *     Called by the OS to initializes the event manager and the stack.
 *     This must be called before the first call to EVM_Process.
 *
 * Returns:
 *     TRUE - Initialization succeeded.
 *
 *     FALSE - Initialization failed.
 */
BOOL EVM_Init(void);


/*---------------------------------------------------------------------------
 * EVM_Deinit()
 *
 *     Called by the OS to deinitialize the event manager and the stack.
 *     Transports are unbound, but not deinitialized when this call
 *     is made.  If it is necessary to deinitialize the transport and/or
 *     the radio module (via HCI commands), then RMGR_RadioShutdown() should
 *     be called prior to calling OS_Deinit().
 *
 *     Do not call EVM_Process after calling this function.
 */
void EVM_Deinit(void);


/*---------------------------------------------------------------------------
 * EVM_Process()
 *
 *     Called by the OS to give a thread of execution to the stack.
 *     Internally, this function calls OS_LockStack and OS_UnlockStack
 *     to protect the stack from reentrancy.
 */
void EVM_Process(void);

/* Prototypes for EVM_StartTimer EVM_CancelTimer macros */
void EvmStartTimer(EvmTimer* timer, TimeT ticks, BOOL stopHardware);
void EvmCancelTimer(EvmTimer* timer, BOOL stopHardware);

/*---------------------------------------------------------------------------
 * EVM_StartTimer()
 *
 *     Called by stack layers to start a timer. After the specified
 *     amount of time elapses, the timer fires by calling the timer's
 *     callback function. The timer fires only once for each call to
 *     EVM_StartTimer().
 *
 *     If the timer was already running, this function resets it with a
 *     new time period.
 *
 *     Internally, timers are managed by calling OS_GetSystemTime during
 *     stack execution. When XA_MULTITASKING is enabled, this function
 *     uses OS_StartTimer, which maps to a single system timer.
 *
 * Parameters:
 *     timer - Pointer to a timer structure. The caller must
 *         initialize the func field, and may also initialize the context
 *         field if desired. When the timer fires, timer.func is called
 *         during stack execution. NOTE: The timer.func will typically check
 *         for Hold, Park or Sniff mode with ME_GetCurrentMode when called
 *         and handle these modes differently (usually restarting the timer
 *         is sufficient.)
 *
 *     ms - Number of milliseconds to wait before firing. This value
 *         is converted to system ticks using the BT_MS_TO_TICKS macro.
 */
void EVM_StartTimer(EvmTimer* timer, TimeT ms);
#define EVM_StartTimer(timer, ms) EvmStartTimer(timer, BT_MS_TO_TICKS(ms), TRUE)

/*---------------------------------------------------------------------------
 * EVM_CancelTimer()
 *
 *     Called by the stack to cancel a timer. This prevents the timer
 *     from firing.
 *
 * Parameters:
 *     timer - Pointer to a timer structure. The timer must have been
 *         started by EVM_StartTimer and must not have fired or
 *         been cancelled already.
 */
void EVM_CancelTimer(EvmTimer* timer);
#define EVM_CancelTimer(timer) EvmCancelTimer(timer, TRUE)

#endif /* (IRDA_STACK == XA_ENABLED) || (BT_STACK == XA_ENABLED) || (XA_EVENTMGR == XA_ENABLED) */

#endif /* __EVENTMGR_H */


