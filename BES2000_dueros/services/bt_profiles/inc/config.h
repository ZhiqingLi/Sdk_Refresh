/***************************************************************************
 *
 * File:
 *     $Id: config.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains declarations for default protocol stack
 *     configuration.
 *
 * Created:
 *     November 19, 1996
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*---------------------------------------------------------------------------
 * General Configuration API layer
 *
 *     The constants in this layer provide general configuration to
 *     communication protocol stacks compiled into the system. To change
 *     a constant, simply #define it in your overide.h include file.
 *
 *     Configuration constants here and in other configuration API files
 *     are used to make the stack more appropriate for a particular
 *     environment. Constants can be modified to allow tradeoffs
 *     between code size, RAM usage, functionality, and throughput.
 *
 *     Some constants are numeric, and others indicate whether a feature
 *     is enabled (defined as XA_ENABLED) or disabled (defined as
 *     XA_DISABLED).
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#define XA_DISABLED 0   /* Disable the feature */
#define XA_ENABLED  1   /* Enable the feature */


/*---------------------------------------------------------------------------
 * Include the override file. Settings in the override file will take 
 * precendence over the settings in this file and other configuration files.
 */
#include "overide.h"


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/


#ifndef XA_DEBUG
#if defined(XADEBUG) || defined(_DEBUG)
/*---------------------------------------------------------------------------
 * XA_DEBUG constant
 *
 *     When XA_DEBUG is enabled, debugging code is enabled throughout the
 *     stack, including OS_Report and OS_Assert calls and other code
 *     that verifies the proper behavior of the stack at run-time.
 *
 *     When XA_DEBUG is disabled, debugging code is not compiled into the
 *     stack. Asserts and diagnostic output are removed. This results
 *     in a significant code size savings.
 *
 *     You can specify the XA_DEBUG setting in the overide.h file. If it is
 *     not specified there, XA_DEBUG depends on the XADEBUG or _DEBUG
 *     constants defined at compile time. If XADEBUG or _DEBUG constants are
 *     defined, XA_DEBUG will be enabled. If the constants are not defined,
 *     XA_DEBUG will be disabled.
 */
#define XA_DEBUG XA_ENABLED
#else /* DEBUG symbols */
#define XA_DEBUG XA_DISABLED
#endif /* DEBUG symbols */
#endif /* XA_DEBUG not defined */


#ifndef XA_DEBUG_PRINT
/*---------------------------------------------------------------------------
 * XA_DEBUG_PRINT constant
 *
 *     When XA_DEBUG_PRINT is enabled, calls to OS_Report are 
 *     enabled throughout the stack. These calls provide useful information
 *     about the internal state of the stack.
 *
 *     When XA_DEBUG is disabled, OS_Report calls are not compiled into the
 *     stack. This results in a significant code size savings.
 *
 *     By default, XA_DEBUG_PRINT inherits the state of the XA_DEBUG option.
 *     By setting it manually, you can control whether debug output messages
 *     appear independently of XA_DEBUG.
 */
#define XA_DEBUG_PRINT XA_DEBUG
#endif /* XA_DEBUG not defined */


#ifndef IRDA_STACK
/*---------------------------------------------------------------------------
 * IRDA_STACK constant
 *
 *     Controls whether IrDA-specific functionality is compiled into the
 *     system. This option is used in systems that can accommodate multiple
 *     transport mechanisms.
 *
 *     By default, the IrDA stack is disabled.
 */
#define IRDA_STACK  XA_DISABLED
#endif


#ifndef BT_STACK
/*---------------------------------------------------------------------------
 * BT_STACK constant
 *
 *     Controls whether Bluetooth-specific functionality is compiled into
 *     the system. This option is used in systems that can accommodate
 *     multiple transport mechanisms.
 */
#define BT_STACK    XA_ENABLED
#endif

#ifndef XA_ERROR_CHECK
/*---------------------------------------------------------------------------
 * XA_ERROR_CHECK constant
 *
 *     Controls whether error checking code is compiled into the system.
 *
 *     When XA_ERROR_CHECK is enabled, error checking code protects the
 *     stack API from illegal usage by verifying the entrance conditions
 *     for API call. This includes checking to make sure the API was called
 *     at the correct time, and checking each parameter for correctness
 *     before proceeding with any other processing.
 *
 *     If an API returns a value marked XA_ERROR_CHECK only, the calling
 *     application has made a defective call which must be fixed.
 *
 *     When XA_ERROR_CHECK is disabled, error checking code is not
 *     compiled into the system. This means that invalid API calls may
 *     cause strange errors to occur. 
 *
 *     Disabling error checking will result in significant code size savings,
 *     but it must only be disabled if you are confident that applications
 *     will not call stack APIs at inappropriate times or with invalid
 *     parameters.
 *
 *     By default, error checking is enabled.
 */
#define XA_ERROR_CHECK XA_ENABLED
#endif
                                                                           
