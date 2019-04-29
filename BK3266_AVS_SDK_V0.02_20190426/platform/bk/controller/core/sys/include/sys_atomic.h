#ifndef _PARTHUS_SYS_ATOMIC_H
#define _PARTHUS_SYS_ATOMIC_H

/**************************************************************************
 * MODULE NAME:    sys_atomic.h
 * PROJECT CODE:   Bluestream
 * DESCRIPTION:    Atomic operations
 * MAINTAINER:     John Sheehy
 * DATE:           8 March 2000
 *
 * SOURCE CONTROL: $Id: sys_atomic.h,v 1.4 2004/07/07 14:22:28 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 **************************************************************************/
 
#include "sys_config.h"

__INLINE__ void SYSatomic_Increment_u_int8(u_int8 *);
__INLINE__ void SYSatomic_Decrement_u_int8(u_int8 *);

__INLINE__ void SYSatomic_Increment_u_int16(u_int16 *);
__INLINE__ void SYSatomic_Decrement_u_int16(u_int16 *);

#ifdef __USE_INLINES__
#include "sys_atomic_impl.h"
#endif


#endif /* _PARTHUS_SYS_ATOMIC_H */
