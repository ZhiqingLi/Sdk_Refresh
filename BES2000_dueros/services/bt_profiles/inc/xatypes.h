/***************************************************************************
 *
 * File:
 *     $Id: xatypes.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     This file contains declarations for BES (formerly Extended 
 *     Systems) protocol stacks.
 *
 * Created:
 *     November 19, 1996
 *
 * Copyright 1996-2005 Extended Systems, Inc.
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

#ifndef __XATYPES_H
#define __XATYPES_H

#include "config.h"

/****************************************************************************
 *
 * Base definitions used by the stack and framer
 *
 ****************************************************************************/

#ifndef BOOL_DEFINED
typedef unsigned int BOOL;   /* IGNORESTYLE */
#endif


typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char  U8;

typedef int S32;
typedef short S16;
typedef char  S8;

/* Defines storage wide enough for either a 32-bit integer OR a pointer.
 * Normally pointers are 32 bit so this can just be a U32.
 *
 * In 64-bit systems, U32_PTR must be wider. If you need this, typedef
 * it to a 64-bit integer in your overide and define U32_PTR_DEFINED to
 * prevent redefinition.
 */
#ifndef U32_PTR_DEFINED
typedef U32 U32_PTR;
#define U32_PTR_DEFINED
#endif /* U32_PTR_DEFINED */

/* Variable sized integers. Used to optimize processor efficiency by
 * using the most efficient data size for counters, arithmatic, etc.
 */
typedef unsigned long  I32;

#if XA_INTEGER_SIZE == 4
typedef unsigned long  I16;
typedef unsigned long  I8;
#elif XA_INTEGER_SIZE == 2
typedef unsigned short I16;
typedef unsigned short I8;
#elif XA_INTEGER_SIZE == 1
typedef unsigned short I16;
typedef unsigned char  I8;
#else
#error No XA_INTEGER_SIZE specified!
#endif

typedef void (*PFV) (void);

/* Boolean Definitions */
#ifndef TRUE
#define TRUE  (1==1)
#endif /* TRUE */

#ifndef FALSE
#define FALSE (0==1)  
#endif /* FALSE */

#endif /* __XATYPES_H */