#ifndef XA_CONTEXT_PTR
/*---------------------------------------------------------------------------
 * XA_CONTEXT_PTR constant
 *
 *     Controls whether stack RAM is allocated dynamically or statically.
 *     All RAM used by stack components is kept in a stack context structure.
 *
 *     When XA_CONTEXT_PTR is enabled, accesses to the context structure
 *     are performed using the "->" operand. This is required when
 *     the context structure is allocated dynamically (at run time). 
 *
 *     When XA_CONTEXT_PTR is disabled, accesses to the context structure
 *     are performed using the more efficient "." operand. This is allowed
 *     only if the context structure is allocated statically (at load time).
 *     This is more time- and ROM-efficient because references to stack
 *     variables are resolved at compile time rather than run time.
 *
 *     Internally, all accesses to the stack context are protected through
 *     special macros. These macros are defined based on this configuration
 *     constant.
 *
 *     By default, XA_CONTEXT_PTR is disabled.
 */
#define XA_CONTEXT_PTR XA_DISABLED
#endif /* XA_CONTEXT_PTR */


#ifndef XA_INTEGER_SIZE
/*---------------------------------------------------------------------------
 * XA_INTEGER_SIZE constant
 *
 *     Defines the preferred width of counter variables. In some systems,
 *     accesses to 2- or 4-byte variables is more time efficient than other
 *     sizes, even though they may take up more RAM. The stack attempts to
 *     honor this preference, except where a greater size is required.
 *
 *     For instance, if XA_INTEGER_SIZE is defined as 2, 8-bit and 16-bit
 *     counters will both be defined as 16-bit variables.
 *
 *     XA_INTEGER_SIZE may be 1, 2, or 4 and will affect the definition
 *     of I8, I16 types used internally by the stack. By default, 2-byte
 *     integers are preferred.
 */
#define XA_INTEGER_SIZE    2
#endif


#ifndef XA_USE_ENDIAN_MACROS
/*---------------------------------------------------------------------------
 * XA_USE_ENDIAN_MACROS constant
 *
 *     Controls how endian conversions are executed by the stack.
 *
 *     When XA_USE_ENDIAN_MACROS is enabled, macros are used to perform
 *     big-endian and little-endian conversions. This increases overall
 *     ROM code size, but reduces RAM usage of the calling stack and
 *     reduces code execution time. Enabling XA_USE_ENDIAN_MACROS is similar
 *     to "inlining" all endian conversion routines.
 *
 *     When XA_USE_ENDIAN_MACROS is disabled, function calls are substituted
 *     to convert big- and little-endian values. This increases execution
 *     time and calling stack RAM usage slightly, but decreases ROM code
 *     size.
 *
 *     By default, endian macros are disabled (functions are used instead).
 */
#define XA_USE_ENDIAN_MACROS XA_DISABLED
#endif


#ifndef XA_MULTITASKING
/*---------------------------------------------------------------------------
 * XA_MULTITASKING constant
 *
 *     Controls whether the stack uses multitasking functions.
 *
 *     When XA_MULTITASKING is enabled, the stack uses calls that protect
 *     the stack from reentrancy and minimize CPU usage. These calls include
 *     OS_StartTimer, OS_CancelTimer, OS_NotifyEvm, OS_LockStack, and
 *     OS_UnlockStack. This setting is appropriate when using a pre-emptive
 *     multitasking system.
 *
 *     When XA_MULTITASKING is disabled, the functions listed above are not
 *     called by the stack. This is appropriate when using round-robin
 *     task scheduler that does not allow task blocking.
 *
 *     By default, multitasking features are disabled.
 */
#define XA_MULTITASKING XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * BT_MS_TO_TICKS()
 *
 *     Defines the conversion between system ticks and milliseconds. On some
 *     platforms, OS_GetSystemTime returns values in ticks instead of
 *     milliseconds. In this case, BT_MS_TO_TICKS must be defined to
 *     convert a duration expressed in milliseconds to ticks.
 *
 *     For example, a system with 4.4 millisecond ticks should define this
 *     macro in overide.h as follows:
 *
 *     #define BT_MS_TO_TICKS(ms) ((ms)*10/44)
 *
 *     By default, no conversion is performed. This is appropriate when
 *     OS_GetSystemTime is able to return values in milliseconds.
 */
#define BT_MS_TO_TICKS(ms)  ((ms)<<4)

#ifndef ContainingRecord
/*---------------------------------------------------------------------------
 * ContainingRecord() 
 *      Calculate the address of the base of the structure given its type
 *      and the address of a field within the structure.
 *
 *      While this definition is within ANSI standards, there will probably 
 *      be some compiler that has a problem with it. When in XA_DEBUG mode,
 *      a function called VerifyMacros exists that checks the behavior of
 *      this macro. This function will assert if this implementation of 
 *      ContainingRecord does not work with your compiler.
 *
 *      You may redefine the behavior of this macro by declaring a different
 *      version of it in your overide.h file.
 *      
 * Parameters:
 *      fieldAddr - address of 'memberName'
 *
 *      structType - structure type that contains 'memberName'
 *
 *      memberName - name of structure member located at 'fieldAddr'
 *
 * Returns:
 *      The address of 'structType' that contains 'memberName' at 'fieldAddr'
 */
