/****************************************************************************
 *
 * File:
 *     $Id: oslib.h 1309 2009-04-11 00:26:47Z khendrix $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 1309 $
 *
 * Description:
 *     Defines the APIs used by the stack to access system services.
 * 
 * Copyright 2001-2005 Extended Systems, Inc.
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

#ifndef __OSLIB_H
#define __OSLIB_H

#include "xatypes.h"


/*---------------------------------------------------------------------------
 * Operating System Library API layer
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

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * OS_Malloc()
 *
 *     Allocate a block of memory.
 *
 * Parameters:
 *     Size - Bytes to allocate.
 *
 * Returns:
 *     Zero -  if allocation failed.
 *     MemBlockPtr - if allocation succeeded.
 */
void *OS_Malloc(U32 Size);


/*---------------------------------------------------------------------------
 * OS_ReAlloc()
 *
 *     Re-allocates a memory block to size Size.
 *
 * Parameters:
 *     MemBlock - Previously allocated memory block to be freed
 *     Size - The new size of the memory block
 */
void * OS_ReAlloc(void *MemBlock, U32 Size);

/*---------------------------------------------------------------------------
 * OS_Free()
 *
 *     Deallocates or frees a memory block.
 *
 * Parameters:
 *     MemBlock - Previously allocated memory block to be freed
 */
void OS_Free(void *MemBlock);

/*---------------------------------------------------------------------------
 * OS_Atoi()
 *
 *    Converts a base-10 number string to an unsigned 32-bit value.
 *
 * Parameters:
 *     buff - a pointer to the sting based representation of the number. 
 * Returns:
 *     The 32 bit integer value.
 */
U32 OS_Atoi(const char *buff);

/*---------------------------------------------------------------------------
 * OS_Itoa()
 *
 *    Converts a in integer value to base 10 ASCII. Returns the number of
 *    bytes consumed by the string (NOT null-terminated). 
 *
 * Parameters:
 *     buff - Pointer to a memory buffer for holding the resulting string. 
 *     val  - The 32 bit integer value to be converted to a string. 
 *
 * Returns: 
 *    The number of bytes of the buffer used to represent the number. 
 */
U8 OS_Itoa(char *buff, U32 val);


#endif /* __OSLIB_H */

