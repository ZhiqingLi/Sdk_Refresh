/***************************************************************************
 *
 * File:
 *     $Id: userheap.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     A compact, portable heap manager.  
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

#ifndef __USERHEAP_H__
#define __USERHEAP_H__

#include "osapi.h"

/*---------------------------------------------------------------------------
 * UH_STATISTICS constant
 *
 *     When enabled, collects heap statistics which can be output using
 *     UH_DumpStatistics(). XA_DEBUG must be enabled to use this feature.
 */
#ifndef UH_STATISTICS
#define UH_STATISTICS XA_DEBUG
#endif
#if (UH_STATISTICS == XA_ENABLED) && (XA_DEBUG != XA_ENABLED)
#error "UH_STATISTICS requires XA_DEBUG!"
#endif

/*---------------------------------------------------------------------------
 * UH_CreateHeap()
 *
 *     Initializes a heap for use.
 *
 * Parameters:
 *     buffer - A buffer of memory to be used as a user heap. This memory
 *         must not be modified or released while in use.
 *
 *     len - Length of heap.
 *
 */
void UH_CreateHeap(U8 *buffer, U32 len);

/*---------------------------------------------------------------------------
 * UH_Alloc()
 *
 *     Allocates a block of memory from a user heap.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 *
 *     size - Amount of memory required.
 *
 * Returns:
 *     An allocated block of memory of the specified size. Returns "0" if 
 *     insufficient memory is available.
 */
U8 *UH_Alloc(U8 *heap, U32 size);

/*---------------------------------------------------------------------------
 * UH_ReAlloc()
 *
 *     Reallocates memory in the user heap. This alters the size of an
 *     allocated block. This function may also modify the location of
 *     the block being realloc'ed if necessary.
 *
 * Parameters:
 *     heap - Starting pointer for the heap (must have been initialized with
 *         UH_CreateHeap prior to this call).
 * 
 *     oldBlock - Pointer to memory block pointer. If successful, the 
 *         reallocated block will contain all of the data contained in
 *         oldBlock (truncated if size is smaller than the previous
 *         allocation). If oldBlock points to 0 then ReAlloc attempts to
 *         allocate an entirely new block. 
 *
 *     size - The new size required for the reallocated block. May be more
 *         than or less than the formerly allocated block's size. If zero,
 *         the oldBlock pointer (if any) is deallocated and set to 0.
 *
 * Returns:
 *     TRUE - oldBlock was successfully reallocated, and now is filled
 *         with a pointer to a memory buffer containing all of the data 
 *         previously pointed to, if any, up to "size" bytes.
 *
 *     FALSE - insufficient memory was available to complete the operation.
 *         and oldBlock is left unmodified.
 *     
 */
BOOL UH_ReAlloc(U8 *heap, U8 **oldBlock, U32 size);

/*---------------------------------------------------------------------------
 * UH_Free()
 *
 *     Frees a previously allocated memory block.
 *
 * Parameters:
 *     block - A pointer to memory previously indicated by UH_Alloc or
 *         UH_ReAlloc.
 */
void UH_Free(U8 *block);

#if UH_STATISTICS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            UH_DumpStatistics()
 *---------------------------------------------------------------------------
 * Report() on usage of the heap
 */
void UH_DumpStatistics(U8 *heap);
#endif /* UH_STATISTICS == XA_ENABLED */

#endif /* __USERHEAP_H__ */