/*@notfunction@*/
#define ContainingRecord(fieldAddr, structType, memberName) ((structType *)( \
                      (U8 *)(fieldAddr) - (U8 *)(&((structType *)0)->memberName)))
#endif

#ifndef XA_EVENTMGR
/*---------------------------------------------------------------------------
 * XA_EVENTMGR constant
 *
 *     Controls whether the Event Manager functionality is compiled into the
 *     system. This option is used to enable the Event Manager when the stack
 *     transport does not automatically include it. Note that Event Manager
 *     support is always included in IrDA and Bluetooth stack builds.
 *
 *     By default, the Event Manager is disabled.
 */
#define XA_EVENTMGR    XA_DISABLED
#endif

#ifndef XA_SNIFFER
/*---------------------------------------------------------------------------
 * XA_SNIFFER constant
 *
 *     Controls whether the protocol sniffer hooks in various protocol layers
 *     are compiled into the system. The sniffer uses an endpoint registration
 *     function to manage a connection table which is used as a guide when 
 *     decoding data packets. When enabled, the function SnifferRegisterEndpoint
 *     as described in debug.h must be implemented. When disabled, the sniffer
 *     functionality can still be accessed, but protocol specific decoding will
 *     stop at the L2CAP layer.
 *
 *     By default, this option is disabled.
 */
#define XA_SNIFFER      XA_DISABLED
#endif

#ifndef XA_DECODER
#if (XA_SNIFFER == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * XA_DECODER constant
 *
 *     Determines whether the XA Decoder subsystem is included. This system
 *     allows internal stack components, profiles, and applications to
 *     register integer types for decoding at runtime, resulting in more
 *     readable debug log entries and displays. These calls are documented
 *     in utils.h.
 *
 *     By default, XA_DECODER is enabled if either XA_DEBUG_PRINT or 
 *     XA_SNIFFER are enabled.
 */
#define XA_DECODER XA_ENABLED
#else
#define XA_DECODER XA_DISABLED
#endif /* (XA_SNIFFER == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */
#endif /* XA_DECODER */


#ifndef XA_LOAD_LIST
/*---------------------------------------------------------------------------
 * XA_LOAD_LIST constant
 *
 * Specifies any optional protocol or profile components in this build, 
 * which require initialization. The core IrDA and Bluetooth protocols are
 * not used in this constant. The components will be initialized in the
 * order specified by this constant. If a component requires inclusion on
 * this load list, it will be stated along with the initialization function
 * prototype, in the main header file for the component and in the Implementer's
 * Guide under the section for the profile or component being implemented. 
 *
 * An example usage of this constant to initialize an OBEX based application is:
 * #define XA_LOAD_LIST XA_MODULE(OBEX)
 *
 * Which will insert the following code in the event manager:
 * extern BOOL OBEX_Init(void);
 * if ((OBEX_Init && TRUE) != TRUE) { return FALSE; }
 *
 * An example of a implementation using GOEP and OBEX is:
 * #define XA_LOAD_LIST XA_MODULE(OBEX) XA_MODULE(GOEP)
 *
 * By default, no optional protocol or profile modules are initialized.
 */ 
#define XA_LOAD_LIST
#endif

#ifndef XA_STATISTICS 
/*---------------------------------------------------------------------------
 * XA_STATISTICS constant
 *
 *     Controls internal statistics-gathering macros. When enabled,
 *     certain stack components keep track of resource usage and timing.
 *     To extract this information, a module in your system must include
 *     /inc/sys/btstats.h and call BTSTAT_Report, which will send a
 *     set of statistics to debug output.
 *
 *     For more information about the meaning of each statistic, see
 *     the Blue SDK Implementer's Guide.
 *
 *     By default, statistics-gathering is disabled to save code size.
 *     
 * Requires:
 *     XA_DEBUG enabled.
 */ 
#define XA_STATISTICS   XA_DISABLED
#endif

#if XA_DEBUG == XA_DISABLED
#undef XA_STATISTICS
#define XA_STATISTICS   XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * Some pragmas to disable useless level-4 warnings. These are 
 * specific to MSVC.
 */

#ifdef WIN32

/* Conditional comparisons are not a problem */
#pragma warning(disable:4127)

/* Intentionally converting int to U8/U16 is done frequently, especially
 * with += operators. */
#pragma warning(disable:4244)

/* If we cast a U16 constant to a U8, let's assume that was intentional */
#pragma warning(disable:4310)

#endif /* WIN32 */

#endif /* __CONFIG_H */

